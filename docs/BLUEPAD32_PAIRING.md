# Pairing PS4 dengan Bluepad32

## Instalasi yang benar

Pada Arduino IDE, gunakan board dari paket **ESP32 + Bluepad32 Arduino**. Jika hanya memilih board dari paket ESP32 biasa, error `Bluepad32.h: No such file or directory` dapat muncul.

## Pairing pertama

1. Upload sketch `Bluepad32_Controller_Test` atau firmware utama.
2. Buka Serial Monitor 115200 baud.
3. Tekan **Share + PS** selama beberapa detik.
4. Light bar akan berkedip cepat.
5. Tunggu sampai callback connected muncul di Serial Monitor.

Setelah berhasil, cukup tekan tombol PS untuk reconnect berikutnya.

## Jika tidak tersambung

1. Pastikan board yang dipilih berasal dari kelompok ESP32 + Bluepad32.
2. Matikan Bluetooth laptop/HP yang pernah terhubung ke controller.
3. Reset controller melalui lubang kecil di belakang selama sekitar 5 detik.
4. Restart ESP32.
5. Ulangi Share + PS.
6. Jika key pairing lama rusak, jalankan `BP32.forgetBluetoothKeys()` **sekali saja**, upload, pairing ulang, lalu hapus kembali pemanggilan itu agar key tidak dihapus setiap boot.

## Mapping tombol Bluepad32 untuk PS4

| PS4 | Bluepad32 |
|---|---|
| Cross | `a()` |
| Circle | `b()` |
| Square | `x()` |
| Triangle | `y()` |
| L2 analog | `brake()` |
| R2 analog | `throttle()` |
| Options | `miscHome()` |
| Share | `miscBack()` |
