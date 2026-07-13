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

- Oxygen meter + air-pocket pickups (adds a reason to leave the safe line).
- Hazards/creatures in the channel (jellyfish that drift on the seeded RNG).
- Daily seeded run with a shareable score URL (`?seed=YYYYMMDD`).
- Cosmetics (diver skins, bubble trails) — pure render-side, zero sim risk.
- Ghost replays of your best run — nearly free because the sim is
  deterministic: store the input timeline, replay it against the same seed.
