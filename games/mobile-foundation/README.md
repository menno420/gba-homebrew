# mobile-foundation — Drift Garden (npm-free static PWA)

The mobile-game foundation for this lane, now carrying its first playable
game: an installable, offline-capable, touch-first web game with **zero
dependencies and no build step**. Framework decision + evidence:
[`DECISION.md`](DECISION.md). Game design + store path:
[`CONCEPT.md`](CONCEPT.md).

## What works (proven headlessly, 2026-07-13)

- **Playable win/lose rounds** — 90 s of daylight per round: tap plants a
  tier-1 mote, mature motes (halo) harvest for essence (5/15/40 by tier),
  reach the quota of 100 before dusk to win; a tap after the round ends
  replants with the next seed.
- **Cross-pollination** — two mature same-tier motes within reach merge
  into one mote of the next tier (up to tier 3, rarer and brighter).
- **Drag currents** — dragging herds nearby motes toward your finger (to
  cluster them for pollination) and repels predator **wisps**; three
  repels dissipate a wisp. Wisps spawn from the edges from 10 s in and
  eat the nearest mote on contact.
- **Seeded determinism** — one mulberry32 stream (`?seed=N`, else a daily
  UTC-date seed) feeds ALL randomness, so (seed + action sequence)
  reproduces the garden exactly; `window.__game.snapshot()` exposes the
  comparable core state.
- **Daily seeded weather** (2026-07-14) — the UTC date (YYYYMMDD)
  deterministically picks the day's named weather (wind-strength
  multiplier + prevailing-wind bias from its own PRNG stream) ONCE at
  boot, never inside the sim step: same UTC day, same weather for every
  player; a different day, a different garden even on the same `?seed=N`.
  Shown as a small `today: <weather>` label (canvas + `#hud` mirror);
  `__game.weatherFor(YYYYMMDD)` exposes the pure derivation for proofs.
- **Fixed-timestep game loop** — 60 Hz simulation accumulator +
  `requestAnimationFrame` rendering (`game.js`), spiral-of-death guarded.
- **Touch input** — `touchstart`/`touchmove`/`touchend` first, pointer
  events fallback.
- **Pause/resume** — `visibilitychange` hidden pauses the simulation
  (hidden time is never replayed), visible resumes.
- **Observable state** — frame/entity/tap/essence/phase counters drawn on
  canvas, mirrored to the `#hud` DOM element and `window.__gameState`,
  plus scripted drivers (`__game.step/tapAt/dragAt/snapshot`) so headless
  proofs can assert real behavior.
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

## Run the smoke proofs

```sh
tools/mobile-foundation-smoke/run.sh       # foundation guarantees
tools/mobile-foundation-smoke/run-game.sh  # game rules (game-smoke.mjs)
```

Each serves this directory, installs `playwright-core` into a scratch dir
(never into the repo — do NOT run `playwright install`; the container's
Chromium at `$PLAYWRIGHT_BROWSERS_PATH` is used via `executablePath`), and
exits nonzero on any failure. `run.sh` asserts the foundation: boot, loop
ticking, tap-changes-state, DOM mirror agreement, pause on hidden, resume
on visible. `run-game.sh` (`game-smoke.mjs`) asserts the game: seeded
boot, a real dispatched `TouchEvent` planting a mote, a scripted win
(pollination to tier 2, harvest to quota before dusk), a scripted loss at
dusk with wisps hunting, byte-identical `snapshot()` determinism for the
same seed (and divergence for a different one), pause/resume, and the
daily-weather cut (pure `weatherFor` derivation, boot under an injected
fake `Date`, same-date determinism, different dates diverging the world
on the same seed).

## Files

| File | Role |
|---|---|
| `index.html` | shell: viewport, fullscreen canvas, HUD, SW registration |
| `game.js` | ES module: loop, input, seeded rules (rounds/pollination/wisps), state surface |
| `manifest.webmanifest` | install metadata (standalone, portrait) |
| `sw.js` | cache-first service worker, versioned cache |
| `icon-192.png` / `icon-512.png` | generated placeholder icons (python3, <3 KB) |
