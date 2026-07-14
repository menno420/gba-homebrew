# Session — Drift Garden growth cut 2: more species

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/drift-garden-species`, started 02:56Z
  from `date -u` at write time)
- mission: build Drift Garden's next named growth cut from
  `games/mobile-foundation/CONCEPT.md` — "more species" (the growth path
  names "biomes, more species, essence spending"; the core loop names
  "motes that cluster cross-pollinate into rarer, brighter species").
  Data-driven extension of the existing tier machinery: a species table
  (two species per tier — a common line carrying the pre-cut values/hues
  exactly, plus a rarer/brighter line with distinct visuals and higher
  essence value), tier-1 planting picks its species from a side-band
  seeded stream (the round's gameplay RNG draw count/order UNCHANGED —
  the PR #111 contract), and cross-pollination becomes species-aware:
  same-species parents breed the common next-tier species, mixed-species
  parents breed the rare one (hybrid vigor). No new persistence, no new
  screens, no server, no URL params, no Makefile.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.

## Plan

1. This card + `control/claims/claude-drift-garden-species.md`, born red
   (claim before build), cut from main @ `aac62b0` (PR #114 merged).
2. `games/mobile-foundation/game.js`: SPECIES table replaces
   TIER_VALUE/TIER_HUE; species roll on a side-band mulberry32 stream
   keyed off the round seed; species-aware pollination; rare-species
   render accent in the existing canvas style; `sp` in `snapshot()`
   motes; `__game.speciesTable()` pure surface for proofs.
3. `sw.js` CACHE v3 -> v4; CONCEPT.md growth line marked BUILT; README
   refreshed.
4. `tools/mobile-foundation-smoke/game-smoke.mjs` extended with species
   assertions (deterministic appearance at fixed seeds under an injected
   fake Date, harvest values per species, pure/hybrid pollination lines,
   and the extended ladder through tier 3). Run twice; foundation run.sh
   still green.
5. Flip this card, PR READY, heartbeat in control/status.md.
