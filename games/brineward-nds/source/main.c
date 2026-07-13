// Brineward — arc slice 8: SYNTHESIZED AUDIO (on the slice-7 danger
// bands + reefs).
//
// The concept doc's "synthesized audio set" ("cannon thump (noise
// burst + low sine), hull crack, splash hiss, a two-note dock chime.
// No sampled or licensed audio, ever") — the brine gets a voice, 100%
// synthesized in code on the Gloamline slice-8 proven pattern: no
// sampled or binary audio asset exists anywhere in this repo.
// Playback is the NDS hardware PSG channels (square + noise
// generators) driven by the default BlocksDS ARM7 core over the
// libnds sound FIFO; the ARM9 spends a handful of integer compares
// per frame. Two voices: (1) an AMBIENCE DRONE — the WEATHER sings in
// the rigging via the pure bw_amb_tier (calm swell / breeze / gale,
// the slice-6 wind level as the drone ladder), and the Maw's presence
// overrides it (shadow included: hearing the telegraph matters); the
// FIFO is touched ONLY when the tier flips (the channel free-runs
// between flips), and the drone is silent on the title, the sunk card
// and in port — dying silences the sea. (2) a CUE channel for
// one-shot chiptune SFX: cannon thump, splash hiss, crate scoop, hull
// crack, reef scrape, wreck, the two-note Graywake dock chime (the
// channel re-tunes mid-cue — bw_cue_freq2), the Maw surfacing, the
// bite, and your own sinking — parameters are pure bw_cue_* table
// rows, and on a frame with several events the highest cue id wins
// the channel (id order IS the priority order, mirrored). Audio feeds
// NOTHING back into the sim — it is render-like output, so every
// pre-slice-8 pin holds bit-identically. Telemetry appends slots
// 38-45 (tier, drone freq, last cue, cue count, ...) with slots 0-37
// frozen; headless CI proves the DECISION chain exactly and honestly
// cannot hear the result (owner playtest for the mix). Session-38
// guard-recipe rail: the scrape cue fires off the player-only
// groundings counter, never off enemy positions.
//
// Below this line the slice-5..7 story still applies verbatim:
//
// Brineward — arc slice 5: THE MAW (roadmap item 3 — the first sea
// monster, on the slice-4 port/upgrades economy).
//
// The concept doc's skeleton cut (docs/concepts/brineward-concept.md):
// one open-water screen off the Graywake breakwater (top screen), momentum
// sailing (LEFT/RIGHT turn, UP/DOWN through battle/half/full sail — the
// ship always makes way), ONE enemy sloop on an intercept-and-circle sail
// AI, L/R broadside batteries with reload + range falloff on BOTH ships,
// hull bars, sink-or-be-sunk -> card -> START instant restart. Bottom
// screen = the Graywake ledger (ship status + gold). Slice 3 added the
// loot loop (flotsam scoop, hold cap, pier banking, sink-forfeits-hold).
// Slice 4 gave banked gold its purpose: the GRAYWAKE PORT off the same
// pier berth — three persistent upgrade tracks plus paid repairs.
// Slice 5 puts teeth in the salvage water: linger too long over the
// wreck's blood and THE MAW stirs — a shadow rises and homes under the
// keel (the telegraph), surfaces, and lunges; a bite costs real hull.
// The batteries wake while it is up: rake it (it breaks up richer than
// any rum-runner), dodge it, run for the pier sanctuary, or put out.
// Slice 7 gives the brine its DEPTH: SELECT in the salvage water puts
// out one danger band deeper — richer crates, worse weather, heavier
// rum-runners with faster crews, and reefs that scrape the keel; the
// deepest band sailed is the charted-waters score on the cards.
// All game rules live in the pure layer (bw_sim.h/.c, mirrored by
// tools/check-brine.py); this file is input, state machine, rendering,
// and the telemetry mailbox.
//
// Determinism: seed = frame counter latched at the START press that
// begins a duel (printed on the HUD and cards, so any human run is
// reproducible); scripted CI presses START on a fixed frame, so the whole
// run is a pure function of the input script. No wall clock, no runtime
// RNG. The port menu freezes the water (pure state machine, no sim
// steps), so port shopping adds no determinism surface.
//
// Telemetry mailbox (Gloamline's pattern): 31 u32 words at the exported
// symbol `bw_telemetry`, rewritten every frame, so headless proofs
// (tools/nds-headless-check.py --elf/--watch) can assert game state
// numerically. Layout below at BW_T_* — slice 5 EXTENDED the mailbox
// 26 -> 31 (session-20 guard recipe: words 0-25 are pinned by the
// existing proofs; extend, never re-map), slice 6 31 -> 34, slice 7
// 34 -> 38, slice 8 38 -> 46.
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
// slice 5 (extension; words 0-25 stay pinned)
#define BW_T_MAWSTATE 26 // Maw state, BW_MAW_* (0 down / 1 shadow / 2 up / 3 lunge)
#define BW_T_MAWX 27    // Maw x, 8.8 fixed (meaningful while not down)
#define BW_T_MAWY 28    // Maw y
#define BW_T_MAWHULL 29 // Maw hull, 0..BW_MAW_HULL (0 = slain)
#define BW_T_MAWS 30    // Maws slain this power-on
// slice 6 (extension; words 0-30 stay pinned)
#define BW_T_WINDLVL 31 // this water's weather: 0 calm / 1 breeze / 2 gale
#define BW_T_WINDHDG 32 // wind heading (blowing TOWARD), 0..1023, rotating
#define BW_T_PSPEED 33  // player speed, units/frame (the point-of-sail word)
// slice 7 (extension; words 0-33 stay pinned)
#define BW_T_BAND 34    // this water's danger band, 0..2
#define BW_T_BEST 35    // deepest band charted this power-on (the score)
#define BW_T_REEFS 36   // rocks in this water (0 in band 0)
#define BW_T_GROUND 37  // reef scrapes taken this water
// slice 8 (extension; words 0-37 stay pinned)
#define BW_T_ATIER 38   // ambience tier the drone plays (BW_AMB_*)
#define BW_T_ACUE 39    // last one-shot cue id fired (0 = none yet)
#define BW_T_ACUES 40   // one-shot cues fired this power-on
#define BW_T_ACUEFR 41  // global frame the last cue fired on
#define BW_T_ADRONE 42  // drone frequency now, Hz (0 = drone silent)
#define BW_T_AFLIPS 43  // drone restarts/stops (tier flips) this power-on
#define BW_T_ASFXL 44   // frames left on the one-shot cue channel
#define BW_T_ACFREQ 45  // cue channel frequency now, Hz (0 = closed;
                        //   the two-note dock chime flips this mid-cue)

volatile uint32_t bw_telemetry[46];

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

// The Maw, 32x32 (slice 5) — two frames, both REGULAR sprites (the
// affine no-hide-bit trap does not apply): the SHADOW (a = deep-water
// dark, dithered rim: the telegraph under the swell) and the RISEN maw
// (b = abyssal flesh, c = bone teeth ringing an a-dark gullet).
static const char MAW_SHADOW_ART[32][33] = {
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "...........a.a.a.a.a.a..........",
    "........a.a.aaaaaaa.a.a.........",
    ".......a.aaaaaaaaaaaaa.a.a......",
    "......a.aaaaaaaaaaaaaaaaa.a.....",
    ".....a.aaaaaaaaaaaaaaaaaaa.a....",
    "....a.aaaaaaaaaaaaaaaaaaaaa.a...",
    "...a.aaaaaaaaaaaaaaaaaaaaaaa....",
    "..a.aaaaaaaaaaaaaaaaaaaaaaa.a...",
    "...aaaaaaaaaaaaaaaaaaaaaaaaa.a..",
    "..a.aaaaaaaaaaaaaaaaaaaaaaaaa...",
    "...aaaaaaaaaaaaaaaaaaaaaaaaa.a..",
    "..a.aaaaaaaaaaaaaaaaaaaaaaaaa...",
    "...aaaaaaaaaaaaaaaaaaaaaaaaa.a..",
    "..a.aaaaaaaaaaaaaaaaaaaaaaaaa...",
    "...a.aaaaaaaaaaaaaaaaaaaaaaa.a..",
    "....aaaaaaaaaaaaaaaaaaaaaaa.a...",
    "...a.aaaaaaaaaaaaaaaaaaaaa.a....",
    "....a.aaaaaaaaaaaaaaaaaaa.a.....",
    ".....a.aaaaaaaaaaaaaaaaa.a......",
    "......a.a.aaaaaaaaaaaaa.a.......",
    ".........a.a.aaaaaaa.a.a........",
    "..........a.a.a.a.a.a...........",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
};

static const char MAW_RISEN_ART[32][33] = {
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
    "..........bbbbbbbbbbbb..........",
    "........bbbbbbbbbbbbbbbb........",
    "......bbbbbbbbccbbccbbbbbb......",
    ".....bbbbbbccbbccbbccbbbbbb.....",
    "....bbbbccbbaaaaaaaaccbbbbbb....",
    "...bbbbbbcaaaaaaaaaaaacbbbbbb...",
    "...bbbccbaaaaaaaaaaaaaacbbbbb...",
    "..bbbbbcaaaaaaaaaaaaaaaacbbbbb..",
    "..bbbcbaaaaaaaaaaaaaaaaaacbbbb..",
    "..bbbccaaaaaaaaaaaaaaaaaaccbbb..",
    "..bbbbcaaaaaaaaaaaaaaaaaabcbbb..",
    "..bbbbbaaaaaaaaaaaaaaaaaabbbbb..",
    "..bbbcbaaaaaaaaaaaaaaaaaacbbbb..",
    "..bbbccaaaaaaaaaaaaaaaaaaccbbb..",
    "..bbbbccaaaaaaaaaaaaaaaabbcbbb..",
    "...bbbbccaaaaaaaaaaaaaaccbbbb...",
    "...bbbbbccaaaaaaaaaaaabbcbbbb...",
    "....bbbbbccbaaaaaaaabccbbbbb....",
    ".....bbbbbccbbccbbccbbbbbbb.....",
    "......bbbbbbcbbccbbcbbbbbb......",
    "........bbbbbbbbbbbbbbbb........",
    "..........bbbbbbbbbbbb..........",
    "................................",
    "................................",
    "................................",
    "................................",
    "................................",
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

// Reef rock, 16x16 (slice 7): d = wet rock, e = foam ring where the
// swell breaks over it (the tell the player charts by).
static const char REEF_ART[16][17] = {
    "................",
    "......e..e......",
    "....e.dddd.e....",
    "...eddddddde....",
    "..e.dddddddd.e..",
    "..eddddddddde...",
    ".e.dddddddddd.e.",
    "..dddddddddddd..",
    ".edddddddddddde.",
    "..dddddddddddd..",
    ".e.dddddddddd.e.",
    "..eddddddddde...",
    "..e.dddddddd.e..",
    "...eddddddde....",
    "....e.dddd.e....",
    "......e..e......",
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
                        // digits 0-9 as themselves; 'a'.. for 10..
                        // (the shared 16-color palette outgrew 0-9
                        // when the Maw's three colors landed)
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
    SPRITE_PALETTE[1] = RGB15(4, 3, 3);      // tar hull
    SPRITE_PALETTE[2] = RGB15(14, 10, 6);    // deck planks
    SPRITE_PALETTE[3] = RGB15(29, 28, 23);   // bone sails
    SPRITE_PALETTE[4] = RGB15(31, 26, 10);   // gold stern lantern
    SPRITE_PALETTE[5] = RGB15(16, 17, 14);   // weathered enemy sails
    SPRITE_PALETTE[6] = RGB15(25, 7, 6);     // red rag
    SPRITE_PALETTE[7] = RGB15(9, 9, 11);     // iron ball
    SPRITE_PALETTE[8] = RGB15(20, 20, 23);   // ball highlight
    SPRITE_PALETTE[9] = RGB15(14, 9, 4);     // wet crate planking
    SPRITE_PALETTE[10] = RGB15(1, 3, 5);     // the shadow / the gullet
    SPRITE_PALETTE[11] = RGB15(8, 6, 10);    // abyssal flesh
    SPRITE_PALETTE[12] = RGB15(28, 29, 26);  // bone teeth
    SPRITE_PALETTE[13] = RGB15(7, 8, 7);     // wet reef rock (slice 7)
    SPRITE_PALETTE[14] = RGB15(24, 27, 28);  // foam breaking over it
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
    uint32_t maws;                       // Maws slain this power-on (slice 5)
    uint32_t best_band;                  // deepest band charted (slice 7)
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
    printf("\x1b[16;4Hafter a win SELECT: deeper\n");
    printf("\x1b[18;9HPRESS START\n");
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
    if (sc->maws > 0)
        printf("\x1b[15;7Hmaws slain %lu", (unsigned long)sc->maws);
    printf("\x1b[16;7Hcharted: band %lu waters",
           (unsigned long)sc->best_band);
    printf("\x1b[18;4HPRESS START: put out again\n");
}

static const char *const TRIM_NAME[3] = { "BTL", "HLF", "FUL" };

// --- the weather gauge (slice 6) -----------------------------------------------
// Sailors name the wind by where it comes FROM: the sim's wind heading
// points TOWARD, so the gauge shows the opposite bearing, folded onto
// the 8-point compass. "NE~" = a breeze out of the northeast, "NE!" =
// a gale; a calm water shows "calm". Pure render of sim state.
static const char *const WIND_FROM[8] = {
    "N", "NE", "E", "SE", "S", "SW", "W", "NW",
};

static const char *wind_from_name(const BwDuel *d)
{
    int32_t from = (bw_wind_heading(d) + BW_CIRCLE / 2) & (BW_CIRCLE - 1);
    return WIND_FROM[((from + 64) & (BW_CIRCLE - 1)) >> 7];
}

static void wind_gauge(const BwDuel *d, char *out /* >= 8 bytes */)
{
    if (d->wind_level == BW_WIND_CALM)
    {
        out[0] = 'c'; out[1] = 'a'; out[2] = 'l'; out[3] = 'm';
        out[4] = 0;
        return;
    }
    const char *n = wind_from_name(d);
    int i = 0;
    while (n[i])
    {
        out[i] = n[i];
        i++;
    }
    out[i++] = d->wind_level == BW_WIND_GALE ? '!' : '~';
    out[i] = 0;
}

// Render-only bank flash: how many frames the "BANKED" line still shows.
static int bank_flash;
static uint32_t bank_amount;

static void draw_hud_row0(const BwDuel *d)
{
    char wind[8];
    wind_gauge(d, wind);                 // slice 6: read the weather
    printf("\x1b[0;1HHULL %3d  L%2d R%2d  %s %s\x1b[K",
           (int)d->player.hull,
           (int)((d->player.reload_l + 9) / 10),
           (int)((d->player.reload_r + 9) / 10),
           TRIM_NAME[d->player.trim], wind);
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
    else if (d->maw.state == BW_MAW_SHADOW)
        printf("\x1b[1;1HTHE WATER GOES DARK...\x1b[K");
    else if (d->maw.state != BW_MAW_DOWN)
        printf("\x1b[1;1HTHE MAW %3d  FIRE OR FLY\x1b[K",
               (int)d->maw.hull);
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
    printf("\x1b[3;1Hband %d water   charted %lu\x1b[K",
           (int)d->band, (unsigned long)sc->best_band);
    draw_bar(4, "HULL", d->player.hull, hull_max);
    draw_bar(6, "PORT", reload_max - d->player.reload_l, reload_max);
    draw_bar(8, "STBD", reload_max - d->player.reload_r, reload_max);
    printf("\x1b[10;1HTRIM %s   %s\x1b[K", TRIM_NAME[d->player.trim],
           state == STATE_DUEL ? "at sea " :
           state == STATE_SUNK ? "SUNK   " :
           state == STATE_SALVAGE ? "SALVAGE" : "in port");
    printf("\x1b[12;1Hsunk %lu  lost %lu  maws %lu\x1b[K",
           (unsigned long)sc->wins, (unsigned long)sc->sinks,
           (unsigned long)sc->maws);
    printf("\x1b[13;1Hhold %d/%d crates %3dg\x1b[K",
           (int)d->hold, BW_HOLD_CAP, (int)d->hold_gold);
    printf("\x1b[14;1HGOLD BANKED %lug\x1b[K", (unsigned long)sc->gold);
    draw_bar(17, "HULL", d->enemy.hull, bw_band_enemy_hull(d->band));
    printf("\x1b[19;1Hrange %3d yd\x1b[K",
           (int)(bw_chebyshev(d->player.x, d->player.y,
                              d->enemy.x, d->enemy.y) / BW_ONE));
    // slice 6: the chart table reads the sky too (pure render of the
    // seeded weather — the watch-map rule, as ever)
    if (d->wind_level == BW_WIND_CALM)
        printf("\x1b[20;1Hdead calm on the water\x1b[K");
    else
        printf("\x1b[20;1Hwind out of the %-2s  %s\x1b[K",
               wind_from_name(d),
               d->wind_level == BW_WIND_GALE ? "GALE" : "breeze");
    // slice 5: the chart table reads the water (a pure render of maw
    // state — the watch-map rule, as ever)
    if (d->maw.state == BW_MAW_SHADOW)
        printf("\x1b[21;1Hsomething big is turning\x1b[K");
    else if (d->maw.state != BW_MAW_DOWN)
        printf("\x1b[21;1HTHE MAW IS UP  hull %3d\x1b[K",
               (int)d->maw.hull);
    else if (d->maw.slain)
        printf("\x1b[21;1Hthe maw is dead planking\x1b[K");
    else
        printf("\x1b[21;1H\x1b[K");
}

// --- sprites ---------------------------------------------------------------------
#define OAM_PLAYER 0
#define OAM_ENEMY 1
#define OAM_BALL0 2
#define OAM_LOOT0 (OAM_BALL0 + BW_MAX_BALLS)
#define OAM_MAW (OAM_LOOT0 + BW_MAX_LOOT)
#define OAM_REEF0 (OAM_MAW + 1)

static void draw_ships_and_balls(const BwDuel *d,
                                 u16 *player_gfx, u16 *enemy_gfx,
                                 u16 *ball_gfx, u16 *crate_gfx,
                                 u16 *maw_shadow_gfx, u16 *maw_risen_gfx,
                                 u16 *reef_gfx,
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
    // The Maw (slice 5): the SHADOW while it homes (the telegraph the
    // player is meant to watch), the RISEN body while it is up. A
    // REGULAR sprite on purpose — the affine no-hide-bit trap does not
    // apply, so plain hide works (session-20 guard recipe).
    bool maw_shown = water_live && d->maw.state != BW_MAW_DOWN;
    oamSet(&oamMain, OAM_MAW,
           d->maw.x / BW_ONE - 16, d->maw.y / BW_ONE - 16,
           2, 0, SpriteSize_32x32, SpriteColorFormat_16Color,
           d->maw.state == BW_MAW_SHADOW ? maw_shadow_gfx : maw_risen_gfx,
           -1, false, !maw_shown, false, false, false);
    // Reefs (slice 7): fixed rocks, REGULAR sprites (plain hide works).
    for (int i = 0; i < BW_MAX_REEFS; i++)
    {
        const BwReef *r = &d->reefs[i];
        oamSet(&oamMain, OAM_REEF0 + i,
               r->x / BW_ONE - 8, r->y / BW_ONE - 8,
               1, 0, SpriteSize_16x16, SpriteColorFormat_16Color,
               reef_gfx, -1, false,
               !(r->live && water_live), false, false, false);
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
                       int32_t carry_hull, int32_t band)
{
    sc->seed = frame;
    bw_water_init(duel, sc->seed, band);
    if ((uint32_t)duel->band > sc->best_band)
        sc->best_band = (uint32_t)duel->band;   // charted when you sail it
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
    u16 *maw_shadow_gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32,
                                         SpriteColorFormat_16Color);
    u16 *maw_risen_gfx = oamAllocateGfx(&oamMain, SpriteSize_32x32,
                                        SpriteColorFormat_16Color);
    u16 *reef_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16,
                                   SpriteColorFormat_16Color);
    load_sprite_gfx(player_gfx, &PLAYER_SHIP_ART[0][0], 16, 16, 17);
    load_sprite_gfx(enemy_gfx, &ENEMY_SHIP_ART[0][0], 16, 16, 17);
    load_sprite_gfx(ball_gfx, &BALL_ART[0][0], 8, 8, 9);
    load_sprite_gfx(crate_gfx, &CRATE_ART[0][0], 8, 8, 9);
    load_sprite_gfx(maw_shadow_gfx, &MAW_SHADOW_ART[0][0], 32, 32, 33);
    load_sprite_gfx(maw_risen_gfx, &MAW_RISEN_ART[0][0], 32, 32, 33);
    load_sprite_gfx(reef_gfx, &REEF_ART[0][0], 16, 16, 17);

    // Slice 8: power the sound block. Every voice below is a hardware
    // PSG/noise channel serviced by the default ARM7 core — synthesized
    // square waves and noise, no sample data anywhere.
    soundEnable();

    BwDuel duel = {0};
    bw_duel_init(&duel, 0);              // idle telemetry before first START
    Score score = {0};
    int state = STATE_TITLE;
    int port_row = 0;              // Graywake port menu cursor (slice 4)
    bool maw_counted = false;      // this water's kill already tallied
    uint32_t frame = 0;
    bool pad_seen_idle = false;    // KEYINPUT boot-trap guard (PLATFORM-LIMITS)

    // Slice-8 audio state (ARM9 bookkeeping only — the sound itself
    // runs on the ARM7/hardware channels). The drone free-runs on its
    // channel between tier flips; the cue channel is opened for
    // bw_cue_len frames then killed (re-tuned once mid-cue when the
    // row carries a second note — the dock chime).
    int amb_ch = -1;               // drone channel handle (-1 = silent)
    uint32_t amb_tier = 0;         // tier the drone is playing
    uint32_t amb_flips = 0;        // drone restarts/stops this power-on
    int cue_ch = -1;               // one-shot cue channel handle
    uint32_t cue_left = 0;         // frames left on the cue channel
    uint32_t cue_flip_at = 0;      // cue_left value at which note 2 starts
    uint32_t cue_freq_now = 0;     // what the cue channel plays right now
    uint32_t last_cue = 0;         // last cue id fired
    uint32_t cues = 0;             // cues fired this power-on
    uint32_t cue_frame = 0;        // global frame of the last cue

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

        // Slice-8 audio event capture: remember where the frame began,
        // so the counter/hull deltas + state flips below say which
        // cues fire. Pure reads — nothing here feeds the sim.
        int prev_state = state;
        int32_t prev_phull = duel.player.hull;
        int32_t prev_ehull = duel.enemy.hull;
        int32_t prev_mawhull = duel.maw.hull;
        int32_t prev_mawstate = duel.maw.state;
        int32_t prev_mawbit = duel.maw.bit;
        int32_t prev_hold = duel.hold;
        int32_t prev_ground = duel.groundings;
        uint32_t prev_wins = score.wins;
        uint32_t prev_maws = score.maws;
        int32_t prev_rel_pl = duel.player.reload_l;
        int32_t prev_rel_pr = duel.player.reload_r;
        int32_t prev_rel_el = duel.enemy.reload_l;
        int32_t prev_rel_er = duel.enemy.reload_r;
        uint32_t prev_balls = 0;
        for (int i = 0; i < BW_MAX_BALLS; i++)
            if (duel.balls[i].live)
                prev_balls++;
        uint32_t banked_frame = 0;   // gold banked THIS frame (chime)

        switch (state)
        {
        case STATE_TITLE:
        case STATE_SUNK:
            if (start)                   // fresh sail: no hold carried (a
            {                            // sunk hold was forfeited), free
                begin_duel(&duel, &score, frame, 0, 0, 0,   // full refit,
                           0);           // home water: band 0 off the port
                maw_counted = false;     // fresh water, fresh teeth
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
                banked_frame = banked;   // slice 8: the chime rings
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
            {                            // and the dents (slice 4), on the
                begin_duel(&duel, &score, frame,       // SAME waters
                           duel.hold, duel.hold_gold, duel.player.hull,
                           duel.band);
                maw_counted = false;     // fresh water, fresh teeth
                state = STATE_DUEL;
                break;
            }
            // SELECT presses DEEPER brineward (slice 7): same carry
            // rules as START, one band down — richer wrecks, worse
            // weather, heavier foes, rocks. bw_water_init clamps at
            // band 2 (there is no deeper water than the deeps).
            if (pad_seen_idle && (down & KEY_SELECT))
            {
                begin_duel(&duel, &score, frame,
                           duel.hold, duel.hold_gold, duel.player.hull,
                           duel.band + 1);
                maw_counted = false;     // deeper water, fresh teeth
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
                // slice 5: the keys reach the sim, but bw_salvage_step
                // gates them on the Maw being UP — batteries stay cold
                // on quiet water (no fire verb without a foe), so the
                // committed slice-3/4 stories are untouched
                .fire_l = pad_seen_idle && (down & KEY_L),
                .fire_r = pad_seen_idle && (down & KEY_R),
            };
            bw_salvage_step(&duel, &in);
            uint32_t banked = (uint32_t)bw_dock_step(&duel);
            if (banked > 0)
            {
                score.gold += banked;
                bank_flash = 120;
                bank_amount = banked;
                banked_frame = banked;   // slice 8: the chime rings
            }

            if (duel.maw.slain && !maw_counted)
            {
                score.maws++;            // the carcass is already flotsam
                maw_counted = true;
            }

            if (duel.over == BW_DUEL_PLAYER_SUNK)
            {
                score.sinks++;           // a lingering rake found its mark
                state = STATE_SUNK;      // (or the brine's own teeth did)
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

        // --- slice-8 audio (decision layer pure + mirrored; playback =
        // hardware channels via the ARM7 sound FIFO; nothing feeds back
        // into the sim) -------------------------------------------------

        // One-shot cue channel: re-tune to the second note halfway
        // through a two-note cue (the dock chime), close it when its
        // frames run out.
        if (cue_left > 0)
        {
            cue_left--;
            if (cue_left == 0)
            {
                soundKill(cue_ch);
                cue_ch = -1;
                cue_freq_now = 0;
            }
            else if (cue_flip_at != 0 && cue_left == cue_flip_at)
            {
                soundKill(cue_ch);
                cue_freq_now = bw_cue_freq2(last_cue);
                cue_ch = soundPlayPSG((DutyCycle)bw_cue_duty(last_cue),
                                      (u16)cue_freq_now,
                                      (u8)bw_cue_vol(last_cue), 64);
            }
        }

        // Which cue does this frame want? Highest id wins the channel
        // (the id order IS the priority order — bw_sim.h). Every test
        // below is a delta against the frame-start snapshot.
        uint32_t cue = BW_CUE_NONE;
        if (duel.player.reload_l > prev_rel_pl
            || duel.player.reload_r > prev_rel_pr
            || duel.enemy.reload_l > prev_rel_el
            || duel.enemy.reload_r > prev_rel_er)
            cue = BW_CUE_CANNON;         // a reload only ever jumps UP
                                         //   on the frame a battery fires
        uint32_t balls_now = 0;
        for (int i = 0; i < BW_MAX_BALLS; i++)
            if (duel.balls[i].live)
                balls_now++;
        if (balls_now < prev_balls)
            cue = BW_CUE_SPLASH;         // a rake left the water (a hit
                                         //   on the same frame outranks
                                         //   this below)
        if (duel.hold > prev_hold)
            cue = BW_CUE_SCOOP;
        if (duel.enemy.hull < prev_ehull || duel.maw.hull < prev_mawhull
            || duel.player.hull < prev_phull)
            cue = BW_CUE_CRACK;          // scrape/bite outrank it below
        if (duel.groundings > prev_ground)
            cue = BW_CUE_SCRAPE;         // player-only counter (the rail)
        if (score.wins > prev_wins || score.maws > prev_maws)
            cue = BW_CUE_WRECK;
        if (banked_frame > 0)
            cue = BW_CUE_DOCK;
        if (duel.maw.state == BW_MAW_SURFACE
            && prev_mawstate != BW_MAW_SURFACE)
            cue = BW_CUE_MAWRISE;
        if (duel.maw.bit && !prev_mawbit)
            cue = BW_CUE_BITE;
        if (state == STATE_SUNK && prev_state != STATE_SUNK)
            cue = BW_CUE_SUNK;
        if (cue != BW_CUE_NONE)
        {
            if (cue_ch >= 0)
                soundKill(cue_ch);       // a new cue preempts
            cue_freq_now = bw_cue_freq(cue);
            cue_ch = (bw_cue_duty(cue) == BW_CUE_ON_NOISE)
                ? soundPlayNoise((u16)cue_freq_now,
                                 (u8)bw_cue_vol(cue), 64)
                : soundPlayPSG((DutyCycle)bw_cue_duty(cue),
                               (u16)cue_freq_now,
                               (u8)bw_cue_vol(cue), 64);
            cue_left = bw_cue_len(cue);
            cue_flip_at = bw_cue_freq2(cue) != 0
                ? cue_left - cue_left / 2 : 0;
            last_cue = cue;
            cues++;
            cue_frame = frame;
        }

        // Ambience drone: on over live water (the duel and the salvage),
        // off on the title, the sunk card and in port — dying silences
        // the sea. The FIFO is touched ONLY when the wanted tier flips
        // or the drone starts/stops — between flips the hardware channel
        // free-runs at zero ARM9 cost.
        if (state == STATE_DUEL || state == STATE_SALVAGE)
        {
            uint32_t tier = bw_amb_tier(duel.maw.state != BW_MAW_DOWN,
                                        duel.wind_level);
            if (amb_ch < 0 || tier != amb_tier)
            {
                if (amb_ch >= 0)
                    soundKill(amb_ch);
                amb_ch = soundPlayPSG((DutyCycle)bw_amb_duty(tier),
                                      (u16)bw_amb_freq(tier),
                                      (u8)bw_amb_vol(tier), 64);
                amb_tier = tier;
                amb_flips++;
            }
        }
        else if (amb_ch >= 0)
        {
            soundKill(amb_ch);
            amb_ch = -1;
            amb_flips++;
        }

        draw_ships_and_balls(&duel, player_gfx, enemy_gfx, ball_gfx,
                             crate_gfx, maw_shadow_gfx, maw_risen_gfx,
                             reef_gfx, state, frame);
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
        bw_telemetry[BW_T_MAWSTATE] = (uint32_t)duel.maw.state;
        bw_telemetry[BW_T_MAWX] = (uint32_t)duel.maw.x;
        bw_telemetry[BW_T_MAWY] = (uint32_t)duel.maw.y;
        bw_telemetry[BW_T_MAWHULL] = (uint32_t)duel.maw.hull;
        bw_telemetry[BW_T_MAWS] = score.maws;
        bw_telemetry[BW_T_WINDLVL] = (uint32_t)duel.wind_level;
        bw_telemetry[BW_T_WINDHDG] = (uint32_t)bw_wind_heading(&duel);
        bw_telemetry[BW_T_PSPEED] = (uint32_t)duel.player.speed;
        bw_telemetry[BW_T_BAND] = (uint32_t)duel.band;
        bw_telemetry[BW_T_BEST] = score.best_band;
        uint32_t rocks = 0;
        for (int i = 0; i < BW_MAX_REEFS; i++)
            if (duel.reefs[i].live)
                rocks++;
        bw_telemetry[BW_T_REEFS] = rocks;
        bw_telemetry[BW_T_GROUND] = (uint32_t)duel.groundings;
        bw_telemetry[BW_T_ATIER] = amb_tier;
        bw_telemetry[BW_T_ACUE] = last_cue;
        bw_telemetry[BW_T_ACUES] = cues;
        bw_telemetry[BW_T_ACUEFR] = cue_frame;
        bw_telemetry[BW_T_ADRONE] = amb_ch >= 0 ? bw_amb_freq(amb_tier) : 0;
        bw_telemetry[BW_T_AFLIPS] = amb_flips;
        bw_telemetry[BW_T_ASFXL] = cue_left;
        bw_telemetry[BW_T_ACFREQ] = cue_freq_now;
    }

    return 0;
}
