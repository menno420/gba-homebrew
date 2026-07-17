# Session — Wickroad crossroads cut 1: THE JUNCTION

> **Status:** `in-progress`

- date: 2026-07-16 (branch `claude/wickroad-crossroads-cut1`, PR #172;
  started 22:04:36Z, card resolved 22:20:29Z — both from `date -u` at
  write time). Held DRAFT — Status stays `in-progress` by design (see
  landing posture).
- mission: **Wickroad crossroads cut 1 — "THE JUNCTION"** per
  `docs/NEXT-MENU-2026-07-15.md` § B2 and the new arc plan
  `games/wickroad/ARC-CROSSROADS.md`: the second arc finally builds the
  idea `CONCEPT.md` named inside a served cut — the map stops being one
  linear road. Refactor the bare linear town index into a small static
  adjacency table (the spine 0↔1↔…↔6 stays bit-identical), hang ONE
  off-spine branch town — **WYRMHOLLOW** (index 7) — off the mid-spine
  junction **DUNWICK** (4), and add the first new travel verb since
  v0.1: the **L+R chord** ("take the fork"), purely additive because no
  committed route presses both shoulders on one frame. RNG: +8 draws
  appended strictly after town 6's at the wider-map freeze point (old
  world bit-identical → P1-P8 carry verbatim). Telemetry `[52] → [56]`
  (words 52-55 appended). Proven by new proof P9. One contained slice
  on the v0.6 audio build (PR #146). Original assets only (WYRMHOLLOW
  is an original name).
- **📊 Model:** Claude Opus 4.x · high · feature build — wickroad
  crossroads cut 1.
- landing posture: PR #172 opened DRAFT and held draft per the standing
  2026-07-16 landing wall — no ready-flip, no merge/approve/auto-merge
  calls from this session; card stays in-progress by design (born-red
  HOLD).

## What shipped

1. Born-red gate (commit `1a12233`): this card `in-progress` +
   `control/claims/claude-wickroad-crossroads-cut1.md` +
   `games/wickroad/ARC-CROSSROADS.md` (the crossroads arc plan — all
   four cuts in build order: the junction / sprite art / seed dial /
   the best ledger), PR #172 opened DRAFT immediately and held draft.
2. **The junction** (commit `09bc100`, `games/wickroad/src/main.cpp`):
   the bare linear town index is refactored into a small static
   `adjacency[town_count]` table (spine-left / spine-right / fork
   neighbour per town). L/R read the spine columns and walk 0↔1↔…↔6
   BYTE-IDENTICALLY (the fork column is off-spine, so a spine ride's
   toll, dawn, hazard leg and timing are untouched). WYRMHOLLOW (7)
   hangs off the DUNWICK (4) junction via the fork column, and the L+R
   chord ("take the fork") rides onto it — a fork ride pays the same
   toll + `dawn()` + `cross(leg)` as a spine ride, on `branch_leg` (an
   id the hazard deck never uses). WYRMHOLLOW is reachable ONLY by the
   chord; R at the junction still goes to HOLLOWFEN, R at MIRGATE
   (spine end) still does nothing. Its market trades on the same price
   law and impact ladder as every spine town.
3. **RNG**: `town_count` 7 → 8, new `spine_town_count = 7` bounds the
   spine travel/display; WYRMHOLLOW's 8 draws (4 base + 4 phase,
   mirroring town 6) are appended STRICTLY after town 6's at the
   wider-map freeze point in `reset_run()`. Towns 0-6 keep their exact
   stream order → the old world is bit-identical → P1-P8 carried
   verbatim on the first post-change run (zero re-pins).
4. **Telemetry**: `wr_telemetry` 52 → 56 — word 52 on-branch flag
   (1 on WYRMHOLLOW), word 53 fork-edge id (7 while a fork is live from
   the current town — at the junction or on the branch — else 0), word
   54 branch-town RESIN price witness (pinned against the mirror even
   from across the spine), word 55 reserved (0). Words 0-51
   byte-unchanged; the header-comment layout block updated.
5. **`visited_mask`** sets bit 7 when WYRMHOLLOW is reached (word 46
   reads 0x9f on the branch this route).
6. **Proof P9 THE JUNCTION** (`games/wickroad/proofs.sh`, additive —
   P1-P8 untouched): walk the spine R×4 to DUNWICK (day 5, gold 52, no
   hazard — every window opens day 15+), where word 53 = 7 and word 52
   = 0; the L+R chord rides onto WYRMHOLLOW (day 6, gold 50, word 52 →
   1, mask bit 7 set); the branch market obeys the same impact law (UP
   to RESIN at the mirror's 18, two buys climb the cursor word 18→19→20
   as gold falls 50→32→13, two sells walk it back to gold 50); plain L
   returns to DUNWICK (day 7, gold 48, word 52 → 0). Word 53 reads 7
   ONLY at the junction and the branch, 0 at a mid-spine town — the
   branch is reachable only by the chord. Every pin derived on the
   host-side Python mirror FIRST; the ROM matched every watch pin on
   the first route probe. Suite result: **`ALL WICKROAD PROOFS PASS`**,
   exit 0; P1-P8 re-passed verbatim (proof words 0-51 + old RNG draws
   are byte-identical), P2-P9 each run twice with `cmp`'d watch-logs
   byte-identical, the whole suite run twice end-to-end.
7. **Ship** (commit `09bc100`): `dist/wickroad.gba` v0.7, 160,212 B,
   sha256 `0f497a3894632f1769840afb6d466f06a2fd1a666a537d88d243329c930
   d8b84`, two clean builds (`make clean && make`) byte-identical;
   `dist/README.md` row updated; `games/wickroad/CONCEPT.md` growth-cut-4
   junction note marked **JUNCTION SERVED — v0.7** and its Determinism
   telemetry size fixed to 56; `docs/current-state.md` Wickroad row →
   v0.7 (crossroads cut 1 in-progress).
8. CI on the head SHA `09bc100` (github `actions_list` by branch):
   **"ROM builds" — success** (run 29539073620, the ROM builds clean);
   **"substrate-gate" — failure** (run 29539073609, RED BY DESIGN =
   main's #151 orphans + the born-red HOLD, named in the PR body, not a
   real failure of this change); auto-merge-enabler skipped. The
   born-red commit `1a12233` also built green (run 29538422187).

## 💡 Session idea

**A committed input grammar with every button already consumed still
has a free verb hiding in its CHORDS — the simultaneous press no single
route ever uses.** Wickroad had spent all ten GBA buttons across five
growth cuts (A/B trade, UP/DOWN cursor, L/R travel, SELECT wait, START
restart, RIGHT/LEFT the pact/guard verbs), and CONCEPT.md flagged the
fork as blocked precisely because "a junction would need a new travel
verb" — read as "there is no button left." The move: a verb doesn't
need a free BUTTON, it needs a free INPUT — and the set of inputs is
the power set of the buttons, not the buttons. `L+R` on one frame is an
input that every committed route provably never produces (routes press
one shoulder at a time), so an `L+R` handler is byte-identical-additive
by construction: guard the single-L and single-R handlers with
`!other_pressed()` (a no-op for every existing route) and the chord
frame is yours. This generalizes the append-only discipline from state
(RNG draws, telemetry words) to the INPUT space: when the buttons are
full, the next verb is a chord, and you prove it's free the same way
you prove an appended draw is free — run the old suite untouched and
watch it pass verbatim. Corollary for the adjacency refactor that rode
along: you can swap a data representation (bare index → static table)
under a hard byte-identity constraint by keeping the OUTPUT function
identical (spine leg = `min(from,to)` reproduces the old `--town`/`cross`
arithmetic exactly) — the table is new, the numbers it emits are not.

## Previous-session review

- Prior lane slice: the newest Wickroad card
  `.sessions/2026-07-15-wickroad-audio.md` (PR #146, growth cut 5 —
  the intended Tiltstone arc-2-cut-5 / PR #170 card has no file under
  `.sessions/`, so the documented fallback applies). Its 💡 —
  "measure the platform tax BEFORE re-deriving pins: run the OLD
  committed suite against the new build first, and let the pass/fail
  pattern tell you whether the change is free" — held up exactly and
  directly shaped this session. Before writing a single P9 assert I
  rebuilt with the adjacency refactor + the appended RNG draws +
  telemetry 52→56 and ran the UNTOUCHED P1-P8: it passed verbatim, so
  the whole "did the refactor perturb the committed world / timing"
  contingency collapsed to a measured non-fact, and P9 could then be
  written against a known-good base with zero re-pin noise. The audio
  card also did the honest work of NAMING the next platform tax rather
  than just the feature — a discipline this card continues by naming
  the substrate-gate RED as by-design in the PR body up front, not as a
  surprise at CI time. One honest gap it left that this cut had to
  discover for itself: the card's determinism note still read
  `wr_telemetry[52]` as the final size with no pointer that the RNG
  freeze point in `reset_run()` is where the NEXT town's draws append —
  I found that freeze point by reading the code, not the card. The
  widened rule (already folded into `ARC-CROSSROADS.md` and the
  reset_run comment): a card that appends state should name, in the
  code, the exact freeze point the next session appends behind.
