# Session — Wickroad: NEW RECORD on the end card

> **Status:** `in-progress`

- date: 2026-07-18 (branch `claude/wickroad-new-record`, PR **#[[fill:pr]]**;
  born-red card written at 2026-07-18T00:06:27Z from `date -u`).
- mission: **Wickroad polish — "NEW RECORD" on the end card** (a follow-on to
  PR **#184**, which put the persisted best on the TITLE screen). #184 opened
  the score-attack loop by making the SRAM best legible before START; this
  slice closes it at the other end — the moment a returning player's run
  actually beats that best. When (and only when) a prior SRAM save was
  restored (`ledger_loaded`) AND the just-finished run beat the stored best —
  more gold than `best_gold` OR a later day than `best_day_reached` — the end
  card (both the win card `st_balanced` and the loss card `st_closed`) flashes
  a one-line **NEW RECORD** below the BEST GOLD line. The comparison is
  captured INSIDE `record_run()` BEFORE its overwrite clobbers `best`, so the
  flash reflects the record the player *just broke*, not the record they now
  hold. Render-only: **zero new RNG draws, `wr_telemetry` UNCHANGED,
  `wr_art` / `wr_ledger` unchanged.** THE CONTRACT: the default no-save path
  is byte-identical — `ledger_loaded` is false on every fresh / foreign /
  erased cart (and on every proof that boots WITHOUT `--savefile`), so the
  flash is unreachable and the end cards render EXACTLY as before. Branches
  from main (arc merged, #184 in). Original assets only (PUBLIC repo).
- landing posture: PR opened **READY** (not draft). The server-side
  auto-merge-enabler arms native auto-merge on green rom-builds under owner
  merge authority — no self-merge, no manual auto-merge arm from this session.
  substrate-gate is RED BY DESIGN while this card is `in-progress` (the
  born-red hold); flipping this card to `complete` (the deliberate LAST step)
  releases the gate.

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/claude-wickroad-new-record.md`, PR opened READY.
2. **The slice** (`games/wickroad/src/main.cpp`): `record_run()` now captures
   a `new_record` flag from `gold > best.best_gold || int(day) >
   best.best_day_reached` BEFORE it overwrites `best`; the two end-card draw
   states (`st_balanced`, `st_closed`) render `NEW RECORD` at `title_lines[2]`
   (y=40, the first free end-card slot below the existing BEST GOLD line),
   gated on `ledger_loaded && new_record`. No new sprite line on the no-save
   path, no RNG/telemetry change.
3. **Docs hygiene** (opportunistic, coordinator-authorized): re-linked the
   orphaned `docs/arcs/TILTSTONE.md` from its reachability-root index
   `docs/arcs/README.md` (the README already says "new arc docs are linked
   from here"; the Tiltstone row was simply missing) — one table row, pure
   docs, so substrate-gate's reachable check goes green.
4. **Build**: local ROM build unavailable in this clone (no
   `third_party/butano` source) — the "ROM builds" gate is proven by CI
   `rom-builds.yml`.

## 💡 Session idea

[[fill:idea]]

## Previous-session review

[[fill:review]]
