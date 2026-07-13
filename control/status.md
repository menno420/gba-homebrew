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
