// Drift Garden — mobile foundation skeleton (original IP).
// Plain ES module, zero dependencies. Fixed-timestep simulation +
// requestAnimationFrame rendering, touch-first input, visibility
// pause/resume, and an observable state surface for headless proofs
// (window.__gameState + the #hud DOM mirror).

const STEP_MS = 1000 / 60;      // fixed simulation timestep (60 Hz)
const MAX_ACCUM_MS = 250;       // spiral-of-death guard
const MAX_ENTITIES = 200;

export function createGame(canvas, opts = {}) {
  const hud = opts.hud || null;
  const ctx = canvas.getContext('2d');

  const state = {
    booted: true,
    frames: 0,        // fixed-step simulation ticks
    renders: 0,       // rAF paints
    entities: 0,
    taps: 0,
    paused: false,
    lastTap: null,    // {x, y} in canvas pixels
  };

  const motes = [];
  let accum = 0;
  let lastNow = null;
  let running = false;

  function resize() {
    const dpr = Math.min(window.devicePixelRatio || 1, 2);
    canvas.width = Math.max(1, Math.floor(canvas.clientWidth * dpr)) || 320;
    canvas.height = Math.max(1, Math.floor(canvas.clientHeight * dpr)) || 480;
  }

  function spawnMote(x, y) {
    if (motes.length >= MAX_ENTITIES) motes.shift();
    const a = Math.random() * Math.PI * 2;
    motes.push({
      x, y,
      vx: Math.cos(a) * 0.4, vy: Math.sin(a) * 0.4 - 0.2,
      r: 4 + Math.random() * 6,
      hue: 120 + Math.random() * 120,
      age: 0,
    });
    state.entities = motes.length;
  }

  // --- input: touch first, pointer events as the fallback path -------------
  function canvasPoint(clientX, clientY) {
    const rect = canvas.getBoundingClientRect();
    const sx = canvas.width / (rect.width || 1);
    const sy = canvas.height / (rect.height || 1);
    return { x: (clientX - rect.left) * sx, y: (clientY - rect.top) * sy };
  }

  function tapAt(clientX, clientY) {
    if (state.paused) return;
    const p = canvasPoint(clientX, clientY);
    state.taps += 1;
    state.lastTap = { x: Math.round(p.x), y: Math.round(p.y) };
    spawnMote(p.x, p.y);
    mirror();
  }

  let dragging = false;
  function bindInput() {
    const touchCapable = 'ontouchstart' in window || (navigator.maxTouchPoints || 0) > 0;
    if (touchCapable) {
      canvas.addEventListener('touchstart', (e) => {
        e.preventDefault();
        for (const t of e.changedTouches) tapAt(t.clientX, t.clientY);
        dragging = true;
      }, { passive: false });
      canvas.addEventListener('touchmove', (e) => {
        e.preventDefault();
        if (!dragging || state.paused) return;
        // drag nudges nearby motes away — a visible touchmove effect
        for (const t of e.changedTouches) nudge(canvasPoint(t.clientX, t.clientY));
      }, { passive: false });
      canvas.addEventListener('touchend', (e) => {
        e.preventDefault();
        dragging = false;
      }, { passive: false });
    } else if (window.PointerEvent) {
      canvas.addEventListener('pointerdown', (e) => { tapAt(e.clientX, e.clientY); dragging = true; });
      canvas.addEventListener('pointermove', (e) => {
        if (dragging && !state.paused) nudge(canvasPoint(e.clientX, e.clientY));
      });
      canvas.addEventListener('pointerup', () => { dragging = false; });
    } else {
      canvas.addEventListener('mousedown', (e) => tapAt(e.clientX, e.clientY));
    }
  }

  function nudge(p) {
    for (const m of motes) {
      const dx = m.x - p.x, dy = m.y - p.y;
      const d2 = dx * dx + dy * dy;
      if (d2 < 80 * 80 && d2 > 0.01) {
        const d = Math.sqrt(d2);
        m.vx += (dx / d) * 0.8;
        m.vy += (dy / d) * 0.8;
      }
    }
  }

  // --- fixed-timestep simulation -------------------------------------------
  function update() {
    state.frames += 1;
    for (const m of motes) {
      m.age += 1;
      m.x += m.vx;
      m.y += m.vy;
      m.vy -= 0.002; // gentle upward drift
      if (m.x < -m.r) m.x = canvas.width + m.r;
      if (m.x > canvas.width + m.r) m.x = -m.r;
      if (m.y < -m.r) m.y = canvas.height + m.r;
      if (m.y > canvas.height + m.r) m.y = -m.r;
    }
  }

  // --- rendering ------------------------------------------------------------
  function render() {
    state.renders += 1;
    const w = canvas.width, h = canvas.height;
    ctx.fillStyle = '#101820';
    ctx.fillRect(0, 0, w, h);
    for (const m of motes) {
      ctx.beginPath();
      ctx.arc(m.x, m.y, m.r, 0, Math.PI * 2);
      ctx.fillStyle = `hsl(${m.hue}, 70%, ${55 + 10 * Math.sin(m.age / 20)}%)`;
      ctx.fill();
    }
    ctx.fillStyle = state.paused ? '#e0a050' : '#9fd8c0';
    ctx.font = `${Math.max(12, Math.round(w / 40))}px monospace`;
    ctx.fillText(
      `f:${state.frames} e:${state.entities} t:${state.taps}${state.paused ? ' PAUSED' : ''}`,
      8, Math.max(16, Math.round(w / 30)));
    mirror();
  }

  // --- observable state (headless-proof surface) ----------------------------
  function mirror() {
    if (hud) {
      hud.textContent =
        `f:${state.frames} e:${state.entities} t:${state.taps} ` +
        (state.paused ? 'PAUSED' : 'RUNNING');
      hud.dataset.frames = String(state.frames);
      hud.dataset.entities = String(state.entities);
      hud.dataset.paused = String(state.paused);
    }
    window.__gameState = { ...state, lastTap: state.lastTap ? { ...state.lastTap } : null };
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
      // a few seed motes so first paint is alive
      for (let i = 0; i < 3; i++) {
        spawnMote(canvas.width * (0.25 + 0.25 * i), canvas.height * 0.5);
      }
      mirror();
      requestAnimationFrame(frame);
    },
    pause: () => setPaused(true),
    resume: () => setPaused(false),
    spawnAt: (x, y) => spawnMote(x, y),
  };
  window.__game = game; // headless-proof handle
  return game;
}
