#!/usr/bin/env python3
"""Brineward save-blob tool: inspect/verify/mutate the Graywake ledger
inside a RAW battery image (arc slice 9 — gloam-save.py's sibling).

The host half of the battery-save evidence: the ledger blob a Brineward
ROM writes to the cartridge backup lives at offset BW_SAVE_ADDR (0) of
the battery image tools/nds-headless-check.py exports with
--battery-out (a raw .sav — the emulated EEPROM's bytes, verbatim).
This tool decodes/crafts that blob through the SAME mirror functions as
everything else (import from tools/check-brine.py — the lockstep rule
stays one file), so a C-encoder drift is caught at byte level.

Subcommands:
  inspect SAV                 decode + print the ledger; exit 1 if the
                              blob does not decode (what the ROM's boot
                              would treat as the fresh ledger)
  expect SAV --gold N --tiers H,C,S[,Hold] --band B
                              assert the blob is BYTE-IDENTICAL to the
                              mirror's bw_save_encode(...) — the
                              write-side roundtrip check
  blank OUT [--size 8192]     write a factory-fresh chip image (all
                              0xFF) — the --battery-in fixture DeSmuME
                              needs to emulate a sized EEPROM
  corrupt SAV --out OUT       flip one bit inside the ledger's checksum
                              word (everything else intact) — the
                              corrupt-save fixture
  version-bump SAV --out OUT  rewrite the ledger with version+1 and a
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
_spec = importlib.util.spec_from_file_location('check_brine',
                                               _HERE / 'check-brine.py')
cb = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(cb)


def read_blob(path):
    data = pathlib.Path(path).read_bytes()
    if len(data) < cb.BW_SAVE_ADDR + cb.BW_SAVE_BYTES:
        sys.exit(f'FAIL: {path} is {len(data)} bytes — smaller than the '
                 'ledger blob')
    return data, data[cb.BW_SAVE_ADDR:cb.BW_SAVE_ADDR + cb.BW_SAVE_BYTES]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='cmd', required=True)

    p = sub.add_parser('inspect')
    p.add_argument('sav')

    p = sub.add_parser('expect')
    p.add_argument('sav')
    p.add_argument('--gold', type=int, required=True)
    p.add_argument('--tiers', required=True,
                   help='hull,cannon,sail[,hold] (e.g. 1,0,0 or 1,0,0,2; '
                        'hold defaults to 0 when omitted)')
    p.add_argument('--band', type=int, required=True)

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
        ok, gold, hull, cannon, sail, hold, band = cb.bw_save_decode(blob)
        print(f'{args.sav}: blob {blob.hex()}')
        if not ok:
            sys.exit('FAIL: blob does not decode — the ROM would boot on '
                     'the fresh ledger')
        print(f'valid ledger: {gold}g banked, tiers {hull}/{cannon}/'
              f'{sail}/{hold}, charted band {band} '
              f'(version {cb.BW_SAVE_VERSION})')
        return

    if args.cmd == 'expect':
        # cut 4 added the hold tier: --tiers takes hull,cannon,sail and
        # an OPTIONAL 4th hold value (defaults to 0 = stock hold), so a
        # 3-value --tiers stays byte-for-byte what it encoded pre-cut-4.
        tiers = [int(t) for t in args.tiers.split(',')]
        if len(tiers) == 3:
            hull, cannon, sail = tiers
            hold = 0
        elif len(tiers) == 4:
            hull, cannon, sail, hold = tiers
        else:
            sys.exit('FAIL: --tiers wants hull,cannon,sail[,hold]')
        want = cb.bw_save_encode(args.gold, hull, cannon, sail, hold,
                                 args.band)
        if blob != want:
            print(f'  got  {blob.hex()}')
            print(f'  want {want.hex()}')
            sys.exit(f'FAIL: ledger is not byte-identical to '
                     f'encode({args.gold}g, {hull}/{cannon}/{sail}/'
                     f'{hold}, band {args.band})')
        print(f'PASS: ledger == bw_save_encode({args.gold}g, '
              f'{hull}/{cannon}/{sail}/{hold}, band {args.band}) '
              'byte-identically')
        return

    out = bytearray(data)
    if args.cmd == 'corrupt':
        # one flipped bit in the checksum word; the ledger data stays
        # intact, so ONLY the checksum check can be what rejects it.
        out[cb.BW_SAVE_ADDR + cb.BW_SAVE_BYTES - 4] ^= 0x01
        label = 'corrupt (checksum bit flipped)'
    else:                            # version-bump
        w = [int.from_bytes(blob[4 * i:4 * i + 4], 'little')
             for i in range(cb.BW_SAVE_WORDS)]
        w[1] = cb.BW_SAVE_VERSION + 1
        w[cb.BW_SAVE_WORDS - 1] = cb.bw_save_checksum(w)
        crafted = b''.join(x.to_bytes(4, 'little') for x in w)
        out[cb.BW_SAVE_ADDR:cb.BW_SAVE_ADDR + cb.BW_SAVE_BYTES] = crafted
        label = (f'version-bump (v{cb.BW_SAVE_VERSION + 1}, checksum '
                 'recomputed valid)')
    ok = cb.bw_save_decode(bytes(out[cb.BW_SAVE_ADDR:
                                     cb.BW_SAVE_ADDR
                                     + cb.BW_SAVE_BYTES]))[0]
    if ok:
        sys.exit(f'FAIL: {label} fixture still decodes — not a fixture')
    pathlib.Path(args.out).write_bytes(bytes(out))
    print(f'{label} fixture written: {args.out} (mirror-confirmed to '
          'NOT decode)')


if __name__ == '__main__':
    main()
