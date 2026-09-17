from __future__ import annotations

import argparse
import os
from pathlib import Path

import cv2 as cv
import numpy as np


DEFAULT_OUTPUT = Path(__file__).resolve().parent / "camera_calibration.yml"


def camera_source(value: str):
    return int(value) if value.isdecimal() else value


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Calibrate the DroidCam camera.")
    parser.add_argument(
        "--camera",
        default=os.getenv("DROIDCAM_URL", "0"),
        help="Camera index or DroidCam URL",
    )
    parser.add_argument("--columns", type=int, default=9, help="Inner corners across")
    parser.add_argument("--rows", type=int, default=6, help="Inner corners down")
    parser.add_argument("--square-size", type=float, default=0.025)
    parser.add_argument("--samples", type=int, default=15)
    parser.add_argument("--minimum-samples", type=int, default=10)
    parser.add_argument("--width", type=int, default=640)
    parser.add_argument("--height", type=int, default=480)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    return parser.parse_args()


def normalize_frame(frame, width: int, height: int):
    source_height, source_width = frame.shape[:2]

    if (source_width, source_height) == (width, height):
        return frame

    if abs(source_width / source_height - width / height) > 0.02:
        raise RuntimeError(
            f"Camera frame is {source_width}x{source_height}; expected a "
            f"{width}:{height} aspect ratio"
        )

    return cv.resize(frame, (width, height))


def main() -> None:
    args = parse_args()
    checkerboard_size = (args.columns, args.rows)

    if args.square_size <= 0.0:
        raise ValueError("square-size must be positive")
    if args.minimum_samples > args.samples:
        raise ValueError("minimum-samples cannot exceed samples")

    object_template = np.zeros(
        (args.columns * args.rows, 3),
        dtype=np.float32,
    )
    object_template[:, :2] = np.mgrid[
        0 : args.columns,
        0 : args.rows,
    ].T.reshape(-1, 2)
    object_template *= args.square_size

    object_points = []
    image_points = []
    image_size = None
    criteria = (
        cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER,
        30,
        0.001,
    )

    source = camera_source(args.camera)
    camera = cv.VideoCapture(source)

    if isinstance(source, int):
        camera.set(cv.CAP_PROP_FRAME_WIDTH, args.width)
        camera.set(cv.CAP_PROP_FRAME_HEIGHT, args.height)

    if not camera.isOpened():
        raise RuntimeError(f"Unable to open camera source: {args.camera}")

    print("Move and tilt the checkerboard between samples.")
    print("Press s to save a detected sample and q to finish.")

    try:
        while len(object_points) < args.samples:
            success, frame = camera.read()
            if not success:
                raise RuntimeError("Camera frame could not be read")

            frame = normalize_frame(frame, args.width, args.height)
            gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            image_size = gray.shape[::-1]
            found, corners = cv.findChessboardCorners(
                gray,
                checkerboard_size,
                cv.CALIB_CB_ADAPTIVE_THRESH + cv.CALIB_CB_NORMALIZE_IMAGE,
            )

            refined = None
            if found:
                refined = cv.cornerSubPix(
                    gray,
                    corners,
                    (11, 11),
                    (-1, -1),
                    criteria,
                )
                cv.drawChessboardCorners(frame, checkerboard_size, refined, found)

            status = (
                f"Samples: {len(object_points)}/{args.samples} | "
                f"Checkerboard: {'FOUND' if found else 'NOT FOUND'}"
            )
            cv.putText(
                frame,
                status,
                (20, 35),
                cv.FONT_HERSHEY_SIMPLEX,
                0.65,
                (0, 255, 0) if found else (0, 0, 255),
                2,
                cv.LINE_AA,
            )
            cv.imshow("Camera Calibration", frame)

            key = cv.waitKey(1) & 0xFF
            if key == ord("s"):
                if found and refined is not None:
                    object_points.append(object_template.copy())
                    image_points.append(refined.copy())
                    print(f"Saved sample {len(object_points)}")
                else:
                    print("Checkerboard is not detected; sample was not saved")
            elif key == ord("q"):
                break
    finally:
        camera.release()
        cv.destroyAllWindows()

    if len(object_points) < args.minimum_samples or image_size is None:
        raise RuntimeError(
            f"Calibration needs at least {args.minimum_samples} valid samples"
        )

    rms_error, camera_matrix, distortion, rotation_vectors, translation_vectors = (
        cv.calibrateCamera(
            object_points,
            image_points,
            image_size,
            None,
            None,
        )
    )

    total_error = 0.0
    for index, points in enumerate(object_points):
        projected, _ = cv.projectPoints(
            points,
            rotation_vectors[index],
            translation_vectors[index],
            camera_matrix,
            distortion,
        )
        total_error += cv.norm(
            image_points[index],
            projected,
            cv.NORM_L2,
        ) / len(projected)

    mean_error = total_error / len(object_points)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    storage = cv.FileStorage(str(args.output), cv.FILE_STORAGE_WRITE)
    storage.write("camera_matrix", camera_matrix)
    storage.write("dist_coeffs", distortion)
    storage.write("image_width", image_size[0])
    storage.write("image_height", image_size[1])
    storage.write("rms_error", float(rms_error))
    storage.write("mean_reprojection_error", float(mean_error))
    storage.release()

    print(f"Calibration saved to: {args.output}")
    print(f"RMS error: {rms_error:.4f}")
    print(f"Mean reprojection error: {mean_error:.4f} px")


if __name__ == "__main__":
    main()

