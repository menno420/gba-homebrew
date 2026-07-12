// Gloamline — pure simulation layer, implementation. See gl_sim.h.
//
// MIRROR RULE (keep in lockstep): tools/check-gloam.py mirrors every
// function here line-for-line. Change both in the same PR.

#include "gl_sim.h"

uint32_t gl_hash(uint32_t a, uint32_t b)
{
    uint32_t h = (a * 0x9E3779B9u) ^ (b * 0x85EBCA6Bu);
    h ^= h >> 16;
    h *= 0x7FEB352Du;
    h ^= h >> 15;
    h *= 0x846CA68Bu;
    h ^= h >> 16;
    return h;
}

static int32_t clamp32(int32_t v, int32_t lo, int32_t hi)
{
    return v < lo ? lo : (v > hi ? hi : v);
}

void gl_spawn_of_night(uint32_t seed, uint32_t night, uint32_t index,
                       int32_t *x, int32_t *y)
{
    // Perimeter walk in whole pixels: the fence rectangle of sprite-center
    // positions, clockwise from the north-west corner.
    const int32_t w = (GL_ARENA_X_MAX - GL_ARENA_X_MIN) / GL_ONE;   // 223
    const int32_t h = (GL_ARENA_Y_MAX - GL_ARENA_Y_MIN) / GL_ONE;   // 143
    const int32_t per = 2 * (w + h);

    int32_t p = (int32_t)(gl_hash(gl_hash(seed, night), index) % (uint32_t)per);

    for (int guard = 0; guard < 2; guard++)
    {
        int32_t sx, sy;
        if (p < w)                        // north fence, west -> east
        {
            sx = GL_ARENA_X_MIN + p * GL_ONE;
            sy = GL_ARENA_Y_MIN;
        }
        else if (p < w + h)               // east fence, north -> south
        {
            sx = GL_ARENA_X_MAX;
            sy = GL_ARENA_Y_MIN + (p - w) * GL_ONE;
        }
        else if (p < 2 * w + h)           // south fence, east -> west
        {
            sx = GL_ARENA_X_MAX - (p - w - h) * GL_ONE;
            sy = GL_ARENA_Y_MAX;
        }
        else                              // west fence, south -> north
        {
            sx = GL_ARENA_X_MIN;
            sy = GL_ARENA_Y_MAX - (p - 2 * w - h) * GL_ONE;
        }

        if (gl_chebyshev(sx, sy, GL_PLAYER_START_X, GL_PLAYER_START_Y)
                >= GL_SAFE_RADIUS || guard == 1)
        {
            *x = sx;
            *y = sy;
            return;
        }
        p = (p + per / 2) % per;          // too close: opposite fence point
    }
}

void gl_player_step(int32_t *px, int32_t *py,
                    int up, int down, int left, int right)
{
    int32_t dx = (right ? 1 : 0) - (left ? 1 : 0);
    int32_t dy = (down ? 1 : 0) - (up ? 1 : 0);
    int32_t speed = (dx && dy) ? GL_PLAYER_DIAG : GL_PLAYER_SPEED;

    *px = clamp32(*px + dx * speed, GL_ARENA_X_MIN, GL_ARENA_X_MAX);
    *py = clamp32(*py + dy * speed, GL_ARENA_Y_MIN, GL_ARENA_Y_MAX);
}

int gl_shambler_staggers(uint32_t zombie_id, uint32_t frame)
{
    // Per-zombie deterministic stagger: skip 1 frame in 4, so a future
    // crowd smears into a readable arc instead of a stack.
    return (gl_hash(zombie_id, frame) & 3u) == 0u;
}

void gl_shambler_stride(int32_t *zx, int32_t *zy, int32_t px, int32_t py)
{
    int32_t dx = px - *zx;
    int32_t dy = py - *zy;
    int32_t sx = (dx > GL_AXIS_DEADZONE) - (dx < -GL_AXIS_DEADZONE);
    int32_t sy = (dy > GL_AXIS_DEADZONE) - (dy < -GL_AXIS_DEADZONE);
    int32_t speed = (sx && sy) ? GL_SHAMBLER_DIAG : GL_SHAMBLER_SPEED;

    *zx = clamp32(*zx + sx * speed, GL_ARENA_X_MIN, GL_ARENA_X_MAX);
    *zy = clamp32(*zy + sy * speed, GL_ARENA_Y_MIN, GL_ARENA_Y_MAX);
}

void gl_shambler_step(int32_t *zx, int32_t *zy, int32_t px, int32_t py,
                      uint32_t zombie_id, uint32_t frame)
{
    // Slice-7 decomposition, bit-identical to the slice-3 original.
    if (gl_shambler_staggers(zombie_id, frame))
        return;
    gl_shambler_stride(zx, zy, px, py);
}

int32_t gl_chebyshev(int32_t ax, int32_t ay, int32_t bx, int32_t by)
{
    int32_t dx = ax > bx ? ax - bx : bx - ax;
    int32_t dy = ay > by ? ay - by : by - ay;
    return dx > dy ? dx : dy;
}

int gl_contact(int32_t px, int32_t py, int32_t zx, int32_t zy)
{
    return gl_chebyshev(px, py, zx, zy) < GL_CONTACT_RANGE;
}

uint32_t gl_wave_count(uint32_t night)
{
    uint32_t count = 2u * night - 1u;        // 1, 3, 5, ... ramp
    return count > GL_ZOMBIE_CAP ? GL_ZOMBIE_CAP : count;
}

uint32_t gl_spawn_frame(uint32_t night, uint32_t index)
{
    return index * GL_WAVE_SPAWN_SPAN / gl_wave_count(night);
}

int gl_shove(int32_t px, int32_t py, int32_t *zx, int32_t *zy)
{
    if (gl_chebyshev(px, py, *zx, *zy) > GL_SHOVE_RANGE)
        return 0;

    int32_t dx = *zx - px;
    int32_t dy = *zy - py;
    int32_t sx = (dx > GL_AXIS_DEADZONE) - (dx < -GL_AXIS_DEADZONE);
    int32_t sy = (dy > GL_AXIS_DEADZONE) - (dy < -GL_AXIS_DEADZONE);

    *zx = clamp32(*zx + sx * GL_SHOVE_PUSH, GL_ARENA_X_MIN, GL_ARENA_X_MAX);
    *zy = clamp32(*zy + sy * GL_SHOVE_PUSH, GL_ARENA_Y_MIN, GL_ARENA_Y_MAX);
    return 1;
}

int gl_barricade_blocks(int32_t bx, int32_t by,
                        int32_t ox, int32_t oy, int32_t nx, int32_t ny)
{
    return gl_chebyshev(nx, ny, bx, by) < GL_BARRICADE_RANGE
        && gl_chebyshev(ox, oy, bx, by) >= GL_BARRICADE_RANGE;
}

uint32_t gl_planks_at_dawn(uint32_t planks)
{
    uint32_t p = planks + GL_PLANK_DAWN;
    return p > GL_PLANK_MAX ? GL_PLANK_MAX : p;
}

void gl_cache_of_interlude(uint32_t seed, uint32_t night, uint32_t index,
                           int32_t *x, int32_t *y)
{
    // Interior box in whole pixels, inset GL_CACHE_INSET from the fence
    // so a cache never lands on the spawn perimeter.
    const int32_t w = (GL_ARENA_X_MAX - GL_ARENA_X_MIN
                       - 2 * GL_CACHE_INSET) / GL_ONE;           // 191
    const int32_t h = (GL_ARENA_Y_MAX - GL_ARENA_Y_MIN
                       - 2 * GL_CACHE_INSET) / GL_ONE;           // 111

    uint32_t hx = gl_hash(gl_hash(seed ^ GL_CACHE_SALT, night), index);
    uint32_t hy = gl_hash(hx, index);

    *x = GL_ARENA_X_MIN + GL_CACHE_INSET
         + (int32_t)(hx % (uint32_t)(w + 1)) * GL_ONE;
    *y = GL_ARENA_Y_MIN + GL_CACHE_INSET
         + (int32_t)(hy % (uint32_t)(h + 1)) * GL_ONE;
}

int gl_cache_grab(int32_t px, int32_t py, int32_t cx, int32_t cy)
{
    return gl_chebyshev(px, py, cx, cy) < GL_CACHE_RANGE;
}

uint32_t gl_planks_after_grab(uint32_t planks)
{
    uint32_t p = planks + GL_CACHE_PLANKS;
    return p > GL_PLANK_MAX ? GL_PLANK_MAX : p;
}

uint32_t gl_oil_burn(uint32_t oil)
{
    return oil > GL_OIL_BURN ? oil - GL_OIL_BURN : 0;
}

int32_t gl_light_radius(uint32_t oil)
{
    if (oil >= GL_OIL_LOW)
        return GL_LIGHT_R_MAX;
    // Linear gutter: (R_MAX - R_MIN) * oil fits int32 (14336 * 2999).
    return GL_LIGHT_R_MIN
        + (int32_t)((GL_LIGHT_R_MAX - GL_LIGHT_R_MIN) * oil / GL_OIL_LOW);
}

int gl_dark_press(uint32_t oil, int32_t dist)
{
    return oil < GL_OIL_LOW && dist > gl_light_radius(oil);
}

void gl_flask_of_interlude(uint32_t seed, uint32_t night, uint32_t index,
                           int32_t *x, int32_t *y)
{
    // Same interior box as the plank caches (inset off the fence), own
    // salted stream so flasks and caches are independent draws.
    const int32_t w = (GL_ARENA_X_MAX - GL_ARENA_X_MIN
                       - 2 * GL_CACHE_INSET) / GL_ONE;           // 191
    const int32_t h = (GL_ARENA_Y_MAX - GL_ARENA_Y_MIN
                       - 2 * GL_CACHE_INSET) / GL_ONE;           // 111

    uint32_t hx = gl_hash(gl_hash(seed ^ GL_FLASK_SALT, night), index);
    uint32_t hy = gl_hash(hx, index);

    *x = GL_ARENA_X_MIN + GL_CACHE_INSET
         + (int32_t)(hx % (uint32_t)(w + 1)) * GL_ONE;
    *y = GL_ARENA_Y_MIN + GL_CACHE_INSET
         + (int32_t)(hy % (uint32_t)(h + 1)) * GL_ONE;
}

uint32_t gl_oil_after_flask(uint32_t oil)
{
    uint32_t o = oil + GL_OIL_FLASK;
    return o > GL_OIL_MAX ? GL_OIL_MAX : o;
}

uint32_t gl_amb_tier(int is_night, uint32_t oil, int press_nearest)
{
    if (!is_night)
        return GL_AMB_TIER_DAY;
    if (oil >= GL_OIL_LOW)
        return GL_AMB_TIER_NIGHT;            // full light: one sound only
    return press_nearest ? GL_AMB_TIER_PRESS : GL_AMB_TIER_GUTTER;
}

// Ambience drone rows, indexed by tier: { freq Hz, duty code, vol }.
// One row per tier — the owner-tunable table. The drone is a low
// square wave; the duty widens and the pitch/volume climb as the
// night closes in (DAY 12.5% barely-there hum -> PRESS 50% throb).
static const uint16_t GL_AMB_ROWS[GL_AMB_TIERS][3] = {
    { 55, 0, 10 },                           // DAY: the moor at dawn
    { 65, 1, 18 },                           // NIGHT: the gloam hum
    { 82, 2, 30 },                           // GUTTER: the lamp fails
    { 110, 3, 44 },                          // PRESS: the dark comes on
};

uint32_t gl_amb_freq(uint32_t tier)
{
    return GL_AMB_ROWS[tier < GL_AMB_TIERS ? tier : 0][0];
}

uint32_t gl_amb_duty(uint32_t tier)
{
    return GL_AMB_ROWS[tier < GL_AMB_TIERS ? tier : 0][1];
}

uint32_t gl_amb_vol(uint32_t tier)
{
    return GL_AMB_ROWS[tier < GL_AMB_TIERS ? tier : 0][2];
}

// One-shot cue rows, indexed by cue id: { freq Hz, len frames, duty
// code or GL_CUE_ON_NOISE, vol }. One row per sound — the owner-
// tunable table. Row 0 (GL_CUE_NONE) is all zeros: a no-op cue.
static const uint16_t GL_CUE_ROWS[GL_CUE_COUNT][4] = {
    { 0, 0, 0, 0 },                          // NONE
    { 196, 8, 6, 88 },                       // SHOVE: G3 thump, wide duty
    { 262, 10, 3, 80 },                      // PLANK: C4 knock of wood
    { 523, 12, 4, 78 },                      // CACHE: C5 pocketed bright
    { 784, 14, 4, 84 },                      // FLASK: G5 brass slosh
    { 900, 20, GL_CUE_ON_NOISE, 100 },       // BREACH: splintering noise
    { 98, 40, 2, 96 },                       // NIGHTFALL: G2 toll
    { 392, 50, 4, 90 },                      // DAWN: G4 bell
    { 220, 60, GL_CUE_ON_NOISE, 112 },       // DEATH: the cold rattle
};

uint32_t gl_cue_freq(uint32_t cue)
{
    return GL_CUE_ROWS[cue < GL_CUE_COUNT ? cue : 0][0];
}

uint32_t gl_cue_len(uint32_t cue)
{
    return GL_CUE_ROWS[cue < GL_CUE_COUNT ? cue : 0][1];
}

uint32_t gl_cue_duty(uint32_t cue)
{
    return GL_CUE_ROWS[cue < GL_CUE_COUNT ? cue : 0][2];
}

uint32_t gl_cue_vol(uint32_t cue)
{
    return GL_CUE_ROWS[cue < GL_CUE_COUNT ? cue : 0][3];
}

uint32_t gl_save_checksum(const uint32_t words[GL_SAVE_WORDS])
{
    uint32_t h = GL_SAVE_SALT;
    for (int i = 0; i < GL_SAVE_WORDS - 1; i++)
        h = gl_hash(h, words[i]);
    return h;
}

void gl_save_encode(uint32_t best_nights, uint32_t best_seed,
                    uint8_t out[GL_SAVE_BYTES])
{
    uint32_t w[GL_SAVE_WORDS] = {
        GL_SAVE_MAGIC, GL_SAVE_VERSION, best_nights, best_seed, 0, 0, 0, 0,
    };
    w[GL_SAVE_WORDS - 1] = gl_save_checksum(w);
    for (int i = 0; i < GL_SAVE_WORDS; i++)          // little-endian words
    {
        out[4 * i + 0] = (uint8_t)(w[i] & 0xFFu);
        out[4 * i + 1] = (uint8_t)((w[i] >> 8) & 0xFFu);
        out[4 * i + 2] = (uint8_t)((w[i] >> 16) & 0xFFu);
        out[4 * i + 3] = (uint8_t)((w[i] >> 24) & 0xFFu);
    }
}

int gl_save_decode(const uint8_t in[GL_SAVE_BYTES],
                   uint32_t *best_nights, uint32_t *best_seed)
{
    uint32_t w[GL_SAVE_WORDS];
    for (int i = 0; i < GL_SAVE_WORDS; i++)
        w[i] = (uint32_t)in[4 * i + 0]
             | ((uint32_t)in[4 * i + 1] << 8)
             | ((uint32_t)in[4 * i + 2] << 16)
             | ((uint32_t)in[4 * i + 3] << 24);
    if (w[0] != GL_SAVE_MAGIC)
        return 0;                                    // blank/garbage chip
    if (w[1] != GL_SAVE_VERSION)
        return 0;                                    // layout changed: reset
    if (w[GL_SAVE_WORDS - 1] != gl_save_checksum(w))
        return 0;                                    // corrupt: reset
    *best_nights = w[2];
    *best_seed = w[3];
    return 1;
}

int gl_record_improves(uint32_t best_nights, uint32_t nights)
{
    return nights > best_nights;                     // strictly better only
}
