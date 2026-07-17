# Wickroad — the CROSSROADS arc (second arc plan)

> **Status:** `cuts 1-4 built, arc growth-complete pending owner clicks
> + audio playtest` (2026-07-17, DRAFT/held; cut 2 stacked on cut 1, cut
> 3 on cut 2, cut 4 on cut 3 — land order #172 → #173 → #174 → this).

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

3. **SEED DIAL** — *SERVED — v0.9 (in-progress, DRAFT/held, stacked on
   cut 2).* A title-screen dial picks the world seed
   (`.sessions/2026-07-14-cindervault-seed.md` + Underroot slice-10
   `ur_dial_seed` precedent), turning the single fixed `0x5749434B`
   world into a family of daily/challenge worlds while keeping every
   committed proof runnable at the default seed. LEFT/RIGHT scan the
   dial (edge-triggered, TITLE ONLY); `dial_seed(0) == seed_constant`,
   so the default world — and P1-P10 — is BIT-IDENTICAL (zero
   behavioural change at default). `reset_run()` seeds `run_seed`, so
   START from a card reruns the SAME dialed world. The live seed is
   published in the APPENDED telemetry word `[56]` (mailbox `[56] →
   [57]`, the append-only wire-format discipline; words 0-55 stay
   byte-identical). Proven by new proof **P11**. Wickroad was the ONLY
   score-attack GBA title with a fixed seed and no dial — this closes
   that gap.

4. **THE BEST LEDGER** — *SERVED — v1.0 (in-progress, DRAFT/held,
   stacked on cut 3) — THE ARC CLOSER.* SRAM-persisted best runs via
   `gl_save.h` (the house save header). Wickroad was the ONLY GBA title
   with zero SRAM persistence; the best-gold ledger is the natural
   score-attack hook now that the seed dial gives runs a fair,
   repeatable world to be best in. A POD `wr_ledger_save {best_gold,
   best_day_reached, best_seed, runs}` behind magic tag `"WLDGR1"` loads
   ONCE at boot (fresh / foreign / erased cart → a zero ledger, NOT
   reset by `reset_run()`), is written to SRAM the instant a run ENDS
   (win `st_balanced` or loss `st_closed`), and survives a power cycle
   (the Deepcast `meta_slot` / Lumen-Drift two-boot precedent). THE
   CONTRACT: the default no-save path is render-cost + timing
   BYTE-IDENTICAL to cut 3 — the persisted best line renders on the end
   cards ONLY when a prior save was restored (`ledger_loaded`), which
   never happens on a fresh cart, so every proof P1-P11 (all run WITHOUT
   `--savefile`) carries VERBATIM. RNG/telemetry delta: **zero new RNG
   draws** (persistence reads the sim's existing end-of-run state, never
   the stream) and **`wr_telemetry` UNCHANGED at 57** — the ledger lives
   in a THIRD committed mailbox `wr_ledger[6]` (`'WLDG'` magic +
   best_gold, best_day_reached, best_seed, runs, loaded flag), exactly
   as cut 2 kept the art in a separate `wr_art` rather than widening the
   sim mailbox. Proven by new proof **P12** (four sub-proofs, the
   Deepcast SRAM power-cycle pattern: fresh-cart baseline · write +
   persist · power-cycle restore · the restored best line renders — run
   twice byte-identical, watch-logs AND the written `.sav` files).

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
