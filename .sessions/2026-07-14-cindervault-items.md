# Session — Cindervault growth cut 1: the item layer + the committed-proofs port

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/cindervault-items`, started 02:01:30Z,
  flipped 02:18Z — both from `date -u` at write time)
- mission: build Cindervault's lowest-risk named growth cut from
  `games/cindervault/CONCEPT.md` — "Item layer: one inventory slot (a
  lantern that halves burn for 20 turns, a blade that bumps for 3)
  picked up like embers — one more decision, zero new verbs" — AND port
  the committed-proofs convention (Cindervault predates it; Shoal and
  Clockwork Courier closed the same gap): `games/cindervault/proofs.sh`
  modeled on `games/shoal/proofs.sh`, plus the host-side Python mirror
  the prototype session used but never committed (see
  `.sessions/2026-07-13-cindervault.md` §5 and its 💡 card), rebuilt,
  extended with the item layer, committed, and re-certified against the
  emulator before any pin is transcribed.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-cindervault-items.md`, born red
   (claim before build), cut from main @ `39fdc87` (PR #111 merged).
   PR #112 opened READY.
2. Toolchain: `tools/setup-toolchain.sh` (pinned devkitARM r68 via the
   leseratte10 mirror + Butano 21.7.1) + `apt-get install mgba-sdl` for
   the preinstalled `mgba==0.10.2` binding — both clean, no walls.
   Certified before touching code: the unmodified tree rebuilt to the
   committed dist hash `d3ad8c78…` EXACTLY (the strongest possible
   toolchain check this repo offers).
3. **Item layer** (`games/cindervault/src/main.cpp`): tiles `t_lantern`
   'o' / `t_blade` '/', one of each generated per floor AFTER all
   existing draws (per-floor prefix RNG word order preserved; the extra
   words re-seed later floors — a new world version by the #111 card's
   own rule, so the old session-41 routes are retired, not patched).
   Single slot + countdown in the run state; pickup activates (zero new
   verbs — walking on = taking = lighting), picking up the other kind
   replaces the held item (the one more decision). Lantern: torch burns
   on every SECOND held turn for 20 turns — integer-exact halving, 10 of
   20 burned, pickup turn free — then gutters out, slot empties. Blade:
   bump 3 instead of 2 while held ('M' one-bump). Telemetry mailbox
   16 → 18 words ([16] item, [17] lantern countdown) + an ITEM HUD line
   at the left edge (respecting the prototype's 240px-clip lesson).
4. **The mirror, committed** (`games/cindervault/tools/mirror.py`): the
   prototype 💡 recipe rebuilt as a versioned tool — xorshift32 +
   generation + full turn core, `--verify` (turn-for-turn certification
   against a `--watch-log` CSV), the BFS route bot (`--design`), a
   per-turn `--trace` pins are read from, and `--keys` emitting the
   exact harness spans. Certified THIS build: **0 mismatches across all
   75 turn states** (33 lantern-route + 42 blade-route, 14 game-state
   words each). Route design was pure host-side search; the bot also
   found a real map fact (floor 2's blade tile is the cut vertex in
   front of its stairs — crossing it is safe exactly because a blade
   re-pick is idempotent) and confirmed the re-seeded world stays
   winnable: WIN turn 93, hp 2, torch 509, score 1524.
5. **The proofs, committed** (`games/cindervault/proofs.sh`, Shoal
   convention, local-only + committed — per-PR CI stays the <60s "ROM
   builds" gate): P1 boot/title (magics, seed, text); P2 THE LANTERN —
   fixed-seed spawn pins, pickup at turn 8 with the pickup turn burning
   nothing (torch 213 at turns 7 AND 8), mid-flight half-burn (208@t17),
   gutter-out at turn 27 with EXACTLY 10 burned over the 20 held turns
   (213 → 203), full burn resumed (198@t32); P3 THE BLADE — lantern
   picked then REPLACED by the blade (slot 1 → 2, turn 10), carried down
   two floors, and on floor 3 three kills in three consecutive turns
   over three alive monsters (kills 4→7, monsters 3→0) — the arithmetic
   witness that no monster took two bumps, so the 3-HP 'M' died in ONE
   (impossible at bump-2). **P2 and P3 each RUN TWICE: watch-log CSVs
   byte-identical (`cmp` inside the script)**. Suite exit 0, 113 assert
   lines green.
6. `dist/cindervault.gba` v0.2 + README row: two clean builds
   byte-identical, 121,700 bytes, sha256 `b53421ba9ffda9ace2f9278baa39
   96bbd93792e2cd3ff94a2285702010228e97`; the proof suite ran at these
   exact bytes.
7. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge (the designed born-red hold) + the
   pre-existing `claims-format` advisory on `order-005-scribe.md` (not
   this lane's file, advisory, left alone). Re-run green after the flip.
8. Heartbeat appended to `control/status.md` (append-only dispatch
   section, PR number confirmed first). No merge/approve/auto-merge
   calls from this session — the server-side enabler decides on green.
9. Claim `control/claims/claude-cindervault-items.md` retired with this
   flip per the claims README (the open PR is the live claim from here).

## 💡 Session idea

**The consecutive-kill witness: pin a damage constant without exposing
combat internals in telemetry.** Cindervault's mailbox has no per-monster
HP words, and adding them just to prove "the blade bumps for 3" would
grow the telemetry surface for one assertion. The proof instead pins an
arithmetic corner the damage constant forces: three kills on three
CONSECUTIVE turns while exactly three monsters are alive (kills 4→5→6→7,
monsters 3→2→1→0, one player input each). Under bump-2 that sequence is
unreachable — the 3-HP 'M' absorbs two turns wherever it falls in the
order — so the pins are satisfiable if and only if bump ≥ 3. Generalized:
when a hidden constant leaves a distinctive reachable/unreachable
boundary in the OBSERVABLE counters, route the proof through a state on
the far side of that boundary instead of widening telemetry; the mirror
makes finding such states cheap (this route was searched host-side in
milliseconds, and the mirror separately confirmed the victim's identity).

## Previous-session review

- The Drift Garden weather session (`.sessions/2026-07-14-drift-garden-weather.md`,
  PR #111) closed its 💡 card with a rule this session immediately had to
  live by from the OTHER side: "a modifier that needs to reorder draws is
  a new world version, not a parameter cut." Items must consume
  generation RNG, so draw-count compatibility was impossible here — and
  following the rule's contrapositive, this session declared the new
  world explicitly (per-floor prefix order kept, extra words appended,
  session-41 routes retired and re-derived via the mirror rather than
  patched), which is exactly the honest failure mode that card predicted
  ("determinism baselines go stale without any assertion failing for the
  right reason") — none of the prototype's pins were reused unverified.
  One gap worth naming: #111's review praised keeping old determinism
  tests untouched as the proof of stream compatibility; Cindervault had
  no committed tests to keep — which is the strongest argument for the
  proofs.sh port this PR ships.
