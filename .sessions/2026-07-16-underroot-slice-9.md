# Session — Underroot arc slice 9: EEPROM best (best SCORE / furthest season)

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/underroot-slice-9`, stacked on
  `claude/underroot-slice-8` @ ec27203; started ~18:22Z, `date -u`)
- mission: **Underroot arc slice 9 — EEPROM best** (docs/arcs/UNDERROOT.md
  slice-9 row): the best `SCORE` and furthest season persist across power
  cycles on the cartridge backup chip, read/written over the card SPI bus (the
  battery DeSmuME emulates as a `.sav`). One fixed 32-byte record — 8
  little-endian u32 words `{ magic, version, best_score, furthest_season,
  run_seed, 0, 0, checksum }` — so serialization is fully deterministic. Safety
  by construction: a corrupt / blank / future-version blob decodes to the fresh
  all-zero table (NEVER a crash or hang). Writes are wear-disciplined: the
  record commits ONLY on a discrete player action (START) and ONLY when the run
  strictly improves the record. The `gl_save` precedent (Gloamline slice 9, PRs
  #50–#75) is reused verbatim for the hand-rolled bounded card-SPI I/O.

## Placeholders to resolve before finish

- PR number / head SHA — [[fill: on push]]
- CI run-ids per check — [[fill: after actions_list]]

*(Body filled in honestly at close-out; card stays `in-progress` — the PR is
draft-parked under the standing landing wall, no ready-flip.)*
