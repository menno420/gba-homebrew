# game-lab seat heartbeat — control/status.md

updated: 2026-07-12T16:20:12Z

Coordinator-only; overwritten wholesale each coordinator turn. Prior
narrative in git history (`0a76b546` boot, `d06456f2` c2, `a7d2e113` c3, `baa39b9b` c4).

- **ts:** 2026-07-12T16:20:12Z
- **phase:** WORK LOOP cycle 5 (merged seat v1) — idle on owner clicks, both next slices pre-built
- **chain:** healthy — pacemaker chain re-armed each turn (serialize-on-busy doctrine, documented c3); failsafe `trig_01JD1t7rD5jUCqkJQJaNCi3E` cron 50 */2 * * * firing on schedule (12:50Z + 14:50Z fires delivered).

## Parked READY+green (owner clicks; either order — coordinator re-parks whichever goes stale)
- PR #68 — Gloamline slice 8 "synthesized audio" (session 30): head `8d4245e`, 19 proofs / 353 asserts, zero re-pins (6th hold), dist sha256 `eab78ae0…215d`. CI 29192781139 + 29192781148 green.
- PR #69 — Brineward slice 5 "the Maw" (session 31): head `7c64f6a`, 12 proofs / 265 asserts, dist sha256 `6e571941…2af7c`. CI 29192807193 + 29192807204 green.

## Pre-built and queued (verified locally, push on merge signal)
- Gloamline slice 9 "best-nights saves" (session 33): 23 proofs / 399 asserts, zero re-pins (7th hold), dist sha256 `dc8798c8…beae`; includes an in-slice fix for a libnds-vs-DeSmuME EEPROM HOLD-bit protocol incompatibility. Queued on #68.
- Brineward slice 6 "wind + sailing feel" (session 34): 21 host checks, 14 proofs / 318 asserts, zero re-pins via calm-anchor seed salt `BW_WIND_SALT 0x57499826`, dist sha256 `2a83ba6d…49c9`. Queued on #69.

## Cross-seat note (2026-07-12 afternoon)
- Owner ordered a fleet-wide consolidation + reset plan in this seat's chat by mistake (meant for fleet-manager). This seat produced read-only research (2 independent 19-repo surveys + adversarial verification) and one artifact — fleet-manager PR #121 (proposal, parked READY+green, codex-reviewed: 3 real findings verified + fixed at `29c9be8`) — then handed off to the fleet-manager coordinator session, which now owns adoption/dispatch. No fleet changes were made from this seat; no game-lab state was affected.

## Per-track state
**Track B — gba-homebrew** (R22 `"private":false`/`"visibility":"public"` PASS, last check ~13:25Z): main = `793d20b7` at landing time — moved past `baa39b9b` by two external-seat substrate-kit upgrade merges (#71 v1.14.0, #72 v1.15.0; named cards, no session-number claims). Inbox: ORDERS 001–004 only, executed; no new orders (re-checked at landing, HEAD `793d20b7`).

**Track A — pokemon-mod-lab** (R22 `"private":true`/`"visibility":"private"` PASS ~13:25Z; strict isolation intact): 0 open PRs, honest idle, owner-gated (its main moved 12:10Z — that repo's own lane activity, not this seat's). Resume anchor: `docs/retro/archive-ready-2026-07-11.md` (that repo).

## ⚑ needs-owner
- Merge clicks: PR #68 + PR #69 (each releases a pre-built next slice).
- melonDS enhancement lane go/no-go (owner asked 2026-07-12 ~14:15Z; complaint research delivered; proposed scope: fork melonDS-android — audio crackle fix, DraStic save-import wizard, layout parity, fast-forward fix).
- Standing: canonical merge clause in control/README.md; NDS jobs → required checks.
- (carried) pokemon-mod-lab OWNER-ACTION items.

## Dispatch 2026-07-12 (session 37, coordinator-seat dispatch)

- **ts:** 2026-07-12T21:27:13Z (this section APPENDED by the session-37
  worker on coordinator-seat dispatch; the cycle-5 body above is left
  intact per the dispatch's append-only instruction)
- **Staleness note:** the cycle-5 heartbeat above is STALE vs HEAD — it
  lists PR #68 (Gloamline slice 8) and PR #69 (Brineward slice 5) as
  parked awaiting owner clicks; both have since MERGED (`27e8851`,
  `a5b5325`), and the queued Gloamline slice 9 also landed (PR #74,
  `4ad557d` = main at this session's start).
- **Shipped this session:** Gloamline slice 10 "WATCH-MAP POLISH" —
  chalk mark (X buttons-first; optional map-tap alias via the pure
  gl_mark_of_cell/gl_mark_of_touch with exact cell round-trips), OUT
  watch line (gl_gloam_out), BEST on the map header. PR #75 (branch
  `claude/gloamline-watch-map`), commits `18886c0` (card+claim),
  `6ad6a80` (pure layer + mirrors), `b9547f7` (glue + harness
  --touch), `e1899f0` (CI proofs 24-26), `a8fd81f` (dist + docs).
  26 proofs / 464 emulator asserts, all 399 pre-slice-10 asserts
  byte-identical (zero re-pins, 8th hold); stress budget steady max 70
  (mean 57.3) with the mark live; dist 118,272 B sha256
  `2ec0afa4…0749`. CI run ids recorded on the PR after the final push.
- **Parked-PR landing path:** PR #75 parked READY (non-draft), no
  self-merge — owner click is the default; alternatively a successor
  session may land it via non-author review-merge per lane precedent.
- **Baton (next 2 tasks):** (1) Brineward slice 6 "wind + sailing
  feel" is pre-built on branch `claude/brineward-wind` (session 34,
  verified locally per the stale section above) and still has NO PR —
  next wake should open it READY on post-#75 main and re-verify.
  (2) The Gloamline concept tree is now COMPLETE (all eight LATER
  SLICES items shipped) — further Gloamline arc work is owner-gated;
  do not invent slices beyond the concept doc.

## Dispatch 2026-07-13 (Brineward slice 9 — saves, PR #94)

- **ts:** 2026-07-13T18:46:26Z (appended by the session-43 worker;
  everything above left intact per the append-only dispatch rule)
- Brineward slice 9 (roadmap item 7, the arc's final named beat:
  "saves: banked gold / upgrades / chart persist") built on branch
  `claude/brineward-saves` from main @ `97e0117` (slice 8 merged as
  PR #91) — Gloamline's proven EEPROM path ported; 32 host checks +
  22 proofs / 616 emulator asserts green in-container, zero re-pins.
- Root-cause fix ridden along in `tools/nds-headless-check.py`:
  DeSmuME keeps an implicit global battery per ROM basename
  (`~/.config/desmume/<rom>.dsv`), so save-writing proofs leaked
  state into later proof runs — the harness now imports a fresh blank
  chip whenever `--battery-in` is absent (hermetic by default; both
  NDS suites re-verified green).
- PR #94 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-13 (Clockwork Courier prototype, PR #96)

- **ts:** 2026-07-13T19:16:53Z (appended by the session-44 worker;
  everything above left intact per the append-only dispatch rule)
- Breadth game #7 per ORDER 005 rule 3's work-finding precedence: the
  track plan at HEAD (`docs/concepts/session-1-concepts.md`) is the
  committed ORDER-001 concept queue — **Clockwork Courier** (its next
  unbuilt item, risk MEDIUM) built as a playable prototype slice on
  branch `claude/clockwork-courier` from main @ `8bac80a`; Shoal
  (risk MEDIUM-HIGH) remains queued behind its own doc-mandated
  profiling gate.
- The hook is proven on camera: 4 headless proofs / 75 asserts (69
  watch + 6 text) — the rewind snap and the ghost's replay pinned to
  IDENTICAL literals 301 frames apart, the ghost-held door crossed
  with the parcel, the win card text exact, run-twice byte-identical;
  a probe-found door-jump exploit was closed in the level before
  pinning. `dist/clockwork-courier.gba` byte-deterministic.
- PR #96 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-13 (Shoal, PR #98)

- **ts:** 2026-07-13T19:50:57Z (appended by the session-45 worker;
  everything above left intact per the append-only dispatch rule)
- Breadth game #8 = **Shoal**, the ORDER-001 concept queue's LAST
  item, built BEHIND ITS OWN COMMITTED GATE ("profile in the FIRST
  session, pivot cheap if the budget says no"): measured on emulated
  hardware via the pinned Butano CPU meter — naive O(n^2) 50-boid
  flock **120.5% of a frame (FAIL)**; the concept's mitigation ladder
  (precision drop + neighbor grid + deterministic 8-neighbor cap) +
  ARM code in REAL IWRAM (`section(".iwram")` — the *.bn_iwram.cpp
  build rule alone does NOT place it) → **52.4% mean / 68.6% steady
  worst = PASS**. The boid slice SHIPPED, no pivot needed; the CPU
  words ride the telemetry mailbox so the budget is a pinned
  regression test.
- 3 headless proofs / 42 asserts green (boot; the gate + determinism;
  a full scripted herd — 40/50 saved at run-frame 1619 with the win
  card text pinned — plus instant restart), run-twice byte-identical.
  `dist/shoal.gba` byte-deterministic. **The ORDER-001 concept queue
  is now FULLY BUILT** (Lumen Drift complete, Courier + Shoal
  prototyped).
- PR #98 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-13 (Shoal predator pass, PR #99)

- **ts:** 2026-07-13T20:15:42Z (appended by the session-46 worker;
  everything above left intact per the append-only dispatch rule)
- Shoal growth rung 1 = **the predator pass** (CONCEPT.md's first
  named cut, "predators locking onto stragglers"), on branch
  `claude/shoal-predator` from main @ `e59b26d`. Input-verb gated
  (SELECT = hungry water; START = calm — every carried game-state pin
  bit-identical). Straggler THRESHOLD is the design find: only fish
  >44 px from the flock centroid are prey — a tight school starves
  the hunters; abandonment kills. Hungry water asks 35 home (measured:
  40 is unreachable against the hunters; the calm sweep banks 35/8).
- Budget rails held and extended: hungry-water idle mean 50.0% / p99
  70.4% / steady worst 75.1% (3075/4096), 74.0% through the winning
  herd; the t[5] < 4096 pin now covers predator frames; predator pass
  IWRAM-placed at 0x030006d8 (map-verified). Re-pins: exactly 4, all
  CPU-measurement literals (+~0.5% from the new per-frame mode gate),
  justified in the card; zero game-state re-pins.
- 6 headless proofs / 88 asserts green (carried boot/gate/calm-win +
  hungry idle-starvation, the winnable hunt, the scatter lose), the
  hungry win run-twice byte-identical. `dist/shoal.gba` v0.2
  byte-deterministic.
- PR #99 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-13 (Shoal gates, PR #100)

- **ts:** 2026-07-13T20:51:25Z (appended by the session-47 worker;
  everything above left intact per the append-only dispatch rule;
  ts corrected at the flip — first committed pre-written, the
  session-45 erratum repeated and re-caught)
- Shoal growth rung 2 = **the gates** (CONCEPT.md's next named cut;
  the committed pitch's "through gates"), on branch
  `claude/shoal-gates` from main @ `c5d3cd6`. Input-verb gated (R =
  the gated run; START/SELECT untouched): two static coral walls
  with OFFSET gaps — the school must be funneled, and the committed
  route banks 40/50 at run-frame 3519. The hungry water's coupled
  difficulty knob (44 px / den 300 / goal 35) untouched by design.
- Carried pins proven cross-ROM this time: the prior sessions' local
  proof scripts died with the container, so calm+hungry idle runs
  were replayed on the merged v0.2 dist AND the new build — zero
  game-state diffs over 3000 frames (CPU words only). The fix is
  committed: `games/shoal/proofs.sh` now carries the full suite
  (routes + asserts), so future carried-pin checks are re-runnable.
- 5 proofs / 82 asserts green (boot; calm gate; hungry starvation;
  the gated win run-twice byte-identical + calm restart; the idle
  discriminator — same no-input run banks 5 calm vs 0 gated). Budget:
  win-route mean 35.9% / p99 57.2% / worst 71.2% (2916/4096), gated
  idle worst 72.0% (2949); `t[5] < 4096` rail covers gate-active
  frames; `sh_gate_update` in IWRAM at 0x03000a2c (map-verified).
  `dist/shoal.gba` v0.3 byte-deterministic.
- PR #100 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-13 (Shoal ratings, PR #101)

- **ts:** 2026-07-13T21:16:29Z (appended by the session-48 worker; everything
  above left intact per the append-only dispatch rule; ts taken from
  `date -u` at write time — the sessions-45/47 erratum not repeated)
- Shoal growth rung 3 = **star ratings proper** (CONCEPT.md's next
  named cut; the committed pitch's "Star rating = fish saved"), on
  branch `claude/shoal-ratings` from main @ `253695d`. A pure
  display-time grading per water: 3 stars = the water's existing goal
  (calm/gated 40, hungry 35 — the coupled 44 px / den 300 / goal 35
  knob untouched, as constrained), 2/1 stars at thirds; stars ride
  the HUD live and BOTH end cards grade the run — losses included
  ("RATING -" on a near-goalless scatter). The sim never consults a
  star.
- Zero game-state re-pins, re-proven cross-ROM: calm + hungry + gated
  idle telemetry diffed against the rebuilt v0.3 dist over 4300
  frames — 0 non-CPU diffs; CPU exacts re-derived (+3..+15/word, the
  HUD-stars render cost), all `t[5] < 4096` rails verbatim.
- The committed suite `games/shoal/proofs.sh` extended in place:
  6 proofs / 102 asserts green (all four grades pinned on screen:
  "-" / "*" / "**" / "***"), the gated win AND the new graded-loss
  scatter each run twice byte-identical. `dist/shoal.gba` v0.4
  byte-deterministic.
- PR #101 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-13 (Shoal levels, PR #102)

- **ts:** 2026-07-13T21:45:47Z (appended by the session-49 worker; everything
  above left intact per the append-only dispatch rule; ts from
  `date -u` at write time)
- Shoal growth rung 4 = **multiple tuned levels** (CONCEPT.md's next
  named cut; the concept's S3 "4 levels with tuned flock
  parameters"), on branch `claude/shoal-levels` from main @
  `b04be5a`. A fourth verb (L) starts a four-level campaign, each a
  distinct deliberate parameter set: L1 THE SHALLOWS (1 gate, no
  hunters, 40) · L2 THE HUNT (one 44px/300 hunter, 36) · L3 THE
  NARROWS (2 gates + one 52px/360 hunter, 32) · L4 DEEP WATER
  (2 gates + two 52px/420 hunters, 28). Per-level knob retunes were
  coordinator-authorized and applied all-three-together with
  justification (card); star lines derive from per-level goals.
- Every level proven winnable by its own derived route in ONE
  deterministic chain (proofs.sh P7, run-twice byte-identical):
  40/0 lost @ rf 2881 · 36/3 @ 1050 · 32/5 @ 2955 · 28/7 @ 1707,
  plus the L-wrap back to L1. Worst frame anywhere: 82.3%
  (3369/4096, L4) — the t[5] < 4096 rail covers it.
- Carried waters byte-identical cross-ROM vs the rebuilt v0.4 dist —
  calm/gated 0 non-CPU diffs; hungry 0 diffs modulo a documented
  one-hw-frame transition shift (vblank boundary), its P6 route
  re-based +1 frame with every game-state pin passing unchanged.
  7 proofs / 146 asserts green; `dist/shoal.gba` v0.5
  byte-deterministic.
- PR #102 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-13 (Shoal audio, PR #103)

- **ts:** 2026-07-13T22:12:44Z (appended by the session-50 worker; everything
  above left intact per the append-only dispatch rule; ts from
  `date -u` at write time)
- Shoal growth rung 5 = **AUDIO, the concept's LAST named cut** — on
  branch `claude/shoal-audio` from main @ `751341c`. Five original
  synthesized cues (deterministic `audio/generate_audio.py`, no
  samples ever) via maxmod, fired as pure decisions on events the sim
  already computed (start / bank / eaten / home / scattered) + B-mute
  gating playback only. Zero sim feedback; every trigger counted in
  `gl_audio_hook`, voicing proven by the mixer-memory watch.
- Budget (binding constraint held): P7 four-level chain measured
  before/after — L4 worst frame 82.30% -> 82.35% (3369 -> 3371/4096);
  per-level means +0.1..0.4pp; `t[5] < 4096` everywhere; **no
  scope-down needed**. Known trap fired as documented: the constant
  mixer cost tipped the TITLE->run spawn spike +1 vblank in every
  water (card->run transitions measured unchanged) — every proof
  clock re-based +1 with EVERY game-state literal carried verbatim
  (the #102 ±1-row-offset method, applied suite-wide).
- 7 proofs / 179 asserts green (the audio-decision ledger pinned
  across the whole campaign: 136 fish banked / 15 eaten / 4 wins /
  1 loss / 5 starts / mute flag; mixer voicing at the win and lose
  cards); P4/P6/P7 run-twice byte-identical. `dist/shoal.gba`
  **v1.0 CONCEPT COMPLETE**, byte-deterministic.
- PR #103 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-13 (Courier ghost-as-platform, PR #105)

- **ts:** 2026-07-13T22:50:12Z (appended by the session-51 worker; everything
  above left intact per the append-only dispatch rule; ts from
  `date -u` at write time)
- Clockwork Courier growth rung 1 = **GHOST-AS-PLATFORM** (the first
  named cut in games/clockwork-courier/CONCEPT.md), on branch
  `claude/courier-ghost` from main @ `256e8cb`. Your past self is
  a one-way platform: fall onto its head and stand; riding follows
  the replay; jumping dismounts. Proof-ground: the BELL LEDGE (32 px)
  above the chute corridor — max ground jump peaks at 38.76 px
  (pinned), ghost-head boost at 30.76 px, so only the boost lands;
  the door wall now runs to the ceiling in cells no carried
  trajectory touches.
- The session-44 proof scripts had died with their container — the
  suite was re-derived against the byte-verified v0.1 dist and is now
  COMMITTED at `games/clockwork-courier/proofs.sh` (the Shoal
  oracle pattern; the re-derivation reproduced the #96 card's own
  literals: door clamp 23808, parcel 3008/6144, switch 6208). Cross-
  ROM carry: P2/P3/P4 replay bit-identically on v0.1 and this build
  (0 differing telemetry rows over 1800 frames, no clock shift) —
  zero game-state re-pins.
- 6 proofs / 95 asserts green: carried boot/kinematics/rewind/
  delivery + P5 the platform SOLVE (mount pinned via the new
  on_ghost word, ledge stand at feet 8192, delivery with REWINDS 2)
  and P6 the counter-solve (strongest scripted cheats pinned to
  FAIL); P4 and P5 run-twice byte-identical.
  `dist/clockwork-courier.gba` v0.2 byte-deterministic.
- PR #105 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-13 (Courier timed chutes, PR #106)

- **ts:** 2026-07-13T23:09:59Z (appended by the session-52 worker; everything
  above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR
  before this commit)
- Special inbox check performed FIRST: control/inbox.md at HEAD
  `04c6ba7` still carries only ORDERS 001-005 — no new unserved
  ORDER; merged PR #104 (`b4717be`) is the coordinator seat's
  outbox ack that the EAP night kickoff arrived WITHOUT an inbox
  ORDER ("please re-deliver or confirm"). No preemption.
- Clockwork Courier growth rung 2 = **TIMED CHUTES** ("multiple
  parcels/chutes with timing windows"), on branch
  `claude/courier-chutes` from main @ `04c6ba7`. THE RUSH ORDER on
  the SELECT verb: two parcels (ledge + step) and the chute keeps
  hours — shutter open 60 frames in every 240, pure run_frames
  arithmetic. jump_v/gravity untouched (the rung-1 reachability rail
  is load-bearing and was not pressured).
- Carried P1-P6: green UNCHANGED on the new build (the classic run
  lives on START by construction) and P4's script re-proven
  cross-ROM vs the rebuilt v0.2 dist — 0 differing telemetry rows
  over 700 frames, no clock shift; zero game-state re-pins.
- 7 proofs / 127 asserts green: + P7 THE RUSH ORDER — both parcels
  picked (carried word 2), 100+ pinned frames at the SHUT chute
  delivering nothing (the refusal: missing the window is a wait, not
  a shortcut), delivery of both at rf 720 (the first open frame),
  card text exact, START from the rush card restarts CLASSIC —
  run-twice byte-identical. `dist/clockwork-courier.gba` v0.3
  byte-deterministic.
- PR #106 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (Courier levels, PR #107)

- **ts:** 2026-07-13T23:35:30Z (appended by the session-53 worker; everything
  above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Clockwork Courier growth rung 3 = **MORE LEVELS** ("more levels" in
  CONCEPT.md; the Shoal levels playbook), on branch
  `claude/courier-levels` from main @ `04802ed`. THE TOWER SHIFTS
  on the L verb: three NEW tile maps with per-level literals — L1 THE
  HIGH SHELF (boost-exclusive parcel; ghostless probes pinned to fail
  at apex 9922), L2 THE NIGHT SHIFT (platform + ghost-threaded door +
  timed window; delivery lands rf 480, the window edge), L3 THE FULL
  ROUND (boost + switch ghost + window; two rewinds, delivery rf
  960). L advances/wraps on win cards. jump_v/gravity and the 240/60
  window untouched.
- Carried P1-P7: green UNCHANGED (all modes read one current-floor
  view; classic/rush pass the original literals verbatim) and the
  rush chassis re-certified cross-ROM vs the rebuilt v0.3 dist — 0
  differing rows over 850 frames, no clock shift; zero game-state
  re-pins.
- 8 proofs / 185 asserts green: + P8 THE TOWER SHIFTS — one
  deterministic chain plays all three floors (solve + refusal per
  floor) and the L-wrap, run-twice byte-identical (P4/P5/P7
  likewise). `dist/clockwork-courier.gba` v0.4 byte-deterministic.
- PR #107 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (Courier audio, PR #108)

- **ts:** 2026-07-13T23:52:27Z (appended by the session-54 worker; everything
  above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Clockwork Courier growth rung 4 = **AUDIO, the concept's LAST
  named cut** — on branch `claude/courier-audio` from main @
  `47985ab`. Six original synthesized cues (deterministic
  `audio/generate_audio.py`, no samples ever) via maxmod, fired as
  pure decisions on events the sim already computed (start deferred
  one frame / pickup / rewind / door / delivery / window tick) +
  B-mute gating playback only; every trigger rides `gl_audio_hook`,
  voicing proven by the mixer-memory watch.
- Clock verdict (rail front-loaded): **NO SHIFT** — full-row
  cross-ROM diff vs the rebuilt v0.4 dist: 0 differing rows over 700
  frames at the SAME frame indices (Courier's transitions are light;
  the Shoal spawn-spike trap did not fire). Every carried literal
  stands verbatim; zero re-pins, zero re-bases.
- 8 proofs / 231 asserts green (the audio decision ledger pinned
  across all four chains — 4 starts / 3 pickups / 4 rewinds / 8 door
  edges / 3 deliveries / 6 window ticks on the tower chain alone;
  mixer voicing at rewind+delivery, silence pinned between);
  P4/P5/P7/P8 run-twice byte-identical.
  `dist/clockwork-courier.gba` **v1.0 CONCEPT COMPLETE**,
  byte-deterministic. **The Courier concept is FULLY BUILT** —
  further gba breadth is owner-routed.
- PR #108 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (web-arcade pack, PR #109)

- **ts:** 2026-07-14T00:07:32Z (appended by the session-55 worker; everything
  above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- **⚑ Self-initiated** (rung 4, coordinator-directed): web-arcade
  packaging refreshed from main @ `982b23a` — **supersedes the
  stale PR #85 pending owner disposition** (#85 is conflicted and
  packaged the pre-#84 Drift Garden foundation; it was not touched,
  closed, or commented on).
- `tools/package-web-arcade.sh` now stages main's ACTUAL shipped
  set: Undertow v1.0, Tiltstone v1.0 (pre-juice — the #92←#93←#95←#97
  stack is still open; juice.js staging + version bump noted as the
  pending follow-up), Drift Garden **v1.0 as the playable seeded
  round game** (#84, merge `7ffcf1c`), arcade bundle v1.1.
  Determinism: fixed epoch + LC_ALL=C-sorted `zip -X`, two runs
  byte-identical; `docs/RELEASES.md` pins every zip AND staged file
  sha256, machine-asserted by the new `--verify` mode.
- With this slice: **Shoal v1.0 and Clockwork Courier v1.0 are both
  CONCEPT-COMPLETE** (PRs #98-#103, #96+#105-#108), the web arcade
  package is current, and no inbox ORDER is unserved (001-005 all
  served; the EAP night ORDER never arrived — the #104 outbox ask to
  the manager stands). Open non-lane PRs: Tiltstone stack
  #92/#93/#95/#97 + control #85/#87/#88/#89/#90.
- PR #109 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.
- **The lane STANDS DOWN pending owner input.**

## Dispatch 2026-07-14 (undertow-daily, PR #110)

- **ts:** 2026-07-14T01:42:02Z (appended by this dispatch's worker; everything
  above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Undertow growth cut 1 — the concept's named daily-seed line — on
  branch `claude/undertow-daily` from main @ `6cb85ee`: `?daily=1`
  derives the seed from the UTC date as YYYYMMDD (read once at boot,
  never in the sim step); gameover offers a copyable challenge link
  (`?seed=N&depth=M`, seed pinned explicitly, `depth` render-only).
  Static hosting only — no server, no leaderboard. game.js v1.1.0.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- Smoke `tools/web-smoke-undertow.mjs` extended (+9 assertions, incl.
  `?daily=1` boot under an injected fake Date and daily≡seeded run
  equivalence); run twice locally: 17/17 PASS both runs, identical
  crash frames (823 / 458).
- Follow-up pointer: `dist/web` Undertow copy now trails main —
  next package refresh bumps web-undertow to v1.1 and re-pins
  `docs/RELEASES.md` (session-55 guard recipe; `--verify` red until
  then is the designed reminder).
- PR #110 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (drift-garden-weather, PR #111)

- **ts:** 2026-07-14T01:55:46Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Drift Garden growth cut 1 — the concept's named daily-weather line — on
  branch `claude/drift-garden-weather` from main @ `49b8bdf`: the UTC
  date (YYYYMMDD) deterministically picks the day's named weather (five
  kinds: wind-strength multiplier + prevailing-wind bias, own mulberry32
  stream) ONCE at boot, never inside the sim step; surfaced as a small
  `today: <weather>` HUD label. Pure-parameter cut: no new persistence,
  screens, server, or URL params; `sw.js` cache v2 → v3.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- Smoke `tools/mobile-foundation-smoke/game-smoke.mjs` extended
  (+4 assertions, incl. boot under an injected fake Date — no test-only
  date param in the product, per #110's boundary-injection discipline —
  and different-date divergence on the SAME seed); run twice locally:
  15/15 PASS both runs, deterministic lines identical. Foundation smoke
  (`run.sh`) 6/6 green before and after.
- PR #111 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (cindervault-items, PR #112)

- **ts:** 2026-07-14T02:18:10Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Cindervault growth cut 1 — the concept's named item-layer line — on
  branch `claude/cindervault-items` from main @ `39fdc87`: one inventory
  slot, a lantern (halves burn for 20 turns: torch burns every second
  held turn) and a blade (bumps for 3: the 3-HP 'M' dies in one hit),
  one of each spawned per floor, picked up like embers — pickup
  activates, picking up the other kind replaces the held one, zero new
  verbs. Item RNG draws appended after each floor's existing generation
  words (new world version); telemetry 16 → 18 words + an ITEM HUD line.
- ALSO: the committed-proofs port (the Shoal/Courier gap, closed for
  Cindervault): `games/cindervault/proofs.sh` (P1 boot, P2 lantern, P3
  blade; P2/P3 each RUN TWICE, watch-logs byte-identical) + the
  prototype card's host-side mirror rebuilt as a COMMITTED tool
  (`games/cindervault/tools/mirror.py`), certified vs the emulator at 0
  mismatches across all 75 turn states before any pin was transcribed.
  Local-only + committed, matching Shoal (per-PR CI stays "ROM builds").
- `dist/cindervault.gba` v0.2 refreshed (121,700 bytes, two clean builds
  byte-identical, sha256 `b53421ba…`); toolchain first re-certified by
  rebuilding v0.1 to its committed dist hash exactly.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #112 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.
