# Session — Underroot arc kickoff (A2): slice 1, dual-screen skeleton

> **Status:** `in-progress`

- date: 2026-07-16 (branch `claude/underroot-slice-1`; started ~15:01Z, `date -u`)
- mission: **kick off arc A2 — Underroot** (owner NEXT-MENU-2026-07-15 §A2 →
  coordinator order, judgment delegated to sharpen the concept) and execute
  ONE slice end-to-end: the arc design doc + a buildable dual-screen NDS
  skeleton (meadow renders the deterministic hawk schedule; bottom screen digs
  a tile-snap tunnel grid) + the host-side lockstep sim mirror, all on the
  proven Gloamline NDS stack (`--touch` scripting, dual-screen screenshots,
  `check-gloam.py`-style host mirror).
- **📊 Model:** Claude Opus 4.8 · high · arc-kickoff
- landing posture: **PR opened DRAFT and left DRAFT.** The standing 07-16
  permission wall (PR ready-flips + auto-merge are classifier-denied —
  `[Merge Without Review]`, recorded in `docs/PLATFORM-LIMITS.md` and the
  #153/#154 bodies, both parked DRAFT at this same wall) makes honest
  draft-park the terminal state; the land path is an owner ready-click. No
  merge / ready / auto-merge calls from this session; #153/#154 untouched.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-1.md`, rendered by `bootstrap
   claim` so the duplicate-scan can see it), PR opened DRAFT immediately.
2. **Arc design doc** `docs/arcs/UNDERROOT.md` (badged `plan`) + a new
   `docs/arcs/README.md` index (a doc-hygiene reachability root, so the new
   arc doc is not a self-inflicted orphan). Sharpened design calls:
   **tile-snap digging** (not freeform — the grid IS the forager path graph
   and a host-mirrorable integer), **forager pathing emerges** from the
   4-connected dug graph (connect-or-waste / short-or-slow / deep-or-dead),
   a **one-year spring→winter v1.0** with an explicit `SURVIVE`/`SCORE`
   survival model, **determinism-first** so CI can prove `simulate(seed,
   dig_plan) → {survives, score}` host-side, an **~11-slice plan** each with
   its headless proof, and the three-leg proof strategy.
3. **Slice-1 code** — buildable NDS skeleton mirroring the Gloamline layout
   exactly: `games/underroot-nds/` (`Makefile` BlocksDS wrapper,
   `source/ur_sim.h` + `ur_sim.c` pure sim, `source/main.c` dual-screen boot
   + telemetry mailbox `ur_telemetry`). Top screen renders the deterministic
   hawk schedule `ur_hawk_x/active/y = f(seed, season, index)`; bottom screen
   digs a 16×12 tile-snap tunnel grid from the stylus, with `ur_burrow_size`
   BFS connectivity (the forager-pathing seed).
4. **Host mirror** `tools/check-underroot.py` (stdlib-only, <1s): line-for-
   line mirror of `ur_sim.c`, proving the hawk schedule pure/bounded/on-band
   over 150 528 (seed,season,frame) cases and the dig grid's
   touch→cell/idempotent-dig/BFS-connectivity, plus the exact
   dug-total/connected the ROM asserts. Runs green locally.
5. **CI** — new `nds-underroot-build` job in `.github/workflows/rom-builds.yml`
   (mirrors `nds-rom-build`/`nds-brineward-build`): host proof → toolchain →
   ROM build → dual-screen boot proof + the **exact `--touch` dig lockstep**
   (`dug=7 con=6` in `ur_telemetry` == host `ci_fixture()`).
6. Rails held: PUBLIC repo, zero Track-A/pokemon-mod-lab content; original IP
   only; no `.substrate/` writes staged; claims/card cite PRs/SHAs; timestamps
   `date -u`.

## Known / honest gaps

- **No local NDS build.** `DEVKITPRO` is empty and BlocksDS is not installed
  in this container, so `games/underroot-nds` was **not compiled locally** —
  the ROM build is proven only when the CI `nds-underroot-build` job runs. The
  C mirrors the Gloamline `main.c` boot idiom closely to de-risk this, but a
  compile error would surface red in that job (honest, not hidden).
- **Boot-proof is post-landing.** Per the headless-boot convention, the
  dual-screen boot screenshot + dig-lockstep are captured by CI, not
  committed pre-build (no committed proof PNGs this slice — the toolchain to
  generate them isn't present locally).
- **Exact hawk-value pins deferred.** The boot-into-`main()` frame offset is
  deterministic-but-empirical (as Gloamline's `seed=118` was learned from a
  first run); slice-1 CI asserts the hawk bounded/live and proves exact hawk
  determinism host-side, and `--watch-log` emits the trace to pin exact ROM
  hawk values in a follow-up.

## 💡 Session idea

**A new game/arc's very first CI job should self-report its boot-into-`main()`
frame offset, so later slices pin exact schedule values without a human
round-trip.** Every NDS title here re-learns the same fact the hard way:
telemetry `FRAME` lags the tool's `frame_number` by a fixed but
ROM-specific boot latency (Gloamline's `seed=118`, Brineward's `118`), so the
first slice can only use `ge`/bounds asserts and a human must read a run log
to discover the constant before exact pins are safe. Cheap guard: a
one-line CI step that greps the committed `--watch-log` for the first frame
where `FRAME==1` and prints `BOOT_OFFSET=<n>` — then slice 2 pins
`ur_hawk_x` at `tool_frame = target_internal_frame + BOOT_OFFSET` with no
guesswork. Guard recipe: add the grep step to `nds-underroot-build` in
`.github/workflows/rom-builds.yml` reading `ur-boot-trace.csv`; the anchor
functions are `ur_hawk_x`/`ur_hawk_active` in
`games/underroot-nds/source/ur_sim.c` ↔ `tools/check-underroot.py`.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-current-state-rows.md` (PR #152, the
  current-state reconcile). Its close-out idea — *"condensation should move
  receipts, never delete them"* — and its own wrinkle (a self-referential
  "this slice's own PR" row rots on merge) are the same discipline this slice
  leaned on: this card names #153/#154 as the standing-wall precedent by
  number+state rather than re-deriving the wall, and the arc doc dates every
  claim to a merged PR/SHA so a successor reconciling it does confirmation,
  not archaeology. One thing that card flagged and this session honoured: its
  `📊 Model:` line was in the taught three-field form (`model · effort ·
  task-class`) — the checker's `model-line-shape` advisory (it silently
  records NOTHING from a malformed line, as it does for the older
  `2026-07-14-wickroad.md` card) — so this card uses the exact two-`·` payload.
