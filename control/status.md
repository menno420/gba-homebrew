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
