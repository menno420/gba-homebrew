# Session — Wickroad crossroads cut 4: THE BEST LEDGER

> **Status:** `in-progress`

- date: 2026-07-17 (branch `claude/wickroad-crossroads-cut4`, PR
  **[[fill:PR]]**; started 00:33:43Z — from `date -u` at write time).
  Held DRAFT — Status stays `in-progress` at the born-red gate; flips to
  `complete` in the heartbeat commit once code + proofs are green (the
  PR alone carries the DRAFT hold, mirroring the cut-3 card convention).
- mission: **Wickroad crossroads cut 4 — "THE BEST LEDGER"** per
  `games/wickroad/ARC-CROSSROADS.md` cut 4, the ARC CLOSER. Wickroad was
  the ONLY GBA title in the lab with zero SRAM persistence; now that the
  seed dial (cut 3) gives runs a fair, repeatable world to be best in,
  this cut banks the **best run across power cycles** in SRAM behind the
  house save header `games/common/include/gl_save.h` (the Deepcast
  `meta_slot` / Lumen-Drift two-boot precedent). A small POD
  `wr_ledger_save {best_gold, best_day_reached, best_seed, runs}` sits
  behind a magic tag `"WLDGR1"`; it loads ONCE at boot (a fresh /
  foreign / erased cart reads as NO save → a zero ledger), is written to
  SRAM the instant a run ENDS (win `st_balanced` or loss `st_closed`),
  and survives a power cycle. THE CONTRACT: the default no-save path is
  render-cost + timing byte-identical to cut 3 — the end-card best line
  is drawn ONLY when a prior save was restored (`ledger_loaded`), which
  never happens on a fresh cart, so every committed proof P1-P11 (all
  run WITHOUT `--savefile`) carried VERBATIM. The ledger publishes in a
  THIRD witness mailbox `wr_ledger[6]` (mirroring how cut 2 added
  `wr_art`), so `wr_telemetry` (57) AND `wr_art` (8) stay byte-unchanged.
  Proven by new proof **P12**. Stacked on cut 3 (PR #174). Original
  assets only (PUBLIC repo).
- **📊 Model:** Claude (Opus family) · high · feature build — wickroad
  crossroads cut 4, the best ledger (arc closer).
- landing posture: PR **[[fill:PR]]** opened DRAFT and held draft per
  the standing 2026-07-16 landing wall — no ready-flip, no merge/
  approve/auto-merge calls from this session. Stacked on
  `claude/wickroad-crossroads-cut3` (#174), NOT on main. substrate-gate
  is RED BY DESIGN (it gates on cut 1's still-in-progress card + main's
  #151 doc orphans + this born-red HOLD); the required gate is "ROM
  builds". Land order for the whole stack: #172 → #173 → #174 → this.

## What shipped

1. Born-red gate (commit **[[fill:C1]]**): this card `in-progress` +
   `control/claims/claude-wickroad-crossroads-cut4.md`, PR **[[fill:PR]]**
   opened DRAFT immediately and held draft.
2. **The cut** (`games/wickroad/src/main.cpp`): `#include "gl_save.h"`
   (+ `../common/include` restored to the Makefile INCLUDES so the house
   header resolves, mirroring Deepcast's Makefile). A POD
   `wr_ledger_save {int best_gold; int best_day_reached; int best_seed;
   int runs;}` behind magic `"WLDGR1"`; `gl::save_slot<wr_ledger_save>
   ledger_slot` + `wr_ledger_save best` declared OUTSIDE the run-state
   block and NOT reset by `reset_run()`, loaded ONCE at boot
   (`ledger_loaded = ledger_slot.load(best)`). A `record_run()` lambda —
   best = highest gold at any run-end, its day + dialed seed riding
   along, a run counter bumped each end — is called at the three end
   sites (`dawn()`'s `st_closed`, the sell win, the pact win) and
   `ledger_slot.save(best)` lands it in SRAM immediately (power-off
   safe). The persisted best gold renders on the end cards in the
   VARIABLE 8x8 font ONLY when `ledger_loaded`, so the default path is
   byte-identical. The ledger publishes every frame in the new witness
   mailbox `wr_ledger[6]` (`'WLDG'` magic, best_gold, best_day_reached,
   best_seed, runs, loaded flag) — a THIRD symbol, so `wr_telemetry` and
   `wr_art` are byte-unchanged. (Commit **[[fill:C2]]**.)
3. **The proof** (`games/wickroad/proofs.sh`, additive — P1-P11
   UNTOUCHED, byte-identical): **P12 THE BEST LEDGER**, mirroring the
   Deepcast SRAM power-cycle proof. **P12a** fresh-cart baseline (no
   `--savefile`): loaded flag 0, zero ledger, the best line NOT drawn.
   **P12b** write + persist: boot on a factory-fresh savefile and drive
   P2's committed `WIN_ROUTE` to the win — the run-end banks {gold 328,
   day 13, seed `0x5749434B`, runs 1} into SRAM (run twice into two
   fresh `.sav`, `cmp` BOTH the watch-logs AND the two written `.sav`).
   **P12c** power-cycle restore: a SEPARATE boot reusing P12b's `.sav`,
   NO input — the best is restored from SRAM and present in `wr_ledger`
   on the TITLE before any key (loaded flag 1); no run ends so no write,
   the `.sav` is UNTOUCHED (`cmp` vs boot1a). **P12d** the restored best
   line renders: reboot + re-run the route, `--assert-text "BEST GOLD
   328"` on the win card (the `ledger_loaded`-gated render), the run
   count bumped 1 → 2 and re-persisted. Every pin host-mirrored from
   EXISTING proofs (gold 328 / day 13 from P2, seed 1464419147 from
   P11). RUN TWICE — watch-logs AND `.sav` byte-identical.
   (Commit **[[fill:C2]]**.)
4. **Build + proofs**: Makefile INCLUDES gains `../common/include`;
   `make -C games/wickroad` builds `wickroad.gba` + `.elf`; `bash
   games/wickroad/proofs.sh` prints **`ALL WICKROAD PROOFS PASS`**, exit
   0 — P1-P11 carried VERBATIM (re-ran green byte-identical before P12
   was appended), P12 added. `dist/wickroad.gba` v1.0, **180,008 bytes**,
   sha256 **`d740b73832607283e4f79f0bf0b33dfb48eb652da58d375eae1ada7807c14cb4`**.
   (cut 3 was 179,396 bytes / `d7be4b99…`; the +612 bytes are the SRAM
   ledger code + the third mailbox.)

## 💡 Session idea

**A SRAM save is the strongest "default path byte-identical" test the arc
has faced, because persistence makes the OFF state a first-class,
externally-supplied input — and the harness proves it by feeding the ROM
a savefile it never wrote.** Cut 3's lesson was that "zero behavioural
change at default" must cover the render budget, not just mailbox values.
Cut 4 sharpens it: a persistence feature has TWO default states, not one
— the fresh cart (no save) AND a foreign/erased save — and BOTH must
read as "no save → the v0.9 world." `gl_save.h`'s magic tag already
enforces that on the read side, but the render side needed the same
discipline: gating the end-card best line on `ledger_loaded` means the
new presentation is literally unreachable on any path P1-P11 exercise
(they never pass `--savefile`), so "additive" is machine-true by
construction, not by careful frame-budgeting. The deeper move is that
`--savefile` lets a proof assert the RESTORE — booting the ROM against
SRAM bytes a PRIOR boot wrote — which no run-twice-in-one-process check
can reach: P12c is the first Wickroad pin whose truth lives ACROSS a
power cycle, and the two-boot pattern (write in boot 1, read in boot 2,
`cmp` the battery file) is the only honest way to prove a save is a save
and not an in-memory accident.

## Previous-session review

Prior slice: `.sessions/2026-07-16-wickroad-crossroads-cut3.md` (PR
**#174**, THE SEED DIAL, head `824f19f`) — **holds up, and was the load
the ledger needed.** Its central claim was the append-only wire-format
discipline carried one rung further: telemetry grown `[56] → [57]` with
words 0-55 byte-unchanged, dial 0 == `seed_constant` so P1-P10 stayed
bit-identical, proven by a cross-build `cmp` of the P9/P10 watch-logs.
This cut leaned directly on two of its results: (a) its host-mirrored
dial-0 seed pin `wr_telemetry[56] == 1464419147` is exactly the value
cut 4 persists as `best_seed` and re-asserts in P12b/c — I didn't
re-derive it, I inherited it; (b) its 💡 — that a new feature's
presentation must be inert in its default state on the vblank budget,
not just in the mailbox — is the reading that shaped cut 4's gating:
rather than prove the best line is timing-neutral on the end cards, I
made it unreachable on the default path entirely (`ledger_loaded` false
on every no-savefile boot), which is the same lesson applied one level
up — don't budget the new render against the pinned frames, keep it out
of the pinned frames' reach. One forward note closing the arc: cut 3
flagged that its card records work `complete` while the PR carries the
DRAFT hold (reconciling cut 2's `in-progress`-by-design card); cut 4
follows that same convention, so the whole crossroads stack now reads
one way — the card is the work record, the PR is the landing gate.
