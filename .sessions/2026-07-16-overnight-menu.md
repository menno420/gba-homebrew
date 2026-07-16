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
- **📊 Model:** Opus 4.8 (Claude family) · medium · planning/docs — overnight veto-menu
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

**A veto-menu's honesty lives in its category tally, not its length —
so the tally belongs IN the doc, machine-checkable, next to the summary
table.** Writing 84 proposals across 18 groups, the real failure mode is
not padding but *silent per-game imbalance*: it is easy to give the
freshest games (Underroot, Tiltstone) five cuts each and quietly leave
an older title (Undertow, Deepcast) with one, so the owner vetoes a rich
menu without noticing a whole game was under-served. The cheap guard is
a committed per-category count table (this doc's foot) that a future
session — or a CI check — can diff against the game roster: "every game
in current-state.md has ≥1 row in the latest planning menu" is a
one-line assertion that turns menu-breadth from a vibe into a pin, the
same move the lane already makes for proofs and claims.

## Previous-session review

- Prior lane slice: `.sessions/2026-07-16-current-state-rows.md` (PR
  #152) — its discipline of citing every published SHA against the live
  API a day later is exactly what this menu leaned on: it reconciled the
  ledger to `d38887c`/#151, so grounding this menu against the #152
  merge (`478bf16`) started from a trusted table rather than a stale
  one, and its 💡 (an ender that condenses control/status.md can drop a
  done-when receipt) is why this slice APPENDS a dated section to
  status.md rather than rewriting it. One thing it could not know:
  today's #155–#170 stack was still unlanded at its write, so this menu
  is the first doc to fold Underroot (the 12th game) and Tiltstone arc 2
  into the roster — as dependency-flagged rows, not shipped state.
