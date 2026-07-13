# Session 47 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/shoal-gates`, 20:31Z → 21:05Z)
- mission: **SHOAL growth rung 2 — GATES.** The next named deliberate
  cut in `games/shoal/CONCEPT.md` @ main `c5d3cd6` ("Deliberately
  still cut: gates, star ratings proper, multiple tuned levels,
  audio" — gates first, in order; the geometry sentence is the
  committed concept's own "past predators, through gates, into the
  safe reef", `docs/concepts/session-1-concepts.md`). Gates only —
  ratings-proper, levels, audio stay cut.
- dedup at claim time: no shoal claim in `control/claims/`, no open
  Shoal PR; inbox at HEAD still ORDERS 001-005, all served per the
  status.md dispatch trail — nothing new unserved.
- constraint honored: the coupled hungry-water difficulty knob
  (straggler 44 px + den 300 + goal 35) is NOT retuned — gates live
  in their own water (the R verb), so the knob is untouched by
  construction; no retune was required by the gates design.
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #100 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.
- erratum, self-caught at the flip: the heartbeat ts was first
  committed pre-written (21:04:30Z) against a real clock of
  20:51:25Z — the EXACT session-45 erratum, repeated; corrected in
  this flip. Timestamps come from `date -u`, not from intention, and
  apparently that rule needs re-learning per session — so it is now
  in the guard recipe below.

## 💡 Session idea

**The input script is part of the pin — a "carried byte-for-byte"
bar that lives in an uncommitted file is a bar you can lose.** The
predator session's 88 asserts were real, but their INPUT SCRIPTS
lived only in the session container, and the container died with
them: this session could not re-run a single prior proof as written.
The carried-pin story had to be rebuilt sideways — replay fresh
idle scripts on the MERGED v0.2 dist (rebuilt from main and verified
against its committed sha256 first) and on the new build, then diff
every non-CPU telemetry word: zero diffs over 3000 frames, which is
the same claim "zero game-state re-pins" makes, proven cross-ROM
instead of cross-session. Two rules extracted: (1) **commit the
proof suite with the game** — `games/shoal/proofs.sh` now carries
every route and assert, so the next rung re-runs the bar instead of
excavating it; (2) **when the original evidence is lost, the
committed artifact IS the oracle** — a byte-deterministic dist plus
its pinned hash lets any future session reconstruct ground truth
without trusting prose. The cheap trick that made the diff honest:
compare ALL words EXCEPT the two CPU meters, so the exclusion list
names exactly what is allowed to drift.

## Previous-session review

- Session 46 (Shoal predator pass): its guard recipe named this
  session's rung and its price precisely — "gates (static geometry
  tests) fit easily" in the ~25% worst-frame headroom (measured
  true: worst gate-active frame 71.2% vs the predator pass's 75.1%),
  and "retune all three together or not at all" on the 44/300/35
  knob shaped the core design call (gates in their own water; knob
  untouched).
- Its 💡 (threshold-vs-superlative) wasn't exercised by static
  geometry, but its second find — CPU exacts move ~±15 on any code
  touch — priced this slice's re-pins exactly (calm 2834→2822,
  hungry 3075→3058: both -12/-17, within the stated band).
- One gap its enders left: "6 headless proofs / 88 asserts green" was
  true but unreproducible after the container died — the proof
  SCRIPTS were never committed. This session's whole carried-pin
  detour (and the committed `proofs.sh`) is that gap being paid for
  and then closed.

## Re-pins (each justified, per the coordinator's rail)

All CPU-MEASUREMENT literals; **zero game-state re-pins** — proven
stronger than carried-prose this time: the calm and hungry idle
telemetry of the new build was diffed word-for-word against the
merged v0.2 dist over 1300 + 1700 frames — **0 differing rows outside
t[4]/t[5]** (the two CPU meter words). The CPU exacts, re-derived
once per the series contract:

1. calm idle `t[4]@400` 2085, `t[5]` 2822 (v0.2 measured 2096/2834
   on the same script — the gate-branch cost plus link-layout shift,
   -11/-12 units ≈ 0.3%).
2. hungry idle `t[5]` 3058 (v0.2: 3075; -17 units).
3. New pins (no prior value): gated win `t[5]` 2916; gated idle
   `t[5]` 2949.

The durable `t[5] < 4096` rails are asserted in every proof and now
cover gate-active frames.

## What this session did

1. Claim + born-red card first (`7680f46`), PR #100 opened READY
   immediately after, cut from main @ `c5d3cd6`.
2. **The gated run** (input-verb gate, the #99 pedigree): R at the
   title starts a run where two static coral walls with OFFSET gaps
   stand between the school and the reef — gap 0 HIGH at x=104, gap
   1 LOW at x=156 (44 px each; wall band ±2 px, and sh_speed_max
   1.5 px/f < the 5 px band means no tunneling by construction).
   START/SELECT never touch the gate pass, so calm and hungry water
   are bit-identical by construction. Title names the verb
   ("R: THE GATED RUN (40)"). Goal stays 40 — calm rules, no
   hunters, knob untouched.
3. **The gate pass** (`sh_gate_update`, IWRAM @ 0x03000a2c,
   map-verified): pure static geometry — a fish pressed into a wall
   band outside its gap is ejected the way it came (vx sign) with
   its approach damped (vx = -vx/2). Telemetry 24 → 25 (words 0-23
   frozen; 24 = fish blocked this frame). OAM +20 coral glyphs,
   generated once at run start.
4. **The committed funnel route** (derived headlessly, screenshots
   at every phase): compress the school into the high lane off the
   bottom rail → pump through gap 0 with left-side vertical sweeps
   drifting right → shepherd the mid-water group down through gap 1
   → two more gather cycles for stragglers → **40/50 banked at
   run-frame 3519** ("CLOCK 58s (3519 FRAMES)" pinned). The same
   no-input run that banks 5 fish calm banks ZERO gated — the walls,
   and only the walls, hold the school (the idle discriminator).
5. **Budget, measured like the gate run** (final build): win-route
   gate-active frames mean **35.9%**, p99 **57.2%**, worst **71.2%**
   (2916/4096) over 3517 frames; gated idle worst **72.0%** (2949).
   Within the predator pass's own worst (75.1%) — the "gates fit
   easily" prediction, measured.
6. **Proofs** (mGBA headless, now COMMITTED at `games/shoal/proofs.sh`):
   **5 proofs / 82 asserts (72 watch + 10 text)** — P1 boot (all six
   title lines incl. every verb); P2 calm idle (carried: school
   positions/centroid/counts exact + CPU + rail); P3 hungry idle
   (carried: eaten 2 by rf~300, 8 by rf~1400, 10 by rf~1690, dens
   exact + CPU + rail); P4 THE GATED RUN (mode/blocked evidence,
   saved milestones 9/25/30/39, win exact incl. card text, budget
   2916 + rail, START restarts CALM: mode 0, 50 at sea) — **run
   twice back-to-back byte-identical**; P5 the idle discriminator
   (0 saved gated vs P2's 5 calm, school centroid pinned, rail).
7. **Ship**: `dist/shoal.gba` v0.3 127,016 B, sha256 `569bda96…8375`,
   byte-deterministic (two clean builds identical; the full suite
   re-run green against the exact dist bytes); CONCEPT.md updated
   (rung 2 moved from the cut list to built, measured numbers);
   dist/README row; heartbeat appended at the END of
   control/status.md.

## Guard recipe (for the next Shoal rung)

- Remaining named cuts, in CONCEPT.md order: star ratings proper,
  multiple tuned levels, audio. Ratings and levels are DATA rungs
  (grading thresholds / gate+den layouts per level) — near-zero sim
  cost; audio via the proven pipeline is render-like. CPU headroom
  after gates: worst measured frame anywhere is still the hungry
  water's 75.1% — the sim budget is unchanged by this rung.
- `games/shoal/proofs.sh` is now the carried-pin oracle: run it
  BEFORE touching code (green = your base is sound), re-derive the
  CPU exacts after (expect ±15/word), and extend it with your rung's
  proofs — never leave a route in the container again.
- The gate constants (x=104/156, gaps 28-68/92-132, band ±2) are
  load-bearing for P4's entire route: ANY gate retune invalidates
  the committed win script — re-derive the route, not just the pins.
- A "levels" rung wants gates + hunters COMBINED; that is the first
  rung that may genuinely require retuning the 44/300/35 knob —
  if so, retune all three deliberately and re-time P4-P6-class
  proofs (the #99 card's rule stands).
- Write the heartbeat ts from `date -u` AT WRITE TIME — two sessions
  have now pre-written it and had to erratum it.

## Session enders

- 5 headless proofs / 82 asserts green via the committed suite; the
  gated win run twice back-to-back byte-identical; calm/hungry
  game-state proven bit-identical to the merged v0.2 dist by
  cross-ROM telemetry diff (0 non-CPU diffs over 3000 frames).
- From-source rebuild == committed `dist/shoal.gba` bit-for-bit
  (sha256 `569bda9632583f11a2ad5354364f9e27286401cb8a9c780c33f774b7
  1cfd8375`); two clean builds identical; the suite re-run against
  the dist copy itself.
- `sh_gate_update` verified at 0x03000a2c in the final map (flock
  0x03000000 and predator 0x030006d8 unmoved).
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/shoal-gates.md` retired with this flip.
