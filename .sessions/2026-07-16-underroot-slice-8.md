# Session — Underroot arc slice 8: winter survival + the survival score

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/underroot-slice-8`, stacked on
  `claude/underroot-slice-7` @ a3d5866; started ~17:59Z, `date -u`)
- mission: **Underroot arc slice 8 — winter survival + the survival score.**
  The arc's marquee exam (docs/arcs/UNDERROOT.md Decision 3 + the slice-8 row):
  winter has **zero meadow food** (slice 7's `UR_ABUND_WINTER=0`), so the colony
  lives or starves on what the drawn tunnels banked. Winter drains the banked
  store by the surviving foragers' appetite — `store` falls by
  `pop × UR_CONSUME_PER_DAY` each of the `UR_WINTER_DAYS`; the colony **SURVIVES**
  iff the store never runs out, and **SCORE = surviving_pop×1000 + leftover_store**
  (0 if starved out). Everything a pure integer function of the drawn plan, so
  the host mirror recomputes survive/score bit-for-bit; the ROM reports
  winter-store/pop/drain/survive/leftover/score telemetry. The **marquee
  lockstep**: a committed *survivor* dig plan clears winter and a
  deliberately-too-small plan STARVES — both predicted host-side and confirmed
  in ROM telemetry.
- **📊 Model:** [[fill: family · effort · task-class]]
- landing posture: **DRAFT, stacked on #161**, base `claude/underroot-slice-7`.
  Under the standing 2026-07-16 LANDING WALL (PR ready-flips + auto-merge
  classifier-denied); honest draft-park is the terminal state. No merge / ready
  / auto-merge calls from this session; PRs #153–#161 untouched. Land order
  #153 → #155 → … → #161 → this.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-8.md`, first commit), PR opened DRAFT
   immediately, base `claude/underroot-slice-7`.
2. **The winter drain + survival score** in
   `games/underroot-nds/source/ur_sim.{h,c}` — [[fill: summarise on close]].
3. **Host mirror** `tools/check-underroot.py` — `prove_winter` +
   ci_fixture/marquee tuples — [[fill]].
4. **CI** (`nds-underroot-build`) — the marquee ROM proof — [[fill]].

## Known / honest gaps

[[fill: on close]]

## 💡 Session idea

[[fill: one genuine idea on close]]

## Previous-session review

[[fill: review of slice 7 / PR #161 on close]]
