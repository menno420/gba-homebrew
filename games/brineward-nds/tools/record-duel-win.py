#!/usr/bin/env python3
"""Brineward duel-WIN route recorder (arc slice 2).

The concept doc's promised route-recorder v0: runs the beam-holding
player policy (tools/check-brine.py bw_policy — the exact policy proof 4
of the host mirror proves wins) CLOSED-LOOP against the mirrored duel
sim, records the inputs it actually issued, verifies the recorded trace
replays OPEN-LOOP to the same win (edge-triggered fire semantics, exactly
like main.c's keysDown), checks alignment robustness (the trace must
still win when shifted whole frames early/late), and emits the trace as
tools/nds-headless-check.py `--keys` tokens.

Frame mapping (pinned empirically from a telemetry CSV run, 2026-07-11,
and re-asserted by the CI proofs via the latched seed): the game frame
counter lags the tool frame by 2 (same offset Gloamline's proofs
pinned), so START held during tool frames [seed+2, seed+7) latches
`seed`, duel frame d consumes the keypad state of tool frame seed+3+d
(a key held for duel frames [a,b) is `--keys (seed+3+a)-(seed+3+b):NAME`),
and an edge-triggered battery fire at duel frame d is a 2-frame hold.
The default seed 126 was picked by scanning press frames 116..144: its
route wins with the player hull UNTOUCHED at every alignment shift in
[-6,+6] (most seeds' routes are chaos-fragile — a 1-frame shift changes
the whole duel because the enemy AI reacts closed-loop).

Usage:
    python3 games/brineward-nds/tools/record-duel-win.py \
        [--seed 126] [--out games/brineward-nds/proof/duel-win-keys.txt]

Prints the duel-end frame numbers (duel/tool space) and the final hulls,
then writes the keys file with the START press included. Deterministic:
same seed -> byte-identical output.
"""

import argparse
import importlib.util
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parents[3]

spec = importlib.util.spec_from_file_location(
    'check_brine', REPO / 'tools' / 'check-brine.py')
brine = importlib.util.module_from_spec(spec)
spec.loader.exec_module(brine)

DEFAULT_SEED = 126           # see the seed scan note in the docstring
MAX_FRAMES = 3600
SHIFT_CHECK = 6              # trace must still win shifted +-this many


def record(seed):
    """Closed-loop policy run -> (per-frame turn list, fire event list,
    end duel state). Fire events are (duel_frame, 'L'|'R') of shots that
    actually left the battery (reload was 0)."""
    d = brine.Duel()
    brine.bw_duel_init(d, seed)
    turns = []
    fires = []
    while d.over == brine.BW_DUEL_RUNNING and len(turns) < MAX_FRAMES:
        inp = brine.bw_policy(d.player, d.enemy)
        # bw_duel_step decrements reloads (ship step) BEFORE its fire
        # gate, so a battery at pre-step reload <= 1 fires this frame.
        if inp.fire_l and d.player.reload_l <= 1:
            fires.append((len(turns), 'L'))
        if inp.fire_r and d.player.reload_r <= 1:
            fires.append((len(turns), 'R'))
        turns.append(inp.turn)
        brine.bw_duel_step(d, inp)
    return turns, fires, d


def replay(seed, turns, fires, shift=0):
    """Open-loop replay with main.c's edge semantics: fire flags are set
    ONLY on the recorded press frames. shift delays (+) or advances (-)
    every input by whole frames, emulating a broken tool/game alignment."""
    fire_l = {f + shift for f, side in fires if side == 'L'}
    fire_r = {f + shift for f, side in fires if side == 'R'}
    d = brine.Duel()
    brine.bw_duel_init(d, seed)
    for frame in range(MAX_FRAMES):
        if d.over != brine.BW_DUEL_RUNNING:
            break
        idx = frame - shift
        turn = turns[idx] if 0 <= idx < len(turns) else 0
        inp = brine.Inputs(turn=turn,
                           fire_l=1 if frame in fire_l else 0,
                           fire_r=1 if frame in fire_r else 0)
        brine.bw_duel_step(d, inp)
    return d


def spans_of_turns(turns):
    """Consecutive equal nonzero turn values -> (start, end, key) spans."""
    spans = []
    run_start = None
    run_val = 0
    for i, v in enumerate(turns + [0]):
        if v != run_val:
            if run_val != 0:
                spans.append((run_start, i,
                              'RIGHT' if run_val > 0 else 'LEFT'))
            run_start = i
            run_val = v
    return spans


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', type=int, default=DEFAULT_SEED)
    parser.add_argument('--out', default=str(
        REPO / 'games' / 'brineward-nds' / 'proof' / 'duel-win-keys.txt'))
    args = parser.parse_args()

    turns, fires, closed = record(args.seed)
    if closed.over != brine.BW_DUEL_ENEMY_SUNK:
        sys.exit(f'FAIL: policy did not win seed {args.seed} '
                 f'(over={closed.over})')

    open_end = replay(args.seed, turns, fires)
    if (open_end.over, open_end.player.hull, open_end.enemy.hull,
            open_end.frame) != (closed.over, closed.player.hull,
                                closed.enemy.hull, closed.frame):
        sys.exit('FAIL: open-loop replay diverged from the closed-loop '
                 'recording — edge semantics bug')

    margins = []
    for shift in range(-SHIFT_CHECK, SHIFT_CHECK + 1):
        d = replay(args.seed, turns, fires, shift)
        margins.append((shift, d.over, d.player.hull))
        if d.over != brine.BW_DUEL_ENEMY_SUNK or d.player.hull <= 0:
            sys.exit(f'FAIL: route loses at alignment shift {shift:+d} '
                     f'(over={d.over}, hull={d.player.hull})')

    # tool-frame mapping: START at [seed+2, seed+7), duel frame d at
    # tool frame seed+3+d (game counter lags the tool by 2 frames).
    off = args.seed + 3
    tokens = ['--keys', f'{args.seed + 2}-{args.seed + 7}:START']
    for a, b, key in spans_of_turns(turns):
        tokens += ['--keys', f'{a + off}-{b + off}:{key}']
    for f, side in fires:
        tokens += ['--keys', f'{f + off}-{f + off + 2}:{side}']
    pathlib.Path(args.out).write_text(' '.join(tokens) + '\n')

    end_tool = closed.frame + off
    worst_hull = min(h for _, _, h in margins)
    print(f'seed {args.seed}: policy WINS at duel frame {closed.frame} '
          f'(tool frame ~{end_tool}), player hull {closed.player.hull}, '
          f'{len(fires)} broadsides fired')
    print(f'alignment margin: still wins at every shift in '
          f'[-{SHIFT_CHECK},+{SHIFT_CHECK}], worst surviving hull '
          f'{worst_hull}')
    print(f'wrote {args.out}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
