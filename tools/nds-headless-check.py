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
        [--elf ELF] [--watch NAME:ADDR:NWORDS ...] [--watch-log PATH]
        [--assert-watch FRAME:NAME:IDX:OP:VALUE ...]
        [--battery-in PATH[:SIZE]] [--battery-out PATH]

Battery saves (Gloamline slice 9 — the best-nights record):
    --battery-in save.sav[:8192]
                            import a RAW battery image into the emulated
                            cartridge backup BEFORE the first frame (the
                            "the cart has this chip with these bytes on
                            it" fixture; SIZE forces the chip size,
                            default 8192 = the 64Kbit-class EEPROM the
                            ROM assumes). Without this fixture DeSmuME's
                            backup is in its autodetect state and a
                            homebrew save write is NOT faithfully
                            emulated — always seed save-path proofs
                            with a sized image (a blank chip is 8192
                            bytes of 0xFF).
    --battery-out save.sav  export the backup as a RAW image after the
                            last frame (py-desmume 0.0.9 quirk: raw
                            .sav export works; .dsv export returns
                            False — use .sav).

Input scripting (subset of headless-screenshot.py's interface):
    --keys 240-420:A        hold the A button during frames [240, 420)
    --shot 120:mid.png      also dump (and non-blank-verify) frame 120
    --require-distinct      assert consecutive shots differ (proves the
                            main loop runs / input changed the scene)

Memory watches (ported from headless-screenshot.py — the Gloamline
telemetry-mailbox evidence path; slice 3 closed the "ELF-symbol
memory-watch tooling not yet proven on NDS" gap):
    --elf game.elf          resolve watch addresses from the ELF's symbol
                            table (minimal stdlib ELF32 parser). A watch
                            ADDR may then be a symbol name (exact match,
                            or a unique substring).
    --watch t:gl_telemetry:16
                            sample NWORDS u32 words at ADDR every frame
    --watch-log t.csv       write every watch, every frame, as CSV
    --assert-watch 300:t:3:eq:1
                            at frame 300, watch `t` word [3] must == 1
                            (OPs: eq ne lt le gt ge; VALUE may be 0x-hex)

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


# --- ELF-resolved memory watches (ported from tools/headless-screenshot.py) --

def elf_symbols(path):
    """Minimal ELF32 symbol table reader -> {name: (address, size)}."""
    with open(path, 'rb') as handle:
        data = handle.read()
    if data[:4] != b'\x7fELF' or data[4] != 1:
        sys.exit(f'FAIL: {path} is not a 32-bit ELF')
    endian = '<' if data[5] == 1 else '>'
    shoff, = struct.unpack_from(endian + 'I', data, 32)
    shentsize, shnum = struct.unpack_from(endian + 'HH', data, 46)
    sections = []
    for index in range(shnum):
        fields = struct.unpack_from(endian + '10I', data,
                                    shoff + index * shentsize)
        # (name, type, flags, addr, offset, size, link, info, align, entsize)
        sections.append(fields)
    symbols = {}
    for section in sections:
        if section[1] != 2:  # SHT_SYMTAB
            continue
        strtab_offset = sections[section[6]][4]
        for index in range(section[5] // section[9]):
            name_offset, value, size, _info, _other, shndx = struct.unpack_from(
                endian + 'IIIBBH', data, section[4] + index * section[9])
            if name_offset and shndx:  # skip unnamed + SHN_UNDEF
                end = data.index(b'\0', strtab_offset + name_offset)
                name = data[strtab_offset + name_offset:end].decode('ascii',
                                                                    'replace')
                symbols[name] = (value, size)
    return symbols


def resolve_address(token, symbols):
    """'0x...'/decimal literal, or an ELF symbol name (unique substring)."""
    try:
        return int(token, 0)
    except ValueError:
        pass
    if symbols is None:
        sys.exit(f'FAIL: watch address {token!r} is a symbol — pass --elf')
    if token in symbols:
        return symbols[token][0]
    matches = [name for name in symbols if token in name]
    if len(matches) != 1:
        sys.exit(f'FAIL: symbol {token!r} matches {len(matches)} ELF symbols'
                 + (f' ({", ".join(sorted(matches)[:5])} ...)'
                    if matches else ''))
    return symbols[matches[0]][0]


def parse_watch(spec):
    """'NAME:ADDR:NWORDS' -> (name, addr_token, nwords)."""
    parts = spec.split(':')
    if len(parts) != 3:
        sys.exit(f'FAIL: bad watch {spec!r} (want NAME:ADDR:NWORDS)')
    name, addr_token, count = parts
    if int(count, 0) < 1:
        sys.exit(f'FAIL: bad watch {spec!r} (NWORDS must be >= 1)')
    return name, addr_token, int(count, 0)


WATCH_OPS = {'eq': lambda a, b: a == b, 'ne': lambda a, b: a != b,
             'lt': lambda a, b: a < b, 'le': lambda a, b: a <= b,
             'gt': lambda a, b: a > b, 'ge': lambda a, b: a >= b}


def parse_assert_watch(spec):
    """'FRAME:NAME:IDX:OP:VALUE' -> (frame, name, idx, op, value)."""
    parts = spec.split(':')
    if len(parts) != 5 or parts[3] not in WATCH_OPS:
        sys.exit(f'FAIL: bad --assert-watch {spec!r} '
                 f'(want FRAME:NAME:IDX:OP:VALUE, OP in {sorted(WATCH_OPS)})')
    return int(parts[0]), parts[1], int(parts[2]), parts[3], int(parts[4], 0)


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
    parser.add_argument('--elf', metavar='PATH',
                        help='ELF matching the ROM: resolve --watch symbol '
                             'addresses from its symbol table')
    parser.add_argument('--watch', action='append', default=[],
                        metavar='NAME:ADDR:NWORDS',
                        help='sample NWORDS u32 words at ADDR (literal or '
                             'ELF symbol) every frame; repeatable')
    parser.add_argument('--watch-log', metavar='PATH',
                        help='write every watch sample, every frame, as CSV')
    parser.add_argument('--assert-watch', action='append', default=[],
                        metavar='FRAME:NAME:IDX:OP:VALUE',
                        help='assert watch NAME word [IDX] against VALUE at '
                             f'FRAME (OPs: {", ".join(sorted(WATCH_OPS))}); '
                             'repeatable')
    parser.add_argument('--battery-in', metavar='PATH[:SIZE]',
                        help='import a RAW battery image into the emulated '
                             'cartridge backup before frame 0 (SIZE forces '
                             'the chip size; default 8192)')
    parser.add_argument('--battery-out', metavar='PATH',
                        help='export the cartridge backup as a RAW image '
                             '(.sav) after the last frame')
    args = parser.parse_args()

    key_spans = [parse_keys(spec) for spec in args.keys]
    shots = sorted(parse_shot(spec) for spec in args.shot)
    if any(frame >= args.frames for frame, _ in shots):
        sys.exit('FAIL: --shot frame beyond --frames')

    symbols = elf_symbols(args.elf) if args.elf else None
    watches = []                     # (name, address, nwords)
    for spec in args.watch:
        name, addr_token, nwords = parse_watch(spec)
        address = resolve_address(addr_token, symbols)
        watches.append((name, address, nwords))
        print(f'watch {name}: 0x{address:08X} x{nwords} words')
    watch_names = {name for name, _, _ in watches}
    asserts = [parse_assert_watch(spec) for spec in args.assert_watch]
    for frame, name, idx, _op, _value in asserts:
        if name not in watch_names:
            sys.exit(f'FAIL: --assert-watch names unknown watch {name!r}')
        if frame >= args.frames:
            sys.exit('FAIL: --assert-watch frame beyond --frames')

    emu = DeSmuME()
    emu.open(args.rom)
    if args.battery_in:
        path, _, size = args.battery_in.partition(':')
        if not emu.backup.import_file(path, int(size) if size else 8192):
            sys.exit(f'FAIL: battery import of {path!r} failed')
        print(f'battery in: {path} ({size or 8192} bytes)')
    emu.volume_set(0)
    # BlocksDS-vs-DeSmuME boot quirk: latch "no keys pressed" BEFORE the
    # first frame (see module docstring / docs/PLATFORM-LIMITS.md).
    emu.input.keypad_update(0)

    log_handle = None
    if args.watch_log:
        log_handle = open(args.watch_log, 'w', encoding='ascii')
        header = ['frame']
        for name, _address, nwords in watches:
            header += [f'{name}[{idx}]' for idx in range(nwords)]
        log_handle.write(','.join(header) + '\n')

    assert_failures = []
    asserts_checked = 0
    saved = []  # (path, frame_number, rgbx-bytes) in capture order
    shot_index = 0
    for frame_number in range(args.frames):
        mask = 0
        for start, end, key in key_spans:
            if start <= frame_number < end:
                mask |= key
        emu.input.keypad_update(mask)
        emu.cycle()

        if watches:
            samples = {}
            for name, address, nwords in watches:
                samples[name] = [emu.memory.unsigned.read_long(address
                                                               + 4 * idx)
                                 for idx in range(nwords)]
            if log_handle:
                row = [str(frame_number)]
                for name, _address, _nwords in watches:
                    row += [str(value) for value in samples[name]]
                log_handle.write(','.join(row) + '\n')
            for frame, name, idx, op, value in asserts:
                if frame != frame_number:
                    continue
                actual = samples[name][idx]
                asserts_checked += 1
                verdict = 'ok' if WATCH_OPS[op](actual, value) else 'FAIL'
                print(f'assert-watch {verdict}: frame {frame} {name}[{idx}] '
                      f'= {actual} {op} {value}')
                if verdict == 'FAIL':
                    assert_failures.append((frame, name, idx, op, value,
                                            actual))

        if shot_index < len(shots) and shots[shot_index][0] == frame_number:
            path = shots[shot_index][1]
            rgbx = bytes(emu.display_buffer_as_rgbx())
            write_png(path, SCREEN_WIDTH, SCREEN_HEIGHT * 2, rgbx)
            saved.append((path, frame_number, rgbx))
            shot_index += 1

    if log_handle:
        log_handle.close()
    if assert_failures:
        sys.exit(f'FAIL: {len(assert_failures)} of {asserts_checked} '
                 'watch asserts failed')

    rgbx = bytes(emu.display_buffer_as_rgbx())
    write_png(args.out_png, SCREEN_WIDTH, SCREEN_HEIGHT * 2, rgbx)
    saved.append((args.out_png, args.frames - 1, rgbx))

    if args.battery_out:
        if not emu.backup.export_file(args.battery_out):
            sys.exit(f'FAIL: battery export to {args.battery_out!r} failed '
                     '(py-desmume 0.0.9 exports RAW .sav only)')
        print(f'battery out: {args.battery_out}')

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
             if args.require_distinct and len(saved) > 1 else '')
          + (f' · {asserts_checked} watch asserts ok'
             if asserts_checked else ''))


if __name__ == '__main__':
    main()
