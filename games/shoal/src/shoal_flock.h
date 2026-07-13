/*
 * SHOAL — the flock pass, shared contract between main.cpp and the
 * IWRAM translation unit (shoal_flock.bn_iwram.cpp).
 *
 * THE GATE'S LADDER, MEASURED (all numbers from this session's
 * headless profiling runs, quoted in the PR/card):
 *   - naive O(n^2), code in ROM:      mean 120.5%  worst-frame 181%
 *   - + 24px neighbor grid:           mean 103.3%  (the school BALLS
 *     UP by design — cohesion defeats the grid: it degenerates to
 *     O(n^2) exactly when the flock flocks)
 *   - + deterministic 8-neighbor cap: mean  96.9%  (bounds the pair
 *     WORK but not the ROM-waitstate instruction fetches)
 *   - isolation probes: sprites off = -4%, WHOLE SIM off = 10.1%
 *     -> the cost was never the pair COUNT alone; it was branchy
 *     sim code fetched from wait-stated ROM.
 *   - the fix that fits: this file compiled ARM into IWRAM (Butano's
 *     own hot-loop discipline, the *.bn_iwram.cpp rule) — measured
 *     result in the PR.
 *
 * All positions/velocities in 8.8 px; the pair pass runs in whole px
 * (the concept's precision-drop mitigation). Deterministic: fixed
 * iteration order, no RNG, no floats.
 */

#ifndef SHOAL_FLOCK_H
#define SHOAL_FLOCK_H

constexpr int sh_fish_count = 50;    // mid of the concept's 40-60
constexpr int sh_fp = 256;

// Water rectangle (whole px) and the reef line.
constexpr int sh_water_x0 = 12, sh_water_x1 = 228;
constexpr int sh_water_y0 = 20, sh_water_y1 = 140;
constexpr int sh_reef_x = 204;

// Boid neighborhoods (whole px) and force gains.
constexpr int sh_see_r2 = 24 * 24;
constexpr int sh_sep_r2 = 9 * 9;
constexpr int sh_neighbor_cap = 8;   // a fish heeds at most 8 mates per
                                     // frame, deterministic first-found
                                     // order (the grid degenerates when
                                     // the school balls up; the cap is
                                     // what bounds the work)
constexpr int sh_coh_shift = 6;
constexpr int sh_ali_shift = 4;
constexpr int sh_sep_gain = 12;
constexpr int sh_wall_push = 14;
constexpr int sh_wall_margin = 12;
constexpr int sh_speed_max = 384;    // 1.5 px/frame per axis
constexpr int sh_drift_min = 24;

// The current (you).
constexpr int sh_push_r2 = 56 * 56;   // wide current: one central lane
                                     //   sweeps most of the water column
constexpr int sh_push_gain = 22;

struct sh_fish
{
    int x = 0, y = 0;                // 8.8 px
    int vx = 0, vy = 0;              // 8.8 px/frame
    int saved = 0;
};

// One deterministic flock frame: boids + the player's current + walls +
// integration + reef settlement. Returns the number of fish NEWLY saved
// this frame (the caller owns the running total). Lives in IWRAM: the
// *.bn_iwram.cpp build rule gives ARM + long calls, and THIS section
// attribute (Butano's own BN_CODE_IWRAM value, bn_hw_common.h) is what
// actually places the code at 0x03xxxxxx — measured the hard way: the
// rule alone left it in ROM as 32-bit ARM on the 16-bit bus.
#define SH_CODE_IWRAM __attribute__((section(".iwram")))
SH_CODE_IWRAM int sh_flock_update(sh_fish* fish, int cursor_x,
                                  int cursor_y, bool pushing);

#endif // SHOAL_FLOCK_H
