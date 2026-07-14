# substrate-kit upgrade report — v1.15.0 → v1.16.0

> Generated 2026-07-14 by `bootstrap.py upgrade`. Rollback: `python3 bootstrap.py upgrade --rollback`.

**Docs:** consumer-edited: 5 · template-improved: 7 · unchanged: 12

| planted doc | class | note |
|---|---|---|
| CONSTITUTION.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/decisions.md | unchanged | template identical across versions |
| docs/architecture.md | unchanged | template identical across versions |
| docs/ownership.md | unchanged | template identical across versions |
| docs/runtime_contracts.md | unchanged | template identical across versions |
| docs/repo-navigation-map.md | unchanged | template identical across versions |
| docs/helper-policy.md | unchanged | template identical across versions |
| docs/collaboration-model.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/ai-project-workflow.md | unchanged | template identical across versions |
| docs/owner-profile.md | unchanged | template identical across versions |
| docs/AGENT_ORIENTATION.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/current-state.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| docs/question-router.md | unchanged | template identical across versions |
| docs/CAPABILITIES.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/SKILLS.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/ROUTINES.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/reading-path.md | unchanged | template identical across versions |
| docs/ideas/README.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| .session-journal.md | unchanged | template identical across versions |
| control/README.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| control/inbox.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| control/status.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| control/claims/README.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| scripts/env-setup.sh | unchanged | template identical across versions |

## ⚠️ Gate carve-outs (host additions the kit-owned regen could not keep)

- carve-out: .github/workflows/auto-merge-enabler.yml — host-added step 'Skip arming while the PR's own in-diff session card is in-progress' in job 'enable-auto-merge' [carried from the previous upgrade report]
- carve-out: full pre-regen enabler banked at .substrate/backup/auto-merge-enabler.pre-regen-a26a45a8.yml — host additions were NOT carried into the regenerated kit-owned enabler; move them into a separate workflow file (e.g. .github/workflows/host-ci.yml) and commit that before shipping this upgrade/adopt PR. [carried from the previous upgrade report]

## Carve-out scan

- carve-out scan: .github/workflows/substrate-gate.yml — ran, 0 found
- carve-out scan: .github/workflows/auto-merge-enabler.yml — ran, 0 found
- carve-out scan: 2 carve-out line(s) reported above (see the ⚠️ section).

## Capability-ledger seed refresh

- capability-seed: docs/CAPABILITIES.md is consumer-untouched — the whole file (fence included) refreshes via `upgrade --apply-docs`; no fence-only refresh needed.

This upgrade ships the venue-scoped capability ledger (grounded-skills §4.2): entries carry a venue token (owner-live · autonomous-project · routine-fired · subagent · any) and the ledger's kit-owned seed block carries the posture decision rule. If this repo carries a local prose copy of the boot-triad/venue-posture rule (superbot Q-0270), that copy is now superseded by docs/CAPABILITIES.md's posture rule — collapse the local copy into a pointer.

## Seat-digest refresh

- seat-digest: docs/seat-digest.md already current — nothing to refresh.

## Applied (--apply-docs)

- applied: CONSTITUTION.md (template@new, hash re-recorded)
- applied: docs/collaboration-model.md (template@new, hash re-recorded)
- applied: docs/AGENT_ORIENTATION.md (template@new, hash re-recorded)
- applied: docs/CAPABILITIES.md (template@new, hash re-recorded)
- applied: docs/SKILLS.md (template@new, hash re-recorded)
- applied: docs/ROUTINES.md (template@new, hash re-recorded)
- applied: control/claims/README.md (template@new, hash re-recorded)
