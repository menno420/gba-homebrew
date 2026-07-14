# Session — Deepcast growth cut 5: the art pass (lake gradient + silhouette fish + rod bend)

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/deepcast-art`, started 07:50Z,
  flipped 08:10Z — both from `date -u` at write time)
- mission: build Deepcast's LAST named growth cut from
  `games/deepcast/CONCEPT.md` — "Real art pass: lake gradient by depth,
  silhouette fish, rod-bend sprite as the analog tension gauge (HUD bar
  becomes diegetic)". Presentation ONLY: game state, RNG word order and
  every `dc_telemetry` / `dc_fishlog` / `dc_linemeta` word untouched, so
  ALL prior proofs must pass verbatim. Original procedural indexed-BMP
  assets (the Lumen Drift / Cindervault `generate_assets.py` convention),
  the text HUD kept alongside the art. This completes Deepcast's named
  growth path (#113 audio, #117 seed dial, #121 species/log, #125 line
  upgrades, this cut).
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-deepcast-art.md`, born red (claim
   before build), cut from main @ `9517806` (PR #127 merged).
   PR #128 opened READY.
2. Toolchain certified before touching code: the unmodified tree rebuilt
   FROM CLEAN (scratch worktree) to the committed `dist/deepcast.gba`
   v0.5 hash `c8ed6f11…` EXACTLY (pinned devkitARM via the leseratte10
   mirror — walls quoted, not re-probed; preinstalled Python 3.11 mgba
   binding reused).
3. **The assets** (`games/deepcast/graphics/generate_assets.py` +
   committed BMPs/JSONs, the Lumen Drift/Cindervault convention:
   stdlib-only, byte-reproducible, original art): 12 BG water tiles (ten
   solid gradient bands + the dusk surface shimmer), a 32x16 silhouette
   fish sprite with 16 frames — one per SPECIES (frame = band*4 + tier:
   the depth band draws the silhouette bigger, the rarity tier paler),
   and a 32x32 rod sprite with 8 bend frames (a quadratic-Bezier blank,
   tension pulls the tip down). No color is exact black or white, so the
   harness's font-pixel text matcher cannot be confused by construction
   — the gradient law's blue floor (4/31) guarantees it at every depth.
4. **The cut** (`games/deepcast/src/main.cpp` + Makefile): three pure
   READS of sim state. (a) THE LAKE: a full-screen ten-band background
   whose ten palette entries are rewritten from a closed-form law of the
   live depth word — dim = 2*band + depth/8, BGR555 = max(1,5-dim/3) |
   max(1,16-dim)<<5 | max(4,26-dim)<<10 — the whole lake deepens as the
   lure sinks and surfaces again at dusk. (b) THE FISH: frame = the
   dc_fishlog species word while fighting and on the CATCH card, hidden
   while sinking and on a SNAP (the species keeps its secret; it flips
   to run on surge frames). (c) THE ROD: bend = tension * 7 /
   line_snap[tier] — the SAME tension-over-snap law the audio ratchet
   reads, so the gauge is honest on any line tier. A FOURTH mailbox
   `dc_artmeta[8]` ('DART', depth word, band-0/band-9 law halfwords,
   fish frame, rod bend, flip flag) publishes the presentation; the
   three sim mailboxes stay untouched by contract.
5. **Carry proven before any proof changed** (the #121/#125/#127 oracle
   move): the whole committed v0.5 P1-P7 suite ran green against the new
   build UNMODIFIED — every pin carried, zero re-derived, zero adjusted
   text asserts (762 watch + 125 text). Then `games/deepcast/proofs.sh`
   grew P8/P9/P10, all literals transcribed from observed --watch-log
   runs and cross-checked against an offline replica of the laws: P8 the
   P2 route with the hardware watched (every P2 pin re-lands; gradient
   palette RAM walks 0x6A05 -> 0x44E2 -> 0x1421 on carried depth pins;
   DISPCNT 0xF840 in every state; the committed art's exact BGR555
   colors in OBJ palette RAM 0x050003C4/0x050003E4; the lake's cells
   0xF00BF00B in screenblock 0x06000800; rod bend 1 at tension 98, 6 at
   588; MOON EEL frame 7 / TRENCH MAW frame 13 on the line); P9 the P6
   route (every P6 pin carried; species frames 7/0/9 across three bands;
   the fish HIDDEN while sinking and on the snap card while fl[0] still
   says 9); P10 the honest gauge (P7's two-SRAM-state pattern on the rod
   law: the SAME fight bends the worn rod 2/6 and the braided rod 1/5 at
   the same pinned tension words — the gauge follows lm[3]). Every new
   leg RUN TWICE: watch-log CSVs (including the hardware reads)
   byte-identical. Suite exit 0: 1330 watch + 189 text asserts.
6. `dist/deepcast.gba` v0.6 + README row: two clean builds
   byte-identical, 159,928 bytes, sha256 `ab9c026b889e92466da3e18ffda9
   66cfefe5a068d8d7546f424115d5b503e777`; the proof suite ran green at
   these exact bytes. CONCEPT.md's art line marked **BUILT** — the named
   growth path is COMPLETE (sellability note updated: the presentation
   is no longer placeholder).
7. Before/after evidence committed under `docs/proof/deepcast-art-*`
   (same routes, same frames: flat-backdrop text vs the graded lake, the
   abyss fight visibly near-dark with the big silhouette on the line)
   and embedded in the PR body.
8. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold; telemetry
   `.substrate/guard-fires.jsonl` committed) + the pre-existing
   `claims-format` advisory on `order-005-scribe.md` (not this lane's
   file, advisory, left alone). Re-run green after the flip.
9. Heartbeat appended to `control/status.md` (append-only dispatch
   section, PR number confirmed first). No merge/approve/auto-merge
   calls from this session — the server-side enabler decides on green.
10. Claim `control/claims/claude-deepcast-art.md` retired (deleted) with
    this flip per the claims README rule 4 — the open PR is the live
    claim from here.

## 💡 Session idea

**When a game's quantities already have proven laws, art is cheapest to
prove as a re-projection of those same laws — pick each visual's driver
so that its witness is a pin you already own.** This cut wrote zero new
routes: every new assert lands on a frame some prior cut already pinned
(depth words from the P2 route, species words from P6, tension-vs-snap
from P7's two-SRAM-state fight). That was only possible because each art
layer was DESIGNED to read an already-witnessed word: the gradient reads
dc[6], the silhouette reads fl[0], the rod reads dc[8]/lm[3] — the exact
pair the audio ratchet reads, so P10 could reuse P7's savefile-as-
treatment pattern verbatim and prove "the gauge is honest on any line"
as pure arithmetic (2/6 vs 1/5 at the same tension words). The
anti-pattern this avoids: inventing a new presentation-side quantity (a
tween, a timer, an eased position) that owns state of its own — then
the proof needs new routes, new pins, and a determinism argument the sim
never promised. Corollary worth keeping: if a visual can't be expressed
as a closed form of an already-pinned word, that is a design smell in
the feature, not just a proof inconvenience.

## Previous-session review

- Prior slice: `.sessions/2026-07-14-cindervault-art.md` (PR #127, the
  Cindervault art pass — that lane's own growth-path completion) — holds
  up completely, and this session is deliberately its sibling: the
  (frozen sim words) x (law-checked derived words in a second mailbox) x
  (raw hardware reads) split transferred one game over without friction,
  down to the same screenblock address (0x06000800) and the same
  palette-bank-15 discovery pattern. Two refinements this lane added on
  top of #127's method: (1) the gradient law lives in palette RAM, so
  the hardware pin is not just "the committed art is present" (#127's
  static color pins) but "the palette MOVES with a pinned sim word" —
  the same RAM address holds three different law values along one route;
  (2) #127's 💡 said presentation should own no state — this cut's P10
  shows the payoff compounding: because the rod owns no state and reads
  lm[3], P7's two-boot savefile pattern proved the gauge's honesty with
  zero new infrastructure. One nit found while reading #127's proofs.sh:
  its P8 leg B route comment says "~160 waited turns" where the key
  script waits ~160 TURN-PERIODS of 6 frames (~950 frames) — the pins
  are unambiguous either way; noted here rather than churning a merged
  file.
