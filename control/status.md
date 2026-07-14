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
