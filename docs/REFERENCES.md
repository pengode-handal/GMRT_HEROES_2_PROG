# Referensi Belajar

Referensi berikut dipilih agar anggota tim dapat mempelajari tiap modul dari sumber resminya.

## Bluepad32 dan controller PS4

- [Bluepad32 untuk Arduino + ESP32](https://bluepad32.readthedocs.io/en/latest/plat_arduino/) — instalasi board package dan contoh dasar.
- [Repository Bluepad32](https://github.com/ricardoquesada/bluepad32) — core Bluetooth host dan dokumentasi controller yang didukung.
- [Bluepad32 Arduino API](https://github.com/ricardoquesada/bluepad32-arduino) — class `Controller`, axis, trigger, dan contoh callback.

Bagian yang perlu dipelajari: `BP32.setup()`, `BP32.update()`, callback connect/disconnect, `axisX/Y`, `axisRX/RY`, `brake()`, `throttle()`, dan mapping tombol.

## ESP32, Wi-Fi, dan mDNS

- [Arduino Core for ESP32](https://github.com/espressif/arduino-esp32) — implementasi resmi Arduino untuk ESP32.
- [Library ESPmDNS](https://github.com/espressif/arduino-esp32/tree/master/libraries/ESPmDNS) — `MDNS.begin()`, `setInstanceName()`, dan `addService()`.
- [ESP-IDF RF Coexistence](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/coexist.html) — pembagian waktu radio Wi-Fi dan Bluetooth.
- [ESP-IDF Wi-Fi Power Save](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi-driver/wifi-performance-and-power-save.html) — `WIFI_PS_MIN_MODEM`, DTIM, dan perilaku coexistence.
- [Implementasi `WiFi.setSleep()`](https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFiGeneric.cpp) — nilai `true` dipetakan ke `WIFI_PS_MIN_MODEM`.
- [Python Zeroconf](https://github.com/python-zeroconf/python-zeroconf) — service discovery mDNS dari laptop.

Bagian yang perlu dipelajari: DHCP vs hostname, domain `.local`, multicast UDP
5353, perbedaan port mDNS dengan port data robot 4210, serta alasan modem sleep
wajib dipertahankan saat Wi-Fi dan Bluetooth aktif bersamaan.

## Vision dan ArUco

- [OpenCV ArUco Detection](https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html) — dictionary, marker corners, dan detector parameters.
- [OpenCV Camera Calibration dan Pose](https://docs.opencv.org/4.x/d9/d0c/group__calib3d.html) — camera matrix, distortion, `solvePnP()`, dan Rodrigues rotation.
- [opencv-contrib-python](https://pypi.org/project/opencv-contrib-python/) — package Python yang menyertakan modul `cv2.aruco`.

Bagian yang perlu dipelajari: satuan marker, konsistensi resolusi kalibrasi, arti `tx/ty/tz`, filter EMA, dan efek delay video DroidCam.

## Motor, servo, dan driver

- [Datasheet L298](https://www.st.com/resource/en/datasheet/l298.pdf) — tabel logika H-bridge dan kondisi motor stop.
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo) — attach, pulse width, dan penulisan sudut servo.

Bagian yang perlu dipelajari: differential drive, PWM, common ground, arus stall motor/servo, dan kalibrasi batas mekanik grip.

## Urutan belajar yang disarankan

1. Jalankan contoh Controller Bluepad32 dan pahami semua nilai input.
2. Jalankan WiFi/mDNS test sampai hostname ditemukan laptop.
3. Pelajari paket UDP dengan simulator, tanpa motor.
4. Uji pose ArUco dan pastikan `tx/tz` stabil.
5. Uji servo tanpa linkage dan motor dengan roda terangkat.
6. Gabungkan Manual, lalu Autonomous pada kecepatan rendah.
