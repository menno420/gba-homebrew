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

## Prototype slice (this PR)

50-fish boids flock + the current + the reef + the win loop (save 40,
clock stamped, instant restart). Deliberately cut (the committed
concept's later rungs): predators locking onto stragglers, gates,
star ratings, multiple tuned levels, audio.

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
