// Underroot — pure simulation layer (arc slice 1: dual-screen skeleton).
//
// EVERYTHING in this header + ur_sim.c is a pure function of its arguments:
// integers only, no wall clock, no runtime RNG, no globals, no libnds. That
// is the arc doc's determinism-first plan (docs/arcs/UNDERROOT.md
// § "Determinism-first"), and it is what lets a host-side Python mirror
// (tools/check-underroot.py) prove a scripted dig plan's outcome the same
// way the ROM computes it.
//
// MIRROR RULE (keep in lockstep): tools/check-underroot.py carries a
// line-for-line Python mirror of every function and constant here. Any
// change to this file MUST land in check-underroot.py in the same PR — the
// same discipline as gl_sim.h <-> check-gloam.py and lumen-drift <->
// check-cave.py.

#ifndef UR_SIM_H
#define UR_SIM_H

#include <stdint.h>

// --- meadow: hawk schedule (top screen) ------------------------------------
// The concept's "hawk shadows on a pure f(seed, season, index) schedule".
// A shadow sweeps the 256px-wide meadow once per UR_HAWK_PERIOD frames,
// taking UR_HAWK_SWEEP frames to cross; direction and lane are hashed off
// the pass INDEX (frame / UR_HAWK_PERIOD) — so the whole schedule is a pure
// function of (seed, season, index).
#define UR_HAWK_PERIOD 512               // frames between hawk passes
#define UR_HAWK_SWEEP 256                // frames a shadow takes to cross
#define UR_MEADOW_Y0 24                  // hawk lane band, top (px)
#define UR_MEADOW_Y1 120                 // hawk lane band, bottom (px, excl.)
#define UR_HAWK_SALT 0x4841574Bu         // 'HAWK' — lane stream != direction

// Slice-1 pinned meadow seed: the skeleton animates from boot on ONE fixed
// seed (season fixed spring) so the boot proof can assert exact hawk values.
// Slice 7 (seasons) and the run-seed latch come later.
#define UR_SEED 0x55Au

// --- meadow: food patches (top screen; slice 2) ----------------------------
// Foragers surface into the meadow to gather food. A patch's position and
// amount are a pure f(seed, season, index) (ur_patch): scattered on the
// meadow's lower FORAGING APRON — the band BELOW the hawk lanes
// (y >= UR_MEADOW_Y1) and above the bottom edge, with room for the render
// half-extent — so a whole patch is provably ON the meadow and OFF the hawk
// lanes (the arc doc's slice-2 invariant): a forager never gathers under a
// lane. Season is fixed spring (0) this slice; the FEED salt keeps the patch
// hash stream independent of the hawk lane stream. Amounts feed the slice-4
// granary store.
#define UR_SCREEN_W 256                  // meadow width (px)
#define UR_SCREEN_H 192                  // meadow height (px)
#define UR_PATCH_COUNT 6                 // food patches per (seed, season)
#define UR_PATCH_R 4                     // patch render half-extent (px)
#define UR_PATCH_MARGIN 8                // keep whole patches off the edges
#define UR_PATCH_MIN 3                   // min food amount (units)
#define UR_PATCH_MAX 9                   // max food amount (units, inclusive)
#define UR_PATCH_SALT 0x46454544u        // 'FEED' — patch stream independent
// The foraging apron box holds patch CENTRES; the +/- UR_PATCH_R keeps a
// whole rendered patch below the hawk band and inside the screen.
#define UR_APRON_X0 (UR_PATCH_MARGIN + UR_PATCH_R)                // 12
#define UR_APRON_X1 (UR_SCREEN_W - UR_PATCH_MARGIN - UR_PATCH_R)  // 244
#define UR_APRON_Y0 (UR_MEADOW_Y1 + UR_PATCH_R)                   // 124
#define UR_APRON_Y1 (UR_SCREEN_H - UR_PATCH_MARGIN - UR_PATCH_R)  // 180

// A food patch: a meadow-local centre (px) and a food amount (units).
typedef struct {
    int32_t x;
    int32_t y;
    int32_t amount;
} ur_patch_t;

// The index-th food patch for (seed, season): pure, deterministic, on the
// foraging apron, amount in [UR_PATCH_MIN, UR_PATCH_MAX].
ur_patch_t ur_patch(uint32_t seed, uint32_t season, uint32_t index);
// Total food across all UR_PATCH_COUNT patches for (seed, season).
uint32_t ur_patch_total(uint32_t seed, uint32_t season);

// --- burrow: the dig grid (bottom screen) ----------------------------------
// A tile-snap tunnel grid: the stylus paints DUG cells, and the burrow is a
// GRAPH — a cell only counts (forager-reachable) if it connects back to the
// entrance mouth over 4-neighbours. Tile-snap (not freeform) is the arc
// doc's core-loop decision: the grid IS the forager path graph and a clean
// integer the host mirror can prove bit-for-bit.
#define UR_GRID_W 16                     // columns
#define UR_GRID_H 12                     // rows
#define UR_CELL 16                       // px per cell (16*16=256, 12*16=192)
#define UR_CELLS (UR_GRID_W * UR_GRID_H)
#define UR_ENTRANCE_COL 8                // the surface mouth column
#define UR_ENTRANCE_ROW 0                // top row (meadow<->burrow seam)

// --- foragers: emergent pathing (slice 3) ----------------------------------
// The arc doc's slice-3 decision: forager pathing EMERGES from the drawn
// tunnel graph, no scripted AI. A meadow patch at column c is gathered
// through the burrow DROP CELL (c, UR_DROP_ROW) — the top subsurface cell of
// its column, one row below the seam: a forager surfaces there to reach the
// patch above. The mouth (the doorway, row 0) by itself opens no patch — you
// must tunnel to just beneath a patch to feed. A forager walks the SHORTEST
// dug path (BFS over the same 4-connected dug graph ur_burrow_size counts)
// from the mouth to the nearest REACHABLE drop cell and back; a patch whose
// drop cell is soil or unconnected is NEVER visited (connect-or-waste).
// Everything here is a pure integer function of the dig plan + (seed,season).
#define UR_DROP_ROW (UR_ENTRANCE_ROW + 1)   // subsurface drop row (1)
#define UR_ROUTE_NONE 0xFFFFFFFFu            // no reachable patch / unreachable

// --- granaries: the food store (slice 4) -----------------------------------
// The arc doc's slice-4 decision: a SECOND stylus verb designates dug cells
// GRANARY (hold R + tap a tunnel cell), and the meadow food the foragers can
// reach is BANKED there up to capacity. Storage inherits the same
// connect-or-waste graph as everything else: a granary cell banks only if it
// is DUG, DESIGNATED, and CONNECTED back to the mouth (finite ur_dig_dist) —
// an unreachable designated pocket adds ZERO capacity, exactly as an
// unreachable patch feeds nothing. Each connected granary cell holds
// UR_GRAN_CAP food units; the store is min(reachable meadow food, capacity)
// — deposits bank UP TO capacity, the overflow is spoilage. Pure integers, so
// the host mirror recomputes the whole store bit-for-bit.
#define UR_GRAN_CAP 4                        // food units one granary cell banks

// --- nurseries + population (slice 5) --------------------------------------
// The arc doc's slice-5 decision: a THIRD stylus verb designates dug cells
// NURSERY (hold L + tap a tunnel cell), and connected nurseries convert the
// granary-banked food (slice 4's store) into a POPULATION of new foragers on a
// pure, bounded schedule. Broodkeeping inherits the same connect-or-waste
// graph: a nursery cell broods only if it is DUG, DESIGNATED, and CONNECTED
// back to the mouth (finite ur_dig_dist) — a disconnected nursery pocket
// broods ZERO, exactly as an unreachable granary banks nothing. Each connected
// nursery can brood UR_NURS_BROOD foragers, and raising one forager consumes
// UR_FOOD_PER_ANT banked food; so the population is
// min(connected*brood, store/food_per_ant) capped at UR_POP_CAP. Pure
// integers, so the host mirror recomputes the whole population bit-for-bit.
#define UR_POP_CAP      8   /* hard bound on raised foragers */
#define UR_FOOD_PER_ANT 2   /* banked food consumed to raise one forager */
#define UR_NURS_BROOD   3   /* foragers one connected nursery can brood */

// --- hawks predate: exposed shallow route cells (slice 6) ------------------
// The arc doc's slice-6 decision (docs/arcs/UNDERROOT.md, Decision 2 "Deep or
// dead" + the slice-6 row): the hawk shadows that sweep the meadow (slice 1)
// catch foragers on the EXPOSED cells of the forager route — the ones near the
// surface, within a hawk's stoop — while cells buried DEEP below the surface
// are safe. The exposed<->deep boundary is a pure grid-row threshold: a route
// cell at row < UR_SAFE_DEPTH is EXPOSED, at row >= UR_SAFE_DEPTH is DEEP. The
// route counted is slice 3's shortest dug path (ur_forage's canonical BFS path
// to the nearest reachable patch's drop cell), so predation is the
// deterministic consequence of the drawn tunnel: a surface-hugging route is
// hawk-food; digging a deeper-only path (so the forager's SHORTEST route dives
// below the exposed band) costs more digging but survives the meadow. The
// colony loses min(exposed route cells, population) foragers; survivors =
// pop - losses. Pure integers, so the host mirror recomputes it bit-for-bit.
// (The per-pass HAWK-SCHEDULE timing scaling — how many of the sweeps catch
// how many foragers over a season — is the slice-7 year clock's concern; slice
// 6 is the instantaneous exposure GEOMETRY, the same time-deferred discipline
// slices 4/5 kept.)
#define UR_SAFE_DEPTH   2   /* grid rows [0, UR_SAFE_DEPTH) are hawk-exposed */

// --- seasons + the year clock (slice 7) ------------------------------------
// The arc doc's Decision 3 + the slice-7 row (docs/arcs/UNDERROOT.md): v1.0 is
// exactly ONE year, SPRING -> SUMMER -> AUTUMN -> WINTER, each a FIXED number
// of days (the decide-and-flag counts fixed here). The clock is driven off the
// frame counter — a day is UR_DAY_FRAMES frames — so the whole schedule is a
// pure integer function of the frame: ur_day(frame) is the day, and
// ur_season_of_day(day) partitions the year into the four seasons (a day past
// the year's end clamps to WINTER, the terminal state the slice-8 survival exam
// reads). Meadow ABUNDANCE scales by season: the reachable meadow haul is
// multiplied by ur_abundance(season)/UR_ABUND_UNIT — SPRING is the baseline
// UNIT (so a spring run is bit-identical to the pre-clock slices 1-6), SUMMER
// is plentiful, AUTUMN thins, WINTER is ZERO (no meadow food — the arc's exam).
// And the per-hawk-pass PREDATION temporal scaling slice 6 deferred here lands:
// ur_hawk_passes(season) sweeps cross a season, and ur_season_predation scales
// the exposed-route loss by that pass count (still bounded by the population),
// with the single-pass season collapsing to slice 6's instantaneous snapshot.
// Pure integers throughout, so the host mirror recomputes the year bit-for-bit.
#define UR_SPRING 0
#define UR_SUMMER 1
#define UR_AUTUMN 2
#define UR_WINTER 3
#define UR_SEASONS 4
// The fixed per-season day counts (decide-and-flag; slice 7 fixes them).
#define UR_SPRING_DAYS 4
#define UR_SUMMER_DAYS 6
#define UR_AUTUMN_DAYS 4
#define UR_WINTER_DAYS 2
#define UR_YEAR_DAYS (UR_SPRING_DAYS + UR_SUMMER_DAYS + UR_AUTUMN_DAYS + UR_WINTER_DAYS) /* 16 */
#define UR_DAY_FRAMES 256                 // frames the clock spends on one day
// Meadow abundance scale (a numerator over UR_ABUND_UNIT). SPRING is the unit
// baseline (spring == the pre-clock behaviour); SUMMER plentiful; AUTUMN thins;
// WINTER zero.
#define UR_ABUND_UNIT   4
#define UR_ABUND_SPRING 4
#define UR_ABUND_SUMMER 6
#define UR_ABUND_AUTUMN 2
#define UR_ABUND_WINTER 0

// --- winter survival + the survival score (slice 8) ------------------------
// The arc doc's Decision 3 + the slice-8 row (docs/arcs/UNDERROOT.md): the
// MARQUEE exam. Winter is the terminal season the clock clamps to
// (ur_season_of_day), and its meadow abundance is ZERO (UR_ABUND_WINTER) — no
// meadow food, so the colony lives or starves on what the drawn tunnels banked.
// Winter DRAINS the banked store: each of the UR_WINTER_DAYS the colony eats
// pop * UR_CONSUME_PER_DAY, where `pop` is the foragers carried into winter (the
// slice-7 season survivors) and `store` is the food banked in reachable
// granaries AFTER the nurseries drew their brood cost (slice 5's
// ur_pop_food — raising each forager consumes UR_FOOD_PER_ANT banked food, so
// what winter inherits is the store minus what the colony already ate to grow).
// The colony SURVIVES iff the running store never goes negative across the
// whole winter (with pop constant this is exactly store >= pop*days*consume),
// and the headline SCORE is surviving_pop*1000 + leftover_store, or 0 if
// starved out — a gradient that rewards banking a MARGIN, not just barely
// clearing. Pure integers throughout, so the host mirror recomputes the whole
// winter exam bit-for-bit and CI can PROVE a scripted dig plan survives (or
// starves) winter, deterministically, host-side AND in the ROM.
#define UR_CONSUME_PER_DAY 1   /* banked food one forager eats per winter day */

// --- pure hash (identical to gl_hash) --------------------------------------
uint32_t ur_hash(uint32_t a, uint32_t b);

// --- hawk schedule ---------------------------------------------------------
uint32_t ur_hawk_pass(uint32_t frame);
int ur_hawk_active(uint32_t frame);
uint32_t ur_hawk_dir(uint32_t seed, uint32_t season, uint32_t index);
// Shadow x in [0,255] while a pass is crossing, or -1 when the sky is clear.
int32_t ur_hawk_x(uint32_t seed, uint32_t season, uint32_t frame);
// Lane y in [UR_MEADOW_Y0, UR_MEADOW_Y1); constant across one pass.
int32_t ur_hawk_y(uint32_t seed, uint32_t season, uint32_t frame);

// --- dig grid --------------------------------------------------------------
// Map a bottom-LCD pixel to its grid cell. Returns 1 and writes *col/*row
// for an in-bounds pixel, 0 (rejecting) for out of bounds.
int ur_cell_of_touch(int32_t tx, int32_t ty, int32_t *col, int32_t *row);
// Set a cell dug; returns 1 iff it changed (was soil), 0 if already dug.
int ur_dig(uint8_t grid[UR_CELLS], int32_t col, int32_t row);
// Initialise a fresh grid: all soil except the pre-dug entrance mouth.
void ur_fresh_grid(uint8_t grid[UR_CELLS]);
// BFS from the entrance over 4-connected dug cells; return the count
// reachable (the burrow the foragers can actually walk). 0 if the mouth
// itself is somehow soil.
uint32_t ur_burrow_size(const uint8_t grid[UR_CELLS]);

// --- foragers: shortest dug path + nearest reachable patch (slice 3) --------
// A meadow patch's burrow drop COLUMN (patch.x / UR_CELL), clamped to the
// grid. The drop cell is that column at UR_DROP_ROW.
int32_t ur_patch_col(ur_patch_t p);
// Shortest dug-path distance (in cells; mouth is 0) from the entrance mouth
// to (col,row) over 4-connected dug cells, or UR_ROUTE_NONE if that cell is
// soil or not connected to the mouth. BFS over the same graph as
// ur_burrow_size, so the route is a consequence of what was drawn.
uint32_t ur_dig_dist(const uint8_t grid[UR_CELLS], int32_t col, int32_t row);
// The nearest reachable meadow patch for (seed, season) over the dug graph.
typedef struct {
    uint32_t index;   // nearest reachable patch index, or UR_ROUTE_NONE
    uint32_t dist;    // one-way steps mouth->drop, or UR_ROUTE_NONE
    uint32_t route;   // round trip (2*dist), or UR_ROUTE_NONE
} ur_forage_t;
// Scans all UR_PATCH_COUNT patches; returns the one whose drop cell has the
// smallest dug-path distance (ties -> lowest index), with route = 2*dist. If
// no patch's drop cell is reachable, every field is UR_ROUTE_NONE.
ur_forage_t ur_forage(const uint8_t grid[UR_CELLS], uint32_t seed, uint32_t season);

// --- granaries: the food store (slice 4) -----------------------------------
// Designate a DUG cell a granary. Returns 1 iff it changed (was an
// undesignated dug cell), 0 for a soil cell (only dug cells hold a granary),
// an already-designated cell, or an out-of-bounds cell. Idempotent + monotone,
// mirroring ur_dig; `gran` is a parallel 0/1 designation layer over the dig
// grid (a granary cell stays a normal dug cell for pathing/connectivity — the
// dug graph is unchanged).
int ur_designate(const uint8_t grid[UR_CELLS], uint8_t gran[UR_CELLS],
                 int32_t col, int32_t row);
// Initialise a fresh designation layer: nothing designated.
void ur_fresh_gran(uint8_t gran[UR_CELLS]);
// Count of DESIGNATED granary cells that are also dug (total placed, reachable
// or not).
uint32_t ur_gran_count(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS]);
// Count of designated granary cells CONNECTED to the mouth (finite
// ur_dig_dist — the same reachability the burrow BFS defines). Unreachable
// designated cells are excluded: they bank nothing.
uint32_t ur_gran_connected(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS]);
// Total granary capacity = connected granary cells * UR_GRAN_CAP.
uint32_t ur_gran_capacity(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS]);
// Total food the colony can HAUL: sum of amounts over the meadow patches whose
// burrow drop cell is reachable (connect-or-waste for patches; slice 3 graph).
uint32_t ur_reachable_food(const uint8_t grid[UR_CELLS], uint32_t seed, uint32_t season);
// The banked store = min(reachable food, granary capacity). Deposits bank up
// to capacity; food over capacity spoils, capacity over food sits empty.
uint32_t ur_store(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                  uint32_t seed, uint32_t season);

// --- nurseries + population (slice 5) --------------------------------------
// Initialise a fresh nursery designation layer: nothing designated. (Mirrors
// ur_fresh_gran; nurseries reuse the same parallel 0/1 layer type as gran.)
void ur_fresh_nurs(uint8_t nurs[UR_CELLS]);
// Designate a DUG cell a nursery. Returns 1 iff it changed (was an
// undesignated dug cell), 0 for a soil cell (only dug cells hold a nursery),
// an already-designated cell, or an out-of-bounds cell. Idempotent + monotone,
// mirroring ur_designate; `nurs` is a parallel 0/1 designation layer over the
// dig grid (a nursery cell stays a normal dug cell for pathing/connectivity).
uint32_t ur_nurse(const uint8_t grid[UR_CELLS], uint8_t nurs[UR_CELLS],
                  uint32_t col, uint32_t row);
// Count of DESIGNATED nursery cells that are also dug (total placed,
// reachable or not).
uint32_t ur_nurs_count(const uint8_t grid[UR_CELLS], const uint8_t nurs[UR_CELLS]);
// Count of designated nursery cells CONNECTED to the mouth (finite
// ur_dig_dist — the same reachability the burrow BFS defines). Unreachable
// designated cells are excluded: they brood nothing.
uint32_t ur_nurs_connected(const uint8_t grid[UR_CELLS], const uint8_t nurs[UR_CELLS]);
// The colony population: banked food converted to foragers by the connected
// nurseries, = min(connected*UR_NURS_BROOD, store/UR_FOOD_PER_ANT) capped at
// UR_POP_CAP. Pure f(dig plan, gran plan, nurs plan, seed, season).
uint32_t ur_pop(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season);
// The banked food that population consumes = ur_pop * UR_FOOD_PER_ANT.
uint32_t ur_pop_food(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                     const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season);

// --- hawks predate: exposed shallow route cells (slice 6) ------------------
// Count of EXPOSED cells (grid row < UR_SAFE_DEPTH) on the canonical shortest
// dug path from the mouth to (col,row). The path is the SAME BFS graph
// ur_dig_dist walks (E,W,S,N neighbour order), reconstructed via first-reached
// predecessors — so the path, and thus the exposed count, is a deterministic
// integer. Returns 0 if (col,row) is soil, out of bounds, or not connected to
// the mouth (no route -> nothing exposed).
uint32_t ur_route_exposed(const uint8_t grid[UR_CELLS], int32_t col, int32_t row);
// Exposed cells on the forager's route: ur_forage picks the nearest reachable
// patch, ur_route_exposed counts the shallow cells on the path to its drop
// cell. 0 if no patch is reachable (no route to predate).
uint32_t ur_forage_exposed(const uint8_t grid[UR_CELLS], uint32_t seed, uint32_t season);
// Foragers lost to hawk predation = min(ur_forage_exposed, ur_pop) — each
// exposed route cell can cost at most one forager, and the colony can lose no
// more foragers than it has (the min's two binding regimes: exposure-bound and
// pop-bound). Pure f(dig plan, gran plan, nurs plan, seed, season).
uint32_t ur_predation(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                      const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season);
// Surviving foragers = ur_pop - ur_predation. Since predation <= pop, this
// never underflows. This is the population the colony carries into winter
// (slice 8's survival score reads it).
uint32_t ur_survivors(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                      const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season);

// --- seasons + the year clock (slice 7) ------------------------------------
// The day the frame clock is on = frame / UR_DAY_FRAMES. Free-running; the
// season clamps to WINTER past the year's end (ur_season_of_day).
uint32_t ur_day(uint32_t frame);
// The fixed day count for a season (UR_SPRING_DAYS.. by season index).
uint32_t ur_season_days(uint32_t season);
// The season a given day falls in: the four seasons partition [0, UR_YEAR_DAYS)
// in order, and any day >= UR_YEAR_DAYS clamps to WINTER (the year ends there).
uint32_t ur_season_of_day(uint32_t day);
// The meadow abundance scale for a season, a numerator over UR_ABUND_UNIT:
// UR_ABUND_SPRING (the unit baseline) / SUMMER / AUTUMN / WINTER (0). Pure.
uint32_t ur_abundance(uint32_t season);
// The seasonal meadow haul: the reachable food (slice 4) scaled by the season's
// abundance = ur_reachable_food * ur_abundance(season) / UR_ABUND_UNIT. Spring
// (the unit) is the raw haul; winter (abundance 0) is zero — no meadow food.
uint32_t ur_season_food(const uint8_t grid[UR_CELLS], uint32_t seed, uint32_t season);
// How many whole hawk sweeps cross a season = season_days * UR_DAY_FRAMES /
// UR_HAWK_PERIOD. The per-season pass count the predation scaling multiplies by.
uint32_t ur_hawk_passes(uint32_t season);
// The per-hawk-pass-scaled predation over a whole season: each of the season's
// ur_hawk_passes catches up to ur_forage_exposed foragers on the exposed route,
// but the colony can lose no more foragers than it has =
// min(ur_forage_exposed * ur_hawk_passes(season), ur_pop). A one-pass season
// collapses to slice 6's instantaneous ur_predation (the degenerate identity).
uint32_t ur_season_predation(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                             const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season);
// Foragers surviving a whole season = ur_pop - ur_season_predation (the season
// predation is capped at ur_pop, so this never underflows). The population that
// carries into the next season (slice 8's winter drain reads the winter value).
uint32_t ur_season_survivors(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                             const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season);

// --- winter survival + the survival score (slice 8) ------------------------
// The store carried INTO winter: the banked granary store (slice 4) minus the
// food the nurseries drew to raise the population (slice 5's ur_pop_food). Since
// ur_pop <= store/UR_FOOD_PER_ANT, the brood cost never exceeds the store, so
// this is a non-negative pure f(dig plan, gran plan, nurs plan, seed, season).
// (Evaluated at the harvest season; winter's own abundance is 0, so the store a
// colony carries in is what it banked over the growing seasons.)
uint32_t ur_winter_store(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                         const uint8_t nurs[UR_CELLS], uint32_t seed, uint32_t season);
// The total food winter demands: `pop` foragers each eat UR_CONSUME_PER_DAY for
// each of the UR_WINTER_DAYS = pop * UR_WINTER_DAYS * UR_CONSUME_PER_DAY. Pure.
uint32_t ur_winter_drain(uint32_t pop);
// Does the colony survive winter? With pop constant across the drain, the store
// falls monotonically, so "the running store never goes negative" is exactly
// store >= ur_winter_drain(pop). Returns 1 (survives) / 0 (starves).
int ur_winter_survives(uint32_t store, uint32_t pop);
// The store LEFT after winter: store - drain if the colony survives, else 0 (a
// starved colony banks nothing forward).
uint32_t ur_winter_leftover(uint32_t store, uint32_t pop);
// The survival SCORE (the headline number, arc doc Decision 3): if the colony
// survives, surviving_pop*1000 + leftover_store (surviving_pop == pop, since a
// survived winter keeps its foragers); 0 if starved out. Pure f(store, pop).
uint32_t ur_winter_score(uint32_t store, uint32_t pop);

// --- telemetry mailbox (ELF-exported; read by tools/nds-headless-check.py) -
#define UR_T_MAGIC0 0    // 0x554E4452 'UNDR'
#define UR_T_MAGIC1 1    // 0x524F4F54 'ROOT'
#define UR_T_FRAME 2     // global frame counter (every vblank)
#define UR_T_SEASON 3    // season (0=spring; fixed slice 1)
#define UR_T_SEED 4      // meadow seed (UR_SEED, fixed slice 1)
#define UR_T_HAWKON 5    // hawk shadow crossing this frame? 0/1
#define UR_T_HAWKX 6     // hawk shadow x px (valid only when HAWKON)
#define UR_T_HAWKY 7     // hawk lane y px (valid only when HAWKON)
#define UR_T_DUG 8       // total dug cells
#define UR_T_CON 9       // dug cells connected to the entrance
#define UR_T_TCOL 10     // last dug col (0xFFFFFFFF = none yet)
#define UR_T_TROW 11     // last dug row (0xFFFFFFFF = none yet)
#define UR_T_TOUCH 12    // stylus down this frame? 0/1
#define UR_T_GW 13       // UR_GRID_W (build-flag visibility)
#define UR_T_GH 14       // UR_GRID_H
#define UR_T_PATCHN 15   // food-patch count (== UR_PATCH_COUNT; slice 2)
#define UR_T_PATCHSUM 16 // total food across patches, f(seed,season) (slice 2)
#define UR_T_ROUTEI 17   // nearest reachable patch index (UR_ROUTE_NONE=none; slice 3)
#define UR_T_ROUTED 18   // one-way dug distance mouth->drop (UR_ROUTE_NONE=none; slice 3)
#define UR_T_ROUTELEN 19 // round-trip route length, 2*dist (UR_ROUTE_NONE=none; slice 3)
#define UR_T_GRANN 20    // designated granary cells (dug), total (slice 4)
#define UR_T_GRANCON 21  // designated granary cells connected to the mouth (slice 4)
#define UR_T_CAP 22      // total granary capacity (GRANCON*UR_GRAN_CAP; slice 4)
#define UR_T_STORE 23    // banked store = min(reachable food, capacity) (slice 4)
#define UR_T_NURSN 24    // designated nursery cells (dug), total (slice 5)
#define UR_T_NURSCON 25  // designated nursery cells connected to the mouth (slice 5)
#define UR_T_POP 26      // population = min(nurscon*brood, store/food_per_ant) capped (slice 5)
#define UR_T_POPFOOD 27  // banked food that population consumes (POP*UR_FOOD_PER_ANT) (slice 5)
#define UR_T_EXPOSED 28  // exposed shallow route cells on the forager route (slice 6)
#define UR_T_LOST 29     // foragers lost to hawk predation = min(exposed, pop) (slice 6)
#define UR_T_SURV 30     // surviving foragers = pop - lost (slice 6)
#define UR_T_DAY 31      // the year-clock day = frame / UR_DAY_FRAMES (slice 7)
#define UR_T_ABUND 32    // meadow abundance scale for the live season, /UR_ABUND_UNIT (slice 7)
#define UR_T_SFOOD 33    // seasonal meadow haul = reachable food * abundance / unit (slice 7)
#define UR_T_HAWKPASS 34 // hawk sweeps that cross the live season (slice 7)
#define UR_T_SPRED 35    // per-hawk-pass season predation = min(exposed*passes, pop) (slice 7)
#define UR_T_SSURV 36    // foragers surviving the season = pop - season predation (slice 7)
#define UR_T_WSTORE 37   // store carried into winter = banked store - brood cost (slice 8)
#define UR_T_WPOP 38     // foragers carried into winter (== season survivors) (slice 8)
#define UR_T_WDRAIN 39   // total winter food demand = wpop*WINTER_DAYS*CONSUME (slice 8)
#define UR_T_WSURV 40    // survives winter? wstore >= wdrain -> 1/0 (slice 8)
#define UR_T_WLEFT 41    // store left after winter = wstore - wdrain if survive else 0 (slice 8)
#define UR_T_WSCORE 42   // survival score = surviving_pop*1000 + leftover (0 if starved) (slice 8)
#define UR_T_SPARE 43    // 0
#define UR_T_WORDS 44

#endif // UR_SIM_H
