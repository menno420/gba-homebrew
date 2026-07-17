#!/usr/bin/env python3
"""Generate Wickroad's original indexed-BMP assets (deterministic).

Butano's asset pipeline (grit) consumes indexed BMPs + a JSON descriptor per
asset. These are ORIGINAL, procedurally-drawn graphics — no third-party art.
Committed BMPs are the build inputs; re-running this script reproduces them
byte-for-byte (stdlib only, no Pillow, no timestamps, no randomness beyond a
fixed-seed LCG). Same convention as Cindervault's
games/cindervault/graphics/generate_assets.py and Lumen Drift's.

    python3 games/wickroad/graphics/generate_assets.py

Outputs (next to this script):
    wr_tiles.bmp    4bpp 8x8 BG tiles (crossroads cut 2, the art pass):
                    0 transparent (backdrop) · 1 sky/parchment ·
                    2 road cobble · 3 road edge · 4 town wall ·
                    5 town roof · 6 lit window · 7 price bar filled ·
                    8 price bar empty · 9 cursor chevron ·
                    10 fork signpost · 11 cargo pip
    wr_palette.bmp  8bpp palette carrier (16 BG colors)

IMPORTANT palette rule (the reason the art is provable AND invisible to the
text matcher): the headless harness's --assert-text matcher treats exact
black (0,0,0) and exact white (255,255,255) as text-ink colors, so no tile
color may be either — the warm parchment/ember/steel family below stays
clear of both by construction. Index 0 is transparent (never rendered).
"""

import os
import struct

OUT_DIR = os.path.dirname(os.path.abspath(__file__))


def write_bmp(path, width, height, bpp, palette, pixels):
    """Write an indexed BMP (bpp 4 or 8). pixels: row-major top-down indices."""
    assert bpp in (4, 8)
    colors = 16 if bpp == 4 else 256
    pal = list(palette) + [(0, 0, 0)] * (colors - len(palette))
    if bpp == 4:
        row_bytes = (width + 1) // 2
    else:
        row_bytes = width
    stride = (row_bytes + 3) & ~3
    image_size = stride * height
    data_offset = 14 + 40 + colors * 4
    with open(path, 'wb') as fh:
        fh.write(struct.pack('<2sIHHI', b'BM', data_offset + image_size, 0, 0,
                             data_offset))
        fh.write(struct.pack('<IiiHHIIiiII', 40, width, height, 1, bpp, 0,
                             image_size, 2835, 2835, colors, 0))
        for r, g, b in pal:
            fh.write(struct.pack('<BBBB', b, g, r, 0))
        for y in range(height - 1, -1, -1):  # BMP rows are bottom-up
            row = pixels[y * width:(y + 1) * width]
            if bpp == 4:
                packed = bytearray()
                for i in range(0, width, 2):
                    hi = row[i] & 0xF
                    lo = row[i + 1] & 0xF if i + 1 < width else 0
                    packed.append((hi << 4) | lo)
                line = bytes(packed)
            else:
                line = bytes(row)
            fh.write(line + b'\x00' * (stride - len(line)))


def lcg(seed):
    """Tiny deterministic PRNG (same constants as glibc's rand)."""
    state = seed

    def _next(mod):
        nonlocal state
        state = (state * 1103515245 + 12345) & 0x7FFFFFFF
        return state % mod

    return _next


# --- BG palette (16 colors; index 0 = transparent) -------------------------
# Wickroad's world is a candlelit trade road: warm parchment browns and
# ember market-fire against cold steel. No color is exact black or white.
BG_PALETTE = [
    (0, 0, 0),        # 0 transparent (backdrop shows through; never inked)
    (36, 28, 20),     # 1 sky / parchment base (warm near-dark)
    (60, 48, 36),     # 2 parchment speckle (sky highlight)
    (84, 64, 44),     # 3 road cobble mid
    (52, 40, 32),     # 4 road seam / edge dim
    (104, 72, 44),    # 5 town wall brown
    (160, 120, 84),   # 6 wall lit edge
    (208, 92, 44),    # 7 town roof ember
    (120, 52, 36),    # 8 roof shadow
    (248, 168, 64),   # 9 price bar bright ember (the market fire)
    (128, 80, 36),    # 10 price bar dim / empty frame
    (200, 208, 224),  # 11 cursor steel bright
    (112, 120, 144),  # 12 fork post steel dim
    (240, 208, 96),   # 13 cargo coin gold
    (248, 224, 140),  # 14 lantern / coin glow bright
    (88, 120, 104),   # 15 town accent green-steel (signboard)
]

CHAR_TO_INDEX = {'.': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6,
                 '7': 7, '8': 8, '9': 9, 'A': 10, 'B': 11, 'C': 12, 'D': 13,
                 'E': 14, 'F': 15}

# tile 2 — road cobble: warm stone with mortar seams (4)
ROAD = [
    '33333333',
    '34333343',
    '33333333',
    '33343333',
    '33333333',
    '43333334',
    '33333333',
    '33343333',
]

# tile 3 — road edge: a cobble lip over packed dirt
ROAD_EDGE = [
    '33333333',
    '44444444',
    '44444444',
    '44444444',
    '44444444',
    '44444444',
    '44444444',
    '44444444',
]

# tile 4 — town wall: running-bond brick, lit top edge (6), mortar seams (4)
WALL = [
    '66666666',
    '55545555',
    '55545555',
    '44444444',
    '55555455',
    '55555455',
    '44444444',
    '55545555',
]

# tile 5 — town roof: shingled ember cap with a shadow underside (8)
ROOF = [
    '77777777',
    '87777778',
    '77877877',
    '77777777',
    '78777787',
    '77777777',
    '88888888',
    '55555555',
]

# tile 6 — lit window: a warm lantern set in the town wall
WINDOW = [
    '55555555',
    '55555555',
    '55DDDD55',
    '5DEEEED5',
    '5DEEEED5',
    '55DDDD55',
    '55555555',
    '55555555',
]

# tile 7 — price bar filled: an ember market column, glow cap + dim sides
BAR_FILL = [
    'EEEEEEEE',
    'A999999A',
    'A999999A',
    'A999999A',
    'A999999A',
    'A999999A',
    'A999999A',
    'AAAAAAAA',
]

# tile 8 — price bar empty: the unfilled frame on parchment (1)
BAR_EMPTY = [
    'AAAAAAAA',
    'A111111A',
    'A111111A',
    'A111111A',
    'A111111A',
    'A111111A',
    'A111111A',
    'AAAAAAAA',
]

# tile 9 — cursor chevron: a steel '>' marking the selected good
CURSOR = [
    '........',
    '.B......',
    '.BB.....',
    '.BBB....',
    '.BBB....',
    '.BB.....',
    '.B......',
    '........',
]

# tile 10 — fork signpost: a two-armed sign on a steel post (the junction)
FORKSIGN = [
    '..CC....',
    '.FFFFF..',
    '.FBBBF..',
    '.FFFFF..',
    '..CC....',
    '..CC....',
    '..CC....',
    '..CC....',
]

# tile 11 — cargo pip: a gold coin on parchment
CARGO_PIP = [
    '11111111',
    '11DDDD11',
    '1DEEEED1',
    '1DEEEED1',
    '1DEEEED1',
    '11DDDD11',
    '11111111',
    '11111111',
]

# tile 1 (sky/parchment) is procedural speckle, drawn below.
HAND_TILES = {
    2: ROAD, 3: ROAD_EDGE, 4: WALL, 5: ROOF, 6: WINDOW,
    7: BAR_FILL, 8: BAR_EMPTY, 9: CURSOR, 10: FORKSIGN, 11: CARGO_PIP,
}

TILE_COUNT = 12  # 0 transparent + 1 sky + 10 hand-drawn


def rows_to(tiles, tile_index, rows):
    for y in range(8):
        for x in range(8):
            tiles[y][tile_index * 8 + x] = CHAR_TO_INDEX[rows[y][x]]


def build_tiles():
    rand = lcg(0x57494152)  # 'WIAR'
    tiles = [[0] * (TILE_COUNT * 8) for _ in range(8)]
    # tile 1 — sky/parchment: warm base (1) with a light speckle (2)
    for y in range(8):
        for x in range(8):
            tiles[y][8 + x] = 2 if rand(6) == 0 else 1
    for idx, rows in HAND_TILES.items():
        rows_to(tiles, idx, rows)
    return [v for row in tiles for v in row]


def main():
    tile_pixels = build_tiles()
    write_bmp(os.path.join(OUT_DIR, 'wr_tiles.bmp'), TILE_COUNT * 8, 8, 4,
              BG_PALETTE, tile_pixels)
    # Palette carrier: an 8bpp BMP whose BMP palette holds the 16 BG colors;
    # the pixel content is an index ramp (grit reads the palette, not pixels).
    ramp = [(x + y * 8) % 16 for y in range(8) for x in range(8)]
    write_bmp(os.path.join(OUT_DIR, 'wr_palette.bmp'), 8, 8, 8,
              BG_PALETTE + [(0, 0, 0)] * (16 - len(BG_PALETTE)), ramp)
    print('wrote wr_tiles.bmp, wr_palette.bmp')


if __name__ == '__main__':
    main()
