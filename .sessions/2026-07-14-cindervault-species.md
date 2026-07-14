# Session — Cindervault growth cut 2: named monster species per depth

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/cindervault-species`, started 03:13Z,
  flipped 03:44Z — both from `date -u` at write time)
- mission: build Cindervault's next named growth cut from
  `games/cindervault/CONCEPT.md` — "Named monster species per depth with
  distinct chase quirks (the greedy step is a plug-in policy)" — with the
  growth-cut-1 de-risker: extend the committed host mirror FIRST, certify
  mirror-vs-emulator 0-mismatch on every route to be pinned, design and
  balance-check routes host-side, THEN transcribe pins into
  `games/cindervault/proofs.sh`.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-cindervault-species.md`, born red
   (claim before build), cut from main @ `a5fec7a` (PR #115 merged).
   PR #116 opened READY.
2. Toolchain certified before touching code: the unmodified tree rebuilt
   to the committed v0.2 dist hash `b53421ba…` EXACTLY (pinned devkitARM
   r68 via the leseratte10 mirror — repo recipe quoted, walls not
   re-probed; preinstalled Python 3.11 mgba binding reused).
3. **Species** (`games/cindervault/src/main.cpp`): one named species per
   depth, each ONE quirk wrapped around the same baseline greedy step —
   the concept's pre-declared plug-in point made literal in
   `monster_phase()`: floor 1 CINDER RAT (baseline, unchanged), floor 2
   SOOT WISP (flits out on odd turn counts — no bite, no step: half-rate
   close and drain), floor 3 ASH HOUND (pack discipline: at most one
   hound bites per phase, slot order), floor 4 VAULT WRAITH (cold grasp:
   axis-aligned at range 2 it bites INSTEAD of stepping), floor 5 HOARD
   SENTINEL (holds its post until the player crosses its leash of 4).
   Species consume NO RNG — pure function of the floor number — so floor
   generation is byte-identical to cut 1 and every fixed-seed spawn pin
   carried. Telemetry unchanged (18 words); the depth's name renders on
   its own HUD line ("FOE ...", ui slot 2, inside the 240px clip).
4. **The mirror extended first** (`games/cindervault/tools/mirror.py`):
   species policies, `--baseline` counterfactual flag (all-rat world —
   for gap stats, never pins), grasp/bite-ring helpers, a callable-avoid
   `drive()`, and the `--design species` route designer (matador
   micro-tactics: never stand on a grasp line, wait at diagonal-2 so the
   closer must step in — moving monsters cannot bite — bump on arrival).
   Certified THIS build against the emulator: **0 mismatches across all
   185 turn states** of the three pinned routes (lantern 33 + blade 42 +
   species 110). Winnability re-checked host-side and on-device: WIN at
   turn 109, hp 3, torch 262, score 1032.
5. **The proofs** (`games/cindervault/proofs.sh`): new P4 THE SPECIES —
   one 109-input descent past every species, one arithmetic witness per
   quirk (wisp: adjacent flit turn bites nothing, first blood exactly on
   the even beat; hound: two adjacent both pinned turns yet exactly
   -1/turn; wraith: first blood at range 2 on a pinned tile one turn
   earlier than any one-step chaser could reach adjacency; sentinel:
   eight pinned turns hp+alive frozen while the torch proves the turns,
   the fourth guard still standing as the vault opens), ending in the
   open vault. P1/P2 carried verbatim (floor 1 = the rat = the baseline;
   the carried pins ARE its witness) + a `FOE CINDER RAT` text pin; P3's
   two hp pins legitimately shifted (turn 38 hp 6 -> 9, end HUD HP 5 ->
   8 — wisps flit, hounds bite as a pack of one) and were re-derived via
   the re-certified mirror, annotated inline. **P2, P3, P4 each RUN
   TWICE: watch-log CSVs byte-identical (`cmp` inside the script).**
   Suite exit 0, 283 green lines. Counterfactual quoted in the header:
   the SAME 109 inputs all-rat are first-bitten at turn 17 (vs 22) and
   die SLAIN on floor 3 at turn 39.
6. `dist/cindervault.gba` v0.3 + README row: two clean builds
   byte-identical, 122,652 bytes, sha256 `f82781e71c3bdc24d4988db2d65ba
   345f7431a3dc714adfcf83ed92f5c931da5`; the proof suite ran at these
   exact bytes. CONCEPT.md growth line marked **BUILT**.
7. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge (the designed born-red hold) + the
   pre-existing `claims-format` advisory on `order-005-scribe.md` (not
   this lane's file, advisory, left alone). Re-run green after the flip.
8. Heartbeat appended to `control/status.md` (append-only dispatch
   section, PR number confirmed first). No merge/approve/auto-merge
   calls from this session — the server-side enabler decides on green.
9. Claim `control/claims/claude-cindervault-species.md` retired with
   this flip per the claims README (the open PR is the live claim from
   here).

## 💡 Session idea

**Prove a movement policy through the bite clock: every chase quirk is a
first-blood arithmetic statement.** Cindervault's telemetry has no
per-monster positions, and adding them to prove "this species moves
differently" would widen the surface for one cut. But every chase policy
leaves a signature in WHEN damage can first land on a fixed input script:
the wisp's half-rate close pushes first blood from turn 17 to 22 and
snaps it to the even beat; the hound's pack cap turns two adjacent
monsters into a -1/turn staircase instead of -2; the wraith's range-2
grasp lands blood a turn EARLIER than adjacency arithmetic permits; the
sentinel's leash makes hp provably constant across eight pinned turns
that the burning torch proves are real. Generalized from #112's boundary
idea: a policy on hidden state is checkable through the reachability
boundary it imposes on the OBSERVABLE damage clock — pin the earliest/
latest turn a counter CAN move under the claimed policy, plus a
`--baseline` counterfactual replay to show the boundary actually moved.
The mirror makes deriving both sides cheap.

## Previous-session review

- Prior slice: `.sessions/2026-07-14-drift-garden-species.md` (PR #115,
  Drift Garden species) — holds up, and its 💡 card ("rare = a reachable
  composition of common things, not a low-probability draw; tests get
  equalities instead of distributions") transferred here as the species
  assignment rule: where #115 rolled species on a side-band stream,
  Cindervault could go further — species is a ZERO-draw pure function of
  depth, so there was no stream to sequence and no distribution to test,
  only equalities at pinned turns. Same discipline, stronger corner.
  #115's other pattern (pre-existing assertions green with zero edits as
  the blast-radius proof) mapped here to P1/P2 carrying verbatim — with
  the honest exception documented: two P3 hp pins SHOULD move when
  monsters bite less, and a determinism suite that stayed fully green
  through a behavior change would itself have been the red flag.
