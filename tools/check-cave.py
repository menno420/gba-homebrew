#!/usr/bin/env python3
"""Lumen Drift cave passability proof (host-side, stdlib-only, <1s).

Review-queue row #23 found 6 echo-band junctions (rows 191-1093) where the
tunnel stayed SOLID-passable but every shared open column between the two
rows carried a crystal in one of them — an unavoidable death gate in the
endless echoes that no pinned replay reaches (both CI replay tiers die far
above row 191). This tool pins the full passability contract so any future
`layout_of_row` / `row_solid` / `row_features` edit that reintroduces the
class of defect fails CI in milliseconds instead of surfacing as an
unwinnable run.

What it checks, for EVERY consecutive world-row pair:

  1. solidity continuity — at least one column open in both rows;
  2. no crystal gate — at least one shared open column that is crystal-free
     in BOTH rows (the descending 6px mote needs one clean column to cross
     the junction);
  3. periodicity self-check — past the v1.1 difficulty plateau (row 244,
     where the depth tier stops ramping) the pure generation only reads cy
     through residues whose lcm divides 10080 (waveforms 32/24, band look
     60, blobs 18, pillar 6, plateau crystals 8/6, shards 12), so rows
     244..244+10080 repeat forever. The tool VERIFIES that empirically
     (row 244+k must equal row 244+10080+k cell-for-cell across one full
     period) rather than trusting the arithmetic, then the pairwise scan
     over rows 0..244+2*10080 covers every row the cave will ever generate
     (the non-periodic tier-ramp rows 124..243 are inside the scan);
  4. difficulty-curve tripwire (v1.1 "the echoes deepen") — crystal cells
     per 60-row echo cycle must strictly increase from tier 0 through the
     plateau tier, so a regression back to a flat endless cave fails CI.

The row functions below are a line-for-line mirror of the pure generator in
games/lumen-drift/src/main.cpp (layout_of_row / row_solid / row_features,
including the row #23 junction guard). KEEP THEM IN LOCKSTEP: any change to
the C++ must land here in the same PR. Drift in rows the replays reach is
caught by the deep-run replay's cell-exact depth asserts; drift beyond that
would silently weaken THIS proof, which is why the mirror rule is stated in
both files.

Usage: python3 tools/check-cave.py            # full proof (exit 0 = green)
       python3 tools/check-cave.py --verbose  # per-junction detail on failure
"""

import sys

MAP_COLS = 32
GALLERY_FIRST_ROW = 24
DEEP_FIRST_ROW = 44
ECHO_FIRST_ROW = 64
ECHO_BAND_ROWS = 20
ECHO_PERIOD = 10080  # lcm of every cy-residue the plateau rows read — verified below

# v1.1 "the echoes deepen": every 60-row echo cycle past the first raises a
# depth tier (denser crystals, tighter gallery echoes), capped at tier 3.
# The ramp rows (124..243) are NOT periodic — the periodicity self-check
# therefore anchors at PLATEAU_ROW (244), the first row of the final tier,
# and the pairwise scan covers rows 0..PLATEAU_ROW + 2 periods explicitly.
ECHO_CYCLE_BANDS = 3
ECHO_TIER_CAP = 3
PLATEAU_ROW = ECHO_FIRST_ROW + ECHO_TIER_CAP * ECHO_CYCLE_BANDS * ECHO_BAND_ROWS  # 244

WAVE_A = [0, 2, 4, 5, 6, 5, 4, 2, 0, -2, -4, -5, -6, -5, -4, -2]
WAVE_B = [0, 1, 2, 1, 0, -1, -2, -1]


def _clamp(value, low, high):
    return low if value < low else high if value > high else value


def section_index_of_row(cy):
    if cy < GALLERY_FIRST_ROW:
        return 0
    if cy < DEEP_FIRST_ROW:
        return 1
    if cy < ECHO_FIRST_ROW:
        return 2
    return 3 + (cy - ECHO_FIRST_ROW) // ECHO_BAND_ROWS


def section_look(index):
    return index if index < 3 else (index - 3) % 3


def echo_tier(index):
    """Mirror of echo_tier(): depth tier of a section index (v1.1)."""
    return 0 if index < 3 else min((index - 3) // ECHO_CYCLE_BANDS,
                                   ECHO_TIER_CAP)


def layout_of_row(cy):
    """Mirror of layout_of_row(): (center, half_width, look, blobs, pillar,
    crystal_step)."""
    index = section_index_of_row(cy)
    look = section_look(index)

    if index == 0:
        center = 16 + WAVE_A[(cy // 2) % 16] + WAVE_B[(cy // 3) % 8]
        half_width = _clamp(5 - cy // 20, 2, 5)
    elif index == 1:
        center = 16 + 2 * WAVE_B[(cy // 4) % 8]
        half_width = 6
    elif index == 2:
        center = 16 + WAVE_A[(cy + 8) % 16] + WAVE_B[(cy // 2) % 8]
        half_width = 3 if cy < 54 else 2
    else:
        center = 16 + WAVE_A[(cy // 2) % 16] + WAVE_B[(cy // 3) % 8]
        half_width = ((3 if echo_tier(index) >= 2 else 4) if look == 1
                      else 2)

    center = _clamp(center, 4, MAP_COLS - 5)
    blobs = look == 0 and cy > 10 and cy % 9 == 4
    pillar = look == 1 and cy >= 26 and cy % 6 == 1
    crystal_step = max((5 if look == 2 else 7) - echo_tier(index), 3)
    return center, half_width, look, blobs, pillar, crystal_step


def row_solid(cy):
    """Mirror of row_solid(): True = rock."""
    out = [True] * MAP_COLS

    if cy <= 0:
        return out

    if cy <= 6:
        for cx in range(10, 23):
            out[cx] = False

    if cy >= 5:
        center, half_width, _, blobs, pillar, _ = layout_of_row(cy)

        for cx in range(center - half_width, center + half_width + 1):
            if 0 < cx < MAP_COLS - 1:
                out[cx] = False

        if blobs:
            left = (cy // 9) % 2 == 0
            for i in range(2):
                cx = center - half_width + i if left else center + half_width - i
                if 0 < cx < MAP_COLS - 1:
                    out[cx] = True

        if pillar:
            for cx in range(center - 1, center + 2):
                if 0 < cx < MAP_COLS - 1:
                    out[cx] = True

    if cy == 10:
        for cx in range(15, 18):
            out[cx] = True

    return out


def row_features(cy, solid):
    """Mirror of row_features() INCLUDING the row #23 junction guard:
    (crystal, shard) bool lists."""
    crystal = [False] * MAP_COLS
    shard = [False] * MAP_COLS

    if cy < 5:
        return crystal, shard

    center, half_width, _, _, _, crystal_step = layout_of_row(cy)

    if cy > 12 and cy % crystal_step == 2:
        above = row_solid(cy - 1)
        below = row_solid(cy + 1)

        def junction_open(neighbour):
            return any(not solid[jx] and not neighbour[jx] and not crystal[jx]
                       for jx in range(1, MAP_COLS - 1))

        left = (cy // crystal_step) % 2 == 0

        for i in range(2):
            cx = center - half_width + i if left else center + half_width - i

            if 0 < cx < MAP_COLS - 1 and not solid[cx]:
                crystal[cx] = True

                if not junction_open(above) or not junction_open(below):
                    crystal[cx] = False  # spare the junction column

    if cy > 6 and cy % 6 == 3:
        left = (cy // 6) % 2 == 0

        for i in range(3):
            cx = center - half_width + i if left else center + half_width - i

            if 0 < cx < MAP_COLS - 1 and not solid[cx] and not crystal[cx]:
                shard[cx] = True
                break

    return crystal, shard


def row_snapshot(cy):
    solid = row_solid(cy)
    crystal, shard = row_features(cy, solid)
    return solid, crystal, shard


def main():
    verbose = "--verbose" in sys.argv[1:]
    failures = 0

    # 3. Periodicity self-check FIRST (it justifies the scan bound): every
    # row in one full echo period must equal its twin one period later.
    # v1.1: anchored at PLATEAU_ROW — the tier ramp (rows 124..243) is not
    # periodic and is covered explicitly by the pairwise scan below.
    for k in range(ECHO_PERIOD + 1):
        a = row_snapshot(PLATEAU_ROW + k)
        b = row_snapshot(PLATEAU_ROW + ECHO_PERIOD + k)
        if a != b:
            failures += 1
            print(f"FAIL periodicity: row {PLATEAU_ROW + k} != "
                  f"row {PLATEAU_ROW + ECHO_PERIOD + k}")
            if not verbose:
                break

    # 4. v1.1 difficulty-curve tripwire: the depth tiers must actually ramp
    # — crystal cells per 60-row echo cycle strictly increase from tier 0
    # through the plateau tier (a silent regression to a flat cave fails
    # here, not in a playtest).
    cycle_rows = ECHO_CYCLE_BANDS * ECHO_BAND_ROWS
    counts = []
    for cycle in range(ECHO_TIER_CAP + 1):
        first = ECHO_FIRST_ROW + cycle * cycle_rows
        counts.append(sum(sum(row_snapshot(cy)[1])
                          for cy in range(first, first + cycle_rows)))
    if not all(a < b for a, b in zip(counts, counts[1:])):
        failures += 1
        print(f"FAIL difficulty curve: crystal cells per echo cycle must "
              f"strictly increase across tiers 0..{ECHO_TIER_CAP}, got "
              f"{counts}")

    # 1 + 2. Pairwise junction scan over the committed window, the tier
    # ramp and two full echo periods — with periodicity proven above, this
    # covers every consecutive row pair the endless cave will ever produce.
    top = PLATEAU_ROW + 2 * ECHO_PERIOD
    gates = []
    breaks = []

    for cy in range(0, top + 1):
        solid_a, crystal_a, _ = row_snapshot(cy)
        solid_b, crystal_b, _ = row_snapshot(cy + 1)
        shared = [cx for cx in range(MAP_COLS)
                  if not solid_a[cx] and not solid_b[cx]]

        if not shared:
            if cy > 0:  # row 0 is the intentionally solid world border
                breaks.append(cy)
            continue

        if not any(not crystal_a[cx] and not crystal_b[cx] for cx in shared):
            gates.append((cy, shared,
                          [cx for cx in shared if crystal_a[cx]],
                          [cx for cx in shared if crystal_b[cx]]))

    for cy in breaks:
        failures += 1
        print(f"FAIL solidity: junction {cy}/{cy + 1} shares no open column")

    for cy, shared, in_a, in_b in gates:
        failures += 1
        print(f"FAIL crystal gate: junction {cy}/{cy + 1} — shared open "
              f"columns {shared} all crystal'd (row {cy}: {in_a}, "
              f"row {cy + 1}: {in_b})")

    if failures:
        print(f"check-cave: {failures} failure(s) over rows 0..{top + 1}")
        return 1

    print(f"check-cave OK: rows 0..{top + 1} scanned (echo periodicity "
          f"verified over {ECHO_PERIOD} rows from plateau row {PLATEAU_ROW})"
          f" — 0 solidity breaks, 0 crystal gates; difficulty curve ramps: "
          f"crystal cells/cycle {counts} across tiers 0..{ECHO_TIER_CAP}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
