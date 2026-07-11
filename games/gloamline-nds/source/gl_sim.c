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

void gl_shambler_step(int32_t *zx, int32_t *zy, int32_t px, int32_t py,
                      uint32_t zombie_id, uint32_t frame)
{
    // Per-zombie deterministic stagger: skip 1 frame in 4, so a future
    // crowd smears into a readable arc instead of a stack.
    if ((gl_hash(zombie_id, frame) & 3u) == 0u)
        return;

    int32_t dx = px - *zx;
    int32_t dy = py - *zy;
    int32_t sx = (dx > GL_AXIS_DEADZONE) - (dx < -GL_AXIS_DEADZONE);
    int32_t sy = (dy > GL_AXIS_DEADZONE) - (dy < -GL_AXIS_DEADZONE);
    int32_t speed = (sx && sy) ? GL_SHAMBLER_DIAG : GL_SHAMBLER_SPEED;

    *zx = clamp32(*zx + sx * speed, GL_ARENA_X_MIN, GL_ARENA_X_MAX);
    *zy = clamp32(*zy + sy * speed, GL_ARENA_Y_MIN, GL_ARENA_Y_MAX);
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
