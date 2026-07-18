# Session — Wickroad: tier-up flash on the end card (the run a tier is first earned)

> **Status:** `in-progress`

- date: 2026-07-18 (branch `claude/wickroad-tier-up-flash`, PR **#[[fill:pr]]**;
  born-red card written at 2026-07-18T21:10:28Z, flipped to complete at
  [[fill:flip-ts]], both from `date -u`).
- **📊 Model:** Claude Opus family · high effort · new-logic gameplay slice —
  a transient END-CARD announce the run a lifetime tier is first crossed, the
  paired "you just earned this" beat for the persistent title tier tag.
- mission: **Wickroad — "THE TIER-UP FLASH"** (a follow-on to PR **#189** the
  run-count milestone flourish and PR **#190**/**#195** the persistent VETERAN
  /MASTER title tier tag). #195's own session idea named this exact next
  surface as the one "with player-facing juice": the persistent tier tag
  (#190/#195) re-shows the earned level every boot, but nothing *announces* the
  moment it is earned. This slice adds the announce: on the run-end whose banked
  lifetime ordinal FIRST reaches a tier threshold (50 → VETERAN, 100 → MASTER),
  the end card shows a one-line "NEW TIER VETERAN" / "NEW TIER MASTER" callout —
  the level-up analog of #185's "NEW RECORD" beat, closing the
  persisted→legible→announced loop for tiers the way #185 closed it for records.
  The crossing decision is a PURE helper — `wr::run_tier_up_label(int
  completed_run_ordinal)` in `games/wickroad/src/wr_milestones.h`, no Butano/GBA
  headers — and it is DERIVED from the single `wr::run_tier_label` table (the
  same descending table #190/#195 built), so the tier thresholds/labels have
  ONE source and cannot drift: the flash fires exactly when
  `run_tier_label(ordinal) != run_tier_label(ordinal - 1)` (this ordinal reaches
  a tier the previous ordinal had not). A stdlib mirror
  `tools/check-run-tier-up.py` proves the crossing for every reachable ordinal.
  Following #189's discipline the ordinal is captured INSIDE `record_run()`
  BEFORE `++best.runs`, so the 50th completed run-end (pre-increment
  `best.runs == 49`, ordinal 50) triggers "NEW TIER VETERAN" — the same count
  #186 shows as `RUNS 50` and #195 tags "VETERAN". **Zero new RNG draws,
  `wr_telemetry` / `wr_art` / `wr_ledger` unchanged.** THE CONTRACT: the callout
  draws ONLY inside the existing `ledger_loaded` gate on a previously-unused
  card slot (`title_lines[4]`), so the default no-save path (fresh / foreign /
  erased cart, any proof booted without `--savefile`) is byte-identical —
  `clear_lines()` blanks the slot each transition, so it stays clear on a
  non-crossing run-end. Branches from main (#184 + #185 + #186 + #189 + #190 +
  #195 in).
- landing posture: PR opened **READY** (not draft). Plain `claude/*` READY PRs
  auto-land server-side on green `rom-builds` via `auto-merge-enabler.yml` under
  owner merge authority — no self-merge, no manual auto-merge arm from this
  session. substrate-gate is RED BY DESIGN while this card is `in-progress` (the
  born-red hold) and possibly additionally red on a pre-existing
  arcs/TILTSTONE.md orphan owned by another team — neither blocks the required
  `ROM builds` gate; flipping this card to `complete` (the deliberate LAST step)
  releases the born-red hold.

## What shipped

1. Born-red gate: this card `in-progress`, PR **#[[fill:pr]]** opened READY;
   flips to `complete` on green `ROM builds`.
2. **The pure helper** (`games/wickroad/src/wr_milestones.h`): a new
   `wr::run_tier_up_label(int completed_run_ordinal)` DERIVED from
   `wr::run_tier_label` — returns the tier word the run-end at this ordinal
   FIRST reaches (an `==` crossing, unlike `run_tier_label`'s `>=` level), or
   `nullptr`. One source of truth: the thresholds/labels live only in
   `run_tier_label`; the crossing is `run_tier_label(n) != run_tier_label(n-1)`.
   No state, no RNG.
3. **The slice** (`games/wickroad/src/main.cpp`): `record_run()` captures
   `tier_up_label = wr::run_tier_up_label(best.runs + 1)` BEFORE `++best.runs`
   (compare-before-increment, mirroring `milestone_label`); both end cards
   (`st_balanced` win, `st_closed` loss) draw `NEW TIER <label>` on the free
   `title_lines[4]` slot (one row below the #189 milestone line) inside the
   existing `ledger_loaded` branch. No RNG/telemetry/art/ledger change; the
   no-save path is byte-identical.
4. **Host test** (`tools/check-run-tier-up.py`): a stdlib-only mirror of the
   derived crossing (the `check-run-milestones.py` convention), asserting
   ordinals 1..49 → none, 50 → "VETERAN", 51..99 → none, 100 → "MASTER",
   101..300 → none; boundaries 49/50/51 and 99/100/101 pinned; 0 and -1 total.
   `check-run-tier.py` + `check-run-milestones.py` re-run for no-regression.
5. **Build**: [[fill:build]]

## 💡 Session idea

[[fill:idea]]

## Previous-session review

[[fill:review]]
