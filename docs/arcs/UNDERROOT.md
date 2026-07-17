# Underroot — arc design doc (A2)

> **Status:** `plan` — the design of record for the Underroot arc.
> Source code and merged slices always win over this file; it states the
> plan and the decide-and-flag design calls, and is updated as slices land.
> Kicked off 2026-07-16 (owner NEXT-MENU-2026-07-15 §A2 → coordinator order,
> judgment delegated to sharpen the concept). Precedent stack: the Gloamline
> NDS arc (PRs #50–#75, `games/gloamline-nds/`, `tools/check-gloam.py`).

## The concept (A2, verbatim seed)

> NDS dual-screen-native burrow-colony survival. Top screen = meadow (food
> patches, hawk shadows on a pure f(seed, season, index) schedule); bottom
> screen = stylus-dug burrow cross-section (nurseries, granaries, forager
> routes); winter is survivable only if the drawn tunnels store enough.

The two screens **are** the game, not a HUD split: the meadow (top) is the
world you cannot touch, only watch and time; the burrow (bottom) is the world
you draw with the stylus. The whole tension is the seam between them — food
is up in the meadow, safety and storage are down in the dirt, and the only
way food crosses the seam is down a tunnel **you** dug. Winter is the exam:
no meadow food, and the colony lives or starves on what the tunnels banked.

---

## Refined core loop

### Decision 1 — tile-snap digging (RECOMMENDED), not freeform

**Verdict: tile-snap.** The stylus paints a coarse grid of dig cells
(slice 1: 16 cols × 12 rows of 16px cells on the 256×192 bottom LCD); a
tap or a drag toggles the cells under the stylus from soil to dug. The
input *feels* like freehand drawing (drag and the tunnel follows the pen),
but the **state** is a clean integer grid. Four reasons this beats
per-pixel freeform:

1. **Determinism is the whole arc.** A tile grid is a small exact integer
   state a host-side Python mirror can hold bit-for-bit; a freeform stroke
   is a sub-pixel polyline whose rasterization is a second source of truth
   that host and ROM must agree on pixel-exactly — a determinism liability
   for zero design gain.
2. **The grid IS the forager path graph.** Cells are graph nodes, dug
   neighbours are edges. Forager pathing (slice 3) is BFS/shortest-path over
   this graph — it *emerges* from what you drew, no separate navmesh.
3. **Storage and population are counts.** Granary capacity = dug cells
   designated granary × per-cell capacity; nursery output scales with
   nursery cells. Clean integers, trivially provable, legible to the player.
4. **The hardware agrees.** The NDS digitizer is a single-touch resistive
   panel; pixel-precise freehand is imprecise and fiddly. Snapping a drag to
   a forgiving 16px cell is more legible on-screen and more forgiving in the
   hand — and a cross-section of soil reads naturally as blocks.

The fantasy ("draw your burrow") is fully preserved; only the *representation*
is quantized. (A later polish slice may sub-sample the grid finer — the design
is grid-size-agnostic — but never goes freeform.)

### Decision 2 — forager pathing EMERGES from the drawn tunnel graph

A cell counts as **colony** only if it is 4-connected back to the entrance
mouth (the meadow↔burrow seam cell, pre-dug). `ur_burrow_size` (slice 1) is
the BFS that measures this; a dug pocket with no corridor to the mouth is
**not** colony — foragers cannot reach it, so a granary drawn there banks
nothing. From slice 3, foragers walk the **shortest dug path** from the mouth
to the nearest reachable food drop and back; from slice 6, path cells near
the surface are **exposed** to hawks while deep cells are safe. So three
design pressures fall directly out of the graph you draw:

- **Connect or waste** — an unconnected chamber is dead dirt.
- **Short or slow** — a longer route is a slower forager (fewer trips/season).
- **Deep or dead** — a route that hugs the surface is hawk-food; burying it
  costs more digging but survives the meadow.

No pathing AI is *scripted*; it is the deterministic consequence of the graph.

### Decision 3 — a one-year spring→winter v1.0, with an explicit survival score

v1.0 is exactly **one year**: `SPRING → SUMMER → AUTUMN → WINTER`, each a
fixed number of days (decide-and-flag; slice 7 fixes the counts). Meadow food
abundance is a pure function of season and the `f(seed, season, index)` patch
schedule — plentiful in summer, thinning through autumn, **zero in winter**.
Each day foragers make trips out drawn routes, carry food to granaries;
nurseries (slice 5) convert banked food into new foragers; hawks (slice 6)
cull foragers caught on exposed routes.

**Survival score (explicit model).** Let, at the first frame of winter:
`store` = total food banked in reachable granaries, `pop` = live foragers.
Winter drains the store: each winter day the colony eats
`pop × CONSUME_PER_ANT_PER_DAY`. The colony **survives** iff the running
store never hits zero across all `WINTER_DAYS`:

```
store_0 = store_at_winter_start
store_{d+1} = store_d − pop_d × CONSUME_PER_ANT_PER_DAY      (pop may fall as it starves)
SURVIVE  ⟺  min_d store_d ≥ 0
SCORE    =  surviving_pop × 1000 + leftover_store            (0 if starved out)
```

The score is a gradient, not a coin flip: it rewards banking a margin, not
just barely clearing. The headline number the player chases (and the EEPROM
best, slice 9) is `SCORE`; the binary the CI marquee asserts is `SURVIVE`.

### Decision 4 — determinism-first, so CI can PROVE a dig plan survives

Every sim quantity is a pure integer function of `(seed, season, index)` and
the **dig plan** (the tunnel grid + chamber designations). No wall clock, no
runtime RNG, no host/ROM float drift. Concretely: the meadow schedules
(hawks slice 1, patches slice 2), the forager step (slice 3), the economy
(slices 4–5), predation (slice 6), and the winter drain (slice 8) are all
in `ur_sim.c`, mirrored line-for-line in `tools/check-underroot.py`. That
makes the arc's defining proof possible:

> **`simulate(seed, dig_plan) → {survives, score}` is a pure function**, and
> the host mirror computes it identically to the ROM. So a *scripted* dig
> plan can be proven — in <1s of host Python, and again in the ROM — to
> survive (or to starve) winter, deterministically, in CI.

---

## Slice plan (~11 slices to v1.0)

Gloamline precedent: ~PRs #50–#75. Each slice states its **headless proof**.

| # | Slice | Headless proof |
|---|---|---|
| 1 | **Dual-screen skeleton** (THIS SLICE) — meadow renders the deterministic hawk schedule `f(seed,season,index)`; bottom-screen stylus digs a tile-snap tunnel grid; telemetry mailbox; host mirror of the hawk schedule + dig-grid connectivity. | check-underroot.py exhaustive (hawk pure/bounded/on-band; dig idempotent/monotone; burrow BFS). nds-headless-check dual-screen boot shots + the **exact `--touch` dig lockstep** (dug=7/con=6 in `ur_telemetry` == host `ci_fixture()`). |
| 2 | **Meadow food patches** — patch positions/amounts on `f(seed,season,index)`; rendered on top screen. | host mirror: patches pure, on-meadow, off the hawk lanes; ROM telemetry patch-count at pinned frames. |
| 3 | **Foragers + emergent pathing** — BFS shortest path over the dug graph from mouth to nearest reachable patch and back; route-length telemetry. | host mirror: ROM forager path == host shortest-path for scripted dig plans; a disconnected patch is never visited. |
| 4 | **Granaries + food store** — a second stylus verb designates dug cells granary; deposits bank food up to capacity. | host mirror: capacity accounting exact, unreachable granaries bank nothing; ROM store telemetry after a scripted haul. |
| 5 | **Nurseries + population** — nursery cells convert banked food to foragers on a pure schedule. | host mirror: pop dynamics deterministic/bounded; ROM pop telemetry over a scripted year fragment. |
| 6 | **Hawks predate** — hawk shadows catch foragers on *exposed* (shallow) route cells; deep routes are safe. | host mirror: catch geometry exact (exposed↔deep boundary); ROM forager-loss telemetry on a scripted exposed vs buried route. |
| 7 | **Seasons + the year clock** — spring→summer→autumn→winter day counter; meadow abundance scales by season. | host mirror: season boundaries + abundance curve pure; ROM season/day telemetry across the year. |
| 8 | **Winter survival + the survival score** — winter drains the store; `SURVIVE`/`SCORE` per the model above. | **the marquee lockstep**: a committed *survivor* dig plan clears winter and a deliberately-too-small plan STARVES — both predicted exactly host-side and confirmed in ROM telemetry. |
| 9 | **EEPROM best** — best `SCORE` / furthest season persisted via card EEPROM. | host mirror: save encode/decode roundtrip + golden bytes + every bad blob rejects to fresh (gl_save pattern); ROM `--battery-in/out` cycle. |
| 10 | **Seed dial + balance** — pick/scan the year seed; a difficulty/escalation pass. | host mirror: seed→schedule wiring pure; ROM seed-select telemetry; balance asserts (a fair seed is always survivable with an optimal plan). |
| 11 | **Audio + polish → v1.0** — synthesized PSG cues (forager return, hawk cry, winter toll), HUD/cross-section polish. | host mirror: audio decision layer pure/PSG-legal (gl_amb pattern); ROM cue telemetry; final dual-screen shots. |

---

## Proof strategy (what CI actually asserts)

Three legs, all headless, mirroring the Gloamline stack that this arc reuses:

1. **`tools/nds-headless-check.py` — dual-screen screenshots + `--touch`
   scripting.** Boots the shipped `.nds` in DeSmuME, drives scripted stylus
   digs and button input, screenshots BOTH screens (asserted non-blank), and
   ELF-resolves the `ur_telemetry` mailbox to `--assert-watch` exact values.
   *Slice 1 asserts:* magic words `UNDR`/`ROOT`, the frame counter running,
   grid dims, the hawk-x bounded to the meadow — and the **exact dig
   lockstep** below. (Exact hawk-*value* pins wait on the observed
   boot-into-`main()` offset, which is deterministic-but-empirical — the same
   reason Gloamline's `seed=118` was learned from a first run, not guessed;
   `--watch-log` emits the trace to pin them in a follow-up. The *exact* hawk
   determinism is meanwhile proven exhaustively host-side.)

2. **`tools/check-underroot.py` — the host-side lockstep sim mirror** (the
   `check-gloam.py` pattern). A line-for-line Python mirror of `ur_sim.c`
   that proves the meaningful invariants for **every reachable input**, not
   the handful a replay touches. *Slice 1 asserts:* the hawk schedule is a
   pure `f(seed,season,index)` — recompute-equal, ON for exactly the first
   `UR_HAWK_SWEEP` of every `UR_HAWK_PERIOD`, x sweeping the full meadow
   width monotonically, lane y in-band — over 256 seeds × 4 seasons × a dense
   frame sweep (150 528 cases); and the dig grid — touch→cell over every
   bottom-LCD pixel, `ur_dig` idempotent+monotone, and `ur_burrow_size` BFS
   connectivity where a corridor grows the burrow by one and an isolated
   pocket is never counted. **Mirror rule:** any change to `ur_sim.c` lands
   in `check-underroot.py` the same PR (enforced by both being CI steps that
   would diverge). From slice 8 the mirror runs the full
   `simulate(seed, dig_plan)` and asserts winter survival.

3. **EEPROM save checks — `--battery-in/out`** (from slice 9). Roundtrip a
   golden save through the emulated card backup; assert best-score
   persistence and that every corrupt/blank/wrong-version blob decodes to the
   fresh table (never a crash). Uses the RAW `.sav` bus-copy path, per the
   `docs/PLATFORM-LIMITS.md` mGBA `load_save` segfault note's NDS sibling.

### What the slice-1 CI lockstep asserts, concretely

A committed `--touch` script (`CI_DIG_PIXELS` in `check-underroot.py`,
replayed verbatim by `rom-builds.yml`) digs a shaft down from the mouth, an
east branch, then one **isolated** pocket. The host mirror's `ci_fixture()`
computes **7 dug cells, 6 connected** to the entrance (the pocket is dug but
unreachable). The ROM, replaying the same stylus path, must report exactly
`ur_telemetry[UR_T_DUG]==7` and `[UR_T_CON]==6`. Host and ROM computed the
same integers from the same drawn plan — the determinism spine the whole arc
stands on, in miniature, offset-free (cumulative, asserted at the end frame).
