# Session — Undertow growth cut 4: oxygen meter + air-pocket pickups

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/undertow-oxygen`, started from
  `date -u` at write time)
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

(in progress — flipped to `complete` in this branch's last commit)
