# Session — Wickroad: MASTER tier on the title screen (runs>=100)

> **Status:** `in-progress`

- date: 2026-07-18 (branch `claude/wickroad-master-tier`, PR **[[fill: PR#]]**;
  born-red card written at 2026-07-18T09:10:22Z, flipped to complete at
  [[fill: flip ISO]], both from `date -u`).
- **📊 Model:** [[fill: model]] · high · gameplay-polish slice — grow the
  persistent Wickroad title tier tag from one tier to a descending first-match
  tier table keyed on lifetime run count.
- mission: **Wickroad — "THE MASTER TIER"** (a follow-on to PR **#184** the
  persisted best on the title, PR **#185** the NEW RECORD end-card flash, PR
  **#186** the lifetime RUNS count on the title, PR **#189** the run-count
  milestone flourish on the end card, and PR **#190** the persistent VETERAN
  tier tag on the title). #190 shipped `wr::run_tier_label` with a deliberately
  one-row descending table and an explicit forward note: add higher rows ABOVE
  the existing one to keep first-match-wins correct. This slice takes that
  next row. The title tier tag now reads a DESCENDING FIRST-MATCH table: runs
  >= 100 → "MASTER", 50..99 → "VETERAN", < 50 → nothing (nullptr). Still a
  persistent LEVEL keyed on the CURRENT stored lifetime run count (the value
  #186 renders as `RUNS n`), re-shown every boot, so `run_tier_label(49) ->
  nullptr`, `run_tier_label(50) -> "VETERAN"`, `run_tier_label(99) ->
  "VETERAN"`, `run_tier_label(100) -> "MASTER"`, `run_tier_label(101) ->
  "MASTER"`. The change is CONTAINED to the pure helper's table — NO new render
  plumbing: `main.cpp`'s `st_title` draw already appends whatever
  `wr::run_tier_label(best.runs)` returns beside `RUNS n`, so growing the
  table is the whole change. **Zero new RNG draws, `wr_telemetry` / `wr_art` /
  `wr_ledger` unchanged.** THE CONTRACT: pure label-string change, no state —
  the tag still draws ONLY inside the existing `ledger_loaded` gate, so the
  default no-save path (fresh / foreign / erased cart, any proof booted without
  `--savefile`) is byte-identical; the `else` branch renders EXACTLY the
  committed flavor line, unchanged. Branches from main (#184 + #185 + #186 +
  #189 + #190 in).
- landing posture: PR opened **READY** (not draft). Plain `claude/*` READY PRs
  auto-land server-side on green `rom-builds` under owner merge authority — no
  self-merge, no manual auto-merge arm from this session. substrate-gate is RED
  BY DESIGN while this card is `in-progress` (the born-red hold) and possibly
  additionally red on a pre-existing arcs/TILTSTONE.md orphan owned by another
  team — neither blocks the required `ROM builds` gate; flipping this card to
  `complete` (the deliberate LAST step) releases the born-red hold.

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/2026-07-18-wickroad-master-tier.md`, PR **[[fill: PR#]]**
   opened READY.
2. **The pure helper** (`games/wickroad/src/wr_milestones.h`): the descending
   tier table inside `wr::run_tier_label(int lifetime_runs)` grows from one row
   to two — `{ 100, "MASTER" }` ABOVE `{ 50, "VETERAN" }` — keeping the
   existing first-match-wins loop unchanged. Highest earned tier wins; below 50
   still returns `nullptr`. No state, no RNG. Function signature/shape and the
   `ledger_loaded`-gated call site untouched.
3. **The slice** (`games/wickroad/src/main.cpp`): NO change — the `st_title`
   draw already appends `wr::run_tier_label(best.runs)` (when non-null) beside
   #186's `RUNS n`, so the grown table surfaces "MASTER" at runs>=100 with zero
   new render plumbing. Verified at the call site.
4. **Host test** (`tools/check-run-tier.py`): the stdlib-only mirror grows to
   the same two-row table, sweeps 0..300, and pins the boundaries 49→none,
   50/51→"VETERAN", 99→"VETERAN", 100/101→"MASTER". `check-run-milestones.py`
   re-run for no-regression.
5. **Build**: [[fill: local build result or wall]] — the "ROM builds" gate is
   proven by CI `rom-builds.yml`.

## 💡 Session idea

[[fill: idea at flip — the one-row-table-with-a-forward-note pattern from #190
paid off: growing an established descending first-match table is a
single-row edit plus a boundary-pin in the mirror, no render or state change,
because the earlier slice deliberately shaped the helper to be extended.]]

## Previous-session review

[[fill: review at flip — prior slice `.sessions/2026-07-18-wickroad-veteran-tier.md`
(PR #190): whether its forward note ("add higher rows ABOVE this one") held up
in practice for this extension.]]
