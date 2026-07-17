# Session — Underroot arc slice 2: meadow food patches

> **Status:** `complete`

- date: 2026-07-16 (branch `claude/underroot-slice-2`, stacked on
  `claude/underroot-slice-1` @ caa7a36; started ~15:40Z, `date -u`)
- mission: **Underroot arc slice 2 — meadow food patches.** Add a pure
  `f(seed, season, index)` food-patch layer to the meadow (top screen):
  positions + amounts scattered on the foraging apron BELOW the hawk lanes,
  rendered on the meadow, and proven host-side (patches pure / on-meadow /
  off the hawk lanes / amount-in-range for every reachable input) plus a
  ROM `ur_telemetry` patch-count + total-food lockstep at pinned frames —
  the slice-2 row of `docs/arcs/UNDERROOT.md`.
- **📊 Model:** Claude Opus 4.8 · high · feature build
- landing posture: **PR opened DRAFT and left DRAFT, base
  `claude/underroot-slice-1` (stacked).** The standing 07-16 permission wall
  (PR ready-flips + auto-merge classifier-denied — `[Merge Without Review]`,
  recorded in `docs/PLATFORM-LIMITS.md` and the #153/#154/#155 bodies, all
  parked DRAFT at this same wall) makes honest draft-park the terminal
  state; the land path is an owner ready-click, in stack order
  #153 → #155 → this. No merge / ready / auto-merge calls; #153/#154/#155
  untouched.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-2.md`), PR opened DRAFT
   immediately, base `claude/underroot-slice-1`.
2. **Pure food-patch layer** in `games/underroot-nds/source/ur_sim.{h,c}`:
   `ur_patch(seed, season, index) -> {x, y, amount}` and
   `ur_patch_total(seed, season)`. Patch centres land on the FORAGING APRON
   (`UR_APRON_*` — below the hawk lane band `y >= UR_MEADOW_Y1`, inside the
   screen with room for the render half-extent), amounts in
   `[UR_PATCH_MIN, UR_PATCH_MAX]`. A `FEED` salt keeps the patch hash stream
   independent of the hawk lane stream. Integers only — no clock, no RNG,
   no float — so the host mirror recomputes them bit-for-bit.
3. **Meadow render** (`main.c`): the six patches drawn as `*` glyphs on the
   apron + a `food patches N  store F` status line; two new telemetry words
   `UR_T_PATCHN` (== `UR_PATCH_COUNT`) and `UR_T_PATCHSUM` (total food for
   the pinned meadow), widening the mailbox to `UR_T_WORDS = 18`.
4. **Host mirror** `tools/check-underroot.py`: line-for-line `ur_patch` /
   `ur_patch_total` mirror + `prove_patches()` — every seed 0..255 ×
   season 0..3 × patch index proven deterministic / on-meadow / off the
   hawk lanes / amount-in-range (6 144 cases), and the pinned
   `patch_n=6 patch_sum=34` printed as the ROM assert anchor. Runs green
   locally (stdlib-only).
5. **CI** (`nds-underroot-build`): mailbox watch widened to
   `t:ur_telemetry:18`; the boot proof (frame 250) and dig proof (frame 190)
   now also assert `t:15 == 6` (patch count) and `t:16 == 34` (total
   food) — the host↔ROM patch lockstep, the same integers
   `check-underroot.py` computes.
6. Rails held: PUBLIC repo, original IP only, zero pokemon-mod-lab content;
   no `.substrate/` writes; claim/card cite the branch/base SHA;
   timestamps `date -u`.

## Known / honest gaps

- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as
  slice 1), so `games/underroot-nds` is **not compiled locally** — the ROM
  build + boot/dig/patch telemetry proofs are green only when the CI
  `nds-underroot-build` job runs. The host mirror (which DID run locally,
  stdlib-only) proves the patch math; a compile error would surface red in
  that job, not hidden.
- **`UR_T_PATCHSUM` pinned from the host mirror, not a read ROM run.** The
  asserted total-food value is `ur_patch_total(0x55A, 0)` as computed by
  `check-underroot.py` locally; CI proves the ROM matches it. If the ROM
  ever diverged the `nds-underroot-build` patch asserts turn red — the
  lockstep is the guard, not an assumption.
- **Season fixed spring (0).** Patches thread `season` through the pure
  function but the run is pinned to season 0 until the slice-7 season clock;
  `prove_patches()` already exercises all four seasons so the layer is ready.
- **Stacked on an unlanded base.** Slice 1 (PR #155) is draft-parked at the
  standing wall, so this branch stacks on `claude/underroot-slice-1`, not
  `main`. It lands only after #155 does; stack order #153 → #155 → this.

## 💡 Session idea

**Give every pure meadow/telemetry layer a one-line "spread" assertion, not
just bounds.** `prove_patches()` proves each patch is on-meadow and
off-the-lanes, but a subtly-broken hash (e.g. a shift that zeroes a field)
can still pass every bounds check while collapsing all six patches onto one
cell or one amount — bounded but degenerate, and invisible until a human
looks at a screenshot. Cheap guard: assert the layer's outputs actually
*vary* (this slice asserts `len({amount_i}) >= 2` for the pinned meadow;
extend to distinct patch cells). Guard recipe: the spread asserts live in
`prove_patches()` in `tools/check-underroot.py`; the anchor functions are
`ur_patch` / `ur_patch_total` in that file ↔
`games/underroot-nds/source/ur_sim.c`; add a distinct-cells assert when
slice 3 packs foragers onto patches so two foragers never share a stale
target.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-1.md` (PR #155, the
  arc kickoff + dual-screen skeleton). It shipped the pure hawk schedule +
  dig-grid mirror and flagged three honest gaps this slice inherits: *no
  local NDS build* (same here — the mirror is the local proof, CI is the ROM
  proof), *boot-into-`main()` offset deterministic-but-empirical* (so this
  slice's patch pins ride the same frame-250/190 windows slice 1 proved the
  mailbox live at — the patch words are frame-invariant constants, needing
  no new offset guess), and its own 💡 idea — *the first CI job should
  self-report its boot offset* — which this slice did NOT need to act on
  (the patch words are constant across frames), but slice 3's moving
  foragers will. Its `📊 Model:` three-field discipline (the checker
  silently drops a malformed line) is mirrored here exactly.
