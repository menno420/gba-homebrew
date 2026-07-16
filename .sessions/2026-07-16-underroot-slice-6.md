# Session — Underroot arc slice 6: hawks predate exposed shallow route cells

> **Status:** complete

- date: 2026-07-16 (branch `claude/underroot-slice-6`, stacked on
  `claude/underroot-slice-5` @ 706876f; started ~17:07Z, `date -u`)
- mission: **Underroot arc slice 6 — hawks predate.** Add the predation layer:
  the hawk shadows that sweep the meadow (slice 1) catch foragers on the
  **exposed** (shallow, near-surface) cells of the forager route, while cells
  buried below the surface are **safe**. Model, pure integers only: a route
  cell at grid row `< UR_SAFE_DEPTH` is EXPOSED, at row `>= UR_SAFE_DEPTH` is
  DEEP; the colony loses `min(exposed route cells, population)` foragers to the
  hawks and `survivors = pop − losses`. The route counted is slice 3's
  shortest dug path (`ur_forage` → the canonical BFS path to the nearest
  reachable patch's drop cell), so predation is the deterministic consequence
  of the tunnel you drew: a route that hugs the surface is hawk-food, burying
  it (digging a deeper-only path so the forager's shortest route dives below
  the exposed band) costs more digging but survives the meadow — the arc doc's
  "Deep or dead" pressure. Proven host-side (catch geometry exact at the
  exposed↔deep boundary; the two `min` regimes — exposure-bound and pop-bound;
  a disconnected/no route predates nothing) plus a ROM `ur_telemetry`
  predation lockstep on the scripted exposed route — the slice-6 row of
  `docs/arcs/UNDERROOT.md`.
- **📊 Model:** Opus 4.x · high · task-class: gameplay increment
- landing posture: **DRAFT, stacked on #159**, base `claude/underroot-slice-5`.
  The standing 2026-07-16 LANDING WALL (PR ready-flips + auto-merge
  classifier-denied — recorded in `docs/PLATFORM-LIMITS.md` and the
  #153/#155/#156/#157/#158/#159 bodies, all parked DRAFT at this same wall)
  makes honest draft-park the terminal state; the land path is an owner
  ready-click, in stack order
  #153 → #155 → #156 → #157 → #158 → #159 → this. No merge / ready / auto-merge
  calls from this session; the pre-existing PRs (#153–#159) untouched.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-6.md`, first commit `f779843`),
   PR opened DRAFT immediately, base `claude/underroot-slice-5`.
2. **Pure predation layer** in `games/underroot-nds/source/ur_sim.{h,c}`:
   - `ur_route_exposed(grid, col, row)` — the EXPOSED (grid row
     `< UR_SAFE_DEPTH`) cells on the **canonical shortest dug path** from the
     mouth to `(col,row)`. The path is reconstructed from a predecessor BFS
     over the **same** 4-connected dug graph `ur_dig_dist` walks (E,W,S,N
     neighbour order, first-reached predecessors), so the path — and thus the
     exposed count — is a deterministic integer both host and ROM compute
     identically. Returns 0 for soil / out-of-bounds / unconnected targets.
   - `ur_forage_exposed(grid, seed, season)` — the exposed cells on the
     forager's route: `ur_forage` picks the nearest reachable patch,
     `ur_route_exposed` counts the shallow cells on the path to its drop cell.
     0 if no patch is reachable (no route to predate).
   - `ur_predation(grid, gran, nurs, seed, season)` =
     `min(ur_forage_exposed, ur_pop)` — each exposed route cell can cost at
     most one forager, and the colony loses no more foragers than it has: the
     min's two binding regimes are **exposure-bound** and **pop-bound**.
   - `ur_survivors(...)` = `ur_pop − ur_predation` (predation ≤ pop, so it
     never underflows) — the population the colony carries into winter (the
     slice-8 survival score will read it).
   New constant `UR_SAFE_DEPTH = 2` (rows `[0,2)` are hawk-exposed — the
   surface and the subsurface drop row). Integers only — no clock, no RNG, no
   float — so the host mirror recomputes predation bit-for-bit.
3. **Meadow render + telemetry** (`main.c`): a new status line
   `hawk  exposed E  lost L  surv S`; three new telemetry words
   `UR_T_EXPOSED` (28), `UR_T_LOST` (29), `UR_T_SURV` (30), pushing
   `UR_T_SPARE` → 31 and widening the mailbox to `UR_T_WORDS = 32`. No new
   stylus verb — predation is a pure deterministic schedule off the drawn
   route, so the L/R/bare-tap verbs are untouched (per the arc doc, hawk
   predation adds no player input).
4. **Host mirror** `tools/check-underroot.py`: line-for-line `ur_route_exposed`
   / `ur_forage_exposed` / `ur_predation` / `ur_survivors` mirror +
   `prove_predation()` (**57604 cases**) — (1) `ur_route_exposed` equals the
   shallow cells on an **independently reconstructed** shortest path that is
   asserted contiguous, all-dug, mouth→target, length `== ur_dig_dist + 1`;
   (2) the **exact exposed↔deep boundary** on single-corridor grids: a
   surface-hugging route to drop `(12,1)` exposes **6**, a buried route to the
   *same* drop exposes **3** — the row-2 cells on the buried path are provably
   NOT counted (the boundary is exactly `UR_SAFE_DEPTH`, and burying below it
   is what saves the foragers); (3) the two `ur_predation` regimes on
   controlled grids, each asserting the winning term **by value**: **pop-bound**
   (shallow route exposing 6, only 2 foragers → losses = pop 2, survivors 0)
   and **exposure-bound** (full grid, shallow shaft exposing 2, 4 foragers →
   losses = exposed 2, survivors 2); (4) emergence — a fresh grid / no route
   predates nothing. `ci_fixture()` extended to return `(exposed 2, lost 2,
   surv 0)`. The C `ur_sim.c` was host-compiled (`-std=c11 -Wall -Wextra
   -Werror`, clean) and **cross-checked bit-equal** to the Python mirror on the
   fixture (`exposed=2 lost=2 surv=0`) and on the geometry contrast
   (`shallow=6 buried=3`).
5. **CI** (`nds-underroot-build`): the mailbox watch widened to
   `t:ur_telemetry:32`; the boot proof asserts the companion "no predation
   until there is a route" (`t:28==0`, `t:29==0`, `t:30==0`); the dig proof
   asserts the predation lockstep settled at frame 260 — `t:28==2` (exposed),
   `t:29==2` (lost), `t:30==0` (survivors) — the ROM's shallow shaft to patch 4
   is fully caught, the same integers `check-underroot.py` computes. The
   existing slice-1..5 asserts are unchanged (predation only ADDS words; dug=7,
   con=6, forage (4,1,2), store (2,1,4,4), pop (2,1,2,4) all still hold).
6. Rails held: PUBLIC repo, original IP only, zero pokemon-mod-lab content; no
   `.substrate/` writes; claim/card cite the branch/base SHA (706876f);
   timestamps `date -u`.

## Known / honest gaps

- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as slices
  1–5), so `games/underroot-nds` is **not compiled for the ARM target
  locally** — the ROM build + boot/dig/predation telemetry proofs are green
  only when CI `nds-underroot-build` runs. What *did* run locally: the host
  mirror (stdlib-only, all proofs green), a host-`gcc` compile of `ur_sim.c`
  (`-std=c11 -Wall -Wextra -Werror`, clean), and a `gcc` harness linking
  `ur_sim.c` that cross-checked the fixture + geometry bit-equal to the Python
  mirror. `main.c`'s new `draw_meadow` signature (three trailing params) and
  telemetry writes were audited **by eye** against the header/call sites (the
  slice-5 lesson: a cross-compile-only signature mismatch is exactly what host
  gcc can't see); a libnds/`main.c` compile error would surface red in CI, not
  hidden.
- **Predation is instantaneous geometry, not yet time-scheduled.** Slice 6
  models the loss as a pure function of the *final* dig+gran+nurs plan: the
  exposed cells on the forager route and the population it can raise, culled
  once. The **per-hawk-pass** scaling — how many of the `f(seed,season,index)`
  sweeps actually catch foragers over a season, the population regrowing from
  nurseries between culls — is the **slice-7 year clock's** concern, the same
  time-deferred discipline slices 4 (storage instantaneous) and 5 (population
  instantaneous) kept. So the live hawk `x`-sweep timing on the top screen is
  rendered (slice 1) but does **not** yet enter the loss arithmetic; only the
  exposure GEOMETRY (which route cells sit in the hawk-exposed band) does.
- **One ROM predation scenario (the exposed/pop-bound route, survivors 0).**
  The pinned `ci_fixture()` route is the shallow shaft to patch 4 (fully
  exposed, whole population caught). The doc's "exposed **vs buried** route"
  contrast is proven **host-side** in `prove_predation()` (buried route exposes
  3 vs shallow 6 to the same drop) rather than in a second emulator run — the
  same "ROM asserts one scenario, host proves all regimes" idiom as slices
  2–5. A second ROM `--touch` plan digging a buried-only route (to show
  survivors > 0 on the ROM) is a cheap follow-up, deferred here to keep the
  slice a tight, robust increment.
- **The pinned meadow can't force a deep ROM route.** Patch 4 sits at the mouth
  column (drop `(8,1)`, dist 1), so on `UR_SEED`/spring any downward dig makes
  patch 4 the nearest reachable patch and the route is unavoidably the shallow
  shaft (exposed 2). The buried-route geometry is therefore exercised on a
  **different** hand-built grid host-side; a seed whose nearest patch is far
  from the mouth (so a buried route is the natural ROM demonstration) waits on
  the slice-10 seed dial.
- **Season fixed spring (0).** `ur_predation`/`ur_survivors` thread `season`
  through `ur_pop` and `ur_forage`, but the run is pinned to season 0 until the
  slice-7 season clock; `prove_predation()` exercises the model over dig plans
  on the pinned meadow.
- **Stacked on an unlanded base.** Slice 5 (PR #159) is draft-parked at the
  standing wall, so this branch stacks on `claude/underroot-slice-5`, not
  `main`. It lands only after the stack ahead does; order
  #153 → #155 → #156 → #157 → #158 → #159 → this.
- **`substrate-gate` red is inherited, not from this slice.** `main` carries 5
  known orphan-doc failures (#151); the check is expected-red on this branch
  too. What must be green — the "NDS Underroot build" job (host proof + ROM
  build + boot/dig/predation lockstep) — is this slice's responsibility.

## 💡 Session idea

**Slice 6 introduces the arc's first quantity that is a *difference of two
independently-computed terms* (`survivors = pop − predation`), and that is a
composition that only stays correct while both terms are static — so the host
mirror should, the moment slice 7 adds the time axis, prove the *per-tick fold*
collapses to this snapshot in the one-tick case.** Right now `ur_pop` and
`ur_predation` are each pure functions of the *final* plan, computed once and
subtracted; the answer is right because nothing moves. But slice 7 makes both
terms move in *opposite* directions on the same clock — nurseries **add**
foragers each season tick, hawks **subtract** them each hawk pass — and a
one-shot `pop − losses` computed from the end-state is then simply the wrong
model (it double-counts foragers that were born after they'd have been eaten,
or eats foragers that weren't born yet). The correct slice-7 shape is a
**fold**: `pop_{t+1} = min(pop_t + brood_t − predation_t, POP_CAP)`, survivors
threaded step by step. The cheap guard that keeps slice 7 honest is one
equivalence proof — **assert the folded survivors over a single degenerate tick
equals slice 6's snapshot `ur_survivors`** — so the temporal model is pinned to
the instantaneous one at its boundary and can't silently drift from it. This is
the natural successor to slice 5's "name which bound binds" idea: there the
binding term was static; here it *migrates over the fold*, and the survival
score (slice 8) reads the fold's endpoint, so pinning the fold-to-snapshot
identity at the one-tick boundary is what lets the slice-8 marquee trust that a
scripted survivor plan really survives.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-5.md` (PR #159, head
  `706876f`, nurseries + population). It shipped the pure `ur_fresh_nurs` /
  `ur_nurse` / `ur_nurs_count` / `ur_nurs_connected` / `ur_pop` / `ur_pop_food`
  layer + pop telemetry, and its close-out idea was exactly the hinge this
  slice turned on: *"for every `min(a,b,…)`-or-cap quantity a slice introduces,
  enumerate its binding cases and add one assert per case that names the winning
  term by value"* — and it explicitly flagged **slice 6 as a `min` of exposure
  and route depth**. Slice 6 honoured that to the letter: `ur_predation =
  min(exposed, pop)` and `prove_predation()` pins **both** regimes to their own
  controlled grid and asserts pop equals the *specific* binding term by value
  (pop-bound: `losses == pop 2`; exposure-bound: `losses == exposed 2`), so a
  refactor that reshuffled the min would fail at least one regime even if the
  single ROM fixture (pop-bound) stayed green.
- **Concrete strength observed reviewing the slice-5 code:** `ur_pop` gets the
  bound-then-cap ordering right — it computes `min(brood, food)` *first* and
  applies `UR_POP_CAP` *after* (`ur_sim.c` L347–348), which is the only correct
  order (capping a term before the min would let a fat brood mask a thin store);
  and `prove_nursery()` earns that with a dedicated **cap** case (5 granaries +
  4 nurseries → `min(12,10)` capped to 8) distinct from the food- and
  brood-bound cases. That three-regime rigor is exactly what made it safe to
  reuse `ur_pop` unmodified as one of the two terms of `ur_predation`.
- **Concrete nit observed:** slice 5's `ur_nurse` diverged from its slice-4
  sibling `ur_designate` in signature — `ur_nurse` takes `uint32_t col, row`
  and returns `uint32_t`, whereas `ur_designate` takes `int32_t col, row` and
  returns `int`. It's *safe* (a negative int passed as `uint32_t` wraps to a
  huge value that the `col >= UR_GRID_W` bound still rejects, and `main.c`
  dutifully casts `(uint32_t)col` at the one call site), but two functions that
  are meant to be exact mirrors of each other reading their coordinates through
  different signedness is a latent trap — a future caller passing a raw
  `int32_t -1` to `ur_nurse` gets "rejected" by wrap-around rather than by the
  explicit `< 0` guard `ur_designate` has. Slice 6 kept its new coordinate
  entry point `ur_route_exposed` on the **`int32_t` + explicit `< 0` guard**
  side (matching `ur_designate`/`ur_dig_dist`), the safer of the two
  conventions, rather than propagating the `ur_nurse` unsigned style. Worth a
  one-line follow-up to align `ur_nurse` to `int32_t`/`int` when the stack next
  touches slice 5.
- Slice 5's honest gaps carry forward unchanged and true here: *no local NDS
  build* (host mirror + `gcc` `ur_sim.c` compile + a link harness are the local
  proof, CI is the ROM proof), *telemetry pinned from the host mirror not a read
  ROM run* (the predation asserts are `ci_fixture()` host values, CI proves the
  ROM matches), *one ROM scenario* (the buried-route contrast proven host-side),
  *season fixed spring*, and *the layer is instantaneous not time-scheduled* —
  slice 5 deferred the per-frame population conversion to the slice-7 clock, and
  slice 6 likewise defers per-hawk-pass predation there, each staying a pure
  accounting layer provable without a time axis. Its `📊 Model:` three-field
  form is mirrored here at **family** level (`Opus 4.x · high · task-class:
  gameplay increment`, no specific model ID). One inheritance held cleanly:
  slice 5 pinned its pop words at a settled frame (260, after input ends 225)
  because they are frame-invariant once the last designation registers; slice 6
  reads its predation words off the same `ur_forage`/`ur_pop` inputs, so they
  are settled at that same frame 260 and are asserted there without a new
  boot-offset guess — the still-open slice-1 self-reporting-offset idea stays
  un-actioned and, for the same reason, still un-needed.
