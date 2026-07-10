---
state: routed
origin: lab
shipped_pr: null
shipped_repo: null
merged_date: null
outcome: open
---

# HUD depth-pixel CI assertion (OCR-free state check for headless runs)

> **Status:** `ideas`

**Idea (from the session-2 card):** extend `tools/headless-screenshot.py`
(or a sibling `tools/assert-state.py`) to check the DEPTH HUD without OCR by
sampling the known text-pixel region against pre-rendered glyph patterns of
the `common::variable_8x8_sprite_font` digits — so CI can assert "this
recorded input sequence reaches depth N", the concept doc's own
regression-test design for Lumen Drift.

**Why now it's stronger:** increment 2 (PR #9) added the game-over card,
which renders final DEPTH and BEST at fixed screen positions — two more
stable text anchors. A depth assertion would turn the current
"screenshots differ" interactivity proof into a semantic regression test
(e.g. "the committed proof script still dies on the crystal at DEPTH 26,
and BEST persists as 26 after restart").

- **Area:** `tools/` (generic harness — works for any game that renders
  score text at a fixed position with the common font).
- **Size:** S–M (glyph table generation + a `--assert-text X,Y:STRING`
  flag; the PNG reader already exists in the script).
- **Risk:** low; fully deterministic on mGBA.
- **Route:** quick-win — good opening task for a future session; pairs with
  the session-3 card's `bn::sram` best-persistence idea (assert BEST
  survives an in-process core reboot).
