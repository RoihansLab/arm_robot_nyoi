# Implementation Plan: Robot Arm & Conveyor ESP32

Dokumen ini dibuat untuk **diikuti urut dari Fase 0 sampai Fase 7**. Jangan lanjut ke fase berikutnya sebelum checklist "Selesai kalau" di fase sekarang terpenuhi. Bisa juga dipakai sebagai instruksi untuk coding agent/AI.

## 0. Keputusan Teknis (sudah final)

| Hal | Keputusan |
| --- | --- |
| Board | ESP32 DevKit, framework Arduino |
| Library | `ESP32Servo`, `WebServer.h`, `WiFi.h`, `Preferences.h` |
| Jaringan | WiFi **Access Point** (SSID `RobotArm`, IP `192.168.4.1`) |
| Halaman web | HTML disimpan di `PROGMEM` (satu file, tanpa library eksternal) |
| Eksekusi | Non-blocking (`millis()`), **dilarang `delay()`** di loop utama |
| Penyimpanan | `Preferences` (NVS), auto-save tiap perubahan |
| Fitur wajib | Simpan flash, batas sudut, home saat boot, E-Stop |

## 1. Struktur File

```
robot_arm_conveyor/
├── robot_arm_conveyor.ino   // setup(), loop(), state machine
├── config.h                 // pin, batas sudut, home, konstanta waktu
├── storage.h                // load/save sequence ke NVS
├── motion.h                 // interpolasi servo, kontrol conveyor
└── webpage.h                // HTML remote (PROGMEM)
```

## 2. Konfigurasi (`config.h`)

```cpp
// Pin
const int PIN_SERVO[4] = {13, 14, 27, 26};   // base, shoulder, elbow, gripper
const int PIN_ENA = 25, PIN_IN1 = 32, PIN_IN2 = 33;

// Batas sudut aman (KALIBRASI DULU di Fase 2)
const int ANGLE_MIN[4] = {0, 20, 20, 30};
const int ANGLE_MAX[4] = {180, 160, 160, 120};

// Posisi home (KALIBRASI DULU di Fase 2)
const int HOME[4] = {90, 90, 90, 90};

// Waktu & gerak
const unsigned long CONVEYOR_MS = 5000;  // sesuai dosen
const int STEP_DEG = 1;                  // derajat per langkah
const int STEP_MS  = 15;                 // jeda antar langkah
const int MAX_FRAMES = 50;
const int CONVEYOR_PWM = 200;            // 0-255
```

## 3. Struktur Data & State

```cpp
struct Frame { uint8_t angle[4]; uint16_t holdMs; };
Frame seq[2][MAX_FRAMES];   // seq[0]=A (taruh), seq[1]=B (ambil)
uint8_t seqLen[2] = {0, 0};

enum State { BOOT_HOME, IDLE, RECORDING, PLAY_A, CONVEYOR_RUN, PLAY_B, ESTOP };
State state = BOOT_HOME;

int curAngle[4];     // sudut aktual saat ini
int tgtAngle[4];     // sudut target
```

### Diagram state

```
BOOT_HOME -> IDLE <-> RECORDING
IDLE --/play--> PLAY_A -> CONVEYOR_RUN (5s) -> PLAY_B -> PLAY_A ... (loop)
semua state --/estop--> ESTOP --/reset--> IDLE
/stop (saat play) -> IDLE
```

## 4. Fase Pengerjaan

### Fase 1: Tes hardware dasar

**Tugas**

- [ ] Wiring sesuai pin. Servo dari buck converter 5-6 V (sumber terpisah, kapasitor terpasang). Baterai 9 V ke terminal +12V L298N, terminal +5V L298N dikosongkan, jumper 5V-EN terpasang, jumper ENA dilepas. Semua GND disatukan.
- [ ] Cek spesifikasi tegangan servo di badan servo sebelum menyalakan.
- [ ] Sketch tes: sapu tiap servo 0 → 180 → 0 satu per satu.
- [ ] Sketch tes: conveyor maju 3 detik, berhenti.

**Selesai kalau:** semua 4 servo bergerak, conveyor berputar, ESP32 tidak reset saat servo bergerak.

### Fase 2: Kalibrasi

**Tugas**

- [ ] Tentukan `ANGLE_MIN/MAX` tiap servo (cari titik sebelum lengan membentur).
- [ ] Tentukan `HOME` (posisi lengan aman dan netral).
- [ ] Isi nilai ke `config.h`.

**Selesai kalau:** semua servo bisa digerakkan di dalam batas tanpa membentur.

### Fase 3: Web server & remote dasar

**Tugas**

- [ ] WiFi AP + `WebServer` port 80.
- [ ] `GET /` kirim halaman HTML dari `PROGMEM`.
- [ ] `GET /servo?id=&angle=`: `constrain()` ke batas, lalu gerakkan servo.
- [ ] Halaman: 4 slider, angka sudut, throttle 50 ms di JS.
- [ ] `GET /status` mengembalikan JSON `{state, lenA, lenB, angles:[...]}`.

**Selesai kalau:** dari HP, slider menggerakkan servo mulus dan tidak bisa melewati batas aman.

### Fase 4: Record & penyimpanan flash

**Tugas**

- [ ] `GET /save?seq=A|B&hold=ms`: tambah frame dari `curAngle`, tolak jika penuh (50).
- [ ] `GET /undo?seq=`, `GET /clear?seq=`.
- [ ] `storage.h`: `saveSeq(i)` dan `loadAll()` memakai `Preferences.putBytes/getBytes` + simpan panjang sequence.
- [ ] Panggil `saveSeq()` setiap save/undo/clear; panggil `loadAll()` di `setup()`.
- [ ] Halaman: tombol pilih A/B, input hold, Save, Undo, Clear, daftar frame.

**Selesai kalau:** rekam A dan B, matikan ESP32, nyalakan lagi, daftar frame masih ada.

### Fase 5: Home saat boot

**Tugas**

- [ ] Di `setup()`: conveyor dipastikan mati (ENA=0, IN1=IN2=LOW).
- [ ] `state = BOOT_HOME`: set `tgtAngle = HOME`, interpolasi pelan dari posisi awal.
- [ ] Setelah semua servo tiba di home, `state = IDLE`.

**Selesai kalau:** setiap boot lengan bergerak halus ke home, conveyor diam, sistem berhenti di IDLE.

### Fase 6: Playback & loop

**Tugas**

- [ ] Fungsi `stepServos()`: tiap `STEP_MS`, geser `curAngle` ke `tgtAngle` sebesar `STEP_DEG`, tulis ke servo, return `true` jika semua sudah tiba.
- [ ] Mesin playback per frame: set target → tunggu tiba → tunggu `holdMs` → frame berikutnya.
- [ ] `PLAY_A` selesai → `CONVEYOR_RUN`: nyalakan conveyor, catat `millis()`, setelah 5000 ms matikan → `PLAY_B`.
- [ ] `PLAY_B` selesai → kembali ke `PLAY_A` (loop).
- [ ] `GET /play`: tolak jika `seqLen[0]==0` atau `seqLen[1]==0`. `GET /stop`: hentikan conveyor, kembali ke IDLE.

Pseudo-code `loop()`:

```cpp
void loop() {
  server.handleClient();
  switch (state) {
    case BOOT_HOME:     if (stepServos()) state = IDLE; break;
    case PLAY_A:        runSequence(0, PLAY_A, CONVEYOR_RUN); break;
    case CONVEYOR_RUN:  if (millis()-tConv >= CONVEYOR_MS) { conveyorOff(); state = PLAY_B; } break;
    case PLAY_B:        runSequence(1, PLAY_B, PLAY_A); break;
    case ESTOP: case IDLE: case RECORDING: break;
  }
}
```

**Selesai kalau:** satu siklus penuh A → conveyor 5 s → B berjalan, lalu otomatis berulang.

### Fase 7: Emergency Stop & pengujian akhir

**Tugas**

- [ ] `GET /estop`: `conveyorOff()`, `tgtAngle = curAngle` (servo membeku), `state = ESTOP`.
- [ ] Saat `ESTOP`, tolak `/servo`, `/play`, `/save`. Hanya `/reset` dan `/status` yang aktif.
- [ ] `GET /reset`: `state = IDLE` (tidak otomatis lanjut loop).
- [ ] Tombol E-Stop merah besar di web, selalu terlihat; tombol Reset muncul saat ESTOP.
- [ ] Jalankan semua uji di bagian 5.

**Selesai kalau:** semua uji di tabel bagian 5 lulus.

## 5. Daftar Uji

| No | Uji | Hasil yang diharapkan |
| --- | --- | --- |
| 1 | Rekam A & B → cabut daya → nyalakan | Rekaman masih ada |
| 2 | Slider melewati batas | Servo berhenti di batas |
| 3 | Nyalakan ESP32 | Lengan ke home pelan, conveyor diam |
| 4 | E-Stop saat Play | Conveyor mati, servo diam \< 200 ms, tidak lanjut sendiri |
| 5 | Reset lalu Play | Lanjut normal |
| 6 | Play dengan sequence kosong | Ditolak, ada pesan |
| 7 | Loop 30 menit | Tidak ada reset/hang |
| 8 | Tahan frame penuh (50) lalu Save lagi | Ditolak, tidak crash |

## 6. Aturan Selama Pengerjaan

1. Satu fase selesai dan lulus uji baru lanjut.
2. Jangan pakai `delay()` di `loop()` atau di handler web.
3. Semua sudut yang masuk ke servo wajib lewat `constrain()` dengan `ANGLE_MIN/MAX`.
4. Setiap perubahan sequence langsung disimpan ke flash.
5. Angka kalibrasi hanya diubah di `config.h`, tidak tersebar di kode.

## 7. Risiko Singkat

- ESP32 reset saat servo jalan → cek supply servo terpisah, kapasitor, GND bersama.
- Timer 5 s kurang pas → ubah `CONVEYOR_MS` atau `CONVEYOR_PWM` saat uji Fase 6.
- Web lemot → pastikan throttle slider aktif dan tidak ada `delay()` panjang.