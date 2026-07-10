# Session 1 — 3 original game concepts (Track B, ORDER 001)

> **Status:** `plan` — awaiting owner pick (joint done-when with Track A's
> 3 concepts; scope-estimate format shared across both tracks: sessions to
> first-playable, systems touched, risk).

All three are **original IP** designed around what the GBA + Butano stack is
actually good at (sprites, tile backgrounds, fixed-point math, 4 face
buttons + D-pad) and what this lane has proven it can verify headlessly
(deterministic frames, screenshot diffs, scripted input).

---

## 1. Lumen Drift — one-button gravity cave drifter

You are a mote of light falling through a dark cave system. One button:
thrust against gravity. Physics does the rest — drift through narrow
passages, ride gravity wells, collect spark shards that extend your light
radius before it decays. Score = depth reached. Short runs, instant restart,
arcade loop.

- **Sessions to first-playable:** **2** (S1: physics + collision against a
  handcrafted tilemap + light-radius render trick; S2: 3 cave sections,
  shards, death/restart loop, score HUD).
- **Systems touched:** fixed-point sprite physics, regular BG tilemaps +
  map collision, palette tricks for the light-decay effect (per-frame
  palette fades — Butano makes this cheap), score/HUD text, headless
  input-script tests (thrust pattern → expected depth).
- **Risk:** **LOW.** Every system is a proven Butano primitive; one-button
  input makes scripted regression tests trivial (a recorded input sequence
  must reach a known depth — deterministic on mGBA). Deepest unknown is
  making the light-decay feel good, which is tuning, not tech.
- **Measured (post-increment 2, 2026-07-10 — estimate CONFIRMED):** built
  under the standing default while the pick is pending. Increment 1 (PR #6,
  ~1 session) landed physics + tilemap collision + light-decay + depth HUD;
  increment 2 (PR #9, ~1 session) landed hazards (crystal spikes + the
  descending surge, replacing the planned "gravity wells" with a stronger
  stall-punisher) + death/restart + best-depth — i.e. **first-playable in
  the estimated 2 sessions**, and every scripted-input regression bet held
  (all proofs deterministic on mGBA). Not yet built from the original S2
  list: spark shards and extra cave sections — a natural increment 3 with
  SRAM best-score persistence. The generic half (input/kinematics/
  collision/run-state headers, harness input scripting) is committed in
  `games/common/` and transfers to either other concept.
- **FIRST-COMPLETE (post-increment 3, 2026-07-10):** the concept's
  committed scope is now **done in 3 sessions** (2 estimated to
  first-playable + 1 to scope-complete). Increment 3 (PR #12) landed the
  last two S2-list items: **spark shards** (edge-hugging collectibles that
  rewind the light decay — the concept's "collect spark shards that extend
  your light before it decays" — with a SPARKS HUD + game-over line) and
  **3 cave sections** (THE BLUFFS / THE GALLERY / THE DEEP: per-section rim
  colors, waveforms, widths, obstacles and hazard density, entry banners).
  Quick-win PR #13 added the concept doc's own regression-test design:
  scripted-input runs now ASSERT HUD/card text (`--assert-text`, e.g. "this
  script still dies at DEPTH 45 with SPARKS 2") in the dispatch-tier CI.
  **What a post-pick polish pass would add** (list as of first-complete;
  the pick is still pending): `bn::sram` best-depth persistence across power
  cycles (session-3 card idea), audio (mmutil is installed and untouched),
  a title screen, light-radius as a *mechanic* (shards currently rewind the
  decay timer; the original text imagined a visible radius), difficulty
  tuning from real play, and more cave beyond row 62. The generic half
  (`gl_input/kinematics/tile_grid/run_state/stage` + harness scripting/
  assertions) transfers to either other concept unchanged.
- **POLISH PASS 1 (session 5, coordinator-directed):** the first two items
  of that list are now BUILT: a **title screen** ("LUMEN DRIFT" card over
  the cave mouth with the BEST depth and PRESS START; physics starts on the
  press, keeping every scripted replay deterministic at a constant +4-frame
  offset) and **SRAM best persistence** (`gl_save.h` magic-checked
  `bn::sram` slot; BEST survives power cycles, proven headlessly with two
  separate emulator boots on one savefile — `docs/proof/session-5-sram-*`).
  Still unbuilt from the list: audio, light-radius mechanic, tuning, cave
  beyond row 62.
- **POLISH PASS 2 (session 6, coordinator-directed):** three more list items
  BUILT: **audio** (four original synthesized sounds — run-start confirm,
  thrust rumble, shard glitter, death sweep — generated deterministically by
  the stdlib-only `games/lumen-drift/audio/generate_audio.py` and built via
  Butano's maxmod pipeline; headlessly PROVEN by asserting in-ROM trigger
  counters + maxmod mixing-buffer activity at must-play vs must-be-silent
  frames, method in `docs/capabilities.md`), the **light radius as a
  mechanic** (the concept's original line at last: the cave is only visible
  inside a circle of light around the mote — `games/common/gl_light_window.h`
  over the hardware window; full view at run start, shrinking with the light
  level, growing back on shard pickup; rendering-only, physics untouched),
  and **tuning** of the doc's named unknown, the light-decay feel (decay
  horizon 3600→2700 frames, fade cap 0.45→0.40, shard refund 1500→1200 —
  the pressure now reads through the shrinking radius). Replays shifted by
  one more constant frame (+5 total vs the session-4 script — the run-start
  stall grew with audio+window init; re-bisected, headless-boot.yml updated).
  **The only list item still unbuilt: more cave beyond row 62.**
- **POLISH PASS 3 (session 7, coordinator-directed) — the polish list is
  now EXHAUSTED:** the last item, **more cave beyond row 62**, is BUILT.
  The stored 64-row map used to BE the world — rows 62-63 were a solid
  floor, every deep run capped at DEPTH 58 parked on the same dead end
  waiting for the surge. Now every row is a pure function of the world row
  and the stored map is a sliding 64-row window over an **endless
  deterministic cave** (GBA regular backgrounds wrap; world row N lives in
  buffer row N % 64): rows 0-61 reproduce the committed layout
  cell-for-cell (the canonical replay passes unchanged — the offset is +4
  now, the new rebake being two frames FASTER, re-bisected), THE DEEP's
  squeeze continues through the old floor, and from row 64 twenty-row
  **echo bands** cycle the three section looks forever over one continuous
  spine ("ECHOES OF THE BLUFFS / THE GALLERY / THE DEEP" banners, cycling
  rims, blobs/pillars/dense-crystal signatures — band transitions provably
  passable). Deep-run proof: an autopilot-recorded deterministic script
  (`docs/proof/session-7-deep-run-keys.txt`, replayed in CI) descends to
  **DEPTH 95** alive — past the old cap at f1372, "ECHOES OF THE BLUFFS"
  at f1500, a second band at f1960 — asserted from HUD text AND the new
  numeric depth telemetry (gl_audio_hook slots 4-6);
  `docs/proof/session-7-*.png`.

  **STATUS: SCOPE-COMPLETE.** The committed scope (3 sessions,
  first-complete) plus the FULL polish list from the first-complete note
  (SRAM best · title screen · audio · light-radius mechanic · tuning ·
  cave beyond row 62) are all built and proven. No further Lumen Drift
  work is queued: the owner concept pick (this doc, ⚑ in
  control/status.md) decides whether polish continues here or the generic
  scaffolding transfers to Clockwork Courier / Shoal.

## 2. Clockwork Courier — rewind-ghost puzzle platformer

A courier robot in a clockwork tower delivers parcels to timed chutes. The
hook: pressing R rewinds YOU 5 seconds but leaves a ghost that replays what
you just did — you cooperate with your own past self (stand on your ghost,
have it hold a switch while present-you runs the parcel). Levels are
single-screen puzzles.

- **Sessions to first-playable:** **3** (S1: platformer movement + tilemap
  collision; S2: input-recording ghost replay + rewind; S3: 4 handcrafted
  puzzle levels + win/lose loop).
- **Systems touched:** platformer physics, input record/replay buffers
  (IWRAM budget!), deterministic simulation (ghost must replay exactly),
  level data pipeline (tilemaps per level), sprite animation states.
- **Risk:** **MEDIUM.** Replay correctness demands full determinism of the
  movement sim — good discipline (and the headless harness can literally
  assert it: record inputs, compare end-state), but subtle to debug when it
  drifts. IWRAM/EWRAM budget for 5s × 60fps input+state snapshots needs
  early sizing.

## 3. Shoal — flock-herding action puzzler

You steer a current (a cursor that pushes water) to herd a school of 40-60
fish past predators, through gates, into the safe reef. The fish are a
boids flock — cohesion/alignment/avoidance — so the game feel is herding
smoke: pressure, not control. Predators lock onto stragglers. Star rating =
fish saved.

- **Sessions to first-playable:** **3** (S1: boids flock on fixed-point +
  cursor current force; S2: predators + gates + lose/save conditions; S3:
  4 levels with tuned flock parameters + rating screen).
- **Systems touched:** many-sprite management against the GBA's 128-OAM
  limit (fish likely need batching/pooling), fixed-point vector math under
  a real CPU budget (Butano profiler from session 1), simple AI (predator
  pursuit), per-level tuning data.
- **Risk:** **MEDIUM-HIGH.** The flock is the game — if 50 boids don't fit
  the CPU/OAM budget the concept degrades fast (mitigations: neighbor grids,
  16.16→8.8 fixed precision drop, flock size tuning). Uniquely GBA-flavored
  and demos beautifully if it works; profile in the FIRST session, pivot
  cheap if the budget says no.

---

## Recommendation (non-binding)

**Lumen Drift** is the safest first pick (lowest risk, 2 sessions,
exercises exactly the physics/collision/palette systems every later concept
reuses). **Shoal** is the most distinctive but should only be picked with
its session-1 profiling gate respected. Owner picks per ORDER 001's joint
done-when; no build starts until then.
