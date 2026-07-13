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

// --- wind tables (slice 6; all one-constant owner-tunables) --------------------
// Weather per seed-hash bucket: calm a quarter of the waters, breeze
// half, gale a quarter. BW_WIND_SALT is pinned so every committed
// anchor seed buckets to CALM (the pin-carry rule in bw_sim.h).
static const int32_t BW_WIND_LEVEL_OF[4] = {
    BW_WIND_CALM, BW_WIND_BREEZE, BW_WIND_BREEZE, BW_WIND_GALE,
};
// push per weather, units/frame at FULL canvas (calm MUST be 0: the
// bit-identity of every committed story rides on it)
static const int32_t BW_WIND_PUSH_OF[BW_WIND_STATES] = { 0, 12, 24 };
// canvas quarters per trim: battle sail shows 1/4 of the push, half
// 2/4, full 4/4 — the doc's "trim interacts with it"
static const int32_t BW_WIND_CANVAS_OF[3] = { 1, 2, 4 };

int32_t bw_wind_heading(const BwDuel *d)
{
    return (d->wind_base + (int32_t)(d->frame >> BW_WIND_TURN_SHIFT))
           & (BW_CIRCLE - 1);
}

int32_t bw_wind_push(const BwDuel *d)
{
    return BW_WIND_PUSH_OF[d->wind_level];
}

// --- danger-band tables (slice 7; all one-constant owner-tunables) --------------
// Band 0 IS the slice-6 water: every band-0 entry equals the legacy
// constant, so all recorded routes and carried proof pins stay
// bit-valid (check-brine.py asserts the identities loudly). Deeper
// rum-runners run heavier hulls and faster gun crews; deeper crates
// are worth more (the concept doc's ~5g band 0 -> ~25g band 2 shape,
// monster salvage richer still); deeper waters roll one weather level
// worse per band (applied in bw_water_init, min-clamped at gale).
static const int32_t BW_BAND_EHULL_OF[BW_BANDS] = { BW_HULL_MAX, 130, 160 };
static const int32_t BW_BAND_ERELOAD_OF[BW_BANDS] = {
    BW_RELOAD_ENEMY, 120, 100,
};
static const int32_t BW_BAND_LOOT_VALUE_OF[BW_BANDS] = {
    BW_LOOT_VALUE, 12, 25,
};
static const int32_t BW_BAND_MAW_VALUE_OF[BW_BANDS] = {
    BW_MAW_LOOT_VALUE, 30, 50,
};
static const int32_t BW_BAND_REEF_OF[BW_BANDS] = { 0, 3, 5 };

int32_t bw_band_enemy_hull(int32_t band)
{
    return BW_BAND_EHULL_OF[band];
}

int32_t bw_band_enemy_reload(int32_t band)
{
    return BW_BAND_ERELOAD_OF[band];
}

// --- upgrade tier tables (slice 4; player only, tier 0 = the slice-2
// sloop EXACTLY — check-brine.py asserts the tier-0 identities so the
// recorded routes and carried pins stay bit-valid) -----------------------------
static const int32_t BW_UP_HULL_OF[BW_UP_TIERS] = { BW_HULL_MAX, 150, 220 };
static const int32_t BW_UP_RELOAD_OF[BW_UP_TIERS] = {
    BW_RELOAD_PLAYER, 70, 55,
};
static const int32_t BW_UP_BALLS_OF[BW_UP_TIERS] = {
    BW_BALLS_PER_SIDE, BW_BALLS_PER_SIDE, 4,
};
static const int32_t BW_UP_SPEED_OF[BW_UP_TIERS] = { 0, 24, 48 };
static const int32_t BW_UP_TURN_OF[BW_UP_TIERS] = { 0, 1, 2 };
// price of buying INTO tier t (index by the NEW tier; tier 0 is free —
// it's the sloop you start with). Triples per step, doc rule.
static const int32_t BW_UP_COST[BW_UP_TIERS] = { 0, 15, 45 };

int32_t bw_up_hull_max(int32_t tier)
{
    return BW_UP_HULL_OF[tier];
}

int32_t bw_up_reload(int32_t tier)
{
    return BW_UP_RELOAD_OF[tier];
}

int32_t bw_up_price(int32_t tier)
{
    return BW_UP_COST[tier];
}

// sail_tier: the SAIL upgrade tier of this ship (enemy always 0).
// wind_hdg/wind_push: the water's wind this frame (slice 6) — BOTH
// ships sail the same weather; a calm water (push 0) is bit-identical
// to the slice-5 step.
static void bw_ship_step(BwShip *s, const BwInputs *in, int32_t sail_tier,
                         int32_t wind_hdg, int32_t wind_push)
{
    s->trim = clamp32(s->trim + in->trim_delta, BW_TRIM_BATTLE, BW_TRIM_FULL);
    s->heading = (s->heading
                  + in->turn * (BW_TURN_OF[s->trim]
                                + BW_UP_TURN_OF[sail_tier]))
                 & (BW_CIRCLE - 1);

    int32_t target = BW_SPEED_OF[s->trim] + BW_UP_SPEED_OF[sail_tier];
    // Point of sail (slice 6): the wind fills or fights the canvas —
    // cos of (heading - wind) scales the push, the trim's canvas
    // quarters scale how much of the weather the rig catches. Shifts
    // only (the signed-division rule: >> floors identically in gcc/ARM
    // and the Python mirror).
    int32_t wrel = ((s->heading - wind_hdg) & (BW_CIRCLE - 1)) >> 2;
    target += (bw_cos(wrel) * wind_push * BW_WIND_CANVAS_OF[s->trim]) >> 10;
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

// Fire one rake abeam. side: -1 = port, +1 = starboard. Rake size is
// the firer's cannon-tier balls-per-broadside (enemy: always tier 0).
// Beam direction (screen y down, heading clockwise from north):
// starboard = (cos, sin), port = (-cos, -sin); keel axis = (sin, -cos).
static void bw_fire(BwDuel *d, const BwShip *s, int32_t owner, int32_t side)
{
    int32_t brad = s->heading >> 2;
    int32_t bx = side * bw_cos(brad);        // beam unit vector, 1/256
    int32_t by = side * bw_sin(brad);
    int32_t kx = bw_sin(brad);               // keel unit vector, 1/256
    int32_t ky = -bw_cos(brad);
    int32_t balls = owner == 0 ? BW_UP_BALLS_OF[d->up_cannon]
                               : BW_BALLS_PER_SIDE;

    for (int32_t k = 0; k < balls; k++)
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

// forward: the slain Maw breaks up mid-ball-pass (defined with the
// loot code below)
static void bw_loot_spawn_at(BwDuel *d, int32_t wx, int32_t wy,
                             int32_t drops, int32_t value);

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
        // Slice 5: a player ball can wound the Maw while it is UP
        // (surface/lunge; a shadow is under the water — balls pass
        // over). Checked before the ship pass: the Maw is the live
        // threat, the wreck is dead planking. In a duel the Maw is
        // BW_MAW_DOWN, so this branch never runs — duel dynamics are
        // bit-identical to slice 4 (routes and pins carry).
        if (ball->owner == 0
            && (d->maw.state == BW_MAW_SURFACE
                || d->maw.state == BW_MAW_LUNGE)
            && bw_chebyshev(ball->x, ball->y, d->maw.x, d->maw.y)
                   < BW_MAW_HIT_RANGE)
        {
            d->maw.hull -= dmg;
            if (d->maw.hull <= 0)
            {
                // Slain: it breaks up richer than any rum-runner.
                d->maw.hull = 0;
                d->maw.slain = 1;
                d->maw.state = BW_MAW_DOWN;
                bw_loot_spawn_at(d, d->maw.x, d->maw.y,
                                 BW_MAW_LOOT_DROPS,
                                 BW_BAND_MAW_VALUE_OF[d->band]);
            }
            ball->live = 0;
            continue;
        }
        BwShip *target = ball->owner == 0 ? &d->enemy : &d->player;
        if (bw_chebyshev(ball->x, ball->y, target->x, target->y)
                < BW_HIT_RANGE)
        {
            // dmg > 0 always, so floor at 0 — no upper clamp: an upgraded
            // player hull sits above BW_HULL_MAX (bit-identical at tier 0).
            target->hull -= dmg;
            if (target->hull < 0)
                target->hull = 0;
            ball->live = 0;
        }
    }
}

// --- loot / gold (slice 3) ----------------------------------------------------

// A carcass breaks up: `drops` crates worth `value` each on a
// BW_LOOT_RING around (wx, wy) at fixed third-of-circle angles (pure
// f(wreck position) — deliberately not frame-hashed, so an input skew
// moves the crates only as far as it moves the wreck). Crates are
// clamped into the sea. Slice 5 generalized this from the rum-runner's
// wreck so the slain Maw can use the same ring with richer crates.
static void bw_loot_spawn_at(BwDuel *d, int32_t wx, int32_t wy,
                             int32_t drops, int32_t value)
{
    static const int32_t BW_LOOT_ANGLE[3] = { 0, 85, 171 };
    for (int32_t k = 0; k < drops; k++)
    {
        for (int32_t i = 0; i < BW_MAX_LOOT; i++)
        {
            BwLoot *c = &d->loot[i];
            if (c->live)
                continue;
            c->x = clamp32(wx + BW_LOOT_RING * bw_sin(BW_LOOT_ANGLE[k]),
                           BW_SEA_X_MIN, BW_SEA_X_MAX);
            c->y = clamp32(wy - BW_LOOT_RING * bw_cos(BW_LOOT_ANGLE[k]),
                           BW_SEA_Y_MIN, BW_SEA_Y_MAX);
            c->value = value;
            c->live = 1;
            break;
        }
    }
}

static void bw_loot_spawn(BwDuel *d)
{
    bw_loot_spawn_at(d, d->enemy.x, d->enemy.y,
                     BW_LOOT_DROPS, BW_BAND_LOOT_VALUE_OF[d->band]);
}

// Scoop pass: sail within BW_SCOOP_RANGE of a crate and it comes aboard,
// until the hold is full (crates the full hold cannot take stay afloat).
static void bw_loot_step(BwDuel *d)
{
    for (int32_t i = 0; i < BW_MAX_LOOT; i++)
    {
        BwLoot *c = &d->loot[i];
        if (!c->live)
            continue;
        if (d->hold >= BW_HOLD_CAP)
            return;
        if (bw_chebyshev(c->x, c->y, d->player.x, d->player.y)
                < BW_SCOOP_RANGE)
        {
            c->live = 0;
            d->hold++;
            d->hold_gold += c->value;    // slice 5: crates carry their worth
        }
    }
}

int32_t bw_dock_step(BwDuel *d)
{
    if (d->hold_gold <= 0)
        return 0;
    if (bw_chebyshev(d->player.x, d->player.y, BW_PIER_X, BW_PIER_Y)
            >= BW_DOCK_RANGE)
        return 0;
    int32_t banked = d->hold_gold;
    d->hold = 0;
    d->hold_gold = 0;
    return banked;
}

// --- reefs (slice 7) --------------------------------------------------------------

// Lay this water's rocks: BW_BAND_REEF_OF[band] reefs at pure
// f(seed) positions. Each rock hashes up to BW_REEF_TRIES candidate
// positions (x off one hash, y off a second hash chained from the
// first — the whole sea reachable) and takes the first one clear of
// the anchorage (BW_REEF_CLEAR) and of the pier harbor
// (BW_MAW_HARBOR — the sanctuary ring is charted water, no rocks);
// if every try fouls a rail, the rock lands on the fixed far-north
// fallback shelf, which is provably clear of both by construction.
// Rock-rock spread is best-effort via the hash stream, never a rail.
static void bw_reef_spawn(BwDuel *d, uint32_t seed)
{
    int32_t n = BW_BAND_REEF_OF[d->band];
    for (int32_t k = 0; k < n; k++)
    {
        BwReef *r = &d->reefs[k];
        r->x = (BW_REEF_SHELF_X + BW_REEF_SHELF_STEP * k) * BW_ONE;
        r->y = BW_REEF_SHELF_Y * BW_ONE;
        for (int32_t t = 0; t < BW_REEF_TRIES; t++)
        {
            uint32_t hx = bw_hash(seed, BW_REEF_SALT
                                        + (uint32_t)(k * BW_REEF_TRIES + t));
            uint32_t hy = bw_hash(hx, BW_REEF_Y_SALT);
            int32_t x = BW_SEA_X_MIN
                        + (int32_t)(hx % (uint32_t)(BW_SEA_X_MAX
                                                    - BW_SEA_X_MIN + 1));
            int32_t y = BW_SEA_Y_MIN
                        + (int32_t)(hy % (uint32_t)(BW_SEA_Y_MAX
                                                    - BW_SEA_Y_MIN + 1));
            if (bw_chebyshev(x, y, BW_PLAYER_START_X, BW_PLAYER_START_Y)
                    < BW_REEF_CLEAR)
                continue;
            if (bw_chebyshev(x, y, BW_PIER_X, BW_PIER_Y) < BW_MAW_HARBOR)
                continue;
            r->x = x;
            r->y = y;
            break;
        }
        r->live = 1;
        r->scraped = 0;
    }
}

// One reef pass: a rock scrapes the PLAYER's keel (the rum-runners and
// the Maw know these waters) for BW_REEF_DMG hull, ONCE per contact —
// the scraped latch clears only when clear of the rock. Groundings can
// sink you (the callers' hull checks flip d->over). Band 0 has no
// reefs, so every slice-2..6 story is untouched.
static void bw_reefs_step(BwDuel *d)
{
    for (int32_t i = 0; i < BW_MAX_REEFS; i++)
    {
        BwReef *r = &d->reefs[i];
        if (!r->live)
            continue;
        if (bw_chebyshev(d->player.x, d->player.y, r->x, r->y)
                < BW_REEF_RANGE)
        {
            if (!r->scraped)
            {
                d->player.hull -= BW_REEF_DMG;
                if (d->player.hull < 0)
                    d->player.hull = 0;
                r->scraped = 1;
                d->groundings++;
            }
        }
        else
            r->scraped = 0;
    }
}

// --- the Maw (slice 5) ----------------------------------------------------------

static void bw_maw_sound(BwMaw *m, uint32_t frame)
{
    // Sound (dive): back beneath the water, stalk again later.
    m->state = BW_MAW_DOWN;
    m->timer = 0;
    m->wake = frame + BW_MAW_RESTIR;
}

// One Maw frame, salvage water only (bw_duel_step never calls this —
// the duel is ships' business; the Maw comes for the lingering victor).
static void bw_maw_step(BwDuel *d)
{
    BwMaw *m = &d->maw;
    if (m->slain)
        return;

    switch (m->state)
    {
    case BW_MAW_DOWN:
        if (d->frame >= m->wake)
        {
            // The stir: the shadow rises at the wreck's blood (pure
            // f(wreck position), like the crate ring). The pier is
            // sanctuary — a wreck inside the harbor radius pushes the
            // rise north of it.
            m->state = BW_MAW_SHADOW;
            m->timer = 0;
            m->stirs++;
            m->x = d->enemy.x;
            m->y = d->enemy.y;
            if (bw_chebyshev(m->x, m->y, BW_PIER_X, BW_PIER_Y)
                    < BW_MAW_HARBOR)
                m->y = BW_PIER_Y - BW_MAW_HARBOR;
        }
        break;

    case BW_MAW_SHADOW:
    {
        // The telegraph: home under the player, one clamped step per
        // axis (chebyshev homing, the lane's metric) — but never into
        // the harbor sanctuary, and never out of the sea.
        m->timer++;
        // The shadow swims WHEREVER it likes (it is under the water —
        // the harbor keeps the JAWS out, not the dark): one clamped
        // step per axis toward the player (chebyshev homing, the
        // lane's metric), held inside the sea.
        m->x = clamp32(m->x + clamp32(d->player.x - m->x,
                                      -BW_MAW_SHADOW_SPEED,
                                      BW_MAW_SHADOW_SPEED),
                       BW_SEA_X_MIN, BW_SEA_X_MAX);
        m->y = clamp32(m->y + clamp32(d->player.y - m->y,
                                      -BW_MAW_SHADOW_SPEED,
                                      BW_MAW_SHADOW_SPEED),
                       BW_SEA_Y_MIN, BW_SEA_Y_MAX);
        if (m->timer >= BW_MAW_SHADOW_FRAMES)
        {
            // The pier is SANCTUARY: it never surfaces inside the
            // harbor ring and never rises on a berthed player — it
            // gives up, sounds, and stalks again later.
            if (bw_chebyshev(d->player.x, d->player.y,
                             BW_PIER_X, BW_PIER_Y) < BW_MAW_HARBOR
                || bw_chebyshev(m->x, m->y,
                                BW_PIER_X, BW_PIER_Y) < BW_MAW_HARBOR)
                bw_maw_sound(m, d->frame);
            else
            {
                m->state = BW_MAW_SURFACE;
                m->timer = 0;
            }
        }
        break;
    }

    case BW_MAW_SURFACE:
        // Up and vulnerable: the windup IS the counterattack window.
        m->timer++;
        if (m->timer >= BW_MAW_SURFACE_FRAMES)
        {
            if (bw_chebyshev(d->player.x, d->player.y,
                             BW_PIER_X, BW_PIER_Y) < BW_MAW_HARBOR)
            {
                // Never begin a lunge at a berthed player (sanctuary).
                bw_maw_sound(m, d->frame);
                break;
            }
            // Latch the lunge at the prey's bearing, chebyshev-
            // normalized so the fast axis rides at full lunge speed.
            // Positive/positive division only (signed-division rule):
            // magnitudes are scaled, signs re-applied.
            int32_t dx = d->player.x - m->x;
            int32_t dy = d->player.y - m->y;
            int32_t adx = dx < 0 ? -dx : dx;
            int32_t ady = dy < 0 ? -dy : dy;
            int32_t big = adx > ady ? adx : ady;
            if (big == 0)
            {
                m->vx = 0;
                m->vy = BW_MAW_LUNGE_SPEED;  // dead under the keel: due south
            }
            else
            {
                m->vx = adx * BW_MAW_LUNGE_SPEED / big;
                m->vy = ady * BW_MAW_LUNGE_SPEED / big;
                if (dx < 0)
                    m->vx = -m->vx;
                if (dy < 0)
                    m->vy = -m->vy;
            }
            m->state = BW_MAW_LUNGE;
            m->timer = 0;
            m->bit = 0;
        }
        break;

    case BW_MAW_LUNGE:
        m->timer++;
        m->x = clamp32(m->x + m->vx, BW_SEA_X_MIN, BW_SEA_X_MAX);
        m->y = clamp32(m->y + m->vy, BW_SEA_Y_MIN, BW_SEA_Y_MAX);
        if (!m->bit
            && bw_chebyshev(m->x, m->y, d->player.x, d->player.y)
                   < BW_MAW_BITE_RANGE)
        {
            d->player.hull -= BW_MAW_BITE;
            if (d->player.hull < 0)
                d->player.hull = 0;
            m->bit = 1;
            bw_maw_sound(m, d->frame);   // it got its bite: down it goes
        }
        else if (m->timer >= BW_MAW_LUNGE_FRAMES)
            bw_maw_sound(m, d->frame);   // missed: sound and stalk again
        break;
    }
}

// --- port + upgrades (slice 4) --------------------------------------------------

int32_t bw_repair_cost(const BwDuel *d)
{
    int32_t missing = BW_UP_HULL_OF[d->up_hull] - d->player.hull;
    if (missing <= 0)
        return 0;
    // positive / positive only (signed-division rule kept trivially)
    return (missing + BW_REPAIR_PER_GOLD - 1) / BW_REPAIR_PER_GOLD;
}

int32_t bw_port_buy(BwDuel *d, int32_t row, int32_t gold)
{
    if (row == BW_PORT_ROW_REPAIR)
    {
        int32_t cost = bw_repair_cost(d);
        if (cost <= 0 || gold < cost)
            return 0;
        d->player.hull = BW_UP_HULL_OF[d->up_hull];
        return cost;
    }

    int32_t *tier = row == BW_PORT_ROW_HULL ? &d->up_hull
                    : row == BW_PORT_ROW_CANNON ? &d->up_cannon
                    : row == BW_PORT_ROW_SAIL ? &d->up_sail
                    : 0;
    if (tier == 0 || *tier >= BW_UP_TIERS - 1)
        return 0;
    int32_t cost = BW_UP_COST[*tier + 1];
    if (gold < cost)
        return 0;
    (*tier)++;                           // new planking raises the ceiling;
    return cost;                         // REPAIR fills it (buying != healing)
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
    for (int32_t i = 0; i < BW_MAX_LOOT; i++)
        d->loot[i].live = 0;
    d->maw.x = 0;                        // slice 5: fresh water, the Maw is
    d->maw.y = 0;                        //   down and biding — the patience
    d->maw.vx = 0;                       //   clock arms on the SINK frame
    d->maw.vy = 0;                       //   (the wreck's blood, not the
    d->maw.state = BW_MAW_DOWN;          //   duel, draws it)
    d->maw.hull = BW_MAW_HULL;
    d->maw.timer = 0;
    d->maw.wake = 0;
    d->maw.stirs = 0;
    d->maw.slain = 0;
    d->maw.bit = 0;
    for (int32_t i = 0; i < BW_MAX_REEFS; i++)
    {
        d->reefs[i].x = 0;               // slice 7: the band-0 water is
        d->reefs[i].y = 0;               //   open sea — no rocks; deeper
        d->reefs[i].live = 0;            //   waters lay theirs in
        d->reefs[i].scraped = 0;         //   bw_water_init
    }
    d->hold = 0;                         // caller re-injects a carried hold
    d->hold_gold = 0;                    // (sinking forfeits: no re-inject)
    d->up_hull = 0;                      // caller re-injects the bought
    d->up_cannon = 0;                    //   tiers (upgrades are NEVER
    d->up_sail = 0;                      //   lost — main.c's Score owns them)
    // Slice 6: this water's weather, pure f(seed) like the spawn ring —
    // printed on the HUD (the seed rule: any run reproducible).
    d->wind_level = BW_WIND_LEVEL_OF[bw_hash(seed, BW_WIND_SALT) & 3u];
    d->wind_base = (int32_t)(bw_hash(seed, BW_WIND_DIR_SALT)
                             & (BW_CIRCLE - 1));
    d->band = 0;                         // slice 7: this IS the band-0
    d->groundings = 0;                   //   water (bw_water_init deepens)
    d->frame = 0;
    d->over = BW_DUEL_RUNNING;
}

void bw_water_init(BwDuel *d, uint32_t seed, int32_t band)
{
    if (band < 0)
        band = 0;
    if (band > BW_BANDS - 1)
        band = BW_BANDS - 1;
    bw_duel_init(d, seed);
    d->band = band;
    // Deeper waters roll one weather level worse per band ("the worse
    // the weather"), min-clamped at gale — band 0 is untouched, and no
    // band exceeds the gale the slice-6 escape rails are proven at.
    d->wind_level = d->wind_level + band > BW_WIND_GALE
                        ? BW_WIND_GALE
                        : d->wind_level + band;
    // The band's rum-runner: heavier hull, faster crews (band 0 rows
    // are the legacy constants — state-identical by construction).
    d->enemy.hull = BW_BAND_EHULL_OF[band];
    d->enemy.reload_l = BW_BAND_ERELOAD_OF[band] / 2;   // same grace rule
    d->enemy.reload_r = BW_BAND_ERELOAD_OF[band] / 2;
    bw_reef_spawn(d, seed);
}

void bw_duel_step(BwDuel *d, const BwInputs *in)
{
    if (d->over != BW_DUEL_RUNNING)
        return;

    BwInputs ai;
    bw_ai(&d->enemy, &d->player, &ai);

    int32_t wind_hdg = bw_wind_heading(d);   // slice 6: one wind, both
    int32_t wind_push = bw_wind_push(d);     //   ships sail it
    bw_ship_step(&d->player, in, d->up_sail, wind_hdg, wind_push);
    bw_ship_step(&d->enemy, &ai, 0, wind_hdg, wind_push);
    bw_reefs_step(d);                    // slice 7: the rocks take their
                                         //   toll where the keel just went

    if (in->fire_l && d->player.reload_l == 0)
    {
        bw_fire(d, &d->player, 0, -1);
        d->player.reload_l = BW_UP_RELOAD_OF[d->up_cannon];
    }
    if (in->fire_r && d->player.reload_r == 0)
    {
        bw_fire(d, &d->player, 0, 1);
        d->player.reload_r = BW_UP_RELOAD_OF[d->up_cannon];
    }
    if (ai.fire_l && d->enemy.reload_l == 0)
    {
        bw_fire(d, &d->enemy, 1, -1);
        d->enemy.reload_l = BW_BAND_ERELOAD_OF[d->band];
    }
    if (ai.fire_r && d->enemy.reload_r == 0)
    {
        bw_fire(d, &d->enemy, 1, 1);
        d->enemy.reload_r = BW_BAND_ERELOAD_OF[d->band];
    }

    bw_balls_step(d);
    bw_loot_step(d);                     // crates exist only after a sink

    if (d->player.hull <= 0)
        d->over = BW_DUEL_PLAYER_SUNK;   // simultaneous sinking = loss
    else if (d->enemy.hull <= 0)
    {
        d->over = BW_DUEL_ENEMY_SUNK;
        bw_loot_spawn(d);                // the wreck breaks up into flotsam
        d->maw.wake = d->frame + BW_MAW_PATIENCE;  // the blood is in the
    }                                    // water: the Maw's clock starts

    d->frame++;
}

void bw_salvage_step(BwDuel *d, const BwInputs *in)
{
    if (d->over != BW_DUEL_ENEMY_SUNK)
        return;

    bw_ship_step(&d->player, in, d->up_sail,
                 bw_wind_heading(d), bw_wind_push(d));
    bw_reefs_step(d);                    // slice 7: rocks scrape salvage
    bw_maw_step(d);                      // slice 5: the water is not empty

    // The batteries WAKE while the Maw is up (any awake state — firing
    // at a mere shadow wastes the rake over its back, which is the
    // player's lesson to learn); they stay cold on quiet water, so
    // every slice-3/4 story (Maw dormant throughout) is untouched.
    if (d->maw.state != BW_MAW_DOWN)
    {
        if (in->fire_l && d->player.reload_l == 0)
        {
            bw_fire(d, &d->player, 0, -1);
            d->player.reload_l = BW_UP_RELOAD_OF[d->up_cannon];
        }
        if (in->fire_r && d->player.reload_r == 0)
        {
            bw_fire(d, &d->player, 0, 1);
            d->player.reload_r = BW_UP_RELOAD_OF[d->up_cannon];
        }
    }

    bw_balls_step(d);                    // a late rake can still strike
    bw_loot_step(d);

    if (d->player.hull <= 0)
        d->over = BW_DUEL_PLAYER_SUNK;   // sunk with the salvage aboard

    d->frame++;
}

// --- synthesized audio (slice 8) -----------------------------------------------

uint32_t bw_amb_tier(int32_t maw_up, int32_t wind_level)
{
    if (maw_up)
        return BW_AMB_MAW;               // the deep overrides the weather
    if (wind_level <= BW_WIND_CALM)
        return BW_AMB_CALM;
    if (wind_level >= BW_WIND_GALE)
        return BW_AMB_GALE;
    return BW_AMB_BREEZE;
}

// Ambience drone rows, indexed by tier: { freq Hz, duty code, vol }.
// One row per tier — the owner-tunable table. The drone is a low
// square wave: the calm swell is a barely-there hum, the canvas fills
// as the weather rises (pitch/duty/volume climb), and the Maw's throb
// sits above the lot — dark water has a pulse.
static const uint16_t BW_AMB_ROWS[BW_AMB_TIERS][3] = {
    { 49, 0, 8 },                        // CALM: the swell against the hull
    { 62, 1, 16 },                       // BREEZE: the canvas fills
    { 78, 3, 30 },                       // GALE: the rigging howls
    { 104, 4, 42 },                      // MAW: the deep has a pulse
};

uint32_t bw_amb_freq(uint32_t tier)
{
    return BW_AMB_ROWS[tier < BW_AMB_TIERS ? tier : 0][0];
}

uint32_t bw_amb_duty(uint32_t tier)
{
    return BW_AMB_ROWS[tier < BW_AMB_TIERS ? tier : 0][1];
}

uint32_t bw_amb_vol(uint32_t tier)
{
    return BW_AMB_ROWS[tier < BW_AMB_TIERS ? tier : 0][2];
}

// One-shot cue rows, indexed by cue id: { freq Hz, len frames, duty
// code or BW_CUE_ON_NOISE, vol, freq2 }. One row per sound — the
// owner-tunable table. Row 0 (BW_CUE_NONE) is all zeros: a no-op cue.
// The dock chime is the one two-note row (C5 -> G5, freq2 nonzero).
static const uint16_t BW_CUE_ROWS[BW_CUE_COUNT][5] = {
    { 0, 0, 0, 0, 0 },                       // NONE
    { 700, 7, BW_CUE_ON_NOISE, 96, 0 },      // CANNON: the thump
    { 1800, 10, BW_CUE_ON_NOISE, 40, 0 },    // SPLASH: a spent rake hisses
    { 587, 9, 4, 70, 0 },                    // SCOOP: D5, a crate aboard
    { 240, 12, BW_CUE_ON_NOISE, 90, 0 },     // CRACK: timber gives
    { 120, 26, BW_CUE_ON_NOISE, 84, 0 },     // SCRAPE: the keel grates
    { 90, 36, BW_CUE_ON_NOISE, 100, 0 },     // WRECK: she goes under
    { 523, 28, 4, 88, 784 },                 // DOCK: C5 -> G5, the chime
    { 73, 30, 6, 104, 0 },                   // MAWRISE: D2, the deep speaks
    { 160, 22, BW_CUE_ON_NOISE, 112, 0 },    // BITE: the jaws close
    { 200, 60, BW_CUE_ON_NOISE, 116, 0 },    // SUNK: the long cold rattle
};

uint32_t bw_cue_freq(uint32_t cue)
{
    return BW_CUE_ROWS[cue < BW_CUE_COUNT ? cue : 0][0];
}

uint32_t bw_cue_len(uint32_t cue)
{
    return BW_CUE_ROWS[cue < BW_CUE_COUNT ? cue : 0][1];
}

uint32_t bw_cue_duty(uint32_t cue)
{
    return BW_CUE_ROWS[cue < BW_CUE_COUNT ? cue : 0][2];
}

uint32_t bw_cue_vol(uint32_t cue)
{
    return BW_CUE_ROWS[cue < BW_CUE_COUNT ? cue : 0][3];
}

uint32_t bw_cue_freq2(uint32_t cue)
{
    return BW_CUE_ROWS[cue < BW_CUE_COUNT ? cue : 0][4];
}
