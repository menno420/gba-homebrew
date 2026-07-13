#!/usr/bin/env python3
"""Brineward host proof (stdlib-only, <5s) — pure-sim mirror + invariants.

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
     trim/heading/speed stay in range;
  6. loot drop (slice 3) — every policy win breaks the wreck into exactly
     BW_LOOT_DROPS crates, inside the sea, on the wreck ring;
  7. salvage convergence (slice 3) — from every win state the salvage
     policy (the exact brain games/brineward-nds/tools/record-salvage.py
     records routes from) scoops all crates and banks them at the pier;
  8. hold cap / dock economy (slice 3) — the hold never exceeds
     BW_HOLD_CAP from any start fill, overflow crates stay afloat, gold
     is exactly BW_LOOT_VALUE per crate, and banking happens strictly
     inside the pier window, emptying the hold exactly once;
  9. salvage containment (slice 3; slice 5 re-arms it with the Maw
     stalking and the fire keys live) — adversarial salvage frames keep
     the ship, every crate, AND the Maw inside the sea, the hold and
     both hulls bounded, and a slain Maw down for good;
 10. upgrade tables (slice 4) — tier 0 of every track IS the slice-2
     sloop (legacy-constant identity, the recorded-route guard), tiers
     strictly improve, prices at-least-triple per step, hull ladder is
     the concept doc's 100/150/220, a max player rake + an enemy rake
     still fit the shared ball pool;
 11. port ledger (slice 4) — bw_port_buy exhaustively: spends exactly
     the price, refuses one gold short / at max tier / on a sound hull /
     on an unknown row (and a refusal changes NOTHING), repair prices
     ceil(missing / BW_REPAIR_PER_GOLD) for EVERY reachable hull value
     at every hull tier and refills to the tier max, tracks never
     cross-talk, buying is position/enemy/loot-pure;
 12. upgraded duels (slice 4) — with each single track maxed and all
     three maxed, the policy player still wins and the idle player is
     still sunk (upgrades delay the loss, never annul it);
 13. upgraded containment (slice 4) — adversarial input at max tiers
     keeps ship/balls in the sea with the raised speed bound;
 14. the Maw stalks (slice 5) — the whole telegraph contract, exact:
     down exactly BW_MAW_PATIENCE frames from the sink, rises at the
     wreck, every shadow exactly BW_MAW_SHADOW_FRAMES, every surfacing
     outside the pier sanctuary and exactly BW_MAW_SURFACE_FRAMES,
     then ONE bite of exactly BW_MAW_BITE hull per lunge and down
     again re-armed — plus the tunable-relation rails (shadow slower
     than full sail, lunge faster than any sail, monster crates
     richer, drops fit pool and hold);
 15. sanctuary (slice 5) — a berthed player is never surfaced on,
     lunged at, or bitten; the Maw keeps re-stirring; the pier still
     banks mid-prowl (the port is always reachable);
 16. the Maw slain (slice 5) — the hunter policy (the exact brain
     games/brineward-nds/tools/record-maw.py records routes from)
     survives, slays it, banks wreck + monster crates together, and a
     slain Maw NEVER stirs again.

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

# loot / gold (slice 3)
BW_MAX_LOOT = 8
BW_LOOT_DROPS = 3
BW_LOOT_VALUE = 5
BW_HOLD_CAP = 8
BW_LOOT_RING = 18
BW_SCOOP_RANGE = 10 * BW_ONE
BW_PIER_X = 128 * BW_ONE
BW_PIER_Y = 172 * BW_ONE
BW_DOCK_RANGE = 12 * BW_ONE

# port + upgrades (slice 4) — player-only tier tables, tier 0 = the
# slice-2 sloop exactly (check_upgrade_tables asserts the identities)
BW_UP_TIERS = 3
BW_REPAIR_PER_GOLD = 4
BW_PORT_ROW_HULL = 0
BW_PORT_ROW_CANNON = 1
BW_PORT_ROW_SAIL = 2
BW_PORT_ROW_REPAIR = 3
BW_PORT_ROWS = 4
BW_UP_HULL_OF = (BW_HULL_MAX, 150, 220)
BW_UP_RELOAD_OF = (BW_RELOAD_PLAYER, 70, 55)
BW_UP_BALLS_OF = (BW_BALLS_PER_SIDE, BW_BALLS_PER_SIDE, 4)
BW_UP_SPEED_OF = (0, 24, 48)
BW_UP_TURN_OF = (0, 1, 2)
BW_UP_COST = (0, 15, 45)

# wind + sailing feel (slice 6) — a slow-rotating seeded wind vector;
# the point of sail scales the trim target speed, canvas quarters per
# trim scale the catch. BW_WIND_SALT is pinned so every committed
# anchor seed (116, 117, 118, 119, 126, 127, 558, 728) is CALM — the
# pin-carry rule (check_wind_tables asserts it loudly).
BW_WIND_CALM, BW_WIND_BREEZE, BW_WIND_GALE = 0, 1, 2
BW_WIND_STATES = 3
BW_WIND_SALT = 0x57499826
BW_WIND_DIR_SALT = 0x57492881
BW_WIND_TURN_SHIFT = 3
BW_WIND_LEVEL_OF = (BW_WIND_CALM, BW_WIND_BREEZE, BW_WIND_BREEZE,
                    BW_WIND_GALE)
BW_WIND_PUSH_OF = (0, 12, 24)            # units/frame at FULL canvas
BW_WIND_CANVAS_OF = (1, 2, 4)            # canvas quarters per trim

# the Maw (slice 5) — shadow telegraph -> surface -> lunge; salvage
# water only, pier sanctuary, richer crates when slain
BW_MAW_PATIENCE = 600
BW_MAW_RESTIR = 240
BW_MAW_SHADOW_FRAMES = 150
BW_MAW_SURFACE_FRAMES = 60
BW_MAW_LUNGE_FRAMES = 55
BW_MAW_SHADOW_SPEED = 140
BW_MAW_LUNGE_SPEED = 520
BW_MAW_HULL = 120
BW_MAW_HIT_RANGE = 12 * BW_ONE
BW_MAW_BITE_RANGE = 12 * BW_ONE
BW_MAW_BITE = 35
BW_MAW_HARBOR = 40 * BW_ONE
BW_MAW_LOOT_DROPS = 3
BW_MAW_LOOT_VALUE = 15
BW_MAW_DOWN, BW_MAW_SHADOW, BW_MAW_SURFACE, BW_MAW_LUNGE = 0, 1, 2, 3

# danger bands + reefs (slice 7) — band tables whose band-0 row is the
# legacy constants (check_band_tables asserts the identities: the
# pin-carry rule, this slice's edition), plus the reef hazard laid at
# pure f(seed) positions with anchorage/pier clearance rails
BW_BANDS = 3
BW_MAX_REEFS = 5
BW_REEF_SALT = 0x8EEF5EED
BW_REEF_Y_SALT = 0xB0A7B0A7
BW_REEF_TRIES = 8
BW_REEF_RANGE = 8 * BW_ONE
BW_REEF_DMG = 15
BW_REEF_CLEAR = 28 * BW_ONE
BW_REEF_SHELF_X = 40
BW_REEF_SHELF_STEP = 40
BW_REEF_SHELF_Y = 44
BW_BAND_EHULL_OF = (BW_HULL_MAX, 130, 160)
BW_BAND_ERELOAD_OF = (BW_RELOAD_ENEMY, 120, 100)
BW_BAND_LOOT_VALUE_OF = (BW_LOOT_VALUE, 12, 25)
BW_BAND_MAW_VALUE_OF = (BW_MAW_LOOT_VALUE, 30, 50)
BW_BAND_REEF_OF = (0, 3, 5)

# synthesized audio (slice 8) — the pure DECISION layer: ambience tiers
# (the weather is the drone ladder, the Maw overrides) + one-shot cue
# tables (id order IS the priority order). Playback is main.c ARM9 glue
# over the sound FIFO and feeds NOTHING back into the sim.
BW_AMB_CALM, BW_AMB_BREEZE, BW_AMB_GALE, BW_AMB_MAW = 0, 1, 2, 3
BW_AMB_TIERS = 4
BW_CUE_NONE = 0
BW_CUE_CANNON = 1
BW_CUE_SPLASH = 2
BW_CUE_SCOOP = 3
BW_CUE_CRACK = 4
BW_CUE_SCRAPE = 5
BW_CUE_WRECK = 6
BW_CUE_DOCK = 7
BW_CUE_MAWRISE = 8
BW_CUE_BITE = 9
BW_CUE_SUNK = 10
BW_CUE_COUNT = 11
BW_CUE_ON_NOISE = 255
# { freq Hz, duty code, vol } per ambience tier (mirror of BW_AMB_ROWS)
BW_AMB_ROWS = (
    (49, 0, 8),                          # CALM: the swell against the hull
    (62, 1, 16),                         # BREEZE: the canvas fills
    (78, 3, 30),                         # GALE: the rigging howls
    (104, 4, 42),                        # MAW: the deep has a pulse
)
# { freq, len, duty|NOISE, vol, freq2 } per cue id (mirror of BW_CUE_ROWS)
BW_CUE_ROWS = (
    (0, 0, 0, 0, 0),                     # NONE
    (700, 7, BW_CUE_ON_NOISE, 96, 0),    # CANNON: the thump
    (1800, 10, BW_CUE_ON_NOISE, 40, 0),  # SPLASH: a spent rake hisses
    (587, 9, 4, 70, 0),                  # SCOOP: D5, a crate aboard
    (240, 12, BW_CUE_ON_NOISE, 90, 0),   # CRACK: timber gives
    (120, 26, BW_CUE_ON_NOISE, 84, 0),   # SCRAPE: the keel grates
    (90, 36, BW_CUE_ON_NOISE, 100, 0),   # WRECK: she goes under
    (523, 28, 4, 88, 784),               # DOCK: C5 -> G5, the chime
    (73, 30, 6, 104, 0),                 # MAWRISE: D2, the deep speaks
    (160, 22, BW_CUE_ON_NOISE, 112, 0),  # BITE: the jaws close
    (200, 60, BW_CUE_ON_NOISE, 116, 0),  # SUNK: the long cold rattle
)

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


class Loot:
    """Mirror of BwLoot."""

    __slots__ = ('x', 'y', 'value', 'live')

    def __init__(self):
        self.live = 0


class Maw:
    """Mirror of BwMaw."""

    __slots__ = ('x', 'y', 'vx', 'vy', 'state', 'hull', 'timer', 'wake',
                 'stirs', 'slain', 'bit')


class Reef:
    """Mirror of BwReef."""

    __slots__ = ('x', 'y', 'live', 'scraped')

    def __init__(self):
        self.live = 0


class Duel:
    """Mirror of BwDuel."""

    __slots__ = ('player', 'enemy', 'balls', 'loot', 'maw', 'reefs',
                 'hold', 'hold_gold', 'up_hull', 'up_cannon', 'up_sail',
                 'wind_level', 'wind_base', 'band', 'groundings',
                 'frame', 'over')

    def __init__(self):
        self.player = Ship()
        self.enemy = Ship()
        self.balls = [Ball() for _ in range(BW_MAX_BALLS)]
        self.loot = [Loot() for _ in range(BW_MAX_LOOT)]
        self.maw = Maw()
        self.reefs = [Reef() for _ in range(BW_MAX_REEFS)]


class Inputs:
    """Mirror of BwInputs."""

    __slots__ = ('turn', 'trim_delta', 'fire_l', 'fire_r')

    def __init__(self, turn=0, trim_delta=0, fire_l=0, fire_r=0):
        self.turn = turn
        self.trim_delta = trim_delta
        self.fire_l = fire_l
        self.fire_r = fire_r


def bw_wind_heading(d):
    """Mirror of bw_wind_heading() — the heading the wind blows TOWARD."""
    return (d.wind_base + (d.frame >> BW_WIND_TURN_SHIFT)) & (BW_CIRCLE - 1)


def bw_wind_push(d):
    """Mirror of bw_wind_push()."""
    return BW_WIND_PUSH_OF[d.wind_level]


def bw_ship_step(s, inputs, sail_tier, wind_hdg, wind_push):
    """Mirror of bw_ship_step(). sail_tier: SAIL upgrade tier (enemy 0);
    wind_hdg/wind_push: this water's wind this frame (slice 6)."""
    s.trim = _clamp(s.trim + inputs.trim_delta, BW_TRIM_BATTLE, BW_TRIM_FULL)
    s.heading = (s.heading
                 + inputs.turn * (BW_TURN_OF[s.trim]
                                  + BW_UP_TURN_OF[sail_tier])) \
        & (BW_CIRCLE - 1)

    target = BW_SPEED_OF[s.trim] + BW_UP_SPEED_OF[sail_tier]
    # point of sail (slice 6): shifts only — >> floors identically in
    # gcc/ARM and Python (the signed-division rule)
    wrel = ((s.heading - wind_hdg) & (BW_CIRCLE - 1)) >> 2
    target += (bw_cos(wrel) * wind_push * BW_WIND_CANVAS_OF[s.trim]) >> 10
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
    balls = BW_UP_BALLS_OF[d.up_cannon] if owner == 0 else BW_BALLS_PER_SIDE

    for k in range(balls):
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
        # slice 5: a player ball can wound the Maw while it is UP
        # (surface/lunge) — checked before the ship pass; in a duel the
        # Maw is BW_MAW_DOWN so this branch never runs (bit-identical)
        if (ball.owner == 0
                and d.maw.state in (BW_MAW_SURFACE, BW_MAW_LUNGE)
                and bw_chebyshev(ball.x, ball.y,
                                 d.maw.x, d.maw.y) < BW_MAW_HIT_RANGE):
            d.maw.hull -= dmg
            if d.maw.hull <= 0:
                d.maw.hull = 0
                d.maw.slain = 1
                d.maw.state = BW_MAW_DOWN
                bw_loot_spawn_at(d, d.maw.x, d.maw.y,
                                 BW_MAW_LOOT_DROPS,
                                 BW_BAND_MAW_VALUE_OF[d.band])
            ball.live = 0
            continue
        target = d.enemy if ball.owner == 0 else d.player
        if bw_chebyshev(ball.x, ball.y, target.x, target.y) < BW_HIT_RANGE:
            # dmg > 0 always, so floor at 0 — no upper clamp: an upgraded
            # player hull sits above BW_HULL_MAX (bit-identical at tier 0)
            target.hull -= dmg
            if target.hull < 0:
                target.hull = 0
            ball.live = 0


BW_LOOT_ANGLE = (0, 85, 171)             # fixed thirds of the crate ring


def bw_loot_spawn_at(d, wx, wy, drops, value):
    """Mirror of bw_loot_spawn_at()."""
    for k in range(drops):
        for c in d.loot:
            if c.live:
                continue
            c.x = _clamp(wx + BW_LOOT_RING * bw_sin(BW_LOOT_ANGLE[k]),
                         BW_SEA_X_MIN, BW_SEA_X_MAX)
            c.y = _clamp(wy - BW_LOOT_RING * bw_cos(BW_LOOT_ANGLE[k]),
                         BW_SEA_Y_MIN, BW_SEA_Y_MAX)
            c.value = value
            c.live = 1
            break


def bw_loot_spawn(d):
    """Mirror of bw_loot_spawn()."""
    bw_loot_spawn_at(d, d.enemy.x, d.enemy.y, BW_LOOT_DROPS,
                     BW_BAND_LOOT_VALUE_OF[d.band])


def bw_loot_step(d):
    """Mirror of bw_loot_step()."""
    for c in d.loot:
        if not c.live:
            continue
        if d.hold >= BW_HOLD_CAP:
            return
        if bw_chebyshev(c.x, c.y, d.player.x, d.player.y) < BW_SCOOP_RANGE:
            c.live = 0
            d.hold += 1
            d.hold_gold += c.value       # slice 5: crates carry their worth


def bw_dock_step(d):
    """Mirror of bw_dock_step() -> gold banked this frame."""
    if d.hold_gold <= 0:
        return 0
    if bw_chebyshev(d.player.x, d.player.y,
                    BW_PIER_X, BW_PIER_Y) >= BW_DOCK_RANGE:
        return 0
    banked = d.hold_gold
    d.hold = 0
    d.hold_gold = 0
    return banked


def bw_reef_spawn(d, seed):
    """Mirror of bw_reef_spawn() — this water's rocks, pure f(seed)."""
    n = BW_BAND_REEF_OF[d.band]
    for k in range(n):
        r = d.reefs[k]
        r.x = (BW_REEF_SHELF_X + BW_REEF_SHELF_STEP * k) * BW_ONE
        r.y = BW_REEF_SHELF_Y * BW_ONE
        for t in range(BW_REEF_TRIES):
            hx = bw_hash(seed, (BW_REEF_SALT + k * BW_REEF_TRIES + t) & U32)
            hy = bw_hash(hx, BW_REEF_Y_SALT)
            x = BW_SEA_X_MIN + hx % (BW_SEA_X_MAX - BW_SEA_X_MIN + 1)
            y = BW_SEA_Y_MIN + hy % (BW_SEA_Y_MAX - BW_SEA_Y_MIN + 1)
            if bw_chebyshev(x, y, BW_PLAYER_START_X,
                            BW_PLAYER_START_Y) < BW_REEF_CLEAR:
                continue
            if bw_chebyshev(x, y, BW_PIER_X, BW_PIER_Y) < BW_MAW_HARBOR:
                continue
            r.x = x
            r.y = y
            break
        r.live = 1
        r.scraped = 0


def bw_reefs_step(d):
    """Mirror of bw_reefs_step() — the player-only reef scrape."""
    for r in d.reefs:
        if not r.live:
            continue
        if bw_chebyshev(d.player.x, d.player.y, r.x, r.y) < BW_REEF_RANGE:
            if not r.scraped:
                d.player.hull -= BW_REEF_DMG
                if d.player.hull < 0:
                    d.player.hull = 0
                r.scraped = 1
                d.groundings += 1
        else:
            r.scraped = 0


def bw_maw_sound(m, frame):
    """Mirror of bw_maw_sound()."""
    m.state = BW_MAW_DOWN
    m.timer = 0
    m.wake = frame + BW_MAW_RESTIR


def bw_maw_step(d):
    """Mirror of bw_maw_step() — salvage water only."""
    m = d.maw
    if m.slain:
        return

    if m.state == BW_MAW_DOWN:
        if d.frame >= m.wake:
            m.state = BW_MAW_SHADOW
            m.timer = 0
            m.stirs += 1
            m.x = d.enemy.x
            m.y = d.enemy.y
            if bw_chebyshev(m.x, m.y, BW_PIER_X, BW_PIER_Y) < BW_MAW_HARBOR:
                m.y = BW_PIER_Y - BW_MAW_HARBOR

    elif m.state == BW_MAW_SHADOW:
        m.timer += 1
        # the shadow swims wherever it likes (the harbor keeps the
        # JAWS out, not the dark)
        m.x = _clamp(m.x + _clamp(d.player.x - m.x,
                                  -BW_MAW_SHADOW_SPEED,
                                  BW_MAW_SHADOW_SPEED),
                     BW_SEA_X_MIN, BW_SEA_X_MAX)
        m.y = _clamp(m.y + _clamp(d.player.y - m.y,
                                  -BW_MAW_SHADOW_SPEED,
                                  BW_MAW_SHADOW_SPEED),
                     BW_SEA_Y_MIN, BW_SEA_Y_MAX)
        if m.timer >= BW_MAW_SHADOW_FRAMES:
            # sanctuary: never surfaces inside the harbor ring, never
            # rises on a berthed player — gives up and sounds instead
            if (bw_chebyshev(d.player.x, d.player.y,
                             BW_PIER_X, BW_PIER_Y) < BW_MAW_HARBOR
                    or bw_chebyshev(m.x, m.y,
                                    BW_PIER_X, BW_PIER_Y) < BW_MAW_HARBOR):
                bw_maw_sound(m, d.frame)
            else:
                m.state = BW_MAW_SURFACE
                m.timer = 0

    elif m.state == BW_MAW_SURFACE:
        m.timer += 1
        if m.timer >= BW_MAW_SURFACE_FRAMES:
            if bw_chebyshev(d.player.x, d.player.y,
                            BW_PIER_X, BW_PIER_Y) < BW_MAW_HARBOR:
                bw_maw_sound(m, d.frame)     # never lunge at the berth
            else:
                dx = d.player.x - m.x
                dy = d.player.y - m.y
                adx = -dx if dx < 0 else dx
                ady = -dy if dy < 0 else dy
                big = adx if adx > ady else ady
                if big == 0:
                    m.vx = 0
                    m.vy = BW_MAW_LUNGE_SPEED
                else:
                    m.vx = adx * BW_MAW_LUNGE_SPEED // big
                    m.vy = ady * BW_MAW_LUNGE_SPEED // big
                    if dx < 0:
                        m.vx = -m.vx
                    if dy < 0:
                        m.vy = -m.vy
                m.state = BW_MAW_LUNGE
                m.timer = 0
                m.bit = 0

    elif m.state == BW_MAW_LUNGE:
        m.timer += 1
        m.x = _clamp(m.x + m.vx, BW_SEA_X_MIN, BW_SEA_X_MAX)
        m.y = _clamp(m.y + m.vy, BW_SEA_Y_MIN, BW_SEA_Y_MAX)
        if (not m.bit
                and bw_chebyshev(m.x, m.y, d.player.x, d.player.y)
                < BW_MAW_BITE_RANGE):
            d.player.hull -= BW_MAW_BITE
            if d.player.hull < 0:
                d.player.hull = 0
            m.bit = 1
            bw_maw_sound(m, d.frame)         # it got its bite
        elif m.timer >= BW_MAW_LUNGE_FRAMES:
            bw_maw_sound(m, d.frame)         # missed: stalk again


def bw_up_hull_max(tier):
    """Mirror of bw_up_hull_max()."""
    return BW_UP_HULL_OF[tier]


def bw_up_reload(tier):
    """Mirror of bw_up_reload()."""
    return BW_UP_RELOAD_OF[tier]


def bw_up_price(tier):
    """Mirror of bw_up_price()."""
    return BW_UP_COST[tier]


def bw_repair_cost(d):
    """Mirror of bw_repair_cost()."""
    missing = BW_UP_HULL_OF[d.up_hull] - d.player.hull
    if missing <= 0:
        return 0
    # positive // positive only (signed-division rule kept trivially)
    return (missing + BW_REPAIR_PER_GOLD - 1) // BW_REPAIR_PER_GOLD


def bw_port_buy(d, row, gold):
    """Mirror of bw_port_buy() -> gold spent (0 = refused)."""
    if row == BW_PORT_ROW_REPAIR:
        cost = bw_repair_cost(d)
        if cost <= 0 or gold < cost:
            return 0
        d.player.hull = BW_UP_HULL_OF[d.up_hull]
        return cost

    attr = {BW_PORT_ROW_HULL: 'up_hull',
            BW_PORT_ROW_CANNON: 'up_cannon',
            BW_PORT_ROW_SAIL: 'up_sail'}.get(row)
    if attr is None or getattr(d, attr) >= BW_UP_TIERS - 1:
        return 0
    cost = BW_UP_COST[getattr(d, attr) + 1]
    if gold < cost:
        return 0
    setattr(d, attr, getattr(d, attr) + 1)   # buying != healing: REPAIR
    return cost                              # fills the raised ceiling


def bw_amb_tier(maw_up, wind_level):
    """Mirror of bw_amb_tier()."""
    if maw_up:
        return BW_AMB_MAW
    if wind_level <= BW_WIND_CALM:
        return BW_AMB_CALM
    if wind_level >= BW_WIND_GALE:
        return BW_AMB_GALE
    return BW_AMB_BREEZE


def bw_amb_freq(tier):
    """Mirror of bw_amb_freq()."""
    return BW_AMB_ROWS[tier if 0 <= tier < BW_AMB_TIERS else 0][0]


def bw_amb_duty(tier):
    """Mirror of bw_amb_duty()."""
    return BW_AMB_ROWS[tier if 0 <= tier < BW_AMB_TIERS else 0][1]


def bw_amb_vol(tier):
    """Mirror of bw_amb_vol()."""
    return BW_AMB_ROWS[tier if 0 <= tier < BW_AMB_TIERS else 0][2]


def bw_cue_freq(cue):
    """Mirror of bw_cue_freq()."""
    return BW_CUE_ROWS[cue if 0 <= cue < BW_CUE_COUNT else 0][0]


def bw_cue_len(cue):
    """Mirror of bw_cue_len()."""
    return BW_CUE_ROWS[cue if 0 <= cue < BW_CUE_COUNT else 0][1]


def bw_cue_duty(cue):
    """Mirror of bw_cue_duty()."""
    return BW_CUE_ROWS[cue if 0 <= cue < BW_CUE_COUNT else 0][2]


def bw_cue_vol(cue):
    """Mirror of bw_cue_vol()."""
    return BW_CUE_ROWS[cue if 0 <= cue < BW_CUE_COUNT else 0][3]


def bw_cue_freq2(cue):
    """Mirror of bw_cue_freq2()."""
    return BW_CUE_ROWS[cue if 0 <= cue < BW_CUE_COUNT else 0][4]


class AudioGlue:
    """Line-for-line model of main.c's slice-8 audio glue: the frame-
    start snapshot, the cue-priority ladder, the cue channel countdown
    (with the two-note re-tune), and the ambience drone flips. Drives
    the audio-slice host checks AND the emulator watch-log prediction
    (slots 38-45), so a drift between this model and main.c fails the
    mirror before it silently re-times a pin."""

    def __init__(self):
        self.amb_on = False
        self.amb_tier = 0
        self.amb_flips = 0
        self.cue_left = 0
        self.cue_flip_at = 0
        self.cue_freq_now = 0
        self.last_cue = 0
        self.cues = 0
        self.cue_frame = 0

    def snapshot(self, d, wins, maws):
        """The frame-start capture (main.c's prev_* block)."""
        return (d.player.hull, d.enemy.hull, d.maw.hull, d.maw.state,
                d.maw.bit, d.hold, d.groundings, wins, maws,
                d.player.reload_l, d.player.reload_r,
                d.enemy.reload_l, d.enemy.reload_r,
                sum(1 for b in d.balls if b.live))

    def frame(self, snap, d, wins, maws, banked, state, prev_state,
              frame, state_sunk):
        """One post-state-machine audio pass. `state`/`prev_state` are
        main.c states; `state_sunk` is the STATE_SUNK constant (2)."""
        (p_ph, p_eh, p_mh, p_ms, p_mb, p_hold, p_gnd, p_wins, p_maws,
         p_rpl, p_rpr, p_rel, p_rer, p_balls) = snap

        if self.cue_left > 0:
            self.cue_left -= 1
            if self.cue_left == 0:
                self.cue_freq_now = 0
            elif self.cue_flip_at != 0 and self.cue_left == self.cue_flip_at:
                self.cue_freq_now = bw_cue_freq2(self.last_cue)

        cue = BW_CUE_NONE
        if (d.player.reload_l > p_rpl or d.player.reload_r > p_rpr
                or d.enemy.reload_l > p_rel or d.enemy.reload_r > p_rer):
            cue = BW_CUE_CANNON
        balls_now = sum(1 for b in d.balls if b.live)
        if balls_now < p_balls:
            cue = BW_CUE_SPLASH
        if d.hold > p_hold:
            cue = BW_CUE_SCOOP
        if (d.enemy.hull < p_eh or d.maw.hull < p_mh
                or d.player.hull < p_ph):
            cue = BW_CUE_CRACK
        if d.groundings > p_gnd:
            cue = BW_CUE_SCRAPE
        if wins > p_wins or maws > p_maws:
            cue = BW_CUE_WRECK
        if banked > 0:
            cue = BW_CUE_DOCK
        if d.maw.state == BW_MAW_SURFACE and p_ms != BW_MAW_SURFACE:
            cue = BW_CUE_MAWRISE
        if d.maw.bit and not p_mb:
            cue = BW_CUE_BITE
        if state == state_sunk and prev_state != state_sunk:
            cue = BW_CUE_SUNK
        if cue != BW_CUE_NONE:
            self.cue_freq_now = bw_cue_freq(cue)
            self.cue_left = bw_cue_len(cue)
            self.cue_flip_at = (self.cue_left - self.cue_left // 2
                                if bw_cue_freq2(cue) != 0 else 0)
            self.last_cue = cue
            self.cues += 1
            self.cue_frame = frame

        at_sea = state in (1, 3)         # STATE_DUEL / STATE_SALVAGE
        if at_sea:
            tier = bw_amb_tier(1 if d.maw.state != BW_MAW_DOWN else 0,
                               d.wind_level)
            if not self.amb_on or tier != self.amb_tier:
                self.amb_on = True
                self.amb_tier = tier
                self.amb_flips += 1
        elif self.amb_on:
            self.amb_on = False
            self.amb_flips += 1
        return cue

    def words(self):
        """Telemetry slots 38-45 exactly as main.c writes them."""
        return (self.amb_tier, self.last_cue, self.cues, self.cue_frame,
                bw_amb_freq(self.amb_tier) if self.amb_on else 0,
                self.amb_flips, self.cue_left, self.cue_freq_now)


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
    for c in d.loot:
        c.live = 0
    m = d.maw                            # slice 5: fresh water, the Maw is
    m.x = m.y = m.vx = m.vy = 0          #   down and biding — the patience
    m.state = BW_MAW_DOWN                #   clock arms on the SINK frame
    m.hull = BW_MAW_HULL
    m.timer = 0
    m.wake = 0
    m.stirs = 0
    m.slain = 0
    m.bit = 0
    for r in d.reefs:                    # slice 7: the band-0 water is
        r.x = r.y = 0                    #   open sea — no rocks; deeper
        r.live = 0                       #   waters lay theirs in
        r.scraped = 0                    #   bw_water_init
    d.hold = 0                           # caller re-injects a carried hold
    d.hold_gold = 0                      # (sinking forfeits: no re-inject)
    d.up_hull = 0                        # caller re-injects the bought
    d.up_cannon = 0                      #   tiers (upgrades are NEVER
    d.up_sail = 0                        #   lost — main.c's Score owns them)
    # slice 6: this water's weather, pure f(seed) like the spawn ring
    d.wind_level = BW_WIND_LEVEL_OF[bw_hash(seed, BW_WIND_SALT) & 3]
    d.wind_base = bw_hash(seed, BW_WIND_DIR_SALT) & (BW_CIRCLE - 1)
    d.band = 0                           # slice 7: this IS the band-0
    d.groundings = 0                     #   water (bw_water_init deepens)
    d.frame = 0
    d.over = BW_DUEL_RUNNING


def bw_water_init(d, seed, band):
    """Mirror of bw_water_init() — a water one band deep."""
    if band < 0:
        band = 0
    if band > BW_BANDS - 1:
        band = BW_BANDS - 1
    bw_duel_init(d, seed)
    d.band = band
    # one weather level worse per band, min-clamped at gale
    d.wind_level = BW_WIND_GALE if d.wind_level + band > BW_WIND_GALE \
        else d.wind_level + band
    d.enemy.hull = BW_BAND_EHULL_OF[band]
    d.enemy.reload_l = BW_BAND_ERELOAD_OF[band] // 2   # same grace rule
    d.enemy.reload_r = BW_BAND_ERELOAD_OF[band] // 2
    bw_reef_spawn(d, seed)


def bw_duel_step(d, inputs):
    """Mirror of bw_duel_step()."""
    if d.over != BW_DUEL_RUNNING:
        return

    ai = bw_ai(d.enemy, d.player)

    wind_hdg = bw_wind_heading(d)        # slice 6: one wind, both
    wind_push = bw_wind_push(d)          #   ships sail it
    bw_ship_step(d.player, inputs, d.up_sail, wind_hdg, wind_push)
    bw_ship_step(d.enemy, ai, 0, wind_hdg, wind_push)
    bw_reefs_step(d)                     # slice 7: the rocks take their
    #   toll where the keel just went

    if inputs.fire_l and d.player.reload_l == 0:
        bw_fire(d, d.player, 0, -1)
        d.player.reload_l = BW_UP_RELOAD_OF[d.up_cannon]
    if inputs.fire_r and d.player.reload_r == 0:
        bw_fire(d, d.player, 0, 1)
        d.player.reload_r = BW_UP_RELOAD_OF[d.up_cannon]
    if ai.fire_l and d.enemy.reload_l == 0:
        bw_fire(d, d.enemy, 1, -1)
        d.enemy.reload_l = BW_BAND_ERELOAD_OF[d.band]
    if ai.fire_r and d.enemy.reload_r == 0:
        bw_fire(d, d.enemy, 1, 1)
        d.enemy.reload_r = BW_BAND_ERELOAD_OF[d.band]

    bw_balls_step(d)
    bw_loot_step(d)                      # crates exist only after a sink

    if d.player.hull <= 0:
        d.over = BW_DUEL_PLAYER_SUNK
    elif d.enemy.hull <= 0:
        d.over = BW_DUEL_ENEMY_SUNK
        bw_loot_spawn(d)                 # the wreck breaks up into flotsam
        d.maw.wake = d.frame + BW_MAW_PATIENCE   # the blood is in the
        # water: the Maw's clock starts

    d.frame += 1


def bw_salvage_step(d, inputs):
    """Mirror of bw_salvage_step()."""
    if d.over != BW_DUEL_ENEMY_SUNK:
        return

    bw_ship_step(d.player, inputs, d.up_sail,
                 bw_wind_heading(d), bw_wind_push(d))
    bw_reefs_step(d)                     # slice 7: rocks scrape salvage
    bw_maw_step(d)                       # slice 5: the water is not empty

    # the batteries WAKE while the Maw is up; cold on quiet water
    if d.maw.state != BW_MAW_DOWN:
        if inputs.fire_l and d.player.reload_l == 0:
            bw_fire(d, d.player, 0, -1)
            d.player.reload_l = BW_UP_RELOAD_OF[d.up_cannon]
        if inputs.fire_r and d.player.reload_r == 0:
            bw_fire(d, d.player, 0, 1)
            d.player.reload_r = BW_UP_RELOAD_OF[d.up_cannon]

    bw_balls_step(d)                     # a late rake can still strike
    bw_loot_step(d)

    if d.player.hull <= 0:
        d.over = BW_DUEL_PLAYER_SUNK

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


# --- the salvage policy (the salvage-route recorder's brain) -------------------
# NOT a mirror of any C — the host-side brain the salvage proof records:
# drop to battle sail (tightest turn radius, ~10 px — under both the
# scoop and dock windows) and put the bow on the nearest crate; once the
# hold has every crate (or is full), come home to the pier and bank.

def bw_goto(p, tx, ty):
    """Sail-to-point policy -> Inputs (battle sail, bow on the target)."""
    out = Inputs()
    out.trim_delta = -1 if p.trim > BW_TRIM_BATTLE else 0
    dx = tx - p.x
    dy = ty - p.y
    brad = p.heading >> 2
    hx = bw_sin(brad)
    hy = -bw_cos(brad)
    dot = hx * dx + hy * dy
    cross = hx * dy - hy * dx
    adot = -dot if dot < 0 else dot
    across = -cross if cross < 0 else cross
    if not (dot > 0 and across <= (adot >> 3)):     # outside ~7deg of bow
        out.turn = 1 if cross > 0 else (-1 if cross < 0 else 1)
    return out


def bw_salvage_policy(d):
    """Salvage brain: nearest crate first, then home to the pier."""
    p = d.player
    best = None
    best_d = None
    if d.hold < BW_HOLD_CAP:
        for c in d.loot:
            if not c.live:
                continue
            dist = bw_chebyshev(p.x, p.y, c.x, c.y)
            if best is None or dist < best_d:
                best, best_d = c, dist
    if best is not None:
        return bw_goto(p, best.x, best.y)
    return bw_goto(p, BW_PIER_X, BW_PIER_Y)


# --- the Maw policies (the maw-route recorder's brains, slice 5) ----------------
# NOT mirrors of any C — host-side player brains that record-maw.py
# records and the Maw host proofs converge with. The BAIT sails at the
# dark water like a fool and takes the bite (the omen/bite proof). The
# HUNTER flees the shadow under full sail (it is slower than full sail
# BY DESIGN), then holds the surfaced Maw on a beam and rakes it — the
# same cross/dot gunnery bw_policy uses — until it is slain, then
# scoops and banks like any salvage.

BW_MUSTER_X = 128 * BW_ONE               # open water, well off the pier
BW_MUSTER_Y = 85 * BW_ONE


def bw_maw_bait_policy(d):
    """The bait: sail straight at whatever is out there (and learn)."""
    m = d.maw
    if m.state != BW_MAW_DOWN:
        return bw_goto(d.player, m.x, m.y)
    return bw_goto(d.player, BW_MUSTER_X, BW_MUSTER_Y)


def bw_run_to(p, tx, ty):
    """Sail-to-point with WAY ON: come about hard at battle sail
    (tightest helm), then sheet home to full once the bow is near
    enough the bearing — full sail is the only trim the shadow cannot
    hold onto, so the hunter never loiters slow."""
    out = Inputs()
    dx = tx - p.x
    dy = ty - p.y
    brad = p.heading >> 2
    hx = bw_sin(brad)
    hy = -bw_cos(brad)
    dot = hx * dx + hy * dy
    cross = hx * dy - hy * dx
    adot = -dot if dot < 0 else dot
    across = -cross if cross < 0 else cross
    if dot > 0 and across <= (adot >> 1):        # within ~27 deg: run
        out.trim_delta = 1 if p.trim < BW_TRIM_FULL else 0
        if not (across <= (adot >> 3)):
            out.turn = 1 if cross > 0 else (-1 if cross < 0 else 1)
    else:                                        # come about first
        out.trim_delta = -1 if p.trim > BW_TRIM_BATTLE else 0
        out.turn = 1 if cross > 0 else (-1 if cross < 0 else 1)
    return out


def bw_flee_shadow(p, mx, my):
    """Cardinal kite: run the axis with the most sea room that neither
    closes on the threat nor bolts into the sanctuary. Cardinal
    headings are the sloop's fastest chebyshev escape (the shadow homes
    in chebyshev too), sea room DOMINATES fleeing pure-away (a cornered
    sloop breaks out along the wall PAST the slow shadow instead of
    pinning itself), and the kite never runs INTO the threat axis —
    sailing at the shadow is how you get lunged head-on."""
    room = {'E': BW_SEA_X_MAX - p.x, 'W': p.x - BW_SEA_X_MIN,
            'S': BW_SEA_Y_MAX - p.y, 'N': p.y - BW_SEA_Y_MIN}
    away = {'E': mx <= p.x, 'W': mx >= p.x,
            'S': my <= p.y, 'N': my >= p.y}
    # a direction pointing straight down the threat's throat is out
    # unless it is the ONLY sea room left
    adx = mx - p.x if mx > p.x else p.x - mx
    ady = my - p.y if my > p.y else p.y - my
    toward = {'E': mx > p.x and adx > 2 * ady,
              'W': mx < p.x and adx > 2 * ady,
              'S': my > p.y and ady > 2 * adx,
              'N': my < p.y and ady > 2 * adx}
    px_off_pier = p.x - BW_PIER_X
    if -55 * BW_ONE < px_off_pier < 55 * BW_ONE:
        room['S'] = 0                    # no bolting into the sanctuary
    best = max(('E', 'W', 'S', 'N'),
               key=lambda k: (room[k] > 35 * BW_ONE and not toward[k],
                              room[k] > 35 * BW_ONE, away[k], room[k]))
    tx, ty = {'E': (BW_SEA_X_MAX, p.y), 'W': (BW_SEA_X_MIN, p.y),
              'S': (p.x, BW_SEA_Y_MAX), 'N': (p.x, BW_SEA_Y_MIN)}[best]
    return bw_run_to(p, tx, ty)


def bw_fight_maw(p, m, fire=1, hold=0):
    """HALF sail, hold the risen Maw on a beam, rake when aligned.

    Beam-on IS the dodge: a lunge is latched at the player's position,
    so a hull moving perpendicular to the latch bearing at half-sail
    speed clears the bite window of any lunge latched beyond ~40 px —
    and the same heading is the one the batteries bear on. Gunnery and
    evasion are one posture (fire=0 keeps it without wasting powder)."""
    out = Inputs()
    out.trim_delta = (1 if p.trim < BW_TRIM_HALF
                      else (-1 if p.trim > BW_TRIM_HALF else 0))
    dx = m.x - p.x
    dy = m.y - p.y
    brad = p.heading >> 2
    hx = bw_sin(brad)
    hy = -bw_cos(brad)
    dot = hx * dx + hy * dy
    cross = hx * dy - hy * dx
    adot = -dot if dot < 0 else dot
    across = -cross if cross < 0 else cross
    dist = bw_chebyshev(p.x, p.y, m.x, m.y)
    if BW_AI_TURN_ARC * adot >= across:
        ds = 1 if dot > 0 else (-1 if dot < 0 else 1)
        cs = 1 if cross > 0 else (-1 if cross < 0 else 1)
        out.turn = -ds * cs
    if hold:
        # a latched lunge is a STRAIGHT line. Crossing lunge: hold
        # course and the latch-time beam velocity carries the hull
        # clear (turning to track it would curve back into its path).
        # In-line lunge (head-on or astern — the jaws run down the
        # keel axis): hard rudder instead, there is nothing to hold.
        mvx, mvy = m.vx, m.vy
        vdot = hx * mvx + hy * mvy
        vcross = hx * mvy - hy * mvx
        avdot = -vdot if vdot < 0 else vdot
        avcross = -vcross if vcross < 0 else vcross
        if avdot > avcross:
            out.turn = 1 if vcross > 0 else -1   # break off the keel line
        else:
            out.turn = 0
    # NEVER let the posture pin the hull on a wall: a clamped keel has
    # zero dodge velocity. If the bow is close to running out of sea,
    # steer for open water instead — the guns still fire on alignment.
    vx = hx * (p.speed if p.speed > 0 else 1)
    vy = hy * (p.speed if p.speed > 0 else 1)
    tx = ty = 1 << 30
    if vx > 0:
        tx = ((BW_SEA_X_MAX - p.x) << 8) // vx
    elif vx < 0:
        tx = ((p.x - BW_SEA_X_MIN) << 8) // -vx
    if vy > 0:
        ty = ((BW_SEA_Y_MAX - p.y) << 8) // vy
    elif vy < 0:
        ty = ((p.y - BW_SEA_Y_MIN) << 8) // -vy
    if min(tx, ty) < 70:
        cx = ((BW_SEA_X_MIN + BW_SEA_X_MAX) >> 1) - p.x
        cy = ((BW_SEA_Y_MIN + BW_SEA_Y_MAX) >> 1) - p.y
        ccross = hx * cy - hy * cx
        out.turn = 1 if ccross > 0 else -1
    if fire and dist < POLICY_RANGE and POLICY_FIRE_ARC * adot < across:
        if cross > 0:
            out.fire_r = 1
        else:
            out.fire_l = 1
    return out


# inset corner stations for the quiet-water patrol (well off every
# wall, outside the pier sanctuary even with the 28 px orbit)
BW_HUNT_STATIONS = ((61 * BW_ONE, 77 * BW_ONE),
                    (194 * BW_ONE, 77 * BW_ONE),
                    (61 * BW_ONE, 128 * BW_ONE),
                    (194 * BW_ONE, 128 * BW_ONE))

BW_HUNT_COMFORT = 75 * BW_ONE            # shadow closer than this: run;
                                         # farther: hold the gun posture
BW_HUNT_STANDOFF = 55 * BW_ONE           # closer than this at the windup:
                                         # sheet home and open the range


def bw_maw_hunt_policy(d):
    """The hunter: run from a close shadow, hold posture on a far one,
    rake the windup, dodge the charge, then salvage the carcass.

    The bite is dodged by GEOMETRY: fleeing keeps the sloop moving off
    the lunge bearing, so a lunge latched from far enough away slides
    past the stern — the standoff/comfort rules keep "far enough" true.
    """
    m = d.maw
    if m.slain:
        return bw_salvage_policy(d)      # the water is safe: scoop and bank
    p = d.player
    if m.state == BW_MAW_SHADOW:
        return bw_flee_shadow(p, m.x, m.y)       # keep sea room
    if m.state == BW_MAW_SURFACE:
        return bw_fight_maw(p, m)        # beam-on: gunnery IS the dodge
    if m.state == BW_MAW_LUNGE:
        return bw_fight_maw(p, m, hold=1)        # dodge straight, guns talking
    # quiet water: hold the inset corner station FARTHEST from the
    # wreck (where the blood is), orbiting it with WAY ON — the
    # rotating waypoint is a pure function of d.frame (recordable),
    # so the stir never catches the sloop slow, close, or pinned
    sx, sy = max(BW_HUNT_STATIONS,
                 key=lambda st: bw_chebyshev(d.enemy.x, d.enemy.y,
                                             st[0], st[1]))
    a = (d.frame >> 1) & 255
    return bw_run_to(p, sx + 28 * bw_cos(a), sy + 28 * bw_sin(a))


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
            # slice 6: a tailwind may fill the canvas past the trim
            # target by at most the water's push
            assert 0 < s.speed <= BW_SPEED_OF[BW_TRIM_FULL] \
                + bw_wind_push(d), f
        for ball in d.balls:
            if ball.live:
                assert BW_SEA_X_MIN <= ball.x <= BW_SEA_X_MAX, f
                assert BW_SEA_Y_MIN <= ball.y <= BW_SEA_Y_MAX, f
    print(f'containment: {frames} adversarial frames — ships and balls '
          'never leave the sea, trim/heading/speed in range')


def _win_state(seed):
    """Run the beam-holding policy duel to the win; returns the Duel."""
    d = Duel()
    bw_duel_init(d, seed)
    for _ in range(3600):
        bw_duel_step(d, bw_policy(d.player, d.enemy))
        if d.over != BW_DUEL_RUNNING:
            break
    assert d.over == BW_DUEL_ENEMY_SUNK, (seed, d.over)
    return d


def check_loot_drop():
    for seed in range(16):
        d = _win_state(seed)
        live = [c for c in d.loot if c.live]
        assert len(live) == BW_LOOT_DROPS, (seed, len(live))
        assert d.hold == 0 and d.hold_gold == 0, seed
        for c in live:
            assert BW_SEA_X_MIN <= c.x <= BW_SEA_X_MAX, seed
            assert BW_SEA_Y_MIN <= c.y <= BW_SEA_Y_MAX, seed
            off = bw_chebyshev(c.x, c.y, d.enemy.x, d.enemy.y)
            assert off <= BW_LOOT_RING * BW_ONE, (seed, off)
    print(f'loot drop: 16 policy wins each break up into exactly '
          f'{BW_LOOT_DROPS} crates, in the sea, on the '
          f'{BW_LOOT_RING} px wreck ring')


def check_salvage_banks():
    worst = 0
    for seed in range(16):
        d = _win_state(seed)
        banked = 0
        for _ in range(3600):
            bw_salvage_step(d, bw_salvage_policy(d))
            banked += bw_dock_step(d)
            if banked > 0:
                break
        assert d.over == BW_DUEL_ENEMY_SUNK, (seed, d.over)
        assert not any(c.live for c in d.loot), seed
        assert banked == BW_LOOT_DROPS * BW_LOOT_VALUE, (seed, banked)
        assert d.hold == 0 and d.hold_gold == 0, seed
        worst = max(worst, d.frame)
    print(f'salvage: from 16 win states the salvage policy scoops all '
          f'{BW_LOOT_DROPS} crates and banks '
          f'{BW_LOOT_DROPS * BW_LOOT_VALUE}g at the pier '
          f'by duel frame {worst}')


def check_hold_cap():
    for h in range(BW_HOLD_CAP + 1):
        d = Duel()
        bw_duel_init(d, h)
        d.hold = h
        d.hold_gold = h * BW_LOOT_VALUE
        for c in d.loot:                 # 8 crates right on the hull
            c.x, c.y = d.player.x, d.player.y
            c.value = BW_LOOT_VALUE
            c.live = 1
        bw_loot_step(d)
        scooped = BW_HOLD_CAP - h
        assert d.hold == BW_HOLD_CAP, (h, d.hold)
        assert d.hold_gold == (h + scooped) * BW_LOOT_VALUE, h
        left = sum(1 for c in d.loot if c.live)
        assert left == BW_MAX_LOOT - scooped, (h, left)
    print(f'hold cap: never exceeds {BW_HOLD_CAP} crates from any start '
          'fill; overflow crates stay afloat; gold = 5g per crate exactly')


def check_dock():
    for off_px, banks in ((0, True), (11, True), (12, False), (40, False)):
        d = Duel()
        bw_duel_init(d, 0)
        d.hold = 3
        d.hold_gold = 3 * BW_LOOT_VALUE
        d.player.x = BW_PIER_X + off_px * BW_ONE
        d.player.y = BW_PIER_Y
        got = bw_dock_step(d)
        if banks:
            assert got == 15 and d.hold == 0 and d.hold_gold == 0, off_px
            assert bw_dock_step(d) == 0     # empty hold banks nothing
        else:
            assert got == 0 and d.hold == 3 and d.hold_gold == 15, off_px
    print('dock: banking happens strictly inside the 12 px pier window, '
          'empties the hold exactly once, never pays twice')


def check_salvage_containment():
    # Slice 5: the salvage water now holds the Maw — the same adversarial
    # sweep drives fire keys too (the batteries wake when it rises), the
    # player hull is pinned back up each frame so bites can't end the run
    # (check_containment's "never let it end" trick), and the Maw's own
    # words are contained: position in the sea whenever it is up, state
    # in range, hull in [0, BW_MAW_HULL], and a slain Maw STAYS down.
    d = Duel()
    bw_duel_init(d, 0x5A1)
    d.enemy.hull = 0
    d.over = BW_DUEL_ENEMY_SUNK
    bw_loot_spawn(d)
    d.maw.wake = d.frame + BW_MAW_PATIENCE
    frames = 8000
    stirs_after_slain = None
    for f in range(frames):
        h = bw_hash(0x10A7, f)
        inp = Inputs(turn=(h & 3) - 1 if (h & 3) < 3 else 0,
                     trim_delta=((h >> 2) & 3) - 1 if ((h >> 2) & 3) < 3
                     else 0,
                     fire_l=(h >> 4) & 1, fire_r=(h >> 5) & 1)
        d.player.hull = BW_HULL_MAX      # adversarial: never let it end
        d.over = BW_DUEL_ENEMY_SUNK
        bw_salvage_step(d, inp)
        bw_dock_step(d)
        s = d.player
        assert BW_SEA_X_MIN <= s.x <= BW_SEA_X_MAX, f
        assert BW_SEA_Y_MIN <= s.y <= BW_SEA_Y_MAX, f
        assert 0 <= d.hold <= BW_HOLD_CAP, f
        if d.hold == 0:
            assert d.hold_gold == 0, f
        else:
            assert d.hold * BW_LOOT_VALUE <= d.hold_gold \
                <= d.hold * BW_MAW_LOOT_VALUE, f
        for c in d.loot:
            if c.live:
                assert BW_SEA_X_MIN <= c.x <= BW_SEA_X_MAX, f
                assert BW_SEA_Y_MIN <= c.y <= BW_SEA_Y_MAX, f
        m = d.maw
        assert m.state in (BW_MAW_DOWN, BW_MAW_SHADOW, BW_MAW_SURFACE,
                           BW_MAW_LUNGE), f
        assert 0 <= m.hull <= BW_MAW_HULL, f
        if m.state != BW_MAW_DOWN:
            assert BW_SEA_X_MIN <= m.x <= BW_SEA_X_MAX, f
            assert BW_SEA_Y_MIN <= m.y <= BW_SEA_Y_MAX, f
        if m.slain:
            assert m.state == BW_MAW_DOWN, f
            if stirs_after_slain is None:
                stirs_after_slain = m.stirs
            assert m.stirs == stirs_after_slain, f
    print(f'salvage containment: {frames} adversarial salvage frames '
          '(fire keys live, the Maw stalking) — ship, crates, and Maw '
          'stay in the sea, hold and hulls stay bounded, a slain Maw '
          'stays down')


def check_upgrade_tables():
    # tier 0 IS the slice-2 sloop — the recorded-route/pin guard: any
    # drift here invalidates every committed route and carried proof pin.
    assert BW_UP_HULL_OF[0] == BW_HULL_MAX
    assert BW_UP_RELOAD_OF[0] == BW_RELOAD_PLAYER
    assert BW_UP_BALLS_OF[0] == BW_BALLS_PER_SIDE
    assert BW_UP_SPEED_OF[0] == 0 and BW_UP_TURN_OF[0] == 0
    assert BW_UP_COST[0] == 0
    # the concept doc's hull ladder, verbatim
    assert BW_UP_HULL_OF == (100, 150, 220)
    for t in range(1, BW_UP_TIERS):
        assert BW_UP_HULL_OF[t] > BW_UP_HULL_OF[t - 1], t
        assert BW_UP_RELOAD_OF[t] < BW_UP_RELOAD_OF[t - 1], t
        assert BW_UP_BALLS_OF[t] >= BW_UP_BALLS_OF[t - 1], t
        assert BW_UP_SPEED_OF[t] > BW_UP_SPEED_OF[t - 1], t
        assert BW_UP_TURN_OF[t] >= BW_UP_TURN_OF[t - 1], t
        assert BW_UP_COST[t] >= 3 * BW_UP_COST[t - 1], t   # doc: ~triple
    assert BW_UP_COST[1] > 0
    # a max-tier player rake + a full enemy rake still fit the ball pool
    assert BW_UP_BALLS_OF[-1] + BW_BALLS_PER_SIDE <= BW_MAX_BALLS
    # the player can always outshoot the enemy crew at any tier
    assert BW_UP_RELOAD_OF[-1] >= 1
    assert BW_UP_RELOAD_OF[0] < BW_RELOAD_ENEMY
    print('upgrade tables: tier 0 = the slice-2 sloop exactly (route '
          'guard), tiers strictly improve, prices triple, hull ladder '
          f'{BW_UP_HULL_OF} per the concept doc')


def _fresh_port_duel(hull=None, tiers=(0, 0, 0)):
    d = Duel()
    bw_duel_init(d, 7)
    d.up_hull, d.up_cannon, d.up_sail = tiers
    d.player.hull = BW_UP_HULL_OF[tiers[0]] if hull is None else hull
    return d


def _duel_fingerprint(d):
    """Everything bw_port_buy must NOT touch."""
    m = d.maw
    return (d.player.x, d.player.y, d.player.heading, d.player.trim,
            d.player.speed, d.player.reload_l, d.player.reload_r,
            d.enemy.x, d.enemy.y, d.enemy.hull, d.hold, d.hold_gold,
            tuple((c.live) for c in d.loot),
            tuple((b.live) for b in d.balls), d.frame, d.over,
            (m.x, m.y, m.vx, m.vy, m.state, m.hull, m.timer, m.wake,
             m.stirs, m.slain, m.bit),
            d.wind_level, d.wind_base,   # slice 6: no shopping the weather
            d.band, d.groundings,        # slice 7: nor the charts
            tuple((r.x, r.y, r.live, r.scraped) for r in d.reefs))


def check_port_buy():
    tracks = ((BW_PORT_ROW_HULL, 'up_hull'),
              (BW_PORT_ROW_CANNON, 'up_cannon'),
              (BW_PORT_ROW_SAIL, 'up_sail'))
    for row, attr in tracks:
        d = _fresh_port_duel()
        fp = _duel_fingerprint(d)
        for t in (1, 2):
            cost = BW_UP_COST[t]
            # one gold short: refused, NOTHING changes
            assert bw_port_buy(d, row, cost - 1) == 0, (row, t)
            assert getattr(d, attr) == t - 1, (row, t)
            # exact price: spends exactly the price, bumps exactly one tier
            assert bw_port_buy(d, row, cost) == cost, (row, t)
            assert getattr(d, attr) == t, (row, t)
        # maxed: refused no matter the purse
        assert bw_port_buy(d, row, 10 ** 6) == 0, row
        assert getattr(d, attr) == BW_UP_TIERS - 1, row
        # no cross-talk and no side effects on the water
        for other_row, other in tracks:
            if other != attr:
                assert getattr(d, other) == 0, (row, other)
        assert _duel_fingerprint(d) == fp, row
        # buying a hull tier raises the ceiling, never heals (row HULL)
        if row == BW_PORT_ROW_HULL:
            assert d.player.hull == BW_HULL_MAX
    # unknown rows are refused
    d = _fresh_port_duel()
    for row in (-1, BW_PORT_ROWS, 99):
        assert bw_port_buy(d, row, 10 ** 6) == 0, row
    # repair: exact ceil pricing for EVERY reachable hull at every tier
    for tier in range(BW_UP_TIERS):
        hull_max = BW_UP_HULL_OF[tier]
        for hull in range(1, hull_max + 1):
            d = _fresh_port_duel(hull=hull, tiers=(tier, 0, 0))
            missing = hull_max - hull
            cost = (missing + BW_REPAIR_PER_GOLD - 1) // BW_REPAIR_PER_GOLD
            assert bw_repair_cost(d) == cost, (tier, hull)
            if cost == 0:
                assert bw_port_buy(d, BW_PORT_ROW_REPAIR, 10 ** 6) == 0
                assert d.player.hull == hull_max
                continue
            assert bw_port_buy(d, BW_PORT_ROW_REPAIR, cost - 1) == 0
            assert d.player.hull == hull, (tier, hull)
            assert bw_port_buy(d, BW_PORT_ROW_REPAIR, cost) == cost
            assert d.player.hull == hull_max, (tier, hull)
            assert bw_repair_cost(d) == 0
    print('port ledger: every buy spends exactly the price; one gold '
          'short / max tier / sound hull / unknown row all refused '
          'changing nothing; repair = ceil(missing/'
          f'{BW_REPAIR_PER_GOLD}) exact for every hull value at every '
          'tier; no cross-talk; the water untouched')


def check_upgraded_duels():
    maxed = BW_UP_TIERS - 1
    for tiers in ((maxed, 0, 0), (0, maxed, 0), (0, 0, maxed),
                  (maxed, maxed, maxed)):
        for seed in range(8):
            d = Duel()
            bw_duel_init(d, seed)
            d.up_hull, d.up_cannon, d.up_sail = tiers
            d.player.hull = BW_UP_HULL_OF[tiers[0]]
            for _ in range(3600):
                bw_duel_step(d, bw_policy(d.player, d.enemy))
                if d.over != BW_DUEL_RUNNING:
                    break
            assert d.over == BW_DUEL_ENEMY_SUNK, (tiers, seed, d.over)
            assert d.player.hull > 0, (tiers, seed)
        idle = Inputs()
        for seed in range(8):
            d = Duel()
            bw_duel_init(d, seed)
            d.up_hull, d.up_cannon, d.up_sail = tiers
            d.player.hull = BW_UP_HULL_OF[tiers[0]]
            for _ in range(10800):
                bw_duel_step(d, idle)
                if d.over != BW_DUEL_RUNNING:
                    break
            assert d.over == BW_DUEL_PLAYER_SUNK, (tiers, seed, d.over)
    print('upgraded duels: each single track maxed AND all three maxed — '
          'the policy player still wins (8 seeds each) and the idle '
          'player is still sunk (upgrades delay the loss, never annul it)')


def check_upgraded_containment():
    d = Duel()
    bw_duel_init(d, 0xC0FFEE & U32)
    maxed = BW_UP_TIERS - 1
    d.up_hull, d.up_cannon, d.up_sail = maxed, maxed, maxed
    speed_cap = (BW_SPEED_OF[BW_TRIM_FULL] + BW_UP_SPEED_OF[maxed]
                 + bw_wind_push(d))      # slice 6: tailwind headroom
    hull_cap = BW_UP_HULL_OF[maxed]
    frames = 10000
    for f in range(frames):
        h = bw_hash(0x5EA4, f)
        inp = Inputs(turn=(h & 3) - 1 if (h & 3) < 3 else 0,
                     trim_delta=((h >> 2) & 3) - 1 if ((h >> 2) & 3) < 3
                     else 0,
                     fire_l=(h >> 4) & 1, fire_r=(h >> 5) & 1)
        d.over = BW_DUEL_RUNNING          # adversarial: never let it end
        d.player.hull = hull_cap
        d.enemy.hull = BW_HULL_MAX
        bw_duel_step(d, inp)
        for s in (d.player, d.enemy):
            assert BW_SEA_X_MIN <= s.x <= BW_SEA_X_MAX, f
            assert BW_SEA_Y_MIN <= s.y <= BW_SEA_Y_MAX, f
            assert 0 <= s.heading < BW_CIRCLE, f
            assert BW_TRIM_BATTLE <= s.trim <= BW_TRIM_FULL, f
        assert 0 < d.player.speed <= speed_cap, f
        assert 0 < d.enemy.speed <= BW_SPEED_OF[BW_TRIM_FULL] \
            + bw_wind_push(d), f
        for ball in d.balls:
            if ball.live:
                assert BW_SEA_X_MIN <= ball.x <= BW_SEA_X_MAX, f
                assert BW_SEA_Y_MIN <= ball.y <= BW_SEA_Y_MAX, f
    print(f'upgraded containment: {frames} adversarial frames at max '
          'tiers — ships and balls never leave the sea, the raised '
          f'speed cap ({speed_cap}) holds, the enemy keeps the stock one')


def check_maw_stalks():
    # The whole telegraph contract, exact: for 16 policy-win waters the
    # Maw stays down for exactly BW_MAW_PATIENCE frames from the sink,
    # rises AT the wreck, shadows for exactly BW_MAW_SHADOW_FRAMES,
    # surfaces OUTSIDE the pier sanctuary for exactly
    # BW_MAW_SURFACE_FRAMES, lunges, and a bait player who sails at the
    # dark water takes EXACTLY ONE bite of exactly BW_MAW_BITE — then
    # the water holds its breath again (down, re-armed).
    # Tunable-relations rails ride here too: the shadow is slower than
    # full sail (escapable BY DESIGN), the lunge is faster than even
    # sails-III (the dodge is the turn, not the sprint), and the Maw's
    # crates fit the pool and the hold next to the wreck's.
    assert BW_MAW_SHADOW_SPEED < (BW_SPEED_OF[BW_TRIM_FULL] * 181) >> 8
    assert BW_MAW_LUNGE_SPEED > BW_SPEED_OF[BW_TRIM_FULL] \
        + BW_UP_SPEED_OF[BW_UP_TIERS - 1]
    assert BW_MAW_LOOT_VALUE > BW_LOOT_VALUE          # monster drops more
    assert BW_MAW_LOOT_DROPS <= len(BW_LOOT_ANGLE)    # ring table bound
    assert BW_LOOT_DROPS + BW_MAW_LOOT_DROPS <= BW_MAX_LOOT
    assert BW_LOOT_DROPS + BW_MAW_LOOT_DROPS <= BW_HOLD_CAP
    assert BW_MAW_BITE < BW_HULL_MAX                  # one bite never
    cycle = (BW_MAW_SHADOW_FRAMES + BW_MAW_SURFACE_FRAMES  # sinks a sound
             + BW_MAW_LUNGE_FRAMES + BW_MAW_RESTIR)        # sloop
    for seed in range(16):
        d = _win_state(seed)
        sink = d.frame - 1               # over was set before frame++
        assert d.maw.wake == sink + BW_MAW_PATIENCE, seed
        assert d.maw.state == BW_MAW_DOWN and d.maw.stirs == 0, seed
        d.player.hull = BW_HULL_MAX      # a sound sloop takes the bite
        first_stir = None
        prev, since = BW_MAW_DOWN, None
        bites = 0
        last_hull = d.player.hull
        for _ in range(BW_MAW_PATIENCE + 4 * cycle):
            bw_salvage_step(d, bw_maw_bait_policy(d))
            m = d.maw
            now = d.frame - 1            # frame of the step just taken
            if m.state != prev:
                # every transition's dwell time is EXACT (give-ups too)
                if prev == BW_MAW_SHADOW:
                    assert now - since == BW_MAW_SHADOW_FRAMES, (seed, now)
                    assert m.state in (BW_MAW_SURFACE, BW_MAW_DOWN), seed
                elif prev == BW_MAW_SURFACE:
                    assert now - since == BW_MAW_SURFACE_FRAMES, (seed, now)
                    assert m.state in (BW_MAW_LUNGE, BW_MAW_DOWN), seed
                elif prev == BW_MAW_LUNGE:
                    assert now - since <= BW_MAW_LUNGE_FRAMES, (seed, now)
                    assert m.state == BW_MAW_DOWN, seed
                if m.state == BW_MAW_SHADOW and first_stir is None:
                    first_stir = now
                prev, since = m.state, now
            if d.player.hull != last_hull:
                assert last_hull - d.player.hull == BW_MAW_BITE, seed
                assert m.bit == 1 and m.state == BW_MAW_DOWN, seed
                bites += 1
                last_hull = d.player.hull
            if m.state == BW_MAW_SURFACE:
                assert bw_chebyshev(m.x, m.y, BW_PIER_X, BW_PIER_Y) \
                    >= BW_MAW_HARBOR, seed
            if bites:
                break
        assert bites == 1, (seed, bites)
        assert d.over == BW_DUEL_ENEMY_SUNK, seed    # bitten, not sunk
        assert first_stir == sink + BW_MAW_PATIENCE, (seed, first_stir)
        m = d.maw
        assert m.state == BW_MAW_DOWN and m.bit == 1, seed
        assert m.wake == (d.frame - 1) + BW_MAW_RESTIR, seed
        assert 1 <= m.stirs <= 4 and m.slain == 0, seed
    print(f'the Maw stalks: 16 wrecks — down exactly {BW_MAW_PATIENCE} '
          f'frames from the sink, every shadow exactly '
          f'{BW_MAW_SHADOW_FRAMES} frames, every surfacing (never in '
          f'the sanctuary) exactly {BW_MAW_SURFACE_FRAMES}, then ONE '
          f'bite of exactly {BW_MAW_BITE} hull and down again; '
          'shadow < full sail < lunge, monster crates richer')


def check_maw_sanctuary():
    # The pier is sanctuary and the port stays reachable: a player who
    # LIVES at the berth is never surfaced on, never lunged at, never
    # bitten — the shadow may prowl the harbor water (the dark is
    # atmosphere; the ring keeps the JAWS out), but it always gives up
    # and sounds, the stir clock keeps running, and the pier still
    # banks mid-prowl.
    d = Duel()
    bw_duel_init(d, 0xBEA7)
    d.enemy.hull = 0
    d.over = BW_DUEL_ENEMY_SUNK
    bw_loot_spawn(d)
    d.maw.wake = d.frame + BW_MAW_PATIENCE
    idle = Inputs()
    banked_mid_prowl = 0
    for f in range(BW_MAW_PATIENCE + 6 * (BW_MAW_SHADOW_FRAMES
                                          + BW_MAW_RESTIR)):
        bw_salvage_step(d, idle)
        d.player.x, d.player.y = BW_PIER_X, BW_PIER_Y   # parked at the berth
        m = d.maw
        assert m.state in (BW_MAW_DOWN, BW_MAW_SHADOW), f
        assert m.bit == 0, f
        if m.state == BW_MAW_SHADOW and banked_mid_prowl == 0:
            d.hold, d.hold_gold = 1, BW_LOOT_VALUE
            banked_mid_prowl = bw_dock_step(d)
            assert banked_mid_prowl == BW_LOOT_VALUE, f
    assert d.player.hull == BW_HULL_MAX      # never bitten
    assert d.maw.stirs >= 3, d.maw.stirs     # it keeps trying
    print(f'sanctuary: a berthed player is never surfaced on, lunged '
          f'at, or bitten across {d.maw.stirs} prowls (the jaws stay '
          f'out of the {BW_MAW_HARBOR // BW_ONE} px harbor ring), and '
          'the pier still banks mid-prowl')


def check_maw_slain():
    # The counterattack converges: from 8 policy-win waters (hull
    # repaired — the smart captain refits before picking THIS fight),
    # the hunter policy flees the shadow, rakes the risen Maw, and
    # slays it; the carcass drops exactly BW_MAW_LOOT_DROPS crates
    # worth BW_MAW_LOOT_VALUE each on the ring, the salvage brain then
    # banks wreck + monster crates together, and a slain Maw NEVER
    # stirs again.
    full = (BW_LOOT_DROPS * BW_LOOT_VALUE
            + BW_MAW_LOOT_DROPS * BW_MAW_LOOT_VALUE)
    worst_frames = 0
    worst_hull = BW_HULL_MAX
    for seed in range(8):
        d = _win_state(seed)
        d.player.hull = BW_HULL_MAX
        banked = 0
        for _ in range(12000):
            bw_salvage_step(d, bw_maw_hunt_policy(d))
            banked += bw_dock_step(d)
            if banked >= full:           # a mid-hunt pass over the pier
                break                    # may bank a stray crate early
        assert d.over == BW_DUEL_ENEMY_SUNK, (seed, d.over)
        assert d.maw.slain == 1 and d.maw.hull == 0, seed
        assert d.maw.state == BW_MAW_DOWN, seed
        assert banked == full, (seed, banked)
        assert not any(c.live for c in d.loot), seed
        assert d.hold == 0 and d.hold_gold == 0, seed
        worst_frames = max(worst_frames, d.frame)
        worst_hull = min(worst_hull, d.player.hull)
        stirs = d.maw.stirs
        for _ in range(BW_MAW_PATIENCE + BW_MAW_RESTIR + 100):
            bw_salvage_step(d, bw_maw_hunt_policy(d))
        assert d.maw.stirs == stirs, seed        # dead is dead
        assert d.maw.state == BW_MAW_DOWN, seed
    print(f'the Maw slain: 8 hunts — the hunter policy survives '
          f'(worst hull {worst_hull}), slays it, and banks the full '
          f'{full}g (wreck + richer monster crates) by frame '
          f'{worst_frames}; a slain Maw never stirs again')


def check_wind_tables():
    # Calm identity (the pin-carry rule): the calm push is zero and the
    # wind term vanishes at EVERY point of sail and trim, so a calm
    # water is bit-identical to the slice-5 sim.
    assert BW_WIND_PUSH_OF[BW_WIND_CALM] == 0
    for trim in range(3):
        for brad in range(256):
            assert (bw_cos(brad) * BW_WIND_PUSH_OF[BW_WIND_CALM]
                    * BW_WIND_CANVAS_OF[trim]) >> 10 == 0
    # The committed anchors are ALL calm: every slice-2..5 recorded
    # route and emulator pin carries verbatim (118 = the kinematics/
    # idle-loss anchor, 126 = the duel-win anchor, 127 = the salvage/
    # port anchor, 116/117/119 = the maw recorder's scan prefix + its
    # anchor, 558/728 = the carry-duel waters proofs 6/8/9/10 pin
    # positions in). A wind salt or table retune fails HERE, loudly,
    # before it silently re-times a committed route.
    for seed in (116, 117, 118, 119, 126, 127, 558, 728):
        lvl = BW_WIND_LEVEL_OF[bw_hash(seed, BW_WIND_SALT) & 3]
        assert lvl == BW_WIND_CALM, seed
    # weather and canvas tables are sane and monotone
    assert BW_WIND_PUSH_OF[BW_WIND_BREEZE] > 0
    assert BW_WIND_PUSH_OF[BW_WIND_GALE] > BW_WIND_PUSH_OF[BW_WIND_BREEZE]
    for t in (1, 2):
        assert BW_WIND_CANVAS_OF[t] > BW_WIND_CANVAS_OF[t - 1]
    assert all(0 <= lv < BW_WIND_STATES for lv in BW_WIND_LEVEL_OF)
    assert BW_WIND_LEVEL_OF[0] == BW_WIND_CALM      # calm waters exist
    assert BW_WIND_GALE in BW_WIND_LEVEL_OF         # so do gales
    # Escapability rails survive the weather: a gale-beating full sail
    # still outruns the shadow on its worst (diagonal) axis, and a
    # gale-running battle-sail scooper still does NOT — the slice-5
    # counterplay is weather-proof in BOTH directions.
    gale = BW_WIND_PUSH_OF[BW_WIND_GALE]
    worst_full = ((BW_SPEED_OF[BW_TRIM_FULL] - gale) * 181) >> 8
    assert worst_full > BW_MAW_SHADOW_SPEED, worst_full
    best_battle = BW_SPEED_OF[BW_TRIM_BATTLE] \
        + ((256 * gale * BW_WIND_CANVAS_OF[BW_TRIM_BATTLE]) >> 10)
    assert best_battle < BW_MAW_SHADOW_SPEED, best_battle
    # the ship ALWAYS makes way: the worst headwind never zeroes a trim
    for trim in range(3):
        drag = (256 * gale * BW_WIND_CANVAS_OF[trim]) >> 10
        assert BW_SPEED_OF[trim] - drag > 0, trim
    print('wind tables: calm term identically zero (all points of sail), '
          'the 8 committed anchor seeds ALL calm (pin-carry rule), '
          f'gale-beating full sail {worst_full} still outruns the shadow '
          f'({BW_MAW_SHADOW_SPEED}), gale-running battle sail '
          f'{best_battle} still does not, every trim keeps way on')


def check_wind_pointofsail():
    # The vector rotates exactly one heading unit per 2^SHIFT frames
    # off the seeded base, wrapping the circle.
    d = Duel()
    bw_duel_init(d, 0)
    for f in (0, 7, 8, 9, 4095, 4096, 8191, 8192):
        d.frame = f
        want = (d.wind_base + (f >> BW_WIND_TURN_SHIFT)) & (BW_CIRCLE - 1)
        assert bw_wind_heading(d) == want, f
    # Exact point-of-sail speed deltas: a lone ship stepped to a settled
    # speed at 8 relative bearings, every trim, breeze and gale — the
    # settled speed IS trim target + (cos(rel) * push * canvas) >> 10,
    # downwind/upwind deltas are exact mirror images (cos symmetry),
    # and abeam is exactly neutral.
    idle = Inputs()
    for level in (BW_WIND_BREEZE, BW_WIND_GALE):
        push = BW_WIND_PUSH_OF[level]
        for trim in range(3):
            deltas = {}
            for rel in (0, 128, 256, 384, 512, 640, 768, 896):
                s = Ship()
                s.x, s.y = BW_PLAYER_START_X, BW_PLAYER_START_Y
                s.heading = rel          # wind toward 0: rel IS the angle
                s.trim = trim
                s.speed = BW_SPEED_OF[trim]
                s.hull = BW_HULL_MAX
                s.reload_l = s.reload_r = 0
                for _ in range(64):      # ease well past the worst delta
                    bw_ship_step(s, idle, 0, 0, push)
                delta = (bw_cos(rel >> 2) * push
                         * BW_WIND_CANVAS_OF[trim]) >> 10
                assert s.speed == BW_SPEED_OF[trim] + delta, \
                    (level, trim, rel, s.speed)
                deltas[rel] = delta
            assert deltas[0] == (push * BW_WIND_CANVAS_OF[trim]) >> 2
            assert deltas[256] == 0 and deltas[768] == 0    # abeam neutral
            # dead runs and dead beats are exact mirror images; at the
            # off-cardinal bearings the arithmetic shift FLOORS, so
            # beating costs at most ONE unit more than running gains —
            # the sea is cruel by a single unit, never generous
            assert deltas[0] == -deltas[512]
            for rel in (0, 128, 256, 384):
                back = deltas[(rel + 512) & 1023]
                assert -deltas[rel] - 1 <= back <= -deltas[rel], rel
    print('wind point of sail: rotation exactly 1 unit per '
          f'{1 << BW_WIND_TURN_SHIFT} frames; settled speeds exact at 8 '
          'bearings x 3 trims x breeze+gale; run/beat mirror-imaged at '
          'the cardinals (floor-crueller by <= 1 off them); abeam '
          'exactly neutral')


def check_wind_duels():
    # Convergence survives the weather, both ways: with the weather
    # FORCED to breeze and to gale (whatever the seed hashed to), the
    # idle player is still sunk and the beam-holding policy player
    # still wins — wind retunes the dance, never the outcomes.
    idle = Inputs()
    for level in (BW_WIND_BREEZE, BW_WIND_GALE):
        for seed in range(8):
            d = Duel()
            bw_duel_init(d, seed)
            d.wind_level = level
            for _ in range(10800):
                bw_duel_step(d, idle)
                if d.over != BW_DUEL_RUNNING:
                    break
            assert d.over == BW_DUEL_PLAYER_SUNK, (level, seed, d.over)
        for seed in range(8):
            d = Duel()
            bw_duel_init(d, seed)
            d.wind_level = level
            for _ in range(3600):
                bw_duel_step(d, bw_policy(d.player, d.enemy))
                if d.over != BW_DUEL_RUNNING:
                    break
            assert d.over == BW_DUEL_ENEMY_SUNK, (level, seed, d.over)
            assert d.player.hull > 0, (level, seed)
    print('wind duels: forced breeze AND gale (8 seeds each) — the idle '
          'player is still sunk, the policy player still wins surviving')


def check_wind_containment():
    # The gale-armed salvage water stays contained: adversarial input
    # with the weather forced to gale, the Maw stalking, and the fire
    # keys live — ship, crates, and Maw stay in the sea, the speed
    # never exceeds the gale-filled trim ceiling or stalls, and a
    # slain Maw stays down (the slice-5 sweep, re-armed under weather).
    d = Duel()
    bw_duel_init(d, 0x6A1E)
    d.wind_level = BW_WIND_GALE
    d.enemy.hull = 0
    d.over = BW_DUEL_ENEMY_SUNK
    bw_loot_spawn(d)
    d.maw.wake = d.frame + BW_MAW_PATIENCE
    cap = BW_SPEED_OF[BW_TRIM_FULL] + BW_WIND_PUSH_OF[BW_WIND_GALE]
    frames = 8000
    stirs_after_slain = None
    for f in range(frames):
        h = bw_hash(0x6A1E, f)
        inp = Inputs(turn=(h & 3) - 1 if (h & 3) < 3 else 0,
                     trim_delta=((h >> 2) & 3) - 1 if ((h >> 2) & 3) < 3
                     else 0,
                     fire_l=(h >> 4) & 1, fire_r=(h >> 5) & 1)
        d.player.hull = BW_HULL_MAX      # adversarial: never let it end
        d.over = BW_DUEL_ENEMY_SUNK
        bw_salvage_step(d, inp)
        bw_dock_step(d)
        s = d.player
        assert BW_SEA_X_MIN <= s.x <= BW_SEA_X_MAX, f
        assert BW_SEA_Y_MIN <= s.y <= BW_SEA_Y_MAX, f
        assert 0 < s.speed <= cap, (f, s.speed)
        for c in d.loot:
            if c.live:
                assert BW_SEA_X_MIN <= c.x <= BW_SEA_X_MAX, f
                assert BW_SEA_Y_MIN <= c.y <= BW_SEA_Y_MAX, f
        m = d.maw
        assert 0 <= m.hull <= BW_MAW_HULL, f
        if m.state != BW_MAW_DOWN:
            assert BW_SEA_X_MIN <= m.x <= BW_SEA_X_MAX, f
            assert BW_SEA_Y_MIN <= m.y <= BW_SEA_Y_MAX, f
        if m.slain:
            assert m.state == BW_MAW_DOWN, f
            if stirs_after_slain is None:
                stirs_after_slain = m.stirs
            assert m.stirs == stirs_after_slain, f
    print(f'wind containment: {frames} adversarial gale frames (fire '
          'keys live, the Maw stalking) — ship, crates, and Maw stay in '
          f'the sea, speed in (0, {cap}], a slain Maw stays down')


def check_band_tables():
    # Band-0 identity (the slice-7 pin-carry rule): every band-0 table
    # row IS the legacy constant, and bw_water_init(d, seed, 0) is
    # STATE-IDENTICAL to bw_duel_init(d, seed) — so every slice-2..6
    # recorded route and emulator pin carries verbatim. A band-table
    # retune fails HERE, loudly, before it silently re-times a story.
    assert BW_BAND_EHULL_OF[0] == BW_HULL_MAX
    assert BW_BAND_ERELOAD_OF[0] == BW_RELOAD_ENEMY
    assert BW_BAND_LOOT_VALUE_OF[0] == BW_LOOT_VALUE
    assert BW_BAND_MAW_VALUE_OF[0] == BW_MAW_LOOT_VALUE
    assert BW_BAND_REEF_OF[0] == 0

    def state(d):
        p, e, m = d.player, d.enemy, d.maw
        return (p.x, p.y, p.heading, p.trim, p.speed, p.hull,
                p.reload_l, p.reload_r,
                e.x, e.y, e.heading, e.trim, e.speed, e.hull,
                e.reload_l, e.reload_r,
                tuple(b.live for b in d.balls),
                tuple(c.live for c in d.loot),
                (m.x, m.y, m.vx, m.vy, m.state, m.hull, m.timer, m.wake,
                 m.stirs, m.slain, m.bit),
                tuple((r.x, r.y, r.live, r.scraped) for r in d.reefs),
                d.hold, d.hold_gold, d.up_hull, d.up_cannon, d.up_sail,
                d.wind_level, d.wind_base, d.band, d.groundings,
                d.frame, d.over)

    for seed in range(512):
        a, b = Duel(), Duel()
        bw_duel_init(a, seed)
        bw_water_init(b, seed, 0)
        assert state(a) == state(b), seed
    # deeper is harder and richer, monotone: heavier hulls, faster
    # crews, dearer crates (the doc's ~5g band 0 -> ~25g band 2 shape),
    # monster salvage richer than the wreck's in every band, more rocks
    for band in (1, 2):
        assert BW_BAND_EHULL_OF[band] > BW_BAND_EHULL_OF[band - 1]
        assert BW_BAND_ERELOAD_OF[band] < BW_BAND_ERELOAD_OF[band - 1]
        assert BW_BAND_LOOT_VALUE_OF[band] > BW_BAND_LOOT_VALUE_OF[band - 1]
        assert BW_BAND_MAW_VALUE_OF[band] > BW_BAND_MAW_VALUE_OF[band - 1]
        assert BW_BAND_REEF_OF[band] > BW_BAND_REEF_OF[band - 1]
        assert BW_BAND_MAW_VALUE_OF[band] > BW_BAND_LOOT_VALUE_OF[band]
    assert BW_BAND_LOOT_VALUE_OF[2] == 5 * BW_BAND_LOOT_VALUE_OF[0]
    assert all(n <= BW_MAX_REEFS for n in BW_BAND_REEF_OF)
    # the weather worsens one level per band, min-clamped at gale — and
    # NEVER exceeds the gale the slice-6 escape rails are proven at
    for seed in range(256):
        base = BW_WIND_LEVEL_OF[bw_hash(seed, BW_WIND_SALT) & 3]
        for band in range(BW_BANDS):
            d = Duel()
            bw_water_init(d, seed, band)
            want = min(BW_WIND_GALE, base + band)
            assert d.wind_level == want, (seed, band)
            assert d.wind_level <= BW_WIND_GALE
    # band clamps: no shallower water than home, no deeper than band 2
    lo, hi = Duel(), Duel()
    bw_water_init(lo, 7, -3)
    bw_water_init(hi, 7, 99)
    assert lo.band == 0 and hi.band == BW_BANDS - 1
    print('band tables: band-0 state-identical to bw_duel_init over 512 '
          'seeds (the slice-7 pin-carry rule), deeper monotonically '
          'harder + richer (crates 5g -> 25g, the doc shape), weather '
          'worsens one level per band min-clamped at the proven gale, '
          'band clamped to 0..2')


def check_reef_spawns():
    # Every water's rocks are pure f(seed): exact band counts, inside
    # the sea, and clear of BOTH sailing rails — the anchorage the
    # player starts on (BW_REEF_CLEAR) and the pier harbor
    # (BW_MAW_HARBOR: the sanctuary ring is charted water) — across
    # 4096 seeds x bands 1/2; band 0 lays none. Determinism: the same
    # seed lays the same rocks twice. (Rock-rock spread is best-effort
    # via the hash stream, deliberately NOT a rail.)
    shelf = 0
    for seed in range(4096):
        for band in (0, 1, 2):
            d = Duel()
            bw_water_init(d, seed, band)
            live = [r for r in d.reefs if r.live]
            assert len(live) == BW_BAND_REEF_OF[band], (seed, band)
            for r in live:
                assert BW_SEA_X_MIN <= r.x <= BW_SEA_X_MAX, (seed, band)
                assert BW_SEA_Y_MIN <= r.y <= BW_SEA_Y_MAX, (seed, band)
                assert bw_chebyshev(r.x, r.y, BW_PLAYER_START_X,
                                    BW_PLAYER_START_Y) >= BW_REEF_CLEAR, \
                    (seed, band)
                assert bw_chebyshev(r.x, r.y, BW_PIER_X,
                                    BW_PIER_Y) >= BW_MAW_HARBOR, (seed, band)
                assert not r.scraped
                if r.y == BW_REEF_SHELF_Y * BW_ONE:
                    shelf += 1
        d2 = Duel()
        bw_water_init(d2, seed, 2)
        assert [(r.x, r.y) for r in d2.reefs] \
            == [(r.x, r.y) for r in d.reefs], seed
    # the fallback shelf itself is provably clear of both rails
    for k in range(BW_MAX_REEFS):
        x = (BW_REEF_SHELF_X + BW_REEF_SHELF_STEP * k) * BW_ONE
        y = BW_REEF_SHELF_Y * BW_ONE
        assert BW_SEA_X_MIN <= x <= BW_SEA_X_MAX
        assert BW_SEA_Y_MIN <= y <= BW_SEA_Y_MAX
        assert bw_chebyshev(x, y, BW_PLAYER_START_X,
                            BW_PLAYER_START_Y) >= BW_REEF_CLEAR
        assert bw_chebyshev(x, y, BW_PIER_X, BW_PIER_Y) >= BW_MAW_HARBOR
    print('reef spawns: 4096 seeds x 3 bands — counts exact (0/3/5), '
          'every rock in the sea and clear of the anchorage '
          f'({BW_REEF_CLEAR >> 8} px) and the pier harbor '
          f'({BW_MAW_HARBOR >> 8} px), deterministic; the fallback '
          f'shelf holds both rails by construction ({shelf} shelf '
          'landings in the sweep)')


def check_reef_grounding():
    # The scrape contract, frame-exact: sailing onto a rock costs
    # exactly BW_REEF_DMG hull ONCE (the latch), sitting on it costs
    # nothing more, clearing it re-arms the latch, and a second contact
    # scrapes again; the groundings counter counts each scrape; the
    # ENEMY parked on the same rock is untouched (the rum-runners know
    # these waters); grounding a 1-hull sloop sinks it (over flips).
    d = Duel()
    bw_water_init(d, 11, 1)
    r = d.reefs[0]
    for other in d.reefs[1:]:            # isolate the rock under test
        other.live = 0                   #   (pure-state surgery, the
    idle = Inputs()                      #   containment checks' idiom)
    d.enemy.hull = 999                   # keep the duel running; park the
    d.enemy.x, d.enemy.y = r.x, r.y      #   enemy ON the rock, unharmed
    d.enemy.speed = 0
    ehull0 = d.enemy.hull
    # teleport the player just clear of the rock, bow dead north, and
    # freeze the enemy in place each frame (pure-state surgery, the
    # containment checks' idiom)
    d.player.x = r.x
    d.player.y = r.y + BW_REEF_RANGE + 4 * BW_ONE
    d.player.heading = 0
    d.player.trim = BW_TRIM_BATTLE
    d.player.speed = BW_SPEED_OF[BW_TRIM_BATTLE]
    d.player.hull = BW_HULL_MAX
    d.wind_level = BW_WIND_CALM          # kinematics under test, not wind
    scrapes = []
    for f in range(400):
        ex, ey = d.enemy.x, d.enemy.y
        hull_before = d.player.hull
        ground_before = d.groundings
        bw_duel_step(d, idle)            # due north: closes on the rock
        d.enemy.x, d.enemy.y = ex, ey    # re-park (the AI would sail off)
        d.enemy.speed = 0
        d.enemy.hull = ehull0
        if d.groundings != ground_before:
            scrapes.append((f, hull_before - d.player.hull))
        if d.player.y <= r.y:            # sailed past the rock: done
            break
    assert scrapes == [(scrapes[0][0], BW_REEF_DMG)], scrapes
    assert d.groundings == 1
    onto_frame = scrapes[0][0]
    # dwell ON the rock: the latch holds, no second toll
    d2 = Duel()
    bw_water_init(d2, 11, 1)
    r2 = d2.reefs[0]
    for other in d2.reefs[1:]:
        other.live = 0
    d2.enemy.hull = 999
    d2.enemy.x, d2.enemy.y = BW_SEA_X_MIN, BW_SEA_Y_MIN
    d2.player.x, d2.player.y = r2.x, r2.y
    d2.player.heading = 0
    d2.player.trim = BW_TRIM_BATTLE
    d2.player.speed = 0
    d2.player.hull = BW_HULL_MAX
    d2.wind_level = BW_WIND_CALM
    still = Inputs(turn=1)               # spin in place-ish over the rock
    d2.player.speed = 1
    for _ in range(120):
        ex, ey = d2.enemy.x, d2.enemy.y
        bw_duel_step(d2, still)
        d2.enemy.x, d2.enemy.y = ex, ey
        d2.enemy.speed = 0
        d2.enemy.hull = 999
        d2.player.x, d2.player.y = r2.x, r2.y   # hold it on the rock
        d2.player.speed = 1
    assert d2.groundings == 1
    assert d2.player.hull == BW_HULL_MAX - BW_REEF_DMG
    # clear the rock, come back: the latch re-arms, a second scrape
    d2.player.x = r2.x
    d2.player.y = r2.y + BW_REEF_RANGE + 2 * BW_ONE
    ex, ey = d2.enemy.x, d2.enemy.y
    bw_duel_step(d2, Inputs())           # one clear frame drops the latch
    d2.enemy.x, d2.enemy.y = ex, ey
    d2.enemy.hull = 999
    assert d2.groundings == 1 and not d2.reefs[0].scraped
    d2.player.x, d2.player.y = r2.x, r2.y
    bw_duel_step(d2, Inputs())
    assert d2.groundings == 2
    assert d2.player.hull == BW_HULL_MAX - 2 * BW_REEF_DMG
    # a grounding can sink you: 1 hull + a rock = the brine takes her
    d3 = Duel()
    bw_water_init(d3, 11, 1)
    for other in d3.reefs[1:]:
        other.live = 0
    d3.player.x, d3.player.y = d3.reefs[0].x, d3.reefs[0].y
    d3.player.hull = 1
    bw_duel_step(d3, Inputs())
    assert d3.player.hull == 0 and d3.over == BW_DUEL_PLAYER_SUNK
    # the parked enemy never grounded in run 1
    assert ehull0 == 999
    # and the scrape works in SALVAGE water too (the win state is live)
    d4 = Duel()
    bw_water_init(d4, 11, 1)
    d4.reefs[0].live = 0                 # the salvage scrape probes rock 1
    d4.reefs[2].live = 0
    d4.enemy.hull = 0
    d4.over = BW_DUEL_ENEMY_SUNK
    d4.player.x, d4.player.y = d4.reefs[1].x, d4.reefs[1].y
    d4.player.hull = BW_HULL_MAX
    bw_salvage_step(d4, Inputs())
    assert d4.player.hull == BW_HULL_MAX - BW_REEF_DMG
    assert d4.groundings == 1
    print(f'reef grounding: one scrape of exactly {BW_REEF_DMG} on '
          f'contact (frame {onto_frame} of the approach), the latch '
          'holds over a 120-frame dwell, clears off the rock and '
          'scrapes again on re-entry (groundings 1 -> 2), a 1-hull '
          'grounding SINKS, the parked enemy is untouched, and salvage '
          'water scrapes too')


def check_band_duels():
    # Convergence survives the deeps, both ways: in REAL band-1 and
    # band-2 waters (band tables + reefs + worsened weather, exactly as
    # bw_water_init deals them), the idle player is still sunk on every
    # probed seed, and the committed beam-holding policy brain still
    # wins every probed band-1 water surviving. Band 2 is deliberately
    # crueller: the same tier-0 brain wins 12 of the 16 probed waters —
    # and every one of the four it loses (seeds 0/3/6/10) FLIPS to a
    # win with the cannons-II battery aboard: the deeps are the
    # treadmill's purpose, priced in gold, not a wall.
    idle = Inputs()
    for band in (1, 2):
        for seed in range(8):
            d = Duel()
            bw_water_init(d, seed, band)
            for _ in range(10800):
                bw_duel_step(d, idle)
                if d.over != BW_DUEL_RUNNING:
                    break
            assert d.over == BW_DUEL_PLAYER_SUNK, (band, seed, d.over)
    for seed in range(8):
        d = Duel()
        bw_water_init(d, seed, 1)
        for _ in range(5400):
            bw_duel_step(d, bw_policy(d.player, d.enemy))
            if d.over != BW_DUEL_RUNNING:
                break
        assert d.over == BW_DUEL_ENEMY_SUNK, (seed, d.over)
        assert d.player.hull > 0, seed
    tier0 = {}
    for seed in range(16):
        d = Duel()
        bw_water_init(d, seed, 2)
        for _ in range(5400):
            bw_duel_step(d, bw_policy(d.player, d.enemy))
            if d.over != BW_DUEL_RUNNING:
                break
        tier0[seed] = d.over
    losses = sorted(s for s, o in tier0.items() if o != BW_DUEL_ENEMY_SUNK)
    assert losses == [0, 3, 6, 10], losses
    for seed in losses:
        d = Duel()
        bw_water_init(d, seed, 2)
        d.up_cannon = 1                  # the port's answer to the deeps
        for _ in range(5400):
            bw_duel_step(d, bw_policy(d.player, d.enemy))
            if d.over != BW_DUEL_RUNNING:
                break
        assert d.over == BW_DUEL_ENEMY_SUNK, seed
        assert d.player.hull > 0, seed
    print('band duels: idle player sunk in every probed band-1/2 water '
          '(8 seeds each); the tier-0 policy brain wins all 8 probed '
          'band-1 waters surviving; band 2 wins 12/16 at tier 0 and '
          'every loss (seeds 0/3/6/10) flips to a win with cannons II '
          '— the deeps are priced in gold, not walled')


def check_band_containment():
    # The deepest water stays contained: 8000 adversarial frames in a
    # forced band-2 water (5 rocks, gale-or-worse weather, the Maw
    # stalking, fire keys live) — ship, crates, Maw stay in the sea,
    # speed respects the weathered ceiling, groundings only ever climb
    # (and each costs exactly BW_REEF_DMG off an adversarially-refilled
    # hull), and a slain Maw stays down. The slice-6 sweep, re-armed on
    # the rocks.
    d = Duel()
    bw_water_init(d, 0x0007, 2)          # this water's adversarial drive
    #   provably crosses its rocks
    assert d.wind_level == BW_WIND_GALE  # band 2 never rolls milder
    d.enemy.hull = 0
    d.over = BW_DUEL_ENEMY_SUNK
    bw_loot_spawn(d)
    d.maw.wake = d.frame + BW_MAW_PATIENCE
    cap = BW_SPEED_OF[BW_TRIM_FULL] + BW_WIND_PUSH_OF[BW_WIND_GALE]
    frames = 8000
    stirs_after_slain = None
    groundings_prev = 0
    scraped_total = 0
    for f in range(frames):
        h = bw_hash(0x0007, f)
        inp = Inputs(turn=(h & 3) - 1 if (h & 3) < 3 else 0,
                     trim_delta=((h >> 2) & 3) - 1 if ((h >> 2) & 3) < 3
                     else 0,
                     fire_l=(h >> 4) & 1, fire_r=(h >> 5) & 1)
        d.player.hull = BW_HULL_MAX      # adversarial: never let it end
        d.over = BW_DUEL_ENEMY_SUNK
        bw_salvage_step(d, inp)
        bw_dock_step(d)
        s = d.player
        assert BW_SEA_X_MIN <= s.x <= BW_SEA_X_MAX, f
        assert BW_SEA_Y_MIN <= s.y <= BW_SEA_Y_MAX, f
        assert 0 < s.speed <= cap, (f, s.speed)
        assert d.groundings >= groundings_prev, f
        if d.groundings > groundings_prev:
            assert d.groundings == groundings_prev + 1, f
            scraped_total += 1
        groundings_prev = d.groundings
        for c in d.loot:
            if c.live:
                assert BW_SEA_X_MIN <= c.x <= BW_SEA_X_MAX, f
                assert BW_SEA_Y_MIN <= c.y <= BW_SEA_Y_MAX, f
        m = d.maw
        assert 0 <= m.hull <= BW_MAW_HULL, f
        if m.state != BW_MAW_DOWN:
            assert BW_SEA_X_MIN <= m.x <= BW_SEA_X_MAX, f
            assert BW_SEA_Y_MIN <= m.y <= BW_SEA_Y_MAX, f
        if m.slain:
            assert m.state == BW_MAW_DOWN, f
            if stirs_after_slain is None:
                stirs_after_slain = m.stirs
            assert m.stirs == stirs_after_slain, f
    print(f'band containment: {frames} adversarial frames in a forced '
          'band-2 gale water (5 rocks, fire keys live, the Maw '
          'stalking) — ship, crates, and Maw stay in the sea, speed in '
          f'(0, {cap}], groundings climb one scrape at a time '
          f'({scraped_total} real scrapes taken), a slain Maw stays down')
    assert scraped_total > 0             # the sweep provably found rock



def check_audio_tables():
    # The slice-8 sound tables hold their contracts: row 0 is the no-op
    # cue, every real cue has a length/volume/valid routing, exactly one
    # row (the dock chime) is two-note — and a two-note cue is square by
    # contract (a noise channel has no pitch to re-tune). The ambience
    # ladder climbs monotonically calm -> breeze -> gale -> Maw in both
    # pitch and volume (the weather must be HEARD to worsen), and the
    # accessors clamp out-of-range ids to row 0 (never index off the
    # table) — the same clamp main.c relies on.
    assert len(BW_AMB_ROWS) == BW_AMB_TIERS
    assert len(BW_CUE_ROWS) == BW_CUE_COUNT
    assert BW_CUE_ROWS[BW_CUE_NONE] == (0, 0, 0, 0, 0)
    two_note = []
    for cue in range(1, BW_CUE_COUNT):
        freq, length, duty, vol, freq2 = BW_CUE_ROWS[cue]
        assert freq > 0 and length > 0, cue
        assert 1 <= vol <= 127, cue
        assert duty == BW_CUE_ON_NOISE or 0 <= duty <= 7, cue
        if freq2:
            two_note.append(cue)
            assert duty != BW_CUE_ON_NOISE, cue   # square by contract
            assert freq2 != freq, cue             # TWO notes
    assert two_note == [BW_CUE_DOCK], two_note
    for a, b in zip(BW_AMB_ROWS, BW_AMB_ROWS[1:]):
        assert a[0] < b[0] and a[2] < b[2], (a, b)
    for row in BW_AMB_ROWS:
        assert 0 <= row[1] <= 7 and 1 <= row[2] <= 127, row
    for bad in (-1, BW_CUE_COUNT, BW_CUE_COUNT + 7):
        assert (bw_cue_freq(bad), bw_cue_len(bad), bw_cue_duty(bad),
                bw_cue_vol(bad), bw_cue_freq2(bad)) == (0, 0, 0, 0, 0), bad
    for bad in (-1, BW_AMB_TIERS, BW_AMB_TIERS + 7):
        assert (bw_amb_freq(bad), bw_amb_duty(bad),
                bw_amb_vol(bad)) == BW_AMB_ROWS[0], bad
    print(f'audio tables: {BW_CUE_COUNT - 1} cue rows valid (one two-note '
          f'chime, square by contract), {BW_AMB_TIERS} drone rows '
          'monotone in pitch and volume, accessors clamp')


def check_audio_tier():
    # bw_amb_tier exhaustively: the Maw overrides every weather; without
    # it the wind level IS the ladder (out-of-range clamped to its end).
    for wind in range(-1, 5):
        assert bw_amb_tier(1, wind) == BW_AMB_MAW, wind
    assert bw_amb_tier(0, BW_WIND_CALM) == BW_AMB_CALM
    assert bw_amb_tier(0, BW_WIND_BREEZE) == BW_AMB_BREEZE
    assert bw_amb_tier(0, BW_WIND_GALE) == BW_AMB_GALE
    assert bw_amb_tier(0, -1) == BW_AMB_CALM
    assert bw_amb_tier(0, 3) == BW_AMB_GALE
    print('ambience tier: exhaustive — the Maw overrides all weather, '
          'the wind level is the ladder, ends clamped')


def check_audio_duel_story():
    # The whole ship-fight cue ladder on the committed win anchor's
    # water (seed 126 — CALM, so the drone must hold ONE tier end to
    # end): both ships' batteries thump, rakes crack hulls and spend
    # themselves in hisses, the wreck cue fires on the sink step
    # exactly, the salvage brain's three scoops each thud, the pier
    # bank rings the chime ONCE — and the chime is provably two-note
    # (C5 for the front half of the cue, G5 for the back). No scrape,
    # no Maw cue, no sinking cue anywhere in a story that ends banked.
    glue = AudioGlue()
    d = Duel()
    bw_duel_init(d, 126)
    assert d.wind_level == BW_WIND_CALM  # the pin-carry anchor rule
    wins = maws = 0
    state = 1                            # STATE_DUEL
    counts = [0] * BW_CUE_COUNT
    player_thumps = enemy_thumps = 0
    wreck_step = sink_step = None
    dock_step = None
    chime_notes = []
    step = 0
    for _ in range(7200):
        snap = glue.snapshot(d, wins, maws)
        prev_state = state
        if state == 1:
            bw_duel_step(d, bw_policy(d.player, d.enemy))
            if d.over == BW_DUEL_ENEMY_SUNK:
                wins += 1
                state = 3                # STATE_SALVAGE
                sink_step = step
        else:
            bw_salvage_step(d, bw_salvage_policy(d))
        banked = bw_dock_step(d)
        fired_p = (d.player.reload_l > snap[9]
                   or d.player.reload_r > snap[10])
        fired_e = (d.enemy.reload_l > snap[11]
                   or d.enemy.reload_r > snap[12])
        cue = glue.frame(snap, d, wins, maws, banked, state, prev_state,
                         step, 2)
        counts[cue] += 1
        if cue == BW_CUE_CANNON:
            player_thumps += 1 if fired_p else 0
            enemy_thumps += 1 if fired_e else 0
        if cue == BW_CUE_WRECK:
            wreck_step = step
        if dock_step is not None and step <= dock_step + 30:
            chime_notes.append(glue.cue_freq_now)
        if cue == BW_CUE_DOCK:
            dock_step = step
            chime_notes.append(glue.cue_freq_now)
        assert glue.amb_tier == BW_AMB_CALM and glue.amb_flips == 1, step
        step += 1
        if dock_step is not None and step > dock_step + 30:
            break
    assert counts[BW_CUE_CANNON] > 0
    assert player_thumps > 0 and enemy_thumps > 0
    assert counts[BW_CUE_CRACK] > 0 and counts[BW_CUE_SPLASH] > 0
    assert counts[BW_CUE_WRECK] == 1 and wreck_step == sink_step
    assert counts[BW_CUE_SCOOP] == BW_LOOT_DROPS
    assert counts[BW_CUE_DOCK] == 1 and dock_step is not None
    for c in (BW_CUE_SCRAPE, BW_CUE_MAWRISE, BW_CUE_BITE, BW_CUE_SUNK):
        assert counts[c] == 0, c
    assert glue.cues == sum(counts[1:])
    # the two-note contract, frame-exact: len 28 -> 14 frames of C5
    # (the fire frame + 13 countdowns), 14 of G5, then the channel
    # closes (freq 0 from the 29th frame on)
    length = bw_cue_len(BW_CUE_DOCK)
    front = length - length // 2
    want = ([bw_cue_freq(BW_CUE_DOCK)] * front
            + [bw_cue_freq2(BW_CUE_DOCK)] * (length // 2)
            + [0] * (31 - length))
    assert chime_notes == want, chime_notes
    print(f'audio, the fight heard (seed 126): {counts[BW_CUE_CANNON]} '
          f'thumps (both ships), {counts[BW_CUE_CRACK]} cracks, '
          f'{counts[BW_CUE_SPLASH]} splashes, wreck on the sink step, '
          f'{counts[BW_CUE_SCOOP]} scoops, ONE two-note chime '
          f'({bw_cue_freq(BW_CUE_DOCK)} -> {bw_cue_freq2(BW_CUE_DOCK)} Hz '
          'frame-exact), drone one CALM tier end to end')


def check_audio_maw_story():
    # The Maw's voice on the bait pattern (check_maw_stalks' water):
    # the drone flips CALM -> MAW the frame the shadow stirs (hearing
    # the telegraph), MAWRISE fires the frame it surfaces, the BITE
    # cue fires on the bite frame (outranking the same frame's hull
    # crack), and the water going quiet flips the drone home. Then the
    # sinking rattle: an idle band-0 player is raked down — the SUNK
    # cue fires once on the card frame and the drone STOPS (dying
    # silences the sea).
    glue = AudioGlue()
    d = _win_state(0)
    d.player.hull = BW_HULL_MAX
    wins, maws = 1, 0
    counts = [0] * BW_CUE_COUNT
    rise_ok = bite_ok = flip_up = flip_home = False
    step = 0
    for _ in range(BW_MAW_PATIENCE + 2000):
        snap = glue.snapshot(d, wins, maws)
        prev_tier_on = (glue.amb_on, glue.amb_tier)
        bw_salvage_step(d, bw_maw_bait_policy(d))
        banked = bw_dock_step(d)
        cue = glue.frame(snap, d, wins, maws, banked, 3, 3, step, 2)
        counts[cue] += 1
        if cue == BW_CUE_MAWRISE:
            rise_ok = (d.maw.state == BW_MAW_SURFACE
                       and snap[3] == BW_MAW_SHADOW)
        if cue == BW_CUE_BITE:
            bite_ok = (snap[0] - d.player.hull == BW_MAW_BITE)
        if (prev_tier_on[1] != BW_AMB_MAW and glue.amb_tier == BW_AMB_MAW):
            flip_up = d.maw.state != BW_MAW_DOWN
        if (prev_tier_on[1] == BW_AMB_MAW and glue.amb_tier != BW_AMB_MAW):
            flip_home = d.maw.state == BW_MAW_DOWN
        step += 1
        if counts[BW_CUE_BITE]:
            break
    assert counts[BW_CUE_MAWRISE] >= 1 and rise_ok
    assert counts[BW_CUE_BITE] == 1 and bite_ok
    assert flip_up and flip_home is not False
    assert counts[BW_CUE_SUNK] == 0 and counts[BW_CUE_DOCK] == 0

    # the sinking rattle (fresh idle water, the proof-3 shape)
    glue = AudioGlue()
    d = Duel()
    bw_duel_init(d, 118)                 # the committed loss anchor
    wins = maws = 0
    state, idle = 1, Inputs()
    sunk_at = drone_at_card = None
    step = 0
    for _ in range(3600):
        snap = glue.snapshot(d, wins, maws)
        prev_state = state
        if state == 1:
            bw_duel_step(d, idle)
            if d.over == BW_DUEL_PLAYER_SUNK:
                state = 2                # STATE_SUNK: the card
        banked = bw_dock_step(d) if state != 2 else 0
        cue = glue.frame(snap, d, wins, maws, banked, state, prev_state,
                         step, 2)
        if cue == BW_CUE_SUNK:
            sunk_at = step
        if sunk_at is not None and step == sunk_at + 2:
            drone_at_card = glue.words()[4]
            break
        step += 1
    assert sunk_at is not None
    assert drone_at_card == 0            # dying silences the sea
    print('audio, the Maw heard then the rattle: drone CALM -> MAW on '
          'the stir and home on the quiet, MAWRISE on the surface '
          'frame, ONE bite cue outranking its own hull crack; the '
          'idle loss fires the SUNK rattle and the drone dies with '
          'the ship')


def main():
    check_sine_table()
    check_spawns()
    check_idle_player_sunk()
    check_policy_player_wins()
    check_containment()
    check_loot_drop()
    check_salvage_banks()
    check_hold_cap()
    check_dock()
    check_salvage_containment()
    check_upgrade_tables()
    check_port_buy()
    check_upgraded_duels()
    check_upgraded_containment()
    check_maw_stalks()
    check_maw_sanctuary()
    check_maw_slain()
    check_wind_tables()
    check_wind_pointofsail()
    check_wind_duels()
    check_wind_containment()
    check_band_tables()
    check_reef_spawns()
    check_reef_grounding()
    check_band_duels()
    check_band_containment()
    check_audio_tables()
    check_audio_tier()
    check_audio_duel_story()
    check_audio_maw_story()
    print('check-brine: ALL GREEN')
    return 0


if __name__ == '__main__':
    sys.exit(main())
