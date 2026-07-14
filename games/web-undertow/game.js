/* Undertow — vertical trench-diver arcade prototype.
 *
 * Zero dependencies. Deterministic: all gameplay randomness flows through a
 * seeded mulberry32 RNG (seed via ?seed=N; ?daily=1 derives the seed from the
 * UTC date as YYYYMMDD; else derived from load timing). The date is read ONCE
 * at boot to pick the seed — never inside the sim step. A run's challenge
 * link (?seed=N&depth=M) is copyable from the gameover screen; the depth
 * param is render-only (a score to beat on the title screen).
 * Fixed-timestep simulation (60 updates/sec) driven by requestAnimationFrame
 * through an accumulator; the sim step never reads the wall clock.
 *
 * Test hooks: window.UNDERTOW (always exposed). With ?headless=1 the RAF loop
 * does not start — drive the sim with UNDERTOW.stepFrames(n). Keyboard events
 * dispatched programmatically also work headlessly.
 */
(function () {
  "use strict";

  var VERSION = "1.1.0";
  var W = 480, H = 640;
  var ROW_H = 16;              // pixels per trench row
  var PLAYER_Y = 150;          // fixed screen y of the diver
  var PLAYER_R = 7;            // collision radius
  var STEER_SPEED = 3.2;       // px per frame of horizontal steering
  var BASE_SPEED = 2.0;        // px per frame descent at surface
  var MAX_SPEED = 5.0;         // descent speed clamp
  var SPEED_RAMP = 0.004;      // extra px/frame per meter of depth
  var START_HALF = 110;        // starting channel half-width
  var MIN_HALF = 46;           // channel half-width clamp
  var NARROW_RATE = 0.055;     // half-width lost per row
  var DRIFT_BASE = 10;         // max center drift per row (px)
  var DRIFT_RAMP = 0.012;      // drift growth per row
  var DRIFT_MAX = 26;
  var PX_PER_METER = 10;       // depth scale for score

  // --- seeded RNG (mulberry32) ---------------------------------------------
  function mulberry32(a) {
    a = a >>> 0;
    return function () {
      a |= 0; a = (a + 0x6D2B79F5) | 0;
      var t = Math.imul(a ^ (a >>> 15), 1 | a);
      t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
      return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
    };
  }

  function defaultSeed() {
    // Non-crypto default; only ever read OUTSIDE the sim step.
    var t = (typeof performance !== "undefined" && performance.now)
      ? performance.now() : 0;
    return (Math.floor(t * 1000) ^ Date.now()) >>> 0;
  }

  function dailySeed(d) {
    // UTC calendar date -> YYYYMMDD as a number (e.g. 20260714). Everyone on
    // the planet gets the same trench on the same UTC day. Only ever read
    // OUTSIDE the sim step (seed selection at boot).
    return (d.getUTCFullYear() * 10000 + (d.getUTCMonth() + 1) * 100 + d.getUTCDate()) >>> 0;
  }

  var params = (function () {
    try { return new URLSearchParams(window.location.search); }
    catch (e) { return { get: function () { return null; } }; }
  })();
  var HEADLESS = params.get("headless") === "1";
  var seedParam = params.get("seed");
  var hasSeedParam = (seedParam !== null && seedParam !== "" && !isNaN(+seedParam));
  // ?daily=1 — today's dive: seed = UTC date as YYYYMMDD. An explicit ?seed=N
  // wins over it: a shared challenge link must replay the SAME run whenever
  // (and wherever) it is opened, so share links always pin the seed.
  var DAILY = !hasSeedParam && params.get("daily") === "1";
  var seed = hasSeedParam ? (+seedParam >>> 0)
    : DAILY ? dailySeed(new Date())
    : defaultSeed();

  // ?depth=M — render-only challenge target carried by share links: shown on
  // the title screen as a score to beat. Never read by the sim.
  var depthParam = params.get("depth");
  var CHALLENGE_DEPTH = (depthParam !== null && depthParam !== "" && !isNaN(+depthParam) && +depthParam > 0)
    ? Math.floor(+depthParam) : 0;

  // --- persistent best score (guarded) --------------------------------------
  var best = 0;
  function loadBest() {
    try {
      var v = window.localStorage.getItem("undertow.best");
      if (v !== null && !isNaN(+v)) best = +v;
    } catch (e) { /* storage unavailable — in-memory best only */ }
  }
  function saveBest() {
    try { window.localStorage.setItem("undertow.best", String(best)); }
    catch (e) { /* ignore */ }
  }
  loadBest();

  // --- shareable challenge URL ------------------------------------------------
  var shareMsg = "";            // gameover share feedback ("" until S is pressed)

  function shareURL(score) {
    // Same page, query replaced: seed pinned explicitly (never ?daily=1, so
    // the link replays this exact run on any day), plus the depth to beat.
    var base = "";
    try { base = window.location.href.split("#")[0].split("?")[0]; }
    catch (e) { /* no location (tests) — relative link */ }
    var q = "?seed=" + seed;
    if (score !== undefined && score !== null && !isNaN(+score) && +score > 0) {
      q += "&depth=" + Math.floor(+score);
    }
    return base + q;
  }

  function copyShare() {
    var url = shareURL(depthMeters());
    var done = function () { shareMsg = "challenge link copied"; };
    var fail = function () { shareMsg = url; }; // no clipboard — show the link
    try {
      if (navigator.clipboard && navigator.clipboard.writeText) {
        navigator.clipboard.writeText(url).then(done, fail);
      } else fail();
    } catch (e) { fail(); }
  }

  // --- game state ------------------------------------------------------------
  var state = "title";          // title | playing | gameover
  var rng = mulberry32(seed);   // gameplay RNG
  var vrng = mulberry32(seed ^ 0x9E3779B9); // visual-only RNG (bubbles)
  var rows = [];                // rows[i] = {c: centerX, h: halfWidth}
  var scrollY = 0;              // world pixels scrolled past the top
  var px = W / 2;               // player x
  var input = { left: false, right: false };
  var frame = 0;                // frames elapsed in the current run
  var crashFrame = -1;
  var bubbles = [];

  function rowAt(i) {
    while (rows.length <= i) {
      var n = rows.length;
      if (n < 12) {
        rows.push({ c: W / 2, h: START_HALF });
      } else {
        var prev = rows[n - 1];
        var half = Math.max(MIN_HALF, START_HALF - n * NARROW_RATE);
        var drift = Math.min(DRIFT_MAX, DRIFT_BASE + n * DRIFT_RAMP);
        var c = prev.c + (rng() * 2 - 1) * drift;
        var margin = half + 14;
        if (c < margin) c = margin;
        if (c > W - margin) c = W - margin;
        rows.push({ c: c, h: half });
      }
    }
    return rows[i];
  }

  function resetWorld() {
    rng = mulberry32(seed);
    vrng = mulberry32(seed ^ 0x9E3779B9);
    rows = [];
    scrollY = 0;
    px = W / 2;
    frame = 0;
    crashFrame = -1;
    bubbles = [];
    input.left = false;
    input.right = false;
  }

  function startRun() {
    resetWorld();
    shareMsg = "";
    state = "playing";
  }

  function depthMeters() { return Math.floor(scrollY / PX_PER_METER); }
  function depthRows() { return Math.floor(scrollY / ROW_H); }

  // --- fixed-timestep simulation --------------------------------------------
  function update() {
    if (state !== "playing") return;
    frame++;

    var meters = depthMeters();
    var speed = Math.min(MAX_SPEED, BASE_SPEED + meters * SPEED_RAMP);
    scrollY += speed;

    if (input.left && !input.right) px -= STEER_SPEED;
    else if (input.right && !input.left) px += STEER_SPEED;
    if (px < PLAYER_R) px = PLAYER_R;
    if (px > W - PLAYER_R) px = W - PLAYER_R;

    // visual bubbles (deterministic, gameplay-inert)
    if (frame % 9 === 0) {
      bubbles.push({ x: px + (vrng() * 2 - 1) * 10, y: PLAYER_Y + 6, r: 1 + vrng() * 2.5 });
    }
    for (var i = bubbles.length - 1; i >= 0; i--) {
      bubbles[i].y -= 1.4 + speed * 0.4;
      if (bubbles[i].y < -8) bubbles.splice(i, 1);
    }

    // collision against the row under the diver
    var r = rowAt(Math.floor((scrollY + PLAYER_Y) / ROW_H));
    if (px - PLAYER_R < r.c - r.h || px + PLAYER_R > r.c + r.h) {
      crashFrame = frame;
      state = "gameover";
      var score = depthMeters();
      if (score > best) { best = score; saveBest(); }
    }
  }

  // --- rendering --------------------------------------------------------------
  var canvas = document.getElementById("game");
  var ctx = canvas ? canvas.getContext("2d") : null;

  function render() {
    if (!ctx) return;
    var meters = depthMeters();
    var dark = Math.min(0.72, meters / 900);

    var g = ctx.createLinearGradient(0, 0, 0, H);
    g.addColorStop(0, shade("#0b3d5c", dark));
    g.addColorStop(1, shade("#041828", dark));
    ctx.fillStyle = g;
    ctx.fillRect(0, 0, W, H);

    // trench walls
    var firstRow = Math.floor(scrollY / ROW_H);
    var offset = scrollY % ROW_H;
    ctx.fillStyle = shade("#123048", dark * 0.6);
    for (var i = 0; i <= H / ROW_H + 1; i++) {
      var r = rowAt(firstRow + i);
      var y = i * ROW_H - offset;
      ctx.fillRect(0, y, r.c - r.h, ROW_H + 1);
      ctx.fillRect(r.c + r.h, y, W - (r.c + r.h), ROW_H + 1);
      // subtle wall texture: deterministic per-row notch
      var notch = ((firstRow + i) * 2654435761 >>> 0) % 11;
      ctx.fillStyle = shade("#0d2436", dark * 0.6);
      ctx.fillRect(r.c - r.h - 6 - notch, y + 4, 6 + notch, ROW_H - 8);
      ctx.fillRect(r.c + r.h, y + 4, 6 + notch, ROW_H - 8);
      ctx.fillStyle = shade("#123048", dark * 0.6);
    }

    // bubbles
    ctx.fillStyle = "rgba(200,230,245,0.35)";
    for (var b = 0; b < bubbles.length; b++) {
      ctx.beginPath();
      ctx.arc(bubbles[b].x, bubbles[b].y, bubbles[b].r, 0, Math.PI * 2);
      ctx.fill();
    }

    // diver
    if (state !== "title") {
      ctx.fillStyle = "#ffd35c";
      ctx.beginPath();
      ctx.arc(px, PLAYER_Y, PLAYER_R, 0, Math.PI * 2);
      ctx.fill();
      ctx.fillStyle = "#04101c";
      ctx.fillRect(px - 3, PLAYER_Y - 2, 6, 3); // visor
    }

    // HUD
    ctx.fillStyle = "#cfe8f5";
    ctx.font = "16px 'Courier New', monospace";
    ctx.textAlign = "left";
    ctx.fillText(meters + " m", 10, 24);
    ctx.textAlign = "right";
    ctx.fillText("best " + best + " m", W - 10, 24);

    ctx.textAlign = "center";
    if (state === "title") {
      ctx.font = "bold 36px 'Courier New', monospace";
      ctx.fillText("UNDERTOW", W / 2, 240);
      ctx.font = "16px 'Courier New', monospace";
      ctx.fillText("dive the trench — don't touch the walls", W / 2, 290);
      ctx.fillText("space / enter / tap to dive", W / 2, 330);
      ctx.fillStyle = "#5f8aa3";
      ctx.fillText((DAILY ? "daily dive " : "seed ") + seed, W / 2, 370);
      if (CHALLENGE_DEPTH > 0) {
        ctx.fillStyle = "#ffd35c";
        ctx.fillText("challenge: beat " + CHALLENGE_DEPTH + " m", W / 2, 400);
      }
    } else if (state === "gameover") {
      ctx.fillStyle = "rgba(4,16,28,0.7)";
      ctx.fillRect(0, 200, W, 200);
      ctx.fillStyle = "#ff7a6b";
      ctx.font = "bold 30px 'Courier New', monospace";
      ctx.fillText("CRUSHED AT " + meters + " m", W / 2, 270);
      ctx.fillStyle = "#cfe8f5";
      ctx.font = "16px 'Courier New', monospace";
      ctx.fillText("best " + best + " m", W / 2, 310);
      ctx.fillText("space / enter / tap to dive again", W / 2, 350);
      ctx.fillStyle = "#5f8aa3";
      ctx.font = "13px 'Courier New', monospace";
      ctx.fillText(shareMsg !== "" ? shareMsg : "S copies your challenge link", W / 2, 382);
    }
  }

  function shade(hex, amount) {
    // darken a #rrggbb color by amount (0..1)
    var n = parseInt(hex.slice(1), 16);
    var r = Math.round(((n >> 16) & 255) * (1 - amount));
    var g = Math.round(((n >> 8) & 255) * (1 - amount));
    var b = Math.round((n & 255) * (1 - amount));
    return "rgb(" + r + "," + g + "," + b + ")";
  }

  // --- input -------------------------------------------------------------------
  function isStartKey(e) {
    return e.code === "Space" || e.key === " " || e.code === "Enter" || e.key === "Enter";
  }
  function onKeyDown(e) {
    if (e.code === "ArrowLeft" || e.key === "ArrowLeft" || e.code === "KeyA" || e.key === "a" || e.key === "A") input.left = true;
    if (e.code === "ArrowRight" || e.key === "ArrowRight" || e.code === "KeyD" || e.key === "d" || e.key === "D") input.right = true;
    if ((e.code === "KeyS" || e.key === "s" || e.key === "S") && state === "gameover") copyShare();
    if (isStartKey(e) && (state === "title" || state === "gameover")) startRun();
    if (e.preventDefault) e.preventDefault();
  }
  function onKeyUp(e) {
    if (e.code === "ArrowLeft" || e.key === "ArrowLeft" || e.code === "KeyA" || e.key === "a" || e.key === "A") input.left = false;
    if (e.code === "ArrowRight" || e.key === "ArrowRight" || e.code === "KeyD" || e.key === "d" || e.key === "D") input.right = false;
  }
  window.addEventListener("keydown", onKeyDown);
  window.addEventListener("keyup", onKeyUp);

  function touchSide(e) {
    var t = e.touches && e.touches[0];
    if (!t || !canvas) return null;
    var rect = canvas.getBoundingClientRect();
    return (t.clientX - rect.left) < rect.width / 2 ? "left" : "right";
  }
  if (canvas) {
    canvas.addEventListener("touchstart", function (e) {
      e.preventDefault();
      if (state === "title" || state === "gameover") { startRun(); return; }
      var side = touchSide(e);
      if (side === "left") { input.left = true; input.right = false; }
      else if (side === "right") { input.right = true; input.left = false; }
    }, { passive: false });
    canvas.addEventListener("touchend", function (e) {
      e.preventDefault();
      input.left = false; input.right = false;
    }, { passive: false });
    canvas.addEventListener("mousedown", function () {
      if (state === "title" || state === "gameover") startRun();
    });
  }

  // --- public API ---------------------------------------------------------------
  window.UNDERTOW = {
    version: VERSION,
    getState: function () { return state; },
    getScore: function () { return depthMeters(); },
    getDepthRows: function () { return depthRows(); },
    getSeed: function () { return seed; },
    getMode: function () { return DAILY ? "daily" : (hasSeedParam ? "seeded" : "free"); },
    getChallengeDepth: function () { return CHALLENGE_DEPTH; },
    getShareURL: function () { return shareURL(depthMeters()); },
    getShareFeedback: function () { return shareMsg; },
    dailySeedFor: function (d) { return dailySeed(d instanceof Date ? d : new Date(d)); },
    getCrashFrame: function () { return crashFrame; },
    stepFrames: function (n) {
      for (var i = 0; i < n; i++) update();
      render(); // rendering is gameplay-inert; keeps headless screenshots useful
      return { state: state, frame: frame, score: depthMeters(), crashFrame: crashFrame };
    },
    reset: function (newSeed) {
      if (newSeed !== undefined && newSeed !== null && !isNaN(+newSeed)) seed = (+newSeed) >>> 0;
      resetWorld();
      state = "title";
    },
    setInput: function (o) {
      if (o && typeof o === "object") {
        if ("left" in o) input.left = !!o.left;
        if ("right" in o) input.right = !!o.right;
      }
    }
  };

  // --- main loop (fixed timestep over RAF) ---------------------------------------
  var STEP_MS = 1000 / 60;
  var acc = 0;
  var last = null;
  function loop(now) {
    if (last === null) last = now;
    var dt = now - last;
    last = now;
    if (dt > 250) dt = 250; // tab-switch clamp
    acc += dt;
    while (acc >= STEP_MS) {
      update();
      acc -= STEP_MS;
    }
    render();
    requestAnimationFrame(loop);
  }

  if (!HEADLESS) {
    requestAnimationFrame(loop);
  } else {
    render(); // one static frame so screenshots show something
  }
})();
