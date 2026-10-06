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

// ── GET /save?seq=A|B|C|D&hold=ms ────────────────────────────────
void handleSave() {
  if (isEstopBlocked()) return;
  String seqName = server.arg("seq");
  uint8_t idx = 0;
  if (seqName.length() > 0) {
      char c = seqName.charAt(0);
      if (c >= 'A' && c <= 'J') idx = c - 'A';
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

// ── GET /undo?seq= ───────────────────────────────────────────
void handleUndo() {
  if (isEstopBlocked()) return;
  String seqName = server.arg("seq");
  uint8_t idx = 0;
  if (seqName.length() > 0) {
      char c = seqName.charAt(0);
      if (c >= 'A' && c <= 'J') idx = c - 'A';
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
      if (c >= 'A' && c <= 'J') idx = c - 'A';
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
  playServoIdx = -1; // akan diproses di dalam function agar langsung dicari yang beda
  waitingServoDelay = true;
  servoDelayStart = millis() - 500; // paksa agar langsung jalan saat masuk loop
  inHold      = false;
  state = PLAY_SEQ;
  sendOK("Play dimulai");
}

// ── GET /stop ─────────────────────────────────────────────────
void handleStop() {
  conveyorOff();
  state = IDLE;
  sendOK("Stop OK");
}

// ── GET /estop ───────────────────────────────────────────────
void handleEstop() {
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

// ── GET /conveyor ────────────────────────────────────────────
void handleConveyor() {
  if (isEstopBlocked()) return;
  if (state == PLAY_SEQ || state == CONVEYOR_RUN) {
    sendErr("Sedang mode play/auto"); return;
  }
  
  int st = server.arg("state").toInt();
  if (st == 1) {
    conveyorOn();
    sendOK("Konveyor ON");
  } else {
    conveyorOff();
    sendOK("Konveyor OFF");
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

    // Jalankan konveyor setiap kali satu sequence selesai
    playSeqIdx = nextSeq;
    conveyorOn();
    tConv = millis();
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
  Serial.printf("Loaded seqA=%d, seqB=%d frames\n", seqLen[0], seqLen[1]);

  // WiFi Access Point
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // Register routes
  server.on("/",       handleRoot);
  server.on("/servo",  handleServo);
  server.on("/save",   handleSave);
  server.on("/undo",   handleUndo);
  server.on("/clear",  handleClear);
  server.on("/clearAll", handleClearAll);
  server.on("/play",   handlePlay);
  server.on("/stop",   handleStop);
  server.on("/estop",  handleEstop);
  server.on("/reset",  handleReset);
  server.on("/status", handleStatus);
  server.on("/conveyor", handleConveyor);
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

  switch (state) {
    case BOOT_HOME:
      if (stepServos()) {
        state = IDLE;
        Serial.println("HOME reached -> IDLE");
      }
      break;

    case PLAY_SEQ:
      runSequence();
      break;

    case CONVEYOR_RUN:
      if (millis() - tConv >= CONVEYOR_MS) {
        conveyorOff();
        // Langsung pindah ke PLAY_SEQ berikutnya yang sudah diset di runSequence
        state = PLAY_SEQ;
        frameMoving = true;
        playServoIdx = -1;
        waitingServoDelay = true;
        servoDelayStart = millis() - 500;
      }
      break;

    // Saat manual/siap, izinkan pergerakan servo
    case IDLE:
    case RECORDING:
      stepServos();
      break;

    // ESTOP: tidak ada aksi aktif di loop (servo freeze)
    case ESTOP:
      break;
  }
}
