# Session — Drift Garden growth cut 4: biomes (wind patterns per biome)

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/drift-garden-biomes`, started 06:18Z,
  flipped 06:33Z — both from `date -u` at write time)
- mission: build Drift Garden's LAST named growth cut from
  `games/mobile-foundation/CONCEPT.md` — biomes ("Depth ramps: wind
  patterns per biome"; the core loop promises "spend essence on new
  biomes/palettes/species" and after PR #119 "biomes remain the named
  follow-up"). Scope: a small set of biomes purchasable with banked
  essence in the dusk-screen shop (the PR #119 wallet/shop plumbing),
  each carrying a distinct WIND PATTERN — sim parameters for how the
  round's wind evolves (gust strength, gust cadence, swirl, sway,
  prevailing-bias coupling), not just colors. Biome choice is read ONCE
  at round start as parameters (the PR #111 weather discipline: the sim
  step never reads meta or the clock); for a given biome + seed + date
  the round is deterministic, and the free default biome reproduces
  pre-cut behavior byte-exactly. This COMPLETES the concept's named
  growth path. No server, no Makefile.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-drift-garden-biomes.md`, born red
   (claim before build), cut from main @ `65c3659` (PR #123 merged).
2. **Baseline first**: pre-change smoke 27/27 PASS, then captured the
   PRE-CUT fixed point from the unmodified build — seed 11 under fake
   UTC date 20300607, hidden boot, `snapshot()` at steps 500 and 1500 —
   and pinned those bytes in the extended smoke.
3. **Biomes** (`games/mobile-foundation/game.js`): a BIOMES table —
   meadow (free) / gale ridge (60) / whorl hollow (130) / tide flats
   (220) — each a wind-pattern parameter set: `windMul` (gust strength),
   `gustEvery` (redraw cadence), `swirlRate` (a deterministic vortex
   rotating the wind vector by swirlRate*roundStep, pure in roundStep,
   zero RNG draws), `swayPeriod` (a sine amplitude envelope), `biasMul`
   (coupling to the day's prevailing weather bias), plus a render-only
   hue `tint` additive with the palette's. `resetRound` reads the active
   biome ONCE into `roundBiome`; the sim step reads only `roundBiome`.
   Default meadow is an identity element: multipliers 1 (IEEE-exact),
   cadence 600, swirl/sway branches gated off so the pre-cut float math
   runs literally unchanged — a meadow round is byte-identical to a
   pre-biomes round, all 27 pre-existing assertions green unedited.
4. **Wallet/shop**: guarded `driftgarden.meta` gains `biomes`/`biome`
   fields (field-by-field validation; pre-biomes stored meta keeps
   defaults). A labelled second shop section on the dusk screen —
   `biomeRows()` shares the pure-geometry renderer/hit-test contract
   from #119; a row tap buys (affordable) or selects (owned); selecting
   never touches the frozen round (roundBiome is only re-read at
   replant). `snapshot()` carries the round's biome (a sim input, like
   weather); surfaces `__game.biomeTable/biomeRows/buyBiome/selectBiome`;
   `sw.js` CACHE `drift-garden-v5` -> `drift-garden-v6`; CONCEPT.md
   biomes lines marked BUILT + status says the named growth path is
   complete; README refreshed.
5. `tools/mobile-foundation-smoke/game-smoke.mjs` extended +8 assertions
   (27 -> 35): `biome-table-and-default` (identity params, fresh meta),
   `biome-default-precut-identity` (the pinned pre-cut bytes reproduce
   under meadow, biome key stripped before compare),
   `biome-deterministic-per-biome` (each biome at seed 11 + fake date
   twice -> byte-identical two-stage snapshots),
   `biome-changes-world` (4 pairwise-distinct worlds on the SAME
   seed + date), `biome-spend-deny-buy` (deny/deduct/re-buy/unowned-
   select), `biome-shop-taps-frozen-round` (REAL tap verb buys/selects;
   the frozen round keeps phase AND biome), `biome-read-at-round-start`
   (replant runs the selected biome, wallet intact),
   `biome-persists-reload` (unlocks + active biome + the booted round's
   biome through the real guarded boot path). `unlocks-inert-to-sim`
   strengthened: every biome OWNED with meadow active is still
   byte-identical to fresh storage (ownership is meta; only the ACTIVE
   biome is a sim input). Fixed a harness pitfall: init-script meta
   pre-seeding now writes only when the key is absent, so a reload
   observes what the game persisted, not the seed. Run twice: 35/35
   PASS both, deterministic lines byte-identical (only live-page
   pause/resume wall-clock frames differ). Foundation smoke 6/6.
6. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge (the designed born-red hold) + the
   pre-existing `claims-format` advisory on `order-005-scribe.md` (not
   this lane's file, advisory, left alone). Telemetry committed.
7. PR #124 opened READY immediately (no draft phase); no
   merge/approve/auto-merge calls from this session — the server-side
   enabler decides on green. Heartbeat appended to `control/status.md`
   (append-only dispatch section, PR number confirmed first).
8. Claim `control/claims/claude-drift-garden-biomes.md` retired with
   this flip per the claims README (the open PR is the live claim from
   here). This closes Drift Garden's named growth path — weather (#111),
   species (#115), essence spending (#119), biomes (#124).

## 💡 Session idea

**When a new parameter layer must default to "no change", make the
default an algebraic identity of the existing math and pin the OLD
build's bytes before you touch it — preservation becomes construction
plus one dead-build assertion.** Two moves, both cheap. First, shape the
default so the old code path literally executes: multipliers of 1 (IEEE
multiplication by 1.0 is exact, so `x * weather.mul * 1` is bit-equal to
`x * weather.mul`), cadences equal to the old constant, and every new
evolution rule behind a `!== 0` gate so the default never runs new
float ops at all — identity holds by construction, not by hoping the
refactor was careful. Second, before the first product edit, run the
UNMODIFIED build and embed its snapshot bytes in the new test as a
literal fixed point ("pre-cut build @ <sha>"): the old build dies in
git history, but its behavior stays enforceable forever, and any future
regression of the default is caught against bytes no one can quietly
re-derive. The two halves cover each other — the gate argument says why
it can't drift, the pinned bytes notice if it somehow does. Generalized:
identity-by-construction plus a dead-build fixed point is the cheapest
honest answer to "this feature changes the sim, but only when asked to".

## Previous-session review

- Prior slice: `.sessions/2026-07-14-undertow-oxygen.md` (PR #123,
  Undertow oxygen + air pockets) — holds up: its central move (a
  sim-CHANGING cut that still carries a layout-identity witness via a
  side-band RNG stream with fixed draw count) is the same contract this
  cut leaned on, and its "split the fixed point" idea directly shaped
  this session's baseline discipline — the pre-cut snapshot was captured
  BEFORE the first edit and pinned as the meadow identity witness, the
  per-biome snapshots re-derived as the legitimately-moved outcomes. Its
  card is precise about what moved vs what stayed (823/194m -> 810/191m
  re-derived; frame-37 wall crash carried verbatim), which made trusting
  the pattern cheap. One nuance it left implicit that this lane hit in
  practice: when a TEST pre-seeds persistent storage via an init script,
  the seed re-runs on reload and silently overwrites what the product
  wrote — #123 never reloads inside a rig so it couldn't see it; this
  session's `biome-persists-reload` did (first red run), and the fix
  (seed only when the key is absent) is now in the shared
  `newScriptedPage` helper for every future cut. No gaps found in #123's
  claims themselves.
