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
 * DETERMINISM CONTRACT (headless proof relies on all of this):
 *   - Integer math only. One xorshift32 PRNG, fixed seed 0xC1DE5EED
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
 *   - START from win/lose restarts the SAME seeded run.
 *
 * Telemetry mailbox for the headless harness (tools/headless-screenshot.py
 * --elf --watch cv:cv_telemetry:16): a volatile 16-word array with
 * C-linkage symbol `cv_telemetry`, updated every frame:
 *   [0] 0x43494E44 'CIND'   magic     [8]  kills this run
 *   [1] 0x56414C54 'VALT'   magic     [9]  score (live formula)
 *   [2] state (0 title, 1 playing,    [10] turn count
 *       2 win, 3 lose)                [11] player x (grid)
 *   [3] PRNG seed constant            [12] player y (grid)
 *   [4] floor (1..5)                  [13] lose reason (0 none,
 *   [5] player hp                          1 darkness, 2 slain)
 *   [6] torch remaining               [14] monsters alive on floor
 *   [7] embers collected              [15] frames since boot
 *
 * Presentation is deliberately trivial (breadth-program one-night slice):
 * the dungeon is drawn as glyph rows in Butano's common FIXED 8x8 sprite
 * font (fixed pitch keeps the grid square); HUD/title/end text uses the
 * common variable 8x8 font (the one tools/headless-screenshot.py
 * --assert-text can read). All code original; fonts are Butano's
 * zlib-licensed common assets (third_party/butano/common).
 */

#include "bn_core.h"
#include "bn_color.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_vector.h"
#include "bn_display.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

#include "common_fixed_8x8_sprite_font.h"
#include "common_variable_8x8_sprite_font.h"

extern "C"
{
    // Headless telemetry mailbox — see the layout table in the header
    // comment. volatile so every write really lands for the emulator bus.
    volatile unsigned cv_telemetry[16];
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
    constexpr int max_monsters = 8;

    // Monsters per floor 1..5 (floor 3+ also upgrades slot 0 to a big 'M').
    constexpr int monster_count[last_floor] = {2, 2, 3, 3, 4};

    enum tile_t : unsigned char
    {
        t_wall = 0,
        t_floor = 1,
        t_ember = 2,
        t_stairs = 3,
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
}

int main()
{
    bn::core::init();

    // Vault-dark backdrop: near-black warm brown, unmistakably not blank.
    bn::bg_palettes::set_transparent_color(bn::color(4, 2, 2));

    bn::sprite_text_generator map_gen(common::fixed_8x8_sprite_font);
    map_gen.set_left_alignment();
    bn::sprite_text_generator ui_gen(common::variable_8x8_sprite_font);
    ui_gen.set_left_alignment();

    constexpr int map_x = -52;            // 13 cols x 8 px, centered
    constexpr int map_y0 = -70;           // rows at -70, -60, ... 0
    text_line map_lines[map_h];

    constexpr int ui_count = 5;
    constexpr int ui_y[ui_count] = {-50, -30, -10, 10, 30};
    constexpr int ui_x = -104;
    constexpr int hud_x = -116;           // HUD hugs the left edge: the
                                          // longest HUD string must fit the
                                          // 240px screen (glyphs clip at the
                                          // right edge and break --assert-text)
    constexpr int hud_y = 40;             // HUD slot is ui_lines[4] in play
    text_line ui_lines[ui_count];

    auto clear_lines = [&]()
    {
        for(int i = 0; i < map_h; ++i)
        {
            map_lines[i].clear();
        }

        for(int i = 0; i < ui_count; ++i)
        {
            ui_lines[i].clear();
        }
    };

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

    auto generate_floor = [&]()
    {
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

        for(int step = 0; step < carve_steps; ++step)
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
        for(int e = 0; e < embers_per_floor; ++e)
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
        int count = monster_count[floor_no - 1];

        for(int m = 0; m < count; ++m)
        {
            bool big = floor_no >= 3 && m == 0;
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
    };

    auto reset_run = [&]()
    {
        rng.s = seed_constant;
        floor_no = 1;
        hp = start_hp;
        torch = start_torch;
        embers = 0;
        kills = 0;
        turns = 0;
        lose_reason = 0;
        state = st_playing;
        generate_floor();
        clear_lines();
    };

    auto monster_phase = [&]()
    {
        for(int i = 0; i < max_monsters; ++i)
        {
            monster_t& m = monsters[i];

            if(! m.alive)
            {
                continue;
            }

            int dx = px - m.x;
            int dy = py - m.y;
            int adx = dx > 0 ? dx : -dx;
            int ady = dy > 0 ? dy : -dy;

            if(adx + ady == 1)
            {
                --hp;  // bite
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
                    // Bump combat.
                    acted = true;
                    monsters[mi].hp -= bump_damage;

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
                    --torch;

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
            ui_lines[0].set(ui_gen, ui_x, ui_y[0], "CINDERVAULT");
            ui_lines[1].set(ui_gen, ui_x, ui_y[1], "SEED C1DE5EED");
            ui_lines[2].set(ui_gen, ui_x, ui_y[2], "EVERY STEP BURNS THE TORCH");
            ui_lines[3].set(ui_gen, ui_x, ui_y[3], "DIVE 5 FLOORS TO THE VAULT");
            ui_lines[4].set(ui_gen, ui_x, ui_y[4], "PRESS START");
            break;

        case st_playing:
        {
            for(int y = 0; y < map_h; ++y)
            {
                bn::string<40> row;

                for(int x = 0; x < map_w; ++x)
                {
                    char glyph;

                    switch(tiles[y][x])
                    {
                    case t_floor:  glyph = '.'; break;
                    case t_ember:  glyph = '*'; break;
                    case t_stairs: glyph = '>'; break;
                    default:       glyph = '#'; break;
                    }

                    int mi = monster_at(x, y);

                    if(mi >= 0)
                    {
                        glyph = monsters[mi].big ? 'M' : 'm';
                    }

                    if(x == px && y == py)
                    {
                        glyph = '@';
                    }

                    row.append(glyph);
                }

                map_lines[y].set(map_gen, map_x, map_y0 + 10 * y, row);
            }

            bn::string<40> hud("FLR ");
            hud.append(bn::to_string<8>(floor_no));
            hud.append(" HP ");
            hud.append(bn::to_string<8>(hp));
            hud.append(" TORCH ");
            hud.append(bn::to_string<8>(torch));
            hud.append(" SCORE ");
            hud.append(bn::to_string<8>(score));
            ui_lines[4].set(ui_gen, hud_x, hud_y, hud);
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
            ui_lines[4].set(ui_gen, ui_x, ui_y[4], "PRESS START");
            break;
        }

        }

        // --- telemetry mailbox: every slot, every frame

        ++frames;
        cv_telemetry[0] = 0x43494E44u;  // 'CIND'
        cv_telemetry[1] = 0x56414C54u;  // 'VALT'
        cv_telemetry[2] = state;
        cv_telemetry[3] = seed_constant;
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

        bn::core::update();
    }
}
