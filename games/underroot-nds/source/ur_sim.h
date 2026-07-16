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
#define UR_T_SPARE 15    // 0
#define UR_T_WORDS 16

#endif // UR_SIM_H
