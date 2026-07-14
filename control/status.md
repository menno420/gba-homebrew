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

## Dispatch 2026-07-13 (session 43, dispatched)

- **ts:** 2026-07-13T13:29:20Z (this section APPENDED by the session-43
  worker; the body above — cycle-5 heartbeat + session-37 footer — is left
  intact per the dispatch's append-only instruction).
- **Staleness note:** the body above is STALE vs HEAD; the freshest
  coordinator heartbeat rides the UNMERGED seat-ender branch
  `claude/seat-ender-20260713` (PR #90) — this section will conflict
  trivially with that wholesale rewrite at sweep time (control-file
  conflicts are the expected, documented cost of the parked-open set).
- **State at wake (13:18Z):** inbox at HEAD `d87f9ad` has no unexecuted
  order (ORDER 005 served — tally PR #87, night report PR #89; ORDER 006
  exists only on PR #88's head and is served by PR #89). 10 PRs open:
  #82–#90 parked green for the owner sweep + #91 (Brineward slice 8)
  ACTIVELY being built by session 42 (branch moving at 13:19Z) — left
  strictly untouched.
- **Shipped this session:** Tiltstone slice 2 "PAR SCORING + UNDO"
  (concept growth-path items 2+3, the doc's named retention hook) —
  pure additive engine `par`/`grade` (v1.1.0, zero edits to
  gen/rotation: all 19 PR-#81 smoke pins re-passed byte-identical),
  shell undo stack (U key/button, honest undo counter, BURIED-card
  step-back, won-card frozen), PAR on the HUD, graded win card.
  PR #92 (branch `claude/tiltstone-par`): `bcb9e29` card+claim,
  `ce0d60a` slice. Proof: engine smoke 24/24 + browser smoke 12/12
  (real Chromium), both local, verbatim in the session card;
  screenshot `docs/proof/session-43-tiltstone-par-win-card.png`.
  No GBA/NDS ROM touched (web dir has no Makefile — "ROM builds"
  unaffected by construction).
- **Landing path:** PR #92 parked READY, no merge/auto-merge/labels from
  this session — owner sweep click, alongside #82–#91.
- **Housekeeping note for the sweeper:** stale claims at HEAD from merged
  sessions — `control/claims/claude-tiltstone-slice.md` (PR #81 merged)
  and `control/claims/order-005-scribe.md` (PR #77 merged, also the one
  advisory nag in `check --strict`) — prunable on sight.
- **Baton (next 2):** (1) owner sweep + the required-checks setting
  (night-report asks [a]/[b]) still gate everything — after the sweep,
  re-verify main and prune the stale claims above; (2) next
  headless-proven increments if the queue stays empty: Brineward slice 9
  "saves" AFTER session 42's #91 closes out (do not collide), or
  Tiltstone growth item 1 "juice"/Undertow daily-seed share URL on the
  web arcade.

## Dispatch 2026-07-13 (Tiltstone slice 3, dispatched)

- **ts:** 2026-07-13T18:29:21Z (APPENDED by the session-44 worker on
  coordinator dispatch; everything above left intact per the append-only
  rule)
- **Shipped this session:** Tiltstone slice 3 "JUICE" — concept-doc growth
  item 1: engine trace surface (pure additive `settleMoves`/`resolveTrace`,
  v1.2.0, results byte-identical to `settle`/`resolve`), shell replay
  (board sweep, gems easing down columns, per-chain pops; cosmetic —
  engine state stays immediate; cancelled by input; reduced-motion skip)
  and `juice.js` synth audio (oscillator sweeps only, chain-rising collect
  pitch, mute persisted, honest cue log). PR #93 (branch
  `claude/tiltstone-juice`, stacks on OPEN PR #92 @ `a072be8`), commits
  `85da0f5` (claim + born-red card), `bc5133d` (slice).
- **Proofs:** engine smoke 24 → 31 asserts (exit 0; all 24 prior pins
  byte-identical), browser smoke 12 → 18 (exit 0, real Chromium; cue log
  == pure-Node composition of the exact inputs, mid-animation frame
  proof), screenshots `docs/proof/session-44-tiltstone-juice-win{,-mid}.png`.
  `check --strict` red pre-flip ONLY on this card's designed born-red hold
  (+ the pre-existing `order-005-scribe.md` advisory) — verified verbatim
  in substrate-gate run 29273800286: "HOLD (by design) … nothing to
  investigate".
- **Inbox at session start (HEAD `48be770`):** ORDERS 001–005 only, all
  served — standing default applied; no collision (only #92, this lane's
  own parent, touches Tiltstone).
- **Follow-up for the packaging lane:** when PR #85 lands, its
  `tools/package-web-arcade.sh` staging list needs `juice.js` added to the
  Tiltstone runtime set (HOSTING.md here already updated).

## Dispatch 2026-07-13 (Tiltstone slice 4, dispatched)

- **ts:** 2026-07-13T18:52:50Z (this section APPENDED by the session-45
  worker on coordinator dispatch; everything above left intact)
- **Shipped this session:** Tiltstone slice 4 "NEW CELL TYPES" — concept
  growth item 4: locked gems (collect adjacent to free, cascades chain
  through the freed gem), ice (slips off piles, left before right), one-way
  grates (porous exactly downward, arrow turns with the cavern) — pure
  settle()/resolve() extensions (engine v1.2.0 → 1.3.0, juice v1.0.0 →
  1.1.0 with an `unlock` cue), entering generation at level 5+ so levels
  1–4 draw the identical RNG stream. PR #95 (branch
  `claude/tiltstone-cells`, base `claude/tiltstone-juice` @ `427f165` —
  stacked on OPEN PR #93 per the standing default). Engine smoke 31 → 50
  asserts (all 31 carried pins byte-identical), browser smoke 18 → 24
  checks in real Chromium (LV5 board matches the pure-Node generator
  byte-for-byte; cue log stays a pure composition), screenshots
  `docs/proof/session-45-tiltstone-cells{,-mid}.png`,
  `python3 bootstrap.py check --strict` exit 0 at the flip.
- **Landing posture:** opened READY, born-red then flipped; no merge /
  auto-merge / label calls from this session — the live enabler may land
  it server-side on green.
- **Baton (next):** Tiltstone growth item 5 "level packs" (curate seeds
  the solver rates hard — long solutions, low slack — into named packs);
  item 6 (daily leaderboard) stays out of static-hosting scope. Packaging
  follow-up for PR #85 still open: Tiltstone staging list needs `juice.js`.

## Dispatch 2026-07-13 (Tiltstone slice 5, dispatched)

- **ts:** 2026-07-13T19:14:01Z (appended by the session-46 worker;
  everything above left intact per the append-only dispatch rule)
- **Shipped this session:** Tiltstone slice 5 "LEVEL PACKS" — the
  concept doc's growth item 5 ("curate seeds the solver rates hard —
  long solutions, low slack — into named packs"). Engine v1.3.0 →
  v1.4.0, pure + additive: `difficulty()` (par-dominant, low-slack
  tiebreak), deterministic `curatePack()`, pinned `PACKS` data
  (GRANITE GAUNTLET: 6 base-rule caverns par 5–7; DEEP CUTS: 6
  level-4 caverns par 7–8) with the smoke re-running the curation and
  asserting the pin byte-identical. Shell: pack picker, staged
  progression, `?pack=&stage=` deep links, guarded progress. PR #97
  (branch `claude/tiltstone-packs`), base = `claude/tiltstone-cells`
  @ `6238f3a` (stacks on #95 ← #93 ← #92). Proofs: engine smoke
  50 → 58 asserts (all 50 prior pins byte-identical), browser smoke
  24 → 31 checks in real Chromium, `bootstrap.py check --strict`
  exit 0; screenshots `docs/proof/session-46-tiltstone-packs*.png`.
  Tiltstone growth items 1–5 are now ALL shipped — item 6 (daily
  leaderboard) is out of static-hosting scope per the concept doc, so
  further Tiltstone arc work is owner-gated.
- **BACKPRESSURE now binds for this lane:** none of the Tiltstone
  stack merged during this session — observed open own-lane PRs at
  19:12Z: **#92, #93, #95, #97 = 4 unmerged** (≥ the 3-PR
  backpressure line even before #97). Repo-wide open PRs observed: 10
  (#85, #87, #88, #89, #90, #92, #93, #95, #96, #97). Next Tiltstone
  slice should NOT be dispatched until the stack starts landing (and
  the concept queue is empty anyway — see above).
- **Landing posture:** PR #97 parked READY, no merge/auto-merge calls
  from this session; the live server-side enabler decides on green.
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

## Dispatch 2026-07-14 (deepcast-audio, PR #113)

- **ts:** 2026-07-14T02:37:03Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Deepcast growth cut 1 — the concept's named audio line ("reel clicks
  that speed up with tension") — on branch `claude/deepcast-audio` from
  main @ `2f2bf68`: while reeling, a dry ratchet click fires on a period
  that shrinks linearly with tension (16 frames at slack -> 4 at the snap
  point) with pitch rising the same way; yielding is silent. Bite/catch/
  snap event cues; B mutes (gates play() only). Four original synthesized
  sounds (`games/deepcast/audio/generate_audio.py`, stdlib-only,
  byte-exact regeneration), maxmod soundbank, pure decision layer counted
  into `gl_audio_hook` (clicks/bites/catches/snaps + per-frame click
  interval + mute) — the Shoal (PR #103) / Courier (PR #108) playbook.
- ALSO: the committed-proofs port (the Shoal/Courier/Cindervault gap,
  closed for Deepcast): `games/deepcast/proofs.sh` (P1 silent title, P2
  the prototype card's seeded run + the audio ledger, P3 the mute
  discriminator; P2/P3 each RUN TWICE, watch-logs byte-identical).
  Enabling the mixer did not shift Deepcast's boot clock — every
  prototype game-state literal carried verbatim. Local-only + committed,
  matching Shoal (per-PR CI stays "ROM builds").
- `dist/deepcast.gba` v0.2 refreshed (132,412 bytes, two clean builds
  byte-identical, sha256 `f5e33083…`); toolchain first re-certified by
  rebuilding v0.1 to its committed dist hash exactly.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #113 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (undertow-cosmetics, PR #114)

- **ts:** 2026-07-14T02:47:47Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Undertow growth cut 2 — the concept's named cosmetics line ("diver
  skins, bubble trails — pure render-side, zero sim risk") — on branch
  `claude/undertow-cosmetics` from main @ `46b5fd4`: four skins
  (classic/abyss/ember/ghost — diver body/visor colors + a bubble-trail
  draw style each), selected via `?skin=ID` or a `C` key-cycle on
  title/gameover, persisted to guarded `localStorage` (`undertow.skin`).
  game.js v1.1.0 -> v1.2.0; rendering only — the sim step and both RNG
  streams are untouched.
- `tools/web-smoke-undertow.mjs` extended 17 -> 22 assertions, headline:
  sim identity across skins — a `?skin=ember` run crashes on the
  identical frame at the identical depth as the default skin for the same
  seed (crashFrame=823 / 194m, unchanged from the pre-change v1.1.0
  baseline). Run twice locally against real Chromium: 22/22 PASS both
  runs, outputs byte-identical.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #114 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (drift-garden-species, PR #115)

- **ts:** 2026-07-14T03:04:30Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Drift Garden growth cut 2 — the concept's "more species" growth line —
  on branch `claude/drift-garden-species` from main @ `aac62b0`: a
  data-driven SPECIES table (two per tier — common line fern/lotus/aurora
  carrying the pre-cut hues/values 5/15/40 exactly, rare line
  clover/iris/solaris with distinct hues, a bright-core accent, values
  8/22/55). Tier-1 plants roll species on a side-band mulberry32 stream
  keyed off the round seed — the gameplay stream's draw count/order are
  unchanged (the PR #111 contract; all 15 pre-existing assertions green
  with zero edits). Cross-pollination is species-aware and pure in the
  parents: same species -> common next-tier line, mixed -> rare (hybrid
  vigor), zero extra draws. sw.js cache v3 -> v4.
- `tools/mobile-foundation-smoke/game-smoke.mjs` extended 15 -> 21
  assertions (pure child derivation, both tier-1 species at a fixed seed
  under an injected fake Date, per-species harvest values, pure pair ->
  lotus, pure ladder to tier-3 aurora via the deterministic drag verb,
  hybrid pair -> iris at +22). Run twice locally against real Chromium:
  21/21 PASS both runs, deterministic lines byte-identical; foundation
  run.sh 6/6.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #115 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (cindervault-species, PR #116)

- **ts:** 2026-07-14T03:42:04Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Cindervault growth cut 2 — the concept's "named monster species per
  depth with distinct chase quirks (the greedy step is a plug-in policy)"
  line — on branch `claude/cindervault-species` from main @ `a5fec7a`:
  one named species per depth (CINDER RAT / SOOT WISP / ASH HOUND /
  VAULT WRAITH / HOARD SENTINEL), each one quirk on the now plugged-in
  greedy chase step; species consume no RNG (pure function of the
  floor), so every fixed-seed spawn pin carried; depth's name on its own
  HUD line ("FOE ...").
- Mirror-first: tools/mirror.py grew the policies, a --baseline
  counterfactual and the species route designer; re-certified vs the
  emulator on all three pinned routes — 0 mismatches / 185 turn states.
  proofs.sh +P4 (one witness per quirk, ends in the open vault: WIN turn
  109, hp 3, score 1032 — winnability re-checked); two P3 hp pins
  re-derived (fewer bites under the quirks), P1/P2 carried verbatim; P2,
  P3, P4 each run twice, watch-logs byte-identical. dist v0.3 (two clean
  builds byte-identical, proofs at dist bytes).
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #116 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (deepcast-daily, PR #117)

- **ts:** 2026-07-14T04:02:52Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Deepcast growth cut 2 — the concept's "daily seed + score-attack"
  line, GBA-shaped — on branch `claude/deepcast-daily` from main @
  `3ca91fc`: the title's seed line is now a dial (UP/DOWN +-1,
  LEFT/RIGHT +-0x100, L/R +-0x10000, 8 hex digits shown), same dialed
  seed = same lake, same fish; the dusk score card names the seed so a
  score is attributable; boot seed 0xDEE9CA57 untouched (no dial = the
  v0.2 run, every proof pin carried). Leaderboard half noted out of GBA
  scope in CONCEPT.md per the Tiltstone PR #97 precedent.
- proofs.sh +P4 (dialed lake 0xDEEACB58: per-step dial pins in dc[4],
  glyph-exact seed digits on title and dusk card, same input ->
  different bite frame + a wt-16 fish where the default stream gave 11;
  RUN TWICE, watch-logs byte-identical) +P5 (dial away and back -> the
  full P2 route lands on every P2 literal). 212 distinct asserts, 500
  green lines, exit 0. dist v0.3 (two clean builds byte-identical,
  proofs re-run at dist bytes; toolchain certified against the v0.2
  hash first).
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #117 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (undertow-ghost, PR #118)

- **ts:** 2026-07-14T04:17:25Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Undertow growth cut 3 — the concept's "ghost replays of your best run"
  line — on branch `claude/undertow-ghost` from main @ `932cfd1`: every
  run records its steer timeline (RLE), the best run per seed persists to
  guarded localStorage (`undertow.ghost.<seed>`), and later runs of the
  same seed get a translucent ghost diver replaying it in lockstep — a
  second sim instance (own RNG, own rows) fed the stored inputs,
  render-only, zero contact with the live run. `?ghost=0` opts out.
- game.js v1.2.0 -> v1.3.0: sim core factored into makeSim/rowIn/physStep
  (both divers execute the SAME step, so replay fidelity is by
  construction); pre-change fixed point for seed 7 (crashFrame=823, 194m)
  carried verbatim. Smoke 22 -> 31 assertions: replay fidelity (stored
  timeline reproduces crashFrame+depth exactly), live-run identity ghost
  on vs cleared storage, lockstep ghost lands on its recorded numbers
  mid-run, best-per-seed keeper, reload persistence, per-seed keying,
  opt-out. Run twice, 31/31 PASS, outputs byte-identical.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #118 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (drift-garden-essence, PR #119)

- **ts:** 2026-07-14T04:34:15Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Drift Garden growth cut 3 — the concept's "essence spending" line — on
  branch `claude/drift-garden-essence` from main @ `afd2918`: each
  round's harvested essence banks into a persistent wallet (guarded
  localStorage, `driftgarden.meta`), and a dusk-screen palette shop
  spends it on garden palettes — pure render (background gradient pair +
  draw-time hue shift), sim step / RNG streams / snapshot() untouched.
  Of the doc's spend targets (biomes/palettes/species), palettes shipped
  as the contained cut; biomes remain the named follow-up.
- sw.js CACHE v4 -> v5. Smoke 21 -> 27 assertions: round-end banking,
  deny/buy/re-buy spend flow, real-tap shop rows on the dusk screen
  (select without replanting), off-row replant with wallet intact +
  cross-round accrual, reload persistence through the real guarded boot
  path, and byte-identical seed-7 snapshots with all palettes unlocked
  vs fresh storage. Run twice, 27/27 PASS both, deterministic lines
  byte-identical; pre-change baseline 21/21; foundation run.sh 6/6.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #119 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (cindervault-endless, PR #120)

- **ts:** 2026-07-14T05:08:48Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Cindervault growth cut 3 — the concept's "Depth banding past floor 5
  for an endless mode; the vault becomes the first milestone instead of
  the end" line — on branch `claude/cindervault-endless` from main @
  `b778d39`: SELECT on the VAULT REACHED screen continues the SAME run
  onto floor 6 from the RNG state the vault left behind (the +torch win
  bonus drops back out of the live score); past floor 5 the generator
  reads a depth-band table (monsters/bigs/embers/carve, one band per 3
  floors, {4,1,2,220} up to {8,4,1,140}) and the five named species
  recur on the five-depth cycle. No second win — endless is
  score-attack. A run that never presses SELECT is bit-identical to
  v0.3: P1-P4 carried verbatim, zero re-derived pins.
- Method held: mirror extended FIRST (bands, 'S' continue, choke/sweep
  helpers, a deterministic beam searcher; verify() upgraded to full
  state-sequence matching) and re-certified — 0 mismatches across all
  414 turn states of the four routes. New P5 THE DESCENT: 228 inputs
  (P4's route + SELECT + 118 deep), band witnesses per parameter,
  ends alive on floor 10 at hp 1 (beam-optimal wraith toll exactly 2
  hp). Suite exit 0; P2-P5 each run twice, watch-logs byte-identical.
- dist/cindervault.gba v0.4: two clean builds byte-identical, 123,172
  bytes, sha256 7bb878a…bdd4a0; proofs ran at those exact bytes;
  toolchain certified against v0.3's committed hash before any change.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #120 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (deepcast-species, PR #121)

- **ts:** 2026-07-14T05:32:47Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Deepcast growth cut 3 — the concept's "Fish species tables per depth
  band with named rarities; a catch log" line — on branch
  `claude/deepcast-species` from main @ `6c6b089`: four depth bands
  (named on the sink card), 16 named species, one rarity tier each
  (COMMON/UNCOMMON/RARE/MYTHIC), drawn at cast time from a SIDE-BAND
  xorshift32 stream seeded `seed ^ 0x51DEF157` — the main stream's word
  order untouched, so every v0.2/v0.3 pin carried VERBATIM (the whole
  v0.3 suite ran green against the new build before any proof changed;
  zero re-derived pins). Catch card names the landed fish; SELECT opens
  a per-run catch log (ring of the last 8 landed fish; snaps never
  log); new `dc_fishlog` telemetry mailbox. Log is session-scope by
  design — SRAM persistence noted as follow-up at the BUILT mark.
- Proofs: new P6 (species/rarity/log glyph-exact, mailbox witnesses,
  snapped-fish-never-logs discriminator; run twice, watch-logs
  byte-identical) + species witnesses threaded additively through
  P1-P5 (incl. P4: the DIALED lake's shallow catch is MUD BREAM COMMON
  where the default lake's was DUSK PERCH UNCOMMON — the seed-select
  contract now extends to the fish's name). 751 green assert lines,
  exit 0; side-stream literals cross-checked against an offline
  xorshift32 replica.
- dist/deepcast.gba v0.4: two clean builds byte-identical, 131,992
  bytes, sha256 f87b797…9185f5; proofs ran at those exact bytes;
  toolchain certified against v0.3's committed hash (clean worktree
  rebuild) before any change.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #121 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (cindervault-seed, PR #122)

- **ts:** 2026-07-14T05:54:30Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Cindervault growth cut 4 — the concept's "daily seed + score-attack
  leaderboard" line, GBA-shaped per the Deepcast PR #117 precedent — on
  branch `claude/cindervault-seed` from main @ `d60b08f`: the title's
  seed line is now a dial (UP/DOWN +-1, LEFT/RIGHT +-0x100, L/R
  +-0x10000, 8 hex digits shown; none clash with the title's only other
  input, START), same dialed seed = same vault, same monsters; the death
  card (the score-attack card — endless runs end only there) names the
  seed so a score is attributable; boot seed 0xC1DE5EED untouched (no
  dial = the v0.4 run, every proof pin carried). Leaderboard half noted
  out of GBA scope in CONCEPT.md per the Tiltstone PR #97 precedent.
- proofs.sh +P6 (dialed vault 0xC1DF5FEE: per-step dial pins in cv[3],
  glyph-exact seed digits on title and death card, floor 1 differs from
  the default's in the words P2 pins at the same frame; RUN TWICE,
  watch-logs byte-identical) +P7 (dial away and back -> the full P2
  route lands on every P2 literal). Suite exit 0, 446 watch + 78 text
  asserts green; mirror re-certified seed-aware with cv[3] in the word
  set (0 mismatches across 453 turn states of five routes). dist v0.5
  (two clean builds byte-identical, proofs re-run at dist bytes;
  toolchain certified against the v0.4 hash first).
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #122 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (undertow-oxygen, PR #123)

- **ts:** 2026-07-14T06:11:30Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Undertow growth cut 4 — the concept's "Oxygen meter + air-pocket
  pickups" line — on branch `claude/undertow-oxygen` from main @
  `2137e48`: the tank drains every frame (faster with depth), air
  pockets spawn inside the channel from a side-band RNG stream
  (seed ^ POCKET_STREAM, fixed two draws per row — the wall stream is
  untouched, channel layout per seed unchanged), contact refills the
  tank, an empty tank ends the run ("OUT OF AIR"). game.js v1.3.0 ->
  v1.4.0. First SIM-changing cut for this lane: the seed-7 no-input
  baseline moved crashFrame 823 / 194m (wall) -> 810 / 191m (air).
  Ghost records re-versioned v1 -> v2; stale pre-oxygen records dropped
  cleanly on load.
- Smoke 31 -> 41 assertions: carried v1.3.0 fixed point (hold-left
  seed-7 wall crash at frame 37 / 7m proves layout identity); tank
  full/drains/deterministic; zero-pocket seeds 4 and 8 die of air on
  the identical frame 634; a pickup extends the run; a greedy
  pocket-chaser witnesses an in-run refill and reaches 724m vs 148m
  no-input at seed 3, deterministic across two driven runs; stale v1
  ghost dropped + fresh v2 written. Run twice: 41/41 PASS, outputs
  byte-identical.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #123 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (drift-garden-biomes, PR #124)

- **ts:** 2026-07-14T06:29:50Z (appended by this dispatch's worker;
  everything above left intact per the append-only dispatch rule; ts from
  `date -u` at write time; PR number confirmed against the live PR)
- Drift Garden growth cut 4 — the concept's "wind patterns per biome"
  line, the LAST named growth cut — on branch `claude/drift-garden-biomes`
  from main @ `65c3659`: four biomes (meadow free / gale ridge 60 /
  whorl hollow 130 / tide flats 220) bought with banked essence in a
  second dusk-screen shop section (PR #119's wallet/rows plumbing). A
  biome is a wind-PATTERN parameter set — gust strength windMul, gust
  cadence gustEvery, a deterministic swirl rotating the wind vector, a
  sine sway breathing its amplitude, prevailing-bias coupling biasMul —
  plus a render-only hue tint. Read ONCE at round start into roundBiome
  (PR #111's weather discipline: the sim step never reads meta or the
  clock); snapshot() carries the round's biome; sw.js cache v5 -> v6.
  Default 'meadow' carries identity parameters, pinned byte-exact against
  the pre-cut build's snapshots (seed 11 / fake date 20300607, steps
  500 + 1500). Completes Drift Garden's named growth path.
- Smoke 27 -> 35 assertions: pinned pre-cut fixed point; per-biome
  determinism (same biome + seed + date twice -> byte-identical two-stage
  snapshots); 4 pairwise-distinct worlds on the same seed + date; spend
  deny/buy/re-buy; real-tap biome rows leave the frozen round untouched;
  round-start read on replant; reload persistence; every-biome-owned +
  meadow-active sim-inert vs fresh storage. Run twice: 35/35 PASS both,
  deterministic lines byte-identical. Foundation smoke 6/6. All 27
  pre-existing assertions unchanged.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #124 opened READY on main; no merge/auto-merge calls from this
  session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (deepcast-upgrades, PR #125)

- Deepcast growth cut 4 (`claude/deepcast-upgrades`, cut from main @
  `e6b7693`): the concept's "line upgrades bought with score (thicker
  line = higher snap threshold, slower reel)" line, marked BUILT.
- Dusk scores deposit into a persistent BANK (gl_save.h magic-checked
  SRAM slot, tag `DCLINE1`); LINE SHOP (SELECT on title, R at dusk)
  buys WORN/BRAIDED/STEEL — snap 600/750/900 up, reel 5/4/3 down.
  Fresh/erased cart loads as no save = v0.4 bit-exact: the whole
  committed P1-P6 suite ran green against the new build unmodified
  before any new pin was written. New `dc_linemeta` 8-word mailbox.
- proofs.sh +P7: worn baseline on fresh cart; boot 1 banks 18, buys
  BRAIDED, refused on STEEL, run twice from two factory-fresh
  savefiles (watch-logs AND savefiles byte-identical); boot 2 on the
  same savefile (mGBA --savefile bus-copy path, the Lumen Drift
  two-boot pattern) restores bank 3/tier 1 and pins the tradeoff as
  arithmetic on the same hold-only fight (line 432-4k vs 432-5k,
  fighting at 623 past worn's 600 snap frame, snap at exactly 750).
  407 assert flags, 887 green assert lines, exit 0.
- dist/deepcast.gba v0.5: two clean builds byte-identical, 134,392
  bytes, sha256 `c8ed6f11…`; suite re-run at dist bytes. Toolchain
  certified first (clean scratch-worktree rebuild of main hit the
  committed v0.4 `f87b7975…` exactly).
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #125 opened READY on main; no merge/approve/auto-merge calls from
  this session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (undertow-hazards, PR #126)

- Undertow growth cut 5 (`claude/undertow-hazards`, cut from main @
  `98b9b60`): the concept's "Hazards/creatures in the channel (jellyfish
  that drift on the seeded RNG)" line, marked BUILT — this completes
  Undertow's named growth path (all five cuts built); the CONCEPT.md
  status line weathered to say so.
- game.js v1.4.0 → v1.5.0: jellyfish spawn from row 40 on via a third
  side-band RNG stream (`seed ^ JELLY_STREAM`, fixed four draws per
  generated row — wall and pocket streams draw exactly what they drew
  in v1.4.0, so a seed's channel AND pocket layout are byte-identical);
  drift is a bounded sine of the sim instance's own step counter;
  contact ends the run ("STUNG AT N m" — one-touch death, the game's
  grammar). SIM change: seed-3 no-input 650/148m (air) → 358/76m
  (sting); seed-7 no-input/hold-left and zero-pocket seeds 4/8 carry
  their v1.4.0 numbers verbatim (layout-identity witnesses). Ghost
  records v2 → v3, stale records dropped cleanly on load.
- Smoke 41 → 48 assertions (spawn/drift determinism at frame 300 twice,
  bounded-sine drift, deterministic jelly-hunter dies STUNG with air
  left, seed-3 sim-change witness, carried fixed points, stale-v2 drop
  + fresh-v3 write). Run twice against real Chromium: 48/48 PASS both,
  outputs byte-identical. Pre-change baseline on main: 41/41 PASS.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #126 opened READY on main; no merge/approve/auto-merge calls from
  this session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (cindervault-art, PR #127)

- Cindervault growth cut 5 (`claude/cindervault-art`, cut from main @
  `0912a1c`): the concept's "Real art pass: tile sprites for
  walls/floor, a torch-radius light fade" line, marked BUILT — this
  completes Cindervault's named growth path (all five named lines
  built; CONCEPT.md notes it).
- Presentation ONLY: the glyph map becomes a dynamic tile background
  (original procedural assets, games/cindervault/graphics/), the
  player a torch-bearer sprite, and the shared gl_light_window.h
  drives a light circle of radius min(16 + torch/2, 200) px around
  the player — the screen literally darkens as the torch burns and
  re-opens on embers. Game state, RNG word order and all 18
  cv_telemetry words untouched: P1-P7 pass verbatim (zero re-derived
  pins, zero adjusted text asserts).
- New P8/P9 prove the art off the hardware: a second presentation
  mailbox cv_light (radius/center) + DISPCNT, BG/OBJ palette RAM and
  VRAM screenblock pins; the radius witness runs 126 -> 48 px over
  carried torch pins (deep burn) and 126 -> 155 px on the ember
  relight. All new legs run twice, watch-logs byte-identical. Suite
  622 watch + 104 text asserts, exit 0, re-run green at the dist
  bytes. dist v0.6 138,692 bytes, two clean builds byte-identical,
  sha256 db7d5918…; toolchain certified first (v0.5 rebuilt to its
  committed hash from a clean tree). Before/after screenshots
  committed under docs/proof/.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #127 opened READY on main; no merge/approve/auto-merge calls from
  this session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (deepcast-art, PR #128)

- Deepcast growth cut 5 (`claude/deepcast-art`, cut from main @
  `9517806`): the concept's "Real art pass: lake gradient by depth,
  silhouette fish, rod-bend sprite as the analog tension gauge" line,
  marked BUILT — this completes Deepcast's named growth path (all five
  named lines built; CONCEPT.md notes it).
- Presentation ONLY: a ten-band lake background (original procedural
  assets, games/deepcast/graphics/) whose palette follows a closed-form
  law of the live depth word (dim = 2*band + depth/8 — the whole lake
  deepens as the lure sinks), a 32x16 silhouette fish whose frame IS the
  species index from cut 3 (band = size, rarity = shade; hidden while
  sinking and on a SNAP card), and a 32x32 rod sprite bending
  tension*7/snap — the audio ratchet's own law, honest on any line
  tier. Game state, RNG word order and all three sim mailboxes
  untouched: P1-P7 pass verbatim (zero re-derived pins, zero adjusted
  text asserts; text HUD kept alongside).
- New P8/P9/P10 prove the art off the hardware: a fourth presentation
  mailbox dc_artmeta + DISPCNT, BG/OBJ palette RAM and VRAM screenblock
  pins; the gradient witness walks 0x6A05 -> 0x44E2 -> 0x1421 in
  palette RAM on carried depth pins, the rod witness bends 2/6 (worn)
  vs 1/5 (braided) at the same tension words on P7's two-SRAM-state
  pattern. All new legs run twice, watch-logs byte-identical. Suite
  1330 watch + 189 text asserts, exit 0, at the dist bytes. dist v0.6
  159,928 bytes, two clean builds byte-identical, sha256 ab9c026b…;
  toolchain certified first (v0.5 rebuilt to its committed hash from a
  clean scratch worktree). Before/after screenshots committed under
  docs/proof/.
- Provenance: LIVE OWNER directive ("see if there is anything else you
  can come up with or improve…"), owner live in the coordinator chat
  ~00:58Z 2026-07-14.
- PR #128 opened READY on main; no merge/approve/auto-merge calls from
  this session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (wickroad, PR #129)

- **Game #11 — Wickroad** (`claude/wickroad`, cut from main @
  `564fa45`): the generative rung. The ORDER-001 concept queue being
  exhausted, a NEW one-page concept (market-route trading loop: five
  towns, four goods, a 30-day clock, and a ledger whose ink ages) was
  written and promoted to a playable prototype slice in one session,
  per the Courier #96 / Shoal #98 breadth grammar. Dedup argument
  stated in `games/wickroad/CONCEPT.md` (nearest neighbors named:
  Cindervault shares turn-based-on-GBA but has no economy; Drift
  Garden has a wallet but no prices/travel/decisions).
- Deterministic by construction: turn-based, integer-only, one
  xorshift32 at fixed seed consumed only at world init; prices are a
  closed form of (day, market impact). The hook is a watch assert:
  `wr_telemetry` words 14/15 are a stale-ink witness pair, and the
  committed proof pins the day the ledger provably lies (ink 27 at
  age 3 vs world 33).
- `games/wickroad/proofs.sh` committed from day one: boot/title pins,
  the committed 13-day winning route (gold 60 -> 328, every leg
  pinned, win card verbatim, instant restart), the impact ladder +
  decay, the pass closing at dawn 31. Both routes run twice,
  watch-logs byte-identical; 166 watch + 26 text assertion passes;
  suite green at the dist bytes. `dist/wickroad.gba` v0.1 121,708 B,
  two clean builds byte-identical, sha256 `40bef942…30db`.
- Provenance: LIVE OWNER directive ("see if there is anything else
  you can come up with or improve…"), owner live in the coordinator
  chat ~00:58Z 2026-07-14.
- PR #129 opened READY on main; no merge/approve/auto-merge calls
  from this session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (arcade-refresh, PR #130)

- **⚑ Self-initiated closing upkeep** (`claude/arcade-refresh`, cut
  from main @ `87680c9`, PR #129 merged): three contained items, one
  PR. (1) Web-arcade repin: `dist/web/` + `dist/releases/` re-run
  through the #109 machinery at HEAD — Undertow zip v1.0 -> v1.5
  (#110/#114/#118/#123/#126), Drift Garden v1.0 -> v1.4
  (#111/#115/#119/#124), arcade bundle v1.1 -> v1.2; Tiltstone
  unchanged and its rebuilt zip reproduced the #109 pin byte-exactly.
  `--verify` green twice; run-twice zips byte-identical.
  (2) `docs/current-state.md` rewritten to HEAD (the old revision's
  log ended at session 8): 11-game roster, all four named growth
  paths complete (#110–#128), committed-proofs convention, dist
  versions per dist/README.md. (3) Stale-claims sweep: six served
  claims deleted, each verified merged at live GitHub (#83, #84,
  #81, #104, #77, #117); the standing `claims-format` advisory on
  `order-005-scribe.md` cleared with it — strict check now runs
  with zero claims advisories. No open PR's claim touched
  (Tiltstone stack #92/#93/#95/#97 and #85 are other lanes').
- Provenance: LIVE OWNER directive ("see if there is anything else
  you can come up with or improve…"), owner live in the coordinator
  chat ~00:58Z 2026-07-14. This is the session's closing upkeep
  slice — the session stands down after this lands, per the owner's
  audit close-out.
- PR #130 opened READY on main; no merge/approve/auto-merge calls
  from this session — the server-side enabler decides on green.

## Dispatch 2026-07-14 (eap-audit)

- ts: 2026-07-14T09:21Z (`date -u` at write time); append-only per the
  session-37 convention — nothing above this section was edited.
- **EAP project audit, public edition** (`claude/eap-audit`, cut from
  main @ `d0290d6`, PR #130 merged): authored
  `docs/audits/eap-project-audit-2026-07-14.md` — a point-in-time,
  fully cited audit of this repo + seat/platform-level findings
  (identity/scale measurements, tooling verdicts, walled capabilities
  with verbatim denials, landing + scheduling friction, ceremony
  cost/benefit, self-fixes, ranked remaining pains with paste-ready
  platform asks, wishlist, honest not-measured gaps). Scope note: this
  copy contains this repo's findings plus seat/platform-level material
  only (the scope statement at the top of the doc has the details).
- Provenance: owner-directed EAP audit (2026-07-14), relayed via the
  game-lab coordinator.
- Strict check run once at authoring: exit 1 = the designed born-red
  hold on this session's in-progress card (guard fire recorded in
  `.substrate/guard-fires.jsonl`); no other findings.
- PR opened READY on main; no merge/approve/auto-merge calls from this
  session — the server-side enabler decides on green.
