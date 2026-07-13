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

// The predator pass (growth rung 1 — CONCEPT.md's "predators locking
// onto stragglers"): two hunters live only in HUNGRY water (the
// SELECT verb; calm START water never spawns them, so every carried
// pin holds by construction). Every SH_PRED_RELOCK frames each
// predator locks the unsaved fish FARTHEST from the flock centroid —
// the straggler, deterministically (ties by index; predator 1 takes
// the runner-up). A caught straggler is EATEN; the hunter drags its
// kill back to its den and stalks again after a cooldown. Counterplay
// is the concept's own feel: keep the school TIGHT.
constexpr int sh_predators = 2;
constexpr int sh_pred_speed = 288;   // 1.125 px/f — a pushed school
                                     //   (1.5) outruns it; a straggler
                                     //   drifting (<=0.4) does not
constexpr int sh_pred_eat_r2 = 6 * 6;
constexpr int sh_straggle_r2 = 44 * 44; // ONLY a genuine straggler is
                                     //   prey: >44px from the flock
                                     //   centroid. A tight (or
                                     //   re-cohered) school STARVES
                                     //   the hunters — abandonment,
                                     //   not existence, is what kills
                                     //   (tuned: without the threshold
                                     //   the committed sweep lost
                                     //   34/11 — leftovers were farmed
                                     //   faster than they re-cohered)
constexpr int sh_pred_relock = 48;   // frames between lock scans
constexpr int sh_pred_cooldown = 300;// den time after a kill (tuned:
                                     //   at 180 the committed sweep LOSES
                                     //   34/11 — hungry water must be
                                     //   tense but winnable, measured)
// Fixed dens, right-center water: the hunters guard the way home.
constexpr int sh_pred_den_x[sh_predators] = {176, 176};
constexpr int sh_pred_den_y[sh_predators] = {40, 120};

// The gates (growth rung 2 — the committed concept's "past predators,
// through gates, into the safe reef"; CONCEPT.md's next named cut).
// STATIC GEOMETRY ONLY: two coral walls with offset gaps stand between
// the school and the reef, and they exist only in THE GATED RUN (the
// R verb; calm START water and hungry SELECT water never run the gate
// pass, so every carried pin holds by construction — and the hungry
// water's coupled difficulty knob, straggler 44 px + den 300 + goal
// 35, is untouched). A fish pressed into the coral is ejected to the
// side it came from with its approach damped; the school must be
// FUNNELED — gap 0 sits high, gap 1 sits low, so the herd snakes.
// No tunneling by construction: per-axis speed is clamped to 1.5 px/f
// (sh_speed_max) and the wall band is 5 px wide, checked every frame.
constexpr int sh_gates = 2;
constexpr int sh_gate_x[sh_gates] = {104, 156};  // clear of the spawn
                                     //   box (x <= 95) and the reef
constexpr int sh_gate_half = 2;      // wall band = gate_x +/- 2 px
constexpr int sh_gate_gap_y0[sh_gates] = {28, 92};   // gap 0 HIGH,
constexpr int sh_gate_gap_y1[sh_gates] = {68, 132};  // gap 1 LOW —
                                     //   44 px tall each: one school
                                     //   width, offset so a straight
                                     //   push cannot bank the shoal

// sh_fish.saved: 0 = at sea, 1 = SAVED in the reef, 2 = EATEN.
struct sh_fish
{
    int x = 0, y = 0;                // 8.8 px
    int vx = 0, vy = 0;              // 8.8 px/frame
    int saved = 0;
};

struct sh_pred
{
    int x = 0, y = 0;                // 8.8 px
    int target = -1;                 // locked fish index (-1 = none)
    int cooldown = 0;                // frames left in the den
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

// One deterministic predator frame (hungry water only; the caller
// simply never calls it in calm water): re-lock on schedule, stalk,
// eat, den up. Returns fish NEWLY eaten this frame. Same IWRAM
// discipline (and the same map-address verification duty).
SH_CODE_IWRAM int sh_pred_update(sh_fish* fish, sh_pred* preds,
                                 unsigned run_frames);

// One deterministic gate frame (THE GATED RUN only; the caller simply
// never calls it in calm or hungry water). Pure static-geometry test:
// eject fish pressed into a wall band outside its gap, damp the
// approach. Returns the number of fish blocked THIS frame (telemetry
// evidence: the school visibly pooling against the coral). Same IWRAM
// discipline (and the same map-address verification duty).
SH_CODE_IWRAM int sh_gate_update(sh_fish* fish);

#endif // SHOAL_FLOCK_H
