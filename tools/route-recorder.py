#!/usr/bin/env python3
"""Closed-loop route recorder for Lumen Drift: records deterministic
proof-route key scripts for tools/headless-screenshot.py.

Sessions 7 and 8 each rebuilt this autopilot from scratch in a scratchpad to
record their deep-run / difficulty-curve proof routes; this file is that
autopilot promoted into the repo (session 8 slice 6) so future feature
proofs record a route instead of rebuilding the recorder.

How it works — a closed-loop pilot, replayed open-loop:

 1. Drives the ROM headlessly in mGBA, reading the mote's exact position
    and the depth high-water mark from the gl_audio_hook telemetry words
    (slots 3-6: deaths, x, y, depth) after every frame.
 2. Plans crystal-free corridors ahead of the mote using the PURE row
    layout mirrored in tools/check-cave.py (the same mirror CI proves
    against the ROM per PR) — it never guesses from pixels.
 3. Steers toward the middle of the best corridor with hysteresis, and a
    braking-distance governor never lets the fall speed exceed what full
    thrust can shed before the next crystal in the mote's column strip.
 4. Once TARGET_DEPTH is reached it parks (hovers) for 90 frames so banner
    text has time on screen for --assert-text, then emits every frame's
    held keys as one open-loop '--keys START-END:NAME ...' line.

Determinism: the emulator, the ROM and this controller are all
deterministic, so the SAME tool version + ROM behavior + arguments always
emit a byte-identical keys line (proven at promotion time by re-recording
docs/proof/session-8-tier-run-keys.txt: `route-recorder.py 185 3200 out`
reproduces the committed line exactly). The recorded line is CLOSED-LOOP
OUTPUT replayed open-loop: per-frame key spans with no settle points, so a
committed route is NOT offset-shiftable — if run-start timing or physics
ever changes, re-record with this tool instead of shifting spans.

Replay a recorded file with the harness's --keys-file flag:

    python3 tools/headless-screenshot.py ROM out.png --frames N \
        --keys-file docs/proof/my-route-keys.txt [asserts...]

(--keys-file reads the LAST non-empty line, so a recorded file can carry a
leading comment block describing what the route proves.)

Tuning constants below (NET_BRAKE, HAZ_HALF) mirror main.cpp's physics
(thrust 0.11, gravity 0.045, hazard_half 2) — keep them in lockstep.

Usage:
    python3 tools/route-recorder.py TARGET_DEPTH MAX_FRAMES OUT_KEYS_TXT
        [--rom PATH] [--elf PATH]

Exits non-zero if the pilot dies or MAX_FRAMES pass before TARGET_DEPTH.

Deps: the pinned headless stack (mgba-sdl + pip mgba==0.10.2 on python
3.11 — docs/capabilities.md) and a built ROM + ELF (tools/build.sh).
"""
import argparse
import importlib.util
import os
import sys

import mgba.core
import mgba.gba
import mgba.log

mgba.log.silence()

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def load_module(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


cc = load_module('cc', os.path.join(REPO, 'tools', 'check-cave.py'))
hs = load_module('hs', os.path.join(REPO, 'tools', 'headless-screenshot.py'))

CELL = 8
NET_BRAKE = 0.065          # thrust 0.11 - gravity 0.045 (main.cpp lockstep)
HAZ_HALF = 2.0             # forgiving crystal hitbox half-extent (lockstep)

_row_cache = {}


def row(cy):
    if cy not in _row_cache:
        solid = cc.row_solid(cy)
        crystal, _ = cc.row_features(cy, solid)
        _row_cache[cy] = (solid, crystal)
    return _row_cache[cy]


def clean_runs(cy, depth_rows):
    """Contiguous column runs open+crystal-free in rows [cy, cy+depth_rows]."""
    ok = []
    for cx in range(1, cc.MAP_COLS - 1):
        good = True
        for ry in range(cy, cy + depth_rows + 1):
            solid, crystal = row(ry)
            if solid[cx] or crystal[cx]:
                good = False
                break
        ok.append(good)
    runs = []
    start = None
    for i, good in enumerate(ok):
        if good and start is None:
            start = i + 1
        elif not good and start is not None:
            runs.append((start, i))
            start = None
    if start is not None:
        runs.append((start, cc.MAP_COLS - 2))
    return runs


def dist_to_crystal_below(x, y):
    """Pixels of fall left before the hazard box touches a crystal cell in
    the columns the box overlaps."""
    lo = int((x - HAZ_HALF - 3.9) // CELL)
    hi = int((x + HAZ_HALF + 3.9) // CELL)
    cols = [cx for cx in range(lo, hi + 1) if 0 <= cx < cc.MAP_COLS
            and abs(x - (cx * CELL + 4)) < HAZ_HALF + 4]
    cy0 = max(int(y // CELL), 0)
    for cy in range(cy0, cy0 + 60):
        solid, crystal = row(cy)
        if any(crystal[cx] for cx in cols):
            return max(cy * CELL - HAZ_HALF - y, 0.0)
    return 1e9


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('target_depth', type=int,
                        help='depth (cells) the route must reach alive')
    parser.add_argument('max_frames', type=int,
                        help='give up (exit 1) after this many frames')
    parser.add_argument('out_keys_txt',
                        help='write the open-loop --keys line here')
    parser.add_argument('--rom',
                        default=os.path.join(REPO, 'games', 'lumen-drift',
                                             'lumen-drift.gba'),
                        help='ROM to drive (default: the lumen-drift build)')
    parser.add_argument('--elf',
                        default=os.path.join(REPO, 'games', 'lumen-drift',
                                             'lumen-drift.elf'),
                        help='matching ELF (gl_audio_hook telemetry address)')
    args = parser.parse_args()

    symbols = hs.elf_symbols(args.elf)
    hook_addr = symbols['gl_audio_hook'][0]

    core = mgba.core.load_path(args.rom)
    if core is None:
        sys.exit(f'FAIL: mGBA could not load {args.rom}')
    core.reset()

    K = mgba.gba.GBA
    frames_keys = []
    prev_x = prev_y = None
    prev_vx = prev_vy = 0.0
    target_cx = None
    reached = 0
    parked = 0
    deaths = 0
    frame = -1

    for frame in range(args.max_frames):
        keys = set()
        if 20 <= frame < 24:
            keys.add('START')

        if frame >= 26 and prev_x is not None:
            x, y, vx, vy = prev_x, prev_y, prev_vx, prev_vy
            cy = int(y // CELL)

            # Corridor: prefer a deep clean window, fall back shallower.
            runs = []
            for depth_rows in (7, 4, 2):
                runs = clean_runs(cy, depth_rows)
                if runs:
                    break

            if runs:
                # Hysteresis: keep the current target while it stays valid.
                keep = target_cx is not None and \
                    any(a <= target_cx <= b for a, b in runs)
                if not keep:
                    col = x / CELL

                    def run_cost(run):
                        a, b = run
                        if a - 0.5 <= col <= b + 0.5:
                            return 0.0
                        return min(abs(col - a), abs(col - b))
                    a, b = min(runs, key=run_cost)
                    target_cx = (a + b) // 2

            tx = target_cx * CELL + 4 if target_cx is not None else x
            err = tx - (x + vx * 12)
            if err > 0.8:
                keys.add('RIGHT')
            elif err < -0.8:
                keys.add('LEFT')

            aligned = abs(tx - x) < 2.5
            vy_cap = 1.30 if aligned else 0.55
            if reached >= args.target_depth:
                vy_cap = 0.05  # park: hover once proven

            # Braking-distance governor: never descend faster than what can
            # be braked off before the next crystal in our column strip.
            d = dist_to_crystal_below(x, y)
            need_brake = vy > 0 and vy * vy / (2 * NET_BRAKE) + 10 > d

            if vy > vy_cap or need_brake:
                keys.add('A')

        frames_keys.append(keys)
        core.set_keys(*[getattr(K, 'KEY_' + name) for name in keys])
        core.run_frame()

        raw = core.memory.u32[hook_addr:hook_addr + 32:4]

        def s32(v):
            return v - (1 << 32) if v >= (1 << 31) else v

        x = s32(raw[4]) / 4096.0
        y = s32(raw[5]) / 4096.0
        depth = raw[6]
        deaths = raw[3]
        if prev_x is not None:
            prev_vx, prev_vy = x - prev_x, y - prev_y
        prev_x, prev_y = x, y
        reached = depth

        if deaths:
            print(f'DEAD at frame {frame} depth {depth} x {x:.1f} y {y:.1f}')
            return 1
        if depth >= args.target_depth:
            parked += 1
            if parked == 1:
                print(f'target depth {args.target_depth} reached at frame {frame}')
            if parked >= 90:   # linger long enough for banner asserts
                break

    print(f'final: frame {frame} depth {reached} deaths {deaths}')
    if reached < args.target_depth:
        return 1

    spans = []
    active = {}
    for f, keys in enumerate(frames_keys + [set()]):
        for name in list(active):
            if name not in keys:
                spans.append((active.pop(name), f, name))
        for name in keys:
            if name not in active:
                active[name] = f
    spans.sort()
    line = ' '.join(f'--keys {a}-{b}:{n}' for a, b, n in spans)
    with open(args.out_keys_txt, 'w') as handle:
        handle.write(line + '\n')
    print(f'{len(spans)} spans, {frame + 1} frames -> {args.out_keys_txt}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
