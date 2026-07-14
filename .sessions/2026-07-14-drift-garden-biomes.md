# Session — Drift Garden growth cut 4: biomes (wind patterns per biome)

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/drift-garden-biomes`, started 06:18Z —
  from `date -u` at write time)
- mission: build Drift Garden's LAST named growth cut from
  `games/mobile-foundation/CONCEPT.md` — biomes ("Depth ramps: wind
  patterns per biome"; the core loop promises "spend essence on new
  biomes/palettes/species" and after PR #119 "biomes remain the named
  follow-up"). Scope: a small set of biomes purchasable with banked
  essence in the dusk-screen shop (the PR #119 wallet/shop plumbing),
  each carrying a distinct WIND PATTERN — sim parameters for how the
  round's wind evolves (gust strength, gust cadence, swirl, prevailing-
  bias coupling), not just colors. Biome choice is read ONCE at round
  start as parameters (the PR #111 weather discipline: sim step never
  reads meta or the clock); for a given biome + seed + date the round is
  deterministic, and the free default biome reproduces pre-cut behavior
  byte-exactly (gameplay RNG draw count/order unchanged). This completes
  the concept's named growth path. No server, no Makefile.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

(in progress — flipped at session close)
