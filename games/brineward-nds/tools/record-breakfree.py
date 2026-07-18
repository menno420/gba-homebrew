#!/usr/bin/env python3
"""Brineward break-free route recorder (bestiary cut 2 — the ram/brace B verb).

The Grasper recorder's sibling. Records ONE whole story on the SAME
grasper-water anchor the seize recorder uses (record-grasper.py): the
committed duel win, then the victor LINGERS over the wreck and the arms
rise, reach, and SEIZE her — and this time, a few frames into the hold,
the player BRACES (the reserved B verb, cut 2) and WRENCHES LOOSE early.
The wrench costs exactly BW_GRASPER_BRACE_HULL hull, ONCE, and the arms
slip exactly BW_GRASPER_BRACE_FRAMES frames later — a hold cut short well
under the full BW_GRASPER_HOLD_FRAMES. This is the cut-2 lesson made a
route: a seized sloop is not doomed to wait out the whole hold; B buys
the way back, at a price.

One key file comes out:

  grasper-breakfree-keys.txt — START + the winning duel for the anchor
      (byte-identical to grasper-duel-keys.txt's duel), then ONE B press
      during the hold: the wrench. The seize itself needs no input (the
      arms come to the lingering sloop, as in the seize route); only the
      break is a keypress.

The pinned seize/brace/release frames it prints are the numbers the ROM
proof in .github/workflows/rom-builds.yml asserts against bw_telemetry
(the mirror predicts them; the ROM build is byte-deterministic, so the
mirror model and the emulator agree — the lane's pin rule).

Frame mapping (the session-20 law, unchanged): for a START press spanning
[seed+2, seed+7) the latched seed is seed and the key at tool frame T
drives duel/salvage step T-(seed+3) — step j <-> tool frame j+seed+3. The
salvage segment continues the same step counter; the B press is emitted at
the wrench step's tool frame.

The anchor and its winning duel are taken from record-grasper.record_story
so this route never leans on a private seed choice — it wrenches the SAME
sloop the seize route pins.

Usage:
    python3 games/brineward-nds/tools/record-breakfree.py \
        [--seed N] [--lead K] [--outdir games/brineward-nds/proof]

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


rg = _load('record_grasper',
           'games/brineward-nds/tools/record-grasper.py')
brine = rg.brine

DEFAULT_LEAD = 10           # idle hold frames before the wrench (early brace)
MAX_STEPS = 2000            # salvage frames to watch for seize + release


def _wrench_facts(seed, duel_tl, seize_step, lead):
    """Replay the duel open-loop, then idle salvage until the seize, brace
    ONCE at step seize_step+lead, and ride out to the early release.
    Returns the frame-exact break facts (or None on any deviation)."""
    _, dwin = rg._duel_timeline(seed)
    if dwin is None or dwin.over != brine.BW_DUEL_ENEMY_SUNK:
        return None
    d = copy.deepcopy(dwin)
    win_hull = d.player.hull
    brace_step = seize_step + lead
    facts = {'seed': seed, 'win_hull': win_hull, 'brace_step': brace_step,
             'lead': lead}
    for _ in range(MAX_STEPS):
        press = 1 if d.frame == brace_step else 0
        prev = d.grasper.state
        prev_hull = d.player.hull
        brine.bw_salvage_step(d, brine.Inputs(brace=press))
        if d.over != brine.BW_DUEL_ENEMY_SUNK:
            return None                  # sunk mid-linger: not this story
        g = d.grasper
        if prev == brine.BW_GRASPER_REACH and g.state == brine.BW_GRASPER_HOLD:
            facts['seize_step'] = d.frame - 1
            facts['seize_hull'] = d.player.hull
            if d.player.hull != win_hull - brine.BW_GRASPER_GRAB_BITE:
                return None
        if press:
            # the wrench must be a real hold frame that shortens it
            if prev != brine.BW_GRASPER_HOLD:
                return None
            facts['brace_hull_before'] = prev_hull
            facts['brace_hull_after'] = d.player.hull
            if d.player.hull != prev_hull - brine.BW_GRASPER_BRACE_HULL:
                return None
            if g.timer != (brine.BW_GRASPER_HOLD_FRAMES
                           - brine.BW_GRASPER_BRACE_FRAMES):
                return None
        if 'seize_step' in facts and prev == brine.BW_GRASPER_HOLD \
                and g.state == brine.BW_GRASPER_DOWN:
            facts['release_step'] = d.frame - 1
            break
    if not {'seize_step', 'release_step', 'brace_hull_after'} <= facts.keys():
        return None
    # the break really is faster than waiting the hold out
    facts['braced_hold'] = facts['release_step'] - facts['seize_step']
    if facts['braced_hold'] >= brine.BW_GRASPER_HOLD_FRAMES:
        return None
    # and the arms slip exactly BRACE_FRAMES after the wrench step
    if facts['release_step'] - facts['brace_step'] \
            != brine.BW_GRASPER_BRACE_FRAMES:
        return None
    return facts


def _duel_tokens(seed, duel_tl):
    """The START + winning-duel key tokens — byte-identical to the seize
    route's duel segment (record-grasper's emission shape)."""
    off = seed + 3
    tokens = ['--keys', f'{seed + 2}-{seed + 7}:START']
    duel_turns = [t for t, _, _, _ in duel_tl]
    for a, b, key in rg.rdw.spans_of_turns(duel_turns):
        tokens += ['--keys', f'{a + off}-{b + off}:{key}']
    for j, (_, _, fl, fr) in enumerate(duel_tl):
        if fl:
            tokens += ['--keys', f'{j + off}-{j + off + 2}:L']
        if fr:
            tokens += ['--keys', f'{j + off}-{j + off + 2}:R']
    return tokens


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', type=int, default=None,
                        help='anchor seed (default: the seize route anchor)')
    parser.add_argument('--lead', type=int, default=DEFAULT_LEAD,
                        help='idle hold frames before the wrench')
    parser.add_argument('--outdir', default=str(
        REPO / 'games' / 'brineward-nds' / 'proof'))
    args = parser.parse_args()

    if args.seed is not None:
        scan = [args.seed]
    else:
        scan = [s for s in range(4096) if brine.bw_has_grasper(s)]

    story = facts = None
    for seed in scan:
        story = rg.record_story(seed)
        if not story:
            continue
        facts = _wrench_facts(seed, story['duel'], story['seize_step'],
                              args.lead)
        if facts:
            break
    if not facts:
        sys.exit('FAIL: no grasper-water anchor carries a clean braced '
                 'break at this lead')

    seed = facts['seed']
    off = seed + 3
    brace_frame = facts['brace_step'] + off
    tokens = _duel_tokens(seed, story['duel'])
    # the wrench: ONE B press at the brace step (a 3-frame span; the
    # emulator's keysDown edge bites only the first frame, so the sim sees
    # exactly one brace input — the recorder's own replay confirms it)
    tokens += ['--keys', f'{brace_frame}-{brace_frame + 2}:B']

    outdir = pathlib.Path(args.outdir)
    out = outdir / 'grasper-breakfree-keys.txt'
    out.write_text(' '.join(tokens) + '\n')

    seize_frame = facts['seize_step'] + off
    release_frame = facts['release_step'] + off
    print(f'seed {seed} (grasper water): duel ends at step '
          f"{story['duel_len']}, win hull {facts['win_hull']}")
    print(f"SEIZE: step {facts['seize_step']} (tool frame {seize_frame}), "
          f"hull {facts['win_hull']} -> {facts['seize_hull']} "
          f"(exactly {brine.BW_GRASPER_GRAB_BITE})")
    print(f"WRENCH (B): step {facts['brace_step']} (tool frame "
          f"{brace_frame}), hull {facts['brace_hull_before']} -> "
          f"{facts['brace_hull_after']} (exactly "
          f"{brine.BW_GRASPER_BRACE_HULL})")
    print(f"BROKE FREE: step {facts['release_step']} (tool frame "
          f"{release_frame}), braced hold {facts['braced_hold']} frames "
          f"(< {brine.BW_GRASPER_HOLD_FRAMES}; the arms slip "
          f"{brine.BW_GRASPER_BRACE_FRAMES} after the wrench)")
    print(f'wrote {out}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
