// =============================================================
//  robot_arm_conveyor.ino
//  ESP32 Robot Arm + Conveyor  -  Full Firmware
//  Framework : Arduino (ESP32)
//  Libraries : ESP32Servo, WebServer.h, WiFi.h, Preferences.h
// =============================================================

#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "motion.h"
#include "storage.h"
#include "webpage.h"

// =============================================================
//  Data utama  (Frame struct didefinisikan di config.h)
// =============================================================
// Definisi aktual array sequence (extern-nya ada di config.h)
Frame   seq[NUM_SEQ][MAX_FRAMES];
uint8_t seqLen[NUM_SEQ] = {0};


enum State { BOOT_HOME, IDLE, RECORDING, PLAY_SEQ, CONVEYOR_RUN, ESTOP };
State state = BOOT_HOME;

const char* stateStr[] = {
  "BOOT_HOME","IDLE","RECORDING","PLAY_SEQ","CONVEYOR_RUN","ESTOP"
};

// =============================================================
//  Variabel playback
// =============================================================
uint8_t  playFrame    = 0;   // frame yang sedang diputar
uint8_t  playSeqIdx   = 0;   // sequence mana yang sedang diputar (0-3)
int8_t   playServoIdx = -1;  // indeks urutan servo yang sedang digerakkan (-1 = belum mulai)
bool     frameMoving  = false; // sedang interpolasi
unsigned long holdStart  = 0; // mulai hold frame
unsigned long servoDelayStart = 0; // mulai delay antar servo
bool     waitingServoDelay = false;
bool     inHold       = false;
unsigned long tConv   = 0;   // millis saat konveyor dinyalakan
bool     sensorClearNeeded = false; // flag tunggu sensor bebas sebelum deteksi objek baru

// =============================================================
//  Web Server
// =============================================================
WebServer server(80);

// ── Helper respons ──────────────────────────────────────────
void sendOK(const String& msg)   { server.send(200, "text/plain", msg); }
void sendErr(const String& msg)  { server.send(400, "text/plain", msg); }
bool isEstopBlocked() {
  if (state == ESTOP) { sendErr("E-STOP aktif, kirim /reset dulu"); return true; }
  return false;
}

// ── GET / ───────────────────────────────────────────────────
void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

// ── GET /servo?id=&angle= ───────────────────────────────────
void handleServo() {
  if (isEstopBlocked()) return;
  if (state == PLAY_SEQ || state == CONVEYOR_RUN) {
    sendErr("Sedang playback"); return;
  }
  int id    = server.arg("id").toInt();
  int angle = server.arg("angle").toInt();
  if (id < 0 || id > 3) { sendErr("id harus 0-3"); return; }
  angle = constrain(angle, ANGLE_MIN[id], ANGLE_MAX[id]);
  moveServo(id, angle);
  sendOK("ok");
}

// ── GET /save?seq=A|B|C&hold=ms ────────────────────────────────
void handleSave() {
  if (isEstopBlocked()) return;
  String seqName = server.arg("seq");
  uint8_t idx = 0;
  if (seqName.length() > 0) {
      char c = seqName.charAt(0);
      if (c >= 'A' && c < 'A' + NUM_SEQ) idx = c - 'A';
  }
  
  if (seqLen[idx] >= MAX_FRAMES) {
    sendErr("Penuh (max " + String(MAX_FRAMES) + " frame)"); return;
  }
  int hold = server.arg("hold").toInt();
  hold = constrain(hold, 0, 9999);
  
  String orderStr = server.arg("order");
  uint8_t order[4] = {0, 1, 2, 3};
  if (orderStr.length() >= 7) { // format "0,1,2,3"
      order[0] = orderStr.substring(0, 1).toInt();
      order[1] = orderStr.substring(2, 3).toInt();
      order[2] = orderStr.substring(4, 5).toInt();
      order[3] = orderStr.substring(6, 7).toInt();
  }
  
  Frame f;
  for (int i = 0; i < 4; i++) {
      f.angle[i] = (uint8_t)curAngle[i];
      f.order[i] = order[i];
  }
  f.holdMs = (uint16_t)hold;
  seq[idx][seqLen[idx]++] = f;
  saveSeq(idx);
  sendOK("Frame " + String(seqLen[idx]) + " tersimpan ke Seq " + seqName);
}

// ── GET /deleteframe?seq=A|B|C&frame=idx ─────────────────────────
void handleDeleteFrame() {
  if (isEstopBlocked()) return;
  String seqName = server.arg("seq");
  uint8_t idx = 0;
  if (seqName.length() > 0) {
      char c = seqName.charAt(0);
      if (c >= 'A' && c < 'A' + NUM_SEQ) idx = c - 'A';
  }
  if (!server.hasArg("frame")) {
    sendErr("Parameter frame diperlukan"); return;
  }
  int fIdx = server.arg("frame").toInt();
  if (fIdx < 0 || fIdx >= seqLen[idx]) {
    sendErr("Indeks frame tidak valid"); return;
  }
  for (int i = fIdx; i < seqLen[idx] - 1; i++) {
    seq[idx][i] = seq[idx][i + 1];
  }
  seqLen[idx]--;
  saveSeq(idx);
  sendOK("Frame " + String(fIdx + 1) + " dihapus dari Seq " + seqName);
}

// ── GET /undo?seq= ───────────────────────────────────────────
void handleUndo() {
  if (isEstopBlocked()) return;
  String seqName = server.arg("seq");
  uint8_t idx = 0;
  if (seqName.length() > 0) {
      char c = seqName.charAt(0);
      if (c >= 'A' && c < 'A' + NUM_SEQ) idx = c - 'A';
  }
  
  if (seqLen[idx] == 0) { sendErr("Sequence kosong"); return; }
  seqLen[idx]--;
  saveSeq(idx);
  sendOK("Undo OK, sisa " + String(seqLen[idx]) + " frame");
}

// ── GET /clear?seq= ──────────────────────────────────────────
void handleClear() {
  if (isEstopBlocked()) return;
  String seqName = server.arg("seq");
  uint8_t idx = 0;
  if (seqName.length() > 0) {
      char c = seqName.charAt(0);
      if (c >= 'A' && c < 'A' + NUM_SEQ) idx = c - 'A';
  }
  
  seqLen[idx] = 0;
  saveSeq(idx);
  sendOK("Sequence " + seqName + " dihapus");
}

// ── GET /clearAll ────────────────────────────────────────────
void handleClearAll() {
  if (isEstopBlocked()) return;
  for (uint8_t i = 0; i < NUM_SEQ; i++) {
    seqLen[i] = 0;
    saveSeq(i);
  }
  sendOK("Semua Sequence dihapus");
}

// ── GET /play ────────────────────────────────────────────────
void handlePlay() {
  if (isEstopBlocked()) return;
  bool hasData = false;
  for (int i = 0; i < NUM_SEQ; i++) {
      if (seqLen[i] > 0) hasData = true;
  }
  if (!hasData) {
    sendErr("Semua Sequence kosong! Rekam dulu."); return;
  }
  if (state == PLAY_SEQ || state == CONVEYOR_RUN) {
    sendErr("Sudah berjalan"); return;
  }
  
  // Cari sequence pertama yang tidak kosong
  playSeqIdx = 0;
  while(playSeqIdx < NUM_SEQ && seqLen[playSeqIdx] == 0) {
      playSeqIdx++;
  }
  if (playSeqIdx >= NUM_SEQ) return;
  
  playFrame   = 0;
  frameMoving = true;
  playServoIdx = -1;
  waitingServoDelay = true;
  servoDelayStart = millis() - 500;
  inHold      = false;

  // Logika Otomatis:
  // 1. Jika sensor mendeteksi benda saat start: konveyor stop, arm langsung jalan
  // 2. Jika belum ada benda: konveyor jalan mencari benda
  if (sensorEnabled && sensorDetected()) {
    conveyorOff();
    state = PLAY_SEQ;
    sendOK("Objek terdeteksi, arm servo mulai berjalan");
  } else {
    sensorClearNeeded = false;
    conveyorOn();
    state = CONVEYOR_RUN;
    sendOK("Konveyor berjalan mencari objek...");
  }
}

// ── GET /stop ─────────────────────────────────────────────────
void handleStop() {
  conveyorManualOn = false;
  conveyorOff();
  state = IDLE;
  sendOK("Stop OK");
}

// ── GET /estop ───────────────────────────────────────────────
void handleEstop() {
  conveyorManualOn = false;
  conveyorOff();
  freezeServos();
  state = ESTOP;
  sendOK("E-STOP aktif");
}

// ── GET /reset ───────────────────────────────────────────────
void handleReset() {
  state = IDLE;
  sendOK("Reset OK");
}

// ── GET /conveyor?state=0|1 ──────────────────────────────────
//    Toggle manual konveyor dari gamepad
void handleConveyor() {
  if (isEstopBlocked()) return;
  if (state == PLAY_SEQ || state == CONVEYOR_RUN) {
    sendErr("Sedang mode play/auto"); return;
  }
  
  int st = server.arg("state").toInt();
  conveyorManualOn = (st == 1);
  if (conveyorManualOn) {
    // Langsung nyalakan konveyor (jika sensor tidak memblokir)
    if (!(sensorEnabled && sensorDetected())) {
      conveyorOn();
    }
    sendOK("Konveyor ON");
  } else {
    conveyorOff();
    sendOK("Konveyor OFF");
  }
}

// ── GET /conveyorspeed?val=0-255 ─────────────────────────────
//    Atur kecepatan konveyor (butuh pin ENA terhubung ke ESP32)
void handleConveyorSpeed() {
  if (isEstopBlocked()) return;
  String valStr = server.arg("val");
  if (valStr.length() == 0) {
    // Query saja
    sendOK(String(conveyorSpeed));
    return;
  }
  int v = constrain(valStr.toInt(), 0, 255);
  setConveyorSpeed((uint8_t)v);
  sendOK("Speed: " + String(v));
}

// ── GET /sensor?enable=0|1 ───────────────────────────────────
//    Toggle apakah sensor Flying Fish aktif atau tidak
void handleSensor() {
  if (isEstopBlocked()) return;
  String enStr = server.arg("enable");
  if (enStr.length() > 0) {
    sensorEnabled = (enStr.toInt() == 1);
    sendOK(sensorEnabled ? "Sensor aktif" : "Sensor nonaktif");
  } else {
    // GET tanpa parameter => query status
    sendOK(sensorEnabled ? "1" : "0");
  }
}

// ── GET /status ──────────────────────────────────────────────
void handleStatus() {
  // Buat JSON manual (hindari library tambahan)
  String json = "{";
  
  String currentStatus = stateStr[state];
  if (state == PLAY_SEQ) {
      currentStatus = "PLAY_";
      currentStatus += (char)('A' + playSeqIdx);
  }
  
  json += "\"state\":\"" + currentStatus + "\",";
  json += "\"conveyorOn\":" + String(conveyorManualOn ? "true" : "false") + ",";
  json += "\"conveyorSpeed\":" + String(conveyorSpeed) + ",";
  json += "\"sensorEnabled\":" + String(sensorEnabled ? "true" : "false") + ",";
  json += "\"sensorDetect\":" + String(sensorDetected() ? "true" : "false") + ",";
  for (int i = 0; i < NUM_SEQ; i++) {
      json += "\"len" + String((char)('A' + i)) + "\":" + String(seqLen[i]) + ",";
  }
  json += "\"angles\":[";
  for (int i = 0; i < 4; i++) {
    json += String(curAngle[i]);
    if (i < 3) json += ",";
  }
  json += "],";

  for (int s = 0; s < NUM_SEQ; s++) {
      json += "\"frames" + String((char)('A' + s)) + "\":[";
      for (int i = 0; i < seqLen[s]; i++) {
        json += "{\"a\":[";
        for (int j = 0; j < 4; j++) {
          json += String(seq[s][i].angle[j]);
          if (j < 3) json += ",";
        }
        json += "],\"o\":[";
        for (int j = 0; j < 4; j++) {
          json += String(seq[s][i].order[j]);
          if (j < 3) json += ",";
        }
        json += "],\"h\":" + String(seq[s][i].holdMs) + "}";
        if (i < seqLen[s]-1) json += ",";
      }
      json += "]";
      if (s < NUM_SEQ - 1) json += ",";
  }
  json += "}";

  server.send(200, "application/json", json);
}

// =============================================================
//  Helper fungsi untuk maju ke target servo berikutnya
// =============================================================
void advanceToNextServo() {
    waitingServoDelay = false;
    while (true) {
        playServoIdx++;
        if (playServoIdx >= 4) {
            // Semua servo pada frame ini sudah selesai
            frameMoving = false;
            holdStart = millis();
            inHold = (seq[playSeqIdx][playFrame].holdMs > 0);
            break;
        }
        uint8_t nextServo = seq[playSeqIdx][playFrame].order[playServoIdx];
        int target = seq[playSeqIdx][playFrame].angle[nextServo];
        if (target != curAngle[nextServo]) {
            moveServo(nextServo, target);
            break; // Menemukan servo yang perlu bergerak, hentikan loop
        }
    }
}

// =============================================================
//  runSequence() - eksekusi playback sequence non-blocking
// =============================================================
void runSequence() {
  if (frameMoving) {
    if (waitingServoDelay) {
        if (millis() - servoDelayStart >= 500) {
            advanceToNextServo();
        }
        return;
    }

    // Tunggu HANYA 1 servo sampai di target
    if (stepServos()) {
        // Karena servo lain tidak punya target yang berbeda (dijaga oleh advanceToNextServo), 
        // stepServos() akan bernilai true ketika servo ke-N sudah selesai bergerak.
        servoDelayStart = millis();
        waitingServoDelay = true;
    }
    return;
  }

  if (inHold) {
    if (millis() - holdStart >= seq[playSeqIdx][playFrame].holdMs) {
      inHold = false;
    }
    return;
  }

  // Maju ke frame berikutnya
  playFrame++;
  if (playFrame >= seqLen[playSeqIdx]) {
    // Sequence selesai
    playFrame   = 0;
    frameMoving = true;
    inHold      = false;

    // Tentukan sequence berikutnya yang tidak kosong
    uint8_t nextSeq = playSeqIdx + 1;
    while(nextSeq < NUM_SEQ && seqLen[nextSeq] == 0) {
        nextSeq++;
    }

    // Jika sudah di sequence terakhir, loop kembali ke sequence pertama yang ada isinya
    if (nextSeq >= NUM_SEQ) {
      nextSeq = 0;
      while(nextSeq < NUM_SEQ && seqLen[nextSeq] == 0) {
          nextSeq++;
      }
    }

    // 4. Konveyor akan berjalan jika arm servo sudah menjalankan frame pada sequence
    playSeqIdx = nextSeq;
    conveyorOn();
    sensorClearNeeded = (sensorEnabled && sensorDetected());
    state = CONVEYOR_RUN;
    return;
  }

  // Set state untuk mulai frame berikutnya
  frameMoving = true;
  playServoIdx = -1;
  waitingServoDelay = true;
  servoDelayStart = millis() - 500; // Paksa eksekusi langsung
}

// =============================================================
//  setup()
// =============================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Robot Arm Conveyor Boot ===");

  // Init hardware
  motionInit();
  conveyorOff();

  // Muat sequence dari flash
  loadAll();
  Serial.printf("Loaded seqA=%d, seqB=%d, seqC=%d frames\n", seqLen[0], seqLen[1], seqLen[2]);

  // WiFi Access Point
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Register routes
  server.on("/",         handleRoot);
  server.on("/servo",    handleServo);
  server.on("/save",     handleSave);
  server.on("/undo",     handleUndo);
  server.on("/deleteframe", handleDeleteFrame);
  server.on("/clear",    handleClear);
  server.on("/clearAll", handleClearAll);
  server.on("/play",     handlePlay);
  server.on("/stop",     handleStop);
  server.on("/estop",    handleEstop);
  server.on("/reset",    handleReset);
  server.on("/status",   handleStatus);
  server.on("/conveyor",      handleConveyor);
  server.on("/conveyorspeed", handleConveyorSpeed);
  server.on("/sensor",        handleSensor);
  server.begin();
  Serial.println("Web server started");

  // Mulai BOOT_HOME: gerak ke posisi home
  state = BOOT_HOME;
  for (int i = 0; i < 4; i++) moveServo(i, HOME[i]);
  Serial.println("Moving to HOME...");
}

// =============================================================
//  loop()
// =============================================================
void loop() {
  server.handleClient();
  updateConveyorKickstart(); // Non-blocking: update timer sentakan awal kickstart motor

  switch (state) {
    case BOOT_HOME:
      if (stepServos()) {
        state = IDLE;
        Serial.println("HOME reached -> IDLE");
      }
      break;

    case PLAY_SEQ:
      // 3. Jika arm servo sedang menjalankan tugas: konveyor TIDAK AKAN BERGERAK
      // meskipun sensor tidak mendeteksi barang (misal barang diangkat)
      conveyorOff();
      runSequence();
      break;

    case CONVEYOR_RUN:
      // Konveyor berjalan membawa objek
      conveyorOn();
      if (sensorClearNeeded) {
        // Tunggu hingga objek sebelumnya lolos dari area sensor
        if (!sensorDetected()) {
          sensorClearNeeded = false;
        }
      } else {
        // 1. Sensor mendeteksi benda -> konveyor berhenti
        if (sensorEnabled && sensorDetected()) {
          conveyorOff();
          // 2. Jika sensor mendeteksi benda -> arm servo dijalankan sesuai sequence berikutnya
          state = PLAY_SEQ;
          playFrame = 0;
          frameMoving = true;
          playServoIdx = -1;
          waitingServoDelay = true;
          servoDelayStart = millis() - 500;
          inHold = false;
        }
      }
      break;

    // Saat manual/siap, izinkan pergerakan servo & update konveyor manual
    case IDLE:
    case RECORDING:
      stepServos();
      // Update konveyor manual berdasarkan tombol gamepad & status sensor
      updateManualConveyor();
      break;

    // ESTOP: tidak ada aksi aktif di loop (servo freeze)
    case ESTOP:
      break;
  }
}
