#!/usr/bin/env python3
"""Brineward «bigger hold» route recorder (bestiary cut 4 — the hold track).

The salvage recorder's sibling for the crate-cap economy. It records ONE
whole story — win the duel, then run the salvage policy to scoop the wreck
and bank at the Graywake pier — and then PROVES the hold-track payoff on
that exact route: a sloop coming home ALREADY carrying a running haul meets
the wreck's crates, and the total is more than the STOCK hold can take. A
stock hold (tier 0) fills to BW_HOLD_CAP and must abandon the last crate
afloat; a bought hold (tier 1) carries the whole deep haul home and banks
strictly more gold. That is the concept's "deeper, richer water is worth
the hold to carry it home" made a route.

One key file comes out:

  bighold-keys.txt — START + the winning duel + the salvage scoop + the
      pier bank for the anchor seed (the SAME token forms record-salvage.py
      emits; a deterministic, replayable artifact the CI sync-diff pins).

HONEST SCOPE (the cut-4 limit, stated plainly): the crate SUPPLY is
untouched by this cut — a natural water sheds at most BW_LOOT_DROPS crates
(a monster's carcass a few more), never more than the stock hold. So the
"deep haul" here is manufactured the only honest way a host recorder can:
the sloop is given a PRE-CARRIED running haul at the salvage start (as if
it had been scooping across earlier waters), which the sim's per-water
hold-reset cannot itself express — so this payoff is HOST-VERIFIED (the
sibling of cut 3's host-proven «lethal» route), not driven as a >8-crate
ROM bank. The ROM witnesses the hold-track ECONOMY the ROM-honest way:
rom-builds.yml buys the hold upgrade at the port and pins BW_T_HOLDCAP
8 -> 12. The natural loot-scaling that would let an ordinary deep water
overflow the stock hold is a clean follow-up beat.

Frame mapping (the session-20 law, unchanged): for a START press spanning
[seed+2, seed+7) the latched seed is seed and the key at tool frame T
drives duel/salvage step T-(seed+3) — step j <-> tool frame j+seed+3.

Usage:
    python3 games/brineward-nds/tools/record-bighold.py \
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


rs = _load('record_salvage', 'games/brineward-nds/tools/record-salvage.py')
brine = rs.brine

DEFAULT_SEED = 127           # record-salvage's shift-stable anchor
# a running haul already aboard when the wreck's crates come up: enough
# that carried + the wreck drops overruns the stock hold by one crate
CARRIED = brine.BW_HOLD_CAP - brine.BW_LOOT_DROPS + 1


def replay_haul(seed, timeline, e0, hold_tier):
    """Replay the recorded duel+salvage story with a PRE-CARRIED haul and
    a chosen hold tier injected at the salvage start. Returns
    (banked, crates_left_afloat) after the whole route."""
    d = brine.Duel()
    brine.bw_duel_init(d, seed)
    d.up_hold = hold_tier
    injected = False
    banked = 0
    for j in range(len(timeline)):
        turn, trim, fl, fr = timeline[j]
        if d.over == brine.BW_DUEL_RUNNING:
            brine.bw_duel_step(
                d, brine.Inputs(turn=turn, trim_delta=trim,
                                fire_l=fl, fire_r=fr))
        elif d.over == brine.BW_DUEL_ENEMY_SUNK:
            if not injected:                 # first salvage frame: load the
                d.hold = CARRIED             #   running haul the sloop carries
                d.hold_gold = CARRIED * brine.BW_LOOT_VALUE
                injected = True
            brine.bw_salvage_step(
                d, brine.Inputs(turn=turn, trim_delta=trim))
        else:
            break
        banked += brine.bw_dock_step(d)
    left = sum(1 for c in d.loot if c.live)
    return banked, left, d


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', type=int, default=DEFAULT_SEED)
    parser.add_argument('--outdir', default=str(
        REPO / 'games' / 'brineward-nds' / 'proof'))
    args = parser.parse_args()

    timeline, e0, scoop_len, home_len, closed = rs.record(args.seed)

    # The payoff, on the recorded route: a STOCK hold (tier 0) caps the
    # deep haul at BW_HOLD_CAP and leaves crates afloat; a bought hold
    # (tier 1) carries it all home and banks strictly more.
    stock_bank, stock_left, _ = replay_haul(args.seed, timeline, e0, 0)
    big_bank, big_left, _ = replay_haul(args.seed, timeline, e0, 1)

    demand = CARRIED + brine.BW_LOOT_DROPS
    stock_want = brine.BW_HOLD_CAP * brine.BW_LOOT_VALUE
    big_want = min(demand, brine.BW_UP_HOLD_OF[1]) * brine.BW_LOOT_VALUE
    if stock_bank != stock_want:
        sys.exit(f'FAIL: stock hold banked {stock_bank}, want {stock_want}')
    if big_bank != big_want:
        sys.exit(f'FAIL: bigger hold banked {big_bank}, want {big_want}')
    if not (big_bank > stock_bank):
        sys.exit('FAIL: the bigger hold did not carry more home')
    if not (stock_left > big_left):
        sys.exit('FAIL: the stock hold did not leave crates the big one took')

    # Emit the route (the same token shape record-salvage.py commits):
    # START + winning duel, then the scoop, then the homing bank — one
    # deterministic file the sync-diff pins.
    off = args.seed + 3
    duel_tokens = ['--keys', f'{args.seed + 2}-{args.seed + 7}:START']
    duel_turns = [timeline[j][0] for j in range(e0)]
    for a, b, key in rs.rdw.spans_of_turns(duel_turns):
        duel_tokens += ['--keys', f'{a + off}-{b + off}:{key}']
    for j in range(e0):
        _, _, fl, fr = timeline[j]
        if fl:
            duel_tokens += ['--keys', f'{j + off}-{j + off + 2}:L']
        if fr:
            duel_tokens += ['--keys', f'{j + off}-{j + off + 2}:R']
    scoop_bank = rs.segment_tokens(
        timeline, e0, e0 + scoop_len + home_len, off)
    tokens = duel_tokens + scoop_bank

    outdir = pathlib.Path(args.outdir)
    out = outdir / 'bighold-keys.txt'
    out.write_text(' '.join(tokens) + '\n')

    print(f'seed {args.seed}: duel ends at step {e0}, salvage scoops the '
          f'{brine.BW_LOOT_DROPS}-crate wreck onto a {CARRIED}-crate '
          f'running haul ({demand} crates of demand)')
    print(f'STOCK hold (tier 0, cap {brine.BW_HOLD_CAP}): banks '
          f'{stock_bank}g, LEAVES {stock_left} crate(s) afloat — the deep '
          'haul the stock hold could not carry')
    print(f'BIGGER hold (tier 1, cap {brine.BW_UP_HOLD_OF[1]}): banks '
          f'{big_bank}g, leaves {big_left} — the whole haul home, '
          f'{big_bank - stock_bank}g more than the stock hold')
    print(f'wrote {out}')
    return 0


if __name__ == '__main__':
    sys.exit(main())