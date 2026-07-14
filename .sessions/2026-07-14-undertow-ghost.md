# Session — Undertow growth cut 3: ghost replays of your best run

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/undertow-ghost`, started 04:10Z from
  `date -u` at write time)
- mission: build the next named growth cut from
  `games/web-undertow/CONCEPT.md` — "Ghost replays of your best run —
  nearly free because the sim is deterministic: store the input
  timeline, replay it against the same seed". Record every run's input
  timeline; persist the best run per seed to guarded `localStorage` as a
  compact input log; on later runs of the SAME seed, a translucent ghost
  diver replays it in lockstep — a second sim instance fed the stored
  inputs. HARD CONSTRAINTS: the live sim must be byte-identical with the
  ghost on vs off (same crashFrame/depth), and a stored timeline must
  replay to the original run's depth exactly — both asserted by the
  smoke.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

(in progress — flipped at session close)
