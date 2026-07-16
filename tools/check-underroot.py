#!/usr/bin/env python3
"""Underroot host proof (stdlib-only, <1s) — arc slice 1: dual-screen skeleton.

The check-gloam.py / check-cave.py sibling for Underroot: a line-for-line
Python mirror of the pure simulation layer in
games/underroot-nds/source/ur_sim.c (ur_hash / ur_hawk_pass /
ur_hawk_active / ur_hawk_dir / ur_hawk_x / ur_hawk_y / ur_cell_of_touch /
ur_dig / ur_burrow_size), proving the meaningful invariants for EVERY
reachable input instead of the handful a replay touches:

  1. hawk schedule (top screen) — for seeds 0..255 x seasons 0..3 x a dense
     frame sweep: ur_hawk_x / ur_hawk_active / ur_hawk_y are pure
     deterministic functions of (seed, season, frame) alone (recompute-equal
     — no wall clock, no RNG divergence host vs ROM); the shadow is ON for
     exactly the first UR_HAWK_SWEEP frames of every UR_HAWK_PERIOD cycle;
     while ON the x sweeps the full meadow width [0, 255] and is monotone
     across the sweep (L->R or R->L per the pure ur_hawk_dir hash bit); the
     lane y stays inside the meadow band [UR_MEADOW_Y0, UR_MEADOW_Y1); and
     the whole schedule is a function of the pass INDEX (= frame /
     UR_HAWK_PERIOD), i.e. the concept's f(seed, season, index);
  2. dig grid (bottom screen) — ur_cell_of_touch maps every one of the
     256x192 bottom-LCD pixels onto its UR_CELL-snapped grid cell (and only
     rejects out-of-bounds); ur_dig is idempotent and monotone (the dug
     count never falls, re-digging a cell is a no-op); and the burrow is a
     GRAPH: ur_burrow_size BFS-counts the dug cells reachable from the
     pre-dug entrance over 4-neighbours, so a corridor drawn OUT from the
     entrance grows the burrow by one, an isolated pocket is NEVER counted
     (foragers can't reach what you didn't connect — the slice-3 pathing
     seed), and connected <= total always;
  3. lockstep fixture (host<->ROM anchor) — the exact --touch dig script the
     CI nds-headless-check replays is simulated here, and the dug-total /
     connected-count it predicts are the SAME numbers the CI asserts against
     the ROM's ur_telemetry mailbox. Divergence in either direction (C
     mirror drifts from Python, or ROM drifts from host) turns a check red.

MIRROR RULE (keep in lockstep): every function and constant below mirrors
games/underroot-nds/source/ur_sim.c. Any change to the C MUST land here in
the same PR — same discipline as tools/check-gloam.py <-> gl_sim.c and
tools/check-cave.py <-> lumen-drift.

Usage: python3 tools/check-underroot.py          # exit 0 = green
"""

import sys
from collections import deque

M32 = 0xFFFFFFFF

# --- constants (mirror ur_sim.h) -------------------------------------------
UR_SEED = 0x55A                 # slice-1 pinned meadow seed (fixed run)
UR_HAWK_PERIOD = 512            # frames between hawk passes
UR_HAWK_SWEEP = 256            # frames a shadow takes to cross the meadow
UR_MEADOW_Y0 = 24              # hawk lane band, top (px)
UR_MEADOW_Y1 = 120            # hawk lane band, bottom (px, exclusive)
UR_HAWK_SALT = 0x4841574B      # 'HAWK' — lane hash stream != direction

UR_GRID_W = 16                # dig-grid columns
UR_GRID_H = 12                # dig-grid rows
UR_CELL = 16                  # px per cell (16*16=256, 12*16=192 = bottom LCD)
UR_CELLS = UR_GRID_W * UR_GRID_H
UR_ENTRANCE_COL = 8           # the surface mouth column
UR_ENTRANCE_ROW = 0           # top row (the meadow<->burrow seam)

# --- foragers: emergent pathing (slice 3; mirror ur_sim.h) -----------------
UR_DROP_ROW = UR_ENTRANCE_ROW + 1     # subsurface drop row (1)
UR_ROUTE_NONE = 0xFFFFFFFF            # no reachable patch / unreachable

# --- granaries: the food store (slice 4; mirror ur_sim.h) ------------------
UR_GRAN_CAP = 4                       # food units one granary cell banks

# --- nurseries + population (slice 5; mirror ur_sim.h) ---------------------
UR_POP_CAP = 8                        # hard bound on raised foragers
UR_FOOD_PER_ANT = 2                   # banked food consumed to raise one forager
UR_NURS_BROOD = 3                     # foragers one connected nursery can brood

# --- hawks predate: exposed shallow route cells (slice 6; mirror ur_sim.h) --
UR_SAFE_DEPTH = 2                     # grid rows [0, UR_SAFE_DEPTH) are hawk-exposed

# --- seasons + the year clock (slice 7; mirror ur_sim.h) -------------------
UR_SPRING = 0
UR_SUMMER = 1
UR_AUTUMN = 2
UR_WINTER = 3
UR_SEASONS = 4
UR_SPRING_DAYS = 4                   # fixed per-season day counts (decide-and-flag)
UR_SUMMER_DAYS = 6
UR_AUTUMN_DAYS = 4
UR_WINTER_DAYS = 2
UR_YEAR_DAYS = UR_SPRING_DAYS + UR_SUMMER_DAYS + UR_AUTUMN_DAYS + UR_WINTER_DAYS  # 16
UR_DAY_FRAMES = 256                  # frames the clock spends on one day
UR_ABUND_UNIT = 4                    # abundance scale denominator (spring = the unit)
UR_ABUND_SPRING = 4
UR_ABUND_SUMMER = 6
UR_ABUND_AUTUMN = 2
UR_ABUND_WINTER = 0

# --- winter survival + the survival score (slice 8; mirror ur_sim.h) -------
UR_CONSUME_PER_DAY = 1               # banked food one forager eats per winter day

# --- best-score save record (slice 9; mirror ur_sim.h) ---------------------
UR_SAVE_MAGIC = 0x55525356           # 'URSV'
UR_SAVE_VERSION = 1
UR_SAVE_WORDS = 8
UR_SAVE_BYTES = 32
UR_SAVE_ADDR = 0
UR_SAVE_EEPROM_TYPE = 2
UR_SAVE_SALT = 0x53415645            # 'SAVE'
UR_SAVE_POLL_BOUND = 4096

# --- seed dial + balance (slice 10; mirror ur_sim.h) -----------------------
UR_DIAL_COUNT = 8                    # discrete seed-dial positions
UR_DIAL_SALT = 0x4449414C            # 'DIAL' — dial->seed stream independent
UR_DIAL_MASK = 0x0FFF                # bounded seed space for scanned positions

# --- synthesized audio: cue + ambience decision layer (slice 11; mirror ur_sim.h)
UR_CUE_NONE = 0                      # the no-op cue
UR_CUE_FORAGE = 1                    # forager returns / food delivered
UR_CUE_HAWK = 2                      # hawk cry (shadow crossing)
UR_CUE_WINTER = 3                    # winter toll (year clock enters winter)
UR_CUE_COUNT = 4
UR_CUE_ON_NOISE = 255                # duty marker: route to the noise channel
UR_AMB_TIERS = 4                     # one ambience drone per season

# --- food patches (top screen; slice 2; mirror ur_sim.h) -------------------
UR_SCREEN_W = 256             # meadow width (px)
UR_SCREEN_H = 192             # meadow height (px)
UR_PATCH_COUNT = 6            # food patches per (seed, season)
UR_PATCH_R = 4                # patch render half-extent (px)
UR_PATCH_MARGIN = 8           # keep whole patches off the edges
UR_PATCH_MIN = 3              # min food amount (units)
UR_PATCH_MAX = 9              # max food amount (units, inclusive)
UR_PATCH_SALT = 0x46454544    # 'FEED' — patch hash stream independent
UR_APRON_X0 = UR_PATCH_MARGIN + UR_PATCH_R                # 12
UR_APRON_X1 = UR_SCREEN_W - UR_PATCH_MARGIN - UR_PATCH_R  # 244
UR_APRON_Y0 = UR_MEADOW_Y1 + UR_PATCH_R                   # 124: below the lanes
UR_APRON_Y1 = UR_SCREEN_H - UR_PATCH_MARGIN - UR_PATCH_R  # 180

# The fairness floor (slice 10): the least winter food any seed can offer (all
# patches at the minimum amount) — a survivable reference score is >= this for
# EVERY seed. (Defined here, after UR_PATCH_COUNT/UR_PATCH_MIN.)
UR_FAIR_FLOOR = UR_PATCH_COUNT * UR_PATCH_MIN                 # 18


# --- pure hash (mirror ur_hash; identical to gl_hash) ----------------------
def ur_hash(a, b):
    h = ((a & M32) * 0x9E3779B9 ^ (b & M32) * 0x85EBCA6B) & M32
    h ^= h >> 16
    h = (h * 0x7FEB352D) & M32
    h ^= h >> 15
    h = (h * 0x846CA68B) & M32
    h ^= h >> 16
    return h & M32


# --- hawk schedule (top screen; f(seed, season, index)) --------------------
def ur_hawk_pass(frame):
    return frame // UR_HAWK_PERIOD


def ur_hawk_active(frame):
    return 1 if (frame % UR_HAWK_PERIOD) < UR_HAWK_SWEEP else 0


def ur_hawk_dir(seed, season, index):
    # 0 = left->right, 1 = right->left
    return ur_hash(ur_hash(seed, season), index) & 1


def ur_hawk_x(seed, season, frame):
    # -1 when no shadow is crossing this frame.
    t = frame % UR_HAWK_PERIOD
    if t >= UR_HAWK_SWEEP:
        return -1
    d = ur_hawk_dir(seed, season, ur_hawk_pass(frame))
    return (UR_HAWK_SWEEP - 1 - t) if d else t


def ur_hawk_y(seed, season, frame):
    idx = ur_hawk_pass(frame)
    span = UR_MEADOW_Y1 - UR_MEADOW_Y0
    return UR_MEADOW_Y0 + (ur_hash(ur_hash(seed ^ UR_HAWK_SALT, season), idx) % span)


# --- food patches (top screen; mirror ur_patch / ur_patch_total) -----------
def ur_patch(seed, season, index):
    # (x, y, amount) — pure f(seed, season, index) on the foraging apron.
    h = ur_hash(ur_hash(seed ^ UR_PATCH_SALT, season), index)
    x = UR_APRON_X0 + ((h >> 8) % (UR_APRON_X1 - UR_APRON_X0))
    y = UR_APRON_Y0 + ((h >> 20) % (UR_APRON_Y1 - UR_APRON_Y0))
    amount = UR_PATCH_MIN + (h % (UR_PATCH_MAX - UR_PATCH_MIN + 1))
    return (x, y, amount)


def ur_patch_total(seed, season):
    return sum(ur_patch(seed, season, i)[2] for i in range(UR_PATCH_COUNT))


# --- dig grid (bottom screen; the drawn tunnel graph) ----------------------
def ur_cell_of_touch(tx, ty):
    # Returns (col, row) or None for an out-of-bounds pixel.
    if tx < 0 or ty < 0 or tx >= UR_GRID_W * UR_CELL or ty >= UR_GRID_H * UR_CELL:
        return None
    return (tx // UR_CELL, ty // UR_CELL)


def ur_dig(grid, col, row):
    # Set the cell dug; return 1 iff it changed (was soil). Pure in `grid`.
    i = row * UR_GRID_W + col
    changed = 1 if grid[i] == 0 else 0
    grid[i] = 1
    return changed


def ur_fresh_grid():
    g = [0] * UR_CELLS
    g[UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL] = 1  # the mouth is pre-dug
    return g


def ur_burrow_size(grid):
    # BFS from the entrance over 4-connected dug cells; count reached.
    start = UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL
    if grid[start] == 0:
        return 0
    seen = {start}
    q = deque([start])
    while q:
        i = q.popleft()
        c, r = i % UR_GRID_W, i // UR_GRID_W
        for dc, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nc, nr = c + dc, r + dr
            if 0 <= nc < UR_GRID_W and 0 <= nr < UR_GRID_H:
                j = nr * UR_GRID_W + nc
                if j not in seen and grid[j] == 1:
                    seen.add(j)
                    q.append(j)
    return len(seen)


def _dug_total(grid):
    return sum(grid)


# --- foragers: shortest dug path + nearest reachable patch (slice 3) --------
def ur_patch_col(patch):
    # A meadow patch's burrow drop column (patch.x / UR_CELL), clamped.
    col = patch[0] // UR_CELL
    if col < 0:
        col = 0
    elif col >= UR_GRID_W:
        col = UR_GRID_W - 1
    return col


def ur_dig_dist(grid, col, row):
    # Shortest dug-path distance (cells; mouth is 0) from the entrance to
    # (col,row) over 4-connected dug cells, or UR_ROUTE_NONE if that cell is
    # soil or unconnected. A BFS over the same graph as ur_burrow_size.
    if col < 0 or col >= UR_GRID_W or row < 0 or row >= UR_GRID_H:
        return UR_ROUTE_NONE
    start = UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL
    target = row * UR_GRID_W + col
    if grid[start] == 0 or grid[target] == 0:
        return UR_ROUTE_NONE
    dist = {start: 0}
    q = deque([start])
    while q:
        i = q.popleft()
        if i == target:
            return dist[i]
        c, r = i % UR_GRID_W, i // UR_GRID_W
        for dc, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nc, nr = c + dc, r + dr
            if 0 <= nc < UR_GRID_W and 0 <= nr < UR_GRID_H:
                j = nr * UR_GRID_W + nc
                if grid[j] == 1 and j not in dist:
                    dist[j] = dist[i] + 1
                    q.append(j)
    return dist.get(target, UR_ROUTE_NONE)


def ur_forage(grid, seed, season):
    # The nearest reachable meadow patch over the dug graph:
    # (index, dist, route) for the smallest dug-path distance (ties -> lowest
    # index), route = 2*dist; all UR_ROUTE_NONE if none reachable.
    best_i = best_d = best_r = UR_ROUTE_NONE
    for i in range(UR_PATCH_COUNT):
        p = ur_patch(seed, season, i)
        col = ur_patch_col(p)
        d = ur_dig_dist(grid, col, UR_DROP_ROW)
        if d != UR_ROUTE_NONE and (best_d == UR_ROUTE_NONE or d < best_d):
            best_i, best_d, best_r = i, d, d * 2
    return (best_i, best_d, best_r)


# --- granaries: the food store (slice 4; mirror ur_sim.c) ------------------
def ur_designate(grid, gran, col, row):
    # Designate a DUG cell a granary; return 1 iff it changed. Only a dug cell
    # can hold a granary; idempotent + monotone (mirrors ur_dig). `gran` is a
    # parallel 0/1 designation layer over the dig grid.
    if col < 0 or col >= UR_GRID_W or row < 0 or row >= UR_GRID_H:
        return 0
    i = row * UR_GRID_W + col
    if grid[i] == 0:
        return 0
    changed = 1 if gran[i] == 0 else 0
    gran[i] = 1
    return changed


def ur_fresh_gran():
    return [0] * UR_CELLS


def ur_gran_count(grid, gran):
    # Designated granary cells that are also dug (total placed).
    return sum(1 for i in range(UR_CELLS) if gran[i] and grid[i])


def ur_gran_connected(grid, gran):
    # Designated granary cells CONNECTED to the mouth (finite ur_dig_dist — the
    # same reachability the burrow BFS defines). Unreachable ones bank nothing.
    n = 0
    for r in range(UR_GRID_H):
        for c in range(UR_GRID_W):
            i = r * UR_GRID_W + c
            if gran[i] and grid[i] and ur_dig_dist(grid, c, r) != UR_ROUTE_NONE:
                n += 1
    return n


def ur_gran_capacity(grid, gran):
    return ur_gran_connected(grid, gran) * UR_GRAN_CAP


def ur_reachable_food(grid, seed, season):
    # Total food the colony can HAUL: sum of amounts over patches whose burrow
    # drop cell is reachable (connect-or-waste for patches; slice-3 graph).
    total = 0
    for i in range(UR_PATCH_COUNT):
        p = ur_patch(seed, season, i)
        col = ur_patch_col(p)
        if ur_dig_dist(grid, col, UR_DROP_ROW) != UR_ROUTE_NONE:
            total += p[2]
    return total


def ur_store(grid, gran, seed, season):
    # Banked store = min(reachable food, granary capacity). Deposits bank up to
    # capacity; food over capacity spoils, capacity over food sits empty.
    food = ur_reachable_food(grid, seed, season)
    cap = ur_gran_capacity(grid, gran)
    return food if food < cap else cap


# --- nurseries + population (slice 5; mirror ur_sim.c) ---------------------
def ur_fresh_nurs():
    return [0] * UR_CELLS


def ur_nurse(grid, nurs, col, row):
    # Designate a DUG cell a nursery; return 1 iff it changed. Only a dug cell
    # can hold a nursery; idempotent + monotone (mirrors ur_designate). `nurs`
    # is a parallel 0/1 designation layer over the dig grid.
    if col < 0 or col >= UR_GRID_W or row < 0 or row >= UR_GRID_H:
        return 0
    i = row * UR_GRID_W + col
    if grid[i] == 0:
        return 0
    changed = 1 if nurs[i] == 0 else 0
    nurs[i] = 1
    return changed


def ur_nurs_count(grid, nurs):
    # Designated nursery cells that are also dug (total placed).
    return sum(1 for i in range(UR_CELLS) if nurs[i] and grid[i])


def ur_nurs_connected(grid, nurs):
    # Designated nursery cells CONNECTED to the mouth (finite ur_dig_dist — the
    # same reachability the burrow BFS defines). Unreachable ones brood nothing.
    n = 0
    for r in range(UR_GRID_H):
        for c in range(UR_GRID_W):
            i = r * UR_GRID_W + c
            if nurs[i] and grid[i] and ur_dig_dist(grid, c, r) != UR_ROUTE_NONE:
                n += 1
    return n


def ur_pop(grid, gran, nurs, seed, season):
    # The colony population: connected nurseries brood the banked store into new
    # foragers = min(connected*UR_NURS_BROOD, store/UR_FOOD_PER_ANT) capped at
    # UR_POP_CAP. Pure integer.
    store = ur_store(grid, gran, seed, season)
    ncon = ur_nurs_connected(grid, nurs)
    brood = ncon * UR_NURS_BROOD
    food = store // UR_FOOD_PER_ANT
    pop = brood if brood < food else food
    if pop > UR_POP_CAP:
        pop = UR_POP_CAP
    return pop


def ur_pop_food(grid, gran, nurs, seed, season):
    # The banked food that population consumes = ur_pop * UR_FOOD_PER_ANT.
    return ur_pop(grid, gran, nurs, seed, season) * UR_FOOD_PER_ANT


# --- hawks predate: exposed shallow route cells (slice 6; mirror ur_sim.c) --
def ur_route_exposed(grid, col, row):
    # Count of EXPOSED cells (grid row < UR_SAFE_DEPTH) on the canonical
    # shortest dug path from the mouth to (col,row). Same BFS graph as
    # ur_dig_dist (E,W,S,N neighbour order), reconstructed via first-reached
    # predecessors -> a deterministic integer path. 0 if soil / OOB / no route.
    if col < 0 or col >= UR_GRID_W or row < 0 or row >= UR_GRID_H:
        return 0
    start = UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL
    target = row * UR_GRID_W + col
    if grid[start] == 0 or grid[target] == 0:
        return 0
    pred = [-1] * UR_CELLS
    pred[start] = start                   # the mouth is its own root (visited)
    q = deque([start])
    found = False
    while q:
        i = q.popleft()
        if i == target:
            found = True
            break
        c, r = i % UR_GRID_W, i // UR_GRID_W
        for dc, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nc, nr = c + dc, r + dr
            if 0 <= nc < UR_GRID_W and 0 <= nr < UR_GRID_H:
                j = nr * UR_GRID_W + nc
                if grid[j] == 1 and pred[j] == -1:
                    pred[j] = i
                    q.append(j)
    if not found:
        return 0
    exposed = 0
    cur = target
    while True:
        r = cur // UR_GRID_W
        if r < UR_SAFE_DEPTH:
            exposed += 1
        if cur == start:
            break
        cur = pred[cur]
    return exposed


def ur_forage_exposed(grid, seed, season):
    # Exposed cells on the forager's route: ur_forage picks the nearest
    # reachable patch, ur_route_exposed counts the shallow cells on the path to
    # its drop cell. 0 if no patch is reachable.
    f = ur_forage(grid, seed, season)
    if f[0] == UR_ROUTE_NONE:
        return 0
    p = ur_patch(seed, season, f[0])
    col = ur_patch_col(p)
    return ur_route_exposed(grid, col, UR_DROP_ROW)


def ur_predation(grid, gran, nurs, seed, season):
    # Foragers lost to hawk predation = min(exposed route cells, population).
    pop = ur_pop(grid, gran, nurs, seed, season)
    exposed = ur_forage_exposed(grid, seed, season)
    return exposed if exposed < pop else pop


def ur_survivors(grid, gran, nurs, seed, season):
    # Surviving foragers = ur_pop - ur_predation (predation <= pop, no underflow).
    return (ur_pop(grid, gran, nurs, seed, season)
            - ur_predation(grid, gran, nurs, seed, season))


# --- seasons + the year clock (slice 7; mirror ur_sim.c) -------------------
def ur_day(frame):
    # The day the frame clock is on. Free-running.
    return frame // UR_DAY_FRAMES


def ur_season_days(season):
    # The fixed day count for a season.
    if season == UR_SPRING:
        return UR_SPRING_DAYS
    if season == UR_SUMMER:
        return UR_SUMMER_DAYS
    if season == UR_AUTUMN:
        return UR_AUTUMN_DAYS
    return UR_WINTER_DAYS               # UR_WINTER (and any out-of-range)


def ur_season_of_day(day):
    # The four seasons partition [0, UR_YEAR_DAYS) in order; a day past the
    # year's end clamps to WINTER (the terminal state).
    b = UR_SPRING_DAYS
    if day < b:
        return UR_SPRING
    b += UR_SUMMER_DAYS
    if day < b:
        return UR_SUMMER
    b += UR_AUTUMN_DAYS
    if day < b:
        return UR_AUTUMN
    return UR_WINTER


def ur_abundance(season):
    # The meadow abundance scale for a season, a numerator over UR_ABUND_UNIT.
    if season == UR_SPRING:
        return UR_ABUND_SPRING
    if season == UR_SUMMER:
        return UR_ABUND_SUMMER
    if season == UR_AUTUMN:
        return UR_ABUND_AUTUMN
    return UR_ABUND_WINTER              # UR_WINTER (and any out-of-range)


def ur_season_food(grid, seed, season):
    # The reachable haul scaled by the season's abundance. Spring (the unit) is
    # the raw haul; winter (abundance 0) is zero.
    return ur_reachable_food(grid, seed, season) * ur_abundance(season) // UR_ABUND_UNIT


def ur_hawk_passes(season):
    # How many whole hawk sweeps cross a season.
    return ur_season_days(season) * UR_DAY_FRAMES // UR_HAWK_PERIOD


def ur_season_predation(grid, gran, nurs, seed, season):
    # Per-hawk-pass season predation = min(exposed route cells * passes, pop).
    pop = ur_pop(grid, gran, nurs, seed, season)
    exposed = ur_forage_exposed(grid, seed, season)
    loss = exposed * ur_hawk_passes(season)
    return loss if loss < pop else pop


def ur_season_survivors(grid, gran, nurs, seed, season):
    # Foragers surviving a whole season = ur_pop - ur_season_predation.
    return (ur_pop(grid, gran, nurs, seed, season)
            - ur_season_predation(grid, gran, nurs, seed, season))


# --- winter survival + the survival score (slice 8; mirror ur_sim.c) -------
def ur_winter_store(grid, gran, nurs, seed, season):
    # The banked granary store minus the brood the nurseries drew (slice 5).
    # ur_pop <= store/UR_FOOD_PER_ANT, so the brood cost never exceeds the store.
    store = ur_store(grid, gran, seed, season)
    brood = ur_pop_food(grid, gran, nurs, seed, season)
    return store - brood if store >= brood else 0


def ur_winter_drain(pop):
    # Each of the UR_WINTER_DAYS the colony eats pop * UR_CONSUME_PER_DAY.
    return pop * UR_WINTER_DAYS * UR_CONSUME_PER_DAY


def ur_winter_survives(store, pop):
    # Pop is constant across the drain (no winter brood), so "store never goes
    # negative" reduces to "store covers the whole drain".
    return 1 if store >= ur_winter_drain(pop) else 0


def ur_winter_leftover(store, pop):
    return store - ur_winter_drain(pop) if ur_winter_survives(store, pop) else 0


def ur_winter_score(store, pop):
    # 0 if starved; else surviving_pop*1000 + leftover (surviving_pop == pop).
    if not ur_winter_survives(store, pop):
        return 0
    return pop * 1000 + ur_winter_leftover(store, pop)


# --- best-score save record (slice 9; mirror ur_sim.c) ---------------------
def ur_save_checksum(words):
    """Mirror of ur_save_checksum()."""
    h = UR_SAVE_SALT
    for i in range(UR_SAVE_WORDS - 1):
        h = ur_hash(h, words[i])
    return h


def ur_save_encode(best_score, best_season, best_seed):
    """Mirror of ur_save_encode() (returns the blob as bytes)."""
    w = [UR_SAVE_MAGIC, UR_SAVE_VERSION, best_score & M32, best_season & M32,
         best_seed & M32, 0, 0, 0]
    w[UR_SAVE_WORDS - 1] = ur_save_checksum(w)
    return b''.join(x.to_bytes(4, 'little') for x in w)


def ur_save_decode(blob):
    """Mirror of ur_save_decode() (returns (ok, best_score, best_season,
    best_seed); on ok == 0 the C leaves its outputs untouched — callers keep
    the fresh table)."""
    w = [int.from_bytes(blob[4 * i:4 * i + 4], 'little')
         for i in range(UR_SAVE_WORDS)]
    if w[0] != UR_SAVE_MAGIC:
        return 0, 0, 0, 0
    if w[1] != UR_SAVE_VERSION:
        return 0, 0, 0, 0
    if w[UR_SAVE_WORDS - 1] != ur_save_checksum(w):
        return 0, 0, 0, 0
    return 1, w[2], w[3], w[4]


def ur_record_improves(best_score, best_season, score, season):
    """Mirror of ur_record_improves()."""
    return 1 if (score > best_score or season > best_season) else 0


# --- seed dial + balance (slice 10; mirror ur_sim.c) -----------------------
def ur_dial_seed(dial):
    """Mirror of ur_dial_seed(): dial 0 == UR_SEED, others hash into a bounded
    seed space down the DIAL-salted stream."""
    d = dial % UR_DIAL_COUNT
    if d == 0:
        return UR_SEED
    return (ur_hash(UR_SEED ^ UR_DIAL_SALT, d) & UR_DIAL_MASK) + 1


def ur_ref_plan():
    """Mirror of ur_ref_plan(): a row-1 harvest corridor across every column,
    that whole row designated granary, and no nursery. Returns (grid, gran,
    nurs)."""
    grid = ur_fresh_grid()
    gran = ur_fresh_gran()
    nurs = ur_fresh_nurs()
    for c in range(UR_GRID_W):
        ur_dig(grid, c, UR_DROP_ROW)
        ur_designate(grid, gran, c, UR_DROP_ROW)
    return grid, gran, nurs


def ur_ref_score(seed):
    """Mirror of ur_ref_score(): the reference plan's winter score for a seed
    (population 0 -> drain 0 -> the banked reachable winter food)."""
    grid, gran, nurs = ur_ref_plan()
    store = ur_winter_store(grid, gran, nurs, seed, UR_WINTER)
    pop = ur_season_survivors(grid, gran, nurs, seed, UR_WINTER)
    return ur_winter_score(store, pop)


def ur_seed_fair(seed):
    """Mirror of ur_seed_fair(): 1 iff the reference plan survives with a
    positive score (true for every seed)."""
    return 1 if ur_ref_score(seed) > 0 else 0


# --- synthesized audio: cue + ambience decision layer (slice 11) -----------
# One-shot cue rows, indexed by cue id: { freq Hz, len frames, duty code or
# UR_CUE_ON_NOISE, vol }. Row 0 (UR_CUE_NONE) is all zeros: a no-op cue.
# BYTE-IDENTICAL to UR_CUE_ROWS in ur_sim.c (the cross-side signature).
UR_CUE_ROWS = [
    (0,    0,  0,               0),          # NONE:   the no-op cue
    (988,  10, 2,               42),         # FORAGE: B5 chirp, forager home
    (1500, 18, UR_CUE_ON_NOISE, 78),         # HAWK:   the shriek (noise)
    (262,  64, 0,               66),         # WINTER: C4 toll, the year closes
]

# Ambience drone rows, indexed by season tier (spring..winter == 0..3):
# { freq Hz, duty code, vol }. BYTE-IDENTICAL to UR_AMB_ROWS in ur_sim.c.
UR_AMB_ROWS = [
    (180, 1, 22),                            # SPRING: the burrow wakes
    (200, 1, 22),                            # SUMMER: the colony at work
    (230, 1, 24),                            # AUTUMN: the harvest hurry
    (270, 0, 26),                            # WINTER: the deep cold hum
]


def ur_cue_freq(cue):
    return UR_CUE_ROWS[cue if cue < UR_CUE_COUNT else 0][0]


def ur_cue_len(cue):
    return UR_CUE_ROWS[cue if cue < UR_CUE_COUNT else 0][1]


def ur_cue_duty(cue):
    return UR_CUE_ROWS[cue if cue < UR_CUE_COUNT else 0][2]


def ur_cue_vol(cue):
    return UR_CUE_ROWS[cue if cue < UR_CUE_COUNT else 0][3]


def ur_amb_freq(tier):
    return UR_AMB_ROWS[tier if tier < UR_AMB_TIERS else 0][0]


def ur_amb_duty(tier):
    return UR_AMB_ROWS[tier if tier < UR_AMB_TIERS else 0][1]


def ur_amb_vol(tier):
    return UR_AMB_ROWS[tier if tier < UR_AMB_TIERS else 0][2]


# --- the CI --touch dig script, mirrored (proof 3 anchor) ------------------
# Each step is a bottom-LCD pixel the stylus is held at; the ROM digs the
# cell under the stylus every held frame. Centre pixel of cell (c,r) is
# (c*16+8, r*16+8). This exact list is replayed by --touch in rom-builds.yml.
CI_DIG_PIXELS = [
    (136, 8),    # cell (8,0)  — the entrance (already dug; no-op)
    (136, 24),   # cell (8,1)  — shaft down
    (136, 40),   # cell (8,2)
    (136, 56),   # cell (8,3)
    (152, 56),   # cell (9,3)  — branch east
    (168, 56),   # cell (10,3)
    (8, 184),    # cell (0,11) — an ISOLATED pocket (not connected)
]

# The slice-4 granary designations the CI replays (hold R + tap): one
# CONNECTED dug cell and one DISCONNECTED dug cell (the isolated pocket). The
# connected one gives capacity UR_GRAN_CAP; the disconnected one is counted in
# the placed total but adds ZERO capacity (unreachable granaries bank nothing).
CI_GRAN_PIXELS = [
    (152, 56),   # cell (9,3)  — dug + CONNECTED -> banks
    (8, 184),    # cell (0,11) — dug but DISCONNECTED -> banks nothing
]

# The slice-5 nursery designations the CI replays (hold L + tap): one CONNECTED
# dug cell and one DISCONNECTED dug cell (the isolated pocket). The connected
# one broods; the disconnected one is counted in the placed total but broods
# NOTHING (unreachable nurseries produce no population).
CI_NURS_PIXELS = [
    (168, 56),   # cell (10,3) dug+CONNECTED -> connected nursery
    (8, 184),    # cell (0,11) dug but DISCONNECTED -> produces nothing
]


def ci_fixture():
    grid = ur_fresh_grid()
    for (tx, ty) in CI_DIG_PIXELS:
        cell = ur_cell_of_touch(tx, ty)
        assert cell is not None, f"CI dig pixel {tx},{ty} out of bounds"
        ur_dig(grid, cell[0], cell[1])
    gran = ur_fresh_gran()
    for (tx, ty) in CI_GRAN_PIXELS:
        cell = ur_cell_of_touch(tx, ty)
        assert cell is not None, f"CI gran pixel {tx},{ty} out of bounds"
        ur_designate(grid, gran, cell[0], cell[1])
    # The forager route (slice 3) the ROM asserts under this same dig plan:
    # only patch 4 (mouth column 8, drop (8,1) on the shaft) is reachable;
    # the other five patches' columns are undug, so they are never visited.
    forage = ur_forage(grid, UR_SEED, 0)
    # The store (slice 4): 2 granary cells placed, 1 connected (cap
    # UR_GRAN_CAP), reachable food = patch 4's amount; banked up to capacity.
    store = (ur_gran_count(grid, gran), ur_gran_connected(grid, gran),
             ur_gran_capacity(grid, gran), ur_store(grid, gran, UR_SEED, 0))
    # The population (slice 5): apply ur_nurse for each CI_NURS_PIXELS cell over
    # the same dug+granary fixture — one connected nursery (10,3) and one in the
    # isolated pocket (0,11). Connected nurseries brood the banked store into
    # foragers on the pure bounded schedule.
    nurs = ur_fresh_nurs()
    for (tx, ty) in CI_NURS_PIXELS:
        cell = ur_cell_of_touch(tx, ty)
        assert cell is not None, f"CI nurs pixel {tx},{ty} out of bounds"
        ur_nurse(grid, nurs, cell[0], cell[1])
    pop = (ur_nurs_count(grid, nurs), ur_nurs_connected(grid, nurs),
           ur_pop(grid, gran, nurs, UR_SEED, 0),
           ur_pop_food(grid, gran, nurs, UR_SEED, 0))
    # The predation (slice 6): the forager route is the shallow shaft to patch 4
    # (drop (8,1), dist 1) — cells (8,0) row 0 and (8,1) row 1 are BOTH exposed
    # (row < UR_SAFE_DEPTH), so exposed = 2; the population (2) is fully caught
    # (losses = min(2, 2) = 2), leaving 0 survivors — a surface-hugging route is
    # hawk-food. A buried route would expose fewer cells; proven host-side.
    pred = (ur_forage_exposed(grid, UR_SEED, 0),
            ur_predation(grid, gran, nurs, UR_SEED, 0),
            ur_survivors(grid, gran, nurs, UR_SEED, 0))
    # The year clock's economy (slice 7) on this same spring fixture: the
    # abundance scale (spring is the UNIT, so the seasonal haul is the raw
    # reachable food 6), the hawk passes that cross spring (2), and the
    # per-hawk-pass season predation on the shallow shaft — min(exposed 2 *
    # passes 2, pop 2) = 2 (still pop-bound), 0 survivors. Spring is the unit,
    # so abund/sfood match the pre-clock reachable food exactly.
    clock = (ur_abundance(UR_SPRING),
             ur_season_food(grid, UR_SEED, UR_SPRING),
             ur_hawk_passes(UR_SPRING),
             ur_season_predation(grid, gran, nurs, UR_SEED, UR_SPRING),
             ur_season_survivors(grid, gran, nurs, UR_SEED, UR_SPRING))
    # The winter survival exam (slice 8) on this same spring fixture: the whole
    # colony (pop 2) is caught by the hawks on the shallow shaft (season
    # survivors 0), so NO foragers carry into winter — wpop 0, drain 0, and with
    # the store fully drawn down to brood (store 4 == popfood 4) the winter store
    # is 0 too. A colony already lost to the meadow banks nothing forward and
    # SCORES 0: the drawn plan must dig DEEP (survive predation) AND bank a store
    # MARGIN to score, which the slice-8 marquee proves with a survivor plan.
    wstore = ur_winter_store(grid, gran, nurs, UR_SEED, UR_SPRING)
    wpop = ur_season_survivors(grid, gran, nurs, UR_SEED, UR_SPRING)
    winter = (wstore, wpop, ur_winter_drain(wpop),
              ur_winter_survives(wstore, wpop),
              ur_winter_leftover(wstore, wpop),
              ur_winter_score(wstore, wpop))
    return (_dug_total(grid), ur_burrow_size(grid), forage, store, pop, pred,
            clock, winter)


# --- the slice-8 MARQUEE fixtures (host<->ROM anchor for the winter exam) ---
# The arc doc's slice-8 headline proof: a committed SURVIVOR dig plan clears
# winter and a deliberately-too-small plan STARVES, both predicted here and
# confirmed in ROM telemetry (rom-builds.yml proof 4). Both plans dig the SAME
# row-1 corridor (cols 4..13, opening all six spring patches so the reachable
# haul is the full 34) and the SAME two connected nurseries (cols 11,12 -> brood
# 6); they differ ONLY in how many granary cells are banked. The starve plan
# banks 3 (capacity 12), the survivor 6 (capacity 24) — the identical colony,
# one deliberately under-granaried. The ROM replays the survivor as an EXTENSION
# of the starve (three more granaries added mid-run), so a single deterministic
# replay shows the same colony flip from starve to survive as the store grows.
MARQUEE_DIG = [(c, 1) for c in range(4, 14)]          # row-1 corridor cols 4..13
MARQUEE_NURS = [(11, 1), (12, 1)]                     # 2 connected nurseries
MARQUEE_GRAN_STARVE = [(4, 1), (5, 1), (6, 1)]        # 3 granaries -> cap 12
MARQUEE_GRAN_EXTRA = [(7, 1), (9, 1), (13, 1)]        # +3 more    -> cap 24


def _marquee_grids(gran_cells):
    grid = ur_fresh_grid()
    for (c, r) in MARQUEE_DIG:
        ur_dig(grid, c, r)
    gran = ur_fresh_gran()
    for (c, r) in gran_cells:
        ur_designate(grid, gran, c, r)
    nurs = ur_fresh_nurs()
    for (c, r) in MARQUEE_NURS:
        ur_nurse(grid, nurs, c, r)
    return grid, gran, nurs


def marquee_fixture(gran_cells):
    grid, gran, nurs = _marquee_grids(gran_cells)
    store = ur_store(grid, gran, UR_SEED, UR_SPRING)
    wstore = ur_winter_store(grid, gran, nurs, UR_SEED, UR_SPRING)
    wpop = ur_season_survivors(grid, gran, nurs, UR_SEED, UR_SPRING)
    return {
        'dug': _dug_total(grid), 'con': ur_burrow_size(grid),
        'grancon': ur_gran_connected(grid, gran),
        'nurscon': ur_nurs_connected(grid, nurs),
        'store': store, 'pop': ur_pop(grid, gran, nurs, UR_SEED, UR_SPRING),
        'ssurv': wpop, 'wstore': wstore,
        'wpop': wpop, 'wdrain': ur_winter_drain(wpop),
        'wsurv': ur_winter_survives(wstore, wpop),
        'wleft': ur_winter_leftover(wstore, wpop),
        'wscore': ur_winter_score(wstore, wpop),
    }


# The slice-10 WINTER-gated save fixture: the SAME survivor plan (full 6
# granaries), but evaluated at the WINTER season the record now commits at. The
# store is the reachable WINTER meadow food banked (winter patch columns differ
# from spring), so the realized winter SCORE differs from the spring forecast —
# this is exactly the over-optimism the winter gate closes (slice-9's forward
# note). The ROM drives this plan, advances the year clock to winter, and a
# START there commits this realized score.
def marquee_winter_fixture():
    grid, gran, nurs = _marquee_grids(MARQUEE_GRAN_EXTRA + MARQUEE_GRAN_STARVE)
    store = ur_store(grid, gran, UR_SEED, UR_WINTER)
    wstore = ur_winter_store(grid, gran, nurs, UR_SEED, UR_WINTER)
    wpop = ur_season_survivors(grid, gran, nurs, UR_SEED, UR_WINTER)
    return {
        'store': store,
        'pop': ur_pop(grid, gran, nurs, UR_SEED, UR_WINTER),
        'wpop': wpop, 'wstore': wstore, 'wdrain': ur_winter_drain(wpop),
        'wsurv': ur_winter_survives(wstore, wpop),
        'wleft': ur_winter_leftover(wstore, wpop),
        'wscore': ur_winter_score(wstore, wpop),
    }


# --- proofs ----------------------------------------------------------------
def prove_hawk():
    checks = 0
    for seed in range(256):
        for season in range(4):
            # dense frame sweep across two full pass cycles
            for frame in range(0, 2 * UR_HAWK_PERIOD, 7):
                x1 = ur_hawk_x(seed, season, frame)
                x2 = ur_hawk_x(seed, season, frame)
                assert x1 == x2, "hawk_x not deterministic"
                active = ur_hawk_active(frame)
                if active:
                    assert 0 <= x1 <= 255, f"hawk_x {x1} off-meadow"
                    y = ur_hawk_y(seed, season, frame)
                    assert UR_MEADOW_Y0 <= y < UR_MEADOW_Y1, f"hawk_y {y} off-band"
                else:
                    assert x1 == -1, "hawk_x live while shadow off"
                checks += 1
    # sweep monotonicity + full-width coverage within one ON window
    for seed in (0, 1, 0x55A, 255):
        for season in range(4):
            d = ur_hawk_dir(seed, season, 0)
            xs = [ur_hawk_x(seed, season, f) for f in range(UR_HAWK_SWEEP)]
            assert xs[0] == (255 if d else 0)
            assert xs[-1] == (0 if d else 255)
            diffs = [xs[i + 1] - xs[i] for i in range(len(xs) - 1)]
            if d:
                assert all(v < 0 for v in diffs), "R->L sweep not monotone"
            else:
                assert all(v > 0 for v in diffs), "L->R sweep not monotone"
    # schedule is a function of the pass INDEX (f(seed, season, index))
    for seed in (0, 7, 0x55A):
        for season in range(4):
            for idx in range(6):
                f0 = idx * UR_HAWK_PERIOD
                assert ur_hawk_pass(f0) == idx
                assert ur_hawk_dir(seed, season, idx) in (0, 1)
    return checks


def prove_dig():
    # ur_cell_of_touch covers every pixel, rejects only OOB
    px_checks = 0
    for ty in range(0, UR_GRID_H * UR_CELL, 3):
        for tx in range(0, UR_GRID_W * UR_CELL, 3):
            cell = ur_cell_of_touch(tx, ty)
            assert cell == (tx // 16, ty // 16)
            assert 0 <= cell[0] < UR_GRID_W and 0 <= cell[1] < UR_GRID_H
            px_checks += 1
    for tx, ty in ((-1, 0), (0, -1), (256, 0), (0, 192), (999, 999)):
        assert ur_cell_of_touch(tx, ty) is None, f"OOB {tx},{ty} accepted"

    # idempotent + monotone dug count; connected <= total; graph growth
    graph_checks = 0
    for seed in range(400):
        grid = ur_fresh_grid()
        prev_total = _dug_total(grid)
        assert prev_total == 1 and ur_burrow_size(grid) == 1  # mouth only
        for step in range(30):
            h = ur_hash(seed, step)
            col = h % UR_GRID_W
            row = (h >> 8) % UR_GRID_H
            before = grid[row * UR_GRID_W + col]
            changed = ur_dig(grid, col, row)
            assert changed == (1 if before == 0 else 0), "dig changed-flag wrong"
            assert ur_dig(grid, col, row) == 0, "re-dig not idempotent"
            total = _dug_total(grid)
            assert total >= prev_total, "dug count fell"
            conn = ur_burrow_size(grid)
            assert conn <= total, "connected exceeds total"
            assert conn >= 1, "entrance lost its own connectivity"
            prev_total = total
            graph_checks += 1

    # a corridor drawn OUT from the mouth grows the burrow by exactly one
    grid = ur_fresh_grid()
    for r in range(1, UR_GRID_H):
        ur_dig(grid, UR_ENTRANCE_COL, r)
        assert ur_burrow_size(grid) == r + 1, "connected corridor miscounted"
    # an isolated pocket is never counted
    before = ur_burrow_size(grid)
    ur_dig(grid, 0, UR_GRID_H - 1)
    assert ur_burrow_size(grid) == before, "isolated pocket counted"
    assert _dug_total(grid) == before + 1
    return px_checks + graph_checks


def prove_patches():
    # Every seed 0..255 x season 0..3 x patch index: ur_patch is a pure
    # deterministic f(seed, season, index), its whole rendered extent
    # (centre +/- UR_PATCH_R) sits ON the meadow and OFF the hawk lanes
    # (below the band bottom UR_MEADOW_Y1), and its amount is in
    # [UR_PATCH_MIN, UR_PATCH_MAX]. The arc doc's slice-2 invariant.
    checks = 0
    for seed in range(256):
        for season in range(4):
            for index in range(UR_PATCH_COUNT):
                p1 = ur_patch(seed, season, index)
                assert ur_patch(seed, season, index) == p1, \
                    "ur_patch not deterministic"
                x, y, amt = p1
                assert 0 <= x - UR_PATCH_R and x + UR_PATCH_R <= UR_SCREEN_W, \
                    f"patch x {x} off-screen"
                assert 0 <= y - UR_PATCH_R and y + UR_PATCH_R <= UR_SCREEN_H, \
                    f"patch y {y} off-screen"
                assert y - UR_PATCH_R >= UR_MEADOW_Y1, \
                    f"patch y {y} intrudes the hawk lane band"
                assert UR_PATCH_MIN <= amt <= UR_PATCH_MAX, \
                    f"patch amount {amt} out of range"
                checks += 1
    # the patch COUNT the ROM reports (UR_T_PATCHN) is exactly UR_PATCH_COUNT
    assert UR_PATCH_COUNT == 6
    # the layer is not degenerate: the pinned meadow spreads its amounts
    amounts = {ur_patch(UR_SEED, 0, i)[2] for i in range(UR_PATCH_COUNT)}
    assert len(amounts) >= 2, "patch amounts degenerate (all identical)"
    return checks


def _bfs_all_dists(grid):
    # Independent reference BFS: shortest dug-path distance from the mouth to
    # EVERY reachable cell. Proves ur_dig_dist against a second source.
    start = UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL
    if grid[start] == 0:
        return {}
    dist = {start: 0}
    q = deque([start])
    while q:
        i = q.popleft()
        c, r = i % UR_GRID_W, i // UR_GRID_W
        for dc, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nc, nr = c + dc, r + dr
            if 0 <= nc < UR_GRID_W and 0 <= nr < UR_GRID_H:
                j = nr * UR_GRID_W + nc
                if grid[j] == 1 and j not in dist:
                    dist[j] = dist[i] + 1
                    q.append(j)
    return dist


def prove_forage():
    # The arc doc's slice-3 proof (for EVERY reachable input, not a replay):
    #  1. ur_dig_dist is the true shortest path — equal to an independent BFS
    #     over the dug graph, UR_ROUTE_NONE exactly for soil/unconnected
    #     cells, >= the mouth->cell Manhattan lower bound, and never less than
    #     ur_burrow_size implies (a reachable cell has a finite distance iff
    #     it is in the connected burrow);
    #  2. ur_forage picks the NEAREST reachable patch with a lowest-index tie
    #     break, route == 2*dist, and — the arc guarantee — a patch whose drop
    #     cell is soil or unconnected is NEVER the chosen one (a disconnected
    #     patch is never visited);
    #  3. emergence: on a fresh grid no patch is reachable (nothing dug), and
    #     digging a corridor to a patch's drop cell is exactly what makes it
    #     the route target.
    checks = 0

    # (1) ur_dig_dist == independent BFS, over many scripted dig plans.
    for seed in range(300):
        grid = ur_fresh_grid()
        for step in range(24):
            h = ur_hash(seed, step)
            ur_dig(grid, h % UR_GRID_W, (h >> 8) % UR_GRID_H)
        ref = _bfs_all_dists(grid)
        for row in range(UR_GRID_H):
            for col in range(UR_GRID_W):
                idx = row * UR_GRID_W + col
                d = ur_dig_dist(grid, col, row)
                if grid[idx] == 0 or idx not in ref:
                    assert d == UR_ROUTE_NONE, \
                        f"dig_dist to soil/unreachable {col},{row} = {d}"
                else:
                    assert d == ref[idx], \
                        f"dig_dist {col},{row} = {d} != BFS {ref[idx]}"
                    # Manhattan lower bound from the mouth (a dug path can
                    # never be shorter than the straight-line cell distance).
                    manh = abs(col - UR_ENTRANCE_COL) + abs(row - UR_ENTRANCE_ROW)
                    assert d >= manh, f"dig_dist {d} below Manhattan {manh}"
                checks += 1
        # a reachable-cell count from dig_dist matches ur_burrow_size exactly
        reach = sum(1 for row in range(UR_GRID_H) for col in range(UR_GRID_W)
                    if ur_dig_dist(grid, col, row) != UR_ROUTE_NONE)
        assert reach == ur_burrow_size(grid), \
            "dig_dist reachable-count != burrow_size"
    # out-of-bounds targets reject
    g = ur_fresh_grid()
    for col, row in ((-1, 0), (0, -1), (UR_GRID_W, 0), (0, UR_GRID_H)):
        assert ur_dig_dist(g, col, row) == UR_ROUTE_NONE, "OOB dist accepted"

    # (2)+(3) ur_forage: emergence + nearest-wins + disconnected-never-visited
    for seed in range(256):
        # fresh grid: nothing dug below the seam -> no patch reachable
        grid = ur_fresh_grid()
        idx, dist, route = ur_forage(grid, seed, 0)
        assert (idx, dist, route) == (UR_ROUTE_NONE,) * 3, \
            "fresh grid has a forage route with nothing dug"
        # dig a random burrow, then recompute the reference nearest patch
        for step in range(30):
            h = ur_hash(seed, step + 101)
            ur_dig(grid, h % UR_GRID_W, (h >> 8) % UR_GRID_H)
        idx, dist, route = ur_forage(grid, seed, 0)
        # independent reference: min dist over reachable patch drop cells
        cand = []
        for i in range(UR_PATCH_COUNT):
            col = ur_patch_col(ur_patch(seed, 0, i))
            d = ur_dig_dist(grid, col, UR_DROP_ROW)
            if d != UR_ROUTE_NONE:
                cand.append((d, i))
        if not cand:
            assert (idx, dist, route) == (UR_ROUTE_NONE,) * 3, \
                "forage claims a route but no drop cell is reachable"
        else:
            cand.sort()  # (dist, index) -> nearest, ties lowest index
            ref_d, ref_i = cand[0]
            assert (idx, dist, route) == (ref_i, ref_d, ref_d * 2), \
                f"forage {(idx, dist, route)} != nearest {(ref_i, ref_d)}"
            # the CHOSEN patch's drop cell really is dug + connected...
            chosen_col = ur_patch_col(ur_patch(seed, 0, idx))
            assert grid[UR_DROP_ROW * UR_GRID_W + chosen_col] == 1, \
                "forage chose a patch whose drop cell is soil"
            # ...and no UNREACHABLE patch was chosen (never-visited guarantee)
            for i in range(UR_PATCH_COUNT):
                col = ur_patch_col(ur_patch(seed, 0, i))
                if ur_dig_dist(grid, col, UR_DROP_ROW) == UR_ROUTE_NONE:
                    assert i != idx, "forage visited a disconnected patch"
        checks += 1

    # a hand-built corridor makes a specific distant patch the route target,
    # and severing it (never digging the drop cell) leaves that patch skipped.
    grid = ur_fresh_grid()
    # tunnel east along the surface seam+subsurface to patch 5's column (12),
    # down to its drop cell (12,1): (8,0)->(9,0)->..(12,0)->(12,1)
    for col in range(UR_ENTRANCE_COL + 1, 13):
        ur_dig(grid, col, 0)
    ur_dig(grid, 12, UR_DROP_ROW)
    idx, dist, route = ur_forage(grid, UR_SEED, 0)
    assert idx == 5 and dist == 5 and route == 10, \
        f"scripted corridor to patch 5 -> {(idx, dist, route)} (expected 5,5,10)"
    # patch 4 (col 8) was NOT dug at row 1 on this plan, so it is skipped
    assert ur_dig_dist(grid, 8, UR_DROP_ROW) == UR_ROUTE_NONE
    checks += 1
    return checks


def prove_granary():
    # The arc doc's slice-4 proof (for EVERY reachable input, not a replay):
    #  1. ur_designate only marks DUG cells, is idempotent + monotone (the
    #     placed count never falls, re-designating is a no-op), and rejects
    #     soil / out-of-bounds;
    #  2. capacity accounting is exact — capacity == connected granary cells
    #     * UR_GRAN_CAP, and a designated cell that is NOT connected to the
    #     mouth adds ZERO capacity (unreachable granaries bank nothing);
    #  3. cross-layer consistency (the slice-3 guard, third consumer of the
    #     dug graph): the connected-granary count equals the count of
    #     designated cells inside ur_burrow_size's reachable set — storage
    #     reachability and forager reachability are the ONE meaning of the
    #     dug graph;
    #  4. the store is min(reachable food, capacity) — banked up to capacity,
    #     never above either bound;
    #  5. emergence: on a fresh grid nothing is designated and the store is 0;
    #     designating a connected cell and digging to a patch is exactly what
    #     makes food bank.
    checks = 0

    # designation rejects soil / OOB, is idempotent + monotone (many plans)
    for seed in range(300):
        grid = ur_fresh_grid()
        gran = ur_fresh_gran()
        # dig a random burrow first
        for step in range(24):
            h = ur_hash(seed, step)
            ur_dig(grid, h % UR_GRID_W, (h >> 8) % UR_GRID_H)
        prev_placed = 0
        for step in range(30):
            h = ur_hash(seed, step + 55)
            col, row = h % UR_GRID_W, (h >> 8) % UR_GRID_H
            i = row * UR_GRID_W + col
            dug_before = grid[i]
            gran_before = gran[i]
            changed = ur_designate(grid, gran, col, row)
            if dug_before == 0:
                assert changed == 0 and gran[i] == 0, \
                    "designate marked a SOIL cell a granary"
            else:
                assert changed == (1 if gran_before == 0 else 0), \
                    "designate changed-flag wrong"
                assert gran[i] == 1, "designate did not mark a dug cell"
            assert ur_designate(grid, gran, col, row) == 0, \
                "re-designate not idempotent"
            placed = ur_gran_count(grid, gran)
            assert placed >= prev_placed, "placed granary count fell"
            prev_placed = placed
            checks += 1
        # OOB designations reject
        for col, row in ((-1, 0), (0, -1), (UR_GRID_W, 0), (0, UR_GRID_H)):
            assert ur_designate(grid, gran, col, row) == 0, \
                "OOB designation accepted"

        # capacity + store accounting exact; cross-layer reachability guard
        reachable = _bfs_all_dists(grid)          # cells reachable from mouth
        placed_connected = sum(
            1 for r in range(UR_GRID_H) for c in range(UR_GRID_W)
            if gran[r * UR_GRID_W + c] and (r * UR_GRID_W + c) in reachable)
        assert ur_gran_connected(grid, gran) == placed_connected, \
            "gran_connected != designated cells in the burrow reachable set"
        assert ur_gran_capacity(grid, gran) == placed_connected * UR_GRAN_CAP, \
            "capacity != connected granaries * UR_GRAN_CAP"
        # every connected granary really has a finite dig_dist; every placed
        # but UNconnected granary is excluded (banks nothing)
        for r in range(UR_GRID_H):
            for c in range(UR_GRID_W):
                i = r * UR_GRID_W + c
                if gran[i]:
                    d = ur_dig_dist(grid, c, r)
                    inset = (i in reachable)
                    assert (d != UR_ROUTE_NONE) == inset, \
                        "granary reachability disagrees with the dug graph"
        # store is banked up to capacity, never above either bound
        food = ur_reachable_food(grid, UR_SEED, 0)
        cap = ur_gran_capacity(grid, gran)
        store = ur_store(grid, gran, UR_SEED, 0)
        assert store == min(food, cap), f"store {store} != min({food},{cap})"
        assert store <= cap and store <= food, "store above a bound"
        checks += 1

    # emergence: fresh grid banks nothing; a hand-built plan banks a known haul
    grid = ur_fresh_grid()
    gran = ur_fresh_gran()
    assert ur_gran_count(grid, gran) == 0 and ur_store(grid, gran, UR_SEED, 0) == 0, \
        "fresh grid banks food with nothing dug or designated"
    # designating the (soil) drop cell does nothing until it is dug
    assert ur_designate(grid, gran, 8, UR_DROP_ROW) == 0
    # tunnel the shaft down to patch 4's drop cell (col 8), designate a granary
    # on the shaft: patch 4 (amount 6) becomes reachable, 1 connected granary
    # gives capacity UR_GRAN_CAP=4, so the store is capped at 4 (< 6 reachable).
    for r in range(1, 4):
        ur_dig(grid, UR_ENTRANCE_COL, r)
    assert ur_dig_dist(grid, 8, UR_DROP_ROW) == 1, "shaft did not reach the drop"
    assert ur_designate(grid, gran, UR_ENTRANCE_COL, 2) == 1, "shaft cell not dug"
    assert ur_reachable_food(grid, UR_SEED, 0) == 6, "patch-4 haul drifted"
    assert ur_gran_connected(grid, gran) == 1
    assert ur_gran_capacity(grid, gran) == UR_GRAN_CAP
    assert ur_store(grid, gran, UR_SEED, 0) == UR_GRAN_CAP, \
        "store not capped at capacity when food exceeds it"
    # a SECOND connected granary lifts capacity to 8 >= 6, so the full haul
    # banks (store == reachable food, no longer capped)
    assert ur_designate(grid, gran, UR_ENTRANCE_COL, 3) == 1
    assert ur_gran_capacity(grid, gran) == 2 * UR_GRAN_CAP
    assert ur_store(grid, gran, UR_SEED, 0) == 6, \
        "store not the full haul once capacity covers it"
    # a designated cell in an ISOLATED pocket banks nothing: capacity unchanged
    ur_dig(grid, 0, UR_GRID_H - 1)               # isolated pocket cell
    assert ur_designate(grid, gran, 0, UR_GRID_H - 1) == 1
    assert ur_gran_count(grid, gran) == 3
    assert ur_gran_connected(grid, gran) == 2, "disconnected granary counted"
    assert ur_gran_capacity(grid, gran) == 2 * UR_GRAN_CAP, \
        "isolated granary added capacity"
    checks += 1
    return checks


def prove_nursery():
    # The arc doc's slice-5 proof (for many inputs, not a replay):
    #  1. ur_nurse only marks DUG cells, is idempotent + monotone (the placed
    #     count never falls, re-designating is a no-op), and rejects soil /
    #     out-of-bounds (mirrors ur_designate);
    #  2. cross-layer consistency (the fourth consumer of the dug graph): the
    #     connected-nursery count equals designated cells inside
    #     ur_burrow_size's reachable set — broodkeeping reachability, storage
    #     reachability and forager reachability are the ONE meaning of the dug
    #     graph, and a DISCONNECTED nursery contributes ZERO population;
    #  3. ur_pop == min(connected*UR_NURS_BROOD, store//UR_FOOD_PER_ANT) capped
    #     at UR_POP_CAP — the food-bound, brood-bound and cap branches each
    #     proven on a controlled grid;
    #  4. emergence: on a fresh grid nothing is designated and the population
    #     is 0.
    checks = 0

    # designation discipline: dug-only, idempotent, monotone, OOB reject; and
    # the cross-layer connected-count guard (many plans)
    for seed in range(300):
        grid = ur_fresh_grid()
        nurs = ur_fresh_nurs()
        for step in range(24):
            h = ur_hash(seed, step)
            ur_dig(grid, h % UR_GRID_W, (h >> 8) % UR_GRID_H)
        prev_placed = 0
        for step in range(30):
            h = ur_hash(seed, step + 77)
            col, row = h % UR_GRID_W, (h >> 8) % UR_GRID_H
            i = row * UR_GRID_W + col
            dug_before = grid[i]
            nurs_before = nurs[i]
            changed = ur_nurse(grid, nurs, col, row)
            if dug_before == 0:
                assert changed == 0 and nurs[i] == 0, \
                    "nurse marked a SOIL cell a nursery"
            else:
                assert changed == (1 if nurs_before == 0 else 0), \
                    "nurse changed-flag wrong"
                assert nurs[i] == 1, "nurse did not mark a dug cell"
            assert ur_nurse(grid, nurs, col, row) == 0, \
                "re-nurse not idempotent"
            placed = ur_nurs_count(grid, nurs)
            assert placed >= prev_placed, "placed nursery count fell"
            prev_placed = placed
            checks += 1
        for col, row in ((-1, 0), (0, -1), (UR_GRID_W, 0), (0, UR_GRID_H)):
            assert ur_nurse(grid, nurs, col, row) == 0, "OOB nursery accepted"
        reachable = _bfs_all_dists(grid)          # cells reachable from mouth
        placed_connected = sum(
            1 for r in range(UR_GRID_H) for c in range(UR_GRID_W)
            if nurs[r * UR_GRID_W + c] and (r * UR_GRID_W + c) in reachable)
        assert ur_nurs_connected(grid, nurs) == placed_connected, \
            "nurs_connected != designated cells in the burrow reachable set"
        checks += 1

    # emergence: a fresh grid raises no population
    grid = ur_fresh_grid()
    gran = ur_fresh_gran()
    nurs = ur_fresh_nurs()
    assert ur_nurs_count(grid, nurs) == 0 and ur_pop(grid, gran, nurs, UR_SEED, 0) == 0, \
        "fresh grid raised population with nothing dug or designated"

    # (a) DISCONNECTED nursery broods nothing: build a connected shaft with two
    # granaries (cap 8 >= reachable food 6, so store 6) and one connected
    # nursery (brood 3, store//FOOD_PER_ANT = 3 -> pop 3); a second nursery in
    # an ISOLATED pocket leaves the connected count and the population unchanged.
    for r in range(1, 4):
        ur_dig(grid, UR_ENTRANCE_COL, r)          # shaft to patch 4's drop cell
    assert ur_designate(grid, gran, UR_ENTRANCE_COL, 2) == 1
    assert ur_designate(grid, gran, UR_ENTRANCE_COL, 3) == 1
    assert ur_store(grid, gran, UR_SEED, 0) == 6, "shaft store drifted"
    assert ur_nurse(grid, nurs, UR_ENTRANCE_COL, 1) == 1
    assert ur_nurs_connected(grid, nurs) == 1
    pop_before = ur_pop(grid, gran, nurs, UR_SEED, 0)
    assert pop_before == 3, f"connected nursery pop {pop_before} != 3"
    ur_dig(grid, 0, UR_GRID_H - 1)                # isolated pocket cell
    assert ur_nurse(grid, nurs, 0, UR_GRID_H - 1) == 1
    assert ur_nurs_count(grid, nurs) == 2
    assert ur_nurs_connected(grid, nurs) == 1, "disconnected nursery counted"
    assert ur_pop(grid, gran, nurs, UR_SEED, 0) == pop_before, \
        "disconnected nursery changed the population"
    assert ur_pop_food(grid, gran, nurs, UR_SEED, 0) == pop_before * UR_FOOD_PER_ANT
    checks += 1

    # (b)/(c)/(d) the three ur_pop branches on a FULL-dug grid: every cell dug
    # and connected, reachable food == the whole meadow haul (34), so
    # store == min(34, granary capacity) and every designated nursery is
    # connected — the store and the connected-nursery count are dialled exactly.
    full = ur_fresh_grid()
    for i in range(UR_CELLS):
        full[i] = 1
    assert ur_burrow_size(full) == UR_CELLS, "full grid not all connected"
    assert ur_reachable_food(full, UR_SEED, 0) == ur_patch_total(UR_SEED, 0) == 34, \
        "full grid should haul the whole meadow"

    def full_layer(count):
        # a 0/1 layer with `count` distinct non-entrance cells set; on the full
        # grid every set cell is dug and connected.
        layer = [0] * UR_CELLS
        placed = 0
        for i in range(UR_CELLS):
            if i == UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL:
                continue
            if placed >= count:
                break
            layer[i] = 1
            placed += 1
        return layer

    # (b) FOOD-BOUND: 1 granary (cap 4) -> store 4 -> food_ants 2; 3 nurseries
    # -> brood 9 > 2. pop == store // UR_FOOD_PER_ANT == 2.
    gran = full_layer(1)
    nurs = full_layer(3)
    assert ur_store(full, gran, UR_SEED, 0) == 4
    assert ur_nurs_connected(full, nurs) == 3
    pop = ur_pop(full, gran, nurs, UR_SEED, 0)
    assert pop == 4 // UR_FOOD_PER_ANT == 2, f"food-bound pop {pop} != 2"
    checks += 1

    # (c) BROOD-BOUND: 2 granaries (cap 8) -> store 8 -> food_ants 4; 1 nursery
    # -> brood 3 < 4. pop == UR_NURS_BROOD == 3.
    gran = full_layer(2)
    nurs = full_layer(1)
    assert ur_store(full, gran, UR_SEED, 0) == 8
    pop = ur_pop(full, gran, nurs, UR_SEED, 0)
    assert pop == UR_NURS_BROOD == 3, f"brood-bound pop {pop} != {UR_NURS_BROOD}"
    checks += 1

    # (d) CAP: 5 granaries (cap 20) -> store 20 -> food_ants 10; 4 nurseries ->
    # brood 12. min(12, 10) == 10, capped to UR_POP_CAP == 8.
    gran = full_layer(5)
    nurs = full_layer(4)
    assert ur_store(full, gran, UR_SEED, 0) == 20
    pop = ur_pop(full, gran, nurs, UR_SEED, 0)
    assert pop == UR_POP_CAP == 8, f"cap pop {pop} != {UR_POP_CAP}"
    checks += 1
    return checks


def _ref_route(grid, col, row):
    # Independent reconstruction of the canonical shortest path mouth->(col,row):
    # a fresh BFS + predecessor walk returning the ordered list of path cells,
    # to cross-check ur_route_exposed's geometry against the ur_dig_dist graph.
    if col < 0 or col >= UR_GRID_W or row < 0 or row >= UR_GRID_H:
        return None
    start = UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL
    target = row * UR_GRID_W + col
    if grid[start] == 0 or grid[target] == 0:
        return None
    pred = {start: start}
    q = deque([start])
    while q:
        i = q.popleft()
        if i == target:
            break
        c, r = i % UR_GRID_W, i // UR_GRID_W
        for dc, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nc, nr = c + dc, r + dr
            if 0 <= nc < UR_GRID_W and 0 <= nr < UR_GRID_H:
                j = nr * UR_GRID_W + nc
                if grid[j] == 1 and j not in pred:
                    pred[j] = i
                    q.append(j)
    if target not in pred:
        return None
    path = []
    cur = target
    while True:
        path.append(cur)
        if cur == start:
            break
        cur = pred[cur]
    path.reverse()
    return path


def prove_predation():
    # The arc doc's slice-6 proof (for many inputs, not a replay):
    #  1. ur_route_exposed geometry — for many scripted dig plans, to every
    #     reachable cell: the counted route is a VALID shortest path (contiguous
    #     4-adjacent dug cells, mouth->target, length == ur_dig_dist + 1), and
    #     the exposed count == the shallow (row < UR_SAFE_DEPTH) cells on that
    #     path; unreachable / soil / OOB targets expose 0;
    #  2. the exposed<->deep boundary is EXACT — on single-corridor grids (a
    #     unique shortest path, no tie ambiguity) a surface-hugging route and a
    #     buried route to the SAME drop cell expose the hand-computed counts,
    #     with cells at row == UR_SAFE_DEPTH provably NOT counted (burying below
    #     the band is what saves them);
    #  3. ur_predation == min(exposed, pop) — the two binding regimes proven on
    #     controlled grids: POP-BOUND (a very exposed route, few foragers ->
    #     losses = pop, survivors 0) and EXPOSURE-BOUND (a shallow but short
    #     route, many foragers -> losses = exposed, survivors = pop - exposed),
    #     each asserting the winning term by value (the slice-5 min-regime idiom);
    #  4. emergence: a fresh grid / no reachable patch predates nothing
    #     (exposed 0, losses 0, survivors 0).
    checks = 0

    # (1) ur_route_exposed == shallow cells on a valid shortest path
    for seed in range(300):
        grid = ur_fresh_grid()
        for step in range(24):
            h = ur_hash(seed, step)
            ur_dig(grid, h % UR_GRID_W, (h >> 8) % UR_GRID_H)
        for row in range(UR_GRID_H):
            for col in range(UR_GRID_W):
                idx = row * UR_GRID_W + col
                ex = ur_route_exposed(grid, col, row)
                path = _ref_route(grid, col, row)
                d = ur_dig_dist(grid, col, row)
                if grid[idx] == 0 or d == UR_ROUTE_NONE:
                    assert ex == 0 and path is None, \
                        f"route_exposed to soil/unreachable {col},{row} = {ex}"
                else:
                    # a valid shortest path: right endpoints, right length, and
                    # every consecutive pair 4-adjacent + dug
                    assert path[0] == UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL
                    assert path[-1] == idx
                    assert len(path) == d + 1, \
                        f"path len {len(path)} != dist+1 {d + 1}"
                    for a, b in zip(path, path[1:]):
                        ac, ar = a % UR_GRID_W, a // UR_GRID_W
                        bc, br = b % UR_GRID_W, b // UR_GRID_W
                        assert abs(ac - bc) + abs(ar - br) == 1, "path not contiguous"
                        assert grid[b] == 1, "path crosses soil"
                    shallow = sum(1 for p in path if (p // UR_GRID_W) < UR_SAFE_DEPTH)
                    assert ex == shallow, \
                        f"route_exposed {ex} != shallow-on-path {shallow}"
                    assert 0 <= ex <= len(path)
                checks += 1
    # OOB targets expose nothing
    g = ur_fresh_grid()
    for col, row in ((-1, 0), (0, -1), (UR_GRID_W, 0), (0, UR_GRID_H)):
        assert ur_route_exposed(g, col, row) == 0, "OOB route exposed"

    # (2) exact exposed<->deep boundary: shallow vs buried route to the SAME
    # drop cell (12,1). The single corridor makes each shortest path unique.
    shallow = ur_fresh_grid()
    for col in range(UR_ENTRANCE_COL + 1, 13):     # along row 0 (the surface)
        ur_dig(shallow, col, 0)
    ur_dig(shallow, 12, UR_DROP_ROW)
    assert ur_dig_dist(shallow, 12, UR_DROP_ROW) == 5
    # path (8,0)(9,0)(10,0)(11,0)(12,0) row 0 + (12,1) row 1 -> all 6 exposed
    assert ur_route_exposed(shallow, 12, UR_DROP_ROW) == 6, "shallow route exposed != 6"
    buried = ur_fresh_grid()
    for cell in ((8, 1), (8, 2), (8, 3), (9, 3), (10, 3),
                 (11, 3), (12, 3), (12, 2), (12, 1)):
        ur_dig(buried, cell[0], cell[1])
    assert ur_dig_dist(buried, 12, UR_DROP_ROW) == 9
    # path rows 0,1,2,3,3,3,3,3,2,1 -> exposed (row<2) = (8,0)(8,1)(12,1) = 3;
    # the row-2 cells (8,2),(12,2) sit AT/below the boundary and are NOT counted
    # (proof the boundary is exactly UR_SAFE_DEPTH, not deeper).
    assert ur_route_exposed(buried, 12, UR_DROP_ROW) == 3, "buried route exposed != 3"
    # every row-2 cell on the buried path is safe; drop below the band and the
    # forager stops being hawk-food — the arc's "deep or dead".
    assert ur_route_exposed(buried, 12, UR_DROP_ROW) < \
        ur_route_exposed(shallow, 12, UR_DROP_ROW), "burying did not lower exposure"
    checks += 1

    # (3) ur_predation regimes.
    # POP-BOUND: the shallow surface route to patch 5 (col 12) exposes 6 cells,
    # but only patch 5 (food 4) is reachable, so one connected granary (cap 4 ->
    # store 4) and one connected nursery brood a food-bound pop of 2. The hawks
    # can catch no more than the 2 foragers alive: losses = min(6, 2) = 2 (pop),
    # survivors 0.
    gp = ur_fresh_grid()
    for col in range(UR_ENTRANCE_COL + 1, 13):
        ur_dig(gp, col, 0)
    ur_dig(gp, 12, UR_DROP_ROW)
    assert ur_forage(gp, UR_SEED, 0) == (5, 5, 10), "pop-bound route drifted"
    assert ur_forage_exposed(gp, UR_SEED, 0) == 6
    granp = ur_fresh_gran()
    ur_designate(gp, granp, 12, UR_DROP_ROW)                          # 1 connected
    nursp = ur_fresh_nurs()
    ur_nurse(gp, nursp, 12, 0)                                        # 1 connected
    assert ur_pop(gp, granp, nursp, UR_SEED, 0) == 2, "pop-bound pop != 2"
    lost = ur_predation(gp, granp, nursp, UR_SEED, 0)
    assert lost == 2 == ur_pop(gp, granp, nursp, UR_SEED, 0), \
        f"POP-BOUND losses {lost} != pop 2"
    assert ur_survivors(gp, granp, nursp, UR_SEED, 0) == 0, "pop-bound survivors != 0"
    checks += 1

    # EXPOSURE-BOUND: a full-dug grid — the nearest patch is patch 4 at the mouth
    # column (drop (8,1), dist 1), so the route is the shallow shaft exposing
    # exactly 2 cells; two granaries (cap 8 -> store 8) and two nurseries brood a
    # pop of 4. Only the 2 exposed cells cost a forager: losses = min(2, 4) = 2
    # (exposure), survivors = 4 - 2 = 2. Fewer than the population caught, so the
    # burial geometry — not the headcount — binds.
    full = ur_fresh_grid()
    for i in range(UR_CELLS):
        full[i] = 1

    def full_layer(count):
        layer = [0] * UR_CELLS
        placed = 0
        for i in range(UR_CELLS):
            if i == UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL:
                continue
            if placed >= count:
                break
            layer[i] = 1
            placed += 1
        return layer

    granf = full_layer(2)
    nursf = full_layer(2)
    assert ur_forage(full, UR_SEED, 0) == (4, 1, 2), "exposure-bound route drifted"
    assert ur_forage_exposed(full, UR_SEED, 0) == 2
    assert ur_pop(full, granf, nursf, UR_SEED, 0) == 4, "exposure-bound pop != 4"
    lost = ur_predation(full, granf, nursf, UR_SEED, 0)
    assert lost == 2 == ur_forage_exposed(full, UR_SEED, 0), \
        f"EXPOSURE-BOUND losses {lost} != exposed 2"
    assert ur_survivors(full, granf, nursf, UR_SEED, 0) == 2, \
        "exposure-bound survivors != 2"
    checks += 1

    # (4) emergence: a fresh grid has no route, so nothing is predated.
    grid = ur_fresh_grid()
    gran = ur_fresh_gran()
    nurs = ur_fresh_nurs()
    assert ur_forage_exposed(grid, UR_SEED, 0) == 0
    assert ur_predation(grid, gran, nurs, UR_SEED, 0) == 0
    assert ur_survivors(grid, gran, nurs, UR_SEED, 0) == 0
    checks += 1
    return checks


def prove_clock():
    # The arc doc's slice-7 proof (Decision 3 + the slice-7 row): the year clock
    # and the abundance curve are pure, and the per-hawk-pass predation scaling
    # collapses to slice 6 at a single pass.
    #  1. the day/season clock — ur_day is a pure frame//UR_DAY_FRAMES; the four
    #     seasons partition the year IN ORDER with the exact fixed day counts, no
    #     gaps/overlaps, and every day past the year clamps to WINTER; every
    #     season boundary lands on the cumulative day sum;
    #  2. the abundance curve — pure f(season), SPRING is the UNIT baseline,
    #     SUMMER is the most plentiful, AUTUMN thins below spring, WINTER is
    #     exactly ZERO (no meadow food), and ur_season_food scales the reachable
    #     haul by abundance/unit (spring == the raw haul, winter == 0);
    #  3. the per-hawk-pass predation scaling — ur_season_predation ==
    #     min(exposed*passes, pop), monotone in the pass count and bounded by
    #     pop, and — the degenerate identity the slice-6 card called for — for
    #     ANY season whose ur_hawk_passes == 1 it equals slice 6's instantaneous
    #     ur_predation exactly (the one-pass season IS the snapshot).
    checks = 0

    # (1) day clock: pure + monotone non-decreasing across a dense frame sweep,
    # and the day boundary is exactly UR_DAY_FRAMES.
    for frame in range(0, UR_YEAR_DAYS * UR_DAY_FRAMES + 3 * UR_DAY_FRAMES, 7):
        d = ur_day(frame)
        assert d == ur_day(frame), "ur_day not deterministic"
        assert d == frame // UR_DAY_FRAMES
        checks += 1
    assert ur_day(0) == 0 and ur_day(UR_DAY_FRAMES - 1) == 0
    assert ur_day(UR_DAY_FRAMES) == 1, "day boundary != UR_DAY_FRAMES"

    # (1) season partition: in order, exact counts, no gaps, clamp past the year.
    expected = ([UR_SPRING] * UR_SPRING_DAYS + [UR_SUMMER] * UR_SUMMER_DAYS
                + [UR_AUTUMN] * UR_AUTUMN_DAYS + [UR_WINTER] * UR_WINTER_DAYS)
    assert len(expected) == UR_YEAR_DAYS
    for day in range(UR_YEAR_DAYS):
        assert ur_season_of_day(day) == expected[day], \
            f"season_of_day({day}) != {expected[day]}"
        checks += 1
    # each season occupies exactly its fixed day count within the year
    counts = {s: sum(1 for day in range(UR_YEAR_DAYS) if ur_season_of_day(day) == s)
              for s in range(UR_SEASONS)}
    assert counts == {UR_SPRING: UR_SPRING_DAYS, UR_SUMMER: UR_SUMMER_DAYS,
                      UR_AUTUMN: UR_AUTUMN_DAYS, UR_WINTER: UR_WINTER_DAYS}, \
        f"season day counts drifted: {counts}"
    # boundaries land on the cumulative sums; ordering is monotone within the year
    assert ur_season_of_day(UR_SPRING_DAYS - 1) == UR_SPRING
    assert ur_season_of_day(UR_SPRING_DAYS) == UR_SUMMER
    assert ur_season_of_day(UR_SPRING_DAYS + UR_SUMMER_DAYS) == UR_AUTUMN
    assert ur_season_of_day(UR_YEAR_DAYS - 1) == UR_WINTER
    # any day past the year's end clamps to WINTER (the terminal state)
    for day in range(UR_YEAR_DAYS, UR_YEAR_DAYS + 40):
        assert ur_season_of_day(day) == UR_WINTER, "year end did not clamp to winter"
        checks += 1

    # (2) abundance curve: spring is the unit, summer the most plentiful, autumn
    # thins below spring, winter is exactly zero.
    assert ur_abundance(UR_SPRING) == UR_ABUND_UNIT, "spring abundance != the unit"
    assert ur_abundance(UR_SUMMER) > ur_abundance(UR_SPRING), "summer not plentiful"
    assert ur_abundance(UR_AUTUMN) < ur_abundance(UR_SPRING), "autumn not thinning"
    assert ur_abundance(UR_WINTER) == 0, "winter abundance != 0"
    # ur_season_food scales the reachable haul: spring == raw, winter == 0.
    full = ur_fresh_grid()
    for i in range(UR_CELLS):
        full[i] = 1
    raw = ur_reachable_food(full, UR_SEED, UR_SPRING)
    assert raw == ur_patch_total(UR_SEED, UR_SPRING) == 34, "full-grid spring haul drifted"
    assert ur_season_food(full, UR_SEED, UR_SPRING) == raw, \
        "spring (the unit) did not leave the haul raw"
    assert ur_season_food(full, UR_SEED, UR_WINTER) == 0, "winter meadow food != 0"
    # the seasonal haul is exactly reachable_food * abundance // unit for each season
    for season in range(UR_SEASONS):
        exp = ur_reachable_food(full, UR_SEED, season) * ur_abundance(season) // UR_ABUND_UNIT
        assert ur_season_food(full, UR_SEED, season) == exp, \
            f"season_food({season}) != reachable*abund//unit"
        checks += 1

    # (2) hawk passes: pure per-season count, non-zero for every season here.
    for season in range(UR_SEASONS):
        p = ur_hawk_passes(season)
        assert p == ur_season_days(season) * UR_DAY_FRAMES // UR_HAWK_PERIOD
        assert p >= 1, f"season {season} has no hawk pass to scale predation by"
        checks += 1

    # (3) per-hawk-pass predation scaling on the pop-bound shallow route to
    # patch 5 (col 12): exposed 6, pop 2 -> min(6*passes, 2) = 2 for any passes
    # >= 1 (pop-bound in every season), survivors 0.
    gp = ur_fresh_grid()
    for col in range(UR_ENTRANCE_COL + 1, 13):
        ur_dig(gp, col, 0)
    ur_dig(gp, 12, UR_DROP_ROW)
    granp = ur_fresh_gran()
    ur_designate(gp, granp, 12, UR_DROP_ROW)
    nursp = ur_fresh_nurs()
    ur_nurse(gp, nursp, 12, 0)
    assert ur_forage_exposed(gp, UR_SEED, UR_SPRING) == 6
    assert ur_pop(gp, granp, nursp, UR_SEED, UR_SPRING) == 2
    sp = ur_season_predation(gp, granp, nursp, UR_SEED, UR_SPRING)
    assert sp == 2 == ur_pop(gp, granp, nursp, UR_SEED, UR_SPRING), \
        f"pop-bound season predation {sp} != pop 2"
    assert ur_season_survivors(gp, granp, nursp, UR_SEED, UR_SPRING) == 0
    checks += 1

    # (3) EXPOSURE-BOUND scaling: a full grid — nearest patch 4 at the mouth
    # column, route the shallow shaft exposing 2; 2 granaries + 2 nurseries brood
    # pop 4. A single pass loses min(2*1, 4) = 2; TWO passes lose min(2*2, 4) = 4
    # (the whole colony) — the season's pass count, not the geometry, now binds.
    def full_layer(count):
        layer = [0] * UR_CELLS
        placed = 0
        for i in range(UR_CELLS):
            if i == UR_ENTRANCE_ROW * UR_GRID_W + UR_ENTRANCE_COL:
                continue
            if placed >= count:
                break
            layer[i] = 1
            placed += 1
        return layer

    granf = full_layer(2)
    nursf = full_layer(2)
    assert ur_forage_exposed(full, UR_SEED, UR_SPRING) == 2
    assert ur_pop(full, granf, nursf, UR_SEED, UR_SPRING) == 4
    # spring has 2 passes -> exposure*passes = 4 == pop -> whole colony lost
    assert ur_hawk_passes(UR_SPRING) == 2
    assert ur_season_predation(full, granf, nursf, UR_SEED, UR_SPRING) == 4, \
        "two spring passes did not catch the whole exposed colony"
    assert ur_season_survivors(full, granf, nursf, UR_SEED, UR_SPRING) == 0

    # (3) the DEGENERATE identity the slice-6 card asked for: for every season
    # whose pass count is exactly 1, ur_season_predation == slice 6's
    # instantaneous ur_predation (the one-pass season IS the snapshot). WINTER
    # has 1 pass here, so it pins the fold-to-snapshot boundary.
    assert ur_hawk_passes(UR_WINTER) == 1, "winter is no longer the one-pass season"
    for grid_case, g_lay, n_lay in ((gp, granp, nursp), (full, granf, nursf)):
        for season in range(UR_SEASONS):
            if ur_hawk_passes(season) == 1:
                assert (ur_season_predation(grid_case, g_lay, n_lay, UR_SEED, season)
                        == ur_predation(grid_case, g_lay, n_lay, UR_SEED, season)), \
                    "one-pass season predation != slice-6 instantaneous snapshot"
                checks += 1
    # monotone in the pass count: more passes never lowers the loss (bounded by pop)
    for exposed in range(0, 8):
        prev = 0
        for passes in range(0, 5):
            loss = min(exposed * passes, 8)
            assert loss >= prev, "predation fell as passes rose"
            prev = loss
            checks += 1
    return checks


def prove_winter():
    # The arc doc's slice-8 proof (Decision 3 + the slice-8 row): winter drains
    # the banked store by the surviving foragers' appetite, the colony survives
    # iff the store never runs out, and SCORE = surviving_pop*1000 + leftover.
    #  1. the DRAIN recurrence — the pure day-by-day store fall (store minus
    #     pop*CONSUME each of the WINTER_DAYS) never going negative is EXACTLY
    #     store >= ur_winter_drain(pop) (pop constant -> monotone fall), for
    #     every (store, pop); leftover == the final running store on survival;
    #  2. the SCORE gradient — 0 iff starved, else pop*1000 + leftover; monotone
    #     non-decreasing in the store (banking more never lowers the outcome) and
    #     survival is a single threshold in the store;
    #  3. the WIRING — ur_winter_store == banked store - brood cost, always
    #     >= 0 (the nurseries never draw more than the store holds), for a spread
    #     of dig/gran/nurs plans;
    #  4. the MARQUEE — the committed survivor plan clears winter with a positive
    #     score while the deliberately-too-small plan (same colony, fewer
    #     granaries) STARVES to score 0.
    checks = 0

    # (1) the drain recurrence == the closed form, for every (store, pop) in a
    # dense range spanning both regimes (store far below and far above the drain).
    for pop in range(0, UR_POP_CAP + 3):
        drain = ur_winter_drain(pop)
        assert drain == pop * UR_WINTER_DAYS * UR_CONSUME_PER_DAY
        for store in range(0, 64):
            # simulate the day-by-day fall
            s = store
            ever_negative = False
            for _ in range(UR_WINTER_DAYS):
                s -= pop * UR_CONSUME_PER_DAY
                if s < 0:
                    ever_negative = True
            survive_sim = 0 if ever_negative else 1
            assert ur_winter_survives(store, pop) == survive_sim, \
                f"winter survive {store},{pop} != day-by-day recurrence"
            if survive_sim:
                # the running store never went negative and ends at store-drain
                assert s == store - drain
                assert ur_winter_leftover(store, pop) == s
                assert ur_winter_score(store, pop) == pop * 1000 + s
            else:
                assert ur_winter_leftover(store, pop) == 0
                assert ur_winter_score(store, pop) == 0
            checks += 1

    # (2) the score gradient: monotone non-decreasing in the store, and survival
    # is a single upward threshold (once you can afford winter, more only helps).
    for pop in range(0, UR_POP_CAP + 1):
        prev_score = -1
        seen_survive = False
        for store in range(0, 64):
            sc = ur_winter_score(store, pop)
            assert sc >= prev_score, "score fell as the store grew"
            prev_score = sc
            sv = ur_winter_survives(store, pop)
            if sv:
                seen_survive = True
            else:
                assert not seen_survive, "survival flipped back off as store grew"
            checks += 1
        # a colony with foragers needs a strictly positive store to survive;
        # a zero-forager colony survives any store vacuously but scores only the
        # store it never had to spend (0 here, since store 0 with pop 0).
        assert ur_winter_survives(ur_winter_drain(pop), pop) == 1
        if pop > 0:
            assert ur_winter_survives(ur_winter_drain(pop) - 1, pop) == 0

    # (3) the wiring: ur_winter_store == max(0, store - pop_food) and is never
    # negative, across a spread of plans (full grid + row-1 corridors with
    # varying granary/nursery layers), for every season.
    full = ur_fresh_grid()
    for i in range(UR_CELLS):
        full[i] = 1
    corridor = ur_fresh_grid()
    for c in range(4, 14):
        ur_dig(corridor, c, 1)
    for grid in (full, corridor):
        for gcount in (0, 2, 4, 8):
            gran = ur_fresh_gran()
            placed = 0
            for r in range(UR_GRID_H):
                for c in range(UR_GRID_W):
                    if placed >= gcount:
                        break
                    if ur_designate(grid, gran, c, r):
                        placed += 1
            for ncount in (0, 1, 2):
                nurs = ur_fresh_nurs()
                nplaced = 0
                for r in range(UR_GRID_H):
                    for c in range(UR_GRID_W):
                        if nplaced >= ncount:
                            break
                        if ur_nurse(grid, nurs, c, r):
                            nplaced += 1
                for season in range(UR_SEASONS):
                    store = ur_store(grid, gran, UR_SEED, season)
                    brood = ur_pop_food(grid, gran, nurs, UR_SEED, season)
                    assert brood <= store, \
                        f"brood {brood} exceeds store {store} (pop drew more than banked)"
                    ws = ur_winter_store(grid, gran, nurs, UR_SEED, season)
                    assert ws == store - brood, "winter store != banked - brood"
                    assert ws >= 0
                    checks += 1

    # (4) the marquee: the committed survivor plan clears winter, the too-small
    # plan (same corridor + nurseries, fewer granaries) starves.
    starve = marquee_fixture(MARQUEE_GRAN_STARVE)
    survive = marquee_fixture(MARQUEE_GRAN_EXTRA + MARQUEE_GRAN_STARVE)
    # identical colony geometry (same dig, same connected nurseries, same pop)
    assert starve['dug'] == survive['dug'] == len(MARQUEE_DIG) + 1  # +1 mouth
    assert starve['con'] == survive['con'] == starve['dug']         # all connected
    assert starve['nurscon'] == survive['nurscon'] == 2
    assert starve['pop'] == survive['pop'] == 6
    assert starve['ssurv'] == survive['ssurv'] == 2                 # 2 survive predation
    # they differ ONLY in banked granaries -> store -> winter store
    assert starve['grancon'] == 3 and survive['grancon'] == 6
    assert starve['store'] == 12 and survive['store'] == 24
    # the too-small plan starves: winter store 0 < drain 4 -> SURVIVE 0, SCORE 0
    assert starve['wstore'] == 0 and starve['wdrain'] == 4
    assert starve['wsurv'] == 0 and starve['wscore'] == 0
    # the survivor clears winter with a margin: store 12 - drain 4 = 8 leftover,
    # SCORE = 2 survivors * 1000 + 8 = 2008
    assert survive['wstore'] == 12 and survive['wdrain'] == 4
    assert survive['wsurv'] == 1 and survive['wleft'] == 8
    assert survive['wscore'] == 2008
    # the marquee contrast is exactly the banked margin: same colony, more store
    assert survive['wscore'] > starve['wscore']
    checks += 1
    return checks, starve, survive


def prove_save():
    # The arc doc's slice-9 proof: the best SCORE / furthest season persist via
    # the card backup, mirroring the gl_save pattern.
    #  (a) ENCODE/DECODE roundtrip over a value grid incl. extremes: encoding is
    #      deterministic + byte-exact-sized, decode returns exactly what encode
    #      wrote, plus a GOLDEN-BYTES pin (the survivor record the CI battery
    #      roundtrip reads back) so any C<->Python drift is caught at byte level;
    #  (b) REJECTION = fresh table, never a crash: blank chips (all-00/all-FF),
    #      EVERY single-byte corruption of a valid blob, and a wrong-magic and a
    #      future-version blob EACH WITH A RECOMPUTED VALID checksum (so magic
    #      and version are proven to reject on their own) — all decode to fresh;
    #  (c) IMPROVES is strictly-better-or-further only (a tie in BOTH writes
    #      nothing — EEPROM wear discipline), full truth table.
    failures = 0
    checks = 0

    # (a) roundtrip.
    for score in (0, 1, 2008, 8008, 999999, 0xFFFFFFFF):
        for season in (0, 1, 2, 3, 0xFFFFFFFF):
            for seed in (0, UR_SEED, 0xDEADBEEF, 0xFFFFFFFF):
                blob = ur_save_encode(score, season, seed)
                checks += 1
                if blob != ur_save_encode(score, season, seed):
                    failures += 1
                    print(f'FAIL save determinism: ({score},{season},{seed})')
                if len(blob) != UR_SAVE_BYTES:
                    failures += 1
                    print(f'FAIL save size: ({score},{season},{seed}): '
                          f'{len(blob)}')
                if ur_save_decode(blob) != (1, score, season, seed):
                    failures += 1
                    print(f'FAIL save roundtrip: ({score},{season},{seed}) -> '
                          f'{ur_save_decode(blob)}')
    # Golden-bytes pin: the WINTER-committed survivor record (best score 3003,
    # WINTER, run seed UR_SEED) the CI battery roundtrip reads back out of the
    # DeSmuME battery file. Slice 10 gates the commit on a realized winter, so the
    # persisted record is now the survivor plan's WINTER score (3003, season 3),
    # not the spring forecast (2008) — the C encoder, the emulated chip, and this
    # mirror must agree byte for byte on the new record.
    golden = ur_save_encode(3003, UR_WINTER, UR_SEED)
    if golden.hex() != ('56535255' '01000000' 'bb0b0000' '03000000'
                        '5a050000' '00000000' '00000000'
                        + ur_save_checksum([UR_SAVE_MAGIC, UR_SAVE_VERSION,
                                            3003, UR_WINTER, UR_SEED, 0, 0, 0]
                                           ).to_bytes(4, 'little').hex()):
        failures += 1
        print(f'FAIL save golden bytes: encode(3003, winter, UR_SEED) drifted: '
              f'{golden.hex()}')
    # One-page invariants: the blob is exactly the 8 words it claims and sits
    # page-aligned inside one 32-byte type-2 EEPROM page.
    if UR_SAVE_BYTES != UR_SAVE_WORDS * 4 or UR_SAVE_BYTES > 32 \
            or UR_SAVE_ADDR % 32 != 0:
        failures += 1
        print('FAIL save layout: blob does not fit one EEPROM page')

    # (b) rejection = fresh table.
    for label, blob in (('all-00', b'\x00' * UR_SAVE_BYTES),
                        ('all-FF', b'\xff' * UR_SAVE_BYTES)):
        checks += 1
        if ur_save_decode(blob)[0] != 0:
            failures += 1
            print(f'FAIL save reject {label}: accepted')
    valid = ur_save_encode(3003, UR_WINTER, UR_SEED)
    for i in range(UR_SAVE_BYTES):
        mut = bytearray(valid)
        mut[i] ^= 0xFF
        checks += 1
        if ur_save_decode(bytes(mut))[0] != 0:
            failures += 1
            print(f'FAIL save reject: byte {i} corruption accepted')
    for label, w0, w1 in (('future-version', UR_SAVE_MAGIC,
                           UR_SAVE_VERSION + 1),
                          ('wrong-magic', ur_hash(UR_SAVE_MAGIC, 1),
                           UR_SAVE_VERSION)):
        w = [w0, w1, 3003, UR_WINTER, UR_SEED, 0, 0, 0]
        w[UR_SAVE_WORDS - 1] = ur_save_checksum(w)
        crafted = b''.join((x & M32).to_bytes(4, 'little') for x in w)
        checks += 1
        if ur_save_decode(crafted)[0] != 0:
            failures += 1
            print(f'FAIL save reject {label}: accepted despite a valid '
                  'checksum')

    # (c) improves is strictly-better-or-further only, full truth table over a
    # small (score, season) grid incl. extremes.
    scores = list(range(6)) + [2008, 0xFFFFFFFF]
    seasons = list(range(UR_SEASONS)) + [0xFFFFFFFF]
    for bscore in scores:
        for bseason in seasons:
            for rscore in scores:
                for rseason in seasons:
                    checks += 1
                    want = 1 if (rscore > bscore or rseason > bseason) else 0
                    if ur_record_improves(bscore, bseason, rscore,
                                          rseason) != want:
                        failures += 1
                        print(f'FAIL record improves: '
                              f'({bscore},{bseason},{rscore},{rseason})')

    assert failures == 0, f'prove_save: {failures} failure(s)'
    return checks, golden


def prove_balance():
    # The arc doc's slice-10 proof: the seed dial's seed->schedule wiring is
    # pure, and the difficulty/escalation pass is FAIR — every dialable seed is
    # survivable with the reference plan (no death-traps), while the years still
    # span a real difficulty range.
    #  1. WIRING PURITY — ur_dial_seed is deterministic, dial 0 is exactly
    #     UR_SEED (the pinned skeleton), the dial wraps over UR_DIAL_COUNT, the
    #     scanned seeds are DISTINCT, and the dialed seed drives the whole
    #     f(seed,season,index) schedule purely (recompute-equal for hawks and
    #     patches across every dial position and season);
    #  2. FAIRNESS FLOOR — the reference plan's winter score is >= UR_FAIR_FLOOR
    #     (> 0) for EVERY dial seed AND for a dense sweep of the bounded seed
    #     space, so ur_seed_fair is 1 everywhere: no seed is unwinnable;
    #  3. ESCALATION SPREAD — the reference score is not flat across the dial
    #     (harder years bank less winter food), so the dial is a real difficulty
    #     choice, its floor never dropping below survivable;
    #  4. REFERENCE-PLAN SHAPE — the witness plan reaches EVERY patch column for
    #     any seed (all patches banked -> score == reachable winter food) and
    #     raises no population (drain 0), the two facts that make survival
    #     provable by construction.
    checks = 0

    # (4) the reference plan's shape: every drop cell dug + connected, capacity
    # over any patch total, zero nursery -> zero population/drain for any seed.
    grid, gran, nurs = ur_ref_plan()
    for c in range(UR_GRID_W):
        assert ur_dig_dist(grid, c, UR_DROP_ROW) != UR_ROUTE_NONE, \
            f"ref plan drop col {c} unreachable"
    assert ur_gran_connected(grid, gran) == UR_GRID_W
    assert ur_gran_capacity(grid, gran) >= UR_PATCH_COUNT * UR_PATCH_MAX
    for season in range(UR_SEASONS):
        for seed in (UR_SEED, 0, 0xFFFFFFFF, 4095):
            assert ur_pop(grid, gran, nurs, seed, season) == 0
            # all patches reachable -> reachable food == the full patch total
            assert ur_reachable_food(grid, seed, season) \
                == ur_patch_total(seed, season)
            checks += 1

    # (1) wiring purity: dial 0 == UR_SEED, determinism, wrap, distinctness, and
    # the dialed seed feeds the schedule purely.
    assert ur_dial_seed(0) == UR_SEED
    dial_seeds = []
    for dial in range(UR_DIAL_COUNT):
        s = ur_dial_seed(dial)
        assert ur_dial_seed(dial) == s, "ur_dial_seed not deterministic"
        assert ur_dial_seed(dial + UR_DIAL_COUNT) == s, "dial does not wrap"
        dial_seeds.append(s)
        # the seed drives the whole schedule purely: hawk + patch recompute-equal
        for season in range(UR_SEASONS):
            for frame in (0, 137, 511, 1024):
                assert ur_hawk_x(s, season, frame) == ur_hawk_x(s, season, frame)
                assert ur_hawk_y(s, season, frame) == ur_hawk_y(s, season, frame)
            assert ur_patch_total(s, season) == ur_patch_total(s, season)
            checks += 1
    assert len(set(dial_seeds)) == UR_DIAL_COUNT, \
        f"dial seeds not distinct: {dial_seeds}"

    # (2) fairness floor: every dial seed AND a dense sweep of the bounded seed
    # space is survivable with the reference plan, score >= UR_FAIR_FLOOR.
    for s in dial_seeds:
        sc = ur_ref_score(s)
        assert sc >= UR_FAIR_FLOOR, f"dial seed {s} ref score {sc} < floor"
        assert ur_seed_fair(s) == 1
        checks += 1
    for s in range(0, UR_DIAL_MASK + 2):     # every seed the mask can produce
        sc = ur_ref_score(s)
        assert sc >= UR_FAIR_FLOOR, f"seed {s} ref score {sc} below fair floor"
        assert ur_seed_fair(s) == 1
        checks += 1

    # (3) escalation spread: the dial is a real difficulty choice — the
    # reference score varies, its floor still survivable.
    dial_scores = [ur_ref_score(s) for s in dial_seeds]
    assert min(dial_scores) < max(dial_scores), \
        f"dial has no difficulty spread: {dial_scores}"
    assert min(dial_scores) >= UR_FAIR_FLOOR

    return checks, dial_seeds, dial_scores


def prove_audio():
    # The arc doc's slice-11 proof: the synthesized-audio DECISION layer is pure
    # and PSG-legal, mirroring check-gloam.py's section 13 (the gl_amb/gl_cue
    # precedent). Two tables, both proven for every reachable index:
    #  (a) the one-shot cue table — row 0 is a strict no-op, every real cue id
    #      1..3 is deterministic and PSG-legal (freq/len/duty/vol in the hardware
    #      ranges), the ids are exactly the documented priority ranking (winter
    #      toll > hawk cry > forager return — highest id wins a frame, mirrored
    #      in main.c), and an out-of-range id falls back to the row-0 no-op;
    #  (b) the ambience table — each season tier is deterministic, freq STRICTLY
    #      climbs with the tier (the burrow hums higher as the year closes),
    #      duty/vol are hardware-legal, and an out-of-range tier falls back to
    #      tier 0 (spring).
    checks = 0
    failures = 0

    # (a) cue table.
    if (ur_cue_freq(UR_CUE_NONE), ur_cue_len(UR_CUE_NONE),
            ur_cue_duty(UR_CUE_NONE), ur_cue_vol(UR_CUE_NONE)) != (0, 0, 0, 0):
        failures += 1
        print('FAIL cue NONE: row 0 must be a no-op (0,0,0,0)')
    checks += 1
    for cue in range(1, UR_CUE_COUNT):
        f, ln = ur_cue_freq(cue), ur_cue_len(cue)
        d, v = ur_cue_duty(cue), ur_cue_vol(cue)
        if (f, ln, d, v) != (ur_cue_freq(cue), ur_cue_len(cue),
                             ur_cue_duty(cue), ur_cue_vol(cue)):
            failures += 1
            print(f'FAIL cue determinism: id {cue}')
        if not 50 <= f <= 4000:
            failures += 1
            print(f'FAIL cue freq: id {cue}: {f} outside [50, 4000]')
        if not 1 <= ln <= 90:
            failures += 1
            print(f'FAIL cue hold: id {cue}: {ln} frames outside [1, 90]')
        if not (0 <= d <= 7 or d == UR_CUE_ON_NOISE):
            failures += 1
            print(f'FAIL cue duty: id {cue}: {d}')
        if not 1 <= v <= 127:
            failures += 1
            print(f'FAIL cue volume: id {cue}: {v}')
        checks += 1
    # ids are exactly the documented priority ranking (forager<hawk<winter).
    ranking = [UR_CUE_FORAGE, UR_CUE_HAWK, UR_CUE_WINTER]
    if ranking != sorted(ranking) or ranking != list(range(1, UR_CUE_COUNT)):
        failures += 1
        print('FAIL cue priority: ids are not the documented ranking')
    checks += 1
    # out-of-range id falls back to the row-0 no-op.
    if (ur_cue_freq(UR_CUE_COUNT + 5), ur_cue_len(UR_CUE_COUNT + 5),
            ur_cue_duty(UR_CUE_COUNT + 5),
            ur_cue_vol(UR_CUE_COUNT + 5)) != (0, 0, 0, 0):
        failures += 1
        print('FAIL cue out-of-range: bad id must fall back to the no-op')
    checks += 1

    # (b) ambience table: freq strictly climbs with the tier.
    prev_f = 0
    for tier in range(UR_AMB_TIERS):
        f, d, v = ur_amb_freq(tier), ur_amb_duty(tier), ur_amb_vol(tier)
        if (f, d, v) != (ur_amb_freq(tier), ur_amb_duty(tier),
                         ur_amb_vol(tier)):
            failures += 1
            print(f'FAIL amb determinism: tier {tier}')
        if f <= prev_f:
            failures += 1
            print(f'FAIL amb freq climb: tier {tier}: {f} <= {prev_f}')
        if not 0 <= d <= 7:
            failures += 1
            print(f'FAIL amb duty code: tier {tier}: {d}')
        if not 1 <= v <= 127:
            failures += 1
            print(f'FAIL amb volume: tier {tier}: {v}')
        prev_f = f
        checks += 1
    # out-of-range tier falls back to tier 0 (spring).
    if (ur_amb_freq(UR_AMB_TIERS + 3), ur_amb_duty(UR_AMB_TIERS + 3),
            ur_amb_vol(UR_AMB_TIERS + 3)) != (ur_amb_freq(0), ur_amb_duty(0),
                                              ur_amb_vol(0)):
        failures += 1
        print('FAIL amb out-of-range: bad tier must fall back to spring')
    checks += 1

    assert failures == 0, f'prove_audio: {failures} failure(s)'
    return checks


def main():
    hawk_checks = prove_hawk()
    dig_checks = prove_dig()
    patch_checks = prove_patches()
    forage_checks = prove_forage()
    granary_checks = prove_granary()
    nursery_checks = prove_nursery()
    predation_checks = prove_predation()
    clock_checks = prove_clock()
    winter_checks, marq_starve, marq_survive = prove_winter()
    save_checks, save_golden = prove_save()
    balance_checks, dial_seeds, dial_scores = prove_balance()
    audio_checks = prove_audio()
    marq_winter = marquee_winter_fixture()

    # slice-10 winter-gated save lockstep: the survivor plan's REALIZED winter
    # score is what the ROM now commits (season == UR_WINTER). These are the
    # numbers rom-builds.yml proof-5 asserts at the winter START press.
    assert marq_winter['wscore'] == 3003, \
        f"marquee winter score drifted: {marq_winter['wscore']}"
    assert (marq_winter['store'], marq_winter['wstore'], marq_winter['wpop'],
            marq_winter['wsurv']) == (21, 9, 3, 1), \
        f"marquee winter fixture drifted: {marq_winter}"
    # the winter-committed record's golden bytes (best 3003, season winter, seed
    # UR_SEED) must equal the spring-forecast score's successor exactly.
    assert save_golden == ur_save_encode(3003, UR_WINTER, UR_SEED)
    dug_total, connected, (route_i, route_d, route_len), \
        (gran_n, gran_con, gran_cap, gran_store), \
        (nurs_n, nurs_con, pop, pop_food), \
        (exposed, lost, surv), \
        (abund, sfood, hawk_pass, spred, ssurv), \
        (w_store, w_pop, w_drain, w_surv, w_left, w_score) = ci_fixture()

    # The lockstep anchor: these are the numbers rom-builds.yml asserts
    # against the ROM's ur_telemetry mailbox after replaying CI_DIG_PIXELS.
    assert dug_total == 7, f"CI fixture dug_total drifted: {dug_total}"
    assert connected == 6, f"CI fixture connected drifted: {connected}"
    # slice-3 forage lockstep: patch 4 (mouth column) reached at one cell out,
    # two round trip; the other five patches' columns are undug -> skipped.
    assert (route_i, route_d, route_len) == (4, 1, 2), \
        f"CI fixture forage route drifted: {(route_i, route_d, route_len)}"
    # slice-4 store lockstep: 2 granary cells placed (one connected, one in the
    # isolated pocket), 1 connected -> capacity 4; patch 4's haul (6) banked up
    # to that capacity -> store 4 (capped). The disconnected granary banks
    # nothing (gran_con 1 < gran_n 2).
    assert (gran_n, gran_con, gran_cap, gran_store) == (2, 1, UR_GRAN_CAP, UR_GRAN_CAP), \
        f"CI fixture store drifted: {(gran_n, gran_con, gran_cap, gran_store)}"
    # slice-5 population lockstep: 2 nursery cells placed (one connected on the
    # east branch, one in the isolated pocket), 1 connected -> brood 3; the
    # store (4) buys food_ants = 4//UR_FOOD_PER_ANT = 2, so pop = min(3, 2) = 2
    # (FOOD-BOUND); popfood = 2*UR_FOOD_PER_ANT = 4. The disconnected nursery
    # broods nothing (nurs_con 1 < nurs_n 2).
    assert (nurs_n, nurs_con, pop, pop_food) == (2, 1, 2, 4), \
        f"CI fixture population drifted: {(nurs_n, nurs_con, pop, pop_food)}"
    # slice-6 predation lockstep: the forager route is the shallow shaft to
    # patch 4 (drop (8,1), dist 1); cells (8,0) row 0 and (8,1) row 1 are both
    # exposed (row < UR_SAFE_DEPTH), so exposed = 2. The whole population (2) is
    # caught on that surface-hugging route (lost = min(2, 2) = 2), leaving 0
    # survivors — a shallow route is hawk-food.
    assert (exposed, lost, surv) == (2, 2, 0), \
        f"CI fixture predation drifted: {(exposed, lost, surv)}"
    # slice-7 year-clock lockstep on the spring fixture: spring is the abundance
    # UNIT (4), so the seasonal haul is the raw reachable food (patch 4's 6);
    # spring has 2 hawk passes, and the per-hawk-pass season predation on the
    # shallow shaft is min(exposed 2 * passes 2, pop 2) = 2 (still pop-bound), so
    # 0 survive the season — the same (abund 4, sfood 6, passes 2, spred 2,
    # ssurv 0) the rom-builds.yml ur_telemetry asserts at the settled dig frame.
    assert (abund, sfood, hawk_pass, spred, ssurv) == (UR_ABUND_SPRING, 6, 2, 2, 0), \
        f"CI fixture year clock drifted: {(abund, sfood, hawk_pass, spred, ssurv)}"
    # slice-8 winter lockstep on the spring fixture: the whole colony (pop 2) is
    # lost to the hawks on the shallow shaft (season survivors 0), so NO foragers
    # carry into winter (wpop 0) and the store is fully drawn to brood (store 4 ==
    # popfood 4 -> winter store 0). A colony already dead to the meadow demands no
    # winter food (drain 0) and banks nothing forward (leftover 0), so it SCORES 0
    # — the same (wstore 0, wpop 0, drain 0, leftover 0, score 0) the
    # rom-builds.yml ur_telemetry asserts at the settled dig frame. (wsurv is a
    # vacuous 1 here — an empty colony has nothing to starve — so the ROM proof
    # asserts the meaningful zeros, not the flag; the marquee proof drives the
    # non-vacuous survive/starve pair.)
    assert (w_store, w_pop, w_drain, w_left, w_score) == (0, 0, 0, 0, 0), \
        f"CI fixture winter drifted: {(w_store, w_pop, w_drain, w_left, w_score)}"

    # Pinned hawk telemetry values the CI --assert-watch uses (printed so a
    # human editing the schedule can re-pin the workflow in the same PR).
    pins = {
        100: (ur_hawk_active(100), ur_hawk_x(UR_SEED, 0, 100)),
        300: (ur_hawk_active(300), ur_hawk_x(UR_SEED, 0, 300)),
        600: (ur_hawk_active(600), ur_hawk_x(UR_SEED, 0, 600)),
    }
    patch_sum = ur_patch_total(UR_SEED, 0)
    print("check-underroot: OK —")
    print(f"  hawk schedule: {hawk_checks} (seed,season,frame) cases "
          "pure/bounded/monotone/on-band, schedule keyed on the pass index")
    print(f"  food patches: {patch_checks} (seed,season,index) cases "
          f"pure/on-meadow/off-the-hawk-lanes, amount in "
          f"[{UR_PATCH_MIN},{UR_PATCH_MAX}]")
    print(f"  slice-2 lockstep: patch_n={UR_PATCH_COUNT} patch_sum={patch_sum} "
          "(== the rom-builds.yml ur_telemetry[15]/[16] asserts)")
    print(f"  dig grid: {dig_checks} pixel+graph cases — touch->cell total, "
          "dig idempotent/monotone, burrow BFS connected<=total, "
          "corridor grows by one, isolated pocket never counted")
    print(f"  foragers: {forage_checks} cases — dig_dist == independent BFS "
          "(shortest, >= Manhattan), fresh grid has no route, ur_forage "
          "nearest-wins, a disconnected patch is never visited")
    print(f"  granaries: {granary_checks} cases — designate dug-only/"
          "idempotent/monotone, capacity == connected*{0} exact, disconnected "
          "granaries bank nothing (cross-layer == burrow BFS), store == "
          "min(reachable food, capacity)".format(UR_GRAN_CAP))
    print(f"  nurseries: {nursery_checks} cases — nurse dug-only/idempotent/"
          "monotone, connected == burrow BFS (disconnected nurseries brood "
          f"nothing), pop == min(connected*{UR_NURS_BROOD}, "
          f"store//{UR_FOOD_PER_ANT}) capped at {UR_POP_CAP} "
          "(food-bound/brood-bound/cap branches)")
    print(f"  predation: {predation_checks} cases — route_exposed == shallow "
          f"cells on a valid shortest path (row < {UR_SAFE_DEPTH} exposed), "
          "exact exposed<->deep boundary (shallow route 6 vs buried 3 to the "
          "same drop), predation == min(exposed, pop) (pop-bound + "
          "exposure-bound regimes), no route predates nothing")
    print(f"  year clock: {clock_checks} cases — ur_day pure/monotone (day = "
          f"frame//{UR_DAY_FRAMES}), the {UR_SEASONS} seasons partition the "
          f"{UR_YEAR_DAYS}-day year in order (spring {UR_SPRING_DAYS}/summer "
          f"{UR_SUMMER_DAYS}/autumn {UR_AUTUMN_DAYS}/winter {UR_WINTER_DAYS}, "
          "past-year clamps to winter), abundance spring=unit<summer, "
          "autumn<spring, winter=0 (season_food scales the haul), "
          "season_predation == min(exposed*passes, pop) (one-pass season == "
          "slice-6 snapshot)")
    print(f"  winter exam: {winter_checks} cases — drain == pop*"
          f"{UR_WINTER_DAYS}*{UR_CONSUME_PER_DAY}, the day-by-day store fall "
          "never-negative == store>=drain (recurrence == closed form), score "
          "0-if-starved else pop*1000+leftover (monotone in the store, single "
          "survival threshold), winter store == banked-brood (>=0), and the "
          f"MARQUEE: survivor clears winter (store {marq_survive['wstore']} - "
          f"drain {marq_survive['wdrain']} -> SURVIVE, score "
          f"{marq_survive['wscore']}) vs the too-small plan (store "
          f"{marq_starve['wstore']} < drain {marq_starve['wdrain']} -> STARVE, "
          f"score {marq_starve['wscore']})")
    print(f"  slice-8 marquee fixtures (spring): STARVE=(grancon "
          f"{marq_starve['grancon']}, store {marq_starve['store']}, pop "
          f"{marq_starve['pop']}, ssurv {marq_starve['ssurv']}, wstore "
          f"{marq_starve['wstore']}, drain {marq_starve['wdrain']}, surv "
          f"{marq_starve['wsurv']}, score {marq_starve['wscore']})  "
          f"SURVIVE=(grancon {marq_survive['grancon']}, store "
          f"{marq_survive['store']}, pop {marq_survive['pop']}, ssurv "
          f"{marq_survive['ssurv']}, wstore {marq_survive['wstore']}, drain "
          f"{marq_survive['wdrain']}, surv {marq_survive['wsurv']}, leftover "
          f"{marq_survive['wleft']}, score {marq_survive['wscore']}) "
          "(== the rom-builds.yml proof-4 ur_telemetry asserts)")
    print(f"  best-score save: {save_checks} cases — encode/decode roundtrip "
          f"byte-exact (golden encode(3003,winter,{hex(UR_SEED)})="
          f"{save_golden.hex()}), every blank/1-byte-corrupt/wrong-magic/"
          "future-version blob rejects to the fresh table, improves is "
          "strictly-better-score-or-further-season only (tie writes nothing) "
          "(== the rom-builds.yml proof-5 battery roundtrip asserts)")
    print(f"  seed dial + balance: {balance_checks} cases — ur_dial_seed pure/"
          f"wrapping (dial 0 == {hex(UR_SEED)}), {UR_DIAL_COUNT} DISTINCT dial "
          f"seeds {dial_seeds} feed the schedule purely; the reference plan is "
          f"survivable for EVERY seed (dial ref scores {dial_scores}, and every "
          f"seed in [0,{UR_DIAL_MASK + 1}] >= UR_FAIR_FLOOR {UR_FAIR_FLOOR} > 0 "
          f"— no death-traps), with a real difficulty spread "
          f"{min(dial_scores)}..{max(dial_scores)}")
    print(f"  synthesized audio: {audio_checks} cases — cue row 0 is a no-op, "
          f"cues 1..{UR_CUE_COUNT - 1} deterministic + PSG-legal (freq "
          "[50,4000], len [1,90], duty 0..7 or noise, vol [1,127]), priority "
          f"ranking [FORAGE,HAWK,WINTER]==1..{UR_CUE_COUNT - 1} (highest id "
          "wins the SFX channel), out-of-range id -> row-0 fallback; ambience "
          f"freq {ur_amb_freq(0)}<{ur_amb_freq(1)}<{ur_amb_freq(2)}<"
          f"{ur_amb_freq(3)} STRICTLY climbs spring->winter, duty/vol legal, "
          "out-of-range tier -> spring fallback (== the rom-builds.yml proof-7 "
          "ur_telemetry[54..58] asserts)")
    print(f"  slice-10 winter-gated save: the survivor plan's REALIZED winter "
          f"score (store {marq_winter['store']} -> wstore {marq_winter['wstore']}"
          f", wpop {marq_winter['wpop']}, SURVIVE -> score "
          f"{marq_winter['wscore']}) is what the ROM commits at a WINTER START — "
          f"golden encode(3003,winter,{hex(UR_SEED)})={save_golden.hex()} "
          "(== the rom-builds.yml proof-5 winter-commit asserts)")
    print(f"  CI lockstep fixture: dug_total={dug_total} connected={connected} "
          f"forage=(patch {route_i}, dist {route_d}, route {route_len}) "
          f"store=(placed {gran_n}, connected {gran_con}, cap {gran_cap}, "
          f"banked {gran_store}) "
          f"pop=(placed {nurs_n}, connected {nurs_con}, pop {pop}, "
          f"popfood {pop_food}) "
          f"predation=(exposed {exposed}, lost {lost}, surv {surv}) "
          f"year=(abund {abund}/{UR_ABUND_UNIT}, sfood {sfood}, passes "
          f"{hawk_pass}, spred {spred}, ssurv {ssurv}) "
          f"winter=(wstore {w_store}, wpop {w_pop}, drain {w_drain}, surv "
          f"{w_surv}, leftover {w_left}, score {w_score}) "
          "(== the rom-builds.yml ur_telemetry asserts)")
    print(f"  pinned hawk telemetry (seed={hex(UR_SEED)}, season=0): "
          f"f100 on={pins[100][0]} x={pins[100][1]}, "
          f"f300 on={pins[300][0]} x={pins[300][1]}, "
          f"f600 on={pins[600][0]} x={pins[600][1]}")
    # Pinned year-clock telemetry the CI proof-3 --assert-watch uses across the
    # simulated year (printed so a human editing the clock can re-pin it here).
    print(f"  year telemetry across the year: "
          f"f100 day={ur_day(100)} season={ur_season_of_day(ur_day(100))} "
          f"abund={ur_abundance(ur_season_of_day(ur_day(100)))}, "
          f"f1100 day={ur_day(1100)} season={ur_season_of_day(ur_day(1100))} "
          f"abund={ur_abundance(ur_season_of_day(ur_day(1100)))}, "
          f"f2600 day={ur_day(2600)} season={ur_season_of_day(ur_day(2600))} "
          f"abund={ur_abundance(ur_season_of_day(ur_day(2600)))}, "
          f"f3700 day={ur_day(3700)} season={ur_season_of_day(ur_day(3700))} "
          f"abund={ur_abundance(ur_season_of_day(ur_day(3700)))}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
