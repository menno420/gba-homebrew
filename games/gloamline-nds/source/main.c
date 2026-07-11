// Gloamline — arc slice 4: SHOVE + WAVES (on the slice-3 skeleton).
//
// The concept doc's next feature cut (docs/concepts/gloamline-concept.md):
// night N sends a deterministic WAVE — gl_wave_count(N) Shamblers (ramp
// 1, 3, 5, ... plateau at GL_ZOMBIE_CAP = 24), each spawning at the pure
// schedule (gl_spawn_frame, gl_spawn_of_night) — and the player gets the
// SHOVE (A): knock the nearest Shambler in reach back GL_SHOVE_PUSH with
// a brief stun, on a cooldown (a pressure valve, not a weapon). Top
// screen = the yard, bottom = watch-map (ALL zombie dots + dawn bar),
// contact = death -> card -> instant restart, survive to dawn -> next
// night. All game rules live in the pure layer (gl_sim.h/.c, mirrored by
// tools/check-gloam.py); this file is input, state machine, rendering,
// and the telemetry mailbox.
//
// GL_STRESS (make GL_STRESS=1 — CI perf proof, NEVER shipped): night 1
// spawns the full 24-Shambler cap at frame 0 and contact does not kill,
// so an idle headless run measures the honest worst-case frame cost via
// the GL_T_VLINES / GL_T_VLMAX scanline telemetry. Every sim/render code
// path is identical to the shipped build.
//
// Determinism: seed = frame counter latched at the START press that begins
// a run (printed on the HUD and cards, so any human run is reproducible);
// scripted CI presses START on a fixed frame, so the whole run is a pure
// function of the input script. No wall clock, no runtime RNG.
//
// Telemetry mailbox (the gl_audio_hook concept ported to NDS): 24 u32
// words at the exported symbol `gl_telemetry`, rewritten every frame, so
// headless proofs (tools/nds-headless-check.py --elf/--watch) can assert
// game state numerically. Layout below at GL_T_*. Slots 0-15 keep their
// slice-3 meanings EXACTLY (the pinned CI asserts read them); slice 4
// appends 16-23 — with one refinement: on a multi-zombie night the
// ZX/ZY/DIST/NSTUN slots describe the NEAREST Shambler (identical to
// slice 3 whenever one zombie is up).
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
#define GL_T_ZX 6       // nearest Shambler x
#define GL_T_ZY 7       // nearest Shambler y
#define GL_T_DAWN 8     // frames left until dawn
#define GL_T_NIGHT 9    // current night (1-based)
#define GL_T_NIGHTS 10  // nights survived
#define GL_T_DEATHS 11  // deaths this power-on
#define GL_T_SEED 12    // seed of the current run
#define GL_T_DIST 13    // Chebyshev player<->nearest Shambler, 8.8 fixed
#define GL_T_RUNFRAME 14// frames into the current night
#define GL_T_NFRAMES 15 // GL_NIGHT_FRAMES (build-flag visibility)
#define GL_T_ZALIVE 16  // Shamblers on the yard (spawned this night)
#define GL_T_ZTOTAL 17  // tonight's full wave size (schedule length)
#define GL_T_SHOVECD 18 // shove cooldown frames left (0 = ready)
#define GL_T_SHOVES 19  // shoves CONNECTED this power-on
#define GL_T_NSTUN 20   // stun frames left on the nearest Shambler
#define GL_T_VLINES 21  // scanlines this frame's work took (from vblank)
#define GL_T_VLMAX 22   // worst GL_T_VLINES since power-on (<71 = the
                        // whole frame fits inside vblank; <263 = 60 fps)
#define GL_T_SPARE 23   // reserved, always 0

volatile uint32_t gl_telemetry[24];

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
// GL_STRESS swaps ONLY the wave schedule (full cap, all at frame 0) and
// disarms death — see the header comment. Shipped builds use the pure
// gl_wave_count/gl_spawn_frame schedule untouched.
#ifdef GL_STRESS
#define WAVE_TOTAL(night) ((uint32_t)GL_ZOMBIE_CAP)
#define SPAWN_FRAME(night, index) 0u
#else
#define WAVE_TOTAL(night) gl_wave_count(night)
#define SPAWN_FRAME(night, index) gl_spawn_frame(night, index)
#endif

typedef struct
{
    uint32_t seed;
    uint32_t night;
    uint32_t dawn_left;
    uint32_t run_frame;
    int32_t px, py;
    int32_t zx[GL_ZOMBIE_CAP], zy[GL_ZOMBIE_CAP];
    uint32_t zstun[GL_ZOMBIE_CAP];
    uint32_t z_count;                // spawned so far; they never despawn
    uint32_t wave_total;             // tonight's schedule length
    uint32_t shove_cd;               // frames until the next shove attempt
} Run;

// Spawn every zombie whose scheduled frame has arrived (index order — the
// schedule is non-decreasing in index, so one pass per frame suffices).
static void spawn_due(Run *run)
{
    while (run->z_count < run->wave_total
           && SPAWN_FRAME(run->night, run->z_count) <= run->run_frame)
    {
        uint32_t i = run->z_count;
        gl_spawn_of_night(run->seed, run->night, i, &run->zx[i], &run->zy[i]);
        run->zstun[i] = 0;
        run->z_count++;
    }
}

static void start_night(Run *run)
{
    run->dawn_left = GL_NIGHT_FRAMES;
    run->run_frame = 0;
    run->px = GL_PLAYER_START_X;
    run->py = GL_PLAYER_START_Y;
    run->z_count = 0;
    run->wave_total = WAVE_TOTAL(run->night);
    run->shove_cd = 0;
    spawn_due(run);                  // index 0 spawns at frame 0, as slice 3
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
    printf("\x1b[13;6Hshove: A (cooldown)\n");
    printf("\x1b[14;6Hdon't get touched\n");
    printf("\x1b[15;6Hsurvive to dawn\n");
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
    printf("\x1b[0;1HNIGHT %lu  DAWN %u:%02u  Z %lu\x1b[K",
           (unsigned long)run->night, secs / 60, secs % 60,
           (unsigned long)run->z_count);
    printf("\x1b[1;1HSEED %lu NIGHTS %lu SHV %c\x1b[K",
           (unsigned long)run->seed, (unsigned long)nights,
           run->shove_cd == 0 ? '+' : '.');
}

// --- bottom-screen watch-map -------------------------------------------------------
// Erase-then-redraw marks: the player + EVERY zombie on the yard, so the
// map is the crowd radar the concept promises. prev arrays remember last
// frame's cells (1 player + up to GL_ZOMBIE_CAP zombies).
static int map_prev_row[1 + GL_ZOMBIE_CAP];
static int map_prev_col[1 + GL_ZOMBIE_CAP];
static int map_prev_n = 0;

static void draw_watch_map_frame(void)
{
    consoleSelect(&bottom_console);
    consoleClear();
    printf("\x1b[0;1HWATCH-MAP");
    printf("\x1b[2;1HP you   Z the shamblers");
    printf("\x1b[4;1H+----------------------------+");
    for (int row = MAP_ROW0; row < MAP_ROW0 + MAP_ROWS_N; row++)
        printf("\x1b[%d;1H|\x1b[%d;30H|", row, row);
    printf("\x1b[19;1H+----------------------------+");
    map_prev_n = 0;
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

    for (int i = 0; i < map_prev_n; i++)
        printf("\x1b[%d;%dH ", map_prev_row[i], map_prev_col[i]);
    map_prev_n = 0;

    for (uint32_t i = 0; i < run->z_count; i++)
    {
        int z_row = map_row_of(run->zy[i]), z_col = map_col_of(run->zx[i]);
        printf("\x1b[%d;%dHZ", z_row, z_col);
        map_prev_row[map_prev_n] = z_row;
        map_prev_col[map_prev_n++] = z_col;
    }
    int p_row = map_row_of(run->py), p_col = map_col_of(run->px);
    printf("\x1b[%d;%dHP", p_row, p_col);   // player wins a shared cell
    map_prev_row[map_prev_n] = p_row;
    map_prev_col[map_prev_n++] = p_col;
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
    uint32_t shoves = 0;           // shoves CONNECTED this power-on
    uint32_t vlines_max = 0;       // worst frame cost seen, in scanlines
    bool pad_seen_idle = false;    // KEYINPUT boot-trap guard (session 16)

    draw_title();
    draw_watch_map_frame();

    gl_telemetry[GL_T_MAGIC0] = 0x474C4F41u;   // 'GLOA'
    gl_telemetry[GL_T_MAGIC1] = 0x4D4C4E45u;   // 'MLNE'
    gl_telemetry[GL_T_NFRAMES] = GL_NIGHT_FRAMES;
    gl_telemetry[GL_T_SPARE] = 0;

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
        {
            spawn_due(&run);                 // tonight's wave trickles in
            gl_player_step(&run.px, &run.py,
                           held & KEY_UP, held & KEY_DOWN,
                           held & KEY_LEFT, held & KEY_RIGHT);

            // The shove: any A press with a cold cooldown is an ATTEMPT
            // (arming the cooldown, hit or whiff); it CONNECTS only if
            // the nearest Shambler is within GL_SHOVE_RANGE — knockback
            // via the pure gl_shove, plus GL_SHOVE_STUN frames frozen.
            if (run.shove_cd > 0)
                run.shove_cd--;
            if ((down & KEY_A) && run.shove_cd == 0)
            {
                run.shove_cd = GL_SHOVE_COOLDOWN;
                uint32_t nearest = 0;
                int32_t best = INT32_MAX;
                for (uint32_t i = 0; i < run.z_count; i++)
                {
                    int32_t d = gl_chebyshev(run.px, run.py,
                                             run.zx[i], run.zy[i]);
                    if (d < best) { best = d; nearest = i; }
                }
                if (run.z_count > 0
                    && gl_shove(run.px, run.py,
                                &run.zx[nearest], &run.zy[nearest]))
                {
                    run.zstun[nearest] = GL_SHOVE_STUN;
                    shoves++;
                }
            }

            for (uint32_t i = 0; i < run.z_count; i++)
            {
                if (run.zstun[i] > 0)
                    run.zstun[i]--;          // stunned: no step this frame
                else
                    gl_shambler_step(&run.zx[i], &run.zy[i], run.px, run.py,
                                     i /* zombie_id */, run.run_frame);
            }
            run.run_frame++;

            int touched = 0;
            for (uint32_t i = 0; i < run.z_count && !touched; i++)
                touched = gl_contact(run.px, run.py, run.zx[i], run.zy[i]);
#ifdef GL_STRESS
            touched = 0;                     // perf build: measure, don't die
#endif
            if (touched)
            {
                deaths++;
                state = STATE_DEAD;
                oamClear(&oamMain, 0, 1 + GL_ZOMBIE_CAP);
                draw_death_card(&run, deaths);
            }
            else if (--run.dawn_left == 0)
            {
                nights_survived = run.night;
                state = STATE_DAWN;
                oamClear(&oamMain, 0, 1 + GL_ZOMBIE_CAP);
                draw_dawn_card(&run, nights_survived);
            }
            else
            {
                draw_hud(&run, nights_survived);
            }
            break;
        }

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
            for (uint32_t i = 0; i < run.z_count; i++)
                oamSet(&oamMain, 1 + (int)i,
                       run.zx[i] / GL_ONE - 8, run.zy[i] / GL_ONE - 8,
                       0, 0, SpriteSize_16x16, SpriteColorFormat_16Color,
                       shambler_gfx, -1, false,
                       run.zstun[i] > 0 /* hflip = reeling from the shove */,
                       false, false, false);
        }
        oamUpdate(&oamMain);
        draw_watch_map(&run, state);

        // Nearest Shambler (the one that can kill you) for the mailbox;
        // arrays persist through the death/dawn cards, so the freeze-frame
        // numbers stay readable there — exactly slice-3 behavior when one
        // zombie is up.
        uint32_t near_i = 0;
        int32_t near_d = 0;
        uint32_t near_stun = 0;
        if (run.z_count > 0)
        {
            near_d = INT32_MAX;
            for (uint32_t i = 0; i < run.z_count; i++)
            {
                int32_t d = gl_chebyshev(run.px, run.py,
                                         run.zx[i], run.zy[i]);
                if (d < near_d) { near_d = d; near_i = i; }
            }
            near_stun = run.zstun[near_i];
        }

        // Frame-cost probe: swiWaitForVBlank returns at scanline 192 (the
        // top of vblank); REG_VCOUNT here says how many scanlines this
        // frame's whole update took. <71 = everything fit inside vblank,
        // <263 = still 60 fps (the next vblank wasn't missed).
        uint32_t vlines = (REG_VCOUNT + 263u - 192u) % 263u;
        if (vlines > vlines_max)
            vlines_max = vlines;

        // Telemetry mailbox: rewritten every frame, read headlessly.
        gl_telemetry[GL_T_FRAME] = frame;
        gl_telemetry[GL_T_STATE] = (uint32_t)state;
        gl_telemetry[GL_T_PX] = (uint32_t)run.px;
        gl_telemetry[GL_T_PY] = (uint32_t)run.py;
        gl_telemetry[GL_T_ZX] =
            (uint32_t)(run.z_count > 0 ? run.zx[near_i] : 0);
        gl_telemetry[GL_T_ZY] =
            (uint32_t)(run.z_count > 0 ? run.zy[near_i] : 0);
        gl_telemetry[GL_T_DAWN] = run.dawn_left;
        gl_telemetry[GL_T_NIGHT] = run.night;
        gl_telemetry[GL_T_NIGHTS] = nights_survived;
        gl_telemetry[GL_T_DEATHS] = deaths;
        gl_telemetry[GL_T_SEED] = run.seed;
        gl_telemetry[GL_T_DIST] = (uint32_t)near_d;
        gl_telemetry[GL_T_RUNFRAME] = run.run_frame;
        gl_telemetry[GL_T_ZALIVE] = run.z_count;
        gl_telemetry[GL_T_ZTOTAL] = run.wave_total;
        gl_telemetry[GL_T_SHOVECD] = run.shove_cd;
        gl_telemetry[GL_T_SHOVES] = shoves;
        gl_telemetry[GL_T_NSTUN] = near_stun;
        gl_telemetry[GL_T_VLINES] = vlines;
        gl_telemetry[GL_T_VLMAX] = vlines_max;
    }

    return 0;
}
