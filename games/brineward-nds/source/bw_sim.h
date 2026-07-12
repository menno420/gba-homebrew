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

// --- loot / gold (arc slice 3: roadmap item 1) -------------------------------
// A sunk rum-runner breaks up into flotsam: BW_LOOT_DROPS crates on a
// BW_LOOT_RING around the wreck at fixed third-of-circle angles (pure
// f(wreck position) — deliberately NOT frame-hashed, so a +-frame input
// skew moves the crates only as far as it moves the wreck). Sail within
// BW_SCOOP_RANGE to scoop; the hold caps at BW_HOLD_CAP crates. Banking:
// lie alongside the Graywake pier (BW_PIER_*, the breakwater south of
// the anchorage) within BW_DOCK_RANGE and the hold empties into banked
// gold. Banked gold is safe forever; sinking forfeits the unbanked hold
// (main.c simply does not carry it into the next duel).
#define BW_MAX_LOOT 8
#define BW_LOOT_DROPS 3                  // crates per sunk ship
#define BW_LOOT_VALUE 5                  // gold per crate (band-0 waters)
#define BW_HOLD_CAP 8                    // crates the sloop can carry
#define BW_LOOT_RING 18                  // px, crate ring radius off the wreck
#define BW_SCOOP_RANGE (10 * BW_ONE)     // chebyshev hull<->crate
#define BW_PIER_X (128 * BW_ONE)
#define BW_PIER_Y (172 * BW_ONE)
#define BW_DOCK_RANGE (12 * BW_ONE)      // chebyshev hull<->pier

// --- port + upgrades (arc slice 4: roadmap item 2) ---------------------------
// Banked gold buys PERSISTENT player-ship upgrades at the Graywake port
// (main.c hangs the menu off the same pier berth the hold banks at).
// Three tracks, tiers I/II/III (0-based 0..2), concept-doc shaped:
//   hull    — max HP 100/150/220 (the doc's exact ladder);
//   cannons — battery reload 90/70/55 frames AND balls per rake 3/3/4;
//   sails   — trim-target speed +0/+24/+48 units/f and turn +0/+1/+2.
// Tier 0 IS the slice-2 sloop: every tier-0 table entry equals the
// legacy constant, so all recorded routes and carried proof pins stay
// bit-valid (check-brine.py asserts this identity). The ENEMY never
// upgrades — the tables apply to the player only. Prices triple per
// step (one banked band-0 wreck = 15g buys the first tier of one
// track); repair prices the MISSING hull at BW_REPAIR_PER_GOLD points
// per gold, rounded against the player. All one-constant owner-tunables
// (tables live in bw_sim.c next to the trim tables).
#define BW_UP_TIERS 3
#define BW_REPAIR_PER_GOLD 4             // hull points one gold repairs
// port menu rows (bw_port_buy's ledger; main.c renders the same order)
#define BW_PORT_ROW_HULL 0
#define BW_PORT_ROW_CANNON 1
#define BW_PORT_ROW_SAIL 2
#define BW_PORT_ROW_REPAIR 3
#define BW_PORT_ROWS 4

// --- the Maw (arc slice 5: roadmap item 3 — first sea monster) ----------------
// The break-the-geometry enemy class (concept doc: "a maw that surfaces
// under you (watch the shadow)"; roadmap: "shadow telegraph -> surface ->
// lunge"). The wreck's blood draws it: BW_MAW_PATIENCE salvage frames
// after the rum-runner goes under, a shadow rises AT THE WRECK (pure
// f(wreck position), same rationale as the crate ring — no frame hash)
// and homes under the player; after a fixed telegraph it SURFACES where
// the shadow lies, winds up, then LUNGES at the player's latched bearing
// — a bite costs BW_MAW_BITE hull, then it sounds and stalks again.
// It has no batteries and no beam to keep: the broadside rules do not
// apply to it — but they apply to YOU: while it is up (surface/lunge)
// the salvage-water batteries wake and a rake can wound it. Slay it
// (BW_MAW_HULL) and it breaks up like any wreck — richer: BW_MAW_LOOT_
// DROPS crates worth BW_MAW_LOOT_VALUE each (the doc's "monster drops
// more"). The pier is SANCTUARY (session-28 guard recipe): the shadow
// never enters, it never surfaces, and it never begins a lunge at a
// player inside BW_MAW_HARBOR of the berth — the port can always be
// reached. The Maw exists only in the salvage water: bw_duel_init
// resets it and bw_duel_step never steps it, so every duel-phase route
// and pin is bit-identical to slice 4. All one-constant owner-tunables.
#define BW_MAW_PATIENCE 600              // quiet salvage frames before the stir
#define BW_MAW_RESTIR 240                // frames until it stalks again
#define BW_MAW_SHADOW_FRAMES 150         // telegraph: the shadow homes this long
#define BW_MAW_SURFACE_FRAMES 60         // windup on the surface before the lunge
#define BW_MAW_LUNGE_FRAMES 55           // committed lunge: ~112 px of reach —
                                         //   a full-sail beam dodge clears it
#define BW_MAW_SHADOW_SPEED 140          // units/frame — full sail outruns it
                                         //   even diagonally (224*sin45 ~ 158);
                                         //   a battle-sail scooper (96) does not
#define BW_MAW_LUNGE_SPEED 520           // units/frame — no sail outruns THIS
#define BW_MAW_HULL 120                  // ~two clean rakes of the tier-0 battery
#define BW_MAW_HIT_RANGE (12 * BW_ONE)   // it is a big target (ball chebyshev)
#define BW_MAW_BITE_RANGE (12 * BW_ONE)  // jaws chebyshev
#define BW_MAW_BITE 35                   // hull points a bite costs
#define BW_MAW_HARBOR (40 * BW_ONE)      // pier sanctuary radius (chebyshev)
#define BW_MAW_LOOT_DROPS 3
#define BW_MAW_LOOT_VALUE 15             // per crate — monster salvage is richer
// Maw lifecycle states
#define BW_MAW_DOWN 0                    // beneath the water, biding
#define BW_MAW_SHADOW 1                  // the telegraph: dark water, homing
#define BW_MAW_SURFACE 2                 // up and vulnerable, winding up
#define BW_MAW_LUNGE 3                   // committed charge at the latched bearing

// --- wind + sailing feel (arc slice 6: roadmap item 4) -------------------------
// The concept doc's owner-taste slice: "a slow-rotating wind vector,
// trim interacts with it". Each water gets a WEATHER from its seed
// (pure f(seed), printed on the HUD like the seed itself): dead calm,
// a breeze, or a gale. The wind VECTOR rotates slowly all run long
// (one heading unit every 2^BW_WIND_TURN_SHIFT frames — a full box of
// the compass in ~2:16), and the point of sail scales the trim's
// target speed: running with the wind fills the canvas (+push),
// beating into it fights the hull (-push), abeam is neutral —
// cos-shaped between, and the more canvas set, the more the weather
// matters (full sail catches 4/4 of the push, half 2/4, battle 1/4:
// the trim interaction the doc asks for; shortening sail is now ALSO
// the storm verb). The wind moves HULLS only — iron balls and the Maw
// (which swims UNDER the water) don't care; the broadside and
// telegraph contracts are untouched by weather.
//
// PIN-CARRY RULE (decide-and-flag): BW_WIND_SALT is chosen so every
// committed proof/recorder anchor seed (116, 117, 118, 119, 126, 127,
// 558, 728) maps to CALM — a calm water's wind term is exactly zero,
// so every slice-2..5 recorded route and emulator pin is bit-identical
// and carries verbatim (the Maw-slice gate pattern: the new mechanic
// provably never fires inside a committed story). check-brine.py
// asserts the calm-anchor identity loudly, so a salt or table retune
// fails the mirror before it silently re-times a route.
//
// Escapability rails survive the weather (asserted in the mirror):
// a gale-beating full sail still outruns the shadow on its worst
// (diagonal) axis ((224-24)*181>>8 = 141 > 140), and a gale-running
// battle-sail scooper still does NOT (96+6 = 102 < 140).
#define BW_WIND_CALM 0
#define BW_WIND_BREEZE 1
#define BW_WIND_GALE 2
#define BW_WIND_STATES 3
#define BW_WIND_SALT 0x57499826u         // level = f(seed); calm anchors, see above
#define BW_WIND_DIR_SALT 0x57492881u     // base bearing = f(seed)
#define BW_WIND_TURN_SHIFT 3             // 1 heading unit per 8 frames (slow box)

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
    int32_t x, y;                        // 8.8 fixed, crate center
    int32_t value;                       // gold when scooped (slice 5: the
    int32_t live;                        //   Maw's crates are richer)
} BwLoot;

typedef struct
{
    int32_t x, y;                        // 8.8 fixed (shadow or surfaced body)
    int32_t vx, vy;                      // latched lunge velocity, units/frame
    int32_t state;                       // BW_MAW_*
    int32_t hull;                        // 0..BW_MAW_HULL
    int32_t timer;                       // frames in the current state
    uint32_t wake;                       // d->frame at which the next stir fires
    int32_t stirs;                       // times it has risen this water
    int32_t slain;                       // 1: dead — never stirs again
    int32_t bit;                         // this lunge already landed its bite
} BwMaw;

typedef struct
{
    BwShip player;
    BwShip enemy;
    BwBall balls[BW_MAX_BALLS];
    BwLoot loot[BW_MAX_LOOT];            // flotsam afloat (slice 3)
    BwMaw maw;                           // the salvage-water stalker (slice 5)
    int32_t hold;                        // crates aboard, 0..BW_HOLD_CAP
    int32_t hold_gold;                   // unbanked gold those crates are worth
    int32_t up_hull;                     // player upgrade tiers, 0..2
    int32_t up_cannon;                   //   (slice 4; enemy never upgrades;
    int32_t up_sail;                     //    caller re-injects, like hold)
    int32_t wind_level;                  // this water's weather, BW_WIND_*
    int32_t wind_base;                   //   (slice 6; both pure f(seed))
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

// The wind right now (slice 6): the heading the wind blows TOWARD
// (0..1023 — sailors would name it by the opposite bearing), rotating
// slowly off the water's seeded base; and the push (units/frame at
// full canvas) of this water's weather — 0 in a calm, so a calm water
// is bit-identical to slice 5.
int32_t bw_wind_heading(const BwDuel *d);
int32_t bw_wind_push(const BwDuel *d);

// Reset a duel: player sloop at the anchorage, enemy sloop on a
// BW_SPAWN_DIST ring at hash angle f(seed), bow toward the player.
void bw_duel_init(BwDuel *d, uint32_t seed);

// Enemy sail AI: pure f(enemy, player). Intercept beyond BW_ENGAGE_RANGE
// (full sail, bow on), circle inside it (battle sail, hold the player
// abeam), fire the facing battery when aligned, ranged, and reloaded.
void bw_ai(const BwShip *e, const BwShip *p, BwInputs *out);

// One duel frame: player inputs + enemy AI + both ships step + broadsides
// spawn + balls fly + hits land + sink check (the sink frame drops the
// wreck's flotsam) + scoop pass. Sets d->over.
void bw_duel_step(BwDuel *d, const BwInputs *in);

// One salvage frame (after the enemy went under): the player sails on,
// lingering balls fly out (a late rake can still strike — sinking during
// salvage flips d->over to BW_DUEL_PLAYER_SUNK), the Maw stalks (slice
// 5: shadow telegraph -> surface -> lunge; a bite or a lingering rake
// can drag the salvage down), crates are scooped. Batteries stay cold
// while the water is quiet — they WAKE while the Maw is up (fire_l/
// fire_r reach the guns only in BW_MAW_SURFACE / BW_MAW_LUNGE /
// BW_MAW_SHADOW states; no fire verb without a foe).
void bw_salvage_step(BwDuel *d, const BwInputs *in);

// Pier pass, run once per live-water frame by the caller: if the player
// lies alongside the Graywake pier with cargo aboard, the hold empties.
// Returns the gold banked this frame (0 if not docked or nothing held).
int32_t bw_dock_step(BwDuel *d);

// --- port + upgrades (slice 4) -----------------------------------------------

// The player sloop's max hull / battery reload at an upgrade tier, and
// the gold price of buying INTO a tier (tier 0 is free — the start ship).
int32_t bw_up_hull_max(int32_t tier);
int32_t bw_up_reload(int32_t tier);
int32_t bw_up_price(int32_t tier);

// Gold to repair the player hull to its tier max (0 = already sound).
// Prices BW_REPAIR_PER_GOLD missing points per gold, rounded UP.
int32_t bw_repair_cost(const BwDuel *d);

// Pure port ledger: attempt to buy menu row `row` (BW_PORT_ROW_*) with
// `gold` banked. Returns the gold actually spent — 0 means REFUSED
// (unknown row, tier already maxed, hull already sound, or gold short of
// the price; a refusal changes nothing). An upgrade row bumps the tier;
// the repair row refills the hull to its tier max.
int32_t bw_port_buy(BwDuel *d, int32_t row, int32_t gold);

#endif // BW_SIM_H
