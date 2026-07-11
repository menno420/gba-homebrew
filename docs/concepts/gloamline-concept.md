# Gloamline — concept slice (new arc, owner-picked: original zombie-survival, Nintendo DS)

> **Status:** `decision-ready` — arc slice 1 of the owner-picked
> zombie-survival arc (supersedes the deepen-LD / Clockwork Courier / Shoal
> menu). **Target: Nintendo DS** (owner correction, relayed 2026-07-11
> mid-slice — not GBA). 100% original IP: title, art, audio, code.
> Title checked against commercial collisions: no game named "Gloamline"
> exists (nearest are unrelated indie/TTRPG "Gloam" titles).

## Pitch

You are the last lamplighter of a fenced hamlet on the moor. Each night the
gloam spills over the fence line and the dead walk toward your lantern.
Hold the line until dawn: kite the crowd, shove clumps back off the
lamplight, and don't let a single cold hand touch you. Score is nights
survived. Short runs, instant restart, arcade loop — the shape this lane
has proven twice.

## Core loop — HORDE-DEFENSE (chosen over scavenge-day/survive-night)

One fixed arena (the hamlet yard, one 256x192 screen), nights as waves.
**Why this one:** (1) a fixed arena + a spawn *schedule* is a pure function
of `(seed, night, index)` — exactly the shape our host-proof and replay
machinery already verifies, where a scavenge world adds streaming +
inventory state that widens the proof surface for no skeleton value;
(2) the fun arrives in slice 1 (a crowd chasing you IS the game);
(3) scavenging slots in later as a between-nights interlude in the same
arena without reworking the loop.

- **Player verbs (skeleton max):** 8-way move (D-pad); **shove** (A):
  short-range knockback burst with a cooldown — spacing management, not a
  gun. That's all. (Sprint/dodge, lantern toggle: later slices.)
- **Zombie v0 — the Shambler:** every frame, step toward the player at a
  fixed sub-pixel speed with a per-zombie deterministic stagger (hash of
  `(zombie_id, frame)` — no runtime RNG), so a crowd smears into a readable
  arc instead of a stack. Contact = death.
- **Survival pressure:** the night clock. What kills you is touch; what you
  manage is *space* — the crowd only grows until dawn, so every shove and
  every corridor you leave yourself is the resource. (Later: lantern oil —
  light radius shrinks as it burns, dark edges spawn faster; the
  Lumen-Drift-style decay-you-can-buy-back pressure.)
- **Session length:** 3–6 minute runs; one night ≈ 60–90 s, difficulty
  ramps per night to a plateau (proof-friendly periodicity, as LD v1.1).
- **Fun in 30 seconds:** you are circling a growing crowd that bends after
  you, you thread a closing gap, shove three of them off the lamplight,
  and the dawn bar is *almost* there. Readable danger, one clean escape
  valve, a clock you can taste.
- **Dual screens (NDS strength, cheap):** top = the yard (all action);
  bottom = the watch-map — dawn clock, night number, zombie dots, later
  the oil gauge. A pure render of the same state, so it's free
  determinism-wise. **Touch optional, never required** (tap the map to
  drop a marker at most); the game is 100% playable on buttons.

## Walking-skeleton cut (arc slice 3) vs later

**SKELETON:** one arena screen, player 8-way move, ONE Shambler chasing,
contact = death → game-over card → instant restart, dawn timer to survive
NIGHT 1, HUD count (night + zombies + dawn bar; bottom screen may be static
text), telemetry mailbox + one recorded replay proof + host-proof stub.
**LATER SLICES:** shove verb + multi-zombie waves; night ramp/plateau;
barricades; between-nights scavenge interlude; lantern-oil light pressure;
synthesized audio set; save-file best-nights; watch-map polish.

## Engine reuse — honest framing (Butano is GBA-only; NDS = libnds/BlocksDS)

No Butano code compiles here. What ports is the **pure-logic and
methodology layer**, deliberately: (1) **pure-function world/spawn
generation** (LD's `layout_of_row` discipline → `spawns_of_night`);
(2) **check-cave-style host proofs** — a stdlib-only Python mirror
(`check-gloam.py`) asserting every night's schedule is survivable-in-
principle (spawn spacing, concurrent-sprite cap, no spawn inside the safe
radius, per-night counts strictly ramp then plateau) with the same
keep-in-lockstep rule; (3) the **telemetry-mailbox concept**
(`gl_audio_hook`: counters + position/state words at an ELF-resolved
symbol) for headless asserts; (4) the **replay/proof harness approach**
(`tools/headless-screenshot.py`, `tools/route-recorder.py`) re-pointed at a
headless NDS emulator — **melonDS or DeSmuME** instead of mGBA; (5) the
**synthesized-audio pipeline** (a `generate_audio.py` sibling; playback API
differs, the deterministic-generation approach doesn't); (6) shape/palette
generated art (`generate_assets.py` sibling — 16x16 sprites, tile BG; NDS
OAM gives 128 sprites/engine, we cap concurrent zombies at 24 for CPU and
proof headroom); (7) the **dist/CI conventions** (sha256-logged builds,
committed player-facing ROM with provenance). `games/common/gl_*.h` headers
port where they're pure (kinematics, run-state, tile-grid); Butano-typed
parts get rewritten thin.

## Determinism + host-provability plan

- **Seed policy:** CI/replays run seed 0 (a latched frame-counter seed at
  START for humans, printed on the HUD/game-over card so any run is
  reproducible). One seed word, set once per run.
- **Pure functions:** spawn schedule = `f(seed, night, index)`; zombie step
  = `f(zombie_state, player_pos, frame)` with hash stagger; fixed-point
  only, no wall clock, no runtime RNG, no uninitialized reads. Bottom
  screen renders state, never owns it.
- **Replay strategy:** recorded button lines replayed headlessly;
  HUD-text + telemetry-watch asserts (night reached, zombies alive, death
  frame) as in LD's CI tiers. **Slice 2 (next, NOT this slice) is the
  feasibility gate:** libnds-or-BlocksDS toolchain + headless
  melonDS/DeSmuME driving — minimal .nds built and boot-verified with a
  screenshot, or a six-field OWNER-ACTION naming the exact missing
  packages. Nothing in this doc claims anything builds yet.

## Cosmetic note for the owner (flag, not a blocker)

The repo is named `gba-homebrew` but will now host an NDS game per your
direction. Suggested layout: `games/gloamline-nds/` (mirrors the existing
`games/<name>/` convention and self-documents the platform); renaming the
repo is your call and nothing here depends on it.
