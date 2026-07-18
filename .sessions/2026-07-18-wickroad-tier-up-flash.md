# Session — Wickroad: tier-up flash on the end card (the run a tier is first earned)

> **Status:** `complete`

- date: 2026-07-18 (branch `claude/wickroad-tier-up-flash`, PR **#201**;
  born-red card written at 2026-07-18T21:10:28Z, flipped to complete at
  2026-07-18T21:17:28Z, both from `date -u`).
- **📊 Model:** Claude Opus · high · feature build — a transient END-CARD
  announce the run a lifetime tier is first crossed, the paired "you just
  earned this" beat for the persistent title tier tag.
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

1. Born-red gate: this card `in-progress`, PR **#201** opened READY;
   flipped to `complete` on green `ROM builds`.
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
5. **Build**: local ROM build unavailable in this clone (`arm-none-eabi-g++`
   not found, `third_party/butano` source absent — same as #189/#190/#195), so
   the "ROM builds" gate is proven by CI `rom-builds.yml`. Locally green: all
   three stdlib host proofs — `tools/check-run-tier-up.py` (the new crossing,
   0..300 swept, boundaries 49/50/51 + 99/100/101 pinned, exactly one crossing
   per tier), plus `tools/check-run-tier.py` and `tools/check-run-milestones.py`
   re-run for no-regression. The branch was rebased onto main `c633f1c` (after
   this session's own control PR #200 landed) with a one-line `control/status.md`
   heartbeat conflict resolved (kept both the #201 pointer and #200's
   `auto-merge-enabler.yml` name fix); host proofs re-run green post-rebase.

## 💡 Session idea

**The end card has now reached THREE independent celebration lines that can all
fire on one run-end — this slice is the "rule of three" signal that the NEXT
beat should refactor the ad-hoc draws into one ordered stack, not add a fourth
copy-paste.** Run 50 already lights `50TH RUN` (milestone, `title_lines[3]`) AND
`NEW TIER VETERAN` (this slice, `title_lines[4]`), and if it also beat gold,
`NEW RECORD` (`title_lines[2]`) — three separate `if(label) title_lines[n].set(
..., y)` blocks, each with its own hand-picked slot and y-offset, duplicated
across both end cards (`st_balanced`, `st_closed`). The reusable move: a small
ordered table of `(predicate, label, )` celebration triples the render walks
top-down, assigning rows from a base y — so the next beat is a table row, the
two end cards share one loop instead of two divergent copy-paste blocks, and
slot collisions become impossible by construction. But two disciplines gate it:
(1) a refactor ships no player-facing change, so it should ride WITH the next
real beat (GRANDMASTER ≥200, once #195's playtest-reachability gate clears), not
as its own slice — every slice must observably change the game; (2) the harder
constraint this slice exposes is LAYOUT, not code — `title_lines[4]` at y=72 is
the last row before the screen's bottom edge, so a FIFTH simultaneous beat has
nowhere to go. That is the real signal: the stack now needs a PRIORITY policy
(show the top-N most important beats when several fire at once), which is a
design decision the copy-paste form silently defers and the ordered-table form
forces you to make. The string-budget watch #195's review raised is satisfied
here (each beat is its own `text_line` primitive, `NEW TIER MASTER` = 15 chars
fits with margin) — the budget that is now actually scarce is vertical rows,
not characters.

## Previous-session review

Prior slice: `.sessions/2026-07-18-wickroad-master-tier.md` (PR **#195**, the
persistent MASTER title tier tag) — **holds up, and its 💡 idea named this
slice as the higher-value pick, correctly.** #195 ranked two follow-ons: (1)
GRANDMASTER ≥200 (trivial one-row table growth, but gated on evidence players
reach 200) and (2) "a tier-up FLASH on the end card the run a boundary is first
crossed ... the one with player-facing juice." This slice cashed in (2) and
left (1) parked on its stated playtest gate — exactly the priority #195 argued.
Its build-forward note was also right that the tier thresholds wanted ONE
source: rather than restate 50/100, `run_tier_up_label` DERIVES the crossing
from `run_tier_label` (`label(n) != label(n-1)`), so a future GRANDMASTER row
added to `run_tier_label` alone makes the flash announce it for free — a
strictly tighter lockstep than the milestone helper's own duplicated table.
One honest divergence to flag forward: #195's slices (#190/#195) were
helper-table-only with `main.cpp` UNTOUCHED, which kept them maximally cheap;
this slice is the first Wickroad tier beat to add render plumbing again (the
`record_run` capture + two `title_lines[4]` draws), so it is a genuinely bigger
change than the pure-table rows — still contained and ledger-gated
(no-save path byte-identical), but the render-plumbing growth is exactly what
the 💡 "rule of three" refactor above is meant to arrest before a fourth beat.
