// Drift Garden — playable slice on the mobile foundation (original IP).
// Plain ES module, zero dependencies. Fixed-timestep simulation +
// requestAnimationFrame rendering, touch-first input, visibility
// pause/resume, and an observable state surface for headless proofs
// (window.__gameState + the #hud DOM mirror).
//
// The round: 90 s of daylight. Tap empty space to plant a tier-1 mote;
// motes mature (halo) and can be tapped to harvest essence. Two mature
// motes of the same tier that drift within reach cross-pollinate into
// the next tier. Drag draws a current that herds motes together — and
// repels predator wisps, which hunt the nearest mote from the edges.
// Reach the essence quota before dusk to win; a tap after dusk replants.
// Every random draw comes from one seeded stream (?seed=N or the UTC
// date), so (seed + action sequence) reproduces the garden exactly.
//
// Daily weather: the UTC calendar date (YYYYMMDD) deterministically picks
// the day's weather — a wind-strength multiplier plus a prevailing-wind
// bias — so every player's Tuesday garden differs from their Monday one,
// and everyone on the planet shares the same weather on the same UTC day.
// The date is read ONCE at boot to select the parameters; the sim step
// never touches the wall clock, so reproducibility is untouched.

const STEP_MS = 1000 / 60;      // fixed simulation timestep (60 Hz)
const MAX_ACCUM_MS = 250;       // spiral-of-death guard
const MAX_ENTITIES = 200;

const ROUND_STEPS = 5400;       // 90 s of daylight
const QUOTA = 100;              // essence needed before dusk
const MATURE_AGE = 300;         // steps until a mote can be harvested
const POLLINATE_R = 40;         // mature same-tier merge distance (canvas px)
const TAP_SLOP = 12;            // extra tap-hit radius around a mote
const HERD_R = 120;             // drag-current reach
const HERD_PULL = 0.35;         // capped per-application herd acceleration
const WISP_FIRST = 600;         // step of the first wisp spawn
const WISP_EVERY = 900;         // mean steps between wisp spawns (±300)
const WISP_CAP = 3;
const WISP_REPELS = 3;          // drags needed to dissipate a wisp
const TIER_VALUE = { 1: 5, 2: 15, 3: 40 };
const TIER_R = { 1: 5, 2: 8, 3: 12 };
const TIER_HUE = { 1: 130, 2: 200, 3: 52 };

// mulberry32 — tiny seedable PRNG, plenty for a garden.
function mulberry32(a) {
  return function () {
    a |= 0; a = (a + 0x6D2B79F5) | 0;
    let t = Math.imul(a ^ (a >>> 15), 1 | a);
    t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

// ?seed=N wins; otherwise the UTC date is everyone's shared daily seed.
function seedFromUrl() {
  const q = new URLSearchParams(window.location.search).get('seed');
  if (q !== null && q !== '' && Number.isFinite(Number(q))) return Number(q);
  return utcDateNum(new Date());
}

// --- daily weather ----------------------------------------------------------
// Named weather kinds: a multiplier on the round's wind draws plus the
// magnitude of a prevailing-wind bias (direction is picked per day below).
const WEATHERS = [
  { name: 'still air', windMul: 0.35, biasMag: 0 },
  { name: 'breeze', windMul: 1.0, biasMag: 0.0004 },
  { name: 'gusts', windMul: 1.9, biasMag: 0.0002 },
  { name: 'crosswind', windMul: 0.9, biasMag: 0.0009 },
  { name: 'halcyon drift', windMul: 0.6, biasMag: 0.0006 },
];

// UTC calendar date -> YYYYMMDD as a number (e.g. 20260714).
function utcDateNum(d) {
  return (d.getUTCFullYear() * 10000 + (d.getUTCMonth() + 1) * 100 + d.getUTCDate()) >>> 0;
}

// Pure date -> weather parameters. Uses its OWN mulberry32 stream (seeded
// by the golden-ratio-mixed date) so weather selection never consumes from
// — or perturbs — the round's gameplay RNG. Same UTC date, same weather,
// for every player; only ever called at boot, never inside the sim step.
function weatherFor(dateNum) {
  const r = mulberry32((dateNum ^ 0x9E3779B9) >>> 0);
  const kind = WEATHERS[Math.floor(r() * WEATHERS.length)];
  const dir = r() * Math.PI * 2; // the day's prevailing-wind direction
  return {
    date: dateNum,
    name: kind.name,
    windMul: kind.windMul,
    biasX: Math.cos(dir) * kind.biasMag,
    biasY: Math.sin(dir) * kind.biasMag,
  };
}

export function createGame(canvas, opts = {}) {
  const hud = opts.hud || null;
  const ctx = canvas.getContext('2d');

  // Today's weather — computed ONCE here at boot from the UTC date and
  // never re-read: a garden left running across midnight keeps its
  // weather until reload, so a round is never perturbed mid-run.
  const weather = weatherFor(utcDateNum(new Date()));

  const state = {
    booted: true,
    frames: 0,        // fixed-step simulation ticks (never reset)
    renders: 0,       // rAF paints
    entities: 0,      // living motes
    taps: 0,
    paused: false,
    lastTap: null,    // {x, y} in canvas pixels
    seed: 0,          // seed of the CURRENT round
    baseSeed: 0,
    roundsPlayed: 0,  // finished rounds (won or lost)
    phase: 'playing', // 'playing' | 'won' | 'lost'
    essence: 0,
    quota: QUOTA,
    timeLeft: ROUND_STEPS / 60, // whole seconds of daylight left
    motesLost: 0,     // eaten by wisps, lifetime
    wispsAlive: 0,
    wispsDissipated: 0,
    weather: weather.name,     // today's weather (date-derived, boot-only)
    weatherDate: weather.date, // the UTC YYYYMMDD that picked it
  };

  const motes = [];
  const wisps = [];
  let rng = mulberry32(0);
  let roundStep = 0;
  let nextWisp = WISP_FIRST;
  let wind = { x: 0, y: 0 };
  let accum = 0;
  let lastNow = null;
  let running = false;

  function resize() {
    const dpr = Math.min(window.devicePixelRatio || 1, 2);
    canvas.width = Math.max(1, Math.floor(canvas.clientWidth * dpr)) || 320;
    canvas.height = Math.max(1, Math.floor(canvas.clientHeight * dpr)) || 480;
  }

  function spawnMote(x, y, tier = 1) {
    if (motes.length >= MAX_ENTITIES) motes.shift();
    const a = rng() * Math.PI * 2;
    const s = 0.02 + rng() * 0.06;
    motes.push({
      x, y, tier,
      vx: Math.cos(a) * s, vy: Math.sin(a) * s,
      r: TIER_R[tier] + rng() * 2,
      hue: TIER_HUE[tier] + (rng() - 0.5) * 30,
      age: 0,
    });
    state.entities = motes.length;
  }

  function spawnWisp() {
    const side = Math.floor(rng() * 4);
    const t = rng();
    let x, y;
    if (side === 0) { x = t * canvas.width; y = -10; }
    else if (side === 1) { x = canvas.width + 10; y = t * canvas.height; }
    else if (side === 2) { x = t * canvas.width; y = canvas.height + 10; }
    else { x = -10; y = t * canvas.height; }
    wisps.push({ x, y, vx: 0, vy: 0, r: 9, repels: 0, trail: [] });
    state.wispsAlive = wisps.length;
  }

  // One wind draw, shaped by today's weather: the seeded stream still
  // supplies the randomness (two rng() calls, exactly as before the
  // weather cut), the daily parameters only scale and bias it.
  function drawWind() {
    return {
      x: (rng() - 0.5) * 0.002 * weather.windMul + weather.biasX,
      y: (rng() - 0.5) * 0.002 * weather.windMul + weather.biasY,
    };
  }

  function resetRound(seed) {
    state.seed = seed;
    rng = mulberry32(seed >>> 0);
    motes.length = 0;
    wisps.length = 0;
    state.phase = 'playing';
    state.essence = 0;
    state.timeLeft = ROUND_STEPS / 60;
    state.wispsAlive = 0;
    roundStep = 0;
    nextWisp = WISP_FIRST;
    wind = drawWind();
    // a few seed motes so first paint is alive
    for (let i = 0; i < 3; i++) {
      spawnMote(canvas.width * (0.25 + 0.25 * i), canvas.height * 0.35, 1);
    }
    state.entities = motes.length;
  }

  function endRound(phase) {
    state.phase = phase;
    state.roundsPlayed += 1;
  }

  // --- input: touch first, pointer events as the fallback path -------------
  function canvasPoint(clientX, clientY) {
    const rect = canvas.getBoundingClientRect();
    const sx = canvas.width / (rect.width || 1);
    const sy = canvas.height / (rect.height || 1);
    return { x: (clientX - rect.left) * sx, y: (clientY - rect.top) * sy };
  }

  function moteAt(p) {
    let best = null, bestD = Infinity;
    for (const m of motes) {
      const d = Math.hypot(m.x - p.x, m.y - p.y);
      if (d <= m.r + TAP_SLOP && d < bestD) { best = m; bestD = d; }
    }
    return best;
  }

  // One tap = plant on empty ground, harvest a mature mote, or (after the
  // round ends) replant the garden with the next round's seed.
  function tapAt(clientX, clientY) {
    const p = canvasPoint(clientX, clientY);
    state.taps += 1;
    state.lastTap = { x: Math.round(p.x), y: Math.round(p.y) };
    if (state.phase !== 'playing') {
      resetRound(state.baseSeed + state.roundsPlayed);
      mirror();
      return;
    }
    const hit = moteAt(p);
    if (hit) {
      if (hit.age >= MATURE_AGE) {
        motes.splice(motes.indexOf(hit), 1);
        state.entities = motes.length;
        state.essence += TIER_VALUE[hit.tier];
        if (state.essence >= state.quota) endRound('won');
      }
    } else {
      spawnMote(p.x, p.y, 1);
    }
    mirror();
  }

  function tapFromEvent(clientX, clientY) {
    if (state.paused) return;
    tapAt(clientX, clientY);
  }

  let dragging = false;
  function bindInput() {
    const touchCapable = 'ontouchstart' in window || (navigator.maxTouchPoints || 0) > 0;
    if (touchCapable) {
      canvas.addEventListener('touchstart', (e) => {
        e.preventDefault();
        for (const t of e.changedTouches) tapFromEvent(t.clientX, t.clientY);
        dragging = true;
      }, { passive: false });
      canvas.addEventListener('touchmove', (e) => {
        e.preventDefault();
        if (!dragging || state.paused) return;
        // drag draws a current — herds motes together, repels wisps
        for (const t of e.changedTouches) current(canvasPoint(t.clientX, t.clientY));
      }, { passive: false });
      canvas.addEventListener('touchend', (e) => {
        e.preventDefault();
        dragging = false;
      }, { passive: false });
    } else if (window.PointerEvent) {
      canvas.addEventListener('pointerdown', (e) => { tapFromEvent(e.clientX, e.clientY); dragging = true; });
      canvas.addEventListener('pointermove', (e) => {
        if (dragging && !state.paused) current(canvasPoint(e.clientX, e.clientY));
      });
      canvas.addEventListener('pointerup', () => { dragging = false; });
    } else {
      canvas.addEventListener('mousedown', (e) => tapFromEvent(e.clientX, e.clientY));
    }
  }

  // A finger current: gentle capped pull on motes toward the touch point,
  // strong push on wisps away from it (enough pushes dissipate one).
  function current(p) {
    if (state.phase !== 'playing') return;
    for (const m of motes) {
      const dx = p.x - m.x, dy = p.y - m.y;
      const d = Math.hypot(dx, dy);
      if (d < HERD_R && d > 0.01) {
        const pull = Math.min(HERD_PULL, 0.08 + HERD_PULL * (d / HERD_R));
        m.vx += (dx / d) * pull;
        m.vy += (dy / d) * pull;
      }
    }
    for (let i = wisps.length - 1; i >= 0; i--) {
      const w = wisps[i];
      const dx = w.x - p.x, dy = w.y - p.y;
      const d = Math.hypot(dx, dy);
      if (d < HERD_R && d > 0.01) {
        w.vx += (dx / d) * 2.5;
        w.vy += (dy / d) * 2.5;
        w.repels += 1;
        if (w.repels >= WISP_REPELS) {
          wisps.splice(i, 1);
          state.wispsDissipated += 1;
          state.wispsAlive = wisps.length;
        }
      }
    }
  }

  // --- fixed-timestep simulation -------------------------------------------
  function wrapEntity(e) {
    if (e.x < -e.r) e.x = canvas.width + e.r;
    if (e.x > canvas.width + e.r) e.x = -e.r;
    if (e.y < -e.r) e.y = canvas.height + e.r;
    if (e.y > canvas.height + e.r) e.y = -e.r;
  }

  function pollinate() {
    for (let i = 0; i < motes.length; i++) {
      const a = motes[i];
      if (a.age < MATURE_AGE || a.tier >= 3) continue;
      for (let j = i + 1; j < motes.length; j++) {
        const b = motes[j];
        if (b.tier !== a.tier || b.age < MATURE_AGE) continue;
        if (Math.hypot(a.x - b.x, a.y - b.y) > POLLINATE_R) continue;
        const nx = (a.x + b.x) / 2, ny = (a.y + b.y) / 2, tier = a.tier + 1;
        motes.splice(j, 1);
        motes.splice(i, 1);
        spawnMote(nx, ny, tier);
        return; // at most one merge per step keeps the scan simple
      }
    }
  }

  function updateWisps() {
    if (roundStep >= nextWisp) {
      if (wisps.length < WISP_CAP) spawnWisp();
      nextWisp += WISP_EVERY + Math.floor(rng() * 601) - 300;
    }
    for (let i = wisps.length - 1; i >= 0; i--) {
      const w = wisps[i];
      let target = null, bd = Infinity;
      for (const m of motes) {
        const d = Math.hypot(m.x - w.x, m.y - w.y);
        if (d < bd) { bd = d; target = m; }
      }
      if (target) {
        w.vx = (w.vx + ((target.x - w.x) / (bd || 1)) * 0.04) * 0.98;
        w.vy = (w.vy + ((target.y - w.y) / (bd || 1)) * 0.04) * 0.98;
      } else {
        w.vx *= 0.995;
        w.vy *= 0.995;
      }
      w.x += w.vx;
      w.y += w.vy;
      wrapEntity(w);
      w.trail.push({ x: w.x, y: w.y });
      if (w.trail.length > 24) w.trail.shift();
      if (target && Math.hypot(target.x - w.x, target.y - w.y) <= w.r + target.r) {
        motes.splice(motes.indexOf(target), 1);
        state.entities = motes.length;
        state.motesLost += 1;
      }
    }
    state.wispsAlive = wisps.length;
  }

  function update() {
    if (state.phase !== 'playing') return; // terminal rounds are frozen
    state.frames += 1;
    roundStep += 1;
    state.timeLeft = Math.ceil((ROUND_STEPS - roundStep) / 60);
    if (roundStep % 600 === 0) {
      wind = drawWind();
    }
    for (const m of motes) {
      m.age += 1;
      m.vx = (m.vx + wind.x) * 0.98;
      m.vy = (m.vy + wind.y) * 0.98;
      const sp = Math.hypot(m.vx, m.vy);
      if (sp > 1.2) { m.vx *= 1.2 / sp; m.vy *= 1.2 / sp; }
      m.x += m.vx;
      m.y += m.vy;
      wrapEntity(m);
    }
    pollinate();
    updateWisps();
    if (roundStep >= ROUND_STEPS) endRound('lost');
  }

  // --- rendering ------------------------------------------------------------
  function render() {
    state.renders += 1;
    const w = canvas.width, h = canvas.height;
    const dusk = Math.min(1, roundStep / ROUND_STEPS);
    ctx.fillStyle = `rgb(${Math.round(16 - 10 * dusk)}, ${Math.round(24 - 20 * dusk)}, ${Math.round(32 - 20 * dusk)})`;
    ctx.fillRect(0, 0, w, h);
    for (const wi of wisps) {
      for (let i = 0; i < wi.trail.length; i++) {
        const p = wi.trail[i];
        ctx.beginPath();
        ctx.arc(p.x, p.y, 2 + 3 * (i / wi.trail.length), 0, Math.PI * 2);
        ctx.fillStyle = `hsla(280, 60%, 30%, ${0.05 + 0.25 * (i / wi.trail.length)})`;
        ctx.fill();
      }
      ctx.beginPath();
      ctx.arc(wi.x, wi.y, wi.r, 0, Math.PI * 2);
      ctx.fillStyle = 'hsl(280, 70%, 22%)';
      ctx.fill();
      ctx.strokeStyle = 'hsl(285, 80%, 55%)';
      ctx.lineWidth = 1.5;
      ctx.stroke();
    }
    for (const m of motes) {
      ctx.beginPath();
      ctx.arc(m.x, m.y, m.r, 0, Math.PI * 2);
      ctx.fillStyle = `hsl(${m.hue}, 70%, ${45 + 8 * m.tier + 8 * Math.sin(m.age / 20)}%)`;
      ctx.fill();
      if (m.age >= MATURE_AGE) {
        ctx.beginPath();
        ctx.arc(m.x, m.y, m.r + 4, 0, Math.PI * 2);
        ctx.strokeStyle = `hsla(${m.hue}, 90%, 75%, 0.9)`;
        ctx.lineWidth = 1.5;
        ctx.stroke();
      }
    }
    ctx.fillStyle = state.paused ? '#e0a050' : '#9fd8c0';
    ctx.font = `${Math.max(12, Math.round(w / 40))}px monospace`;
    ctx.fillText(
      `f:${state.frames} e:${state.entities} t:${state.taps} ` +
      `ess:${state.essence}/${state.quota} ${state.timeLeft}s ${state.phase}` +
      `${state.paused ? ' PAUSED' : ''}`,
      8, Math.max(16, Math.round(w / 30)));
    // small daily-weather label, second HUD line
    ctx.fillStyle = 'hsla(160, 30%, 70%, 0.75)';
    ctx.font = `${Math.max(10, Math.round(w / 48))}px monospace`;
    ctx.fillText(`today: ${state.weather}`, 8, Math.max(32, Math.round(w / 30) + 16));
    if (state.phase !== 'playing') {
      ctx.textAlign = 'center';
      ctx.fillStyle = state.phase === 'won' ? '#ffe08a' : '#b090c8';
      ctx.font = `${Math.max(20, Math.round(w / 16))}px monospace`;
      ctx.fillText(state.phase === 'won' ? 'GARDEN ASCENDS' : 'DUSK FELL', w / 2, h / 2 - 16);
      ctx.font = `${Math.max(13, Math.round(w / 32))}px monospace`;
      ctx.fillText(`essence ${state.essence}/${state.quota} — tap to replant`, w / 2, h / 2 + 20);
      ctx.textAlign = 'start';
    }
    mirror();
  }

  // --- observable state (headless-proof surface) ----------------------------
  function mirror() {
    if (hud) {
      hud.textContent =
        `f:${state.frames} e:${state.entities} t:${state.taps} ` +
        `ess:${state.essence}/${state.quota} ${state.timeLeft}s ${state.phase} ` +
        (state.paused ? 'PAUSED' : 'RUNNING') +
        ` · ${state.weather}`;
      hud.dataset.frames = String(state.frames);
      hud.dataset.entities = String(state.entities);
      hud.dataset.paused = String(state.paused);
      hud.dataset.essence = String(state.essence);
      hud.dataset.phase = state.phase;
      hud.dataset.timeLeft = String(state.timeLeft);
      hud.dataset.weather = state.weather;
      hud.dataset.weatherDate = String(state.weatherDate);
    }
    window.__gameState = { ...state, lastTap: state.lastTap ? { ...state.lastTap } : null };
  }

  // JSON-safe deterministic core (no render counters, no wall-clock —
  // weather is a boot-time sim INPUT, so it belongs in the comparable core:
  // two runs only claim equality if they shared the same day's weather).
  function snapshot() {
    const r2 = (v) => Math.round(v * 100) / 100;
    return {
      frames: state.frames,
      seed: state.seed,
      weather: state.weather,
      weatherDate: state.weatherDate,
      phase: state.phase,
      essence: state.essence,
      taps: state.taps,
      wisps: wisps.length,
      motes: motes.map((m) => ({ tier: m.tier, x: r2(m.x), y: r2(m.y) })),
    };
  }

  // --- pause / resume on visibility -----------------------------------------
  function setPaused(p) {
    state.paused = !!p;
    if (!state.paused) { lastNow = null; accum = 0; } // don't replay hidden time
    mirror();
  }
  document.addEventListener('visibilitychange', () => {
    setPaused(document.hidden);
  });

  // --- main loop -------------------------------------------------------------
  function frame(now) {
    if (!running) return;
    if (!state.paused) {
      if (lastNow === null) lastNow = now;
      accum = Math.min(accum + (now - lastNow), MAX_ACCUM_MS);
      lastNow = now;
      while (accum >= STEP_MS) {
        update();
        accum -= STEP_MS;
      }
    } else {
      lastNow = null;
    }
    render();
    requestAnimationFrame(frame);
  }

  const game = {
    state,
    start() {
      if (running) return;
      running = true;
      resize();
      bindInput();
      window.addEventListener('resize', resize);
      state.baseSeed = seedFromUrl();
      resetRound(state.baseSeed);
      if (document.hidden) setPaused(true); // e.g. restored background tab
      mirror();
      requestAnimationFrame(frame);
    },
    pause: () => setPaused(true),
    resume: () => setPaused(false),
    spawnAt: (x, y) => spawnMote(x, y, 1),
    // headless-proof drivers: same logic paths as real input, plus a
    // synchronous clock so proofs never depend on rAF timing.
    tapAt: (clientX, clientY) => tapAt(clientX, clientY),
    dragAt: (clientX, clientY) => current(canvasPoint(clientX, clientY)),
    step(n = 1) {
      for (let i = 0; i < n; i++) update();
      lastNow = null;
      accum = 0;
      render();
    },
    snapshot,
    // pure derivation, exposed for headless proofs (no test-only date
    // param in the product — proofs stub Date itself at the boundary)
    weatherFor: (dateNum) => weatherFor(dateNum),
  };
  window.__game = game; // headless-proof handle
  return game;
}
