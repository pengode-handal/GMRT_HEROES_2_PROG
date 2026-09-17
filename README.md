# GMRT HEROES 2 — Arduino IDE + Bluepad32 + ArUco

Paket ini adalah integrasi akhir robot mobil 2WD dengan dua mode:

- **Manual:** dikendalikan controller PS4 melalui Bluepad32.
- **Autonomous:** pose marker ArUco dari DroidCam/OpenCV dikirim ke ESP32 melalui UDP.

Versi ini **tidak menggunakan HC-SR04**. Alamat ESP32 tidak perlu ditulis sebagai IP karena ESP32 mengumumkan hostname **`gmrt-heroes-2.local`** melalui mDNS.

## Fitur utama

- Differential drive dua motor melalui L298N mini 4-input.
- Pairing controller PS4 menggunakan Bluepad32, tanpa MAC address controller.
- Grip MG90S terbuka penuh saat ESP32 menyala.
- Posisi grip mengikuti tekanan analog R2.
- L2 menjadi rem manual bertahap.
- Segitiga menukar mode Manual dan Autonomous.
- Wi-Fi mati pada mode Manual dan baru dinyalakan saat masuk Autonomous.
- Wi-Fi memakai `WIFI_PS_MIN_MODEM` agar dapat coexist dengan Bluetooth.
- Cross mengaktifkan emergency stop; Circle membukanya kembali.
- Dua servo tambahan untuk arm dan wrist melalui stick kanan.
- UDP timeout 300 ms dan penghentian otomatis saat marker hilang.
- Hostname mDNS dan service discovery UDP.
- Mode simulasi yang tidak mengaktifkan motor maupun servo.

## Struktur paket

```text
firmware/
  GMRT_HEROES_2/             Sketch utama Arduino IDE
  Bluepad32_Controller_Test/ Tes pairing dan nilai input PS4
  WiFi_Test/                 Tes Wi-Fi dan mDNS
vision/
  detect_markers.py          Deteksi ArUco, filter pose, kirim UDP
  mdns_resolver.py           Resolusi hostname/service mDNS
  pose_filter.py             Filter EMA untuk tx dan tz
  udp_sender.py              Encoder dan pengirim paket UDP
  calibration/               Kalibrasi DroidCam
  tools/                     Simulator dan penerima UDP
  tests/                     Unit test Python
docs/                        Dokumentasi teknis
```

## 1. Persiapan Arduino IDE

Gunakan Arduino IDE 2.x. Tambahkan dua URL berikut pada **File → Preferences → Additional Boards Manager URLs**:

```text
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
```

Kemudian:

1. Buka **Tools → Board → Boards Manager**.
2. Pasang paket **ESP32**.
3. Pasang paket **ESP32 + Bluepad32**.
4. Pilih board dari kelompok **ESP32 + Bluepad32 Arduino**, lalu pilih board yang sesuai dengan ESP32 kalian, misalnya **DOIT ESP32 DEVKIT V1**.
5. Pasang library **ESP32Servo** melalui Library Manager.

Bluepad32 bukan pengganti yang cukup dipasang sebagai library biasa untuk konfigurasi ini; sketch harus dikompilasi menggunakan board package **ESP32 + Bluepad32**.

## 2. Mengisi Wi-Fi

Di folder `firmware/GMRT_HEROES_2`:

1. Duplikat `secrets.example.h`.
2. Ubah nama hasil duplikat menjadi `secrets.h`.
3. Isi SSID dan password hotspot/Wi-Fi 2,4 GHz.

```cpp
#define ROBOT_WIFI_SSID "NAMA_WIFI"
#define ROBOT_WIFI_PASSWORD "PASSWORD_WIFI"
```

Jangan mengunggah `secrets.h` ke GitHub.

## 3. Upload firmware utama

1. Buka `firmware/GMRT_HEROES_2/GMRT_HEROES_2.ino`.
2. Pilih board Bluepad32 dan COM port ESP32.
3. Compile lalu upload.
4. Buka Serial Monitor pada 115200 baud.

Jika berhasil, Serial Monitor menampilkan:

```text
Bluepad32 ready...
Wi-Fi configured. It will start in Autonomous mode.
Mode: MANUAL
```

Wi-Fi belum tersambung pada kondisi awal. Setelah controller tersambung dan
Segitiga ditekan, Serial Monitor kemudian menampilkan proses koneksi Wi-Fi,
mDNS, dan UDP.

## 4. Pairing controller PS4

1. Nyalakan ESP32 dan tunggu pesan Bluepad32 ready.
2. Tekan dan tahan **Share + PS** bersamaan.
3. Tunggu light bar berkedip cepat.
4. Lepaskan tombol dan tunggu pesan `Bluepad32 controller connected`.

Pairing pertama tidak memerlukan MAC address. Kunci pairing disimpan sehingga controller dapat menyambung kembali dengan tombol PS pada penggunaan berikutnya.

Gunakan sketch `firmware/Bluepad32_Controller_Test/Bluepad32_Controller_Test.ino` jika ingin menguji controller tanpa menjalankan robot lengkap.

## 5. Mapping controller PS4

| Input PS4 | Fungsi |
|---|---|
| Stick kiri Y | Maju/mundur |
| Stick kiri X | Belok kiri/kanan |
| R2 / trigger kanan | Grip dari terbuka penuh menuju tertutup |
| L2 / trigger kiri | Rem motor bertahap |
| Stick kanan Y | Arm naik/turun |
| Stick kanan X | Wrist kiri/kanan |
| Segitiga | Toggle Manual ↔ Autonomous |
| Cross | Emergency stop terkunci |
| Circle | Membuka emergency stop |

Bluepad32 membaca R2 dan L2 pada rentang `0..1023`.

Sudut grip dihitung dengan interpolasi:

```text
grip = open + (R2 / 1023) × (closed - open)
```

Konfigurasi awal menggunakan `open = 0°` dan `closed = 120°`. Nilai ini wajib disesuaikan dengan arah pemasangan linkage sebelum grip diberi beban.

Rem manual menggunakan:

```text
skala_motor = (1023 - L2) / 1023
PWM_setelah_rem = PWM_joystick × skala_motor
```

Saat L2 mendekati nilai maksimum, output motor dipaksa ke kondisi brake.

## 6. Cara kerja toggle Segitiga

Mode awal adalah `MANUAL`: Bluetooth Bluepad32 aktif dan Wi-Fi mati. Ketika
Segitiga ditekan, firmware melakukan urutan berikut:

1. Rem kedua motor.
2. Hentikan UDP lama jika masih aktif.
3. Ubah state menjadi `AUTONOMOUS`.
4. Nyalakan Wi-Fi station.
5. Aktifkan minimum modem sleep dengan `WiFi.setSleep(true)`.
6. Sambungkan Wi-Fi, mulai mDNS, lalu mulai UDP.
7. Motor tetap direm sampai pose marker yang valid dan baru diterima.

Bluetooth sengaja tidak dimatikan di mode Autonomous. ESP32 mendukung
coexistence Wi-Fi dan Bluetooth dengan pembagian waktu radio. Bluetooth tetap
diperlukan agar Segitiga, Cross, dan Circle masih dapat dibaca. Jika Bluetooth
benar-benar dimatikan, controller terputus dan Segitiga tidak dapat digunakan
untuk kembali ke Manual.

Saat Segitiga ditekan lagi, motor direm, UDP dan mDNS dihentikan, Wi-Fi
dimatikan, lalu state kembali ke `MANUAL`. Penjelasan lengkap tersedia di
`docs/RADIO_MODE_SWITCH.md`.

## 7. Menjalankan vision

Buka terminal pada folder `vision`:

```powershell
python -m venv .venv
.venv\Scripts\activate
python -m pip install -r requirements.txt
```

Jalankan DroidCam, lalu:

```powershell
python detect_markers.py --camera http://IP_HP:4747/video
```

Hostname default sudah `gmrt-heroes-2.local`, jadi IP ESP32 tidak perlu dimasukkan. Contoh dengan semua opsi penting:

```powershell
python detect_markers.py `
  --camera http://192.168.1.10:4747/video `
  --robot-host gmrt-heroes-2.local `
  --robot-port 4210 `
  --marker-id 1 `
  --marker-size 0.045
```

Laptop, HP DroidCam, dan ESP32 harus berada pada jaringan Wi-Fi yang sama. Jaringan guest yang memblokir komunikasi antarklien tidak dapat digunakan.

## 8. Simulasi tanpa motor

Pada `config.h`, ubah:

```cpp
#define ROBOT_SIMULATION 1
```

Jika ingin langsung menguji autonomous tanpa controller, ubah sementara:

```cpp
constexpr bool START_IN_AUTONOMOUS = true;
```

Upload firmware, lalu jalankan:

```powershell
python tools\udp_pose_simulator.py
```

Serial Monitor akan menampilkan PWM kiri/kanan tanpa mengaktifkan output motor dan servo.

## 9. Catatan keselamatan

- Uji arah motor dengan roda terangkat dari lantai.
- Servo harus mendapat suplai 5 V yang cukup dari buck converter, bukan dari pin 3,3 V ESP32.
- Satukan GND ESP32, L298N, buck converter, servo, dan baterai.
- Lepaskan linkage grip saat pertama kali menentukan sudut minimum dan maksimum.
- GPIO 2, 5, 12, dan 15 merupakan strapping pin ESP32. Jika board gagal boot, periksa kondisi rangkaian pada pin tersebut saat reset.
- Mode autonomous saat ini hanya mengandalkan pose kamera dan timeout UDP karena HC-SR04 telah dihapus.

Dokumentasi lanjutan dan daftar sumber belajar tersedia di folder `docs`.
