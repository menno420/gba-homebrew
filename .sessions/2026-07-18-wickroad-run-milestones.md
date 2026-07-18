# Session — Wickroad: run-count milestone flourish on the end card

> **Status:** `complete`

- date: 2026-07-18 (branch `claude/wickroad-run-milestones`, PR **#189**;
  born-red card written at 2026-07-18T02:04:10Z, flipped to complete at
  2026-07-18T02:12:00Z, both from `date -u`).
- **📊 Model:** Opus 4.8 · high · new-logic gameplay slice — a deterministic
  run-count milestone callout on the Wickroad end card.
- mission: **Wickroad — "THE MILESTONE FLOURISH"** (a follow-on to PR **#184**
  the persisted best on the title, PR **#185** the NEW RECORD end-card flash,
  and PR **#186** the lifetime RUNS count on the title). #186's own session idea
  named this exact next surface: `best.runs` is a monotone lifetime counter with
  no "record" beat, so its natural celebration is a milestone nudge at runs
  10 / 25 / 50. This slice adds it: when a just-completed run's ordinal crosses
  a threshold, the end card shows a one-line callout ("10TH RUN" / "25TH RUN" /
  "50TH RUN") on a free card slot below the NEW RECORD line. The crossing
  decision is a PURE helper — `wr::run_milestone_label(int completed_run_ordinal)`
  in `games/wickroad/src/wr_milestones.h`, no Butano/GBA headers — so it is
  trivially inspectable and host-testable; a stdlib mirror `tools/check-run-
  milestones.py` proves the threshold table for every ordinal. Following #185's
  discipline the ordinal is captured INSIDE `record_run()` BEFORE `++best.runs`,
  so the 10th completed run-end (pre-increment `best.runs == 9`, ordinal 10)
  triggers "10TH RUN" — the same number #186 shows as "RUNS 10". **Zero new RNG
  draws, `wr_telemetry` / `wr_art` / `wr_ledger` unchanged.** THE CONTRACT: the
  callout draws ONLY inside the existing `ledger_loaded` gate, so the default
  no-save path (fresh / foreign / erased cart, any proof booted without
  `--savefile`) is byte-identical — `clear_lines()` blanks the slot each
  transition, so it stays clear on a non-milestone run-end. Branches from main
  (#184 + #185 + #186 in).
- landing posture: PR opened **READY** (not draft). Plain `claude/*` READY PRs
  auto-land server-side on green `rom-builds` under owner merge authority — no
  self-merge, no manual auto-merge arm from this session. substrate-gate is RED
  BY DESIGN while this card is `in-progress` (the born-red hold) and
  additionally red on a pre-existing arcs/TILTSTONE.md orphan owned by another
  team — neither blocks the required `ROM builds` gate; flipping this card to
  `complete` (the deliberate LAST step) releases the born-red hold.

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/2026-07-18-wickroad-run-milestones.md`, PR **#189**
   opened READY.
2. **The pure helper** (`games/wickroad/src/wr_milestones.h`): a self-contained,
   Butano/GBA-free header with `wr::run_milestone_label(int completed_run_ordinal)
   -> const char*` returning the callout for ordinals 10 / 25 / 50 and `nullptr`
   otherwise. No state, no RNG, `constexpr`-friendly `switch`.
3. **The slice** (`games/wickroad/src/main.cpp`): `record_run()` captures
   `milestone_label = wr::run_milestone_label(best.runs + 1)` BEFORE
   `++best.runs` (compare-before-increment, mirroring `new_record`); both end
   cards (`st_balanced` win, `st_closed` loss) draw the callout on
   `title_lines[3]` (y=56, one 16px row below the NEW RECORD line) inside the
   existing `ledger_loaded` branch. No RNG/telemetry/art/ledger change; the
   no-save path is byte-identical.
4. **Host test** (`tools/check-run-milestones.py`): a stdlib-only mirror of the
   helper (the check-underroot.py convention), asserting ordinals 1..9 → none,
   10 → "10TH RUN", 11..24 → none, 25 → "25TH RUN", 26..49 → none, 50 →
   "50TH RUN", 51..200 → none. Exit 0 = green.
5. **Build**: local ROM build unavailable in this clone (no
   `third_party/butano` source) — the "ROM builds" gate is proven by CI
   `rom-builds.yml`.

## 💡 Session idea

**This slice is the first DERIVED surfacing on the Wickroad ledger — a pure
function OVER a persisted field, not a raw field readout — and that opens a
cleaner follow-on than any remaining struct member.** #184/#185/#186 drained
`wr_ledger_save`'s stored-but-unshown fields one by one (gold, day, runs); this
slice instead computes something new (a threshold label) from `runs`. The
milestone flourish has one honest limitation: it flashes ONCE, on the end card
of the crossing run, then is gone — a player who powers off never re-sees that
they hit 50. The natural next slice reuses THIS slice's pure helper verbatim:
promote `run_milestone_label` to also return a persistent TIER
(e.g. runs >= 50 → a "VETERAN" tag) and render that on the TITLE screen beside
#186's existing `RUNS n` line, so the achievement persists instead of
flashing. It stays render-only, ledger-gated, zero-RNG, and the host test
extends by one column — the same shape as this slice, one surface up (transient
end card → durable title). The remaining raw field, `best_seed`, is a
different (bigger) slice: surfacing it usefully means a "REPLAY THIS WORLD"
input verb on the end card, which crosses out of render-only, so it is
deliberately not the cheap next step.

## Previous-session review

Prior slice: `.sessions/2026-07-18-wickroad-lifetime-runs.md` (PR **#186**, the
lifetime RUNS count on the title) — **holds up, and it literally specified this
slice.** #186's own 💡 idea line named the exact next surface ("a milestone
nudge ... at runs 10/25/50 ... deliberately left as a follow-up, not folded in
here"), and its reasoning — `best.runs` is a monotone counter with no "record"
beat, so it wants a milestone rather than #185's transition flash — set the
scope precisely. This slice cashes that note in unchanged: same thresholds, and
the compare-before-increment discipline #186 inherited from #185 is what makes
"the 10th completed run shows 10TH RUN" line up with #186's own `RUNS 10`. One
honest note: #186 called `wr_ledger_save` "fully drained" for display; that was
true of raw fields, and this slice does not reopen it — it adds a *derived*
readout, a category #186 didn't count, so the two claims coexist.
