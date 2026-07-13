/* Tiltstone page shell — render + input ONLY.
 * Every game rule lives in engine.js (pure, DOM-free); timing + synth cue
 * recipes live in juice.js (pure). This file just draws state and forwards
 * inputs. The slice-3 juice replay is COSMETIC: the engine state is applied
 * immediately on every input (the smokes and the "shell adds nothing"
 * property depend on that); the animation merely replays the engine's own
 * resolveTrace on top, is cancelled by any new input, and is skipped
 * entirely under prefers-reduced-motion. Audio is never load-bearing —
 * every cue lands in an honest log whether or not it is audible.
 */
"use strict";
(function () {
  var E = window.TiltstoneEngine;
  var J = window.TiltstoneJuice;
  var canvas = document.getElementById("game");
  var ctx = canvas.getContext("2d");
  var TILE = canvas.width / E.SIZE;
  var audio = J.makeAudio();

  // Distinct hue + shape per gem color (shape carries the info without color).
  var GEMS = [
    { fill: "#d84f4f", edge: "#8f2a2a", shape: "diamond"  },
    { fill: "#4fa7d8", edge: "#2a628f", shape: "circle"   },
    { fill: "#63c96a", edge: "#2f7a35", shape: "triangle" },
    { fill: "#d8c94f", edge: "#8f832a", shape: "square"   }
  ];

  // --- state ---------------------------------------------------------------
  var qs = new URLSearchParams(window.location.search);
  var utcToday = new Date().toISOString().slice(0, 10);
  var DAILY = E.dailySeed(utcToday);
  var seed = qs.has("seed") ? (parseInt(qs.get("seed"), 10) >>> 0) : DAILY;
  var levelIndex = qs.has("level") ? Math.max(0, parseInt(qs.get("level"), 10) | 0) : 0;
  var state = E.newGame(seed, levelIndex);
  var history = [];   // immutable prior states — engine states never mutate, so undo is a stack
  var undos = 0;      // honest take-back count for this attempt (shown on the win card)
  var anim = null;    // live replay of the last rotation's resolveTrace, or null

  function reset(newSeed, newLevel) {
    cancelAnim();
    seed = newSeed >>> 0;
    levelIndex = Math.max(0, newLevel | 0);
    state = E.newGame(seed, levelIndex);
    history = [];
    undos = 0;
    setMsg("");
    render();
  }

  function gradeLine() {
    var p = E.par(state.level), g = E.grade(state.used, p);
    return "PAR " + p + " — YOU " + state.used + " — " + g.label +
      (undos ? " (" + undos + " undo" + (undos === 1 ? "" : "s") + ")" : "");
  }

  function act(dir) {
    if (state.status !== "playing") return;
    cancelAnim();
    var prev = state;
    state = E.rotate(state, dir);         // authoritative, IMMEDIATE
    history.push(prev);
    audio.play("rotate");
    if (state.events.length) {
      var got = state.collected - prev.collected;
      var chains = state.events[state.events.length - 1].chain;
      setMsg("+" + got + " gem" + (got === 1 ? "" : "s") + (chains > 1 ? " (chain x" + chains + "!)" : ""));
    } else setMsg("");
    if (state.status === "won") { setMsg("QUOTA MET — " + gradeLine() + ". N for the next level."); saveBest(); }
    if (state.status === "lost") setMsg("Out of rotations. U to take one back, R to start this cavern over.");
    updateHud();
    // cosmetic replay of the very trace the engine just resolved
    var trace = E.resolveTrace(E.rotateGrid(prev.grid, dir));
    if (E.gridString(trace.grid) !== E.gridString(state.grid)) { render(); return; } // defensive: never animate a lie
    var terminal = state.status === "won" ? "win" : state.status === "lost" ? "lose" : null;
    startReplay(prev.grid, dir, trace, terminal);
  }

  // Take back the last rotation. Free on a misread (this is a deliberation
  // game), honest on the scoreboard (the win card carries the undo count).
  // Allowed while playing and from a BURIED card (step back instead of a full
  // restart); a WON card stays frozen like the engine's own terminal rule.
  function undo() {
    if (!history.length || state.status === "won") return;
    cancelAnim();
    state = history.pop();
    undos++;
    audio.play("undo");
    setMsg("took back a turn (" + undos + " undo" + (undos === 1 ? "" : "s") + " this attempt)");
    render();
  }

  // --- juice replay ----------------------------------------------------------
  // The engine's resolveTrace phases, scheduled by juice.timeline, drawn with
  // requestAnimationFrame: board sweep (rotateMs), gems easing down their
  // columns, collected groups popping per chain. Cues fire on the schedule;
  // cancelling (any new input) FLUSHES unfired cues into the log, so the cue
  // sequence of a move is deterministic regardless of frame timing.

  function reducedMotion() {
    return !!(window.matchMedia && window.matchMedia("(prefers-reduced-motion: reduce)").matches);
  }

  function fireStepCues(step, when) { // when: "start" | "end" | "both"
    for (var i = 0; i < step.cues.length; i++) {
      var cue = step.cues[i];
      var flag = cue.at === "start" ? "_startFired" : "_endFired";
      if (step[flag]) continue;
      if (when !== "both" && cue.at !== when) continue;
      step[flag] = true;
      audio.play(cue.name, cue.chain);
    }
  }

  function cancelAnim() {
    if (!anim) return;
    if (anim.raf) cancelAnimationFrame(anim.raf);
    for (var i = 0; i < anim.steps.length; i++) fireStepCues(anim.steps[i], "both");
    anim = null;
  }

  function startReplay(prevGrid, dir, trace, terminalCue) {
    var built = J.timeline(trace.phases);
    var steps = built.steps.slice();
    if (terminalCue) steps.push({ phase: null, t0: built.total, t1: built.total, cues: [{ name: terminalCue, at: "start" }] });
    if (reducedMotion()) {                    // no animation: cues in order, final frame, done
      for (var i = 0; i < steps.length; i++) fireStepCues(steps[i], "both");
      render();
      return;
    }
    var pres = [], cur = E.rotateGrid(prevGrid, dir);  // grid BEFORE each step
    for (var s = 0; s < steps.length; s++) { pres.push(cur); if (steps[s].phase) cur = steps[s].phase.grid; }
    var off = document.createElement("canvas");        // pre-rotation snapshot for the sweep
    off.width = canvas.width; off.height = canvas.height;
    drawBoardTo(off.getContext("2d"), prevGrid);
    anim = { steps: steps, pres: pres, total: built.total, off: off, dir: dir,
             rotated: pres.length ? pres[0] : cur, start: performance.now(), raf: 0 };
    frame();  // synchronous first frame — the input visibly lands this tick
  }

  function easeOut(p) { return 1 - (1 - p) * (1 - p); }
  function easeIn(p) { return p * p; }

  function frame() {
    if (!anim) return;
    var R = J.TIMING.rotateMs;
    var now = performance.now() - anim.start;
    var t = now - R;                                   // trace-local time
    for (var i = 0; i < anim.steps.length; i++) {      // cue schedule (in order)
      var st = anim.steps[i];
      if (t >= st.t0) fireStepCues(st, "start");
      if (t >= st.t1) fireStepCues(st, "end");
      else break;
    }
    if (t >= anim.total) {                             // replay over
      cancelAnim();
      render();
      return;
    }
    if (now < R) {                                     // board sweep
      var p = Math.max(now / R, 0.06);
      var a = easeOut(p) * (Math.PI / 2) * (anim.dir === "cw" ? 1 : -1);
      var s = 1 / (Math.abs(Math.cos(a)) + Math.abs(Math.sin(a))); // stay inside the frame
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      ctx.save();
      ctx.translate(canvas.width / 2, canvas.height / 2);
      ctx.rotate(a); ctx.scale(s, s);
      ctx.drawImage(anim.off, -canvas.width / 2, -canvas.height / 2);
      ctx.restore();
    } else {
      drawTraceAt(Math.max(0, t));
    }
    anim.raf = requestAnimationFrame(frame);
  }

  function drawTraceAt(t) {
    var steps = anim.steps, active = -1, lastDone = -1;
    for (var i = 0; i < steps.length; i++) {
      if (!steps[i].phase) continue;
      if (t >= steps[i].t1) { lastDone = i; continue; }
      if (t >= steps[i].t0) active = i;
      break;
    }
    if (active === -1) {                               // between steps: rest on the last settled look
      drawBoardTo(ctx, lastDone === -1 ? anim.rotated : steps[lastDone].phase.grid);
      return;
    }
    var step = steps[active], ph = step.phase, pre = anim.pres[active];
    var p = Math.min(1, (t - step.t0) / Math.max(1, step.t1 - step.t0));
    if (ph.type === "fall") {
      var moving = {};
      for (var m = 0; m < ph.moves.length; m++) moving[ph.moves[m].from[0] + "," + ph.moves[m].from[1]] = true;
      drawBoardTo(ctx, pre, moving);                   // movers blanked at their source…
      var q = easeIn(p);                               // …and drawn mid-drop (gravity accelerates)
      for (var m2 = 0; m2 < ph.moves.length; m2++) {
        var mv = ph.moves[m2];
        var y = (mv.from[0] + (mv.to[0] - mv.from[0]) * q) * TILE;
        var x2 = (mv.from[1] + (mv.to[1] - mv.from[1]) * q) * TILE; // slipping ice moves columns
        drawCellTo(ctx, x2, y, mv.v);
      }
    } else {                                           // collect: the group pops out
      drawBoardTo(ctx, ph.grid);                       // board with the group already gone
      for (var c = 0; c < ph.cells.length; c++) {
        var cell = ph.cells[c], x = cell[1] * TILE, y2 = cell[0] * TILE;
        ctx.save();
        ctx.globalAlpha = 1 - p;
        drawCellTo(ctx, x, y2, pre[cell[0]][cell[1]]);
        ctx.globalAlpha = (1 - p) * 0.9;
        ctx.strokeStyle = "#ffffff"; ctx.lineWidth = 2.5;
        ctx.beginPath();
        ctx.arc(x + TILE / 2, y2 + TILE / 2, (TILE * 0.2) + (TILE * 0.45) * p, 0, Math.PI * 2);
        ctx.stroke();
        ctx.restore();
      }
      if (ph.unlocked) {                               // slice 4: cages springing open
        for (var u = 0; u < ph.unlocked.length; u++) {
          var uc = ph.unlocked[u];
          ctx.save();
          ctx.globalAlpha = 1 - p;
          ctx.strokeStyle = "#ffe9a8"; ctx.lineWidth = 3;
          ctx.strokeRect(uc[1] * TILE + 3, uc[0] * TILE + 3, TILE - 6, TILE - 6);
          ctx.restore();
        }
      }
    }
  }

  // --- persistence (guarded — blocked storage degrades silently) -----------
  function saveBest() {
    try {
      var k = "tiltstone-best-" + seed;
      var prev = parseInt(window.localStorage.getItem(k) || "-1", 10);
      if (levelIndex > prev) window.localStorage.setItem(k, String(levelIndex));
    } catch (e) { /* private mode etc. */ }
  }
  function loadMuted() {
    try { return window.localStorage.getItem("tiltstone-muted") === "1"; } catch (e) { return false; }
  }
  function saveMuted(m) {
    try { window.localStorage.setItem("tiltstone-muted", m ? "1" : "0"); } catch (e) { /* guarded */ }
  }

  // --- rendering ------------------------------------------------------------
  function setMsg(t) { document.getElementById("msg").textContent = t; }

  function drawGemTo(g2d, x, y, spec) {
    var pad = TILE * 0.18, s = TILE - pad * 2, cx = x + TILE / 2, cy = y + TILE / 2;
    g2d.fillStyle = spec.fill; g2d.strokeStyle = spec.edge; g2d.lineWidth = 2;
    g2d.beginPath();
    if (spec.shape === "diamond") {
      g2d.moveTo(cx, y + pad); g2d.lineTo(x + TILE - pad, cy); g2d.lineTo(cx, y + TILE - pad); g2d.lineTo(x + pad, cy);
    } else if (spec.shape === "circle") {
      g2d.arc(cx, cy, s / 2, 0, Math.PI * 2);
    } else if (spec.shape === "triangle") {
      g2d.moveTo(cx, y + pad); g2d.lineTo(x + TILE - pad, y + TILE - pad); g2d.lineTo(x + pad, y + TILE - pad);
    } else {
      g2d.rect(x + pad, y + pad, s, s);
    }
    g2d.closePath(); g2d.fill(); g2d.stroke();
    // small inner glint
    g2d.fillStyle = "rgba(255,255,255,0.35)";
    g2d.beginPath(); g2d.arc(cx - s * 0.15, cy - s * 0.15, s * 0.09, 0, Math.PI * 2); g2d.fill();
  }

  // One cell's content at pixel (x, y) — x/y may be fractional mid-tween.
  function drawCellTo(g2d, x, y, v) {
    if (v === E.WALL) {
      g2d.fillStyle = "#4a3b28"; g2d.fillRect(x + 1, y + 1, TILE - 2, TILE - 2);
      g2d.strokeStyle = "#6b563a"; g2d.lineWidth = 2;
      g2d.strokeRect(x + 3.5, y + 3.5, TILE - 7, TILE - 7);
    } else if (v === E.STONE) {
      g2d.fillStyle = "#7d746a"; g2d.strokeStyle = "#4d463e"; g2d.lineWidth = 2;
      g2d.beginPath(); g2d.arc(x + TILE / 2, y + TILE / 2 + 2, TILE * 0.34, 0, Math.PI * 2);
      g2d.fill(); g2d.stroke();
    } else if (v === E.ICE) {                        // slice 4: slippery block
      var ip = TILE * 0.14;
      g2d.fillStyle = "rgba(150,205,235,0.82)"; g2d.strokeStyle = "#7db6d6"; g2d.lineWidth = 2;
      g2d.beginPath();
      g2d.roundRect ? g2d.roundRect(x + ip, y + ip, TILE - ip * 2, TILE - ip * 2, 5)
                    : g2d.rect(x + ip, y + ip, TILE - ip * 2, TILE - ip * 2);
      g2d.fill(); g2d.stroke();
      g2d.strokeStyle = "rgba(255,255,255,0.7)"; g2d.lineWidth = 1.5;
      g2d.beginPath(); g2d.moveTo(x + TILE * 0.3, y + TILE * 0.62); g2d.lineTo(x + TILE * 0.62, y + TILE * 0.3); g2d.stroke();
    } else if (E.isLocked(v)) {                      // slice 4: caged gem
      var spec = GEMS[(v - E.LOCK0) % GEMS.length];
      g2d.save(); g2d.globalAlpha *= 0.55;
      drawGemTo(g2d, x, y, spec);
      g2d.restore();
      g2d.strokeStyle = "#c9c2b4"; g2d.lineWidth = 2;
      var bp = TILE * 0.12;
      g2d.strokeRect(x + bp, y + bp, TILE - bp * 2, TILE - bp * 2);
      g2d.beginPath();
      g2d.moveTo(x + TILE / 3, y + bp); g2d.lineTo(x + TILE / 3, y + TILE - bp);
      g2d.moveTo(x + TILE * 2 / 3, y + bp); g2d.lineTo(x + TILE * 2 / 3, y + TILE - bp);
      g2d.stroke();
    } else if (E.isGrate(v)) {                       // slice 4: one-way grate
      var o = v - E.GRATE0;                          // 0=up 1=right 2=down 3=left
      g2d.fillStyle = "#3a4148"; g2d.fillRect(x + 1, y + 1, TILE - 2, TILE - 2);
      g2d.strokeStyle = o === 2 ? "#9fd6a8" : "#8a939c"; g2d.lineWidth = 2;
      g2d.beginPath();
      for (var s = 1; s <= 3; s++) {
        if (o === 1 || o === 3) { g2d.moveTo(x + (TILE * s) / 4, y + 5); g2d.lineTo(x + (TILE * s) / 4, y + TILE - 5); }
        else { g2d.moveTo(x + 5, y + (TILE * s) / 4); g2d.lineTo(x + TILE - 5, y + (TILE * s) / 4); }
      }
      g2d.stroke();
      var cx = x + TILE / 2, cy = y + TILE / 2, a = TILE * 0.16;   // orientation arrow
      g2d.fillStyle = o === 2 ? "#9fd6a8" : "#c9c2b4";
      g2d.beginPath();
      if (o === 0)      { g2d.moveTo(cx, cy - a); g2d.lineTo(cx + a, cy + a); g2d.lineTo(cx - a, cy + a); }
      else if (o === 1) { g2d.moveTo(cx + a, cy); g2d.lineTo(cx - a, cy + a); g2d.lineTo(cx - a, cy - a); }
      else if (o === 2) { g2d.moveTo(cx, cy + a); g2d.lineTo(cx + a, cy - a); g2d.lineTo(cx - a, cy - a); }
      else              { g2d.moveTo(cx - a, cy); g2d.lineTo(cx + a, cy + a); g2d.lineTo(cx + a, cy - a); }
      g2d.closePath(); g2d.fill();
    } else if (v >= E.GEM0) {
      drawGemTo(g2d, x, y, GEMS[(v - E.GEM0) % GEMS.length]);
    }
  }

  // Floor + every cell of `grid`; cells whose "r,c" key is in `skip` draw
  // floor only (their piece is being tweened elsewhere this frame).
  function drawBoardTo(g2d, grid, skip) {
    g2d.clearRect(0, 0, canvas.width, canvas.height);
    for (var r = 0; r < E.SIZE; r++) {
      for (var c = 0; c < E.SIZE; c++) {
        var x = c * TILE, y = r * TILE;
        g2d.fillStyle = (r + c) % 2 ? "#221a13" : "#1d1712";       // checker floor
        g2d.fillRect(x, y, TILE, TILE);
        if (skip && skip[r + "," + c]) continue;
        drawCellTo(g2d, x, y, grid[r][c]);
      }
    }
  }

  function updateHud() {
    document.getElementById("hud-gems").textContent = "GEMS " + state.collected + "/" + state.quota;
    document.getElementById("hud-turns").textContent = "TURNS " + state.used + "/" + state.budget;
    document.getElementById("hud-par").textContent = "PAR " + E.par(state.level);
    document.getElementById("hud-level").textContent = "LV " + (state.levelIndex + 1);
    document.getElementById("hud-seed").textContent = "SEED " + state.seed + (state.seed === DAILY ? "*" : "");
    document.getElementById("btn-next").disabled = state.status !== "won";
    document.getElementById("btn-undo").disabled = !history.length || state.status === "won";
  }

  function render() {
    drawBoardTo(ctx, state.grid);
    if (state.status !== "playing") {                                // end card
      ctx.fillStyle = "rgba(10,7,5,0.72)"; ctx.fillRect(0, 0, canvas.width, canvas.height);
      ctx.fillStyle = state.status === "won" ? "#d9b96c" : "#d84f4f";
      ctx.font = "bold 34px 'Courier New', monospace"; ctx.textAlign = "center";
      ctx.fillText(state.status === "won" ? "CLEARED" : "BURIED", canvas.width / 2, canvas.height / 2 - 24);
      ctx.fillStyle = "#e8dcc8"; ctx.font = "15px 'Courier New', monospace";
      if (state.status === "won") {
        ctx.fillText(gradeLine(), canvas.width / 2, canvas.height / 2 + 8);
        ctx.fillText("press N — deeper cavern", canvas.width / 2, canvas.height / 2 + 32);
      } else {
        ctx.fillText("press U — take a turn back", canvas.width / 2, canvas.height / 2 + 8);
        ctx.fillText("press R — same cavern, fresh turns", canvas.width / 2, canvas.height / 2 + 32);
      }
      ctx.textAlign = "start";
    }
    updateHud();
  }

  // --- input -----------------------------------------------------------------
  function nextLevel() { if (state.status === "won") reset(seed, levelIndex + 1); }
  function loadSeedBox() {
    var v = document.getElementById("seedbox").value.trim();
    if (v !== "" && isFinite(+v)) reset(+v, 0);
  }
  function toggleMute() {
    var m = !audio.isMuted();
    audio.setMuted(m);
    saveMuted(m);
    document.getElementById("btn-mute").textContent = m ? "Sound: off" : "Sound: on";
  }
  audio.setMuted(loadMuted());
  document.getElementById("btn-mute").textContent = audio.isMuted() ? "Sound: off" : "Sound: on";

  window.addEventListener("keydown", function (ev) {
    if (ev.target && ev.target.id === "seedbox") {
      if (ev.key === "Enter") loadSeedBox();
      return;
    }
    var k = ev.key.toLowerCase();
    if (k === "arrowleft" || k === "a") { ev.preventDefault(); act("ccw"); }
    else if (k === "arrowright" || k === "d") { ev.preventDefault(); act("cw"); }
    else if (k === "r") reset(seed, levelIndex);
    else if (k === "n") nextLevel();
    else if (k === "u") undo();
    else if (k === "m") toggleMute();
  });
  document.getElementById("btn-ccw").addEventListener("click", function () { act("ccw"); });
  document.getElementById("btn-cw").addEventListener("click", function () { act("cw"); });
  document.getElementById("btn-undo").addEventListener("click", undo);
  document.getElementById("btn-restart").addEventListener("click", function () { reset(seed, levelIndex); });
  document.getElementById("btn-next").addEventListener("click", nextLevel);
  document.getElementById("btn-daily").addEventListener("click", function () { reset(DAILY, 0); });
  document.getElementById("btn-load").addEventListener("click", loadSeedBox);
  document.getElementById("btn-mute").addEventListener("click", toggleMute);

  // --- test API (harmless for players; used by tools/web-smoke-tiltstone.mjs) --
  window.TILTSTONE = {
    version: E.VERSION,
    engine: E,
    juice: J,
    getState: function () { return state; },
    gridString: function () { return E.gridString(state.grid); },
    rotate: function (dir) { act(dir); return state; },
    reset: function (s, l) { reset(s, l || 0); return state; },
    undo: function () { undo(); return state; },
    getUndos: function () { return undos; },
    getHistoryLength: function () { return history.length; },
    getCueLog: function () { return audio.getLog(); },
    isAnimating: function () { return !!anim; },
    isMuted: function () { return audio.isMuted(); },
    render: function () { render(); },
    dailySeed: DAILY
  };

  render();
})();
