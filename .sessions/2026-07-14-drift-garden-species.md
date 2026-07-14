# Session — Drift Garden growth cut 2: more species

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/drift-garden-species`, started 02:56Z,
  flipped 03:06Z — both from `date -u` at write time)
- mission: build Drift Garden's next named growth cut from
  `games/mobile-foundation/CONCEPT.md` — "more species" (the growth path
  names "biomes, more species, essence spending"; the core loop promises
  motes that "cross-pollinate into rarer, brighter species"). Data-driven
  extension of the existing tier machinery; no new persistence, no new
  screens, no server, no URL params, no Makefile.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-drift-garden-species.md`, born red
   (claim before build), cut from main @ `aac62b0` (PR #114 merged).
2. **Species** (`games/mobile-foundation/game.js`): a SPECIES table
   replaces TIER_VALUE/TIER_HUE — two species per tier, index 0 the
   COMMON line carrying the pre-cut hues/values exactly (fern/lotus/
   aurora, 130/200/52, 5/15/40), index 1 the RARE line (clover/iris/
   solaris, distinct hues, a bright-core render accent in the existing
   canvas idiom, values 8/22/55). Tier-1 spawns roll their species on a
   SIDE-BAND mulberry32 stream keyed off the round seed
   (`seed ^ 0x85EBCA6B`, one draw per tier-1 spawn) — the gameplay
   stream's draw count and order are UNCHANGED (the PR #111 weather-cut
   contract; all 15 pre-existing assertions stayed green with zero
   edits). Cross-pollination is species-aware and PURE in the parents:
   same-species pair -> common next-tier line, mixed pair -> rare
   (hybrid vigor), zero extra random draws — herding unlike motes
   together is now the high-value play.
3. **Surface**: `sp` per mote in `snapshot()`; pure proof surfaces
   `__game.speciesTable()` and `__game.childSpeciesFor(tier, a, b)` (no
   game state touched). `sw.js` CACHE `drift-garden-v3` ->
   `drift-garden-v4` (shipped shell change); CONCEPT.md growth line +
   core-loop line marked BUILT; README.md refreshed.
4. `tools/mobile-foundation-smoke/game-smoke.mjs` extended +6 assertions
   (15 -> 21): `species-child-derivation` (pure parents->child table),
   `species-both-kinds` (both tier-1 species appear deterministically at
   seed 12 under an injected fake Date; every mote table-valid),
   `species-harvest-values` (fern +5, clover +8), `species-pure-pair`
   (seed 27's fern quartet -> two tier-2 lotus), `species-tier3-ladder`
   (the two lotus herded together with the real drag verb — `current()`
   draws no randomness — mature and merge to tier-3 aurora by frame 620),
   `species-hybrid-pair` (seed 9's fern+clover -> tier-2 iris, harvested
   for +22). Species pages pin seed AND fake date, so they are byte-
   stable regardless of run day. Run twice against real Chromium: 21/21
   PASS both runs, all deterministic assertion lines byte-identical
   (only live-page pause/resume wall-clock frame counts differ).
   Pre-change baseline 15/15; untouched foundation smoke (`run.sh`) 6/6.
5. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge (the designed born-red hold) + the
   pre-existing `claims-format` advisory on `order-005-scribe.md` (not
   this lane's file, advisory, left alone).
6. PR #115 opened READY immediately (no draft phase, no labels); no
   merge/approve/auto-merge calls from this session — the server-side
   enabler decides on green. Heartbeat appended to `control/status.md`
   (append-only dispatch section, PR number confirmed first).
7. Claim `control/claims/claude-drift-garden-species.md` retired with
   this flip per the claims README (the open PR is the live claim from
   here).

## 💡 Session idea

**Make the rare outcome a pure function of player-composable inputs, not
a rarity roll — depth lands in the verb and the proof shrinks to a table
lookup.** The default way to add rare content to a seeded game is a
loot-table roll, which buys no gameplay (the player can't pursue it) and
costs determinism surface (another draw to sequence). This cut put the
rarity in the COMBINATION instead: which two species you herd together
decides the child (`childSpeciesFor` is pure in the parents, zero draws),
so the rare line is something you play FOR with the existing drag verb —
and the smoke asserts it with exact-value checks (fern+clover -> iris,
+22) instead of statistical ones. The only random species decision left
is the tier-1 roll, and that rode a side-band stream. Generalized: when
adding rarity to a deterministic system, prefer "rare = a reachable
composition of common things" over "rare = a low-probability draw" —
players get agency, and tests get equalities instead of distributions.

## Previous-session review

- Prior slice: `.sessions/2026-07-14-undertow-cosmetics.md` (PR #114,
  Undertow diver skins + bubble trails) — holds up: its headline
  discipline (prove a cut's blast radius with an identity assertion —
  same crashFrame/depth across skins — rather than prose) transferred
  here inverted: where #114 proved "render-only, world identical", this
  cut CHANGES the world, so the analogous proof became "gameplay stream
  draw order untouched" (pre-existing assertions green with zero edits)
  plus exact-value species assertions; no gaps found in #114's card, and
  its `?skin=` URL-param persistence pattern was correctly NOT copied
  here (species are seed-derived world state, not player cosmetics).
