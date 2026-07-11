// Brineward — pure simulation layer, implementation. See bw_sim.h.
//
// MIRROR RULE (keep in lockstep): tools/check-brine.py mirrors every
// function here line-for-line. Change both in the same PR.
//
// SIGNED-DIVISION RULE: possibly-negative values are only ever divided by
// arithmetic right shift (>> 8) so the Python mirror (whose // and >>
// both floor) agrees bit-for-bit with gcc/ARM.

#include "bw_sim.h"

uint32_t bw_hash(uint32_t a, uint32_t b)
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

// Quarter-wave sine table: round(sin(i * pi/128) * 256) for i = 0..64.
// The SAME 65 literals live in tools/check-brine.py — regenerate both
// together or not at all.
static const int32_t BW_QSIN[65] = {
    0, 6, 13, 19, 25, 31, 38, 44, 50, 56, 62, 68, 74,
    80, 86, 92, 98, 104, 109, 115, 121, 126, 132, 137, 142, 147,
    152, 157, 162, 167, 172, 177, 181, 185, 190, 194, 198, 202, 206,
    209, 213, 216, 220, 223, 226, 229, 231, 234, 237, 239, 241, 243,
    245, 247, 248, 250, 251, 252, 253, 254, 255, 255, 256, 256, 256,
};

int32_t bw_sin(int32_t brad)
{
    int32_t a = brad & 255;
    if (a < 64) return BW_QSIN[a];
    if (a < 128) return BW_QSIN[128 - a];
    if (a < 192) return -BW_QSIN[a - 128];
    return -BW_QSIN[256 - a];
}

int32_t bw_cos(int32_t brad)
{
    return bw_sin(brad + 64);
}

int32_t bw_chebyshev(int32_t ax, int32_t ay, int32_t bx, int32_t by)
{
    int32_t dx = ax > bx ? ax - bx : bx - ax;
    int32_t dy = ay > by ? ay - by : by - ay;
    return dx > dy ? dx : dy;
}

// --- ship kinematics (momentum sailing) --------------------------------------

static const int32_t BW_SPEED_OF[3] = {
    BW_SPEED_BATTLE, BW_SPEED_HALF, BW_SPEED_FULL,
};
static const int32_t BW_TURN_OF[3] = {
    BW_TURN_BATTLE, BW_TURN_HALF, BW_TURN_FULL,
};

static void bw_ship_step(BwShip *s, const BwInputs *in)
{
    s->trim = clamp32(s->trim + in->trim_delta, BW_TRIM_BATTLE, BW_TRIM_FULL);
    s->heading = (s->heading + in->turn * BW_TURN_OF[s->trim])
                 & (BW_CIRCLE - 1);

    int32_t target = BW_SPEED_OF[s->trim];
    s->speed += clamp32(target - s->speed, -BW_ACCEL, BW_ACCEL);

    int32_t brad = s->heading >> 2;
    s->x = clamp32(s->x + ((bw_sin(brad) * s->speed) >> 8),
                   BW_SEA_X_MIN, BW_SEA_X_MAX);
    s->y = clamp32(s->y + ((-bw_cos(brad) * s->speed) >> 8),
                   BW_SEA_Y_MIN, BW_SEA_Y_MAX);

    if (s->reload_l > 0) s->reload_l--;
    if (s->reload_r > 0) s->reload_r--;
}

// --- broadsides ----------------------------------------------------------------

// Fire one 3-ball rake abeam. side: -1 = port, +1 = starboard.
// Beam direction (screen y down, heading clockwise from north):
// starboard = (cos, sin), port = (-cos, -sin); keel axis = (sin, -cos).
static void bw_fire(BwDuel *d, const BwShip *s, int32_t owner, int32_t side)
{
    int32_t brad = s->heading >> 2;
    int32_t bx = side * bw_cos(brad);        // beam unit vector, 1/256
    int32_t by = side * bw_sin(brad);
    int32_t kx = bw_sin(brad);               // keel unit vector, 1/256
    int32_t ky = -bw_cos(brad);

    for (int32_t k = 0; k < BW_BALLS_PER_SIDE; k++)
    {
        int32_t along = (k - 1) * BW_BALL_SPREAD;
        for (int32_t i = 0; i < BW_MAX_BALLS; i++)
        {
            BwBall *ball = &d->balls[i];
            if (ball->live)
                continue;
            ball->x = s->x + kx * along + bx * BW_BALL_MUZZLE;
            ball->y = s->y + ky * along + by * BW_BALL_MUZZLE;
            ball->vx = (bx * BW_BALL_SPEED) >> 8;
            ball->vy = (by * BW_BALL_SPEED) >> 8;
            ball->age = 0;
            ball->owner = owner;
            ball->live = 1;
            break;
        }
    }
}

static void bw_balls_step(BwDuel *d)
{
    for (int32_t i = 0; i < BW_MAX_BALLS; i++)
    {
        BwBall *ball = &d->balls[i];
        if (!ball->live)
            continue;
        ball->x += ball->vx;
        ball->y += ball->vy;
        ball->age++;
        if (ball->age >= BW_BALL_LIFE
            || ball->x < BW_SEA_X_MIN || ball->x > BW_SEA_X_MAX
            || ball->y < BW_SEA_Y_MIN || ball->y > BW_SEA_Y_MAX)
        {
            ball->live = 0;
            continue;
        }
        // range falloff: damage eases from NEAR by FALL over the flight
        int32_t dmg = BW_DMG_NEAR - (ball->age * BW_DMG_FALL) / BW_BALL_LIFE;
        BwShip *target = ball->owner == 0 ? &d->enemy : &d->player;
        if (bw_chebyshev(ball->x, ball->y, target->x, target->y)
                < BW_HIT_RANGE)
        {
            target->hull = clamp32(target->hull - dmg, 0, BW_HULL_MAX);
            ball->live = 0;
        }
    }
}

// --- enemy sail AI ----------------------------------------------------------------

void bw_ai(const BwShip *e, const BwShip *p, BwInputs *out)
{
    out->turn = 0;
    out->trim_delta = 0;
    out->fire_l = 0;
    out->fire_r = 0;

    int32_t dx = p->x - e->x;
    int32_t dy = p->y - e->y;
    int32_t brad = e->heading >> 2;
    int32_t hx = bw_sin(brad);               // heading unit vector, 1/256
    int32_t hy = -bw_cos(brad);
    int32_t dot = hx * dx + hy * dy;         // >0: player ahead of the beam
    int32_t cross = hx * dy - hy * dx;       // >0: player to starboard
    int32_t adot = dot < 0 ? -dot : dot;
    int32_t across = cross < 0 ? -cross : cross;
    int32_t dist = bw_chebyshev(e->x, e->y, p->x, p->y);

    if (dist > BW_ENGAGE_RANGE)
    {
        // Intercept: full sail, bow on the player.
        out->trim_delta = e->trim < BW_TRIM_FULL ? 1 : 0;
        if (!(dot > 0 && across <= (adot >> 3)))     // outside ~7deg of bow
            out->turn = cross > 0 ? 1 : (cross < 0 ? -1 : 1);
        return;
    }

    // Circle: battle sail, hold the player on a beam, rake when aligned.
    out->trim_delta = e->trim > BW_TRIM_BATTLE ? -1 : 0;
    if (BW_AI_TURN_ARC * adot >= across)
    {
        // Off the beam line: rotate the near beam onto the bearing.
        int32_t ds = dot > 0 ? 1 : (dot < 0 ? -1 : 1);
        int32_t cs = cross > 0 ? 1 : (cross < 0 ? -1 : 1);
        out->turn = -ds * cs;
    }
    if (dist < BW_FIRE_RANGE && BW_AI_FIRE_ARC * adot < across)
    {
        if (cross > 0)
            out->fire_r = 1;
        else
            out->fire_l = 1;
    }
}

// --- duel ------------------------------------------------------------------------

void bw_duel_init(BwDuel *d, uint32_t seed)
{
    d->player.x = BW_PLAYER_START_X;
    d->player.y = BW_PLAYER_START_Y;
    d->player.heading = BW_PLAYER_START_HDG;
    d->player.trim = BW_TRIM_HALF;
    d->player.speed = BW_SPEED_HALF;
    d->player.hull = BW_HULL_MAX;
    d->player.reload_l = 0;
    d->player.reload_r = 0;

    // Enemy on a BW_SPAWN_DIST ring around the anchorage, angle f(seed),
    // clamped into the sea, bow toward the player (ring angle + half turn).
    int32_t a = (int32_t)(bw_hash(seed, 0xB71E) & 255u);
    d->enemy.x = clamp32(BW_PLAYER_START_X + BW_SPAWN_DIST * bw_sin(a),
                         BW_SEA_X_MIN, BW_SEA_X_MAX);
    d->enemy.y = clamp32(BW_PLAYER_START_Y - BW_SPAWN_DIST * bw_cos(a),
                         BW_SEA_Y_MIN, BW_SEA_Y_MAX);
    d->enemy.heading = (a * 4 + BW_CIRCLE / 2) & (BW_CIRCLE - 1);
    d->enemy.trim = BW_TRIM_FULL;
    d->enemy.speed = BW_SPEED_FULL;
    d->enemy.hull = BW_HULL_MAX;
    d->enemy.reload_l = BW_RELOAD_ENEMY / 2;    // grace: no frame-0 rake
    d->enemy.reload_r = BW_RELOAD_ENEMY / 2;

    for (int32_t i = 0; i < BW_MAX_BALLS; i++)
        d->balls[i].live = 0;
    d->frame = 0;
    d->over = BW_DUEL_RUNNING;
}

void bw_duel_step(BwDuel *d, const BwInputs *in)
{
    if (d->over != BW_DUEL_RUNNING)
        return;

    BwInputs ai;
    bw_ai(&d->enemy, &d->player, &ai);

    bw_ship_step(&d->player, in);
    bw_ship_step(&d->enemy, &ai);

    if (in->fire_l && d->player.reload_l == 0)
    {
        bw_fire(d, &d->player, 0, -1);
        d->player.reload_l = BW_RELOAD_PLAYER;
    }
    if (in->fire_r && d->player.reload_r == 0)
    {
        bw_fire(d, &d->player, 0, 1);
        d->player.reload_r = BW_RELOAD_PLAYER;
    }
    if (ai.fire_l && d->enemy.reload_l == 0)
    {
        bw_fire(d, &d->enemy, 1, -1);
        d->enemy.reload_l = BW_RELOAD_ENEMY;
    }
    if (ai.fire_r && d->enemy.reload_r == 0)
    {
        bw_fire(d, &d->enemy, 1, 1);
        d->enemy.reload_r = BW_RELOAD_ENEMY;
    }

    bw_balls_step(d);

    if (d->player.hull <= 0)
        d->over = BW_DUEL_PLAYER_SUNK;   // simultaneous sinking = loss
    else if (d->enemy.hull <= 0)
        d->over = BW_DUEL_ENEMY_SUNK;

    d->frame++;
}
