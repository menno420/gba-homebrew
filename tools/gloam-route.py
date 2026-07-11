#!/usr/bin/env python3
"""Gloamline route tool: derive + verify survive routes on the mirror sim.

Slice 3 derived its survive-to-dawn kiting route with a scratch script; its
session card told slice 4 to promote the pattern into tools/ — this is that
tool. It carries a FULL-GAME mirror: the pure functions from
tools/check-gloam.py (import, not copy — the lockstep rule stays one file)
plus a line-faithful Python mirror of the main.c state machine (title /
playing / dead / dawn, seed latch = frame counter at START, wave spawns,
shove + stun + cooldown, contact, dawn countdown), driven by the same
`--keys START-END:NAME` spans tools/nds-headless-check.py replays.

Emulator/ROM alignment (session-17 guard): the py-desmume frontend's frame
N sets input that the ROM's loop iteration sees ~2 frames later (ROM frame
f = emu frame e - 2 — that is how the pinned seed 118 falls out of a START
press at emu 120), with occasional single-frame skips WITHIN a run. The
envelope the slice-3 scratch proved and this tool enforces: keep the START
presses (and therefore the latched seed + spawn schedule) fixed, shift
every MOVEMENT span by every delta in +-6 frames, and require the route to
survive ALL of them with the distance margin — so a mid-run frame skip
cannot turn a green proof into a corpse. (A GLOBAL alignment change would
shift the latched seed itself and is caught loudly by the pinned seed
asserts in CI, not defended here.)

Usage:
  # verify a route file: survives N nights at every movement skew?
  python3 tools/gloam-route.py verify --keys-file FILE --nights N
      [--skew 6] [--min-dist 20] [--probe EMUFRAME ...]

  # derive a route that survives nights 1..N, write the keys file
  python3 tools/gloam-route.py derive --nights N --out FILE

`--probe E` prints the mirror's predicted telemetry after emulator frame E
(nominal alignment) — the numbers a pinned --assert-watch should expect.

Stdlib-only, no emulator needed: this is the host half of the evidence;
the headless replay of the derived route on the real ROM is the other.
"""

import argparse
import importlib.util
import pathlib
import sys

_HERE = pathlib.Path(__file__).resolve().parent
_spec = importlib.util.spec_from_file_location('check_gloam',
                                               _HERE / 'check-gloam.py')
cg = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(cg)

NOMINAL_OFFSET = 2          # emu frame ~= rom frame + 2 (session-17 pin)
CONTACT_PX = cg.GL_CONTACT_RANGE // cg.GL_ONE

STATE_TITLE, STATE_PLAYING, STATE_DEAD, STATE_DAWN = 0, 1, 2, 3

DIR_KEYS = {'UP', 'DOWN', 'LEFT', 'RIGHT'}


def parse_keys_specs(specs):
    """['START-END:NAME', ...] -> [(start, end, NAME), ...] (emu frames)."""
    spans = []
    for spec in specs:
        span, _, name = spec.partition(':')
        start, _, end = span.partition('-')
        spans.append((int(start), int(end), name.upper()))
    return spans


def keys_line(spans):
    return ' '.join(f'--keys {start}-{end}:{name}'
                    for start, end, name in spans)


class GloamSim:
    """Line-faithful mirror of games/gloamline-nds/source/main.c."""

    def __init__(self, spans, offset=NOMINAL_OFFSET):
        self.spans = spans
        self.offset = offset
        self.state = STATE_TITLE
        self.frame = 0                       # mirrors main.c `frame`
        self.deaths = 0
        self.nights_survived = 0
        self.shoves = 0
        self.pad_seen_idle = False
        self.prev_held = frozenset()
        # run state (mirrors Run)
        self.seed = 0
        self.night = 0
        self.dawn_left = cg.GL_NIGHT_FRAMES
        self.run_frame = 0
        self.px = self.py = 0
        self.zx, self.zy, self.zstun = [], [], []
        self.wave_total = 0
        self.shove_cd = 0
        # evidence
        self.min_dist = None                 # over all PLAYING frames
        self.dawn_emu_frames = []            # emu frame of each dawn flip

    def held_timeline(self, end_emu_frame):
        """Precomputed emu frame -> frozenset of held key names."""
        held = [set() for _ in range(end_emu_frame + 1)]
        for start, end, name in self.spans:
            for emu in range(max(start, 0), min(end, end_emu_frame + 1)):
                held[emu].add(name)
        return [frozenset(h) for h in held]

    def start_night(self):
        self.dawn_left = cg.GL_NIGHT_FRAMES
        self.run_frame = 0
        self.px, self.py = cg.GL_PLAYER_START_X, cg.GL_PLAYER_START_Y
        self.zx, self.zy, self.zstun = [], [], []
        self.wave_total = cg.gl_wave_count(self.night)
        self.shove_cd = 0
        self.spawn_due()

    def spawn_due(self):
        while (len(self.zx) < self.wave_total
               and cg.gl_spawn_frame(self.night,
                                     len(self.zx)) <= self.run_frame):
            x, y = cg.gl_spawn_of_night(self.seed, self.night, len(self.zx))
            self.zx.append(x)
            self.zy.append(y)
            self.zstun.append(0)

    def start_run(self, seed):
        self.seed = seed
        self.night = 1
        self.start_night()

    def nearest(self):
        best, best_i = None, 0
        for i in range(len(self.zx)):
            d = cg.gl_chebyshev(self.px, self.py, self.zx[i], self.zy[i])
            if best is None or d < best:
                best, best_i = d, i
        return best_i, (best if best is not None else 0)

    def step(self, emu_frame, held):
        """One main-loop iteration (the ROM iteration run at emu_frame)."""
        self.frame += 1
        down = held - self.prev_held
        self.prev_held = held
        if not held:
            self.pad_seen_idle = True
        start = self.pad_seen_idle and 'START' in down

        if self.state == STATE_TITLE:
            if start:
                self.start_run(self.frame)
                self.state = STATE_PLAYING
        elif self.state == STATE_PLAYING:
            self.spawn_due()
            self.px, self.py = cg.gl_player_step(
                self.px, self.py, 'UP' in held, 'DOWN' in held,
                'LEFT' in held, 'RIGHT' in held)
            if self.shove_cd > 0:
                self.shove_cd -= 1
            if 'A' in down and self.shove_cd == 0:
                self.shove_cd = cg.GL_SHOVE_COOLDOWN
                i, _dist = self.nearest()
                if self.zx:
                    hit, self.zx[i], self.zy[i] = cg.gl_shove(
                        self.px, self.py, self.zx[i], self.zy[i])
                    if hit:
                        self.zstun[i] = cg.GL_SHOVE_STUN
                        self.shoves += 1
            for i in range(len(self.zx)):
                if self.zstun[i] > 0:
                    self.zstun[i] -= 1
                else:
                    self.zx[i], self.zy[i] = cg.gl_shambler_step(
                        self.zx[i], self.zy[i], self.px, self.py, i,
                        self.run_frame)
            self.run_frame += 1
            touched = any(
                cg.gl_contact(self.px, self.py, self.zx[i], self.zy[i])
                for i in range(len(self.zx)))
            _i, dist = self.nearest()
            if self.min_dist is None or dist < self.min_dist:
                self.min_dist = dist
            if touched:
                self.deaths += 1
                self.state = STATE_DEAD
            else:
                self.dawn_left -= 1
                if self.dawn_left == 0:
                    self.nights_survived = self.night
                    self.state = STATE_DAWN
                    self.dawn_emu_frames.append(emu_frame)
        elif self.state == STATE_DEAD:
            if start:
                self.start_run(self.frame)
                self.state = STATE_PLAYING
        elif self.state == STATE_DAWN:
            if start:
                self.night += 1
                self.start_night()
                self.state = STATE_PLAYING

    def run(self, end_emu_frame, probes=(), stop_on_death=True):
        probes = set(probes)
        held = self.held_timeline(end_emu_frame)
        for emu in range(end_emu_frame + 1):
            if emu - self.offset < 1:
                continue                     # boot frames before the loop
            self.step(emu, held[emu])
            if emu in probes:
                self.probe(emu)
            if stop_on_death and self.state == STATE_DEAD:
                return

    def probe(self, emu):
        _i, dist = self.nearest()
        print(f'  probe emu {emu}: state {self.state} night {self.night} '
              f'seed {self.seed} run_frame {self.run_frame} '
              f'z {len(self.zx)}/{self.wave_total} dist {dist} '
              f'({dist / cg.GL_ONE:.1f} px) shove_cd {self.shove_cd} '
              f'shoves {self.shoves} nights {self.nights_survived} '
              f'deaths {self.deaths} px {self.px} py {self.py}')


def skewed(spans, delta):
    """Shift every non-START span by delta frames (START anchors stay)."""
    return [(start, end, name) if name == 'START'
            else (start + delta, end + delta, name)
            for start, end, name in spans]


def verify(spans, nights, end, skew, min_dist_px, probes=()):
    """Simulate every movement skew in +-skew; return (ok, report_lines)."""
    lines = []
    ok = True
    for delta in range(-skew, skew + 1):
        sim = GloamSim(skewed(spans, delta), NOMINAL_OFFSET)
        sim.run(end + skew, probes=probes if delta == 0 else ())
        survived = (sim.deaths == 0 and sim.nights_survived >= nights)
        dist_px = (sim.min_dist or 0) / cg.GL_ONE
        verdict = 'ok' if survived and dist_px >= min_dist_px else 'FAIL'
        if verdict == 'FAIL':
            ok = False
        lines.append(
            f'{verdict}: skew {delta:+d} seed {sim.seed} -> '
            f'nights {sim.nights_survived} deaths {sim.deaths} '
            f'min dist {dist_px:.1f} px (floor {min_dist_px})')
    return ok, lines


# --- kiting autopilot ---------------------------------------------------------
# A fixed racetrack survives night 1 but blunders into night 2+'s mid-night
# spawns, so the deriver plans against the exact schedule instead: every
# `replan` frames it scores 9 candidate headings (8 compass + idle) by a
# `horizon`-frame rollout of the real pursuit (spawns included) and keeps
# the one that maximizes the worst zombie distance, with a mild preference
# for staying off the fence and for its current heading (no dithering).
# The +-skew verifier then proves the derived inputs are margin-robust.

HEADINGS = [frozenset(), frozenset({'UP'}), frozenset({'DOWN'}),
            frozenset({'LEFT'}), frozenset({'RIGHT'}),
            frozenset({'UP', 'LEFT'}), frozenset({'UP', 'RIGHT'}),
            frozenset({'DOWN', 'LEFT'}), frozenset({'DOWN', 'RIGHT'})]
WALL_WEIGHT = 0.3
WALL_CAP = 40 * cg.GL_ONE
HYSTERESIS = 64                 # score bonus for keeping the heading


def rollout_score(sim, held, horizon):
    """Worst zombie distance over a rollout of `held`, + wall/zombie terms."""
    px, py = sim.px, sim.py
    zx, zy = list(sim.zx), list(sim.zy)
    zstun = list(sim.zstun)
    rf = sim.run_frame
    up, down = 'UP' in held, 'DOWN' in held
    left, right = 'LEFT' in held, 'RIGHT' in held
    worst = None
    for _ in range(horizon):
        while (len(zx) < sim.wave_total
               and cg.gl_spawn_frame(sim.night, len(zx)) <= rf):
            x, y = cg.gl_spawn_of_night(sim.seed, sim.night, len(zx))
            zx.append(x)
            zy.append(y)
            zstun.append(0)
        px, py = cg.gl_player_step(px, py, up, down, left, right)
        for i in range(len(zx)):
            if zstun[i] > 0:
                zstun[i] -= 1
            else:
                zx[i], zy[i] = cg.gl_shambler_step(zx[i], zy[i], px, py, i,
                                                   rf)
        rf += 1
        dist = min(cg.gl_chebyshev(px, py, zx[i], zy[i])
                   for i in range(len(zx)))
        if worst is None or dist < worst:
            worst = dist
    wall = min(px - cg.GL_ARENA_X_MIN, cg.GL_ARENA_X_MAX - px,
               py - cg.GL_ARENA_Y_MIN, cg.GL_ARENA_Y_MAX - py)
    return worst + WALL_WEIGHT * min(wall, WALL_CAP)


def autopilot(prefix, anchor, replan, horizon):
    """Steer one night from `anchor`; return this night's movement spans."""
    sim = GloamSim(prefix, NOMINAL_OFFSET)
    timeline = sim.held_timeline(anchor - 1)
    for emu in range(anchor):
        if emu - sim.offset >= 1:
            sim.step(emu, timeline[emu])
    if sim.state != STATE_PLAYING:
        return None

    held = frozenset()
    per_frame = []
    emu = anchor
    while sim.state == STATE_PLAYING and len(per_frame) < 4000:
        if len(per_frame) % replan == 0:
            best, best_score = held, None
            for candidate in HEADINGS:
                score = rollout_score(sim, candidate, horizon)
                if candidate == held:
                    score += HYSTERESIS
                if best_score is None or score > best_score:
                    best, best_score = candidate, score
            held = best
        sim.step(emu, held)
        per_frame.append((emu, held))
        emu += 1
    if sim.state != STATE_DAWN:
        return None

    spans = []                   # compress per key: maximal held runs
    for key in sorted({key for _e, h in per_frame for key in h}):
        run_start = None
        for frame, h in per_frame:
            if key in h and run_start is None:
                run_start = frame
            elif key not in h and run_start is not None:
                spans.append((run_start, frame, key))
                run_start = None
        if run_start is not None:
            spans.append((run_start, per_frame[-1][0] + 1, key))
    return sorted(spans)


def derive(nights, skew, min_dist_px):
    """Build a multi-night route night by night; return spans or None."""
    spans = [(120, 125, 'START')]
    anchor = 121
    for night in range(1, nights + 1):
        for replan, horizon in ((8, 48), (4, 64), (8, 80), (6, 96)):
            night_spans = autopilot(spans, anchor, replan, horizon)
            if night_spans is None:
                continue
            trial = spans + night_spans
            end = max(end for _s, end, _n in trial) + 200
            ok, lines = verify(trial, night, end, skew, min_dist_px)
            if ok:
                print(f'night {night}: autopilot (replan {replan}, '
                      f'horizon {horizon}) survives every movement skew')
                for line in lines:
                    print(f'  {line}')
                spans = trial
                break
        else:
            print(f'night {night}: NO autopilot setting survives every '
                  'skew')
            return None
        if night == nights:
            break
        # Nominal-alignment dawn anchors the next night's START press.
        sim = GloamSim(spans, NOMINAL_OFFSET)
        sim.run(max(end for _s, end, _n in spans) + 200)
        if len(sim.dawn_emu_frames) < night:
            return None
        press = sim.dawn_emu_frames[night - 1] + 80     # slack for skew
        spans = spans + [(press, press + 5, 'START')]
        anchor = press + 1
    return spans


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('mode', choices=['verify', 'derive'])
    parser.add_argument('--keys-file', help='route file of --keys spans')
    parser.add_argument('--keys', action='append', default=[],
                        metavar='START-END:NAME', help='extra span(s)')
    parser.add_argument('--nights', type=int, required=True,
                        help='nights the route must survive')
    parser.add_argument('--end', type=int,
                        help='emu frames to simulate (default: derived)')
    parser.add_argument('--skew', type=int, default=6,
                        help='movement-span skew envelope in frames '
                             '(default 6 = the slice-3 +-6 guard)')
    parser.add_argument('--min-dist', type=float, default=20.0,
                        help='required min player<->zombie distance in px '
                             '(contact is 10; default 20 = 10 px margin)')
    parser.add_argument('--probe', action='append', type=int, default=[],
                        metavar='EMUFRAME',
                        help='print predicted telemetry after this emu '
                             'frame (nominal alignment); repeatable')
    parser.add_argument('--out', help='derive: write the route file here')
    args = parser.parse_args()

    if args.mode == 'derive':
        spans = derive(args.nights, args.skew, args.min_dist)
        if spans is None:
            sys.exit('FAIL: no route found')
        line = keys_line(spans)
        if args.out:
            pathlib.Path(args.out).write_text(line + '\n',
                                              encoding='ascii')
            print(f'route written: {args.out}')
        else:
            print(line)
        return

    specs = list(args.keys)
    if args.keys_file:
        specs += pathlib.Path(args.keys_file).read_text().split('--keys')
        specs = [spec.strip() for spec in specs if spec.strip()]
    spans = parse_keys_specs(specs)
    end = args.end or max(end for _s, end, _n in spans) + 400
    ok, lines = verify(spans, args.nights, end, args.skew, args.min_dist,
                       probes=args.probe)
    for line in lines:
        print(line)
    if not ok:
        sys.exit('FAIL: route does not survive every movement skew with '
                 'margin')
    print(f'PASS: route survives {args.nights} night(s) at every movement '
          f'skew +-{args.skew} with min dist >= {args.min_dist} px')


if __name__ == '__main__':
    main()
