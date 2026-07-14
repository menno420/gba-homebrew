# Session 52 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/courier-chutes`, 23:03Z → 23:42Z)
- mission: **CLOCKWORK COURIER growth rung 2 — TIMED CHUTES.** The
  next named deliberate cut in `games/clockwork-courier/CONCEPT.md` @
  main `04c6ba7` ("multiple parcels/chutes with timing windows") —
  the pitch's own "delivers parcels to timed chutes", built as a pure
  data + clock rung. Chutes only — more levels and audio stay cut.
- special inbox check FIRST: control/inbox.md at HEAD read fully —
  still ORDERS 001-005 only, no new unserved ORDER; merged PR #104
  (`b4717be`) is the coordinator seat's outbox ack that the EAP night
  kickoff arrived WITHOUT an inbox ORDER ("please re-deliver or
  confirm"). No preemption; reported upstream in the worker output.
- constraint honored: jump_v / gravity UNTOUCHED — the rung-1
  reachability rail (apex 9922 / ghost band [32, 38.76)) was never
  pressured; the window is `(run_frames % 240) < 60`, arithmetic
  only.
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #106 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.

## 💡 Session idea

**Verb-gated modes make "carried" a property of the DESIGN instead of
a property of your diff discipline — and this session got the payoff
in its purest form: the whole carried suite passed UNCHANGED on the
first build.** Shoal learned this pattern under duress (rungs 1-4);
Courier's rung 2 applied it from the first line: the rush order lives
on SELECT, the classic run on START, the new pickup/window/delivery
code sits behind `mode == 1` branches whose mode-0 paths compile to
the old behavior — so P1-P6 (127-32 = 95 carried asserts) went green
with zero edits, before any cross-ROM diff was even run. The diff
then certified what the design already guaranteed (0 differing rows).
Two smaller finds: (1) **the window pin wants a round number** —
opening the shutter on `rf % 240 < 60` made the delivery frame land
on rf 720 exactly, a pin that documents the mechanic's arithmetic in
its own literal (a tuned-feeling window like 250/70 would have pinned
an opaque number); (2) **the 20-sprite text_line cap is a real HUD
budget** — "GHOST OUT PARCELS 2 HELD CHUTE SHUT" silently truncated
mid-glyph on camera ("...CHUTE SHU"), caught only because the text
assert reads pixels, not intentions. HUD strings on this engine are
20 glyphs, spaces free; write them terse or they lie.

## Previous-session review

- Session 51 (ghost-as-platform): its branch-flow erratum ("the
  landing flow is not scaffolding to skip") was this session's
  checklist item #1 — claim + born-red card were the branch's first
  commit, before any build work, and the heartbeat's PR number was
  confirmed against the live PR before committing (its other
  correction).
- Its committed oracle carried the whole load: the P4 route was
  reused VERBATIM as the rush route's chassis (same spans, SELECT
  instead of START), and its reachability arithmetic note is why this
  rung never even considered touching physics.
- Its 💡 ("unreachable is an interval — compute the band first")
  generalized here to time: the window is a band on the clock the
  same way the ledge was a band on height, and the refusal proof has
  the same shape (stand in the right PLACE at the wrong TIME,
  asserted inert).

## What this session did

1. Claim + born-red card first (`f325d9f`), PR #106 opened READY
   immediately after, cut from main @ `04c6ba7`.
2. **THE RUSH ORDER** (SELECT at the title; START and the classic
   run bit-identical by construction): a second parcel waits on the
   step ((6,4) — the exact cell the carried route already lands on),
   and the chute KEEPS HOURS: shutter open 60 frames in every 240.
   Both parcels through an open window end the rush ("RUSH ORDER: 2
   PARCELS" on the card); carrying both at once is allowed; missing
   the window is a WAIT, never a fail — the tower is patient.
   Telemetry 17 → 20 (words 0-16 frozen; 17 mode, 18 delivered,
   19 window). Title gains "SELECT: RUSH ORDER (2)"; the rush HUD
   shows held count + OPEN/SHUT (terse, per the 💡).
3. **Carried proofs**: P1-P6 green UNCHANGED on the new build; P4's
   script replayed cross-ROM vs the rebuilt v0.2 dist
   (sha256-verified `689bc792…6f00`) — **0 differing telemetry rows
   over 700 frames, all 17 rung-1 words, no clock shift; zero
   game-state re-pins.**
4. **P7 THE RUSH ORDER** (committed into
   `games/clockwork-courier/proofs.sh`): the P4 chassis on SELECT —
   the step landing picks parcel 2 (carried word 1 at the same
   14208/10240 literals), the ledge pickup makes it 2, the ghost
   holds the door, and then the REFUSAL: 100+ pinned frames standing
   at the chute with both parcels while the shutter is SHUT
   (delivered 0, state 1, "GHOST OUT 2 HELD SHUT" on camera at 700)
   — then **rf 720, the first open frame, delivers both at once**
   (window word cycle also pinned at rf 9/64: open then shut).
   START from the rush card restarts the CLASSIC run (mode word 0).
   Run twice byte-identical.
5. **Ship**: 7 proofs / 127 asserts (109 watch + 18 text), all
   green; `dist/clockwork-courier.gba` v0.3 122,112 B, sha256
   `e88a83a6…531a`, byte-deterministic (two clean builds identical;
   the suite re-run green against the exact dist bytes); CONCEPT.md
   rung 2 recorded; dist/README row; heartbeat appended with live
   `date -u` ts and the confirmed PR number.

## Guard recipe (for the next Courier rung)

- Remaining named cuts, in CONCEPT.md order: **more levels** (the
  Shoal levels playbook — a level table + per-level routes in the
  oracle; note Courier levels mean new tile maps, so every carried
  literal is per-level from day one), then **audio** (the Shoal
  rung-5 playbook — the MIXER'S BASE LOAD moves clocks, not the
  cues; re-check boot-lag with the full-row diff after enabling).
- The window constants (240/60) are load-bearing for P7's rf-720
  delivery pin and the shut-span asserts; retune them and P7
  re-derives (one route, cheap — but say so).
- The rush chassis IS the P4 route: any change to the classic solve
  re-derives both proofs together.
- HUD budget: 20 glyphs per text_line (spaces free). The win card
  has one free line left (y 40) before it needs a second column.
- proofs.sh runs in ~3 minutes; run it before AND after any change.

## Session enders

- 7 headless proofs / 127 asserts green via the committed suite; the
  delivery, the platform solve, and the rush order each run twice
  back-to-back byte-identical; carried P1-P6 green unchanged +
  cross-ROM certified vs the v0.2 dist (0 differing rows, no clock
  shift).
- From-source rebuild == committed `dist/clockwork-courier.gba`
  bit-for-bit (sha256 `e88a83a6527cf60ce0cee6e66daf61e662c896598
  5f025b5dd5b6d0ba7d9531a`); two clean builds identical; the suite
  re-run against the dist copy itself.
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/courier-chutes.md` retired with this flip.
