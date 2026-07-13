# Session 41 — Cindervault: turn-based dungeon-dive roguelike, GBA (breadth program, night run)

> **Status:** `in-progress`

- date: 2026-07-13 (branch `claude/cindervault`, started 04:02:32Z per `date -u`)
- mission: game #6 of tonight's breadth program — a NEW original GBA micro-game distinct in genre from every sibling (Lumen Drift GBA gravity drifter, Gloamline NDS survival, Brineward NDS naval PR #82, Undertow web reflex arcade, Tiltstone web gravity puzzle, Drift Garden mobile idle-garden PR #84, Deepcast GBA fishing tension arcade PR #83): **Cindervault**, a seeded turn-based dungeon-dive roguelike. Dive 5 floors of a seeded dungeon; every step burns torchlight (torch 0 = DARKNESS), embers refill the torch, bump-combat monsters chase one step per player turn (player HP 0 = SLAIN), reach the stairs on floor 5 to open the vault (WIN). Turns advance ONLY on player input — no frame-time in the logic. Deliverables: `games/cindervault/` (Butano game + CONCEPT.md), `dist/cindervault.gba` + README row, headless proof via `tools/headless-screenshot.py` with a `cv_telemetry` mailbox.
- session number: 41 chosen because open PR #85's card (`.sessions/2026-07-13-release-packaging.md`) self-identifies as session 40 (the current max — #83 is 39, #82 is 38), PR #84's drift-garden card is unnumbered, and no card on main or in an open PR claims 41.
- provenance: ORDER 005 (owner night-run directive, 2026-07-13, scribed verbatim in `control/inbox.md` @ HEAD d87f9ad): "THE BREADTH PROGRAM starts now: multiple NEW small games tonight — each a playable prototype slice + a one-page concept (genre, loop, platform, sellability guess)." Dispatched breadth-program session; no merge action, no labels. Base: main @ d87f9ad.
- heartbeat: SKIPPED by dispatch constraint (`control/status.md` intentionally untouched this session, no `control/outbox.md` writes); recorded here instead.
- 📊 Model: Fable 5 (family-level self-report from this session's own harness, per ORDER 003)

## What this session did

1. This card + `control/claims/claude-cindervault.md`, born red (claim before build). Checked collisions first at main tip d87f9ad: open PRs are only #82 (Brineward bands), #83 (Deepcast), #84 (Drift Garden), #85 (release packaging) — no dungeon-crawler/roguelike sibling, no `cindervault` anywhere in claims or branches. PR opened READY at this commit.

_(sections below fill in as the session runs; badge flips only at close)_

## Harness evidence (verbatim)

_(pending — pinned from observed runs before the flip)_

## 💡 Session idea

_(pending)_

## Previous-session review

_(pending)_
