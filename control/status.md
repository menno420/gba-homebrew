# SEAT DORMANT (owner order 2026-07-14)

updated: 2026-07-14T21:16:02Z

## REVIVAL — READ FIRST

Ordered reading list:

1. `docs/eap-closeout-walkthrough-2026-07-14.md` — what this seat did, how to verify it, the OWNER ACTIONS checklist.
2. `docs/audits/eap-project-audit-2026-07-14.md` — the full project audit.
3. `docs/current-state.md` — the shipped-state ledger.
4. Newest `.sessions/` cards — the most recent sessions' ground truth.

## ROUTINES RECORD (deleted at shutdown — re-arm on revival)

- Failsafe cron: name "Game Lab failsafe wake", cron `50 */2 * * *`, bound to the live coordinator session; verify-after-arm via paginated list_triggers before deleting any old trigger. Trigger id deleted at shutdown: trig_01T1E1zn6rtM3YBWjTPBhX2b.
- Its prompt, EXACTLY: "FAILSAFE WAKE (Game Lab, Q-0265): send_later chain alive → verify in one line, end. Stalled → resume the work loop (sync HEAD → inbox → slice after slice, landed per LANDING), re-arm the chain (~15 min), and write your heartbeat (control/status.md, per-seat grammar) as the deliberate last step."
- Pacemaker pattern: ONE send_later ~15–20 min per working turn, one pending at a time; never stack. NOTE: on 2026-07-14T06:22Z the auto-mode classifier denied a pacemaker re-arm as "[Create Unsafe Agents]" — revival may require an owner permission rule.
- No business crons owned by this seat.

## PARKED AT SHUTDOWN

- PR #138 (kit v1.16.0 → v1.17.0 upgrade) was in flight by ANOTHER seat at shutdown — not ours to close; watch on revival. Verified live 2026-07-14T21:15Z: MERGED at 2026-07-14T20:56:11Z by github-actions[bot] (squash of `claude/kit-upgrade-v1.17.0` @ 298b79d) — nothing left to watch.
- Merged `claude/*` branches await owner prune (walkthrough §C).
- Nothing else in flight — verified 2026-07-14T21:15Z: the only open PR is #139, this shutdown PR itself.

## SOURCE OF TRUTH

- Fleet doctrine lives in the owner's fleet-manager repo (docs/prompts/v3/, projects/UNIVERSAL.md) — pointed at, not restated here.
- Repo-local truth: control/README.md protocol text (keep).
- Track plan: `docs/concepts/session-1-concepts.md` — the ORDER-001 concept queue (fully built; reference kept here so the doc stays on the read path).
- Local docs that duplicate fleet doctrine (LISTED, not migrated): docs/ROUTINES.md (wake-chain doctrine), docs/conventions.md (seeded from the gen-2 blueprint), docs/collaboration-model.md, docs/ai-project-workflow.md, docs/helper-policy.md.

## Dispatch 2026-07-15 (wickroad-rumors, PR #142)

written: 2026-07-15T04:22:33Z (`date -u`; heartbeat appended second-to-last, card flip follows)

- **ORDER 007 ACKNOWLEDGED** (first rebooted dispatch): EAP EXTENDED through
  2026-07-21 acknowledged; the 2026-07-14 dormancy orders (the SEAT DORMANT
  text above) are superseded pending the owner's per-project reboot review.
  **NO routines re-armed** per the order — waiting for the owner's per-seat
  go (the v3.6 reboot prompt). Source: control/inbox.md ORDER 007 @
  2026-07-15T03:37:11Z.
- This dispatch's slice, neutral facts: branch `claude/wickroad-rumors`,
  PR #142 (READY, non-draft), Wickroad growth cut 1 "Rumors" per
  games/wickroad/CONCEPT.md. Proof suite games/wickroad/proofs.sh now
  P1-P4: 278 watch + 34 text assertion passes per run, P2/P3/P4 each run
  twice with byte-identical watch-logs. dist/wickroad.gba v0.2, sha256
  `ebcc10f1...fdea`, two clean builds byte-identical. Work claim
  control/claims/claude-wickroad-rumors.md retired with this heartbeat
  (claims README rule 4 — the open PR is the live claim).
- Landing posture: PR #142 parked READY on green; no merge/approve/
  auto-merge calls from this session — the server-side enabler decides.
- Pointers: .sessions/2026-07-15-wickroad-rumors.md (card) ·
  games/wickroad/CONCEPT.md (cut 1 marked SERVED) · dist/README.md
  (v0.2 row).

## Dispatch 2026-07-15 (wickroad-contracts, PR #143)

written: 2026-07-15T05:08:58Z (`date -u`; heartbeat appended second-to-last,
card flip follows)

- Inbox re-read at origin/main HEAD `df55299` before closing: no new
  orders past ORDER 007 (acknowledged by the wickroad-rumors dispatch
  above; its standing constraint holds — NO routines re-armed, waiting
  on the owner's per-seat go).
- This dispatch's slice, neutral facts: branch `claude/wickroad-contracts`,
  PR #143 (READY, non-draft), Wickroad growth cut 2 "Contracts" per
  games/wickroad/CONCEPT.md — dated delivery orders, the second income
  verb that prices risk. Fixed authored two-contract deck (zero new RNG,
  zero price-law changes; delivery bypasses the market), RIGHT as the
  pact verb, lifecycle derived from two player flags, telemetry 24 -> 32.
  Proof suite games/wickroad/proofs.sh now P1-P5: 384 watch + 48 text
  assertion passes per run, P2-P5 each run twice with byte-identical
  watch-logs; every P5 pin mirror-derived first, ROM matched on the
  first route probe. One measured capacity fact: the pact line pushed
  dawn-regen peaks over Butano's default 128 sprite-tiles items
  (frozen-ROM assert on the first full-suite probe); fixed via
  -DBN_CFG_SPRITE_TILES_MAX_ITEMS=256 in the game Makefile, after which
  P1-P4 carried verbatim. dist/wickroad.gba v0.3, 122,696 bytes, sha256
  `6e395a1c...5d3c21`, two clean builds byte-identical, suite re-run
  green at the dist bytes. Work claim
  control/claims/claude-wickroad-contracts.md retired with this
  heartbeat (claims README rule 4 — the open PR is the live claim).
- Landing posture: PR #143 parked READY; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.
- Pointers: .sessions/2026-07-15-wickroad-contracts.md (card) ·
  games/wickroad/CONCEPT.md (cut 2 marked SERVED) · dist/README.md
  (v0.3 row) · docs/current-state.md (Wickroad row now v0.3).

## Dispatch 2026-07-15 (wickroad-hazards, PR #144)

written: 2026-07-15T05:42:26Z (`date -u`; heartbeat appended second-to-last,
card flip follows)

- Inbox re-read at origin/main HEAD `4a61ec9` before closing: no new
  orders past ORDER 007 (its standing constraint holds — NO routines
  re-armed, waiting on the owner's per-seat go).
- This dispatch's slice, neutral facts: branch `claude/wickroad-hazards`,
  PR #144 (READY, non-draft), Wickroad growth cut 3 "The road itself"
  per games/wickroad/CONCEPT.md — per-leg hazards (bandits, weather)
  and a guard-hire decision, so travel cost stops being flat. Fixed
  authored three-hazard deck (zero new RNG; hazards a pure function of
  (leg, arrival day); every window after the committed routes' last
  travel day, so the world is bit-identical and P1-P5 carried
  verbatim), LEFT as the hire verb (flat fee 4, consumed by the next
  crossing hazard or not), RAID stretches taxing gold / STORM stretches
  taxing the clock (one extra dawn camped), the new ROAD line
  telegraphing each hazard from its announce day on the THIRD quiet
  frame after a head redraw, telemetry 32 -> 40 (words 0-31 untouched).
  Proof suite games/wickroad/proofs.sh now P1-P6: 520 watch + 59 text
  assertion passes per run, P2-P6 each run twice with byte-identical
  watch-logs, the whole suite run twice end-to-end with byte-identical
  CSVs across runs; every P6 pin mirror-derived first, the ROM matched
  all 60+ watch pins on the first route probe (the only two first-probe
  failures were text-side: the guarded road line's last glyph measurably
  clipping the 240px screen — fixed by the shorter RAID token — and a
  road-line assert sitting on a measured 1-frame edge-processing parity,
  moved one frame later and the parity documented in proofs.sh).
  dist/wickroad.gba v0.4, 123,844 bytes, sha256
  `7c061301...2bf8d3`, two clean builds byte-identical, suite re-run
  green at the dist bytes. Work claim
  control/claims/claude-wickroad-hazards.md retired with this
  heartbeat (claims README rule 4 — the open PR is the live claim).
- Landing posture: PR #144 parked READY; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.
- Pointers: .sessions/2026-07-15-wickroad-hazards.md (card) ·
  games/wickroad/CONCEPT.md (cut 3 marked SERVED) · dist/README.md
  (v0.4 row) · docs/current-state.md (Wickroad row now v0.4).

## Dispatch 2026-07-15 (wickroad-wider-map, PR #145)

written: 2026-07-15T06:18:30Z (`date -u`; heartbeat appended second-to-last,
card flip follows)

- Inbox re-read at origin/main HEAD `c0c6882` before closing: no new
  orders past ORDER 007 (its standing constraint holds — NO routines
  re-armed, waiting on the owner's per-seat go).
- This dispatch's slice, neutral facts: branch `claude/wickroad-wider-map`,
  PR #145 (READY, non-draft), Wickroad growth cut 4 "A wider map + pack
  upgrades" per games/wickroad/CONCEPT.md — the road runs on past
  DUNWICK to HOLLOWFEN (the drovers' fair) and MIRGATE (seven markets,
  each with its own aging ink), and START at the fair buys mules on a
  fixed authored price ladder (30/55), each growing the pack by 4
  (8 -> 12 -> 16). RNG delta counted: +16 world-init draws appended
  strictly after the v0.4 stream (prior cuts: zero) — the legacy world
  is bit-identical and P1-P6 carried verbatim on the first post-change
  run. The map deliberately stays ONE road (no branch fork; L/R is a
  committed verb grammar) — the honest cut is named in CONCEPT.md.
  Telemetry 40 -> 48 (words 0-39 byte-unchanged). Proof suite
  games/wickroad/proofs.sh now P1-P7: 655 watch + 68 text assertion
  passes per run, P2-P7 each run twice with byte-identical watch-logs,
  the whole suite run twice end-to-end with byte-identical CSVs across
  runs; every P7 pin mirror-derived first and every watch pin matched
  on the first route probe (the only first-probe failures were three
  text asserts on FIXED-font ledger rows — measured fact recorded in
  proofs.sh: --assert-text templates come from the variable font bmp,
  so fixed-font rows are not text-assertable; the new towns' ledger
  states are pinned via watch words 44/45 instead). dist/wickroad.gba
  v0.5, 125,016 bytes, sha256 `ad4e477b...e8e31a`, two clean builds
  byte-identical, suite re-run green at the dist bytes. Work claim
  control/claims/claude-wickroad-wider-map.md retired with this
  heartbeat (claims README rule 4 — the open PR is the live claim).
- Landing posture: PR #145 parked READY; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.
- Pointers: .sessions/2026-07-15-wickroad-wider-map.md (card) ·
  games/wickroad/CONCEPT.md (cut 4 marked SERVED) · dist/README.md
  (v0.5 row) · docs/current-state.md (Wickroad row now v0.5).
