# Session — Deepcast growth cut 2: seed-select score-attack (the "daily seed" line, GBA-shaped)

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/deepcast-daily`, started 04:00Z from
  `date -u` at write time)
- mission: build Deepcast's next named growth cut from
  `games/deepcast/CONCEPT.md` — "Daily seed + score-attack: the
  deterministic core makes a shareable 'same lake, same fish' challenge
  free to build" — as the GBA-honest version of "daily": there is no
  clock and no server on a cartridge, so the challenge mechanism is a
  **title-screen seed picker** (L/R/dpad dials the visible seed number;
  two players who dial the same seed fish the SAME lake — same bites,
  same weights, same surge rhythm) with the seed shown on the title AND
  on the dusk score card, so any score is attributable to its seed. The
  default boot seed stays `0xDEE9CA57` untouched — every existing proof
  pin carries. The leaderboard half of the concept line follows the
  Tiltstone PR #97 precedent: the seed/challenge mechanism ships; the
  leaderboard itself is out of GBA scope (no network), noted in
  CONCEPT.md when the line is marked BUILT.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## Plan

1. This card + `control/claims/claude-deepcast-daily.md`, born red
   (claim before build), cut from main @ `3ca91fc` (PR #116 merged).
2. Certify the toolchain by rebuilding main's unmodified tree to the
   committed `dist/deepcast.gba` v0.2 hash `f5e33083…` exactly.
3. The cut: seed picker on the title (UP/DOWN ±1, LEFT/RIGHT ±0x100,
   L/R ±0x10000, wrapping, the xorshift dead state 0 skipped), the
   active seed published in telemetry, seed hex on the dusk card.
4. Extend `games/deepcast/proofs.sh`: dialed-seed route (glyph-exact
   seed digits on the title, telemetry-verified different world, RUN
   TWICE byte-identical) + dial-away-and-back route that must land on
   every default-run literal.
5. dist v0.3 (two clean builds byte-identical, proofs re-run at dist
   bytes), CONCEPT.md line marked BUILT, heartbeat, flip.
