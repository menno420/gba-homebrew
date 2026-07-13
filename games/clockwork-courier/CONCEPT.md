# Clockwork Courier — concept

> **Status:** `reference` — the one-page concept for breadth game #7.
> This is the ORDER-001 concept queue's item 2
> (`docs/concepts/session-1-concepts.md`), promoted to a playable
> prototype slice under ORDER 005's breadth program; the original
> committed pitch, scope ladder and risk note live in that doc.

## Pitch

A courier robot in a clockwork tower delivers parcels to timed chutes.
The hook: pressing R rewinds YOU 5 seconds — but leaves a GHOST that
replays exactly what you just did. You cooperate with your own past
self: have it stand on the pressure switch that holds the door open
while present-you runs the parcel through. Levels are single-screen
puzzles; the clock is your score.

## Genre

Rewind-ghost puzzle platformer (single-screen, deterministic — the
"cooperate with your own recording" mechanic of speedrun-ghost fame,
promoted to being the entire puzzle language).

## Core loop

Read the room → act out the future partner you will need (walk to the
switch, stand on it) → R: time snaps you back 5 seconds and your ghost
sets off to repeat it → race your own past (grab the parcel, thread
the door while your ghost holds it) → chute! The delivery clock is the
score; fewer seconds and fewer rewinds are the brag.

## Target platform

Game Boy Advance (Butano, text-glyph presentation from the shared
common fonts — no local assets). Runs in any GBA emulator; ships as
`dist/clockwork-courier.gba`.

## Prototype slice (PR #96) + growth rung 1 (PR #105)

Movement (LEFT/RIGHT walk, A jump, fixed-point gravity + tile
collision), ONE handcrafted single-screen level whose door DEMANDS the
mechanic, the full rewind-ghost contract (5 s pose ring buffer; R
snaps you back and spawns the replaying ghost; the switch counts the
ghost's weight; the door never crushes — it holds while occupied), a
parcel, a chute, a delivery clock, instant restart. **Growth rung 1
(GHOST-AS-PLATFORM, PR #105): your past self is a one-way platform** —
fall onto the ghost's head and stand (it never blocks you sideways or
from below); riding follows the replay, jumping dismounts. The
proof-ground is the BELL LEDGE above the chute corridor: the door
wall now runs to the ceiling and the ledge tops out at 32 px, while a
max jump from flat ground peaks at feet 38.76 px (measured, pinned)
and from a ghost's head (feet 56 px) at 30.76 px — only a boosted
jump inside the corridor lands there, and the committed counter-solve
probes (max-jump+drift, under-ledge bump, door-side hop) are asserted
to FAIL. The committed platform solve delivers with REWINDS 2,
detouring onto the ledge mid-run. Deliberately still cut: multiple
parcels/chutes with timing windows, more levels, audio.

## Determinism

No RNG anywhere — a handcrafted level and input-driven physics: the
same input script replays bit-identically (the concept doc's own bet:
"the headless harness can literally assert it"). The ghost replays
recorded POSES, so replay exactness is by construction, and the
headless proof pins ghost positions to the same literals as the
original walk. Telemetry mailbox `cc_telemetry[17]` is written every
frame for `tools/headless-screenshot.py --elf --watch` asserts
(word 16 = standing-on-ghost, rung 1); the proof suite is committed
at `proofs.sh`.

## Sellability guess

Honest: near-zero as a paid standalone in this glyph presentation —
but the MECHANIC is the demo: rewind-ghost puzzlers (The Misadventures
of P.B. Winterbottom, Braid's time themes) have real shelf presence,
and this prototype proves the hard part (exact replay determinism) on
real handheld constraints. Flash-cart/itch.io homebrew niche as-is;
a sprite-art + 10-level cut would be a plausible small itch title.
