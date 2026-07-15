# Session — Wickroad growth cut 2: CONTRACTS (a second income verb that prices risk)

> **Status:** `complete`

- date: 2026-07-15 (branch `claude/wickroad-contracts`, PR #143; started
  04:47Z, flipped 05:10Z — both from `date -u` at write time)
- mission: **Wickroad growth cut 2 — "Contracts"** per
  `games/wickroad/CONCEPT.md`: dated delivery orders (X resin to
  Thornby by day D for a premium) — a second income verb that prices
  risk. One contained slice on the v0.2 rumors build (PR #142): single
  `main.cpp`, a fixed authored contract deck (zero new RNG draws, the
  world stays bit-identical), mirror-first pins, new proof P5 in the
  committed `proofs.sh`, dist v0.3. New per-dawn UI rides the deferral
  pattern (the #142 frame-budget lesson: dawn frames are SPENT).
- **📊 Model:** Fable-family · medium · feature build — wickroad growth cut 2
- landing posture: PR #143 opened READY on main at the born-red commit;
  no merge/approve/auto-merge calls from this session — the server-side
  enabler decides on green.

## What shipped

1. Born-red gate (commit acee852): this card `in-progress` +
   `control/claims/claude-wickroad-contracts.md` (bootstrap-rendered),
   PR #143 opened READY immediately; claim retired at the heartbeat
   (2733b8e) per claims README rule 4.
2. **The contract law** (commit 489941d, `games/wickroad/src/main.cpp`):
   a FIXED two-contract deck on the rumor-deck pattern — (offer day,
   good, destination, qty, deadline, premium) — C1 2 RESIN to THORNBY
   D8 +60 (the CONCEPT.md sentence, verbatim served), C2 2 IRON to
   GLASSMERE D14 +70. Zero new RNG draws and ZERO price-law changes:
   delivery bypasses the market (goods leave the pack, the premium
   lands whole, prices/impact/ledger untouched), so the v0.2 world is
   bit-identical and P1-P4 carried verbatim. Lifecycle DERIVED, not
   stored — two accept/paid flags per contract are the only new state;
   offered/lapsed are pure functions of the day (the simple lapse rule,
   no forfeit). RIGHT (d-pad, previously unused) is the pact verb:
   take when offered, hand over at the destination by the deadline.
   The premium prices risk by authorship: it beats spot value by a
   margin that grows with the haul and the deadline's tightness.
3. **The pact line** (trading UI + a title hint), regen deferred to the
   SECOND quiet frame after a head redraw — the crier owns the first
   (the #142 stagger, extended): scheduling stays a pure function of
   the input history, dawn frames keep their measured cost.
4. **A measured capacity wall** (documented in the game Makefile): the
   pact line pushed dawn-redraw regeneration peaks over Butano's
   default 128 sprite-tiles items — the FIRST full-suite probe froze
   P2 mid-route on `bn_sprite_tiles_manager` "No more sprite tiles
   items available" at the day-5 dawn (diagnosed by the #142 card's
   own move: ROM frame counter vs harness index — the counter STOPPED,
   a halt, not lag). Fix: -DBN_CFG_SPRITE_TILES_MAX_ITEMS=256; P1-P4
   then passed verbatim, frame alignment intact.
5. **Telemetry**: `wr_telemetry` 24 -> 32 — contract witness words
   (latest posted id, lifecycle state 1-offered/2-accepted/3-paid/
   4-lapsed, good, destination, qty, deadline, premium, cumulative
   premium banked); words 0-23 byte-for-byte unchanged.
6. **Proof P5 THE CONTRACTS** (`games/wickroad/proofs.sh`): the offer
   pinned before any player action (day 2, state 1, all seven witness
   words), RIGHT accepting (1 -> 2, gold unmoved), the 3-leg haul and
   the impact-ladder buys (21, 22 — cursor word 21 -> 22 -> 23), the
   delivery gold JUMP 10 -> 70 == the authored premium 60 exactly
   (state 3, word 31 banks it), C2 posted day 10 and LAPSED unpaid at
   dawn 15 (state 4, word 31 unchanged), pact lines exact on screen,
   day-15 gold 60 pinning the whole route ledger. Suite total P1-P5:
   **384 watch + 48 text assertion passes**, exit 0; P2-P5 each run
   twice, watch-logs byte-identical. Every pin derived on the
   host-side Python mirror FIRST; the ROM matched the mirror exactly
   on the first probe of the route — zero route iterations.
7. **Ship** (commit 4c46aad): `dist/wickroad.gba` v0.3, 122,696 B,
   sha256 `6e395a1c42cfb62cbb469b3c443d3c8f36eaaa68e61bbec0e77c83af5c
   5d3c21`, two clean builds byte-identical, suite re-run green at the
   dist bytes; dist/README row updated; CONCEPT.md cut 2 marked
   **SERVED**; docs/current-state.md Wickroad row brought current
   (was one slice stale at v0.1 — now v0.3, cuts 1-2 served).
8. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold; guard-fires
   telemetry committed, 35ff22c) + pre-existing model-line-shape
   advisories on 2026-07-14 cards (not this lane's files, advisory,
   left alone).

## 💡 Session idea

**A deterministic sim can freeze without a single sim bug: draw-side
resource pools are part of the frame-budget interface too.** The
committed pins encode "one loop iteration fits one frame" — but the
failure mode this session hit was not lag, it was a HALT: Butano's
sprite-tiles item pool (a static allocator, not VRAM itself) ran out
at a dawn-redraw peak because regeneration briefly holds old and new
line sprites at once, and the engine's assert screen stops the world.
The diagnosing move that told lag and halt apart in one look: the
ROM's own frame counter in the watch CSV — climbing lag means budget,
a FLAT counter means assert/halt, and the harness's final screenshot
then shows the engine's own error text naming the exhausted pool.
Corollary worth keeping: when a text-heavy GBA game adds its Nth
always-on line, audit the engine's static pools (sprite items, tiles
items, palettes) against the WORST redraw frame, not the average one
— and bump the config with a comment recording the measured overflow,
so the next cut knows the pool is now sized to a measured peak, not a
default.

## Previous-session review

- Prior lane slice: `.sessions/2026-07-15-wickroad-rumors.md` (PR #142,
  growth cut 1) — its 💡 ("treat the frame budget as a committed
  interface; put new draw work on frames the pins don't own") held up
  and directly shaped this slice: the pact line shipped on a SECOND
  quiet frame (staggered behind the crier's first) and every P2-P4 pin
  carried verbatim on the first post-fix run. Its explicit corollary —
  "the frame budget on dawn frames is now SPENT; any new per-dawn UI
  must ride the deferral pattern" — was followed to the letter and
  proved out. One honest improvement: that card's frame-budget frame
  of mind (iterations vs frames) did not name the OTHER way a redraw
  peak kills a run — static pool exhaustion halts the ROM outright
  (this session's item 4) — so "budget" should be read as cycles AND
  allocator headroom; this card's idea records that widening.
