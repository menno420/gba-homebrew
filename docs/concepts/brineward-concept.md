# Brineward — concept slice (new arc: original pirate naval-action, Nintendo DS)

> **Status:** `plan` — decision-ready; arc slice 1 of a SECOND original
> NDS game running beside the Gloamline arc (parallel arcs, zero shared
> game code). Genre-inspired by browser naval-action games of the Pirate
> Storm era — the LOOP is the inspiration, nothing else: 100% original
> IP — title, art, audio, code; no external names, assets, or mechanics
> lifted verbatim. Title checked against commercial collisions
> (2026-07-11 web sweep): no game, brand, or studio named "Brineward"
> exists — nearest hits are an unrelated FFXIV free company ("Brineward
> Pact") and a spell name inside one indie RPG; also swept and clean but
> not picked: Keelfire, Squallmark. Nothing in this doc claims anything
> builds yet.

## Pitch

You are a freelance gun-captain with a leaky sloop and a debt to the
harbormaster of Graywake. Sail out past the breakwater, pick fights you
can win — broadside duels against rum-runners, revenue cutters, and the
things with too many arms that drag ships under — and limp back to port
with a hold full of salvage. Sell it, patch the hull, buy one more
cannon, sail out again. Each run out is a bet: the farther brineward you
push, the richer the wrecks and the worse the weather. Sink and you lose
the cargo, never the ship. Score is gold banked and waters charted.
Short runs, instant restart, arcade loop — the shape this lane has
proven three times.

## Core loop — the naval-MMO loop, reframed single-player

The genre ancestor is a massively-multiplayer grind: sail open water,
trade broadsides, kill sea monsters, sell loot in port, upgrade
hull/cannons/sails, repeat. Single-player on a handheld, that loop
reframes as a **run-based out-and-back**: port is the safe hub between
runs, open water is the run, and the upgrade treadmill becomes the
between-runs economy. No other captains needed — the ocean itself is
the opponent ladder.

- **Sail** (continuous): your ship has heading and momentum, not
  8-way position. LEFT/RIGHT turn, the ship always makes way; sail
  trim (UP/DOWN through half/full/battle sail) trades speed against
  turn rate. Water is an open scrolling field with islands, reefs, and
  wrecks — not a corridor.
- **Fight** (broadside geometry, the whole skill ceiling): cannons fire
  in fixed arcs off port and starboard, NEVER forward. L fires the
  port battery, R fires starboard, each on its own reload. The entire
  duel is positional: turn your side toward them while staying out of
  theirs. Range falloff + a short ball flight time make closing angle
  matter. Getting raked while reloading is how you die; crossing their
  stern while they reload is how you win.
- **Monsters** (the PvE ladder's teeth): things that do NOT fight like
  ships — a maw that surfaces under you (watch the shadow), arms that
  grapple and hold you still while cutters close in. They break the
  broadside geometry on purpose; ships teach the rules, monsters bend
  them.
- **Loot**: sunk enemies drop flotsam (crates bobbing on the swell);
  sail through to scoop. Hold space is finite — deep-water salvage is
  worth more per crate, so greed has a shape: one more fight or turn
  for home?
- **Port**: dock at Graywake to bank gold (banked gold is safe
  forever), repair, and buy upgrades across three tracks — **hull**
  (max HP), **cannons** (damage / reload / extra barrels per
  broadside), **sails** (speed / turn rate). Sinking forfeits only the
  unbanked hold: runs stay tense, the treadmill stays kind.
- **Session length:** 3–6 minute runs; sink or dock, instant restart
  from port. Difficulty is spatial, not temporal — danger scales with
  distance from the breakwater (concentric danger bands), so the player
  sets their own ramp by where they dare to sail.
- **Fun in 30 seconds:** you cut across a rum-runner's stern, L-broadside
  at half reload, three crates bob up in your wake — and the water ahead
  goes dark under something big turning your way. Hold or run home?

## Dual screens (NDS-native, cheap)

- **Top screen — the water:** the sailing/combat view. Your ship
  centered-ish, enemies, ball splashes, flotsam, the swell. All action
  here; all of it playable on buttons alone.
- **Bottom screen — the chart table:** sea chart (explored water,
  port bearing, danger bands, enemy pips) + ship status (hull bar,
  port/starboard reload bars, sail trim, hold count, unbanked gold).
  A pure render of the same sim state, so it costs nothing
  determinism-wise (Gloamline's watch-map rule).
- **Touch optional, never required** (Gloamline's proven rule): tap the
  chart to drop a heading marker, tap shop rows in port. Every touch
  action has a button path; the game is 100% playable on buttons.

## Controls sketch (buttons-first)

| Input | At sea | In port |
|---|---|---|
| D-pad LEFT/RIGHT | turn (rate set by sail trim) | menu move |
| D-pad UP/DOWN | sail trim up/down (full / half / battle sail) | menu move |
| L | fire port broadside (own reload) | — |
| R | fire starboard broadside (own reload) | — |
| A | interact (dock, scoop confirm if needed) | buy/confirm |
| B | — (reserved: later ram/brace verb) | back |
| START | pause / restart after sinking | leave port |
| Touch (optional) | chart marker | shop taps |

## Progression / economy sketch

- One currency: **gold**. Flotsam value scales with danger band
  (band 0 crates ~5g, band 2 ~25g, monster drops more). Hold cap starts
  at 8 crates.
- Banked gold buys upgrade tiers, flat and readable:
  hull I/II/III (HP 100/150/220), cannons I/II/III (reload and balls
  per broadside), sails I/II/III (speed / turn). Tier prices roughly
  triple per step so band-2 runs stay necessary, never optional.
- Sinking: lose the hold (unbanked crates), respawn at port with full
  (repaired) hull. Banked gold and upgrades NEVER lost. Repairs after a
  damaged-but-docked run cost a small fraction — coming home hurt is
  strictly better than sinking.
- Score surfaced on the HUD and the sinking/dock cards: gold banked,
  deepest band reached, ships sunk, monsters sunk (Gloamline's
  nights-survived surface, reskinned).

## Art / audio direction (original, NDS-feasible)

- **Art:** all code-authored, like both shipped games — no external
  assets. Top-down ships as 16x16/32x32 OAM sprites (hull silhouette +
  sail state frames), sea as a tiled BG with a cheap two-frame swell
  shimmer, ball splashes and muzzle smoke as short sprite bursts.
  Palette: slate sea, bone sails, tar hulls, one hot accent for muzzle
  flash and gold. Monsters read by silhouette (shadow under the water
  BEFORE the sprite surfaces — telegraphing is the difficulty knob).
  NDS OAM gives 128 sprites/engine; budget ~6 ships + balls + flotsam
  + monster segments, comfortably under Gloamline's proven cap
  discipline.
- **Audio:** deferred to a later slice (as Gloamline did), but the
  direction is the proven `generate_audio.py` synthesized pipeline:
  cannon thump (noise burst + low sine), hull crack, splash hiss,
  a two-note dock chime. No sampled or licensed audio, ever.

## Determinism + host-provability plan (the lane's non-negotiables)

- **Pure functions:** enemy spawn table = `f(seed, band, index)`;
  enemy sail AI = `f(enemy_state, player_state, frame)` with the hash
  stagger pattern (no runtime RNG); ball flight and damage fixed-point
  only. Bottom screen renders state, never owns it.
- **Seed policy:** frame-latched at START, printed on HUD and
  sink/dock cards (Gloamline's rule) — any run reproducible; CI scripts
  a fixed press frame.
- **Host mirror:** `tools/check-brine.py`, line-for-line mirror of the
  pure sim (`bw_sim.c`), lockstep rule — asserting spawn validity (no
  spawn inside the harbor safe radius, band counts correct), broadside
  damage math, and a duel-convergence property (an idle player IS
  eventually sunk; a scripted circling player CAN win) before any
  emulator boots.
- **Telemetry mailbox:** `bw_telemetry[16]` from day one (positions,
  headings, hulls, reloads, gold, state) — every headless proof is a
  numeric assert, per the Gloamline pattern.
- **Toolchain (all proven, reuse as-is):** BlocksDS 1.21.1 via the
  sha256-pinned `tools/setup-nds-toolchain.sh`; headless DeSmuME via
  `tools/nds-headless-check.py` with `--elf/--watch/--assert-watch`;
  respect the `keypad_update(0)` frame-1 boot trap (PLATFORM-LIMITS)
  in every new proof. CI: a **separate `nds-brineward-build` job** in
  `rom-builds.yml` shaped exactly like `nds-rom-build` — Gloamline's
  job, its proofs, and the required GBA "ROM builds" gate are NOT
  touched by this arc, ever.

## Walking-skeleton cut (v0 — arc slice 3, after a slice-2 scaffold) vs later

**SKELETON (v0):** one open-water screen worth of sea around the
breakwater, player sloop with heading/momentum sailing + 3-state sail
trim, ONE enemy sloop that sails an intercept-and-circle pattern, L/R
broadsides with reload + range falloff on BOTH ships, hull bars,
sink-or-be-sunk → card → START instant restart, HUD (hull / reloads /
seed), bottom screen = chart v0 (two pips + port bearing), telemetry
mailbox + host-mirror proof + pinned headless proofs (boot; sail
kinematics exact-position; duel-loss idle; duel-WIN via a recorded
route). No loot, no port menu, no monsters in v0 — the broadside duel
IS the game, so it ships first and alone.

**LATER SLICES (roadmap, one per slice):**
1. loot + gold: flotsam scoop, hold cap, dock-to-bank at the pier,
   sink-forfeits-hold;
2. port + upgrades: Graywake menu, the three tracks, repair costs;
3. first sea monster (the Maw: shadow telegraph → surface → lunge) —
   the break-the-geometry enemy class;
4. wind + sailing feel: a slow-rotating wind vector, trim interacts
   with it (owner-taste slice, hand-feel gated);
5. danger bands + regions: band spawn tables, reefs/islands, deepest
   band = charted-waters score;
6. synthesized audio set;
7. saves: banked gold / upgrades / chart persist (SRAM pattern already
   proven on GBA; NDS save path is a known open question below).

## Honest open questions

- **Save persistence on NDS is UNPROVEN in this repo** — Gloamline has
  no saves yet either; DeSmuME headless save-flash behavior needs a
  feasibility probe before slice 7 promises anything. (The GBA lane's
  mGBA savefile segfault taught us to probe this early and headlessly.)
- **Momentum sailing + ball flight time is a bigger sim step than
  Gloamline's grid-free kinematics** — fixed-point heading/velocity
  math is fine, but the duel-WIN headless proof needs a recorded route
  against a deterministic AI; budget slice-3 time for a route-recorder
  tool (session 17's guard recipe already recommends promoting one).
- **Enemy sail AI tuning is owner-taste** — intercept-and-circle can be
  provably losable/winnable and still feel dumb; numbers land
  provable-first (the lane's pattern), feel passes owner-gated.
- **Does a second live arc strain CI minutes?** Two NDS jobs per PR
  once Brineward builds; if it bites, path-filter the two NDS jobs per
  game dir (a later, deliberate workflow change — flagged here so it's
  a decision, not drift).
- **Repo name** (carried from Gloamline's doc): `gba-homebrew` now
  hosts two NDS arcs; rename remains the owner's call, nothing depends
  on it.
