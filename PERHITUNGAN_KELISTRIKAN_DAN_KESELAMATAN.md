# Lembar Perhitungan Kelistrikan dan Keselamatan SAUVAC

Dokumen ini adalah lembar kerja untuk memeriksa risiko panas berlebih,
korsleting, dan kebakaran sebelum ROV diberi daya. **Ini bukan sertifikasi
keselamatan dan tidak menjamin sistem bebas kebakaran.** Data baterai, ESC,
thruster, kabel, konektor, regulator, dan kondisi pemasangan harus diisi dari
datasheet komponen yang benar-benar digunakan. Jangan menebak nilai fuse atau
ukuran kabel.

## Status data perangkat

Isi sebelum menentukan fuse, kabel, atau batas operasi. Catat merek/model dan
tautan datasheet/manual agar perhitungan dapat diperiksa ulang.

| Perangkat | Merek/model | Data yang wajib dicatat |
|---|---|---|
| Baterai | Belum diisi | Kimia, jumlah sel/tegangan maksimum, kapasitas (Ah), arus kontinu dan puncak, batas arus/BMS, instruksi fuse |
| ESC 1-6 | Belum diisi | Tegangan yang didukung, arus kontinu/puncak, pendinginan, fuse yang disarankan, jenis sinyal/netral |
| Thruster 1-6 | Belum diisi | Tegangan, arus maksimum/terkunci (jika tersedia), kurva dorong/arus dan batas kerja |
| Kabel utama/cabang | Belum diisi | Bahan, luas penampang atau AWG, panjang, rating temperatur, rating arus setelah derating |
| Konektor/sakelar | Belum diisi | Rating DC tegangan dan arus, rating saat penyambungan |
| Konverter DC-DC | Belum diisi | Rentang input, output, arus kontinu, efisiensi, pendinginan, proteksi |
| Raspberry Pi Pico W | Pico W | Supply sesuai dokumentasi resmi board; jangan beri 5 V ke GPIO |
| Sensor | GY-271 dan GY-521 | VCC 3,3 V sesuai koneksi proyek, arus dari datasheet modul |

Firmware proyek saat ini mengatur enam output ESC pada GP2-GP7 dan sensor
berbagi I2C1: GP26 (SDA) dan GP27 (SCL). Ini hanya pemetaan sinyal, **bukan**
skema distribusi daya. Jangan mengalirkan daya thruster melalui Pico W atau
jalur 3V3 board.

## Perhitungan arus dan daya

Gunakan arus maksimum kontinu yang dinyatakan pabrikan pada tegangan baterai
yang dipilih. Jika arus maksimum thruster tidak tercantum, minta data pabrikan
atau ukur dengan alat dan prosedur yang sesuai; jangan menjadikan arus tanpa
beban sebagai arus desain.

Untuk enam thruster:

```text
I_thruster_total = I1 + I2 + I3 + I4 + I5 + I6
I_beban_total    = I_thruster_total + I_Pico + I_sensor + I_beban_lain
P_beban          = V_baterai * I_beban_total
```

| Beban | Tegangan kerja (V) | Arus kontinu maksimum (A) | Arus puncak (A) | Sumber data |
|---|---:|---:|---:|---|
| Thruster 1 + ESC 1 | | | | |
| Thruster 2 + ESC 2 | | | | |
| Thruster 3 + ESC 3 | | | | |
| Thruster 4 + ESC 4 | | | | |
| Thruster 5 + ESC 5 | | | | |
| Thruster 6 + ESC 6 | | | | |
| Pico W dan sensor | | | | |
| Beban lain (jika ada) | | | | |
| **Total** | | **Jumlah arus kontinu** | **Evaluasi kondisi puncak** | |

Jangan menganggap semua thruster selalu menarik arus yang sama. Periksa kondisi
terburuk yang diizinkan: akselerasi bersamaan, arah thrust, thruster tertahan,
tegangan baterai maksimum, dan keterbatasan pendinginan di dalam badan ROV.
Jangan menjalankan thruster dalam keadaan tertahan untuk mengukur arus kecuali
prosedur uji pabrikan memang mengizinkannya.

## Baterai dan waktu operasi

Kapasitas baterai bukan rating arus. Pastikan kemampuan arus kontinu baterai
dan BMS memenuhi beban, termasuk kondisi transien yang ditentukan pabrikan.

```text
I_baterai_desain <= batas arus kontinu baterai dan BMS
Waktu_teoretis_jam = kapasitas_Ah * fraksi_kapasitas_yang_diizinkan / I_rata_rata
```

Fraksi kapasitas yang diizinkan harus mengikuti rekomendasi pabrikan baterai
dan BMS. Waktu teoretis bukan jaminan waktu operasi; rugi konverter, suhu,
penuaan baterai, variasi beban, serta batas tegangan rendah akan menguranginya.
Jangan mengoperasikan baterai di luar rentang tegangan/temperatur pabrikan.

| Parameter | Nilai | Sumber/catatan |
|---|---:|---|
| Kimia dan konfigurasi baterai | | |
| Tegangan maksimum saat penuh (V) | | |
| Batas tegangan minimum yang diizinkan (V) | | |
| Kapasitas nominal (Ah) | | |
| Batas arus kontinu baterai (A) | | |
| Batas arus kontinu BMS (A) | | |
| Arus rata-rata terukur saat misi (A) | | |
| Fraksi kapasitas yang diizinkan | | |
| Estimasi waktu teoretis (jam) | | |

## Fuse, kabel, konektor, dan sakelar

Fuse melindungi kabel dari arus berlebih; fuse tidak menjamin ESC, baterai,
atau komponen lain tidak rusak. Pasang fuse utama sedekat mungkin dengan
terminal positif baterai. Pertimbangkan fuse cabang untuk tiap ESC agar
kegagalan satu cabang tidak membebani kabel cabang lain.

Pilih fuse hanya setelah memeriksa seluruh hal berikut:

1. Tegangan DC maksimum fuse lebih tinggi dari tegangan maksimum baterai.
2. Rating pemutusan DC fuse memadai untuk arus gangguan yang mungkin diberikan
   baterai.
3. Kurva waktu-arus fuse menoleransi arus operasi/transien yang sah, tetapi
   membuka sebelum kabel, konektor, atau komponen yang dilindungi melewati
   batasnya.
4. Rating arus kabel dan konektor setelah derating pemasangan tidak lebih
   rendah dari kebutuhan operasi dan koordinasi proteksi.
5. Fuse cabang sesuai rekomendasi pabrikan ESC dan melindungi kabel cabang.
6. Pemutus daya utama memiliki rating DC yang sesuai dan dapat memutus arus
   sistem dengan aman.

Rating arus kabel bergantung pada luas penampang, panjang, bahan isolasi,
temperatur, bundel kabel, ventilasi, dan lingkungan tertutup. Gunakan tabel
derating dari pabrikan/standar yang berlaku untuk pemasangan aktual. Pastikan
kabel positif dan negatif sama-sama mampu membawa arus. Jangan menggunakan
rating kabel otomotif/rumah tangga secara otomatis untuk kabel yang terendam
atau tertutup rapat.

Untuk memperkirakan jatuh tegangan kabel tembaga:

```text
R_loop = rho * (2 * panjang_satu_arah) / luas_penampang
V_jatuh = I * R_loop
P_hilang = I^2 * R_loop
```

Gunakan resistivitas (`rho`) dan luas penampang yang sesuai temperatur serta
spesifikasi kabel. Hitung tiap jalur utama/cabang dengan panjang sebenarnya.
Rugi daya dan panas meningkat sebanding kuadrat arus.

| Jalur | Arus desain (A) | Panjang satu arah (m) | Kabel/luas penampang | Rating setelah derating (A) | Fuse (tipe/rating DC) | Lulus? |
|---|---:|---:|---|---:|---|---|
| Baterai ke distribusi | | | | | | |
| Distribusi ke ESC 1 | | | | | | |
| Distribusi ke ESC 2 | | | | | | |
| Distribusi ke ESC 3 | | | | | | |
| Distribusi ke ESC 4 | | | | | | |
| Distribusi ke ESC 5 | | | | | | |
| Distribusi ke ESC 6 | | | | | | |

**Jangan menyalakan sistem** jika rating fuse, kemampuan pemutusan DC, rating
kabel/konektor, atau arus maksimum baterai belum terverifikasi. Jangan
memasang fuse yang lebih besar sekadar agar fuse tidak putus.

## Catu Pico W dan sensor

- Pisahkan jalur daya thruster/ESC dari catu logika Pico W. Gunakan regulator
  yang rentang inputnya sesuai tegangan baterai maksimum dan arus outputnya
  cukup untuk Pico W serta semua beban 3,3 V, dengan margin sesuai datasheet.
- Perhitungkan efisiensi regulator:

  ```text
  P_input = P_output / efisiensi
  I_input = P_input / V_input
  ```

- Jangan menyambungkan beberapa output 5 V/BEC ESC secara paralel kecuali
  manual ESC secara eksplisit menyatakan hal tersebut diperbolehkan. Pilih satu
  sumber logika yang dirancang untuk beban, isolasi BEC lain sesuai manual.
- Ground sinyal ESC dan ground Pico harus memiliki referensi bersama sesuai
  rancangan, tetapi kabel arus tinggi thruster tidak boleh melewati jalur
  ground tipis/board Pico. Rancang jalur balik arus daya agar tidak
  menimbulkan ground bounce pada sinyal.
- GPIO Pico W adalah logika 3,3 V. Jangan pernah menghubungkan 5 V ke GPIO,
  SDA, atau SCL. Pastikan pull-up I2C tidak menarik jalur ke 5 V.
- Pico W memantau VSYS melalui pembagi tegangan bawaan ke ADC3/GPIO29.
  Pembacaan itu hanya mewakili tegangan pada VSYS: jika baterai 12 V masuk
  ke buck converter lalu buck memberi daya pada Pico, yang terukur adalah
  output buck, bukan tegangan baterai. Batas VSYS adalah 5,5 V; jangan
  sambungkan baterai 12 V langsung ke VSYS. Untuk mengukur baterai utama,
  gunakan sensor atau pembagi tegangan eksternal yang dirancang dan dikalibrasi
  sesuai rentang tegangan aktual. ADC VSYS tidak mengukur arus maupun kapasitas.
- Sensor GY-271 dan GY-521 di proyek ini diberi 3V3 dan berbagi GP26/GP27.
  Pastikan modul yang digunakan kompatibel dengan logika 3,3 V dan jangan
  menghubungkan output 5 V ke Pico.

| Jalur logika | Tegangan masuk (min/maks) | Tegangan/arus output | Beban dihitung/diukur | Margin dan rating termal |
|---|---|---|---|---|
| Baterai ke regulator logika | | | | |
| Regulator ke Pico W | | | | |
| 3V3 Pico ke sensor | | | | |

## Pemeriksaan sebelum uji dan operasi

### Sebelum diberi daya

- [ ] Baca manual semua baterai, ESC, thruster, regulator, fuse, dan konektor.
- [ ] Semua nilai pada lembar perhitungan terisi dan diperiksa orang kedua.
- [ ] Polaritas, isolasi sambungan, strain relief, ukuran kabel, dan rating
      konektor sudah diperiksa; tidak ada tembaga terbuka atau kabel terjepit.
- [ ] Fuse DC terpasang dekat baterai; sakelar/pemutus utama dapat dijangkau.
- [ ] Tidak ada kabel daya thruster yang melewati breadboard atau jalur Pico.
- [ ] Tidak ada output BEC yang diparalelkan tanpa izin manual ESC.
- [ ] Tidak ada jalur I2C/GPIO yang dapat menerima 5 V.
- [ ] Baling-baling dilepas, ROV DISARM, dan baterai dapat segera diputus.
- [ ] Uji kontinuitas/korsleting dilakukan tanpa baterai tersambung.

### Uji bertahap

1. Uji regulator logika tanpa ESC terhubung; ukur tegangan dan temperatur.
2. Sambungkan Pico/sensor, pastikan tegangan tetap dalam spesifikasi.
3. Uji satu ESC pada satu waktu dengan baling-baling dilepas, pembatas arus
   yang sesuai, dan catu yang dapat diputus cepat.
4. Periksa tegangan, arus, konektor, kabel, regulator, dan temperatur setelah
   setiap tahap. Hentikan jika ada bau, asap, suara abnormal, konektor panas,
   tegangan turun tidak wajar, atau komponen melampaui batas datasheet.
5. Tambahkan beban hanya setelah tahap sebelumnya lolos. Jangan langsung
   menguji enam thruster pada daya penuh.
6. Sebelum uji air, lakukan inspeksi kebocoran dan uji segel secara terpisah.
   Jauhkan sambungan listrik yang tidak dirancang untuk basah dari air.
   Jangan mengisi baterai di dalam ROV atau di dekat material mudah terbakar.

Firmware dapat mengirim pulsa netral saat STOP/failsafe, tetapi itu bukan
pemutus daya dan tidak melindungi dari korsleting atau ESC yang gagal.
Sediakan pemutus daya fisik yang mudah dijangkau. Jangan pernah meninggalkan
baterai yang sedang diuji atau diisi tanpa pengawasan.

## Catatan hasil verifikasi

| Tanggal | Konfigurasi baterai/thruster | Arus terukur | Temperatur tertinggi | Hasil dan tindakan |
|---|---|---:|---:|---|
| | | | | |

Perbarui lembar ini setiap kali baterai, thruster, ESC, kabel, konektor,
regulator, atau tata letak distribusi daya berubah.
