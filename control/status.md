# COORDINATOR HEARTBEAT — overnight hygiene wrap (2026-07-17)

updated: 2026-07-17T03:34:07Z (`date -u`; overnight hygiene wrap,
session `.sessions/2026-07-17-overnight-hygiene.md`, branch
`claude/2026-07-17-overnight-hygiene`)

kit: v1.17.0

> This section is the coordinator's state, overwritten wholesale each
> coordinator pass. Worker-appended sections below the line are left
> intact (control/README.md writer-ownership).
> **All PR facts below were verified against LIVE GitHub at ~03:30Z
> 2026-07-17** (list_pull_requests + per-PR get_check_runs), not read
> from the prior ender heartbeat — which was stale (it recorded "no
> open PRs" at 2026-07-16T00:49Z, before the whole overnight run).

## Repo identity note (correction)

This repo is **Track B (public)** of game-lab per README + CONSTITUTION
(`gba-homebrew`); the private Nintendo-IP track is `pokemon-mod-lab`
(Track A). Any "Track A" label for this repo in a dispatch is a
mislabel — the public/private hard rail is unchanged.

## Overnight output 2026-07-16 → 17 (all DRAFT, owner-click-gated)

Every PR below is **open + DRAFT**, verified at live GitHub. Build jobs
(ROM builds / NDS ROM build / NDS Brineward|Underroot build) are GREEN
on each; `substrate-gate` is RED by design on the draft cards
(born-red doctrine holds each red until its card flips complete) —
that red is inherited/intended, not a failure. `enable-auto-merge` is
`skipped` on all (drafts don't arm). None land until the owner clicks.

- **Underroot v1.0 arc** — `#155–#165` (11 slices, stacked; each PR
  based on the previous slice's branch, root `#155` on `main`). Land
  after `#153`, then in PR order `#155 → … → #165`.
  Heads: 155 `caa7a36b` · 156 `84f8c289` · 157 `d8cfb927` · 158
  `a0c2f784` · 159 `706876ff` · 160 `02e554fc` · 161 `a3d5866c` · 162
  `ec272038` · 163 `19660ace` · 164 `9bed7a31` · 165 `82e5c08d`.
- **Tiltstone arc 2** — `#166–#170` (5 cuts, stacked; root `#166` on
  `main`). Land in order `#166 → … → #170`.
  Heads: 166 `baf2ee3a` · 167 `cde02756` · 168 `7ad73ef0` · 169
  `0d04b09a` · 170 `ec582f81`.
- **Wickroad crossroads** — `#172–#175` (4 cuts, stacked, dist v1.0;
  root `#172` on `main`). Land in order `#172 → … → #175`.
  Heads: 172 `2992d691` · 173 `9ae55ef2` · 174 `824f19f3` · 175
  `bb28fe12`.
- **Brineward bestiary** — `#176–#179` (4 cuts, stacked; root `#176`
  on `main`). Land in order `#176 → … → #179`.
  Heads: 176 `a3f3a0a9` · 177 `b1d9d120` · 178 `02f0e57d` · 179
  `3d95f89f`.
- **Planning menu** — `#171` (84 proposals,
  `docs/planning/OVERNIGHT-MENU-2026-07-16.md`; head `318814c2`, base
  `main`). Independent — land anytime.

## Pre-existing PRs

- **`#153`** — gate-orphan fix (restore read-path reachability for the
  gate). Open + DRAFT, head `80156a6e`, base `main`. **CI GREEN** —
  `substrate-gate=success`, ROM builds / NDS ROM build / NDS Brineward
  build all `success`, `enable-auto-merge=skipped`. This is the **FIRST
  click**: it clears main's gate-red. Everything else lands after it.
- **`#154`** — denial-triage ledger (two dated classifier-denial
  entries). Open + DRAFT, head `f1f12986`, base `main`,
  `substrate-gate` red-by-design (born-red), builds green. Lands after
  `#153`.

## Land orders (for the owner's clicks)

1. `#153` FIRST (green — clears main's gate-red).
2. Then each stack in PR order: `#155 → … → #165` · `#166 → … → #170`
   · `#172 → … → #175` · `#176 → … → #179`.
3. `#154` after `#153`; `#171` anytime.

Because the stacks are branch-on-branch, a child cannot merge before
its parent — landing in order is required, not just preferred.

## Standing wall (record, do NOT fight) — see docs/PLATFORM-LIMITS.md

The auto-mode classifier denies **ready-flips / auto-merge / PR-writes**
on coordinator-relayed authorization, since mid-day 2026-07-16. Effect:
finished work can only be **opened as DRAFT**; it cannot be flipped
ready or merged from an executing/coordinator session. **Unblock =
owner clicking in the GitHub UI** (Ready for review → Merge) **or the
owner typing directly in an executing session.** The owner is
deliberately deferring the clicks pending an Anthropic update — this is
a chosen hold, not a stall. This hygiene-wrap PR itself is opened DRAFT
under the same wall.

## ⚑ needs-owner (pointers; detail in the PR bodies + docs/NEXT-MENU-2026-07-15.md)

1. **Land the overnight run** — click per the land orders above
   (`#153` first). Nothing merges without this.
2. **Menu letters** — vetoes/picks on the `#171` overnight menu
   (`docs/planning/OVERNIGHT-MENU-2026-07-16.md`, 84 proposals) and the
   prior `docs/NEXT-MENU-2026-07-15.md` (in-doc rec B1).
3. **Arcade go-live** — Settings → Pages → Source "GitHub Actions",
   then Actions → "Deploy web arcade to Pages" → Run workflow → main.
   (Pages deploy itself is VERIFIED live — see baton.)
4. **Required-checks console fix** — `substrate-gate` and
   `nds-rom-build` are not in the branch-protection required set.

## next-tasks baton (fresh-seat pickup)

1. Owner vetoes on the `#171` menu; remaining NEXT-MENU options
   **A1 / A3 / C**.
2. Wickroad audio playtest.
3. Required-checks console fix (`substrate-gate` + `nds-rom-build` not
   in the required set).
4. Pages deploy is **VERIFIED live** — run `29508297853`, 2026-07-16
   14:49Z. (Go-live still needs the owner's one Settings click above.)
5. Failsafe cron `trig_0123fLkN1pzY6uNN3Y7ksYaW` (`50 */2 * * *`) is
   the dead-man bridge — it fires into a stale session that stands down
   cleanly each fire. A fresh seat should **rebind it at boot** per the
   cutover doctrine. (Not located in the first two pages of
   list_triggers this pass — those pages are entirely tonight's
   one-shot pacemaker/work-loop `send_later` triggers; the recurring
   cron sits further back. Reconfirm on rebind.)

## Notes for the fresh seat

- HEAD at wrap = `478bf16` (#152). No merges happened during this
  wrap — the wall holds every overnight PR as a draft.
- This wrap did NOT touch any pre-existing PR, did not force-push, and
  opened only its own DRAFT heartbeat PR.

---

## Dispatched slice 2026-07-16 — current-state rows (append-only)

- updated: 2026-07-16T01:13:55Z (`date -u`); session card
  `.sessions/2026-07-16-current-state-rows.md`, branch
  `claude/current-state-rows`, PR #152.
- What: docs/current-state.md reconciled against the live GitHub PR
  list at HEAD `d38887c` — the stale In-flight NEXT-MENU row resolved
  (merged as #150, `588aa4e`, 2026-07-15T21:42:05Z) and the shipped
  ledger carried through #148–#151 with merge SHAs + merged_at from
  the live API. Docs-only; no dist/, no workflows, no routines touched.
- ORDER 007 ack check: the done-when ack IS on record — "ORDER 007
  ACKNOWLEDGED (first rebooted dispatch)" landed in control/status.md
  at `df55299` (#142, 2026-07-15); no re-ack needed here.
