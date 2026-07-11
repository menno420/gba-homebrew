// Gloamline — arc slice 5: BARRICADES (on the slice-4 shove + waves).
//
// The concept doc's next feature cut (docs/concepts/gloamline-concept.md
// slice order): BARRICADES. B plants a barricade at the lamplighter's
// feet for 1 plank — or, if an intact one is within reach, repairs it to
// full instead (never wasting a plank on a full one). The dead cannot
// walk INTO a barricade's radius: a blocked step chews the barricade for
// exactly 1 hp instead, and at hp 0 it breaches (slot free again). The
// player is never blocked (can't seal themselves in, by construction)
// and a blocked Shambler always chews (a wall is a timer, never a
// permanent wall-out) — both invariants host-proven in check-gloam.py.
// Resource: GL_PLANK_STOCK planks per run + gl_planks_at_dawn() per
// dawn; the concept's between-nights scavenge interlude (NEXT slice)
// becomes the real plank source. Barricades persist across nights
// within a run. Everything else is slice 4: night N sends a
// deterministic WAVE (pure schedule), SHOVE (A) is the pressure valve,
// top screen = the yard, bottom = watch-map (zombie dots + barricade
// marks + dawn bar), contact = death -> card -> instant restart. All
// game rules live in the pure layer (gl_sim.h/.c, mirrored by
// tools/check-gloam.py); this file is input, state machine, rendering,
// and the telemetry mailbox.
//
// GL_STRESS (make GL_STRESS=1 — CI perf proof, NEVER shipped): night 1
// spawns the full 24-Shambler cap at frame 0, PRE-PLACES the full
// 8-barricade ring around the player start (so the crowd chews and the
// block/draw paths all run), and contact does not kill, so an idle
// headless run measures the honest worst-case frame cost via the
// GL_T_VLINES / GL_T_VLMAX scanline telemetry. Every sim/render code
// path is identical to the shipped build.
//
// Determinism: seed = frame counter latched at the START press that begins
// a run (printed on the HUD and cards, so any human run is reproducible);
// scripted CI presses START on a fixed frame, so the whole run is a pure
// function of the input script. No wall clock, no runtime RNG.
//
// Telemetry mailbox (the gl_audio_hook concept ported to NDS): 32 u32
// words at the exported symbol `gl_telemetry`, rewritten every frame, so
// headless proofs (tools/nds-headless-check.py --elf/--watch) can assert
// game state numerically. Layout below at GL_T_*. Slots 0-23 keep their
// slice-3/4 meanings EXACTLY (the pinned CI asserts read them); slice 5
// appends 24-31 (planks + barricade state). On a multi-zombie night the
// ZX/ZY/DIST/NSTUN slots describe the NEAREST Shambler (identical to
// slice 3 whenever one zombie is up); BX/BY/BHP likewise describe the
// intact barricade nearest the player.
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
#define GL_T_PLANKS 24  // planks in pocket
#define GL_T_BARR 25    // intact barricades on the yard
#define GL_T_BHP 26     // hp of the intact barricade nearest the player
#define GL_T_BREACH 27  // barricades breached (chewed to 0) this power-on
#define GL_T_PLACES 28  // barricades PLACED this power-on
#define GL_T_REPAIRS 29 // barricades REPAIRED this power-on
#define GL_T_BX 30      // nearest intact barricade x, 8.8 fixed (0 = none)
#define GL_T_BY 31      // nearest intact barricade y

volatile uint32_t gl_telemetry[32];

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

// Barricade = crossed moor-planks on two posts (indices a-c = 10-12:
// plank body, plank top-light, dark outline). Vertically asymmetric on
// purpose: a half-broken barricade renders v-flipped (splintered).
static const char BARRICADE_ART[16][17] = {
    "................",
    "..cc........cc..",
    ".cbbc......cbbc.",
    "cbaabbbbbbbbaabc",
    "cbaaaaaaaaaaaabc",
    ".caaacaaaacaaac.",
    "..cc..c..c..cc..",
    "..cc........cc..",
    "cbbabbbbbbbbabbc",
    "caaaaaaaaaaaaaac",
    ".ccacc.cc.ccacc.",
    "..ca........ac..",
    "..ca........ac..",
    "..ca........ac..",
    "..cc........cc..",
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
                        // '.' = transparent, '0'-'9' = 0-9, 'a'+ = 10+
                        u16 nib = (c == '.') ? 0
                                : (c >= 'a') ? (u16)(c - 'a' + 10)
                                : (u16)(c - '0');
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
    SPRITE_PALETTE[10] = RGB15(16, 10, 4);   // barricade plank body
    SPRITE_PALETTE[11] = RGB15(23, 16, 8);   // barricade plank top-light
    SPRITE_PALETTE[12] = RGB15(7, 4, 2);     // barricade dark outline
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
    // Barricades (slice 5): fixed slots, hp 0 = free/breached. They
    // persist across nights within a run; a fresh run clears them.
    int32_t bx[GL_BARRICADE_CAP], by[GL_BARRICADE_CAP];
    uint32_t bhp[GL_BARRICADE_CAP];
    uint32_t planks;
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
    // Barricades deliberately NOT reset: they persist across nights.
#ifdef GL_STRESS
    // Perf build only: pre-place the full ring so the crowd chews and
    // every block/draw path is on the measured frame.
    {
        static const int8_t ring[GL_BARRICADE_CAP][2] = {
            {24, 0}, {-24, 0}, {0, 24}, {0, -24},
            {24, 24}, {24, -24}, {-24, 24}, {-24, -24},
        };
        for (int i = 0; i < GL_BARRICADE_CAP; i++)
        {
            run->bx[i] = run->px + ring[i][0] * GL_ONE;
            run->by[i] = run->py + ring[i][1] * GL_ONE;
            run->bhp[i] = GL_BARRICADE_HP;
        }
    }
#endif
    spawn_due(run);                  // index 0 spawns at frame 0, as slice 3
}

static void start_run(Run *run, uint32_t seed)
{
    run->seed = seed;
    run->night = 1;
    run->planks = GL_PLANK_STOCK;
    for (int i = 0; i < GL_BARRICADE_CAP; i++)
        run->bhp[i] = 0;             // fresh run: bare yard
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
    printf("\x1b[14;6Hbarricade: B (planks)\n");
    printf("\x1b[15;6Hdon't get touched\n");
    printf("\x1b[16;6Hsurvive to dawn\n");
    printf("\x1b[18;9HPRESS START\n");
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
    printf("\x1b[1;1HSEED %lu NTS %lu SHV %c PK %lu\x1b[K",
           (unsigned long)run->seed, (unsigned long)nights,
           run->shove_cd == 0 ? '+' : '.', (unsigned long)run->planks);
}

// --- bottom-screen watch-map -------------------------------------------------------
// Erase-then-redraw marks: the player + EVERY zombie + every intact
// barricade, so the map is the yard radar the concept promises. prev
// arrays remember last frame's cells.
static int map_prev_row[1 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP];
static int map_prev_col[1 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP];
static int map_prev_n = 0;

static void draw_watch_map_frame(void)
{
    consoleSelect(&bottom_console);
    consoleClear();
    printf("\x1b[0;1HWATCH-MAP");
    printf("\x1b[2;1HP you  Z the dead  # wall");
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

    for (int i = 0; i < GL_BARRICADE_CAP; i++)
    {
        if (run->bhp[i] == 0)
            continue;
        int b_row = map_row_of(run->by[i]), b_col = map_col_of(run->bx[i]);
        printf("\x1b[%d;%dH#", b_row, b_col);
        map_prev_row[map_prev_n] = b_row;
        map_prev_col[map_prev_n++] = b_col;
    }
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
    u16 *barricade_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16,
                                        SpriteColorFormat_16Color);
    load_sprite_gfx(player_gfx, PLAYER_ART);
    load_sprite_gfx(shambler_gfx, SHAMBLER_ART);
    load_sprite_gfx(barricade_gfx, BARRICADE_ART);

    Run run = {0};
    run.dawn_left = GL_NIGHT_FRAMES;         // title-screen dawn bar: empty
    int state = STATE_TITLE;
    uint32_t frame = 0;
    uint32_t deaths = 0;
    uint32_t nights_survived = 0;
    uint32_t shoves = 0;           // shoves CONNECTED this power-on
    uint32_t places = 0;           // barricades placed this power-on
    uint32_t repairs = 0;          // barricades repaired this power-on
    uint32_t breaches = 0;         // barricades chewed to 0 this power-on
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

            // The barricade verb (B): repair the intact barricade in
            // reach (if it needs it), else place a new one at the
            // lamplighter's feet — 1 plank either way. No cooldown: the
            // plank pocket is the limiter.
            if ((down & KEY_B) && run.planks > 0)
            {
                int32_t best_b = INT32_MAX;
                int near_b = -1;
                for (int i = 0; i < GL_BARRICADE_CAP; i++)
                {
                    if (run.bhp[i] == 0)
                        continue;
                    int32_t d = gl_chebyshev(run.px, run.py,
                                             run.bx[i], run.by[i]);
                    if (d < best_b) { best_b = d; near_b = i; }
                }
                if (near_b >= 0 && best_b <= GL_BARRICADE_RANGE)
                {
                    if (run.bhp[near_b] < GL_BARRICADE_HP)
                    {                        // repair to full, 1 plank
                        run.bhp[near_b] = GL_BARRICADE_HP;
                        run.planks--;
                        repairs++;
                    }                        // full one in reach: no waste
                }
                else
                {
                    for (int i = 0; i < GL_BARRICADE_CAP; i++)
                        if (run.bhp[i] == 0)
                        {                    // place in the first free slot
                            run.bx[i] = run.px;
                            run.by[i] = run.py;
                            run.bhp[i] = GL_BARRICADE_HP;
                            run.planks--;
                            places++;
                            break;
                        }
                }
            }

            for (uint32_t i = 0; i < run.z_count; i++)
            {
                if (run.zstun[i] > 0)
                {
                    run.zstun[i]--;          // stunned: no step this frame
                    continue;
                }
                // Propose the pure chase step; an intact barricade blocks
                // any step that would ENTER its radius — the blocked
                // attempt chews it for exactly 1 hp instead (hp 0 =
                // breached, slot free). First blocking slot wins:
                // deterministic. A stagger frame proposes no move, so it
                // neither enters nor chews.
                int32_t nx = run.zx[i], ny = run.zy[i];
                gl_shambler_step(&nx, &ny, run.px, run.py,
                                 i /* zombie_id */, run.run_frame);
                int blocker = -1;
                for (int j = 0; j < GL_BARRICADE_CAP; j++)
                    if (run.bhp[j] > 0
                        && gl_barricade_blocks(run.bx[j], run.by[j],
                                               run.zx[i], run.zy[i], nx, ny))
                    {
                        blocker = j;
                        break;
                    }
                if (blocker >= 0)
                {
                    if (--run.bhp[blocker] == 0)
                        breaches++;
                }
                else
                {
                    run.zx[i] = nx;
                    run.zy[i] = ny;
                }
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
                oamClear(&oamMain, 0, 1 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP);
                draw_death_card(&run, deaths);
            }
            else if (--run.dawn_left == 0)
            {
                nights_survived = run.night;
                state = STATE_DAWN;
                oamClear(&oamMain, 0, 1 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP);
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
                run.planks = gl_planks_at_dawn(run.planks);
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
            for (int i = 0; i < GL_BARRICADE_CAP; i++)
            {
                int oam_id = 1 + GL_ZOMBIE_CAP + i;
                if (run.bhp[i] == 0)
                {
                    oamSetHidden(&oamMain, oam_id, true);
                    continue;
                }
                oamSet(&oamMain, oam_id,
                       run.bx[i] / GL_ONE - 8, run.by[i] / GL_ONE - 8,
                       1 /* under player + dead */, 0, SpriteSize_16x16,
                       SpriteColorFormat_16Color, barricade_gfx, -1, false,
                       false,
                       run.bhp[i] <= GL_BARRICADE_HP / 2
                       /* vflip = splintered: half hp or less */,
                       false, false);
            }
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

        // Intact barricade nearest the player (mirrors the ZX/ZY idiom).
        uint32_t barr_up = 0;
        int nearb_i = -1;
        int32_t nearb_d = INT32_MAX;
        for (int i = 0; i < GL_BARRICADE_CAP; i++)
        {
            if (run.bhp[i] == 0)
                continue;
            barr_up++;
            int32_t d = gl_chebyshev(run.px, run.py, run.bx[i], run.by[i]);
            if (d < nearb_d) { nearb_d = d; nearb_i = i; }
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
        gl_telemetry[GL_T_PLANKS] = run.planks;
        gl_telemetry[GL_T_BARR] = barr_up;
        gl_telemetry[GL_T_BHP] = nearb_i >= 0 ? run.bhp[nearb_i] : 0;
        gl_telemetry[GL_T_BREACH] = breaches;
        gl_telemetry[GL_T_PLACES] = places;
        gl_telemetry[GL_T_REPAIRS] = repairs;
        gl_telemetry[GL_T_BX] = (uint32_t)(nearb_i >= 0 ? run.bx[nearb_i]
                                                        : 0);
        gl_telemetry[GL_T_BY] = (uint32_t)(nearb_i >= 0 ? run.by[nearb_i]
                                                        : 0);
    }

    return 0;
}
