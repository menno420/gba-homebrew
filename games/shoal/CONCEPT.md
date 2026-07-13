# Shoal — concept

> **Status:** `reference` — the one-page concept for breadth game #8.
> This is the ORDER-001 concept queue's item 3 — its LAST item —
> (`docs/concepts/session-1-concepts.md`), promoted to a playable
> prototype slice under ORDER 005; the original committed pitch,
> scope ladder, risk note and THE PROFILING GATE live in that doc.

## Pitch

You steer a current — a cursor that pushes water — to herd a school
of 50 fish into the safe reef. The fish are a boids flock (cohesion /
alignment / separation), so the game feel is herding smoke: pressure,
not control. Sweep behind the school, watch it bloom away from your
push, gather the stragglers, bring them home. The clock is the score.

## Genre

Flock-herding action puzzler (indirect control; the boids ARE the
game feel).

## Core loop

Read the school's shape → press the current in behind it (hold A) →
the school streams away from your push toward the reef → fish that
reach the reef settle and are SAVED → circle back for stragglers →
save 40 of 50 → the shoal is home; clock and saves are the brag.
START reruns the identical seeded school.

## Target platform

Game Boy Advance (Butano; fish are one-glyph text sprites moved per
frame — no local assets). Ships as `dist/shoal.gba`.

## THE GATE (the concept doc's own words, honored first)

"The flock is the game — if 50 boids don't fit the CPU/OAM budget the
concept degrades fast (mitigations: neighbor grids, 16.16→8.8 fixed
precision drop, flock size tuning)... profile in the FIRST session,
pivot cheap if the budget says no."

**Measured verdict (this session, headless mGBA, Butano's core CPU
meter pinned in the telemetry mailbox; 4096 = one frame):**

| build | mean | worst steady frame |
|---|---|---|
| naive O(n²), THUMB in ROM | **120.5%** — FAIL | 181% |
| + 24px neighbor grid | 103.3% — FAIL (the school BALLS UP by design: cohesion defeats the grid exactly when the flock flocks) | — |
| + deterministic 8-neighbor cap | 96.9% — FAIL (bounds the work, not the wait-stated fetches) | — |
| same code, ARM in ROM (misplaced "IWRAM" — the build rule alone does NOT place the section) | 93.4% | — |
| **grid + cap, ARM in REAL IWRAM** (`section(".iwram")`) | **52.4% — PASS** | **68.6%** (2810/4096); 67.7% (2772) through the full scripted herd |

OAM: 50 fish + cursor + HUD text ≈ 60 sprites — far under the
128-sprite budget. **The budget says yes; the boid slice ships** (no
pivot needed). The mailbox carries the CPU words permanently, so the
budget is a pinned regression test, not a one-time claim.

## Prototype slice (PR #98) + growth rungs 1-3 (PRs #99 / #100 / #101)

50-fish boids flock + the current + the reef + the win loop (save 40,
clock stamped, instant restart). **Growth rung 1 (the predator pass,
PR #99): THE HUNGRY WATER** — SELECT starts a run with two hunters
(START keeps the calm water, so every calm pin holds by construction).
Every 48 frames each hunter locks the fish farthest from the flock
centroid — but ONLY a genuine straggler (>44 px out): **a tight or
re-cohered school starves them; abandonment, not existence, is what
kills** (measured: without the threshold the committed sweep lost
34/11 — leftovers were farmed faster than they re-cohered). A caught
straggler is eaten; the hunter dens up for 300 frames. The hungry
water asks for 35 home (the committed concept's "star rating = fish
saved" grading, per water; 40 is provably out of reach against the
hunters — the same sweep that banks 40 calm banks 35/8 hungry, and a
scatter-in-place loses 16). **Growth rung 2 (the gates, PR #100):
THE GATED RUN** — R starts a run where two static coral walls with
OFFSET gaps (gap 0 high at x=104, gap 1 low at x=156) stand between
the school and the reef: the committed pitch's "past predators,
through gates, into the safe reef", built as pure static geometry
(position tests, no per-frame state). A fish pressed into the coral
is ejected the way it came with its approach damped, so the school
must be FUNNELED — compressed into a gap lane and pumped through,
then snaked down to the other gap. The same no-input run that banks
5 fish in calm water banks ZERO gated: the walls, and only the
walls, hold the school. Goal stays 40 (calm rules — no hunters, so
the hungry water's coupled difficulty knob, straggler 44 px + den
300 + goal 35, is untouched); the committed funnel route banks 40/50
at run-frame 3519 (~59 s — the clock is the brag). **Growth rung 3
(star ratings proper, PR #101)**: the committed pitch's own sentence
— "Star rating = fish saved" — mechanized as a pure display-time
lookup graded PER WATER: the 3-star line IS each water's existing
goal (calm/gated 40, hungry 35 — the coupled hungry knob 44 px /
den 300 / goal 35 untouched), 2 and 1 stars at thirds of it
(calm/gated 14/27/40, hungry 12/24/35). Stars ride the HUD live as
the school banks, and BOTH end cards grade the run — a scattered
shoal keeps whatever stars its saves earned, and a near-goalless one
reads "RATING -"; grading losses is what makes the ratings proper.
The sim never consults a star: zero game-state drift in any water
(proven cross-ROM against the v0.3 dist). Deliberately still cut:
multiple tuned levels, audio.

Hungry-water frame budget (same meter as the gate, final build):
idle mean 50.0%, p99 70.4%, steady worst 75.1% (3075/4096); 74.0%
(3033) through the winning herd — the `t[5] < 4096` regression rail
now covers predator-live frames too. Predator pass in IWRAM at
0x030006d8 (map-verified).

Gated-run frame budget (same meter, rung-2 build): the winning
funnel route runs mean 35.9%, p99 57.2%, worst 71.2% (2916/4096)
over its 3517 gate-active frames; the gated idle school holds 72.0%
(2949) worst. The rail covers gate-active frames too. Gate pass in
IWRAM at 0x03000a2c (map-verified). The proof suite (input scripts
included — they are part of the pin) is committed at
`games/shoal/proofs.sh`.

## Determinism

One xorshift32 at fixed seed `0x510A17E5`, consumed only at school
init; integer/fixed-point math only; the pair pass runs in whole-pixel
space (the precision-drop mitigation). Same input script = identical
run (proven run-twice byte-identical). Telemetry mailbox
`sh_telemetry[16]` every frame.

## Sellability guess

Honest: near-zero as a paid standalone in glyph presentation — but of
the whole breadth set this is the strongest DEMO piece ("50 boids on
a GBA at 52% CPU" is a homebrew-scene calling card), and herding-smoke
gameplay with sprite art + predators + 10 levels is a plausible small
itch title. Flash-cart/itch.io niche as-is.
