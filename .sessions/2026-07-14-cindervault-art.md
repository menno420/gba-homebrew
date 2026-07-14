# Session — Cindervault growth cut 5: the art pass (tile sprites + torch-radius light fade)

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/cindervault-art`, started 07:20Z from
  `date -u` at write time)
- mission: build Cindervault's LAST named growth cut from
  `games/cindervault/CONCEPT.md` — "Real art pass: tile sprites for
  walls/floor, a torch-radius light fade (the mechanic made diegetic —
  the screen literally darkens as the torch burns down)". Presentation
  ONLY: game state, RNG word order and every `cv_telemetry` word are
  untouched, so ALL existing proofs must pass verbatim. The light fade
  reuses the repo's shared `games/common/include/gl_light_window.h`
  (Lumen Drift's hardware-window light circle); the tiles follow Lumen
  Drift's dynamic regular-bg idiom with original procedurally-generated
  indexed-BMP assets. This completes Cindervault's named growth path.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

(in progress — flipped to `complete` with the final commit of the
branch; the badge holds the substrate gate red until then, by design)
