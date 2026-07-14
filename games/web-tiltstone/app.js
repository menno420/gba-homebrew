/* Tiltstone page shell — render + input ONLY.
 * Every game rule lives in engine.js (pure, DOM-free); this file just draws
 * the current state and forwards inputs. Turn-based -> no animation loop:
 * the canvas repaints on demand after each action.
 */
"use strict";
(function () {
  var E = window.TiltstoneEngine;
  var canvas = document.getElementById("game");
  var ctx = canvas.getContext("2d");
  var TILE = canvas.width / E.SIZE;

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

  function reset(newSeed, newLevel) {
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
    var prev = state;
    state = E.rotate(state, dir);
    history.push(prev);
    if (state.events.length) {
      var got = state.collected - prev.collected;
      var chains = state.events[state.events.length - 1].chain;
      setMsg("+" + got + " gem" + (got === 1 ? "" : "s") + (chains > 1 ? " (chain x" + chains + "!)" : ""));
    } else setMsg("");
    if (state.status === "won") { setMsg("QUOTA MET — " + gradeLine() + ". N for the next level."); saveBest(); }
    if (state.status === "lost") setMsg("Out of rotations. U to take one back, R to start this cavern over.");
    render();
  }

  // Take back the last rotation. Free on a misread (this is a deliberation
  // game), honest on the scoreboard (the win card carries the undo count).
  // Allowed while playing and from a BURIED card (step back instead of a full
  // restart); a WON card stays frozen like the engine's own terminal rule.
  function undo() {
    if (!history.length || state.status === "won") return;
    state = history.pop();
    undos++;
    setMsg("took back a turn (" + undos + " undo" + (undos === 1 ? "" : "s") + " this attempt)");
    render();
  }

  // --- persistence (guarded — blocked storage degrades silently) -----------
  function saveBest() {
    try {
      var k = "tiltstone-best-" + seed;
      var prev = parseInt(window.localStorage.getItem(k) || "-1", 10);
      if (levelIndex > prev) window.localStorage.setItem(k, String(levelIndex));
    } catch (e) { /* private mode etc. */ }
  }

  // --- rendering ------------------------------------------------------------
  function setMsg(t) { document.getElementById("msg").textContent = t; }

  function drawGem(x, y, spec) {
    var pad = TILE * 0.18, s = TILE - pad * 2, cx = x + TILE / 2, cy = y + TILE / 2;
    ctx.fillStyle = spec.fill; ctx.strokeStyle = spec.edge; ctx.lineWidth = 2;
    ctx.beginPath();
    if (spec.shape === "diamond") {
      ctx.moveTo(cx, y + pad); ctx.lineTo(x + TILE - pad, cy); ctx.lineTo(cx, y + TILE - pad); ctx.lineTo(x + pad, cy);
    } else if (spec.shape === "circle") {
      ctx.arc(cx, cy, s / 2, 0, Math.PI * 2);
    } else if (spec.shape === "triangle") {
      ctx.moveTo(cx, y + pad); ctx.lineTo(x + TILE - pad, y + TILE - pad); ctx.lineTo(x + pad, y + TILE - pad);
    } else {
      ctx.rect(x + pad, y + pad, s, s);
    }
    ctx.closePath(); ctx.fill(); ctx.stroke();
    // small inner glint
    ctx.fillStyle = "rgba(255,255,255,0.35)";
    ctx.beginPath(); ctx.arc(cx - s * 0.15, cy - s * 0.15, s * 0.09, 0, Math.PI * 2); ctx.fill();
  }

  function render() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    for (var r = 0; r < E.SIZE; r++) {
      for (var c = 0; c < E.SIZE; c++) {
        var x = c * TILE, y = r * TILE, v = state.grid[r][c];
        ctx.fillStyle = (r + c) % 2 ? "#221a13" : "#1d1712";       // checker floor
        ctx.fillRect(x, y, TILE, TILE);
        if (v === E.WALL) {
          ctx.fillStyle = "#4a3b28"; ctx.fillRect(x + 1, y + 1, TILE - 2, TILE - 2);
          ctx.strokeStyle = "#6b563a"; ctx.lineWidth = 2;
          ctx.strokeRect(x + 3.5, y + 3.5, TILE - 7, TILE - 7);
        } else if (v === E.STONE) {
          ctx.fillStyle = "#7d746a"; ctx.strokeStyle = "#4d463e"; ctx.lineWidth = 2;
          ctx.beginPath(); ctx.arc(x + TILE / 2, y + TILE / 2 + 2, TILE * 0.34, 0, Math.PI * 2);
          ctx.fill(); ctx.stroke();
        } else if (v >= E.GEM0) {
          drawGem(x, y, GEMS[(v - E.GEM0) % GEMS.length]);
        }
      }
    }
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
    }
    document.getElementById("hud-gems").textContent = "GEMS " + state.collected + "/" + state.quota;
    document.getElementById("hud-turns").textContent = "TURNS " + state.used + "/" + state.budget;
    document.getElementById("hud-par").textContent = "PAR " + E.par(state.level);
    document.getElementById("hud-level").textContent = "LV " + (state.levelIndex + 1);
    document.getElementById("hud-seed").textContent = "SEED " + state.seed + (state.seed === DAILY ? "*" : "");
    document.getElementById("btn-next").disabled = state.status !== "won";
    document.getElementById("btn-undo").disabled = !history.length || state.status === "won";
  }

  // --- input -----------------------------------------------------------------
  function nextLevel() { if (state.status === "won") reset(seed, levelIndex + 1); }
  function loadSeedBox() {
    var v = document.getElementById("seedbox").value.trim();
    if (v !== "" && isFinite(+v)) reset(+v, 0);
  }

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
  });
  document.getElementById("btn-ccw").addEventListener("click", function () { act("ccw"); });
  document.getElementById("btn-cw").addEventListener("click", function () { act("cw"); });
  document.getElementById("btn-undo").addEventListener("click", undo);
  document.getElementById("btn-restart").addEventListener("click", function () { reset(seed, levelIndex); });
  document.getElementById("btn-next").addEventListener("click", nextLevel);
  document.getElementById("btn-daily").addEventListener("click", function () { reset(DAILY, 0); });
  document.getElementById("btn-load").addEventListener("click", loadSeedBox);

  // --- test API (harmless for players; used by tools/web-smoke-tiltstone.mjs) --
  window.TILTSTONE = {
    version: E.VERSION,
    engine: E,
    getState: function () { return state; },
    gridString: function () { return E.gridString(state.grid); },
    rotate: function (dir) { act(dir); return state; },
    reset: function (s, l) { reset(s, l || 0); return state; },
    undo: function () { undo(); return state; },
    getUndos: function () { return undos; },
    getHistoryLength: function () { return history.length; },
    dailySeed: DAILY
  };

  render();
})();
