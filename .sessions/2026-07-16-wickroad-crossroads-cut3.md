# Session — Wickroad crossroads cut 3: THE SEED DIAL

> **Status:** `in-progress`

- date: 2026-07-16 (branch `claude/wickroad-crossroads-cut3`, PR
  **[[fill:PR]]**; started 23:43:40Z — from `date -u` at write time).
  Held DRAFT — Status stays `in-progress` by design (see landing
  posture).
- mission: **Wickroad crossroads cut 3 — "THE SEED DIAL"** per
  `games/wickroad/ARC-CROSSROADS.md` cut 3 and the Cindervault-seed
  (`.sessions/2026-07-14-cindervault-seed.md`) + Underroot slice-10
  (`claude/underroot-slice-10`, `ur_dial_seed`) precedents. Wickroad is
  the ONLY score-attack GBA title with a single fixed seed and no dial;
  this cut adds a **title-screen world-seed dial** (LEFT/RIGHT scan,
  edge-triggered, title ONLY — its keys are trading verbs everywhere
  else), turning the single fixed `0x5749434B` 'WICK' world into a
  family of daily/challenge worlds. THE CONTRACT (mirroring Underroot
  slice-10 / Cindervault-seed): **dial 0 IS `seed_constant`**, so the
  default boot world — and every committed proof P1-P10 — is
  bit-identical (zero behavioral change at default). `reset_run()` seeds
  from the dialed value, so START from a win/lose card reruns the SAME
  dialed world. The live dialed seed is published in an APPENDED
  telemetry word `[56]` (the append-only wire-format discipline extended
  `[56] → [57]`; the 56 committed sim words 0-55 stay BYTE-IDENTICAL by
  contract), so every dial step is machine-watchable. Proven by new
  proof **P11**. Stacked on cut 2 (PR #173). Original assets only
  (PUBLIC repo).
- **📊 Model:** Claude (Opus family) · high · feature build — wickroad
  crossroads cut 3, the seed dial.
- landing posture: PR **[[fill:PR]]** opened DRAFT and held draft per
  the standing 2026-07-16 landing wall — no ready-flip, no merge/
  approve/auto-merge calls from this session; card stays in-progress by
  design (born-red HOLD). Stacked on `claude/wickroad-crossroads-cut2`
  (#173), NOT on main. substrate-gate is RED BY DESIGN (main #151
  orphans + born-red HOLD); the required gate is "ROM builds".

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/claude-wickroad-crossroads-cut3.md`, PR
   **[[fill:PR]]** opened DRAFT immediately and held draft.
2. **The cut** (`games/wickroad/src/main.cpp`): a `dial_seed(d)`
   function — `d == 0` returns `seed_constant` EXACTLY (the pinned
   world), non-zero positions hash off the constant down a DIAL-salted
   xorshift into the bounded space `[1, 0xFFFFFF+1]` (never 0, the
   xorshift dead state). A `seed_dial` (0..15) + `run_seed` pair lives
   on the title; LEFT/RIGHT scan it edge-triggered on the title ONLY;
   `reset_run()` now seeds `rng_t rng(run_seed)` (was `seed_constant`)
   so dial 0 draws the exact committed stream. The live seed renders on
   the title's START line as `SEED xxxxxxxx` (the pinned `PRESS START`
   prefix carries) and is published in the appended telemetry word
   `wr_telemetry[56]` (array grown `[56] → [57]`), so each dial step is
   watchable and the run-time seed is pinned.
3. **The proof** (`games/wickroad/proofs.sh`, additive — P1-P10
   UNTOUCHED, byte-identical): **P11 THE SEED DIAL** — the title dial
   scans RIGHT×3 / LEFT×3 with `wr_telemetry[56]` pinned at every step
   against the host mirror (dial 0 `0x5749434B` → dial 1 `0x002FC1A3` →
   dial 2 `0x003B0B8A` → dial 3 `0x00405452` and back), the `SEED`
   digits glyph-exact on the title, state staying `title` throughout;
   dial 0 START reproduces the pinned world (word 13 = 9, the P2 day-1
   EMBERTON TALLOW pin) with `[56]` carrying the seed into the run; a
   dialed world DIFFERS (dial 1 word 13 = 12 ≠ 9) and its seed carries.
   RUN TWICE — watch-log byte-identical.
4. **Build + proofs**: [[fill:build result]].

## 💡 Session idea

[[fill:idea]]

## Previous-session review

[[fill:prev-review]]
