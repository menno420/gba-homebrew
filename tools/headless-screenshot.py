#!/usr/bin/env python3
"""Headless GBA boot proof: boot a ROM in mGBA, run N frames, dump a PNG.

Exits non-zero if the resulting frame is (near-)blank, so this doubles as a
boot smoke test: a ROM that crashes to a black/white screen fails the check.

Deps (pinned recipe — docs/capabilities.md):
    apt install mgba-sdl            # system libmgba 0.10.x
    pip install mgba==0.10.2        # binding MUST match system libmgba 0.10.x

Usage:
    python3 tools/headless-screenshot.py ROM.gba OUT.png [--frames N]
        [--keys START-END:NAME ...] [--keys-pattern START-END:PERIOD:DUTY:NAME ...]
        [--shot FRAME:PATH ...] [--require-distinct] [--savefile PATH]

Input scripting (generic — works for any ROM):
    --keys 240-420:A        hold the A button during frames [240, 420)
    --keys-pattern 30-140:3:1:A
                            duty-cycle a button during frames [30, 140):
                            held on the first DUTY frames of every PERIOD
                            frames (throttled thrust/movement — an open-loop
                            periodic pattern is invariant under a constant
                            input-latency shift, unlike hand-tuned pulses)
    --shot 238:mid.png      also dump (and non-blank-verify) frame 238
    --require-distinct      assert consecutive shots differ (proves the
                            scripted input changed what is on screen)
    --assert-text 1779:DEPTH 45
                            OCR-free semantic assertion: at frame 1779 the
                            exact string must be rendered somewhere on screen
                            in the Butano common variable_8x8 sprite font
                            (glyph patterns are read from the pinned
                            third_party/butano checkout, so this needs
                            tools/setup-toolchain.sh to have run). Turns a
                            "screenshots differ" proof into a regression
                            test: "this input script still reaches DEPTH 45".
    --savefile game.sav     persistent battery save. The file maps 1:1 onto
                            the cartridge save memory (Butano SRAM ROMs carry
                            the SRAM_V marker, so mGBA exposes 32KB of SRAM
                            at GBA bus address 0xE000000): the file contents
                            are loaded into that region right after reset
                            (power-on with this battery), and the region is
                            snapshotted back to the file after the last frame
                            (battery keeps the game's SRAM writes). Missing
                            file = 32KB of 0xFF, exactly what factory-erased
                            SRAM reads as. Boot the same savefile in two
                            separate runs to prove power-cycle persistence
                            headlessly; without the flag, save memory
                            vanishes at exit.
                            (Implementation note: the file is copied through
                            the emulated bus, NOT attached as an mGBA native
                            savefile — core.load_save() + a mapped VFile
                            segfaults this binding pin ~64 frames after the
                            game's first SRAM write, when mGBA's deferred
                            savedata flush fires. See PLATFORM-LIMITS.md.)
"""

import argparse
import os
import re
import struct
import sys
import tempfile
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
    return png_pixels_data(data)


def png_pixels_data(data):
    """Decode PNG bytes -> (width, height, [(r, g, b), ...] row-major)."""
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

SRAM_SIZE = 32 * 1024       # --savefile: cartridge SRAM size (Butano SRAM
                            # ROMs; mGBA maps it at GBA bus 0xE000000)


def parse_keys(spec):
    """'START-END:NAME' -> (start, end, 1, 1, key). Held in [start, end)."""
    span, _, name = spec.partition(':')
    start, _, end = span.partition('-')
    key = KEY_NAMES.get(name.upper())
    if key is None:
        sys.exit(f'FAIL: unknown key {name!r} (known: {", ".join(sorted(KEY_NAMES))})')
    return int(start), int(end), 1, 1, key


def parse_keys_pattern(spec):
    """'START-END:PERIOD:DUTY:NAME' -> (start, end, period, duty, key).

    Within [start, end) the key is held on frames whose offset from START
    modulo PERIOD is < DUTY.
    """
    span, _, rest = spec.partition(':')
    start, _, end = span.partition('-')
    period, _, rest = rest.partition(':')
    duty, _, name = rest.partition(':')
    key = KEY_NAMES.get(name.upper())
    if key is None:
        sys.exit(f'FAIL: unknown key {name!r} (known: {", ".join(sorted(KEY_NAMES))})')
    if int(period) < 1 or not 0 < int(duty) <= int(period):
        sys.exit(f'FAIL: bad pattern {spec!r} (need PERIOD >= 1, 0 < DUTY <= PERIOD)')
    return int(start), int(end), int(period), int(duty), key


def parse_shot(spec):
    """'FRAME:PATH' -> (frame, path)."""
    frame, _, path = spec.partition(':')
    return int(frame), path


def parse_assert_text(spec):
    """'FRAME:STRING' -> (frame, string)."""
    frame, _, text = spec.partition(':')
    if not text:
        sys.exit(f'FAIL: bad --assert-text {spec!r} (want FRAME:STRING)')
    return int(frame), text


# --- OCR-free text assertion against the Butano common 8x8 variable font ----
#
# The font's glyph strip + per-character advance widths live in the pinned
# third_party/butano checkout (glyphs are 8x8, ASCII '!'.. '~' top to bottom;
# ' ' has an advance but no glyph). Text sprites render glyph pixels with two
# palette colors (white core, black shading) that background palette fades
# never touch, so an exact-color template match is deterministic on mGBA.

FONT_BMP = 'third_party/butano/common/graphics/common_variable_8x8_font.bmp'
FONT_HEADER = 'third_party/butano/common/include/common_variable_8x8_sprite_font.h'
GLYPH_INK = {12: (0, 0, 0), 14: (255, 255, 255)}  # palette idx -> mGBA RGB
INK_COLORS = frozenset(GLYPH_INK.values())


def load_font():
    """-> ({char: (advance, {(x, y): rgb})}) for ASCII 32..126."""
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    bmp_path = os.path.join(root, FONT_BMP)
    header_path = os.path.join(root, FONT_HEADER)
    if not (os.path.isfile(bmp_path) and os.path.isfile(header_path)):
        sys.exit('FAIL: --assert-text needs the pinned Butano checkout '
                 f'({FONT_BMP}) — run tools/setup-toolchain.sh first')
    with open(header_path, encoding='utf-8') as handle:
        widths = {int(code): int(width) for width, code in
                  re.findall(r'(\d+),\s*//\s*(\d+)', handle.read())}
    with open(bmp_path, 'rb') as handle:
        data = handle.read()
    offset, = struct.unpack('<I', data[10:14])
    width, height = struct.unpack('<ii', data[18:26])
    assert width == 8, 'unexpected font strip width'
    stride = ((width + 1) // 2 + 3) & ~3

    def pixel(x, y):  # 4bpp indexed, rows stored bottom-up
        byte = data[offset + (height - 1 - y) * stride + x // 2]
        return (byte >> 4) & 0xF if x % 2 == 0 else byte & 0xF

    font = {' ': (widths[32], {})}
    for code in range(33, 127):
        glyph_top = (code - 33) * 8
        ink = {}
        for y in range(8):
            for x in range(8):
                index = pixel(x, glyph_top + y)
                if index:
                    ink[(x, y)] = GLYPH_INK.get(index)
                    if ink[(x, y)] is None:
                        sys.exit(f'FAIL: font glyph {code} uses unexpected '
                                 f'palette index {index}')
        font[chr(code)] = (widths[code], ink)
    return font


def text_template(font, text):
    """-> (width, {(x, y): rgb}) — the string as one expected pixel patch."""
    ink = {}
    pen = 0
    for char in text:
        if char not in font:
            sys.exit(f'FAIL: --assert-text char {char!r} not in the 8x8 font')
        advance, glyph = font[char]
        for (gx, gy), rgb in glyph.items():
            ink[(pen + gx, gy)] = rgb
        pen += advance
    return pen, ink


def find_text(pixels, width, height, template_width, template_ink):
    """Exact template search: ink pixels must match their color, non-ink
    pixels inside the patch must not be ANY ink color (so 'DEPTH 4' cannot
    pass in front of a rendered 'DEPTH 45'... except at patch edges).
    Returns (x, y) of the first match or None."""
    ink_items = sorted(template_ink.items())
    if not ink_items:
        return None
    (ax, ay), anchor_rgb = ink_items[0]
    non_ink = [(x, y) for y in range(8) for x in range(template_width)
               if (x, y) not in template_ink]
    for py in range(height - 7):
        for px in range(width - template_width + 1):
            if pixels[(py + ay) * width + px + ax] != anchor_rgb:
                continue
            if all(pixels[(py + y) * width + px + x] == rgb
                   for (x, y), rgb in ink_items) and \
               all(pixels[(py + y) * width + px + x] not in INK_COLORS
                   for (x, y) in non_ink):
                return px, py
    return None


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
    parser.add_argument('--keys-pattern', action='append', default=[],
                        metavar='START-END:PERIOD:DUTY:NAME',
                        help='duty-cycle a button during frames [START, END): '
                             'held on the first DUTY frames of every PERIOD '
                             'frames; repeatable')
    parser.add_argument('--shot', action='append', default=[], metavar='FRAME:PATH',
                        help='dump an extra verified screenshot at FRAME; repeatable')
    parser.add_argument('--require-distinct', action='store_true',
                        help='assert consecutive screenshots differ '
                             f'(>= {MIN_DIFF_PIXELS} pixels) — proves interactivity')
    parser.add_argument('--assert-text', action='append', default=[],
                        metavar='FRAME:STRING',
                        help='assert STRING is rendered on screen at FRAME '
                             '(Butano common variable_8x8 font, exact pixels); '
                             'repeatable')
    parser.add_argument('--savefile', metavar='PATH',
                        help='attach a persistent battery-save file (created '
                             'as 32KB of 0xFF if missing); in-game SRAM '
                             'writes persist to it across runs — reuse the '
                             'file to prove power-cycle persistence')
    args = parser.parse_args()

    key_spans = [parse_keys(spec) for spec in args.keys]
    key_spans += [parse_keys_pattern(spec) for spec in args.keys_pattern]
    shots = sorted(parse_shot(spec) for spec in args.shot)
    if any(frame >= args.frames for frame, _ in shots):
        sys.exit('FAIL: --shot frame beyond --frames')
    asserts = sorted(parse_assert_text(spec) for spec in args.assert_text)
    if any(frame >= args.frames for frame, _ in asserts):
        sys.exit('FAIL: --assert-text frame beyond --frames')
    font = load_font() if asserts else None

    save_data = None
    if args.savefile:
        if os.path.exists(args.savefile):
            with open(args.savefile, 'rb') as handle:
                save_data = handle.read()
            if len(save_data) != SRAM_SIZE:
                sys.exit(f'FAIL: savefile {args.savefile} is '
                         f'{len(save_data)} bytes, want {SRAM_SIZE}')
        else:
            save_data = b'\xff' * SRAM_SIZE  # factory-erased SRAM

    core = mgba.core.load_path(args.rom)
    if core is None:
        sys.exit(f'FAIL: mGBA could not load {args.rom}')
    width, height = core.desired_video_dimensions()
    frame = mgba.image.Image(width, height)
    core.set_video_buffer(frame)
    core.reset()

    if save_data is not None:
        # Persistent battery save, copied through the emulated bus: restore
        # the file into cartridge SRAM before frame 0 (= power-on with this
        # battery). Deliberately NOT core.load_save() — see the docstring.
        sram = core.memory.sram.u8
        for address, byte in enumerate(save_data):
            sram[address] = byte

    saved = []  # (path, frame_number) in capture order
    shot_index = 0
    assert_index = 0
    assert_failures = 0
    for frame_number in range(args.frames):
        active = [key for start, end, period, duty, key in key_spans
                  if start <= frame_number < end
                  and (frame_number - start) % period < duty]
        core.set_keys(*active)
        core.run_frame()
        if shot_index < len(shots) and shots[shot_index][0] == frame_number:
            path = shots[shot_index][1]
            with open(path, 'wb') as handle:
                frame.save_png(handle)
            saved.append((path, frame_number))
            shot_index += 1
        while assert_index < len(asserts) and \
                asserts[assert_index][0] == frame_number:
            text = asserts[assert_index][1]
            with tempfile.NamedTemporaryFile(suffix='.png', delete=False) as tmp:
                frame.save_png(tmp)
                tmp_path = tmp.name
            _, _, pixels = png_pixels(tmp_path)
            os.unlink(tmp_path)
            template_width, template_ink = text_template(font, text)
            hit = find_text(pixels, width, height, template_width, template_ink)
            if hit is None:
                print(f'ASSERT FAIL: {text!r} not rendered at frame {frame_number}')
                assert_failures += 1
            else:
                print(f'assert-text: {text!r} found at {hit} (frame {frame_number})')
            assert_index += 1
    with open(args.out_png, 'wb') as handle:
        frame.save_png(handle)
    saved.append((args.out_png, args.frames - 1))
    if save_data is not None:
        # Snapshot cartridge SRAM back to the battery file (the game's
        # bn::sram writes survive this process).
        with open(args.savefile, 'wb') as handle:
            handle.write(bytes(core.memory.sram.u8[0:SRAM_SIZE:1]))
        print(f'savefile: SRAM snapshot written to {args.savefile}')
    held = ', '.join(args.keys + args.keys_pattern) or 'none'
    print(f'ran {args.frames} frames (keys: {held}), '
          f'saved {len(saved)} {width}x{height} PNG(s)')

    if assert_failures:
        sys.exit(f'FAIL: {assert_failures} --assert-text assertion(s) failed')

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
