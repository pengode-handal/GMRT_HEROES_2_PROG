# Integrasi mDNS

## Tujuan

Alamat IP dari DHCP dapat berubah setiap kali hotspot atau ESP32 restart. Dengan mDNS, program vision selalu menggunakan:

```text
gmrt-heroes-2.local
```

## Sisi ESP32

Pada boot default Manual, Wi-Fi dan mDNS belum aktif. Setelah Segitiga menyalakan
mode Autonomous dan Wi-Fi tersambung, `network_manager.cpp` menjalankan:

```cpp
MDNS.begin("gmrt-heroes-2");
MDNS.setInstanceName("gmrt-heroes-2");
MDNS.addService("gmrt-udp", "udp", 4210);
```

Hasilnya adalah host `gmrt-heroes-2.local` dan service `_gmrt-udp._udp.local.`.

## Sisi Python

`mdns_resolver.py` melakukan dua jalur resolusi:

1. Menemukan service dengan package `zeroconf`.
2. Mencoba resolver hostname bawaan OS sebagai fallback.

Karena itu vision tetap dapat bekerja di Windows meskipun `ping nama.local` tidak didukung oleh konfigurasi Windows tertentu.

## Syarat jaringan

- Laptop, ESP32, dan HP DroidCam berada pada subnet/Wi-Fi yang sama.
- Multicast UDP port 5353 tidak diblokir.
- Fitur AP/client isolation pada router atau hotspot dimatikan.
- Gunakan Wi-Fi 2,4 GHz yang didukung ESP32.

## Diagnosis

Jika hostname tidak ditemukan:

1. Pastikan robot sudah berada di mode Autonomous.
2. Lihat Serial Monitor dan pastikan ada pesan `mDNS ready`.
3. Jalankan `WiFi_Test.ino`.
4. Pastikan dependency Python terpasang dari `requirements.txt`.
5. Matikan sementara firewall hanya untuk diagnosis, lalu buat rule yang tepat; jangan membiarkan firewall mati permanen.
6. Coba jaringan/hotspot lain yang tidak mengaktifkan client isolation.
