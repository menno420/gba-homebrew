# Session — Deepcast growth cut 2: seed-select score-attack (the "daily seed" line, GBA-shaped)

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/deepcast-daily`, started 04:00Z,
  flipped 04:04Z — both from `date -u` at write time)
- mission: build Deepcast's next named growth cut from
  `games/deepcast/CONCEPT.md` — "Daily seed + score-attack: the
  deterministic core makes a shareable 'same lake, same fish' challenge
  free to build" — as the GBA-honest version of "daily": there is no
  clock and no server on a cartridge, so the challenge mechanism is a
  **title-screen seed picker** (UP/DOWN dials the seed +-1, LEFT/RIGHT
  +-0x100, L/R +-0x10000; two players who dial the same 8 hex digits
  fish the SAME lake — same bites, same weights, same surge rhythm)
  with the seed shown on the title AND on the dusk score card, so any
  score is attributable to its seed. The default boot seed stays
  `0xDEE9CA57` untouched — every existing proof pin carries. The
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

1. This card + `control/claims/claude-deepcast-daily.md`, born red
   (claim before build), cut from main @ `3ca91fc` (PR #116 merged).
   PR #117 opened READY.
2. Toolchain certified before touching code: devkitARM r68 + Butano
   21.7.1 already present (leseratte10 mirror pin), mgba binding under
   Python 3.11 imported clean; main's unmodified tree rebuilt to the
   committed `dist/deepcast.gba` v0.2 hash `f5e33083…` EXACTLY.
3. **THE cut** (`games/deepcast/src/main.cpp`): the title's seed line
   becomes a dial — edge-triggered (one press = one step, so a
   challenge code is a repeatable press sequence), 32-bit wrapping,
   xorshift32's dead state 0 skipped in the dial's own direction.
   `reset_run()` seeds from the dialed value; START from game over
   reruns the SAME dialed lake; the picker only lives on the title.
   The dusk card gains `SEED <hex8>` (score attribution), the title
   gains the dial hint appended to the mute line (the pinned "B: MUTE"
   prefix carries). Telemetry `dc[4]` now publishes the SELECTED seed —
   boot value unchanged so all v0.2 pins carry, and each dial step is
   watchable as its own literal.
4. **The proofs** (`games/deepcast/proofs.sh`): P1/P2/P3 carried with
   two additive pins (hint line, default-lake dusk seed line). +P4 the
   dialed lake: UP/RIGHT/R to `0xDEEACB58`, each step pinned in dc[4]
   (3739863639 -> +1 -> +0x100 -> +0x10000 = 3739929432), seed digits
   glyph-exact on the title; SAME charge input as P2's first cast but
   the world differs — still sinking at frame 471 (the default lake's
   pinned bite frame), bites at 489, and a later 72m cast pulls a wt-16
   fish where the default stream gave 11; four casts to dusk (score 6 /
   catches 1, ledger 21 clicks / 4 bites / 1 catch / 3 snaps), dusk
   card names the seed; audio laws carry to any seed (first reeling
   frame clicks at interval 16, cues mixer-voiced). **RUN TWICE:
   watch-logs byte-identical.** +P5 dial away and back: every
   down-magnitude exercised, dial returns to `0xDEE9CA57`, and the FULL
   P2 route lands on EVERY P2 literal — the dial is fully reversible.
   212 distinct asserts, 500 green assert lines, exit 0.
5. `dist/deepcast.gba` v0.3 + README row: two clean builds
   byte-identical, 132,700 bytes, sha256 `2a7010697667132b2c89a24af1
   e1ba7f521d80dc0225df185aebadba9e88e457`; the proof suite re-ran
   green at these exact bytes. CONCEPT.md's daily-seed line marked
   **BUILT** with the leaderboard-scope note.
6. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold; telemetry
   `.substrate/guard-fires.jsonl` committed) + the pre-existing
   `claims-format` advisory on `order-005-scribe.md` (not this lane's
   file, advisory, left alone).
7. Heartbeat appended to `control/status.md` (append-only dispatch
   section, PR number confirmed first). No merge/approve/auto-merge
   calls from this session — the server-side enabler decides on green.
8. Claim `control/claims/claude-deepcast-daily.md` retired with this
   flip per the claims README (the open PR is the live claim from here).

## 💡 Session idea

**Publish the player's CHOICE in telemetry the moment it exists, not
when it takes effect.** The obvious seed-select design keeps the dialed
value in a local until START commits it into the RNG — provable only
indirectly, one whole run per dial step. Publishing the live dial in
`dc[4]` every frame made each keypress its own one-frame literal
(+1, +0x100, +0x10000 pinned at frames 153/157/200 with zero gameplay),
turned the reversibility proof into four cheap asserts, and cost
nothing because the boot value is the old constant — the pins that
pinned a constant now pin a default without changing a byte of their
text. Generalized: any pre-commit UI state (menu cursor, selected
loadout, dialed seed) that will later shape a whole run deserves a
telemetry word of its own — the choice becomes assertable at the
moment of choosing, and the "does the choice leave a mark after being
un-chosen?" question becomes a direct watch instead of a full-run
counterfactual.

## Previous-session review

- Prior slice: PR #116 (Cindervault named species, growth cut 2). Its
  discipline transferred directly: certify the toolchain against the
  committed dist hash before touching code, derive every pin from an
  observed run, run the new route twice and `cmp` inside the committed
  script, and mark the concept line BUILT in the same PR. The
  instructive difference is which way the RNG contract bent: #116's
  species consume NO RNG (pure function of the floor) so its spawn pins
  carried by construction; this cut changes WHICH stream the whole run
  draws from, so the carry had to be proven the other way — not "the
  feature consumes nothing" but "the feature's default is the old
  constant" (P5's dial-away-and-back run re-landing every P2 literal is
  that proof made mechanical). Same convention, opposite burden — worth
  naming because the next seed-adjacent cut (line upgrades bought with
  score) will face the #112 version: new consumption, re-derived pins.
