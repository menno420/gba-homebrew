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
     pickup, lantern half-burn, stairs, monster chase phase, win/lose).
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

Commands: U D L R (steps/bumps) and W (the A-button wait). One character
per player input edge, exactly the emulator key script's press order.
"""

import argparse
import sys
from collections import deque

# --- tuning constants (MUST mirror src/main.cpp verbatim) -------------------

SEED = 0xC1DE5EED
MAP_W = 13
MAP_H = 8
CARVE_STEPS = 240
LAST_FLOOR = 5
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
    """The playing-state core. reset() == pressing START on the title."""

    def __init__(self):
        self.rng = Rng(SEED)
        self.monsters = [Monster() for _ in range(MAX_MONSTERS)]
        self.reset()

    def reset(self):
        self.rng.s = SEED
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
        self.tiles = [[T_WALL] * MAP_W for _ in range(MAP_H)]
        for m in self.monsters:
            m.alive = False

        rng = self.rng
        self.px = 1 + rng.range(MAP_W - 2)
        self.py = 1 + rng.range(MAP_H - 2)

        carved = [(self.px, self.py)]
        cx, cy = self.px, self.py
        self.tiles[cy][cx] = T_FLOOR
        for _ in range(CARVE_STEPS):
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

        for _ in range(EMBERS_PER_FLOOR):
            for _ in range(100):
                x, y = carved[rng.range(len(carved))]
                if self.tiles[y][x] == T_FLOOR and (x, y) != (self.px, self.py):
                    self.tiles[y][x] = T_EMBER
                    break

        for m in range(MONSTER_COUNT[self.floor - 1]):
            big = self.floor >= 3 and m == 0
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
        for m in self.monsters:
            if not m.alive:
                continue
            dx, dy = self.px - m.x, self.py - m.y
            adx, ady = abs(dx), abs(dy)
            if adx + ady == 1:
                self.hp -= 1  # bite
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
        """One player input edge. Returns True if a turn was consumed."""
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
        """cv_telemetry[2..14,16,17] as a dict (the game-state words)."""
        return {
            2: self.state, 4: self.floor, 5: self.hp, 6: self.torch,
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
    `done(sim)`. Returns the cmd string it played."""
    cmds = ''
    for _ in range(limit):
        if done(sim) or sim.state != ST_PLAYING:
            return cmds
        mon = adjacent_monster(sim)
        if mon is not None:
            cmd = DIR_CMD[(mon.x - sim.px, mon.y - sim.py)]
        else:
            targets = frozenset(targets_fn(sim)) - {(sim.px, sim.py)}
            cmd = bfs_step(sim, targets, avoid)
            if cmd is None:
                raise RuntimeError('goal unreachable:\n' + sim.render())
        sim.step(cmd)
        cmds += cmd
    raise RuntimeError('drive() hit the turn limit')


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


def design(route_name):
    """The two committed proof routes (see games/cindervault/proofs.sh)."""
    sim = Sim()
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
    else:
        raise SystemExit(f'unknown route {route_name!r}')
    return sim, cmds


# --- emulator transcription ---------------------------------------------------

START_PRESS = (240, 242)   # START on the title
BASE = 300                 # first turn press frame
CADENCE = 6                # frames per turn press
DUTY = 2                   # frames held

KEY_NAME = {'U': 'UP', 'D': 'DOWN', 'L': 'LEFT', 'R': 'RIGHT', 'W': 'A'}


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

GAME_WORDS = (2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17)


def verify(cmds, csv_path):
    """Certify the mirror: every turn advance in the emulator CSV must land
    exactly on the mirror's predicted state. Returns the mismatch count."""
    sim = Sim()
    expected = {0: dict(sim.words())}
    for cmd in cmds:
        if sim.step(cmd):
            expected[sim.turns] = dict(sim.words())

    checked = mismatches = 0
    last_turn = None
    with open(csv_path, encoding='ascii') as fh:
        header = fh.readline().strip().split(',')
        col = {name: idx for idx, name in enumerate(header)}

        def word(row, w):
            return int(row[col[f'cv[{w}]']])

        for line in fh:
            row = line.strip().split(',')
            if word(row, 2) == ST_TITLE:
                continue
            turn = word(row, 10)
            if turn == last_turn:
                continue
            last_turn = turn
            if turn not in expected:
                print(f'frame {row[0]}: emulator reached turn {turn} '
                      f'the mirror never produced')
                mismatches += 1
                continue
            checked += 1
            for w in GAME_WORDS:
                if word(row, w) != expected[turn][w]:
                    print(f'frame {row[0]} turn {turn}: cv[{w}] emulator='
                          f'{word(row, w)} mirror={expected[turn][w]}')
                    mismatches += 1
    print(f'verify: {checked} turn states checked, {mismatches} mismatches')
    return mismatches


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cmds', help='route as a UDLRW string')
    parser.add_argument('--design', choices=('lantern', 'blade'),
                        help='derive a committed proof route with the bot')
    parser.add_argument('--trace', action='store_true',
                        help='print per-turn game-state words')
    parser.add_argument('--keys', action='store_true',
                        help='print headless-screenshot.py key spans')
    parser.add_argument('--verify', metavar='CSV',
                        help='certify the mirror against a --watch-log CSV')
    args = parser.parse_args()

    if args.design:
        sim, cmds = design(args.design)
        print(f'route {args.design}: {len(cmds)} inputs, '
              f'{sim.turns} turns consumed')
        print(cmds)
    elif args.cmds:
        cmds = args.cmds
        sim = Sim()
    else:
        parser.error('need --cmds or --design')

    if args.trace:
        sim = Sim()
        print(f'turn 0: {sim.words()}')
        for i, cmd in enumerate(cmds):
            if sim.step(cmd):
                print(f'turn {sim.turns} (input {i + 1} {cmd}, assert frame '
                      f'{turn_frame(i + 1) + 4}): {sim.words()}')
    if args.keys:
        print(' \\\n  '.join(keys_args(cmds)))
    if args.verify:
        if verify(cmds, args.verify):
            sys.exit(1)


if __name__ == '__main__':
    main()
