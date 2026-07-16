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
#define UR_T_SPARE 28    // 0
#define UR_T_WORDS 29

#endif // UR_SIM_H
