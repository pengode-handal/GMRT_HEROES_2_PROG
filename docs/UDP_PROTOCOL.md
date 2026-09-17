# Protokol UDP dan mDNS

## Endpoint

| Parameter | Nilai |
|---|---|
| Hostname | `gmrt-heroes-2.local` |
| Service mDNS | `_gmrt-udp._udp.local.` |
| Instance | `gmrt-heroes-2._gmrt-udp._udp.local.` |
| Port UDP | `4210` |
| Encoding | ASCII |
| Frekuensi vision | 20 Hz |
| Timeout ESP32 | 300 ms |

ESP32 tetap memperoleh IP dari router, tetapi pengguna tidak perlu menulis IP itu ke program vision. `mdns_resolver.py` menemukan endpoint berdasarkan service mDNS.

## Paket pose

```text
P,sequence,marker_id,tx,tz\n
```

Contoh:

```text
P,42,1,0.1050,0.6000
```

- `sequence`: bilangan 32-bit yang naik pada setiap paket.
- `marker_id`: ID ArUco target.
- `tx`: posisi horizontal marker relatif terhadap kamera dalam meter.
- `tz`: kedalaman marker di depan kamera dalam meter.

## Paket marker tidak terlihat

```text
N,sequence\n
```

Contoh:

```text
N,43
```

Paket ini langsung membatalkan target lama agar robot tidak terus bergerak menggunakan pose yang sudah tidak berlaku.

## Validasi ESP32

Paket ditolak jika:

- format atau jumlah field salah;
- nilai `tx/tz` bukan angka finite;
- `tz <= 0`;
- marker bukan target yang dikonfigurasi;
- sequence lebih lama atau duplikat;
- ukuran paket melebihi buffer.

Motor direm jika pose tidak valid atau tidak diperbarui selama 300 ms.

## Mengapa UDP

UDP dipakai karena pose dikirim berkala dan data terbaru lebih penting daripada retransmisi data lama. Fitur sequence number dan timeout menutup dua risiko utamanya: paket datang tidak berurutan dan koneksi pengirim berhenti.
