# Session 51 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/courier-ghost`, 22:58Z → 23:34Z)
- mission: **CLOCKWORK COURIER growth rung 1 — GHOST-AS-PLATFORM.**
  The first named deliberate cut in
  `games/clockwork-courier/CONCEPT.md` @ main `256e8cb` ("standing ON
  the ghost as a platform"). Platform only — timed chutes, more
  levels, audio stay cut.
- dedup at claim time: no courier claim, no open Courier PR; inbox at
  HEAD unchanged — nothing new unserved.
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #105 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.
- erratum, self-caught mid-session: the first increment commit landed
  ON LOCAL MAIN — the branch/claim/born-red steps had been skipped in
  the rush to build. The push was rejected (remote main had advanced),
  nothing reached the remote; recovered by soft-reset + stash, then
  the proper flow (claim first, born-red card as the branch's first
  commit, PR READY, increment second). Rule: the landing flow is not
  scaffolding to skip when the build is exciting — it is what makes a
  mistake this size recoverable in two commands.

## 💡 Session idea

**On a jump-physics board, "unreachable" is an interval, not a
feeling — compute the exclusive band FIRST and let it place the
geometry.** The whole rung turned on three numbers: a max ground jump
peaks at feet 38.76 px (pinned as 9922), a ghost's head is an 8 px
pedestal, so a boosted jump peaks at 30.76 px — the ghost-exclusive
band is tops in [32, 38.76), exactly one tile row. The first
proof-ground design (a sealed chimney over the spawn) died on contact
with that arithmetic: every wall thick enough to seal the shaft
intruded on a carried trajectory's corridor (the step-jump apex
clips row 3 at col 7; the door-jump apex clips row 3 at col 11), and
a probe proved the v0.1 step route itself flies through cells the
chimney wanted. The design that worked put the ledge INSIDE territory
the door already gates (the chute corridor) and extended the door
wall to the ceiling through cells no carried route ever occupies —
verified not by inspection but by replaying every carried script on
both ROMs and diffing all 16 telemetry words: 0 rows differ. Rules:
(1) derive the reachability interval from pinned physics before
drawing any cell; (2) a "safe" cell is one a replayed-proof diff says
is safe, never one that looks empty; (3) when a mechanic's exclusive
band is one tile tall, the refusal proof is cheap and sharp — pin the
apex literal and let arithmetic testify.

## Previous-session review

- Session 50 (Shoal audio): its lane-complete baton routed this
  session correctly to the next game's rung 1. Its committed-oracle
  pattern (proofs.sh with routes inline) was applied here from the
  start — and for the second game in a row the prototype's proof
  scripts turned out to have died with their container, which
  converts that pattern from "nice hygiene" to the lane's standing
  law: this session's re-derivation walked back into the #96 card's
  own literals (23808 / 3008 / 6144 / 6208) as a cross-check.
- Its boot-lag vigilance transferred as a checklist item: the Courier
  cross-ROM diffs were run full-row expecting a possible ±1 shift —
  none appeared (Courier's transition frame is light; the trap is
  audio-engine-shaped, and this rung ships no audio).
- Its erratum discipline (numbers from measurements, not prose
  arithmetic) is why every pin in proofs.sh was read off a CSV row.

## What this session did

1. **Erratum first** (above): work begun on main was recovered onto
   `claude/courier-ghost`; claim + born-red card became the branch's
   true first commit (`9901c7d`), PR #105 opened READY immediately
   after, cut from main @ `256e8cb`.
2. **The mechanic** (`src/main.cpp`): one-way TOP collision on the
   ghost — while falling, horizontally over it, feet crossing its
   head this frame, seat clear → stand (`on_ghost`); riding follows
   the replayed pose each frame (gravity suspended, dislodged if the
   seat squeezes into a wall or overlap is lost); jumping dismounts.
   Rising or sideways never tests it — one-way by construction.
   Telemetry 16 → 17 (words 0-15 frozen; 16 = on_ghost). Title gains
   "AND YOU CAN STAND ON IT".
3. **The proof-ground**: the BELL LEDGE (14,4) tops out at 32 px in
   the chute corridor; the door wall extended to the ceiling
   ((12,1)-(12,3)). Every new cell sits where no carried trajectory
   flies — proven by cross-ROM replay (below), not by eyeball. The
   first design (a chimney over the spawn) was measured into
   retirement per the 💡.
4. **Carried proofs, re-derived and COMMITTED**
   (`games/clockwork-courier/proofs.sh`, the Shoal oracle pattern):
   P1 boot/title; P2 kinematics + the door refusal (clamp 23808
   grounded AND mid-air, apex 9922); P3 the rewind contract (fill
   300, snap 13888, ghost replay pinned to the player's own recorded
   literal 301 frames apart — 11008 at both ends, switch/door held
   from afar, expiry shuts the door); P4 the delivery (step 10240 →
   platform 6144 → parcel 3008 → switch 6208 → rewind → chute at
   rf 599, card text exact, START restarts) — **cross-ROM carry:
   P2/P3/P4 replay on the v0.1 dist (rebuilt, sha256-verified
   ed877798…40a8) and this build with 0 differing telemetry rows over
   1800 frames — zero game-state re-pins, no clock shift.**
5. **The rung's own proofs**: **P5 GHOST-AS-PLATFORM used in a full
   solve** — the delivery detours into the corridor, a second rewind
   parks a ghost under the ledge, the courier mounts its head
   (on_ghost=1, feet 14336), boosts onto the ledge (**feet 8192 =
   32 px, grounded, x 28288**), steps off across the ghost's head
   again, and delivers with **REWINDS 2** at rf 1054, card text
   exact — run twice byte-identical (P4 likewise). **P6 the
   counter-solve fails**: max-jump+drift at the ledge peaks at 9922
   with the flank clamping x at 27648; jumping under the ledge bumps
   at 12408; the door-side hop bumps at 14452; the ledge is never
   stood on and the run never ends.
6. **Ship**: 6 proofs / 95 asserts (82 watch + 13 text), all green;
   `dist/clockwork-courier.gba` v0.2 121,600 B, sha256
   `689bc792…6f00`, byte-deterministic (two clean builds identical;
   the suite re-run green against the exact dist bytes); CONCEPT.md
   rung 1 recorded; dist/README row; heartbeat appended with live
   `date -u` ts.

## Guard recipe (for the next Courier rung)

- Remaining named cuts, in CONCEPT.md order: multiple parcels/chutes
  with timing windows, more levels, audio. Timed chutes are a DATA +
  clock rung (windows over run_frames); more levels re-plays the
  Shoal levels playbook (a table + per-level routes in the oracle);
  audio is the Shoal rung-5 playbook — and remember its finding: the
  MIXER'S BASE LOAD, not the cues, is what moves clocks. Courier's
  transition frame is light, but re-check boot-lag with the full-row
  diff after enabling audio.
- The reachability arithmetic is now load-bearing everywhere: ground
  apex 38.76 px (9922), head boost apex 30.76 px, exclusive band =
  tops in [32, 38.76). ANY change to jump_v/gravity re-prices every
  ledge and re-derives every route — the #96 warning, now with the
  suite committed so the re-derive is mechanical.
- The corridor park spot must stay ≥ 108.0 (one pixel-step clear of
  the closed door's cells) or the never-crush rule blocks the
  ghost-seat test — the 15702-vs-16384 bug in this session's first
  suite run was exactly that off-by-one-frame subtlety.
- proofs.sh runs in ~2 minutes; run it before AND after any change.

## Session enders

- 6 headless proofs / 95 asserts green via the committed suite; the
  delivery and the platform solve each run twice back-to-back
  byte-identical; carried P2/P3/P4 proven bit-identical to the v0.1
  dist cross-ROM (0 differing rows, all 16 words, no clock shift).
- From-source rebuild == committed `dist/clockwork-courier.gba`
  bit-for-bit (sha256 `689bc792e32fd33de7fd66323a6bc1b5d8682008e1bac
  875528ac5d5cd766f00`); two clean builds identical; the suite re-run
  against the dist copy itself.
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/courier-ghost.md` retired with this flip.
