# Session — Wickroad: the lifetime run count on the title

> **Status:** `in-progress`

- date: 2026-07-18 (branch `claude/wickroad-lifetime-runs`, PR **#[[fill:pr]]**;
  born-red card written at 2026-07-18T00:19:39Z from `date -u`).
- **📊 Model:** opus-4.8 · high · polish slice — surface the SRAM-banked
  lifetime run count on the Wickroad title screen.
- mission: **Wickroad polish — "THE RUN COUNT ON THE TITLE"** (a follow-on to
  PR **#184** — the persisted best on the title — and PR **#185** — the NEW
  RECORD flash on the end card). Both prior slices exploited the same pattern:
  a value that Wickroad already banks in SRAM but renders on no screen. This
  slice takes the last such field. `best.runs` (the `wr_ledger_save.runs`
  member) is incremented in `record_run()` at every run-end and persisted to
  SRAM word `wr_ledger[4]`, but it is drawn on ZERO screens — a returning
  player can see their best gold and day (via #184) but never how many roads
  they have walked. This slice appends `RUNS n` to the title's existing
  `BEST GOLD x DAY y` line (`title_lines[1]`), gated on the SAME `ledger_loaded`
  flag, so the lifetime count rides the line a score-attack player already
  reads before START. Render-only: **zero new RNG draws, `wr_telemetry`
  UNCHANGED, `wr_art` / `wr_ledger` unchanged.** THE CONTRACT: the default
  no-save path is byte-identical — `ledger_loaded` is false on every fresh /
  foreign / erased cart (and on every proof booted WITHOUT `--savefile`), so
  the title's second line renders EXACTLY the committed flavor line at the same
  slot/position. Branches from main (#184 + #185 in).
- landing posture: PR opened **READY** (not draft). The server-side
  auto-merge-enabler arms native auto-merge on green rom-builds under owner
  merge authority — no self-merge, no manual auto-merge arm from this session.
  substrate-gate is RED BY DESIGN while this card is `in-progress` (the
  born-red hold) and additionally red on a pre-existing arcs/TILTSTONE.md
  orphan owned by another team — neither blocks the required `ROM builds` gate;
  flipping this card to `complete` (the deliberate LAST step) releases the
  born-red hold.

## What shipped

_(filled at closeout)_

## 💡 Session idea

_(filled at closeout)_

## Previous-session review

_(filled at closeout)_
