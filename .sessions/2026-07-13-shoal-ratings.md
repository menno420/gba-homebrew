# Session 48 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/shoal-ratings`, 21:05Z → 21:35Z)
- mission: **SHOAL growth rung 3 — STAR RATINGS proper.** The next
  named deliberate cut in `games/shoal/CONCEPT.md` @ main `253695d`
  ("Deliberately still cut: star ratings proper, multiple tuned
  levels, audio") — built exactly as the committed concept names the
  mechanic: **"Star rating = fish saved"**
  (`docs/concepts/session-1-concepts.md`). Ratings only — levels and
  audio stay cut.
- dedup at claim time: no shoal claim in `control/claims/`, no open
  Shoal PR; inbox at HEAD unchanged since `c5d3cd6` (ORDERS 001-005,
  all served) — nothing new unserved.
- constraint honored: the coupled hungry-water difficulty knob
  (straggler 44 px + den 300 + goal 35) is NOT retuned and the
  ratings did not need it — the 3-star line simply REUSES each
  water's existing goal constant; no new number enters the sim.
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #101 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.

## 💡 Session idea

**A rating rung on a goal-terminated game is really a LOSS-grading
rung — the win card's stars are a constant, so all the design lives
below the goal line.** "Star rating = fish saved" sounds like it
grades wins, but every Shoal win ends AT the goal by construction
(the state flips the frame `saved >= goal`), so a win is always
3 stars and a rating that only decorated wins would be a static
sticker. What the stars actually price is everything else: the live
HUD stars tell you mid-run how much shoal you have already made safe
(the gated run walks visibly through * at 25 and ** at 30 on its way
to banking 40), and the SCATTERED card finally answers "how badly
did I lose?" — a remnant-farmed collapse with 6 banked reads
"RATING -", while a sweep that banked 29 before the hunters won
would keep its "**". Two implementation rules fell out: (1) **derive
the top grade FROM the existing goal constant instead of minting a
new threshold** — thirds of the goal gave 14/27/40 and 12/24/35 with
zero new tuning, which is what kept the coupled-knob constraint
trivially satisfied; (2) **a display-only rung still moves the CPU
exacts** (+3..+15 data units from rendering one to three asterisks)
— the re-derive-once ritual applies to pure cosmetics too, which is
exactly why the durable `< 4096` rail, not the exact literal, is the
contract.

## Previous-session review

- Session 47 (Shoal gates): its committed `proofs.sh` did precisely
  what its card promised — this session ran it green BEFORE touching
  code (base sanity), extended it in place, and never had to
  excavate a route; the "carried byte-for-byte is a re-runnable
  command" claim held on first contact.
- Its guard recipe priced this rung correctly ("ratings and levels
  are DATA rungs — near-zero sim cost": measured, the sim words
  never moved) and its cross-ROM-diff method was reused verbatim
  against the v0.3 dist it shipped.
- One friction it did NOT predict: its own P6-shaped gap — the suite
  had no committed LOSING hungry run (the #99 scatter route died
  with the container and was never reconstructed), so this session
  had to derive a fresh graded-loss route from scratch (three probe
  runs: an A-hold "scatter" that accidentally WON 35/13, a
  bank-then-sabotage that won at rf 488, then the pin-the-school-to-
  the-left-wall route that loses honestly at rf 3420). The suite now
  carries it as P6.
- Its ts-erratum warning was heeded: the heartbeat ts was taken from
  `date -u` at write time, no correction needed this session.

## Re-pins (each justified, per the coordinator's rail)

All CPU-MEASUREMENT literals; **zero game-state re-pins** — re-proven
cross-ROM: calm (1300 f) + hungry (1700 f) + gated (1300 f) idle
telemetry of the new build diffed word-for-word against the rebuilt
v0.3 dist (verified against its committed sha256 first) — **0
differing rows outside t[4]/t[5]** in all three waters. The CPU
exacts, re-derived once (the HUD-stars render cost):

1. P2 calm `t[4]@400` 2085 → 2088, `t[5]` 2822 → 2833 (×2 frames).
2. P3 hungry `t[5]` 3058 → 3073 (×2 frames).
3. P4 gated win `t[5]@3540` 2916 → 2927.
4. P5 gated idle `t[5]@1299` 2949 → 2958.
5. New pin (no prior value): P6 graded-loss `t[5]` 3094 (75.5% — the
   session's worst frame: hungry water + full-school push; still
   under the predator-era 75.1%+rail envelope and the rail).

Every `t[5] < 4096` rail carried verbatim; every game-state literal
in P1-P5 (positions, centroid, counts, run-frames, card texts)
passed unchanged.

## What this session did

1. Claim + born-red card first (`e70b71f`), PR #101 opened READY
   immediately after, cut from main @ `253695d`.
2. **The ratings** (display-only; the sim never consults a star):
   `stars_of(saved, mode)` — a constexpr per-water lookup whose
   3-star line IS the water's goal constant (calm/gated 40, hungry
   35) with 2/1 stars at thirds, rounded up (14/27/40 and 12/24/35).
   Stars ride the HUD live ("SAVED 25/40 *"), the title names the
   rule ("STARS = FISH SAVED"), and BOTH end cards grade the run:
   "RATING ***" on the win card, "RATING -" on a near-goalless
   scatter — losses grade too, which is what "proper" adds over the
   rung-1 goal-only grading.
3. **A graded loss, committed** (P6): SELECT + slip the cursor under
   the school to its right flank + pin the school against the LEFT
   wall — six leak home, the hunters farm the rest one straggler at
   a time, the goal dies at eaten 16: SCATTERED at rf 3420, card
   text exact incl. "RATING -", run twice byte-identical.
4. **Proofs** (the committed suite, extended in place): **6 proofs /
   102 asserts (82 watch + 20 text)** — P1 boot (+ the STARS title
   line); P2 calm idle (carried + the 0-star HUD "SAVED 5/40 -");
   P3 hungry idle (carried verbatim); P4 gated win (carried verbatim
   + live-star HUD pins `* @25`, `** @30` + "RATING ***") — run
   twice byte-identical; P5 idle discriminator (carried); P6 the
   graded loss (new) — run twice byte-identical. **All four grades
   (- / * / ** / ***) are pinned on screen.**
5. **Ship**: `dist/shoal.gba` v0.4 126,616 B, sha256 `a3a9f7f9…cf38`,
   byte-deterministic (two clean builds identical; the suite re-run
   green against the exact dist bytes); CONCEPT.md updated (rung 3
   moved from the cut list to built); dist/README row; heartbeat
   appended at the END of control/status.md with a live `date -u`
   ts.

## Guard recipe (for the next Shoal rung)

- Remaining named cuts, in CONCEPT.md order: multiple tuned levels,
  audio. Levels = per-level gate/den layout + goal tables — the
  first rung that may genuinely need the 44/300/35 knob retuned
  (per-level); if so, retune all three deliberately and re-derive
  the affected routes, not just pins (the #99/#100 rules compose).
  Audio via the proven synth pipeline is render-like; expect only
  CPU-exact drift.
- `games/shoal/proofs.sh` now carries win AND loss routes for the
  hungry/gated waters; run it green before touching code, extend in
  place, re-derive the CPU exacts after (this slice measured
  +3..+15/word for pure display code).
- The star thresholds are DERIVED (thirds of the goal, rounded up)
  — a levels rung that changes goals gets its star lines for free;
  never mint independent star constants.
- P6's route is knife-edge by nature (it must LOSE): any sim or
  knob change re-derives it first — check P6 before believing a
  regression elsewhere.
- IWRAM map: flock 0x03000000 / predator 0x030006d8 / gate
  0x03000a2c — unmoved this slice (display-only rungs shouldn't
  touch the map; if an "audio" rung moves these, ask why).

## Session enders

- 6 headless proofs / 102 asserts green via the committed suite; the
  gated win and the graded loss each run twice back-to-back
  byte-identical; calm/hungry/gated game-state proven bit-identical
  to the v0.3 dist by cross-ROM telemetry diff (0 non-CPU diffs over
  4300 frames).
- From-source rebuild == committed `dist/shoal.gba` bit-for-bit
  (sha256 `a3a9f7f934467fc9d804cfd82df9b3c6ec2b35fd339c2dc74ccf065a
  8568cf38`); two clean builds identical; the suite re-run against
  the dist copy itself.
- IWRAM symbols verified unmoved in the final map (0x03000000 /
  0x030006d8 / 0x03000a2c).
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/shoal-ratings.md` retired with this flip.
