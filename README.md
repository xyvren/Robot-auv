# ROV 6 Thruster - Raspberry Pi Pico W + PlatformIO

Firmware ini mengontrol enam ESC bidirectional dengan mixer sumbu surge,
sway, heave, yaw, pitch, dan roll. Konfigurasi awal mengasumsikan empat
thruster horizontal berpola X serta dua thruster vertikal. Arah pemasangan
dan urutan fisik motor belum diverifikasi dari STL bracket, jadi uji mixer
dengan baling-baling dilepas sebelum digunakan di air.

LED bawaan Pico W menyala tetap setelah firmware dan hotspot siap.

## Pin awal

| Thruster | Posisi asumsi | GPIO Pico W |
|----------|---------------|-------------|
| T1 | Depan kiri | GP2 |
| T2 | Depan kanan | GP3 |
| T3 | Belakang kiri | GP4 |
| T4 | Belakang kanan | GP5 |
| T5 | Vertikal depan | GP6 |
| T6 | Vertikal belakang | GP7 |

Pin dan parameter motor bisa diubah dari halaman web. Perubahan pin,
kalibrasi ESC, dan mixer disimpan saat tombol **Simpan** ditekan; Pico
kemudian reboot agar output diinisialisasi ulang.
GP26 dan GP27 disediakan khusus untuk bus I2C sensor dan tidak tersedia
sebagai pin thruster.

## Struktur kode dan konfigurasi

- `src/modules/rov_config.h`: pin thruster default, pin/alamat I2C, tipe sensor,
  mixer awal, pulsa ESC, batas daya, ramp, dan waktu failsafe. Ubah nilai
  awal di sini sebelum build.
- `src/modules/rov_sensors.h` dan `src/modules/rov_sensors.cpp`: deteksi serta pembacaan
  MPU-6050, HMC5883L, QMC5883L, dan MS5803-14BA. Akuisisi I2C berjalan di
  core 1; data dipublikasikan sebagai snapshot tersinkron untuk core utama.
  MPU-6050 yang gagal dibaca dicoba inisialisasi ulang secara berkala.
- `src/modules/web_pages.h` dan `src/modules/web_pages.cpp`: halaman setup dan antarmuka
  web (Monitoring, Kontrol, Pengaturan).
- `src/main.cpp`: orkestrasi firmware, validasi/penyimpanan konfigurasi,
  API web, kendali thruster, serta web server dan loop utama di core 0.

Core 0 menangani web server, API kontrol, dan pembaruan output thruster.
Core 1 membaca sensor I2C setiap 50 ms agar transaksi sensor tidak menahan
loop web/kendali. Web menerima salinan snapshot sensor yang dilindungi critical
section singkat; pembacaan I2C tidak dilakukan sambil mengunci data bersama.

Pengaturan motor yang disimpan di EEPROM melalui halaman web tetap
menggantikan nilai default dari `rov_config.h`. Jika mengubah pin bus sensor,
perbarui `I2C_SDA_PIN` dan `I2C_SCL_PIN`; pin tersebut otomatis dilarang
digunakan oleh thruster.

## Sensor dan monitoring

GY-521 (MPU-6050), GY-271 (HMC5883L), dan MS5803-14BA berbagi satu bus I2C:

| Sensor | VCC | GND | SDA | SCL |
|--------|-----|-----|-----|-----|
| GY-521 / MPU-6050 | 3V3 | GND | GP26 | GP27 |
| GY-271 / HMC5883L | 3V3 | GND | GP26 | GP27 |
| SparkFun MS5803-14BA | 3V3 | GND | GP26 | GP27 |

Firmware memakai I2C1 (`Wire1`) pada GP26/GP27 di Pico W dan memindai
MPU-6050, kompas, dan MS5803-14BA saat boot. GY-271 yang menggunakan chip
QMC5883L kompatibel juga dideteksi otomatis. MS5803 dipindai pada alamat I2C
0x76 dan 0x77 serta memvalidasi CRC koefisien PROM. Hubungkan VCC sensor ke
3V3 Pico W, bukan 5 V; semua sensor harus memakai ground bersama dan level
logika I2C 3,3 V. Pastikan PCB/konektor breakout dipasang di housing yang
kedap air dan sesuai tekanan; jangan menganggap semua breakout kedap air.

### Posisi awal MPU-6050

Sebelum menyalakan Pico, letakkan ROV diam pada permukaan datar dan rata dalam
posisi badan yang dianggap level saat operasi. Pasang modul dengan kuat dan
sejajarkan sumbu X modul dengan arah hidung-belakang ROV, sumbu Y dengan arah
kiri-kanan, dan sumbu Z ke atas saat ROV level. Periksa label sumbu pada
breakout karena orientasinya dapat berbeda. Setelah boot, periksa telemetri
Pitch/Roll: keduanya seharusnya mendekati 0° saat ROV tetap level.

Firmware menginisialisasi estimasi sudut dari sampel akselerometer pertama,
tetapi belum mengukur bias giroskop, menyimpan offset kalibrasi, atau
mengompensasi pemasangan sensor yang miring. Jika sudut tidak mendekati 0°,
periksa kembali kerataan ROV dan orientasi/pemasangan modul; jangan
mengandalkan stabilisasi sebelum arah respons pitch dan roll diverifikasi
dengan baling-baling dilepas.

## Urutan uji gerak berdurasi

Urutan pada halaman **Kontrol** tidak aktif saat boot dan hanya dapat dimulai
manual setelah thruster di-ARM. Urutannya: maju, putar kanan, mundur, maju,
geser kanan, lalu geser kiri; tiap tahap berjalan 10 detik. Kekuatan mengikuti
slider **Kekuatan gerakan** dan tetap dibatasi oleh batas daya sistem. Urutan
otomatis DISARM setelah selesai. Tombol **STOP** membatalkan urutan dan
menetralkan semua output; heartbeat web yang terputus juga memicu failsafe.

Ini adalah uji berbasis waktu, **bukan** navigasi atau jaminan jarak 1 m /
putaran 180°. Firmware tidak memiliki lokalisasi untuk mengukur perpindahan
atau heading. Hasil dipengaruhi mixer, pemasangan, tegangan, beban, dan arus
air. Pastikan T1–T4 sudah diuji berfungsi; lakukan uji awal tanpa baling-baling,
amankan ROV, awasi langsung, dan siapkan STOP/pemutus daya independen. Jangan
menjalankan urutan tanpa pengawasan atau di area yang berisiko.

Pico W juga memantau tegangan suplai papan melalui pembagi tegangan VSYS
bawaan ke ADC3/GPIO29. Menu **Monitoring** menampilkan nilai VSYS; pembacaan
diperbarui setiap 250 ms dan dirata-rata dari delapan sampel. Nilainya adalah
tegangan yang masuk ke Pico, bukan selalu tegangan baterai utama. Jika baterai
12 V memberi daya ke Pico melalui buck converter, dashboard menampilkan
tegangan keluaran buck (misalnya sekitar 5 V), bukan tegangan baterai 12 V.
Jangan sambungkan baterai 12 V langsung ke VSYS; batas input VSYS Pico W
adalah 5,5 V. Pengukuran internal ini bukan pengukur arus atau persentase
kapasitas baterai, dan nilai tegangan ADC bersifat perkiraan tanpa kalibrasi.
Pantau baterai utama dengan sensor/rangkaian pengukur yang sesuai dengan
tegangan serta arus maksimum baterai.

Menu **Monitoring** menampilkan status sensor, heading magnetometer,
grafik histori pitch/roll dan laju giroskop, akselerasi, status thruster,
tekanan absolut dan suhu MS5803, serta grafik tekanan, status I2C, dan waktu
hidup. Tekan **Nolkan di permukaan** saat ROV diam di permukaan air untuk
menghitung estimasi kedalaman relatif dari perubahan tekanan (asumsi massa
jenis air laut 1025 kg/m³). Titik nol hanya tersimpan di RAM dan perlu diatur
ulang setelah Pico reboot. Hasilnya perkiraan kedalaman relatif, bukan
navigasi posisi atau kontrol otomatis. Grafik menyimpan histori terbaru di
browser dan digambar tanpa library eksternal; sampel diperbarui setiap
500 ms. Heading kompas masih mentah, belum dikalibrasi, dan belum
dikoreksi terhadap kemiringan; nilainya dapat terganggu oleh ESC, motor,
arus besar, logam, dan orientasi pemasangan sensor. MPU-6050 tidak
menyediakan referensi heading absolut, sehingga yaw dari giroskop tidak
ditampilkan sebagai arah yang stabil.

Stabilisasi pitch/roll tersedia sebagai opsi di menu **Kontrol** setelah ARM
dan hanya berjalan saat dinyalakan operator. Saat diaktifkan, firmware
menargetkan sudut pitch/roll 0°; input Pitch/Roll mengubah target sudut,
sedangkan gerak lain tetap manual. Pengendali memakai sudut komplementer
MPU-6050 dan laju giroskop, tanpa komponen integral, dengan koreksi output
dibatasi 25%. Jika IMU gagal, stabilisasi mati otomatis dan sumbu Pitch/Roll
dinetralkan sampai operator menekan **Reset stabilisasi**. Fitur ini tidak
menahan kedalaman, posisi, atau heading.

Mixer default tidak menyediakan roll independen, sehingga stabilisasi roll
baru bekerja setelah kontribusi sumbu Roll diatur dengan mixer thruster yang
sesuai. Arah sumbu sensor dan mixer bergantung orientasi pemasangan; validasi
arah koreksi dan mixer dengan propeler dilepas, lalu uji dengan daya rendah
serta ROV diamankan. Fitur stabilisasi tidak menggantikan pemeriksaan sensor,
kalibrasi, failsafe, atau pengawasan operator. MS5803 mengukur tekanan;
estimasi kedalaman bergantung pada nol permukaan, massa jenis air, pemasangan,
dan kalibrasi.

## Hotspot statis dan halaman kontrol

1. Pico membuat hotspot sendiri tanpa router dengan IP tetap
   `192.168.4.1` dan subnet `255.255.255.0`.
2. Pada setup pertama, sambungkan ke hotspot terbuka `SAUVAC` tanpa password.
   Buka `http://192.168.4.1` dan buat password web unik (minimal 6 karakter).
   Hotspot memang tidak memakai password Wi-Fi.
3. Setelah Pico reboot, sambungkan ke nama hotspot yang dipilih dan buka
   `http://192.168.4.1`. Perubahan nama/password hotspot bisa dilakukan
   dari halaman parameter dan diterapkan setelah reboot.
4. Login menggunakan nama pengguna `menang` dan password web yang dipilih.
   Kredensial disimpan di EEPROM Pico. Tidak ada password web bawaan.
   Konfigurasi versi 2 lama direset ke setup awal karena menggunakan
   kredensial bawaan yang tidak aman.
5. Di menu **Kontrol**, tekan **ARM**, lalu tahan tombol gerakan seperti
   Maju, Kiri, Naik, atau Putar kiri. Lepaskan tombol untuk menetralkan
   sumbu tersebut. Atur kekuatan gerakan dengan slider; kontrol praktis
   memakai mixer thruster. Bagian **Kontrol langsung T1–T6** menggerakkan
   satu thruster secara terpisah: tahan arah + atau arah −, lalu lepaskan
   untuk kembali netral. Kontrol langsung menonaktifkan mixer sementara;
   batas daya dan ramp tetap berlaku. Arah +/− mengikuti konfigurasi kabel
   dan pemasangan, jadi uji setiap motor dengan daya rendah dan propeler
   dilepas atau ROV diamankan. Slider sumbu presisi tetap tersedia.
   Toggle **Stabilisasi pitch/roll** bersifat opsional: saat aktif,
    stabilisasi menargetkan orientasi 0° saat diaktifkan, dan input Pitch/Roll
   mengubah target sudut. Sumbu yang tidak memiliki kontribusi mixer tidak
   bisa distabilkan.
   **STOP** menghentikan semua output.
   Jika halaman kehilangan fokus/tersembunyi atau heartbeat terputus,
   firmware melepas ARM dan mengembalikan output ke netral.
6. Di menu **Pengaturan**, bagian **Parameter setiap thruster** menampilkan
   konfigurasi GPIO, pulsa minimum/netral/maksimum, arah putaran, dan nilai
   mixer T1–T6. Kolom mixer: **Surge** maju/mundur, **Sway** geser kiri/kanan,
   **Heave** naik/turun, **Yaw** putar kiri/kanan, **Pitch** hidung naik/turun,
   dan **Roll** badan miring kiri/kanan. Setiap baris adalah satu thruster;
   nilai `0` berarti tidak membantu sumbu tersebut, tanda positif/negatif
   menentukan arah kontribusi, dan besar nilai menentukan bobotnya (`1`
   penuh, `0.5` setengah, `-1` penuh ke arah sebaliknya). Arah fisik tetap
   bergantung pada pemasangan motor dan pengaturan Balik arah. Susunan enam
   thruster ini tidak menyediakan kendali roll independen secara bawaan.
   Bagian **Parameter sistem dan koneksi** mengatur batas daya,
   ramp, waktu failsafe, password web, dan nama hotspot.
   Hotspot selalu tanpa password Wi-Fi; password halaman kontrol tetap wajib.

Web kontrol memakai HTTP dan autentikasi Basic, bukan HTTPS. Siapa pun dalam
jangkauan dapat bergabung ke hotspot terbuka, dan kredensial dapat terlihat
oleh penyadap jaringan. Gunakan password unik, batasi akses fisik ke hotspot,
dan jangan port-forward ke Internet.

## Catatan commissioning dan keselamatan

- Sebelum menyambungkan baterai, isi dan tinjau [lembar perhitungan kelistrikan
  dan keselamatan](./PERHITUNGAN_KELISTRIKAN_DAN_KESELAMATAN.md). Nilai fuse,
  kabel, dan catu daya harus berdasarkan datasheet perangkat yang benar-benar
  digunakan; jangan menebak rating.
- Nilai awal ESC adalah min/netral/maks `1000/1500/2000` mikrodetik dan
  batas output 15%. Ini nilai dugaan umum, bukan kalibrasi ESC yang sudah
  diverifikasi; cek manual ESC dan ubah nilainya di halaman web.
- Firmware boot dalam kondisi DISARM. ARM hanya setelah area aman. Lepas
  baling-baling saat menguji arah, mixer, rentang pulsa, dan ESC.
- Gunakan ESC bidirectional yang mengenali pulsa netral. Jangan gunakan
  firmware ini dengan ESC yang tidak mendukung thrust maju/mundur.
- Gunakan ground bersama antara Pico dan input sinyal ESC. Jangan pernah
  memasukkan 5 V ke GPIO; GPIO Pico W bekerja pada 3,3 V.
- Empat thruster horizontal dan dua vertikal tidak memberi kendali roll
  independen pada mixer awal. Seluruh koefisien mixer dapat disetel dari web.
- Tombol STOP dan failsafe menghentikan sinyal dengan mengirim pulsa netral;
  keselamatan listrik tetap memerlukan pemutus daya fisik untuk ESC.

## Ide pengembangan: RL-PID (belum diimplementasikan)

RL-PID dapat dipertimbangkan sebagai opsi masa depan di menu Pengaturan agar
pengguna bisa mengaktifkan atau menonaktifkan pengujian pengendali. Saat ini
firmware **belum memiliki loop PID maupun Reinforcement Learning**; kontrol
tetap manual melalui sumbu dan mixer. Bagian ini hanya catatan rencana, bukan
fitur yang bisa diaktifkan.

Sebelum implementasi, tentukan sumbu dan setpoint yang akan diuji, serta
pastikan mixer dan susunan thruster benar-benar mampu mengendalikan sumbu
tersebut. Mixer awal, misalnya, tidak memberi kendali roll independen.
Pengujian sebaiknya dimulai dalam simulasi atau dengan propeller dilepas,
RL-PID default **OFF**, gain dan output thruster dibatasi, dan tersedia
**STOP** serta fallback ke kontrol manual/PID tetap. Tentukan juga cara
mengukur respons/reward dan kapan hasil belajar boleh disimpan; jangan
menulis hasil ke flash pada setiap siklus kendali.

## Build dan upload

Build:

```sh
pio run -e picow
```

Untuk upload manual, salin `.pio/build/picow/firmware.uf2` ke drive `RPI-RP2`
saat Pico W berada dalam mode BOOTSEL.
