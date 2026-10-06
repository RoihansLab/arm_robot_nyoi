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
.mode-wrap{display:flex;align-items:center;gap:12px;margin-bottom:10px;}
.mode-label{font-size:.62rem;color:#444;text-transform:uppercase;letter-spacing:1px;}
.mode-toggle{display:flex;background:rgba(0,0,0,0.5);border:1px solid rgba(255,255,255,0.08);border-radius:20px;overflow:hidden;}
.mode-btn{padding:6px 16px;font-size:.7rem;font-weight:700;border:none;background:transparent;color:#444;cursor:pointer;transition:all .2s;letter-spacing:.5px;text-transform:uppercase;}
.mode-btn.active{background:#6c63ff;color:#fff;box-shadow:0 0 12px rgba(108,99,255,.5);}

/* GAMEPAD BODY */
.gamepad{width:100%;max-width:480px;position:relative;background:linear-gradient(165deg,#1c1c2e 0%,#141422 60%,#0e0e1c 100%);border-radius:36px 36px 50px 50px;border:1px solid rgba(255,255,255,0.06);box-shadow:0 25px 70px rgba(0,0,0,.9),inset 0 1px 0 rgba(255,255,255,.05),0 0 0 1px rgba(108,99,255,.08);}
/* Grips */
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

/* SEQ TABS in center bumper area */
.seq-area{display:flex;flex-direction:column;align-items:center;gap:3px;padding-bottom:2px;}
.seq-lbl{font-size:.52rem;color:#333;text-transform:uppercase;letter-spacing:.5px;}
.seq-tabs{display:flex;gap:4px;}
.seq-tab{width:34px;height:22px;border:1px solid rgba(255,255,255,.08);border-radius:4px;background:rgba(0,0,0,.4);color:#444;font-size:.68rem;font-weight:700;cursor:pointer;transition:all .15s;}
.seq-tab.active{background:#6c63ff;color:#fff;border-color:#6c63ff;box-shadow:0 0 8px rgba(108,99,255,.4);}

/* INNER PANEL */
.inner{background:linear-gradient(to bottom,#161624,#101018);border-radius:0 0 30px 30px;border:1px solid rgba(255,255,255,.04);border-top:1.5px solid rgba(255,255,255,.03);padding:14px 10px 16px;}

/* CONTROLS ROW */
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
.del-btn{background:linear-gradient(to right,rgba(231,76,60,.12),rgba(231,76,60,.04));border:1px solid rgba(231,76,60,.2);color:#e74c3c;border-radius:8px;padding:7px 22px;font-size:.68rem;font-weight:700;letter-spacing:1px;text-transform:uppercase;cursor:pointer;transition:all .15s;}
.del-btn:hover{background:rgba(231,76,60,.2);border-color:rgba(231,76,60,.4);}
.del-btn:active{transform:scale(.96);}

/* ANGLE DISPLAY */
.ang-display{display:flex;gap:12px;flex-wrap:wrap;justify-content:center;margin-top:48px;max-width:480px;}
.ang-item{display:flex;flex-direction:column;align-items:center;gap:1px;}
.ang-lbl{font-size:.52rem;color:#333;text-transform:uppercase;letter-spacing:.5px;}
.ang-val{color:#6c63ff;font-size:.95rem;font-weight:700;font-variant-numeric:tabular-nums;min-width:36px;text-align:center;}

/* DISABLED overlay */
.ctrl-disabled{opacity:.2;pointer-events:none;}

/* FRAME LIST */
.frame-panel{width:100%;max-width:480px;margin-top:10px;background:rgba(255,255,255,.025);border:1px solid rgba(255,255,255,.05);border-radius:10px;padding:8px 12px;}
.frame-panel-hdr{display:flex;justify-content:space-between;align-items:center;margin-bottom:6px;}
.fp-title{font-size:.62rem;color:#555;text-transform:uppercase;letter-spacing:.5px;}
.fp-count{font-size:.62rem;color:#6c63ff;}
.frame-list{font-size:.62rem;color:#444;max-height:90px;overflow-y:auto;line-height:1.8;}
.frame-list .fl-item{border-bottom:1px solid rgba(255,255,255,.03);padding:1px 0;}
.frame-list .fl-item:last-child{border:none;}

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
  <button class="mode-btn" id="btnConvToggle" onclick="toggleConveyor()" style="margin-left:auto; border-radius:10px; background:#4a4a4a; color:#fff;">Tes Konveyor: OFF</button>
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
        <button class="seq-tab" id="tabD" onclick="selSeq('D')">D</button>
        <button class="seq-tab" id="tabE" onclick="selSeq('E')">E</button>
        <button class="seq-tab" id="tabF" onclick="selSeq('F')">F</button>
        <button class="seq-tab" id="tabG" onclick="selSeq('G')">G</button>
        <button class="seq-tab" id="tabH" onclick="selSeq('H')">H</button>
        <button class="seq-tab" id="tabI" onclick="selSeq('I')">I</button>
        <button class="seq-tab" id="tabJ" onclick="selSeq('J')">J</button>
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

    <!-- DELETE ALL -->
    <div class="btm-row" style="gap:10px;">
      <button class="del-btn" id="btnDel" onclick="doDeleteCurSeq()">&#128465; Hapus Seq</button>
      <button class="del-btn" id="btnDelAll" onclick="doDeleteAll()" style="color:#f39c12;border-color:rgba(243,156,18,0.3);background:rgba(243,156,18,0.1);">Reset Semua</button>
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

<!-- FRAME LIST -->
<div class="frame-panel">
  <div class="frame-panel-hdr">
    <span class="fp-title" id="fpTitle">Seq A Frames</span>
    <span class="fp-count" id="fpCount">0 frame</span>
  </div>
  <div class="frame-list" id="frameList"><div style="color:#2a2a3a">Belum ada frame</div></div>
</div><script>
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
let angles = [90, 90, 90, 90];  // local tracking
let redoStack = { A:[], B:[], C:[], D:[], E:[], F:[], G:[], H:[], I:[], J:[] };
let heldBtns = {};               // { 'tri': true, ... }
let moveOrder = [];              // Track movement order
let convState = 0;               // 0 = off, 1 = on

const stateNames = {
  BOOT_HOME:'Boot - Menuju Home', IDLE:'Siap', RECORDING:'Recording',
  PLAY_A:'Playing Seq A', PLAY_B:'Playing Seq B', PLAY_C:'Playing Seq C', PLAY_D:'Playing Seq D',
  PLAY_E:'Playing Seq E', PLAY_F:'Playing Seq F', PLAY_G:'Playing Seq G', PLAY_H:'Playing Seq H',
  PLAY_I:'Playing Seq I', PLAY_J:'Playing Seq J',
  CONVEYOR_RUN:'Konveyor Berjalan', ESTOP:'!!! E-STOP !!!'
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
}

function applyModeUI() {
  const isManual = mode === 'manual';
  // Manual controls: d-pad + face buttons
  document.getElementById('dpadWrapper').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('facePad').classList.toggle('ctrl-disabled', !isManual);
  
  // Record/Edit controls should be active ONLY in manual mode
  document.getElementById('btnL1').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnR1').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnGuide').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnDel').classList.toggle('ctrl-disabled', !isManual);
  document.getElementById('btnDelAll').classList.toggle('ctrl-disabled', !isManual);
  // Start/Select always active
}

// ============================================================
// SEQUENCE TAB
// ============================================================
function selSeq(s) {
  curSeq = s;
  const seqs = ['A','B','C','D','E','F','G','H','I','J'];
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
const DEADZONE = 0.07;

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

  updateAngleDisplay();
}, 50);

function updateAngleDisplay() {
  document.getElementById('angB').textContent = Math.round(angles[0]);
  document.getElementById('angS').textContent = Math.round(angles[1]);
  document.getElementById('angE').textContent = Math.round(angles[2]);
  document.getElementById('angG').textContent = Math.round(angles[3]);
}

// ============================================================
// FACE BUTTONS & D-PAD (press & hold for continuous)
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

// Triangle: elbow up
setupHoldBtn(document.getElementById('btnTri'), 'tri');
// Cross: elbow down
setupHoldBtn(document.getElementById('btnCross'), 'cross');

// Square: grip (move to min)
setupHoldBtn(document.getElementById('btnSq'), 'sq', () => {
  if (mode !== 'manual') return;
  if (!moveOrder.includes(3)) moveOrder.push(3);
  angles[3] = ANGLE_MIN[3];
  sendServo(3, angles[3]);
  updateAngleDisplay();
});

// Circle: release (move to max)
setupHoldBtn(document.getElementById('btnCirc'), 'circ', () => {
  if (mode !== 'manual') return;
  if (!moveOrder.includes(3)) moveOrder.push(3);
  angles[3] = ANGLE_MAX[3];
  sendServo(3, angles[3]);
  updateAngleDisplay();
});

// ============================================================
// BUMPERS: L1 = Undo, R1 = Redo
// ============================================================
setupHoldBtn(document.getElementById('btnL1'), 'l1', () => doUndo());
setupHoldBtn(document.getElementById('btnR1'), 'r1', () => doRedo());

// ============================================================
// KEYFRAME ACTIONS
// ============================================================
let lastFrames = { A:[], B:[], C:[], D:[], E:[], F:[], G:[], H:[], I:[], J:[] };

async function doSaveFrame() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  const hold = parseInt(document.getElementById('holdMs').value) || 500;
  
  // Lengkapi order dengan servo yang tidak ditekan
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
  if (r.ok) updateStatus();
}

async function doRedo() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  const stack = redoStack[curSeq];
  if (stack.length === 0) { showToast('Tidak ada redo tersedia', true); return; }
  const frame = stack.pop();
  // Move servos to that frame position
  for (let i = 0; i < 4; i++) {
    angles[i] = frame.a[i];
    await sendServo(i, frame.a[i]);
  }
  updateAngleDisplay();
  const hold = frame.h;
  const r = await fetch('/save?seq=' + curSeq + '&hold=' + hold);
  showToast('Redo: ' + (await r.text()), !r.ok);
  if (r.ok) updateStatus();
}

async function doDeleteCurSeq() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  if (!confirm('Hapus SEMUA frame Seq ' + curSeq + '?')) return;
  const r = await fetch('/clear?seq=' + curSeq);
  showToast(await r.text(), !r.ok);
  if (r.ok) { redoStack[curSeq] = []; updateStatus(); }
}

async function doDeleteAll() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  if (!confirm('Hapus SEMUA SEQUENCE (A sampai J)?')) return;
  const r = await fetch('/clearAll');
  showToast(await r.text(), !r.ok);
  if (r.ok) { 
    redoStack = { A:[], B:[], C:[], D:[], E:[], F:[], G:[], H:[], I:[], J:[] }; 
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
}

async function toggleConveyor() {
  if (serverState === 'ESTOP') { showToast('E-STOP aktif!', true); return; }
  convState = convState === 0 ? 1 : 0;
  const btn = document.getElementById('btnConvToggle');
  
  const r = await fetch('/conveyor?state=' + convState);
  if (r.ok) {
    btn.textContent = 'Tes Konveyor: ' + (convState ? 'ON' : 'OFF');
    btn.style.background = convState ? '#2ea88a' : '#4a4a4a';
  } else {
    convState = convState === 0 ? 1 : 0; // revert on fail
    showToast(await r.text(), true);
  }
}

// ============================================================
// FRAME LIST RENDER
// ============================================================
function renderFrames() {
  const frames = lastFrames[curSeq];
  const ul = document.getElementById('frameList');
  const cnt = document.getElementById('fpCount');
  cnt.textContent = (frames ? frames.length : 0) + ' frame';
  if (!frames || frames.length === 0) {
    ul.innerHTML = '<div style="color:#2a2a3a">Belum ada frame</div>'; return;
  }
  ul.innerHTML = frames.map((f,i) =>
    '<div class="fl-item">#' + (i+1) + ' [' + f.a.join(',') + '] hold:' + f.h + 'ms (Ord:' + (f.o ? f.o.join(',') : '') + ')</div>'
  ).join('');
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
    document.getElementById('statusBar').innerHTML =
      'State: <span class="s-state">' + sName + '</span><br>' +
      '<span class="s-seq">A:' + (d.lenA||0) + ' B:' + (d.lenB||0) + ' C:' + (d.lenC||0) + ' D:' + (d.lenD||0) + ' E:' + (d.lenE||0) + ' F:' + (d.lenF||0) + ' G:' + (d.lenG||0) + ' H:' + (d.lenH||0) + ' I:' + (d.lenI||0) + ' J:' + (d.lenJ||0) + '</span>' +
      '<br>[' + d.angles.join('/') + '] deg';

    // sync local angles from server (when not dragging/pressing)
    if (!heldBtns['up'] && !heldBtns['down'] && !heldBtns['left'] && !heldBtns['right'] && !heldBtns['tri'] && !heldBtns['cross'] && !heldBtns['sq'] && !heldBtns['circ']) {
      for (let i = 0; i < 4; i++) angles[i] = d.angles[i];
      updateAngleDisplay();
    }

    ['A','B','C','D','E','F','G','H','I','J'].forEach(sq => {
        if (d['frames' + sq]) lastFrames[sq] = d['frames' + sq];
    });
    renderFrames();
  } catch(e) {}
}

setInterval(updateStatus, 500);
updateStatus();
applyModeUI();
</script>
</body>
</html>)rawliteral";
