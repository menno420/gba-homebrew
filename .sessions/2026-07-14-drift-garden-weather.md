# Session — Drift Garden growth cut 1: daily seeded weather

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/drift-garden-weather`, started 01:49Z,
  flipped 01:56Z — both from `date -u` at write time)
- mission: build Drift Garden's lowest-risk named growth cut from
  `games/mobile-foundation/CONCEPT.md` — "daily seeded 'weather' so every
  player's Tuesday garden differs". Date-derived (UTC YYYYMMDD) daily
  weather parameters (wind strength multiplier + a prevailing-direction
  bias) that feed the sim deterministically: same UTC date → same weather
  for every player. The date is read ONCE at boot to pick the parameters —
  never inside the sim step — so (seed + action sequence) stays exactly
  reproducible within a day. Surfaced lightly: a `today: <weather>` label
  (canvas second HUD line + `#hud` mirror), matching the existing style.
  Pure-parameter cut: no new persistence, no new screens, no server, no
  URL params, no Makefile.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-drift-garden-weather.md`, born red
   (claim before build), cut from main @ `49b8bdf` (PR #110 merged).
2. **Daily weather** (`games/mobile-foundation/game.js`): `weatherFor(n)`
   is a pure function mapping UTC YYYYMMDD to one of five named weather
   kinds — still air / breeze / gusts / crosswind / halcyon drift, each a
   wind-strength multiplier (0.35×–1.9×) plus a prevailing-wind bias whose
   direction is picked per day. It runs on its OWN mulberry32 stream
   (seeded by the golden-ratio-mixed date), so weather selection never
   consumes from the round's gameplay RNG; the wind draw (`drawWind()`)
   still takes exactly two `rng()` calls as before — the daily parameters
   only scale and bias it. `utcDateNum(new Date())` is read once in
   `createGame` (boot), never re-read: a garden left open across midnight
   keeps its weather until reload, and the update loop stays
   wall-clock-free. Same date-derivation shape as Undertow's `dailySeed`
   (#110).
3. **Surface**: `weather`/`weatherDate` on `window.__gameState`, the
   `#hud` textContent + dataset, and `snapshot()` (a boot-time sim input
   belongs in the comparable core: two runs only claim equality if they
   shared the same day's weather); a small second HUD line on canvas.
   `sw.js` CACHE `drift-garden-v2` → `drift-garden-v3` (shipped shell
   change); CONCEPT.md growth line marked BUILT; README.md refreshed.
   New test API: `__game.weatherFor(dateNum)` — the pure derivation only;
   NO test-only date param in the product (the #110 discipline).
4. `tools/mobile-foundation-smoke/game-smoke.mjs` extended +4 assertions
   (11 → 15): `weather-pure-derivation` (same date stable, dates
   distinct, boot weather matches the derivation), `weather-fake-date-boot`
   (`Date` stubbed via `addInitScript` BEFORE load — the REAL boot path
   derives 20300607 → 'crosswind'), `weather-same-date-deterministic`
   (same fake date + same seed + same script → byte-identical 130-byte
   snapshots), `weather-changes-world` (a different fake date diverges
   the world on the SAME seed — weather names differ in the snapshots).
   Run twice against real Chromium: 15/15 PASS both runs, all
   deterministic assertion lines byte-identical (only live-page
   pause/resume wall-clock frame counts differ). Pre-change baseline
   11/11 and the untouched foundation smoke (`run.sh`) 6/6 also green.
5. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge (the designed born-red hold) + the
   pre-existing `claims-format` advisory on `order-005-scribe.md` (not
   this lane's file, advisory, left alone).
6. PR #111 opened READY immediately (no draft phase, no labels); no
   merge/approve/auto-merge calls from this session — the server-side
   enabler decides on green. Heartbeat appended to `control/status.md`
   (append-only dispatch section, PR number confirmed first).
7. Claim `control/claims/claude-drift-garden-weather.md` retired with
   this flip per the claims README (the open PR is the live claim from
   here).

## 💡 Session idea

**Side-band parameters get their own PRNG stream, keyed off the boundary
input — and "draw-count compatibility" becomes the API contract.** The
trap when adding a modifier system (weather, mutators, difficulty bands)
to a seeded game is feeding the new selection through the gameplay RNG:
every prior (seed → world) mapping silently shifts, and byte-level
determinism baselines go stale without any assertion failing for the
right reason. This cut keyed `weatherFor` to its own mulberry32 stream
(golden-ratio-mixed date) and kept the gameplay stream's draw sequence
IDENTICAL — `drawWind()` still consumes exactly two `rng()` calls — so
all 11 pre-existing assertions (including the seed-7 byte-identity
baseline) stayed meaningful and green with zero edits. Generalized: when
extending a deterministic system, state the invariant "the existing
stream's draw count and order are unchanged" in the diff, and prove it by
leaving the old determinism tests untouched — a modifier that needs to
reorder draws is a new world version, not a parameter cut.

## Previous-session review

- The Undertow daily-seed session (`.sessions/2026-07-14-undertow-daily.md`,
  PR #110) holds up completely as this slice's template: its idea card —
  "test the calendar by injecting the clock at the boundary, not by
  adding a test-only date param to the product" — was applied here
  verbatim (the smoke stubs `Date` via `addInitScript` and asserts the
  real boot path; the product ships zero date affordances), and its
  `dailySeed` YYYYMMDD derivation shape was reused for `utcDateNum` so
  the two games agree on what "today" means. One divergence worth
  naming: #110's daily≡seeded equivalence run has no direct analog here
  because weather is a parameter layer, not a seed — the analogous proof
  became same-date byte-identity plus cross-date divergence on the same
  seed. One small gap: #110's FakeDate init-script is now copy-pasted in
  two smokes; a third calendar-coupled cut should hoist it into a shared
  tools/ helper.
