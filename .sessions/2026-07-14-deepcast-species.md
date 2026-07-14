# Session — Deepcast growth cut 3: species tables per depth band + catch log

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/deepcast-species`, started 05:18Z,
  flipped 05:33Z — both from `date -u` at write time)
- mission: build Deepcast's next named growth cut from
  `games/deepcast/CONCEPT.md` — "Fish species tables per depth band with
  named rarities; a catch log" — as a data-table extension of the
  existing bite-by-depth logic: four depth bands (THE SHALLOWS /
  MIDWATER / THE DEEPS / THE ABYSS, named on the sink card), 16 named
  species — one per rarity tier (COMMON / UNCOMMON / RARE / MYTHIC) per
  band — the species revealed on the catch card, and a per-run catch
  log opened with SELECT (the one unused key). Species selection rides
  a SIDE-BAND xorshift32 stream seeded from `seed ^ 0x51DEF157` (the
  Drift Garden PR #115 side-band pattern), consumed one word per cast,
  so the MAIN stream's word order is untouched and every v0.2/v0.3 pin
  carries verbatim — while the PR #117 seed-select contract extends to
  the fish's name (same dialed seed = same species on the same casts).
  The log is session-scope (per run) by design — GBA SRAM persistence
  of a lifetime log is a bigger cut, noted as follow-up at the BUILT
  mark.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-deepcast-species.md`, born red
   (claim before build), cut from main @ `6c6b089` (PR #120 merged).
   PR #121 opened READY.
2. Toolchain certified before touching code: devkitARM (leseratte10
   mirror pin) + Butano present, mgba binding under Python 3.11 imported
   clean; main's unmodified tree rebuilt FROM CLEAN (scratch worktree —
   the in-place `make` was a no-op on prebuilt artifacts, which
   certifies nothing) to the committed `dist/deepcast.gba` v0.3 hash
   `2a701069…` EXACTLY.
3. **THE cut** (`games/deepcast/src/main.cpp`): pure data + one
   side-band word per cast. `depth_band(target)` splits the lake at
   52/102/152m; species = band*4 + tier, tier rolled at cast time from
   the side stream (thresholds 55/83/96 of a %100 roll); the side
   stream follows the title dial live (the daily cut's
   publish-the-choice idea), resets with `reset_run()`, advances
   exactly one word per cast. Catch card names the landed fish
   (species + rarity lines); snaps reveal nothing and never log.
   SELECT between casts or at dusk opens `st_log` — a ring of the last
   8 landed fish, "SPECIES RARITY WEIGHT" rows, SELECT closes back; no
   RNG is consumed by the log. New `dc_fishlog` 16-word mailbox
   (species/rarity indices, log count, open flag, packed ring
   species*256+weight, live side-stream state, 'FISH' magic).
4. **Carry proven before any proof changed**: the whole committed v0.3
   suite ran green against the new build UNMODIFIED — every pin
   carried, zero re-derived. Then `games/deepcast/proofs.sh` grew +P6
   (the species/log route: MOON EEL MYTHIC off P2's exact first cast,
   log open/close glyph-exact, MUD BREAM COMMON shallow catch, a 144m
   THE DEEPS IRON GAR snap that must NOT log; run twice, watch-logs
   byte-identical) and species witnesses threaded ADDITIVELY through
   P1-P5 (P1 boot sentinels + derived side seed on the title; P2/P5
   both default catch cards glyph-exact + the full ring at dusk; P3
   mute never touches the log; P4 the dialed lake's shallow catch is
   MUD BREAM COMMON where the default's was DUSK PERCH UNCOMMON). All
   literals transcribed from observed --watch-log runs AND
   cross-checked against an offline xorshift32 replica of the side
   stream. 329 committed assert flags, 751 green assert lines, exit 0;
   P2/P3/P4/P6 each run twice, byte-identical.
5. `dist/deepcast.gba` v0.4 + README row: two clean builds
   byte-identical, 131,992 bytes, sha256 `f87b79754c21414c6c7b0356cf
   fd2ca0df82501438fbf27580974696ae9185f5`; the proof suite re-ran
   green at these exact bytes. CONCEPT.md's species line marked
   **BUILT** with the SRAM-persistence follow-up note.
6. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold; telemetry
   `.substrate/guard-fires.jsonl` committed) + the pre-existing
   `claims-format` advisory on `order-005-scribe.md` (not this lane's
   file, advisory, left alone).
7. Heartbeat appended to `control/status.md` (append-only dispatch
   section, PR number confirmed first). No merge/approve/auto-merge
   calls from this session — the server-side enabler decides on green.
8. Claim `control/claims/claude-deepcast-species.md` retired with this
   flip — the file is DELETED per the claims README rule 4 ("delete
   your own claim file at session close"; the open PR is the live
   claim from here).

## 💡 Session idea

**Run the OLD suite against the NEW build before writing a single new
assert.** The natural order is build-then-prove: write the feature, then
write its proofs, then check the old ones still pass. Inverting the last
two steps made the carry claim FREE: the committed v0.3 suite, run
unmodified against the species build, was green on the first try — which
proved "the default path is untouched" with zero new proof code, before
any new pin existed to argue with. Everything added afterwards was then
purely additive by construction, and a failure at that step would have
localized the bug to the feature (not to a stale pin) while the diff was
still small. Generalized: a committed proof suite is a free regression
oracle for the NEXT feature's default path — consult the oracle before
extending it, because a green oracle turns every subsequent proof change
into an addition instead of a negotiation.

## Previous-session review

- Prior slice: PR #120 (Cindervault endless depth banding, growth cut
  3). Its discipline transferred directly — certify the toolchain
  against the committed dist hash from a CLEAN tree, derive every pin
  from an observed run, run new routes twice with `cmp` inside the
  committed script, mark the concept line BUILT in the same PR — and
  its "the feature must be invisible until invoked" burden (no SELECT =
  bit-identical v0.3) is exactly the burden this cut carried, proven
  the same way (old pins verbatim) plus the cheaper way this lane's
  #116 sibling pioneered: species that consume no MAIN-stream RNG carry
  the old world by construction, so the whole v0.3 suite doubled as the
  carry proof before P6 existed.
