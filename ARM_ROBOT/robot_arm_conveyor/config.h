#pragma once

// =============================================================
//  config.h  -  Konstanta hardware & waktu
//  KALIBRASI: ubah ANGLE_MIN, ANGLE_MAX, HOME setelah Fase 2
// =============================================================

// -- Pin Servo (base, shoulder, elbow, gripper) ---------------
const int PIN_SERVO[4] = {13, 14, 27, 26};

// -- Pin Motor Conveyor (L298N) --------------------------------
//    ENA sudah dijumper di modul L298N (tidak perlu pin ESP32)
const int PIN_IN1 = 32;
const int PIN_IN2 = 33;

// -- Batas Sudut Aman per Servo (UBAH SETELAH KALIBRASI) ------
//   indeks: 0=base  1=shoulder  2=elbow  3=gripper
const int ANGLE_MIN[4] = {  0,  20,  20,  30};
const int ANGLE_MAX[4] = {180, 160, 160, 120};

// -- Posisi Home (UBAH SETELAH KALIBRASI) ---------------------
const int HOME[4] = {90, 90, 90, 90};

// -- Konveyor -------------------------------------------------
const unsigned long CONVEYOR_MS  = 5000;   // durasi jalan (ms)

// -- Interpolasi Servo ----------------------------------------
const int STEP_DEG = 1;    // derajat per langkah
const int STEP_MS  = 15;   // interval antar langkah (ms)

// -- Recording ------------------------------------------------
const int MAX_FRAMES  = 50;  // maks frame per sequence
const int NUM_SEQ     = 10;  // jumlah sequence (1-10)

// -- Tipe data frame (didefinisikan di sini agar semua header bisa pakai) --
struct Frame {
    uint8_t  angle[4];  // sudut tiap servo
    uint8_t  order[4];  // urutan pergerakan servo
    uint16_t holdMs;    // durasi tahan (ms)
};

// -- Data sequence (definisi di .ino, dideklarasikan extern di sini) --------
extern Frame   seq[NUM_SEQ][MAX_FRAMES];
extern uint8_t seqLen[NUM_SEQ];

// -- WiFi Access Point ----------------------------------------
const char* AP_SSID = "RobotArm";
const char* AP_PASS = "";   // tanpa password
