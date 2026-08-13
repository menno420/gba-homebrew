# substrate-kit upgrade report — v1.20.1 → v1.21.0

> Generated 2026-08-13 by `bootstrap.py upgrade`. Rollback: `python3 bootstrap.py upgrade --rollback`.

**Docs:** consumer-edited: 6 · diverged: 2 · template-improved: 2 · unchanged: 14

| planted doc | class | note |
|---|---|---|
| CONSTITUTION.md | diverged | both the template and the doc moved — manual merge |
| docs/decisions.md | unchanged | template identical across versions |
| docs/architecture.md | unchanged | template identical across versions |
| docs/ownership.md | unchanged | template identical across versions |
| docs/runtime_contracts.md | unchanged | template identical across versions |
| docs/repo-navigation-map.md | unchanged | template identical across versions |
| docs/helper-policy.md | unchanged | template identical across versions |
| docs/collaboration-model.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/ai-project-workflow.md | unchanged | template identical across versions |
| docs/owner-profile.md | unchanged | template identical across versions |
| docs/AGENT_ORIENTATION.md | unchanged | template identical across versions |
| docs/current-state.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| docs/question-router.md | unchanged | template identical across versions |
| docs/CAPABILITIES.md | diverged | both the template and the doc moved — manual merge |
| docs/SKILLS.md | template-improved | consumer-untouched + template improved — safe to apply with `upgrade --apply-docs` |
| docs/ROUTINES.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| docs/reading-path.md | unchanged | template identical across versions |
| docs/ideas/README.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| .session-journal.md | unchanged | template identical across versions |
| control/README.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| control/inbox.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| control/status.md | consumer-edited | template unchanged — consumer-owned, nothing to apply |
| control/claims/README.md | unchanged | template identical across versions |
| scripts/env-setup.sh | unchanged | template identical across versions |

## ⚠️ Gate carve-outs (host additions the kit-owned regen could not keep)

- carve-out: .github/workflows/auto-merge-enabler.yml — host-added step 'Skip arming while the PR's own in-diff session card is in-progress' in job 'enable-auto-merge'
- carve-out: full pre-regen enabler banked at .substrate/backup/auto-merge-enabler.pre-regen-a26a45a8.yml — host additions were NOT carried into the regenerated kit-owned enabler; move them into a separate workflow file (e.g. .github/workflows/host-ci.yml) and commit that before shipping this upgrade/adopt PR.

## Carve-out scan

- carve-out scan: .github/workflows/substrate-gate.yml — ran, 0 found
- carve-out scan: 2 carve-out line(s) reported above (see the ⚠️ section).

## Capability-ledger seed refresh

- capability-seed: NOT refreshed — the fenced seed block in docs/CAPABILITIES.md differs from the kit-form fence (edited inside the fence, or the old templates are unavailable). The fence is kit-owned: move your own findings BELOW the fence into the append log, restore the block between the BEGIN/END markers to kit form (copy it from the new template render), and the next upgrade refreshes it automatically.

This upgrade ships the venue-scoped capability ledger (grounded-skills §4.2): entries carry a venue token (owner-live · autonomous-project · routine-fired · subagent · any) and the ledger's kit-owned seed block carries the posture decision rule. If this repo carries a local prose copy of the boot-triad/venue-posture rule (superbot Q-0270), that copy is now superseded by docs/CAPABILITIES.md's posture rule — collapse the local copy into a pointer.

## Seat-digest refresh

- seat-digest: regenerated docs/seat-digest.md (derived render — skills index + venue-filtered walls re-rendered from the current tree; venue filter preserved from the committed doc).

## Template deltas for diverged docs

### CONSTITUTION.md

```diff
--- CONSTITUTION.md (template@old, current slots)
+++ CONSTITUTION.md (template@new, current slots)
@@ -69,6 +69,29 @@
   (https://github.com/menno420/substrate-kit/blob/main/docs/recipes/pinned-feed-contract.md).
 - When a doc and a source file disagree: Docs follow reality: on doc/code drift, fix the doc in the same PR that reveals it; binding conventions change only via a deliberate PR that says so; control-file drift is governed by control/README.md writer-ownership
 
+## Boot read path
+
+Read in this order at session start. **This is the one list** — the task router
+at `docs/AGENT_ORIENTATION.md` points here rather than repeating it, so a boot
+set can never exist in two places that disagree.
+
+1. This file — the working agreement + autonomy rails.
+2. `docs/current-state.md` — the living status ledger. Source and merged PRs
+   always win over it.
+3. `docs/CAPABILITIES.md` — verified session capabilities and walls. THE
+   DISCOVERY RULE lives there: append what you verify, never a limitation.
+
+Then `docs/AGENT_ORIENTATION.md` when a task needs a route into the deeper
+docs — it is a router, not boot reading.
+
+<!-- Keep every path above resolvable: check_boot_path asserts this section
+     exists and that each path it names is on disk. A boot pointer into a
+     missing file is the exact defect measured across 11 adopter trees on
+     2026-08-06 — 0 of 11 had a boot path that resolved, because the 07-12
+     fix repointed the router at this agreement before the agreement had a
+     list to point at. Add repo-specific boot docs here; never move the list
+     back into the router. -->
+
 ## Autonomy rails — act vs. ask
 
 The full twelve-item autonomy rider is PL-012 (cite it, don't copy it);
@@ -88,9 +111,9 @@
   **land your own work** — flip to ready, arm auto-merge, or merge it
   yourself (MCP/REST, or let a merge-on-green workflow land it) the moment
   CI is green. Landing a green PR, your own or a sibling's, is a **normal
-  agent action, not an owner action** — there is **no standing
-  "classifier-denied" merge wall; do not invent one, and never route a
-  mergeable green PR to the owner.** If the branch falls behind, update it
+  agent action, not an owner action** — there is
+  **no standing "classifier-denied" merge wall; do not invent one, and never
+  route a mergeable green PR to the owner.** If the branch falls behind, update it
   (merge, never force). Only if a *specific* merge/arm call returns a
   real, verbatim permission refusal *this session* do you park that one
   call (attempt-once rule), queue ONE owner item for the systemic cause,
@@ -145,7 +168,7 @@
 substrate-kit repo at `docs/program/rulings.md` — the [PL-NNN] register
 (https://github.com/menno420/substrate-kit/blob/main/docs/program/rulings.md),
 e.g. PL-001 decide-and-flag · PL-006 source-wins / false-green ·
-PL-012 the autonomy rider.
+PL-012 the autonomy rider · PL-013 inhabiting beats observing.
 **Cite PL-IDs — never copy ruling bodies into this repo** (the register is
 the one home; a local copy is drift by construction). Repo-local rulings
 stay in `docs/decisions.md` / `docs/question-router.md`.
```

### docs/CAPABILITIES.md

```diff
--- docs/CAPABILITIES.md (template@old, current slots)
+++ docs/CAPABILITIES.md (template@new, current slots)
@@ -38,6 +38,20 @@
 Before declaring anything impossible, and before assuming a tool or
 credential is missing:
 
+0. **If the owner stated it, it is already verified — act on it.** *"The token
+   is account-scoped." · "You have access to that credential." · "Use this
+   provider."* He configured the environment and knows what he enabled. Do not
+   probe to check whether he is right, and do not answer his instruction with
+   questions about what a credential can or cannot do — **do the thing.**
+   Working *is* the verification, which is what step 3 already asks for; failing
+   gives you a real error instead of a hypothetical doubt. **This is not an
+   exception to verify-first.** That doctrine guards against stale *records* and
+   your own *inferences*, and the owner is neither — he is the source a record
+   would be describing, so probing his statement first is checking a source
+   against its own output. The boundary, and it is the whole boundary: he is
+   authoritative on **provisioning**; the **response to a specific call** is
+   still read every time, and a real error is still reported verbatim. He is not
+   claiming your next request returns 200.
 1. **Check this file** — the capability or wall may already be recorded for
    your venue.
 2. **Check the environment** — `printenv` / list the available tools BEFORE
@@ -65,28 +79,32 @@
 - `any` · **Provisioned credentials**: the environment often carries
   tokens/keys as env vars — `printenv` first; a missing-looking credential is
   usually a missing *look*. — LAST-VERIFIED: 2026-07-10
-- `any` · **Release cutting despite the tag wall**: `workflow_dispatch` on
-  the release workflow (with a version input) creates the tag in-Actions —
-  proven repeatedly fleet-wide after direct tag pushes 403'd.
-  — LAST-VERIFIED: 2026-07-12
+- `any` · **Release cutting via `workflow_dispatch`**: the release workflow
+  (with a version input) creates the tag in-Actions — the durable path that
+  works from every venue, including ones whose proxied git route refuses
+  tag pushes. — LAST-VERIFIED: 2026-07-12
+- `any` · **GitHub REST + git write operations work over the
+  direct-credential path**: tag push, release create, branch deletion (git
+  push `:branch` and REST) and direct `api.github.com` calls all succeed
+  with the provisioned credential over direct egress (bypassing the
+  environment's git/HTTP proxy). The old wall rows for these — "tag push /
+  release create 403", "branch deletion 403 on every path",
+  "`api.github.com` blocked, MCP-tools-only" — recorded the PROXIED route's
+  403s as if they were platform walls; a route quirk is not a wall, and the
+  retraction is measured, not inferred (fleet-manager append log,
+  2026-08-11 audit: all three refuted with live calls). If a specific call
+  403s, switch routes and record the venue — do not re-seed the wall.
+  — LAST-VERIFIED: 2026-08-11
 
 ## Walls — verified blocked (use the workaround; don't rediscover)
 
-- `any` · **Tag push / release create via git**: HTTP 403 from the
-  environment's git proxy → use the workflow_dispatch release path.
-  — LAST-VERIFIED: 2026-07-12
-- `any` · **Branch deletion**: 403 on every path (git push `:branch` and
-  API) → owner deletes by hand / enables "Automatically delete head
-  branches". — LAST-VERIFIED: 2026-07-10
-- `any` · **`api.github.com` direct HTTP**: blocked → GitHub access is
-  MCP-tools-only. — LAST-VERIFIED: 2026-07-10
 - `any` · **Environment / Project creation**: owner-click actions in the
   console — queue them as structured owner asks, never wait silently.
   Routine/schedule creation is NO LONGER a blanket wall: `create_trigger`
   arms routines agent-side (proven 2026-07-11); the console-only knobs
-  (model class, plan/seat settings) remain owner-only. **Branch creation
-  and commit-pushes work agent-side** — only ref *deletion* is walled (see
-  Branch deletion above). — LAST-VERIFIED: 2026-07-18
+  (model class, plan/seat settings) remain owner-only. **Branch creation,
+  commit-pushes and ref deletion all work agent-side** (deletion via the
+  direct-credential path above). — LAST-VERIFIED: 2026-08-11
 - **Merging works agent-side — NOT a wall.** Agents flip drafts to ready,
   arm auto-merge, and merge their own or a sibling's PR (MCP/REST) once CI
   is green — verified 2026-07-18 by a direct MCP merge. There is **no
```

