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
