# Session 54 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/courier-audio`, 00:42Z → 01:12Z)
- mission: **CLOCKWORK COURIER growth rung 4 — AUDIO, the LAST named
  cut.** The final item of `games/clockwork-courier/CONCEPT.md` @
  main `47985ab`. **With this rung the Courier concept is FULLY
  BUILT** — every named item of the pitch and its scope ladder is on
  cartridge behind committed headless proofs.
- inbox check: control/inbox.md at HEAD read fully — ORDERS 001-005
  only; no new unserved ORDER. No preemption.
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #108 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.

## 💡 Session idea

**A hard-won trap is only worth its cost if the NEXT engine gets the
verdict cheap — front-load the check, and let "the trap did not
fire" be a one-line result instead of a mid-session diagnosis.**
Shoal's audio rung burned hours discovering that the mixer's base
load tips vblank boundaries; this session ran that check FIRST (the
full-row cross-ROM diff before any proof work) and got the clean
verdict in one command: 0 differing rows at the same frame indices —
Courier's transition frames are light, so the trap physically cannot
fire the way Shoal's 50-sprite spawn spike did. Everything after
that was pure addition: every carried literal stood verbatim, the
audio asserts stacked on top, and the whole suite went green on the
first post-pin run. The general rule for a lane that keeps its
lessons in guard recipes: **convert every previous disaster into a
cheap ordered PRE-check (diff first, then build), and record the
negative result explicitly** — "no shift, here is the diff" is
evidence the next session can lean on, where silence would just
restart the investigation. Smaller find: on a shared decision-ledger
(gl_audio_hook) the LEVELS chain becomes an audio integration test
for free — 4 starts / 4 rewinds / 8 door edges / 6 window ticks
accumulated across three floors pin the cue wiring at every seam
without one extra input frame.

## Previous-session review

- Session 53 (Courier levels): its guard recipe WAS this session's
  execution plan, almost line for line — the cue sketch (start
  deferred / pickup / rewind / door / delivery / window tick) shipped
  exactly as written, and its "run the full-row cross-ROM diff first"
  instruction produced the clean clock verdict above.
- Its tape-design 💡 paid an unexpected audio dividend: because the
  LV2/LV3 doors open via ghost choreography, the door-edge counter
  pins (3 then 7 then 8 edges) encode the whole ghost dance as a
  one-word assertion.
- Its tooling note (assert the replace target exists) was applied to
  every proofs.sh edit this session; no silent no-op edits occurred.

## What this session did

1. Claim + born-red card first (`d2af520`), PR #108 opened READY
   immediately after, cut from main @ `47985ab`.
2. **The cue set** (`games/clockwork-courier/audio/generate_audio.py`
   — stdlib-only, byte-exact regeneration verified, no samples ever;
   maxmod via `AUDIO := audio`): `cc_start` (a mainspring winding,
   D4→A4 — DEFERRED one frame off the run transition, the Shoal
   rule), `cc_pickup` (a bright shelf plink), `cc_rewind` (time runs
   backwards — a falling sweep with ratchet flutter), `cc_door` (a
   low brass clunk-and-slide), `cc_deliver` (a two-note resolve over
   a soft thump), `cc_tick` (one escapement click when the chute
   window opens). The tower's palette: brass and escapements.
3. **Pure decision layer**: cues fire from events the sim already
   computed; B toggles MUTE (gates play() ONLY — counters always
   bump; title gains "B: MUTE"). Nothing writes back into the sim.
   Hook slots: starts / pickups / rewinds / door-openings /
   deliveries / window ticks / mute state.
4. **The clock verdict, front-loaded**: full-row cross-ROM diff of
   the classic route on the rebuilt v0.4 dist (sha256-verified
   `bed161b5…c5e0`) vs the audio build — **0 differing rows over 700
   frames at the SAME indices** (the +1-offset comparison shows 689
   diffs, confirming no shift). The Shoal spawn-spike trap did not
   fire; **zero re-pins, zero re-bases** — the carried suite passed
   UNCHANGED before a single audio assert was added.
5. **Proofs** (committed suite extended): **8 proofs / 231 asserts
   (P1 11 · P2 13 · P3 19 · P4 32 · P5 21 · P6 10 · P7 44 · P8 81)**,
   all green on the first post-pin run; P4/P5/P7/P8 run-twice
   byte-identical. The audio ledger pinned across all four chains:
   P4 (1 start / 1 pickup / 1 rewind / 1 door / 1 delivery, restart
   makes starts 2), P7 (2 pickups, 3 window ticks, the delivery on
   the tick frame), P8 (the tower campaign accumulates 4 starts /
   3 pickups / 4 rewinds / 8 door edges / 3 deliveries / 6 ticks —
   including LV1's doors 0, the no-door floor pinned silent); mixer
   voicing (264 nonzero words) at rewind and delivery cues, silence
   (0) pinned between and at the muted-flag title check.
6. **Ship**: `dist/clockwork-courier.gba` **v1.0 CONCEPT COMPLETE**
   145,752 B (~23KB soundbank), sha256 `ad73419f…c75c`,
   byte-deterministic (two clean builds identical; the suite re-run
   green against the exact dist bytes); WAV regeneration
   byte-stable; CONCEPT.md closes the ladder ("FULLY BUILT");
   dist/README v1.0 row; heartbeat appended with live `date -u` ts
   and the confirmed PR number.

## Guard recipe (Courier is CONCEPT-COMPLETE — for whoever touches it next)

- There is no next named cut. Anything further (sprite art, more
  floors, packaging per the CONCEPT's honest sellability guess) is
  NEW scope: one-page concept first, owner-routed.
- The clock map at v1.0: no transition frame skips extra vblanks
  even with the mixer live — but ANY future heavy frame (sprite art!)
  re-runs the full-row diff first; cc_telemetry has no CPU words, so
  any drift IS a clock shift.
- The reachability arithmetic stands (apex 9922 / boost band
  [32, 38.76)); the window constants stand (240/60); the four
  run-twice chains (P4/P5/P7/P8) are the regression floor.
- `games/clockwork-courier/proofs.sh` runs ~6 minutes; before AND
  after any change, always.

## Session enders

- 8 headless proofs / 231 asserts green via the committed suite;
  P4/P5/P7/P8 each run twice back-to-back byte-identical; the clock
  verdict measured (no shift) and every carried literal verbatim.
- From-source rebuild == committed `dist/clockwork-courier.gba`
  bit-for-bit (sha256 `ad73419f71cee59236fcd8c3c8efe59c26ab4f7e54c7
  bcdbcf8d486e3afcc75c`); two clean builds identical; the suite
  re-run against the dist copy itself; `audio/generate_audio.py`
  re-run leaves the committed WAVs byte-identical.
- `gl_audio_hook` (0x03001d68) and the maxmod mixing buffer
  (0x03001e48) resolve in the shipped ELF.
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/courier-audio.md` retired with this flip.
