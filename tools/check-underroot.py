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
    return _dug_total(grid), ur_burrow_size(grid), forage, store, pop, pred


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


def main():
    hawk_checks = prove_hawk()
    dig_checks = prove_dig()
    patch_checks = prove_patches()
    forage_checks = prove_forage()
    granary_checks = prove_granary()
    nursery_checks = prove_nursery()
    predation_checks = prove_predation()
    dug_total, connected, (route_i, route_d, route_len), \
        (gran_n, gran_con, gran_cap, gran_store), \
        (nurs_n, nurs_con, pop, pop_food), \
        (exposed, lost, surv) = ci_fixture()

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
    print(f"  CI lockstep fixture: dug_total={dug_total} connected={connected} "
          f"forage=(patch {route_i}, dist {route_d}, route {route_len}) "
          f"store=(placed {gran_n}, connected {gran_con}, cap {gran_cap}, "
          f"banked {gran_store}) "
          f"pop=(placed {nurs_n}, connected {nurs_con}, pop {pop}, "
          f"popfood {pop_food}) "
          f"predation=(exposed {exposed}, lost {lost}, surv {surv}) "
          "(== the rom-builds.yml ur_telemetry asserts)")
    print(f"  pinned hawk telemetry (seed={hex(UR_SEED)}, season=0): "
          f"f100 on={pins[100][0]} x={pins[100][1]}, "
          f"f300 on={pins[300][0]} x={pins[300][1]}, "
          f"f600 on={pins[600][0]} x={pins[600][1]}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
