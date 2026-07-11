// Brineward — pure simulation layer (arc slice 2, walking skeleton).
//
// EVERYTHING in this header + bw_sim.c is a pure function of its arguments:
// fixed-point integers only, no wall clock, no runtime RNG, no globals, no
// libnds. That is the concept doc's determinism plan
// (docs/concepts/brineward-concept.md § "Determinism + host-provability"),
// same discipline as Gloamline's gl_sim layer.
//
// MIRROR RULE (keep in lockstep): tools/check-brine.py carries a
// line-for-line Python mirror of every function and constant here. Any
// change to this file MUST land in check-brine.py in the same PR — same
// discipline as gl_sim.c <-> check-gloam.py.
//
// SIGNED-DIVISION RULE (mirror safety): negative operands only ever meet
// division as arithmetic right shifts (>> 8), which gcc/ARM and Python
// both evaluate as floor division — never `/` on a possibly-negative
// value, so the C and the Python mirror agree bit-for-bit.

#ifndef BW_SIM_H
#define BW_SIM_H

#include <stdint.h>

// --- fixed point: 8.8 (1 px = 256 units) -----------------------------------
#define BW_ONE 256

// --- the sea (one open-water screen off the Graywake breakwater) ------------
// Text-console frame: HUD rows 0-1, swell rows 2 and 23. Positions are
// sprite CENTERS (16x16 ships), clamped to the water rectangle.
#define BW_SEA_X_MIN (16 * BW_ONE)
#define BW_SEA_X_MAX (239 * BW_ONE)
#define BW_SEA_Y_MIN (32 * BW_ONE)
#define BW_SEA_Y_MAX (175 * BW_ONE)

#define BW_PLAYER_START_X (128 * BW_ONE)
#define BW_PLAYER_START_Y (140 * BW_ONE)
#define BW_PLAYER_START_HDG 0            // due north, bow to open water

// --- headings ----------------------------------------------------------------
// Heading is 0..1023 (BW_CIRCLE units per full turn), 0 = north, clockwise.
// Direction vectors are 256-brad quantized: brad = heading >> 2, and
// (dx, dy) = (bw_sin(brad), -bw_cos(brad)) in 1/256 units (screen y down).
#define BW_CIRCLE 1024

// --- sail trim (the momentum-sailing knob) -----------------------------------
// 0 = battle sail (slow, tight turn), 1 = half sail, 2 = full sail.
#define BW_TRIM_BATTLE 0
#define BW_TRIM_HALF 1
#define BW_TRIM_FULL 2
// target speed per trim, units/frame (0.375 / 0.625 / 0.875 px/f)
#define BW_SPEED_BATTLE 96
#define BW_SPEED_HALF 160
#define BW_SPEED_FULL 224
// turn rate per trim, heading units/frame (full turn in 2.8s / 4.3s / 8.5s)
#define BW_TURN_BATTLE 6
#define BW_TURN_HALF 4
#define BW_TURN_FULL 2
// momentum: speed eases toward the trim target this fast (units/frame^2)
#define BW_ACCEL 2

// --- broadsides ----------------------------------------------------------------
#define BW_MAX_BALLS 16                  // shared pool, both ships
#define BW_BALLS_PER_SIDE 3              // one broadside = a 3-ball rake
#define BW_BALL_SPEED 640                // 2.5 px/frame
#define BW_BALL_LIFE 48                  // frames -> 120 px range
#define BW_BALL_MUZZLE 10                // px abeam of the hull at spawn
#define BW_BALL_SPREAD 6                 // px between rake balls (along keel)
#define BW_HIT_RANGE (6 * BW_ONE)        // chebyshev ball<->hull center
// range falloff: damage 30 at the muzzle easing to 10 at max range
#define BW_DMG_NEAR 30
#define BW_DMG_FALL 20                   // lost linearly over BW_BALL_LIFE

#define BW_HULL_MAX 100
#define BW_RELOAD_PLAYER 90              // frames per battery
#define BW_RELOAD_ENEMY 150              // the rum-runner's crews are slower

// --- enemy sloop ---------------------------------------------------------------
#define BW_SPAWN_DIST 90                 // px from the player anchorage
#define BW_SPAWN_MIN_DIST (32 * BW_ONE)  // chebyshev floor after sea clamp
#define BW_ENGAGE_RANGE (110 * BW_ONE)   // beyond: intercept; inside: circle
#define BW_FIRE_RANGE (112 * BW_ONE)     // AI holds fire past this
// AI alignment arcs, as |cross| : |dot| ratios of the beam-line test:
// fire when 12*|dot| < |cross| (~4.8 deg off the beam); stop correcting
// the turn when 16*|dot| < |cross| so the helm settles INSIDE the arc.
#define BW_AI_FIRE_ARC 12
#define BW_AI_TURN_ARC 16

// --- duel outcome ----------------------------------------------------------------
#define BW_DUEL_RUNNING 0
#define BW_DUEL_PLAYER_SUNK 1            // simultaneous sinking counts as loss
#define BW_DUEL_ENEMY_SUNK 2

// --- state -----------------------------------------------------------------------
typedef struct
{
    int32_t x, y;                        // 8.8 fixed, sprite center
    int32_t heading;                     // 0..1023
    int32_t trim;                        // 0..2
    int32_t speed;                       // units/frame, eases to trim target
    int32_t hull;                        // 0..BW_HULL_MAX
    int32_t reload_l, reload_r;          // frames until each battery is ready
} BwShip;

typedef struct
{
    int32_t x, y;                        // 8.8 fixed
    int32_t vx, vy;                      // units/frame
    int32_t age;                         // frames since fired
    int32_t owner;                       // 0 = player, 1 = enemy
    int32_t live;
} BwBall;

typedef struct
{
    BwShip player;
    BwShip enemy;
    BwBall balls[BW_MAX_BALLS];
    uint32_t frame;                      // duel frames stepped so far
    int32_t over;                        // BW_DUEL_*
} BwDuel;

// Player inputs for one duel frame. turn in {-1,0,+1}; trim_delta in
// {-1,0,+1} (edge-triggered by the caller); fire_* edge-triggered.
typedef struct
{
    int32_t turn;
    int32_t trim_delta;
    int32_t fire_l;                      // port battery
    int32_t fire_r;                      // starboard battery
} BwInputs;

// --- pure functions ----------------------------------------------------------------

// Deterministic 32-bit hash (Gloamline's gl_hash, the lane's no-runtime-RNG
// primitive).
uint32_t bw_hash(uint32_t a, uint32_t b);

// Quarter-table sine/cosine, 256 brads per circle, result -256..256.
int32_t bw_sin(int32_t brad);
int32_t bw_cos(int32_t brad);

// Chebyshev distance between two centers (the range metric).
int32_t bw_chebyshev(int32_t ax, int32_t ay, int32_t bx, int32_t by);

// Reset a duel: player sloop at the anchorage, enemy sloop on a
// BW_SPAWN_DIST ring at hash angle f(seed), bow toward the player.
void bw_duel_init(BwDuel *d, uint32_t seed);

// Enemy sail AI: pure f(enemy, player). Intercept beyond BW_ENGAGE_RANGE
// (full sail, bow on), circle inside it (battle sail, hold the player
// abeam), fire the facing battery when aligned, ranged, and reloaded.
void bw_ai(const BwShip *e, const BwShip *p, BwInputs *out);

// One duel frame: player inputs + enemy AI + both ships step + broadsides
// spawn + balls fly + hits land + sink check. Sets d->over.
void bw_duel_step(BwDuel *d, const BwInputs *in);

#endif // BW_SIM_H
