# Session — PLATFORM-LIMITS: correct the 2026-07-16 landing-path wall

> **Status:** `in-progress`

- date: 2026-07-18 (branch `claude/platform-limits-correction`, PR pending;
  born-red card written at 2026-07-18T00:59:18Z).
- mission: **Docs correction — the "SUPERSEDED 2026-07-16" landing-path entry
  in `docs/PLATFORM-LIMITS.md` is now partially stale.** That entry claims the
  WHOLE agent-landing path is walled ("arming auto-merge, flipping
  draft→ready, and PR writes ... all now trip the guard"; landing path is
  "the owner reviews and merges server-side"). The overnight run of
  2026-07-17→18 disproved the strongest reading: PRs **#183–#187** all merged
  autonomously with zero owner clicks, because a plain READY (non-draft)
  `claude/*` PR gets native squash auto-merge armed by
  `auto-merge-enabler.yml` (github-actions[bot]) on open/ready and merges on
  green required checks. This session appends a dated **CORRECTION 2026-07-18**
  beneath that entry — history is preserved, not rewritten — recording what is
  proven landable and what is STILL walled (the agent itself calling merge,
  arming auto-merge, or using draft→ready as a landing lever).
- scope: `docs/PLATFORM-LIMITS.md` only (append a dated correction). Does NOT
  edit `docs/capabilities.md` (its "Arm auto-merge while checks are pending"
  section cross-references the wall, noted but left for a follow-up). Touches
  no code, no workflow, no `control/*` status/inbox, no arc docs.
- landing posture: PR opened **READY** (not draft) from the start — which is
  itself the practice this correction documents. The server-side
  `auto-merge-enabler` arms native auto-merge on green rom-builds under owner
  merge authority; no self-merge, no manual auto-merge arm from this session.
  substrate-gate is RED BY DESIGN while this card is `in-progress` (the
  born-red hold); flipping this card to `complete` (the deliberate LAST step)
  releases the gate.

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/claude-platform-limits-correction.md`, PR opened READY.
2. **The correction** (`docs/PLATFORM-LIMITS.md`): a dated
   `**CORRECTION 2026-07-18 ...**` continuation appended beneath the
   "SUPERSEDED 2026-07-16" self-merge/landing-path bullet — the 2026-07-16
   text is left verbatim, the correction sits below it citing PRs #183–#187.
