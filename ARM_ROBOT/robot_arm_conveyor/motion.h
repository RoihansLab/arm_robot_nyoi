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

// -- Inisialisasi servo & pin konveyor ------------------------
void motionInit() {
    for (int i = 0; i < 4; i++) {
        srv[i].attach(PIN_SERVO[i]);
        curAngle[i] = HOME[i];
        tgtAngle[i] = HOME[i];
        srv[i].write(HOME[i]);
    }
    pinMode(PIN_IN1, OUTPUT);
    pinMode(PIN_IN2, OUTPUT);
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
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
            if (sequential) break; // Hanya gerakkan satu servo jika mode sequential
        }
    }
    return allDone;
}

// -- Freeze servo di posisi saat ini (untuk E-Stop) -----------
void freezeServos() {
    for (int i = 0; i < 4; i++) tgtAngle[i] = curAngle[i];
}

// -- Konveyor -------------------------------------------------
void conveyorOn() {
    digitalWrite(PIN_IN1, HIGH);
    digitalWrite(PIN_IN2, LOW);
}

void conveyorOff() {
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
}
