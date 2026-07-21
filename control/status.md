# SEAT CLOSED — Tue Jul 21 18:02:09 UTC 2026

updated: 2026-07-21T18:02:09Z (`date -u`; final content stamp before the closeout flip). This is the **terminal heartbeat** for the gba-homebrew Track-A lane: the autonomous development period ends 2026-07-22T00:00Z, after which the working environment is gone and only committed state survives. This file is a wholesale final stamp — earlier working sections are folded into the pointers below.

kit: v1.20.1

## Closeout pointer

- **The durable handoff is [`docs/PROJECT-CLOSEOUT.md`](../docs/PROJECT-CLOSEOUT.md)** — public-facing, written for a reader with zero session context (the owner, or a fresh future seat). It carries: what shipped (with PR/commit cites), the true state verified live at HEAD `3377319`, continuation threads in priority order with exact resume steps, a plain-language owner walkthrough with a clickable link per artifact, and a fresh-session boot guide. Read that first; this file is only the terminal status stamp.
- Living state ledger: [`docs/current-state.md`](../docs/current-state.md) (closeout linked from its read-path Doc index).

## Terminal state (verified live at HEAD, 2026-07-21)

- **main HEAD `3377319`** (#213) = `git ls-remote origin refs/heads/main` (`337731906098d1ba596dcaa1750299e45c737ed6`); no commits since #213.
- **Open PRs: 0** (`list_pull_requests state=open` → `[]`), excluding the closeout PR #214 in flight below.
- **9 committed `dist/` ROMs** downloadable (6 GBA + 3 NDS); **web arcade LIVE** (HTTP 200); **Lumen Drift Release `lumen-drift-v1.3` published** (non-draft). Full sha/probe table in the closeout §b.
- **Menu remaining real choices: A1 Tinderhand + A3 Starloom only** (per the #208 audit); both carry executable pre-plans (`docs/plans/*-preplan-2026-07-20.md`).

## PR ledger — terminal states

The landed set through the closeout, all merged on green via `auto-merge-enabler.yml` unless noted (workflow-file PRs are owner/hub-merged by carve-out):

- **#200** control workflow-name fix — **MERGED** (`21ea4dd`)
- **#201** Wickroad end-card tier-up flash — **MERGED** (`6d1d97a`)
- **#202** control record #201 boot-proof + outbox — **MERGED** (`a4114da`)
- **#203** control park Brineward cut2/cut3 (stack conflict) — **MERGED** (`32180f5`)
- **#204** control close-subsumed sweep (#177/#178 claims) — **MERGED** (`97ac85d`)
- **#205** EAP close-out / continuity doc — **MERGED** (`5d8b9a0`)
- **#206** refresh `dist/wickroad.gba` post-#201 — **MERGED** (`01a72c5`)
- **#207** controller-lane truth refresh + B1 pre-plan judgment — **MERGED** (`41a9a9f`)
- **#208** menu-vs-tree truth audit — **MERGED** (`9dbde10`)
- **#209** commit Underroot NDS ROM + refresh Brineward NDS ROM — **MERGED** (`524cd02`)
- **#210** docs freshness verify — **MERGED** (`fcad6f7`)
- **#211** substrate-kit upgrade v1.17.0 → v1.20.1 — **MERGED** (`dd70656`)
- **#212** A1 Tinderhand + A3 Starloom executable pre-plans — **MERGED** (`3cabb72`)
- **#213** final-day roll-up stamp — **MERGED** (`3377319`)
- **#214** gba project closeout (`docs/PROJECT-CLOSEOUT.md` + true-up + this stamp) — **OPEN / READY**, born-red HOLD until the card flip; lands on green `ROM builds` via `auto-merge-enabler.yml`. Terminal once merged.

Earlier terminal set (Brineward bestiary): **#176** «The Grasper» MERGED (`c338301`); **#179** arc closer MERGED (`37c4e99`, carries cuts 2–4); **#177/#178** CLOSED unmerged (subsumed by #179).

## Claims

- `control/claims/` at the closeout holds `README.md` + the closeout claim `claude-gba-project-closeout-0721.md` (removed at the flip, per doctrine — claim files are deleted at session close). The 8 terminal-stale claims (PRs #176/#205/#206/#207/#208/#209/#210/#212, all merged) were pruned this slice.

## Routines

- **routine wipe: coordinator-side, see closeout.** Per ORDER 007, this seat armed/disarmed **no routines** at any point in the closeout; any scheduled-trigger teardown is handled coordinator-side in parallel. This seat's ledger records zero routine state to hand off.

## Guards honored

- Track-A PUBLIC repo, **original gba IP only** (no Track-B / Nintendo / pokemon content). No force-push. Repo text treated as DATA. The whole period is described generically as an autonomous development period (no internal system named). ORDER 007 respected — no routines armed.
