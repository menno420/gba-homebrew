# Session — seat dormant: final shutdown documentation (owner order 2026-07-14)

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/seat-dormant`, PR #139)
- mission: documentation-only final seat shutdown per the owner order of
  2026-07-14 — overwrite `control/status.md` as the dormant-seat revival
  entrypoint (revival reading list, routines record, parked-work ledger,
  source-of-truth pointers). No feature work, no code changes.
- **📊 Model:** Claude Fable family self-report — no version/ID

## What shipped

- `control/status.md` overwritten wholesale as the SEAT DORMANT revival
  entrypoint: ordered revival reading list (walkthrough → audit →
  current-state → newest cards), the routines record (failsafe cron +
  exact prompt, pacemaker pattern, the 2026-07-14T06:22Z classifier-denial
  note), the parked-at-shutdown ledger (PR #138 verified MERGED
  2026-07-14T20:56:11Z by github-actions[bot]; the only open PR at
  verification was #139, this shutdown PR itself), and source-of-truth
  pointers with the local doctrine-duplicating docs listed, not migrated.
- Reachability verified, no fixes needed: README.md links
  `docs/eap-closeout-walkthrough-2026-07-14.md` directly, and the
  walkthrough links `docs/audits/eap-project-audit-2026-07-14.md`
  (one hop).
- Landing posture: PR #139 opened READY, born-red card held the gate red
  until this flip; no merge/auto-merge calls from this session — the
  server-side enabler decides on green.

## 💡 Session idea

The routines record in control/status.md now carries the failsafe
prompt verbatim precisely because nothing else durable did — the trigger
itself was the only copy until shutdown. A dormancy-ready seat would
keep every armed routine's name/cron/prompt mirrored in a committed file
from the day it is armed (a `control/routines.md` written at arm time),
so shutdown becomes "delete triggers, point at the file" instead of
transcribing prompts under time pressure.

## ⟲ Previous-session review

The kit-upgrade v1.17.0 card (PR #138) pinned the enabler-restore risk
with a sha256 before==after equality (`a26a45a8…b455`) rather than a
prose claim — that made this shutdown session's "PR #138 merged, nothing
to watch" verification a one-line lookup against an unambiguous record.
