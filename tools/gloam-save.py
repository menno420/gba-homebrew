#!/usr/bin/env python3
"""Gloamline save-blob tool: inspect/verify/mutate the best-nights record
inside a RAW battery image (slice 9).

The host half of the battery-save evidence: the record blob a Gloamline
ROM writes to the cartridge backup lives at offset GL_SAVE_ADDR (0) of
the battery image tools/nds-headless-check.py exports with
--battery-out (a raw .sav — the emulated EEPROM's bytes, verbatim).
This tool decodes/crafts that blob through the SAME mirror functions as
everything else (import from tools/check-gloam.py — the lockstep rule
stays one file), so a C-encoder drift is caught at byte level.

Subcommands:
  inspect SAV                 decode + print the record; exit 1 if the
                              blob does not decode (what the ROM's boot
                              would treat as the fresh table)
  expect SAV --best N --seed S
                              assert the blob is BYTE-IDENTICAL to the
                              mirror's gl_save_encode(N, S) — the
                              write-side roundtrip check
  blank OUT [--size 8192]     write a factory-fresh chip image (all
                              0xFF) — the --battery-in fixture DeSmuME
                              needs to emulate a sized EEPROM
  corrupt SAV --out OUT       flip one bit inside the record's checksum
                              word (everything else intact) — the
                              corrupt-save fixture
  version-bump SAV --out OUT  rewrite the record with version+1 and a
                              RECOMPUTED VALID checksum — the
                              version-mismatch fixture (proves the
                              version check rejects on its own)

Stdlib-only. Exit 0 = ok, 1 = failed check.
"""

import argparse
import importlib.util
import pathlib
import sys

_HERE = pathlib.Path(__file__).resolve().parent
_spec = importlib.util.spec_from_file_location('check_gloam',
                                               _HERE / 'check-gloam.py')
cg = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(cg)


def read_blob(path):
    data = pathlib.Path(path).read_bytes()
    if len(data) < cg.GL_SAVE_ADDR + cg.GL_SAVE_BYTES:
        sys.exit(f'FAIL: {path} is {len(data)} bytes — smaller than the '
                 'record blob')
    return data, data[cg.GL_SAVE_ADDR:cg.GL_SAVE_ADDR + cg.GL_SAVE_BYTES]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='cmd', required=True)

    p = sub.add_parser('inspect')
    p.add_argument('sav')

    p = sub.add_parser('expect')
    p.add_argument('sav')
    p.add_argument('--best', type=int, required=True)
    p.add_argument('--seed', type=int, required=True)

    p = sub.add_parser('blank')
    p.add_argument('out')
    p.add_argument('--size', type=int, default=8192)

    p = sub.add_parser('corrupt')
    p.add_argument('sav')
    p.add_argument('--out', required=True)

    p = sub.add_parser('version-bump')
    p.add_argument('sav')
    p.add_argument('--out', required=True)

    args = parser.parse_args()

    if args.cmd == 'blank':
        pathlib.Path(args.out).write_bytes(b'\xff' * args.size)
        print(f'blank chip image written: {args.out} ({args.size} bytes '
              'of 0xFF)')
        return

    data, blob = read_blob(args.sav)

    if args.cmd == 'inspect':
        ok, best, seed = cg.gl_save_decode(blob)
        print(f'{args.sav}: blob {blob.hex()}')
        if not ok:
            sys.exit('FAIL: blob does not decode — the ROM would boot on '
                     'the fresh table')
        print(f'valid record: best {best} night(s), seed {seed} '
              f'(version {cg.GL_SAVE_VERSION})')
        return

    if args.cmd == 'expect':
        want = cg.gl_save_encode(args.best, args.seed)
        if blob != want:
            print(f'  got  {blob.hex()}')
            print(f'  want {want.hex()}')
            sys.exit(f'FAIL: record is not byte-identical to '
                     f'encode({args.best}, {args.seed})')
        print(f'PASS: record == gl_save_encode({args.best}, {args.seed}) '
              'byte-identically')
        return

    out = bytearray(data)
    if args.cmd == 'corrupt':
        # one flipped bit in the checksum word; the record data stays
        # intact, so ONLY the checksum check can be what rejects it.
        out[cg.GL_SAVE_ADDR + cg.GL_SAVE_BYTES - 4] ^= 0x01
        label = 'corrupt (checksum bit flipped)'
    else:                            # version-bump
        w = [int.from_bytes(blob[4 * i:4 * i + 4], 'little')
             for i in range(cg.GL_SAVE_WORDS)]
        w[1] = cg.GL_SAVE_VERSION + 1
        w[cg.GL_SAVE_WORDS - 1] = cg.gl_save_checksum(w)
        crafted = b''.join(x.to_bytes(4, 'little') for x in w)
        out[cg.GL_SAVE_ADDR:cg.GL_SAVE_ADDR + cg.GL_SAVE_BYTES] = crafted
        label = (f'version-bump (v{cg.GL_SAVE_VERSION + 1}, checksum '
                 'recomputed valid)')
    ok, _b, _s = cg.gl_save_decode(bytes(out[cg.GL_SAVE_ADDR:
                                             cg.GL_SAVE_ADDR
                                             + cg.GL_SAVE_BYTES]))
    if ok:
        sys.exit(f'FAIL: {label} fixture still decodes — not a fixture')
    pathlib.Path(args.out).write_bytes(bytes(out))
    print(f'{label} fixture written: {args.out} (mirror-confirmed to '
          'reject)')


if __name__ == '__main__':
    main()
