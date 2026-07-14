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
 * --elf --watch sh:sh_telemetry:26), volatile, C-linkage, every frame
 * (words 0-15 FROZEN since #98, 16-23 since #99, 24 since #100; the
 * levels append 25):
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
 *   [16] mode (0 calm/1 hungry/  [20] pred1 x   [21] pred1 y
 *        2 gated/3 tuned level)  [22] pred0 lock (fish idx+1, 0=none)
 *   [17] fish EATEN              [23] pred1 lock
 *   [18] pred0 x (whole px)      [24] fish blocked by a gate wall
 *   [19] pred0 y                      THIS frame (gated/level runs)
 *   [25] level index (0-3, tuned levels only; 0 otherwise)
 *
 * AUDIO (growth rung 5, the concept's LAST cut): five original
 * synthesized cues (audio/generate_audio.py — deterministic, no
 * samples ever) played through Butano's maxmod pipeline as pure
 * DECISIONS on events the sim already computed: run start / a fish
 * banks / a straggler eaten / the shoal home / the shoal scattered.
 * Nothing feeds back into the sim. Every trigger bumps a cumulative
 * gl_audio_hook counter (games/common/include/gl_audio_hook.h):
 *   [0] run starts   [1] fish banked (cumulative, += per fish)
 *   [2] fish eaten   [3] wins   [4] losses   [5] mute state (B)
 * so the headless harness pins the decisions and the mixer-memory
 * nonzero watch proves the voicing (docs/capabilities.md).
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
#include "bn_sound_items.h"
#include "bn_sprite_text_generator.h"

#include "shoal_flock.h"
#include "gl_audio_hook.h"

#include "common_fixed_8x8_sprite_font.h"
#include "common_variable_8x8_sprite_font.h"

extern "C"
{
    // Headless telemetry mailbox — layout in the header comment.
    volatile unsigned sh_telemetry[26];
}

namespace
{
    enum state_t : unsigned
    {
        st_title = 0,
        st_herding = 1,
        st_home = 2,
        st_scattered = 3,            // hungry water only: the goal
    };                               //   became unreachable

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
    constexpr int save_goal = 40;        // calm water (carried pins)
    constexpr int save_goal_hungry = 35; // the hungry water asks for 35
                                         // home: measured — vs two
                                         // threshold-locked hunters the
                                         // committed sweep banks 35 with
                                         // 8 lost; 40 is not reachable
                                         // without scattering the
                                         // remnant into their jaws (the
                                         // concept's own "star rating =
                                         // fish saved" grading, per
                                         // water)
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

    // --- star ratings proper (growth rung 3) --------------------------------
    // The committed concept's own sentence, mechanized: "Star rating =
    // fish saved" — graded PER WATER (the rung-1 precedent), as a pure
    // display-time lookup. The 3-star line IS each water's existing
    // goal constant (the coupled hungry knob 44px/300/35 is untouched
    // — no new tuning enters the sim); 2 and 1 stars sit at thirds of
    // it, rounded up: calm/gated 14/27/40, hungry 12/24/35. Losses
    // finally grade too — that is what makes the ratings PROPER: a
    // scattered shoal still banks its stars, and a goalless run reads
    // "-". Display-only: the sim never consults a star.
    constexpr int stars_of(unsigned saved_count, int goal)
    {
        return int(saved_count) >= goal ? 3
             : int(saved_count) >= (goal * 2 + 2) / 3 ? 2
             : int(saved_count) >= (goal + 2) / 3 ? 1 : 0;
    }

    void append_stars(bn::string<40>& text, int stars)
    {
        if(stars == 0)
        {
            text.append("-");
            return;
        }

        for(int i = 0; i < stars; ++i)
        {
            text.append("*");
        }
    }

    // --- multiple tuned levels (growth rung 4) ------------------------------
    // The committed concept's S3 scope ("4 levels with tuned flock
    // parameters"), on a fourth verb: L. Each level is a DISTINCT,
    // deliberate parameter set over the shipped mechanics — hunter
    // count + the coupled knob triple (straggler ring / den time /
    // goal, retuned ALL THREE TOGETHER per level, the #99 rule) and
    // gate geometry (the first N committed walls). The carried waters
    // never read this table. Justifications live in the session card;
    // every goal is priced by a committed win route in proofs.sh.
    struct level_def
    {
        const char* name;
        int hunters;
        int straggle_r2;                 // knob 1 (0 when hunters = 0)
        int cooldown;                    // knob 2
        int gates;
        int goal;                        // knob 3 (stars derive from it)
    };

    constexpr int level_count = 4;
    constexpr level_def level_defs[level_count] = {
        {"THE SHALLOWS", 0, 0, 0, 1, 40},
        {"THE HUNT", 1, 44 * 44, 300, 0, 36},
        {"THE NARROWS", 1, 52 * 52, 360, 2, 32},
        {"DEEP WATER", 2, 52 * 52, 420, 2, 28},
    };
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
    text_line ui_lines[10];
    text_line reef_marks[3];
    text_line cursor_glyph;
    text_line pred_glyphs[sh_predators]; // 'X' hunters (hungry water)

    // The gate walls (THE GATED RUN only): '#' coral glyphs, generated
    // ONCE at run start like the fish sprites (no per-frame text cost)
    // — 8 px rows over the water column, skipping each wall's gap.
    bn::vector<bn::sprite_ptr, 40> gate_sprites;

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

        for(auto& g : pred_glyphs)
        {
            g.clear();
        }

        fish_sprites.clear();
        gate_sprites.clear();
    };

    // --- run state (reset_run() restores the exact boot school) ------------
    unsigned state = st_title;
    unsigned frames = 0;                 // monotonic since boot
    unsigned max_cpu = 0;                // worst last_cpu_usage.data() seen
    fish_t fish[fish_count];
    sh_pred preds[sh_predators];
    unsigned mode = 0;                   // 0 calm (START) / 1 hungry
                                         // (SELECT) / 2 gated (R)
    unsigned eaten = 0;
    unsigned gate_blocked = 0;           // fish pressed into coral this frame
    unsigned level = 0;                  // tuned-levels index (mode 3 only)
    bool muted = false;                  // B toggles; hook counters
                                         // bump regardless (the audio
                                         // DECISION is always proven)
    bool start_cue_pending = false;      // the run-start cue fires on
                                         // the FIRST herding frame, not
                                         // the spawn frame: that frame
                                         // already skips vblanks (the
                                         // 50-sprite generation spike)
                                         // and sits on a boundary — the
                                         // boot-lag trap (#102 card)
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

        for(int p = 0; p < sh_predators; ++p)
        {
            preds[p].x = sh_pred_den_x[p] * fp;
            preds[p].y = sh_pred_den_y[p] * fp;
            preds[p].target = -1;
            preds[p].cooldown = 0;
        }

        eaten = 0;
        gate_blocked = 0;
        cursor_x = 40;
        cursor_y = 80;
        pushing = false;
        saved = 0;
        run_frames = 0;
    };

    while(true)
    {
        bool start = bn::keypad::start_pressed();
        bool select = bn::keypad::select_pressed();
        bool rkey = bn::keypad::r_pressed();
        bool lkey = bn::keypad::l_pressed();

        if(bn::keypad::b_pressed())
        {
            muted = ! muted;             // gates play() ONLY — never
        }                                //   the hook counters

        switch(state)
        {

        case st_title:
        case st_home:
        case st_scattered:
            if(start || select || rkey || lkey)
            {
                unsigned prev_mode = mode;          // SELECT: hungry /
                mode = select ? 1                   // R: the gated run /
                     : (rkey ? 2 : (lkey ? 3 : 0)); // L: tuned levels

                if(mode == 3)
                {
                    // L from a level's WIN card advances, from its
                    // SCATTERED card retries, from anywhere else
                    // starts at level 1.
                    level = prev_mode != 3 || state == st_title ? 0
                          : state == st_home ? (level + 1) % level_count
                          : level;
                }

                reset_run();
                clear_lines();

                bn::string<40> glyph(">");

                for(int i = 0; i < fish_count; ++i)
                {
                    bn::vector<bn::sprite_ptr, 4> out;
                    map_gen.generate(0, 0, glyph, out);
                    fish_sprites.push_back(bn::move(out[0]));
                }

                int gates_now = mode == 2 ? sh_gates
                              : mode == 3 ? level_defs[level].gates : 0;

                if(gates_now > 0)
                {
                    // The coral walls, laid once (static geometry — the
                    // sprites never move; collision is the constants).
                    bn::string<40> coral("#");

                    for(int g = 0; g < gates_now; ++g)
                    {
                        for(int y = water_y0 + 4; y <= water_y1 - 4;
                            y += 8)
                        {
                            if(y >= sh_gate_gap_y0[g]
                               && y <= sh_gate_gap_y1[g])
                            {
                                continue;
                            }

                            bn::vector<bn::sprite_ptr, 4> out;
                            map_gen.generate(scr_x(sh_gate_x[g]) - 4,
                                             scr_y(y), coral, out);
                            gate_sprites.push_back(bn::move(out[0]));
                        }
                    }
                }

                start_cue_pending = true;

                state = st_herding;
            }
            break;

        case st_herding:
        {
            ++run_frames;

            if(start_cue_pending)
            {
                start_cue_pending = false;

                if(! muted)
                {
                    bn::sound_items::sh_start.play(bn::fixed(0.8));
                }

                gl::count_audio(0);      // run starts
            }

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
            int banked = sh_flock_update(fish, cursor_x, cursor_y,
                                         pushing);
            saved += unsigned(banked);

            if(banked > 0)
            {
                if(! muted)
                {
                    bn::sound_items::sh_save.play(bn::fixed(0.5));
                }

                gl_audio_hook[1] = gl_audio_hook[1] + unsigned(banked);
            }

            // --- the hunters (hungry water: the shipped knob verbatim,
            // so every carried pin holds; tuned levels: the level's own
            // triple; calm/gated water never runs this).
            int taken = 0;

            if(mode == 1)
            {
                taken = sh_pred_update(fish, preds, run_frames,
                                       sh_straggle_r2,
                                       sh_pred_cooldown,
                                       sh_predators);
            }
            else if(mode == 3 && level_defs[level].hunters > 0)
            {
                taken = sh_pred_update(fish, preds, run_frames,
                                       level_defs[level].straggle_r2,
                                       level_defs[level].cooldown,
                                       level_defs[level].hunters);
            }

            eaten += unsigned(taken);

            if(taken > 0)
            {
                if(! muted)
                {
                    bn::sound_items::sh_eaten.play(bn::fixed(0.8));
                }

                gl_audio_hook[2] = gl_audio_hook[2] + unsigned(taken);
            }

            // --- the gates (gated run: all committed walls, so every
            // carried pin holds; tuned levels: the level's first N;
            // calm/hungry water never runs this).
            if(mode == 2)
            {
                gate_blocked = unsigned(sh_gate_update(fish, sh_gates));
            }
            else if(mode == 3 && level_defs[level].gates > 0)
            {
                gate_blocked = unsigned(sh_gate_update(fish,
                                            level_defs[level].gates));
            }

            unsigned goal = mode == 1 ? save_goal_hungry
                          : mode == 3 ? unsigned(level_defs[level].goal)
                          : save_goal;

            if(saved >= goal)
            {
                state = st_home;
                clear_lines();

                if(! muted)
                {
                    bn::sound_items::sh_win.play(bn::fixed(0.9));
                }

                gl::count_audio(3);      // wins
            }
            else if(eaten > 0
                    && fish_count - int(eaten) < int(goal))
            {
                state = st_scattered;    // the goal died with the shoal
                clear_lines();

                if(! muted)
                {
                    bn::sound_items::sh_lose.play(bn::fixed(0.9));
                }

                gl::count_audio(4);      // losses
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
            ui_lines[5].set(ui_gen, ui_x, 24, "SELECT: HUNGRY WATER (35)");
            ui_lines[6].set(ui_gen, ui_x, 36, "R: THE GATED RUN (40)");
            ui_lines[7].set(ui_gen, ui_x, 48, "STARS = FISH SAVED");
            ui_lines[8].set(ui_gen, ui_x, 60, "L: TUNED LEVELS (4)");
            ui_lines[9].set(ui_gen, ui_x, 72, "B: MUTE");
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

            int hunters_now = mode == 1 ? sh_predators
                            : mode == 3 ? level_defs[level].hunters : 0;

            for(int p = 0; p < sh_predators; ++p)
            {
                if(p < hunters_now)
                {
                    bn::string<40> jaws("X");
                    pred_glyphs[p].set(map_gen,
                                       scr_x(preds[p].x / fp) - 4,
                                       scr_y(preds[p].y / fp), jaws);
                }
                else
                {
                    pred_glyphs[p].clear();
                }
            }

            unsigned goal_now = mode == 1 ? save_goal_hungry
                              : mode == 3 ? unsigned(level_defs[level].goal)
                              : save_goal;
            bn::string<40> hud;

            if(mode == 3)
            {
                hud.append("L");
                hud.append(bn::to_string<8>(level + 1));
                hud.append(" ");
            }

            hud.append("SAVED ");
            hud.append(bn::to_string<8>(saved));
            hud.append("/");
            hud.append(bn::to_string<8>(goal_now));
            hud.append(" ");
            append_stars(hud, stars_of(saved, int(goal_now))); // live

            if(mode == 1 || hunters_now > 0)
            {
                hud.append("  LOST ");
                hud.append(bn::to_string<8>(eaten));
            }

            hud.append("  CLOCK ");
            hud.append(bn::to_string<8>(run_frames / 60));
            hud.append("s");

            if(muted)
            {
                hud.append("  MUTE");
            }

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
            bn::string<40> stars_line("RATING ");
            append_stars(stars_line, stars_of(saved,
                mode == 1 ? save_goal_hungry
              : mode == 3 ? level_defs[level].goal : save_goal));
            ui_lines[5].set(ui_gen, ui_x, 24, stars_line);

            if(mode == 3)
            {
                bn::string<40> lv_line("L");
                lv_line.append(bn::to_string<8>(level + 1));
                lv_line.append(" ");
                lv_line.append(level_defs[level].name);
                lv_line.append(" - L: NEXT");
                ui_lines[6].set(ui_gen, ui_x, 40, lv_line);
            }

            break;
        }

        case st_scattered:
        {
            ui_lines[0].set(ui_gen, ui_x, -60, "THE SHOAL SCATTERED");
            bn::string<40> line1("EATEN ");
            line1.append(bn::to_string<8>(eaten));
            line1.append("  SAVED ");
            line1.append(bn::to_string<8>(saved));
            ui_lines[1].set(ui_gen, ui_x, -40, line1);
            bn::string<40> line2("CLOCK ");
            line2.append(bn::to_string<8>(run_frames / 60));
            line2.append("s (");
            line2.append(bn::to_string<8>(run_frames));
            line2.append(" FRAMES)");
            ui_lines[2].set(ui_gen, ui_x, -28, line2);
            ui_lines[3].set(ui_gen, ui_x, -16, "KEEP THE SCHOOL TIGHT");
            ui_lines[4].set(ui_gen, ui_x, 8, "PRESS START");
            bn::string<40> stars_line("RATING ");
            append_stars(stars_line, stars_of(saved,
                mode == 1 ? save_goal_hungry
              : mode == 3 ? level_defs[level].goal : save_goal));
            ui_lines[5].set(ui_gen, ui_x, 24, stars_line);

            if(mode == 3)
            {
                bn::string<40> lv_line("L");
                lv_line.append(bn::to_string<8>(level + 1));
                lv_line.append(" ");
                lv_line.append(level_defs[level].name);
                lv_line.append(" - L: RETRY");
                ui_lines[6].set(ui_gen, ui_x, 40, lv_line);
            }

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
        // slice 2 (extension; words 0-15 stay pinned)
        sh_telemetry[16] = mode;
        sh_telemetry[17] = eaten;
        sh_telemetry[18] = unsigned(preds[0].x / fp);
        sh_telemetry[19] = unsigned(preds[0].y / fp);
        sh_telemetry[20] = unsigned(preds[1].x / fp);
        sh_telemetry[21] = unsigned(preds[1].y / fp);
        sh_telemetry[22] = unsigned(preds[0].target + 1);
        sh_telemetry[23] = unsigned(preds[1].target + 1);
        // slice 3 (extension; words 0-23 stay pinned)
        sh_telemetry[24] = gate_blocked;
        // slice 4 (extension; words 0-24 stay pinned)
        sh_telemetry[25] = mode == 3 ? level : 0u;
        // slice 5: the audio decision layer's mute flag rides the hook
        gl_audio_hook[5] = muted ? 1u : 0u;

        bn::core::update();
    }
}
