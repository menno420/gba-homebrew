#!/usr/bin/env python3
"""Headless GBA boot proof: boot a ROM in mGBA, run N frames, dump a PNG.

Exits non-zero if the resulting frame is (near-)blank, so this doubles as a
boot smoke test: a ROM that crashes to a black/white screen fails the check.

Deps (pinned recipe — docs/capabilities.md):
    apt install mgba-sdl            # system libmgba 0.10.x
    pip install mgba==0.10.2        # binding MUST match system libmgba 0.10.x

Usage:
    python3 tools/headless-screenshot.py ROM.gba OUT.png [--frames N]
"""

import argparse
import struct
import sys
import zlib

import mgba.core
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


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('rom')
    parser.add_argument('out_png')
    parser.add_argument('--frames', type=int, default=300,
                        help='frames to run before the screenshot (default 300 = 5s)')
    args = parser.parse_args()

    core = mgba.core.load_path(args.rom)
    if core is None:
        sys.exit(f'FAIL: mGBA could not load {args.rom}')
    width, height = core.desired_video_dimensions()
    frame = mgba.image.Image(width, height)
    core.set_video_buffer(frame)
    core.reset()
    for _ in range(args.frames):
        core.run_frame()
    with open(args.out_png, 'wb') as handle:
        frame.save_png(handle)
    print(f'ran {args.frames} frames, saved {width}x{height} PNG -> {args.out_png}')

    # Non-blank verification: unique colors + grayscale variance.
    png_width, png_height, pixels = png_pixels(args.out_png)
    assert (png_width, png_height) == (width, height), 'PNG size mismatch'
    unique_colors = len(set(pixels))
    grays = [(r * 299 + g * 587 + b * 114) / 1000 for r, g, b in pixels]
    mean = sum(grays) / len(grays)
    variance = sum((g - mean) ** 2 for g in grays) / len(grays)
    print(f'unique colors: {unique_colors} · gray variance: {variance:.1f}')
    if unique_colors < MIN_UNIQUE_COLORS or variance < MIN_VARIANCE:
        sys.exit(f'FAIL: frame looks blank (colors={unique_colors}, '
                 f'variance={variance:.1f}) — thresholds: '
                 f'{MIN_UNIQUE_COLORS} colors, {MIN_VARIANCE} variance')
    print('PASS: frame is non-blank — ROM booted and rendered')


if __name__ == '__main__':
    main()
