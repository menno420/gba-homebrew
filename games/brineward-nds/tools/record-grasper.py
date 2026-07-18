#!/usr/bin/env python3
"""Brineward Grasper route recorder (bestiary cut 1 — the SECOND monster).

The Maw's sibling recorder. Records ONE whole story on a GRASPER-water
anchor seed (bw_has_grasper(seed) == 1 — a water that holds the Grasper
instead of the Maw): the committed duel win, then the SEIZE — the victor
lingers over the wreck (pure idle: no salvage input at all), the arms
rise out of the deep, reach under the sloop, and SEIZE her — one seize's
worth of hull, then the sloop is PINNED STILL (position frozen, no way)
for the whole hold, and the arms slip and reach again. This is the cut-1
lesson made a route: in a grasper water, LINGERING is how you get held —
the escape (cut 1's, spatial) is sea room, and the idle victor has none.

Two key files come out:

  grasper-duel-keys.txt  — START + the winning duel for the anchor seed.
  grasper-seize-keys.txt — the salvage continuation (pure idle: NO keys,
      the demonstration needs none — the arms come to a lingering sloop).

The pinned seize/hold/release frames it prints are the numbers the ROM
proof in .github/workflows/rom-builds.yml asserts against bw_telemetry
(the mirror predicts them; the ROM build is byte-deterministic, so the
mirror model and the emulator agree — the lane's pin rule).

Frame mapping (the session-20 law, unchanged): for a START press
spanning [seed+2, seed+7) the latched seed is seed and the key at tool
frame T drives duel/salvage step T-(seed+3) — step j <-> tool frame
j+seed+3. The salvage segment continues the same step counter.

Anchor seeds are scanned in grasper-water order until one carries the
whole story at every input shift in [-6,+6]; the winning seed's duel is
emitted alongside the idle continuation, so the story never leans on
another recorder's committed seed choice.

Usage:
    python3 games/brineward-nds/tools/record-grasper.py \
        [--seed N] [--outdir games/brineward-nds/proof]

Deterministic: same seed -> byte-identical outputs.
"""

import argparse
import copy
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

MAX_SEIZE_FRAMES = 1400     # idle salvage frames to watch for the seize
SHIFT_CHECK = 6             # the story must hold shifted +-this many
MIN_WIN_HULL = brine.BW_GRASPER_GRAB_BITE + 40   # room for a seize + margin


def _duel_timeline(seed):
    """The recorded duel win as a (turn, trim, fire_l, fire_r) timeline
    plus the end Duel — or (None, None) if the policy does not win."""
    turns, fires, d = rdw.record(seed)
    if d.over != brine.BW_DUEL_ENEMY_SUNK:
        return None, None
    fire_l = {f for f, s in fires if s == 'L'}
    fire_r = {f for f, s in fires if s == 'R'}
    tl = [(turns[j], 0, 1 if j in fire_l else 0, 1 if j in fire_r else 0)
          for j in range(len(turns))]
    return tl, d


def _scan_seize(d):
    """From a duel-ended (ENEMY_SUNK) state, step IDLE salvage until the
    FIRST seize -> hold -> release; return the seize facts, or None if no
    clean seize plays out. The pin (held STILL) is asserted only once the
    hold branch is pinning (g.timer >= 1 — the seize frame itself still
    carries the sloop's last way, zeroed the next frame)."""
    win_hull = d.player.hull
    seize_step = release_step = seize_hull = None
    held = 0
    idle = brine.Inputs()
    for _ in range(MAX_SEIZE_FRAMES):
        prev = d.grasper.state
        brine.bw_salvage_step(d, idle)
        if d.over != brine.BW_DUEL_ENEMY_SUNK:
            return None                  # sunk mid-linger: not this story
        g = d.grasper
        if prev == brine.BW_GRASPER_REACH and g.state == brine.BW_GRASPER_HOLD:
            seize_step = d.frame - 1
            seize_hull = d.player.hull
            if seize_hull != win_hull - brine.BW_GRASPER_GRAB_BITE:
                return None              # a seize costs exactly GRAB_BITE
        if g.state == brine.BW_GRASPER_HOLD:
            held += 1
            if g.timer >= 1 and (d.player.x != g.gx
                                 or d.player.y != g.gy or d.player.speed):
                return None              # not held STILL: not this story
        if seize_step is not None and prev == brine.BW_GRASPER_HOLD \
                and g.state == brine.BW_GRASPER_DOWN:
            release_step = d.frame - 1
            break
    if seize_step is None or release_step is None \
            or held != brine.BW_GRASPER_HOLD_FRAMES:
        return None
    return {'win_hull': win_hull, 'seize_step': seize_step,
            'release_step': release_step, 'held': held,
            'seize_hull': seize_hull}


def _replay(seed, duel_tl, shift, slack=200):
    """Open-loop: replay the duel at `shift`, then scan IDLE salvage for
    the first seize. Returns the shift's seize facts (or None)."""
    d = brine.Duel()
    brine.bw_duel_init(d, seed)
    limit = len(duel_tl) + abs(shift) + slack
    for j in range(limit):
        if d.over != brine.BW_DUEL_RUNNING:
            break
        k = j - shift
        turn, trim, fl, fr = duel_tl[k] if 0 <= k < len(duel_tl) \
            else (0, 0, 0, 0)
        brine.bw_duel_step(d, brine.Inputs(turn=turn, trim_delta=trim,
                                           fire_l=fl, fire_r=fr))
    if d.over != brine.BW_DUEL_ENEMY_SUNK:
        return None
    return _scan_seize(d)


def record_story(seed):
    """The whole story for one anchor, or None on any failure."""
    duel_tl, dwin = _duel_timeline(seed)
    if duel_tl is None or dwin.player.hull < MIN_WIN_HULL:
        return None
    facts = _scan_seize(copy.deepcopy(dwin))
    if facts is None:
        return None
    # alignment robustness (the story-skeleton rule, session-28): the
    # seize must still land whole — exactly one GRAB_BITE, a full hold —
    # at every shift, though the exact win hull may drift off shift 0.
    for shift in range(-SHIFT_CHECK, SHIFT_CHECK + 1):
        r = _replay(seed, duel_tl, shift)
        if r is None or r['held'] != brine.BW_GRASPER_HOLD_FRAMES:
            return None
    facts['seed'] = seed
    facts['duel'] = duel_tl
    facts['duel_len'] = len(duel_tl)
    return facts


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', type=int, default=None,
                        help='anchor seed (default: scan grasper waters)')
    parser.add_argument('--outdir', default=str(
        REPO / 'games' / 'brineward-nds' / 'proof'))
    args = parser.parse_args()

    if args.seed is not None:
        scan = [args.seed]
    else:
        scan = [s for s in range(4096) if brine.bw_has_grasper(s)]
    story = None
    for seed in scan:
        story = record_story(seed)
        if story:
            break
    if not story:
        sys.exit('FAIL: no grasper-water seed carries the seize story at '
                 f'every shift in [-{SHIFT_CHECK},+{SHIFT_CHECK}]')

    seed = story['seed']
    off = seed + 3                       # step j <-> tool frame j + off
    outdir = pathlib.Path(args.outdir)

    # duel keys: START + the winning duel (record-maw's emission shape)
    duel_tokens = ['--keys', f'{seed + 2}-{seed + 7}:START']
    duel_turns = [t for t, _, _, _ in story['duel']]
    for a, b, key in rdw.spans_of_turns(duel_turns):
        duel_tokens += ['--keys', f'{a + off}-{b + off}:{key}']
    for j, (_, _, fl, fr) in enumerate(story['duel']):
        if fl:
            duel_tokens += ['--keys', f'{j + off}-{j + off + 2}:L']
        if fr:
            duel_tokens += ['--keys', f'{j + off}-{j + off + 2}:R']

    duel_path = outdir / 'grasper-duel-keys.txt'
    seize_path = outdir / 'grasper-seize-keys.txt'
    duel_path.write_text(' '.join(duel_tokens) + '\n')
    seize_path.write_text('\n')          # pure idle: no continuation keys

    seize_frame = story['seize_step'] + off
    release_frame = story['release_step'] + off
    print(f'seed {seed} (grasper water): duel ends at step '
          f"{story['duel_len']}, win hull {story['win_hull']}")
    print(f"SEIZE: step {story['seize_step']} (tool frame {seize_frame}), "
          f"hull {story['win_hull']} -> {story['seize_hull']} "
          f"(exactly {brine.BW_GRASPER_GRAB_BITE})")
    print(f"HELD STILL: {story['held']} frames "
          f"(== BW_GRASPER_HOLD_FRAMES), release step "
          f"{story['release_step']} (tool frame {release_frame})")
    print(f'alignment margin: the seize holds at every shift in '
          f'[-{SHIFT_CHECK},+{SHIFT_CHECK}]')
    for p in (duel_path, seize_path):
        print(f'wrote {p}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
