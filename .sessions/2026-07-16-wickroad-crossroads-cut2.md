# Session — Wickroad crossroads cut 2: THE SPRITE ART PASS

> **Status:** `in-progress`

- date: 2026-07-16 (branch `claude/wickroad-crossroads-cut2`, PR
  [[fill:pr]]; started 22:42:13Z, card resolved [[fill:resolved]]Z —
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
- landing posture: PR [[fill:pr]] opened DRAFT and held draft per the
  standing 2026-07-16 landing wall — no ready-flip, no merge/approve/
  auto-merge calls from this session; card stays in-progress by design
  (born-red HOLD). Stacked on `claude/wickroad-crossroads-cut1` (#172),
  NOT on main. substrate-gate is RED BY DESIGN (main #151 orphans +
  born-red HOLD); the required gate is "ROM builds".

## What shipped

1. Born-red gate (commit [[fill:bornred]]): this card `in-progress` +
   `control/claims/claude-wickroad-crossroads-cut2.md`, PR [[fill:pr]]
   opened DRAFT immediately and held draft.
2. **The assets** (commit [[fill:impl]],
   `games/wickroad/graphics/generate_assets.py` + the committed
   `wr_tiles.bmp`/`wr_palette.bmp` + their JSON descriptors, the
   Cindervault/Lumen Drift convention: stdlib-only, byte-reproducible,
   original procedural art): a 16-color warm BG palette (parchment
   browns, ember oranges, cold steel — no color is exact black or exact
   white, so the harness's font-ink `--assert-text` matcher cannot be
   confused by construction) and [[fill:tilecount]] 8x8 BG tiles — sky/
   parchment, road cobble + edge, town wall + roof + lit window, price
   bar filled/empty, cursor chevron, fork signpost, cargo pip.
3. **The cut** (commit [[fill:impl]], `games/wickroad/src/main.cpp` +
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
4. **The proof** (commit [[fill:impl]], `games/wickroad/proofs.sh`,
   additive — P1-P9 UNTOUCHED, byte-identical): **P10 THE ART PASS**
   re-runs the P9 junction route with the presentation watched — the
   `wr_art` mailbox pinned deterministically at chosen frames (scene id,
   town, cursor, checksum fold distinct across title/junction/branch),
   plus the art read straight off the hardware exactly as Cindervault's
   art proof does: DISPCNT ([[fill:dispcnt]]) shows the bg-enable bit
   set, BG palette RAM holds the committed art's exact BGR555 colors
   ([[fill:bgpal]]), and the VRAM screenblock holds baked cells
   ([[fill:vram]]). RUN TWICE — watch-log byte-identical. Suite tail
   still prints `ALL WICKROAD PROOFS PASS`; proof count 9 -> 10.
5. **Build + proofs**: `make -C games/wickroad` builds
   `wickroad.gba` + `.elf`; `bash games/wickroad/proofs.sh` prints
   **`ALL WICKROAD PROOFS PASS`**, exit 0 — P1-P9 carried VERBATIM
   (zero re-pins: the 56 telemetry words and the 27 variable-font text
   lines are byte-identical), P10 added and run twice byte-identical.
6. **Ship** (commit [[fill:impl]]): `dist/wickroad.gba` v0.8,
   [[fill:size]] bytes, sha256 [[fill:sha]], two clean builds
   byte-identical; `dist/README.md` row + `docs/current-state.md`
   Wickroad row → v0.8; `games/wickroad/ARC-CROSSROADS.md` cut 2
   marked SERVED.
7. CI on the head SHA [[fill:impl]] (github `actions_list` by branch):
   **"ROM builds"** [[fill:rombuild]]; **"substrate-gate"**
   [[fill:substrate]] (RED BY DESIGN = main's #151 orphans + the
   born-red HOLD, named in the PR body).

## 💡 Session idea

[[fill:idea]]

## Previous-session review

[[fill:review]]
