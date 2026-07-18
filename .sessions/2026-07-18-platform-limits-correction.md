# Session — PLATFORM-LIMITS: correct the 2026-07-16 landing-path wall

> **Status:** `complete`

- date: 2026-07-18 (branch `claude/platform-limits-correction`, PR **#188**;
  born-red card written at 2026-07-18T00:59:18Z, flipped to complete at
  2026-07-18T01:00:49Z, both from `date -u`).
- **📊 Model:** Claude Opus family · high · docs-correction — append a dated
  CORRECTION to the stale 2026-07-16 landing-path wall in
  `docs/PLATFORM-LIMITS.md`.
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
3. **PR #188 opened READY** (not draft) — the practice the correction
   documents, applied to the correction's own PR.

## 💡 Session idea

**A "walled" ledger entry has a machine-checkable expiry the moment it names
the mechanism that supposedly blocks it — cross-reference that mechanism
against merged-PR history and stale entries surface themselves.** This entry
went stale precisely because it made a falsifiable claim ("READY/auto-merge
levers all trip the guard, so nothing lands autonomously") that the merge
record could refute — and did, five times overnight (#183–#187). The reusable
move: a small hygiene check that, for each wall entry mentioning
`auto-merge` / `draft` / `merge`, greps the last N merged PRs for
`merged_by == github-actions[bot]` with no owner review event; any hit against
an entry that says "autonomous landing is walled" is flagged as a
correction-candidate. Walls that name a concrete, observable mechanism are the
cheapest to keep honest, because the mechanism leaves a paper trail — the
danger is the absolute-sounding entry that no longer matches the trail but is
still cited as current (as #154/#180 cited this one). Auto-detection turns
"someone eventually notices" into a scheduled diff.

## Previous-session review

Prior card: `.sessions/2026-07-18-wickroad-new-record.md` (PR **#185**, NEW
RECORD on the Wickroad end card) — **its landing posture is exactly the
evidence this correction rests on.** That card opened its PR READY and
explicitly documented the born-red hold + server-side `auto-merge-enabler`
arming native auto-merge on green, "no self-merge, no manual auto-merge arm" —
and #185 is one of the #183–#187 set that merged autonomously overnight,
retiring the very wall its own posture note quietly worked around. Honest
review: the card's landing-posture paragraph was already living the corrected
rule before the ledger caught up, which is exactly why the 2026-07-16 wall
read as stale rather than merely wrong — the sessions had moved on; the doc
hadn't.
