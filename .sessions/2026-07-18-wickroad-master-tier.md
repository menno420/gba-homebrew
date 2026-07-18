# Session — Wickroad: MASTER tier on the title screen (runs>=100)

> **Status:** `complete`

- date: 2026-07-18 (branch `claude/wickroad-master-tier`, PR **#195**;
  born-red card written at 2026-07-18T09:10:22Z, flipped to complete at
  2026-07-18T09:16:15Z, both from `date -u`).
- **📊 Model:** Sonnet family · high effort · gameplay-polish slice — grow the
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
   `control/claims/2026-07-18-wickroad-master-tier.md`, PR **#195**
   opened READY; flipped to `complete` on green `ROM builds`.
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
5. **Build**: local ROM build unavailable in this clone (`arm-none-eabi-g++`
   not found, `third_party/butano` source absent) — the "ROM builds" gate is
   proven by CI `rom-builds.yml`, which went **green** on head `3e48250`
   (`ROM builds`, `NDS ROM build`, `NDS Underroot build`, `NDS Brineward build`
   all `success`; `substrate-gate` red by design — born-red hold now cleared +
   the pre-existing non-required TILTSTONE.md orphan).

## 💡 Session idea

**A descending first-match table shaped to grow turns each new tier into a
one-row edit — the cheapest kind of gameplay slice, so the real question is
not "can we add a tier" but "which tier earns its row."** #190 deliberately
built `run_tier_label` as a one-row table with a forward note ("add higher
rows ABOVE this one"); this slice cashed that in — MASTER cost exactly one C++
row + one mirror row + a boundary pin, zero render and zero state change,
because the seam was pre-shaped. That cheapness is a trap as much as a gift:
it invites piling on thresholds nobody reaches. The disciplined next moves,
in order of value: (1) a **GRANDMASTER tier at runs>=200** — same one-row
pattern, but only worth it if telemetry/playtest shows players actually reach
200 lifetime runs (a tier no one sees is dead weight on the title string's
`bn::string<64>` budget); (2) more valuable than another static row, a
**tier-up FLASH on the end card the run a boundary is first crossed** — reuse
`run_milestone_label`'s transient-flash surface (#189) to *announce* the
level-up (e.g. "MASTER" once at run 100) so the persistent title tag has a
paired "you just earned this" beat, closing the persisted→legible→announced
model #185 named for tiers the way it already exists for records. Idea (2) is
the one with player-facing juice; idea (1) is a trivial follow-on gated on
evidence the tier is reachable.

## Previous-session review

Prior slice: `.sessions/2026-07-18-wickroad-veteran-tier.md` (PR **#190**, the
persistent VETERAN tier tag) — **holds up, and its forward note was exactly
right.** #190 shipped the tier table with one row and an explicit invariant:
"add higher rows ABOVE this one to keep first-match-wins correct." This slice
followed that note literally — `{ 100, "MASTER" }` above `{ 50, "VETERAN" }` —
and the descending loop needed no change, confirming the note described a real,
stable extension point rather than aspirational tidiness. One honest self-review
of THIS slice: the change is genuinely minimal (helper table + host mirror +
boundary pins, main.cpp untouched — verified at the call site, not assumed),
and the pinned boundaries 49/50/51/99/100/101 catch the only off-by-one risk a
first-match table has (the rim between two live tiers), so the proof is
proportionate to the risk. The single thing to watch forward is string budget:
each tier row can lengthen `title_lines[1]` (currently `bn::string<64>`); MASTER
fits, but a third/fourth tier label should re-check the worst-case
`BEST GOLD … DAY … RUNS … <TIER>` length against that cap.
