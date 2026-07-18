# Session — Wickroad: the lifetime run count on the title

> **Status:** `complete`

- date: 2026-07-18 (branch `claude/wickroad-lifetime-runs`, PR **#186**;
  born-red card written at 2026-07-18T00:19:39Z, flipped to complete at
  2026-07-18T00:21:40Z, both from `date -u`).
- **📊 Model:** opus-4.8 · high · polish slice — surface the SRAM-banked
  lifetime run count on the Wickroad title screen.
- mission: **Wickroad polish — "THE RUN COUNT ON THE TITLE"** (a follow-on to
  PR **#184** — the persisted best on the title — and PR **#185** — the NEW
  RECORD flash on the end card). Both prior slices exploited the same pattern:
  a value that Wickroad already banks in SRAM but renders on no screen. This
  slice takes the last such field. `best.runs` (the `wr_ledger_save.runs`
  member) is incremented in `record_run()` at every run-end and persisted to
  SRAM word `wr_ledger[4]`, but it is drawn on ZERO screens — a returning
  player can see their best gold and day (via #184) but never how many roads
  they have walked. This slice appends `RUNS n` to the title's existing
  `BEST GOLD x DAY y` line (`title_lines[1]`), gated on the SAME `ledger_loaded`
  flag, so the lifetime count rides the line a score-attack player already
  reads before START. Render-only: **zero new RNG draws, `wr_telemetry`
  UNCHANGED, `wr_art` / `wr_ledger` unchanged.** THE CONTRACT: the default
  no-save path is byte-identical — `ledger_loaded` is false on every fresh /
  foreign / erased cart (and on every proof booted WITHOUT `--savefile`), so
  the title's second line renders EXACTLY the committed flavor line at the same
  slot/position. Branches from main (#184 + #185 in).
- landing posture: PR opened **READY** (not draft). The server-side
  auto-merge-enabler arms native auto-merge on green rom-builds under owner
  merge authority — no self-merge, no manual auto-merge arm from this session.
  substrate-gate is RED BY DESIGN while this card is `in-progress` (the
  born-red hold) and additionally red on a pre-existing arcs/TILTSTONE.md
  orphan owned by another team — neither blocks the required `ROM builds` gate;
  flipping this card to `complete` (the deliberate LAST step) releases the
  born-red hold.

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/claude-wickroad-lifetime-runs.md`, PR **#186** opened READY.
2. **The slice** (`games/wickroad/src/main.cpp`): the `st_title` draw's
   `ledger_loaded` branch now appends `" RUNS "` + `best.runs` to `best_line`
   (`title_lines[1]`, y=-2); the local string capacity was widened from
   `bn::string<40>` to `bn::string<56>` to hold the extra segment. Plain-space
   separator matches the existing `" DAY "` style — no new font glyphs, no new
   sprite line, no position change. Still under `ledger_loaded`, so the no-save
   path renders the committed flavor line byte-for-byte. No RNG/telemetry/art/
   ledger change.
3. **Build**: local ROM build unavailable in this clone (no
   `third_party/butano` source) — the "ROM builds" gate is proven by CI
   `rom-builds.yml`.

## 💡 Session idea

**When three sibling slices all mine the same seam — "banked in SRAM, shown on
no screen" — the seam itself is the finding, not any one field.** #184
surfaced `best_gold` + `best_day_reached` on the title; #185 announced them at
the beat they change; this slice takes `best.runs`, the last member of
`wr_ledger_save` that reached persistence (tier 1) but never legibility (tier
2). That the WHOLE struct shipped invisible for a full arc — every field
written, every field restored, zero fields read by any draw — is the pattern
worth naming: **a save format tends to be built field-complete before a single
consumer exists, because writing the struct is the "feature" and reading it is
"UI later."** The cheap audit that catches this on any persistence layer:
grep the save struct's members against the draw code — every member with a
write site and no read site is a latent, already-paid-for polish slice.
Wickroad's `wr_ledger_save` is now fully drained (gold, day, runs all legible;
seed is intentionally internal, used to replay worlds not to display). The
forward note: `best.runs` is a lifetime *count*, not a best — it only ever
grows, so unlike gold/day it has no "record" beat to announce (#185's move
doesn't apply). Its natural next surface, if wanted, is a milestone nudge
(e.g. a one-line flourish at runs 10/25/50) — but that would add a compare and
a threshold table, crossing out of pure render-only, so it is deliberately
left as a follow-up, not folded in here.

## Previous-session review

Prior slice: `.sessions/2026-07-18-wickroad-new-record.md` (PR **#185**, NEW
RECORD on the end card) — **holds up, and its framing directly set up this
one.** #185's thesis was the three-tier model (persisted → legible → announced)
and it argued tier 3 is the cheapest-yet-most-skipped surfacing. This slice is
a clean datapoint for the complementary claim: tier 2 is skipped just as
often, and for the same reason — the value is "already stored," so nobody
reads it. #185 also correctly scoped itself to *records* (values with a beat
that changes them); `best.runs` sits just outside that scope (a monotone
counter, no record beat), which is exactly why it wanted a decision-screen
readout (#184's move) rather than a transition flash (#185's move) — the two
prior slices between them covered both surfaces, and picking the right one for
this field was a matter of asking which tier the field was actually missing.
One honest note: #185 claimed the WIK-03 score-attack thread was "closed
end-to-end"; that was true for the *best* (gold/day chase→catch), and this
slice does not reopen it — the run count is context, not a score, so the loop
stays closed and this is a coda, not a fourth act.
