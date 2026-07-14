# Session — Cindervault growth cut 4: seed-select score-attack (the "daily seed" line, GBA-shaped)

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/cindervault-seed`, started 05:40Z —
  from `date -u` at write time)
- mission: build Cindervault's next named growth cut from
  `games/cindervault/CONCEPT.md` — "Daily seed + score-attack
  leaderboard: the deterministic core makes 'same vault, same monsters'
  a shareable challenge for free" — as the GBA-honest version of
  "daily", mirroring the Deepcast PR #117 precedent exactly in spirit:
  a cartridge has no clock and no server, so the challenge mechanism is
  a **title-screen seed dial** (UP/DOWN +-1, LEFT/RIGHT +-0x100, L/R
  +-0x10000 — none of these clash with the title's only existing input,
  START; edge-triggered; the seed rendered live on the title). Two
  players who dial the same 8 hex digits dive the SAME vault — same
  floors, same monsters, same embers. The default boot seed stays
  `0xC1DE5EED` untouched, so every existing proof pin carries. The
  leaderboard half of the concept line follows the Tiltstone PR #97
  precedent: the seed/challenge mechanism ships; the leaderboard itself
  is out of GBA scope (no network), noted in CONCEPT.md at the BUILT
  mark.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

(build in progress — this card holds the substrate gate red until the
closing flip, by design: claim before build)
