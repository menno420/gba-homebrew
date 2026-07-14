/*
 * CINDERVAULT (game-lab Track B, breadth program game #6)
 *
 * Turn-based dungeon-dive roguelike, original IP. Dive 5 floors of a
 * seeded dungeon; every turn burns 1 torchlight (torch 0 = death by
 * DARKNESS). D-pad steps one tile (a step into a wall costs NOTHING —
 * wall bumps are free, by design), A waits one turn. Moving into a
 * monster hits it for 2 (bump combat: 'm' has 1-2 HP, 'M' has 3); after
 * every consumed player turn each monster steps one tile toward the
 * player (greedy chase, deterministic tie-break) and bites for 1 HP when
 * adjacent (player HP 10, 0 = SLAIN). Embers '*' refill the torch +25.
 * Stairs '>' descend; descending floor 5 opens the vault — WIN. Score =
 * 100 x floors-cleared + 25 x embers + 10 x kills (+ remaining torch on
 * the win).
 *
 * ITEM LAYER (growth cut 1 — CONCEPT.md: "one inventory slot (a lantern
 * that halves burn for 20 turns, a blade that bumps for 3) picked up
 * like embers — one more decision, zero new verbs"): each floor spawns
 * one lantern 'o' and one blade '/'. Walking onto one picks it up into
 * the SINGLE slot — pickup activates it, and picking up the other item
 * replaces (loses) the held one; that swap is the one more decision.
 * Lantern: for its 20 held turns the torch burns on every SECOND turn
 * only (exactly 10 burned over 20 — halved), then it gutters out and
 * the slot empties. Blade: bumps hit for 3 instead of 2 (a 3-HP 'M'
 * dies in ONE bump) for as long as it is held.
 *
 * SPECIES (growth cut 2 — CONCEPT.md: "Named monster species per depth
 * with distinct chase quirks (the greedy step is a plug-in policy)"): every
 * monster on a floor is that depth's named species, and each species is one
 * quirk wrapped around the same plugged-in greedy chase step:
 *   floor 1  CINDER RAT     — the baseline greedy chase (unchanged).
 *   floor 2  SOOT WISP      — flits: on odd turn counts it phases out
 *                             entirely (no bite, no step) — half-rate.
 *   floor 3  ASH HOUND      — pack: at most ONE hound bites per phase;
 *                             the denied biters hold the ring.
 *   floor 4  VAULT WRAITH   — cold grasp: axis-aligned at range 2 it bites
 *                             through the dark instead of stepping.
 *   floor 5  HOARD SENTINEL — stands guard: never leaves its post until
 *                             the player comes within its leash (4).
 * Species consume NO RNG — the species is a pure function of the floor
 * number, so floor generation is byte-identical to growth cut 1 and every
 * fixed-seed spawn pin carries. The current depth's species name is shown
 * on its own HUD line ("FOE ...").
 *
 * ENDLESS DEPTH BANDING (growth cut 3 — CONCEPT.md: "Depth banding past
 * floor 5 for an endless mode; the vault becomes the first milestone
 * instead of the end"): descending floor 5 still opens the vault — the
 * win screen, the banked score with its +torch bonus, all of it stands,
 * and pressing START still restarts the same seeded run. But SELECT on
 * the VAULT REACHED screen now DELVES DEEPER: floor 6 is generated from
 * the RNG state the vault left behind, hp/torch/items/turn count all
 * carry, and the +torch win bonus drops back out of the live score — the
 * price of not stopping. Past floor 5 every floor draws its generator
 * parameters (monster count, big-monster count, embers, carve-walk
 * length) from DEPTH BANDS — one band per 3 floors, each meaner than the
 * last, the deepest band the floor of the world — and the five named
 * species RECUR on the same five-depth cycle (floor 6 = CINDER RAT
 * again, ... floor 10 = HOARD SENTINEL, and around). There is no second
 * win: an endless run ends only in DARKNESS or SLAIN, banking 100 per
 * cleared floor plus embers and kills — score-attack. A run that never
 * presses SELECT is bit-identical to growth cut 2.
 *
 * SEED-SELECT SCORE-ATTACK (growth cut 4 — the concept's "daily seed +
 * score-attack leaderboard" line, GBA-shaped per the Deepcast precedent:
 * a cartridge has no clock and no server, so "daily" becomes a DIALABLE
 * seed): on the title, the seed line is a dial — UP/DOWN adjusts the
 * seed +-1, LEFT/RIGHT +-0x100, L/R +-0x10000 (all edge-triggered,
 * 32-bit wrapping; the xorshift dead state 0 is skipped in the dial's
 * own direction). None of these keys clash with the title's only other
 * input, START. Two players who dial the same 8 hex digits dive the
 * SAME vault — same floors, same monsters, same embers — and the seed
 * is repeated on the death card (endless runs end only there: the
 * score-attack card), so a score is a claim anyone with a cartridge
 * can check. The boot seed stays 0xC1DE5EED: with no dial input the
 * game is bit-identical to growth cut 3. START from win/lose restarts
 * the same dialed seed; the dial only lives on the title. The
 * leaderboard half of the concept line is out of GBA scope (no
 * network) per the Tiltstone precedent — the seed is what makes an
 * off-cartridge leaderboard honest.
 *
 * DETERMINISM CONTRACT (headless proof relies on all of this):
 *   - Integer math only. One xorshift32 PRNG, boot seed 0xC1DE5EED
 *     (shown on the title card). RNG words are consumed ONLY inside
 *     floor generation (spawn, carve walk, embers, monsters), in a fixed
 *     order, so the same input script replays bit-identically.
 *   - Turns advance ONLY on player input edges: no frame-time, no
 *     vblank count feeds the simulation. Waiting at a screen forever
 *     changes nothing.
 *   - Floors are carved by a seeded random walk from the spawn tile, so
 *     a spawn->stairs path exists BY CONSTRUCTION (the stairs tile is
 *     the carved tile farthest from spawn); embers and monsters are
 *     placed only on carved tiles.
 *   - START from win/lose restarts the SAME seeded run (the dialed
 *     seed, growth cut 4; the boot seed when the dial was never
 *     touched).
 *
 * Telemetry mailbox for the headless harness (tools/headless-screenshot.py
 * --elf --watch cv:cv_telemetry:18): a volatile 18-word array with
 * C-linkage symbol `cv_telemetry`, updated every frame:
 *   [0] 0x43494E44 'CIND'   magic     [8]  kills this run
 *   [1] 0x56414C54 'VALT'   magic     [9]  score (live formula)
 *   [2] state (0 title, 1 playing,    [10] turn count
 *       2 win, 3 lose)                [11] player x (grid)
 *   [3] selected PRNG seed (boot      [12] player y (grid)
 *       0xC1DE5EED; title dial live)
 *   [4] floor (1..5; 6+ endless)      [13] lose reason (0 none,
 *   [5] player hp                          1 darkness, 2 slain)
 *   [6] torch remaining               [14] monsters alive on floor
 *   [7] embers collected              [15] frames since boot
 *   [16] item slot (0 empty,          [17] lantern turns remaining
 *        1 lantern, 2 blade)               (0 unless a lantern is held)
 *
 * ART PASS (growth cut 5 — CONCEPT.md: "Real art pass: tile sprites for
 * walls/floor, a torch-radius light fade (the mechanic made diegetic —
 * the screen literally darkens as the torch burns down)" — the LAST named
 * growth line, presentation ONLY): the dungeon's glyph rows are replaced
 * by an 8x8 tile background (original procedurally-generated assets,
 * graphics/generate_assets.py — floor, brick wall, glowing ember, steel
 * stairs, lantern, blade, and both monster breeds), baked from the SAME
 * tiles[][]/monsters[] state the glyphs read; the player is a torch-
 * bearer sprite. The torch mechanic goes diegetic through the shared
 * gl_light_window.h (Lumen Drift's hardware-window light circle): the
 * dungeon background is visible only inside a circle around the player
 * whose radius follows the torch —
 *
 *     light_radius_px = min(16 + torch / 2, 200)
 *
 * — so the light literally closes in as the torch burns (and re-opens
 * +12px on every ember, torch economy made visible). Sprites ride OVER
 * the window (Butano windows clip backgrounds only), so the HUD text and
 * the torch-bearer stay lit: they are carrying the flame. Nothing in the
 * simulation reads any of it — game state, RNG word order and every
 * cv_telemetry word are byte-identical to growth cut 4, so every
 * existing proof pin carries verbatim.
 *
 * Presentation telemetry for the headless harness (a SECOND mailbox —
 * cv_telemetry's 18 words are untouched by contract): volatile unsigned
 * cv_light[4], C linkage, updated every frame:
 *   [0] 0x4C495445 'LITE' magic     [2] light center screen x (0..239)
 *   [1] light radius in pixels      [3] light center screen y (0..159)
 *       (the law above; 200 = full
 *       screen on title/win/lose)
 *
 * HUD/title/end text still uses Butano's common variable 8x8 font (the
 * one tools/headless-screenshot.py --assert-text can read). All code and
 * art original; the font is Butano's zlib-licensed common asset
 * (third_party/butano/common).
 */

#include "bn_core.h"
#include "bn_color.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_vector.h"
#include "bn_display.h"
#include "bn_window.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_regular_bg_map_item.h"
#include "bn_regular_bg_map_cell_info.h"

#include "bn_sprite_items_cv_player.h"
#include "bn_regular_bg_tiles_items_cv_tiles.h"
#include "bn_bg_palette_items_cv_palette.h"

#include "common_variable_8x8_sprite_font.h"

#include "gl_light_window.h"

extern "C"
{
    // Headless telemetry mailbox — see the layout table in the header
    // comment. volatile so every write really lands for the emulator bus.
    volatile unsigned cv_telemetry[18];

    // Presentation telemetry (growth cut 5, the art pass) — the light
    // fade's own mailbox, so cv_telemetry's 18 simulation words stay
    // untouched by contract. Layout in the header comment.
    volatile unsigned cv_light[4];
}

namespace
{
    // --- deterministic core -------------------------------------------------

    constexpr unsigned seed_constant = 0xC1DE5EED;

    struct rng_t
    {
        unsigned s;

        explicit rng_t(unsigned seed) : s(seed) {}

        unsigned next()
        {
            s ^= s << 13;
            s ^= s >> 17;
            s ^= s << 5;
            return s;
        }

        int range(int n)  // [0, n)
        {
            return int(next() % unsigned(n));
        }
    };

    enum state_t : unsigned
    {
        st_title = 0,
        st_playing = 1,
        st_win = 2,
        st_lose = 3,
    };

    // Tuning (all integers; see CONCEPT.md for the intended strategy space).
    constexpr int map_w = 13;           // glyph columns (border walls included)
    constexpr int map_h = 8;            // glyph rows
    constexpr int carve_steps = 240;    // random-walk length per floor
    constexpr int last_floor = 5;
    constexpr int start_torch = 220;    // -1 per consumed turn
    constexpr int ember_torch = 25;     // torch refill per ember
    constexpr int embers_per_floor = 3;
    constexpr int start_hp = 10;
    constexpr int big_monster_hp = 3;   // 'M'; small 'm' rolls 1-2
    constexpr int bump_damage = 2;      // player bump: 'm' dies in one hit,
                                        // 'M' takes two (hit-for-1 proved
                                        // unsurvivable vs equal-speed chasers)
    constexpr int blade_damage = 3;     // bump while holding the blade:
                                        // 'M' dies in ONE hit (CONCEPT.md:
                                        // "a blade that bumps for 3")
    constexpr int lantern_turns = 20;   // held turns of halved burn
    constexpr int max_monsters = 8;

    // Monsters per floor 1..5 (floor 3+ also upgrades slot 0 to a big 'M').
    constexpr int monster_count[last_floor] = {2, 2, 3, 3, 4};

    // Species (growth cut 2): one named species per depth, each one quirk
    // on the plugged-in greedy chase step (see the header comment). NO RNG
    // is consumed by species — pure function of the floor number.
    enum species_t : unsigned char
    {
        sp_rat = 0,       // CINDER RAT     — baseline greedy chase
        sp_wisp = 1,      // SOOT WISP      — flits out on odd turn counts
        sp_hound = 2,     // ASH HOUND      — one pack bite per phase
        sp_wraith = 3,    // VAULT WRAITH   — cold grasp at range 2, aligned
        sp_sentinel = 4,  // HOARD SENTINEL — holds post outside its leash
    };

    constexpr species_t floor_species[last_floor] = {
        sp_rat, sp_wisp, sp_hound, sp_wraith, sp_sentinel,
    };

    constexpr const char* species_name[last_floor] = {
        "CINDER RAT", "SOOT WISP", "ASH HOUND", "VAULT WRAITH",
        "HOARD SENTINEL",
    };

    constexpr int wraith_reach = 2;    // cold-grasp Manhattan distance
    constexpr int sentinel_leash = 4;  // guard wakes inside this distance

    // Endless depth bands (growth cut 3): past floor 5 every floor draws
    // its generator parameters from this table — one band per band_floors
    // floors, the last band is the floor of the world. Species recur on
    // the same five-depth cycle: (floor - 1) % last_floor is the species
    // law for EVERY floor, floors 1-5 included, unchanged.
    constexpr int band_floors = 3;

    struct band_t
    {
        unsigned char monsters;
        unsigned char bigs;
        unsigned char embers;
        short carve;
    };

    constexpr band_t depth_bands[] = {
        {4, 1, 2, 220},  // band 1: floors  6-8
        {5, 2, 2, 200},  // band 2: floors  9-11
        {6, 2, 1, 180},  // band 3: floors 12-14
        {7, 3, 1, 160},  // band 4: floors 15-17
        {8, 4, 1, 140},  // band 5: floors 18+
    };

    constexpr int band_count = sizeof(depth_bands) / sizeof(depth_bands[0]);

    enum tile_t : unsigned char
    {
        t_wall = 0,
        t_floor = 1,
        t_ember = 2,
        t_stairs = 3,
        t_lantern = 4,
        t_blade = 5,
    };

    enum item_t : unsigned
    {
        it_none = 0,
        it_lantern = 1,
        it_blade = 2,
    };

    struct monster_t
    {
        int x = 0;
        int y = 0;
        int hp = 0;
        bool big = false;
        bool alive = false;
    };

    // --- text presentation ---------------------------------------------------

    struct text_line
    {
        bn::vector<bn::sprite_ptr, 16> sprites;
        bn::string<40> cached;
        bool dirty_clear = false;

        void set(bn::sprite_text_generator& gen, int x, int y,
                 const bn::string<40>& text)
        {
            if(! dirty_clear && cached == text)
            {
                return;
            }

            dirty_clear = false;
            cached = text;
            sprites.clear();
            gen.generate(x, y, text, sprites);
        }

        void clear()
        {
            cached.clear();
            sprites.clear();
            dirty_clear = true;
        }
    };

    bn::string<40> seed_hex(unsigned value)
    {
        // The seed as exactly 8 uppercase hex digits — the shareable
        // challenge code two players compare on their title screens
        // (growth cut 4).
        bn::string<40> out;

        for(int shift = 28; shift >= 0; shift -= 4)
        {
            unsigned digit = (value >> shift) & 0xFu;
            out.push_back(char(digit < 10 ? '0' + digit : 'A' + digit - 10));
        }

        return out;
    }
}

int main()
{
    bn::core::init();

    // Vault-dark backdrop: near-black warm brown, unmistakably not blank.
    bn::bg_palettes::set_transparent_color(bn::color(4, 2, 2));

    bn::sprite_text_generator ui_gen(common::variable_8x8_sprite_font);
    ui_gen.set_left_alignment();

    // --- the art pass (growth cut 5): tile background + torch light ------

    // The dungeon grid, cell (x, y) rendered as an 8x8 tile with the
    // grid's top-left at screen-centered (-52, -74) — the same band the
    // glyph rows occupied (HUD text lives below y -10: no overlap, so
    // every --assert-text pin is undisturbed).
    constexpr int grid_px_x = -52;
    constexpr int grid_px_y = -74;
    constexpr int bg_cols = 32;
    constexpr int bg_rows = 32;

    // tiles[][] value -> cv_tiles tile index (monsters bake on top; only
    // the non-grid cells stay tile 0 = transparent backdrop).
    constexpr int tile_gfx[6] = {2, 1, 3, 4, 5, 6};  // wall floor ember
                                                     // stairs lantern blade

    alignas(int) bn::regular_bg_map_cell bg_cells[bg_cols * bg_rows] = {};
    bn::regular_bg_map_item bg_map_item(bg_cells[0],
                                        bn::size(bg_cols, bg_rows));
    bn::regular_bg_item bg_item(bn::regular_bg_tiles_items::cv_tiles,
                                bn::bg_palette_items::cv_palette,
                                bg_map_item);
    // create_bg() places the MAP CENTER, so the map's cell (0, 0) lands
    // its top-left exactly on (grid_px_x, grid_px_y).
    bn::regular_bg_ptr dungeon_bg = bg_item.create_bg(
            grid_px_x + bg_cols * 4, grid_px_y + bg_rows * 4);
    bn::regular_bg_map_ptr dungeon_map = dungeon_bg.map();

    // The torch-bearer sprite rides OVER the light window (Butano windows
    // clip backgrounds only) — the player is carrying the flame.
    bn::sprite_ptr player_sprite =
            bn::sprite_items::cv_player.create_sprite(0, 0);

    // The torch-radius light fade: the dungeon bg renders only inside the
    // light circle; outside it falls to the vault-dark backdrop.
    gl::light_window light;
    bn::window::outside().set_show_bg(dungeon_bg, false);

    constexpr int ui_count = 5;
    constexpr int ui_y[ui_count] = {-50, -30, -10, 10, 30};
    constexpr int ui_x = -104;
    constexpr int hud_x = -116;           // HUD hugs the left edge: the
                                          // longest HUD string must fit the
                                          // 240px screen (glyphs clip at the
                                          // right edge and break --assert-text)
    constexpr int hud_y = 40;             // HUD slot is ui_lines[4] in play
    constexpr int item_y = 20;            // item slot line is ui_lines[3]
    constexpr int foe_y = 10;             // species line is ui_lines[2]
    text_line ui_lines[ui_count];

    auto clear_lines = [&]()
    {
        for(int i = 0; i < ui_count; ++i)
        {
            ui_lines[i].clear();
        }
    };

    // Seed-select (growth cut 4): the dialed challenge seed. Boot value
    // is the classic constant, only the title dial changes it, and
    // reset_run() reads it — so with no dial input every run is the boot
    // run, and a dialed value survives win/lose (START reruns the SAME
    // dialed vault).
    unsigned seed_sel = seed_constant;

    // --- run state (reset_run() restores the exact boot run)
    rng_t rng(seed_constant);
    unsigned state = st_title;
    unsigned frames = 0;                  // monotonic since boot, never reset
    unsigned char tiles[map_h][map_w];
    monster_t monsters[max_monsters];
    int floor_no = 1;
    int px = 1;
    int py = 1;
    int hp = start_hp;
    int torch = start_torch;
    int embers = 0;
    int kills = 0;
    int turns = 0;
    int lose_reason = 0;                  // 0 none · 1 darkness · 2 slain
    unsigned item = it_none;              // the single inventory slot
    int item_turns = 0;                   // lantern countdown (0 otherwise)

    auto monster_at = [&](int x, int y) -> int
    {
        for(int i = 0; i < max_monsters; ++i)
        {
            if(monsters[i].alive && monsters[i].x == x && monsters[i].y == y)
            {
                return i;
            }
        }

        return -1;
    };

    // Bake the dungeon state into the tile background (growth cut 5 —
    // pure presentation: reads the SAME tiles[][]/monsters[] the glyph
    // rows read, writes only VRAM-bound cells).
    auto bake_map = [&]()
    {
        for(int y = 0; y < map_h; ++y)
        {
            for(int x = 0; x < map_w; ++x)
            {
                int gfx = tile_gfx[tiles[y][x]];
                int mi = monster_at(x, y);

                if(mi >= 0)
                {
                    gfx = monsters[mi].big ? 8 : 7;
                }

                bn::regular_bg_map_cell& cell =
                        bg_cells[bg_map_item.cell_index(x, y)];
                bn::regular_bg_map_cell_info cell_info(cell);
                cell_info.set_tile_index(gfx);
                cell = cell_info.cell();
            }
        }

        dungeon_map.reload_cells_ref();
    };

    auto generate_floor = [&]()
    {
        // Per-floor generator parameters: the classic tables for floors
        // 1-5 verbatim (the default vault run is bit-identical to growth
        // cut 2), depth_bands past the vault (growth cut 3). Same RNG
        // word order either way — only the loop bounds move.
        int mon_count, bigs, ember_count, carve;

        if(floor_no <= last_floor)
        {
            mon_count = monster_count[floor_no - 1];
            bigs = floor_no >= 3 ? 1 : 0;
            ember_count = embers_per_floor;
            carve = carve_steps;
        }
        else
        {
            int b = (floor_no - last_floor - 1) / band_floors;

            if(b > band_count - 1)
            {
                b = band_count - 1;
            }

            mon_count = depth_bands[b].monsters;
            bigs = depth_bands[b].bigs;
            ember_count = depth_bands[b].embers;
            carve = depth_bands[b].carve;
        }

        for(int y = 0; y < map_h; ++y)
        {
            for(int x = 0; x < map_w; ++x)
            {
                tiles[y][x] = t_wall;
            }
        }

        for(int i = 0; i < max_monsters; ++i)
        {
            monsters[i].alive = false;
        }

        // Spawn (RNG words 1-2 of the floor), then the carve walk. Every
        // walkable tile of the floor is on the walk, so spawn->stairs is
        // connected by construction.
        px = 1 + rng.range(map_w - 2);
        py = 1 + rng.range(map_h - 2);

        int carved_x[map_h * map_w];
        int carved_y[map_h * map_w];
        int carved_count = 0;
        int cx = px;
        int cy = py;
        tiles[cy][cx] = t_floor;
        carved_x[carved_count] = cx;
        carved_y[carved_count] = cy;
        ++carved_count;

        for(int step = 0; step < carve; ++step)
        {
            int dir = rng.range(4);
            int nx = cx + (dir == 2 ? -1 : dir == 3 ? 1 : 0);
            int ny = cy + (dir == 0 ? -1 : dir == 1 ? 1 : 0);

            if(nx >= 1 && nx <= map_w - 2 && ny >= 1 && ny <= map_h - 2)
            {
                cx = nx;
                cy = ny;

                if(tiles[cy][cx] == t_wall)
                {
                    tiles[cy][cx] = t_floor;
                    carved_x[carved_count] = cx;
                    carved_y[carved_count] = cy;
                    ++carved_count;
                }
            }
        }

        // Stairs: the carved tile farthest from spawn (first max in carve
        // order — deterministic).
        int best = 0;
        int best_dist = -1;

        for(int i = 0; i < carved_count; ++i)
        {
            int dist = (carved_x[i] > px ? carved_x[i] - px : px - carved_x[i])
                     + (carved_y[i] > py ? carved_y[i] - py : py - carved_y[i]);

            if(dist > best_dist)
            {
                best_dist = dist;
                best = i;
            }
        }

        tiles[carved_y[best]][carved_x[best]] = t_stairs;

        // Embers on carved plain-floor tiles (never the spawn).
        for(int e = 0; e < ember_count; ++e)
        {
            for(int attempt = 0; attempt < 100; ++attempt)
            {
                int i = rng.range(carved_count);
                int x = carved_x[i];
                int y = carved_y[i];

                if(tiles[y][x] == t_floor && ! (x == px && y == py))
                {
                    tiles[y][x] = t_ember;
                    break;
                }
            }
        }

        // Monsters on carved plain-floor tiles, Manhattan > 3 from spawn.
        // The first `bigs` slots are 3-HP bigs (floors 1-5: slot 0 from
        // floor 3 — unchanged); bigs roll no HP word, so the bigs count
        // is part of the floor's pinned RNG word order.
        for(int m = 0; m < mon_count; ++m)
        {
            bool big = m < bigs;
            int mhp = big ? big_monster_hp : 1 + rng.range(2);

            for(int attempt = 0; attempt < 100; ++attempt)
            {
                int i = rng.range(carved_count);
                int x = carved_x[i];
                int y = carved_y[i];
                int dist = (x > px ? x - px : px - x)
                         + (y > py ? y - py : py - y);

                if(tiles[y][x] == t_floor && dist > 3 && monster_at(x, y) < 0)
                {
                    monsters[m].x = x;
                    monsters[m].y = y;
                    monsters[m].hp = mhp;
                    monsters[m].big = big;
                    monsters[m].alive = true;
                    break;
                }
            }
        }

        // Items (growth cut 1): one lantern, then one blade, on carved
        // plain-floor tiles clear of the spawn and monsters. Placed LAST so
        // each floor's pre-item RNG word order is the prototype's; the
        // extra draws re-seed later floors — a new world version, and the
        // proofs pin the new world.
        for(int it = 0; it < 2; ++it)
        {
            for(int attempt = 0; attempt < 100; ++attempt)
            {
                int i = rng.range(carved_count);
                int x = carved_x[i];
                int y = carved_y[i];

                if(tiles[y][x] == t_floor && ! (x == px && y == py)
                   && monster_at(x, y) < 0)
                {
                    tiles[y][x] = it == 0 ? t_lantern : t_blade;
                    break;
                }
            }
        }
    };

    auto reset_run = [&]()
    {
        rng.s = seed_sel;
        floor_no = 1;
        hp = start_hp;
        torch = start_torch;
        embers = 0;
        kills = 0;
        turns = 0;
        lose_reason = 0;
        item = it_none;
        item_turns = 0;
        state = st_playing;
        generate_floor();
        clear_lines();
    };

    auto monster_phase = [&]()
    {
        // The greedy step is a plug-in policy (growth cut 2): every monster
        // on a floor is that depth's named species, and each species is one
        // quirk wrapped around the same baseline greedy step below.
        species_t sp = floor_species[(floor_no - 1) % last_floor];
        bool pack_bite_taken = false;

        for(int i = 0; i < max_monsters; ++i)
        {
            monster_t& m = monsters[i];

            if(! m.alive)
            {
                continue;
            }

            // SOOT WISP flits: on odd turn counts it phases out entirely —
            // no bite, no step — so it closes (and drains) at half rate.
            if(sp == sp_wisp && (turns & 1) == 1)
            {
                continue;
            }

            int dx = px - m.x;
            int dy = py - m.y;
            int adx = dx > 0 ? dx : -dx;
            int ady = dy > 0 ? dy : -dy;

            if(adx + ady == 1)
            {
                // ASH HOUND pack discipline: at most ONE hound bites per
                // phase (slot order); the denied biters hold the ring.
                if(sp == sp_hound)
                {
                    if(! pack_bite_taken)
                    {
                        --hp;
                        pack_bite_taken = true;
                    }
                }
                else
                {
                    --hp;  // bite
                }

                continue;
            }

            // VAULT WRAITH cold grasp: axis-aligned at reach 2 it bites
            // through the dark INSTEAD of stepping (never needs adjacency).
            if(sp == sp_wraith && adx + ady == wraith_reach
               && (dx == 0 || dy == 0))
            {
                --hp;
                continue;
            }

            // HOARD SENTINEL stands its ground until the player crosses
            // the leash; outside it, it never leaves its post.
            if(sp == sp_sentinel && adx + ady > sentinel_leash)
            {
                continue;
            }

            int sx = dx > 0 ? 1 : (dx < 0 ? -1 : 0);
            int sy = dy > 0 ? 1 : (dy < 0 ? -1 : 0);
            bool horiz_first = adx >= ady;  // tie-break: horizontal wins

            for(int attempt = 0; attempt < 2; ++attempt)
            {
                bool horiz = (attempt == 0) == horiz_first;
                int nx = m.x + (horiz ? sx : 0);
                int ny = m.y + (horiz ? 0 : sy);

                if((nx == m.x && ny == m.y)
                   || tiles[ny][nx] != t_floor
                   || (nx == px && ny == py)
                   || monster_at(nx, ny) >= 0)
                {
                    continue;
                }

                m.x = nx;
                m.y = ny;
                break;
            }
        }
    };

    while(true)
    {
        if(state == st_title || state == st_win || state == st_lose)
        {
            if(bn::keypad::start_pressed())
            {
                reset_run();  // same seed -> the identical run, by contract
            }
            else if(state == st_win && bn::keypad::select_pressed())
            {
                // DELVE DEEPER (growth cut 3): the vault win stands as the
                // first milestone; the endless descent continues from the
                // exact RNG state the vault left behind. hp/torch/items/
                // turn count all carry; the +torch win bonus drops back
                // out of the live score (it pays only if you stop here).
                // Consumes NO turn — a menu edge, not a dungeon turn.
                ++floor_no;
                generate_floor();
                state = st_playing;
                clear_lines();
            }
            else if(state == st_title)
            {
                // The seed dial (growth cut 4; edge-triggered so one
                // press is one step — a challenge code is a repeatable
                // press sequence). 32-bit wrap; 0 is xorshift32's dead
                // state, so it is skipped in whichever direction the
                // dial was turning. The dial only lives on the title:
                // these keys are dungeon verbs everywhere else.
                unsigned delta = 0;

                if(bn::keypad::up_pressed())
                {
                    delta += 1;
                }

                if(bn::keypad::down_pressed())
                {
                    delta -= 1;
                }

                if(bn::keypad::right_pressed())
                {
                    delta += 0x100;
                }

                if(bn::keypad::left_pressed())
                {
                    delta -= 0x100;
                }

                if(bn::keypad::r_pressed())
                {
                    delta += 0x10000;
                }

                if(bn::keypad::l_pressed())
                {
                    delta -= 0x10000;
                }

                if(delta != 0)
                {
                    seed_sel += delta;

                    if(seed_sel == 0)
                    {
                        seed_sel += delta;
                    }
                }
            }
        }
        else  // st_playing: one turn per input edge, nothing else moves
        {
            int dx = 0;
            int dy = 0;
            bool wait = false;

            if(bn::keypad::up_pressed())
            {
                dy = -1;
            }
            else if(bn::keypad::down_pressed())
            {
                dy = 1;
            }
            else if(bn::keypad::left_pressed())
            {
                dx = -1;
            }
            else if(bn::keypad::right_pressed())
            {
                dx = 1;
            }
            else if(bn::keypad::a_pressed())
            {
                wait = true;
            }

            bool acted = false;

            if(wait)
            {
                acted = true;
            }
            else if(dx != 0 || dy != 0)
            {
                int nx = px + dx;
                int ny = py + dy;
                int mi = monster_at(nx, ny);

                if(mi >= 0)
                {
                    // Bump combat (the blade upgrades the bump to 3).
                    acted = true;
                    monsters[mi].hp -= item == it_blade ? blade_damage
                                                        : bump_damage;

                    if(monsters[mi].hp <= 0)
                    {
                        monsters[mi].alive = false;
                        ++kills;
                    }
                }
                else if(tiles[ny][nx] == t_wall)
                {
                    acted = false;  // wall bumps are free, by design
                }
                else
                {
                    px = nx;
                    py = ny;
                    acted = true;

                    if(tiles[py][px] == t_ember)
                    {
                        tiles[py][px] = t_floor;
                        torch += ember_torch;
                        ++embers;
                    }
                    else if(tiles[py][px] == t_lantern
                            || tiles[py][px] == t_blade)
                    {
                        // Pickup activates (zero new verbs); the single
                        // slot means the held item is replaced and lost.
                        item = tiles[py][px] == t_lantern ? it_lantern
                                                          : it_blade;
                        item_turns = item == it_lantern ? lantern_turns : 0;
                        tiles[py][px] = t_floor;
                    }
                    else if(tiles[py][px] == t_stairs)
                    {
                        if(floor_no == last_floor)
                        {
                            state = st_win;  // the vault opens
                            clear_lines();
                        }
                        else
                        {
                            ++floor_no;
                            generate_floor();
                        }
                    }
                }
            }

            if(acted)
            {
                ++turns;

                if(state == st_playing)
                {
                    bool burn = true;

                    if(item == it_lantern)
                    {
                        // Halved burn: the torch burns on every SECOND
                        // held turn only (odd countdown values are free),
                        // so exactly 10 of the 20 lantern turns burn.
                        --item_turns;
                        burn = (item_turns & 1) == 0;

                        if(item_turns <= 0)
                        {
                            item = it_none;  // the lantern gutters out
                            item_turns = 0;
                        }
                    }

                    if(burn)
                    {
                        --torch;
                    }

                    if(torch <= 0)
                    {
                        torch = 0;
                        lose_reason = 1;  // DARKNESS
                        state = st_lose;
                        clear_lines();
                    }
                    else
                    {
                        monster_phase();

                        if(hp <= 0)
                        {
                            hp = 0;
                            lose_reason = 2;  // SLAIN
                            state = st_lose;
                            clear_lines();
                        }
                    }
                }
            }
        }

        // Live score formula (win adds the remaining torch).
        int floors_cleared = state == st_win ? last_floor : floor_no - 1;
        int score = 100 * floors_cleared + 25 * embers + 10 * kills
                  + (state == st_win ? torch : 0);
        int monsters_alive = 0;

        for(int i = 0; i < max_monsters; ++i)
        {
            if(monsters[i].alive)
            {
                ++monsters_alive;
            }
        }

        // --- draw (text only; lines re-render only when their string changes)

        switch(state)
        {

        case st_title:
        {
            ui_lines[0].set(ui_gen, ui_x, ui_y[0], "CINDERVAULT");
            bn::string<40> seed_line("SEED ");
            seed_line.append(seed_hex(seed_sel));
            ui_lines[1].set(ui_gen, ui_x, ui_y[1], seed_line);
            ui_lines[2].set(ui_gen, ui_x, ui_y[2], "EVERY STEP BURNS THE TORCH");
            ui_lines[3].set(ui_gen, ui_x, ui_y[3], "DIVE 5 FLOORS TO THE VAULT");
            // The dial hint shares the START line (rendered from the HUD
            // margin so the longer line still clears the 240px edge —
            // see the hud_x comment).
            ui_lines[4].set(ui_gen, hud_x, ui_y[4],
                            "PRESS START  DPAD L R: DIAL SEED");
            break;
        }

        case st_playing:
        {
            // The art pass (growth cut 5): tiles instead of glyphs. The
            // bake reads the same state the glyph rows read.
            bake_map();
            player_sprite.set_position(grid_px_x + 8 * px + 4,
                                       grid_px_y + 8 * py + 4);

            bn::string<40> hud("FLR ");
            hud.append(bn::to_string<8>(floor_no));
            hud.append(" HP ");
            hud.append(bn::to_string<8>(hp));
            hud.append(" TORCH ");
            hud.append(bn::to_string<8>(torch));
            hud.append(" SCORE ");
            hud.append(bn::to_string<8>(score));
            ui_lines[4].set(ui_gen, hud_x, hud_y, hud);

            // The item slot gets its own short line (the HUD line already
            // reaches near the 240px edge — see the hud_x comment).
            bn::string<40> item_line("ITEM ");

            if(item == it_lantern)
            {
                item_line.append("LANTERN ");
                item_line.append(bn::to_string<8>(item_turns));
            }
            else if(item == it_blade)
            {
                item_line.append("BLADE");
            }
            else
            {
                item_line.append("-");
            }

            ui_lines[3].set(ui_gen, hud_x, item_y, item_line);

            // The depth's named species (growth cut 2) — "named" is
            // player-visible, not just a policy index.
            bn::string<40> foe_line("FOE ");
            foe_line.append(species_name[(floor_no - 1) % last_floor]);
            ui_lines[2].set(ui_gen, hud_x, foe_y, foe_line);
            break;
        }

        case st_win:
        {
            ui_lines[0].set(ui_gen, ui_x, ui_y[0], "VAULT REACHED");
            bn::string<40> line1("SCORE ");
            line1.append(bn::to_string<8>(score));
            ui_lines[1].set(ui_gen, ui_x, ui_y[1], line1);
            bn::string<40> line2("EMBERS ");
            line2.append(bn::to_string<8>(embers));
            line2.append("  KILLS ");
            line2.append(bn::to_string<8>(kills));
            line2.append("  TORCH ");
            line2.append(bn::to_string<8>(torch));
            ui_lines[2].set(ui_gen, ui_x, ui_y[2], line2);
            ui_lines[3].set(ui_gen, ui_x, ui_y[3], "SELECT DELVES DEEPER");
            ui_lines[4].set(ui_gen, ui_x, ui_y[4], "PRESS START");
            break;
        }

        case st_lose:
        {
            ui_lines[0].set(ui_gen, ui_x, ui_y[0],
                            lose_reason == 1 ? "DARKNESS. THE TORCH IS OUT"
                                             : "SLAIN IN THE DEEP");
            bn::string<40> line1("SCORE ");
            line1.append(bn::to_string<8>(score));
            ui_lines[1].set(ui_gen, ui_x, ui_y[1], line1);
            bn::string<40> line2("FLOOR ");
            line2.append(bn::to_string<8>(floor_no));
            line2.append("  TURNS ");
            line2.append(bn::to_string<8>(turns));
            ui_lines[2].set(ui_gen, ui_x, ui_y[2], line2);
            // Score attribution (growth cut 4): endless runs end only
            // here, so the death card is the score-attack card — it
            // names the seed the score was dived on.
            bn::string<40> line3("SEED ");
            line3.append(seed_hex(seed_sel));
            ui_lines[3].set(ui_gen, ui_x, ui_y[3], line3);
            ui_lines[4].set(ui_gen, ui_x, ui_y[4], "PRESS START");
            break;
        }

        }

        // --- the torch-radius light fade (presentation only): the dungeon
        // bg and the torch-bearer show only during play, and the light
        // circle's radius follows the torch — the burn economy on screen.
        bool in_dungeon = state == st_playing;
        dungeon_bg.set_visible(in_dungeon);
        player_sprite.set_visible(in_dungeon);

        int light_x = 0;
        int light_y = 0;
        int light_radius = gl::light_window::full_radius();

        if(in_dungeon)
        {
            light_x = grid_px_x + 8 * px + 4;   // the player's tile center
            light_y = grid_px_y + 8 * py + 4;
            light_radius = bn::min(16 + torch / 2,
                                   gl::light_window::full_radius());
        }

        light.set(bn::fixed_point(light_x, light_y), light_radius);

        cv_light[0] = 0x4C495445u;  // 'LITE'
        cv_light[1] = unsigned(light_radius);
        cv_light[2] = unsigned(light_x + bn::display::width() / 2);
        cv_light[3] = unsigned(light_y + bn::display::height() / 2);

        // --- telemetry mailbox: every slot, every frame

        ++frames;
        cv_telemetry[0] = 0x43494E44u;  // 'CIND'
        cv_telemetry[1] = 0x56414C54u;  // 'VALT'
        cv_telemetry[2] = state;
        cv_telemetry[3] = seed_sel;
        cv_telemetry[4] = unsigned(floor_no);
        cv_telemetry[5] = unsigned(hp);
        cv_telemetry[6] = unsigned(torch);
        cv_telemetry[7] = unsigned(embers);
        cv_telemetry[8] = unsigned(kills);
        cv_telemetry[9] = unsigned(score);
        cv_telemetry[10] = unsigned(turns);
        cv_telemetry[11] = unsigned(px);
        cv_telemetry[12] = unsigned(py);
        cv_telemetry[13] = unsigned(lose_reason);
        cv_telemetry[14] = unsigned(monsters_alive);
        cv_telemetry[15] = frames;
        cv_telemetry[16] = item;
        cv_telemetry[17] = unsigned(item_turns);

        bn::core::update();
    }
}
