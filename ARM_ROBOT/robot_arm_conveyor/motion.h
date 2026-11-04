#pragma once
#include <ESP32Servo.h>
#include "config.h"

// =============================================================
//  motion.h  -  Interpolasi servo & kontrol konveyor
// =============================================================

Servo srv[4];

// -- State interpolasi servo ----------------------------------
int curAngle[4];
int tgtAngle[4];
static unsigned long lastStepMs = 0;

// -- State Konveyor & Kickstart -------------------------------
bool          conveyorManualOn   = false;                  // true = user aktifkan via gamepad
bool          sensorEnabled      = true;                   // true = sensor aktif (default)
uint8_t       conveyorSpeed      = CONVEYOR_SPEED_DEFAULT; // 0-255
bool          isConveyorRunning  = false;                  // status aktual motor konveyor aktif
bool          isKickstarting     = false;                  // true selama fase sentakan awal
unsigned long kickstartStartTime = 0;                      // millis saat kickstart dimulai

// Helper untuk menulis duty cycle PWM ke pin ENA
inline void writeConveyorPWM(uint8_t duty) {
    if (PIN_ENA >= 0) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcWrite(PIN_ENA, duty);
#else
        ledcWrite(PWM_CHANNEL, duty);
#endif
    }
}

// -- Inisialisasi servo, pin konveyor, pin ENA, & pin sensor --
void motionInit() {
    for (int i = 0; i < 4; i++) {
        srv[i].attach(PIN_SERVO[i]);
        curAngle[i] = HOME[i];
        tgtAngle[i] = HOME[i];
        srv[i].write(HOME[i]);
    }

    // Setup IN1 / IN2
    pinMode(PIN_IN1, OUTPUT);
    pinMode(PIN_IN2, OUTPUT);
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);

    // Setup ENA PWM via LEDC (hanya jika PIN_ENA >= 0)
    if (PIN_ENA >= 0) {
#if defined(ESP_ARDUINO_VERSION_MAJOR) && ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcAttach(PIN_ENA, PWM_FREQ, PWM_BITS);
        ledcWrite(PIN_ENA, 0); // mulai mati
#else
        ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_BITS);
        ledcAttachPin(PIN_ENA, PWM_CHANNEL);
        ledcWrite(PWM_CHANNEL, 0); // mulai mati
#endif
    }

    // Sensor Flying Fish: Pin 34 adalah pin Input-Only (bebas dari strapping flash)
    // Modul sensor Flying Fish sudah memiliki pull-up resistor sendiri di board-nya
    pinMode(PIN_SENSOR, INPUT);
}

// -- Baca sensor: true = objek terdeteksi ---------------------
bool sensorDetected() {
    return (digitalRead(PIN_SENSOR) == LOW);
}

// -- Gerakkan satu sudut servo (dengan constrain) -------------
void moveServo(uint8_t id, int angle) {
    angle = constrain(angle, ANGLE_MIN[id], ANGLE_MAX[id]);
    tgtAngle[id] = angle;
}

// -- Interpolasi non-blocking: panggil di loop() ---------------
// Mengembalikan true jika SEMUA servo sudah mencapai target
bool stepServos(bool sequential = false) {
    unsigned long now = millis();
    if (now - lastStepMs < (unsigned long)STEP_MS) return false;
    lastStepMs = now;

    bool allDone = true;
    for (int i = 0; i < 4; i++) {
        if (curAngle[i] != tgtAngle[i]) {
            if (curAngle[i] < tgtAngle[i]) curAngle[i] = min(curAngle[i] + STEP_DEG, tgtAngle[i]);
            else curAngle[i] = max(curAngle[i] - STEP_DEG, tgtAngle[i]);
            srv[i].write(curAngle[i]);
            allDone = false;
            if (sequential) break;
        }
    }
    return allDone;
}

// -- Freeze servo di posisi saat ini (untuk E-Stop) -----------
void freezeServos() {
    for (int i = 0; i < 4; i++) tgtAngle[i] = curAngle[i];
}

// -- Update Kickstart Timer (Non-Blocking) --------------------
// Dipanggil secara berkala untuk menurunkan PWM ke speed normal setelah sentakan
void updateConveyorKickstart() {
    if (isConveyorRunning && isKickstarting) {
        if (millis() - kickstartStartTime >= KICKSTART_MS) {
            isKickstarting = false;
            writeConveyorPWM(conveyorSpeed);
        }
    }
}

// -- Konveyor: nyalakan dengan sentakan awal (Kickstart) ------
void conveyorOn() {
    digitalWrite(PIN_IN1, HIGH);
    digitalWrite(PIN_IN2, LOW);

    if (!isConveyorRunning) {
        // Transisi dari MATI -> HIDUP (start / resume):
        // Berikan sentakan awal PWM maksimal (255) selama KICKSTART_MS (50ms)
        isConveyorRunning  = true;
        isKickstarting     = true;
        kickstartStartTime = millis();
        writeConveyorPWM(255);
    } else {
        // Jika sudah berjalan, periksa apakah durasi kickstart sudah habis
        updateConveyorKickstart();
    }
}

// -- Konveyor: matikan ----------------------------------------
void conveyorOff() {
    isConveyorRunning = false;
    isKickstarting    = false;
    writeConveyorPWM(0);
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
}

// -- Set speed konveyor (0-255) & langsung apply jika sedang ON
void setConveyorSpeed(uint8_t spd) {
    conveyorSpeed = spd;
    // Jika sedang ON dan BUKAN sedang kickstart, langsung update PWM
    if (isConveyorRunning && !isKickstarting) {
        writeConveyorPWM(conveyorSpeed);
    }
}

// -- Update konveyor manual berdasarkan sensor ----------------
// Panggil di loop() saat state == IDLE atau RECORDING
// Jika sensorEnabled=true dan sensor deteksi objek => berhenti
void updateManualConveyor() {
    if (!conveyorManualOn) {
        conveyorOff();
        return;
    }
    // Jika sensor aktif dan mendeteksi objek, hentikan konveyor sementara
    if (sensorEnabled && sensorDetected()) {
        conveyorOff();
    } else {
        conveyorOn();
    }
}
