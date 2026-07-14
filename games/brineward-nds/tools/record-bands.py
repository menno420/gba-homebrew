#!/usr/bin/env python3
"""Brineward deeper-water route recorder (arc slice 7 — danger bands).

Records the arc's first two DEEPER stories on top of the committed
seed-126 duel-win anchor (proof 4's water — water 1 of both stories
reuses games/brineward-nds/proof/duel-win-keys.txt verbatim): after
the band-0 win the sloop drifts the salvage water idle, presses
SELECT (the slice-7 verb) and puts out ONE BAND DEEPER — a band-1
water with a heavier rum-runner (130 hull, 120-frame crews), reefs,
worsened weather, and 12g crates. The committed beam-holding brain
(record-duel-win.py's policy, unchanged) WINS the deeper duel. TWO
independent stories are recorded (each on its own scanned drift gap —
its own SELECT frame, so its own deeper water; the joint story that
holds BOTH continuations at every shift does not exist in the
scannable gap window, measured 2026-07-13):

  bands-deep-keys.txt + bands-bank-keys.txt — the BANK story: the
      committed salvage brain scoops all three 12g crates and banks
      36g at the Graywake pier, grounding on NOTHING, before the Maw
      ever stirs (the wind-story criteria, one band down).
  bands-reef-deep-keys.txt + bands-reef-keys.txt — the REEF story:
      the sloop is sailed deliberately ONTO a rock — one scrape of
      exactly BW_REEF_DMG (the latch holds while the keel crosses the
      rock, ONE toll for the whole contact), clear water re-arms the
      latch, and a second crossing scrapes again (groundings 1 -> 2),
      all before the Maw stirs.

Each *deep* file carries its story's SELECT press plus the deeper
duel win itself.

Frame mapping (the session-20 law, extended to SELECT — the SAME
keysDown edge in the SAME salvage-state handler as the proof-6 START):
for a press spanning [C, C+5) the latched seed of the NEW water is
C-2, and the key at tool frame T drives its duel/salvage step T-(C-1)
— i.e. step j takes the key at tool frame seed2+3+j. Water 1 executes
exactly seed2-seed1-1 steps before the switch.

Each story's gap is scanned so that its water-2 timeline holds
OPEN-LOOP with main.c edge semantics at every alignment shift in
[-6,+6] (re-run against the fixed seed2/carry state — the water-1
anchor's own shift margin is duel-win-keys.txt's, already committed).
Exact literals are pinned from the unshifted run only (session-28
rule).

Usage:
    python3 games/brineward-nds/tools/record-bands.py \
        [--bank-gap N] [--reef-gap N] \
        [--outdir games/brineward-nds/proof]

Deterministic: same gaps -> byte-identical outputs.
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

SEED1 = 126                  # the committed duel-win anchor (proof 4)
BAND = 1                     # the story presses ONE band deeper
GAP_SCAN = range(40, 560)    # idle drift frames after the win, scanned
MAX_FRAMES = 6000
SHIFT_CHECK = 6              # water-2 stories must hold shifted +-this
TRIM_GAP = 3                 # same-key 2-frame holds must never touch
REEF_CLEAR_OUT = 24 * 256    # sail this clear of the rock between passes

BANKED_WANT = brine.BW_LOOT_DROPS * brine.BW_BAND_LOOT_VALUE_OF[BAND]


def water1_state(gap):
    """Water 1 (seed 126, band 0): the committed win + `gap` idle drift
    salvage steps. Returns (duel state at the SELECT edge, win step)."""
    turns, fires, closed = rdw.record(SEED1)
    if closed.over != brine.BW_DUEL_ENEMY_SUNK:
        sys.exit('FAIL: the committed anchor no longer wins — re-derive '
                 'duel-win-keys.txt first')
    e0 = int(closed.frame)               # the WIN step, before the drift
    d = closed
    idle = brine.Inputs()
    for _ in range(gap):
        brine.bw_salvage_step(d, idle)
        brine.bw_dock_step(d)
        if d.over != brine.BW_DUEL_ENEMY_SUNK:
            return None, None
    if d.hold != 0 or d.hold_gold != 0:  # the drift must scoop NOTHING
        return None, None                #   (clean 36g arithmetic below)
    if d.maw.stirs != 0 or d.maw.slain:
        return None, None
    return d, e0


def water2_init(seed2, carry_hull):
    """The deeper water exactly as main.c's begin_duel deals it."""
    d = brine.Duel()
    brine.bw_water_init(d, seed2, BAND)
    d.player.hull = carry_hull           # dents carry (slice-4 rule);
    return d                             # hold/gold carried are 0 by scan


def record_duel2(seed2, carry_hull):
    """Closed-loop policy win of the deeper duel -> (timeline, state).
    Timeline entries are (turn, trim, fire_l, fire_r) with fire flags on
    edge frames only (rdw.record's reload-gate rule, restated here for
    the carried/banded water)."""
    d = water2_init(seed2, carry_hull)
    timeline = []
    while d.over == brine.BW_DUEL_RUNNING and len(timeline) < MAX_FRAMES:
        inp = brine.bw_policy(d.player, d.enemy)
        fl = 1 if inp.fire_l and d.player.reload_l <= 1 else 0
        fr = 1 if inp.fire_r and d.player.reload_r <= 1 else 0
        timeline.append((inp.turn, 0, fl, fr))
        brine.bw_duel_step(d, inp)
    if d.over != brine.BW_DUEL_ENEMY_SUNK or d.player.hull <= 0:
        return None, None
    if d.groundings != 0:                # the duel itself must stay off
        return None, None                #   the rocks (the reef story is
    return timeline, d                   #   the deliberate one)


def continue_bank(d):
    """Scoop + bank continuation (record-wind's brain, one band down)."""
    timeline = []
    last_trim = -TRIM_GAP
    banked = 0
    while banked < BANKED_WANT and len(timeline) < MAX_FRAMES:
        inp = brine.bw_salvage_policy(d)
        trim = inp.trim_delta
        if trim != 0 and len(timeline) - last_trim < TRIM_GAP:
            trim = 0
        if trim != 0:
            last_trim = len(timeline)
        step_in = brine.Inputs(turn=inp.turn, trim_delta=trim)
        timeline.append((inp.turn, trim, 0, 0))
        brine.bw_salvage_step(d, step_in)
        banked += brine.bw_dock_step(d)
        if d.over != brine.BW_DUEL_ENEMY_SUNK:
            break
    return timeline, banked


def continue_reef(d):
    """The deliberate grounding: cross the nearest rock (ONE scrape, the
    latch holding through the whole contact), sail clear, cross back
    (the second scrape). Returns (timeline, rock, contact_frames)."""
    p = d.player
    rock = min((r for r in d.reefs if r.live),
               key=lambda r: brine.bw_chebyshev(p.x, p.y, r.x, r.y))
    timeline = []
    last_trim = -TRIM_GAP

    def step(target_x, target_y):
        inp = brine.bw_goto(d.player, target_x, target_y)
        trim = inp.trim_delta
        if trim != 0 and len(timeline) - last[0] < TRIM_GAP:
            trim = 0
        if trim != 0:
            last[0] = len(timeline)
        step_in = brine.Inputs(turn=inp.turn, trim_delta=trim)
        timeline.append((inp.turn, trim, 0, 0))
        brine.bw_salvage_step(d, step_in)
        brine.bw_dock_step(d)

    last = [last_trim]
    contact = 0
    # pass 1: bow onto the rock until the scrape lands
    while d.groundings == 0 and len(timeline) < MAX_FRAMES:
        step(rock.x, rock.y)
        if d.over != brine.BW_DUEL_ENEMY_SUNK:
            return None, None, None
    # dwell: keep driving THROUGH the rock — the latch must hold for the
    # whole contact (count the frames the keel spends on it)
    while (brine.bw_chebyshev(d.player.x, d.player.y, rock.x, rock.y)
           < brine.BW_REEF_RANGE and len(timeline) < MAX_FRAMES):
        contact += 1
        away_x = d.player.x + (d.player.x - rock.x) * 8
        away_y = d.player.y + (d.player.y - rock.y) * 8
        step(away_x, away_y)
    if d.groundings != 1:
        return None, None, None
    # clear water: open REEF_CLEAR_OUT off the rock so the latch re-arms
    while (brine.bw_chebyshev(d.player.x, d.player.y, rock.x, rock.y)
           < REEF_CLEAR_OUT and len(timeline) < MAX_FRAMES):
        away_x = d.player.x + (d.player.x - rock.x) * 8
        away_y = d.player.y + (d.player.y - rock.y) * 8
        step(away_x, away_y)
        if d.groundings != 1:
            return None, None, None
    # pass 2: back onto the rock — the second scrape
    while d.groundings == 1 and len(timeline) < MAX_FRAMES:
        step(rock.x, rock.y)
        if d.over != brine.BW_DUEL_ENEMY_SUNK:
            return None, None, None
    if d.groundings != 2 or len(timeline) >= MAX_FRAMES:
        return None, None, None
    for _ in range(8):                   # settle a few frames for pins
        step(rock.x, rock.y)
    if d.groundings != 2:                # (still on the rock: latched)
        return None, None, None
    return timeline, rock, contact


def replay2(carry_hull, seed2, timeline, shift=0, slack=60):
    """Open-loop water-2 replay with main.c edge semantics (the duel
    rolls into salvage on its own d.over). Returns (state, banked)."""
    d = water2_init(seed2, carry_hull)
    banked = 0
    for j in range(len(timeline) + shift + slack):
        k = j - shift
        turn, trim, fl, fr = timeline[k] if 0 <= k < len(timeline) \
            else (0, 0, 0, 0)
        inp = brine.Inputs(turn=turn, trim_delta=trim, fire_l=fl,
                           fire_r=fr)
        if d.over == brine.BW_DUEL_RUNNING:
            brine.bw_duel_step(d, inp)
        elif d.over == brine.BW_DUEL_ENEMY_SUNK:
            brine.bw_salvage_step(d, inp)
        else:
            break
        banked += brine.bw_dock_step(d)
    return d, banked


def story_base(gap):
    """Water 1 + the drift + the deeper duel win, shared by both
    stories. None on any failure."""
    w1, e0 = water1_state(gap)
    if w1 is None:
        return None
    seed2 = SEED1 + 1 + e0 + gap         # the SELECT edge latches this
    carry_hull = w1.player.hull
    duel2, dwin = record_duel2(seed2, carry_hull)
    if duel2 is None:
        return None
    win_hull2 = dwin.player.hull
    if win_hull2 <= 2 * brine.BW_REEF_DMG:     # the reef story costs 30
        return None
    return {'gap': gap, 'seed2': seed2, 'e0': e0, 'e2': len(duel2),
            'carry_hull': carry_hull, 'win_hull2': win_hull2,
            'duel2': duel2, 'dwin': dwin}


def record_bank_story(gap):
    """The BANK story for one drift gap; None on any failure."""
    base = story_base(gap)
    if base is None:
        return None
    seed2, e2 = base['seed2'], base['e2']
    duel2 = base['duel2']

    bank = copy.deepcopy(base['dwin'])
    bank_tl, banked = continue_bank(bank)
    if (banked != BANKED_WANT or bank.over != brine.BW_DUEL_ENEMY_SUNK
            or bank.hold != 0 or bank.hold_gold != 0
            or any(c.live for c in bank.loot)
            or bank.groundings != 0 or bank.maw.stirs != 0
            or bank.maw.slain):
        return None
    # the UNSHIFTED run is the pin source (session-28 rule): the water
    # must stay quiet a pin margin past the bank, so t[26] pins 0
    quiet = copy.deepcopy(bank)
    for _ in range(30):
        brine.bw_salvage_step(quiet, brine.Inputs())
    if quiet.maw.stirs != 0:
        return None

    # Story-SKELETON criteria at every shift (session-28 rule; exact
    # literals are pinned from the unshifted run only): the deeper duel
    # is still won surviving, and the continuation still banks exactly
    # the wreck's 36g, grounding on nothing, scooping everything.
    for shift in range(-SHIFT_CHECK, SHIFT_CHECK + 1):
        r, rbanked = replay2(base['carry_hull'], seed2,
                             duel2 + bank_tl, shift)
        if (r.over != brine.BW_DUEL_ENEMY_SUNK or rbanked != BANKED_WANT
                or r.player.hull <= 0 or r.hold != 0 or r.groundings != 0
                or any(c.live for c in r.loot) or r.maw.slain):
            return None

    base.update(bank=bank_tl, bank_end=bank, banked=banked)
    return base


def record_reef_story(gap):
    """The REEF story for one drift gap; None on any failure."""
    base = story_base(gap)
    if base is None:
        return None
    seed2, e2 = base['seed2'], base['e2']
    duel2 = base['duel2']
    win_hull2 = base['win_hull2']

    reef = copy.deepcopy(base['dwin'])
    reef_tl, rock, contact = continue_reef(reef)
    if reef_tl is None:
        return None
    if (reef.maw.stirs != 0 or reef.maw.slain
            or reef.player.hull != win_hull2 - 2 * brine.BW_REEF_DMG
            or reef.player.hull <= 0):
        return None
    if contact < 20:                     # the latch must be SEEN holding
        return None

    # skeleton at every shift: still won, still grounds (>= 1 scrape),
    # still afloat; the exactly-2 literal is the unshifted run's pin
    for shift in range(-SHIFT_CHECK, SHIFT_CHECK + 1):
        r2, _ = replay2(base['carry_hull'], seed2,
                        duel2 + reef_tl, shift)
        if (r2.over != brine.BW_DUEL_ENEMY_SUNK or r2.groundings < 1
                or r2.player.hull <= 0 or r2.maw.slain):
            return None

    base.update(reef=reef_tl, reef_end=reef, rock=rock, contact=contact)
    return base


def emit_segment(timeline, start_step, off, with_trim):
    """--keys tokens for timeline[start_step:] at tool offset `off`."""
    tokens = []
    turns = [t for t, _, _, _ in timeline[start_step:]]
    for a, b, key in rdw.spans_of_turns(turns):
        tokens += ['--keys',
                   f'{a + start_step + off}-{b + start_step + off}:{key}']
    for j in range(start_step, len(timeline)):
        _, trim, fl, fr = timeline[j]
        if fl:
            tokens += ['--keys', f'{j + off}-{j + off + 2}:L']
        if fr:
            tokens += ['--keys', f'{j + off}-{j + off + 2}:R']
        if with_trim and trim != 0:
            key = 'UP' if trim > 0 else 'DOWN'
            tokens += ['--keys', f'{j + off}-{j + off + 2}:{key}']
    return tokens


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--bank-gap', type=int, default=None,
                        help='bank-story drift gap (default: scan)')
    parser.add_argument('--reef-gap', type=int, default=None,
                        help='reef-story drift gap (default: scan)')
    parser.add_argument('--outdir', default=str(
        REPO / 'games' / 'brineward-nds' / 'proof'))
    args = parser.parse_args()

    bank_scan = [args.bank_gap] if args.bank_gap is not None else GAP_SCAN
    bank = None
    for gap in bank_scan:
        bank = record_bank_story(gap)
        if bank:
            break
    if not bank:
        sys.exit('FAIL: no scanned drift gap carries the BANK story at '
                 f'every shift in [-{SHIFT_CHECK},+{SHIFT_CHECK}]')

    reef_scan = [args.reef_gap] if args.reef_gap is not None else GAP_SCAN
    reef = None
    for gap in reef_scan:
        reef = record_reef_story(gap)
        if reef:
            break
    if not reef:
        sys.exit('FAIL: no scanned drift gap carries the REEF story at '
                 f'every shift in [-{SHIFT_CHECK},+{SHIFT_CHECK}]')

    outdir = pathlib.Path(args.outdir)
    files = {}

    def emit_story(st, deep_name, cont_name, cont_tl):
        seed2 = st['seed2']
        off2 = seed2 + 3                 # water-2 step j <-> tool j + off2
        e2 = st['e2']
        deep = ['--keys', f'{seed2 + 2}-{seed2 + 7}:SELECT']
        deep += emit_segment(st['duel2'], 0, off2, False)
        cont = emit_segment(st['duel2'] + cont_tl, e2, off2, True)
        files[deep_name] = deep
        files[cont_name] = cont

    emit_story(bank, 'bands-deep-keys.txt', 'bands-bank-keys.txt',
               bank['bank'])
    emit_story(reef, 'bands-reef-deep-keys.txt', 'bands-reef-keys.txt',
               reef['reef'])
    for name, tokens in files.items():
        (outdir / name).write_text(' '.join(tokens) + '\n')

    print(f'water 1 (both stories): the committed seed-{SEED1} win '
          f'(step {bank["e0"]}), nothing scooped on the drift, hull '
          f'{bank["carry_hull"]} carried')
    print(f'BANK story: drift {bank["gap"]} -> SELECT at tool frame '
          f'{bank["seed2"] + 2}, water 2 = seed {bank["seed2"]}, band '
          f'{BAND} — deeper duel WON at step {bank["e2"]} (tool frame '
          f'~{bank["e2"] + bank["seed2"] + 3}), hull {bank["win_hull2"]}; '
          f'{bank["banked"]}g (3 x {brine.BW_BAND_LOOT_VALUE_OF[BAND]}g '
          f'crates) banked by step {bank["e2"] + len(bank["bank"])}, no '
          'grounding, the Maw never stirred')
    rock = reef['rock']
    print(f'REEF story: drift {reef["gap"]} -> SELECT at tool frame '
          f'{reef["seed2"] + 2}, water 2 = seed {reef["seed2"]}, band '
          f'{BAND} — deeper duel WON at step {reef["e2"]}, hull '
          f'{reef["win_hull2"]}; rock at ({rock.x >> 8},{rock.y >> 8}) '
          f'px — one scrape of {brine.BW_REEF_DMG} despite '
          f'{reef["contact"]} contact frames (the latch), clear water '
          're-arms, second crossing scrapes again (hull '
          f'{reef["win_hull2"]} -> {reef["reef_end"].player.hull}), '
          'the Maw never stirred')
    print(f'alignment margin: both water-2 stories hold at every shift '
          f'in [-{SHIFT_CHECK},+{SHIFT_CHECK}] (story-skeleton criteria; '
          'literals pinned from the unshifted runs)')
    for name in files:
        print(f'wrote {outdir / name}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
