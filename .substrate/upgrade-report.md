# substrate-kit upgrade report — v1.7.1 → v1.8.0

> Generated 2026-07-11 by `bootstrap.py upgrade`. Rollback: `python3 bootstrap.py upgrade --rollback`.

**Docs:** consumer-edited: 4 · diverged: 1 · missing: 2 · template-improved: 14

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
| control/claims/README.md | missing | absent — upgrade's adopt pass replants it |
| scripts/env-setup.sh | missing | absent — upgrade's adopt pass replants it |

## Carve-out scan

- carve-out scan: .github/workflows/substrate-gate.yml — ran, 0 found

## Template deltas for diverged docs

### control/README.md

```diff
--- control/README.md (template@old, current slots)
+++ control/README.md (template@new, current slots)
@@ -89,6 +89,25 @@
 (Shipped by inbox ORDER 007 — the root-cause fix for the twin-execution failure; the
 ritual was live-proven manually on this repo's own orders before graduating here.)
 
+## Claiming work (not an ORDER) — one file per claim under `control/claims/`
+
+Order claims cover the inbox; **work claims** cover everything else two
+parallel sessions could both pick up — a coordinator-assigned slice, a
+self-initiated build, a shared-surface change. Before starting such work,
+create **one file per claim** — `control/claims/<branch-or-scope>.md`, a
+single bullet `` - `branch-or-scope` · **scope** — detail · YYYY-MM-DD `` —
+land it on main FAST (claims are `control/**` traffic and ride the CI fast
+lane), re-read the directory at HEAD, build, then **delete the file at
+session close**. Per-file is the measured winner over any shared list (~98%
+merge-conflict rate for shared-append vs 0% per-file — superbot
+`tools/sim/claim_layout_sim.py`); first claim merged to main wins a
+collision; ~72h with no activity = abandoned, prune on sight. Full
+convention + checker contract: `control/claims/README.md`. (`check` nags —
+advisory-only — on unparseable, stale, duplicate, or legacy-located claims;
+legacy homes `docs/owner/claims/` and root `claims/` are auto-detected
+during the migration window, and a deliberate different home is pinned via
+`substrate.config.json` → `claims_dir`.)
+
 ## `status.md` format (what you write every session — your heartbeat)
 
 ```markdown
@@ -103,6 +122,8 @@
 ⚑ needs-owner: <a decision/action only the owner can give, or `none`>
 notes: <anything the manager should know>
 ```
+
+Grammar source of truth: the tokens, field lists, and regexes of this format are kit-owned constants in the kit's `src/engine/grammar.py` (EAP §6.8) — the SAME module the `check` enforcers consume, so writer and enforcer cannot drift; agreement is pinned by the kit's `tests/test_grammar.py`.
 
 The `kit:` line is the **substrate-coordinator visibility** channel (kit-lab reads it via the
 manager relay — zero write access to this repo): `v<X.Y.Z>` = the vendored kit version this
@@ -137,6 +158,8 @@
 the list is drift), and **fewer, clearer asks beat complete lists**. `check` warns — advisory,
 never exit-affecting — when a non-`none` ⚑ needs-owner list lacks these fields.
 
+Grammar source of truth: the tokens, field lists, and regexes of this format are kit-owned constants in the kit's `src/engine/grammar.py` (EAP §6.8) — the SAME module the `check` enforcers consume, so writer and enforcer cannot drift; agreement is pinned by the kit's `tests/test_grammar.py`.
+
 ## `inbox.md` order format (manager-written, append-only)
 
 ```markdown
@@ -146,6 +169,8 @@
 why: <one line>
 done-when: <acceptance test>
 ```
+
+Grammar source of truth: the tokens, field lists, and regexes of this format are kit-owned constants in the kit's `src/engine/grammar.py` (EAP §6.8) — the SAME module the `check` enforcers consume, so writer and enforcer cannot drift; agreement is pinned by the kit's `tests/test_grammar.py`.
 
 ## CI + auto-merge notes (learned live, 2026-07-09)
 
```

