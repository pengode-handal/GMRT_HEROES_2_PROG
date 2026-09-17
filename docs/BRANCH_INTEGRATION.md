# Catatan Integrasi Branch

Paket ini disusun ulang untuk Arduino IDE dari branch repository `pengode-handal/GMRT_HEROES_2_PROG`.

Snapshot branch diperiksa pada 16 September 2026.

| Branch | Commit acuan | Bagian yang digunakan |
|---|---|---|
| `robot_controller_PS4_paired` | `ca68a41` | Bluepad32, callback connect/disconnect, axis controller, dan mapping PS4 |
| `udp-receiver` | `a29a126` | Dasar UDP parser serta penggunaan `ESPmDNS` |
| `autonomous-main` | `9b3dc3d` | Rumus pose-ke-PWM, stop distance, align-only, dan simulation output |
| `vision-main` | `215a009` | Deteksi ArUco, kalibrasi, filter EMA, dan protokol UDP |
| `hardprog-modular` | `9b42a09` | Pemisahan fungsi motor/servo/controller menjadi modul |

## Perbaikan saat integrasi

- Menghapus seluruh dependency HC-SR04.
- Mengganti `PS4Controller.h` menjadi Bluepad32.
- Mengisi konfigurasi sudut grip yang masih kosong pada branch Bluepad32.
- Mengubah grip R1/L1 menjadi kontrol analog R2.
- Menambahkan L2 sebagai rem proporsional.
- Mengubah toggle mode dari Options menjadi Segitiga.
- Menyatukan port UDP menjadi 4210.
- Mengubah alamat tujuan vision dari IP statis menjadi `gmrt-heroes-2.local`.
- Mengiklankan service `_gmrt-udp._udp.local.` melalui mDNS.
- Menjaga hanya satu `setup()` dan satu `loop()` pada sketch utama.
- Memindahkan SSID/password ke `secrets.h` yang tidak perlu diunggah.
- Menambahkan UDP timeout, sequence validation, dan latched emergency stop.

Paket ini tidak otomatis menulis perubahan ke branch GitHub. Gunakan isinya sebagai branch integrasi baru setelah pengujian hardware selesai.
