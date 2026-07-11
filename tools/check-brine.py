#!/usr/bin/env python3
"""Brineward host proof (stdlib-only, <5s) — arc slice 2 walking skeleton.

The check-gloam.py sibling for Brineward: a line-for-line Python mirror of
the pure simulation layer in games/brineward-nds/source/bw_sim.c
(bw_hash / bw_sin / bw_cos / bw_chebyshev / ship step / broadside fire /
ball flight + damage / enemy sail AI / bw_duel_init / bw_duel_step),
proving the skeleton's meaningful invariants for whole input families
instead of the handful a replay touches:

  1. sine table — endpoint/symmetry identities of the shared quarter-wave
     table (sin 0 = 0, sin 64 = 256, sin(a+128) = -sin a, cos = shifted
     sin) plus |sin| <= 256 everywhere;
  2. spawn schedule — for seeds 0..4095: bw_duel_init is deterministic
     (recompute-equal), the enemy spawns inside the sea rectangle and
     NEVER closer than BW_SPAWN_MIN_DIST to the anchorage;
  3. duel convergence, loss side — for seeds 0..63: an idle player (no
     input ever) IS sunk by the AI within 3600 frames, before the enemy
     takes any hull damage;
  4. duel convergence, win side — for seeds 0..15: the beam-holding
     policy player (the exact policy games/brineward-nds/tools/
     record-duel-win.py records routes from) SINKS the enemy within 3600
     frames and survives it;
  5. containment — under 20,000 frames of hash-driven adversarial input
     both ships and every live ball stay inside the sea rectangle, and
     trim/heading/speed stay in range.

MIRROR RULE (keep in lockstep): every function and constant below mirrors
games/brineward-nds/source/bw_sim.c. Any change to the C MUST land here
in the same PR — same discipline as check-gloam.py <-> gl_sim.c.

SIGNED-DIVISION RULE: the C only ever divides possibly-negative values by
arithmetic right shift (>> 8); Python's >> floors identically, so no
truncating-division shims are needed — keep it that way.

Usage: python3 tools/check-brine.py           # exit 0 = green
Import: the sim functions are importable (used by the duel-win route
recorder games/brineward-nds/tools/record-duel-win.py).
"""

import sys

# --- constants (mirror bw_sim.h; fixed point 8.8) ---------------------------
BW_ONE = 256

BW_SEA_X_MIN = 16 * BW_ONE
BW_SEA_X_MAX = 239 * BW_ONE
BW_SEA_Y_MIN = 32 * BW_ONE
BW_SEA_Y_MAX = 175 * BW_ONE

BW_PLAYER_START_X = 128 * BW_ONE
BW_PLAYER_START_Y = 140 * BW_ONE
BW_PLAYER_START_HDG = 0

BW_CIRCLE = 1024

BW_TRIM_BATTLE, BW_TRIM_HALF, BW_TRIM_FULL = 0, 1, 2
BW_SPEED_OF = (96, 160, 224)             # battle / half / full
BW_TURN_OF = (6, 4, 2)
BW_ACCEL = 2

BW_MAX_BALLS = 16
BW_BALLS_PER_SIDE = 3
BW_BALL_SPEED = 640
BW_BALL_LIFE = 48
BW_BALL_MUZZLE = 10
BW_BALL_SPREAD = 6
BW_HIT_RANGE = 6 * BW_ONE
BW_DMG_NEAR = 30
BW_DMG_FALL = 20

BW_HULL_MAX = 100
BW_RELOAD_PLAYER = 90
BW_RELOAD_ENEMY = 150

BW_SPAWN_DIST = 90
BW_SPAWN_MIN_DIST = 32 * BW_ONE
BW_ENGAGE_RANGE = 110 * BW_ONE
BW_FIRE_RANGE = 112 * BW_ONE
BW_AI_FIRE_ARC = 12
BW_AI_TURN_ARC = 16

BW_DUEL_RUNNING, BW_DUEL_PLAYER_SUNK, BW_DUEL_ENEMY_SUNK = 0, 1, 2

U32 = 0xFFFFFFFF


def bw_hash(a, b):
    """Mirror of bw_hash()."""
    h = ((a * 0x9E3779B9) & U32) ^ ((b * 0x85EBCA6B) & U32)
    h ^= h >> 16
    h = (h * 0x7FEB352D) & U32
    h ^= h >> 15
    h = (h * 0x846CA68B) & U32
    h ^= h >> 16
    return h


# Quarter-wave sine table: round(sin(i * pi/128) * 256) for i = 0..64.
# The SAME 65 literals live in bw_sim.c — regenerate both together.
BW_QSIN = (
    0, 6, 13, 19, 25, 31, 38, 44, 50, 56, 62, 68, 74,
    80, 86, 92, 98, 104, 109, 115, 121, 126, 132, 137, 142, 147,
    152, 157, 162, 167, 172, 177, 181, 185, 190, 194, 198, 202, 206,
    209, 213, 216, 220, 223, 226, 229, 231, 234, 237, 239, 241, 243,
    245, 247, 248, 250, 251, 252, 253, 254, 255, 255, 256, 256, 256,
)


def bw_sin(brad):
    """Mirror of bw_sin()."""
    a = brad & 255
    if a < 64:
        return BW_QSIN[a]
    if a < 128:
        return BW_QSIN[128 - a]
    if a < 192:
        return -BW_QSIN[a - 128]
    return -BW_QSIN[256 - a]


def bw_cos(brad):
    """Mirror of bw_cos()."""
    return bw_sin(brad + 64)


def bw_chebyshev(ax, ay, bx, by):
    """Mirror of bw_chebyshev()."""
    dx = ax - bx if ax > bx else bx - ax
    dy = ay - by if ay > by else by - ay
    return dx if dx > dy else dy


def _clamp(v, lo, hi):
    return lo if v < lo else hi if v > hi else v


class Ship:
    """Mirror of BwShip."""

    __slots__ = ('x', 'y', 'heading', 'trim', 'speed', 'hull',
                 'reload_l', 'reload_r')


class Ball:
    """Mirror of BwBall."""

    __slots__ = ('x', 'y', 'vx', 'vy', 'age', 'owner', 'live')

    def __init__(self):
        self.live = 0


class Duel:
    """Mirror of BwDuel."""

    __slots__ = ('player', 'enemy', 'balls', 'frame', 'over')

    def __init__(self):
        self.player = Ship()
        self.enemy = Ship()
        self.balls = [Ball() for _ in range(BW_MAX_BALLS)]


class Inputs:
    """Mirror of BwInputs."""

    __slots__ = ('turn', 'trim_delta', 'fire_l', 'fire_r')

    def __init__(self, turn=0, trim_delta=0, fire_l=0, fire_r=0):
        self.turn = turn
        self.trim_delta = trim_delta
        self.fire_l = fire_l
        self.fire_r = fire_r


def bw_ship_step(s, inputs):
    """Mirror of bw_ship_step()."""
    s.trim = _clamp(s.trim + inputs.trim_delta, BW_TRIM_BATTLE, BW_TRIM_FULL)
    s.heading = (s.heading + inputs.turn * BW_TURN_OF[s.trim]) \
        & (BW_CIRCLE - 1)

    target = BW_SPEED_OF[s.trim]
    s.speed += _clamp(target - s.speed, -BW_ACCEL, BW_ACCEL)

    brad = s.heading >> 2
    s.x = _clamp(s.x + ((bw_sin(brad) * s.speed) >> 8),
                 BW_SEA_X_MIN, BW_SEA_X_MAX)
    s.y = _clamp(s.y + ((-bw_cos(brad) * s.speed) >> 8),
                 BW_SEA_Y_MIN, BW_SEA_Y_MAX)

    if s.reload_l > 0:
        s.reload_l -= 1
    if s.reload_r > 0:
        s.reload_r -= 1


def bw_fire(d, s, owner, side):
    """Mirror of bw_fire()."""
    brad = s.heading >> 2
    bx = side * bw_cos(brad)
    by = side * bw_sin(brad)
    kx = bw_sin(brad)
    ky = -bw_cos(brad)

    for k in range(BW_BALLS_PER_SIDE):
        along = (k - 1) * BW_BALL_SPREAD
        for ball in d.balls:
            if ball.live:
                continue
            ball.x = s.x + kx * along + bx * BW_BALL_MUZZLE
            ball.y = s.y + ky * along + by * BW_BALL_MUZZLE
            ball.vx = (bx * BW_BALL_SPEED) >> 8
            ball.vy = (by * BW_BALL_SPEED) >> 8
            ball.age = 0
            ball.owner = owner
            ball.live = 1
            break


def bw_balls_step(d):
    """Mirror of bw_balls_step()."""
    for ball in d.balls:
        if not ball.live:
            continue
        ball.x += ball.vx
        ball.y += ball.vy
        ball.age += 1
        if (ball.age >= BW_BALL_LIFE
                or ball.x < BW_SEA_X_MIN or ball.x > BW_SEA_X_MAX
                or ball.y < BW_SEA_Y_MIN or ball.y > BW_SEA_Y_MAX):
            ball.live = 0
            continue
        dmg = BW_DMG_NEAR - (ball.age * BW_DMG_FALL) // BW_BALL_LIFE
        target = d.enemy if ball.owner == 0 else d.player
        if bw_chebyshev(ball.x, ball.y, target.x, target.y) < BW_HIT_RANGE:
            target.hull = _clamp(target.hull - dmg, 0, BW_HULL_MAX)
            ball.live = 0


def bw_ai(e, p):
    """Mirror of bw_ai() -> Inputs."""
    out = Inputs()

    dx = p.x - e.x
    dy = p.y - e.y
    brad = e.heading >> 2
    hx = bw_sin(brad)
    hy = -bw_cos(brad)
    dot = hx * dx + hy * dy
    cross = hx * dy - hy * dx
    adot = -dot if dot < 0 else dot
    across = -cross if cross < 0 else cross
    dist = bw_chebyshev(e.x, e.y, p.x, p.y)

    if dist > BW_ENGAGE_RANGE:
        out.trim_delta = 1 if e.trim < BW_TRIM_FULL else 0
        if not (dot > 0 and across <= (adot >> 3)):
            out.turn = 1 if cross > 0 else (-1 if cross < 0 else 1)
        return out

    out.trim_delta = -1 if e.trim > BW_TRIM_BATTLE else 0
    if BW_AI_TURN_ARC * adot >= across:
        ds = 1 if dot > 0 else (-1 if dot < 0 else 1)
        cs = 1 if cross > 0 else (-1 if cross < 0 else 1)
        out.turn = -ds * cs
    if dist < BW_FIRE_RANGE and BW_AI_FIRE_ARC * adot < across:
        if cross > 0:
            out.fire_r = 1
        else:
            out.fire_l = 1
    return out


def bw_duel_init(d, seed):
    """Mirror of bw_duel_init()."""
    p = d.player
    p.x, p.y = BW_PLAYER_START_X, BW_PLAYER_START_Y
    p.heading = BW_PLAYER_START_HDG
    p.trim = BW_TRIM_HALF
    p.speed = BW_SPEED_OF[BW_TRIM_HALF]
    p.hull = BW_HULL_MAX
    p.reload_l = p.reload_r = 0

    a = bw_hash(seed, 0xB71E) & 255
    e = d.enemy
    e.x = _clamp(BW_PLAYER_START_X + BW_SPAWN_DIST * bw_sin(a),
                 BW_SEA_X_MIN, BW_SEA_X_MAX)
    e.y = _clamp(BW_PLAYER_START_Y - BW_SPAWN_DIST * bw_cos(a),
                 BW_SEA_Y_MIN, BW_SEA_Y_MAX)
    e.heading = (a * 4 + BW_CIRCLE // 2) & (BW_CIRCLE - 1)
    e.trim = BW_TRIM_FULL
    e.speed = BW_SPEED_OF[BW_TRIM_FULL]
    e.hull = BW_HULL_MAX
    e.reload_l = e.reload_r = BW_RELOAD_ENEMY // 2

    for ball in d.balls:
        ball.live = 0
    d.frame = 0
    d.over = BW_DUEL_RUNNING


def bw_duel_step(d, inputs):
    """Mirror of bw_duel_step()."""
    if d.over != BW_DUEL_RUNNING:
        return

    ai = bw_ai(d.enemy, d.player)

    bw_ship_step(d.player, inputs)
    bw_ship_step(d.enemy, ai)

    if inputs.fire_l and d.player.reload_l == 0:
        bw_fire(d, d.player, 0, -1)
        d.player.reload_l = BW_RELOAD_PLAYER
    if inputs.fire_r and d.player.reload_r == 0:
        bw_fire(d, d.player, 0, 1)
        d.player.reload_r = BW_RELOAD_PLAYER
    if ai.fire_l and d.enemy.reload_l == 0:
        bw_fire(d, d.enemy, 1, -1)
        d.enemy.reload_l = BW_RELOAD_ENEMY
    if ai.fire_r and d.enemy.reload_r == 0:
        bw_fire(d, d.enemy, 1, 1)
        d.enemy.reload_r = BW_RELOAD_ENEMY

    bw_balls_step(d)

    if d.player.hull <= 0:
        d.over = BW_DUEL_PLAYER_SUNK
    elif d.enemy.hull <= 0:
        d.over = BW_DUEL_ENEMY_SUNK

    d.frame += 1


# --- the beam-holding duel policy (the win-route recorder's brain) ------------
# NOT a mirror of any C — this is the host-side player policy that the
# duel-WIN proof records: hold half sail, steer to keep the enemy on a
# beam (the same cross/dot steering the AI uses) with an INWARD bias when
# the range opens (two exact beam-holders orbit apart — measured: the
# unbiased policy drifts to ~125 px and stalls), fire whichever battery
# bears once tightly aligned and inside 100 px. It exists here so proof 4
# (win reachability) and the route recorder run the SAME policy.

POLICY_FIRE_ARC = 12
POLICY_RANGE = 100 * BW_ONE
POLICY_CLOSE = 80 * BW_ONE


def bw_policy(p, e):
    """Beam-holding player policy -> Inputs (fires edge-style every frame
    it bears; bw_duel_step's reload gate turns that into discrete rakes)."""
    out = Inputs()

    dx = e.x - p.x
    dy = e.y - p.y
    brad = p.heading >> 2
    hx = bw_sin(brad)
    hy = -bw_cos(brad)
    dot = hx * dx + hy * dy
    cross = hx * dy - hy * dx
    adot = -dot if dot < 0 else dot
    across = -cross if cross < 0 else cross
    dist = bw_chebyshev(p.x, p.y, e.x, e.y)

    # Steering target: exactly abeam when close (so the batteries bear);
    # ~14 deg forward of the beam when the range has opened, which turns
    # the orbit into an inward spiral (velocity gains a closing component).
    db = dot if dist <= POLICY_CLOSE else dot - (across >> 2)
    adb = -db if db < 0 else db
    if BW_AI_TURN_ARC * adb >= across:
        ds = 1 if db > 0 else (-1 if db < 0 else 1)
        cs = 1 if cross > 0 else (-1 if cross < 0 else 1)
        out.turn = -ds * cs
    if dist < POLICY_RANGE and POLICY_FIRE_ARC * adot < across:
        if cross > 0:
            out.fire_r = 1
        else:
            out.fire_l = 1
    return out


# --- proofs -------------------------------------------------------------------

def check_sine_table():
    assert bw_sin(0) == 0 and bw_sin(64) == 256
    assert bw_cos(0) == 256 and bw_cos(64) == 0
    for a in range(256):
        assert -256 <= bw_sin(a) <= 256
        assert bw_sin(a + 128) == -bw_sin(a), a
        assert bw_sin(64 - a) == bw_sin(64 + a), a
        assert bw_cos(a) == bw_sin(a + 64), a
    for i in range(64):
        assert BW_QSIN[i] <= BW_QSIN[i + 1], i   # quarter-wave monotone
    print('sine table: endpoints, symmetry, monotone quarter OK (256 brads)')


def check_spawns():
    for seed in range(4096):
        d = Duel()
        bw_duel_init(d, seed)
        d2 = Duel()
        bw_duel_init(d2, seed)
        assert (d.enemy.x, d.enemy.y, d.enemy.heading) \
            == (d2.enemy.x, d2.enemy.y, d2.enemy.heading), seed
        assert BW_SEA_X_MIN <= d.enemy.x <= BW_SEA_X_MAX, seed
        assert BW_SEA_Y_MIN <= d.enemy.y <= BW_SEA_Y_MAX, seed
        dist = bw_chebyshev(d.enemy.x, d.enemy.y,
                            BW_PLAYER_START_X, BW_PLAYER_START_Y)
        assert dist >= BW_SPAWN_MIN_DIST, (seed, dist)
    print('spawns: 4096 seeds deterministic, in the sea, '
          f'>= {BW_SPAWN_MIN_DIST // BW_ONE} px off the anchorage')


def check_idle_player_sunk():
    idle = Inputs()
    worst = 0
    for seed in range(64):
        d = Duel()
        bw_duel_init(d, seed)
        for _ in range(3600):
            bw_duel_step(d, idle)
            if d.over != BW_DUEL_RUNNING:
                break
        assert d.over == BW_DUEL_PLAYER_SUNK, (seed, d.over, d.player.hull)
        assert d.enemy.hull == BW_HULL_MAX, (seed, d.enemy.hull)
        worst = max(worst, d.frame)
    print(f'duel loss: idle player sunk in <= {worst} frames '
          '(64 seeds), enemy unscratched')


def check_policy_player_wins():
    worst = 0
    worst_hull = BW_HULL_MAX
    for seed in range(16):
        d = Duel()
        bw_duel_init(d, seed)
        for _ in range(3600):
            bw_duel_step(d, bw_policy(d.player, d.enemy))
            if d.over != BW_DUEL_RUNNING:
                break
        assert d.over == BW_DUEL_ENEMY_SUNK, (seed, d.over, d.enemy.hull)
        assert d.player.hull > 0, seed
        worst = max(worst, d.frame)
        worst_hull = min(worst_hull, d.player.hull)
    print(f'duel win: policy player sinks the enemy in <= {worst} frames '
          f'(16 seeds), worst surviving hull {worst_hull}')


def check_containment():
    d = Duel()
    bw_duel_init(d, 0xC0FFEE & U32)
    frames = 20000
    for f in range(frames):
        h = bw_hash(0x5EA, f)
        inp = Inputs(turn=(h & 3) - 1 if (h & 3) < 3 else 0,
                     trim_delta=((h >> 2) & 3) - 1 if ((h >> 2) & 3) < 3
                     else 0,
                     fire_l=(h >> 4) & 1, fire_r=(h >> 5) & 1)
        d.over = BW_DUEL_RUNNING          # adversarial: never let it end
        d.player.hull = d.enemy.hull = BW_HULL_MAX
        bw_duel_step(d, inp)
        for s in (d.player, d.enemy):
            assert BW_SEA_X_MIN <= s.x <= BW_SEA_X_MAX, f
            assert BW_SEA_Y_MIN <= s.y <= BW_SEA_Y_MAX, f
            assert 0 <= s.heading < BW_CIRCLE, f
            assert BW_TRIM_BATTLE <= s.trim <= BW_TRIM_FULL, f
            assert 0 < s.speed <= BW_SPEED_OF[BW_TRIM_FULL], f
        for ball in d.balls:
            if ball.live:
                assert BW_SEA_X_MIN <= ball.x <= BW_SEA_X_MAX, f
                assert BW_SEA_Y_MIN <= ball.y <= BW_SEA_Y_MAX, f
    print(f'containment: {frames} adversarial frames — ships and balls '
          'never leave the sea, trim/heading/speed in range')


def main():
    check_sine_table()
    check_spawns()
    check_idle_player_sunk()
    check_policy_player_wins()
    check_containment()
    print('check-brine: ALL GREEN')
    return 0


if __name__ == '__main__':
    sys.exit(main())
