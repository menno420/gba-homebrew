#!/usr/bin/env python3
"""Gloamline host proof (stdlib-only, <2s) — arc slice 3 skeleton stub.

The check-cave.py sibling for Gloamline: a line-for-line Python mirror of
the pure simulation layer in games/gloamline-nds/source/gl_sim.c
(gl_hash / gl_spawn_of_night / gl_player_step / gl_shambler_step /
gl_chebyshev / gl_contact), proving the skeleton's meaningful invariants
for EVERY reachable input instead of the handful a replay touches:

  1. spawn schedule — for seeds 0..255 x nights 1..8: the spawn is a pure
     deterministic function (recompute-equal), lands ON the fence
     perimeter, inside the arena, and NEVER inside the safe radius of the
     player's night-start position;
  2. chase convergence — from every spawn (seeds 0..63 x nights 1..4), an
     idle player is reached: the Chebyshev distance is monotonically
     non-increasing under gl_shambler_step and contact happens within 400
     frames (the stagger hash can only delay, never stall);
  3. arena containment — under 20,000 frames of hash-driven adversarial
     input the player never leaves the arena, and the chasing Shambler
     never does either.

MIRROR RULE (keep in lockstep): every function and constant below mirrors
games/gloamline-nds/source/gl_sim.c. Any change to the C MUST land here in
the same PR — same discipline as tools/check-cave.py <-> lumen-drift.

Usage: python3 tools/check-gloam.py           # exit 0 = green
"""

import sys

# --- constants (mirror gl_sim.h; fixed point 8.8) ---------------------------
GL_ONE = 256
GL_ARENA_X_MIN = 16 * GL_ONE
GL_ARENA_X_MAX = 239 * GL_ONE
GL_ARENA_Y_MIN = 32 * GL_ONE
GL_ARENA_Y_MAX = 175 * GL_ONE
GL_PLAYER_START_X = 128 * GL_ONE
GL_PLAYER_START_Y = 104 * GL_ONE
GL_PLAYER_SPEED = 384
GL_PLAYER_DIAG = 271
GL_SHAMBLER_SPEED = 192
GL_SHAMBLER_DIAG = 135
GL_AXIS_DEADZONE = 128
GL_CONTACT_RANGE = 10 * GL_ONE
GL_SAFE_RADIUS = 64 * GL_ONE

U32 = 0xFFFFFFFF


def gl_hash(a, b):
    """Mirror of gl_hash()."""
    h = ((a * 0x9E3779B9) & U32) ^ ((b * 0x85EBCA6B) & U32)
    h ^= h >> 16
    h = (h * 0x7FEB352D) & U32
    h ^= h >> 15
    h = (h * 0x846CA68B) & U32
    h ^= h >> 16
    return h


def _clamp(v, lo, hi):
    return lo if v < lo else hi if v > hi else v


def gl_spawn_of_night(seed, night, index):
    """Mirror of gl_spawn_of_night() -> (x, y)."""
    w = (GL_ARENA_X_MAX - GL_ARENA_X_MIN) // GL_ONE            # 223
    h = (GL_ARENA_Y_MAX - GL_ARENA_Y_MIN) // GL_ONE            # 143
    per = 2 * (w + h)

    p = gl_hash(gl_hash(seed, night), index) % per

    for guard in range(2):
        if p < w:                          # north fence, west -> east
            sx, sy = GL_ARENA_X_MIN + p * GL_ONE, GL_ARENA_Y_MIN
        elif p < w + h:                    # east fence, north -> south
            sx, sy = GL_ARENA_X_MAX, GL_ARENA_Y_MIN + (p - w) * GL_ONE
        elif p < 2 * w + h:                # south fence, east -> west
            sx, sy = GL_ARENA_X_MAX - (p - w - h) * GL_ONE, GL_ARENA_Y_MAX
        else:                              # west fence, south -> north
            sx = GL_ARENA_X_MIN
            sy = GL_ARENA_Y_MAX - (p - 2 * w - h) * GL_ONE

        if (gl_chebyshev(sx, sy, GL_PLAYER_START_X, GL_PLAYER_START_Y)
                >= GL_SAFE_RADIUS or guard == 1):
            return sx, sy
        p = (p + per // 2) % per           # too close: opposite fence point
    raise AssertionError('unreachable')


def gl_player_step(px, py, up, down, left, right):
    """Mirror of gl_player_step() -> (px, py)."""
    dx = (1 if right else 0) - (1 if left else 0)
    dy = (1 if down else 0) - (1 if up else 0)
    speed = GL_PLAYER_DIAG if (dx and dy) else GL_PLAYER_SPEED
    return (_clamp(px + dx * speed, GL_ARENA_X_MIN, GL_ARENA_X_MAX),
            _clamp(py + dy * speed, GL_ARENA_Y_MIN, GL_ARENA_Y_MAX))


def gl_shambler_step(zx, zy, px, py, zombie_id, frame):
    """Mirror of gl_shambler_step() -> (zx, zy)."""
    if (gl_hash(zombie_id, frame) & 3) == 0:
        return zx, zy

    dx = px - zx
    dy = py - zy
    sx = (1 if dx > GL_AXIS_DEADZONE else 0) - (1 if dx < -GL_AXIS_DEADZONE
                                                else 0)
    sy = (1 if dy > GL_AXIS_DEADZONE else 0) - (1 if dy < -GL_AXIS_DEADZONE
                                                else 0)
    speed = GL_SHAMBLER_DIAG if (sx and sy) else GL_SHAMBLER_SPEED
    return (_clamp(zx + sx * speed, GL_ARENA_X_MIN, GL_ARENA_X_MAX),
            _clamp(zy + sy * speed, GL_ARENA_Y_MIN, GL_ARENA_Y_MAX))


def gl_chebyshev(ax, ay, bx, by):
    """Mirror of gl_chebyshev()."""
    return max(abs(ax - bx), abs(ay - by))


def gl_contact(px, py, zx, zy):
    """Mirror of gl_contact()."""
    return gl_chebyshev(px, py, zx, zy) < GL_CONTACT_RANGE


# --- proofs ------------------------------------------------------------------

def on_perimeter(x, y):
    return (x in (GL_ARENA_X_MIN, GL_ARENA_X_MAX)
            and GL_ARENA_Y_MIN <= y <= GL_ARENA_Y_MAX) or \
           (y in (GL_ARENA_Y_MIN, GL_ARENA_Y_MAX)
            and GL_ARENA_X_MIN <= x <= GL_ARENA_X_MAX)


def main():
    failures = 0

    # 1. spawn schedule: pure, on-fence, in-bounds, outside the safe radius.
    spawns = 0
    for seed in range(256):
        for night in range(1, 9):
            x, y = gl_spawn_of_night(seed, night, 0)
            spawns += 1
            if (x, y) != gl_spawn_of_night(seed, night, 0):
                failures += 1
                print(f'FAIL determinism: spawn({seed},{night},0) unstable')
            if not on_perimeter(x, y):
                failures += 1
                print(f'FAIL perimeter: spawn({seed},{night},0) = '
                      f'({x},{y}) is off the fence line')
            if gl_chebyshev(x, y, GL_PLAYER_START_X,
                            GL_PLAYER_START_Y) < GL_SAFE_RADIUS:
                failures += 1
                print(f'FAIL safe radius: spawn({seed},{night},0) = '
                      f'({x},{y}) inside {GL_SAFE_RADIUS}')

    # 2. chase convergence onto an idle player: monotone, bounded.
    chases = 0
    worst = 0
    for seed in range(64):
        for night in range(1, 5):
            zx, zy = gl_spawn_of_night(seed, night, 0)
            px, py = GL_PLAYER_START_X, GL_PLAYER_START_Y
            dist = gl_chebyshev(px, py, zx, zy)
            chases += 1
            for frame in range(400):
                if gl_contact(px, py, zx, zy):
                    worst = max(worst, frame)
                    break
                zx, zy = gl_shambler_step(zx, zy, px, py, 0, frame)
                new_dist = gl_chebyshev(px, py, zx, zy)
                if new_dist > dist:
                    failures += 1
                    print(f'FAIL monotone chase: seed {seed} night {night} '
                          f'frame {frame}: {dist} -> {new_dist}')
                    break
                dist = new_dist
            else:
                failures += 1
                print(f'FAIL bounded chase: seed {seed} night {night}: '
                      f'no contact in 400 frames (dist {dist})')

    # 3. arena containment under adversarial hash-driven input.
    px, py = GL_PLAYER_START_X, GL_PLAYER_START_Y
    zx, zy = gl_spawn_of_night(0, 1, 0)
    for frame in range(20000):
        h = gl_hash(0xADBEEF, frame)
        px, py = gl_player_step(px, py, h & 1, h & 2, h & 4, h & 8)
        zx, zy = gl_shambler_step(zx, zy, px, py, 0, frame)
        for label, x, y in (('player', px, py), ('shambler', zx, zy)):
            if not (GL_ARENA_X_MIN <= x <= GL_ARENA_X_MAX
                    and GL_ARENA_Y_MIN <= y <= GL_ARENA_Y_MAX):
                failures += 1
                print(f'FAIL containment: {label} at ({x},{y}) frame {frame}')

    if failures:
        print(f'check-gloam: {failures} failure(s)')
        return 1

    print(f'check-gloam OK: {spawns} spawns pure/on-fence/safe-radius, '
          f'{chases} idle-player chases converge monotonically '
          f'(worst contact frame {worst}), player+Shambler contained over '
          f'20000 adversarial-input frames')
    return 0


if __name__ == '__main__':
    sys.exit(main())
