# Session — Undertow growth cut 4: oxygen meter + air-pocket pickups

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/undertow-oxygen`, started 06:00Z,
  flipped 06:13Z — both from `date -u` at write time)
- mission: build the next named growth cut from
  `games/web-undertow/CONCEPT.md` — "Oxygen meter + air-pocket pickups
  (adds a reason to leave the safe line)". An oxygen meter that depletes
  as you dive; air-pocket pickups spawned in the channel from the seeded
  RNG; running out of oxygen ends the run like a crash. This is a NEW
  SIM MECHANIC: it legitimately changes run outcomes for a given seed.
  HARD CONSTRAINTS: pickups draw from a side-band RNG stream derived
  from the seed so the channel layout for a given seed is IDENTICAL to
  v1.3.0; oxygen lives inside the sim instance so ghost replay fidelity
  is preserved by construction; stored v1 ghost records (recorded under
  the oxygen-free sim) are dropped cleanly via a record-version bump.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-undertow-oxygen.md`, born red
   (claim before build), cut from main @ `2137e48`.
2. **Baseline first**: ran the v1.3.0 smoke before touching product code
   — 31/31 PASS, seed-7 fixed point pinned at crashFrame=823 / 194m.
3. **Oxygen + air pockets** (`games/web-undertow/game.js` v1.3.0 →
   v1.4.0): the tank (100 units) drains every frame, faster with depth
   (pressure); air pockets spawn inside the channel from
   `POCKET_START_ROW` on and refill 40 units on contact (pickups NEVER
   alter motion — pure oxygen); an empty tank ends the run like a crash
   (`physStep` now returns "wall" / "air" / null; gameover reads
   "OUT OF AIR AT N m"). HUD oxygen bar (red under 25%), pocket pickups
   drawn from sim row data, title hint line. New test API: `getOxygen`,
   `getCrashCause`, `getPocketProbe` (driver hook — probing forces row
   generation, which is index-determined and so sim-inert).
4. **Stream discipline**: pockets draw from a side-band RNG
   (`mulberry32(seed ^ POCKET_STREAM)`, owned per sim instance), a fixed
   two draws per generated row whether or not a pocket spawns. The wall
   stream draws exactly what it drew in v1.3.0 → a seed's channel layout
   is byte-identical; only OUTCOMES move. Seed-7 no-input baseline:
   crashFrame 823 / 194m (wall) → 810 / 191m (out of air, one
   accidental pickup). The hold-left seed-7 wall crash at frame 37 / 7m
   carries verbatim — asserted as the layout-identity witness.
5. **Ghost storage**: oxygen/pockets live in `makeSim`, so ghost replays
   stay exact by construction (the recorded timeline replays through the
   same `physStep`, air deaths included). Record version bumped 1 → 2:
   `loadGhostRec` accepts only v2, stale pre-oxygen records are dropped
   cleanly (no ghost, no error) and the next crashed run writes a fresh
   v2 record. `CONCEPT.md` oxygen line marked BUILT; `HOSTING.md` notes
   the record versioning.
6. `tools/web-smoke-undertow.mjs` extended 31 → 41 assertions: tank
   full/drains/deterministic at frame 240; oxygen-out ends the run at
   the re-derived seed-7 baseline; two dev-verified zero-pocket seeds
   (4, 8) die of air on the IDENTICAL frame 634 — the drain schedule is
   seed-independent, a literal-free fixed point; a collected pocket
   extends the run (810 > 634); a greedy pocket-chaser witnesses an
   in-run refill (79.17 → 99.87), collects 48 pockets and reaches 724m
   vs 148m no-input at seed 3 ("a reason to leave the safe line"),
   deterministic across two driven runs; stale v1 ghost dropped + fresh
   v2 written. Run twice against real Chromium: 41/41 PASS both runs,
   outputs byte-identical (`diff` empty). All prior daily/share/skins/
   ghost assertions pass at the new baseline.
7. `python3 bootstrap.py check --strict` pre-flip: the designed born-red
   hold on this card + the pre-existing `claims-format` advisory on
   `order-005-scribe.md` (not this lane's file, advisory, left alone).
   `.substrate/guard-fires.jsonl` telemetry committed.
8. PR #123 opened READY immediately (no draft phase); no
   merge/approve/auto-merge calls from this session — the server-side
   enabler decides on green. Heartbeat appended to `control/status.md`
   (append-only dispatch section, PR number confirmed first).
9. Claim `control/claims/claude-undertow-oxygen.md` retired with this
   flip per the claims README (the open PR is the live claim from here).

## 💡 Session idea

**A sim-changing feature and a sim-preserving feature need different
proofs — split the fixed point instead of abandoning it.** The easy
readings were wrong in both directions: "outcomes changed, so the old
baseline is useless" throws away the only witness that the walls didn't
move; "keep all the old literals" is impossible when the feature's
whole job is to change outcomes. The split: give the new mechanic its
own RNG stream with a FIXED draw count per unit of world-gen, then
carry forward one old literal that the new mechanic provably cannot
touch (here: a wall crash at frame 37 — before any drain matters,
reachable without crossing a pocket) and re-derive the rest. The
carried literal proves what stayed identical; the re-derived ones
document what legitimately moved (823/194m → 810/191m, old → new, in
the PR). Bonus form of the same idea: when a new constant would be a
magic number in a test (the pure-drain death frame), don't hardcode it
— assert two independent seeds land on it TOGETHER, which pins the
property (seed-independence) rather than the value.

## Previous-session review

- The Cindervault seed-select session (PR #122, `control/status.md`
  dispatch entry): kept the boot seed 0xC1DE5EED untouched so every
  v0.4 proof pin carried while the dial changed only opt-in runs — the
  inverse discipline of this card (there the default path was
  preserved and proven; here the default path legitimately moved, so
  the carried witness had to be an outcome the feature cannot reach);
  its run-twice byte-identical watch-log habit is the same two-run
  discipline this smoke uses; no erratum found to carry.
