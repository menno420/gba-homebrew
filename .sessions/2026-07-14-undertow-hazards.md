# Session — Undertow growth cut 5: jellyfish hazards in the channel

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/undertow-hazards`, started 06:59Z from
  `date -u` at write time)
- mission: build the LAST named growth cut from
  `games/web-undertow/CONCEPT.md` — "Hazards/creatures in the channel
  (jellyfish that drift on the seeded RNG)". Jellyfish spawn in the
  channel and drift deterministically; touching one ends the run like a
  crash (the game's grammar is one-touch death — walls and an empty tank
  both end the run, so a jellyfish sting does too). This is a SIM change
  (the #123 precedent): jellyfish draw from a side-band RNG stream
  (`seed ^ const`, fixed draw count per generated row) so the wall-channel
  layout per seed is UNCHANGED; run outcomes for seeds where a jellyfish
  intersects the dive path legitimately move — that is the feature.
  Ghost records bump v2 → v3 (same clean-drop pattern as #123's v1 → v2:
  pre-jellyfish timelines replay to a different outcome, so stale records
  are dropped on load, no ghost, no error). Version v1.4.0 → v1.5.0.
  This completes Undertow's named growth path.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: (to be filled at flip)

## Plan

1. This card + `control/claims/claude-undertow-hazards.md`, born red
   (claim before build).
2. Baseline first: run the v1.4.0 smoke before touching product code.
3. Jellyfish sim mechanic on a new side-band stream; ghost record v3;
   HUD/render; CONCEPT.md line marked BUILT (growth path complete).
4. Smoke extended: spawn/drift determinism, sting kills, layout-identity
   witness carried, all prior cuts green, run twice byte-identical.
5. `python3 bootstrap.py check --strict`; PR opened READY; status.md
   heartbeat; flip this card last.
