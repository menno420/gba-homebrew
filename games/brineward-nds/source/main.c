// Brineward — arc slice 4: PORT + UPGRADES (roadmap item 2 on the
// slice-3 loot/gold economy).
//
// The concept doc's skeleton cut (docs/concepts/brineward-concept.md):
// one open-water screen off the Graywake breakwater (top screen), momentum
// sailing (LEFT/RIGHT turn, UP/DOWN through battle/half/full sail — the
// ship always makes way), ONE enemy sloop on an intercept-and-circle sail
// AI, L/R broadside batteries with reload + range falloff on BOTH ships,
// hull bars, sink-or-be-sunk -> card -> START instant restart. Bottom
// screen = the Graywake ledger (ship status + gold). Slice 3 added the
// loot loop (flotsam scoop, hold cap, pier banking, sink-forfeits-hold).
// Slice 4 gives banked gold its purpose: press A alongside the same pier
// (from the salvage water — no shopping under fire) and the GRAYWAKE
// PORT menu opens: three persistent upgrade tracks (hull / cannons /
// sails, tiers I/II/III) plus paid repairs. Upgrades are NEVER lost;
// hull damage now CARRIES when you put out again from salvage (repair is
// the fix — coming home hurt beats sinking, which still refits the sloop
// free but forfeits the hold). All game rules live in the pure layer
// (bw_sim.h/.c, mirrored by tools/check-brine.py); this file is input,
// state machine, rendering, and the telemetry mailbox.
//
// Determinism: seed = frame counter latched at the START press that
// begins a duel (printed on the HUD and cards, so any human run is
// reproducible); scripted CI presses START on a fixed frame, so the whole
// run is a pure function of the input script. No wall clock, no runtime
// RNG. The port menu freezes the water (pure state machine, no sim
// steps), so port shopping adds no determinism surface.
//
// Telemetry mailbox (Gloamline's pattern): 26 u32 words at the exported
// symbol `bw_telemetry`, rewritten every frame, so headless proofs
// (tools/nds-headless-check.py --elf/--watch) can assert game state
// numerically. Layout below at BW_T_* — slice 4 EXTENDED the mailbox
// 20 -> 26 (session-20 guard recipe: words 0-19 are pinned by the
// existing proofs; extend, never re-map).
//
// 100% original content: code, text, and the code-authored sprites.

#include <nds.h>

#include <stdio.h>

#include "bw_sim.h"

// --- telemetry mailbox -------------------------------------------------------
#define BW_T_MAGIC0 0   // 0x4252494E 'BRIN'
#define BW_T_MAGIC1 1   // 0x57415244 'WARD'
#define BW_T_FRAME 2    // global frame counter (every vblank)
#define BW_T_STATE 3    // 0 title / 1 duel / 2 sunk / 3 salvage
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
// slice 3 (extension; words 0-15 stay pinned)
#define BW_T_HOLD 16    // crates aboard, 0..BW_HOLD_CAP
#define BW_T_HOLDGOLD 17 // unbanked gold the hold is worth
#define BW_T_GOLD 18    // gold banked at Graywake (safe forever)
#define BW_T_LOOT 19    // crates afloat right now
// slice 4 (extension; words 0-19 stay pinned)
#define BW_T_UPHULL 20  // hull upgrade tier, 0..2
#define BW_T_UPCANNON 21 // cannon upgrade tier, 0..2
#define BW_T_UPSAIL 22  // sail upgrade tier, 0..2
#define BW_T_PORTROW 23 // port menu cursor row (meaningful in state 4)
#define BW_T_HULLMAX 24 // player max hull at the current hull tier
#define BW_T_RELOADL 25 // player PORT battery frames-to-ready

volatile uint32_t bw_telemetry[26];

enum
{
    STATE_TITLE = 0,
    STATE_DUEL = 1,
    STATE_SUNK = 2,
    STATE_SALVAGE = 3,   // slice 3: the win state is live water, not a card
    STATE_PORT = 4,      // slice 4: the Graywake port menu (water frozen)
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

// Flotsam crate, 8x8: 9 wet planking, 4 gold glint (shared accent).
static const char CRATE_ART[8][9] = {
    "........",
    ".999999.",
    ".944449.",
    ".949949.",
    ".949949.",
    ".944449.",
    ".999999.",
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
    SPRITE_PALETTE[9] = RGB15(14, 9, 4);     // wet crate planking
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
    uint32_t gold;                       // banked at Graywake — safe forever
    int32_t up_hull;                     // upgrade tiers — bought with
    int32_t up_cannon;                   //   banked gold, NEVER lost
    int32_t up_sail;                     //   (concept-doc rule)
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

// The Graywake pier (BW_PIER_X/Y = 128,172 px -> console col 16, row 21):
// lie alongside to bank the hold.
static void draw_pier(void)
{
    consoleSelect(&top_console);
    printf("\x1b[21;12H=######=");
    printf("\x1b[22;12HGRAYWAKE");
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
    printf("\x1b[14;4Hsink her, scoop the crates,\n");
    printf("\x1b[15;4Hbank gold at the pier\n");
    printf("\x1b[17;9HPRESS START\n");
}

static void draw_sunk_card(const Score *sc, const BwDuel *d)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[6;7HTHE BRINE TAKES\n");
    printf("\x1b[7;9HYOUR SLOOP\n");
    printf("\x1b[10;7Hseed %lu", (unsigned long)sc->seed);
    printf("\x1b[11;7Hships sunk %lu", (unsigned long)sc->wins);
    printf("\x1b[12;7Htimes sunk %lu", (unsigned long)sc->sinks);
    printf("\x1b[13;7Hhold lost %d crates %3dg",
           (int)d->hold, (int)d->hold_gold);
    printf("\x1b[14;7Hbanked gold safe %lug",
           (unsigned long)sc->gold);
    printf("\x1b[17;4HPRESS START: put out again\n");
}

static const char *const TRIM_NAME[3] = { "BTL", "HLF", "FUL" };

// Render-only bank flash: how many frames the "BANKED" line still shows.
static int bank_flash;
static uint32_t bank_amount;

static void draw_hud_row0(const BwDuel *d)
{
    printf("\x1b[0;1HHULL %3d  L%2d R%2d  %s\x1b[K",
           (int)d->player.hull,
           (int)((d->player.reload_l + 9) / 10),
           (int)((d->player.reload_r + 9) / 10),
           TRIM_NAME[d->player.trim]);
}

static void draw_hud(const BwDuel *d, const Score *sc)
{
    consoleSelect(&top_console);
    draw_hud_row0(d);
    if (bank_flash > 0)
        printf("\x1b[1;1HBANKED %lug  GOLD %lu\x1b[K",
               (unsigned long)bank_amount, (unsigned long)sc->gold);
    else
        printf("\x1b[1;1HFOE %3d  SEED %lu\x1b[K",
               (int)d->enemy.hull, (unsigned long)sc->seed);
}

static void draw_salvage_hud(const BwDuel *d, const Score *sc)
{
    consoleSelect(&top_console);
    draw_hud_row0(d);
    if (bank_flash > 0)
        printf("\x1b[1;1HBANKED %lug  GOLD %lu\x1b[K",
               (unsigned long)bank_amount, (unsigned long)sc->gold);
    else
        printf("\x1b[1;1HSALVAGE hold %d/%d %3dg >PIER\x1b[K",
               (int)d->hold, BW_HOLD_CAP, (int)d->hold_gold);
}

// --- the Graywake port menu (slice 4) ----------------------------------------
// Pure render of duel tiers + banked gold; every rule (prices, refusals,
// repair math) lives in bw_port_buy — this only draws what it will say.
static const char *const TIER_NAME[BW_UP_TIERS] = { "I  ", "II ", "III" };

static void draw_port_row(int row, int cursor, const char *label,
                          int32_t tier, const char *effect, int32_t cost,
                          int maxed)
{
    printf("\x1b[%d;3H%c %s ", 9 + 2 * row, cursor == row ? '>' : ' ',
           label);
    if (maxed)
        printf("%s %-9s   MAX\x1b[K", TIER_NAME[tier], effect);
    else
        printf("%s %-9s %3dg\x1b[K", TIER_NAME[tier + 1], effect,
               (int)cost);
}

static void draw_port(const BwDuel *d, const Score *sc, int row)
{
    static char fx[3][10];
    consoleSelect(&top_console);
    printf("\x1b[3;9HGRAYWAKE PORT");
    printf("\x1b[5;7HGOLD BANKED %lug\x1b[K", (unsigned long)sc->gold);

    int hull_maxed = d->up_hull >= BW_UP_TIERS - 1;
    int can_maxed = d->up_cannon >= BW_UP_TIERS - 1;
    int sail_maxed = d->up_sail >= BW_UP_TIERS - 1;
    snprintf(fx[0], sizeof fx[0], "%d hp",
             (int)bw_up_hull_max(hull_maxed ? d->up_hull : d->up_hull + 1));
    snprintf(fx[1], sizeof fx[1], "rld %d",
             (int)bw_up_reload(can_maxed ? d->up_cannon : d->up_cannon + 1));
    snprintf(fx[2], sizeof fx[2], "swifter");
    draw_port_row(BW_PORT_ROW_HULL, row, "HULL   ", d->up_hull,
                  fx[0], bw_up_price(hull_maxed ? d->up_hull
                                                : d->up_hull + 1),
                  hull_maxed);
    draw_port_row(BW_PORT_ROW_CANNON, row, "CANNONS", d->up_cannon,
                  fx[1], bw_up_price(can_maxed ? d->up_cannon
                                               : d->up_cannon + 1),
                  can_maxed);
    draw_port_row(BW_PORT_ROW_SAIL, row, "SAILS  ", d->up_sail,
                  fx[2], bw_up_price(sail_maxed ? d->up_sail
                                                : d->up_sail + 1),
                  sail_maxed);

    int32_t rcost = bw_repair_cost(d);
    printf("\x1b[%d;3H%c REPAIR  ", 9 + 2 * BW_PORT_ROW_REPAIR,
           row == BW_PORT_ROW_REPAIR ? '>' : ' ');
    if (rcost > 0)
        printf("hull %d>%d %3dg\x1b[K", (int)d->player.hull,
               (int)bw_up_hull_max(d->up_hull), (int)rcost);
    else
        printf("hull sound\x1b[K");

    printf("\x1b[18;3HUP/DOWN pick   A buy");
    printf("\x1b[19;3HSTART: back to the water");
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
    // render-only clamp: a cannon-tier purchase can briefly leave a
    // mid-count reload above the NEW (shorter) max — never overdraw
    int filled = value * 20 / max;
    if (filled < 0)
        filled = 0;
    if (filled > 20)
        filled = 20;
    printf("\x1b[%d;1H%s [", row, label);
    for (int i = 0; i < 20; i++)
        putchar(i < filled ? '=' : '-');
    printf("] %3d", value);
}

static void draw_status(const BwDuel *d, const Score *sc, int state)
{
    // Bars scale to the UPGRADED maxima (slice 4) — tier 0 = the old
    // constants, so the slice-2/3 look is unchanged until a purchase.
    int32_t hull_max = bw_up_hull_max(d->up_hull);
    int32_t reload_max = bw_up_reload(d->up_cannon);
    consoleSelect(&bottom_console);
    draw_bar(4, "HULL", d->player.hull, hull_max);
    draw_bar(6, "PORT", reload_max - d->player.reload_l, reload_max);
    draw_bar(8, "STBD", reload_max - d->player.reload_r, reload_max);
    printf("\x1b[10;1HTRIM %s   %s\x1b[K", TRIM_NAME[d->player.trim],
           state == STATE_DUEL ? "at sea " :
           state == STATE_SUNK ? "SUNK   " :
           state == STATE_SALVAGE ? "SALVAGE" : "in port");
    printf("\x1b[12;1Hsunk %lu   lost %lu\x1b[K",
           (unsigned long)sc->wins, (unsigned long)sc->sinks);
    printf("\x1b[13;1Hhold %d/%d crates %3dg\x1b[K",
           (int)d->hold, BW_HOLD_CAP, (int)d->hold_gold);
    printf("\x1b[14;1HGOLD BANKED %lug\x1b[K", (unsigned long)sc->gold);
    draw_bar(17, "HULL", d->enemy.hull, BW_HULL_MAX);
    printf("\x1b[19;1Hrange %3d yd\x1b[K",
           (int)(bw_chebyshev(d->player.x, d->player.y,
                              d->enemy.x, d->enemy.y) / BW_ONE));
}

// --- sprites ---------------------------------------------------------------------
#define OAM_PLAYER 0
#define OAM_ENEMY 1
#define OAM_BALL0 2
#define OAM_LOOT0 (OAM_BALL0 + BW_MAX_BALLS)

static void draw_ships_and_balls(const BwDuel *d,
                                 u16 *player_gfx, u16 *enemy_gfx,
                                 u16 *ball_gfx, u16 *crate_gfx,
                                 int state, uint32_t frame)
{
    // The survivor sails the salvage water; the sunk ship is gone.
    // NOTE: an OAM entry in rotate/scale mode has NO hide bit (attr0 bit
    // 9 becomes size-double), so a hidden ship must be set as a REGULAR
    // sprite with hide=true — passing hide to an affine oamSet does
    // nothing (verified headlessly: both ships ghosted over the title).
    bool water_live = state == STATE_DUEL || state == STATE_SALVAGE;
    bool player_shown = water_live;
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
               !(ball->live && water_live), false, false, false);
    }
    for (int i = 0; i < BW_MAX_LOOT; i++)
    {
        // Crates bob on the swell: a 1 px render-only wobble off the
        // frame counter (the sim position never moves — watch-map rule).
        const BwLoot *c = &d->loot[i];
        int bob = (int)(((frame >> 4) + (uint32_t)i) & 1u);
        oamSet(&oamMain, OAM_LOOT0 + i,
               c->x / BW_ONE - 4, c->y / BW_ONE - 4 + bob,
               1, 0, SpriteSize_8x8, SpriteColorFormat_16Color,
               crate_gfx, -1, false,
               !(c->live && water_live), false, false, false);
    }
}

// Begin a duel: latch the frame-counter seed, reset the sim, and re-inject
// whatever the score says survives the transition — a hold carried out of
// salvage water rides along (a sunk hold was forfeited: nothing carried),
// the bought upgrade tiers ALWAYS ride (never lost), and slice 4 carries
// the HULL too: putting out from salvage keeps the damage (repair at the
// port is the fix), while sinking or a fresh power-on refits free
// (carry_hull <= 0 = full at the current hull tier — the concept's
// "respawn at port with full (repaired) hull").
static void begin_duel(BwDuel *duel, Score *sc, uint32_t frame,
                       int32_t carry_hold, int32_t carry_gold,
                       int32_t carry_hull)
{
    sc->seed = frame;
    bw_duel_init(duel, sc->seed);
    duel->hold = carry_hold;
    duel->hold_gold = carry_gold;
    duel->up_hull = sc->up_hull;
    duel->up_cannon = sc->up_cannon;
    duel->up_sail = sc->up_sail;
    duel->player.hull = carry_hull > 0 ? carry_hull
                                       : bw_up_hull_max(sc->up_hull);
    consoleSelect(&top_console);
    consoleClear();
    draw_swell();
    draw_pier();
    draw_status_frame();
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
    u16 *crate_gfx = oamAllocateGfx(&oamMain, SpriteSize_8x8,
                                    SpriteColorFormat_16Color);
    load_sprite_gfx(player_gfx, &PLAYER_SHIP_ART[0][0], 16, 16, 17);
    load_sprite_gfx(enemy_gfx, &ENEMY_SHIP_ART[0][0], 16, 16, 17);
    load_sprite_gfx(ball_gfx, &BALL_ART[0][0], 8, 8, 9);
    load_sprite_gfx(crate_gfx, &CRATE_ART[0][0], 8, 8, 9);

    BwDuel duel = {0};
    bw_duel_init(&duel, 0);              // idle telemetry before first START
    Score score = {0};
    int state = STATE_TITLE;
    int port_row = 0;              // Graywake port menu cursor (slice 4)
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

        if (bank_flash > 0)
            bank_flash--;

        switch (state)
        {
        case STATE_TITLE:
        case STATE_SUNK:
            if (start)                   // fresh sail: no hold carried (a
            {                            // sunk hold was forfeited), free
                begin_duel(&duel, &score, frame, 0, 0, 0);  // full refit
                state = STATE_DUEL;
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
            // A hold carried into the fight can still be banked mid-duel:
            // running for the pier IS the "turn for home" choice.
            uint32_t banked = (uint32_t)bw_dock_step(&duel);
            if (banked > 0)
            {
                score.gold += banked;
                bank_flash = 120;
                bank_amount = banked;
            }

            if (duel.over == BW_DUEL_PLAYER_SUNK)
            {
                score.sinks++;           // the hold goes down with the ship
                state = STATE_SUNK;
                draw_sunk_card(&score, &duel);
            }
            else if (duel.over == BW_DUEL_ENEMY_SUNK)
            {
                score.wins++;            // flotsam is up: salvage the wreck
                state = STATE_SALVAGE;
                draw_salvage_hud(&duel, &score);
            }
            else
            {
                draw_hud(&duel, &score);
            }
            break;
        }

        case STATE_SALVAGE:
        {
            if (start)                   // put out again WITH the hold —
            {                            // and the dents (slice 4)
                begin_duel(&duel, &score, frame,
                           duel.hold, duel.hold_gold, duel.player.hull);
                state = STATE_DUEL;
                break;
            }
            // A alongside the pier opens the Graywake port (slice 4).
            // Only from the salvage water: no shopping under fire — the
            // mid-duel berth still BANKS (turn for home), nothing more.
            if (pad_seen_idle && (down & KEY_A)
                && bw_chebyshev(duel.player.x, duel.player.y,
                                BW_PIER_X, BW_PIER_Y) < BW_DOCK_RANGE)
            {
                state = STATE_PORT;
                port_row = 0;
                consoleSelect(&top_console);
                consoleClear();
                draw_port(&duel, &score, port_row);
                break;
            }
            BwInputs in = {
                .turn = ((held & KEY_RIGHT) ? 1 : 0)
                        - ((held & KEY_LEFT) ? 1 : 0),
                .trim_delta = ((down & KEY_UP) ? 1 : 0)
                              - ((down & KEY_DOWN) ? 1 : 0),
                .fire_l = 0,             // batteries stay cold: no foe
                .fire_r = 0,
            };
            bw_salvage_step(&duel, &in);
            uint32_t banked = (uint32_t)bw_dock_step(&duel);
            if (banked > 0)
            {
                score.gold += banked;
                bank_flash = 120;
                bank_amount = banked;
            }

            if (duel.over == BW_DUEL_PLAYER_SUNK)
            {
                score.sinks++;           // a lingering rake found its mark
                state = STATE_SUNK;
                draw_sunk_card(&score, &duel);
            }
            else
            {
                draw_salvage_hud(&duel, &score);
            }
            break;
        }

        case STATE_PORT:
        {
            // The water is FROZEN: no sim steps, a pure menu machine.
            // UP/DOWN pick a row, A buys (bw_port_buy owns every rule —
            // a refusal changes nothing), START/B walk back to the water.
            if (down & KEY_UP)
                port_row = port_row > 0 ? port_row - 1 : 0;
            if (down & KEY_DOWN)
                port_row = port_row < BW_PORT_ROWS - 1 ? port_row + 1
                                                       : BW_PORT_ROWS - 1;
            if (down & KEY_A)
            {
                int32_t spent = bw_port_buy(&duel, port_row,
                                            (int32_t)score.gold);
                if (spent > 0)
                {
                    score.gold -= (uint32_t)spent;
                    score.up_hull = duel.up_hull;      // tiers are score:
                    score.up_cannon = duel.up_cannon;  // bought once,
                    score.up_sail = duel.up_sail;      // kept forever
                }
            }
            if (start || (pad_seen_idle && (down & KEY_B)))
            {
                state = STATE_SALVAGE;   // back to the berth, water live
                consoleSelect(&top_console);
                consoleClear();
                draw_swell();
                draw_pier();
                draw_salvage_hud(&duel, &score);
                break;
            }
            draw_port(&duel, &score, port_row);
            break;
        }
        }

        draw_ships_and_balls(&duel, player_gfx, enemy_gfx, ball_gfx,
                             crate_gfx, state, frame);
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
        bw_telemetry[BW_T_HOLD] = (uint32_t)duel.hold;
        bw_telemetry[BW_T_HOLDGOLD] = (uint32_t)duel.hold_gold;
        bw_telemetry[BW_T_GOLD] = score.gold;
        uint32_t afloat = 0;
        for (int i = 0; i < BW_MAX_LOOT; i++)
            if (duel.loot[i].live)
                afloat++;
        bw_telemetry[BW_T_LOOT] = afloat;
        bw_telemetry[BW_T_UPHULL] = (uint32_t)duel.up_hull;
        bw_telemetry[BW_T_UPCANNON] = (uint32_t)duel.up_cannon;
        bw_telemetry[BW_T_UPSAIL] = (uint32_t)duel.up_sail;
        bw_telemetry[BW_T_PORTROW] = (uint32_t)port_row;
        bw_telemetry[BW_T_HULLMAX] = (uint32_t)bw_up_hull_max(duel.up_hull);
        bw_telemetry[BW_T_RELOADL] = (uint32_t)duel.player.reload_l;
    }

    return 0;
}
