// Gloamline — pure simulation layer (arc slice 7: lantern-oil light
// pressure, on the slice-6 scavenge interlude).
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

// --- lantern oil (slice 7) -------------------------------------------------------
// The concept doc's stated pressure: "lantern oil — light radius shrinks
// as it burns, dark edges spawn faster; the Lumen-Drift-style
// decay-you-can-buy-back pressure." The lantern burns GL_OIL_BURN oil
// every NIGHT frame (daylight — the interlude and the cards — burns
// nothing). While the tank holds at least GL_OIL_LOW the light is full
// (GL_LIGHT_R_MAX) and NOTHING below changes — that is the zero-re-pin
// gate: a fresh lantern (GL_OIL_MAX = 3 nights of burn) outlasts every
// window a pre-slice-7 proof crosses. Once oil runs LOW the light
// radius shrinks linearly toward GL_LIGHT_R_MIN (the lantern gutters
// but NEVER dies — the pressure is speed, not an unwinnable blackout),
// and the dead beyond the lamplight stop hesitating: the 1-in-4
// stagger skip is cancelled outside the light circle (gl_dark_press),
// so the gloam presses ~33% faster where you cannot see. Buy-back:
// GL_FLASK_COUNT oil flasks lie in the scavenge interlude on their own
// salted pure schedule (gl_flask_of_interlude), each worth GL_OIL_FLASK
// (one night of burn), capped at GL_OIL_MAX and NEVER consumed on a
// full tank (no flask is wasted — slice 6's no-waste rule). The START
// skip path grants NO oil: like the planks, the interlude is the only
// source (loot-replaces-grant, reused). All numbers are decide-and-flag
// owner-tunables.
#define GL_OIL_MAX 10800                     // a fresh lantern: 3 nights
#define GL_OIL_BURN 1                        // oil per night frame
#define GL_OIL_LOW 3000                      // below this the light fails
#define GL_LIGHT_R_MAX (80 * GL_ONE)         // full lamplight radius
#define GL_LIGHT_R_MIN (24 * GL_ONE)         // guttering — never darker
#define GL_FLASK_COUNT 2                     // flasks per interlude
#define GL_OIL_FLASK 3600                    // one night of burn each
#define GL_FLASK_SALT 0xF1A5C01Du            // flask hash stream != caches

// --- synthesized audio (slice 8) -------------------------------------------------
// The concept doc's "synthesized audio set", per session 27's guard
// recipe: playback lives on the NDS hardware PSG channels (square-wave
// + white-noise generators driven by the default BlocksDS ARM7 core
// over the libnds sound FIFO) — every sound is synthesized in code
// from the pure parameters below, there is NO sampled or binary audio
// asset anywhere in the repo, and the ARM9 per-frame cost is a few
// integer compares (the slice-7 frame budget left 1 modeled scanline).
// The DECISION layer is pure and mirrored: gl_amb_tier says which
// ambience the night wants this frame, gl_cue_* say what each one-shot
// cue sounds like, and the cue ids double as the priority order (on a
// frame with several events the HIGHEST id fires the one SFX channel).
// Playback itself (FIFO writes) is render-like ARM9 glue in main.c —
// no sim state feeds back from audio, so every pre-slice-8 pin holds.
// All numbers are decide-and-flag owner-tunables (one table row per
// sound in gl_sim.c).
//
// Ambience tiers: the night drone tracks the lantern.
#define GL_AMB_TIER_DAY 0                    // scavenge daylight: calm
#define GL_AMB_TIER_NIGHT 1                  // lit night: the gloam hum
#define GL_AMB_TIER_GUTTER 2                 // oil LOW: the lamp gutters
#define GL_AMB_TIER_PRESS 3                  // dark press live: it comes
#define GL_AMB_TIERS 4
//
// One-shot cue ids (0 = none). ID ORDER IS PRIORITY: death > dawn >
// nightfall > breach > flask > cache > plank > shove.
#define GL_CUE_NONE 0
#define GL_CUE_SHOVE 1                       // a body thumped off the light
#define GL_CUE_PLANK 2                       // the knock of a placed plank
#define GL_CUE_CACHE 3                       // planks pocketed
#define GL_CUE_FLASK 4                       // brass flask, bright slosh
#define GL_CUE_BREACH 5                      // a barricade splinters
#define GL_CUE_NIGHTFALL 6                   // the night tolls in
#define GL_CUE_DAWN 7                        // the dawn bell
#define GL_CUE_DEATH 8                       // the cold hands
#define GL_CUE_COUNT 9                       // ids 0..8
//
// gl_cue_duty(cue) == GL_CUE_ON_NOISE routes the cue to the hardware
// noise channel (soundPlayNoise) instead of a square wave; otherwise
// the value is the libnds DutyCycle register code (0..7).
#define GL_CUE_ON_NOISE 255

// --- best-nights save (slice 9) ------------------------------------------------
// The concept doc's "save-file best-nights": the lamplighter's best-run
// record persists across power cycles on the cartridge backup chip
// (EEPROM over the card SPI bus — the battery save DeSmuME emulates as
// a .dsv file). The record is one fixed 32-byte blob: 8 little-endian
// u32 words { magic, version, best nights survived, seed of that run,
// 3 spares (0), checksum-of-words-0..6 }, so serialization is fully
// deterministic and one word 4 = one type-2 EEPROM page write.
// SAFETY BY CONSTRUCTION: gl_save_decode returns 0 (and the caller
// keeps the fresh all-zero table) on ANY bad blob — wrong magic, wrong
// version, bad checksum, blank/0xFF chip — a corrupt save can reset
// the record, NEVER crash or hang the game. Writes are wear-disciplined:
// the ARM9 writes the blob ONLY on a dawn that strictly improves the
// record (gl_record_improves) — never per frame, never at death (a
// death cannot improve a best-nights record). All constants are
// decide-and-flag owner-tunables; GL_SAVE_EEPROM_TYPE is the libnds
// addrtype the ROM ASSUMES (64Kbit-class EEPROM, 2-byte addressing) —
// a fixed constant, not a boot-time chip probe.
#define GL_SAVE_MAGIC 0x474C5356u            // 'GLSV'
#define GL_SAVE_VERSION 1u                   // bump on any layout change
#define GL_SAVE_WORDS 8
#define GL_SAVE_BYTES 32                     // one type-2 EEPROM page
#define GL_SAVE_ADDR 0                       // blob offset in the backup
#define GL_SAVE_EEPROM_TYPE 2                // libnds addrtype (see card.h)
#define GL_SAVE_SALT 0x53415645u             // 'SAVE' checksum stream
// Iteration cap on every card-SPI wait in the write path (main.c's
// save_write_backup): libnds's cardWriteEeprom polls the chip's status
// register UNBOUNDEDLY and hangs forever under DeSmuME's backup
// emulation (measured in this slice: the ROM froze on the first dawn
// write). Every wait in our write flow gives up after this many
// iterations instead. Sizing (both rails measured in this slice):
// big enough that ~4096 RDSR polls outlast a real EEPROM's ~5 ms page
// program with margin, small enough that a chip/emulator that NEVER
// answers (DeSmuME reads the status as busy forever) costs only a
// bounded sub-frame stall on the one dawn-card frame — at 65536 the
// expired poll ate 2 whole emulated frames and every post-dawn pin
// would have shifted. An expired poll is harmless by construction:
// the page data commits on chip-deselect and the chip finishes
// programming internally; the next backup command is minutes away
// (the next improving dawn) or next power-on.
#define GL_SAVE_POLL_BOUND 4096u

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
// Slice 7 decomposition (bit-identical): gl_shambler_step ==
// gl_shambler_staggers ? no-op : gl_shambler_stride. The caller may
// cancel a stagger with gl_dark_press (the dead don't hesitate in the
// dark) by calling gl_shambler_stride directly.
void gl_shambler_step(int32_t *zx, int32_t *zy, int32_t px, int32_t py,
                      uint32_t zombie_id, uint32_t frame);

// 1 if this zombie's chase step is a stagger skip this frame (the
// hash-of-(id, frame) 1-in-4 rule, extracted verbatim from
// gl_shambler_step).
int gl_shambler_staggers(uint32_t zombie_id, uint32_t frame);

// The chase move itself, sans stagger (extracted verbatim from
// gl_shambler_step).
void gl_shambler_stride(int32_t *zx, int32_t *zy, int32_t px, int32_t py);

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

// The lantern burns one night frame of oil: -GL_OIL_BURN, floored at 0
// (an empty lantern gutters, it does not underflow). Pure f(oil).
uint32_t gl_oil_burn(uint32_t oil);

// Light radius of the lantern at this oil level: GL_LIGHT_R_MAX while
// oil >= GL_OIL_LOW (the zero-re-pin gate — full light changes
// nothing), then shrinking linearly to GL_LIGHT_R_MIN at empty. Pure,
// monotone non-decreasing in oil, never below GL_LIGHT_R_MIN.
int32_t gl_light_radius(uint32_t oil);

// 1 if the dark press cancels a Shambler's stagger skip: the light is
// failing (oil < GL_OIL_LOW) AND the body is beyond the lamplight
// (dist > gl_light_radius(oil)). At full light this is ALWAYS 0 — the
// pre-slice-7 chase is bit-identical. Pure f(oil, dist).
int gl_dark_press(uint32_t oil, int32_t dist);

// Flask schedule: position of oil flask `index` of night `night`'s
// interlude for `seed` — same interior box as the plank caches
// (GL_CACHE_INSET off the fence), on a salted hash stream disjoint
// from both spawns and caches. Pure f(seed, night, index). Pickup
// reach is gl_cache_grab (the same Chebyshev metric family).
void gl_flask_of_interlude(uint32_t seed, uint32_t night, uint32_t index,
                           int32_t *x, int32_t *y);

// Tank after pocketing a flask: + GL_OIL_FLASK, capped at GL_OIL_MAX.
// Pure f(oil). (The caller must not consume a flask on a full tank —
// gl_cache_grab + an oil < GL_OIL_MAX check: no flask is wasted.)
uint32_t gl_oil_after_flask(uint32_t oil);

// Ambience tier this frame (slice 8): DAY in the daylight interlude
// (is_night = 0 — daylight presses nobody and hums calm regardless of
// the tank), else NIGHT while the oil is healthy (>= GL_OIL_LOW — the
// zero-re-pin shape: at full light the night sounds exactly one way),
// GUTTER once the light fails, PRESS when the dark press is live on
// the nearest of the dead (press_nearest = the GL_T_PRESS predicate).
// Pure f(is_night, oil, press_nearest).
uint32_t gl_amb_tier(int is_night, uint32_t oil, int press_nearest);

// Ambience drone parameters of a tier: square-wave frequency (Hz),
// libnds DutyCycle register code (0..7), volume (0..127). Pure lookup;
// out-of-range tiers return the DAY row. gl_amb_freq is strictly
// increasing in tier (the moor hums higher as the night closes in).
uint32_t gl_amb_freq(uint32_t tier);
uint32_t gl_amb_duty(uint32_t tier);
uint32_t gl_amb_vol(uint32_t tier);

// One-shot cue parameters: frequency (Hz — square-wave pitch, or the
// noise-generator rate for GL_CUE_ON_NOISE cues), length the ARM9
// holds the channel open (frames, bounded — a cue can never hog the
// SFX channel), duty (DutyCycle code 0..7, or GL_CUE_ON_NOISE for the
// noise channel), volume (0..127). Pure lookups; GL_CUE_NONE and
// out-of-range ids return 0 across the board (a no-op cue).
uint32_t gl_cue_freq(uint32_t cue);
uint32_t gl_cue_len(uint32_t cue);
uint32_t gl_cue_duty(uint32_t cue);
uint32_t gl_cue_vol(uint32_t cue);

// Checksum of a save blob's words 0..GL_SAVE_WORDS-2 (a gl_hash fold on
// the GL_SAVE_SALT stream). Pure; word GL_SAVE_WORDS-1 is not read.
uint32_t gl_save_checksum(const uint32_t words[GL_SAVE_WORDS]);

// Serialize the best-run record into a GL_SAVE_BYTES blob (slice 9):
// { GL_SAVE_MAGIC, GL_SAVE_VERSION, best_nights, best_seed, 0, 0, 0,
// checksum } as little-endian u32 words. Deterministic: equal records
// encode to identical bytes.
void gl_save_encode(uint32_t best_nights, uint32_t best_seed,
                    uint8_t out[GL_SAVE_BYTES]);

// Parse a blob read back from the backup: returns 1 and fills the
// record ONLY if magic, version AND checksum all match; returns 0 on
// any other input (corrupt, blank, 0xFF, future version) WITHOUT
// touching the outputs — the caller's fresh table stands. Never
// crashes on any 32-byte input. Pure.
int gl_save_decode(const uint8_t in[GL_SAVE_BYTES],
                   uint32_t *best_nights, uint32_t *best_seed);

// 1 if a run of `nights` beats the record — STRICTLY better only
// (equal runs write nothing: EEPROM wear discipline). Pure.
int gl_record_improves(uint32_t best_nights, uint32_t nights);

// --- watch-map geometry + the chalk mark (slice 10) --------------------------
// The concept doc's LAST later-slice cut, "watch-map polish", honoring
// its own watch-map words: "Touch optional, never required (tap the
// map to drop a marker at most); the game is 100% playable on
// buttons." The lamplighter chalks ONE mark on the watch-map: X
// places it at the player's own position / clears it again
// (buttons-first — BINDING), and tapping the map plot is the optional
// alias that drops or moves the mark to the tapped cell. The mark is
// chalk ON THE MAP, not a thing in the yard: nothing in the sim reads
// it (the dead ignore chalk), so every pre-slice-10 pin holds
// bit-identically. It persists across nights within a run (chalk
// doesn't wash off at dawn) and wipes on a fresh run.
//
// The map plot geometry (previously main.c statics, moved here
// VERBATIM so the render is bit-identical and the inverse is
// provable): a GL_MAP_COLS x GL_MAP_ROWS console-cell plot at
// (GL_MAP_COL0, GL_MAP_ROW0), each cell GL_MAP_CELL_PX x
// GL_MAP_CELL_PX LCD pixels; yard pixel spans x 16..239 / y 32..175
// (the sprite-center arena) map linearly onto it.
#define GL_MAP_COL0 2
#define GL_MAP_ROW0 5
#define GL_MAP_COLS 28
#define GL_MAP_ROWS 14
#define GL_MAP_CELL_PX 8                     // console cell = 8x8 LCD px

// Yard position (8.8 fixed) -> watch-map plot column/row (clamped to
// the plot). Pure; the exact math main.c has rendered with since
// slice 3.
int gl_map_col(int32_t x);
int gl_map_row(int32_t y);

// Chalk-mark placement from a map plot cell: returns 1 and fills the
// yard position (8.8 fixed) of the CELL's mid-span iff (col, row) is
// inside the plot; returns 0 (outputs untouched) for any cell off the
// plot — a tap on the border/header/gauges drops nothing. EXACT
// ROUND-TRIP BY CONSTRUCTION (proved host-side for every plot cell):
// gl_map_col/gl_map_row of the returned position give back exactly
// (col, row) — the mark renders in the very cell you tapped. The
// returned point is strictly interior to the arena. Pure.
int gl_mark_of_cell(int col, int row, int32_t *x, int32_t *y);

// The optional touch alias, fully pure: bottom-screen LCD pixel
// (tx, ty) -> the plot cell under the stylus -> gl_mark_of_cell.
// Same contract (1 + yard position inside the plot, else 0).
int gl_mark_of_touch(int tx, int ty, int32_t *x, int32_t *y);

// The watch line's count (slice 10): how many of tonight's dead are
// still OUT in the gloam — scheduled but not yet over the fence.
// Pure f(wave_total, spawned), floored at 0 (never underflows).
uint32_t gl_gloam_out(uint32_t wave_total, uint32_t spawned);

#endif // GL_SIM_H
