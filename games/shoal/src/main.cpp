/*
 * SHOAL (game-lab Track B, breadth program game #8)
 *
 * Flock-herding action puzzler, original IP — the ORDER-001 concept
 * queue's LAST item (docs/concepts/session-1-concepts.md), built
 * BEHIND ITS OWN COMMITTED GATE: "The flock is the game — if 50 boids
 * don't fit the CPU/OAM budget the concept degrades fast... profile
 * in the FIRST session, pivot cheap if the budget says no."
 *
 * THE GATE, MECHANIZED: this ROM measures itself. Butano's core CPU
 * meter (bn::core::last_cpu_usage — "the Butano profiler from
 * session 1" the concept cites) is written into the telemetry mailbox
 * EVERY frame, plus a power-on maximum — so the headless harness pins
 * the real measured cost of the full O(n^2) 50-boid flock on emulated
 * hardware, and the profiling verdict is a regression-tested number,
 * not a claim. OAM cost is structural: 50 one-glyph fish sprites +
 * cursor + HUD text comfortably under the 128-sprite budget (Butano
 * manages allocation; a failure to allocate would assert loudly).
 *
 * THE GAME (concept S1 scope + the smallest win loop): 50 fish swim a
 * boids flock — cohesion / alignment / separation, full O(n^2)
 * neighborhood in dropped-precision pixel math (the concept's own
 * "16.16->8.8 precision drop" mitigation applied from frame one) —
 * and YOU are a current: D-pad steers the cursor, holding A pushes
 * the water outward around it. Herd the school past nothing at all
 * (predators are the committed S2 rung, deliberately cut) into the
 * SAFE REEF on the right edge; a fish that reaches the reef settles
 * and is SAVED. Save 40 of the 50 and the shoal is home — the clock
 * is your score. START restarts the same seeded school.
 *
 * DETERMINISM CONTRACT (headless proof relies on all of this):
 *   - One xorshift32 PRNG at fixed seed 0x5H0A1... (0x510A17E5),
 *     consumed ONLY at school init, in a fixed order. No other RNG.
 *   - Integer math only: positions/velocities in 8.8 px; the O(n^2)
 *     pair pass runs in WHOLE-PIXEL space (the precision-drop
 *     mitigation — and the overflow rail: 240^2 pair distances fit
 *     32 bits with room).
 *   - The sim steps once per frame off that frame's input; the same
 *     input script replays bit-identically.
 *
 * Telemetry mailbox for the headless harness (tools/headless-screenshot.py
 * --elf --watch sh:sh_telemetry:16), volatile, C-linkage, every frame:
 *   [0] 0x53484F41 'SHOA' magic     [8]  flock centroid x (whole px)
 *   [1] 0x4C524546 'LREF' magic     [9]  flock centroid y (whole px)
 *   [2] state (0 title, 1 herding,  [10] fish[0] x (8.8 px)
 *       2 shoal-home win)           [11] fish[0] y (8.8 px)
 *   [3] frames since boot           [12] cursor x (whole px)
 *   [4] last_cpu_usage (bn::fixed   [13] cursor y (whole px)
 *       .data(): 4096 = 100% of     [14] current active (A held)
 *       one frame)                  [15] run frames elapsed (the
 *   [5] max cpu usage since boot         clock the reef stamps)
 *   [6] fish still at sea
 *   [7] fish SAVED in the reef
 *
 * Presentation is deliberately trivial (breadth-program slice): fish
 * are one-glyph text sprites ('>') moved per frame, the cursor is
 * '+', the reef a glyph column; HUD/title/win text uses the common
 * variable 8x8 font (--assert-text readable). All code original;
 * fonts are Butano's zlib-licensed common assets.
 */

#include "bn_core.h"
#include "bn_color.h"
#include "bn_fixed.h"
#include "bn_keypad.h"
#include "bn_string.h"
#include "bn_vector.h"
#include "bn_display.h"
#include "bn_bg_palettes.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"

#include "shoal_flock.h"

#include "common_fixed_8x8_sprite_font.h"
#include "common_variable_8x8_sprite_font.h"

extern "C"
{
    // Headless telemetry mailbox — layout in the header comment.
    volatile unsigned sh_telemetry[16];
}

namespace
{
    enum state_t : unsigned
    {
        st_title = 0,
        st_herding = 1,
        st_home = 2,
    };

    constexpr unsigned seed_constant = 0x510A17E5;  // fixed; init-only

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

    // --- the school -----------------------------------------------------------
    // Sim constants, the fish struct and the flock pass live in
    // shoal_flock.h / shoal_flock.bn_iwram.cpp (the IWRAM hot loop —
    // the gate's measured story is told there). Here: display + goal.
    constexpr int fish_count = sh_fish_count;
    constexpr int save_goal = 40;
    constexpr int fp = sh_fp;
    constexpr int water_x0 = sh_water_x0, water_x1 = sh_water_x1;
    constexpr int water_y0 = sh_water_y0, water_y1 = sh_water_y1;
    constexpr int reef_x = sh_reef_x;
    constexpr int cursor_speed = 2;      // px/frame
    using fish_t = sh_fish;

    struct text_line
    {
        bn::vector<bn::sprite_ptr, 20> sprites;
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

    // Screen conversion: water px (0..240 / 0..160, top-left) -> Butano
    // centered coordinates.
    constexpr int scr_x(int px) { return px - 120; }
    constexpr int scr_y(int px) { return px - 80; }
}

int main()
{
    bn::core::init();

    // Deep-water backdrop.
    bn::bg_palettes::set_transparent_color(bn::color(1, 3, 8));

    bn::sprite_text_generator map_gen(common::fixed_8x8_sprite_font);
    map_gen.set_left_alignment();
    bn::sprite_text_generator ui_gen(common::variable_8x8_sprite_font);
    ui_gen.set_left_alignment();

    constexpr int ui_x = -110;
    text_line ui_lines[5];
    text_line reef_marks[3];
    text_line cursor_glyph;

    // One '>' sprite per fish, generated once and MOVED per frame (the
    // no-assets trick without per-frame text regeneration). 50 fish +
    // cursor + HUD text sprites sit far under the 128-OAM budget.
    bn::vector<bn::sprite_ptr, fish_count> fish_sprites;

    auto clear_lines = [&]()
    {
        for(auto& line : ui_lines)
        {
            line.clear();
        }

        for(auto& line : reef_marks)
        {
            line.clear();
        }

        cursor_glyph.clear();
        fish_sprites.clear();
    };

    // --- run state (reset_run() restores the exact boot school) ------------
    unsigned state = st_title;
    unsigned frames = 0;                 // monotonic since boot
    unsigned max_cpu = 0;                // worst last_cpu_usage.data() seen
    fish_t fish[fish_count];
    int cursor_x = 40, cursor_y = 80;    // whole px
    bool pushing = false;
    unsigned saved = 0;
    unsigned run_frames = 0;

    auto reset_run = [&]()
    {
        // The SAME school every run: the PRNG restarts at the fixed
        // seed and is consumed only here, in a fixed order.
        rng_t rng(seed_constant);

        for(auto& f : fish)
        {
            f.x = (24 + rng.range(72)) * fp;      // left-water school
            f.y = (36 + rng.range(88)) * fp;
            f.vx = rng.range(129) - 64;           // -64..64 units
            f.vy = rng.range(129) - 64;
            f.saved = false;
        }

        cursor_x = 40;
        cursor_y = 80;
        pushing = false;
        saved = 0;
        run_frames = 0;
    };

    while(true)
    {
        bool start = bn::keypad::start_pressed();

        switch(state)
        {

        case st_title:
        case st_home:
            if(start)
            {
                reset_run();
                clear_lines();

                bn::string<40> glyph(">");

                for(int i = 0; i < fish_count; ++i)
                {
                    bn::vector<bn::sprite_ptr, 4> out;
                    map_gen.generate(0, 0, glyph, out);
                    fish_sprites.push_back(bn::move(out[0]));
                }

                state = st_herding;
            }
            break;

        case st_herding:
        {
            ++run_frames;

            // Cursor: the current you steer.
            if(bn::keypad::left_held() && cursor_x > water_x0)
            {
                cursor_x -= cursor_speed;
            }

            if(bn::keypad::right_held() && cursor_x < water_x1)
            {
                cursor_x += cursor_speed;
            }

            if(bn::keypad::up_held() && cursor_y > water_y0)
            {
                cursor_y -= cursor_speed;
            }

            if(bn::keypad::down_held() && cursor_y < water_y1)
            {
                cursor_y += cursor_speed;
            }

            pushing = bn::keypad::a_held();

            // --- the flock: one deterministic IWRAM pass (see
            // shoal_flock.h for the measured ladder that shaped it).
            saved += unsigned(sh_flock_update(fish, cursor_x, cursor_y,
                                              pushing));

            if(saved >= save_goal)
            {
                state = st_home;
                clear_lines();
            }

            break;
        }

        }

        // --- draw ------------------------------------------------------------

        switch(state)
        {

        case st_title:
            ui_lines[0].set(ui_gen, ui_x, -60, "SHOAL");
            ui_lines[1].set(ui_gen, ui_x, -40, "YOU ARE A CURRENT");
            ui_lines[2].set(ui_gen, ui_x, -28, "DPAD STEERS - A PUSHES");
            ui_lines[3].set(ui_gen, ui_x, -16, "HERD 40 FISH TO THE REEF");
            ui_lines[4].set(ui_gen, ui_x, 8, "PRESS START");
            break;

        case st_herding:
        {
            // The reef, marked once (three glyph columns on the right).
            bn::string<40> reef_col("||");
            reef_marks[0].set(map_gen, scr_x(reef_x), scr_y(40), reef_col);
            reef_marks[1].set(map_gen, scr_x(reef_x), scr_y(80), reef_col);
            reef_marks[2].set(map_gen, scr_x(reef_x), scr_y(120), reef_col);

            for(int i = 0; i < fish_count; ++i)
            {
                bn::sprite_ptr& spr = fish_sprites[i];

                if(fish[i].saved)
                {
                    spr.set_visible(false);
                }
                else
                {
                    spr.set_position(scr_x(fish[i].x / fp),
                                     scr_y(fish[i].y / fp));
                }
            }

            bn::string<40> cur("+");
            cursor_glyph.set(map_gen, scr_x(cursor_x) - 4,
                             scr_y(cursor_y), cur);

            bn::string<40> hud("SAVED ");
            hud.append(bn::to_string<8>(saved));
            hud.append("/50  CLOCK ");
            hud.append(bn::to_string<8>(run_frames / 60));
            hud.append("s");
            ui_lines[0].set(ui_gen, ui_x, 66, hud);
            break;
        }

        case st_home:
        {
            ui_lines[0].set(ui_gen, ui_x, -60, "THE SHOAL IS HOME");
            bn::string<40> line1("SAVED ");
            line1.append(bn::to_string<8>(saved));
            line1.append("/50");
            ui_lines[1].set(ui_gen, ui_x, -40, line1);
            bn::string<40> line2("CLOCK ");
            line2.append(bn::to_string<8>(run_frames / 60));
            line2.append("s (");
            line2.append(bn::to_string<8>(run_frames));
            line2.append(" FRAMES)");
            ui_lines[2].set(ui_gen, ui_x, -28, line2);
            ui_lines[3].set(ui_gen, ui_x, -16, "THE TIDE THANKS YOU");
            ui_lines[4].set(ui_gen, ui_x, 8, "PRESS START");
            break;
        }

        }

        // --- telemetry mailbox: every slot, every frame — INCLUDING the
        // profiler words the concept's gate demands.

        unsigned cpu = unsigned(bn::core::last_cpu_usage().data());

        if(state == st_herding && run_frames > 1 && cpu > max_cpu)
        {
            max_cpu = cpu;               // steady-state only: the first
        }                                // herding frame pays the one-time
                                         // 50-sprite generation + console
                                         // clear (a transition, not the
                                         // flock) — the budget word must
                                         // measure the flock

        int at_sea = 0;
        long long cen_x = 0, cen_y = 0;

        for(const fish_t& f : fish)
        {
            if(! f.saved)
            {
                ++at_sea;
                cen_x += f.x / fp;
                cen_y += f.y / fp;
            }
        }

        if(at_sea > 0)
        {
            cen_x /= at_sea;
            cen_y /= at_sea;
        }

        ++frames;
        sh_telemetry[0] = 0x53484F41u;   // 'SHOA'
        sh_telemetry[1] = 0x4C524546u;   // 'LREF'
        sh_telemetry[2] = state;
        sh_telemetry[3] = frames;
        sh_telemetry[4] = cpu;
        sh_telemetry[5] = max_cpu;
        sh_telemetry[6] = unsigned(at_sea);
        sh_telemetry[7] = saved;
        sh_telemetry[8] = unsigned(cen_x);
        sh_telemetry[9] = unsigned(cen_y);
        sh_telemetry[10] = unsigned(fish[0].x);
        sh_telemetry[11] = unsigned(fish[0].y);
        sh_telemetry[12] = unsigned(cursor_x);
        sh_telemetry[13] = unsigned(cursor_y);
        sh_telemetry[14] = pushing ? 1u : 0u;
        sh_telemetry[15] = run_frames;

        bn::core::update();
    }
}
