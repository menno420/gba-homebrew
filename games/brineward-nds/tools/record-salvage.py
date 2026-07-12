#!/usr/bin/env python3
"""Brineward salvage route recorder (arc slice 3 — loot + gold).

The salvage sibling of record-duel-win.py: it records a WHOLE story —
win the duel (the beam-holding policy), then run the salvage policy
(tools/check-brine.py bw_salvage_policy — the exact brain host proof 7
proves converges) CLOSED-LOOP: drop to battle sail, scoop the crates
nearest-first, then come home and bank at the Graywake pier. The inputs
actually issued are recorded, re-verified OPEN-LOOP through a faithful
model of main.c's state machine (duel steps while the duel runs, salvage
steps after the sink, the pier pass every frame, trim on key EDGES like
keysDown), and checked for alignment robustness: the combined trace must
still scoop everything and bank the full hold when every input lands
whole frames early/late — the [-6,+6] window the duel route holds.

THREE key files come out, so the carry/forfeit proof can stop mid-story:

  salvage-duel-keys.txt  — START + the winning duel for THIS tool's
      seed. (The committed duel-win route keeps ITS seed 126: that
      seed's WIN is shift-robust but its END STATE is not — under a
      -5..-3 shift it wins a whole different duel ~90 steps earlier
      with the wreck ~25 px away, so no salvage trace recorded on it
      can hold. Seed 127 was scanned for end-state stability across
      the full window; the scan lives in this recorder's history.)
  salvage-scoop-keys.txt — the scoop segment only: after it, the hold
      carries all 3 crates (15g) and the crates are gone from the water.
      Proof 6 replays this, then presses START to sail out WITH the hold
      and idles into the rake — sink-forfeits-hold, banked gold safe.
  salvage-bank-keys.txt  — the homing segment: bow on the pier, dock,
      bank. Proof 5 replays duel + scoop + bank: hold -> banked gold.

Frame mapping (the session-20 law, unchanged): for a START press
spanning [A, A+5) the latched seed is A-2 and the key at tool frame T
drives duel/salvage step T-(A+1) — i.e. step j takes the key at tool
frame seed+3+j. The salvage segment simply continues the same step
counter: the duel ends at step E, salvage step s is step E+s.

Usage:
    python3 games/brineward-nds/tools/record-salvage.py \
        [--seed 127] [--outdir games/brineward-nds/proof]

Deterministic: same seed -> byte-identical outputs.
"""

import argparse
import importlib.util
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parents[3]


def _load(name, rel):
    spec = importlib.util.spec_from_file_location(name, REPO / rel)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


brine = _load('check_brine', 'tools/check-brine.py')
rdw = _load('record_duel_win', 'games/brineward-nds/tools/record-duel-win.py')

DEFAULT_SEED = 127           # scanned for end-state shift stability
MAX_FRAMES = 3600
SHIFT_CHECK = 6              # traces must still work shifted +-this many


def record(seed):
    """Closed-loop duel win + salvage run.

    Returns (timeline, e0, scoop_len, home_len, end) where timeline[j] =
    (turn, trim_delta, fire_l, fire_r) for step j in main.c's continuous
    step space, e0 = duel steps, scoop segment = [e0, e0+scoop_len),
    homing segment = the rest, and end is the final mirrored Duel.
    """
    turns, fires, d = rdw.record(seed)
    if d.over != brine.BW_DUEL_ENEMY_SUNK:
        sys.exit(f'FAIL: duel route does not win seed {seed}')
    e0 = len(turns)
    fire_l = {f for f, s in fires if s == 'L'}
    fire_r = {f for f, s in fires if s == 'R'}
    timeline = [(turns[j], 0, 1 if j in fire_l else 0,
                 1 if j in fire_r else 0) for j in range(e0)]

    scoop_len = None
    banked = 0
    while banked == 0 and len(timeline) - e0 < MAX_FRAMES:
        inp = brine.bw_salvage_policy(d)
        timeline.append((inp.turn, inp.trim_delta, 0, 0))
        brine.bw_salvage_step(d, inp)
        banked += brine.bw_dock_step(d)
        if scoop_len is None and not any(c.live for c in d.loot):
            scoop_len = len(timeline) - e0
    if banked != brine.BW_LOOT_DROPS * brine.BW_LOOT_VALUE:
        sys.exit(f'FAIL: salvage policy banked {banked}, seed {seed}')
    if d.over != brine.BW_DUEL_ENEMY_SUNK:
        sys.exit('FAIL: player sunk during closed-loop salvage')
    home_len = len(timeline) - e0 - scoop_len
    return timeline, e0, scoop_len, home_len, d


def replay(seed, timeline, shift=0, slack=400):
    """Open-loop replay through main.c's state machine model.

    Duel steps while the duel runs, salvage steps once the enemy is
    under, the pier pass every live-water frame; fire flags only reach
    the sim while the duel runs (salvage batteries stay cold, like
    main.c). Returns (duel, banked_total).
    """
    d = brine.Duel()
    brine.bw_duel_init(d, seed)
    banked = 0
    for j in range(len(timeline) + shift + slack):
        k = j - shift
        turn, trim, fl, fr = timeline[k] if 0 <= k < len(timeline) \
            else (0, 0, 0, 0)
        if d.over == brine.BW_DUEL_RUNNING:
            brine.bw_duel_step(
                d, brine.Inputs(turn=turn, trim_delta=trim,
                                fire_l=fl, fire_r=fr))
        elif d.over == brine.BW_DUEL_ENEMY_SUNK:
            brine.bw_salvage_step(
                d, brine.Inputs(turn=turn, trim_delta=trim))
        else:
            break                        # player sunk: caller judges
        banked += brine.bw_dock_step(d)
    return d, banked


def segment_tokens(timeline, lo, hi, off):
    """--keys tokens for steps [lo, hi): held-turn spans + trim edges."""
    turns = [timeline[j][0] for j in range(lo, hi)]
    tokens = []
    for a, b, key in rdw.spans_of_turns(turns):
        tokens += ['--keys', f'{a + lo + off}-{b + lo + off}:{key}']
    prev = 0
    for j in range(lo, hi):
        trim = timeline[j][1]
        if trim != 0:
            assert j - prev >= 2, 'trim edges too close for keysDown'
            key = 'UP' if trim > 0 else 'DOWN'
            tokens += ['--keys', f'{j + off}-{j + off + 2}:{key}']
            prev = j
    return tokens


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', type=int, default=DEFAULT_SEED)
    parser.add_argument('--outdir', default=str(
        REPO / 'games' / 'brineward-nds' / 'proof'))
    args = parser.parse_args()

    timeline, e0, scoop_len, home_len, closed = record(args.seed)
    full = brine.BW_LOOT_DROPS * brine.BW_LOOT_VALUE

    # Alignment robustness, full trace: still banks everything.
    for shift in range(-SHIFT_CHECK, SHIFT_CHECK + 1):
        d, banked = replay(args.seed, timeline, shift)
        if (d.over != brine.BW_DUEL_ENEMY_SUNK or banked != full
                or d.hold != 0 or any(c.live for c in d.loot)):
            sys.exit(f'FAIL: full trace at shift {shift:+d} '
                     f'(over={d.over}, banked={banked}, hold={d.hold})')

    # Alignment robustness, scoop-only prefix: full hold, nothing banked.
    scoop_only = timeline[:e0 + scoop_len]
    for shift in range(-SHIFT_CHECK, SHIFT_CHECK + 1):
        d, banked = replay(args.seed, scoop_only, shift)
        if (d.over != brine.BW_DUEL_ENEMY_SUNK or banked != 0
                or d.hold != brine.BW_LOOT_DROPS
                or d.hold_gold != full or any(c.live for c in d.loot)):
            sys.exit(f'FAIL: scoop trace at shift {shift:+d} '
                     f'(over={d.over}, banked={banked}, hold={d.hold})')

    off = args.seed + 3                  # step j <-> tool frame j + off
    outdir = pathlib.Path(args.outdir)

    # Duel segment: START press + turn spans + edge-fire holds (the same
    # token forms record-duel-win.py emits).
    duel_tokens = ['--keys', f'{args.seed + 2}-{args.seed + 7}:START']
    duel_turns = [timeline[j][0] for j in range(e0)]
    for a, b, key in rdw.spans_of_turns(duel_turns):
        duel_tokens += ['--keys', f'{a + off}-{b + off}:{key}']
    for j in range(e0):
        _, _, fl, fr = timeline[j]
        if fl:
            duel_tokens += ['--keys', f'{j + off}-{j + off + 2}:L']
        if fr:
            duel_tokens += ['--keys', f'{j + off}-{j + off + 2}:R']

    duel_path = outdir / 'salvage-duel-keys.txt'
    scoop_path = outdir / 'salvage-scoop-keys.txt'
    bank_path = outdir / 'salvage-bank-keys.txt'
    duel_path.write_text(' '.join(duel_tokens) + '\n')
    scoop_path.write_text(
        ' '.join(segment_tokens(timeline, e0, e0 + scoop_len, off)) + '\n')
    bank_path.write_text(
        ' '.join(segment_tokens(timeline, e0 + scoop_len,
                                e0 + scoop_len + home_len, off)) + '\n')

    print(f'seed {args.seed}: duel ends at step {e0} (tool frame '
          f'{e0 + off}); scoop takes {scoop_len} steps, homing+bank '
          f'{home_len} more (banked at tool frame ~{len(timeline) + off})')
    print(f'hold after scoop: {brine.BW_LOOT_DROPS} crates {full}g; '
          f'banked {full}g, player hull {closed.player.hull}')
    print(f'alignment margin: both traces hold at every shift in '
          f'[-{SHIFT_CHECK},+{SHIFT_CHECK}]')
    print(f'wrote {duel_path}')
    print(f'wrote {scoop_path}')
    print(f'wrote {bank_path}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
