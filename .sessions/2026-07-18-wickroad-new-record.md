# Session — Wickroad: NEW RECORD on the end card

> **Status:** `complete`

- date: 2026-07-18 (branch `claude/wickroad-new-record`, PR **#185**;
  born-red card written at 2026-07-18T00:06:27Z, flipped to complete at
  2026-07-18T00:09:00Z, both from `date -u`).
- **📊 Model:** opus-4.8 · high · polish slice — flash NEW RECORD on the
  Wickroad end card when a returning run beats the SRAM-persisted best.
- mission: **Wickroad polish — "NEW RECORD" on the end card** (a follow-on to
  PR **#184**, which put the persisted best on the TITLE screen). #184 opened
  the score-attack loop by making the SRAM best legible before START; this
  slice closes it at the other end — the moment a returning player's run
  actually beats that best. When (and only when) a prior SRAM save was
  restored (`ledger_loaded`) AND the just-finished run beat the stored best —
  more gold than `best_gold` OR a later day than `best_day_reached` — the end
  card (both the win card `st_balanced` and the loss card `st_closed`) flashes
  a one-line **NEW RECORD** below the BEST GOLD line. The comparison is
  captured INSIDE `record_run()` BEFORE its overwrite clobbers `best`, so the
  flash reflects the record the player *just broke*, not the record they now
  hold. Render-only: **zero new RNG draws, `wr_telemetry` UNCHANGED,
  `wr_art` / `wr_ledger` unchanged.** THE CONTRACT: the default no-save path
  is byte-identical — `ledger_loaded` is false on every fresh / foreign /
  erased cart (and on every proof that boots WITHOUT `--savefile`), so the
  flash is unreachable and the end cards render EXACTLY as before. Branches
  from main (arc merged, #184 in). Original assets only (PUBLIC repo).
- landing posture: PR opened **READY** (not draft). The server-side
  auto-merge-enabler arms native auto-merge on green rom-builds under owner
  merge authority — no self-merge, no manual auto-merge arm from this session.
  substrate-gate is RED BY DESIGN while this card is `in-progress` (the
  born-red hold); flipping this card to `complete` (the deliberate LAST step)
  releases the gate.

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/claude-wickroad-new-record.md`, PR opened READY.
2. **The slice** (`games/wickroad/src/main.cpp`): `record_run()` now captures
   a `new_record` flag from `gold > best.best_gold || int(day) >
   best.best_day_reached` BEFORE it overwrites `best`; the two end-card draw
   states (`st_balanced`, `st_closed`) render `NEW RECORD` at `title_lines[2]`
   (y=40, the first free end-card slot below the existing BEST GOLD line),
   gated on `ledger_loaded && new_record`. No new sprite line on the no-save
   path, no RNG/telemetry change.
3. **Docs hygiene** (opportunistic, coordinator-authorized): re-linked the
   orphaned `docs/arcs/TILTSTONE.md` from its reachability-root index
   `docs/arcs/README.md` (the README already says "new arc docs are linked
   from here"; the Tiltstone row was simply missing) — one table row, pure
   docs, so substrate-gate's reachable check goes green.
4. **Build**: local ROM build unavailable in this clone (no
   `third_party/butano` source) — the "ROM builds" gate is proven by CI
   `rom-builds.yml`.

## 💡 Session idea

**A persistence feature isn't finished when the value is stored and legible —
it's finished when the player is told, at the exact instant, that the number
moved.** #184 made the SRAM best legible on the two screens where a
score-attack player forms and reviews intent (title before START, best line on
the end card). But legible ≠ noticed: the end card already SHOWED `BEST GOLD
n`, yet a player who just set that record had to read the number and recall
their old one to know they'd won — the achievement was on screen but silent.
This slice cost one captured bool and two `if` blocks to convert a passive
readout into an event. The reusable move: a stored value has three tiers of
surfacing — (1) persisted, (2) legible on a decision screen, (3) *announced at
the transition that changes it*. Tier 3 is almost always the cheapest yet the
one most often skipped, because the value is "already shown." The tell here
was the same one #184 flagged: `best_day_reached` had been banked since
crossroads cut 4 and only reached tier 2 in #184; folding it into the
new-record test (gold OR day) finally lets a survival-further run — one that
didn't out-earn the best but out-lasted it — announce itself too, so the loss
card can celebrate progress the win-only framing would have swallowed.

## Previous-session review

Prior slice: `.sessions/2026-07-17-wickroad-title-best.md` (PR **#184**, THE
BEST ON THE TITLE) — **holds up, and set this slice up cleanly.** Its central
claim was that a persisted field is a latent feature until a screen reads it,
and its practical gift was the `ledger_loaded` gate: one flag that makes every
new best-render unreachable on the no-save path by construction. This slice
reused that gate verbatim for a THIRD render site (the NEW RECORD line) — the
default cart stays byte-identical again, no new proof pins, exactly as #184's
gate was built to let presentation grow. One honest forward note the #184 card
predicted and this one confirms: #184 asked "on which screen does the player
DECIDE" and answered title + end card; the missing beat it left was the
*transition*, not a screen — the moment the best is beaten. That's this slice,
and it's the natural terminus of the loop, so the WIK-03 score-attack thread is
now closed end-to-end (chase it on the title, catch it on the end card).
