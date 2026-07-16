# Wickroad — the CROSSROADS arc (second arc plan)

> **Status:** `cuts 1-2 in-progress` (2026-07-16, DRAFT/held; cut 2
> stacked on cut 1) — cuts 3-4 planned.

The named *growth path* (`CONCEPT.md` § Growth cuts, cuts 1-5) is
**complete** as of 2026-07-15 (v0.6, PRs #142-#146). Its biggest idea,
though, was named as a cut *inside* a served cut and deliberately left
un-built: the map "stays ONE road — no branch fork … a junction would
need a new travel verb — the honest cut named here" (`CONCEPT.md`
§ Growth cuts item 4). The **crossroads arc** is the second arc that
finally builds it, and three neighbours the same title now wants.

Source: the owner NEXT menu, **`docs/NEXT-MENU-2026-07-15.md` § B2**
("Wickroad: the crossroads" — "4 cuts: «The junction», «Sprite art
pass», «Seed dial», «The best ledger»").

## The four cuts, in build order

1. **THE JUNCTION** — *branch roads + the fork verb.* **← THIS CUT
   (in-progress).** Refactor the bare linear town index into a small
   static adjacency table (spine 0↔1↔…↔6 stays bit-identical), hang
   ONE off-spine branch town — **WYRMHOLLOW** (index 7) — off the
   mid-spine junction **DUNWICK** (4), and add the first new travel
   verb since v0.1: the **L+R chord** ("take the fork"), which is
   purely additive because no committed route ever presses both
   shoulders on one frame. Reachable ONLY by the chord; R at the
   junction still follows the spine and never reaches it. RNG: +8
   draws appended strictly after town 6's (mirror town 6: 4 base + 4
   phase) at the wider-map cut's named freeze point — the old world
   stays bit-identical, so P1-P8 carry verbatim. Telemetry grows
   `[52] → [56]` (words 52-55 appended). Proven by new proof **P9 THE
   JUNCTION**.

2. **SPRITE ART PASS** — *SERVED — v0.8 (in-progress, DRAFT/held,
   stacked on cut 1).* Gives the towns and the road a real face on the
   Cindervault art method (`.sessions/2026-07-14-cindervault-art.md`):
   an authored `regular_bg` (original procedural tiles/palette,
   `games/wickroad/graphics/generate_assets.py`) baked from the SAME sim
   state the glyph rows read (town, price[], cargo[], cursor, the
   fork/junction words) — a road band with the fork drawn when a
   junction is live, the current town's skyline, a four-good market
   whose bars scale to the live prices. The bg sits at BACKMOST priority
   so the variable-font glyphs still draw on top; the bake is DEFERRED
   off every committed edge frame so the dawn/stagger sprite budget is
   untouched. Presentation ONLY — the committed telemetry (56 words),
   the 27 `--assert-text` lines and proofs P1-P9 are the guard rail,
   carried BYTE-IDENTICAL; the art itself is proven in a second `wr_art`
   mailbox + DISPCNT/palette-RAM/VRAM hardware pins in new proof P10.

3. **SEED DIAL** — *planned.* A title-screen dial that picks the world
   seed (`.sessions/2026-07-14-cindervault-seed.md` precedent), turning
   the single fixed `0x5749434B` world into a family of daily/challenge
   worlds while keeping every committed proof runnable at the default
   seed. Wickroad is currently the ONLY score-attack GBA title with a
   fixed seed and no dial — this closes that gap.

4. **THE BEST LEDGER** — *planned.* SRAM-persisted best runs via
   `gl_save.h` (the house save header). Wickroad is currently the ONLY
   GBA title with zero SRAM persistence; the best-gold-by-day-N ledger
   is the natural score-attack hook once the seed dial gives runs a
   fair, repeatable world to be best in.

**Slice budget:** 4-5 (the junction likely 2 — this cut being the
first, self-contained core; a sprite/polish follow could split off).

## Proof strategy (unchanged from the growth-path arc)

The cut-4/cut-5 discipline carries verbatim — it is *why* an arc this
deep stays honest:

- **Host-mirror-first.** Every watch-word pin is derived on a
  host-side Python mirror of the closed-form price law BEFORE the ROM
  is probed — the ROM then has to MATCH the mirror, not define truth.
- **Run-twice byte-identical.** Every new proof runs twice and `cmp`s
  its watch-log; the determinism contract (`seed 0x5749434B`, one
  xorshift32 consumed only at world init in a committed order) is
  re-proven each cut, not assumed.
- **Additive proofs.** New proofs are appended (P9, P10, …); P1-P8
  are NEVER edited. They passing verbatim after a change is the
  machine-checked proof that the change touched only appended state —
  the append-only wire-format discipline (the wider-map cut's 💡:
  "a committed RNG stream is append-only — treat the draw ORDER like a
  wire format") applied to RNG draws AND telemetry words AND the proof
  suite itself.
- **Assert what the harness can READ.** Watch words pin sim state;
  `--assert-text` pins only VARIABLE-font UI lines (the fixed-font
  market/ledger rows are not text-assertable — the wider-map cut's
  measured harness-reach rule).

Each cut names its RNG delta and its telemetry delta in the code at the
freeze point, so the next session widens the world without re-rolling
it.
