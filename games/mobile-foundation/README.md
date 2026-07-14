# mobile-foundation — Drift Garden (npm-free static PWA)

The mobile-game foundation for this lane, now carrying its first playable
game: an installable, offline-capable, touch-first web game with **zero
dependencies and no build step**. Framework decision + evidence:
[`DECISION.md`](DECISION.md). Game design + store path:
[`CONCEPT.md`](CONCEPT.md).

## What works (proven headlessly, 2026-07-13)

- **Playable win/lose rounds** — 90 s of daylight per round: tap plants a
  tier-1 mote, mature motes (halo) harvest for essence (by species —
  common line 5/15/40 per tier, rare line 8/22/55), reach the quota of
  100 before dusk to win; a tap after the round ends replants with the
  next seed.
- **Cross-pollination** — two mature same-tier motes within reach merge
  into one mote of the next tier (up to tier 3, rarer and brighter).
- **Species** (2026-07-14) — two species per tier (fern/clover,
  lotus/iris, aurora/solaris), each with its own hue; rare species carry
  a bright-core accent and a higher essence value. Planted tier-1 motes
  roll their species on a side-band seeded stream (the gameplay stream's
  draw order is untouched); cross-pollination children are a pure
  function of the parents — same-species pairs breed the common next-tier
  line, mixed pairs the rare one (hybrid vigor).
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
- **Essence spending** (2026-07-14) — when a round ends (won or lost),
  its harvested essence banks into a persistent wallet (guarded
  `localStorage`; storage failure degrades to session-only, play never
  breaks). The dusk/ascension screen shows a palette shop: tap a row to
  buy (wallet permitting) or select a garden palette — pure render (a
  background gradient pair + a draw-time hue shift), so the sim step,
  both RNG streams, and `snapshot()` are untouched whatever is unlocked.
  Proof surfaces: `__game.paletteTable/metaState/shopRows/buyPalette/
  selectPalette`.
- **Biomes** (2026-07-14, the last named growth cut) — the wallet also
  buys **biomes**, each a distinct WIND PATTERN: a parameter set for how
  the round's wind evolves (gust strength `windMul`, gust cadence
  `gustEvery`, a deterministic swirl that rotates the wind vector over
  the round, a sine sway that breathes its amplitude, and how strongly
  the day's prevailing weather bias couples in) plus a render-only hue
  tint. The active biome is meta, but it is a real sim input: it is read
  ONCE at round start (like the day's weather at boot — the sim step
  never reads meta or the clock), so a given biome + seed + date
  reproduces exactly, switching biomes only changes the NEXT round, and
  the free default ('meadow') reproduces pre-biome rounds byte-exactly.
  Proof surfaces: `__game.biomeTable/biomeRows/buyBiome/selectBiome`;
  `snapshot()` carries the round's biome.
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
same seed (and divergence for a different one), pause/resume, the
daily-weather cut (pure `weatherFor` derivation, boot under an injected
fake `Date`, same-date determinism, different dates diverging the world
on the same seed), and the species cut (both tier-1 species appearing
deterministically at a fixed seed, per-species harvest values, pure-pair
-> common / hybrid-pair -> rare pollination, and the pure-line ladder
through tier 3), and the essence-spending cut (round-end wallet banking,
deny/buy/re-buy spend flow, real-tap shop rows on the dusk screen,
accrual across rounds, persistence across a full reload, and
byte-identical sim snapshots with all palettes unlocked vs fresh
storage), and the biomes cut (default-biome identity with a pinned
pre-cut baseline, per-biome byte-identical determinism at a fixed
seed + fake date, four pairwise-distinct worlds on the same seed,
the spend/deny/re-buy flow, real-tap biome rows leaving the frozen
round untouched, the round-start read on replant, and persistence
across reload).

## Files

| File | Role |
|---|---|
| `index.html` | shell: viewport, fullscreen canvas, HUD, SW registration |
| `game.js` | ES module: loop, input, seeded rules (rounds/pollination/wisps), state surface |
| `manifest.webmanifest` | install metadata (standalone, portrait) |
| `sw.js` | cache-first service worker, versioned cache |
| `icon-192.png` / `icon-512.png` | generated placeholder icons (python3, <3 KB) |
