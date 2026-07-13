# mobile-foundation — Drift Garden skeleton (npm-free static PWA)

The mobile-game foundation for this lane: an installable, offline-capable,
touch-first web game skeleton with **zero dependencies and no build step**.
Framework decision + evidence: [`DECISION.md`](DECISION.md). First target
game + store path: [`CONCEPT.md`](CONCEPT.md).

## What works (proven headlessly, 2026-07-13)

- **Fixed-timestep game loop** — 60 Hz simulation accumulator +
  `requestAnimationFrame` rendering (`game.js`), spiral-of-death guarded.
- **Touch input** — `touchstart`/`touchmove`/`touchend` first, pointer
  events fallback: tap plants a drifting mote, drag nudges nearby motes.
- **Pause/resume** — `visibilitychange` hidden pauses the simulation
  (hidden time is never replayed), visible resumes.
- **Observable state** — frame/entity/tap counters + paused flag drawn on
  canvas, mirrored to the `#hud` DOM element and `window.__gameState` so
  headless proofs can assert real behavior.
- **PWA shell** — `manifest.webmanifest` (standalone, portrait, icons) +
  `sw.js` (versioned cache-first precache) = add-to-home-screen and
  offline relaunch.

## Run it locally

```sh
cd games/mobile-foundation
python3 -m http.server 8901
# open http://<host>:8901/ — on a phone, use the LAN address and
# "Add to Home Screen" for the standalone install.
```

No build step. Editing any file and reloading is the whole pipeline
(bump `CACHE` in `sw.js` when shipping so installed clients refresh).

## Run the smoke proof

```sh
tools/mobile-foundation-smoke/run.sh
```

Serves this directory, installs `playwright-core` into a scratch dir
(never into the repo — do NOT run `playwright install`; the container's
Chromium at `$PLAYWRIGHT_BROWSERS_PATH` is used via `executablePath`), and
asserts: boot, loop ticking, tap-changes-state, DOM mirror agreement,
pause on hidden, resume on visible. Exits nonzero on any failure.

## Files

| File | Role |
|---|---|
| `index.html` | shell: viewport, fullscreen canvas, HUD, SW registration |
| `game.js` | ES module: loop, input, entities, state surface |
| `manifest.webmanifest` | install metadata (standalone, portrait) |
| `sw.js` | cache-first service worker, versioned cache |
| `icon-192.png` / `icon-512.png` | generated placeholder icons (python3, <3 KB) |
