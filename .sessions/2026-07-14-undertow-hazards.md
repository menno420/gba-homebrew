# Session — Undertow growth cut 5: jellyfish hazards in the channel

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/undertow-hazards`, started 06:59Z,
  flipped 07:11Z — both from `date -u` at write time)
- mission: build the LAST named growth cut from
  `games/web-undertow/CONCEPT.md` — "Hazards/creatures in the channel
  (jellyfish that drift on the seeded RNG)". Jellyfish spawn in the
  channel and drift deterministically; touching one ends the run like a
  crash (the game's grammar is one-touch death — walls and an empty tank
  both end the run, so a jellyfish sting does too, no damage system
  invented). This is a SIM change (the #123 precedent) on a THIRD
  side-band RNG stream, so the wall-channel layout AND the pocket layout
  per seed are unchanged; run outcomes for seeds where a jellyfish
  intersects the dive path legitimately move — that is the feature.
  Ghost records bumped v2 → v3 (same clean-drop pattern as #123's
  v1 → v2). Version v1.4.0 → v1.5.0. This completes Undertow's named
  growth path — all five cuts built.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-undertow-hazards.md`, born red
   (claim before build), cut from main @ `98b9b60`.
2. **Baseline first**: ran the v1.4.0 smoke before touching product code
   — 41/41 PASS, seed-7 no-input pinned at crashFrame=810 / 191m (air).
3. **Jellyfish hazards** (`games/web-undertow/game.js` v1.4.0 → v1.5.0):
   jellyfish spawn from row 40 on (per-row roll from a third side-band
   stream), drift horizontally on a bounded sine
   (`x0 + amp * sin(ph + t * JELLY_FREQ)`) of the sim instance's OWN
   step counter `s.t` — a pure function of row data + time, no per-frame
   RNG, no shared state; contact returns "sting" from `physStep`
   (same-frame ties resolve wall > sting > air); gameover reads
   "STUNG AT N m"; title hint line; dome-and-tentacles render reads
   `jellyX(row, sim.t)` — the renderer never owns creature state. New
   test API: `getJellies` (windowed positions at the live sim's t),
   `getJellyProbe` (nearest jelly below, the hunter-driver hook) — both
   probing-forces-generation-safe because rows are index-determined.
4. **Stream discipline**: jellies draw from
   `mulberry32(seed ^ JELLY_STREAM)`, owned per sim instance, a fixed
   FOUR draws per generated row whether or not a jelly spawns. The wall
   and pocket streams draw exactly what they drew in v1.4.0 → a seed's
   channel and pocket layout are byte-identical; only outcomes move.
   Sim-change witness: seed-3 no-input 650 / 148m (air) → 358 / 76m
   (sting); the seed-3 pocket-chaser 2241 / 724m / 48 pockets →
   1157 / 293m / 18 (sting). Carried verbatim (no jelly crosses these
   paths): seed-7 no-input 810 / 191m (air), hold-left seed-7 wall
   crash at frame 37 / 7m (carried since v1.3.0 — it crashes above the
   first jelly row), zero-pocket seeds 4/8 at 634 / 144m, daily seed
   20300607 at 458 / 100m.
5. **Ghost storage**: drift lives in the sim instance (`s.t` steps in
   `physStep`), so ghost replays stay exact by construction. Record
   version bumped 2 → 3: `loadGhostRec` accepts only v3, stale
   pre-jellyfish records are dropped cleanly (no ghost, no error) and
   the next crashed run writes a fresh v3 record. `CONCEPT.md` hazards
   line marked BUILT and the doc's status line weathered to "growth
   path complete: all five named cuts BUILT"; `HOSTING.md` records the
   v3 versioning.
6. `tools/web-smoke-undertow.mjs` extended 41 → 48 assertions: jellies
   spawn rows 40+ with bounded amplitude; same seed → byte-identical
   positions at frame 300, twice; drift is real and bounded (moved by
   frame 390, never beyond anchor ± amp); a greedy jelly-hunter dies
   STUNG with air left in the tank (42.13 — the sting, not the tank or
   a wall, ended the run), identical across two driven runs (413 / 89m
   twice); seed-3 sim-change witness pinned; the carried v1.4.0 fixed
   points asserted as the layout-identity witnesses; stale v2 ghost
   dropped + fresh v3 written. Run twice against real Chromium: 48/48
   PASS both runs, outputs byte-identical (`diff` empty). All prior
   daily/share/skins/ghost/oxygen assertions pass at the carried or
   re-derived baselines.
7. `python3 bootstrap.py check --strict` pre-flip: the designed born-red
   hold on this card + the pre-existing `claims-format` advisory on
   `order-005-scribe.md` (not this lane's file, advisory, left alone).
   `.substrate/guard-fires.jsonl` telemetry committed.
8. PR #126 opened READY immediately (no draft phase); no
   merge/approve/auto-merge calls from this session — the server-side
   enabler decides on green. Heartbeat appended to `control/status.md`
   (append-only dispatch section, PR number confirmed first).
9. Claim `control/claims/claude-undertow-hazards.md` retired with this
   flip per the claims README (the open PR is the live claim from here).

## 💡 Session idea

**Give a drifting creature a clock, not a velocity — position as a pure
function of (row data, t) makes "moving hazard" as cheap to prove as
"static pickup".** The tempting implementation is stateful: store each
jelly's x, nudge it every frame, worry about who steps it (live sim?
ghost sim? render?) and when. The sine form `x0 + amp*sin(ph + t*f)`
dissolves all of that: the side-band stream draws the parameters ONCE at
row generation (keeping the fixed-draws-per-row discipline), the sim
instance's step counter is the only mutable piece, and it already had to
exist per-instance for ghost fidelity. Every consumer — live collision,
ghost collision, renderer, test probe — evaluates the same closed-form
expression and cannot disagree. The smoke's "same positions at frame N,
twice" assertion is then a one-liner instead of a state-replay harness,
and the drift-boundedness proof (|x - x0| <= amp) is arithmetic on the
stored parameters rather than an induction over frames. When a feature
wants motion, ask first whether it can be a closed-form read of time
before giving it state.

## Previous-session review

- The Deepcast line-upgrades session (PR #125, `control/status.md`
  dispatch entry): its "fresh cart loads as no save = v0.4 bit-exact —
  the whole committed suite ran green unmodified before any new pin was
  written" is the same baseline-first habit this session used (41/41 on
  main before touching game.js), and its two-savefile run-twice
  byte-identity is the same two-run discipline this smoke's `diff` step
  uses. Difference in kind worth noting: #125 could keep EVERY old pin
  because its mechanic hides behind a save-gate; a channel hazard has no
  gate, so this session had to split the fixed point (#123's pattern) —
  carry the witnesses the feature provably cannot reach, re-derive the
  seeds it legitimately moved, and say which is which in the PR. No
  erratum found in #125's entry to carry.
