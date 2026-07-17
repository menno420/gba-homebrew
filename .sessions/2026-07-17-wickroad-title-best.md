# Session — Wickroad: the best on the title

> **Status:** `in-progress`

- date: 2026-07-17 (branch `claude/wickroad-title-best`, PR **#[[fill:pr]]**;
  born-red card written at 2026-07-17T23:54:42Z from `date -u`).
- mission: **Wickroad polish — "THE BEST ON THE TITLE"** (owner menu row
  **WIK-03**, the SRAM best ledger — "unblocks score attack"). The
  crossroads arc's cut 4 (PR #175) persists the best run in SRAM behind
  `gl_save.h` and restores it at boot, but the persisted best is drawn
  ONLY on the end cards — a returning player cannot see the record they
  are chasing until AFTER a run ends. This slice surfaces it where a
  score-attack player looks first: the **TITLE screen**. When (and only
  when) a prior SRAM save was restored (`ledger_loaded`), the title's
  second pitch line renders `BEST GOLD x DAY y` — both the persisted
  `best_gold` AND `best_day_reached` (the latter is stored but was never
  shown anywhere). Render-only: **zero new RNG draws, `wr_telemetry`
  UNCHANGED (57), `wr_art` and `wr_ledger` unchanged.** THE CONTRACT: the
  default no-save path is byte-identical — `ledger_loaded` is false on
  every fresh / foreign / erased cart (and on every proof that boots
  WITHOUT `--savefile`), so the flavor line renders EXACTLY as before and
  proofs **P1-P12** carry verbatim (P1's title-line text asserts run on a
  fresh cart; the only save-restore title proof, P12c, asserts the
  `wr_ledger` mailbox, not this text). Branches from main (arc merged).
  Original assets only (PUBLIC repo).
- **📊 Model:** [[fill:model]]
- landing posture: PR **#[[fill:pr]]** opened **READY** (not draft) —
  gba's server-side auto-merge-enabler arms native auto-merge on READY
  `claude/*` PRs, so this lands itself on green under owner merge
  authority. No self-merge, no manual auto-merge arm from this session.
  substrate-gate is RED BY DESIGN while this card is `in-progress` (the
  born-red hold); the required gate is "ROM builds". Flipping this card
  to `complete` (the deliberate LAST step) releases the gate.

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/claude-wickroad-title-best.md`, PR **#[[fill:pr]]**
   opened READY.
2. **The slice** (`games/wickroad/src/main.cpp`, `st_title` draw): the
   title's second pitch line (`title_lines[1]`, y=-2) becomes conditional
   — `BUT THE INK AGES` on the default no-save path (byte-identical),
   `BEST GOLD x DAY y` from the restored `best` struct when
   `ledger_loaded`. No new sprite line (reuses the existing slot/y — no
   clip, no sprite-budget delta), no RNG/telemetry change.
3. **Build**: local ROM build unavailable in this clone (no
   `third_party/butano` source) — the "ROM builds" gate is proven by CI
   `rom-builds.yml`. Proofs P1-P12 carry verbatim by the `ledger_loaded`
   gate (no proof asserts absence of this line; the render is unreachable
   on every no-`--savefile` boot).

## 💡 Session idea

[[fill:idea]]

## Previous-session review

[[fill:review]]
