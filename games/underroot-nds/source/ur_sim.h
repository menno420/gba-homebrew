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
#define UR_T_SPARE 17    // 0
#define UR_T_WORDS 18

#endif // UR_SIM_H
