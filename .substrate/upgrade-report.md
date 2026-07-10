# substrate-kit upgrade report — v1.6.0 → v1.7.0

> Generated 2026-07-10 by `bootstrap.py upgrade`. Rollback: `python3 bootstrap.py upgrade --rollback`.

**Docs:** consumer-edited: 4 · diverged: 1 · template-improved: 14

| planted doc | class | note |
|---|---|---|
| CONSTITUTION.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/decisions.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/architecture.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/ownership.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/runtime_contracts.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/repo-navigation-map.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/helper-policy.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/collaboration-model.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/ai-project-workflow.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/owner-profile.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/AGENT_ORIENTATION.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/current-state.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| docs/question-router.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/CAPABILITIES.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/ideas/README.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| .session-journal.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| control/README.md | diverged | both the template and the doc moved — manual merge |
| control/inbox.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| control/status.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |

## Template deltas for diverged docs

### control/README.md

```diff
--- control/README.md (template@old, current slots)
+++ control/README.md (template@new, current slots)
@@ -37,6 +37,10 @@
   Stop hook's overwrite reminder clears when any lane's heartbeat is fresh (it cannot know which
   lane a session belongs to). An empty list falls back to the default — misconfiguration never
   silently disables the gate.
+- **One command, not hand-edits** — a Project joining a SHARED repo runs
+  `bootstrap adopt --lane <name>`: it plants `control/status-<name>.md` (skip-if-exists),
+  declares it in `heartbeat_files`, and leaves `inbox.md`/`README.md` single — a second lane
+  never re-plants the first Project's files (the double-adoption fix).
 
 ## Per-session ritual (every session, and every routine wake)
 
```

