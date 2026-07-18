# Session — Wickroad: the best on the title

> **Status:** `complete`

- date: 2026-07-17 (branch `claude/wickroad-title-best`, PR **#184**;
  born-red card written at 2026-07-17T23:54:42Z, resolved at
  2026-07-17T23:58:00Z, both from `date -u`).
- mission: **Wickroad polish — "THE BEST ON THE TITLE"** (owner menu row
  **WIK-03**, the SRAM best ledger — "unblocks score attack"). The
  crossroads arc's cut 4 (PR #175) persists the best run in SRAM behind
  `gl_save.h` and restores it at boot, but the persisted best is drawn
  ONLY on the end cards — a returning player cannot see the record they
  are chasing until AFTER a run ends. This slice surfaces it where a
  score-attack player looks first: the **TITLE screen**. When (and only
  when) a prior SRAM save was restored (`ledger_loaded`), the title's
  second pitch line renders `BEST GOLD x DAY y` — both the persisted
  `best_gold` AND `best_day_reached` (the latter is stored but was never
  shown anywhere). Render-only: **zero new RNG draws, `wr_telemetry`
  UNCHANGED (57), `wr_art` and `wr_ledger` unchanged.** THE CONTRACT: the
  default no-save path is byte-identical — `ledger_loaded` is false on
  every fresh / foreign / erased cart (and on every proof that boots
  WITHOUT `--savefile`), so the flavor line renders EXACTLY as before and
  proofs **P1-P12** carry verbatim (P1's title-line text asserts run on a
  fresh cart; the only save-restore title proof, P12c, asserts the
  `wr_ledger` mailbox, not this text). Branches from main (arc merged).
  Original assets only (PUBLIC repo).
- **📊 Model:** opus-4.8 · high · polish slice — surface the
  SRAM-persisted best run on the Wickroad title screen (WIK-03).
- landing posture: PR **#184** opened **READY** (not draft) —
  gba's server-side auto-merge-enabler arms native auto-merge on READY
  `claude/*` PRs, so this lands itself on green under owner merge
  authority. No self-merge, no manual auto-merge arm from this session.
  substrate-gate is RED BY DESIGN while this card is `in-progress` (the
  born-red hold); the required gate is "ROM builds". Flipping this card
  to `complete` (the deliberate LAST step) releases the gate.

## What shipped

1. Born-red gate: this card `in-progress` +
   `control/claims/claude-wickroad-title-best.md`, PR **#184**
   opened READY.
2. **The slice** (`games/wickroad/src/main.cpp`, `st_title` draw): the
   title's second pitch line (`title_lines[1]`, y=-2) becomes conditional
   — `BUT THE INK AGES` on the default no-save path (byte-identical),
   `BEST GOLD x DAY y` from the restored `best` struct when
   `ledger_loaded`. No new sprite line (reuses the existing slot/y — no
   clip, no sprite-budget delta), no RNG/telemetry change.
3. **Build**: local ROM build unavailable in this clone (no
   `third_party/butano` source) — the "ROM builds" gate is proven by CI
   `rom-builds.yml`. Proofs P1-P12 carry verbatim by the `ledger_loaded`
   gate (no proof asserts absence of this line; the render is unreachable
   on every no-`--savefile` boot).

## 💡 Session idea

**A persisted value is only as valuable as the screen it is legible on —
cut 4 proved the SRAM save was byte-safe and correct, but "correct and
invisible until you have already finished a run" left the best ledger
doing half its job.** The crossroads arc spent four cuts making the best
run bankable, restorable, and power-cycle-honest; the cheapest possible
follow-up — one conditional, zero sim delta — moved that number to the
one screen where a score-attack player forms intent (the title, before
START). The general lesson for the roster: after landing a persistence
feature, immediately ask "on which screen does the player DECIDE, and is
the persisted value legible there?" `best_day_reached` was the tell —
it had been computed, banked, and re-read across a power cycle since cut
4 yet rendered on ZERO screens. A stored field that no screen reads is a
latent feature; surfacing it costs a line and is often higher
value-per-byte than the next new mechanic. And the same `ledger_loaded`
gate cut 4 built for the end cards carried this render for free: the
default no-save path stays byte-identical by construction, so the whole
proof suite P1-P12 carried verbatim without touching a single pin.

## Previous-session review

Prior slice: `.sessions/2026-07-17-wickroad-crossroads-cut4.md` (PR
**#175**, THE BEST LEDGER, the crossroads arc closer) — **holds up, and
handed this slice everything it needed.** Its central claim was that a
persistence feature has TWO default states (fresh cart AND foreign/erased
save) and the render side must match the read side's discipline: gating
the end-card best line on `ledger_loaded` made the new presentation
literally unreachable on every no-`--savefile` boot, so "additive" was
machine-true, not frame-budgeted. This slice leaned on that gate
directly — the SAME `ledger_loaded` flag now gates a SECOND render site
(the title), and because it is false on every proof that boots without a
savefile, P1-P12 carried verbatim again with zero new pins, exactly as
cut 4 predicted its gate would let later presentation grow. One honest
forward note: cut 4's card recorded work `complete` while its PR carried
a DRAFT hold (the 2026-07-16 landing wall); that wall has since lifted —
this session's PR #184 is opened **READY** and lands on green via the
auto-merge-enabler, so the card and the PR now read the same way.
