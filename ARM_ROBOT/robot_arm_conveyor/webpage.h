#pragma once

// =============================================================
//  webpage.h  -  DualShock Gamepad UI  (PROGMEM)
// =============================================================
const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>Robot Arm Gamepad</title>
<style>
*,*::before,*::after{box-sizing:border-box;margin:0;padding:0;-webkit-tap-highlight-color:transparent;}
html,body{height:100%;background:#07070f;color:#e0e0e0;font-family:'Segoe UI',system-ui,sans-serif;overflow-x:hidden;}
body{display:flex;flex-direction:column;align-items:center;padding:8px 6px 80px;min-height:100vh;}

/* STATUS */
.status-bar{width:100%;max-width:480px;background:rgba(255,255,255,0.03);border:1px solid rgba(255,255,255,0.06);border-radius:10px;padding:6px 14px;font-size:.68rem;color:#555;margin-bottom:8px;text-align:center;line-height:1.7;}
.s-state{color:#6c63ff;font-weight:700;}
.s-seq{color:#f39c12;display:block;}

/* TOAST */
.toast{position:fixed;top:12px;left:50%;transform:translateX(-50%) translateY(-20px);background:rgba(10,10,22,.97);border:1px solid #6c63ff;border-radius:10px;padding:9px 20px;font-size:.75rem;color:#e0e0e0;z-index:999;opacity:0;transition:all .25s;pointer-events:none;white-space:nowrap;}
.toast.show{opacity:1;transform:translateX(-50%) translateY(0);}
.toast.err{border-color:#e74c3c;color:#e74c3c;}

/* MODE SWITCH */
.mode-wrap{display:flex;align-items:center;gap:8px;margin-bottom:10px;flex-wrap:wrap;justify-content:center;}
.mode-label{font-size:.62rem;color:#444;text-transform:uppercase;letter-spacing:1px;}
.mode-toggle{display:flex;background:rgba(0,0,0,0.5);border:1px solid rgba(255,255,255,0.08);border-radius:20px;overflow:hidden;}
.mode-btn{padding:6px 16px;font-size:.7rem;font-weight:700;border:none;background:transparent;color:#444;cursor:pointer;transition:all .2s;letter-spacing:.5px;text-transform:uppercase;}
.mode-btn.active{background:#6c63ff;color:#fff;box-shadow:0 0 12px rgba(108,99,255,.5);}

/* CONVEYOR CONTROLS ROW */
.conv-row{display:flex;align-items:center;gap:8px;width:100%;max-width:480px;margin-bottom:8px;background:rgba(255,255,255,0.03);border:1px solid rgba(255,255,255,0.06);border-radius:10px;padding:7px 12px;}
.conv-label{font-size:.62rem;color:#555;text-transform:uppercase;letter-spacing:.5px;flex-shrink:0;}
.conv-btn{padding:6px 14px;font-size:.7rem;font-weight:700;border:none;border-radius:8px;cursor:pointer;transition:all .2s;letter-spacing:.5px;text-transform:uppercase;}
.conv-btn-on{background:#1a4a3a;color:#2ea88a;border:1px solid rgba(46,168,138,.3);}
.conv-btn-on.active{background:#2ea88a;color:#fff;box-shadow:0 0 12px rgba(46,168,138,.5);}
.conv-btn-off{background:#3a1a1a;color:#e74c3c;border:1px solid rgba(231,76,60,.3);}
.conv-btn-off.active{background:#e74c3c;color:#fff;box-shadow:0 0 10px rgba(231,76,60,.4);}
.sensor-wrap{display:flex;align-items:center;gap:6px;margin-left:auto;}
.sensor-dot{width:9px;height:9px;border-radius:50%;background:#333;border:1px solid #555;transition:background .2s;}
.sensor-dot.detect{background:#e74c3c;box-shadow:0 0 6px rgba(231,76,60,.7);}
.sensor-dot.clear{background:#2ea88a;box-shadow:0 0 6px rgba(46,168,138,.5);}
.sensor-lbl{font-size:.6rem;color:#444;}
.sensor-toggle{font-size:.6rem;color:#555;cursor:pointer;border:1px solid rgba(255,255,255,.08);border-radius:5px;background:rgba(0,0,0,.4);padding:3px 7px;white-space:nowrap;}
.sensor-toggle.enabled{color:#6c63ff;border-color:rgba(108,99,255,.3);}

/* GAMEPAD BODY */
.gamepad{width:100%;max-width:480px;position:relative;background:linear-gradient(165deg,#1c1c2e 0%,#141422 60%,#0e0e1c 100%);border-radius:36px 36px 50px 50px;border:1px solid rgba(255,255,255,0.06);box-shadow:0 25px 70px rgba(0,0,0,.9),inset 0 1px 0 rgba(255,255,255,.05),0 0 0 1px rgba(108,99,255,.08);}
.grip-l,.grip-r{position:absolute;bottom:-22px;width:36%;height:50px;background:linear-gradient(165deg,#1a1a2c,#0e0e1c);border:1px solid rgba(255,255,255,0.04);border-top:none;box-shadow:0 10px 25px rgba(0,0,0,.7);}
.grip-l{left:14px;border-radius:0 0 28px 28px;}
.grip-r{right:14px;border-radius:0 0 28px 28px;}

/* BUMPERS */
.bumper-row{display:flex;justify-content:space-between;align-items:flex-end;padding:0 8px;}
.bgrp{display:flex;flex-direction:column;}
.bgrp-l{align-items:flex-start;}
.bgrp-r{align-items:flex-end;}
.bumper{border:none;cursor:pointer;font-weight:700;letter-spacing:.5px;text-transform:uppercase;transition:all .12s;}
.bL2,.bR2{width:68px;height:14px;background:linear-gradient(to bottom,#202032,#181828);color:#2a2a3a;font-size:.55rem;border-radius:8px 8px 0 0;border:1px solid rgba(255,255,255,.04);border-bottom:none;}
.bL1,.bR1{width:68px;height:24px;background:linear-gradient(to bottom,#252540,#1c1c34);color:#555;font-size:.62rem;border:1px solid rgba(255,255,255,.07);border-bottom:none;border-radius:4px 4px 0 0;display:flex;align-items:center;justify-content:center;}
.bL1.pressed,.bR1.pressed{background:linear-gradient(to bottom,#6c63ff,#5249d0);color:#fff;box-shadow:0 0 12px rgba(108,99,255,.5);}
.bL1:active,.bR1:active{transform:scaleY(.88);}

/* SEQ TABS in center bumper area (3 sequences: A, B, C) */
.seq-area{display:flex;flex-direction:column;align-items:center;gap:3px;padding-bottom:2px;}
.seq-lbl{font-size:.52rem;color:#333;text-transform:uppercase;letter-spacing:.5px;}
.seq-tabs{display:flex;gap:6px;}
.seq-tab{width:42px;height:24px;border:1px solid rgba(255,255,255,.08);border-radius:6px;background:rgba(0,0,0,.4);color:#666;font-size:.75rem;font-weight:700;cursor:pointer;transition:all .15s;}
.seq-tab.active{background:#6c63ff;color:#fff;border-color:#6c63ff;box-shadow:0 0 10px rgba(108,99,255,.5);}

/* INNER PANEL */
.inner{background:linear-gradient(to bottom,#161624,#101018);border-radius:0 0 30px 30px;border:1px solid rgba(255,255,255,.04);border-top:1.5px solid rgba(255,255,255,.03);padding:14px 10px 16px;}
.ctrl-row{display:flex;justify-content:space-between;align-items:center;gap:4px;}

/* D-PAD */
.dpad{display:grid;grid-template-columns:1fr 1fr 1fr;grid-template-rows:1fr 1fr 1fr;gap:4px;width:110px;height:110px;justify-items:center;align-items:center;flex-shrink:0;}
.dpad-btn{width:36px;height:36px;background:rgba(255,255,255,0.05);border:1px solid rgba(255,255,255,0.1);border-radius:8px;color:#fff;font-size:1.2rem;cursor:pointer;transition:0.1s;display:flex;align-items:center;justify-content:center;}
.dpad-btn.held{background:rgba(108,99,255,0.6);border-color:rgba(108,99,255,0.8);box-shadow:0 0 10px rgba(108,99,255,0.5);}
.dpad-up{grid-column:2;grid-row:1;}
.dpad-left{grid-column:1;grid-row:2;}
.dpad-right{grid-column:3;grid-row:2;}
.dpad-down{grid-column:2;grid-row:3;}
.stick-lbl{text-align:center;margin-top:5px;font-size:.55rem;color:#333;line-height:1.6;}

/* CENTER CONTROLS */
.center{display:flex;flex-direction:column;align-items:center;gap:8px;flex:1;}
.meta-row{display:flex;align-items:center;gap:6px;}
.meta-btn{width:38px;height:18px;border:1px solid rgba(255,255,255,.08);border-radius:4px;background:linear-gradient(to bottom,#232335,#191927);color:#555;font-size:.52rem;font-weight:700;letter-spacing:.5px;text-transform:uppercase;cursor:pointer;transition:all .15s;}
.meta-btn:active{transform:scale(.9);}
.meta-btn.hl{background:#e74c3c;color:#fff;border-color:#e74c3c;box-shadow:0 0 10px rgba(231,76,60,.4);}
.guide-btn{width:46px;height:46px;border-radius:50%;background:radial-gradient(circle at 38% 32%,#5a52e0,#3330a0);border:2px solid rgba(108,99,255,.45);color:#fff;font-size:.85rem;cursor:pointer;display:flex;align-items:center;justify-content:center;box-shadow:0 4px 16px rgba(108,99,255,.4),inset 0 1px 0 rgba(255,255,255,.2);transition:all .15s;}
.guide-btn:hover{box-shadow:0 4px 22px rgba(108,99,255,.65),inset 0 1px 0 rgba(255,255,255,.2);}
.guide-btn:active{transform:scale(.9);}
.guide-lbl{font-size:.52rem;color:#444;text-align:center;margin-top:1px;}
.hold-row{display:flex;align-items:center;gap:5px;font-size:.58rem;color:#393950;}
.hold-row input{width:52px;background:rgba(0,0,0,.5);border:1px solid rgba(255,255,255,.06);border-radius:4px;color:#666;font-size:.62rem;padding:3px 6px;text-align:center;}
.hold-row input:focus{outline:none;border-color:#6c63ff;color:#ccc;}

/* FACE BUTTONS */
.face-pad{display:flex;flex-direction:column;align-items:center;gap:5px;flex-shrink:0;}
.face-row{display:flex;gap:5px;}
.face-btn{width:46px;height:46px;border-radius:50%;font-size:1.1rem;font-weight:700;cursor:pointer;display:flex;align-items:center;justify-content:center;border:2px solid rgba(255,255,255,.08);box-shadow:0 4px 12px rgba(0,0,0,.6);transition:all .1s;position:relative;}
.face-btn::after{content:'';position:absolute;inset:0;border-radius:50%;background:rgba(255,255,255,.06);opacity:0;transition:opacity .1s;}
.face-btn:active::after,.face-btn.held::after{opacity:1;}
.face-btn.held{filter:brightness(1.4);}
.f-tri{background:radial-gradient(circle at 38% 30%,#2ea88a,#1a7060);border-color:rgba(46,168,138,.35);color:#80ffdf;--gc:rgba(46,168,138,.6);}
.f-tri.held{box-shadow:0 0 20px var(--gc);}
.f-circ{background:radial-gradient(circle at 38% 30%,#c0392b,#8b1a1a);border-color:rgba(192,57,43,.35);color:#ff8888;--gc:rgba(192,57,43,.6);}
.f-circ.held{box-shadow:0 0 20px var(--gc);}
.f-sq{background:radial-gradient(circle at 38% 30%,#8e44ad,#5b2c6f);border-color:rgba(142,68,173,.35);color:#dda0ff;--gc:rgba(142,68,173,.6);}
.f-sq.held{box-shadow:0 0 20px var(--gc);}
.f-cross{background:radial-gradient(circle at 38% 30%,#2980b9,#1a5f8a);border-color:rgba(41,128,185,.35);color:#88ccff;--gc:rgba(41,128,185,.6);}
.f-cross.held{box-shadow:0 0 20px var(--gc);}

/* BOTTOM */
.btm-row{padding:10px 16px 0;display:flex;justify-content:center;}
.del-btn{background:linear-gradient(to right,rgba(231,76,60,.12),rgba(231,76,60,.04));border:1px solid rgba(231,76,60,.2);color:#e74c3c;border-radius:8px;padding:7px 18px;font-size:.65rem;font-weight:700;letter-spacing:.8px;text-transform:uppercase;cursor:pointer;transition:all .15s;}
.del-btn:hover{background:rgba(231,76,60,.2);border-color:rgba(231,76,60,.4);}
.del-btn:active{transform:scale(.96);}

/* ANGLE DISPLAY */
.ang-display{display:flex;gap:12px;flex-wrap:wrap;justify-content:center;margin-top:48px;max-width:480px;}
.ang-item{display:flex;flex-direction:column;align-items:center;gap:1px;}
.ang-lbl{font-size:.52rem;color:#333;text-transform:uppercase;letter-spacing:.5px;}
.ang-val{color:#6c63ff;font-size:.95rem;font-weight:700;font-variant-numeric:tabular-nums;min-width:36px;text-align:center;}

/* DISABLED overlay */
.ctrl-disabled{opacity:.2;pointer-events:none;}

/* FRAME LIST & SELECTION */
.frame-panel{width:100%;max-width:480px;margin-top:10px;background:rgba(255,255,255,.025);border:1px solid rgba(255,255,255,.05);border-radius:10px;padding:10px 12px;}
.frame-panel-hdr{display:flex;justify-content:space-between;align-items:center;margin-bottom:8px;}
.fp-title{font-size:.65rem;color:#777;text-transform:uppercase;letter-spacing:.5px;font-weight:700;}
.fp-count{font-size:.65rem;color:#6c63ff;font-weight:700;}

.frame-actions-bar{display:flex;align-items:center;justify-content:space-between;background:rgba(0,0,0,0.3);border:1px solid rgba(255,255,255,0.05);border-radius:8px;padding:6px 10px;margin-bottom:8px;font-size:.65rem;}
.sel-info{color:#aaa;}
.sel-info b{color:#80ffdf;}
.btn-del-sel{background:#3a1a1a;color:#e74c3c;border:1px solid rgba(231,76,60,0.4);border-radius:6px;padding:3px 10px;font-size:.62rem;font-weight:700;cursor:pointer;transition:all .15s;}
.btn-del-sel:hover:not(:disabled){background:#e74c3c;color:#fff;}
.btn-del-sel:disabled{opacity:.3;cursor:not-allowed;}

.frame-list{font-size:.62rem;color:#777;max-height:130px;overflow-y:auto;display:flex;flex-direction:column;gap:3px;}
.fl-item{display:flex;align-items:center;justify-content:space-between;padding:5px 8px;border-radius:6px;background:rgba(255,255,255,0.02);border:1px solid transparent;cursor:pointer;transition:all .15s;}
.fl-item:hover{background:rgba(255,255,255,0.05);border-color:rgba(255,255,255,0.08);}
.fl-item.selected{background:rgba(108,99,255,0.18);border-color:#6c63ff;color:#fff;box-shadow:0 0 8px rgba(108,99,255,0.3);}
.fl-data{display:flex;align-items:center;gap:8px;}
.fl-num{font-weight:700;color:#6c63ff;min-width:22px;}
.fl-item.selected .fl-num{color:#80ffdf;}
.fl-btn-del{background:transparent;border:none;color:#555;font-size:.75rem;cursor:pointer;padding:2px 6px;border-radius:4px;transition:all .15s;}
.fl-btn-del:hover{color:#e74c3c;background:rgba(231,76,60,0.15);}

@media(max-width:360px){
  .dpad{width:96px;height:96px;}
  .dpad-btn{width:30px;height:30px;}
  .face-btn{width:40px;height:40px;font-size:.95rem;}
  .bL1,.bR1,.bL2,.bR2{width:58px;}
}
</style>
</head>
<body><div class="toast" id="toast"></div>

<div class="status-bar" id="statusBar">Menghubungkan ke ESP32...</div>

<div class="mode-wrap">
  <span class="mode-label">Mode:</span>
  <div class="mode-toggle">
    <button class="mode-btn active" id="btnManual" onclick="setMode('manual')">Manual</button>
    <button class="mode-btn" id="btnAuto" onclick="setMode('auto')">Otomatis</button>
  </div>
</div>

<!-- CONVEYOR CONTROL ROW -->
<div class="conv-row">
  <span class="conv-label">Konveyor:</span>
  <button class="conv-btn conv-btn-on" id="btnConvOn" onclick="setConveyor(1)">&#9654; ON</button>
  <button class="conv-btn conv-btn-off active" id="btnConvOff" onclick="setConveyor(0)">&#9646;&#9646; OFF</button>
  <div class="sensor-wrap">
    <div class="sensor-dot" id="sensorDot" title="Status Sensor Flying Fish"></div>
    <span class="sensor-lbl" id="sensorLbl">Sensor</span>
    <button class="sensor-toggle enabled" id="btnSensorToggle" onclick="toggleSensor()" title="Toggle sensor ON/OFF">&#9673; Aktif</button>
  </div>
</div>

<!-- CONVEYOR SPEED DISPLAY (STATIS 60 PWM) -->
<div class="conv-row" id="speedRow" style="justify-content:space-between;align-items:center;">
  <div style="display:flex;align-items:center;gap:8px;">
    <span class="conv-label">Speed Motor:</span>
    <span style="font-size:.75rem;color:#2ea88a;font-weight:700;background:rgba(46,168,138,.12);border:1px solid rgba(46,168,138,.3);padding:2px 10px;border-radius:6px;">60 PWM (Statis)</span>
  </div>
  <span style="font-size:.62rem;color:#555;">Kickstart: 50 ms</span>
</div>

<div class="gamepad" id="gamepad">
  <div class="grip-l"></div>
  <div class="grip-r"></div>

  <!-- BUMPER ROW -->
  <div class="bumper-row">
    <div class="bgrp bgrp-l">
      <button class="bumper bL2">L2</button>
      <button class="bumper bL1" id="btnL1">L1 Undo</button>
    </div>
    <div class="seq-area">
      <span class="seq-lbl">Sequence</span>
      <div class="seq-tabs" id="seqTabs">
        <button class="seq-tab active" id="tabA" onclick="selSeq('A')">A</button>
        <button class="seq-tab" id="tabB" onclick="selSeq('B')">B</button>
        <button class="seq-tab" id="tabC" onclick="selSeq('C')">C</button>
      </div>
    </div>
    <div class="bgrp bgrp-r">
      <button class="bumper bR2">R2</button>
      <button class="bumper bR1" id="btnR1">R1 Redo</button>
    </div>
  </div>

  <!-- INNER PANEL -->
  <div class="inner">
    <div class="ctrl-row">

      <!-- LEFT: D-Pad -->
      <div id="dpadWrapper">
        <div class="dpad" id="dpadZone">
          <button class="dpad-btn dpad-up" id="btnUp">&#9650;</button>
          <button class="dpad-btn dpad-left" id="btnLeft">&#9664;</button>
          <button class="dpad-btn dpad-right" id="btnRight">&#9654;</button>
          <button class="dpad-btn dpad-down" id="btnDown">&#9660;</button>
        </div>
        <div class="stick-lbl">Base (L/R) &bull; Shoulder (U/D)</div>
      </div>

      <!-- CENTER -->
      <div class="center">
        <div class="meta-row">
          <button class="meta-btn hl" id="btnSelect" onclick="doStop()">SEL<br>Stop</button>
          <div style="display:flex;flex-direction:column;align-items:center;gap:3px;">
            <button class="guide-btn" id="btnGuide" onclick="doSaveFrame()">&#9711;</button>
            <div class="guide-lbl">SAVE</div>
          </div>
          <button class="meta-btn" id="btnStart" onclick="doPlay()" style="background:linear-gradient(to bottom,#1a4a1a,#122012);color:#3a8a3a;border-color:rgba(46,139,46,.2);">STR<br>Play</button>
        </div>
        <div class="hold-row">
          Hold: <input type="number" id="holdMs" value="500" min="0" max="9999"> ms
        </div>
      </div>

      <!-- RIGHT: Face Buttons -->
      <div class="face-pad" id="facePad">
        <button class="face-btn f-tri" id="btnTri">&#9651;</button>
        <div class="face-row">
          <button class="face-btn f-sq" id="btnSq">&#9633;</button>
          <button class="face-btn f-circ" id="btnCirc">&#9711;</button>
        </div>
        <button class="face-btn f-cross" id="btnCross">&#10005;</button>
      </div>

    </div><!-- end ctrl-row -->

    <!-- DELETE CUR SEQ & RESET -->
    <div class="btm-row" style="gap:10px;">
      <button class="del-btn" id="btnDel" onclick="doDeleteCurSeq()">&#128465; Hapus Seq</button>
      <button class="del-btn" id="btnDelAll" onclick="doDeleteAll()" style="color:#f39c12;border-color:rgba(243,156,18,0.3);background:rgba(243,156,18,0.1);">Reset Semua (A-C)</button>
    </div>

  </div><!-- end inner -->
</div><!-- end gamepad -->

<!-- ANGLE DISPLAY -->
<div class="ang-display">
  <div class="ang-item"><span class="ang-lbl">Base</span><span class="ang-val" id="angB">90</span></div>
  <div class="ang-item"><span class="ang-lbl">Shoulder</span><span class="ang-val" id="angS">90</span></div>
  <div class="ang-item"><span class="ang-lbl">Elbow</span><span class="ang-val" id="angE">90</span></div>
  <div class="ang-item"><span class="ang-lbl">Gripper</span><span class="ang-val" id="angG">90</span></div>
</div>

<!-- FRAME LIST & SELECT/DELETE SPECIFIC FRAME -->
<div class="frame-panel">
  <div class="frame-panel-hdr">
    <span class="fp-title" id="fpTitle">Seq A Frames</span>
    <span class="fp-count" id="fpCount">0 frame</span>
  </div>
  <div class="frame-actions-bar">
    <span class="sel-info" id="selFrameInfo">Pilih frame di bawah untuk preview / hapus</span>
    <button class="btn-del-sel" id="btnDelSelected" onclick="doDeleteSelectedFrame()" disabled>&#128465; Hapus Frame Terpilih</button>
  </div>
  <div class="frame-list" id="frameList"><div style="color:#444">Belum ada frame</div></div>
</div>

<script>
// ============================================================
// CONFIG (mirror dari config.h)
// ============================================================
const ANGLE_MIN = [0, 20, 20, 30];
const ANGLE_MAX = [180, 160, 160, 120];

// ============================================================
// STATE
// ============================================================
let mode = 'manual';
let curSeq = 'A';
let selectedFrameIdx = -1;       // Indeks frame yang sedang dipilih
let angles = [90, 90, 90, 90];  // local tracking
let redoStack = { A:[], B:[], C:[] };
let lastFrames = { A:[], B:[], C:[] };
let heldBtns = {};               // { 'tri': true, ... }
let moveOrder = [];              // Track movement order
let convState = 0;               // 0 = off, 1 = on (mirror dari ESP32)
let sensorEnabledState = true;   // mirror dari ESP32

const stateNames = {
  BOOT_HOME:'Boot - Menuju Home', IDLE:'Siap', RECORDING:'Recording',
  PLAY_A:'Playing Seq A', PLAY_B:'Playing Seq B', PLAY_C:'Playing Seq C',
  CONVEYOR_RUN:'Konveyor Berjalan (Menunggu Objek)', ESTOP:'!!! E-STOP !!!'
};
let serverState = 'IDLE';

// ============================================================
// TOAST
// ============================================================
let toastTimer = null;
function showToast(txt, isErr) {
  const el = document.getElementById('toast');
  el.textContent = txt;
  el.className = 'toast show' + (isErr ? ' err' : '');
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => el.classList.remove('show'), 2500);
}

// ============================================================
// MODE
// ============================================================
function setMode(m) {
  mode = m;
  document.getElementById('btnManual').classList.toggle('active', m === 'manual');
  document.getElementById('btnAuto').classList.toggle('active', m === 'auto');
  applyModeUI();
  if (m === 'auto') {
    if (serverState === 'IDLE') {
      doPlay();
    }
  } else {
    if (serverState === 'PLAY_SEQ' || serverState === 'CONVEYOR_RUN' || serverState.startsWith('PLAY_')) {
      doStop();
    }
  }
}

function applyModeUI() {
  const isManual = mode === 'manual';
  document.getElementById('dpadWrapper').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('facePad').classList.toggle('ctrl-disabled', !isManual);
  
  document.getElementById('btnL1').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnR1').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnGuide').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnDel').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnDelAll').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnDelSelected').classList.toggle('ctrl-disabled', !isManual);

  document.getElementById('btnConvOn').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnConvOff').classList.toggle('ctrl-disabled', !isManual);
}

// ============================================================
// SEQUENCE TAB (A, B, C)
// ============================================================
function selSeq(s) {
  curSeq = s;
  selectedFrameIdx = -1; // reset pilihan saat ganti sequence
  const seqs = ['A','B','C'];
  seqs.forEach(sq => {
      let el = document.getElementById('tab' + sq);
      if (el) el.classList.toggle('active', sq === s);
  });
  document.getElementById('fpTitle').textContent = 'Seq ' + s + ' Frames';
  renderFrames();
}

// ============================================================
// SERVO API
// ============================================================
let pendingSend = {};
async function sendServo(id, angle) {
  angle = Math.round(clamp(angle, ANGLE_MIN[id], ANGLE_MAX[id]));
  if (pendingSend[id] === angle) return;
  pendingSend[id] = angle;
  try { await fetch('/servo?id=' + id + '&angle=' + angle); } catch(e) {}
}

function clamp(v, mn, mx) { return Math.min(Math.max(v, mn), mx); }

// ============================================================
// CONTROL LOOP (50ms)
// ============================================================
const SPEED = 2.2; // deg/tick

setInterval(() => {
  if (mode !== 'manual') return;
  if (serverState === 'ESTOP') return;

  if (heldBtns['left']) {
    if (!moveOrder.includes(0)) moveOrder.push(0);
    angles[0] = clamp(angles[0] + SPEED, ANGLE_MIN[0], ANGLE_MAX[0]);
    sendServo(0, angles[0]);
  }
  if (heldBtns['right']) {
    if (!moveOrder.includes(0)) moveOrder.push(0);
    angles[0] = clamp(angles[0] - SPEED, ANGLE_MIN[0], ANGLE_MAX[0]);
    sendServo(0, angles[0]);
  }
  if (heldBtns['up']) {
    if (!moveOrder.includes(1)) moveOrder.push(1);
    angles[1] = clamp(angles[1] - SPEED, ANGLE_MIN[1], ANGLE_MAX[1]);
    sendServo(1, angles[1]);
  }
  if (heldBtns['down']) {
    if (!moveOrder.includes(1)) moveOrder.push(1);
    angles[1] = clamp(angles[1] + SPEED, ANGLE_MIN[1], ANGLE_MAX[1]);
    sendServo(1, angles[1]);
  }
  if (heldBtns['tri']) {
    if (!moveOrder.includes(2)) moveOrder.push(2);
    angles[2] = clamp(angles[2] + SPEED, ANGLE_MIN[2], ANGLE_MAX[2]);
    sendServo(2, angles[2]);
  }
  if (heldBtns['cross']) {
    if (!moveOrder.includes(2)) moveOrder.push(2);
    angles[2] = clamp(angles[2] - SPEED, ANGLE_MIN[2], ANGLE_MAX[2]);
    sendServo(2, angles[2]);
  }

  // Gripper: Square = tutup, Circle = buka
  if (heldBtns['sq']) {
    if (!moveOrder.includes(3)) moveOrder.push(3);
    angles[3] = clamp(angles[3] - SPEED, ANGLE_MIN[3], ANGLE_MAX[3]);
    sendServo(3, angles[3]);
  }
  if (heldBtns['circ']) {
    if (!moveOrder.includes(3)) moveOrder.push(3);
    angles[3] = clamp(angles[3] + SPEED, ANGLE_MIN[3], ANGLE_MAX[3]);
    sendServo(3, angles[3]);
  }

  updateAngleDisplay();
}, 50);

function updateAngleDisplay() {
  document.getElementById('angB').textContent = Math.round(angles[0]);
  document.getElementById('angS').textContent = Math.round(angles[1]);
  document.getElementById('angE').textContent = Math.round(angles[2]);
  document.getElementById('angG').textContent = Math.round(angles[3]);
}

// ============================================================
// FACE BUTTONS & D-PAD
// ============================================================
function setupHoldBtn(el, key, onPress) {
  const press = e => { e.preventDefault(); heldBtns[key] = true; el.classList.add('held'); if(onPress) onPress(); };
  const release = () => { heldBtns[key] = false; el.classList.remove('held'); };
  el.addEventListener('mousedown', press);
  el.addEventListener('touchstart', press, {passive:false});
  window.addEventListener('mouseup', release);
  window.addEventListener('touchend', release);
}

// D-Pad
setupHoldBtn(document.getElementById('btnUp'), 'up');
setupHoldBtn(document.getElementById('btnDown'), 'down');
setupHoldBtn(document.getElementById('btnLeft'), 'left');
setupHoldBtn(document.getElementById('btnRight'), 'right');

// Face buttons
setupHoldBtn(document.getElementById('btnTri'), 'tri');
setupHoldBtn(document.getElementById('btnCross'), 'cross');
setupHoldBtn(document.getElementById('btnSq'), 'sq');
setupHoldBtn(document.getElementById('btnCirc'), 'circ');

// Bumpers
setupHoldBtn(document.getElementById('btnL1'), 'l1', () => doUndo());
setupHoldBtn(document.getElementById('btnR1'), 'r1', () => doRedo());

// ============================================================
// CONVEYOR CONTROLS
// ============================================================
async function setConveyor(on) {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  if (mode !== 'manual') { showToast('Mode harus Manual', true); return; }

  const r = await fetch('/conveyor?state=' + on);
  const txt = await r.text();
  if (r.ok) {
    convState = on;
    updateConveyorUI();
    showToast(txt);
  } else {
    showToast(txt, true);
  }
}

function updateConveyorUI() {
  const btnOn  = document.getElementById('btnConvOn');
  const btnOff = document.getElementById('btnConvOff');
  btnOn.classList.toggle('active', convState === 1);
  btnOff.classList.toggle('active', convState === 0);
}

// ============================================================
// SENSOR TOGGLE
// ============================================================
async function toggleSensor() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  const newEnable = sensorEnabledState ? 0 : 1;
  const r = await fetch('/sensor?enable=' + newEnable);
  if (r.ok) {
    sensorEnabledState = (newEnable === 1);
    updateSensorUI(sensorEnabledState, false);
    showToast(sensorEnabledState ? 'Sensor Flying Fish AKTIF' : 'Sensor Flying Fish NONAKTIF');
  } else {
    showToast(await r.text(), true);
  }
}

function updateSensorUI(enabled, detected) {
  const dot  = document.getElementById('sensorDot');
  const lbl  = document.getElementById('sensorLbl');
  const btn  = document.getElementById('btnSensorToggle');

  if (!enabled) {
    dot.className = 'sensor-dot';
    lbl.textContent = 'Sensor';
    btn.className   = 'sensor-toggle';
    btn.textContent = '\u25CB Nonaktif';
  } else if (detected) {
    dot.className   = 'sensor-dot detect';
    lbl.style.color = '#e74c3c';
    lbl.textContent = 'Objek!';
    btn.className   = 'sensor-toggle enabled';
    btn.textContent = '\u25D9 Aktif';
  } else {
    dot.className   = 'sensor-dot clear';
    lbl.style.color = '#2ea88a';
    lbl.textContent = 'Clear';
    btn.className   = 'sensor-toggle enabled';
    btn.textContent = '\u25D9 Aktif';
  }
}

// ============================================================
// KEYFRAME ACTIONS (Save, Undo, Redo, Select & Delete Frame)
// ============================================================
async function doSaveFrame() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  const hold = parseInt(document.getElementById('holdMs').value) || 500;
  
  let finalOrder = [...moveOrder];
  for(let i=0; i<4; i++) {
    if(!finalOrder.includes(i)) finalOrder.push(i);
  }
  
  const r = await fetch('/save?seq=' + curSeq + '&hold=' + hold + '&order=' + finalOrder.join(','));
  const t = await r.text();
  showToast(t, !r.ok);
  if (r.ok) { 
    redoStack[curSeq] = []; 
    moveOrder = []; 
    updateStatus(); 
  }
}

async function doUndo() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  const frames = lastFrames[curSeq];
  if (frames && frames.length > 0) {
    redoStack[curSeq].push(frames[frames.length - 1]);
  }
  const r = await fetch('/undo?seq=' + curSeq);
  showToast(await r.text(), !r.ok);
  if (r.ok) {
    if (selectedFrameIdx >= (frames ? frames.length - 1 : 0)) {
      selectedFrameIdx = -1;
    }
    updateStatus();
  }
}

async function doRedo() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  const stack = redoStack[curSeq];
  if (!stack || stack.length === 0) { showToast('Tidak ada redo tersedia', true); return; }
  const frame = stack.pop();
  for (let i = 0; i < 4; i++) {
    angles[i] = frame.a[i];
    await sendServo(i, frame.a[i]);
  }
  updateAngleDisplay();
  const hold = frame.h;
  const order = (frame.o && frame.o.length === 4) ? frame.o.join(',') : '0,1,2,3';
  const r = await fetch('/save?seq=' + curSeq + '&hold=' + hold + '&order=' + order);
  showToast('Redo: ' + (await r.text()), !r.ok);
  if (r.ok) updateStatus();
}

// Memilih frame tertentu dan menggerakkan robot ke posisi frame tersebut
async function selectFrame(idx) {
  if (mode !== 'manual') return;
  const frames = lastFrames[curSeq];
  if (!frames || idx < 0 || idx >= frames.length) return;
  selectedFrameIdx = idx;
  const f = frames[idx];
  for (let i = 0; i < 4; i++) {
    angles[i] = f.a[i];
    await sendServo(i, f.a[i]);
  }
  updateAngleDisplay();
  showToast('Frame #' + (idx + 1) + ' dipilih & robot digerakkan');
  renderFrames();
}

// Menghapus frame terpilih
async function doDeleteSelectedFrame() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  if (selectedFrameIdx < 0) {
    showToast('Pilih frame di daftar terlebih dahulu', true);
    return;
  }
  if (!confirm('Hapus Frame #' + (selectedFrameIdx + 1) + ' dari Seq ' + curSeq + '?')) return;
  
  const r = await fetch('/deleteframe?seq=' + curSeq + '&frame=' + selectedFrameIdx);
  const txt = await r.text();
  showToast(txt, !r.ok);
  if (r.ok) {
    selectedFrameIdx = -1;
    redoStack[curSeq] = [];
    updateStatus();
  }
}

// Menghapus frame tertentu via tombol silang (✕)
async function doDeleteSpecificFrame(idx, e) {
  if (e) e.stopPropagation();
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  if (!confirm('Hapus Frame #' + (idx + 1) + ' dari Seq ' + curSeq + '?')) return;

  const r = await fetch('/deleteframe?seq=' + curSeq + '&frame=' + idx);
  const txt = await r.text();
  showToast(txt, !r.ok);
  if (r.ok) {
    if (selectedFrameIdx === idx) selectedFrameIdx = -1;
    else if (selectedFrameIdx > idx) selectedFrameIdx--;
    redoStack[curSeq] = [];
    updateStatus();
  }
}

async function doDeleteCurSeq() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  if (!confirm('Hapus SEMUA frame Seq ' + curSeq + '?')) return;
  const r = await fetch('/clear?seq=' + curSeq);
  showToast(await r.text(), !r.ok);
  if (r.ok) { 
    selectedFrameIdx = -1;
    redoStack[curSeq] = []; 
    updateStatus(); 
  }
}

async function doDeleteAll() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  if (!confirm('Hapus SEMUA SEQUENCE (A, B, dan C)?')) return;
  const r = await fetch('/clearAll');
  showToast(await r.text(), !r.ok);
  if (r.ok) { 
    selectedFrameIdx = -1;
    redoStack = { A:[], B:[], C:[] }; 
    updateStatus(); 
  }
}

async function doPlay() {
  const r = await fetch('/play');
  showToast(await r.text(), !r.ok);
}

async function doStop() {
  const r = await fetch('/stop');
  showToast(await r.text(), !r.ok);
  convState = 0;
  updateConveyorUI();
}

// ============================================================
// FRAME LIST RENDER
// ============================================================
function renderFrames() {
  const frames = lastFrames[curSeq];
  const listEl = document.getElementById('frameList');
  const cnt = document.getElementById('fpCount');
  const selInfo = document.getElementById('selFrameInfo');
  const btnDelSel = document.getElementById('btnDelSelected');
  
  const count = (frames ? frames.length : 0);
  cnt.textContent = count + ' frame';

  if (selectedFrameIdx >= 0 && selectedFrameIdx < count) {
    selInfo.innerHTML = 'Frame Terpilih: <b>#' + (selectedFrameIdx + 1) + '</b>';
    btnDelSel.disabled = false;
  } else {
    selectedFrameIdx = -1;
    selInfo.textContent = 'Klik frame di bawah untuk preview & memilih';
    btnDelSel.disabled = true;
  }

  if (!frames || frames.length === 0) {
    listEl.innerHTML = '<div style="color:#444;padding:6px;">Belum ada frame pada Seq ' + curSeq + '</div>';
    return;
  }

  listEl.innerHTML = frames.map((f, i) => {
    const isSel = (i === selectedFrameIdx);
    return '<div class="fl-item' + (isSel ? ' selected' : '') + '" onclick="selectFrame(' + i + ')">' +
      '<div class="fl-data">' +
        '<span class="fl-num">#' + (i + 1) + '</span>' +
        '<span>[' + f.a.join(', ') + ']&deg;</span>' +
        '<span style="color:#666">h:' + f.h + 'ms</span>' +
      '</div>' +
      '<button class="fl-btn-del" title="Hapus frame ini" onclick="doDeleteSpecificFrame(' + i + ', event)">&#10005;</button>' +
    '</div>';
  }).join('');
}

// ============================================================
// STATUS POLL
// ============================================================
async function updateStatus() {
  try {
    const r = await fetch('/status');
    if (!r.ok) return;
    const d = await r.json();
    serverState = d.state;
    const sName = stateNames[d.state] || d.state;

    // Sync conveyor & sensor state dari server
    if (typeof d.conveyorOn !== 'undefined') {
      convState = d.conveyorOn ? 1 : 0;
      updateConveyorUI();
    }
    if (typeof d.sensorEnabled !== 'undefined') {
      sensorEnabledState = d.sensorEnabled;
    }
    if (typeof d.sensorDetect !== 'undefined') {
      updateSensorUI(sensorEnabledState, d.sensorDetect);
    }

    document.getElementById('statusBar').innerHTML =
      'State: <span class="s-state">' + sName + '</span><br>' +
      '<span class="s-seq">A: ' + (d.lenA||0) + ' &bull; B: ' + (d.lenB||0) + ' &bull; C: ' + (d.lenC||0) + ' frame</span>' +
      '[' + d.angles.join(' / ') + '] deg';

    // Sync sudut saat tidak ditekan
    if (!heldBtns['up'] && !heldBtns['down'] && !heldBtns['left'] && !heldBtns['right'] && 
        !heldBtns['tri'] && !heldBtns['cross'] && !heldBtns['sq'] && !heldBtns['circ']) {
      for (let i = 0; i < 4; i++) angles[i] = d.angles[i];
      updateAngleDisplay();
    }

    ['A','B','C'].forEach(sq => {
        if (d['frames' + sq]) lastFrames[sq] = d['frames' + sq];
    });
    renderFrames();
  } catch(e) {}
}

setInterval(updateStatus, 500);
updateStatus();
applyModeUI();
updateConveyorUI();
</script>
</body>
</html>)rawliteral";
