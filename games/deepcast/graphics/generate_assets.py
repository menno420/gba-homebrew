#!/usr/bin/env python3
"""Generate Deepcast's original indexed-BMP assets (deterministic).

Butano's asset pipeline (grit) consumes indexed BMPs + a JSON descriptor per
asset. These are ORIGINAL, procedurally-drawn graphics — no third-party art.
Committed BMPs are the build inputs; re-running this script reproduces them
byte-for-byte (stdlib only, no Pillow). Same convention as Lumen Drift's and
Cindervault's games/*/graphics/generate_assets.py.

    python3 games/deepcast/graphics/generate_assets.py

Outputs (next to this script):
    dc_water.bmp    4bpp 8x8 BG tiles (growth cut 5, the art pass): tile 0
                    empty · tiles 1..10 solid gradient bands (color index =
                    tile index; the LAKE GRADIENT lives in the palette, so
                    depth can re-color the whole lake by rewriting 10
                    palette entries) · tile 11 the dusk surface shimmer
    dc_water_palette.bmp  8bpp palette carrier (16 BG colors — the gradient
                    at depth 0, i.e. the lake as the title sees it)
    dc_fish.bmp     4bpp 32x16 sprite, 16 frames — one SILHOUETTE per
                    species (frame = band*4 + tier, the dc_fishlog species
                    index): the depth band sets the silhouette's SIZE,
                    the rarity tier its SHADE (mythics glow palest)
    dc_rod.bmp      4bpp 32x32 sprite, 8 frames — the rod, bending: frame 0
                    at rest through frame 7 at the snap point (the analog
                    tension gauge the concept names)

IMPORTANT palette rule: the headless harness's --assert-text matcher treats
exact black (0,0,0) and exact white (255,255,255) as text-ink colors, so no
tile or sprite color may be either (index 0 placeholders are transparent and
never rendered). The gradient law keeps blue >= 4/31 for exactly this
reason: the deepest water is never pure black.
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


# --- the lake-gradient law (5-bit channels; MUST match main.cpp) ------------
# Band i (0 = surface .. 9 = bottom) at line depth d metres:
#     dim = 2*i + d/8
#     r = max(1, 5 - dim/3) · g = max(1, 16 - dim) · b = max(4, 26 - dim)
# The committed palette is the law at d = 0 (the lake as the title sees it);
# main.cpp rewrites entries 1..10 with the same law as the line sinks.

def gradient_color5(i, d):
    dim = 2 * i + d // 8
    r = max(1, 5 - dim // 3)
    g = max(1, 16 - dim)
    b = max(4, 26 - dim)
    return r, g, b


def c8(rgb5):
    """5-bit channels -> the RGB888 the BMP stores (grit quantizes by >>3)."""
    return tuple(v << 3 for v in rgb5)


# --- BG palette (16 colors; index 0 = transparent) --------------------------
BG_PALETTE = (
    [(0, 0, 0)]                                    # 0 transparent
    + [c8(gradient_color5(i, 0)) for i in range(10)]   # 1..10 the gradient
    + [c8((24, 29, 30))]                           # 11 surface glint
    + [c8((1, 1, 2))] * 4                          # 12..15 unused filler
)

# --- BG tiles: 12 tiles of 8x8, laid out horizontally (96x8 BMP) ------------
# tile 0 empty; tile 1+i solid color index 1+i (the band); tile 11 the
# surface shimmer: band-0 water with sparse glints on the top rows.
SHIMMER = [
    'B11B111B',
    '1B111B11',
    '11111111',
    '111B1111',
    '11111111',
    '11111111',
    '11111111',
    '11111111',
]
CHAR_TO_INDEX = {'.': 0, '1': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6,
                 '7': 7, '8': 8, '9': 9, 'A': 10, 'B': 11, 'C': 12, 'D': 13,
                 'E': 14, 'F': 15}

tiles = [[0] * 96 for _ in range(8)]
for band in range(10):
    for y in range(8):
        for x in range(8):
            tiles[y][(1 + band) * 8 + x] = 1 + band
for y in range(8):
    for x in range(8):
        tiles[y][11 * 8 + x] = CHAR_TO_INDEX[SHIMMER[y][x]]
TILE_PIXELS = [v for row in tiles for v in row]

# --- Fish silhouettes: 32x16 sprite, 16 frames (frame = band*4 + tier) ------
# Size from the DEPTH BAND (deeper water holds bigger shapes), shade from
# the RARITY TIER (the dc_fishlog species index drives the frame directly).
FISH_PALETTE = [
    (255, 0, 255),    # 0 transparent (magenta convention)
    c8((6, 7, 10)),   # 1 COMMON silhouette (near-dusk slate)
    c8((10, 12, 15)), # 2 UNCOMMON
    c8((14, 17, 19)), # 3 RARE
    c8((21, 24, 25)), # 4 MYTHIC (palest — a ghost in the water)
]

FISH_LEN = [12, 17, 22, 27]      # body length by band
FISH_HGT = [6, 8, 10, 12]        # body height by band


def fish_frame(band, tier):
    """One 32x16 silhouette: ellipse body + tail fin, facing right."""
    ink = 1 + tier
    px = [0] * (32 * 16)
    a = FISH_LEN[band] // 2
    b = FISH_HGT[band] // 2
    cx, cy = 18, 8
    for y in range(16):
        for x in range(32):
            if (x - cx) * (x - cx) * b * b + (y - cy) * (y - cy) * a * a \
                    <= a * a * b * b:
                px[y * 32 + x] = ink
    # tail: a triangle opening leftward from the body's left tip
    for t in range(b + 2):
        for y in range(cy - t, cy + t + 1):
            x = cx - a - (b + 2) + t
            if 0 <= x < 32 and 0 <= y < 16:
                px[y * 32 + x] = ink
    return px


FISH_PIXELS = []
for band in range(4):
    for tier in range(4):
        FISH_PIXELS.extend(fish_frame(band, tier))

# --- Rod sprite: 32x32, 8 bend frames (frame 7 = the snap point) ------------
ROD_PALETTE = [
    (255, 0, 255),    # 0 transparent
    c8((17, 12, 7)),  # 1 rod blank (wood)
    c8((24, 19, 11)), # 2 rod tip (bright wound guide)
    c8((22, 24, 25)), # 3 the line (pale, down into the water)
]


def rod_frame(k):
    """Quadratic Bezier rod from butt (4,29) to a tip that tension pulls
    down: tip = (27, 3 + 3k), control (14, 12) — frame 0 the resting arc,
    frame 7 fully loaded. A pale line falls from the tip to the water."""
    px = [0] * (32 * 32)
    p0 = (4, 29)
    p1 = (14, 12)
    p2 = (27, 3 + 3 * k)

    def plot(x, y, ink):
        if 0 <= x < 32 and 0 <= y < 32:
            px[y * 32 + x] = ink

    steps = 64
    for s in range(steps + 1):
        # integer Bezier: t = s/steps, all math scaled by steps^2
        omt = steps - s
        x = (omt * omt * p0[0] + 2 * omt * s * p1[0] + s * s * p2[0]) \
            // (steps * steps)
        y = (omt * omt * p0[1] + 2 * omt * s * p1[1] + s * s * p2[1]) \
            // (steps * steps)
        ink = 2 if s > steps * 3 // 4 else 1
        plot(x, y, ink)
        plot(x, y + 1, ink)      # 2px thick
    # the line: from the tip straight down to the canvas bottom
    for y in range(p2[1] + 1, 32):
        plot(p2[0] + 1, y, 3)
    return px


ROD_PIXELS = []
for k in range(8):
    ROD_PIXELS.extend(rod_frame(k))


def main():
    write_bmp(os.path.join(OUT_DIR, 'dc_water.bmp'), 96, 8, 4,
              BG_PALETTE, TILE_PIXELS)
    # Palette carrier: 8bpp BMP whose BMP palette holds the 16 BG colors;
    # pixel content is an index ramp (grit reads the palette, not the pixels).
    ramp = [(x + y * 8) % 16 for y in range(8) for x in range(8)]
    write_bmp(os.path.join(OUT_DIR, 'dc_water_palette.bmp'), 8, 8, 8,
              BG_PALETTE, ramp)
    write_bmp(os.path.join(OUT_DIR, 'dc_fish.bmp'), 32, 16 * 16, 4,
              FISH_PALETTE, FISH_PIXELS)
    write_bmp(os.path.join(OUT_DIR, 'dc_rod.bmp'), 32, 32 * 8, 4,
              ROD_PALETTE, ROD_PIXELS)
    print('wrote dc_water.bmp, dc_water_palette.bmp, dc_fish.bmp, dc_rod.bmp')


if __name__ == '__main__':
    main()
