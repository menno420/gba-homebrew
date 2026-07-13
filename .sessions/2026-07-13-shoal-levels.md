# Session 49 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/shoal-levels`, 21:28Z → 22:12Z)
- mission: **SHOAL growth rung 4 — MULTIPLE TUNED LEVELS.** The next
  named deliberate cut in `games/shoal/CONCEPT.md` @ main `b04be5a`;
  the committed concept's S3 scope names it "4 levels with tuned
  flock parameters + rating screen"
  (`docs/concepts/session-1-concepts.md`). Levels only — audio stays
  cut.
- dedup at claim time: no shoal claim, no open Shoal PR (search
  `is:open shoal` = 0); inbox at HEAD unchanged — nothing new
  unserved.
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #102 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.

## 💡 Session idea

**A "byte-identical carried mode" claim is really TWO claims — same
simulation AND same clock — and a heavy mode-transition frame can
break the second without touching the first.** This slice added a
few hundred cycles to the run-start frame (level branches in the
first HUD draw), and the HUNGRY water's transition — which was
sitting exactly on a vblank boundary — started skipping one more
frame: boot-lag 1 → 2. The sim stayed bit-identical in run-frame
terms (proven: re-diffing old-vs-new hungry telemetry with a +1 row
offset left ZERO differing run rows), but every scripted hungry
route now delivered its keys one run-frame early, and the committed
graded-loss route quietly became a different play (eaten 15 ≠ 16 at
the pinned frame). Calm and gated never moved — their transitions
sat comfortably clear of the boundary — which made the failure look
like a sim bug in one water. Rules extracted: (1) **when a carried
scripted proof fails after a "display-only" change, check the
mode's boot-lag BEFORE suspecting the sim** — diff the two telemetry
logs at ±1 row offset first; it is a one-liner and it classifies the
failure instantly; (2) **idle proofs with plateau pins are
shift-immune, scripted routes are not** — P3 sailed through the same
shift that broke P6, which is an argument for keeping one idle proof
per water forever; (3) the fix is a mechanical ROUTE re-base (+1 on
every post-press span), not a game-state re-pin — the route is the
thing that owns the clock.

## Previous-session review

- Session 48 (Shoal ratings): its guard recipe called this rung's
  two laws in advance and both held — "levels = the first rung that
  may genuinely need the knob retuned per-level" (it did, and the
  coordinator pre-authorized it under the all-three-together rule),
  and "star thresholds are DERIVED — a levels rung gets its star
  lines for free" (they came for free: per-level goals fed
  `stars_of` unchanged).
- Its P6-is-knife-edge warning was exactly right, just for a reason
  it could not have guessed: P6 broke not from a knob change but
  from the one-hw-frame transition shift (the 💡 above); its own
  "check P6 before believing a regression elsewhere" advice is what
  routed the debugging.
- Its 💡 (loss-grading is where rating design lives) fed this rung's
  card lines directly: the level cards reuse the rated
  win/scattered machinery with per-level goals, no new grading code
  at all.

## The level set (each knob triple deliberate, all-three-together)

| lvl | name | hunters | ring/den | gates | goal | why this triple |
|---|---|---|---|---|---|---|
| L1 | THE SHALLOWS | 0 | — | 1 (high gap) | 40 | teach the funnel with no teeth; calm's own goal proves the single wall costs nothing but time (rf 2881 vs calm's 1619-era sweeps) |
| L2 | THE HUNT | 1 | 44 px / 300 | 0 | 36 | the SHIPPED knob, halved teeth: one hunter starves on a tight school exactly like the hungry water, so the ring/den carry unchanged; goal 36 measured — the blast-push + gather banks 36 with 3 lost (40 is out of reach: the gather strands the tail within the 44 px ring) |
| L3 | THE NARROWS | 1 | 52 px / 360 | 2 | 32 | gates + hunter is the new interaction: the funnel QUEUES fish at the gaps, and a 44 px ring farms the queue (the #99 "leftovers farmed faster than they re-cohere" failure reborn); ring 52 + den 360 loosened TOGETHER so queuing is survivable but abandonment still kills; goal 32 measured (route banks 32/5) |
| L4 | DEEP WATER | 2 | 52 px / 420 | 2 | 28 | everything at once; second hunter doubles the kill rate, so the den stretches to 420 and the goal drops to 28 — measured 28/7 with the funnel; goal 32 here is NOT reachable by the committed route (saved 31 at eaten 8 trajectory) |

- The hungry water's own triple (44/300/35) is untouched — levels
  carry their OWN table; `sh_pred_update`/`sh_gate_update` were
  parameterized and the carried waters pass the shipped constants
  verbatim (bit-identity re-proven cross-ROM).
- Routes re-derived per level, never re-pinned: L1 re-priced the
  funnel (wins earlier without wall 2); L2's route was derived fresh
  (blast-push + bottom gather + lane sweeps); L3/L4 re-based the
  funnel against their own knobs and the win frames moved as the
  hunters ate (2955 / 1707).

## Re-pins (each justified)

- **Zero game-state re-pins.** Cross-ROM vs the rebuilt v0.4 dist
  (sha256-verified): calm 1300 f and gated 1300 f — 0 non-CPU
  diffs; hungry 1700 f — 0 diffs modulo the one-hw-frame transition
  shift (the 💡; the shifted diff has 0 differing run rows). Every
  committed game-state literal in P1-P6 passed unchanged after the
  P6 route re-base.
- **One route re-base, not a re-pin** (P6): every post-press span
  +1 hw frame; all 15 of its asserts (incl. "CLOCK 57s (3420
  FRAMES)") pass byte-for-byte.
- CPU exacts re-derived once (level branches ride the shared frame):
  P2 2088/2833→2106/2851 (×2), P3 3073→3089 (×2), P4 2927→2988,
  P5 2958→3018, P6 3094→3112. All `t[5] < 4096` rails verbatim, and
  the new chain's rail covers the worst frame the game has ever run
  (82.3%).

## What this session did

1. Claim + born-red card first (`ff73697`), PR #102 opened READY
   immediately after, cut from main @ `b04be5a`.
2. **Parameterized the IWRAM passes** (`sh_pred_update` gains
   straggle_r2/cooldown/hunters, `sh_gate_update` gains the wall
   count; hungry/gated call sites pass the shipped constants
   verbatim). Map re-verified: flock 0x03000000, predator
   0x030006d8, gate 0x03000a30 — all real IWRAM.
3. **The campaign shell**: L at the title starts L1; L on a win card
   advances (wraps after L4), on a scattered card retries; HUD gains
   the "L2 " prefix and per-level goal; cards gain "L2 THE HUNT -
   L: NEXT" / "- L: RETRY"; title gains "L: TUNED LEVELS (4)".
   Telemetry 25 → 26 (words 0-24 frozen; 25 = level index).
4. **Proofs** (committed suite, extended in place): **7 proofs /
   146 asserts (114 watch + 32 text)** — carried P1-P6 (P6 route
   re-based, above) + **P7: ONE deterministic chain plays all four
   levels back to back** — every level won by its own route, level
   words pinned at each hand-off, per-level win pins (saved / LOST /
   run-frame / card text incl. each level's name line and RATING
   ***), the L4 mid-run 1-star HUD ("L4 SAVED 15/28 *"), the L-wrap
   back to L1 — **run twice byte-identical** (so is P4 and P6).
5. **Budget** (per-level, herding frames, t[4]): L1 mean 37.3% /
   worst 75.1%; L2 33.4% / 75.5%; L3 37.1% / 82.1%; **L4 46.5% /
   82.3% (3369/4096)** — two hunters + two walls + a pushed 50-boid
   school is the most expensive frame Shoal has ever run, with
   ~17.7% still banked under the rail.
6. **Ship**: `dist/shoal.gba` v0.5 127,880 B, sha256 `9cd48733…7d9a`,
   byte-deterministic (two clean builds identical; the suite re-run
   green against the exact dist bytes); CONCEPT.md rung 4 recorded
   with the full level table story; dist/README row; heartbeat
   appended with a live `date -u` ts.

## Guard recipe (for the next Shoal rung)

- Remaining named cut: **audio** (the last one). Render-like via the
  proven synth pipeline; expect CPU-exact drift only — but note the
  headroom is now priced by L4's 82.3% worst frame, so any audio
  work that costs sim-side cycles must re-measure the CHAIN, not the
  calm gate.
- The transition-frame vblank boundary is now a known trap: hungry
  sits at boot-lag 2 (was 1), calm at 1, gated/levels at 2. ANY code
  in the input-transition or first-draw path can flip one of these;
  when a scripted proof breaks, diff telemetry at ±1 row offset
  FIRST (one-liner), then re-base the route, never the pins.
- P7's chain is long (8900 frames ×2): its segment bases (12 / 2943
  / 4034 / 7034) hard-code the win frames of the preceding levels —
  ANY level retune re-derives everything downstream of it; retune
  from L4 backwards if you can.
- The level table is display+wiring data in main.cpp; the sim knows
  only the parameters it is passed per frame. A fifth level is one
  table row + one route — but check OAM if it adds sprites, and the
  L4 CPU number before adding teeth.
- IWRAM: the gate pass moved 4 bytes (0x03000a2c → 0x03000a30) when
  the predator signature grew — harmless, but it is a reminder that
  the map addresses are per-build facts: re-grep the map every
  slice, never quote the previous card.

## Session enders

- 7 headless proofs / 146 asserts green via the committed suite; P4,
  P6 and the P7 four-level chain each run twice back-to-back
  byte-identical; carried waters proven bit-identical to the v0.4
  dist cross-ROM (hungry modulo the documented +1 hw-frame shift
  with zero run-row diffs).
- From-source rebuild == committed `dist/shoal.gba` bit-for-bit
  (sha256 `9cd4873303f5bccca555fff98c7251475e7eeeca4e8c60e18871232f
  aeaa7d9a`); two clean builds identical; the suite re-run against
  the dist copy itself.
- IWRAM symbols map-verified this build: 0x03000000 / 0x030006d8 /
  0x03000a30.
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/shoal-levels.md` retired with this flip.
