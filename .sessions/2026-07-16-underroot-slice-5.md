# Session — Underroot arc slice 5: nurseries + population

> **Status:** complete

- date: 2026-07-16 (branch `claude/underroot-slice-5`, stacked on
  `claude/underroot-slice-4` @ a0c2f78; started ~16:37Z, `date -u`)
- mission: **Underroot arc slice 5 — nurseries + population.** Add the
  population layer: a **third stylus verb** (hold L + tap a dug cell)
  designates that dug cell a **nursery**; connected nursery cells brood
  foragers, converting the granary-banked food (slice 4's store) into a
  **population** on a pure, bounded schedule — `pop = min(connected ×
  UR_NURS_BROOD, store / UR_FOOD_PER_ANT)` capped at `UR_POP_CAP`. The arc's
  connect-or-waste rule extends to broodkeeping: a **disconnected nursery
  produces nothing** (a designated pocket with no corridor to the mouth broods
  zero), just as an unreachable granary banks nothing and an unreachable patch
  feeds nothing. Proven host-side (pop deterministic/bounded, the food-bound /
  brood-bound / cap branches, disconnected nurseries excluded) plus a ROM
  `ur_telemetry` pop lockstep over a scripted dig+granary+nursery fragment —
  the slice-5 row of `docs/arcs/UNDERROOT.md`.
- **📊 Model:** Opus family · high · feature build
- landing posture: **PR #159 opened DRAFT and left DRAFT, base
  `claude/underroot-slice-4` (stacked).** The standing 2026-07-16 LANDING
  WALL (PR ready-flips + auto-merge classifier-denied — recorded in
  `docs/PLATFORM-LIMITS.md` and the #153/#155/#156/#157/#158 bodies, all parked
  DRAFT at this same wall) makes honest draft-park the terminal state; the
  land path is an owner ready-click, in stack order
  #153 → #155 → #156 → #157 → #158 → this. No merge / ready / auto-merge calls
  from this session; the pre-existing PRs (#153–#158) untouched.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-5.md`), PR #159 opened DRAFT
   immediately, base `claude/underroot-slice-4`.
2. **Pure nursery/population layer** in
   `games/underroot-nds/source/ur_sim.{h,c}`:
   - `ur_fresh_nurs(nurs)` — initialise the parallel 0/1 nursery designation
     layer (mirrors `ur_fresh_gran`).
   - `ur_nurse(grid, nurs, col, row)` — designate a **dug** cell a nursery;
     returns the changed flag, rejects soil / already-designated / out-of-bounds
     (mirrors `ur_designate`, idempotent + monotone). The `nurs` layer leaves
     the dug graph unchanged, so slice 1–4 connectivity/pathing/storage are
     untouched.
   - `ur_nurs_count` / `ur_nurs_connected` — placed nursery cells, and the
     subset **connected** to the mouth (finite `ur_dig_dist` — the *same*
     reachability the burrow BFS defines). Unreachable designated cells are
     excluded: they brood nothing.
   - `ur_pop(grid, gran, nurs, seed, season)` =
     `min(connected × UR_NURS_BROOD, store / UR_FOOD_PER_ANT)` capped at
     `UR_POP_CAP` — connected nurseries brood the slice-4 store into new
     foragers; **food-bound** when the store is thin, **brood-bound** when it
     is ample, **cap-bound** at the ceiling.
   - `ur_pop_food` = `ur_pop × UR_FOOD_PER_ANT` — the banked food that
     population consumes.
   New constants `UR_POP_CAP = 8`, `UR_FOOD_PER_ANT = 2`, `UR_NURS_BROOD = 3`.
   Integers only — no clock, no RNG, no float — so the host mirror recomputes
   the whole population bit-for-bit.
3. **Meadow + burrow render** (`main.c`): the burrow draws an `N` at each
   designated nursery cell (legend `#soil .dug Ggran Nnurs Mmouth`); the meadow
   status line extends to `food F  store S/C  pop P`. Input gains the third
   verb: with `KEY_L` held a stylus tap **broods** (designates a nursery),
   else `KEY_R` designates a granary, else a bare tap digs. Four new telemetry
   words `UR_T_NURSN` (placed), `UR_T_NURSCON` (connected), `UR_T_POP`
   (population), `UR_T_POPFOOD` (food consumed), reusing the old spare slot and
   widening the mailbox to `UR_T_WORDS = 29` (`UR_T_SPARE` → 28).
4. **Host mirror** `tools/check-underroot.py`: line-for-line `ur_fresh_nurs` /
   `ur_nurse` / `ur_nurs_count` / `ur_nurs_connected` / `ur_pop` /
   `ur_pop_food` mirror + `prove_nursery()` — designation discipline
   (dug-only, idempotent, monotone, OOB-rejecting), the **cross-layer guard**
   (connected-nursery count == designated cells inside `ur_burrow_size`'s
   reachable set — broodkeeping reach == storage reach == forager reach == the
   one dug graph), a **disconnected-nursery-broods-zero** scenario, and the
   three `ur_pop` branches on a controlled grid: **food-bound** (store 4 →
   pop 2), **brood-bound** (one nursery → pop 3 = `UR_NURS_BROOD`), and **cap**
   (huge store + many nurseries → pop 8 = `UR_POP_CAP`). The pinned
   `ci_fixture()` extended to designate one connected + one isolated nursery
   and return the pop tuple the ROM asserts. The C `ur_sim.c` was compiled
   (`-Wall -Wextra -Werror`, clean) and **cross-checked bit-equal** to the
   Python mirror on the fixture (`nursn=2 nurscon=1 pop=2 popfood=4`).
5. **CI** (`nds-underroot-build`): the mailbox watch widened to
   `t:ur_telemetry:29`; the boot proof (fresh grid) asserts the companion
   "no population until you dig+designate a nursery" (`t:24 == 0`,
   `t:26 == 0`); the dig proof extended `--frames 240 → 280` replays two
   **L+tap** nursery designations after the granary ones — cell (10,3) on the
   connected east branch and the isolated pocket (0,11) — and asserts at a
   settled frame 260 `t:24 == 2` (two placed), `t:25 == 1` (one connected),
   `t:26 == 2` (population food-bound at 2), `t:27 == 4` (food consumed), with
   `dug=7`/`con=6`/store re-asserted unchanged (designation does not dig). The
   same integers `check-underroot.py` computes — the host↔ROM population
   lockstep. The `L` key maps through the harness's dynamic `Keys`-enum table
   (the sibling of the `R` slice 4 already drives; an absent key would fail
   loudly, never silently mis-map).
6. Rails held: PUBLIC repo, original IP only, zero pokemon-mod-lab content;
   no `.substrate/` writes (the bootstrap-check guard-fires append was
   restored, not committed); claim/card cite the branch/base SHA; timestamps
   `date -u`.

## Known / honest gaps

- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as
  slices 1–4), so `games/underroot-nds` is **not compiled locally** for the
  ARM target — the ROM build + boot/dig/pop telemetry proofs are green only
  when the CI `nds-underroot-build` job runs. What *did* run locally: the host
  mirror (stdlib-only, all proofs green) and a host-`gcc` compile of
  `ur_sim.c` (`-Wall -Wextra -Werror`, clean) cross-checked bit-equal to the
  Python mirror on the fixture. A libnds/`main.c` compile error would surface
  red in the CI job, not hidden.
- **Population telemetry pinned from the host mirror, not a read ROM run.** The
  asserted `(placed 2, connected 1, pop 2, popfood 4)` are `ci_fixture()` as
  computed by `check-underroot.py`; CI proves the ROM matches. If the ROM ever
  diverged the `nds-underroot-build` pop asserts turn red — the lockstep is the
  guard, not an assumption.
- **One ROM pop scenario (food-bound, one connected nursery).** The pinned
  fixture broods a single food-bound population (store 4 → 2 foragers, one
  connected nursery brood 3) plus one disconnected nursery that broods nothing.
  The *brood-bound* case (ample store → pop = `UR_NURS_BROOD`), the *cap* case
  (pop = `UR_POP_CAP`), and the designation discipline are proven exhaustively
  host-side in `prove_nursery()`, not in a second emulator run — matching the
  slice-2/3/4 extend-the-existing-proofs idiom.
- **Population is instantaneous, not yet time-scheduled.** Slice 5 models the
  population as a pure function of the dig+granary+nursery plan (what the store
  and connected brood *permit* right now); the per-frame conversion over a year
  (a nursery raising N foragers per season tick, the store drawn down over
  time) is the **slice-7 year clock's** concern. This keeps slice 5 a clean
  pure accounting layer, provable without a time axis — the same discipline
  slice 4's "storage is instantaneous, not a per-trip deposit" gap named.
- **Season fixed spring (0).** `ur_pop`/`ur_pop_food` thread `season` through
  the pure store (via `ur_reachable_food`/`ur_patch`) but the run is pinned to
  season 0 until the slice-7 season clock; `prove_nursery()` exercises the
  population over dig+granary+nursery plans on the pinned meadow.
- **Stacked on an unlanded base.** Slice 4 (PR #158) is draft-parked at the
  standing wall, so this branch stacks on `claude/underroot-slice-4`, not
  `main`. It lands only after the stack ahead does; order
  #153 → #155 → #156 → #157 → #158 → this.

## 💡 Session idea

**When a slice's output is a `min()` of two independent bounds plus a cap, the
proof owes one scenario per *binding* bound — not one scenario per function.**
`ur_pop = min(brood, food_ants)` capped at `UR_POP_CAP` has three regimes, and
a single fixture only ever exercises one of them (the CI ROM run is
food-bound). The cheap instinct is to prove the function "works" on that one
fixture and call the arithmetic self-evident; but the bugs live exactly at the
*switch* between regimes — a `<` that should be `<=`, a cap applied before the
min instead of after, a brood term that silently saturates. `prove_nursery()`
pins each regime to its own controlled grid (food-bound: thin store, fat brood;
brood-bound: fat store, one nursery; cap: both terms over the ceiling) and
asserts pop equals the *specific* binding expression (`store // FOOD_PER_ANT`,
`UR_NURS_BROOD`, `UR_POP_CAP`) — so a refactor that reshuffles the min/cap
order fails at least one regime even if the ROM fixture stays green. Guard
recipe: **for every `min(a, b, …)`-or-cap quantity a slice introduces, enumerate
its binding cases and add one assert per case that names the winning term by
value, not just the min's output.** It is three asserts, needs no emulator, and
it is where the "which bound actually fired" bugs that a single fixture sails
past actually live. Carry it to slice 6 (hawk predation, a `min` of exposure
and route depth) and slice 7 (the year clock, where the store is drawn down and
the *binding bound migrates over time* — each season tick can flip pop from
brood-bound to food-bound, so the per-tick assert must name which).

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-4.md` (PR #158, head
  `a0c2f78`, granaries + food store). It shipped the pure `ur_designate` /
  `ur_gran_count` / `ur_gran_connected` / `ur_gran_capacity` /
  `ur_reachable_food` / `ur_store` layer + store telemetry, and its close-out
  idea was precisely the hinge this slice turned on: *"when a later slice adds
  a THIRD consumer of a shared graph, the cheapest proof it belongs to the arc
  is to assert it rejects the same negatives the graph already rejects"* — and
  it named the exact next step, that nurseries would be that consumer. Slice 5
  did exactly that: `ur_nurs_connected` reuses `ur_dig_dist` rather than
  rolling a fourth BFS, and `prove_nursery()` asserts both the accept side
  (connected-nursery count == designated cells in the burrow reachable set) and
  the **reject** side slice 4 flagged as higher-value — a disconnected nursery
  contributes **exactly zero** population, the negative-space mirror of "an
  isolated granary banks nothing / an isolated patch feeds nothing / an
  isolated dig-cell is never counted". So broodkeeping reachability, storage
  reachability, forager reachability, and burrow connectivity are provably the
  **one** meaning of the dug graph — the fourth consumer, held to the same
  guard. Slice 4's five standing honest gaps carry forward unchanged and true
  here: *no local NDS build* (host mirror + a host-`gcc` `ur_sim.c` compile are
  the local proof; CI is the ROM proof), *telemetry pinned from the host mirror
  not a read ROM run* (the pop asserts are `ci_fixture()` host values, CI
  proves the ROM matches), *one ROM scenario* (the other regimes proven
  host-side), *season fixed spring* (pop threads `season` but pins 0), and
  *the layer is instantaneous not time-scheduled* — slice 4 said storage was
  not a per-trip deposit and named nurseries → population as a slice-5+ economy
  concern; slice 5 is that layer, and it in turn defers the per-frame
  conversion to slice 7's year clock, keeping each slice a pure accounting
  layer provable without a time axis. Its `📊 Model:` three-field form is
  mirrored here at **family** level (`Opus family · high · feature build`, no
  specific model ID). One inheritance held cleanly: slice 4's store words are
  frame-invariant once the last designation registers, so it pinned them at a
  settled frame (230) without a boot-offset guess; slice 5's pop words are
  likewise settled once the last L+tap registers, so the ROM lockstep pins them
  at frame 260 (after input ends at 225) — the still-open slice-1
  self-reporting-offset idea stays un-actioned and, for the same reason, still
  un-needed.
