# Session — Deepcast growth cut 4: line upgrades bought with score + SRAM bank

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/deepcast-upgrades`, started 06:41Z from
  `date -u` at write time)
- mission: build Deepcast's next named growth cut from
  `games/deepcast/CONCEPT.md` — "Line upgrades bought with score (thicker
  line = higher snap threshold, slower reel) — a run-to-run meta without
  breaking determinism" — as the game's first persistent meta: each run's
  dusk score deposits into a BANK that survives power cycles (the
  `games/common/include/gl_save.h` magic-checked SRAM slot, the Lumen
  Drift `LDRIFT1` pattern), and a small LINE SHOP (SELECT on the title,
  R at dusk — edge-triggered, glyph text, the existing UI grammar) spends
  the bank on three line tiers with the doc's exact tradeoff: higher snap
  threshold AND slower reel per tier. A fresh cart (zeroed/erased SRAM)
  must boot exactly like v0.4 — every committed P1-P6 pin carries on
  first boot, since tier 0 IS the v0.4 constants.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-deepcast-upgrades.md`, born red
   (claim before build), cut from main @ `e6b7693` (PR #124 merged).
2. Toolchain certified before touching code: devkitARM (leseratte10
   mirror pin) + Butano present, mgba binding under Python 3.11 imported
   clean; main's unmodified tree rebuilt FROM CLEAN (scratch worktree)
   to the committed `dist/deepcast.gba` v0.4 hash `f87b7975…` EXACTLY.
3. (in progress — the cut, proofs, dist, flip)

## 💡 Session idea

(to be written at the flip)

## Previous-session review

(to be written at the flip)
