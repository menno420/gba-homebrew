// Brineward — arc slice 2: the WALKING SKELETON (first playable).
//
// The concept doc's skeleton cut (docs/concepts/brineward-concept.md):
// one open-water screen off the Graywake breakwater (top screen), momentum
// sailing (LEFT/RIGHT turn, UP/DOWN through battle/half/full sail — the
// ship always makes way), ONE enemy sloop on an intercept-and-circle sail
// AI, L/R broadside batteries with reload + range falloff on BOTH ships,
// hull bars, sink-or-be-sunk -> card -> START instant restart. Bottom
// screen = ship status v0 (hull / battery reloads / trim / score). No
// loot, no port, no monsters — the broadside duel IS the game, so it
// ships first and alone. All game rules live in the pure layer
// (bw_sim.h/.c, mirrored by tools/check-brine.py); this file is input,
// state machine, rendering, and the telemetry mailbox.
//
// Determinism: seed = frame counter latched at the START press that
// begins a duel (printed on the HUD and cards, so any human run is
// reproducible); scripted CI presses START on a fixed frame, so the whole
// run is a pure function of the input script. No wall clock, no runtime
// RNG.
//
// Telemetry mailbox (Gloamline's pattern): 16 u32 words at the exported
// symbol `bw_telemetry`, rewritten every frame, so headless proofs
// (tools/nds-headless-check.py --elf/--watch) can assert game state
// numerically. Layout below at BW_T_*.
//
// 100% original content: code, text, and the code-authored sprites.

#include <nds.h>

#include <stdio.h>

#include "bw_sim.h"

// --- telemetry mailbox -------------------------------------------------------
#define BW_T_MAGIC0 0   // 0x4252494E 'BRIN'
#define BW_T_MAGIC1 1   // 0x57415244 'WARD'
#define BW_T_FRAME 2    // global frame counter (every vblank)
#define BW_T_STATE 3    // 0 title / 1 duel / 2 sunk / 3 victory
#define BW_T_PX 4       // player x, 8.8 fixed (sprite center)
#define BW_T_PY 5       // player y
#define BW_T_PHDG 6     // player heading, 0..1023 (0 = north, clockwise)
#define BW_T_PHULL 7    // player hull, 0..100
#define BW_T_EX 8       // enemy x
#define BW_T_EY 9       // enemy y
#define BW_T_EHULL 10   // enemy hull
#define BW_T_SINKS 11   // times the player sank this power-on
#define BW_T_WINS 12    // enemy ships sunk this power-on
#define BW_T_SEED 13    // seed of the current duel
#define BW_T_DIST 14    // chebyshev player<->enemy, 8.8 fixed
#define BW_T_TRIM 15    // player sail trim, 0..2

volatile uint32_t bw_telemetry[16];

enum
{
    STATE_TITLE = 0,
    STATE_DUEL = 1,
    STATE_SUNK = 2,
    STATE_VICTORY = 3,
};

// --- original sprite art (code-authored, 4bpp, shared 16-color palette) ------
// '.' = transparent. Top-down sloops, bow pointing NORTH (heading 0);
// affine OAM matrices rotate them to the live heading.
// Player: 1 tar hull, 2 deck, 3 bone sails, 4 gold accent (stern lantern).
// Enemy: same hull, 5 weathered sails, 6 red rag at the stern.
static const char PLAYER_SHIP_ART[16][17] = {
    "................",
    ".......1........",
    ".......1........",
    "......131.......",
    "......333.......",
    ".....13331......",
    ".....13331......",
    ".....12221......",
    "....1333331.....",
    "....1333331.....",
    "....1333331.....",
    ".....13331......",
    ".....12221......",
    "......121.......",
    ".......4........",
    "................",
};

static const char ENEMY_SHIP_ART[16][17] = {
    "................",
    ".......1........",
    ".......1........",
    "......151.......",
    "......555.......",
    ".....15551......",
    ".....15551......",
    ".....12221......",
    "....1555551.....",
    "....1555551.....",
    "....1555551.....",
    ".....15551......",
    ".....12221......",
    "......161.......",
    ".......6........",
    "................",
};

// Cannonball, 8x8: 7 iron, 8 highlight.
static const char BALL_ART[8][9] = {
    "........",
    "........",
    "...77...",
    "..7877..",
    "..7777..",
    "...77...",
    "........",
    "........",
};

// Pack WxH character art into 4bpp tiles (8x8 tile raster order).
static void load_sprite_gfx(u16 *gfx, const char *art, int w, int h,
                            int stride)
{
    int out = 0;
    for (int ty = 0; ty < h / 8; ty++)
        for (int tx = 0; tx < w / 8; tx++)
            for (int y = 0; y < 8; y++)
                for (int half = 0; half < 2; half++)
                {
                    u16 v = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        char c = art[(ty * 8 + y) * stride
                                     + tx * 8 + half * 4 + i];
                        u16 nib = (c == '.') ? 0 : (u16)(c - '0');
                        v |= nib << (4 * i);
                    }
                    gfx[out++] = v;
                }
}

static void load_palette(void)
{
    SPRITE_PALETTE[1] = RGB15(4, 3, 3);      // tar hull
    SPRITE_PALETTE[2] = RGB15(14, 10, 6);    // deck planks
    SPRITE_PALETTE[3] = RGB15(29, 28, 23);   // bone sails
    SPRITE_PALETTE[4] = RGB15(31, 26, 10);   // gold stern lantern
    SPRITE_PALETTE[5] = RGB15(16, 17, 14);   // weathered enemy sails
    SPRITE_PALETTE[6] = RGB15(25, 7, 6);     // red rag
    SPRITE_PALETTE[7] = RGB15(9, 9, 11);     // iron ball
    SPRITE_PALETTE[8] = RGB15(20, 20, 23);   // ball highlight
    // slate sea on both backdrops
    BG_PALETTE[0] = RGB15(2, 5, 9);
    BG_PALETTE_SUB[0] = RGB15(2, 4, 7);
}

// --- score kept across duels (one power-on) ------------------------------------
typedef struct
{
    uint32_t seed;
    uint32_t sinks;
    uint32_t wins;
} Score;

// --- top-screen scenes -----------------------------------------------------------
static PrintConsole top_console;
static PrintConsole bottom_console;

static void draw_swell(void)
{
    consoleSelect(&top_console);
    printf("\x1b[2;0H~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~-");
    printf("\x1b[23;0H~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~-");
}

static void draw_title(void)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[5;11HBRINEWARD\n");
    printf("\x1b[7;5Ha gun-captain's debt to\n");
    printf("\x1b[8;5Hthe port of Graywake\n");
    printf("\x1b[11;4Hturn: LEFT/RIGHT\n");
    printf("\x1b[12;4Hsail trim: UP/DOWN\n");
    printf("\x1b[13;4Hbroadsides: L port, R stbd\n");
    printf("\x1b[14;4Hsink the rum-runner\n");
    printf("\x1b[17;9HPRESS START\n");
}

static void draw_sunk_card(const Score *sc)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[7;7HTHE BRINE TAKES\n");
    printf("\x1b[8;9HYOUR SLOOP\n");
    printf("\x1b[11;7Hseed %lu", (unsigned long)sc->seed);
    printf("\x1b[12;7Hships sunk %lu", (unsigned long)sc->wins);
    printf("\x1b[13;7Htimes sunk %lu", (unsigned long)sc->sinks);
    printf("\x1b[16;4HPRESS START: put out again\n");
}

static void draw_victory_card(const Score *sc)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[7;5HTHE RUM-RUNNER GOES\n");
    printf("\x1b[8;9HUNDER\n");
    printf("\x1b[11;7Hseed %lu", (unsigned long)sc->seed);
    printf("\x1b[12;7Hships sunk %lu", (unsigned long)sc->wins);
    printf("\x1b[13;7Htimes sunk %lu", (unsigned long)sc->sinks);
    printf("\x1b[16;3HPRESS START: the next sail\n");
}

static const char *const TRIM_NAME[3] = { "BTL", "HLF", "FUL" };

static void draw_hud(const BwDuel *d, const Score *sc)
{
    consoleSelect(&top_console);
    printf("\x1b[0;1HHULL %3d  L%2d R%2d  %s\x1b[K",
           (int)d->player.hull,
           (int)((d->player.reload_l + 9) / 10),
           (int)((d->player.reload_r + 9) / 10),
           TRIM_NAME[d->player.trim]);
    printf("\x1b[1;1HFOE %3d  SEED %lu\x1b[K",
           (int)d->enemy.hull, (unsigned long)sc->seed);
}

// --- bottom-screen ship status v0 ------------------------------------------------
static void draw_status_frame(void)
{
    consoleSelect(&bottom_console);
    consoleClear();
    printf("\x1b[0;1HGRAYWAKE LEDGER");
    printf("\x1b[2;1Hyour sloop");
    printf("\x1b[15;1Hthe rum-runner");
}

static void draw_bar(int row, const char *label, int value, int max)
{
    int filled = value * 20 / max;
    printf("\x1b[%d;1H%s [", row, label);
    for (int i = 0; i < 20; i++)
        putchar(i < filled ? '=' : '-');
    printf("] %3d", value);
}

static void draw_status(const BwDuel *d, const Score *sc, int state)
{
    consoleSelect(&bottom_console);
    draw_bar(4, "HULL", d->player.hull, BW_HULL_MAX);
    draw_bar(6, "PORT", BW_RELOAD_PLAYER - d->player.reload_l,
             BW_RELOAD_PLAYER);
    draw_bar(8, "STBD", BW_RELOAD_PLAYER - d->player.reload_r,
             BW_RELOAD_PLAYER);
    printf("\x1b[10;1HTRIM %s   %s\x1b[K", TRIM_NAME[d->player.trim],
           state == STATE_DUEL ? "at sea " :
           state == STATE_SUNK ? "SUNK   " :
           state == STATE_VICTORY ? "VICTORY" : "in port");
    printf("\x1b[12;1Hsunk %lu   lost %lu\x1b[K",
           (unsigned long)sc->wins, (unsigned long)sc->sinks);
    draw_bar(17, "HULL", d->enemy.hull, BW_HULL_MAX);
    printf("\x1b[19;1Hrange %3d yd\x1b[K",
           (int)(bw_chebyshev(d->player.x, d->player.y,
                              d->enemy.x, d->enemy.y) / BW_ONE));
}

// --- sprites ---------------------------------------------------------------------
#define OAM_PLAYER 0
#define OAM_ENEMY 1
#define OAM_BALL0 2

static void draw_ships_and_balls(const BwDuel *d,
                                 u16 *player_gfx, u16 *enemy_gfx,
                                 u16 *ball_gfx, int state)
{
    // The survivor floats on the outcome cards; the sunk ship is gone.
    // NOTE: an OAM entry in rotate/scale mode has NO hide bit (attr0 bit
    // 9 becomes size-double), so a hidden ship must be set as a REGULAR
    // sprite with hide=true — passing hide to an affine oamSet does
    // nothing (verified headlessly: both ships ghosted over the title).
    bool player_shown = state == STATE_DUEL || state == STATE_VICTORY;
    bool enemy_shown = state == STATE_DUEL || state == STATE_SUNK;

    // libnds angle: 32768 units per circle, counter-clockwise; our
    // heading is 1024 per circle, clockwise from north -> negate.
    oamRotateScale(&oamMain, 0, -(d->player.heading * 32),
                   intToFixed(1, 8), intToFixed(1, 8));
    oamSet(&oamMain, OAM_PLAYER,
           d->player.x / BW_ONE - 16, d->player.y / BW_ONE - 16,
           0, 0, SpriteSize_16x16, SpriteColorFormat_16Color,
           player_gfx, player_shown ? 0 : -1, player_shown /* sizeDouble */,
           !player_shown, false, false, false);
    oamRotateScale(&oamMain, 1, -(d->enemy.heading * 32),
                   intToFixed(1, 8), intToFixed(1, 8));
    oamSet(&oamMain, OAM_ENEMY,
           d->enemy.x / BW_ONE - 16, d->enemy.y / BW_ONE - 16,
           0, 0, SpriteSize_16x16, SpriteColorFormat_16Color,
           enemy_gfx, enemy_shown ? 1 : -1, enemy_shown,
           !enemy_shown, false, false, false);
    for (int i = 0; i < BW_MAX_BALLS; i++)
    {
        const BwBall *ball = &d->balls[i];
        oamSet(&oamMain, OAM_BALL0 + i,
               ball->x / BW_ONE - 4, ball->y / BW_ONE - 4,
               0, 0, SpriteSize_8x8, SpriteColorFormat_16Color,
               ball_gfx, -1, false,
               !(ball->live && state == STATE_DUEL), false, false, false);
    }
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
    u16 *enemy_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16,
                                    SpriteColorFormat_16Color);
    u16 *ball_gfx = oamAllocateGfx(&oamMain, SpriteSize_8x8,
                                   SpriteColorFormat_16Color);
    load_sprite_gfx(player_gfx, &PLAYER_SHIP_ART[0][0], 16, 16, 17);
    load_sprite_gfx(enemy_gfx, &ENEMY_SHIP_ART[0][0], 16, 16, 17);
    load_sprite_gfx(ball_gfx, &BALL_ART[0][0], 8, 8, 9);

    BwDuel duel = {0};
    bw_duel_init(&duel, 0);              // idle telemetry before first START
    Score score = {0};
    int state = STATE_TITLE;
    uint32_t frame = 0;
    bool pad_seen_idle = false;    // KEYINPUT boot-trap guard (PLATFORM-LIMITS)

    draw_title();
    draw_status_frame();

    bw_telemetry[BW_T_MAGIC0] = 0x4252494Eu;   // 'BRIN'
    bw_telemetry[BW_T_MAGIC1] = 0x57415244u;   // 'WARD'

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        frame++;

        uint32_t held = keysHeld();
        uint32_t down = keysDown();
        // Only trust buttons once the pad has been seen idle: an emulator
        // whose KEYINPUT reads all-pressed (active-low zero) before its
        // first input latch must not skip the title at boot.
        if (held == 0)
            pad_seen_idle = true;
        bool start = pad_seen_idle && (down & KEY_START);

        switch (state)
        {
        case STATE_TITLE:
        case STATE_SUNK:
        case STATE_VICTORY:
            if (start)                   // latch the frame-counter seed
            {
                score.seed = frame;
                bw_duel_init(&duel, score.seed);
                state = STATE_DUEL;
                consoleSelect(&top_console);
                consoleClear();
                draw_swell();
                draw_status_frame();
            }
            break;

        case STATE_DUEL:
        {
            BwInputs in = {
                .turn = ((held & KEY_RIGHT) ? 1 : 0)
                        - ((held & KEY_LEFT) ? 1 : 0),
                .trim_delta = ((down & KEY_UP) ? 1 : 0)
                              - ((down & KEY_DOWN) ? 1 : 0),
                .fire_l = pad_seen_idle && (down & KEY_L),
                .fire_r = pad_seen_idle && (down & KEY_R),
            };
            bw_duel_step(&duel, &in);

            if (duel.over == BW_DUEL_PLAYER_SUNK)
            {
                score.sinks++;
                state = STATE_SUNK;
                draw_sunk_card(&score);
            }
            else if (duel.over == BW_DUEL_ENEMY_SUNK)
            {
                score.wins++;
                state = STATE_VICTORY;
                draw_victory_card(&score);
            }
            else
            {
                draw_hud(&duel, &score);
            }
            break;
        }
        }

        draw_ships_and_balls(&duel, player_gfx, enemy_gfx, ball_gfx, state);
        oamUpdate(&oamMain);
        draw_status(&duel, &score, state);

        // Telemetry mailbox: rewritten every frame, read headlessly.
        bw_telemetry[BW_T_FRAME] = frame;
        bw_telemetry[BW_T_STATE] = (uint32_t)state;
        bw_telemetry[BW_T_PX] = (uint32_t)duel.player.x;
        bw_telemetry[BW_T_PY] = (uint32_t)duel.player.y;
        bw_telemetry[BW_T_PHDG] = (uint32_t)duel.player.heading;
        bw_telemetry[BW_T_PHULL] = (uint32_t)duel.player.hull;
        bw_telemetry[BW_T_EX] = (uint32_t)duel.enemy.x;
        bw_telemetry[BW_T_EY] = (uint32_t)duel.enemy.y;
        bw_telemetry[BW_T_EHULL] = (uint32_t)duel.enemy.hull;
        bw_telemetry[BW_T_SINKS] = score.sinks;
        bw_telemetry[BW_T_WINS] = score.wins;
        bw_telemetry[BW_T_SEED] = score.seed;
        bw_telemetry[BW_T_DIST] =
            (uint32_t)bw_chebyshev(duel.player.x, duel.player.y,
                                   duel.enemy.x, duel.enemy.y);
        bw_telemetry[BW_T_TRIM] = (uint32_t)duel.player.trim;
    }

    return 0;
}
