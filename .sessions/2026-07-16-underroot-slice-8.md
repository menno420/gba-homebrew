# Session — Underroot arc slice 8: winter survival + the survival score

> **Status:** complete

- date: 2026-07-16 (branch `claude/underroot-slice-8`, stacked on
  `claude/underroot-slice-7` @ a3d5866; started ~17:59Z, `date -u`)
- mission: **Underroot arc slice 8 — winter survival + the survival score.**
  The arc's marquee exam (docs/arcs/UNDERROOT.md Decision 3 + the slice-8 row):
  winter has **zero meadow food** (slice 7's `UR_ABUND_WINTER=0`), so the colony
  lives or starves on what the drawn tunnels banked. Winter drains the banked
  store by the surviving foragers' appetite — `store` falls by
  `pop × UR_CONSUME_PER_DAY` each of the `UR_WINTER_DAYS`; the colony **SURVIVES**
  iff the store never runs out, and **SCORE = surviving_pop×1000 + leftover_store**
  (0 if starved out). Everything a pure integer function of the drawn plan, so
  the host mirror recomputes survive/score bit-for-bit; the ROM reports
  winter-store/pop/drain/survive/leftover/score telemetry. The **marquee
  lockstep**: a committed *survivor* dig plan clears winter and a
  deliberately-too-small plan STARVES — both predicted host-side and confirmed
  in ROM telemetry.
- **📊 Model:** Opus 4.8 · high · task-class: gameplay increment
- landing posture: **DRAFT, stacked on #161**, base `claude/underroot-slice-7`.
  Under the standing 2026-07-16 LANDING WALL (PR ready-flips + auto-merge
  classifier-denied); honest draft-park is the terminal state. This is the
  born-red flip that would *release* landing — flipped to `complete` per ritual
  (card completeness ≠ PR ready-flip) — but the PR itself is LEFT DRAFT. No
  merge / ready / auto-merge calls from this session; PRs #153–#161 untouched.
  Land order #153 → #155 → … → #161 → this.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-8.md`, first commit `dc71d6b`), PR
   opened DRAFT immediately, base `claude/underroot-slice-7`.
2. **The winter drain + survival score** in
   `games/underroot-nds/source/ur_sim.{h,c}`:
   - `UR_CONSUME_PER_DAY = 1` — banked food one forager eats per winter day.
   - `ur_winter_store(grid,gran,nurs,seed,season)` — the store carried INTO
     winter: the banked granary store (slice 4) **minus the brood cost the
     nurseries drew** (slice 5's `ur_pop_food` — raising each forager consumes
     `UR_FOOD_PER_ANT` banked food). Since `ur_pop ≤ store/UR_FOOD_PER_ANT`, the
     brood never exceeds the store, so this is a non-negative pure integer.
   - `ur_winter_drain(pop)` = `pop × UR_WINTER_DAYS × UR_CONSUME_PER_DAY` — what
     winter demands.
   - `ur_winter_survives(store,pop)` — pop is constant across the drain (winter
     grows no brood, abundance 0), so the store falls monotonically and "never
     negative across all days" reduces exactly to `store ≥ drain`.
   - `ur_winter_leftover(store,pop)` = `store − drain` if survives else 0.
   - `ur_winter_score(store,pop)` = `surviving_pop×1000 + leftover` (surviving_pop
     == pop on a survived winter), or **0 if starved out** — a gradient that
     rewards banking a MARGIN, not just barely clearing.
   - Six telemetry words `UR_T_WSTORE` (37) / `WPOP` (38) / `WDRAIN` (39) /
     `WSURV` (40) / `WLEFT` (41) / `WSCORE` (42), pushing `UR_T_SPARE` → 43 and
     widening the mailbox to `UR_T_WORDS = 44`. `main.c` threads them (winter
     computed each frame from the live-season economy) and shows a new render
     line `winter  store S  SURVIVE/starve  score N`.
3. **Host mirror** `tools/check-underroot.py`: line-for-line `ur_winter_store` /
   `ur_winter_drain` / `ur_winter_survives` / `ur_winter_leftover` /
   `ur_winter_score` + `prove_winter()` (**1377 cases**) — (1) the day-by-day
   drain recurrence's never-negative == the closed-form `store ≥ drain` for every
   `(store, pop)`, with leftover == the final running store on survival; (2) the
   score gradient monotone non-decreasing in the store with a **single upward
   survival threshold**; (3) the wiring `ur_winter_store == banked − brood ≥ 0`
   for a spread of dig/gran/nurs plans × every season (brood provably never
   exceeds the store); (4) the **marquee** — `marquee_fixture()` builds the two
   committed plans and asserts the survivor clears winter (score 2008) while the
   too-small plan starves (score 0), differing only in banked granaries.
   `ci_fixture()` extended with the spring winter tuple `(wstore 0, wpop 0, drain
   0, surv 1-vacuous, leftover 0, score 0)`. `ur_sim.c` was host-compiled
   (`-std=c11 -Wall -Wextra -Werror`, clean) and **cross-checked bit-equal** to
   the Python mirror on both marquee fixtures and the exhaustive winter core
   (`bad=0` over `pop×store`).
4. **CI** (`nds-underroot-build`): the mailbox watch widened to
   `t:ur_telemetry:44`; the boot proof gains the "no winter exam until there is a
   colony" zeros (t[37]/38/39/42 = 0), the dig proof gains the shallow-plan
   winter zeros (the colony lost to hawks banks nothing forward, score 0); a new
   **proof 4 — THE MARQUEE** drives ONE deterministic replay where the SAME
   colony flips starve→survive as the store grows: a row-1 corridor (cols 4..13,
   all six spring patches, reachable haul 34) + two connected nurseries (brood 6)
   + three granaries → at frame 310 store 12, winter store 0 < drain 4 → **STARVE,
   score 0**; then three MORE granaries → at frame 390 store 24, winter store 12 ≥
   drain 4 with 8 to spare → **SURVIVE, score 2008**. All within spring (t[3]=0),
   so the boot-latency offset never leaves the season; every pin computed from
   the host mirror.
5. Rails held: PUBLIC repo, original IP only, zero pokemon-mod-lab content; no
   `.substrate/` writes; claim/card cite the branch/base SHA (a3d5866);
   timestamps `date -u`.

## Known / honest gaps

- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as slices
  1–7), so `games/underroot-nds` is **not compiled for the ARM target locally** —
  the ROM build + boot/dig/year/winter telemetry proofs are green only when CI
  `nds-underroot-build` runs. What *did* run locally: the host mirror
  (stdlib-only, all proofs green incl. the 1377-case `prove_winter`), a host-`gcc`
  compile of `ur_sim.c` (`-std=c11 -Wall -Wextra -Werror`, clean), and a `gcc`
  harness linking `ur_sim.c` that cross-checked both marquee fixtures + the
  exhaustive winter core bit-equal to the Python mirror. `main.c`'s new
  `draw_meadow` signature (three trailing params — wstore/wsurv/wscore) and the
  six new telemetry writes were audited **by eye** against the header/call site
  (the slice-5/6 lesson: a cross-compile-only signature mismatch is what host
  gcc can't see across the libnds TU); arity confirmed 22-def == 22-call.
- **The winter exam is a live-season FORECAST, evaluated where the marquee digs
  (spring).** `main.c` computes the winter words from the LIVE season's economy
  each frame — so during spring the readout is "given the colony you've built,
  here is the winter verdict", and it becomes the literal verdict once the clock
  reaches winter. The marquee plans are driven and asserted in spring (frames
  < 1024, robustly inside `UR_SPRING` under the ~50-frame boot offset), where the
  economy is the abundance UNIT. Evaluating the exam *at* the winter season is
  degenerate by construction (winter abundance 0 → nothing reachable → store 0),
  which is exactly the arc's point: the store must be **banked before** winter.
  A ROM run that digs a plan AND crosses into the winter season to watch the
  forecast become the verdict live waits on the slice-10 seed dial (patch layout
  is season-dependent, so a fixed dig plan opens different patches per season) —
  the same time-deferred discipline slice 7 kept for the abundance-scaled haul.
- **The vacuous-survive edge is real and left in, honestly.** By the arc doc's
  literal formula (`SURVIVE ⟺ min store ≥ 0`), a colony with **zero** foragers
  (undug, or wiped by hawks) "survives" winter vacuously — there is nothing to
  starve — so `WSURV` (t[40]) reads 1 at boot and on the shallow dig plan. But
  its **SCORE is 0** (surviving_pop 0, leftover 0), so the gradient correctly
  reports a dead colony as worthless. To avoid asserting a confusing "empty
  colony survives", the boot/dig ROM proofs assert the meaningful winter **zeros**
  (store/pop/drain/score) and NOT the `WSURV` flag; the marquee proof drives the
  non-vacuous survive↔starve pair where the flag is meaningful. A later slice
  could gate `WSURV` on `wpop > 0` if the vacuous 1 ever reads as a real survival
  anywhere it matters; today it never scores, so it is left literal to the doc.
- **The brood-cost subtraction is the load-bearing design call.** The doc's
  formula names `store_at_winter_start` and `pop = live foragers` without saying
  whether the store was drawn down by raising the colony. Taking it literally
  (winter store == full banked `ur_store`) makes a living colony **unstarvable**:
  `ur_pop` is store-bound, so any plan with foragers already banks far more than
  its winter drain, and the "too-small plan starves" marquee is impossible. The
  fix is faithful to slice 5's own mechanic — "nurseries **convert** banked food
  into foragers", i.e. the food was consumed — so winter inherits `ur_store −
  ur_pop_food`. That is what creates the real tension the marquee needs: raise a
  colony *and* bank a surplus beyond what growing it cost. `ur_store` /
  `ur_pop_food` themselves are unchanged (slice 4/5's separate readouts hold);
  slice 8 only *combines* them for the winter carry-in.
- **One winter scenario per behaviour in the ROM.** The exhaustive `(store, pop)`
  core, the score gradient, and the wiring are proven host-side for every
  reachable input; the ROM confirms the two committed marquee plans (starve→
  survive in one replay). The buried-vs-shallow route contrast that keeps
  survivors alive is slice 6's proof, reused here unmodified.
- **Stacked on an unlanded base.** Slice 7 (PR #161) is draft-parked at the
  standing wall, so this branch stacks on `claude/underroot-slice-7`, not `main`.
  It lands only after the stack ahead does; order
  #153 → #155 → #156 → #157 → #158 → #159 → #160 → #161 → this.
- **`substrate-gate` red is inherited, not from this slice.** `main` carries 5
  known orphan-doc failures (#151); the check is expected-red on this branch too.
  What must be green — the "NDS Underroot build" job (host proof + ROM build +
  boot/dig/year/winter lockstep) — is this slice's responsibility.

## 💡 Session idea

**The survival SCORE is now a single monotone integer over the whole dig plan —
which makes it the arc's first natural fitness function, and a cheap
`prove_winter`-style search could turn the slice-10 "a fair seed is always
survivable with an optimal plan" assertion from a hand-built claim into a
*proven* one, for free, before slice 10 ever opens.** Every quantity feeding
`ur_winter_score` is a pure `f(dig plan, gran, nurs, seed, season)` and the whole
grid is a tiny integer state (192 cells × three 0/1 layers). Today the marquee
proves *two hand-authored* plans; but because the score is deterministic and
bounded, a host-side hill-climb (or even a bounded exhaustive search over a
restricted plan family — a shaft depth, a granary count, a nursery count) can
compute `max_plan ur_winter_score(seed)` for any seed in well under the mirror's
1-second budget. That unlocks two guards the later slices will otherwise assert
by eye: (1) **the fairness invariant slice 10's row already promises** — "a fair
seed is always survivable with an optimal plan" becomes `assert best_score(seed)
> 0 for every seed in the dial's range", a `prove_winter`-shaped sweep; and (2)
**a balance tripwire** — pin `best_score(UR_SEED)` (the shipped seed) so any
future economy tweak (a granary cap, a brood rate, a consume bump) that
accidentally makes the shipped seed unsurvivable, or trivially maxed, fails CI
loudly instead of silently shifting the whole game's difficulty. This is the
direct successor to slice 7's "pin the two clocks' commensurability" idea: there
the guard pinned a *schedule* invariant; here the guard pins the *outcome*
invariant — that the seed the player actually gets is neither impossible nor
free — which is the one property the survival score exists to make legible and
the one no single scripted plan can prove.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-7.md` (PR #161, head
  `a3d5866`, seasons + the year clock). It shipped the pure `ur_day` /
  `ur_season_of_day` / `ur_abundance` / `ur_season_food` / `ur_hawk_passes` /
  `ur_season_predation` / `ur_season_survivors` layer + `prove_clock()` (802
  cases), and its close-out **idea was the exact hinge this slice needed**: it
  foresaw that `ur_hawk_passes` is a **floor division** where the two clocks (the
  hawk sweep and the season) multiply, a "silent balance lever that every later
  slice inherits and none can see", and asked that slice 8's fairness assert not
  inherit that drift blind. Slice 8 honoured the spirit: the survival score
  reads `ur_season_survivors` (which folds `ur_hawk_passes` in), and this card's
  own idea escalates slice 7's exactly — from pinning the *schedule*
  commensurability to pinning the *outcome* (`best_score(seed) > 0`), the guard
  that makes the floor's silent mercy visible in the one number that matters.
- **Concrete strength observed reviewing the slice-7 code:** slice 7 was
  scrupulous about **not** rewiring the store to the abundance-scaled haul —
  its card flagged, verbatim, that "rewiring the store to drink the seasonal haul
  is the **slice-8 winter drain's** job" and left `ur_store` reading the raw
  `ur_reachable_food`. That restraint is exactly what let slice 8 land cleanly:
  the store/pop chain was a stable, pinned foundation to build the winter carry-in
  on (`ur_store − ur_pop_food`) without disturbing a single slice-1..7 value —
  every pinned economy number (store 4, pop 2, ssurv 0 on the CI fixture) is
  bit-identical, and the winter words are purely additive, as slice 7's own
  additive-layer discipline was.
- **Concrete decision slice 7 handed forward, now resolved:** slice 7's card
  explicitly deferred **the population-regrowth fold** ("`pop_{t+1} = min(pop_t +
  brood_t − predation_t, POP_CAP)` … naturally the slice-8 winter-drain's
  concern, where the store actually depletes tick by tick") to this slice. Slice 8
  makes the honest, tractable call the arc doc's formula actually specifies:
  winter grows **no** brood (abundance 0 → no meadow food → no conversion), so
  the population is *constant* across the drain and the recurrence collapses to
  the closed form `store ≥ drain` — proven equal to the day-by-day fold in
  `prove_winter` (1). The richer between-cull regrowth fold slice 6/7 floated is a
  *growing-season* dynamic, not a winter one; it stays a future-slice concern
  (the seed-dial/balance pass), where population moving on the same clock as
  predation would actually have food to move on.
- **Concrete nit carried, still open:** slice 6→7 flagged a slice-5 `ur_nurse`
  signedness asymmetry (unsigned `col,row` vs `ur_designate`'s `int32_t` +
  explicit `< 0` guard). Slice 8 adds **no** new coordinate entry points (its new
  functions take `store`/`pop`/`season`, not grid coordinates), so — as in slice
  7 — there was nothing to align; it stays the same one-line follow-up for
  whenever the stack next reopens slice 5. Slice 7's honest gaps carry forward
  unchanged and true here (no local NDS build; telemetry pinned from the host
  mirror not a read ROM run; one ROM scenario per behaviour). Its `📊 Model:`
  three-field form is mirrored here at **family** level (`Opus 4.8 · high ·
  task-class: gameplay increment`).
