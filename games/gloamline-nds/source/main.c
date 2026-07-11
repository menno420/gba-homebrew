// Gloamline — arc slice 3: the WALKING SKELETON (first playable).
//
// The concept doc's skeleton cut (docs/concepts/gloamline-concept.md):
// one arena screen (top = the yard), 8-way D-pad move, ONE Shambler
// chasing, contact = death -> card -> instant restart (START), dawn timer
// to survive NIGHT 1 (then the next night begins), HUD, bottom screen =
// watch-map v0 (live player + Shambler dots, dawn bar). All game rules
// live in the pure layer (gl_sim.h/.c, mirrored by tools/check-gloam.py);
// this file is input, state machine, rendering, and the telemetry mailbox.
//
// Determinism: seed = frame counter latched at the START press that begins
// a run (printed on the HUD and cards, so any human run is reproducible);
// scripted CI presses START on a fixed frame, so the whole run is a pure
// function of the input script. No wall clock, no runtime RNG.
//
// Telemetry mailbox (the gl_audio_hook concept ported to NDS): 16 u32
// words at the exported symbol `gl_telemetry`, rewritten every frame, so
// headless proofs (tools/nds-headless-check.py --elf/--watch) can assert
// game state numerically. Layout below at GL_T_*.
//
// 100% original content: code, text, and the two code-authored sprites.

#include <nds.h>

#include <stdio.h>

#include "gl_sim.h"

// --- telemetry mailbox -------------------------------------------------------
#define GL_T_MAGIC0 0   // 0x474C4F41 'GLOA'
#define GL_T_MAGIC1 1   // 0x4D4C4E45 'MLNE'
#define GL_T_FRAME 2    // global frame counter (every vblank)
#define GL_T_STATE 3    // 0 title / 1 playing / 2 dead / 3 dawn
#define GL_T_PX 4       // player x, 8.8 fixed (sprite center)
#define GL_T_PY 5       // player y
#define GL_T_ZX 6       // Shambler x
#define GL_T_ZY 7       // Shambler y
#define GL_T_DAWN 8     // frames left until dawn
#define GL_T_NIGHT 9    // current night (1-based)
#define GL_T_NIGHTS 10  // nights survived
#define GL_T_DEATHS 11  // deaths this power-on
#define GL_T_SEED 12    // seed of the current run
#define GL_T_DIST 13    // Chebyshev player<->Shambler, 8.8 fixed
#define GL_T_RUNFRAME 14// frames into the current night
#define GL_T_NFRAMES 15 // GL_NIGHT_FRAMES (build-flag visibility)

volatile uint32_t gl_telemetry[16];

enum
{
    STATE_TITLE = 0,
    STATE_PLAYING = 1,
    STATE_DEAD = 2,
    STATE_DAWN = 3,
};

// --- original sprite art (code-authored, 4bpp, shared 16-color palette) ------
// '.' = transparent; digits = palette index. Player = the lamplighter
// (indices 1-5: coat, skin, lantern brass, lantern glow, hat/boots).
// Shambler = the walking dead (indices 6-9: rot, pale flesh, shadow, eye).
static const char PLAYER_ART[16][17] = {
    "................",
    "....55555.......",
    "...5555555......",
    ".....222........",
    ".....222........",
    "....11111.......",
    "...1111111......",
    "..11111111.44...",
    "..1.11111.4334..",
    "..1.11111.4334..",
    "..2.11111..44...",
    "....11111...3...",
    "....11.11.......",
    "....11.11.......",
    "...555.555......",
    "................",
};

static const char SHAMBLER_ART[16][17] = {
    "................",
    ".....777........",
    "....79777.......",
    "....77777.......",
    ".....866........",
    "....66666.......",
    "..66666666666...",
    ".66.666666..66..",
    ".7..666666...7..",
    "....666666......",
    "....66.666......",
    "....66..66......",
    "...866...66.....",
    "...88....868....",
    "....8.....88....",
    "................",
};

static void load_sprite_gfx(u16 *gfx, const char art[16][17])
{
    int out = 0;
    for (int ty = 0; ty < 2; ty++)
        for (int tx = 0; tx < 2; tx++)
            for (int y = 0; y < 8; y++)
                for (int half = 0; half < 2; half++)
                {
                    u16 v = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        char c = art[ty * 8 + y][tx * 8 + half * 4 + i];
                        u16 nib = (c == '.') ? 0 : (u16)(c - '0');
                        v |= nib << (4 * i);
                    }
                    gfx[out++] = v;
                }
}

static void load_palette(void)
{
    SPRITE_PALETTE[1] = RGB15(6, 6, 9);      // coat: gloam blue-grey
    SPRITE_PALETTE[2] = RGB15(27, 22, 17);   // skin
    SPRITE_PALETTE[3] = RGB15(31, 26, 10);   // lantern brass/flame
    SPRITE_PALETTE[4] = RGB15(31, 30, 21);   // lantern glow
    SPRITE_PALETTE[5] = RGB15(4, 3, 5);      // hat + boots
    SPRITE_PALETTE[6] = RGB15(11, 15, 10);   // shambler rot green
    SPRITE_PALETTE[7] = RGB15(21, 22, 18);   // shambler pale flesh
    SPRITE_PALETTE[8] = RGB15(5, 6, 5);      // shambler shadow
    SPRITE_PALETTE[9] = RGB15(25, 7, 6);     // shambler eye
}

// --- watch-map (bottom screen) geometry ---------------------------------------
#define MAP_COL0 2
#define MAP_ROW0 5
#define MAP_COLS_N 28
#define MAP_ROWS_N 14

static int map_col_of(int32_t fx)
{
    int px = fx / GL_ONE;                    // 16..239
    int col = MAP_COL0 + (px - 16) * MAP_COLS_N / 224;
    if (col < MAP_COL0) col = MAP_COL0;
    if (col > MAP_COL0 + MAP_COLS_N - 1) col = MAP_COL0 + MAP_COLS_N - 1;
    return col;
}

static int map_row_of(int32_t fy)
{
    int py = fy / GL_ONE;                    // 32..175
    int row = MAP_ROW0 + (py - 32) * MAP_ROWS_N / 144;
    if (row < MAP_ROW0) row = MAP_ROW0;
    if (row > MAP_ROW0 + MAP_ROWS_N - 1) row = MAP_ROW0 + MAP_ROWS_N - 1;
    return row;
}

// --- run state -----------------------------------------------------------------
typedef struct
{
    uint32_t seed;
    uint32_t night;
    uint32_t dawn_left;
    uint32_t run_frame;
    int32_t px, py;
    int32_t zx, zy;
} Run;

static void start_night(Run *run)
{
    run->dawn_left = GL_NIGHT_FRAMES;
    run->run_frame = 0;
    run->px = GL_PLAYER_START_X;
    run->py = GL_PLAYER_START_Y;
    gl_spawn_of_night(run->seed, run->night, 0, &run->zx, &run->zy);
}

static void start_run(Run *run, uint32_t seed)
{
    run->seed = seed;
    run->night = 1;
    start_night(run);
}

// --- top-screen scenes -----------------------------------------------------------
static PrintConsole top_console;
static PrintConsole bottom_console;

static void draw_fence(void)
{
    consoleSelect(&top_console);
    printf("\x1b[2;0H################################");
    for (int row = 3; row <= 22; row++)
        printf("\x1b[%d;0H#\x1b[%d;31H#", row, row);
    printf("\x1b[23;0H################################");
}

static void draw_title(void)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[6;10HGLOAMLINE\n");
    printf("\x1b[8;5Hthe last lamplighter\n");
    printf("\x1b[9;5Hholds the fence line\n");
    printf("\x1b[12;6Hmove: +pad (8-way)\n");
    printf("\x1b[13;6Hdon't get touched\n");
    printf("\x1b[14;6Hsurvive to dawn\n");
    printf("\x1b[17;9HPRESS START\n");
}

static void draw_death_card(const Run *run, uint32_t deaths)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[7;7HTHE COLD HANDS\n");
    printf("\x1b[8;9HFOUND YOU\n");
    printf("\x1b[11;7Hnight %lu", (unsigned long)run->night);
    printf("\x1b[12;7Hseed %lu", (unsigned long)run->seed);
    printf("\x1b[13;7Hdeaths %lu", (unsigned long)deaths);
    printf("\x1b[16;5HPRESS START: retry\n");
}

static void draw_dawn_card(const Run *run, uint32_t nights)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[7;9HDAWN BREAKS\n");
    printf("\x1b[10;5Hnight %lu survived", (unsigned long)run->night);
    printf("\x1b[11;5Hnights total %lu", (unsigned long)nights);
    printf("\x1b[12;5Hseed %lu", (unsigned long)run->seed);
    printf("\x1b[15;3HPRESS START: night %lu",
           (unsigned long)(run->night + 1));
}

static void draw_hud(const Run *run, uint32_t nights)
{
    unsigned int secs = run->dawn_left / 60;
    consoleSelect(&top_console);
    printf("\x1b[0;1HNIGHT %lu   DAWN %u:%02u \x1b[K",
           (unsigned long)run->night, secs / 60, secs % 60);
    printf("\x1b[1;1HSEED %lu  NIGHTS %lu\x1b[K",
           (unsigned long)run->seed, (unsigned long)nights);
}

// --- bottom-screen watch-map -------------------------------------------------------
static int map_prev_p_row = -1, map_prev_p_col = -1;
static int map_prev_z_row = -1, map_prev_z_col = -1;

static void draw_watch_map_frame(void)
{
    consoleSelect(&bottom_console);
    consoleClear();
    printf("\x1b[0;1HWATCH-MAP");
    printf("\x1b[2;1HP you   Z the shambler");
    printf("\x1b[4;1H+----------------------------+");
    for (int row = MAP_ROW0; row < MAP_ROW0 + MAP_ROWS_N; row++)
        printf("\x1b[%d;1H|\x1b[%d;30H|", row, row);
    printf("\x1b[19;1H+----------------------------+");
    map_prev_p_row = map_prev_p_col = -1;
    map_prev_z_row = map_prev_z_col = -1;
}

static void draw_watch_map(const Run *run, int state)
{
    consoleSelect(&bottom_console);

    // dawn bar: 20 cells fill as the night burns down
    unsigned int filled =
        (unsigned int)((GL_NIGHT_FRAMES - run->dawn_left) * 20
                       / GL_NIGHT_FRAMES);
    printf("\x1b[21;1HDAWN [");
    for (unsigned int i = 0; i < 20; i++)
        putchar(i < filled ? '=' : '-');
    printf("] N%lu", (unsigned long)run->night);

    if (state != STATE_PLAYING)
        return;

    int p_row = map_row_of(run->py), p_col = map_col_of(run->px);
    int z_row = map_row_of(run->zy), z_col = map_col_of(run->zx);

    if (map_prev_z_row >= 0)
        printf("\x1b[%d;%dH ", map_prev_z_row, map_prev_z_col);
    if (map_prev_p_row >= 0)
        printf("\x1b[%d;%dH ", map_prev_p_row, map_prev_p_col);
    printf("\x1b[%d;%dHZ", z_row, z_col);
    printf("\x1b[%d;%dHP", p_row, p_col);   // player wins a shared cell
    map_prev_p_row = p_row; map_prev_p_col = p_col;
    map_prev_z_row = z_row; map_prev_z_col = z_col;
}

// --- main ------------------------------------------------------------------------
int main(void)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&top_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0,
                true /* main engine */, true);
    consoleInit(&bottom_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0,
                false /* sub engine */, true);

    oamInit(&oamMain, SpriteMapping_1D_32, false);
    load_palette();
    u16 *player_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16,
                                     SpriteColorFormat_16Color);
    u16 *shambler_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16,
                                       SpriteColorFormat_16Color);
    load_sprite_gfx(player_gfx, PLAYER_ART);
    load_sprite_gfx(shambler_gfx, SHAMBLER_ART);

    Run run = {0};
    run.dawn_left = GL_NIGHT_FRAMES;         // title-screen dawn bar: empty
    int state = STATE_TITLE;
    uint32_t frame = 0;
    uint32_t deaths = 0;
    uint32_t nights_survived = 0;
    bool pad_seen_idle = false;    // KEYINPUT boot-trap guard (session 16)

    draw_title();
    draw_watch_map_frame();

    gl_telemetry[GL_T_MAGIC0] = 0x474C4F41u;   // 'GLOA'
    gl_telemetry[GL_T_MAGIC1] = 0x4D4C4E45u;   // 'MLNE'
    gl_telemetry[GL_T_NFRAMES] = GL_NIGHT_FRAMES;

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        frame++;

        uint32_t held = keysHeld();
        uint32_t down = keysDown();
        // Only trust START once the pad has been seen idle: an emulator
        // whose KEYINPUT reads all-pressed (active-low zero) before its
        // first input latch must not skip the title at boot.
        if (held == 0)
            pad_seen_idle = true;
        bool start = pad_seen_idle && (down & KEY_START);

        switch (state)
        {
        case STATE_TITLE:
            if (start)
            {
                start_run(&run, frame);     // latch the frame-counter seed
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame();
            }
            break;

        case STATE_PLAYING:
            gl_player_step(&run.px, &run.py,
                           held & KEY_UP, held & KEY_DOWN,
                           held & KEY_LEFT, held & KEY_RIGHT);
            gl_shambler_step(&run.zx, &run.zy, run.px, run.py,
                             0 /* zombie_id */, run.run_frame);
            run.run_frame++;

            if (gl_contact(run.px, run.py, run.zx, run.zy))
            {
                deaths++;
                state = STATE_DEAD;
                oamClear(&oamMain, 0, 2);
                draw_death_card(&run, deaths);
            }
            else if (--run.dawn_left == 0)
            {
                nights_survived = run.night;
                state = STATE_DAWN;
                oamClear(&oamMain, 0, 2);
                draw_dawn_card(&run, nights_survived);
            }
            else
            {
                draw_hud(&run, nights_survived);
            }
            break;

        case STATE_DEAD:
            if (start)                       // instant restart, fresh seed
            {
                start_run(&run, frame);
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame();
            }
            break;

        case STATE_DAWN:
            if (start)                       // same run, next night
            {
                run.night++;
                start_night(&run);
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame();
            }
            break;
        }

        if (state == STATE_PLAYING)
        {
            oamSet(&oamMain, 0, run.px / GL_ONE - 8, run.py / GL_ONE - 8,
                   0, 0, SpriteSize_16x16, SpriteColorFormat_16Color,
                   player_gfx, -1, false, false, false, false, false);
            oamSet(&oamMain, 1, run.zx / GL_ONE - 8, run.zy / GL_ONE - 8,
                   0, 0, SpriteSize_16x16, SpriteColorFormat_16Color,
                   shambler_gfx, -1, false, false, false, false, false);
        }
        oamUpdate(&oamMain);
        draw_watch_map(&run, state);

        // Telemetry mailbox: rewritten every frame, read headlessly.
        gl_telemetry[GL_T_FRAME] = frame;
        gl_telemetry[GL_T_STATE] = (uint32_t)state;
        gl_telemetry[GL_T_PX] = (uint32_t)run.px;
        gl_telemetry[GL_T_PY] = (uint32_t)run.py;
        gl_telemetry[GL_T_ZX] = (uint32_t)run.zx;
        gl_telemetry[GL_T_ZY] = (uint32_t)run.zy;
        gl_telemetry[GL_T_DAWN] = run.dawn_left;
        gl_telemetry[GL_T_NIGHT] = run.night;
        gl_telemetry[GL_T_NIGHTS] = nights_survived;
        gl_telemetry[GL_T_DEATHS] = deaths;
        gl_telemetry[GL_T_SEED] = run.seed;
        gl_telemetry[GL_T_DIST] =
            (uint32_t)gl_chebyshev(run.px, run.py, run.zx, run.zy);
        gl_telemetry[GL_T_RUNFRAME] = run.run_frame;
    }

    return 0;
}
