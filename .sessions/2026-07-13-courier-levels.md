# Session 53 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13/14 (branch `claude/courier-levels`, 23:47Z → 00:38Z)
- mission: **CLOCKWORK COURIER growth rung 3 — MORE LEVELS.** The
  next named deliberate cut in `games/clockwork-courier/CONCEPT.md` @
  main `04802ed` ("more levels"), per the Shoal levels playbook:
  three genuinely distinct new tile maps, each teaching or combining
  the shipped mechanics. Levels only — audio stays cut.
- inbox check: control/inbox.md at HEAD read fully — ORDERS 001-005
  only; the missing EAP night ORDER is known upstream. No preemption.
- constraints honored: jump_v/gravity UNTOUCHED (apex 9922 priced
  every shelf); window constants 240/60 and the rush chassis
  untouched; classic/rush verbs bit-identical (all modes read one
  current-floor view — classic and rush pass the original literals
  verbatim, certified cross-ROM).
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #107 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.

## 💡 Session idea

**On a rewind-tape engine, level design is TAPE design — every route
is really two routes, and the second one is played by a ghost you
choreographed by accident.** Three separate derivation surprises in
one session, all the same lesson: (1) LV2's door opened not because
the ghost STOOD on the switch but because its replayed walk AMBLED
ACROSS it — two frames of transient switch-hold that the courier,
already pressed against the door face, threads deterministically;
the "wrong" solve is legitimate, pinned, and honestly more elegant
than the intended one. (2) LV3's first attempt died because R snaps
the player to the TAPE START — the post-rewind walk began from a
mid-walk pose 300 frames old, not from where the player pressed R;
every segment after a rewind must be scripted from the SNAP
position. (3) The ghost's standing tail landed on x=48.0 — cell 6,
one pixel past the switch cell — and the door never opened: on a
cell-quantized board, where you STOP is a pin, and "about there" is
a different cell. Rule extracted: **derive rewind puzzles backwards
from the tape — decide what the ghost must be doing in its last 150
frames, and script the player's pre-rewind movements to record
exactly that; the player's own convenience comes second.** Also
re-learned twice in one file: the 20-sprite text_line cap (the level
card needed name and hint on separate lines) — and a silent tooling
lesson: a python edit that prints success unconditionally can lie
(the P8 block was "committed" once without landing; the fix asserts
the target exists before replacing).

## Previous-session review

- Session 52 (timed chutes): its guard recipe drove the design —
  "Courier levels mean new tile maps, so carried literals go
  per-level from day one" became the floor_def table with every
  named cell per level; "the mixer's base load moves clocks" stays
  parked for the audio rung; the window constants were reused
  verbatim (240/60), and its rf-480/720 round-number pin habit gave
  LV2/LV3 their delivery pins on window edges (480, 960).
- Its 💡 (verb-gated = carried-by-design) was extended one level
  deeper here: not just a verb gate but a current-floor VIEW —
  classic and rush read the original literals through the same
  pointer the levels use, so the carried suite passed unchanged on
  the first successful build.
- Its HUD-cap warning fired again anyway (the level card line) —
  budget rules get re-learned until they live in a table; it is in
  this card's recipe now with the arithmetic.

## The level set (each floor distinct, mechanics earn their maps)

| floor | map | mechanics | goal | why |
|---|---|---|---|---|
| L1 THE HIGH SHELF | new 16x9: bare hall, shelf (6,4) top 32 px, chute (14,7) | ghost-boost ONLY (no door, no switch, untimed chute) | parcel on the shelf | teach the boost in isolation: the shelf sits in the boost-exclusive band [32, 38.76) — ghostless probes pinned at apex 9922; win rf 597, 1 rewind |
| L2 THE NIGHT SHIFT | new 16x9: parcel platform (0-1,6), capped door col 9, timed chute (13,7) | rewind-ghost door + TIMED window | parcel through the door in an open window | compose rung-0 and rung-2: the ghost's replayed walk threads the door (transient switch-hold, pinned) and the shut window holds the courier 40+ pinned frames; delivery lands rf 480 — the window edge; 1 rewind |
| L3 THE FULL ROUND | new 16x9: shelf (2,4), capped door col 10, switch (5,7), timed chute (13,7) | boost + switch-standing ghost + window | everything in one delivery | the full composite: rewind 1 boosts the parcel off the shelf, rewind 2 parks a ghost ON the switch; delivery at rf 960 with REWINDS 2 |

- The window is shared data (240/60 verbatim); no knob was retuned.
- L on a win card advances; wraps to L1 after L3 (pinned).

## What this session did

1. Claim + born-red card first (`cffded1`), PR #107 opened READY
   immediately after, cut from main @ `04802ed`.
2. **The current-floor view** (`floor_def`): map pointer + per-level
   named cells (switch/parcel/parcel2/chute/door/timed) — the sim
   reads ONE view in every mode; classic (`base_classic`) and rush
   (`base_rush`) pass the original literals verbatim, so their sims
   are bit-identical by construction. Telemetry 20 → 21 (words 0-19
   frozen; 20 = floor index). Title gains "L: THE TOWER SHIFTS (3)";
   level win cards show "L2 THE NIGHT SHIFT" + "L: NEXT SHIFT"
   (separate lines — the 20-sprite cap).
3. **Three floors** (table above), derived empirically per segment
   with the rewind-tape lessons in the 💡.
4. **Proofs** (committed suite extended): **8 proofs / 185 asserts
   (158 watch + 27 text)** — carried P1-P7 green UNCHANGED + **P8
   THE TOWER SHIFTS**: one deterministic 2300-frame chain plays all
   three floors — per-floor refusal probes (apex 9922 at the shelf,
   twice; the LV2/LV3 shut-window spans) and per-floor wins (rf 597
   / 480 / 960, card texts exact incl. floor names) — then the
   L-wrap back to floor 1 — **run twice byte-identical** (P4/P5/P7
   likewise). Cross-ROM: the rush chassis replayed on the rebuilt
   v0.3 dist (sha256-verified `e88a83a6…531a`) vs this build — **0
   differing rows over 850 frames, all 20 words, no clock shift;
   zero game-state re-pins.**
5. **Ship**: `dist/clockwork-courier.gba` v0.4 122,540 B, sha256
   `bed161b5…c5e0`, byte-deterministic (two clean builds identical;
   the suite re-run green against the exact dist bytes); CONCEPT.md
   rung 3 recorded with the floor table; dist/README row; heartbeat
   appended with live `date -u` ts and the confirmed PR number.

## Guard recipe (for the next Courier rung — AUDIO, the last one)

- Audio = the Shoal rung-5 playbook, with its one hard finding front
  and center: **the mixer's BASE LOAD, not the cues, moves clocks**
  — enabling maxmod may add a constant per-frame cost that shifts a
  vblank boundary on the heaviest transition frame. Courier's
  transitions are light (no 50-sprite spawn), so a shift is LESS
  likely than Shoal's — but run the full-row cross-ROM diff first
  and expect the suite-wide ±1 re-base if it fires. cc_telemetry has
  no CPU words, so any drift IS a clock shift, never noise.
- Cue set sketch (pure decisions on existing events): run start
  (deferred one frame off the transition, the Shoal rule), parcel
  pickup, rewind fired, door open, delivery, window-open tick.
  gl_audio_hook + mixer-memory evidence per docs/capabilities.md.
- The tape-design rule (💡) applies to any future floor: choreograph
  the ghost's tail first. Stop positions are cell-quantized pins.
- proofs.sh runs ~5 minutes now; still run it before AND after.

## Session enders

- 8 headless proofs / 185 asserts green via the committed suite; the
  delivery, the platform solve, the rush order, and the three-floor
  tower chain each run twice back-to-back byte-identical; carried
  P1-P7 green unchanged + rush chassis cross-ROM certified vs the
  v0.3 dist (0 differing rows, no clock shift).
- From-source rebuild == committed `dist/clockwork-courier.gba`
  bit-for-bit (sha256 `bed161b5b2b7c69a583b2c0b412f0c855c1a66cee44f
  b3458f5cd731c0ffc5e0`); two clean builds identical; the suite
  re-run against the dist copy itself.
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/courier-levels.md` retired with this flip.
