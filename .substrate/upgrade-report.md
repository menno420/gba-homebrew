# substrate-kit upgrade report — v1.14.0 → v1.15.0

> Generated 2026-07-12 by `bootstrap.py upgrade`. Rollback: `python3 bootstrap.py upgrade --rollback`.

**Docs:** consumer-edited: 3 · diverged: 2 · template-improved: 3 · unchanged: 15

| planted doc | class | note |
|---|---|---|
| CONSTITUTION.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/decisions.md | unchanged | template identical across versions |
| docs/architecture.md | unchanged | template identical across versions |
| docs/ownership.md | unchanged | template identical across versions |
| docs/runtime_contracts.md | unchanged | template identical across versions |
| docs/repo-navigation-map.md | unchanged | template identical across versions |
| docs/helper-policy.md | unchanged | template identical across versions |
| docs/collaboration-model.md | unchanged | template identical across versions |
| docs/ai-project-workflow.md | unchanged | template identical across versions |
| docs/owner-profile.md | unchanged | template identical across versions |
| docs/AGENT_ORIENTATION.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/current-state.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| docs/question-router.md | unchanged | template identical across versions |
| docs/CAPABILITIES.md | unchanged | template identical across versions |
| docs/SKILLS.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/ROUTINES.md | unchanged | template identical across versions |
| docs/ideas/README.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| .session-journal.md | unchanged | template identical across versions |
| control/README.md | diverged | both the template and the doc moved — manual merge |
| control/inbox.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| control/status.md | diverged | both the template and the doc moved — manual merge |
| control/claims/README.md | unchanged | template identical across versions |
| scripts/env-setup.sh | unchanged | template identical across versions |

## Carve-out scan

- carve-out scan: .github/workflows/substrate-gate.yml — ran, 0 found

## Seat-digest refresh

- seat-digest: docs/seat-digest.md already current — nothing to refresh.

## Applied (--apply-docs)

- applied: CONSTITUTION.md (template@new, hash re-recorded)
- applied: docs/AGENT_ORIENTATION.md (template@new, hash re-recorded)
- applied: docs/SKILLS.md (template@new, hash re-recorded)

## Template deltas for diverged docs

### control/README.md

```diff
--- control/README.md (template@old, current slots)
+++ control/README.md (template@new, current slots)
@@ -131,6 +131,27 @@
 the latest `check --strict` verdict on this tree; `engaged:` = the post-adopt engagement gate
 (`yes` once no UNRENDERED banner/slot remains, live CI runs the gate, and the session loop
 has engaged).
+
+**Exact grammar or invisible — keep the `kit:` token PLAIN.** The parser accepts a bold label
+*before* a plain token (`- **kit heartbeat:** kit: v1.2.3 · check: green · engaged: yes` is a
+live valid shape), but bolding the token itself does NOT parse — the fleet registry then reads
+the row as "no `kit:` line" and the lane's engaged signal silently vanishes (a live adopter
+incident, not a hypothetical). The taught negative example:
+
+```markdown
+- **kit:** v1.2.3 · check: green · engaged: yes
+```
+
+← does NOT parse (`KIT_LINE_RE`, kit `src/engine/grammar.py` — the optional bold group cannot
+contain the `kit:` token). If your heartbeat wants a bold label, put it *before* a plain
+`kit:` token.
+
+**Version truth defers to the generated registry, never to this line.** Heartbeat `kit:`
+lines are self-reports and chronically lag 1–3 releases behind the tree (the fleet's
+recurring self-report DRIFT class); the kit repo's generated `docs/adopters.md` —
+regenerated from each adopter's committed tree — is the fleet's version truth, and your own
+committed tree (the vendored dist) is yours. Never hand-assert a fleet version spread from
+heartbeat lines; keep this line in sync as a courtesy signal, not as proof.
 
 ## ⚑ needs-owner — the OWNER-ACTION item format (quality contract)
 
```

### control/status.md

```diff
--- control/status.md (template@old, current slots)
+++ control/status.md (template@new, current slots)
@@ -13,3 +13,8 @@
 The `kit:` line is your kit self-report (substrate-coordinator visibility): keep the version in
 sync with your vendored kit on every upgrade, `check:` = your last `check --strict` verdict,
 `engaged:` = the post-adopt engagement gate (yes once `check` reports ENGAGED/green live CI).
+Keep the `kit:` token PLAIN — the bold-label form `- **kit:** v1.2.3 · check: green · engaged: yes`
+does NOT parse and the fleet registry reads it as no `kit:` line at all (grammar + the valid
+bold-label-before-plain-token shape: `control/README.md` § "status.md format"). And this line is
+a self-report, not version truth — self-reports chronically lag; the kit repo's generated
+`docs/adopters.md` and your committed tree are the version truth to defer to.
```

