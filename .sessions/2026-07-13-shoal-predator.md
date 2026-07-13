# Session 46 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/shoal-predator`, 20:04Z → 20:18Z)
- mission: **SHOAL growth rung 1 — THE PREDATOR PASS.** The first
  named deliberate cut in `games/shoal/CONCEPT.md` @ main `e59b26d`
  ("predators locking onto stragglers") — and only that item; gates,
  ratings-proper, levels, audio stay cut. Coordinator constraint
  honored: no second flock attempted (it does not fit the measured
  budget). Session 46 claimed (45 = Shoal, PR #98, merged `e59b26d`).
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #99 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.

## 💡 Session idea

**"Lock onto stragglers" is a THRESHOLD sentence, not a superlative
sentence — and the difference is whether the mechanic has
counterplay.** First cut: hunters lock the farthest-from-centroid
fish (the superlative reading). Measured result: the committed sweep
LOST 34/11 — because after the main mass banks, whatever remains IS
"farthest", so leftovers were farmed faster than they could re-cohere;
the player had no move that made fish safe. The fix that made the
mode a game was one constant: prey must be a GENUINE straggler
(>44 px out). Now a tight school starves the hunters, a re-cohered
remnant is immune, and the losing thing is a player CHOICE
(abandonment / blasting the school apart) — the counterplay the
concept's own pitch ("keep the school tight") always implied. Rule
extracted: **when a pursuit mechanic reads "targets the worst X",
implement "targets X beyond a threshold" — a superlative always has a
victim, so a superlative hunter is unbeatable by construction; a
threshold hunter is beaten by play.** Second, smaller find: exact
CPU-cost pins are per-build measurements — ANY code touch (even a
HUD string) shifts them ~10 data units, so expect to re-derive them
once per slice and keep the durable `< 4096` rail beside them; the
pair is the point (exact = regression tripwire, rail = the contract).

## Previous-session review

- Session 45 (Shoal): its guard recipe drove this session line by
  line — the predator pass was named as the rung that fits (~31%
  worst-frame headroom: cashed, worst now 75.1%, ~25% left), the
  `t[5] < 4096` rail was kept and extended over predator frames, the
  IWRAM discipline was applied with the map-address check
  (0x030006d8), and IWRAM growth stayed tiny (predator fn ~0.4KB).
- Its 💡 (map-file claims; probes with exclusive output channels) was
  simply followed — no measurement mystery this session, first probe
  numbers were trusted only after the win/lose/idle triangle agreed.
- One friction its recipe predicted exactly: "ANY tuning re-pins all
  three [proofs]" — landed as the 4 CPU-literal re-pins below.

## Re-pins (each justified, per the coordinator's rail)

Exactly **4**, all CPU-MEASUREMENT literals; **zero game-state
re-pins** (every position/count/state/clock pin carried verbatim):

1. P2 `400:t[4]` 2164 → 2187 (+~0.5%): the calm loop now evaluates
   the per-frame mode gate + predator-glyph clear; measured cost of
   the gate itself.
2. P2 `400:t[5]` 2810 → 2834 — same cause (max tracks the same rise).
3. P2 `1199:t[5]` 2810 → 2834 — same word, later frame.
4. P3 `2000:t[5]` 2772 → 2796 — same cause in the calm win run.

The durable `t[5] < 4096` budget rails are UNCHANGED and now also
asserted in all three hungry-water proofs.

## What this session did

1. Claim + born-red card first (`57f1cbd`), PR #99 opened READY
   immediately after, cut from main @ `e59b26d`.
2. **The hungry water** (input-verb gate, Brineward slice-7
   pedigree): SELECT starts a run with two hunters; START keeps the
   calm water — no committed script presses SELECT, so the calm game
   is bit-identical by construction. Title names the verb
   ("SELECT: HUNGRY WATER (35)").
3. **The hunters** (`sh_pred_update`, IWRAM @ 0x030006d8,
   map-verified): every 48 frames lock the fish farthest from the
   flock centroid — but ONLY beyond the 44 px straggler threshold
   (the idea above); per-axis pursuit at 1.125 px/f (a pushed school
   outruns them); a catch eats the fish, and the hunter dens up 300
   frames (tuned from 180: measured too hungry). Lose when the goal
   becomes unreachable ("THE SHOAL SCATTERED / KEEP THE SCHOOL
   TIGHT"). The hungry water asks **35** home (the concept's
   "star rating = fish saved" grading applied per water; 40 measured
   unreachable — the calm-40 sweep banks 35/8 hungry).
4. **Budget, measured like the gate run** (final build): hungry idle
   mean **50.0%**, p99 **70.4%**, steady worst **75.1%** (3075/4096);
   **74.0%** (3033) through the winning herd. Calm exact words
   2187/2834 (the +~0.5% mode-gate cost). Telemetry 16 → 24 (words
   0-15 frozen; 16 mode, 17 eaten, 18-21 hunter positions, 22-23
   locks). OAM +2 sprites.
5. **Proofs** (mGBA headless, local per series convention; no CI
   edits): **6 proofs / 88 asserts (74 watch + 14 text)** — P1 boot
   (carried + the new verb line); P2 gate+determinism (carried, calm
   mode words asserted 0); P3 calm win (carried verbatim incl.
   "SAVED 40/50" at rf 1619 — the calm game provably untouched);
   P4 the hungry water (dens exact, the opening scramble eats 2 by
   300 and 8 by 1400, then the STARVATION PLATEAU — eaten still 8 at
   1599 with the school cohered; budget 3075 exact + rail); P5 the
   hunt is winnable (same sweep: 35 home / 8 lost at rf 1591, card
   text exact, budget 3033 + rail) — **run twice byte-identical**;
   P6 abandonment kills (scatter-in-place: 16 eaten → SCATTERED at
   rf 2848, card text exact, then START restarts CALM: mode 0,
   eaten 0, 50 at sea).
6. **Ship**: `dist/shoal.gba` v0.2 127,196 B, sha256 `e63c2f67…5fdc`,
   byte-deterministic (two clean builds identical; the shipped dist
   spot-checked against the win pins); CONCEPT.md updated (rung 1
   moved from the cut list to built, with the measured numbers);
   dist/README row; heartbeat appended at the END of
   control/status.md.

## Guard recipe (for the next Shoal rung)

- Remaining named cuts, in CONCEPT.md order: gates, star ratings
  proper, multiple tuned levels, audio. Headroom after the hunters:
  ~25% worst-frame — gates (static geometry tests) fit easily; audio
  via the proven generate_audio.py pipeline is render-like (zero
  sim cost); a THIRD hunter or a second flock does NOT fit.
- The straggler threshold (44 px) and den cooldown (300) are the
  hungry water's whole difficulty — retuning either re-times P4-P6
  and re-prices the 35 goal; retune all three together or not at all.
- The 35 goal is load-bearing in P5's card text ("SAVED 35/50" is the
  saved COUNT, not the goal — the goal shows on the HUD/title only).
- Expect the 4 CPU-exact literals to move ~±15 data units on ANY code
  change: re-derive once per slice, keep the `< 4096` rails verbatim.
- sh_fish.saved is now a 3-state enum in disguise (0 sea / 1 reef /
  2 eaten) — every `if(f.saved)` truthiness test treats eaten like
  saved (skip); a 4th state must audit those sites.

## Session enders

- 6 headless proofs / 88 asserts green; the hungry win run twice
  back-to-back byte-identical; the calm game's state pins carried
  with zero re-pins.
- From-source rebuild == committed `dist/shoal.gba` bit-for-bit
  (sha256 `e63c2f6777a85fcd540c2992cb3b30ac254562364e57afa71878185cf
  dfa5fdc`); two clean builds identical; dist spot-checked live.
- `sh_pred_update` verified at 0x030006d8 in the final map.
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/shoal-predator.md` retired with this flip.
