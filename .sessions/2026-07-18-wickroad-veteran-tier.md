# Session — Wickroad: persistent VETERAN tier tag on the title screen

> **Status:** `in-progress`

- date: 2026-07-18 (branch `claude/wickroad-veteran-tier`, PR **#[[fill: PR number]]**;
  born-red card written at 2026-07-18T02:18:30Z from `date -u`).
- **📊 Model:** Opus 4.8 · high · new-logic gameplay slice — a deterministic
  persistent tier tag on the Wickroad title screen keyed on lifetime run count.
- mission: **Wickroad — "THE VETERAN TAG"** (a follow-on to PR **#184** the
  persisted best on the title, PR **#185** the NEW RECORD end-card flash, PR
  **#186** the lifetime RUNS count on the title, and PR **#189** the run-count
  milestone flourish on the end card). #189's own session idea named this exact
  next surface: the milestone flourish flashes ONCE, on the end card of the
  crossing run, then is gone — a player who powers off never re-sees they hit
  50. This slice makes the achievement PERSIST: when the SRAM ledger's lifetime
  run count reaches a tier threshold, the TITLE screen shows a persistent tier
  tag beside #186's existing `RUNS n` line (runs >= 50 → "VETERAN"). Unlike
  #189's transient crossing flash, this reads the CURRENT stored lifetime total
  — a persistent LEVEL, not a crossing event — so `run_tier_label(49) ->
  nullptr`, `run_tier_label(50) -> "VETERAN"`, `run_tier_label(200) ->
  "VETERAN"`, and it re-shows every boot. The decision is a SECOND PURE helper —
  `wr::run_tier_label(int lifetime_runs)` in `games/wickroad/src/wr_milestones.h`,
  a deliberately SEPARATE function from `run_milestone_label` (ordinal-crossing
  for the end card) since it is a different concern (current-total level vs
  crossing event), no Butano/GBA headers — so it is trivially inspectable and
  host-testable; a stdlib mirror `tools/check-run-tier.py` proves the tier table
  for every count. **Zero new RNG draws, `wr_telemetry` / `wr_art` /
  `wr_ledger` unchanged.** THE CONTRACT: the tag draws ONLY inside the existing
  `ledger_loaded` gate on the title, appended to the same best line #186 owns, so
  the default no-save path (fresh / foreign / erased cart, any proof booted
  without `--savefile`) is byte-identical — the `else` branch renders EXACTLY the
  committed "BUT THE INK AGES" flavor line, unchanged. Branches from main
  (#184 + #185 + #186 + #189 in).
- landing posture: PR opened **READY** (not draft). Plain `claude/*` READY PRs
  auto-land server-side on green `rom-builds` under owner merge authority — no
  self-merge, no manual auto-merge arm from this session. substrate-gate is RED
  BY DESIGN while this card is `in-progress` (the born-red hold) and
  additionally red on a pre-existing arcs/TILTSTONE.md orphan owned by another
  team — neither blocks the required `ROM builds` gate; flipping this card to
  `complete` (the deliberate LAST step) releases the born-red hold.

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/2026-07-18-wickroad-veteran-tier.md`, PR **#[[fill: PR number]]**
   opened READY.
2. **The pure helper** (`games/wickroad/src/wr_milestones.h`): a SECOND
   self-contained, Butano/GBA-free function `wr::run_tier_label(int
   lifetime_runs) -> const char*` returning a persistent tier label for the
   current stored lifetime total or `nullptr`. Small descending tier table,
   first-match-wins; starts with one tier `runs >= 50 -> "VETERAN"`. No state,
   no RNG. Deliberately NOT an overload of `run_milestone_label` — different
   concern (current level vs crossing event).
3. **The slice** (`games/wickroad/src/main.cpp`): the `st_title` render path
   appends `wr::run_tier_label(best.runs)` (when non-null) to the existing best
   line beside #186's `RUNS n`, inside the existing `ledger_loaded` branch. No
   RNG/telemetry/art/ledger change; the no-save `else` path is byte-identical.
4. **Host test** (`tools/check-run-tier.py`): a stdlib-only mirror of the
   helper (the check-run-milestones.py convention), sweeping 0..200 asserting
   only runs >= 50 yield "VETERAN", boundary 49 -> none / 50 -> "VETERAN"
   pinned, negatives -> none. Exit 0 = green.
5. **Build**: local ROM build unavailable in this clone (no
   `third_party/butano` source) — the "ROM builds" gate is proven by CI
   `rom-builds.yml`.

## 💡 Session idea

[[fill: one genuine next-slice idea at flip]]

## Previous-session review

[[fill: one-line review remark on the #189 prior slice at flip]]
