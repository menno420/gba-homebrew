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
//
// Species: each tier hosts TWO species — a common line (carrying the
// pre-cut hues/values exactly) and a rarer, brighter line worth more
// essence. Planted tier-1 motes roll their species on a SIDE-BAND seeded
// stream (keyed off the round seed), so the gameplay stream's draw count
// and order are unchanged. Cross-pollination is species-aware: two
// same-species parents breed the common next-tier species; mixed parents
// breed the rare one (hybrid vigor) — a pure function of the parents,
// zero extra random draws.
//
// Essence spending: when a round ends, its harvested essence banks into a
// persistent wallet (guarded localStorage — storage failure degrades to
// session-only meta, never breaks play). On the dusk/ascension screen a
// small palette shop spends the wallet on garden palettes; a palette is
// PURE RENDER (a background gradient pair + a hue shift applied at draw
// time). The wallet, unlocks, and active palette are meta state: the sim
// step, both RNG streams, their draw order, and snapshot() never touch
// them — a round is byte-identical whatever is unlocked or active.

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
const TIER_R = { 1: 5, 2: 8, 3: 12 };

// --- species ---------------------------------------------------------------
// Two species per tier. Index 0 is the COMMON line and carries the pre-cut
// per-tier values/hues exactly (old TIER_VALUE 5/15/40, old TIER_HUE
// 130/200/52); index 1 is the RARE line — a distinct hue, a bright-core
// render accent, and a higher essence value.
const SPECIES = {
  1: [
    { id: 'fern',    hue: 130, value: 5,  rare: false },
    { id: 'clover',  hue: 95,  value: 8,  rare: true },
  ],
  2: [
    { id: 'lotus',   hue: 200, value: 15, rare: false },
    { id: 'iris',    hue: 250, value: 22, rare: true },
  ],
  3: [
    { id: 'aurora',  hue: 52,  value: 40, rare: false },
    { id: 'solaris', hue: 20,  value: 55, rare: true },
  ],
};
const RARE_CHANCE = 0.25; // tier-1 planting odds of the rare species

// Cross-pollination child: same-species parents breed the common
// next-tier line, mixed-species parents the rare one (hybrid vigor).
// Pure in the parents — no random draw.
function childSpeciesFor(parentTier, idA, idB) {
  return SPECIES[parentTier + 1][idA === idB ? 0 : 1];
}

// --- palettes (essence spending — meta, PURE RENDER) -------------------------
// Garden palettes bought with banked essence. A palette only re-skins the
// canvas: bg0/bg1 replace the daylight->dusk background gradient endpoints
// and hueShift rotates every mote hue at DRAW time (per-mote hues stay
// stored unshifted). Index 0 is the free default and reproduces the
// pre-cut rendering exactly (bg 16,24,32 -> 6,4,12; shift 0). Nothing
// here is read by the sim step, the RNG streams, or snapshot().
const PALETTES = [
  { id: 'verdant',   name: 'verdant',    cost: 0,   hueShift: 0,   bg0: [16, 24, 32], bg1: [6, 4, 12] },
  { id: 'duskbloom', name: 'dusk bloom', cost: 40,  hueShift: 45,  bg0: [26, 16, 34], bg1: [10, 4, 16] },
  { id: 'moonlit',   name: 'moonlit',    cost: 90,  hueShift: 150, bg0: [10, 18, 40], bg1: [2, 6, 18] },
  { id: 'emberdawn', name: 'ember dawn', cost: 160, hueShift: -95, bg0: [34, 18, 14], bg1: [14, 6, 4] },
];

// Persistent meta (wallet + palette unlocks) — guarded localStorage, the
// lane's guarded-access pattern: any storage failure (private mode,
// headless, quota) degrades to in-memory defaults and play continues.
const META_KEY = 'driftgarden.meta';
function loadMeta() {
  const meta = { wallet: 0, unlocked: ['verdant'], palette: 'verdant' };
  try {
    const raw = window.localStorage.getItem(META_KEY);
    if (raw) {
      const p = JSON.parse(raw);
      if (Number.isFinite(p.wallet) && p.wallet >= 0) meta.wallet = Math.floor(p.wallet);
      if (Array.isArray(p.unlocked)) {
        for (const id of p.unlocked) {
          if (PALETTES.some((pl) => pl.id === id) && !meta.unlocked.includes(id)) {
            meta.unlocked.push(id);
          }
        }
      }
      if (typeof p.palette === 'string' && meta.unlocked.includes(p.palette)) {
        meta.palette = p.palette;
      }
    }
  } catch (e) { /* storage unavailable/corrupt — session-only meta */ }
  return meta;
}
function saveMeta(meta) {
  try {
    window.localStorage.setItem(META_KEY, JSON.stringify({
      wallet: meta.wallet, unlocked: meta.unlocked, palette: meta.palette,
    }));
  } catch (e) { /* ignore — meta stays in memory for this session */ }
}

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

  // Persistent meta: essence wallet + palette unlocks (guarded storage).
  // Loaded once at boot; only round-end banking and shop actions write it.
  const meta = loadMeta();

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
    wallet: meta.wallet,       // banked essence (meta — never read by the sim)
    palette: meta.palette,     // active palette id (meta — pure render)
    unlocked: meta.unlocked.join(','), // owned palette ids (meta)
  };

  function syncMetaState() {
    state.wallet = meta.wallet;
    state.palette = meta.palette;
    state.unlocked = meta.unlocked.join(',');
  }

  function activePalette() {
    return PALETTES.find((p) => p.id === meta.palette) || PALETTES[0];
  }

  // Buy an affordable, not-yet-owned palette from the wallet (auto-selects
  // it); select an owned one. Both are meta-only: no sim state, no RNG.
  function buyPalette(id) {
    const pal = PALETTES.find((p) => p.id === id);
    if (!pal || meta.unlocked.includes(id) || meta.wallet < pal.cost) return false;
    meta.wallet -= pal.cost;
    meta.unlocked.push(id);
    meta.palette = id;
    saveMeta(meta);
    syncMetaState();
    mirror();
    return true;
  }
  function selectPalette(id) {
    if (!meta.unlocked.includes(id)) return false;
    meta.palette = id;
    saveMeta(meta);
    syncMetaState();
    mirror();
    return true;
  }

  const motes = [];
  const wisps = [];
  let rng = mulberry32(0);
  // Side-band species stream (the weather-cut discipline, PR #111): keyed
  // off the round seed with its own mix constant, one draw per tier-1
  // spawn, so the gameplay stream's draw count/order stay EXACTLY as
  // before this cut — every prior (seed -> world) mapping is preserved
  // modulo the species labels themselves.
  let speciesRng = mulberry32(0);
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

  // Tier-1 spawns (taps, boot seeds) roll their species on the side-band
  // stream; pollination children pass theirs explicitly (pure in parents).
  function spawnMote(x, y, tier = 1, species = null) {
    if (motes.length >= MAX_ENTITIES) motes.shift();
    const sp = species || SPECIES[tier][speciesRng() < RARE_CHANCE ? 1 : 0];
    const a = rng() * Math.PI * 2;
    const s = 0.02 + rng() * 0.06;
    motes.push({
      x, y, tier,
      sp: sp.id, value: sp.value, rare: sp.rare,
      vx: Math.cos(a) * s, vy: Math.sin(a) * s,
      r: TIER_R[tier] + rng() * 2,
      hue: sp.hue + (rng() - 0.5) * 30,
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
    speciesRng = mulberry32((seed ^ 0x85EBCA6B) >>> 0);
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
    // Bank the round's harvest into the persistent wallet — win or lose,
    // what you harvested is yours. Meta-only write: the sim never reads
    // the wallet, so banking cannot perturb the round that produced it.
    if (state.essence > 0) {
      meta.wallet += state.essence;
      saveMeta(meta);
      syncMetaState();
    }
  }

  // --- dusk-screen palette shop (meta UI) -----------------------------------
  // Row geometry is a pure function of the canvas size, shared by the
  // renderer and the tap hit-test (and exposed for headless proofs).
  function shopRows() {
    const w = canvas.width, h = canvas.height;
    const rowH = Math.max(24, Math.round(w / 14));
    const rowW = Math.min(w * 0.82, 360);
    const top = h / 2 + Math.max(44, Math.round(w / 8));
    return PALETTES.map((p, i) => ({
      id: p.id, x: (w - rowW) / 2, y: top + i * (rowH + 6), w: rowW, h: rowH,
    }));
  }
  function shopRowAt(p) {
    for (const r of shopRows()) {
      if (p.x >= r.x && p.x <= r.x + r.w && p.y >= r.y && p.y <= r.y + r.h) return r;
    }
    return null;
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
      // Dusk-screen shop first: a tap on a palette row buys it (if
      // affordable) or selects it (if owned) — meta only, the frozen
      // round is untouched. Any other tap replants, as before.
      const row = shopRowAt(p);
      if (row) {
        if (meta.unlocked.includes(row.id)) selectPalette(row.id);
        else buyPalette(row.id);
        mirror();
        return;
      }
      resetRound(state.baseSeed + state.roundsPlayed);
      mirror();
      return;
    }
    const hit = moteAt(p);
    if (hit) {
      if (hit.age >= MATURE_AGE) {
        motes.splice(motes.indexOf(hit), 1);
        state.entities = motes.length;
        state.essence += hit.value;
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
        const child = childSpeciesFor(a.tier, a.sp, b.sp);
        motes.splice(j, 1);
        motes.splice(i, 1);
        spawnMote(nx, ny, tier, child);
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
    // active palette: background gradient endpoints + a draw-time hue
    // shift (pure render — the default palette reproduces the pre-cut
    // rgb(16-10d, 24-20d, 32-20d) background and shift 0 exactly)
    const pal = activePalette();
    const mix = (i) => Math.round(pal.bg0[i] + (pal.bg1[i] - pal.bg0[i]) * dusk);
    const shift = pal.hueShift;
    ctx.fillStyle = `rgb(${mix(0)}, ${mix(1)}, ${mix(2)})`;
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
      ctx.fillStyle = `hsl(${m.hue + shift}, 70%, ${45 + 8 * m.tier + 8 * Math.sin(m.age / 20)}%)`;
      ctx.fill();
      if (m.rare) {
        // rare-species accent: a bright inner core, same idiom as the halo
        ctx.beginPath();
        ctx.arc(m.x, m.y, Math.max(1.5, m.r * 0.4), 0, Math.PI * 2);
        ctx.fillStyle = `hsla(${m.hue + shift + 40}, 95%, 85%, 0.9)`;
        ctx.fill();
      }
      if (m.age >= MATURE_AGE) {
        ctx.beginPath();
        ctx.arc(m.x, m.y, m.r + 4, 0, Math.PI * 2);
        ctx.strokeStyle = `hsla(${m.hue + shift}, 90%, 75%, 0.9)`;
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
    // small daily-weather + wallet label, second HUD line
    ctx.fillStyle = 'hsla(160, 30%, 70%, 0.75)';
    ctx.font = `${Math.max(10, Math.round(w / 48))}px monospace`;
    ctx.fillText(`today: ${state.weather} · bank: ${state.wallet}`, 8, Math.max(32, Math.round(w / 30) + 16));
    if (state.phase !== 'playing') {
      ctx.textAlign = 'center';
      ctx.fillStyle = state.phase === 'won' ? '#ffe08a' : '#b090c8';
      ctx.font = `${Math.max(20, Math.round(w / 16))}px monospace`;
      ctx.fillText(state.phase === 'won' ? 'GARDEN ASCENDS' : 'DUSK FELL', w / 2, h / 2 - 16);
      ctx.font = `${Math.max(13, Math.round(w / 32))}px monospace`;
      ctx.fillText(`essence ${state.essence}/${state.quota} — tap to replant`, w / 2, h / 2 + 20);
      // palette shop: banked essence + one tappable row per palette
      ctx.font = `${Math.max(11, Math.round(w / 40))}px monospace`;
      ctx.fillStyle = '#9fd8c0';
      const rows = shopRows();
      ctx.fillText(`essence bank: ${state.wallet}`, w / 2, rows[0].y - 12);
      for (const r of rows) {
        const p = PALETTES.find((pl) => pl.id === r.id);
        const owned = meta.unlocked.includes(p.id);
        const active = meta.palette === p.id;
        ctx.strokeStyle = active ? '#ffe08a' : 'hsla(160, 30%, 70%, 0.5)';
        ctx.lineWidth = active ? 2 : 1;
        ctx.strokeRect(r.x, r.y, r.w, r.h);
        ctx.fillStyle = owned ? '#9fd8c0' : 'hsla(160, 20%, 55%, 0.9)';
        ctx.fillText(
          `${p.name} — ${active ? 'active' : owned ? 'owned' : `${p.cost} essence`}`,
          r.x + r.w / 2, r.y + r.h / 2 + 4);
      }
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
        ` · ${state.weather} · bank ${state.wallet}`;
      hud.dataset.frames = String(state.frames);
      hud.dataset.entities = String(state.entities);
      hud.dataset.paused = String(state.paused);
      hud.dataset.essence = String(state.essence);
      hud.dataset.phase = state.phase;
      hud.dataset.timeLeft = String(state.timeLeft);
      hud.dataset.weather = state.weather;
      hud.dataset.weatherDate = String(state.weatherDate);
      hud.dataset.wallet = String(state.wallet);
      hud.dataset.palette = state.palette;
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
      motes: motes.map((m) => ({ tier: m.tier, sp: m.sp, x: r2(m.x), y: r2(m.y) })),
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
    // pure species surfaces for proofs: the data table (deep copy) and
    // the parents -> child derivation (no game state touched)
    speciesTable: () => JSON.parse(JSON.stringify(SPECIES)),
    childSpeciesFor: (parentTier, idA, idB) => ({ ...childSpeciesFor(parentTier, idA, idB) }),
    // essence-spending surfaces: the palette table (deep copy), the live
    // meta state (copy), the shop's pure row layout, and the same
    // buy/select actions the dusk-screen taps drive (meta-only — the
    // frozen round and the RNG streams are never touched)
    paletteTable: () => JSON.parse(JSON.stringify(PALETTES)),
    metaState: () => ({ wallet: meta.wallet, unlocked: [...meta.unlocked], palette: meta.palette }),
    shopRows: () => shopRows().map((r) => ({ ...r })),
    buyPalette: (id) => buyPalette(id),
    selectPalette: (id) => selectPalette(id),
  };
  window.__game = game; // headless-proof handle
  return game;
}
