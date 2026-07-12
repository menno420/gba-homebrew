#!/usr/bin/env python3
"""Brineward Maw route recorder (arc slice 5 — the first sea monster).

The Maw sibling of record-salvage.py: records TWO whole stories on one
anchor seed, both starting from the same recorded duel win and playing
out in the salvage water where the Maw stalks:

  the BITE — the bait brain (tools/check-brine.py bw_maw_bait_policy)
      sails straight at the dark water and takes exactly one lunge on
      the chin: shadow telegraph -> surface -> lunge -> bite (the omen
      proof: ignoring the shadow costs BW_MAW_BITE hull), then the Maw
      sounds and the sloop is still afloat.
  the KILL — the hunter brain (bw_maw_hunt_policy, the exact policy
      host proof 16 proves converges) keeps sea room off the shadow,
      rakes the risen Maw beam-on, dodges the lunges, slays it, then
      scoops BOTH carcasses' crates and banks wreck + monster gold
      together at the Graywake pier.

Inputs actually issued are recorded closed-loop, then re-verified
OPEN-LOOP with main.c's edge semantics (trim edges and battery fires
only on the recorded press frames — a fire event is recorded ONLY when
the shot actually left the battery, and trim edges are suppressed
closed-loop until they are >= 3 frames apart so the emitted 2-frame
holds never merge into one keysDown edge), and checked for alignment
robustness: BOTH stories must still play out whole when every input
lands whole frames early/late across the full [-6,+6] window
(story-skeleton criteria; exact hull/gold/position literals are pinned
from the unshifted run only, session-28 rule). Anchor seeds are scanned
in order until one passes everything — the winning seed's duel segment
is emitted alongside the two continuations, so the stories never lean
on another recorder's committed seed choice.

THREE key files come out:

  maw-duel-keys.txt — START + the winning duel for the anchor seed.
  maw-bite-keys.txt — the bait continuation (steps after the sink).
  maw-kill-keys.txt — the hunt + scoop + bank continuation.

Frame mapping (the session-20 law, unchanged): for a START press
spanning [A, A+5) the latched seed is A-2 and the key at tool frame T
drives duel/salvage step T-(A+1) — i.e. step j takes the key at tool
frame seed+3+j. The salvage segment continues the same step counter.

Usage:
    python3 games/brineward-nds/tools/record-maw.py \
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

SEED_SCAN = [127] + list(range(116, 160))    # 127 first: the slice-3 anchor
MAX_FRAMES = 9000
SHIFT_CHECK = 6              # traces must still work shifted +-this many
TRIM_GAP = 3                 # same-key 2-frame holds must never touch


def _duel_timeline(seed):
    """The recorded duel win as a (turn, trim, fire_l, fire_r) timeline."""
    turns, fires, d = rdw.record(seed)
    if d.over != brine.BW_DUEL_ENEMY_SUNK:
        return None, None
    fire_l = {f for f, s in fires if s == 'L'}
    fire_r = {f for f, s in fires if s == 'R'}
    return [(turns[j], 0, 1 if j in fire_l else 0, 1 if j in fire_r else 0)
            for j in range(len(turns))], d


def _continue_with(d, policy, done, cap, banked):
    """Extend a story closed-loop with `policy` until done(d, banked)
    or cap, docking once per frame after the step exactly like main.c.

    Records what actually reached the sim: trim edges are SUPPRESSED
    until TRIM_GAP frames apart (so the emitted key holds keep main.c's
    keysDown edge semantics), and a fire flag is recorded only when the
    battery actually fires (pre-step reload <= 1, the reload having
    already ticked down inside the ship step).
    """
    timeline = []
    last_trim = -TRIM_GAP
    while not done(d, banked) and len(timeline) < cap:
        inp = policy(d)
        trim = inp.trim_delta
        if trim != 0 and len(timeline) - last_trim < TRIM_GAP:
            trim = 0                     # too soon for another keysDown edge
        if trim != 0:
            last_trim = len(timeline)
        fl = 1 if (inp.fire_l and d.maw.state != brine.BW_MAW_DOWN
                   and d.player.reload_l <= 1) else 0
        fr = 1 if (inp.fire_r and d.maw.state != brine.BW_MAW_DOWN
                   and d.player.reload_r <= 1) else 0
        step_in = brine.Inputs(turn=inp.turn, trim_delta=trim,
                               fire_l=inp.fire_l, fire_r=inp.fire_r)
        timeline.append((inp.turn, trim, fl, fr))
        brine.bw_salvage_step(d, step_in)
        banked[0] += brine.bw_dock_step(d)
        if d.over != brine.BW_DUEL_ENEMY_SUNK:
            break                        # sunk mid-story: caller judges
    return timeline


def replay(seed, timeline, shift=0, slack=60):
    """Open-loop replay: duel steps while the duel runs, salvage steps
    after the sink (fire edges DO reach the salvage sim — slice 5's
    main.c passes them and bw_salvage_step gates them on the Maw being
    up), the pier pass every live-water frame. Returns (duel, banked)."""
    d = brine.Duel()
    brine.bw_duel_init(d, seed)
    banked = 0
    for j in range(len(timeline) + shift + slack):
        k = j - shift
        turn, trim, fl, fr = timeline[k] if 0 <= k < len(timeline) \
            else (0, 0, 0, 0)
        inp = brine.Inputs(turn=turn, trim_delta=trim,
                           fire_l=fl, fire_r=fr)
        if d.over == brine.BW_DUEL_RUNNING:
            brine.bw_duel_step(d, inp)
        elif d.over == brine.BW_DUEL_ENEMY_SUNK:
            brine.bw_salvage_step(d, inp)
        else:
            break                        # player sunk: caller judges
        banked += brine.bw_dock_step(d)
    return d, banked


def record_stories(seed):
    """Record both stories for one anchor. Returns None on any failure
    (caller scans on), else a dict of everything the writer needs."""
    import copy
    duel_tl, dwin = _duel_timeline(seed)
    if duel_tl is None:
        return None
    win_hull = dwin.player.hull
    if win_hull <= brine.BW_MAW_BITE:
        return None                      # the bite must not sink the bait

    # --- the BITE: bait until bitten, then watch it sound -------------------
    d = copy.deepcopy(dwin)
    bitten = [None]

    def bite_done(dd, banked):
        if (bitten[0] is None
                and dd.player.hull == win_hull - brine.BW_MAW_BITE):
            bitten[0] = dd.frame
        return (bitten[0] is not None
                and dd.frame >= bitten[0] + 40)     # sounding visible
    bite_banked = [0]
    bite_tl = _continue_with(d, brine.bw_maw_bait_policy, bite_done,
                             MAX_FRAMES, bite_banked)
    if bitten[0] is None or d.over != brine.BW_DUEL_ENEMY_SUNK:
        return None
    if d.maw.slain or d.maw.state != brine.BW_MAW_DOWN or d.maw.stirs != 1:
        return None
    bite_end = copy.deepcopy(d)

    # --- the KILL: hunt, slay, scoop both carcasses, bank -------------------
    full = (brine.BW_LOOT_DROPS * brine.BW_LOOT_VALUE
            + brine.BW_MAW_LOOT_DROPS * brine.BW_MAW_LOOT_VALUE)
    d = copy.deepcopy(dwin)
    kill_banked = [0]

    def kill_done(dd, banked):
        return banked[0] >= full
    kill_tl = _continue_with(d, brine.bw_maw_hunt_policy, kill_done,
                             MAX_FRAMES, kill_banked)
    if (kill_banked[0] != full or d.over != brine.BW_DUEL_ENEMY_SUNK
            or not d.maw.slain or d.player.hull <= 0
            or any(c.live for c in d.loot) or d.hold != 0):
        return None
    kill_end = d

    # --- alignment robustness: both stories whole at every shift ------------
    for shift in range(-SHIFT_CHECK, SHIFT_CHECK + 1):
        r, banked = replay(seed, duel_tl + bite_tl, shift)
        if (r.over != brine.BW_DUEL_ENEMY_SUNK or banked != 0
                or r.player.hull != win_hull - brine.BW_MAW_BITE
                or r.maw.slain or r.maw.state != brine.BW_MAW_DOWN
                or r.maw.stirs != 1 or r.hold != bite_end.hold):
            return None
        r, banked = replay(seed, duel_tl + kill_tl, shift, slack=200)
        if (r.over != brine.BW_DUEL_ENEMY_SUNK or banked != full
                or not r.maw.slain or r.player.hull <= 0
                or any(c.live for c in r.loot) or r.hold != 0):
            return None

    return {'seed': seed, 'duel': duel_tl, 'bite': bite_tl,
            'kill': kill_tl, 'win_hull': win_hull,
            'bite_frame': bitten[0], 'bite_end': bite_end,
            'kill_end': kill_end, 'full': full}


def segment_tokens(timeline, lo, hi, off):
    """--keys tokens for steps [lo, hi): held-turn spans, trim edges,
    and 2-frame fire holds (the fire events are pre-spaced by reload)."""
    turns = [timeline[j][0] for j in range(lo, hi)]
    tokens = []
    for a, b, key in rdw.spans_of_turns(turns):
        tokens += ['--keys', f'{a + lo + off}-{b + lo + off}:{key}']
    prev = -TRIM_GAP
    for j in range(lo, hi):
        trim = timeline[j][1]
        if trim != 0:
            assert j - prev >= TRIM_GAP, 'trim edges too close for keysDown'
            key = 'UP' if trim > 0 else 'DOWN'
            tokens += ['--keys', f'{j + off}-{j + off + 2}:{key}']
            prev = j
    for j in range(lo, hi):
        _, _, fl, fr = timeline[j]
        if fl:
            tokens += ['--keys', f'{j + off}-{j + off + 2}:L']
        if fr:
            tokens += ['--keys', f'{j + off}-{j + off + 2}:R']
    return tokens


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', type=int, default=None,
                        help='anchor seed (default: scan)')
    parser.add_argument('--outdir', default=str(
        REPO / 'games' / 'brineward-nds' / 'proof'))
    args = parser.parse_args()

    scan = [args.seed] if args.seed is not None else SEED_SCAN
    story = None
    for seed in scan:
        story = record_stories(seed)
        if story:
            break
    if not story:
        sys.exit('FAIL: no scanned seed carries both Maw stories at '
                 f'every shift in [-{SHIFT_CHECK},+{SHIFT_CHECK}]')

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

    duel_path = outdir / 'maw-duel-keys.txt'
    bite_path = outdir / 'maw-bite-keys.txt'
    kill_path = outdir / 'maw-kill-keys.txt'
    duel_path.write_text(' '.join(duel_tokens) + '\n')
    bite = story['duel'] + story['bite']
    kill = story['duel'] + story['kill']
    bite_path.write_text(
        ' '.join(segment_tokens(bite, e0, len(bite), off)) + '\n')
    kill_path.write_text(
        ' '.join(segment_tokens(kill, e0, len(kill), off)) + '\n')

    be, ke = story['bite_end'], story['kill_end']
    print(f'seed {seed}: duel ends at step {e0} (tool frame {e0 + off}), '
          f'win hull {story["win_hull"]}')
    print(f'BITE story: bitten at step {story["bite_frame"]} (tool frame '
          f'~{story["bite_frame"] + off}), hull '
          f'{story["win_hull"]} -> {be.player.hull}, Maw sounded, '
          f'story ends at step {e0 + len(story["bite"])}')
    print(f'KILL story: Maw slain, {story["full"]}g banked (wreck + '
          f'monster crates), player hull {ke.player.hull}, story ends '
          f'at step {e0 + len(story["kill"])} (tool frame '
          f'~{e0 + len(story["kill"]) + off})')
    print(f'alignment margin: both stories hold at every shift in '
          f'[-{SHIFT_CHECK},+{SHIFT_CHECK}]')
    for p in (duel_path, bite_path, kill_path):
        print(f'wrote {p}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
