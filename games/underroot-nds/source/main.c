// Underroot — arc slice 1: DUAL-SCREEN SKELETON.
//
// The two screens ARE the game (docs/arcs/UNDERROOT.md):
//   TOP    = the meadow. A placeholder grass field with the hawk shadow
//            sweeping across it on the DETERMINISTIC pure schedule
//            ur_hawk_x/ur_hawk_active/ur_hawk_y = f(seed, season, index)
//            (ur_sim.c). Slice 1 runs one fixed seed (UR_SEED), fixed
//            season (spring), so the boot proof can assert exact values.
//   BOTTOM = the burrow cross-section. A tile-snap dig grid: hold the
//            stylus to paint DUG cells (a drawn tunnel), soil elsewhere.
//            The burrow is a GRAPH — ur_burrow_size BFS-counts the cells
//            reachable from the entrance mouth, so a disconnected pocket is
//            visibly NOT part of the colony (the slice-3 forager-pathing
//            seed).
//
// Everything the sim decides is pure (ur_sim.c) and mirrored host-side
// (tools/check-underroot.py); this file is libnds boot + render + input +
// the telemetry mailbox only. The mailbox is a fixed array of words at the
// exported symbol `ur_telemetry`, rewritten every frame, so a headless
// DeSmuME run (tools/nds-headless-check.py) can ELF-resolve it and assert
// the pure schedule and the dig accounting against these very functions.

#include <nds.h>
#include <stdio.h>

#include "ur_sim.h"

// The telemetry mailbox — external linkage so the ELF carries the symbol.
volatile uint32_t ur_telemetry[UR_T_WORDS];

static PrintConsole top_console;
static PrintConsole bottom_console;

// --- meadow (top screen): grass + the deterministic hawk shadow ------------
static void draw_meadow(uint32_t frame, int hawk_on, int32_t hawk_x,
                        int32_t hawk_y, uint32_t dug, uint32_t con)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[0;0HUNDERROOT  the meadow");
    printf("\x1b[1;0Hspring   frame %lu", (unsigned long)frame);

    // A placeholder grass field, rows 3..15 (the hawk lane band y 24..119).
    for (int row = 3; row < 16; row++)
    {
        printf("\x1b[%d;0H", row);
        for (int col = 0; col < 32; col++)
            printf(",");
    }

    // The hawk shadow: one glyph at its pure-scheduled meadow cell.
    if (hawk_on)
    {
        int hc = hawk_x / 8;             // px -> 8px console column
        int hr = hawk_y / 8;             // px -> 8px console row
        if (hc < 0) hc = 0; else if (hc > 31) hc = 31;
        if (hr < 3) hr = 3; else if (hr > 15) hr = 15;
        printf("\x1b[%d;%dHV", hr, hc);
        printf("\x1b[17;0Hhawk x=%ld", (long)hawk_x);
    }
    else
    {
        printf("\x1b[17;0Hsky clear");
    }

    printf("\x1b[19;0Hdig below: hold stylus");
    printf("\x1b[20;0Hburrow  dug %lu  con %lu",
           (unsigned long)dug, (unsigned long)con);
}

// --- burrow (bottom screen): the tile-snap dig grid ------------------------
static void draw_burrow(const uint8_t grid[UR_CELLS])
{
    consoleSelect(&bottom_console);
    consoleClear();
    printf("\x1b[0;0HTHE BURROW  cross-section");
    printf("\x1b[1;0H# soil  . dug  M mouth");

    // The grid, one char per cell, drawn from console row 3, col 4.
    for (int r = 0; r < UR_GRID_H; r++)
    {
        printf("\x1b[%d;4H", r + 3);
        for (int c = 0; c < UR_GRID_W; c++)
        {
            char ch;
            if (c == UR_ENTRANCE_COL && r == UR_ENTRANCE_ROW)
                ch = 'M';
            else
                ch = grid[r * UR_GRID_W + c] ? '.' : '#';
            printf("%c", ch);
        }
    }
}

static uint32_t dug_total(const uint8_t grid[UR_CELLS])
{
    uint32_t n = 0;
    for (int i = 0; i < UR_CELLS; i++)
        n += grid[i];
    return n;
}

int main(void)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&top_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0,
                true /* main engine */, true);
    consoleInit(&bottom_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0,
                false /* sub engine */, true);

    uint8_t grid[UR_CELLS];
    ur_fresh_grid(grid);

    uint32_t frame = 0;
    int32_t last_col = -1;
    int32_t last_row = -1;
    bool pad_seen_idle = false;          // KEYINPUT boot-trap guard (PL note)
    bool burrow_dirty = true;            // redraw the grid on next frame

    ur_telemetry[UR_T_MAGIC0] = 0x554E4452u;   // 'UNDR'
    ur_telemetry[UR_T_MAGIC1] = 0x524F4F54u;   // 'ROOT'
    ur_telemetry[UR_T_GW] = UR_GRID_W;
    ur_telemetry[UR_T_GH] = UR_GRID_H;
    ur_telemetry[UR_T_SPARE] = 0;

    draw_burrow(grid);

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        frame++;

        uint32_t held = keysHeld();
        // Don't trust the pad (incl. KEY_TOUCH) until it has read idle once:
        // an emulator whose KEYINPUT boots all-pressed must not phantom-dig
        // at frame 1 (the py-desmume boot-trap, docs/PLATFORM-LIMITS.md).
        if (held == 0)
            pad_seen_idle = true;

        int touch_now = 0;
        if (pad_seen_idle && (held & KEY_TOUCH))
        {
            touchPosition touch = {0};
            touchRead(&touch);
            int32_t col, row;
            if (ur_cell_of_touch(touch.px, touch.py, &col, &row))
            {
                touch_now = 1;
                last_col = col;
                last_row = row;
                if (ur_dig(grid, col, row))
                    burrow_dirty = true;
            }
        }

        int hawk_on = ur_hawk_active(frame);
        int32_t hawk_x = ur_hawk_x(UR_SEED, 0, frame);
        int32_t hawk_y = ur_hawk_y(UR_SEED, 0, frame);
        uint32_t dug = dug_total(grid);
        uint32_t con = ur_burrow_size(grid);

        // Top screen animates every frame (the hawk sweeps); the burrow is
        // redrawn only when a dig changed it (frame-budget discipline).
        draw_meadow(frame, hawk_on, hawk_x, hawk_y, dug, con);
        if (burrow_dirty)
        {
            draw_burrow(grid);
            burrow_dirty = false;
        }

        ur_telemetry[UR_T_FRAME] = frame;
        ur_telemetry[UR_T_SEASON] = 0;
        ur_telemetry[UR_T_SEED] = UR_SEED;
        ur_telemetry[UR_T_HAWKON] = (uint32_t)hawk_on;
        ur_telemetry[UR_T_HAWKX] = hawk_on ? (uint32_t)hawk_x : 0;
        ur_telemetry[UR_T_HAWKY] = hawk_on ? (uint32_t)hawk_y : 0;
        ur_telemetry[UR_T_DUG] = dug;
        ur_telemetry[UR_T_CON] = con;
        ur_telemetry[UR_T_TCOL] = (uint32_t)last_col;
        ur_telemetry[UR_T_TROW] = (uint32_t)last_row;
        ur_telemetry[UR_T_TOUCH] = (uint32_t)touch_now;
    }

    return 0;
}
