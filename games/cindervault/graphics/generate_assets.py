#!/usr/bin/env python3
"""Generate Cindervault's original indexed-BMP assets (deterministic).

Butano's asset pipeline (grit) consumes indexed BMPs + a JSON descriptor per
asset. These are ORIGINAL, procedurally-drawn graphics — no third-party art.
Committed BMPs are the build inputs; re-running this script reproduces them
byte-for-byte (stdlib only, no Pillow). Same convention as Lumen Drift's
games/lumen-drift/graphics/generate_assets.py.

    python3 games/cindervault/graphics/generate_assets.py

Outputs (next to this script):
    cv_tiles.bmp    4bpp 8x8 BG tiles (growth cut 5, the art pass):
                    0 empty (backdrop) · 1 floor · 2 wall · 3 ember ·
                    4 stairs · 5 lantern · 6 blade · 7 small monster ·
                    8 big monster
    cv_palette.bmp  8bpp palette carrier (16 BG colors)
    cv_player.bmp   4bpp 8x8 sprite: the torch-bearer (sprites ride OVER
                    the light window, so the player stays visible as the
                    screen darkens — they are CARRYING the light)

IMPORTANT palette rule: the headless harness's --assert-text matcher treats
exact black (0,0,0) and exact white (255,255,255) as text-ink colors, so no
tile or sprite color may be either (index 0 placeholders are transparent and
never rendered).
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
# Warm vault-dark family: browns and ember oranges against cold steel.
BG_PALETTE = [
    (0, 0, 0),        # 0 transparent (backdrop color set in code)
    (32, 20, 16),     # 1 floor base (warm near-dark)
    (56, 36, 24),     # 2 floor speckle mid
    (80, 56, 32),     # 3 floor speckle bright
    (48, 40, 48),     # 4 wall mortar (cold dark)
    (104, 88, 96),    # 5 wall brick mid
    (152, 136, 144),  # 6 wall brick lit edge
    (248, 144, 48),   # 7 ember bright (the game's signature orange)
    (168, 64, 24),    # 8 ember dim rim
    (216, 88, 56),    # 9 monster hide bright (small)
    (248, 216, 96),   # 10 lantern glass bright
    (136, 96, 32),    # 11 lantern frame dim
    (200, 208, 224),  # 12 steel bright (blade edge, stair lip)
    (112, 120, 144),  # 13 steel dim
    (120, 32, 56),    # 14 brute hide dark (big)
    (56, 24, 40),     # 15 brute shadow
]

# --- Hand-drawn 8x8 tiles ('.'=index 0; letters map below) ------------------
CHAR_TO_INDEX = {'.': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6,
                 '7': 7, '8': 8, '9': 9, 'A': 10, 'B': 11, 'C': 12, 'D': 13,
                 'E': 14, 'F': 15}

# tile 3 — ember: a glowing coal on the floor (bright core, dim rim)
EMBER = [
    '11111111',
    '11181111',
    '11878111',
    '18777811',
    '11877811',
    '11181111',
    '11111111',
    '11111111',
]

# tile 4 — stairs down: a steel '>' chevron (echoes the old glyph)
STAIRS = [
    '11111111',
    '1CD11111',
    '11CD1111',
    '111CD111',
    '111CD111',
    '11CD1111',
    '1CD11111',
    '11111111',
]

# tile 5 — lantern: warm glass in a dim frame, hung on the floor
LANTERN = [
    '11111111',
    '111BB111',
    '11BAAB11',
    '1BAAAAB1',
    '1BAAAAB1',
    '11BAAB11',
    '111BB111',
    '11111111',
]

# tile 6 — blade: a steel dagger laid diagonally, dim hilt
BLADE = [
    '111111C1',
    '11111CC1',
    '1111CC11',
    '111CC111',
    '11CD1111',
    '1BD11111',
    '1DB11111',
    '11111111',
]

# tile 7 — small monster: a crouched ember-lit critter, orange eyes
SMALL_MONSTER = [
    '11111111',
    '11199111',
    '11999911',
    '19979791',
    '19999991',
    '11911911',
    '11111111',
    '11111111',
]

# tile 8 — big monster: the 3-HP brute, hulking dark hide, ember eyes
BIG_MONSTER = [
    '11FFFF11',
    '1FFFFFF1',
    'FF7FF7FF',
    'FFFFFFFF',
    '1FEFFEF1',
    '1FFFFFF1',
    '1FF11FF1',
    '11111111',
]


def rows_to(tiles, tile_index, rows):
    for y in range(8):
        for x in range(8):
            tiles[y][tile_index * 8 + x] = CHAR_TO_INDEX[rows[y][x]]


# --- BG tiles: 9 tiles of 8x8, laid out horizontally (72x8 BMP) -------------
rand = lcg(0xC1DE5EED)
tiles = [[0] * 72 for _ in range(8)]
for y in range(8):
    for x in range(8):
        # tile 1 (floor): warm dark base with mid/bright speckle noise
        v = rand(16)
        tiles[y][8 + x] = 1 if v < 11 else (2 if v < 15 else 3)
        # tile 2 (wall): brickwork — mortar seams, lit top edge per brick
        brick_row = y % 4
        if brick_row == 3:
            w = 4                                # horizontal mortar seam
        else:
            shift = 0 if y < 4 else 4            # running bond offset
            in_seam = (x + shift) % 8 == 7       # vertical mortar seam
            w = 4 if in_seam else (6 if brick_row == 0 else 5)
        tiles[y][16 + x] = w
rows_to(tiles, 3, EMBER)
rows_to(tiles, 4, STAIRS)
rows_to(tiles, 5, LANTERN)
rows_to(tiles, 6, BLADE)
rows_to(tiles, 7, SMALL_MONSTER)
rows_to(tiles, 8, BIG_MONSTER)
TILE_PIXELS = [v for row in tiles for v in row]

# --- Sprite: 8x8 torch-bearer (the player) -----------------------------------
PLAYER_PALETTE = [
    (255, 0, 255),    # 0 transparent (magenta convention)
    (248, 232, 184),  # 1 face / flame core
    (240, 144, 48),   # 2 torch flame
    (88, 104, 128),   # 3 traveling cloak (cold steel-blue vs the warm vault)
    (40, 32, 48),     # 4 hood + boots (dark)
    (136, 96, 56),    # 5 torch shaft (wood)
]

PLAYER = [
    '......2.',
    '.44..12.',
    '.11..5..',
    '.3335...',
    '.333....',
    '.333....',
    '.3.3....',
    '.4.4....',
]
PLAYER_PIXELS = [CHAR_TO_INDEX['.' if c == '.' else c]
                 for row in PLAYER for c in row]


def main():
    write_bmp(os.path.join(OUT_DIR, 'cv_tiles.bmp'), 72, 8, 4,
              BG_PALETTE, TILE_PIXELS)
    # Palette carrier: 8bpp BMP whose BMP palette holds the 16 BG colors;
    # pixel content is an index ramp (grit reads the palette, not the pixels).
    ramp = [(x + y * 8) % 16 for y in range(8) for x in range(8)]
    write_bmp(os.path.join(OUT_DIR, 'cv_palette.bmp'), 8, 8, 8,
              BG_PALETTE + [(0, 0, 0)] * (16 - len(BG_PALETTE)), ramp)
    write_bmp(os.path.join(OUT_DIR, 'cv_player.bmp'), 8, 8, 4,
              PLAYER_PALETTE, PLAYER_PIXELS)
    print('wrote cv_tiles.bmp, cv_palette.bmp, cv_player.bmp')


if __name__ == '__main__':
    main()
