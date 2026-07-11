#!/usr/bin/env python3
"""Gloamline host proof (stdlib-only, <2s) — arc slice 4: shove + waves.

The check-cave.py sibling for Gloamline: a line-for-line Python mirror of
the pure simulation layer in games/gloamline-nds/source/gl_sim.c
(gl_hash / gl_spawn_of_night / gl_player_step / gl_shambler_step /
gl_chebyshev / gl_contact / gl_wave_count / gl_spawn_frame / gl_shove),
proving the meaningful invariants for EVERY reachable input instead of
the handful a replay touches:

  1. spawn schedule — for seeds 0..255 x nights 1..8 (index 0) PLUS every
     scheduled index of seeds 0..127 x nights 1..13: the spawn is a pure
     deterministic function (recompute-equal), lands ON the fence
     perimeter, inside the arena, and NEVER inside the safe radius of the
     player's night-start position;
  2. chase convergence — from every spawn (seeds 0..63 x nights 1..4), an
     idle player is reached: the Chebyshev distance is monotonically
     non-increasing under gl_shambler_step and contact happens within 400
     frames (the stagger hash can only delay, never stall);
  3. arena containment — under 20,000 frames of hash-driven adversarial
     input (movement AND shove attempts against a full 24-Shambler
     crowd) neither the player nor any Shambler ever leaves the arena;
  4. wave schedule — for nights 1..64: gl_wave_count ramps by +2 per
     night from 1, never exceeds the 24 cap, reaches it at night 13 and
     plateaus; gl_spawn_frame is deterministic, starts every night at
     frame 0, is non-decreasing in index, and finishes strictly inside
     the GL_WAVE_SPAWN_SPAN window (crowd only grows until dawn);
  5. shove — for thousands of hash-generated reachable configurations:
     deterministic (recompute-equal), a miss (outside GL_SHOVE_RANGE)
     never moves the zombie, a hit never pulls it closer, a hit with
     wall room pushes the Chebyshev distance out by exactly
     GL_SHOVE_PUSH, and the result is always inside the arena.

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
GL_ZOMBIE_CAP = 24
GL_WAVE_SPAWN_SPAN = 2400
GL_SHOVE_RANGE = 24 * GL_ONE
GL_SHOVE_PUSH = 40 * GL_ONE
GL_SHOVE_STUN = 45
GL_SHOVE_COOLDOWN = 90
GL_NIGHT_FRAMES = 3600

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


def gl_wave_count(night):
    """Mirror of gl_wave_count()."""
    count = 2 * night - 1                  # 1, 3, 5, ... ramp
    return GL_ZOMBIE_CAP if count > GL_ZOMBIE_CAP else count


def gl_spawn_frame(night, index):
    """Mirror of gl_spawn_frame()."""
    return index * GL_WAVE_SPAWN_SPAN // gl_wave_count(night)


def gl_shove(px, py, zx, zy):
    """Mirror of gl_shove() -> (connected, zx, zy)."""
    if gl_chebyshev(px, py, zx, zy) > GL_SHOVE_RANGE:
        return 0, zx, zy

    dx = zx - px
    dy = zy - py
    sx = (1 if dx > GL_AXIS_DEADZONE else 0) - (1 if dx < -GL_AXIS_DEADZONE
                                                else 0)
    sy = (1 if dy > GL_AXIS_DEADZONE else 0) - (1 if dy < -GL_AXIS_DEADZONE
                                                else 0)
    return (1,
            _clamp(zx + sx * GL_SHOVE_PUSH, GL_ARENA_X_MIN, GL_ARENA_X_MAX),
            _clamp(zy + sy * GL_SHOVE_PUSH, GL_ARENA_Y_MIN, GL_ARENA_Y_MAX))


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

    # 1b. every SCHEDULED index (waves): same spawn invariants hold for
    # index > 0 across the full ramp (night 13 = the 24 cap).
    for seed in range(128):
        for night in range(1, 14):
            for index in range(gl_wave_count(night)):
                x, y = gl_spawn_of_night(seed, night, index)
                spawns += 1
                if (x, y) != gl_spawn_of_night(seed, night, index):
                    failures += 1
                    print(f'FAIL determinism: spawn({seed},{night},{index}) '
                          'unstable')
                if not on_perimeter(x, y):
                    failures += 1
                    print(f'FAIL perimeter: spawn({seed},{night},{index}) = '
                          f'({x},{y}) is off the fence line')
                if gl_chebyshev(x, y, GL_PLAYER_START_X,
                                GL_PLAYER_START_Y) < GL_SAFE_RADIUS:
                    failures += 1
                    print(f'FAIL safe radius: spawn({seed},{night},{index}) '
                          f'= ({x},{y}) inside {GL_SAFE_RADIUS}')

    # 3. arena containment under adversarial hash-driven input — now with
    # the full 24-Shambler crowd and hash-driven shove attempts (bit 4 =
    # press A at the nearest zombie), the whole slice-4 state surface.
    px, py = GL_PLAYER_START_X, GL_PLAYER_START_Y
    zombies = [list(gl_spawn_of_night(0, 13, i)) + [0]   # [zx, zy, stun]
               for i in range(gl_wave_count(13))]
    for frame in range(20000):
        h = gl_hash(0xADBEEF, frame)
        px, py = gl_player_step(px, py, h & 1, h & 2, h & 4, h & 8)
        if h & 16:
            nearest = min(zombies,
                          key=lambda z: gl_chebyshev(px, py, z[0], z[1]))
            hit, nearest[0], nearest[1] = gl_shove(px, py, nearest[0],
                                                   nearest[1])
            if hit:
                nearest[2] = GL_SHOVE_STUN
        for zid, z in enumerate(zombies):
            if z[2] > 0:
                z[2] -= 1
            else:
                z[0], z[1] = gl_shambler_step(z[0], z[1], px, py, zid, frame)
        actors = [('player', px, py)] + [(f'shambler{i}', z[0], z[1])
                                         for i, z in enumerate(zombies)]
        for label, x, y in actors:
            if not (GL_ARENA_X_MIN <= x <= GL_ARENA_X_MAX
                    and GL_ARENA_Y_MIN <= y <= GL_ARENA_Y_MAX):
                failures += 1
                print(f'FAIL containment: {label} at ({x},{y}) frame {frame}')

    # 4. wave schedule: ramp, cap, plateau, spawn-frame shape.
    prev_count = 0
    for night in range(1, 65):
        count = gl_wave_count(night)
        expected = min(2 * night - 1, GL_ZOMBIE_CAP)
        if count != expected:
            failures += 1
            print(f'FAIL wave count: night {night}: {count} != {expected}')
        if count < prev_count or count > GL_ZOMBIE_CAP:
            failures += 1
            print(f'FAIL wave ramp/cap: night {night}: {prev_count} -> '
                  f'{count} (cap {GL_ZOMBIE_CAP})')
        if night >= 13 and count != GL_ZOMBIE_CAP:
            failures += 1
            print(f'FAIL wave plateau: night {night}: {count} != cap')
        prev_count = count
        prev_frame = None
        for index in range(count):
            sframe = gl_spawn_frame(night, index)
            if sframe != gl_spawn_frame(night, index):
                failures += 1
                print(f'FAIL spawn-frame determinism: ({night},{index})')
            if index == 0 and sframe != 0:
                failures += 1
                print(f'FAIL spawn frame: night {night} index 0 at {sframe}')
            if prev_frame is not None and sframe < prev_frame:
                failures += 1
                print(f'FAIL spawn order: night {night} index {index}: '
                      f'{prev_frame} -> {sframe}')
            if not 0 <= sframe < GL_WAVE_SPAWN_SPAN:
                failures += 1
                print(f'FAIL spawn window: night {night} index {index}: '
                      f'{sframe} outside [0, {GL_WAVE_SPAWN_SPAN})')
            prev_frame = sframe

    # 5. shove: deterministic, miss = no-op, hit never pulls closer, hit
    # with wall room pushes Chebyshev out by exactly GL_SHOVE_PUSH, and
    # the result stays inside the arena. Configurations are hash-driven
    # reachable states: player anywhere in the arena, zombie offset up to
    # ~2x shove range on each axis but never inside contact range (a
    # contact frame is death, not a shove).
    shoves = 0
    exact_pushes = 0
    for case in range(4096):
        px = GL_ARENA_X_MIN + gl_hash(case, 1) % (GL_ARENA_X_MAX
                                                  - GL_ARENA_X_MIN + 1)
        py = GL_ARENA_Y_MIN + gl_hash(case, 2) % (GL_ARENA_Y_MAX
                                                  - GL_ARENA_Y_MIN + 1)
        span = 4 * GL_SHOVE_RANGE + 1
        zx = _clamp(px + gl_hash(case, 3) % span - 2 * GL_SHOVE_RANGE,
                    GL_ARENA_X_MIN, GL_ARENA_X_MAX)
        zy = _clamp(py + gl_hash(case, 4) % span - 2 * GL_SHOVE_RANGE,
                    GL_ARENA_Y_MIN, GL_ARENA_Y_MAX)
        before = gl_chebyshev(px, py, zx, zy)
        if before < GL_CONTACT_RANGE:
            continue                        # dead, not shoving
        shoves += 1
        hit, nx, ny = gl_shove(px, py, zx, zy)
        if (hit, nx, ny) != gl_shove(px, py, zx, zy):
            failures += 1
            print(f'FAIL shove determinism: case {case}')
        after = gl_chebyshev(px, py, nx, ny)
        if not (GL_ARENA_X_MIN <= nx <= GL_ARENA_X_MAX
                and GL_ARENA_Y_MIN <= ny <= GL_ARENA_Y_MAX):
            failures += 1
            print(f'FAIL shove containment: case {case}: ({nx},{ny})')
        if not hit:
            if before <= GL_SHOVE_RANGE:
                failures += 1
                print(f'FAIL shove range: case {case}: miss at {before}')
            if (nx, ny) != (zx, zy):
                failures += 1
                print(f'FAIL shove miss moved: case {case}')
            continue
        if before > GL_SHOVE_RANGE:
            failures += 1
            print(f'FAIL shove range: case {case}: hit at {before}')
        if after < before:
            failures += 1
            print(f'FAIL shove pulled closer: case {case}: '
                  f'{before} -> {after}')
        room = (GL_ARENA_X_MIN + GL_SHOVE_PUSH <= zx
                <= GL_ARENA_X_MAX - GL_SHOVE_PUSH
                and GL_ARENA_Y_MIN + GL_SHOVE_PUSH <= zy
                <= GL_ARENA_Y_MAX - GL_SHOVE_PUSH)
        if room:
            if after - before != GL_SHOVE_PUSH:
                failures += 1
                print(f'FAIL shove push: case {case}: {before} -> {after} '
                      f'(want +{GL_SHOVE_PUSH})')
            exact_pushes += 1

    if failures:
        print(f'check-gloam: {failures} failure(s)')
        return 1

    print(f'check-gloam OK: {spawns} spawns pure/on-fence/safe-radius '
          '(incl. every scheduled wave index to the night-13 cap), '
          f'{chases} idle-player chases converge monotonically '
          f'(worst contact frame {worst}), player + 24-Shambler crowd '
          'contained over 20000 adversarial move+shove frames, wave '
          'schedule ramps 1..24 and plateaus with in-window ordered spawn '
          f'frames (nights 1..64), {shoves} shove cases deterministic/'
          f'never-closer/contained ({exact_pushes} wall-free cases pushed '
          f'exactly +{GL_SHOVE_PUSH})')
    return 0


if __name__ == '__main__':
    sys.exit(main())
