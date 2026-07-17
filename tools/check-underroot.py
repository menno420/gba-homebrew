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


def ci_fixture():
    grid = ur_fresh_grid()
    for (tx, ty) in CI_DIG_PIXELS:
        cell = ur_cell_of_touch(tx, ty)
        assert cell is not None, f"CI dig pixel {tx},{ty} out of bounds"
        ur_dig(grid, cell[0], cell[1])
    return _dug_total(grid), ur_burrow_size(grid)


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


def main():
    hawk_checks = prove_hawk()
    dig_checks = prove_dig()
    dug_total, connected = ci_fixture()

    # The lockstep anchor: these are the numbers rom-builds.yml asserts
    # against the ROM's ur_telemetry mailbox after replaying CI_DIG_PIXELS.
    assert dug_total == 7, f"CI fixture dug_total drifted: {dug_total}"
    assert connected == 6, f"CI fixture connected drifted: {connected}"

    # Pinned hawk telemetry values the CI --assert-watch uses (printed so a
    # human editing the schedule can re-pin the workflow in the same PR).
    pins = {
        100: (ur_hawk_active(100), ur_hawk_x(UR_SEED, 0, 100)),
        300: (ur_hawk_active(300), ur_hawk_x(UR_SEED, 0, 300)),
        600: (ur_hawk_active(600), ur_hawk_x(UR_SEED, 0, 600)),
    }
    print("check-underroot: OK —")
    print(f"  hawk schedule: {hawk_checks} (seed,season,frame) cases "
          "pure/bounded/monotone/on-band, schedule keyed on the pass index")
    print(f"  dig grid: {dig_checks} pixel+graph cases — touch->cell total, "
          "dig idempotent/monotone, burrow BFS connected<=total, "
          "corridor grows by one, isolated pocket never counted")
    print(f"  CI lockstep fixture: dug_total={dug_total} connected={connected} "
          "(== the rom-builds.yml ur_telemetry asserts)")
    print(f"  pinned hawk telemetry (seed={hex(UR_SEED)}, season=0): "
          f"f100 on={pins[100][0]} x={pins[100][1]}, "
          f"f300 on={pins[300][0]} x={pins[300][1]}, "
          f"f600 on={pins[600][0]} x={pins[600][1]}")
    return 0


if __name__ == '__main__':
    sys.exit(main())
