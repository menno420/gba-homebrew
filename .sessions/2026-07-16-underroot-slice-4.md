# Session — Underroot arc slice 4: granaries + food store

> **Status:** complete

- date: 2026-07-16 (branch `claude/underroot-slice-4`, stacked on
  `claude/underroot-slice-3` @ d8cfb92; started ~16:15Z, `date -u`)
- mission: **Underroot arc slice 4 — granaries + food store.** Add the
  storage layer: a **second stylus verb** (hold R + tap a dug cell)
  designates that dug cell a **granary**; connected granary cells give the
  colony a **capacity**, and the meadow haul the foragers can reach
  (slice 3's reachable patches) is **banked up to that capacity**. The arc's
  connect-or-waste rule extends to storage: an **unreachable granary banks
  nothing** (a designated pocket with no corridor to the mouth adds zero
  capacity), just as an unreachable patch feeds nothing. Proven host-side
  (capacity accounting exact, designation idempotent/monotone on dug cells
  only, disconnected granaries never bank, store == min(reachable food,
  capacity) for every reachable input) plus a ROM `ur_telemetry` store
  lockstep after a scripted haul — the slice-4 row of
  `docs/arcs/UNDERROOT.md`.
- **📊 Model:** Opus family · high · feature build
- landing posture: **PR opened DRAFT and left DRAFT, base
  `claude/underroot-slice-3` (stacked).** The standing 2026-07-16 LANDING
  WALL (PR ready-flips + auto-merge classifier-denied — recorded in
  `docs/PLATFORM-LIMITS.md` and the #153/#155/#156/#157 bodies, all parked
  DRAFT at this same wall) makes honest draft-park the terminal state; the
  land path is an owner ready-click, in stack order
  #153 → #155 → #156 → #157 → this. No merge / ready / auto-merge calls from
  this session; the pre-existing PRs (#153–#157) untouched.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-4.md`), PR opened DRAFT
   immediately, base `claude/underroot-slice-3`.
2. **Pure granary/store layer** in `games/underroot-nds/source/ur_sim.{h,c}`:
   - `ur_designate(grid, gran, col, row)` — mark a **dug** cell a granary
     over a parallel 0/1 `gran` designation layer; returns the changed flag,
     rejects soil / already-designated / out-of-bounds. Idempotent + monotone
     (mirrors `ur_dig`); the `gran` layer leaves the dug graph itself
     unchanged, so slice 1–3 connectivity/pathing is untouched.
   - `ur_gran_count` / `ur_gran_connected` — placed granary cells, and the
     subset **connected** to the mouth (finite `ur_dig_dist` — the *same*
     reachability the burrow BFS defines). Unreachable designated cells are
     excluded: they bank nothing.
   - `ur_gran_capacity` = connected granaries × `UR_GRAN_CAP` (4).
   - `ur_reachable_food` — the haul: sum of amounts over patches whose drop
     cell is reachable (connect-or-waste for patches, slice-3 graph).
   - `ur_store` = `min(reachable food, capacity)` — deposits bank **up to**
     capacity; food over capacity spoils, capacity over food sits empty.
   Integers only — no clock, no RNG, no float — so the host mirror recomputes
   the whole store bit-for-bit.
3. **Meadow + burrow render** (`main.c`): the burrow draws a `G` at each
   designated granary cell; the meadow status line shows `store S/C` (banked
   vs capacity). Input gains the second verb: with `KEY_R` held a stylus tap
   **designates** (else it digs). Four new telemetry words `UR_T_GRANN`
   (placed), `UR_T_GRANCON` (connected), `UR_T_CAP` (capacity), `UR_T_STORE`
   (banked), widening the mailbox to `UR_T_WORDS = 25` (`UR_T_SPARE` → 24).
4. **Host mirror** `tools/check-underroot.py`: line-for-line `ur_designate` /
   `ur_gran_count` / `ur_gran_connected` / `ur_gran_capacity` /
   `ur_reachable_food` / `ur_store` mirror + `prove_granary()` — designation
   discipline (dug-only, idempotent, monotone, OOB-rejecting), exact capacity
   accounting, the **cross-layer guard** (connected-granary count ==
   designated cells inside `ur_burrow_size`'s reachable set — storage reach ==
   forager reach == the one dug graph), `store == min(food, capacity)` with
   both bounds, and an emergence scenario that banks a *capped* haul then
   uncaps it with a second granary. The pinned `ci_fixture()` extended to
   designate one connected + one isolated granary and return the store the ROM
   asserts. The C `ur_sim.c` was compiled and **cross-checked equal** to the
   Python mirror on the fixture (`con=6 forage=(4,1,2) placed 2 connected 1
   cap 4 banked 4 reachfood 6`).
5. **CI** (`nds-underroot-build`): mailbox watch widened to
   `t:ur_telemetry:25`; the boot proof (fresh grid) asserts the companion
   "no store until you dig+designate" (`t:20 == 0`, `t:22 == 0`, `t:23 == 0`);
   the dig proof, after the slice-1 `CI_DIG_PIXELS` shaft+branch+pocket,
   replays two **R+tap** granary designations — cell (9,3) on the connected
   branch and the isolated pocket (0,11) — and asserts `t:20 == 2` (two
   placed), `t:21 == 1` (one connected), `t:22 == 4` (capacity), `t:23 == 4`
   (patch 4's 6-unit haul **capped** at capacity 4), with `dug=7`/`con=6`
   re-asserted unchanged (designation does not dig). The same integers
   `check-underroot.py` computes — the host↔ROM store lockstep.
6. Rails held: PUBLIC repo, original IP only, zero pokemon-mod-lab content;
   no `.substrate/` writes; claim/card cite the branch/base SHA; timestamps
   `date -u`.

## Known / honest gaps

- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as
  slices 1–3), so `games/underroot-nds` is **not compiled locally** for the
  ARM target — the ROM build + boot/dig/store telemetry proofs are green only
  when the CI `nds-underroot-build` job runs. What *did* run locally: the host
  mirror (stdlib-only, all proofs green) and a host-`gcc` compile of `ur_sim.c`
  (`-Wall -Wextra -Werror`, clean) cross-checked bit-equal to the Python
  mirror on the fixture. A libnds/`main.c` compile error would surface red in
  the CI job, not hidden.
- **Store telemetry pinned from the host mirror, not a read ROM run.** The
  asserted `(placed 2, connected 1, cap 4, banked 4)` are `ci_fixture()` as
  computed by `check-underroot.py`; CI proves the ROM matches. If the ROM ever
  diverged the `nds-underroot-build` store asserts turn red — the lockstep is
  the guard, not an assumption.
- **One ROM store scenario (capped, one connected granary).** The pinned
  fixture banks a single capped haul (6 reachable, cap 4 → store 4) plus one
  disconnected granary that banks nothing. The *uncapped* case (capacity ≥
  food → store = food), multi-granary capacity, and the designation discipline
  are proven exhaustively host-side in `prove_granary()`, not in a second
  emulator run — matching the slice-2/3 extend-the-existing-proofs idiom.
- **Season fixed spring (0).** The store threads `season` through the pure
  `ur_reachable_food`/`ur_store` (via `ur_patch`) but the run is pinned to
  season 0 until the slice-7 season clock; `prove_granary()` exercises the
  store over dig+designate plans on the pinned meadow.
- **Storage is instantaneous, not a per-trip deposit.** Slice 4 models the
  *banked total* as a pure function of the dig+designate plan (what the
  reachable haul + connected capacity permit); the frame-by-frame forager
  carry (a trip depositing N units per return) is a slice-5+ economy concern
  (nurseries convert the banked store to population). This keeps slice 4 a
  clean pure accounting layer, provable without a time axis.
- **Stacked on an unlanded base.** Slice 3 (PR #157) is draft-parked at the
  standing wall, so this branch stacks on `claude/underroot-slice-3`, not
  `main`. It lands only after the stack ahead does; order
  #153 → #155 → #156 → #157 → this.

## 💡 Session idea

**When a later slice adds a THIRD consumer of a shared graph, the cheapest
proof it belongs to the arc is to assert it *rejects* the same negatives the
graph already rejects — not just that it accepts the positives.** Slice 3's
idea asked every new consumer of the dug graph to be cross-checked against the
burrow BFS; slice 4 honoured it on the accept side (connected-granary count ==
designated cells in the reachable set). But the higher-value assertion turned
out to be the *reject* side: `prove_granary()` asserts that a designated cell
whose `ur_dig_dist` is `UR_ROUTE_NONE` contributes **exactly zero** capacity —
so an isolated granary pocket banks nothing, the same way an isolated patch
feeds nothing and an isolated dig-cell is never counted. That single negative
assertion catches the whole class of "reachability drift" bugs (a granary that
banks from a pocket the player was told is dead dirt) that a positive-only
capacity check sails straight past. Guard recipe: for every graph-derived
quantity Q that a slice introduces, add one assert of the form *"Q over the
UNreachable set is the identity/zero element"* alongside the positive
accounting — it is one line, it needs no new fixture, and it is where the
connect-or-waste fantasy actually lives. Extend it at slice 6 (hawks predate
route cells): assert a **deep** route cell contributes zero predation exposure,
the exact negative-side mirror of this granary guard, against the same graph.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-3.md` (PR #157, head
  `d8cfb92`, foragers + emergent pathing). It shipped the pure `ur_patch_col`
  / `ur_dig_dist` / `ur_forage` BFS layer + route telemetry and its close-out
  idea was precisely the hinge this slice turned on: *"a pure-sim layer that
  reuses another layer's graph should assert their equivalence"*, and it named
  the exact next step — *"extend it whenever a later slice adds a third
  consumer of the dug graph (granary reach, slice 4)."* Slice 4 did exactly
  that: `ur_gran_connected` reuses `ur_dig_dist` rather than rolling a third
  BFS, and `prove_granary()` asserts the connected-granary count equals the
  designated cells inside `ur_burrow_size`'s reachable set — so storage
  reachability, forager reachability, and burrow connectivity are provably the
  **one** meaning of the dug graph, which is what slice 3 asked for. Its three
  standing honest gaps carry forward unchanged and true here too: *no local
  NDS build* (host mirror + a host-`gcc` `ur_sim.c` compile are the local
  proof; CI is the ROM proof), *telemetry pinned from the host mirror not a
  read ROM run* (the store asserts are `ci_fixture()` host values, CI proves
  the ROM matches), and *season fixed spring* (the store threads `season` but
  pins 0). Its `📊 Model:` three-field form is mirrored here, at **family**
  level per this session's order (`Opus family · high · feature build`, no
  specific model ID). One inheritance held cleanly: slice 3 rode slice 1's
  proven frame windows for its constant route words; slice 4's store words are
  likewise frame-invariant once the last designation registers, so the ROM
  lockstep pins them at a settled frame (230, after all input ends at 195)
  without a new boot-offset guess — the still-open slice-1 self-reporting-
  offset idea stays un-actioned and, for the same reason, still un-needed.
