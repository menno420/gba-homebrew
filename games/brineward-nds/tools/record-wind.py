#!/usr/bin/env python3
"""Brineward wind route recorder (arc slice 6 — wind + sailing feel).

Records ONE whole story on a GALE-weather anchor seed: the committed
beam-holding duel brain (record-duel-win.py's policy) WINS a broadside
duel with both ships sailing a rotating gale, then the committed
salvage brain (tools/check-brine.py bw_salvage_policy) scoops all
three crates and banks them at the Graywake pier — the entire slice-3
loop, played out in weather, and DONE before the Maw ever stirs (the
story leaves the salvage water quiet: no committed key may poke the
slice-5 telegraph).

Anchor seeds are scanned in order over the gale bucket only (the
weather is pure f(seed), so the scan simply skips calm and breeze
waters); the emitted seed's whole story must hold OPEN-LOOP with
main.c's edge semantics at every alignment shift in [-6,+6]
(story-skeleton criteria — win, survive, bank exactly the wreck's
15g, hold empty, Maw never stirred; exact literals are pinned from
the unshifted run only, session-28 rule).

TWO key files come out:

  wind-duel-keys.txt — START + the winning gale duel for the anchor.
  wind-bank-keys.txt — the scoop + bank continuation after the sink.

Frame mapping (the session-20 law, unchanged): for a START press
spanning [A, A+5) the latched seed is A-2 and the key at tool frame T
drives duel/salvage step T-(A+1) — i.e. step j takes the key at tool
frame seed+3+j.

Usage:
    python3 games/brineward-nds/tools/record-wind.py \
        [--seed N] [--outdir games/brineward-nds/proof]

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

SEED_SCAN = range(120, 400)  # gale waters only (non-gale seeds skipped)
MAX_FRAMES = 6000
SHIFT_CHECK = 6              # the story must hold shifted +-this many
TRIM_GAP = 3                 # same-key 2-frame holds must never touch

FULL = brine.BW_LOOT_DROPS * brine.BW_LOOT_VALUE


def _duel_timeline(seed):
    """The recorded gale duel win as a (turn, trim, fl, fr) timeline."""
    turns, fires, d = rdw.record(seed)
    if d.over != brine.BW_DUEL_ENEMY_SUNK or d.player.hull <= 0:
        return None, None
    fire_l = {f for f, s in fires if s == 'L'}
    fire_r = {f for f, s in fires if s == 'R'}
    return [(turns[j], 0, 1 if j in fire_l else 0, 1 if j in fire_r else 0)
            for j in range(len(turns))], d


def _continue_bank(d):
    """Extend the win closed-loop with the salvage brain until the
    wreck's gold is banked (or the cap trips). Trim edges are
    SUPPRESSED until TRIM_GAP frames apart so the emitted 2-frame
    holds keep main.c's keysDown edge semantics; the salvage brain
    never fires (and the quiet-water batteries are cold anyway)."""
    timeline = []
    last_trim = -TRIM_GAP
    banked = 0
    while banked < FULL and len(timeline) < MAX_FRAMES:
        inp = brine.bw_salvage_policy(d)
        trim = inp.trim_delta
        if trim != 0 and len(timeline) - last_trim < TRIM_GAP:
            trim = 0                     # too soon for another keysDown edge
        if trim != 0:
            last_trim = len(timeline)
        step_in = brine.Inputs(turn=inp.turn, trim_delta=trim)
        timeline.append((inp.turn, trim, 0, 0))
        brine.bw_salvage_step(d, step_in)
        banked += brine.bw_dock_step(d)
        if d.over != brine.BW_DUEL_ENEMY_SUNK:
            break                        # sunk mid-story: caller judges
    return timeline, banked


def replay(seed, timeline, shift=0, slack=60):
    """Open-loop replay with main.c edge semantics; returns (d, banked)."""
    d = brine.Duel()
    brine.bw_duel_init(d, seed)
    banked = 0
    for j in range(len(timeline) + shift + slack):
        k = j - shift
        turn, trim, fl, fr = timeline[k] if 0 <= k < len(timeline) \
            else (0, 0, 0, 0)
        inp = brine.Inputs(turn=turn, trim_delta=trim, fire_l=fl, fire_r=fr)
        if d.over == brine.BW_DUEL_RUNNING:
            brine.bw_duel_step(d, inp)
        elif d.over == brine.BW_DUEL_ENEMY_SUNK:
            brine.bw_salvage_step(d, inp)
        else:
            break                        # player sunk: caller judges
        banked += brine.bw_dock_step(d)
    return d, banked


def record_story(seed):
    """Record the gale story for one anchor; None on any failure."""
    probe = brine.Duel()
    brine.bw_duel_init(probe, seed)
    if probe.wind_level != brine.BW_WIND_GALE:
        return None                      # this slice's story sails a GALE
    duel_tl, dwin = _duel_timeline(seed)
    if duel_tl is None:
        return None
    win_hull = dwin.player.hull

    import copy
    d = copy.deepcopy(dwin)
    bank_tl, banked = _continue_bank(d)
    if (banked != FULL or d.over != brine.BW_DUEL_ENEMY_SUNK
            or d.hold != 0 or d.hold_gold != 0
            or any(c.live for c in d.loot)):
        return None
    # the story must leave the water QUIET: banked and done before the
    # Maw's patience runs out, telegraph never poked
    if d.maw.stirs != 0 or d.maw.state != brine.BW_MAW_DOWN or d.maw.slain:
        return None

    for shift in range(-SHIFT_CHECK, SHIFT_CHECK + 1):
        r, rbanked = replay(seed, duel_tl + bank_tl, shift)
        if (r.over != brine.BW_DUEL_ENEMY_SUNK or rbanked != FULL
                or r.player.hull <= 0 or r.hold != 0
                or any(c.live for c in r.loot)
                or r.maw.stirs != 0 or r.maw.slain):
            return None

    return {'seed': seed, 'duel': duel_tl, 'bank': bank_tl,
            'win_hull': win_hull, 'end': d, 'banked': banked}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', type=int, default=None,
                        help='anchor seed (default: scan the gale bucket)')
    parser.add_argument('--outdir', default=str(
        REPO / 'games' / 'brineward-nds' / 'proof'))
    args = parser.parse_args()

    scan = [args.seed] if args.seed is not None else SEED_SCAN
    story = None
    for seed in scan:
        story = record_story(seed)
        if story:
            break
    if not story:
        sys.exit('FAIL: no scanned gale seed carries the story at every '
                 f'shift in [-{SHIFT_CHECK},+{SHIFT_CHECK}]')

    seed = story['seed']
    off = seed + 3                       # step j <-> tool frame j + off
    e0 = len(story['duel'])
    outdir = pathlib.Path(args.outdir)

    duel_tokens = ['--keys', f'{seed + 2}-{seed + 7}:START']
    duel_turns = [t for t, _, _, _ in story['duel']]
    for a, b, key in rdw.spans_of_turns(duel_turns):
        duel_tokens += ['--keys', f'{a + off}-{b + off}:{key}']
    for j, (_, _, fl, fr) in enumerate(story['duel']):
        if fl:
            duel_tokens += ['--keys', f'{j + off}-{j + off + 2}:L']
        if fr:
            duel_tokens += ['--keys', f'{j + off}-{j + off + 2}:R']

    whole = story['duel'] + story['bank']
    bank_tokens = []
    turns = [whole[j][0] for j in range(e0, len(whole))]
    for a, b, key in rdw.spans_of_turns(turns):
        bank_tokens += ['--keys', f'{a + e0 + off}-{b + e0 + off}:{key}']
    prev = -TRIM_GAP
    for j in range(e0, len(whole)):
        trim = whole[j][1]
        if trim != 0:
            assert j - prev >= TRIM_GAP, 'trim edges too close for keysDown'
            key = 'UP' if trim > 0 else 'DOWN'
            bank_tokens += ['--keys', f'{j + off}-{j + off + 2}:{key}']
            prev = j

    duel_path = outdir / 'wind-duel-keys.txt'
    bank_path = outdir / 'wind-bank-keys.txt'
    duel_path.write_text(' '.join(duel_tokens) + '\n')
    bank_path.write_text(' '.join(bank_tokens) + '\n')

    end = story['end']
    print(f'seed {seed}: GALE water (wind base '
          f'{end.wind_base}), duel won at step {e0} '
          f'(tool frame {e0 + off}), win hull {story["win_hull"]}')
    print(f'BANK story: {story["banked"]}g banked at the pier by step '
          f'{len(whole)} (tool frame ~{len(whole) + off}), hold empty, '
          'the Maw never stirred')
    print(f'alignment margin: the whole story holds at every shift in '
          f'[-{SHIFT_CHECK},+{SHIFT_CHECK}]')
    for p in (duel_path, bank_path):
        print(f'wrote {p}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
