#!/usr/bin/env python3
"""Brineward ambush-survived route recorder (bestiary cut 3 — the ambush).

The break-free recorder's sibling. Records ONE whole story on an AMBUSH-
water anchor seed (bw_grasper_ambush(seed) == 1 — a grasper water whose
HOLDs spring converging cutters): the committed duel win, then the victor
LINGERS over the wreck and the arms rise, reach, and SEIZE her — and the
seize springs the trap, BW_CUTTER_COUNT light cutters closing on the pin.
A few frames into the hold the player BRACES (the cut-2 B verb) and
WRENCHES LOOSE early — the hold ends BEFORE any cutter reaches bite range,
so the cutters never bite and DISPERSE: «ambush survived». The wrench
still costs exactly BW_GRASPER_BRACE_HULL hull ONCE; the extra
BW_CUTTER_COUNT x BW_CUTTER_BITE the pin WOULD have cost is dodged whole.
This is cut 3's lesson made a route: a full hold is lethal, but the braced
break is the counter — end the hold before the cutters close.

One key file comes out:

  ambush-survived-keys.txt — START + the winning duel for the anchor
      (a full duel-win prefix, byte-identical in shape to the grasper
      recorders' duel), then ONE B press during the hold: the wrench that
      ends the ambush before it bites. The seize itself needs no input
      (the arms come to the lingering sloop); only the break is a keypress.

The pinned seize/brace/release frames it prints are the numbers the ROM
proof in .github/workflows/rom-builds.yml asserts against bw_telemetry
(the mirror predicts them; the ROM build is byte-deterministic, so the
mirror model and the emulator agree — the lane's pin rule). The cutters
carry no telemetry word of their own; the ambush is witnessed through the
player hull (BW_T_PHULL) NOT dropping past the seize + brace cost, and the
grasper state word (BW_T_GRASPSTATE) — the survived route pins them both.

Frame mapping (the session-20 law, unchanged): for a START press spanning
[seed+2, seed+7) the latched seed is seed and the key at tool frame T
drives duel/salvage step T-(seed+3) — step j <-> tool frame j+seed+3. The
salvage segment continues the same step counter; the B press is emitted at
the wrench step's tool frame.

The anchor and its winning duel come from record-grasper.record_story so
this route never leans on a private seed choice — it survives the SAME
sloop the seize/break-free routes pin, only in an ambush water.

Usage:
    python3 games/brineward-nds/tools/record-ambush.py \
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

DEFAULT_LEAD = 8            # idle hold frames before the wrench (brace EARLY,
                           #   well before the first cutter reaches the pin)
MAX_STEPS = 2000           # salvage frames to watch for seize + release


def _ambush_facts(seed, seize_step, lead):
    """Replay the duel open-loop, then idle salvage until the seize (which
    springs the cutters), brace ONCE at step seize_step+lead, and ride out
    to the early release. Prove the ambush is REAL (cutters spawned) and
    SURVIVED (the hold ends before any cutter bites; cutters dispersed).
    Returns the frame-exact facts, or None on any deviation."""
    _, dwin = rg._duel_timeline(seed)
    if dwin is None or dwin.over != brine.BW_DUEL_ENEMY_SUNK:
        return None
    if not dwin.ambush_water:
        return None                      # not an ambush water: not this story
    d = copy.deepcopy(dwin)
    win_hull = d.player.hull
    brace_step = seize_step + lead
    facts = {'seed': seed, 'win_hull': win_hull, 'brace_step': brace_step,
             'lead': lead}
    bites = 0
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
            # the seize sprang the trap: every cutter spawned at its offset
            for i in range(brine.BW_CUTTER_COUNT):
                c = g.cutters[i]
                if c.bit or (c.x == 0 and c.y == 0):
                    return None
        # a cutter bite would drop hull mid-hold with NO brace this frame —
        # the survived route must take ZERO cutter bites
        if prev == brine.BW_GRASPER_HOLD and not press \
                and d.player.hull < prev_hull:
            bites += 1
        if press:
            if prev != brine.BW_GRASPER_HOLD:
                return None              # the wrench must be a real hold frame
            facts['brace_hull_before'] = prev_hull
            facts['brace_hull_after'] = d.player.hull
            if d.player.hull != prev_hull - brine.BW_GRASPER_BRACE_HULL:
                return None              # exactly one brace cost, no cutter bite
            if g.timer != (brine.BW_GRASPER_HOLD_FRAMES
                           - brine.BW_GRASPER_BRACE_FRAMES):
                return None
        if 'seize_step' in facts and prev == brine.BW_GRASPER_HOLD \
                and g.state == brine.BW_GRASPER_DOWN:
            facts['release_step'] = d.frame - 1
            # dispersed: the cutters are all zeroed at release
            for c in g.cutters:
                if c.x != 0 or c.y != 0 or c.bit != 0:
                    return None
            break
    if not {'seize_step', 'release_step', 'brace_hull_after'} <= facts.keys():
        return None
    if bites != 0:
        return None                      # a cutter bit: the ambush was NOT survived
    facts['bites'] = bites
    # the break really is faster than waiting the (lethal) hold out
    facts['braced_hold'] = facts['release_step'] - facts['seize_step']
    if facts['braced_hold'] >= brine.BW_GRASPER_HOLD_FRAMES:
        return None
    # the arms slip exactly BRACE_FRAMES after the wrench step
    if facts['release_step'] - facts['brace_step'] \
            != brine.BW_GRASPER_BRACE_FRAMES:
        return None
    # the hull dodged: the pin WOULD have cost this much more unbraced
    facts['dodged'] = brine.BW_CUTTER_COUNT * brine.BW_CUTTER_BITE
    return facts


def _duel_tokens(seed, duel_tl):
    """The START + winning-duel key tokens — the grasper recorders' shape."""
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
                        help='anchor seed (default: scan ambush waters)')
    parser.add_argument('--lead', type=int, default=DEFAULT_LEAD,
                        help='idle hold frames before the wrench')
    parser.add_argument('--outdir', default=str(
        REPO / 'games' / 'brineward-nds' / 'proof'))
    args = parser.parse_args()

    if args.seed is not None:
        scan = [args.seed]
    else:
        scan = [s for s in range(4096) if brine.bw_grasper_ambush(s)]

    story = facts = None
    for seed in scan:
        story = rg.record_story(seed)
        if not story:
            continue
        facts = _ambush_facts(seed, story['seize_step'], args.lead)
        if facts:
            break
    if not facts:
        sys.exit('FAIL: no ambush-water anchor carries a clean survived '
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
    out = outdir / 'ambush-survived-keys.txt'
    out.write_text(' '.join(tokens) + '\n')

    seize_frame = facts['seize_step'] + off
    release_frame = facts['release_step'] + off
    print(f'seed {seed} (ambush water): duel ends at step '
          f"{story['duel_len']}, win hull {facts['win_hull']}")
    print(f"SEIZE + AMBUSH: step {facts['seize_step']} (tool frame "
          f"{seize_frame}), hull {facts['win_hull']} -> {facts['seize_hull']} "
          f"(exactly {brine.BW_GRASPER_GRAB_BITE}); "
          f"{brine.BW_CUTTER_COUNT} cutters spring and start closing")
    print(f"WRENCH (B): step {facts['brace_step']} (tool frame "
          f"{brace_frame}), hull {facts['brace_hull_before']} -> "
          f"{facts['brace_hull_after']} (exactly "
          f"{brine.BW_GRASPER_BRACE_HULL})")
    print(f"AMBUSH SURVIVED: step {facts['release_step']} (tool frame "
          f"{release_frame}), braced hold {facts['braced_hold']} frames "
          f"(< {brine.BW_GRASPER_HOLD_FRAMES}); {facts['bites']} cutter "
          f"bites, {facts['dodged']} hull dodged, the cutters dispersed")
    print(f'wrote {out}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
