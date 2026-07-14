# Session — Drift Garden growth cut 3: essence spending

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/drift-garden-essence`, started 04:26Z,
  flipped 04:35Z — both from `date -u` at write time)
- mission: build Drift Garden's next named growth cut from
  `games/mobile-foundation/CONCEPT.md` — "essence spending" (the growth
  path names "biomes, essence spending"; the core loop promises "harvest
  matured motes for essence, spend essence on new biomes/palettes/
  species"). Scope pick: a persistent essence wallet (guarded
  localStorage, the lane's guarded-access pattern) banked at round end,
  plus a dusk-screen palette shop unlocking RENDER-ONLY garden palettes —
  palettes/cosmetics are the contained reading of the doc's examples;
  biomes are the largest surface and remain the named follow-up. No
  server, no Makefile, sim untouched (gameplay RNG draw order unchanged
  for a given seed, the PR #111/#115 side-band discipline).
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-drift-garden-essence.md`, born red
   (claim before build), cut from main @ `afd2918` (PR #118 merged).
2. **Wallet** (`games/mobile-foundation/game.js`): persistent meta
   `driftgarden.meta` (wallet + unlocked palette ids + active palette)
   behind guarded localStorage — every touch in try/catch, corrupt or
   unknown stored values dropped field-by-field, storage failure
   degrades to session-only meta. `endRound` banks the round's harvest
   (win or lose); the sim never reads the wallet.
3. **Palette shop**: a PALETTES table (verdant 0 / dusk bloom 40 /
   moonlit 90 / ember dawn 160) — each palette PURE RENDER: a
   daylight->dusk background gradient pair + a draw-time hue shift
   (per-mote hues stay stored unshifted); index 0 reproduces the pre-cut
   rendering exactly. Dusk-screen shop rows: geometry a pure function of
   canvas size, shared by renderer and tap hit-test; on the end screen a
   row tap buys (affordable) or selects (owned), any other tap replants
   as before. No new RNG draws, no sim-step meta reads, snapshot()
   unchanged — all 21 pre-existing assertions green with zero edits.
4. **Surface**: `__game.paletteTable/metaState/shopRows/buyPalette/
   selectPalette`; wallet/palette in `#hud` dataset + `__gameState`.
   `sw.js` CACHE `drift-garden-v4` -> `drift-garden-v5`; CONCEPT.md
   essence-spending line marked BUILT (biomes-remaining note explicit);
   README refreshed.
5. `tools/mobile-foundation-smoke/game-smoke.mjs` extended +6 assertions
   (21 -> 27): `wallet-banks-at-round-end` (fresh storage 0; a won round
   banks its exact harvest), `spend-deny-buy-rebuy` (unaffordable refuse
   with wallet unchanged, affordable deduct/unlock/activate, re-buy
   refused), `shop-taps-select-not-replant` (REAL tap verb on shop rows
   switches palettes, frozen round stays frozen),
   `replant-keeps-wallet-and-accrues` (off-row tap replants, wallet
   intact; round 2's dusk banks on top), `meta-persists-reload` (full
   page reload through the real guarded boot path),
   `unlocks-inert-to-sim` (seed-7 script: fresh storage vs wallet 500 +
   all palettes + non-default active -> byte-identical snapshots). Meta
   pre-seeding writes real localStorage before boot — no test-only
   injection point in the product. Run twice against real Chromium:
   27/27 PASS both runs, deterministic assertion lines byte-identical
   (only live-page pause/resume wall-clock frames differ). Pre-change
   baseline 21/21; untouched foundation smoke (`run.sh`) 6/6.
6. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge (the designed born-red hold) + the
   pre-existing `claims-format` advisory on `order-005-scribe.md` (not
   this lane's file, advisory, left alone).
7. PR #119 opened READY immediately (no draft phase, no labels); no
   merge/approve/auto-merge calls from this session — the server-side
   enabler decides on green. Heartbeat appended to `control/status.md`
   (append-only dispatch section, PR number confirmed first).
8. Claim `control/claims/claude-drift-garden-essence.md` retired with
   this flip per the claims README (the open PR is the live claim from
   here).

## 💡 Session idea

**Give meta UI a pure layout function shared by renderer and hit-test,
and the untestable "did the tap land on the button?" problem disappears
from canvas games.** Canvas UIs usually bury button rectangles inside the
draw call, so tests either replay pixel coordinates copied from a
screenshot (brittle) or bypass the UI entirely via a test API (proves
nothing about the real surface). This cut factored the shop rows into
`shopRows()` — a pure function of canvas size — consumed by BOTH the
renderer and `tapAt`'s hit-test, and exposed as a proof surface. The
smoke then drives the REAL tap verb at `shopRows()` centers: the
assertion exercises the same geometry the player's thumb hits, stays
valid under any viewport/DPR, and needed zero test-only branches in the
product. Generalized: when adding tappable chrome to a canvas game,
define the geometry once as pure data, render from it, hit-test from it,
and export it — layout becomes a contract instead of a coincidence
between two hand-maintained coordinate lists.

## Previous-session review

- Prior slice: `.sessions/2026-07-14-undertow-ghost.md` (PR #118,
  Undertow ghost replays) — holds up: its central discipline (persist to
  guarded localStorage, keep the persisted thing render-only, and prove
  live-run identity against cleared storage) is exactly the pattern this
  cut reused for the wallet/palettes, with `unlocks-inert-to-sim` as the
  direct analogue of #118's ghost-on-vs-cleared-storage identity
  assertion; its factoring move (both divers execute the SAME physStep,
  making fidelity structural) maps to this cut's shared `shopRows()`
  geometry (render and hit-test can't drift apart). No gaps found in
  #118's card; one nuance it left implicit — a storage schema carrying
  multiple fields wants field-by-field validation on load, not
  all-or-nothing parse — this card makes explicit in `loadMeta()`.
