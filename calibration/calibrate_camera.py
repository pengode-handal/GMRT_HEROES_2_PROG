from pathlib import Path

import cv2
import numpy as np


# 9 x 6 sudt dalam
#  10 x 7 kotak.
CHECKERBOARD_SIZE = (9, 6)

# Panjang satu kotak checkerboard: 25 mm = 0,025 meter.
SQUARE_SIZE_METERS = 0.025

MIN_SAMPLES = 10
TARGET_SAMPLES = 15
CAMERA_ID = 0

OUTPUT_FILE = (
    Path(__file__).resolve().parent
    / "camera_calibration.yml"
)


def main():
    # Koordinat sudut checkerboard di real life.
    object_template = np.zeros(
        (CHECKERBOARD_SIZE[0] * CHECKERBOARD_SIZE[1], 3),
        dtype=np.float32,
    )
    object_template[:, :2] = np.mgrid[
        0 : CHECKERBOARD_SIZE[0],
        0 : CHECKERBOARD_SIZE[1],
    ].T.reshape(-1, 2)
    object_template *= SQUARE_SIZE_METERS

    object_points = []
    image_points = []
    image_size = None

    criteria = (
        cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER,
        30,
        0.001,
    )

    camera = cv2.VideoCapture(CAMERA_ID)

    if not camera.isOpened():
        print("Webcam tidak dapat dibuka.")
        raise SystemExit

    print("Arahkan checkerboard ke webcam.")
    print("Tekan s untuk menyimpan sampel, q untuk berhenti.")
    print("Gerakkan dan miringkan papan sebelum mengambil sampel berikutnya.")

    try:
        while len(object_points) < TARGET_SAMPLES:
            success, frame = camera.read()

            if not success:
                print("Frame dari webcam gagal dibaca.")
                break

            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            image_size = gray.shape[::-1]

            found, corners = cv2.findChessboardCorners(
                gray,
                CHECKERBOARD_SIZE,
                cv2.CALIB_CB_ADAPTIVE_THRESH
                + cv2.CALIB_CB_NORMALIZE_IMAGE,
            )

            refined_corners = None

            if found:
                refined_corners = cv2.cornerSubPix(
                    gray,
                    corners,
                    (11, 11),
                    (-1, -1),
                    criteria,
                )
                cv2.drawChessboardCorners(
                    frame,
                    CHECKERBOARD_SIZE,
                    refined_corners,
                    found,
                )

            status = (
                f"Sample: {len(object_points)}/{TARGET_SAMPLES} | "
                f"Checkerboard: {'TERDETEKSI' if found else 'BELUM'}"
            )
            cv2.putText(
                frame,
                status,
                (20, 35),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.65,
                (0, 255, 0) if found else (0, 0, 255),
                2,
                cv2.LINE_AA,
            )

            cv2.imshow("Camera Calibration", frame)
            key = cv2.waitKey(1) & 0xFF

            if key == ord("s"):
                if found and refined_corners is not None:
                    object_points.append(object_template.copy())
                    image_points.append(refined_corners.copy())
                    print(f"Sampel ke-{len(object_points)} tersimpan.")
                else:
                    print("Checkerboard belum terdeteksi. Sampel tidak disimpan.")

            elif key == ord("q"):
                break

    finally:
        camera.release()
        cv2.destroyAllWindows()

    if len(object_points) < MIN_SAMPLES or image_size is None:
        print(
            f"Kalibrasi dibatalkan. Diperlukan minimal {MIN_SAMPLES} sampel."
        )
        raise SystemExit

    print("Menghitung parameter kalibrasi...")

    rms_error, camera_matrix, dist_coeffs, rvecs, tvecs = (
        cv2.calibrateCamera(
            object_points,
            image_points,
            image_size,
            None,
            None,
        )
    )

    # Mean reprojection error.
    total_error = 0.0

    for index in range(len(object_points)):
        projected_points, _ = cv2.projectPoints(
            object_points[index],
            rvecs[index],
            tvecs[index],
            camera_matrix,
            dist_coeffs,
        )

        imgpoints_detected = image_points[index].reshape(-1, 2)
        imgpoints_projected = projected_points.reshape(-1, 2)

        error = cv2.norm(
            imgpoints_detected,
            imgpoints_projected,
            cv2.NORM_L2,
        ) / len(imgpoints_projected)
        total_error += error

    mean_error = total_error / len(object_points)

    OUTPUT_FILE.parent.mkdir(parents=True, exist_ok=True)
    storage = cv2.FileStorage(str(OUTPUT_FILE), cv2.FILE_STORAGE_WRITE)
    storage.write("camera_matrix", camera_matrix)
    storage.write("dist_coeffs", dist_coeffs)
    storage.write("image_width", image_size[0])
    storage.write("image_height", image_size[1])
    storage.write("rms_error", float(rms_error))
    storage.write("mean_reprojection_error", float(mean_error))
    storage.release()

    print("Kalibrasi selesai.")
    print(f"File tersimpan di: {OUTPUT_FILE}")
    print(f"RMS error: {rms_error:.4f}")
    print(f"Mean reprojection error: {mean_error:.4f} pixel")
    print("Semakin mendekati 0, hasil kalibrasi semakin baik.")


if __name__ == "__main__":
    main()
