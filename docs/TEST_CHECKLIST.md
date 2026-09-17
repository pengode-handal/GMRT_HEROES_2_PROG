# Checklist Pengujian Bertahap

Jangan langsung menguji seluruh robot di lantai. Ikuti urutan berikut.

## 1. Pemeriksaan daya

- [ ] Output buck converter benar-benar 5 V dengan multimeter.
- [ ] Polaritas baterai, driver, ESP32, dan servo benar.
- [ ] Semua GND tersambung.
- [ ] Roda belum menyentuh lantai.
- [ ] Linkage grip dilepas saat kalibrasi awal.

## 2. Bluepad32 saja

Upload `Bluepad32_Controller_Test.ino`.

- [ ] Share + PS membuat light bar berkedip cepat.
- [ ] Serial menampilkan controller connected.
- [ ] Stick kiri/kanan menghasilkan nilai sekitar `-511..512`.
- [ ] L2 menghasilkan `brake=0..1023`.
- [ ] R2 menghasilkan `throttle=0..1023`.
- [ ] Cross, Circle, dan Triangle terbaca benar.

## 3. Wi-Fi dan mDNS saja

Upload `WiFi_Test.ino` setelah mengisi SSID/password.

- [ ] ESP32 terhubung ke Wi-Fi 2,4 GHz.
- [ ] Serial menampilkan `mDNS ready: gmrt-heroes-2.local`.
- [ ] Dari laptop, `ping gmrt-heroes-2.local` berhasil jika OS mendukung mDNS ping.
- [ ] Jika ping tidak didukung, jalankan simulator vision; library `zeroconf` tetap dapat menemukan service.

## 4. Simulasi firmware terintegrasi

Atur `ROBOT_SIMULATION=1` dan `START_IN_AUTONOMOUS=true`.

- [ ] Serial menampilkan `Wi-Fi power save: WIFI_PS_MIN_MODEM`.
- [ ] Firmware menerima paket simulator.
- [ ] Target tengah menghasilkan PWM kiri dan kanan hampir sama.
- [ ] `tx > 0` mengoreksi robot ke kanan.
- [ ] `tx < 0` mengoreksi robot ke kiri.
- [ ] `tz <= 0,25` menghasilkan PWM 0/0.
- [ ] Setelah paket berhenti lebih dari 300 ms, PWM menjadi 0/0.

## 5. Servo tanpa linkage

Kembalikan mode Manual.

- [ ] Saat boot, grip berada pada sudut open.
- [ ] R2 dilepas menghasilkan sudut open.
- [ ] R2 ditekan perlahan menggerakkan grip menuju closed.
- [ ] Stick kanan menggerakkan arm dan wrist sesuai arah.
- [ ] Tidak ada servo bergetar atau menahan di batas mekanik.

Jika arah grip terbalik, tukar nilai open/closed atau sesuaikan linkage. Jangan memaksa servo.

## 6. Motor dengan roda terangkat

- [ ] Stick kiri maju memutar kedua roda maju.
- [ ] Stick kiri mundur memutar kedua roda mundur.
- [ ] Belok kanan/kiri menghasilkan differential drive yang benar.
- [ ] L2 mengurangi PWM secara bertahap.
- [ ] L2 penuh menghentikan kedua motor.
- [ ] Cross mengunci emergency stop.
- [ ] Circle membuka emergency stop tetapi motor tetap diam sampai joystick digerakkan.
- [ ] Segitiga hanya menukar mode satu kali pada satu penekanan.
- [ ] Saat boot Manual, status menunjukkan `wifi_active=0`.
- [ ] Setelah Segitiga ditekan, motor tetap brake selama Wi-Fi/UDP belum siap.
- [ ] Di Autonomous, controller tetap tersambung dan Segitiga masih terbaca.
- [ ] Setelah Segitiga ditekan lagi, status kembali `wifi_active=0`.

## 7. Autonomous dengan roda terangkat

- [ ] Jalankan DroidCam dan `detect_markers.py`.
- [ ] Hostname ditemukan tanpa memasukkan IP ESP32.
- [ ] Marker ID 1 mengubah PWM sesuai posisi.
- [ ] Marker lain tidak menggerakkan motor.
- [ ] Marker hilang membuat motor direm.
- [ ] Grip tidak berubah di mode Autonomous.

## 8. Uji lantai kecepatan rendah

- [ ] Mulai dengan `MANUAL_PWM_LIMIT` rendah.
- [ ] Sediakan sakelar daya fisik yang mudah dijangkau.
- [ ] Uji rem dan emergency stop terlebih dahulu.
- [ ] Naikkan PWM secara bertahap setelah arah dan mekanik benar.
