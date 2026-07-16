# Session — Wickroad crossroads cut 2: THE SPRITE ART PASS

> **Status:** `in-progress`

- date: 2026-07-16 (branch `claude/wickroad-crossroads-cut2`, PR
  **#173**; started 22:42:13Z, card resolved 23:21:16Z —
  both from `date -u` at write time). Held DRAFT — Status stays
  `in-progress` by design (see landing posture).
- mission: **Wickroad crossroads cut 2 — "THE SPRITE ART PASS"** per
  `games/wickroad/ARC-CROSSROADS.md` cut 2 and the Cindervault art
  precedent (`.sessions/2026-07-14-cindervault-art.md`). The whole game
  today is text glyphs over a candlelit backdrop; this cut gives the
  town/market/junction a real face by adding an authored **regular
  background** layer baked from the SAME sim state (`town`, `price[]`,
  `cargo[]`, `cursor`, and the fork/junction words 52-55). Strictly
  ADDITIVE presentation: zero sim change. The committed telemetry
  (`wr_telemetry[56]`), the 27 variable-font `--assert-text` strings and
  proofs P1-P9 are the guard rail that the art pass changes nothing the
  sim decides — they carry BYTE-IDENTICAL. Stacked on cut 1 (PR #172).
- **📊 Model:** Claude (Opus family) · high · implementation — wickroad
  crossroads cut 2, the sprite art pass.
- landing posture: PR **#173** opened DRAFT and held draft per the
  standing 2026-07-16 landing wall — no ready-flip, no merge/approve/
  auto-merge calls from this session; card stays in-progress by design
  (born-red HOLD). Stacked on `claude/wickroad-crossroads-cut1` (#172),
  NOT on main. substrate-gate is RED BY DESIGN (main #151 orphans +
  born-red HOLD); the required gate is "ROM builds".

## What shipped

1. Born-red gate (commit `e69fbf3`): this card `in-progress` +
   `control/claims/claude-wickroad-crossroads-cut2.md`, PR **#173**
   opened DRAFT immediately and held draft.
2. **The assets** (commit `f9f6436`,
   `games/wickroad/graphics/generate_assets.py` + the committed
   `wr_tiles.bmp`/`wr_palette.bmp` + their JSON descriptors, the
   Cindervault/Lumen Drift convention: stdlib-only, byte-reproducible,
   original procedural art): a 16-color warm BG palette (parchment
   browns, ember oranges, cold steel — no color is exact black or exact
   white, so the harness's font-ink `--assert-text` matcher cannot be
   confused by construction) and **12** 4bpp 8x8 BG tiles
   (`TILE_COUNT = 12`: index 0 transparent + index 1 procedural sky/
   parchment speckle + 10 hand-drawn) — sky/parchment, road cobble +
   edge, town wall + roof + lit window, price bar filled/empty, cursor
   chevron, fork signpost, cargo pip.
3. **The cut** (commit `f9f6436`, `games/wickroad/src/main.cpp` +
   Makefile): a 32x32-cell `regular_bg` at backmost priority
   (`set_priority(3)`) is baked from the SAME sim state the glyph rows
   read — a road band with the fork drawn when a junction is live, the
   current town's skyline, and a market panel whose four price bars
   scale to `price(town, g)` with the `cursor` good highlighted and
   `cargo[]` shown as pips. The glyph sprites stay at their default
   priority, so every `--assert-text` line still renders ON TOP and
   readable (GBA draws OBJ over BG). The bake re-runs + `reload_cells_ref()`
   ONLY when a cached state key changes (day/gold/town/cursor/prices/
   cargo/fork), so quiet frames and the pinned dawn edges do no bg work.
   A SECOND mailbox `volatile unsigned wr_art[8]` ('WART' magic, scene
   id, town, cursor, a deterministic checksum fold of the 32x32 baked
   cells, the bg-enable flag, the fork-edge mirror, the rebake count)
   publishes the presentation — `wr_telemetry`'s 56 sim words stay
   untouched by contract.
4. **The proof** (commit `f9f6436`, `games/wickroad/proofs.sh`,
   additive — P1-P9 UNTOUCHED, byte-identical): **P10 THE ART PASS**
   re-runs the P9 junction route with the presentation watched — the
   `wr_art` mailbox pinned deterministically at chosen frames (scene id,
   town, cursor, checksum fold distinct across title/junction/branch),
   plus the art read straight off the hardware exactly as Cindervault's
   art proof does: DISPCNT at 0x04000000 (`io[0]` = **0xF840** = 63552,
   bit 11 / 0x0800 = the road BG3 enabled) shows the bg-enable bit
   set, BG palette RAM at 0x050001E0 holds the committed art's exact
   BGR555 colors (`bgpal[0]` = **0x08640000** — the parchment color
   0x0864 = (36,28,20) BGR555, `wr_palette` index 1, over the
   transparent index-0 slot), and the VRAM screenblock at 0x06000800
   holds baked cells (`vmap[0]` = **0xF001F001** — two bank-15 sky
   cells, tile 1). RUN TWICE — watch-log byte-identical. Suite tail
   still prints `ALL WICKROAD PROOFS PASS`; proof count 9 -> 10.
5. **Build + proofs**: `make -C games/wickroad` builds
   `wickroad.gba` + `.elf`; `bash games/wickroad/proofs.sh` prints
   **`ALL WICKROAD PROOFS PASS`**, exit 0 — P1-P9 carried VERBATIM
   (zero re-pins: the 56 telemetry words and the 27 variable-font text
   lines are byte-identical), P10 added and run twice byte-identical.
6. **Ship** (commit `f9f6436`): `dist/wickroad.gba` v0.8,
   **178,932** bytes, sha256
   `b50153b5effd6e0c37607661dd2ab7e26eec607d4c7aa27b5f7decf536fdc393`,
   two clean builds
   byte-identical; `dist/README.md` row + `docs/current-state.md`
   Wickroad row → v0.8; `games/wickroad/ARC-CROSSROADS.md` cut 2
   marked SERVED.
7. CI on the head SHA `f9f6436` (github `actions_list` by branch):
   **"ROM builds"** — the required gate; its live conclusion + run-id
   could NOT be read at card-resolve time — the GitHub REST API
   returned HTTP 503 across every poll during this heartbeat (the same
   outage flagged at hand-off). The gate is expected `success` (local
   `make clean && make` builds `wickroad.gba` clean and
   `bash games/wickroad/proofs.sh` prints `ALL WICKROAD PROOFS PASS`),
   but that expectation is NOT a live CI read — see the re-check note.
   **"substrate-gate"** — RED BY DESIGN (main's #151 orphans + the
   born-red HOLD, named in the PR body); its live conclusion + run-id
   were likewise unread under the same 503 outage.

## 💡 Session idea

**A sprite-only game is already carrying a free layer behind its own
picture — because the GBA composites OBJ above BG, an authored regular
background can be dropped in behind the glyph HUD and provably perturb
nothing the sim decides.** Wickroad's entire screen was text glyphs on a
flat candlelit backdrop; the whole face of the town/market/junction was
unauthored. The move is the exact sibling of cut 1's `L+R` chord: cut 1
found a free VERB hiding in an already-fully-consumed button grammar (the
input the committed routes never produce); cut 2 finds a free LAYER
hiding in an already-fully-consumed screen (the BG plane the OBJ-only
renderer never wrote). Both are the same discipline — *find the axis the
committed format left open and add there* — and both prove freeness the
same append-only way: run the OLD suite untouched and watch it pass
BYTE-IDENTICAL. The 27 variable-font `--assert-text` lines still match
because glyphs render at default OBJ priority ON TOP of a `set_priority(3)`
backmost BG; the 56 telemetry words still match because the sim never
reads the bake. The crucial corollary is that **presentation earns its
own mailbox**: `wr_art` (a separate 'WART'-magic 8-word block — scene id,
town, cursor, a checksum fold of the 32×32 baked cells, the bg-enable
flag, the fork-edge mirror, the rebake count) keeps the art's proof OFF
the sim's telemetry, so P10 pins the *hardware* the art actually drives
(DISPCNT 0xF840, BG palette RAM 0x08640000, the VRAM screenblock
0xF001F001) rather than asserting against code — the same hardware-read
method Cindervault's art proof used. When a format looks fully consumed,
the next feature is on the axis you haven't spent yet (a chord in input
space, a plane in display space), and you give it a private witness so
the old contract's guard rails never have to move to admit it.

## Previous-session review

- Prior lane slice: cut 1, `.sessions/2026-07-16-wickroad-crossroads-cut1.md`
  (PR #172, "THE JUNCTION"). Cut 1 finally built the thing `CONCEPT.md`
  had flagged as blocked: it refactored the bare linear town index into a
  small static `adjacency[town_count]` table (the spine 0↔1↔…↔6 kept
  bit-identical by keeping the output arithmetic identical), hung ONE
  off-spine branch town — WYRMHOLLOW (index 7) — off the mid-spine
  junction DUNWICK (4), and added the first new travel verb since v0.1,
  the **L+R chord** ("take the fork"), additive by construction because no
  committed route presses both shoulders on one frame. RNG grew 7→8 towns
  (WYRMHOLLOW's 8 draws appended strictly after town 6's at the wider-map
  freeze point, so the old stream is byte-identical), telemetry grew
  52→56 (words 52-55: on-branch flag, fork-edge id, branch RESIN witness,
  reserved), and it landed proof P9 THE JUNCTION with P1-P8 carried
  verbatim, shipping `dist/wickroad.gba` v0.7. It is still **DRAFT and
  held on the same standing 2026-07-16 landing wall** (born-red HOLD,
  no ready-flip/merge), and cut 2 is stacked directly on it (base
  `claude/wickroad-crossroads-cut1`), NOT on main — so cut 2 inherits its
  substrate-gate RED (main #151 orphans) and its DRAFT posture by design.
  What cut 2 built ON: cut 1's telemetry word 53 (fork-edge id = 7 while a
  fork is live) is exactly the sim fact P10's `wr_art[6]` mirrors to prove
  the junction/branch get their signpost face — the art rides the verb
  cut 1 opened. What cut 2 had to work AROUND: cut 1 spent every remaining
  design axis it could in *sim* space (input + state), so cut 2 could not
  add another sim word without re-pinning P1-P9; the resolution was to
  leave `wr_telemetry[56]` frozen and give presentation its own `wr_art`
  mailbox, so the art pass reads as zero-sim-change and P1-P9 carry
  byte-identical the same way cut 1's P1-P8 did. Cut 1's honest gap — a
  card should name, in the code, the exact freeze point the next session
  appends behind — was already folded into `reset_run()` and
  `ARC-CROSSROADS.md`; cut 2 continues that discipline by fixing the
  hardware witness addresses (DISPCNT/palette RAM/VRAM screenblock) in the
  P10 comment block so the next cut (the seed dial) knows exactly what the
  art plane pins before it touches anything.
