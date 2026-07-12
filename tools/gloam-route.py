#!/usr/bin/env python3
"""Gloamline route tool: derive + verify survive routes on the mirror sim.

Slice 3 derived its survive-to-dawn kiting route with a scratch script; its
session card told slice 4 to promote the pattern into tools/ — this is that
tool. It carries a FULL-GAME mirror: the pure functions from
tools/check-gloam.py (import, not copy — the lockstep rule stays one file)
plus a line-faithful Python mirror of the main.c state machine (title /
playing / dead / dawn / scavenge, seed latch = frame counter at START,
wave spawns, shove + stun + cooldown, barricades — B place/repair,
blocked steps chew 1 hp, breach, plank stock + dawn grant, persistence
across nights — the slice-6 scavenge interlude — SELECT at the dawn
card recenters the player, returns the dead to their spawn points,
scatters the pure cache schedule; grabs pocket planks, never on a full
pocket; START leaves early; the timer starts the next night; taking
the interlude replaces the flat dawn grant — contact, dawn countdown —
and the slice-7 lantern oil: the lantern burns GL_OIL_BURN per NIGHT
frame, below GL_OIL_LOW the light radius gutters and the dark press
cancels the stagger of every Shambler beyond the lamplight; oil flasks
in the interlude top the tank up, never wasted on a full one; daylight
burns nothing and presses nobody — and the slice-8 synthesized-audio
DECISION mirror: which one-shot cue fires each frame (counter deltas +
state flips, highest cue id wins) and which ambience tier the drone
plays, so audio telemetry pins are mirror-predicted like everything
else), driven by the same `--keys START-END:NAME` spans
tools/nds-headless-check.py replays.

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
STATE_SCAVENGE = 4

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
        # barricades (slice 5): fixed slots, hp 0 = free/breached;
        # persist across nights within a run
        self.bx = [0] * cg.GL_BARRICADE_CAP
        self.by = [0] * cg.GL_BARRICADE_CAP
        self.bhp = [0] * cg.GL_BARRICADE_CAP
        self.planks = 0
        self.places = 0
        self.repairs = 0
        self.breaches = 0
        # scavenge interlude (slice 6): caches on the ground + the
        # dawn-light clock; live only inside the interlude
        self.cx = [0] * cg.GL_CACHE_COUNT
        self.cy = [0] * cg.GL_CACHE_COUNT
        self.cache_up = [0] * cg.GL_CACHE_COUNT
        self.scav_left = 0
        self.scavenged = 0
        self.scavs = 0
        # lantern oil (slice 7): the tank + the interlude's flasks
        self.oil = 0
        self.fx = [0] * cg.GL_FLASK_COUNT
        self.fy = [0] * cg.GL_FLASK_COUNT
        self.flask_up = [0] * cg.GL_FLASK_COUNT
        self.oil_grabs = 0
        # synthesized audio (slice 8): the DECISION mirror — which cue
        # fires when, which ambience tier the drone plays. Playback is
        # ARM7/hardware-side and mirrors nothing back.
        self.amb_on = False
        self.amb_tier = 0
        self.amb_flips = 0
        self.cue_left = 0
        self.last_cue = 0
        self.cues = 0
        self.cue_frame = 0
        # evidence
        self.min_dist = None                 # over PLAYING+SCAVENGE frames
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
        # scavenge state is night-inert (mirrors main.c start_night)
        self.cache_up = [0] * cg.GL_CACHE_COUNT
        self.flask_up = [0] * cg.GL_FLASK_COUNT
        self.scav_left = 0
        self.spawn_due()

    def begin_scavenge(self):
        """Mirror of main.c begin_scavenge()."""
        self.px, self.py = cg.GL_PLAYER_START_X, cg.GL_PLAYER_START_Y
        for i in range(len(self.zx)):
            self.zx[i], self.zy[i] = cg.gl_spawn_of_night(self.seed,
                                                          self.night, i)
            self.zstun[i] = 0
        for i in range(cg.GL_CACHE_COUNT):
            self.cx[i], self.cy[i] = cg.gl_cache_of_interlude(self.seed,
                                                              self.night, i)
            self.cache_up[i] = 1
        for i in range(cg.GL_FLASK_COUNT):
            self.fx[i], self.fy[i] = cg.gl_flask_of_interlude(self.seed,
                                                              self.night, i)
            self.flask_up[i] = 1
        self.scav_left = cg.GL_SCAVENGE_FRAMES
        self.shove_cd = 0

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
        self.planks = cg.GL_PLANK_STOCK
        self.oil = cg.GL_OIL_MAX                 # a fresh lantern
        self.bhp = [0] * cg.GL_BARRICADE_CAP     # fresh run: bare yard
        self.start_night()

    def nearest(self):
        best, best_i = None, 0
        for i in range(len(self.zx)):
            d = cg.gl_chebyshev(self.px, self.py, self.zx[i], self.zy[i])
            if best is None or d < best:
                best, best_i = d, i
        return best_i, (best if best is not None else 0)

    def shove_verb(self, down):
        """Mirror of main.c do_shove_verb()."""
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

    def barricade_verb(self, down):
        """Mirror of main.c do_barricade_verb()."""
        if 'B' in down and self.planks > 0:
            best_b, near_b = None, -1
            for i in range(cg.GL_BARRICADE_CAP):
                if self.bhp[i] == 0:
                    continue
                d = cg.gl_chebyshev(self.px, self.py,
                                    self.bx[i], self.by[i])
                if best_b is None or d < best_b:
                    best_b, near_b = d, i
            if near_b >= 0 and best_b <= cg.GL_BARRICADE_RANGE:
                if self.bhp[near_b] < cg.GL_BARRICADE_HP:
                    self.bhp[near_b] = cg.GL_BARRICADE_HP
                    self.planks -= 1
                    self.repairs += 1
            else:
                for i in range(cg.GL_BARRICADE_CAP):
                    if self.bhp[i] == 0:
                        self.bx[i] = self.px
                        self.by[i] = self.py
                        self.bhp[i] = cg.GL_BARRICADE_HP
                        self.planks -= 1
                        self.places += 1
                        break

    def step_the_dead(self, oil_for_press):
        """Mirror of main.c step_the_dead() (incl. the run_frame tick).

        oil_for_press: the oil level the dark press sees — the real
        tank at night (PLAYING), GL_OIL_MAX in the daylight interlude
        (the dawn light presses nobody).
        """
        for i in range(len(self.zx)):
            if self.zstun[i] > 0:
                self.zstun[i] -= 1
                continue
            nx, ny = self.zx[i], self.zy[i]
            if (not cg.gl_shambler_staggers(i, self.run_frame)
                    or cg.gl_dark_press(oil_for_press,
                                        cg.gl_chebyshev(
                                            self.px, self.py,
                                            self.zx[i], self.zy[i]))):
                nx, ny = cg.gl_shambler_stride(
                    self.zx[i], self.zy[i], self.px, self.py)
            blocker = next(
                (j for j in range(cg.GL_BARRICADE_CAP)
                 if self.bhp[j] > 0
                 and cg.gl_barricade_blocks(self.bx[j], self.by[j],
                                            self.zx[i], self.zy[i],
                                            nx, ny)), -1)
            if blocker >= 0:
                self.bhp[blocker] -= 1
                if self.bhp[blocker] == 0:
                    self.breaches += 1
            else:
                self.zx[i], self.zy[i] = nx, ny
        self.run_frame += 1

    def the_cold_hands(self):
        """Mirror of main.c the_cold_hands() (+ min-dist evidence)."""
        touched = any(
            cg.gl_contact(self.px, self.py, self.zx[i], self.zy[i])
            for i in range(len(self.zx)))
        _i, dist = self.nearest()
        if self.min_dist is None or dist < self.min_dist:
            self.min_dist = dist
        return touched

    def audio_update(self, prev_state, prev_counts):
        """Mirror of main.c's slice-8 audio block (runs after the state
        machine each frame): one-shot cue selection by counter deltas +
        state flips with highest-id-wins priority, the cue channel's
        frame countdown, and the ambience drone's tier flips."""
        (prev_shoves, prev_places, prev_repairs, prev_breaches,
         prev_scavenged, prev_oil_grabs) = prev_counts
        _i, dist = self.nearest()
        press_now = 1 if (self.state == STATE_PLAYING and self.zx
                          and cg.gl_dark_press(self.oil, dist)) else 0
        if self.cue_left > 0:
            self.cue_left -= 1               # channel closes at 0
        cue = cg.GL_CUE_NONE
        if self.shoves > prev_shoves:
            cue = cg.GL_CUE_SHOVE
        if self.places > prev_places or self.repairs > prev_repairs:
            cue = cg.GL_CUE_PLANK
        if self.scavenged > prev_scavenged:
            cue = cg.GL_CUE_CACHE
        if self.oil_grabs > prev_oil_grabs:
            cue = cg.GL_CUE_FLASK
        if self.breaches > prev_breaches:
            cue = cg.GL_CUE_BREACH
        if self.state == STATE_PLAYING and prev_state != STATE_PLAYING:
            cue = cg.GL_CUE_NIGHTFALL        # every path into a night
        if self.state == STATE_DAWN and prev_state == STATE_PLAYING:
            cue = cg.GL_CUE_DAWN
        if self.state == STATE_DEAD and prev_state != STATE_DEAD:
            cue = cg.GL_CUE_DEATH
        if cue != cg.GL_CUE_NONE:
            self.cue_left = cg.gl_cue_len(cue)
            self.last_cue = cue
            self.cues += 1
            self.cue_frame = self.frame
        if self.state in (STATE_PLAYING, STATE_SCAVENGE):
            tier = cg.gl_amb_tier(self.state == STATE_PLAYING, self.oil,
                                  press_now)
            if not self.amb_on or tier != self.amb_tier:
                self.amb_on = True
                self.amb_tier = tier
                self.amb_flips += 1
        elif self.amb_on:
            self.amb_on = False
            self.amb_flips += 1

    def step(self, emu_frame, held):
        """One main-loop iteration (the ROM iteration run at emu_frame)."""
        self.frame += 1
        down = held - self.prev_held
        self.prev_held = held
        if not held:
            self.pad_seen_idle = True
        start = self.pad_seen_idle and 'START' in down
        prev_state = self.state
        prev_counts = (self.shoves, self.places, self.repairs,
                       self.breaches, self.scavenged, self.oil_grabs)

        if self.state == STATE_TITLE:
            if start:
                self.start_run(self.frame)
                self.state = STATE_PLAYING
        elif self.state == STATE_PLAYING:
            self.oil = cg.gl_oil_burn(self.oil)   # the lantern burns
            self.spawn_due()
            self.px, self.py = cg.gl_player_step(
                self.px, self.py, 'UP' in held, 'DOWN' in held,
                'LEFT' in held, 'RIGHT' in held)
            self.shove_verb(down)
            self.barricade_verb(down)
            self.step_the_dead(self.oil)
            if self.the_cold_hands():
                self.deaths += 1
                self.state = STATE_DEAD
            else:
                self.dawn_left -= 1
                if self.dawn_left == 0:
                    self.nights_survived = self.night
                    self.state = STATE_DAWN
                    self.dawn_emu_frames.append(emu_frame)
        elif self.state == STATE_SCAVENGE:
            if start:                        # to the fence — night comes
                self.night += 1              # loot is the grant: no +2
                self.start_night()
                self.state = STATE_PLAYING
            else:
                self.px, self.py = cg.gl_player_step(
                    self.px, self.py, 'UP' in held, 'DOWN' in held,
                    'LEFT' in held, 'RIGHT' in held)
                self.shove_verb(down)
                self.barricade_verb(down)
                for i in range(cg.GL_CACHE_COUNT):
                    if (self.cache_up[i]
                            and self.planks < cg.GL_PLANK_MAX
                            and cg.gl_cache_grab(self.px, self.py,
                                                 self.cx[i], self.cy[i])):
                        self.cache_up[i] = 0
                        self.planks = cg.gl_planks_after_grab(self.planks)
                        self.scavenged += 1
                for i in range(cg.GL_FLASK_COUNT):
                    if (self.flask_up[i]
                            and self.oil < cg.GL_OIL_MAX
                            and cg.gl_cache_grab(self.px, self.py,
                                                 self.fx[i], self.fy[i])):
                        self.flask_up[i] = 0
                        self.oil = cg.gl_oil_after_flask(self.oil)
                        self.oil_grabs += 1
                # daylight burns nothing and presses nobody
                self.step_the_dead(cg.GL_OIL_MAX)
                if self.the_cold_hands():
                    self.deaths += 1
                    self.state = STATE_DEAD
                else:
                    self.scav_left -= 1
                    if self.scav_left == 0:  # dawn light spent
                        self.night += 1      # loot is the grant: no +2
                        self.start_night()
                        self.state = STATE_PLAYING
        elif self.state == STATE_DEAD:
            if start:
                self.start_run(self.frame)
                self.state = STATE_PLAYING
        elif self.state == STATE_DAWN:
            if start:
                self.night += 1
                self.planks = cg.gl_planks_at_dawn(self.planks)
                self.start_night()
                self.state = STATE_PLAYING
            elif self.pad_seen_idle and 'SELECT' in down:
                self.scavs += 1
                self.begin_scavenge()
                self.state = STATE_SCAVENGE
        self.audio_update(prev_state, prev_counts)

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
        intact = [i for i in range(cg.GL_BARRICADE_CAP) if self.bhp[i] > 0]
        nearb = min(intact, key=lambda i: cg.gl_chebyshev(
            self.px, self.py, self.bx[i], self.by[i]), default=-1)
        up = [i for i in range(cg.GL_CACHE_COUNT) if self.cache_up[i]]
        nearc = min(up, key=lambda i: cg.gl_chebyshev(
            self.px, self.py, self.cx[i], self.cy[i]), default=-1)
        fup = [i for i in range(cg.GL_FLASK_COUNT) if self.flask_up[i]]
        nearf = min(fup, key=lambda i: cg.gl_chebyshev(
            self.px, self.py, self.fx[i], self.fy[i]), default=-1)
        press = (1 if (self.state == STATE_PLAYING and self.zx
                       and cg.gl_dark_press(self.oil, dist)) else 0)
        print(f'  probe emu {emu}: state {self.state} night {self.night} '
              f'seed {self.seed} run_frame {self.run_frame} '
              f'z {len(self.zx)}/{self.wave_total} dist {dist} '
              f'({dist / cg.GL_ONE:.1f} px) shove_cd {self.shove_cd} '
              f'shoves {self.shoves} nights {self.nights_survived} '
              f'deaths {self.deaths} px {self.px} py {self.py} '
              f'planks {self.planks} barr {len(intact)} '
              f'bhp {self.bhp[nearb] if nearb >= 0 else 0} '
              f'breaches {self.breaches} places {self.places} '
              f'repairs {self.repairs} '
              f'bx {self.bx[nearb] if nearb >= 0 else 0} '
              f'by {self.by[nearb] if nearb >= 0 else 0} '
              f'scav_left {self.scav_left} caches {len(up)} '
              f'scavenged {self.scavenged} scavs {self.scavs} '
              f'cx {self.cx[nearc] if nearc >= 0 else 0} '
              f'cy {self.cy[nearc] if nearc >= 0 else 0} '
              f'cdist {cg.gl_chebyshev(self.px, self.py, self.cx[nearc], self.cy[nearc]) if nearc >= 0 else 0} '
              f'oil {self.oil} radius {cg.gl_light_radius(self.oil)} '
              f'press {press} flasks {len(fup)} '
              f'oil_grabs {self.oil_grabs} '
              f'fx {self.fx[nearf] if nearf >= 0 else 0} '
              f'fy {self.fy[nearf] if nearf >= 0 else 0} '
              f'fdist {cg.gl_chebyshev(self.px, self.py, self.fx[nearf], self.fy[nearf]) if nearf >= 0 else 0} '
              f'atier {self.amb_tier if self.amb_on else 0} '
              f'acue {self.last_cue} acues {self.cues} '
              f'acuefr {self.cue_frame} '
              f'adrone {cg.gl_amb_freq(self.amb_tier) if self.amb_on else 0} '
              f'aflips {self.amb_flips} asfxl {self.cue_left}')


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
    """Worst zombie distance over a rollout of `held`, + wall/zombie terms.

    Mirrors main.c's PLAYING step order (spawn -> player -> zombie steps
    with barricade blocking -> contact); the autopilot presses no B, but
    any barricades already up in `sim` block (and get chewed) here too.
    """
    px, py = sim.px, sim.py
    zx, zy = list(sim.zx), list(sim.zy)
    zstun = list(sim.zstun)
    bhp = list(sim.bhp)
    rf = sim.run_frame
    oil = sim.oil                    # slice 7: the rollout burns too,
    up, down = 'UP' in held, 'DOWN' in held  # so the planner sees the press
    left, right = 'LEFT' in held, 'RIGHT' in held
    worst = None
    for _ in range(horizon):
        oil = cg.gl_oil_burn(oil)
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
                continue
            nx, ny = zx[i], zy[i]
            if (not cg.gl_shambler_staggers(i, rf)
                    or cg.gl_dark_press(oil, cg.gl_chebyshev(
                        px, py, zx[i], zy[i]))):
                nx, ny = cg.gl_shambler_stride(zx[i], zy[i], px, py)
            blocker = next(
                (j for j in range(cg.GL_BARRICADE_CAP)
                 if bhp[j] > 0
                 and cg.gl_barricade_blocks(sim.bx[j], sim.by[j],
                                            zx[i], zy[i], nx, ny)), -1)
            if blocker >= 0:
                bhp[blocker] -= 1
            else:
                zx[i], zy[i] = nx, ny
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
