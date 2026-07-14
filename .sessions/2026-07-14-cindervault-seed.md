# Session — Cindervault growth cut 4: seed-select score-attack (the "daily seed" line, GBA-shaped)

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/cindervault-seed`, started 05:40Z,
  flipped 05:55Z — both from `date -u` at write time)
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

1. This card + `control/claims/claude-cindervault-seed.md`, born red
   (claim before build), cut from main @ `d60b08f` (PR #121 merged).
   PR #122 opened READY.
2. Toolchain certified before touching code: the unmodified tree rebuilt
   to the committed v0.4 dist hash `7bb878a8…` EXACTLY (pinned devkitARM
   via the leseratte10 mirror — walls quoted, not re-probed;
   preinstalled Python 3.11 mgba binding reused).
3. **The cut** (`games/cindervault/src/main.cpp`): the title's seed line
   becomes a dial — edge-triggered (one press = one step, so a
   challenge code is a repeatable press sequence), 32-bit wrapping,
   xorshift32's dead state 0 skipped in the dial's own direction, and
   the dial lives ONLY on the title (its keys are dungeon verbs
   everywhere else). `reset_run()` seeds from the dialed value, so
   START from win/lose reruns the SAME dialed vault. The death card —
   the score-attack card, endless runs end only there — gains
   `SEED <hex8>` (score attribution); the title's START line gains the
   dial hint (the pinned "PRESS START" prefix carries). Telemetry
   `cv[3]` now publishes the LIVE dial — boot value is the old
   constant, so every v0.4 pin carries and each dial step is watchable
   as its own literal (the #117 💡, applied wholesale).
4. **The mirror extended first** (`games/cindervault/tools/mirror.py`):
   `Sim` grew a `seed` parameter (`--seed` on the CLI; `--design`/
   `--trace`/`--keys`/`--verify` all seed-aware) and the verifier's
   word set grew `cv[3]` — the emulator's live seed word must now match
   the mirror's on every turn state of every certified route, strictly
   stronger than cut 3's verifier at zero cost to the old routes.
   Certified THIS build against the emulator: **0 mismatches across all
   453 turn states of the five routes** (lantern 33 + blade 42 +
   species 110 + endless 229 + dialed 39).
5. **The proofs** (`games/cindervault/proofs.sh`): +P6 THE DIALED VAULT
   — UP/RIGHT/R dial to `0xC1DF5FEE`, each step pinned in `cv[3]`
   (3252575981 -> +1 -> +0x100 -> +0x10000 = 3252641774), seed digits
   glyph-exact on the title; START dives the dialed vault and floor 1
   differs from the default seed's IN THE WORDS P2 PINS AT THE SAME
   FRAME (spawn (1,4) vs (6,1)) while every law carries — bump-2 kills,
   +25 embers, the wisp flit beat (hp flat on odd turn 31, -2 on even
   turn 32) — ending SLAIN at turn 38 with the death card naming its
   seed: score 220, attributable. +P7 dial away and back: every
   down-magnitude exercised, `cv[3]` returns to the boot constant, and
   the FULL P2 route lands on EVERY P2 literal — the dial is fully
   reversible (the Deepcast P5 precedent). **P1-P5 carried VERBATIM —
   zero re-derived pins.** **P2, P3, P4, P5, P6 each RUN TWICE:
   watch-log CSVs byte-identical.** Suite exit 0 (446 watch + 78 text
   asserts green).
6. `dist/cindervault.gba` v0.5 + README row: two clean builds
   byte-identical, 123,632 bytes, sha256 `0555a8c9e41a6c5e469eab83a3fe
   f094c4c524cbe9437275cf342c63daf911c0`; the proof suite re-ran green
   at these exact bytes. CONCEPT.md's daily-seed line marked **BUILT**
   with the leaderboard-scope note — every named Cindervault growth
   line except the art pass is now built.
7. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge (the designed born-red hold; telemetry
   `.substrate/guard-fires.jsonl` committed) + the pre-existing
   `claims-format` advisory on `order-005-scribe.md` (not this lane's
   file, advisory, left alone). Re-run green after the flip.
8. Heartbeat appended to `control/status.md` (append-only dispatch
   section, PR number confirmed first). No merge/approve/auto-merge
   calls from this session — the server-side enabler decides on green.
9. Claim `control/claims/claude-cindervault-seed.md` retired with this
   flip per the claims README (the open PR is the live claim from here).

## 💡 Session idea

**When a cut changes the run's CONTRACT, grow the verifier's word set in
the same commit — old routes re-certify the new dimension for free.**
This cut changed which stream a run draws from, so the honest question
became "is the seed word right at every moment of play?", not just at
the few frames a proof happens to assert. Adding `cv[3]` to the mirror
verifier's GAME_WORDS cost one tuple element and zero changes to the
four existing certified routes — but it silently upgraded ALL of them:
every one of the 453 turn states now re-proves that play never perturbs
the selected seed, which is exactly the invariant the feature's carry
argument leans on ("the dial only lives on the title"). The general
form: a mirror verifier's word set is a claim about WHICH state the
determinism contract covers, so every cut that adds a contract word
should extend the verifier before deriving pins — the old certified
routes then become regression witnesses for the new word at no design
cost. Composes with #117's 💡 (publish the choice in telemetry the
moment it exists): publish the new word first, then verify it
everywhere, then pin it somewhere.

## Previous-session review

- Prior slice: `.sessions/2026-07-14-deepcast-species.md` (PR #121,
  Deepcast species tables + catch log) — holds up: 16 named species on
  a side-band stream landed with the main stream's word order untouched
  and every prior pin verbatim, and its scope discipline (SRAM
  persistence named as follow-up rather than smuggled in) is the same
  reading this cut gave the leaderboard half of its concept line. The
  instructive contrast is HOW each cut kept the old pins: #121 added a
  new stream so the old one couldn't feel it (carry by construction);
  this cut re-aims the one existing stream, so the carry had to be
  proven behaviorally — the default is the old constant (P7's
  dial-away-and-back re-landing every P2 literal), exactly the
  #116-vs-#117 asymmetry the deepcast-daily card predicted would recur.
  Its 💡 (name the side-band seed constant in the proof header so the
  literal is greppable) transferred here as the dial-arithmetic header
  in proofs.sh P6/P7: the challenge seed and all three step magnitudes
  are spelled out where the pins live.
