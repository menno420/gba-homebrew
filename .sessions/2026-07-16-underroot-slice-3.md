# Session — Underroot arc slice 3: foragers + emergent pathing

> **Status:** `complete`

- date: 2026-07-16 (branch `claude/underroot-slice-3`, stacked on
  `claude/underroot-slice-2` @ 84f8c28; started ~15:57Z, `date -u`)
- mission: **Underroot arc slice 3 — foragers + emergent pathing.** Add the
  forager route layer: a pure BFS **shortest dug-path** from the entrance
  mouth to the nearest **reachable** meadow patch (each patch projects to a
  burrow drop cell in its own column, one row below the seam), with a
  round-trip route length in the telemetry mailbox — and the arc's core
  guarantee, **a disconnected patch is never visited** (its drop cell is not
  dug/connected, so the forager can't reach it). Proven host-side (BFS
  shortest-path pure/monotone, nearest-wins, disconnected-never-visited for
  every reachable input) plus a ROM `ur_telemetry` route lockstep at pinned
  frames — the slice-3 row of `docs/arcs/UNDERROOT.md`.
- **📊 Model:** Claude Opus 4.8 · high · feature build
- landing posture: **PR opened DRAFT and left DRAFT, base
  `claude/underroot-slice-2` (stacked).** The standing 2026-07-16 landing
  wall (PR ready-flips + auto-merge classifier-denied — `[Merge Without
  Review]`, recorded in `docs/PLATFORM-LIMITS.md` and the #153/#155/#156
  bodies, all parked DRAFT at this same wall) makes honest draft-park the
  terminal state; the land path is an owner ready-click, in stack order
  #153 → #155 → #156 → this. No merge / ready / auto-merge calls from this
  session; the pre-existing PRs (#153/#154/#155/#156) untouched.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-3.md`), PR opened DRAFT
   immediately, base `claude/underroot-slice-2`.
2. **Pure forager-route layer** in `games/underroot-nds/source/ur_sim.{h,c}`:
   - `ur_patch_col(patch)` — a meadow patch's burrow **drop column**
     (`patch.x / UR_CELL`, clamped to the grid). The drop cell is that
     column at `UR_DROP_ROW` (row 1, one below the seam): to gather a patch
     a forager must tunnel to just beneath it, so the bare mouth (the
     doorway, row 0) feeds nothing until you dig.
   - `ur_dig_dist(grid, col, row)` — BFS **shortest** dug path (in cells)
     from the mouth over 4-connected dug cells to `(col,row)`, or
     `UR_ROUTE_NONE` when the target is soil or unreachable (the same graph
     `ur_burrow_size` walks — pathing *emerges* from the drawn tunnel).
   - `ur_forage(grid, seed, season)` — the nearest reachable patch: scans
     all `UR_PATCH_COUNT` patches, returns `{index, dist, route}` for the
     smallest `dist` (ties → lowest index), `route = 2*dist` (out and back),
     or all `UR_ROUTE_NONE` when no patch's drop cell is reachable.
   Integers only — no clock, no RNG, no float — so the host mirror recomputes
   the whole route bit-for-bit.
3. **Meadow render** (`main.c`): the nearest-patch target is marked (`o`) and
   a `forager -> patch N route L` / `no forage route (dig to a patch)` status
   line drawn; three new telemetry words `UR_T_ROUTEI` (nearest reachable
   patch index), `UR_T_ROUTED` (one-way dug distance) and `UR_T_ROUTELEN`
   (round-trip route length), widening the mailbox to `UR_T_WORDS = 21`
   (`UR_T_SPARE` shifted to 20).
4. **Host mirror** `tools/check-underroot.py`: line-for-line `ur_patch_col` /
   `ur_dig_dist` / `ur_forage` mirror + `prove_forage()` — BFS shortest-path
   correctness (path length matches an independent Dijkstra/BFS, monotone,
   never shorter than Manhattan), nearest-wins tie-break, and the arc
   guarantee that a **disconnected** patch's drop cell is `UR_ROUTE_NONE`
   (never visited), over many scripted dig plans; plus the pinned lockstep
   `ci_fixture()` extended to also return the forage route the ROM asserts.
5. **CI** (`nds-underroot-build`): mailbox watch widened to
   `t:ur_telemetry:21`; the boot proof (fresh grid, frame 250) asserts
   `t:17 == 0xFFFFFFFF` (no patch reachable before any dig — the emergent
   "no route yet"), and the dig proof (the slice-1 `CI_DIG_PIXELS` shaft +
   east branch + isolated pocket, frame 190) asserts `t:17 == 4`,
   `t:18 == 1`, `t:19 == 2` — the forager reaches patch 4 (col 8, drop
   `(8,1)` on the shaft) at one cell out, two round trip, and the other five
   patches (disconnected columns) are provably never visited. The same
   integers `check-underroot.py` computes — the host↔ROM route lockstep.
6. Rails held: PUBLIC repo, original IP only, zero pokemon-mod-lab content;
   no `.substrate/` writes; claim/card cite the branch/base SHA; timestamps
   `date -u`.

## Known / honest gaps

- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as
  slices 1–2), so `games/underroot-nds` is **not compiled locally** — the ROM
  build + boot/dig/route telemetry proofs are green only when the CI
  `nds-underroot-build` job runs. The host mirror (which DID run locally,
  stdlib-only) proves the route math; a compile error would surface red in
  that job, not hidden.
- **Route telemetry pinned from the host mirror, not a read ROM run.** The
  asserted route values are `ur_forage(...)` as computed by
  `check-underroot.py` locally; CI proves the ROM matches them. If the ROM
  ever diverged the `nds-underroot-build` route asserts turn red — the
  lockstep is the guard, not an assumption.
- **One ROM route scenario (patch 4, dist 1).** The pinned meadow's mouth-
  column patch (patch 4, col 8) is the nearest the slice-1 dig fixture can
  reach, so the ROM lockstep pins a dist-1 route; distances > 1, a distant
  patch winning, and nearest-wins tie-breaks are proven exhaustively
  host-side (`prove_forage()`), not in a second emulator run (matching
  slice 2's extend-the-existing-proofs idiom rather than adding a run).
- **Season fixed spring (0).** The route threads `season` through the pure
  `ur_forage` (via `ur_patch`) but the run is pinned to season 0 until the
  slice-7 season clock; `prove_forage()` exercises the route over dig plans
  independent of season.
- **Stacked on an unlanded base.** Slice 2 (PR #156) is draft-parked at the
  standing wall, so this branch stacks on `claude/underroot-slice-2`, not
  `main`. It lands only after the stack ahead does; order
  #153 → #155 → #156 → this.

## 💡 Session idea

**A pure-sim layer that reuses another layer's graph should assert their
equivalence, not just each in isolation.** `ur_dig_dist` (slice 3) and
`ur_burrow_size` (slice 1) both BFS the same 4-connected dug graph, so a
subtly-broken `ur_dig_dist` — e.g. a neighbour offset typo that walks a
diagonal or drops an edge — can still pass a bounds/idempotence check while
silently disagreeing with the connectivity the player already trusts. The
cheap, high-value guard `prove_forage()` uses: assert the two layers are
*consistent* — "the count of cells with a finite `ur_dig_dist` equals
`ur_burrow_size`" — so any divergence between the reachability the burrow
readout shows and the reachability the forager route walks turns a check
red, before a player ever sees a forager route to a cell the burrow says is
unreachable. Guard recipe: the cross-layer equivalence assert lives in
`prove_forage()` in `tools/check-underroot.py`; the anchor functions are
`ur_dig_dist` / `ur_burrow_size` in that file ↔
`games/underroot-nds/source/ur_sim.c`. Extend it whenever a later slice
adds a third consumer of the dug graph (granary reach, slice 4; hawk
exposure of route cells, slice 6): assert the new layer's reachability
against the same BFS, so the graph has exactly one meaning across the arc.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-2.md` (PR #156, the
  meadow food patches). It shipped the pure `ur_patch(seed,season,index)`
  layer + `PATCHN`/`PATCHSUM` telemetry and its close-out idea was exactly
  the discipline this slice needed: *give every pure layer a "spread"
  assertion, not just bounds* — and it named the specific next step, *"add a
  distinct-cells assert when slice 3 packs foragers onto patches so two
  foragers never share a stale target."* Slice 3 honoured the spirit: rather
  than only bounding the route, `prove_forage()` proves `ur_dig_dist` equals
  an **independent** BFS (a second source of truth, not a re-run of the same
  code) and that the nearest-patch selection is non-degenerate (a scripted
  corridor makes a *distant* patch win, not always the mouth-column one) —
  the anti-degenerate guard slice 2 asked for, generalised from "spread" to
  "cross-checked against a reference." Its three honest gaps carry forward
  unchanged: *no local NDS build* (the mirror is the local proof, CI is the
  ROM proof — same here), *telemetry pinned from the host mirror not a read
  ROM run* (the route asserts are `ur_forage(...)` host values, CI proves
  the ROM matches), and *season fixed spring* (the route threads `season`
  but pins season 0). Its `📊 Model:` three-field form (`model · effort ·
  task-class` — the checker silently drops a malformed line) is mirrored
  here exactly. One wrinkle slice 2 flagged and this slice inherited cleanly:
  because it *reused* slice 1's frame-250/190 windows for its constant patch
  words, slice 3's route words — also frame-invariant once the last dig
  registers — ride the same proven windows, needing no new boot-offset guess
  (the still-open slice-1 idea of self-reporting the boot offset stays
  un-actioned, and still un-needed, for the same reason).
