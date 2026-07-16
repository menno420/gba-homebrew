# Session — Overnight planning menu (OVERNIGHT-MENU 2026-07-16)

> **Status:** `in-progress`

- date: 2026-07-16 (branch `claude/overnight-menu-2026-07-16`; started
  21:51Z, `date -u`)
- mission: build the owner's veto-ready **overnight planning menu** —
  as many concrete, distinct proposals as honestly possible across all
  12 games + engine/tooling/arcade/release/new-game/cross-game, so the
  owner skims the whole menu and vetoes by id. Quantity is deliberate;
  the veto is the filter, not the seat. **Planning docs only — nothing
  built.** Grounded against main HEAD
  `478bf167892751f907b02d26b59ec07596f602e2` (#152), with today's
  unlanded stack (#155–#165 Underroot v1.0, #166–#170 Tiltstone arc 2)
  read so dependent proposals are marked.
- **📊 Model:** [[fill: model family · effort · task-class]]
- landing posture: **DRAFT ONLY** under the STANDING 07-16 LANDING WALL
  — draft PR, NO ready-flip, NO auto-merge, does not touch #153–#170;
  awaiting owner ready-click. Card **stays `in-progress`** per the wall
  (we are NOT landing), holding the PR red by design.

## What shipped

1. Born-red gate (this commit): this card `in-progress` + work claim
   `control/claims/claude-overnight-menu-2026-07-16.md` (written by
   `bootstrap.py claim`) — committed FIRST so the HOLD is the opening
   commit and the PR is born red.
2. **The menu** (`docs/planning/OVERNIGHT-MENU-2026-07-16.md`): a
   Status-badged veto menu — a summary table (id · title · S/M/L ·
   one-liner) then one section per category, each proposal with a 2–3
   line pitch, effort, risk/reversibility, and what it unblocks; deps
   on today's stack marked. **84 proposals** across 18 category groups
   (44 per-game across all 12 games; 40 engine/CI/arcade/release/new-
   game/cross-game). Every proposal cited to a file@478bf16 / concept
   doc / PR where possible. Track isolation held: PUBLIC-repo original
   IP only, no Nintendo-derived material.
3. Dated worker section appended to `control/status.md` (append-only,
   coordinator convention).

## 💡 Session idea

[[fill: one genuine session idea]]

## Previous-session review

[[fill: one-line prev-session review]]
