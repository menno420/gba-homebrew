// Gloamline — pure simulation layer (arc slice 6: scavenge interlude).
//
// EVERYTHING in this header + gl_sim.c is a pure function of its arguments:
// fixed-point integers only, no wall clock, no runtime RNG, no globals, no
// libnds. That is the concept doc's determinism plan
// (docs/concepts/gloamline-concept.md § "Determinism + host-provability"),
// and it is what makes the host proof possible at all.
//
// MIRROR RULE (keep in lockstep): tools/check-gloam.py carries a
// line-for-line Python mirror of every function and constant here. Any
// change to this file MUST land in check-gloam.py in the same PR — same
// discipline as games/lumen-drift ↔ tools/check-cave.py.

#ifndef GL_SIM_H
#define GL_SIM_H

#include <stdint.h>

// --- fixed point: 8.8 (1 px = 256 units) -----------------------------------
#define GL_ONE 256

// --- arena (the hamlet yard, one 256x192 screen) ----------------------------
// Text-console fence ring: HUD rows 0-1, fence rows 2 and 23, cols 0 and 31.
// Walkable interior in pixels: x 8..247, y 24..183. Positions below are
// sprite CENTERS (16x16 sprites), so the center range is inset 8px more.
#define GL_ARENA_X_MIN (16 * GL_ONE)
#define GL_ARENA_X_MAX (239 * GL_ONE)
#define GL_ARENA_Y_MIN (32 * GL_ONE)
#define GL_ARENA_Y_MAX (175 * GL_ONE)

#define GL_PLAYER_START_X (128 * GL_ONE)
#define GL_PLAYER_START_Y (104 * GL_ONE)

// --- speeds (units per frame) -----------------------------------------------
#define GL_PLAYER_SPEED 384                  // 1.50 px/frame
#define GL_PLAYER_DIAG 271                   // 384 * 181/256 (~1/sqrt2)
#define GL_SHAMBLER_SPEED 192                // 0.75 px/frame
#define GL_SHAMBLER_DIAG 135                 // 192 * 181/256

// Shambler axis deadzone: don't oscillate across the player's axis.
#define GL_AXIS_DEADZONE 128                 // 0.5 px

// Contact = death (Chebyshev distance between centers).
#define GL_CONTACT_RANGE (10 * GL_ONE)

// Spawns never land inside this radius of the player's night-start position.
#define GL_SAFE_RADIUS (64 * GL_ONE)

// --- waves (slice 4) ----------------------------------------------------------
// Concurrent-zombie cap: 24 sprites (concept doc — NDS OAM gives 128/engine,
// we cap at 24 for CPU and proof headroom).
#define GL_ZOMBIE_CAP 24
// All of a night's zombies spawn inside the first GL_WAVE_SPAWN_SPAN frames
// (40 s of the 60 s night), evenly spread: the crowd only grows until dawn.
#define GL_WAVE_SPAWN_SPAN 2400

// --- shove (slice 4) ----------------------------------------------------------
// The shove (A) is a pressure valve, not a weapon: if the NEAREST Shambler
// is within GL_SHOVE_RANGE it is knocked GL_SHOVE_PUSH back (per axis, away
// from the player, clamped to the arena) and stunned GL_SHOVE_STUN frames.
// Any attempt (hit or whiff) arms the GL_SHOVE_COOLDOWN — no mashing.
#define GL_SHOVE_RANGE (24 * GL_ONE)
#define GL_SHOVE_PUSH (40 * GL_ONE)          // 5 tiles of breathing room
#define GL_SHOVE_STUN 45                     // 0.75 s frozen
#define GL_SHOVE_COOLDOWN 90                 // 1.5 s between attempts

// --- barricades (slice 5) ------------------------------------------------------
// B places a barricade at the lamplighter's feet (1 plank), or — if an
// intact one is within GL_BARRICADE_RANGE — repairs it to full instead
// (1 plank; a full barricade is never "repaired", no plank is wasted).
// The dead cannot ENTER a barricade's radius: a Shambler step that would
// end inside an intact barricade's GL_BARRICADE_RANGE (from outside it)
// is blocked, and the blocked attempt chews the barricade for exactly
// 1 hp instead. hp 0 = breached (slot free again). NO-TRAP INVARIANTS BY
// CONSTRUCTION: the player is never an argument to the block predicate
// (can't be sealed in), a body already inside the radius may always
// leave (no pinning), and every blocked attempt costs 1 hp (a wall is a
// timer — the dead always get through eventually).
// Resource: a starting stock of planks per run + a small dawn grant
// (capped). The concept doc's scavenge interlude (NEXT slice) becomes
// the real plank source; this slice's stock is the placeholder economy.
#define GL_BARRICADE_CAP 8
#define GL_BARRICADE_HP 240                  // 4 s of one Shambler chewing
#define GL_BARRICADE_RANGE (16 * GL_ONE)     // block/repair radius
#define GL_PLANK_STOCK 6                     // planks at run start
#define GL_PLANK_DAWN 2                      // planks granted at each dawn
#define GL_PLANK_MAX 9                       // pocket cap

// --- scavenge interlude (slice 6) ----------------------------------------------
// The concept doc's between-nights phase, and slice 5's flagged "real
// plank source": at the dawn card, SELECT (a NEW button — START keeps
// the old skip-straight-to-night path bit-identical) enters a timed
// daylight interlude in the same yard. The lamplighter returns to the
// lamppost (the night-start position — so the spawn safe radius
// guarantees no instant death), the leftover dead fall back to the
// fence line (their own night spawn points — pure, already proven
// outside the safe radius) and keep walking in, and GL_CACHE_COUNT
// plank caches lie scattered on a pure f(seed, night, index) schedule.
// Walking within GL_CACHE_RANGE of a cache pockets GL_CACHE_PLANKS
// plank(s) — but NEVER on a full pocket (no cache is wasted; it stays
// on the ground). START leaves early; when the GL_SCAVENGE_FRAMES of
// dawn light run out the next night begins. Taking the interlude
// REPLACES the flat GL_PLANK_DAWN skip grant: loot what you carry out.
// Contact still kills — the interlude is bought planks, not safety.
#define GL_SCAVENGE_FRAMES 1200              // 20 s of dawn light
#define GL_CACHE_COUNT 5                     // caches per interlude
#define GL_CACHE_PLANKS 1                    // planks per cache
#define GL_CACHE_RANGE (12 * GL_ONE)         // pickup reach
#define GL_CACHE_INSET (16 * GL_ONE)         // caches keep off the fence
#define GL_CACHE_SALT 0x5CAF0157u            // cache hash stream != spawns

// --- night clock -------------------------------------------------------------
// One night = 60 s at 60 fps. Headless DeSmuME runs ~800 fps in CI, so the
// survive-to-dawn proof replays a FULL night on the shipped ROM — no
// shortened test-night build needed at skeleton stage.
#define GL_NIGHT_FRAMES 3600

// --- pure functions ----------------------------------------------------------

// Deterministic 32-bit hash of two words (the concept's "no runtime RNG":
// every stagger/spawn decision is a hash, never a generator with state).
uint32_t gl_hash(uint32_t a, uint32_t b);

// Spawn schedule: position of zombie `index` of night `night` for `seed`,
// as a point on the fence perimeter, guaranteed outside GL_SAFE_RADIUS of
// the player's night-start position. Pure f(seed, night, index).
void gl_spawn_of_night(uint32_t seed, uint32_t night, uint32_t index,
                       int32_t *x, int32_t *y);

// One player step: 8-way input flags -> new clamped position.
void gl_player_step(int32_t *px, int32_t *py,
                    int up, int down, int left, int right);

// One Shambler step toward the player: fixed sub-pixel speed with a
// per-zombie deterministic stagger (hash of (zombie_id, frame) skips 1
// frame in 4). Pure f(zombie_state, player_pos, frame).
void gl_shambler_step(int32_t *zx, int32_t *zy, int32_t px, int32_t py,
                      uint32_t zombie_id, uint32_t frame);

// Chebyshev distance between two centers (the contact metric).
int32_t gl_chebyshev(int32_t ax, int32_t ay, int32_t bx, int32_t by);

// 1 if the cold hands reach you.
int gl_contact(int32_t px, int32_t py, int32_t zx, int32_t zy);

// Wave size of night `night` (1-based): 1, 3, 5, ... ramping by 2 per night,
// plateauing at GL_ZOMBIE_CAP (night 13+). Pure f(night).
uint32_t gl_wave_count(uint32_t night);

// Night-frame on which zombie `index` of night `night` spawns: index 0 at
// frame 0, the rest evenly spread across GL_WAVE_SPAWN_SPAN. Together with
// gl_spawn_of_night(seed, night, index) this is the concept's spawn
// schedule as a pure function of (seed, night, index).
uint32_t gl_spawn_frame(uint32_t night, uint32_t index);

// The shove: if the Shambler at (*zx,*zy) is within GL_SHOVE_RANGE of the
// player, push it GL_SHOVE_PUSH away per axis (sign of player->zombie
// delta, GL_AXIS_DEADZONE like the chase step), clamped to the arena, and
// return 1. Out of range: no movement, returns 0. Stun/cooldown state
// lives in the caller (main loop) using GL_SHOVE_STUN / GL_SHOVE_COOLDOWN.
int gl_shove(int32_t px, int32_t py, int32_t *zx, int32_t *zy);

// Barricade block predicate: 1 if a move from (ox,oy) to (nx,ny) is
// blocked by an intact barricade at (bx,by) — i.e. the new position is
// inside GL_BARRICADE_RANGE while the old one was not. Entering is
// blocked; standing inside (spawn/placement overlap) may always walk
// out. A no-move "step" (stagger frame) is never blocked. Pure.
int gl_barricade_blocks(int32_t bx, int32_t by,
                        int32_t ox, int32_t oy, int32_t nx, int32_t ny);

// Plank grant at dawn: current pocket + GL_PLANK_DAWN, capped at
// GL_PLANK_MAX. Pure f(planks).
uint32_t gl_planks_at_dawn(uint32_t planks);

// Cache schedule: position of plank cache `index` of night `night`'s
// interlude for `seed`, an interior point at least GL_CACHE_INSET off
// the fence line (never ON the spawn perimeter). Pure f(seed, night,
// index), on a salted hash stream disjoint from the spawn schedule.
void gl_cache_of_interlude(uint32_t seed, uint32_t night, uint32_t index,
                           int32_t *x, int32_t *y);

// 1 if the lamplighter can pocket the cache (Chebyshev reach, same
// metric family as contact/shove/barricade).
int gl_cache_grab(int32_t px, int32_t py, int32_t cx, int32_t cy);

// Pocket after grabbing a cache: + GL_CACHE_PLANKS, capped at
// GL_PLANK_MAX. Pure f(planks). (The caller must not consume a cache
// on a full pocket — gl_cache_grab + a planks < GL_PLANK_MAX check.)
uint32_t gl_planks_after_grab(uint32_t planks);

#endif // GL_SIM_H
