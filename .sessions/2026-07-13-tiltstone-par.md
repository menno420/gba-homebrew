# Session 43 — Tiltstone slice 2: PAR SCORING + UNDO (web arcade)

> **Status:** `in-progress`

- date: 2026-07-13 (branch `claude/tiltstone-par`, started 13:24:31Z per `date -u`, base main `d87f9ad`)
- mission: the Tiltstone concept doc's growth-path items 2 + 3 — its named
  retention hook: **par scoring** (the generator's BFS solver already knows a
  shortest winning line; grade the player's turn count against it on every
  cleared cavern) and **undo** (engine states are immutable snapshots — a
  history stack in the shell, free by construction). Engine changes are pure
  and ADDITIVE (`par`, `grade` — zero edits to generation/rotation, so every
  pinned smoke value from PR #81 must re-pass byte-identical); the shell gains
  a U-key/button undo with an honest undo counter shown on the win card.
  Proof surface: extend `games/web-tiltstone/smoke.mjs` (pure Node) +
  `tools/web-smoke-tiltstone.mjs` (real Chromium), both run locally, verbatim
  output pinned here before the flip.
- provenance: work-ladder rung 3 (no unexecuted order in `control/inbox.md`
  at HEAD `d87f9ad` — ORDER 005 served per PR #87/#89; the seat-ender baton's
  two items are owner-gated or actively claimed). Standing owner direction:
  "continue expanding the games". Collision check at session start: Brineward
  slice 8 is actively built by session 42 (PR #91, branch
  `claude/brineward-slice-8`, pushing live at 13:19Z) — untouched here;
  Gloamline is scope-complete + owner-gated; Tiltstone is on main (PR #81
  merged `f8540b1`), its prototype claim's session closed complete, and no
  open PR or claim covers a Tiltstone slice 2. `control/claims/claude-tiltstone-par.md`
  filed in this commit (claim-before-build).
- landing posture: PR opened READY at this born-red commit; **no merge, no
  auto-merge arming, no labels from this session** — parked green for the
  owner sweep alongside #82–#91 (dispatch rail; matches the night-run set's
  posture and the documented "[Self-Approval]…Merge Without Review" wall).
- 📊 Model: Claude (Fable family)

## What this session did

(born red — filled at the flip)
