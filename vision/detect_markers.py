from __future__ import annotations

import argparse
import os
from pathlib import Path
import time

import cv2 as cv
import numpy as np

from pose_filter import EmaPoseFilter
from udp_sender import PoseUdpSender


DEFAULT_CALIBRATION_FILE = (
    Path(__file__).resolve().parent
    / "calibration"
    / "camera_calibration.yml"
)

MARKER_ROLES = {
    0: "Standby",
    1: "Autonomous target",
    2: "Marker 2",
    3: "Marker 3",
    4: "Marker 4",
}


def camera_source(value: str):
    return int(value) if value.isdecimal() else value


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Detect an ArUco target and send its filtered pose to the ESP32."
    )
    parser.add_argument(
        "--camera",
        default=os.getenv("DROIDCAM_URL", "http://192.168.51.160:4747/video/force/640x480"),
        help="Camera index or DroidCam URL, for example http://192.168.1.2:4747/video",
    )
    parser.add_argument(
        "--robot-host",
        default=os.getenv("ROBOT_HOST", "gmrt-heroes-2.local"),
        help="Hostname mDNS ESP32. Default: gmrt-heroes-2.local",
    )
    parser.add_argument("--robot-port", type=int, default=4210)
    parser.add_argument("--marker-id", type=int, default=1)
    parser.add_argument("--marker-size", type=float, default=0.045)
    parser.add_argument("--send-hz", type=float, default=20.0)
    parser.add_argument("--filter-alpha", type=float, default=0.35)
    parser.add_argument("--lost-grace-ms", type=int, default=150)
    parser.add_argument(
        "--calibration",
        type=Path,
        default=DEFAULT_CALIBRATION_FILE,
    )
    parser.add_argument("--headless", action="store_true")
    return parser.parse_args()


def load_calibration(path: Path):
    if not path.exists():
        raise FileNotFoundError(
            f"Calibration file not found: {path}. Run calibrate_camera.py first."
        )

    storage = cv.FileStorage(str(path), cv.FILE_STORAGE_READ)
    camera_matrix = storage.getNode("camera_matrix").mat()
    distortion = storage.getNode("dist_coeffs").mat()
    width = int(storage.getNode("image_width").real())
    height = int(storage.getNode("image_height").real())
    storage.release()

    if camera_matrix is None or distortion is None or width <= 0 or height <= 0:
        raise ValueError("Calibration file is incomplete")

    return camera_matrix, distortion, width, height


def marker_object_points(marker_size_m: float) -> np.ndarray:
    if marker_size_m <= 0.0:
        raise ValueError("marker size must be positive")

    half = marker_size_m / 2.0
    return np.array(
        [
            [-half, half, 0.0],
            [half, half, 0.0],
            [half, -half, 0.0],
            [-half, -half, 0.0],
        ],
        dtype=np.float32,
    )


def normalize_frame(frame, width: int, height: int):
    frame_height, frame_width = frame.shape[:2]

    if (frame_width, frame_height) == (width, height):
        return frame

    source_ratio = frame_width / frame_height
    calibration_ratio = width / height

    if abs(source_ratio - calibration_ratio) > 0.02:
        raise RuntimeError(
            f"Camera frame is {frame_width}x{frame_height}, but calibration is "
            f"{width}x{height}. Use the same aspect ratio or recalibrate."
        )

    return cv.resize(frame, (width, height))


def main() -> None:
    args = parse_args()

    if args.send_hz <= 0.0:
        raise ValueError("send-hz must be positive")

    camera_matrix, distortion, width, height = load_calibration(args.calibration)
    object_points = marker_object_points(args.marker_size)
    axis_length = args.marker_size / 2.0

    dictionary = cv.aruco.getPredefinedDictionary(cv.aruco.DICT_4X4_50)
    parameters = cv.aruco.DetectorParameters()
    parameters.cornerRefinementMethod = cv.aruco.CORNER_REFINE_SUBPIX
    detector = cv.aruco.ArucoDetector(dictionary, parameters)

    pose_filter = EmaPoseFilter(args.filter_alpha)
    sender = PoseUdpSender(args.robot_host, args.robot_port)
    source = camera_source(args.camera)
    camera = cv.VideoCapture(source)

    if isinstance(source, int):
        camera.set(cv.CAP_PROP_FRAME_WIDTH, width)
        camera.set(cv.CAP_PROP_FRAME_HEIGHT, height)

    send_interval = 1.0 / args.send_hz
    lost_grace_s = args.lost_grace_ms / 1000.0
    last_send = 0.0
    last_seen = float("-inf")
    last_console = 0.0
    last_packet = "waiting"

    try:
        if not camera.isOpened():
            raise RuntimeError(f"Unable to open camera source: {args.camera}")

        print(f"Camera source: {args.camera}")
        print(f"Target marker: {args.marker_id}, size: {args.marker_size:.3f} m")
        print(f"UDP destination: {sender.destination}")
        print("Press q to quit.")

        while True:
            success, frame = camera.read()
            if not success:
                raise RuntimeError("Camera frame could not be read")

            frame = normalize_frame(frame, width, height)
            gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            corners, ids, _ = detector.detectMarkers(gray)
            target_pose = None

            if ids is not None:
                cv.aruco.drawDetectedMarkers(frame, corners, ids)

                for index, detected_id in enumerate(ids.flatten()):
                    marker_id = int(detected_id)
                    image_points = corners[index].reshape(4, 2).astype(np.float32)
                    found, rotation_vector, translation_vector = cv.solvePnP(
                        object_points,
                        image_points,
                        camera_matrix,
                        distortion,
                        flags=cv.SOLVEPNP_IPPE_SQUARE,
                    )

                    if not found:
                        continue

                    cv.drawFrameAxes(
                        frame,
                        camera_matrix,
                        distortion,
                        rotation_vector,
                        translation_vector,
                        axis_length,
                        2,
                    )

                    tx, ty, tz = (float(value) for value in translation_vector.reshape(3))
                    distance_cm = float(np.linalg.norm(translation_vector)) * 100.0
                    rotation_matrix, _ = cv.Rodrigues(rotation_vector)
                    roll, pitch, yaw = cv.RQDecomp3x3(rotation_matrix)[0]
                    role = MARKER_ROLES.get(marker_id, "Unknown")

                    text_x, text_y = image_points[0].astype(int)
                    text_x = max(text_x, 10)
                    text_y = max(text_y - 60, 25)
                    lines = [
                        f"ID {marker_id} - {role}",
                        f"tx={tx * 100:.1f} cm  ty={ty * 100:.1f} cm  tz={tz * 100:.1f} cm",
                        f"distance={distance_cm:.1f} cm",
                        f"R/P/Y={roll:.1f}/{pitch:.1f}/{yaw:.1f} deg",
                    ]

                    for line_number, text in enumerate(lines):
                        cv.putText(
                            frame,
                            text,
                            (text_x, text_y + line_number * 20),
                            cv.FONT_HERSHEY_SIMPLEX,
                            0.48,
                            (0, 255, 0),
                            2,
                            cv.LINE_AA,
                        )

                    if marker_id == args.marker_id and tz > 0.0:
                        filtered_tx, filtered_tz = pose_filter.update(tx, tz)
                        target_pose = (marker_id, filtered_tx, filtered_tz)
                        last_seen = time.monotonic()

            now = time.monotonic()

            if now - last_send >= send_interval:
                if target_pose is not None:
                    marker_id, filtered_tx, filtered_tz = target_pose
                    last_packet = sender.send_pose(
                        marker_id,
                        filtered_tx,
                        filtered_tz,
                    )
                    last_send = now
                elif now - last_seen >= lost_grace_s:
                    pose_filter.reset()
                    last_packet = sender.send_no_marker()
                    last_send = now

            cv.putText(
                frame,
                f"UDP: {last_packet}",
                (10, height - 15),
                cv.FONT_HERSHEY_SIMPLEX,
                0.48,
                (255, 255, 0),
                1,
                cv.LINE_AA,
            )

            if now - last_console >= 1.0:
                print(f"UDP: {last_packet}")
                last_console = now

            if not args.headless:
                cv.imshow("GMRT ArUco Pose", frame)
                if cv.waitKey(1) & 0xFF == ord("q"):
                    break

    except KeyboardInterrupt:
        print("Stopped by user.")
    finally:
        try:
            sender.send_no_marker()
        except OSError:
            pass
        sender.close()
        camera.release()
        cv.destroyAllWindows()


if __name__ == "__main__":
    main()
