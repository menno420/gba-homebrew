# Session — Underroot arc slice 10: Seed dial + balance (pick/scan the year seed; a difficulty/escalation pass)

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/underroot-slice-10`, stacked on
  `claude/underroot-slice-9` @ 19660ac; started ~18:40Z, `date -u`)
- mission: **Underroot arc slice 10 — Seed dial + balance** (docs/arcs/UNDERROOT.md
  slice-10 row): pick/scan the year seed with a runtime SEED DIAL, thread the
  dialed seed through the whole `f(seed,season,index)` schedule, and land a
  difficulty/escalation pass whose balance invariant — **a fair seed is always
  survivable with the reference plan** — is proven headless-side. Plus slice-9's
  forward note: **gate the START record-commit on a realized winter**
  (`season == UR_WINTER`), so the persisted best is an outcome the colony
  actually reached, not a spring forecast. Determinism-first throughout: the
  dial→seed wiring, the reference plan and its winter score are pure integer
  functions, mirrored line-for-line in `tools/check-underroot.py`.
- **📊 Model:** Claude Opus 4.8 family · high · task-class: gameplay increment
  (seed selection / balance + save-gate)
- landing posture: **DRAFT, stacked on #163**, base `claude/underroot-slice-9`.
  Under the standing 2026-07-16 LANDING WALL (PR ready-flips + auto-merge
  classifier-denied); honest draft-park is the terminal state. Card stays
  `in-progress` — no ready-flip, no merge / auto-merge calls from this session;
  PRs #153–#163 untouched. Land order: #153 → #155…#163 → this.

## What shipped

_(filled at close-out — see the impl summary in PR / CI below.)_

## 💡 Session idea

_(filled at close-out.)_

## Known / honest gaps

_(filled at close-out.)_

## Previous-session review

_(filled at close-out.)_

## PR / CI (filled at close-out)

- PR: _(filled at close-out)_
- CI (`nds-underroot-build` + the gates): recorded in the session report /
  status; substrate-gate red is the inherited #151 orphan set, not this slice.
