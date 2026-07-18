# Session — Wickroad crossroads cut 3: THE SEED DIAL

> **Status:** `complete`

- date: 2026-07-16 (branch `claude/wickroad-crossroads-cut3`, PR
  **#174**; started 23:43:40Z, card resolved 23:59:00Z — both from
  `date -u` at write time). PR held DRAFT per the landing wall; the card
  records the work as done.
- mission: **Wickroad crossroads cut 3 — "THE SEED DIAL"** per
  `games/wickroad/ARC-CROSSROADS.md` cut 3 and the Cindervault-seed
  (`.sessions/2026-07-14-cindervault-seed.md`) + Underroot slice-10
  (`claude/underroot-slice-10`, `ur_dial_seed`) precedents. Wickroad is
  the ONLY score-attack GBA title with a single fixed seed and no dial;
  this cut adds a **title-screen world-seed dial** (LEFT/RIGHT scan,
  edge-triggered, title ONLY — its keys are trading verbs everywhere
  else), turning the single fixed `0x5749434B` 'WICK' world into a
  family of daily/challenge worlds. THE CONTRACT (mirroring Underroot
  slice-10 / Cindervault-seed): **dial 0 IS `seed_constant`**, so the
  default boot world — and every committed proof P1-P10 — is
  bit-identical (zero behavioral change at default). `reset_run()` seeds
  from the dialed value, so START from a win/lose card reruns the SAME
  dialed world. The live dialed seed is published in an APPENDED
  telemetry word `[56]` (the append-only wire-format discipline extended
  `[56] → [57]`; the 56 committed sim words 0-55 stay BYTE-IDENTICAL by
  contract), so every dial step is machine-watchable. Proven by new
  proof **P11**. Stacked on cut 2 (PR #173). Original assets only
  (PUBLIC repo).
- **📊 Model:** Claude (Opus family) · high · feature build — wickroad
  crossroads cut 3, the seed dial.
- landing posture: PR **#174** opened DRAFT and held draft per the
  standing 2026-07-16 landing wall — no ready-flip, no merge/approve/
  auto-merge calls from this session. Stacked on
  `claude/wickroad-crossroads-cut2` (#173), NOT on main. substrate-gate
  is RED BY DESIGN (it gates on cut 1's still-in-progress card + main's
  #151 doc orphans); the required gate is "ROM builds". PR #173's "ROM
  builds" read — pending a GitHub REST 503 at cut 2's dispatch — has
  since backfilled GREEN (run 29542209699, head 9ae55ef, conclusion
  success, 2026-07-16T23:29:45Z), and a local clean rebuild of cut 2
  reproduced its committed dist hash `b50153b5…` exactly, so cut 3 is
  stacked on solid ground.

## What shipped

1. Born-red gate (commit `14c4e80`): this card `in-progress` +
   `control/claims/claude-wickroad-crossroads-cut3.md`, PR **#174**
   opened DRAFT immediately and held draft.
2. **The cut** (`games/wickroad/src/main.cpp`): a `dial_seed(d)`
   function — `d == 0` returns `seed_constant` EXACTLY (the pinned
   world), non-zero positions hash off the constant down a DIAL-salted
   xorshift into the bounded space `[1, 0xFFFFFF+1]` (never 0, the
   xorshift dead state). A `seed_dial` (0..15) + `run_seed` pair lives
   on the title; LEFT/RIGHT scan it edge-triggered on the title ONLY;
   `reset_run()` now seeds `rng_t rng(run_seed)` (was `seed_constant`)
   so dial 0 draws the exact committed stream. The live seed renders on
   the title's START line as `SEED xxxxxxxx` (the pinned `PRESS START`
   prefix carries) and is published in the appended telemetry word
   `wr_telemetry[56]` (array grown `[56] → [57]`), so each dial step is
   watchable and the run-time seed is pinned.
3. **The proof** (`games/wickroad/proofs.sh`, additive — P1-P10
   UNTOUCHED, byte-identical): **P11 THE SEED DIAL** — the title dial
   scans RIGHT×3 / LEFT×3 with `wr_telemetry[56]` pinned at every step
   against the host mirror (dial 0 `0x5749434B` → dial 1 `0x002FC1A3` →
   dial 2 `0x003B0B8A` → dial 3 `0x00405452` and back), the `SEED`
   digits glyph-exact on the title, state staying `title` throughout;
   dial 0 START reproduces the pinned world (word 13 = 9, the P2 day-1
   EMBERTON TALLOW pin) with `[56]` carrying the seed into the run; a
   dialed world DIFFERS (dial 1 word 13 = 12 ≠ 9) and its seed carries.
   RUN TWICE — watch-log byte-identical. (Commit `45a1486`.)
4. **Build + proofs**: `make -C games/wickroad` builds `wickroad.gba` +
   `.elf`; `bash games/wickroad/proofs.sh` prints **`ALL WICKROAD
   PROOFS PASS`**, exit 0 — P1-P10 carried VERBATIM, P11 added. Proven
   byte-identical to cut 2 the STRONG way: rebuilt the cut-2 tree and
   the cut-3 tree and `cmp`'d their P9 (all 56 sim words) and P10
   (`wr_art` + hardware) watch-logs — **byte-identical across builds**,
   not merely run-twice within one. `dist/wickroad.gba` v0.9, **179,396
   bytes**, sha256 **`d7be4b99eb4956e555eae6d20637fb776971908b0bd06bb4e
   0c840eeacc446bc`** (two clean builds byte-identical; the suite re-ran
   green at these exact bytes). Toolchain certified first: the
   unmodified cut-2 tree rebuilt to its committed dist hash
   `b50153b5…` exactly.

## 💡 Session idea

**"Zero behavioural change at default" must cover the RENDER budget, not
just the mailbox values — a new feature's presentation has to be inert
in its default/off state byte-for-byte, because the committed proofs
live in that default state.** The seed *word* (`wr_telemetry[56]`) was
timing-neutral: adding it and the LEFT/RIGHT dial logic left P2's
frame-10 START edge landing exactly where cut 2 caught it (state → 1 at
frame 12, measured identical). But the on-title `SEED xxxxxxxx` *text* —
rendered every title frame — added enough per-frame sprite/vblank cost
to shift the boot/START-window alignment by a couple frames, so the
committed proofs' same-frame START edge silently stopped transitioning
(state stuck at 0 through frame 30, every P2-P10 assert failing at once).
The telemetry carried byte-identical; the DISPLAY did not. The fix:
render the dial-0 title with the EXACT committed START line and enrich
it only once dialed off 0 — so the default state is render-cost-identical
and the enrichment only ever runs after a human has dialed. This is cut
2's deferred-bake lesson (presentation work off committed edge frames)
generalised one rung: presentation added *for a new feature* must be a
no-op in the feature's default state, on the vblank budget as well as in
the mailbox — otherwise "additive" quietly moves the frames the pins own.

## Previous-session review

Prior slice: `.sessions/2026-07-16-wickroad-crossroads-cut2.md` (PR
**#173**, the sprite art pass, head `9ae55ef`) — **holds up, and
independently re-verified this session.** Its central claim was
presentation-only carry: 56 sim words + 27 `--assert-text` lines + P1-P9
byte-identical, the art in a *separate* `wr_art` mailbox so the sim
mailbox stays untouched. I didn't take that on faith — stacking cut 3
required cut 2 to be solid, so I (a) confirmed cut 2's "ROM builds"
backfilled GREEN after its dispatch-time 503 (run 29542209699), (b)
rebuilt the cut-2 tree and reproduced its committed dist hash
`b50153b5…` exactly, and (c) `cmp`'d the cut-2-built and cut-3-built P9
watch-logs (all 56 words) — byte-identical, so cut 2's carry claim is
machine-true across an independent build. Its scope discipline (the
mailbox split, the DEFERRED bake off edge frames) is exactly the reading
that saved this cut: cut 2 hit the SAME class of frame-budget bug (its
bake "measurably broke P6's frame-100 pin" until deferred), and that
precedent is what let me recognise cut 3's START-window shift as a
render-budget problem, not a logic one. One small forward note for the
arc: cut 2's card held its own Status at `in-progress` by design; this
cut instead records the work `complete` (the PR alone carries the DRAFT
hold), which reads more honestly once the code and proofs are green —
worth reconciling as a house convention across the stack.
