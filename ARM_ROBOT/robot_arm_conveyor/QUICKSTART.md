# Quickstart - Robot Arm Conveyor

## Library yang Harus Diinstall di Arduino IDE

### ⬇️ Install Manual via Library Manager
> Arduino IDE → **Sketch → Include Library → Manage Libraries...**

| Nama Library (ketik di search) | Author | Keterangan |
|---|---|---|
| `ESP32Servo` | Kevin Harrington, John K. Bennett | Kontrol servo di ESP32 — **WAJIB install** |

### ✅ Sudah Built-in (tidak perlu install)
> Otomatis ada setelah install board **esp32 by Espressif** di Board Manager

| Library | Keterangan |
|---|---|
| `WebServer.h` | HTTP web server bawaan ESP32 |
| `WiFi.h` | WiFi AP & Station bawaan ESP32 |
| `Preferences.h` | Simpan data ke flash NVS bawaan ESP32 |

### 📦 Cara Install Board ESP32 (kalau belum)
1. Arduino IDE → **File → Preferences**
2. Tambahkan URL ini di *Additional Board Manager URLs*:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. **Tools → Board → Boards Manager** → cari `esp32` → Install **esp32 by Espressif Systems**

## Board Setting Arduino IDE
- Board: `ESP32 Dev Module`
- Upload Speed: `921600`
- Flash Size: `4MB (32Mb)`
- Partition Scheme: `Default 4MB with spiffs`

## Cara Upload
1. Buka folder `robot_arm_conveyor/`
2. Klik `robot_arm_conveyor.ino` (harus nama folder = nama .ino)
3. Pilih port COM yang sesuai
4. Upload

## Cara Pakai

### 1. Koneksi
1. Setelah upload, ESP32 membuat WiFi AP: **"RobotArm"** (tanpa password)
2. Hubungkan HP/laptop ke WiFi **"RobotArm"**
3. Buka browser → `http://192.168.4.1`
4. Tampil antarmuka **Gamepad DualShock** — siap dipakai!

### 2. Pilih Mode
- **Manual** → kontrol servo langsung pakai analog & tombol wajah
- **Otomatis** → recording & playback keyframe

---

### 3. Mode MANUAL — Kontrol Servo Langsung

| Kontrol | Fungsi |
|---|---|
| **Analog Kiri ←→** | Gerakkan **Base** (servo 0) ke kiri/kanan |
| **Analog Kiri ↑↓** | Gerakkan **Shoulder** (servo 1) naik/turun |
| **△ Triangle** (tahan) | **Elbow** naik (servo 2) |
| **✕ Cross** (tahan) | **Elbow** turun (servo 2) |
| **□ Square** | **Grip** — tutup gripper ke posisi minimum |
| **○ Circle** | **Release** — buka gripper ke posisi maximum |

> Analog stick bekerja sebagai **rate controller**: makin jauh dari tengah = makin cepat bergerak. Lepas = servo berhenti di posisi saat ini.

---

### 4. Mode OTOMATIS — Record & Playback Keyframe

#### Merekam Sequence
1. Pilih tab **Seq A** atau **Seq B** (di antara bumper L1/R1)
2. Ganti ke mode **Manual** dulu → posisikan lengan ke posisi yang diinginkan
3. Ganti ke mode **Otomatis**
4. Atur **Hold (ms)** — berapa lama lengan diam di posisi ini
5. Tekan **Guide ⬤ (SAVE)** → frame tersimpan ke sequence aktif
6. Ulangi untuk semua titik gerakan

#### Tombol Keyframe

| Tombol | Fungsi |
|---|---|
| **Guide / PS ⬤** | Simpan posisi saat ini sebagai keyframe |
| **L1** | Undo — hapus frame terakhir |
| **R1** | Redo — kembalikan frame yang di-undo |
| **🗑 Delete All** | Hapus semua frame sequence aktif (konfirmasi dulu) |

#### Menjalankan Simulasi
1. Pastikan **Seq A** dan **Seq B** masing-masing sudah punya minimal 1 frame
2. Tekan **STR / Start** → loop berjalan: **A → Konveyor 5 detik → B → A → ...**
3. Tekan **SEL / Select** untuk **Stop**

---

### 5. Status Bar
Di bagian atas layar selalu tampil:
```
State: Siap | A:3  B:2 frame | [90 / 90 / 90 / 90] deg
```
- **State** — kondisi sistem saat ini
- **A / B** — jumlah frame per sequence
- **Sudut** — posisi aktual servo Base / Shoulder / Elbow / Gripper

## Urutan Kalibrasi (Fase 2)
1. Upload firmware, buka web
2. Gerakkan tiap servo ke batas aman minimal dan maksimal
3. Catat angka sudutnya
4. Edit `config.h` bagian `ANGLE_MIN`, `ANGLE_MAX`, `HOME`
5. Upload ulang

## Pin Mapping
| Fungsi | GPIO |
|---|---|
| Servo Base | 13 |
| Servo Shoulder | 14 |
| Servo Elbow | 27 |
| Servo Gripper | 26 |
| L298N ENA (PWM) | 25 |
| L298N IN1 | 32 |
| L298N IN2 | 33 |
