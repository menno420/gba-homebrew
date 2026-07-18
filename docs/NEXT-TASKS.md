# gba-homebrew — NEXT-TASKS (fresh-start landing mission + owner console)

> **Status:** `owner-guidance`
>
> Written 2026-07-17 by the fresh-start cleanup pass; this is the ordered work
> list the next session picks up. Agents land these PRs themselves on green CI.
> Source code + merged PRs win over this file; re-verify PR state at HEAD before
> acting (MCP PR reads lag; confirm via `git fetch` / the Actions runs).

## Mission #1 — LAND THE FOUR FINISHED GAME ARCS

24 PRs are open, awaiting one coordinated **rebase-onto-current-`main`** pass:
finished, **ROM-builds-green** content. All branches are preserved. Agents land
these directly on green — the only reason they aren't merged yet is ordering.
Land each chain **in order** — the
children are stacked on their parents (each cut targets its sibling branch, not
`main`), so merge cut-1 first, let GitHub retarget the next child to `main`,
then merge in order:

1. **Underroot** (new game **#12**, the marquee) — `#155 → #156 → … → #165`
   (v1.0 at slice 11).
2. **Tiltstone arc 2** — `#166 → #167 → #168 → #169 → #170`.
3. **Wickroad** — `#172 → #173 → #174 → #175`.
4. **Brineward** — `#176 → #177 → #178 → #179`.

Plus two standalones:

- **#154** — denial-triage docs (two dated classifier-denial entries for the
  capabilities/walls ledger).
- **#171** — the overnight veto-menu (see owner-console item 3 below).

**Conflict resolution (each rebase):** the only conflicts are the built
`dist/*.gba` (and `dist/web/`) binaries and the per-game status / `RELEASES.md`
doc rows. Resolve **binary / `dist/` conflicts by taking the arc branch's built
ROM** (the arc is the newer, intended content); let the **"ROM builds" CI job
re-verify** the ROM from source on the rebased PR — it rebuilds every
`games/*/Makefile` and logs sha256s, so a wrong take is caught. Resolve
status-doc conflicts by keeping both sets of receipts (append, never delete).

**After each arc lands:** repin `dist/` (`tools/package-web-arcade.sh --verify`)
and update `docs/current-state.md` (game count 11 → 12 with Underroot added;
per-arc rows).

**Do NOT bulk-merge** and do NOT merge out of order — the stacked bases make
order load-bearing. All arcs are ROM-builds-green today; the only blocker is the
stacked bases (land parents first), not the (non-required, by-design-red)
`substrate-gate`.

## Owner console items (require an owner click — no agent API surface)

1. **Pages arcade go-live** (one-time). Settings → Pages → Source =
   **"GitHub Actions"**, then Actions → **"Deploy web arcade to Pages"** →
   Run workflow → `main`. Verify the run is green and
   **https://menno420.github.io/gba-homebrew/** loads every web build.
   `.github/workflows/deploy-pages.yml` publishes `dist/web/` only (no ROMs /
   zips) and has had **zero runs** to date; `GITHUB_TOKEN` cannot enable
   Pages, so this click is the standing blocker (carried since 2026-07-15).
   Risk: ✅ reversible.
2. **Branch-protection required checks.** Today the **only** required check is
   the GBA **"ROM builds"** job, so **`nds-rom-build`** (the NDS proofs) and
   **`substrate-gate`** (the doc-reachability / session gate) can be red at
   merge. Decide: add **`nds-rom-build`** and/or **`substrate-gate`** to the
   required set, or explicitly record that they stay advisory.
   **Recommendation:** add **`nds-rom-build`** to required (the NDS games ship
   real proofs and should gate); keep **`substrate-gate`** advisory while the
   born-red machinery is being retired. Risk: ⚠️ changes merge gating.
3. **Curate the #171 overnight menu, then close #171.** `#171` /
   `docs/planning/OVERNIGHT-MENU-2026-07-16.md` is an **84-proposal** veto-menu
   (44 per-game cuts across 12 games + engine / tooling / CI / web-arcade /
   release / new-game / cross-game). Pick a small next set (3–6 items:
   next-game cuts + one engine/tooling item). A suggested shortlist to veto
   down from — the best of the menu, chosen for proven strength (breadth) and
   low risk:
   - a **new small game** from the menu's NEW-concept category (breadth is the
     lab's proven strength — five prototypes in one night on 2026-07-13);
   - **Underroot post-v1.0** polish or a first growth cut, once #155–#165 land;
   - an **arc-2** on a concept-complete GBA title — **Deepcast** or
     **Cindervault** sit at v0.6 and can each take one;
   - one **engine / tooling** item — e.g. the HUD-depth CI assert or the
     toolchain-checksum pin already sitting in `docs/ideas/`.

## Retired scaffolding (context)

Some EAP-era scaffolding is no longer load-bearing and is kept only for history
— **not** durable project rules:

- the `control/` manager↔lane order-bus (`control/inbox.md` / `outbox.md` /
  `status.md` / `README.md`) — kept for history; `control/inbox.md` is
  append-only-protected by `substrate-gate`, so it is left untouched rather
  than banner-stamped;
- `docs/ROUTINES.md` wake-chain doctrine — kept as reference doctrine
  (`Status: reference`);
- the failsafe cron **`trig_0123fLkN1pzY6uNN3Y7ksYaW`** ("Game Lab failsafe
  wake", `50 */2 * * *`) — delete in the owner's Routines screen if no longer
  wanted.

Note: the `auto-merge-enabler` workflow is **kept and live** — it arms native
squash auto-merge on open so agents' green PRs land without a manual merge call.
The merge doctrine in `CONSTITUTION.md`, `docs/conventions.md`, and
`docs/PLATFORM-LIMITS.md` is: **agents open PRs ready and merge their own green
PRs directly** (via a merge call, native auto-merge, or the enabler workflow).
Workflow files, `games/` source, and `dist/` ROMs are all **kept**.
