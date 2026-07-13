# Session 50 — game-lab Track B

> **Status:** `in-progress`

- date: 2026-07-13 (branch `claude/shoal-audio`, started 21:58Z)
- mission: **SHOAL growth rung 5 — AUDIO, the LAST named cut.** The
  final item of `games/shoal/CONCEPT.md` @ main `751341c`
  ("Deliberately still cut: audio"). Synthesized-only per the
  repo-wide rule, via the proven Lumen-Drift pipeline
  (`audio/generate_audio.py` deterministic WAVs -> maxmod soundbank ->
  `bn::sound_items` + `gl_audio_hook` counters + mixer-memory voicing
  evidence, `docs/capabilities.md`). Pure decision layer — nothing
  feeds back into the sim.
- binding budget constraint: L4's worst frame is 82.3% (3369/4096);
  the FULL P7 chain is measured before/after and the `t[5] < 4096`
  rail must hold everywhere — the mixer scopes DOWN before the rail
  gives.
- dedup at claim time: no shoal claim, no open Shoal PR; inbox at
  HEAD unchanged — nothing new unserved.
- landing posture: PR opened READY on main immediately after this
  born-red commit; no merge/approve/auto-merge calls from this
  session — the server-side enabler decides on green.

(Body lands at the flip. substrate-gate holds red on this badge until
then — the designed born-red hold; nothing to investigate.)
