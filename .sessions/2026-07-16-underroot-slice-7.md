# Session — Underroot arc slice 7: seasons + the year clock

> **Status:** complete

- date: 2026-07-16 (branch `claude/underroot-slice-7`, stacked on
  `claude/underroot-slice-6` @ 02e554f; started ~17:30Z, `date -u`)
- mission: **Underroot arc slice 7 — seasons + the year clock.** Add the time
  axis the arc has deferred since slice 4: a `SPRING → SUMMER → AUTUMN →
  WINTER` day counter driven off the frame clock, with the season a pure
  function of the day; **meadow abundance scales by season** — plentiful in
  summer, thinning through autumn, **zero in winter** (the arc doc Decision 3);
  and the **per-hawk-pass predation temporal scaling** slice 6 explicitly
  deferred to "the slice-7 year clock's concern" — a whole season's predation
  scales with how many hawk sweeps actually cross it, still bounded by the
  population. Everything a pure integer function of `(seed, season, frame)` and
  the drawn plan, so the host mirror recomputes the clock and the abundance
  curve bit-for-bit; the ROM reports season/day/abundance/hawk-passes telemetry
  across a whole simulated year. The slice-7 row of `docs/arcs/UNDERROOT.md`.
- **📊 Model:** Opus 4.8 · high · task-class: gameplay increment
- landing posture: **DRAFT, stacked on #160**, base `claude/underroot-slice-6`.
  The standing 2026-07-16 LANDING WALL (PR ready-flips + auto-merge
  classifier-denied) makes honest draft-park the terminal state; the land path
  is an owner ready-click, in stack order
  #153 → #155 → #156 → #157 → #158 → #159 → #160 → this. No merge / ready /
  auto-merge calls from this session; the pre-existing PRs (#153–#160)
  untouched.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-7.md`, first commit `3a26cc9`),
   PR opened DRAFT immediately, base `claude/underroot-slice-6`.
2. **The year clock** in `games/underroot-nds/source/ur_sim.{h,c}`:
   - `ur_day(frame)` = `frame / UR_DAY_FRAMES` (256 frames/day) — the day the
     frame clock is on, free-running.
   - `ur_season_of_day(day)` — the four seasons partition `[0, UR_YEAR_DAYS)`
     **in order** with the fixed decide-and-flag day counts
     (`UR_SPRING_DAYS=4`, `UR_SUMMER_DAYS=6`, `UR_AUTUMN_DAYS=4`,
     `UR_WINTER_DAYS=2` → a 16-day year); any day past the year's end **clamps
     to `UR_WINTER`**, the terminal state the slice-8 survival exam reads.
   - `ur_season_days(season)` — the fixed count per season (used by the pass
     scaling).
3. **Meadow abundance scales by season** (the arc doc Decision 3):
   - `ur_abundance(season)` — a scale numerator over `UR_ABUND_UNIT=4`:
     `SPRING=4` (the **unit baseline**, so a spring run is bit-identical to the
     pre-clock slices 1–6), `SUMMER=6` (plentiful), `AUTUMN=2` (thinning),
     `WINTER=0` (**zero meadow food** — the arc's exam).
   - `ur_season_food(grid, seed, season)` =
     `ur_reachable_food * ur_abundance(season) / UR_ABUND_UNIT` — the reachable
     haul scaled by the season. Spring == the raw haul; winter == 0.
4. **Per-hawk-pass predation temporal scaling** (the slice-6-deferred piece):
   - `ur_hawk_passes(season)` =
     `ur_season_days(season) * UR_DAY_FRAMES / UR_HAWK_PERIOD` — how many whole
     hawk sweeps cross a season (spring 2, summer 3, autumn 2, winter 1).
   - `ur_season_predation(...)` = `min(ur_forage_exposed * ur_hawk_passes(season),
     ur_pop)` — each of the season's passes catches up to `exposed` foragers on
     the exposed route, over the season `exposed*passes`, but never more than the
     `pop` alive. A **one-pass** season collapses exactly to slice 6's
     instantaneous `ur_predation` (the degenerate identity slice 6's card asked
     for; winter has 1 pass and pins it).
   - `ur_season_survivors(...)` = `ur_pop − ur_season_predation` (season
     predation ≤ pop, so no underflow) — the population carried across the
     season; slice 8's winter drain reads the winter value.
   The slice-6 **instantaneous** `ur_predation` / `ur_survivors` are UNCHANGED —
   the temporal layer is purely additive, so every slice-6 pinned value holds.
5. **Live season through the whole run** (`main.c`): the season is now
   `ur_season_of_day(ur_day(frame))`, LIVE — the meadow (hawk schedule + patch
   layout), the forage/economy chain, and every telemetry word thread it instead
   of the pinned spring. Because `UR_DAY_FRAMES=256` keeps every frame the
   slices 1–6 proofs assert (all ≤ 260, i.e. spring day 0/1) inside `UR_SPRING`,
   every pinned slice-1..6 value is **bit-identical** to before. Two render
   lines: the header shows the live season name + day; a new status line shows
   `season  abund A/4  food F`. Six new telemetry words `UR_T_DAY` (31),
   `UR_T_ABUND` (32), `UR_T_SFOOD` (33), `UR_T_HAWKPASS` (34), `UR_T_SPRED`
   (35), `UR_T_SSURV` (36), pushing `UR_T_SPARE` → 37 and widening the mailbox
   to `UR_T_WORDS = 38`; `UR_T_SEASON` (t[3]) is now LIVE.
6. **Host mirror** `tools/check-underroot.py`: line-for-line `ur_day` /
   `ur_season_days` / `ur_season_of_day` / `ur_abundance` / `ur_season_food` /
   `ur_hawk_passes` / `ur_season_predation` / `ur_season_survivors` +
   `prove_clock()` (**802 cases**) — (1) the day clock pure/monotone (day =
   frame//256, the boundary exactly `UR_DAY_FRAMES`); (2) the season partition
   in order with **exact** per-season day counts, no gaps/overlaps, and every
   day past the year clamping to winter; (3) the abundance curve
   `spring=unit < summer`, `autumn < spring`, `winter=0`, and `ur_season_food`
   scaling the haul (spring == raw 34, winter == 0); (4) the per-hawk-pass
   predation min-regime (pop-bound and the new **exposure×passes-bound** where
   two spring passes catch the whole exposed colony) plus the **one-pass ==
   slice-6 snapshot** identity (winter, 1 pass). `ci_fixture()` extended to
   return the spring year tuple `(abund 4, sfood 6, passes 2, spred 2, ssurv
   0)`. The C `ur_sim.c` was host-compiled (`-std=c11 -Wall -Wextra -Werror`,
   clean) and **cross-checked bit-equal** to the Python mirror on the fixture
   and on the year pins (f100 spring/f1100 summer/f2600 autumn/f3700 winter).
7. **CI** (`nds-underroot-build`): the mailbox watch widened to
   `t:ur_telemetry:38`; proofs 1/2 + 2/2 gained the **spring** year-clock
   asserts (frame 250 boot: day 0, abund 4, passes 2, sfood/spred/ssurv 0;
   frame 260 dig: season 0, abund 4, sfood 6, passes 2, spred 2, ssurv 0 — all
   within spring, so the slice-1..6 asserts are re-asserted unchanged); a new
   **proof 3** drives the clock across the whole simulated year (3850 frames, no
   dig) and asserts the season (t:3), the day climbing (t:31 `ge` bounds — the
   exact day is boot-latency-sensitive so a `>=` is used), the abundance scale
   (t:32 — 4/6/2/**0**) and the per-season hawk-pass count (t:34 — 2/3/2/1) at
   four pins comfortably mid-season (SPRING f400 → SUMMER f1800 → AUTUMN f3072 →
   WINTER f3800), so the ~50-frame boot latency never straddles a boundary.
8. Rails held: PUBLIC repo, original IP only, zero pokemon-mod-lab content; no
   `.substrate/` writes; claim/card cite the branch/base SHA (02e554f);
   timestamps `date -u`.

## Known / honest gaps

- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as slices
  1–6), so `games/underroot-nds` is **not compiled for the ARM target
  locally** — the ROM build + boot/dig/year-clock telemetry proofs are green
  only when CI `nds-underroot-build` runs. What *did* run locally: the host
  mirror (stdlib-only, all proofs green incl. the 802-case `prove_clock`), a
  host-`gcc` compile of `ur_sim.c` (`-std=c11 -Wall -Wextra -Werror`, clean),
  and a `gcc` harness linking `ur_sim.c` that cross-checked the fixture + the
  year pins bit-equal to the Python mirror. `main.c`'s new `draw_meadow`
  signature (four trailing params — season/day/abund/sfood) and the six new
  telemetry writes were audited **by eye** against the header/call site (the
  slice-5/6 lesson: a cross-compile-only signature mismatch is what host gcc
  can't see across the libnds TU); arity was confirmed 19-def == 19-call.
- **The economy chain (store/pop) is still the RAW haul, not the seasonal one.**
  Slice 7 exposes `ur_season_food` (the abundance-scaled haul) and proves it,
  but the existing `ur_store` → `ur_pop` → predation chain still reads the raw
  `ur_reachable_food`, not `ur_season_food`. This is deliberate: rewiring the
  store to drink the seasonal haul is the **slice-8 winter drain**'s job (the
  arc doc's Decision 3 survival model — winter's zero abundance is what starves
  the colony), and doing it here would both overreach the slice-7 row and
  disturb the pinned instantaneous chain. Slice 7 is the clock + the abundance
  *curve as a proven quantity*; slice 8 wires it into the drain.
- **Predation scaling is multiplicative, not yet a per-day fold.** Slice 6's
  card floated the fuller model — a per-day fold
  `pop_{t+1} = min(pop_t + brood_t − predation_t, POP_CAP)` with nurseries
  regrowing the population *between* culls. Slice 7 lands the tractable, honest
  first cut: predation scales by the season's hawk-pass count
  (`min(exposed*passes, pop)`), with the **one-pass season pinned equal to the
  slice-6 snapshot** so the temporal model provably reduces to the instantaneous
  one at its boundary. The brood-regrowth fold (population moving in the opposite
  direction on the same clock) is naturally the slice-8 winter-drain's concern,
  where the store actually depletes tick by tick; folding it in here would
  entangle with the survival score before its slice.
- **Abundance shown, not yet driven, in the ROM year proof.** Proof 3 runs the
  clock **undug**, so `ur_season_food` is 0 all year (nothing reachable) — what
  it proves in the ROM is the pure `ur_abundance`/season/day/pass schedule
  (t:32 = 4/6/2/**0**), which is exactly "abundance scales by season, zero in
  winter". The abundance *applied to a real haul* (sfood 6 in spring) is pinned
  in the dig proof and proven for every season host-side in `prove_clock`; a
  ROM run that digs a route AND crosses seasons (to watch sfood scale live)
  waits on the slice-10 seed dial, since patch layout is season-dependent and a
  fixed dig plan opens different patches per season.
- **Exact ROM day is boot-latency-sensitive.** The season/abundance/pass words
  are season-level and robust to the ~50-frame boot offset, but the exact day
  counter (t:31) shifts with it, so proof 3 asserts it with `>=` bounds (day
  climbing 1→6→11→14) rather than exact pins — the same
  deterministic-but-empirical boot-offset discipline slice 1 used for the hawk
  frame counter (`t:2:ge:200`). The exact day is proven pure host-side.
- **Stacked on an unlanded base.** Slice 6 (PR #160) is draft-parked at the
  standing wall, so this branch stacks on `claude/underroot-slice-6`, not
  `main`. It lands only after the stack ahead does; order
  #153 → #155 → #156 → #157 → #158 → #159 → #160 → this.
- **`substrate-gate` red is inherited, not from this slice.** `main` carries 5
  known orphan-doc failures (#151); the check is expected-red on this branch
  too. What must be green — the "NDS Underroot build" job (host proof + ROM
  build + boot/dig/year-clock lockstep) — is this slice's responsibility.

## 💡 Session idea

**The arc now has two clocks that must stay commensurate — the hawk sweep
(`UR_HAWK_PERIOD`, a slice-1 constant) and the season (`UR_DAY_FRAMES ×
season_days`, slice-7 constants) — and `ur_hawk_passes` is the *only* place they
multiply, so its integer floor is a silent balance lever that every later slice
inherits and none of them can see.** `ur_hawk_passes(season) = season_days *
UR_DAY_FRAMES // UR_HAWK_PERIOD` is a floor division: a season that is 1.9 hawk
periods long counts **1** pass, not 2, and the 0.9 remainder of a sweep — a hawk
that *is* crossing when the season ends — is silently dropped. Today that is
benign (every season here is a clean multiple: 4·256/512=2, 6·256/512=3, …), but
it is exactly the kind of quantity the **slice-10 seed dial + balance** pass will
tune, and the moment someone picks day counts that aren't clean multiples of the
hawk period, `ur_hawk_passes` starts rounding predation *down* by up to a whole
sweep per season — a hidden mercy the survival score (slice 8) will bake into
its "is this seed fair" assertion without anyone choosing it. The cheap guard
that keeps the balance pass honest is one **invariant test that pins the
commensurability**: assert that `sum over seasons of ur_hawk_passes(season)`
equals `UR_YEAR_DAYS * UR_DAY_FRAMES // UR_HAWK_PERIOD` — i.e. that summing the
per-season floors loses no whole pass across the year (no season boundary
straddles a sweep and drops it). It holds now (2+3+2+1 = 8 = 16·256//512); the
day it *stops* holding is the signal that the two clocks have gone
incommensurate and the predation total has quietly changed — precisely the
drift the slice-8 marquee and the slice-10 fairness assert must not inherit
blind. This is the natural successor to slice 6's "pin the fold-to-snapshot
identity at the one-tick boundary" idea: there the guard pinned the *temporal*
model to the *instantaneous* one; here the guard pins the *per-season* sum to
the *whole-year* total, so neither the fold nor the floor can silently leak a
pass.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-6.md` (PR #160, head
  `02e554f`, hawk predation on exposed shallow route cells). It shipped the pure
  `ur_route_exposed` / `ur_forage_exposed` / `ur_predation` / `ur_survivors`
  layer + `prove_predation()` (57604 cases) and its close-out **idea was the
  exact hinge this slice turned on**: it foresaw that slice 7 makes `ur_pop` and
  `ur_predation` *move in opposite directions on the same clock*, that a one-shot
  `pop − losses` from the end-state would then be the wrong model, and it asked
  for one guard — *"assert the folded survivors over a single degenerate tick
  equals slice 6's snapshot `ur_survivors`"*. Slice 7 honoured that to the
  letter: `ur_season_predation` is the temporal fold's first term, and
  `prove_clock` pins **the one-pass season (winter) equal to slice 6's
  instantaneous `ur_predation`** on two independent grids — the fold-to-snapshot
  identity, exactly as asked, at the boundary where `passes == 1`.
- **Concrete strength observed reviewing the slice-6 code:** `ur_route_exposed`
  reconstructs the canonical shortest path from a **first-reached predecessor
  BFS over the same (E,W,S,N) neighbour order** `ur_dig_dist` walks
  (`ur_sim.c` L381–408), and `prove_predation` earns that determinism by
  cross-checking the exposed count against an **independently reconstructed**
  path (`_ref_route`) asserted contiguous, all-dug, mouth→target, and length
  `== ur_dig_dist + 1`. That "two independent BFS agree on one integer" rigour
  is exactly what let slice 7 reuse `ur_forage_exposed` unmodified as the
  multiplicand of the pass count — the geometry was already pinned bit-for-bit,
  so multiplying it by `ur_hawk_passes` needed no new geometry proof, only the
  min-regime + the identity.
- **Concrete nit observed:** slice 6's telemetry comment for the season word
  (`ur_sim.h` L281, `UR_T_SEASON` — *"season (0=spring; fixed slice 1)"*) went
  stale the moment this slice made `t[3]` live, and slice 6 (correctly, for its
  scope) left `main.c` writing `ur_telemetry[UR_T_SEASON] = 0;` as a literal
  rather than a named `UR_SPRING`. Both are *safe* — the literal 0 **is**
  spring — but a reader can't tell "0 because the clock says spring" from "0
  because the season is hardwired". Slice 7 resolved it: `t[3]` now carries the
  live `season`, the write is `= season`, and the header comment reads "LIVE".
  The slice-5 `ur_nurse` signedness nit slice 6 flagged (unsigned `col,row` vs
  `ur_designate`'s `int32_t` + explicit `< 0` guard) is **untouched here** —
  slice 7 adds no coordinate entry points (its new functions take `season` /
  `frame`, not grid coordinates), so there was nothing to align; it stays a
  one-line follow-up for whenever the stack next reopens slice 5.
- Slice 6's honest gaps carry forward unchanged and true here: *no local NDS
  build* (host mirror + `gcc` `ur_sim.c` compile + link harness are the local
  proof, CI is the ROM proof), *telemetry pinned from the host mirror not a read
  ROM run* (the year pins are `check-underroot.py` values, CI proves the ROM
  matches), and *one ROM scenario per behaviour* (the abundance-scaled *haul*
  contrast is proven host-side, the ROM year proof shows the pure abundance
  curve). Its `📊 Model:` three-field form is mirrored here at **family** level
  (`Opus 4.8 · high · task-class: gameplay increment`). One inheritance held
  cleanly: slice 6 read its predation words off settled `ur_forage`/`ur_pop`
  inputs at frame 260; slice 7's spring year-clock words are settled at that
  same frame and asserted there unchanged, and the *new* across-the-year proof
  is the first Underroot proof to deliberately step the frame clock far past
  boot (3850 frames) — where it inherits, rather than re-litigates, slice 1's
  boot-offset discipline by using `>=` day bounds and mid-season season pins.
