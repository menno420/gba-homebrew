#!/usr/bin/env python3
"""Cindervault host-side mirror — the committed twin of src/main.cpp's core.

The prototype session (.sessions/2026-07-13-cindervault.md, 💡 card) proved
the recipe but never committed the tool; this file is that recipe, rebuilt
and extended with the item layer (growth cut 1), so it stays maintained
alongside the game:

  1. The mirror re-implements the DETERMINISM-CONTRACT core exactly:
     xorshift32 at seed 0xC1DE5EED, floor generation (spawn, carve walk,
     stairs, embers, monsters, items — same RNG word order), and the whole
     turn resolution (bump combat + blade, free wall bumps, ember/item
     pickup, lantern half-burn, stairs, monster chase phase with the
     per-depth species policies of growth cut 2, win/lose, and growth cut
     3's endless continue: SELECT on the win screen, DEPTH_BANDS past
     floor 5, the five-depth species cycle).
  2. `--verify` certifies the mirror against a real emulator watch-log CSV
     (tools/headless-screenshot.py --watch-log) turn-for-turn: at every
     frame where the turn counter advances, all 14 game-state words must
     match the mirror's prediction. Only a 0-mismatch mirror may be used
     to transcribe proof pins.
  3. `--design` runs the route bot (BFS pathing + the greedy
     clear-biters/sweep-goals policy) to derive the committed proof routes
     at Python speed; `--trace` prints the per-turn state a pin is read
     from; `--keys` emits the transcription (headless-screenshot.py
     --keys/--keys-pattern spans at the committed cadence).

Usage (from the repo root):
    python3 games/cindervault/tools/mirror.py --design lantern
    python3 games/cindervault/tools/mirror.py --cmds RRDD... --trace
    python3 games/cindervault/tools/mirror.py --cmds ... --keys
    python3 games/cindervault/tools/mirror.py --cmds ... --verify run.csv

Commands: U D L R (steps/bumps), W (the A-button wait) and S (SELECT —
the delve-deeper continue on the VAULT REACHED screen, growth cut 3). One
character per player input edge, exactly the emulator key script's press
order.
"""

import argparse
import sys
from collections import deque

# --- tuning constants (MUST mirror src/main.cpp verbatim) -------------------

SEED = 0xC1DE5EED
MAP_W = 13
MAP_H = 8
CARVE_STEPS = 240
LAST_FLOOR = 5  # the vault floor — the first milestone, not the end (cut 3)
START_TORCH = 220
EMBER_TORCH = 25
EMBERS_PER_FLOOR = 3
START_HP = 10
BIG_MONSTER_HP = 3
BUMP_DAMAGE = 2
BLADE_DAMAGE = 3
LANTERN_TURNS = 20
MAX_MONSTERS = 8
MONSTER_COUNT = [2, 2, 3, 3, 4]

# Species (growth cut 2): one named species per depth, each a quirk on the
# plugged-in greedy chase step. NO RNG consumed — species is a pure
# function of the floor number, so floor generation is byte-identical to
# growth cut 1 and every spawn pin carries.
SP_RAT, SP_WISP, SP_HOUND, SP_WRAITH, SP_SENTINEL = range(5)
FLOOR_SPECIES = [SP_RAT, SP_WISP, SP_HOUND, SP_WRAITH, SP_SENTINEL]
SPECIES_NAME = ['CINDER RAT', 'SOOT WISP', 'ASH HOUND', 'VAULT WRAITH',
                'HOARD SENTINEL']
WRAITH_REACH = 2    # cold grasp: bites axis-aligned at this Manhattan dist
SENTINEL_LEASH = 4  # stands guard until the player is this close

# Endless depth bands (growth cut 3): past floor 5 every floor draws its
# generator parameters from this table — one band per BAND_FLOORS floors,
# the last band is the floor of the world. Species RECUR on the same
# five-depth cycle (floor 6 = CINDER RAT again, ... floor 10 = HOARD
# SENTINEL, and around), so (floor - 1) % LAST_FLOOR is the species law
# for EVERY floor — floors 1-5 included, unchanged.
BAND_FLOORS = 3
DEPTH_BANDS = [
    # (monsters, bigs, embers, carve_steps)
    (4, 1, 2, 220),  # band 1: floors  6-8
    (5, 2, 2, 200),  # band 2: floors  9-11
    (6, 2, 1, 180),  # band 3: floors 12-14
    (7, 3, 1, 160),  # band 4: floors 15-17
    (8, 4, 1, 140),  # band 5: floors 18+
]


def floor_params(floor):
    """(monsters, bigs, embers, carve_steps) for a floor. Floors 1-5 are
    the classic tables verbatim — the default vault run is bit-identical —
    and floors 6+ read DEPTH_BANDS."""
    if floor <= LAST_FLOOR:
        return (MONSTER_COUNT[floor - 1], 1 if floor >= 3 else 0,
                EMBERS_PER_FLOOR, CARVE_STEPS)
    band = min((floor - LAST_FLOOR - 1) // BAND_FLOORS, len(DEPTH_BANDS) - 1)
    return DEPTH_BANDS[band]


def species_of(floor):
    """The five-depth species cycle (identity on floors 1-5)."""
    return FLOOR_SPECIES[(floor - 1) % LAST_FLOOR]

T_WALL, T_FLOOR, T_EMBER, T_STAIRS, T_LANTERN, T_BLADE = range(6)
IT_NONE, IT_LANTERN, IT_BLADE = range(3)
ST_TITLE, ST_PLAYING, ST_WIN, ST_LOSE = range(4)

MASK = 0xFFFFFFFF


class Rng:
    """xorshift32, exactly main.cpp's rng_t."""

    def __init__(self, seed):
        self.s = seed & MASK

    def next(self):
        s = self.s
        s ^= (s << 13) & MASK
        s ^= s >> 17
        s ^= (s << 5) & MASK
        self.s = s
        return s

    def range(self, n):
        return self.next() % n


class Monster:
    __slots__ = ('x', 'y', 'hp', 'big', 'alive')

    def __init__(self):
        self.x = self.y = self.hp = 0
        self.big = False
        self.alive = False


class Sim:
    """The playing-state core. reset() == pressing START on the title.

    `seed` mirrors growth cut 4's title dial: main.cpp's reset_run()
    seeds from the dialed value, so a Sim(seed=...) is the run a player
    starts after dialing that value (default: the boot constant — the
    undialed run, bit-identical to growth cut 3)."""

    def __init__(self, seed=SEED):
        self.seed = seed & MASK
        self.rng = Rng(self.seed)
        self.monsters = [Monster() for _ in range(MAX_MONSTERS)]
        self.reset()

    def reset(self):
        self.rng.s = self.seed
        self.floor = 1
        self.hp = START_HP
        self.torch = START_TORCH
        self.embers = 0
        self.kills = 0
        self.turns = 0
        self.lose_reason = 0
        self.item = IT_NONE
        self.item_turns = 0
        self.state = ST_PLAYING
        self.generate_floor()

    def monster_at(self, x, y):
        for i, m in enumerate(self.monsters):
            if m.alive and m.x == x and m.y == y:
                return i
        return -1

    def generate_floor(self):
        # Per-floor generator parameters: classic tables for floors 1-5,
        # DEPTH_BANDS past the vault (growth cut 3). Same RNG word order
        # either way — only the loop bounds move.
        mon_count, bigs, ember_count, carve = floor_params(self.floor)

        self.tiles = [[T_WALL] * MAP_W for _ in range(MAP_H)]
        for m in self.monsters:
            m.alive = False

        rng = self.rng
        self.px = 1 + rng.range(MAP_W - 2)
        self.py = 1 + rng.range(MAP_H - 2)

        carved = [(self.px, self.py)]
        cx, cy = self.px, self.py
        self.tiles[cy][cx] = T_FLOOR
        for _ in range(carve):
            d = rng.range(4)
            nx = cx + (-1 if d == 2 else 1 if d == 3 else 0)
            ny = cy + (-1 if d == 0 else 1 if d == 1 else 0)
            if 1 <= nx <= MAP_W - 2 and 1 <= ny <= MAP_H - 2:
                cx, cy = nx, ny
                if self.tiles[cy][cx] == T_WALL:
                    self.tiles[cy][cx] = T_FLOOR
                    carved.append((cx, cy))

        best, best_dist = 0, -1
        for i, (x, y) in enumerate(carved):
            dist = abs(x - self.px) + abs(y - self.py)
            if dist > best_dist:
                best_dist, best = dist, i
        sx, sy = carved[best]
        self.tiles[sy][sx] = T_STAIRS

        for _ in range(ember_count):
            for _ in range(100):
                x, y = carved[rng.range(len(carved))]
                if self.tiles[y][x] == T_FLOOR and (x, y) != (self.px, self.py):
                    self.tiles[y][x] = T_EMBER
                    break

        for m in range(mon_count):
            big = m < bigs
            mhp = BIG_MONSTER_HP if big else 1 + rng.range(2)
            for _ in range(100):
                x, y = carved[rng.range(len(carved))]
                dist = abs(x - self.px) + abs(y - self.py)
                if self.tiles[y][x] == T_FLOOR and dist > 3 \
                        and self.monster_at(x, y) < 0:
                    mon = self.monsters[m]
                    mon.x, mon.y, mon.hp = x, y, mhp
                    mon.big, mon.alive = big, True
                    break

        # Items (growth cut 1): one lantern, then one blade — placed LAST.
        for it in range(2):
            for _ in range(100):
                x, y = carved[rng.range(len(carved))]
                if self.tiles[y][x] == T_FLOOR and (x, y) != (self.px, self.py) \
                        and self.monster_at(x, y) < 0:
                    self.tiles[y][x] = T_LANTERN if it == 0 else T_BLADE
                    break

    def monster_phase(self):
        # The greedy step is a plug-in policy (growth cut 2): every monster
        # on a floor is one named species, and each species is one quirk
        # wrapped around the same baseline greedy step.
        sp = species_of(self.floor)
        pack_bite_taken = False
        for m in self.monsters:
            if not m.alive:
                continue
            # SOOT WISP flits: on odd turn counts it phases out entirely —
            # no bite, no step — so it closes (and drains) at half rate.
            if sp == SP_WISP and (self.turns & 1) == 1:
                continue
            dx, dy = self.px - m.x, self.py - m.y
            adx, ady = abs(dx), abs(dy)
            if adx + ady == 1:
                # ASH HOUND pack discipline: at most ONE hound bites per
                # phase (slot order); the denied biters hold the ring.
                if sp == SP_HOUND:
                    if not pack_bite_taken:
                        self.hp -= 1
                        pack_bite_taken = True
                else:
                    self.hp -= 1  # bite
                continue
            # VAULT WRAITH cold grasp: axis-aligned at reach 2 it bites
            # through the dark INSTEAD of stepping (never needs adjacency).
            if sp == SP_WRAITH and adx + ady == WRAITH_REACH \
                    and (dx == 0 or dy == 0):
                self.hp -= 1
                continue
            # HOARD SENTINEL stands its ground until the player crosses
            # the leash; outside it, it never leaves its post.
            if sp == SP_SENTINEL and adx + ady > SENTINEL_LEASH:
                continue
            sx = 1 if dx > 0 else -1 if dx < 0 else 0
            sy = 1 if dy > 0 else -1 if dy < 0 else 0
            horiz_first = adx >= ady
            for attempt in range(2):
                horiz = (attempt == 0) == horiz_first
                nx = m.x + (sx if horiz else 0)
                ny = m.y + (0 if horiz else sy)
                if (nx, ny) == (m.x, m.y) or self.tiles[ny][nx] != T_FLOOR \
                        or (nx, ny) == (self.px, self.py) \
                        or self.monster_at(nx, ny) >= 0:
                    continue
                m.x, m.y = nx, ny
                break

    def step(self, cmd):
        """One player input edge. Returns True if the state advanced."""
        if cmd == 'S':
            # SELECT on the VAULT REACHED screen delves deeper (growth cut
            # 3): the win stands as the first milestone; hp/torch/items/
            # turn counter all carry, floor 6 is generated from the RNG
            # state the vault left behind, and the +torch win bonus drops
            # out of the live score — the price of not stopping. Consumes
            # NO turn (a menu edge, not a dungeon turn).
            if self.state != ST_WIN:
                return False
            self.floor += 1
            self.generate_floor()
            self.state = ST_PLAYING
            return True
        if self.state != ST_PLAYING:
            return False
        dx = dy = 0
        wait = False
        if cmd == 'U':
            dy = -1
        elif cmd == 'D':
            dy = 1
        elif cmd == 'L':
            dx = -1
        elif cmd == 'R':
            dx = 1
        elif cmd == 'W':
            wait = True
        else:
            raise ValueError(f'bad cmd {cmd!r}')

        acted = False
        if wait:
            acted = True
        else:
            nx, ny = self.px + dx, self.py + dy
            mi = self.monster_at(nx, ny)
            if mi >= 0:
                acted = True
                mon = self.monsters[mi]
                mon.hp -= BLADE_DAMAGE if self.item == IT_BLADE else BUMP_DAMAGE
                if mon.hp <= 0:
                    mon.alive = False
                    self.kills += 1
            elif self.tiles[ny][nx] == T_WALL:
                acted = False  # wall bumps are free
            else:
                self.px, self.py = nx, ny
                acted = True
                tile = self.tiles[ny][nx]
                if tile == T_EMBER:
                    self.tiles[ny][nx] = T_FLOOR
                    self.torch += EMBER_TORCH
                    self.embers += 1
                elif tile in (T_LANTERN, T_BLADE):
                    self.item = IT_LANTERN if tile == T_LANTERN else IT_BLADE
                    self.item_turns = LANTERN_TURNS if tile == T_LANTERN else 0
                    self.tiles[ny][nx] = T_FLOOR
                elif tile == T_STAIRS:
                    if self.floor == LAST_FLOOR:
                        self.state = ST_WIN
                    else:
                        self.floor += 1
                        self.generate_floor()

        if acted:
            self.turns += 1
            if self.state == ST_PLAYING:
                burn = True
                if self.item == IT_LANTERN:
                    self.item_turns -= 1
                    burn = (self.item_turns & 1) == 0
                    if self.item_turns <= 0:
                        self.item = IT_NONE
                        self.item_turns = 0
                if burn:
                    self.torch -= 1
                if self.torch <= 0:
                    self.torch = 0
                    self.lose_reason = 1
                    self.state = ST_LOSE
                else:
                    self.monster_phase()
                    if self.hp <= 0:
                        self.hp = 0
                        self.lose_reason = 2
                        self.state = ST_LOSE
        return acted

    # --- telemetry-equivalent view (cv_telemetry word order) ----------------

    def score(self):
        cleared = LAST_FLOOR if self.state == ST_WIN else self.floor - 1
        return 100 * cleared + 25 * self.embers + 10 * self.kills \
            + (self.torch if self.state == ST_WIN else 0)

    def monsters_alive(self):
        return sum(1 for m in self.monsters if m.alive)

    def words(self):
        """cv_telemetry[2..14,16,17] as a dict (the game-state words).

        Word 3 is growth cut 4's selected-seed word: constant for the
        whole run (the dial lives on the title, before the Sim starts),
        so the mirror publishes the seed it was built with."""
        return {
            2: self.state, 3: self.seed,
            4: self.floor, 5: self.hp, 6: self.torch,
            7: self.embers, 8: self.kills, 9: self.score(), 10: self.turns,
            11: self.px, 12: self.py, 13: self.lose_reason,
            14: self.monsters_alive(), 16: self.item, 17: self.item_turns,
        }

    def render(self):
        rows = []
        for y in range(MAP_H):
            row = ''
            for x in range(MAP_W):
                g = '#.*>o/'[self.tiles[y][x]]
                mi = self.monster_at(x, y)
                if mi >= 0:
                    g = 'M' if self.monsters[mi].big else 'm'
                if (x, y) == (self.px, self.py):
                    g = '@'
                row += g
            rows.append(row)
        return '\n'.join(rows)


# --- the route bot (BFS pathing + the greedy policy) -------------------------

def bfs_step(sim, targets, avoid=frozenset()):
    """First step of a shortest path from the player to any target tile.

    Walkable: any non-wall tile not in `avoid` and not under a live monster
    (target tiles themselves are allowed, so 'walk onto the ember/item/
    stairs' and 'step adjacent to a monster' both terminate). Returns a
    cmd char, or None when unreachable.
    """
    start = (sim.px, sim.py)
    if start in targets:
        return ''
    blocked = {(m.x, m.y) for m in sim.monsters if m.alive}
    prev = {start: None}
    queue = deque([start])
    while queue:
        pos = queue.popleft()
        for cmd, (dx, dy) in (('U', (0, -1)), ('D', (0, 1)),
                              ('L', (-1, 0)), ('R', (1, 0))):
            nxt = (pos[0] + dx, pos[1] + dy)
            if nxt in prev:
                continue
            if sim.tiles[nxt[1]][nxt[0]] == T_WALL:
                continue
            if nxt in targets:
                prev[nxt] = (pos, cmd)
                node = nxt
                while prev[node][0] != start:
                    node = prev[node][0]
                return prev[node][1]
            if nxt in avoid or nxt in blocked:
                continue
            prev[nxt] = (pos, cmd)
            queue.append(nxt)
    return None


def adjacent_monster(sim):
    for m in sim.monsters:
        if m.alive and abs(m.x - sim.px) + abs(m.y - sim.py) == 1:
            return m
    return None


DIR_CMD = {(0, -1): 'U', (0, 1): 'D', (-1, 0): 'L', (1, 0): 'R'}


def drive(sim, targets_fn, done, avoid=frozenset(), limit=400):
    """Greedy policy loop (the prototype card's bot): bump an adjacent
    biter first, else take the BFS step toward the current targets, until
    `done(sim)`. `avoid` may be a frozenset or a callable(sim) recomputed
    every turn (growth cut 2: species make threat squares move, so a
    static avoid set is no longer always enough). Returns the cmd string
    it played."""
    cmds = ''
    for _ in range(limit):
        if done(sim) or sim.state != ST_PLAYING:
            return cmds
        mon = adjacent_monster(sim)
        if mon is not None:
            cmd = DIR_CMD[(mon.x - sim.px, mon.y - sim.py)]
        else:
            targets = frozenset(targets_fn(sim)) - {(sim.px, sim.py)}
            av = avoid(sim) if callable(avoid) else avoid
            cmd = bfs_step(sim, targets, av)
            if cmd is None:
                raise RuntimeError('goal unreachable:\n' + sim.render())
        sim.step(cmd)
        cmds += cmd
    raise RuntimeError('drive() hit the turn limit')


def bite_ring(sim):
    """Squares orthogonally adjacent to a live monster (bite range)."""
    sq = set()
    for m in sim.monsters:
        if m.alive:
            sq |= {(m.x + 1, m.y), (m.x - 1, m.y),
                   (m.x, m.y + 1), (m.x, m.y - 1)}
    return frozenset(sq)


def grasp_tiles(sim):
    """Squares a live VAULT WRAITH cold-grasps (axis-aligned, dist 2)."""
    sq = set()
    for m in sim.monsters:
        if m.alive:
            for d in (-2, 2):
                sq.add((m.x + d, m.y))
                sq.add((m.x, m.y + d))
    return frozenset(sq)


def find_tiles(sim, tile):
    return [(x, y) for y in range(MAP_H) for x in range(MAP_W)
            if sim.tiles[y][x] == tile]


def item_tiles(sim):
    return frozenset(find_tiles(sim, T_LANTERN) + find_tiles(sim, T_BLADE))


def kill_all(sim, avoid=None):
    # Hunt every monster on the floor; by default never walk over an item
    # tile (pass a narrower avoid set when a re-pickup would be harmless).
    return drive(sim,
                 lambda s: [(m.x, m.y) for m in s.monsters if m.alive],
                 lambda s: s.monsters_alive() == 0,
                 avoid=item_tiles(sim) if avoid is None else avoid)


def take(sim, tile):
    want = IT_LANTERN if tile == T_LANTERN else IT_BLADE
    other = item_tiles(sim) - frozenset(find_tiles(sim, tile))
    return drive(sim,
                 lambda s: find_tiles(s, tile),
                 lambda s: s.item == want and not find_tiles(s, tile),
                 avoid=other)


def descend(sim, avoid=None):
    frm = sim.floor
    avoid = item_tiles(sim) if avoid is None else avoid
    return drive(sim,
                 lambda s: find_tiles(s, T_STAIRS),
                 lambda s: s.floor != frm,
                 avoid=avoid)


def kill_adjacent(sim):
    """Bump the currently adjacent monster until it is dead."""
    want = sim.kills + 1
    return drive(sim, lambda s: [], lambda s: s.kills >= want)


def wait_until_bitten(sim):
    """Stand still until hp drops (the wraith cold-grasp witness)."""
    hp0 = sim.hp
    cmds = ''
    while sim.hp == hp0 and sim.state == ST_PLAYING:
        sim.step('W')
        cmds += 'W'
    return cmds


def choke_fight(sim, until=0, limit=80):
    """Hold the current tile and bump whatever steps into reach (growth
    cut 3's endless workhorse): a monster that ARRIVES adjacent spent its
    phase moving — movers cannot bite — and the answering bump lands
    before its next phase, so staggered arrivals die for free. Only
    simultaneous arrivals cost hp; pick the choke tile so the arrival
    distances differ."""
    out = ''
    for _ in range(limit):
        if sim.monsters_alive() <= until or sim.state != ST_PLAYING:
            return out
        mon = adjacent_monster(sim)
        cmd = DIR_CMD[(mon.x - sim.px, mon.y - sim.py)] if mon else 'W'
        sim.step(cmd)
        out += cmd
    raise RuntimeError('choke_fight hit the turn limit:\n' + sim.render())


def sweep_embers(sim):
    """Collect every ember on the floor (the band ember-economy witness)."""
    return drive(sim,
                 lambda s: find_tiles(s, T_EMBER),
                 lambda s: not find_tiles(s, T_EMBER),
                 avoid=item_tiles(sim))


def beam_descend(sim, width=600, depth=60):
    """Beam-search one floor's descent for the max-hp line to the stairs
    (used for endless floor 9: five wraiths' crossing grasp lines beat
    every greedy policy — even the optimal line pays hp). Deterministic:
    stable sort, fixed expansion order. Returns (cmds, end_sim)."""
    import copy

    def key(s):
        return (s.px, s.py, s.hp, s.item,
                tuple((m.x, m.y, m.hp) for m in s.monsters if m.alive))

    target = sim.floor + 1
    beam = [(sim, '')]
    best = None
    seen = set()
    for _ in range(depth):
        nxt = []
        for s, path in beam:
            for cmd in 'UDLRW':
                s2 = copy.deepcopy(s)
                if not s2.step(cmd):
                    continue
                if s2.state == ST_LOSE:
                    continue
                if s2.floor == target:
                    cand = (s2.hp, -s2.turns, s2.torch)
                    if best is None or cand > best[0]:
                        best = (cand, path + cmd, s2)
                    continue
                k = key(s2)
                if k in seen:
                    continue
                seen.add(k)
                nxt.append((s2, path + cmd))

        def rank(item):
            s, _ = item
            st = find_tiles(s, T_STAIRS)
            d = abs(s.px - st[0][0]) + abs(s.py - st[0][1]) if st else 0
            return (-s.hp, d, -s.torch)

        nxt.sort(key=rank)
        beam = nxt[:width]
        if not beam:
            break
    if best is None:
        raise RuntimeError('beam_descend found no surviving line')
    return best[1], best[2]


def design(route_name, seed=SEED):
    """The committed proof routes (see games/cindervault/proofs.sh)."""
    sim = Sim(seed)
    cmds = ''
    if route_name == 'lantern':
        # P2: clear floor 1, take the lantern, then wait it out — the
        # half-burn arithmetic and the 20-turn gutter-out on a still floor.
        cmds += kill_all(sim)
        cmds += take(sim, T_LANTERN)
        for _ in range(24):
            sim.step('W')
            cmds += 'W'
    elif route_name == 'blade':
        # P3: floor 1 — clear the biters, take the LANTERN first, then walk
        # onto the BLADE (the single slot swaps: the one more decision),
        # descend twice dodging floor items, and one-bump floor 3's 3-HP
        # 'M' (blade evidence: it dies in ONE hit; bump-2 needs two).
        cmds += kill_all(sim)
        cmds += take(sim, T_LANTERN)
        cmds += take(sim, T_BLADE)
        for _ in range(2):
            # Holding the blade, only lanterns can hurt the slot — walking
            # over a sibling blade merely re-picks the same item (floor 2's
            # blade is in fact the cut tile in front of its stairs).
            lanterns = frozenset(find_tiles(sim, T_LANTERN))
            cmds += kill_all(sim, avoid=lanterns)
            cmds += descend(sim, avoid=lanterns)
        # floor 3: hunt the big M plus the rest — 3 blade bump kills.
        cmds += kill_all(sim, avoid=frozenset(find_tiles(sim, T_LANTERN)))
    elif route_name == 'species':
        # P4: one full descent past every species (growth cut 2), one
        # witness per quirk, ending in the vault — which doubles as the
        # winnability re-check for the species world.
        lanterns = lambda s: frozenset(find_tiles(s, T_LANTERN))  # noqa: E731

        def walkable(s, x, y):
            return s.tiles[y][x] != T_WALL and s.monster_at(x, y) < 0

        def hunt(s, limit=100):
            # Blade the floor's wraiths down with matador steps: never
            # stand on a grasp line, wait at diagonal-2 so the closer has
            # to step in (moving monsters cannot bite), bump on arrival.
            out = ''
            for _ in range(limit):
                if s.monsters_alive() == 0 or s.state != ST_PLAYING:
                    return out
                mon = adjacent_monster(s)
                if mon is not None:
                    cmd = DIR_CMD[(mon.x - s.px, mon.y - s.py)]
                else:
                    near = [m for m in s.monsters if m.alive
                            and abs(m.x - s.px) + abs(m.y - s.py) == 2]
                    cmd = None
                    if near:
                        m = near[0]
                        if m.x == s.px or m.y == s.py:
                            # on a grasp line: any walkable step off every
                            # grasp line and out of bite range breaks it
                            gt = grasp_tiles(s)
                            ring = bite_ring(s)
                            for (dx, dy), c in DIR_CMD.items():
                                nt = (s.px + dx, s.py + dy)
                                if walkable(s, *nt) and nt not in gt \
                                        and nt not in ring \
                                        and nt not in lanterns(s):
                                    cmd = c
                                    break
                        else:
                            cmd = 'W'
                    if cmd is None:
                        alive = frozenset((m.x, m.y) for m in s.monsters
                                          if m.alive)
                        cmd = bfs_step(s, alive, lanterns(s) | grasp_tiles(s)
                                       | bite_ring(s)) \
                            or bfs_step(s, alive,
                                        lanterns(s) | grasp_tiles(s)) \
                            or bfs_step(s, alive, lanterns(s)) or 'W'
                s.step(cmd)
                out += cmd
            raise RuntimeError('hunt hit the turn limit')

        def careful_descend(s, limit=200):
            # Per turn: bump what is adjacent; else the safest stairs
            # path that still exists (full avoid, then progressively
            # relaxed); else — the floor's monsters ARE the wall —
            # advance on them to bait-and-blade them off the cut tiles.
            frm = s.floor
            out = ''
            for _ in range(limit):
                if s.floor != frm or s.state != ST_PLAYING:
                    return out
                mon = adjacent_monster(s)
                if mon is not None:
                    cmd = DIR_CMD[(mon.x - s.px, mon.y - s.py)]
                else:
                    stairs = frozenset(find_tiles(s, T_STAIRS))
                    cmd = None
                    for av in (lanterns(s) | grasp_tiles(s) | bite_ring(s),
                               lanterns(s) | grasp_tiles(s),
                               lanterns(s) | bite_ring(s),
                               lanterns(s)):
                        cmd = bfs_step(s, stairs, av)
                        if cmd is not None:
                            break
                    if cmd is None:
                        alive = frozenset((m.x, m.y) for m in s.monsters
                                          if m.alive)
                        cmd = bfs_step(s, alive, lanterns(s)) or 'W'
                s.step(cmd)
                out += cmd
            raise RuntimeError('careful_descend hit the turn limit')

        # floor 1 — CINDER RAT: the baseline chase (the carried cut-1
        # pins are its witness); clear the biters and descend.
        cmds += kill_all(sim)
        cmds += descend(sim)

        # floor 2 — SOOT WISP: stand still. They close at HALF rate
        # (hold on odd turn counts), so adjacency forms at turn 20 but
        # first blood waits for even turn 22 — the flit turn 21 is
        # bite-free with a wisp standing adjacent. Kill it with the
        # answering bump (dead before its phase), then run for the
        # stairs: the pocket sits behind the floor's blade tile, so the
        # blade is picked up crossing it; the ember-caged second wisp is
        # left behind.
        while sim.turns < 23:
            sim.step('W')
            cmds += 'W'
        cmds += kill_adjacent(sim)
        cmds += descend(sim, avoid=lambda s: lanterns(s) | bite_ring(s))

        # floor 3 — ASH HOUND: walk to the floor's own blade tile — the
        # seed puts it exactly where TWO hounds ring the player at once —
        # sit through two pack turns (hp -1 per turn, never -2: only one
        # hound bites per phase), then blade the pack down (the 3-HP 'M'
        # alpha one-bumped) and descend.
        cmds += take(sim, T_BLADE)
        for _ in range(2):
            sim.step('W')
            cmds += 'W'
        cmds += kill_all(sim, avoid=lanterns(sim))
        cmds += descend(sim, avoid=lanterns(sim))

        # floor 4 — VAULT WRAITH: stand at the entry square until the
        # first wraith cold-grasps from range 2 (hp -1 with nothing
        # adjacent — unreachable under any one-step chase this early),
        # then hunt the three down matador-style and take the stairs.
        cmds += wait_until_bitten(sim)
        cmds += hunt(sim)
        cmds += descend(sim, avoid=lanterns(sim))

        # floor 5 — HOARD SENTINEL: stand for eight turns — nothing
        # moves, nothing bites (a baseline chaser would be biting by
        # now: the stillness witness) — then walk the vault run,
        # blade-bumping any guard whose leash the path must cross.
        for _ in range(8):
            sim.step('W')
            cmds += 'W'
        cmds += careful_descend(sim)
    elif route_name == 'endless':
        # P5 THE DESCENT (growth cut 3): the full species route first —
        # the vault opens at turn 109 exactly as P4 pins it — then SELECT
        # delves deeper. Band 1 (floors 6-8) is cleared at ZERO hp cost
        # (entry chokes: staggered arrivals die on arrival), every band-1
        # ember is swept (the ember-economy witness: exactly 2 per floor)
        # and floor 7 gets the item two-step on a cleared floor. Band 2
        # opens with wraith floor 9 — five graspers whose crossing lines
        # beat every greedy policy: the beam-search-optimal line still
        # pays 2 hp — and the route ends standing on floor 10 among
        # sentinels frozen outside their leash, hp 1: survivable, but
        # escalating.
        sim, cmds = design('species')
        sim.step('S')
        cmds += 'S'

        lanterns = lambda s: frozenset(find_tiles(s, T_LANTERN))  # noqa: E731

        # floor 6 — CINDER RATs again (the recurrence): choke at the entry
        # tile (arrival distances 4/7/9/10 — all staggered), sweep, down.
        cmds += choke_fight(sim)
        cmds += sweep_embers(sim)
        cmds += descend(sim)

        # floor 7 — SOOT WISPs: the three mobile wisps die at the entry
        # choke; the big is greedy-caged (wall on its row, ember on the
        # next) until the player steps off the shared row — two D steps
        # re-open its chase, and it dies on arrival like the rest. Then
        # the item two-step on the emptied floor (blade -> lantern ->
        # blade: the endless floors still spawn both), sweep, down.
        cmds += choke_fight(sim, until=1)
        for c in 'DD':
            sim.step(c)
            cmds += c
        cmds += choke_fight(sim)
        cmds += take(sim, T_LANTERN)
        cmds += take(sim, T_BLADE)
        cmds += sweep_embers(sim)
        cmds += descend(sim, avoid=lanterns(sim))

        # floor 8 — ASH HOUNDs: one step south desyncs the arrival
        # distances (3/4/5/7 from (8,3) vs two dist-4 pairs from the
        # entry), then the choke kills the pack piecewise; sweep, down.
        sim.step('D')
        cmds += 'D'
        cmds += choke_fight(sim)
        cmds += sweep_embers(sim)
        cmds += descend(sim, avoid=lanterns(sim))

        # floor 9 — VAULT WRAITHs, band 2: five graspers. Flee, do not
        # fight: the beam searcher's max-hp line to the stairs.
        leg, sim = beam_descend(sim)
        cmds += leg

        # floor 10 — HOARD SENTINELs, band 2: three pinned turns standing
        # still among five frozen guards, the closing witness.
        for _ in range(3):
            sim.step('W')
            cmds += 'W'
    else:
        raise SystemExit(f'unknown route {route_name!r}')
    return sim, cmds


# --- emulator transcription ---------------------------------------------------

START_PRESS = (240, 242)   # START on the title
BASE = 300                 # first turn press frame
CADENCE = 6                # frames per turn press
DUTY = 2                   # frames held

KEY_NAME = {'U': 'UP', 'D': 'DOWN', 'L': 'LEFT', 'R': 'RIGHT', 'W': 'A',
            'S': 'SELECT'}


def turn_frame(turn):
    """Press frame of turn N (1-based); assert at turn_frame(N)+4."""
    return BASE + CADENCE * (turn - 1)


def keys_args(cmds):
    args = [f'--keys {START_PRESS[0]}-{START_PRESS[1]}:START']
    i = 0
    while i < len(cmds):
        j = i
        while j < len(cmds) and cmds[j] == cmds[i]:
            j += 1
        start = turn_frame(i + 1)
        name = KEY_NAME[cmds[i]]
        if j - i == 1:
            args.append(f'--keys {start}-{start + DUTY}:{name}')
        else:
            end = turn_frame(j) + DUTY
            args.append(f'--keys-pattern {start}-{end}:{CADENCE}:{DUTY}:{name}')
        i = j
    return args


# --- verification against an emulator watch-log CSV ---------------------------

GAME_WORDS = (2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17)


def verify(cmds, csv_path, seed=SEED):
    """Certify the mirror: the emulator CSV's sequence of DISTINCT
    game-state word tuples must be exactly the mirror's predicted state
    sequence, in order, with nothing extra, nothing missing and nothing
    different. Returns the mismatch count.

    (Growth cut 3 made this stricter than the old per-turn keying: a
    SELECT continue changes state/floor/score at the SAME turn count, so
    the turn counter no longer uniquely keys a state. Every state is held
    for >= 4 frames at the committed cadence, so no state can be skipped
    between CSV rows.)"""
    sim = Sim(seed)
    expected = [dict(sim.words())]
    for cmd in cmds:
        if sim.step(cmd):
            expected.append(dict(sim.words()))

    checked = mismatches = 0
    idx = 0                       # expected[idx] = state the emulator is in
    last = None
    with open(csv_path, encoding='ascii') as fh:
        header = fh.readline().strip().split(',')
        col = {name: idx for idx, name in enumerate(header)}

        def word(row, w):
            return int(row[col[f'cv[{w}]']])

        for line in fh:
            row = line.strip().split(',')
            if word(row, 2) == ST_TITLE:
                continue
            st = {w: word(row, w) for w in GAME_WORDS}
            if st == last:
                continue
            if last is None:      # first playing frame must be state 0
                if st != expected[0]:
                    diff = [w for w in GAME_WORDS if st[w] != expected[0][w]]
                    print(f'frame {row[0]}: boot state differs on '
                          f'{[f"cv[{w}]" for w in diff]}')
                    mismatches += 1
                else:
                    checked += 1
                last = st
                continue
            last = st
            if idx + 1 >= len(expected):
                print(f'frame {row[0]}: emulator advanced past the '
                      f'mirror\'s final state (turn {st[10]})')
                mismatches += 1
                continue
            idx += 1
            checked += 1
            for w in GAME_WORDS:
                if st[w] != expected[idx][w]:
                    print(f'frame {row[0]} state {idx} (turn {st[10]}): '
                          f'cv[{w}] emulator={st[w]} '
                          f'mirror={expected[idx][w]}')
                    mismatches += 1
    if idx + 1 != len(expected):
        print(f'emulator log ended {len(expected) - 1 - idx} mirror '
              f'state(s) early')
        mismatches += len(expected) - 1 - idx
    print(f'verify: {checked} turn states checked, {mismatches} mismatches')
    return mismatches


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cmds', help='route as a UDLRW string')
    parser.add_argument('--design',
                        choices=('lantern', 'blade', 'species', 'endless'),
                        help='derive a committed proof route with the bot')
    parser.add_argument('--baseline', action='store_true',
                        help='counterfactual: run every floor on the CINDER '
                             'RAT baseline policy (species quirks off) — for '
                             'first-blood / arrival gap stats, never for pins')
    parser.add_argument('--trace', action='store_true',
                        help='print per-turn game-state words')
    parser.add_argument('--keys', action='store_true',
                        help='print headless-screenshot.py key spans')
    parser.add_argument('--verify', metavar='CSV',
                        help='certify the mirror against a --watch-log CSV')
    parser.add_argument('--seed', type=lambda s: int(s, 0), default=SEED,
                        help='run at this dialed seed (growth cut 4: the '
                             'title dial; default the boot constant '
                             '0xC1DE5EED)')
    args = parser.parse_args()

    if args.baseline:
        FLOOR_SPECIES[:] = [SP_RAT] * LAST_FLOOR

    if args.design:
        sim, cmds = design(args.design, seed=args.seed)
        print(f'route {args.design}: {len(cmds)} inputs, '
              f'{sim.turns} turns consumed')
        print(cmds)
    elif args.cmds:
        cmds = args.cmds
        sim = Sim(args.seed)
    else:
        parser.error('need --cmds or --design')

    if args.trace:
        sim = Sim(args.seed)
        print(f'turn 0: {sim.words()}')
        for i, cmd in enumerate(cmds):
            if sim.step(cmd):
                print(f'turn {sim.turns} (input {i + 1} {cmd}, assert frame '
                      f'{turn_frame(i + 1) + 4}): {sim.words()}')
    if args.keys:
        print(' \\\n  '.join(keys_args(cmds)))
    if args.verify:
        if verify(cmds, args.verify, seed=args.seed):
            sys.exit(1)


if __name__ == '__main__':
    main()
