# Undertow — concept

> **Status:** `reference`

## Pitch

You are a free-diver dropping into a bottomless ocean trench. The trench
walls scroll past faster and squeeze tighter the deeper you go; one touch
and the dive is over. Depth is the only score, and every run of the same
seed plays out identically — a dare you can hand to someone else.

## Genre

One-hand arcade survival / endless diver (vertical "flappy-wall" family:
steer a single axis, survive a procedurally generated corridor).

## Core loop

Steer left/right to stay in the open channel → the channel drifts, narrows,
and the descent accelerates → touch a wall, die, see your depth → tap once
to dive again. Runs last on the order of 30 seconds, which is the point:
the retry is cheaper than the disappointment.

## Target platform

The owner's web arcade. Static single page (index.html + game.js, no build,
no server, no external requests), desktop keyboard (arrows / A,D) and mobile
touch (left/right half of the canvas).

## Sellability guess (honest)

Near-zero as a standalone paid title — this mechanic is a known genre with
hundreds of free implementations. Reasonable as free arcade filler: it earns
session time and ad impressions because runs are short and retries are
one tap. The one differentiator worth building on is determinism: every run
is fully seeded, so a shared daily seed with a comparable leaderboard is
cheap and is the only version of this game with a reason to return.

## Growth path

- Oxygen meter + air-pocket pickups (adds a reason to leave the safe line) —
  **BUILT** (growth cut 4): the tank drains every frame, faster with depth;
  air pockets spawn inside the channel from a side-band RNG stream derived
  from the seed (a fixed two draws per row — the wall stream draws exactly
  what it drew before, so a seed's channel layout is unchanged); contact
  refills the tank (pickups never alter motion); an empty tank ends the run
  like a crash ("OUT OF AIR"). This is a SIM change — run outcomes for a
  given seed legitimately moved (seed-7 no-input: crashFrame 823 / 194m on
  the wall → 810 / 191m out of air). Oxygen and pockets live inside the sim
  instance, so ghost replays stay exact by construction; ghost records were
  re-versioned (v1 → v2) and stale pre-oxygen records are dropped cleanly.
- Hazards/creatures in the channel (jellyfish that drift on the seeded RNG).
- Daily seeded run with a shareable score URL (`?seed=YYYYMMDD`) —
  **BUILT** (growth cut 1): `?daily=1` derives the seed from the UTC date
  as YYYYMMDD (read once at boot, never in the sim step); the gameover
  screen shares a `?seed=N&depth=M` challenge link (`S` copies it; the
  `depth` param is render-only — a title-screen score to beat). Static
  hosting only — the URL itself is the share, no server, no leaderboard.
- Cosmetics (diver skins, bubble trails) — pure render-side, zero sim risk —
  **BUILT** (growth cut 2): four skins (diver colors + a bubble-trail draw
  style each), selected via `?skin=ID` or a `C` key-cycle on the
  title/gameover screens, persisted to guarded `localStorage`. Skins touch
  rendering only — the sim step and both RNG streams are byte-identical
  across skins, and the smoke asserts identical crashFrame/depth for the
  same seed under a non-default skin.
- Ghost replays of your best run — nearly free because the sim is
  deterministic: store the input timeline, replay it against the same seed —
  **BUILT** (growth cut 3): every run records its input timeline
  (run-length encoded steer per frame); the best run per seed persists to
  guarded `localStorage` (`undertow.ghost.<seed>`); on later runs of the
  same seed a translucent ghost diver replays it in lockstep — a second
  sim instance (own RNG, own trench rows) fed the stored inputs,
  render-only presence with zero contact with the live run's sim or RNG.
  The live run is byte-identical with the ghost on or off, and a stored
  timeline replays to the original crash frame and depth exactly (both
  asserted by the smoke). `?ghost=0` opts out, render-side only.
