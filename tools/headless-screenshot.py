#!/usr/bin/env python3
"""Headless GBA boot proof: boot a ROM in mGBA, run N frames, dump a PNG.

Exits non-zero if the resulting frame is (near-)blank, so this doubles as a
boot smoke test: a ROM that crashes to a black/white screen fails the check.

Deps (pinned recipe — docs/capabilities.md):
    apt install mgba-sdl            # system libmgba 0.10.x
    pip install mgba==0.10.2        # binding MUST match system libmgba 0.10.x

Usage:
    python3 tools/headless-screenshot.py ROM.gba OUT.png [--frames N]
        [--keys START-END:NAME ...] [--shot FRAME:PATH ...] [--require-distinct]

Input scripting (generic — works for any ROM):
    --keys 240-420:A        hold the A button during frames [240, 420)
    --shot 238:mid.png      also dump (and non-blank-verify) frame 238
    --require-distinct      assert consecutive shots differ (proves the
                            scripted input changed what is on screen)
"""

import argparse
import struct
import sys
import zlib

import mgba.core
import mgba.gba
import mgba.image
import mgba.log

mgba.log.silence()  # emulator debug spam otherwise floods stderr

MIN_UNIQUE_COLORS = 3       # a booted scene shows text on a backdrop
                            # (the skeleton scene is exactly 3: backdrop +
                            # two font shades — a blank/crashed screen is 1)
MIN_VARIANCE = 100.0        # grayscale pixel variance; blank screens are ~0


def png_pixels(path):
    """Minimal RGBA PNG reader (no Pillow dependency in the container)."""
    with open(path, 'rb') as handle:
        data = handle.read()
    assert data[:8] == b'\x89PNG\r\n\x1a\n', 'not a PNG'
    pos, width, height, idat = 8, None, None, b''
    while pos < len(data):
        length, ctype = struct.unpack('>I4s', data[pos:pos + 8])
        chunk = data[pos + 8:pos + 8 + length]
        if ctype == b'IHDR':
            width, height, bit_depth, color_type = struct.unpack('>IIBB', chunk[:10])
            assert (bit_depth, color_type) in ((8, 6), (8, 2)), \
                f'unsupported PNG format: depth={bit_depth} color={color_type}'
            channels = 4 if color_type == 6 else 3
        elif ctype == b'IDAT':
            idat += chunk
        pos += 12 + length
    raw = zlib.decompress(idat)
    stride = width * channels
    pixels = []
    prior = bytearray(stride)
    pos = 0
    for _ in range(height):
        filt = raw[pos]
        line = bytearray(raw[pos + 1:pos + 1 + stride])
        pos += 1 + stride
        for i in range(stride):
            a = line[i - channels] if i >= channels else 0
            b = prior[i]
            c = prior[i - channels] if i >= channels else 0
            if filt == 1:
                line[i] = (line[i] + a) & 0xFF
            elif filt == 2:
                line[i] = (line[i] + b) & 0xFF
            elif filt == 3:
                line[i] = (line[i] + (a + b) // 2) & 0xFF
            elif filt == 4:
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                pred = a if pa <= pb and pa <= pc else (b if pb <= pc else c)
                line[i] = (line[i] + pred) & 0xFF
        prior = line
        for x in range(width):
            off = x * channels
            pixels.append(tuple(line[off:off + 3]))
    return width, height, pixels


KEY_NAMES = {
    name[len('KEY_'):]: getattr(mgba.gba.GBA, name)
    for name in dir(mgba.gba.GBA) if name.startswith('KEY_')
}

MIN_DIFF_PIXELS = 100       # --require-distinct: consecutive shots must
                            # differ in at least this many pixels


def parse_keys(spec):
    """'START-END:NAME' -> (start_frame, end_frame, key). Held in [start, end)."""
    span, _, name = spec.partition(':')
    start, _, end = span.partition('-')
    key = KEY_NAMES.get(name.upper())
    if key is None:
        sys.exit(f'FAIL: unknown key {name!r} (known: {", ".join(sorted(KEY_NAMES))})')
    return int(start), int(end), key


def parse_shot(spec):
    """'FRAME:PATH' -> (frame, path)."""
    frame, _, path = spec.partition(':')
    return int(frame), path


def verify_non_blank(path, width, height):
    """Exit non-zero unless the PNG at `path` looks like a rendered scene."""
    png_width, png_height, pixels = png_pixels(path)
    assert (png_width, png_height) == (width, height), 'PNG size mismatch'
    unique_colors = len(set(pixels))
    grays = [(r * 299 + g * 587 + b * 114) / 1000 for r, g, b in pixels]
    mean = sum(grays) / len(grays)
    variance = sum((g - mean) ** 2 for g in grays) / len(grays)
    print(f'{path}: unique colors: {unique_colors} · gray variance: {variance:.1f}')
    if unique_colors < MIN_UNIQUE_COLORS or variance < MIN_VARIANCE:
        sys.exit(f'FAIL: {path} looks blank (colors={unique_colors}, '
                 f'variance={variance:.1f}) — thresholds: '
                 f'{MIN_UNIQUE_COLORS} colors, {MIN_VARIANCE} variance')
    return pixels


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rom')
    parser.add_argument('out_png')
    parser.add_argument('--frames', type=int, default=300,
                        help='frames to run before the final screenshot (default 300 = 5s)')
    parser.add_argument('--keys', action='append', default=[], metavar='START-END:NAME',
                        help='hold a button during frames [START, END); repeatable')
    parser.add_argument('--shot', action='append', default=[], metavar='FRAME:PATH',
                        help='dump an extra verified screenshot at FRAME; repeatable')
    parser.add_argument('--require-distinct', action='store_true',
                        help='assert consecutive screenshots differ '
                             f'(>= {MIN_DIFF_PIXELS} pixels) — proves interactivity')
    args = parser.parse_args()

    key_spans = [parse_keys(spec) for spec in args.keys]
    shots = sorted(parse_shot(spec) for spec in args.shot)
    if any(frame >= args.frames for frame, _ in shots):
        sys.exit('FAIL: --shot frame beyond --frames')

    core = mgba.core.load_path(args.rom)
    if core is None:
        sys.exit(f'FAIL: mGBA could not load {args.rom}')
    width, height = core.desired_video_dimensions()
    frame = mgba.image.Image(width, height)
    core.set_video_buffer(frame)
    core.reset()

    saved = []  # (path, frame_number) in capture order
    shot_index = 0
    for frame_number in range(args.frames):
        active = [key for start, end, key in key_spans if start <= frame_number < end]
        core.set_keys(*active)
        core.run_frame()
        if shot_index < len(shots) and shots[shot_index][0] == frame_number:
            path = shots[shot_index][1]
            with open(path, 'wb') as handle:
                frame.save_png(handle)
            saved.append((path, frame_number))
            shot_index += 1
    with open(args.out_png, 'wb') as handle:
        frame.save_png(handle)
    saved.append((args.out_png, args.frames - 1))
    held = ', '.join(args.keys) if args.keys else 'none'
    print(f'ran {args.frames} frames (keys: {held}), '
          f'saved {len(saved)} {width}x{height} PNG(s)')

    previous = None
    for path, frame_number in saved:
        pixels = verify_non_blank(path, width, height)
        if args.require_distinct and previous is not None:
            diff = sum(1 for a, b in zip(previous[1], pixels) if a != b)
            print(f'{previous[0]} vs {path}: {diff} pixels differ')
            if diff < MIN_DIFF_PIXELS:
                sys.exit(f'FAIL: shots {previous[0]} and {path} are too similar '
                         f'({diff} < {MIN_DIFF_PIXELS} pixels) — input had no '
                         'visible effect')
        previous = (path, pixels)
    print('PASS: all frames non-blank — ROM booted and rendered'
          + (' · consecutive shots distinct — input changed the scene'
             if args.require_distinct and len(saved) > 1 else ''))


if __name__ == '__main__':
    main()
