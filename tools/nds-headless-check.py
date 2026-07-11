#!/usr/bin/env python3
"""Headless NDS boot proof: boot a .nds in DeSmuME, run N frames, dump PNGs.

The NDS sibling of tools/headless-screenshot.py (mGBA/GBA): boots the ROM
with the py-desmume binding, runs N frames headlessly, and dumps BOTH
screens (top = main engine, bottom = sub engine) as one stacked 256x384
PNG. Exits non-zero if EITHER screen is (near-)blank, so this doubles as a
dual-screen boot smoke test — a ROM that crashes, powers off, or only
initializes one display engine fails the check.

Deps (pinned recipe — docs/BUILDING.md):
    pip install py-desmume==0.0.9   # bundles its own DeSmuME core + SDL2
    # headless containers: SDL needs no display — this script forces the
    # dummy SDL drivers itself before importing the binding.

Usage:
    python3 tools/nds-headless-check.py ROM.nds OUT.png [--frames N]
        [--keys START-END:NAME ...] [--shot FRAME:PATH ...]
        [--require-distinct]

Input scripting (subset of headless-screenshot.py's interface):
    --keys 240-420:A        hold the A button during frames [240, 420)
    --shot 120:mid.png      also dump (and non-blank-verify) frame 120
    --require-distinct      assert consecutive shots differ (proves the
                            main loop runs / input changed the scene)

KNOWN QUIRK (docs/PLATFORM-LIMITS.md, 2026-07-11): the py-desmume core
reads REG_KEYINPUT as active-low ZERO (= every button held) until the
frontend's first keypad latch. The BlocksDS default ARM7 core treats
SELECT+START+L+R as its exit combo, so without a keypad reset BEFORE the
first frame every BlocksDS ROM "powers off via ARM7 SPI" at frame 1.
emu.input.keypad_update(0) before cycling is the fix — do not remove it.
"""

import argparse
import os
import struct
import sys
import zlib

# Headless container: force SDL's dummy drivers BEFORE the binding loads,
# or DeSmuME aborts with "Error trying to initialize SDL: No available
# video device".
os.environ.setdefault('SDL_VIDEODRIVER', 'dummy')
os.environ.setdefault('SDL_AUDIODRIVER', 'dummy')

from desmume.controls import Keys, keymask                     # noqa: E402
from desmume.emulator import (DeSmuME, SCREEN_HEIGHT,          # noqa: E402
                              SCREEN_WIDTH)

MIN_UNIQUE_COLORS = 2       # a booted console scene is text on a backdrop
                            # (>= 2 colors per screen); a blank/crashed
                            # screen is exactly 1
MIN_VARIANCE = 100.0        # grayscale pixel variance; blank screens are ~0
MIN_DIFF_PIXELS = 50        # --require-distinct: consecutive shots must
                            # differ in at least this many pixels

KEY_NAMES = {
    name[len('KEY_'):]: getattr(Keys, name)
    for name in dir(Keys)
    if name.startswith('KEY_') and name not in ('KEY_NONE',)
}


def parse_keys(spec):
    """'START-END:NAME' -> (start, end, keymask). Held in [start, end)."""
    span, _, name = spec.partition(':')
    start, _, end = span.partition('-')
    key = KEY_NAMES.get(name.upper())
    if key is None:
        sys.exit(f'FAIL: unknown key {name!r} '
                 f'(known: {", ".join(sorted(KEY_NAMES))})')
    return int(start), int(end), keymask(key)


def parse_shot(spec):
    """'FRAME:PATH' -> (frame, path)."""
    frame, _, path = spec.partition(':')
    return int(frame), path


def write_png(path, width, height, rgbx):
    """Minimal RGB PNG writer (no Pillow dependency in the container)."""
    rows = []
    for y in range(height):
        row = bytearray([0])  # filter: None
        base = y * width * 4
        for x in range(width):
            row += rgbx[base + x * 4:base + x * 4 + 3]
        rows.append(bytes(row))
    raw = b''.join(rows)

    def chunk(ctype, data):
        return (struct.pack('>I', len(data)) + ctype + data
                + struct.pack('>I', zlib.crc32(ctype + data)))

    with open(path, 'wb') as handle:
        handle.write(b'\x89PNG\r\n\x1a\n')
        handle.write(chunk(b'IHDR', struct.pack('>IIBBBBB', width, height,
                                                8, 2, 0, 0, 0)))
        handle.write(chunk(b'IDAT', zlib.compress(raw)))
        handle.write(chunk(b'IEND', b''))


def screen_pixels(rgbx, screen_index):
    """RGBX framebuffer -> [(r, g, b), ...] for one 256x192 screen.

    The binding exposes both screens stacked: screen 0 (top / main engine)
    then screen 1 (bottom / sub engine).
    """
    offset = screen_index * SCREEN_WIDTH * SCREEN_HEIGHT * 4
    return [tuple(rgbx[offset + i * 4:offset + i * 4 + 3])
            for i in range(SCREEN_WIDTH * SCREEN_HEIGHT)]


def verify_non_blank(label, pixels):
    """Exit non-zero unless one screen's pixels look like a rendered scene."""
    unique_colors = len(set(pixels))
    grays = [(r * 299 + g * 587 + b * 114) / 1000 for r, g, b in pixels]
    mean = sum(grays) / len(grays)
    variance = sum((g - mean) ** 2 for g in grays) / len(grays)
    print(f'{label}: unique colors: {unique_colors} · '
          f'gray variance: {variance:.1f}')
    if unique_colors < MIN_UNIQUE_COLORS or variance < MIN_VARIANCE:
        sys.exit(f'FAIL: {label} looks blank (colors={unique_colors}, '
                 f'variance={variance:.1f}) — thresholds: '
                 f'{MIN_UNIQUE_COLORS} colors, {MIN_VARIANCE} variance')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rom')
    parser.add_argument('out_png')
    parser.add_argument('--frames', type=int, default=300,
                        help='frames to run before the final screenshot '
                             '(default 300 = 5s)')
    parser.add_argument('--keys', action='append', default=[],
                        metavar='START-END:NAME',
                        help='hold a button during frames [START, END); '
                             'repeatable')
    parser.add_argument('--shot', action='append', default=[],
                        metavar='FRAME:PATH',
                        help='dump an extra verified both-screens PNG at '
                             'FRAME; repeatable')
    parser.add_argument('--require-distinct', action='store_true',
                        help='assert consecutive screenshots differ '
                             f'(>= {MIN_DIFF_PIXELS} pixels) — proves the '
                             'main loop runs')
    args = parser.parse_args()

    key_spans = [parse_keys(spec) for spec in args.keys]
    shots = sorted(parse_shot(spec) for spec in args.shot)
    if any(frame >= args.frames for frame, _ in shots):
        sys.exit('FAIL: --shot frame beyond --frames')

    emu = DeSmuME()
    emu.open(args.rom)
    emu.volume_set(0)
    # BlocksDS-vs-DeSmuME boot quirk: latch "no keys pressed" BEFORE the
    # first frame (see module docstring / docs/PLATFORM-LIMITS.md).
    emu.input.keypad_update(0)

    saved = []  # (path, frame_number, rgbx-bytes) in capture order
    shot_index = 0
    for frame_number in range(args.frames):
        mask = 0
        for start, end, key in key_spans:
            if start <= frame_number < end:
                mask |= key
        emu.input.keypad_update(mask)
        emu.cycle()
        if shot_index < len(shots) and shots[shot_index][0] == frame_number:
            path = shots[shot_index][1]
            rgbx = bytes(emu.display_buffer_as_rgbx())
            write_png(path, SCREEN_WIDTH, SCREEN_HEIGHT * 2, rgbx)
            saved.append((path, frame_number, rgbx))
            shot_index += 1

    rgbx = bytes(emu.display_buffer_as_rgbx())
    write_png(args.out_png, SCREEN_WIDTH, SCREEN_HEIGHT * 2, rgbx)
    saved.append((args.out_png, args.frames - 1, rgbx))

    held = ', '.join(args.keys) or 'none'
    print(f'ran {args.frames} frames (keys: {held}), saved {len(saved)} '
          f'{SCREEN_WIDTH}x{SCREEN_HEIGHT * 2} both-screens PNG(s)')

    previous = None
    for path, frame_number, shot_rgbx in saved:
        for index, label in ((0, 'top screen'), (1, 'bottom screen')):
            verify_non_blank(f'{path} [{label}]',
                             screen_pixels(shot_rgbx, index))
        if args.require_distinct and previous is not None:
            diff = sum(1 for a, b in zip(previous[1], shot_rgbx) if a != b)
            print(f'{previous[0]} vs {path}: {diff} bytes differ')
            if diff < MIN_DIFF_PIXELS:
                sys.exit(f'FAIL: shots {previous[0]} and {path} are too '
                         f'similar ({diff} < {MIN_DIFF_PIXELS}) — the scene '
                         'did not change')
        previous = (path, shot_rgbx)
    print('PASS: both screens non-blank on every shot — ROM booted and '
          'rendered dual-screen'
          + (' · consecutive shots distinct — the main loop is running'
             if args.require_distinct and len(saved) > 1 else ''))


if __name__ == '__main__':
    main()
