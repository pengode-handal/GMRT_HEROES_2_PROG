# Pinout dan Kelistrikan

Pin berikut mengikuti skema terbaru yang diberikan.

## ESP32 ke L298N mini

| Fungsi | GPIO ESP32 |
|---|---:|
| Motor kiri IN1 | 5 |
| Motor kiri IN2 | 4 |
| Motor kanan IN1 | 2 |
| Motor kanan IN2 | 15 |

Driver pada skema menggunakan empat input arah/PWM tanpa ENA dan ENB terpisah.

## ESP32 ke servo

| Fungsi | Tipe default | GPIO ESP32 |
|---|---|---:|
| Grip | MG90S | 27 |
| Arm | SG90 | 19 |
| Wrist | SG90 | 18 |

HC-SR04 tidak dipakai dan GPIO 12/14 tidak digunakan oleh firmware ini.

## Distribusi daya

- Baterai 11 V masuk ke jalur motor dan input buck converter sesuai rating perangkat.
- Buck converter diatur menjadi 5 V sebelum servo/ESP32 disambungkan.
- Servo mendapat 5 V dari rail yang mampu menyediakan arus puncak cukup.
- Semua GND wajib tersambung menjadi common ground.
- Jangan mengambil arus servo dari pin 3,3 V ESP32.

## Perhatian strapping pin

GPIO 2, 5, 12, dan 15 memengaruhi proses boot ESP32. Skema memakai GPIO 2, 5, dan 15 untuk driver motor. Jika upload atau boot gagal:

1. Lepaskan sementara koneksi driver motor.
2. Pastikan input driver tidak memaksa level yang salah saat reset.
3. Upload ulang firmware.
4. Jika masalah berulang, pindahkan pin motor ke GPIO non-strapping dan sesuaikan `config.h` serta skema.

## Kalibrasi arah

Jika satu motor terbalik, jangan langsung menukar banyak kabel. Ubah dahulu:

```cpp
constexpr bool LEFT_REVERSED = false;
constexpr bool RIGHT_REVERSED = false;
```

menjadi `true` untuk sisi yang terbalik.

Untuk grip, lepas linkage dan uji `GRIP_OPEN_DEG` serta `GRIP_CLOSED_DEG` secara bertahap agar MG90S tidak menekan mekanik melewati batas.
