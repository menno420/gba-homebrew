#!/usr/bin/env python3
"""Generate Lumen Drift's original indexed-BMP assets (deterministic).

Butano's asset pipeline (grit) consumes indexed BMPs + a JSON descriptor per
asset. These are ORIGINAL, procedurally-drawn graphics — no third-party art.
Committed BMPs are the build inputs; re-running this script reproduces them
byte-for-byte (stdlib only, no Pillow).

    python3 games/lumen-drift/graphics/generate_assets.py

Outputs (next to this script):
    ld_tiles.bmp    4bpp 8x8 BG tiles: 0 empty · 1 rock · 2 rim-glow rock ·
                    3 crystal spike (hazard) · 4 surge front (hazard) ·
                    5 consumed rock (behind the surge) · 6 spark shard
                    (collectible) · 7 gallery rim rock (section 2) ·
                    8 deep rim rock (section 3)
    ld_palette.bmp  8bpp palette carrier (16 BG colors)
    ld_mote.bmp     4bpp 16x16 sprite: the light mote (radial glow)
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
BG_PALETTE = [
    (0, 0, 0),        # 0 transparent (backdrop color set in code)
    (32, 16, 56),     # 1 deep rock
    (56, 32, 96),     # 2 rock mid
    (80, 72, 136),    # 3 rock speckle
    (136, 224, 208),  # 4 rim glow (bright moss)
    (72, 144, 160),   # 5 rim dim
    (248, 88, 120),   # 6 crystal bright (hazard — warm pink vs the teal rim)
    (160, 40, 88),    # 7 crystal dim
    (248, 200, 112),  # 8 surge front hot ember
    (112, 48, 40),    # 9 consumed rock ember
    (248, 240, 168),  # 10 spark shard bright (warm near-white)
    (168, 144, 72),   # 11 spark shard dim
    (240, 168, 88),   # 12 gallery rim glow (amber — section 2 signature)
    (144, 96, 56),    # 13 gallery rim dim
    (184, 128, 248),  # 14 deep rim glow (violet — section 3 signature)
    (104, 64, 152),   # 15 deep rim dim
]

# --- Spark shard (collectible tile 6): hand-drawn 8x8 diamond sparkle -------
#     '.'=air A=bright(10) B=dim(11)
SHARD = [
    '...B....',
    '..BAB...',
    '.BAAAB..',
    'BAAAAAB.',
    '.BAAAB..',
    '..BAB...',
    '...B....',
    '........',
]

# --- Crystal spike (hazard tile 3): hand-drawn 8x8, '.'=air 6=bright 7=dim --
CRYSTAL = [
    '...6....',
    '..676.6.',
    '..676676',
    '.6777676',
    '.6777776',
    '67777776',
    '67777776',
    '66666666',
]

# --- BG tiles: 9 tiles of 8x8, laid out horizontally (72x8 BMP) -------------
rand = lcg(0xC0FFEE)
tiles = [[0] * 72 for _ in range(8)]
for y in range(8):
    for x in range(8):
        # tile 1 (rock): deep base with mid/speckle noise
        v = rand(16)
        tiles[y][8 + x] = 1 if v < 10 else (2 if v < 14 else 3)
        # tile 2 (rim rock): rock core, glowing 1px border
        border = x in (0, 7) or y in (0, 7)
        inner = x in (1, 6) or y in (1, 6)
        tiles[y][16 + x] = 4 if border else (5 if inner else 2)
        # tile 3 (crystal spike hazard): hand-drawn, sits on open cave air
        c = CRYSTAL[y][x]
        tiles[y][24 + x] = 0 if c == '.' else int(c)
# Separate deterministic streams for the surge tiles (appending to the tile-1
# stream would work too, but independent seeds keep each tile self-contained).
rand_front = lcg(0x5EED01)
rand_consumed = lcg(0x5EED02)
for y in range(8):
    for x in range(8):
        # tile 4 (surge front): crackling hot ember wall
        v = rand_front(16)
        tiles[y][32 + x] = 8 if v < 9 else (6 if v < 12 else 9)
        # tile 5 (consumed rock): dark ember cinders behind the front
        v = rand_consumed(16)
        tiles[y][40 + x] = 9 if v < 6 else 1
for y in range(8):
    for x in range(8):
        # tile 6 (spark shard collectible): hand-drawn diamond sparkle
        c = SHARD[y][x]
        tiles[y][48 + x] = 0 if c == '.' else (10 if c == 'A' else 11)
        # tiles 7/8 (section rim rocks): same shape as tile 2, recolored —
        # the rim color is each cave section's visual signature
        border = x in (0, 7) or y in (0, 7)
        inner = x in (1, 6) or y in (1, 6)
        tiles[y][56 + x] = 12 if border else (13 if inner else 2)  # gallery
        tiles[y][64 + x] = 14 if border else (15 if inner else 2)  # deep
TILE_PIXELS = [v for row in tiles for v in row]

# --- Sprite: 16x16 light mote (radial glow) ---------------------------------
MOTE_PALETTE = [
    (255, 0, 255),    # 0 transparent (magenta convention)
    (248, 248, 216),  # 1 hot core
    (232, 208, 128),  # 2 inner glow
    (192, 160, 64),   # 3 outer glow
    (112, 80, 32),    # 4 faint halo
]
mote = []
for y in range(16):
    for x in range(16):
        dx, dy = x - 7.5, y - 7.5
        d2 = dx * dx + dy * dy
        if d2 <= 6:
            mote.append(1)
        elif d2 <= 16:
            mote.append(2)
        elif d2 <= 32:
            mote.append(3)
        elif d2 <= 52:
            mote.append(4)
        else:
            mote.append(0)


def main():
    write_bmp(os.path.join(OUT_DIR, 'ld_tiles.bmp'), 72, 8, 4,
              BG_PALETTE, TILE_PIXELS)
    # Palette carrier: 8bpp BMP whose BMP palette holds the 16 BG colors;
    # pixel content is an index ramp (grit reads the palette, not the pixels).
    ramp = [(x + y * 8) % 16 for y in range(8) for x in range(8)]
    write_bmp(os.path.join(OUT_DIR, 'ld_palette.bmp'), 8, 8, 8,
              BG_PALETTE + [(0, 0, 0)] * (16 - len(BG_PALETTE)), ramp)
    write_bmp(os.path.join(OUT_DIR, 'ld_mote.bmp'), 16, 16, 4,
              MOTE_PALETTE, mote)
    print('wrote ld_tiles.bmp, ld_palette.bmp, ld_mote.bmp')


if __name__ == '__main__':
    main()
