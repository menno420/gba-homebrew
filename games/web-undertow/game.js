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
 * Cosmetics (?skin=ID, C cycles on title/gameover, persisted to
 * localStorage) are PURE RENDER: skins pick diver colors and a bubble-trail
 * draw style only. The sim step, both RNG streams and their draw order are
 * untouched — a run with any skin is byte-identical to the default skin.
 *
 * Ghost replays: every run records its effective steer (-1/0/+1) per frame,
 * run-length encoded; the best run per seed persists to guarded localStorage
 * ("undertow.ghost.<seed>"). On later runs of the SAME seed a translucent
 * ghost diver replays that timeline in lockstep — a SECOND sim instance with
 * its own RNG and its own trench rows, fed the stored inputs. The ghost is
 * render-only presence: it never touches the live run's sim state or RNG
 * streams, so a run is byte-identical with the ghost on or off, and (because
 * the live diver and the ghost execute the SAME physStep) a stored timeline
 * replays to the original run's crash frame and depth exactly. ?ghost=0
 * opts out (render-side only).
 *
 * Oxygen + air pockets (v1.4.0 — a SIM change): the diver carries a tank
 * that drains every frame, faster with depth; hitting zero ends the run like
 * a crash ("out of air"). Air-pocket pickups spawn inside the channel and
 * refill the tank on contact — the reason to leave the safe line. Pockets
 * draw from a SIDE-BAND RNG stream (mulberry32 of seed ^ POCKET_STREAM),
 * one fixed pair of draws per generated row, so the channel layout for a
 * given seed is byte-identical to v1.3.0 — but run OUTCOMES for a seed
 * legitimately change (air can run out before the walls get you). Oxygen,
 * pockets and the collected count all live inside the sim instance, so the
 * ghost replay stays exact by construction. Stored ghost records recorded
 * under the oxygen-free sim are meaningless here: the record version bumped
 * 1 -> 2 and v1 records are dropped cleanly on load (no ghost, no error).
 *
 * Jellyfish hazards (v1.5.0 — a SIM change; the last named growth cut):
 * jellyfish spawn in the channel and drift horizontally on a deterministic
 * sine around their anchor; touching one ends the run like a crash
 * ("STUNG") — the game's grammar is one-touch death, so a sting is a third
 * way to die next to the wall and the empty tank. Jellyfish draw from their
 * OWN side-band RNG stream (mulberry32 of seed ^ JELLY_STREAM), one fixed
 * quadruple of draws per generated row, so both the wall stream and the
 * pocket stream draw exactly what they drew in v1.4.0 — the channel layout
 * AND the pocket layout for a given seed are byte-identical. Run OUTCOMES
 * for seeds where a jellyfish intersects the dive path legitimately change:
 * that is the feature. Drift is a pure function of row data and the sim
 * instance's own step counter (s.t), so the ghost replay stays exact by
 * construction; ghost records bumped 2 -> 3 (pre-jellyfish timelines would
 * replay to a different outcome) and v2 records are dropped cleanly on load.
 *
 * Test hooks: window.UNDERTOW (always exposed). With ?headless=1 the RAF loop
 * does not start — drive the sim with UNDERTOW.stepFrames(n). Keyboard events
 * dispatched programmatically also work headlessly.
 */
(function () {
  "use strict";

  var VERSION = "1.5.0";
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

  // --- oxygen + air pockets (SIM constants) ---------------------------------
  var OXY_MAX = 100;           // tank capacity
  var OXY_DRAIN = 0.11;        // tank drain per frame at the surface
  var OXY_DRAIN_RAMP = 0.0007; // extra drain per frame per meter of depth
  var OXY_REFILL = 40;         // tank gained per air pocket (capped at max)
  var POCKET_R = 9;            // air-pocket pickup radius
  var POCKET_START_ROW = 24;   // first row that may hold a pocket
  var POCKET_CHANCE = 0.10;    // per-row spawn probability
  var POCKET_MARGIN = 15;      // min distance of a pocket center from a wall
  var POCKET_STREAM = 0x1F123BB5; // seed XOR for the pocket side-band RNG

  // --- jellyfish hazards (SIM constants) ------------------------------------
  var JELLY_R = 10;            // jellyfish collision radius
  var JELLY_START_ROW = 40;    // first row that may hold a jellyfish
  var JELLY_CHANCE = 0.07;     // per-row spawn probability
  var JELLY_MARGIN = 18;       // min distance of a jelly ANCHOR from a wall
  var JELLY_AMP_MIN = 10;      // horizontal drift amplitude range (px)
  var JELLY_AMP_MAX = 34;
  var JELLY_FREQ = 0.02;       // drift angular speed (radians per sim step)
  var JELLY_STREAM = 0x2E11F15A; // seed XOR for the jelly side-band RNG

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

  // --- cosmetics: diver skins + bubble-trail styles (PURE RENDER) ----------
  // Skins never touch the sim: they pick fill/stroke colors for the diver
  // and a draw style for the (already deterministic, gameplay-inert)
  // bubbles. Selection happens outside the sim step: at boot (?skin= param,
  // else guarded localStorage) or on a C keypress on title/gameover.
  var SKINS = [
    { id: "classic", body: "#ffd35c", visor: "#04101c", trail: "rgba(200,230,245,0.35)", trailStyle: "bubble" },
    { id: "abyss",   body: "#7ee0c3", visor: "#052018", trail: "rgba(126,224,195,0.45)", trailStyle: "ring"   },
    { id: "ember",   body: "#ff8a5c", visor: "#2a0a04", trail: "rgba(255,176,120,0.40)", trailStyle: "streak" },
    { id: "ghost",   body: "#e4f2fb", visor: "#31465a", trail: "rgba(224,240,252,0.18)", trailStyle: "bubble" }
  ];
  function skinIndexOf(id) {
    for (var i = 0; i < SKINS.length; i++) if (SKINS[i].id === id) return i;
    return -1;
  }
  function loadSkinIndex() {
    // Resolution order: explicit ?skin=ID, else persisted choice, else default.
    var p = params.get("skin");
    var i = (p !== null) ? skinIndexOf(p) : -1;
    if (i !== -1) return i;
    try {
      i = skinIndexOf(window.localStorage.getItem("undertow.skin"));
      if (i !== -1) return i;
    } catch (e) { /* storage unavailable — default skin */ }
    return 0;
  }
  function saveSkin() {
    try { window.localStorage.setItem("undertow.skin", SKINS[skinIndex].id); }
    catch (e) { /* ignore */ }
  }
  var skinIndex = loadSkinIndex();
  function cycleSkin() {
    skinIndex = (skinIndex + 1) % SKINS.length;
    saveSkin();
  }

  // --- ghost replays: best-run input timeline per seed (guarded) -------------
  // Every run records its effective steer per frame, run-length encoded as
  // "<count><l|n|r>" segments (e.g. "120n45l30r"). The best run for a seed
  // persists to localStorage; on later runs of the same seed the stored
  // timeline drives a translucent ghost diver. ?ghost=0 opts out — a
  // render-side switch read once at boot, never by the sim.
  var GHOST_OFF = params.get("ghost") === "0";

  function ghostKey(s) { return "undertow.ghost." + (s >>> 0); }

  function encodeLog(segs) {
    var out = "";
    for (var i = 0; i < segs.length; i++) {
      out += segs[i].n + (segs[i].s === -1 ? "l" : segs[i].s === 1 ? "r" : "n");
    }
    return out;
  }
  function decodeLog(str) {
    var segs = [];
    var re = /(\d+)([lnr])/g;
    var m;
    while ((m = re.exec(str)) !== null) {
      segs.push({ n: +m[1], s: m[2] === "l" ? -1 : m[2] === "r" ? 1 : 0 });
    }
    return segs;
  }

  // Record version 3 = the jellyfish sim (v1.5.0). v2 records were recorded
  // under the jellyfish-free sim (v1.4.0), v1 under the oxygen-free sim
  // (<= v1.3.0): either timeline would replay to a DIFFERENT outcome here
  // (a sting can end the run first), so their depth and crashFrame no longer
  // mean anything — loadGhostRec drops them cleanly (no ghost, no error) and
  // the next crashed run writes a fresh v3 record.
  var GHOST_REC_V = 3;

  var ghostRec = null; // {depth, crashFrame, log} for the current seed, or null
  function loadGhostRec() {
    ghostRec = null;
    try {
      var raw = window.localStorage.getItem(ghostKey(seed));
      if (!raw) return;
      var o = JSON.parse(raw);
      if (o && o.v === GHOST_REC_V && typeof o.log === "string"
          && typeof o.depth === "number" && o.depth > 0
          && typeof o.crashFrame === "number" && o.crashFrame > 0) {
        ghostRec = { depth: Math.floor(o.depth), crashFrame: Math.floor(o.crashFrame), log: o.log };
      }
    } catch (e) { /* storage unavailable or corrupt — no ghost */ }
  }
  function saveGhostRec(rec) {
    try {
      window.localStorage.setItem(ghostKey(seed),
        JSON.stringify({ v: GHOST_REC_V, depth: rec.depth, crashFrame: rec.crashFrame, log: rec.log }));
    } catch (e) { /* ignore — in-memory ghost only */ }
  }
  loadGhostRec();

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

  // A sim instance owns everything one diver's outcome depends on: its own
  // gameplay RNG, its own pocket side-band RNG, its own jelly side-band RNG,
  // its own lazily generated trench rows (walls + air pockets + jellyfish),
  // scroll, x, oxygen, pocket count and step counter (jelly drift phase).
  // The live run and the ghost replay each hold one — they never share state.
  function makeSim() {
    return {
      rng: mulberry32(seed),
      prng: mulberry32((seed ^ POCKET_STREAM) >>> 0), // pockets ONLY — the
      // wall stream above draws exactly what it drew in v1.3.0, so the
      // channel layout for a given seed is unchanged by this feature
      jrng: mulberry32((seed ^ JELLY_STREAM) >>> 0), // jellyfish ONLY — its
      // own side-band, so both streams above draw exactly what they drew
      // in v1.4.0: channel AND pocket layout per seed are unchanged
      rows: [], scrollY: 0, x: W / 2, speed: 0,
      oxy: OXY_MAX, pockets: 0, t: 0
    };
  }

  // A jellyfish's x at sim step t: pure function of its row data and t —
  // no state, no RNG — so the live run, the ghost replay and the renderer
  // all see the same creature in the same place.
  function jellyX(j, t) {
    return j.x0 + j.amp * Math.sin(j.ph + t * JELLY_FREQ);
  }

  function rowIn(s, i) {
    // rows[i] = {c: centerX, h: halfWidth, p: air pocket or null, j:
    // jellyfish or null}; each new row consumes exactly one draw from the
    // instance's OWN gameplay RNG (the wall shape), from POCKET_START_ROW on
    // exactly two draws from the pocket side-band RNG, and from
    // JELLY_START_ROW on exactly four draws from the jelly side-band RNG —
    // always the same count, spawn or not, so every row's value depends only
    // on its index, never on who forced its generation.
    while (s.rows.length <= i) {
      var n = s.rows.length;
      if (n < 12) {
        s.rows.push({ c: W / 2, h: START_HALF, p: null, j: null });
      } else {
        var prev = s.rows[n - 1];
        var half = Math.max(MIN_HALF, START_HALF - n * NARROW_RATE);
        var drift = Math.min(DRIFT_MAX, DRIFT_BASE + n * DRIFT_RAMP);
        var c = prev.c + (s.rng() * 2 - 1) * drift;
        var margin = half + 14;
        if (c < margin) c = margin;
        if (c > W - margin) c = W - margin;
        var row = { c: c, h: half, p: null, j: null };
        if (n >= POCKET_START_ROW) {
          var roll = s.prng();
          var fx = s.prng();
          var span = half - POCKET_MARGIN;
          if (roll < POCKET_CHANCE && span > 0) {
            row.p = { x: c - span + fx * span * 2, taken: false };
          }
        }
        if (n >= JELLY_START_ROW) {
          // always four draws — spawn or not — so the jelly stream stays
          // index-locked and never skews itself
          var jroll = s.jrng();
          var jfx = s.jrng();
          var jfa = s.jrng();
          var jfp = s.jrng();
          var jspan = half - JELLY_MARGIN;
          if (jroll < JELLY_CHANCE && jspan > 0) {
            row.j = {
              x0: c - jspan + jfx * jspan * 2,                       // anchor
              amp: JELLY_AMP_MIN + jfa * (JELLY_AMP_MAX - JELLY_AMP_MIN),
              ph: jfp * Math.PI * 2                                  // phase
            };
          }
        }
        s.rows.push(row);
      }
    }
    return s.rows[i];
  }

  // One fixed-timestep physics step for a sim instance under a steer input
  // (-1 left, 0 none, +1 right). Returns "wall", "sting" or "air" when the
  // diver's run ended this frame (same-frame ties resolve in that order),
  // null otherwise — all truthy/falsy compatible with the old boolean. This
  // is THE step: the live run and the ghost replay execute this same code,
  // so a recorded input timeline replays to the identical crash frame,
  // depth and cause by construction (oxygen, pockets and the jelly drift
  // clock all live inside the sim instance).
  function physStep(s, steer) {
    s.t++; // the sim's own step counter — the jelly drift clock
    var meters = Math.floor(s.scrollY / PX_PER_METER);
    s.speed = Math.min(MAX_SPEED, BASE_SPEED + meters * SPEED_RAMP);
    s.scrollY += s.speed;
    if (steer === -1) s.x -= STEER_SPEED;
    else if (steer === 1) s.x += STEER_SPEED;
    if (s.x < PLAYER_R) s.x = PLAYER_R;
    if (s.x > W - PLAYER_R) s.x = W - PLAYER_R;
    var ri = Math.floor((s.scrollY + PLAYER_Y) / ROW_H);
    // air pockets: contact with an untaken pocket in the diver's row band
    // refills the tank (never alters motion — pickups are pure oxygen)
    var wy = s.scrollY + PLAYER_Y;
    var rr = (PLAYER_R + POCKET_R) * (PLAYER_R + POCKET_R);
    for (var i = (ri > 0 ? ri - 1 : 0); i <= ri + 1; i++) {
      var q = rowIn(s, i);
      if (q.p && !q.p.taken) {
        var dx = s.x - q.p.x;
        var dy = wy - (i * ROW_H + ROW_H / 2);
        if (dx * dx + dy * dy <= rr) {
          q.p.taken = true;
          s.pockets++;
          s.oxy = Math.min(OXY_MAX, s.oxy + OXY_REFILL);
        }
      }
    }
    // the tank drains every frame, faster with depth (pressure)
    s.oxy -= OXY_DRAIN + meters * OXY_DRAIN_RAMP;
    var r = rowIn(s, ri);
    if (s.x - PLAYER_R < r.c - r.h || s.x + PLAYER_R > r.c + r.h) return "wall";
    // jellyfish: contact with a drifting jelly in the diver's row band ends
    // the run — one-touch death, same grammar as the walls
    var jr = (PLAYER_R + JELLY_R) * (PLAYER_R + JELLY_R);
    for (var k = (ri > 0 ? ri - 1 : 0); k <= ri + 1; k++) {
      var w = rowIn(s, k);
      if (w.j) {
        var jdx = s.x - jellyX(w.j, s.t);
        var jdy = wy - (k * ROW_H + ROW_H / 2);
        if (jdx * jdx + jdy * jdy <= jr) return "sting";
      }
    }
    if (s.oxy <= 0) { s.oxy = 0; return "air"; }
    return null;
  }

  var sim = makeSim();          // the live run
  var vrng = mulberry32(seed ^ 0x9E3779B9); // visual-only RNG (bubbles)
  var input = { left: false, right: false };
  var frame = 0;                // frames elapsed in the current run
  var crashFrame = -1;
  var crashCause = null;        // "wall" | "air" once the run ended
  var bubbles = [];

  function steerOf(inp) {
    return (inp.left && !inp.right) ? -1 : (inp.right && !inp.left) ? 1 : 0;
  }

  // input recording: RLE segments of the per-frame steer, every run
  var recSegs = [];
  var recLast = null;
  function recPush(steer) {
    if (recLast !== null && recLast.s === steer) recLast.n++;
    else { recLast = { s: steer, n: 1 }; recSegs.push(recLast); }
  }

  // the active ghost replay (null: nothing stored, opted out, or on title)
  var ghost = null;
  function makeGhostRun(rec) {
    return { sim: makeSim(), segs: decodeLog(rec.log), si: 0, frame: 0, crashed: false, rec: rec };
  }
  function stepGhost(g) {
    // next steer from the RLE log (0 once exhausted); pure physics, no guard
    var steer = 0;
    if (g.si < g.segs.length) {
      steer = g.segs[g.si].s;
      if (--g.segs[g.si].n === 0) g.si++;
    }
    g.frame++;
    if (physStep(g.sim, steer)) g.crashed = true;
  }

  function resetWorld() {
    sim = makeSim();
    vrng = mulberry32(seed ^ 0x9E3779B9);
    frame = 0;
    crashFrame = -1;
    crashCause = null;
    bubbles = [];
    input.left = false;
    input.right = false;
    recSegs = [];
    recLast = null;
    ghost = null;
  }

  function startRun() {
    resetWorld();
    shareMsg = "";
    if (!GHOST_OFF && ghostRec !== null) ghost = makeGhostRun(ghostRec);
    state = "playing";
  }

  function depthMeters() { return Math.floor(sim.scrollY / PX_PER_METER); }
  function depthRows() { return Math.floor(sim.scrollY / ROW_H); }

  // --- fixed-timestep simulation --------------------------------------------
  function update() {
    if (state !== "playing") return;
    frame++;

    var steer = steerOf(input);
    var crashed = physStep(sim, steer);
    recPush(steer);

    // visual bubbles (deterministic, gameplay-inert)
    if (frame % 9 === 0) {
      bubbles.push({ x: sim.x + (vrng() * 2 - 1) * 10, y: PLAYER_Y + 6, r: 1 + vrng() * 2.5 });
    }
    for (var i = bubbles.length - 1; i >= 0; i--) {
      bubbles[i].y -= 1.4 + sim.speed * 0.4;
      if (bubbles[i].y < -8) bubbles.splice(i, 1);
    }

    // ghost replay: lockstep step of its OWN sim instance — zero contact
    // with the live run's state or RNG streams
    if (ghost !== null && !ghost.crashed) {
      stepGhost(ghost);
      // corrupt-storage guard: freeze a ghost that outlives its record
      if (!ghost.crashed && ghost.frame > ghost.rec.crashFrame) ghost.crashed = true;
    }

    if (crashed) {
      crashFrame = frame;
      crashCause = crashed; // "wall" | "air"
      state = "gameover";
      var score = depthMeters();
      if (score > best) { best = score; saveBest(); }
      // persist this run's timeline as the seed's ghost when it is the best
      if (score > 0 && (ghostRec === null || score > ghostRec.depth)) {
        ghostRec = { depth: score, crashFrame: frame, log: encodeLog(recSegs) };
        saveGhostRec(ghostRec);
      }
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
    var firstRow = Math.floor(sim.scrollY / ROW_H);
    var offset = sim.scrollY % ROW_H;
    ctx.fillStyle = shade("#123048", dark * 0.6);
    for (var i = 0; i <= H / ROW_H + 1; i++) {
      var r = rowIn(sim, firstRow + i);
      var y = i * ROW_H - offset;
      ctx.fillRect(0, y, r.c - r.h, ROW_H + 1);
      ctx.fillRect(r.c + r.h, y, W - (r.c + r.h), ROW_H + 1);
      // subtle wall texture: deterministic per-row notch
      var notch = ((firstRow + i) * 2654435761 >>> 0) % 11;
      ctx.fillStyle = shade("#0d2436", dark * 0.6);
      ctx.fillRect(r.c - r.h - 6 - notch, y + 4, 6 + notch, ROW_H - 8);
      ctx.fillRect(r.c + r.h, y + 4, 6 + notch, ROW_H - 8);
      // air pocket (untaken): a bright pickup bubble in the channel —
      // drawn from sim row data, position/existence never depends on render
      if (r.p && !r.p.taken) {
        var py = y + ROW_H / 2;
        ctx.fillStyle = "rgba(214,240,255,0.85)";
        ctx.beginPath();
        ctx.arc(r.p.x, py, POCKET_R, 0, Math.PI * 2);
        ctx.fill();
        ctx.fillStyle = "rgba(120,190,230,0.9)";
        ctx.beginPath();
        ctx.arc(r.p.x - 3, py - 3, 2.4, 0, Math.PI * 2);
        ctx.fill();
      }
      // jellyfish: dome + tentacles at the sim-truth drift position — the
      // renderer READS jellyX(row, sim.t); it never owns creature state
      if (r.j) {
        var jx = jellyX(r.j, sim.t);
        var jy = y + ROW_H / 2;
        var pulse = 0.55 + 0.2 * Math.sin(r.j.ph + sim.t * JELLY_FREQ * 3);
        ctx.fillStyle = "rgba(233,150,222," + pulse.toFixed(3) + ")";
        ctx.beginPath();
        ctx.arc(jx, jy - 2, JELLY_R - 1, Math.PI, 0);
        ctx.fill();
        ctx.strokeStyle = "rgba(233,150,222,0.55)";
        ctx.lineWidth = 1.4;
        for (var tn = -1; tn <= 1; tn++) {
          ctx.beginPath();
          ctx.moveTo(jx + tn * 5, jy - 2);
          ctx.lineTo(jx + tn * 6, jy + 7);
          ctx.stroke();
        }
      }
      ctx.fillStyle = shade("#123048", dark * 0.6);
    }

    // bubbles — trail STYLE is cosmetic; positions/sizes come from the sim
    // state unchanged, so every skin draws the same underlying particles.
    var skin = SKINS[skinIndex];
    for (var b = 0; b < bubbles.length; b++) {
      var bb = bubbles[b];
      ctx.beginPath();
      if (skin.trailStyle === "ring") {
        ctx.strokeStyle = skin.trail;
        ctx.lineWidth = 1.2;
        ctx.arc(bb.x, bb.y, bb.r + 0.8, 0, Math.PI * 2);
        ctx.stroke();
      } else if (skin.trailStyle === "streak") {
        ctx.fillStyle = skin.trail;
        ctx.ellipse(bb.x, bb.y, bb.r * 0.55, bb.r * 1.7, 0, 0, Math.PI * 2);
        ctx.fill();
      } else { // "bubble"
        ctx.fillStyle = skin.trail;
        ctx.arc(bb.x, bb.y, bb.r, 0, Math.PI * 2);
        ctx.fill();
      }
    }

    // ghost diver (translucent, render-only presence) — drawn under the live
    // diver. Its screen y is its own world position relative to the live
    // scroll: overlapping while both dive, drifting up once the ghost crashed.
    if (ghost !== null && state !== "title") {
      var gy = PLAYER_Y + (ghost.sim.scrollY - sim.scrollY);
      if (gy > -PLAYER_R - 4 && gy < H + PLAYER_R + 4) {
        ctx.globalAlpha = 0.32;
        ctx.fillStyle = "#cfe8f5";
        ctx.beginPath();
        ctx.arc(ghost.sim.x, gy, PLAYER_R, 0, Math.PI * 2);
        ctx.fill();
        ctx.fillStyle = "#31465a";
        ctx.fillRect(ghost.sim.x - 3, gy - 2, 6, 3); // visor
        ctx.globalAlpha = 1;
      }
    }

    // diver
    if (state !== "title") {
      ctx.fillStyle = skin.body;
      ctx.beginPath();
      ctx.arc(sim.x, PLAYER_Y, PLAYER_R, 0, Math.PI * 2);
      ctx.fill();
      ctx.fillStyle = skin.visor;
      ctx.fillRect(sim.x - 3, PLAYER_Y - 2, 6, 3); // visor
    }

    // HUD
    ctx.fillStyle = "#cfe8f5";
    ctx.font = "16px 'Courier New', monospace";
    ctx.textAlign = "left";
    ctx.fillText(meters + " m", 10, 24);
    ctx.textAlign = "right";
    ctx.fillText("best " + best + " m", W - 10, 24);
    // oxygen meter (top center): drains as you dive, refilled by air pockets
    if (state !== "title") {
      var ow = 130, oh = 9, oxx = (W - ow) / 2, oyy = 15;
      var frac = Math.max(0, Math.min(1, sim.oxy / OXY_MAX));
      ctx.strokeStyle = "#5f8aa3";
      ctx.lineWidth = 1;
      ctx.strokeRect(oxx - 0.5, oyy - 0.5, ow + 1, oh + 1);
      ctx.fillStyle = frac < 0.25 ? "#ff7a6b" : "#7ee0c3";
      ctx.fillRect(oxx, oyy, ow * frac, oh);
      ctx.fillStyle = "#5f8aa3";
      ctx.font = "10px 'Courier New', monospace";
      ctx.textAlign = "center";
      ctx.fillText("air", W / 2, oyy + oh + 12);
      ctx.font = "16px 'Courier New', monospace";
      ctx.textAlign = "right";
      ctx.fillStyle = "#cfe8f5";
    }
    if (ghost !== null && state === "playing") {
      ctx.fillStyle = "#5f8aa3";
      ctx.font = "12px 'Courier New', monospace";
      ctx.fillText("ghost " + ghost.rec.depth + " m", W - 10, 42);
      ctx.font = "16px 'Courier New', monospace";
    }

    ctx.textAlign = "center";
    if (state === "title") {
      ctx.font = "bold 36px 'Courier New', monospace";
      ctx.fillText("UNDERTOW", W / 2, 240);
      ctx.font = "16px 'Courier New', monospace";
      ctx.fillText("dive the trench — don't touch the walls", W / 2, 290);
      ctx.fillText("grab air pockets before your tank runs dry", W / 2, 312);
      ctx.fillText("dodge the jellyfish — one sting ends the dive", W / 2, 334);
      ctx.fillText("space / enter / tap to dive", W / 2, 358);
      ctx.fillStyle = "#5f8aa3";
      ctx.fillText((DAILY ? "daily dive " : "seed ") + seed, W / 2, 384);
      if (CHALLENGE_DEPTH > 0) {
        ctx.fillStyle = "#ffd35c";
        ctx.fillText("challenge: beat " + CHALLENGE_DEPTH + " m", W / 2, 408);
      }
      // skin preview: swatch diver + label, C cycles
      ctx.fillStyle = skin.body;
      ctx.beginPath();
      ctx.arc(W / 2 - 78, 435, PLAYER_R, 0, Math.PI * 2);
      ctx.fill();
      ctx.fillStyle = skin.visor;
      ctx.fillRect(W / 2 - 81, 433, 6, 3);
      ctx.fillStyle = "#5f8aa3";
      ctx.textAlign = "left";
      ctx.fillText("skin: " + skin.id + " — C to change", W / 2 - 62, 440);
      ctx.textAlign = "center";
      if (!GHOST_OFF && ghostRec !== null) {
        ctx.fillStyle = "#7ea8c0";
        ctx.fillText("ghost: your best " + ghostRec.depth + " m dives with you", W / 2, 472);
      }
    } else if (state === "gameover") {
      ctx.fillStyle = "rgba(4,16,28,0.7)";
      ctx.fillRect(0, 200, W, 200);
      ctx.fillStyle = "#ff7a6b";
      ctx.font = "bold 30px 'Courier New', monospace";
      ctx.fillText((crashCause === "air" ? "OUT OF AIR AT "
        : crashCause === "sting" ? "STUNG AT "
        : "CRUSHED AT ") + meters + " m", W / 2, 270);
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
    if ((e.code === "KeyC" || e.key === "c" || e.key === "C") && (state === "title" || state === "gameover")) cycleSkin();
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
    getCrashCause: function () { return crashCause; }, // "wall" | "sting" | "air" | null
    getOxygen: function () {
      return { oxygen: sim.oxy, max: OXY_MAX, pockets: sim.pockets };
    },
    getPocketProbe: function (lookRows) {
      // Test/driver hook: the nearest untaken pocket at or below the diver
      // within lookRows rows (default 40). Probing may force row generation,
      // but rows depend only on their index, so it never perturbs the sim.
      var start = Math.floor((sim.scrollY + PLAYER_Y) / ROW_H);
      var look = (lookRows && lookRows > 0) ? Math.floor(lookRows) : 40;
      for (var i = start; i <= start + look; i++) {
        var r = rowIn(sim, i);
        if (r.p && !r.p.taken) {
          return {
            x: r.p.x,
            row: i,
            dy: (i * ROW_H + ROW_H / 2) - (sim.scrollY + PLAYER_Y),
            diverX: sim.x
          };
        }
      }
      return null;
    },
    getJellies: function (fromRow, count) {
      // Test/driver hook: every jellyfish in rows [fromRow, fromRow+count)
      // with its CURRENT drift position at the live sim's step counter.
      // Probing may force row generation, but rows depend only on their
      // index, so it never perturbs the sim.
      var out = [];
      var start = Math.floor(fromRow) || 0;
      var n = (count && count > 0) ? Math.floor(count) : 40;
      for (var i = start; i < start + n; i++) {
        var r = rowIn(sim, i);
        if (r.j) {
          out.push({ row: i, x: jellyX(r.j, sim.t), x0: r.j.x0, amp: r.j.amp, t: sim.t });
        }
      }
      return out;
    },
    getJellyProbe: function (lookRows) {
      // Test/driver hook: the nearest jellyfish at or below the diver
      // within lookRows rows (default 40), at its current drift position.
      var start = Math.floor((sim.scrollY + PLAYER_Y) / ROW_H);
      var look = (lookRows && lookRows > 0) ? Math.floor(lookRows) : 40;
      for (var i = start; i <= start + look; i++) {
        var r = rowIn(sim, i);
        if (r.j) {
          return {
            x: jellyX(r.j, sim.t),
            row: i,
            dy: (i * ROW_H + ROW_H / 2) - (sim.scrollY + PLAYER_Y),
            diverX: sim.x
          };
        }
      }
      return null;
    },
    getSkin: function () { var s = SKINS[skinIndex]; return { id: s.id, trailStyle: s.trailStyle, index: skinIndex }; },
    getSkinIds: function () { return SKINS.map(function (s) { return s.id; }); },
    getGhostInfo: function () {
      return {
        enabled: !GHOST_OFF,
        stored: ghostRec !== null ? { depth: ghostRec.depth, crashFrame: ghostRec.crashFrame } : null,
        active: ghost !== null
      };
    },
    getGhostRun: function () {
      if (ghost === null) return null;
      return {
        frame: ghost.frame,
        crashed: ghost.crashed,
        depth: Math.floor(ghost.sim.scrollY / PX_PER_METER),
        x: ghost.sim.x,
        recDepth: ghost.rec.depth,
        recCrashFrame: ghost.rec.crashFrame
      };
    },
    verifyGhost: function (maxFrames) {
      // Pure replay-fidelity probe: replays the STORED timeline through a
      // fresh sim instance (natural physics only — no freeze guard) and
      // reports where it crashed. Never touches the live run.
      if (ghostRec === null) return null;
      var g = makeGhostRun(ghostRec);
      var cap = (maxFrames && maxFrames > 0) ? maxFrames : 36000;
      while (!g.crashed && g.frame < cap) stepGhost(g);
      return {
        crashed: g.crashed,
        frame: g.frame,
        depth: Math.floor(g.sim.scrollY / PX_PER_METER),
        recDepth: ghostRec.depth,
        recCrashFrame: ghostRec.crashFrame
      };
    },
    clearGhost: function () {
      ghost = null;
      ghostRec = null;
      try { window.localStorage.removeItem(ghostKey(seed)); } catch (e) { /* ignore */ }
    },
    stepFrames: function (n) {
      for (var i = 0; i < n; i++) update();
      render(); // rendering is gameplay-inert; keeps headless screenshots useful
      return { state: state, frame: frame, score: depthMeters(), crashFrame: crashFrame, cause: crashCause };
    },
    reset: function (newSeed) {
      if (newSeed !== undefined && newSeed !== null && !isNaN(+newSeed)) {
        seed = (+newSeed) >>> 0;
        loadGhostRec(); // ghost records are keyed by seed
      }
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
