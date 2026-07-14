# Session — Cindervault growth cut 3: endless depth banding

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/cindervault-endless`, started 04:41Z,
  flipped 05:09Z — both from `date -u` at write time)
- mission: build Cindervault's next named growth cut from
  `games/cindervault/CONCEPT.md` — "Depth banding past floor 5 for an
  endless mode; the vault becomes the first milestone instead of the
  end" — with the established de-risker: extend the committed host
  mirror FIRST, certify mirror-vs-emulator 0-mismatch on every route to
  be pinned, design and balance-check routes host-side, THEN transcribe
  pins into `games/cindervault/proofs.sh`.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-cindervault-endless.md`, born red
   (claim before build), cut from main @ `b778d39` (PR #119 merged).
   PR #120 opened READY.
2. Toolchain certified before touching code: the unmodified tree rebuilt
   to the committed v0.3 dist hash `f82781e7…` EXACTLY (pinned devkitARM
   r68 via the leseratte10 mirror — walls quoted, not re-probed;
   preinstalled Python 3.11 mgba binding reused).
3. **The cut** (`games/cindervault/src/main.cpp`): SELECT on the VAULT
   REACHED screen delves deeper — the win stands as the first milestone
   (screen, banked score, +torch bonus at that moment, START restart all
   unchanged) and the SAME run continues onto floor 6 from the RNG state
   the vault left behind; hp/torch/items/kills/turn count carry, the
   +torch bonus drops back out of the live score (the concept scores it
   only "on a win" — the price of not stopping). Past floor 5 the
   generator reads a depth-band table — (monsters, bigs, embers, carve)
   per 3 floors, {4,1,2,220} up to {8,4,1,140} — and species recur on
   the five-depth cycle ((floor-1) % 5, the identity on floors 1-5). No
   second win: endless ends only in DARKNESS or SLAIN — score-attack at
   100/floor. Floors 1-5 read the same tables as before, so a run that
   never presses SELECT is bit-identical to growth cut 2.
4. **The mirror extended first** (`games/cindervault/tools/mirror.py`):
   DEPTH_BANDS + floor_params(), the species cycle, the 'S' continue
   edge, choke_fight()/sweep_embers() bot helpers, and a deterministic
   per-floor beam searcher (beam_descend()) for floors where every
   greedy policy dies. verify() upgraded to full state-SEQUENCE matching
   (a continue changes state at the same turn count, so per-turn keying
   stopped being unique — the new check is strictly stronger). Certified
   THIS build against the emulator: **0 mismatches across all 414 turn
   states of the four routes** (lantern 33 + blade 42 + species 110 +
   endless 229).
5. **The proofs** (`games/cindervault/proofs.sh`): new P5 THE DESCENT —
   228 inputs = P4's route byte-for-byte (the vault opens at turn 109
   exactly as pinned, and the win screen now offers the delve) + SELECT
   + a 118-input deep leg. Witnesses: the continue (floor 6 at turn
   count 109, everything carried, score 1032 -> 770 = EXACTLY the 262
   torch bonus), band-1 monster mix (cv[14]=4 on floors 6/7/8, all
   twelve killed at entry chokes at zero hp cost), band-1 ember economy
   (exactly-2 sweeps at pinned turns), species recurrence (five FOE
   lines up the cycle), the item two-step at depth, the band-2 boundary
   (cv[14]=5 on floors 9/10), and the wraith toll — the beam-search
   OPTIMAL flee line still pays exactly 2 hp — ending alive on floor 10
   at hp 1 among frozen sentinels, score 1495, no torch bonus. bigs and
   carve pinned structurally through the RNG word order (declared in
   the header). **P1-P4 carried VERBATIM — zero re-derived pins.**
   **P2, P3, P4, P5 each RUN TWICE: watch-log CSVs byte-identical.**
   Suite exit 0 (349 watch + 56 text asserts green).
6. `dist/cindervault.gba` v0.4 + README row: two clean builds
   byte-identical, 123,172 bytes, sha256 `7bb878a834b8c4136ec7298a7ed12
   a610ce9227d6f28b622590c9ef720bdd4a0`; the proof suite ran at these
   exact bytes. CONCEPT.md growth line marked **BUILT** (the daily-seed
   + leaderboard line stays unbuilt; leaderboards out of GBA scope per
   the Tiltstone #97 precedent).
7. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge (the designed born-red hold) + the
   pre-existing `claims-format` advisory on `order-005-scribe.md` (not
   this lane's file, advisory, left alone). Re-run green after the flip.
8. Heartbeat appended to `control/status.md` (append-only dispatch
   section, PR number confirmed first). No merge/approve/auto-merge
   calls from this session — the server-side enabler decides on green.
9. Claim `control/claims/claude-cindervault-endless.md` retired with
   this flip per the claims README (the open PR is the live claim from
   here).

## 💡 Session idea

**Prove balance as a bound, not an anecdote: when the claim is "this is
survivable but costly", pin the OPTIMUM.** A hand-played (or greedy-bot)
route that loses 2 hp on the wraith floor only proves "some line loses
2"; the interesting design claim — the band is fair but it will bleed
you — needs "NO line loses less than 2". Because the committed mirror is
a cheap, exact copy of the sim, a beam search over it turns the first
statement into the second at design time, and the searched line then
gets transcribed and pinned like any other route: the pin stops being a
sample of play and becomes a property of the game. Same trick
generalizes to any single-resource clock (fewest turns, least torch,
earliest kill): search the mirror for the extremal route, pin the
extremum, and quote the counterfactual ("every greedy policy the mirror
has dies here") in the proof header. Composes with #116's idea — there
the policy was checked through the damage clock's reachability boundary;
here the TUNING is checked through the optimum of the same clock.

## Previous-session review

- Prior slice: `.sessions/2026-07-14-drift-garden-essence.md` (PR #119,
  Drift Garden essence spending) — holds up: wallet + palette shop
  landed render-only with the sim byte-untouched, 27/27 smoke twice, and
  its discipline ("of the doc's spend targets, ship the contained
  reading and name the follow-up") transferred here as the win-screen
  reading: of the ways "endless" could meet "the vault becomes the first
  milestone", the contained one keeps the win intact and grafts the
  continue onto the exact state the old proofs already pin — P1-P4
  carried verbatim the same way #119's seed-7 snapshots stayed
  byte-identical with all palettes unlocked. Its 💡 (spend-gates proven
  by deny/buy/re-buy equalities) maps to P5's score step: the -262 drop
  at the continue is the same "the ledger balances exactly" assertion,
  one formula deeper in the game.
