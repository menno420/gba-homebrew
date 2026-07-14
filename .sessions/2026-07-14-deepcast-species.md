# Session — Deepcast growth cut 3: species tables per depth band + catch log

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/deepcast-species`, started 05:18Z —
  from `date -u` at write time)
- mission: build Deepcast's next named growth cut from
  `games/deepcast/CONCEPT.md` — "Fish species tables per depth band with
  named rarities; a catch log" — as a data-table extension of the
  existing bite-by-depth logic: 16 named species (four depth bands x
  four rarity tiers), the species revealed on the catch card, and a
  per-run catch log opened with SELECT (the one unused key). Species
  selection rides a SIDE-BAND RNG stream derived from the dialed seed
  (the Drift Garden PR #115 pattern), so the main stream's word order is
  untouched and every v0.3 pin carries; same seed = same casts = same
  fish, including species. The log is session-scope (per run) — GBA SRAM
  persistence is a bigger cut, noted as follow-up at the BUILT mark.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-deepcast-species.md`, born red
   (claim before build), cut from main @ `6c6b089` (PR #120 merged).
2. Toolchain certified before touching code: devkitARM (leseratte10
   mirror pin) + Butano present, mgba binding under Python 3.11 imported
   clean; main's unmodified tree rebuilt to the committed
   `dist/deepcast.gba` v0.3 hash `2a701069…` EXACTLY.
3. (work in progress — flips with the final commit)
