# Session — Underroot arc slice 5: nurseries + population

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/underroot-slice-5`, stacked on
  `claude/underroot-slice-4` @ a0c2f78; started ~16:37Z, `date -u`)
- mission: **Underroot arc slice 5 — nurseries + population.** Add the
  population layer: a **third stylus verb** (hold L + tap a dug cell)
  designates that dug cell a **nursery**; connected nursery cells brood
  foragers, converting the granary-banked food (slice 4's store) into a
  **population** on a pure, bounded schedule — `pop = min(connected ×
  UR_NURS_BROOD, store / UR_FOOD_PER_ANT)` capped at `UR_POP_CAP`. The arc's
  connect-or-waste rule extends to broodkeeping: a **disconnected nursery
  produces nothing** (a designated pocket with no corridor to the mouth broods
  zero), just as an unreachable granary banks nothing and an unreachable patch
  feeds nothing. Proven host-side (pop deterministic/bounded, the food-bound /
  brood-bound / cap branches, disconnected nurseries excluded) plus a ROM
  `ur_telemetry` pop lockstep over a scripted dig+granary+nursery fragment —
  the slice-5 row of `docs/arcs/UNDERROOT.md`.
- **📊 Model:** [[fill: Opus family · <effort> · feature build]]
- landing posture: **PR opened DRAFT and left DRAFT, base
  `claude/underroot-slice-4` (stacked).** The standing 2026-07-16 LANDING
  WALL (PR ready-flips + auto-merge classifier-denied — recorded in
  `docs/PLATFORM-LIMITS.md` and the #153/#155/#156/#157/#158 bodies, all parked
  DRAFT at this same wall) makes honest draft-park the terminal state; the
  land path is an owner ready-click, in stack order
  #153 → #155 → #156 → #157 → #158 → this. No merge / ready / auto-merge calls
  from this session; the pre-existing PRs (#153–#158) untouched.

## What shipped

[[fill: numbered list — born-red gate; the pure nursery/pop layer in
ur_sim.{h,c} naming each new function; main.c render+input+telemetry; the
host mirror + prove_nursery(); the CI pop lockstep — with telemetry indices
and CI assert numbers]]

## Known / honest gaps

[[fill: no local NDS build; pop telemetry pinned from host mirror not a read
ROM run; one ROM pop scenario (food-bound); pop is instantaneous not yet
time-scheduled (slice 7's year clock); season fixed spring; stacked on an
unlanded base]]

## 💡 Session idea

[[fill: one genuine session idea]]

## Previous-session review

[[fill: review of the slice-4 card / PR #158]]
