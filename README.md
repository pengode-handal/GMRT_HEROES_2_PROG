# PS4 Robot Controller

Project ini merupakan program kendali robot menggunakan ESP32 dan controller PS4 DualShock 4. Komunikasi antara controller dan ESP32 dilakukan melalui Bluetooth.

Robot menggunakan driver motor L298N untuk mengendalikan dua motor DC. Kecepatan motor dikontrol menggunakan PWM sehingga pergerakan robot dapat disesuaikan dengan posisi analog stick PS4.

Selain motor, robot memiliki dua buah servo yang digunakan sebagai mekanisme gripper untuk mengambil dan melepaskan barang.

## Fitur

- Kontrol robot menggunakan PS4 DualShock 4
- Komunikasi melalui Bluetooth
- Kontrol maju dan mundur menggunakan analog stick kiri
- Kontrol belok menggunakan analog stick kiri
- Kontrol kecepatan motor menggunakan PWM
- Kontrol dua motor DC menggunakan L298N
- Kontrol gripper menggunakan dua servo
- Tombol emergency stop
- Motor otomatis berhenti ketika controller terputus
- Program dibuat secara modular menggunakan beberapa class

## Hardware

Komponen yang digunakan:

- ESP32 DOIT DevKit V1 / ESP32-WROOM-32
- Controller PS4 DualShock 4
- L298N Motor Driver
- 2x Motor DC
- 2x Servo
- Baterai / power supply
- Kabel jumper
- Mekanisme gripper

## Pin Configuration

### L298N

| Komponen | ESP32 GPIO |
|----------|------------|
| ENA | GPIO 14 |
| IN1 | GPIO 27 |
| IN2 | GPIO 12 |
| IN3 | GPIO 13 |
| IN4 | GPIO 15 |
| ENB | GPIO 25 |

ENA dan ENB digunakan sebagai pin PWM untuk mengatur kecepatan masing-masing motor.

PIN ini belum dimasukan di schematic wiring, ntar didiskusikan lagi sama elektronis.

### Servo

| Servo | ESP32 GPIO |
|-------|------------|
| Servo 1 | GPIO 19 |
| Servo 2 | GPIO 18 |

Kedua servo digunakan untuk mekanisme gripper.

## PS4 Controller Mapping

Mapping controller yang digunakan saat ini:

| Tombol / Stick | Fungsi |
|----------------|--------|
| Left Stick Up | Robot maju |
| Left Stick Down | Robot mundur |
| Left Stick Left | Belok kiri |
| Left Stick Right | Belok kanan |
| R1 | Membuka gripper |
| L1 | Menutup gripper |
| Cross (X) | Emergency Stop |
| Right Stick | Belum digunakan |

### Kontrol Pergerakan

Analog stick kiri digunakan untuk mengontrol pergerakan robot.

```text
             LEFT STICK

                 ↑
               MAJU
                 |
       ← BELok --+-- BELOK →
                 |
              MUNDUR
                 ↓