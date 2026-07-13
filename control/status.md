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

- **ts:** 2026-07-13T21:04:30Z (appended by the session-47 worker;
  everything above left intact per the append-only dispatch rule)
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
