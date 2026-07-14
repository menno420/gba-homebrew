/*
 * DEEPCAST (game-lab Track B, breadth program game #4)
 *
 * Single-mechanic fishing / line-tension-management arcade, original IP.
 * A quiet lake at dusk. HOLD A to charge a cast — hold time IS the target
 * depth. The lure sinks; a seeded fish bites by depth (deeper = heavier =
 * worth more; hooking is automatic and forgiving — the game is the FIGHT,
 * not the reflex). In the fight: HOLD A to reel (tension rises, line
 * shortens), RELEASE to let the fish run (tension falls, line pays out).
 * The fish alternates seeded REST and SURGE phases — reel through the
 * rests, yield to the surges. Tension maxed = the line SNAPS (fish lost +
 * one of 3 lures); line reeled to zero = CATCH, score += weight. The run
 * ends when all 3 lures are gone.
 *
 * DETERMINISM CONTRACT (headless proof relies on all of this):
 *   - Integer math only. One xorshift32 PRNG, boot seed 0xDEE9CA57
 *     (shown on the title card). RNG words are consumed ONLY at pinned
 *     events (cast: bite delay + fish weight; fight: phase lengths), in a
 *     fixed order, so the same input script replays bit-identically.
 *   - Frame-count-driven: no wall clock, no interrupts feeding the sim.
 *   - START from game over restarts the SAME seeded run.
 *
 * SEED-SELECT SCORE-ATTACK (growth cut 2 — the concept's "daily seed +
 * score-attack" line, GBA-shaped: a cartridge has no clock and no server,
 * so "daily" becomes a DIALABLE seed): on the title, the seed line is a
 * picker — UP/DOWN dials the seed +-1, LEFT/RIGHT +-0x100, L/R +-0x10000
 * (all edge-triggered, 32-bit wrapping; the xorshift dead state 0 is
 * skipped). Two players who dial the same 8 hex digits fish the SAME
 * lake — same bites, same weights, same surge rhythm — and the seed is
 * repeated on the dusk score card, so a score is a claim anyone with a
 * cartridge can check. The boot seed stays 0xDEE9CA57: with no dial
 * input the game is bit-identical to v0.2. START from game over restarts
 * the same dialed seed; the picker only lives on the title.
 *
 * Telemetry mailbox for the headless harness (tools/headless-screenshot.py
 * --elf --watch dc:dc_telemetry:16): a volatile 16-word array with
 * C-linkage symbol `dc_telemetry`, updated every frame:
 *   [0] 0x44454550 'DEEP'   magic     [8]  tension (0..600 snap)
 *   [1] 0x43415354 'CAST'   magic     [9]  line remaining (catch at 0)
 *   [2] state id (enum State)         [10] current/last fish weight
 *   [3] frames since boot (monotonic) [11] fish phase (1 surge, 0 rest)
 *   [4] selected PRNG seed (boot      [12] lures remaining
 *       0xDEE9CA57; title dial live)
 *   [5] charge frames (this cast)     [13] catches this run
 *   [6] lure depth (m)                [14] score this run
 *   [7] target depth (m)              [15] last fight result (1 catch, 2 snap)
 *
 * AUDIO (growth cut 1 — the concept's named line: "reel clicks that
 * speed up with tension"): four original synthesized cues
 * (audio/generate_audio.py — deterministic, no samples ever) played
 * through Butano's maxmod pipeline as pure DECISIONS on state the sim
 * already computed. THE cut: while reeling in the fight, a dry ratchet
 * click fires every click-interval frames, and the interval shrinks
 * linearly with tension (16 frames at slack -> 4 at the snap point) while
 * the click's pitch rises — the tension bar, readable with eyes closed.
 * Supporting cues on the loop's three events: the bite, the catch, the
 * snap. Nothing feeds back into the sim (no RNG, no state writes).
 * B toggles MUTE (gates play() ONLY — counters still bump, so the
 * decision layer stays provable muted or not). Every trigger bumps a
 * cumulative gl_audio_hook slot (games/common/include/gl_audio_hook.h):
 *   [0] reel clicks   [1] bites   [2] catches   [3] snaps
 *   [4] click interval THIS frame (0 = not reeling; the speed-up word)
 *   [5] mute state
 * so the headless harness pins the decisions and the maxmod
 * mixer-memory nonzero watch proves the voicing (docs/capabilities.md).
 *
 * Presentation is deliberately trivial (breadth-program one-night slice):
 * Butano common sprite-text font over a dusk backdrop color, text bars for
 * charge/tension. All code original; font is Butano's zlib-licensed common
 * asset (third_party/butano/common).
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

#include "gl_audio_hook.h"

#include "common_variable_8x8_sprite_font.h"

extern "C"
{
    // Headless telemetry mailbox — see the layout table in the header
    // comment. volatile so every write really lands for the emulator bus.
    volatile unsigned dc_telemetry[16];
}

namespace
{
    // --- deterministic core -------------------------------------------------

    constexpr unsigned seed_constant = 0xDEE9CA57;

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
        st_title = 0,   // press START
        st_charge = 1,  // hold A to charge the cast, release to cast
        st_sink = 2,    // lure sinks to target depth, seeded bite delay
        st_fight = 3,   // hold A reel / release run
        st_result = 4,  // CATCH / SNAP card, auto-advances
        st_over = 5,    // dusk falls: final score, START restarts
    };

    // Tuning (all integers; see CONCEPT.md for the intended strategy space).
    constexpr int max_charge = 160;        // frames of A-hold, caps the cast
    constexpr int min_depth = 12;          // target depth = min_depth + charge
    constexpr int sink_rate = 2;           // m per frame while sinking
    constexpr int line_per_depth = 6;      // line at hook = depth * this
    constexpr int tension_snap = 600;      // tension >= this -> line snaps
    constexpr int reel_line = 5;           // line lost per reeling frame
    constexpr int reel_tension_base = 6;   // tension gained per reeling frame...
    constexpr int surge_tension_extra = 18;// ...plus this while the fish surges
    constexpr int slack_tension = 9;       // tension lost per released frame
    constexpr int run_rest = 1;            // line paid out per released frame...
    constexpr int run_surge = 2;           // ...or this while the fish surges
    constexpr int result_frames = 90;      // RESULT card duration
    constexpr int start_lures = 3;

    // Audio (pure decision layer — see the header comment). The reel
    // click period shrinks linearly with tension: every 16 frames at
    // slack, every 4 at the snap point (~4 -> ~15 clicks/second).
    constexpr int click_max_interval = 16; // frames between clicks, tension 0
    constexpr int click_min_interval = 4;  // ...at tension_snap

    // --- text presentation ---------------------------------------------------

    struct text_line
    {
        bn::vector<bn::sprite_ptr, 24> sprites;
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

    bn::string<40> bar_string(const char* label, int value, int max_value)
    {
        // "LABEL [####------]" — 10 cells, deterministic integer fill.
        bn::string<40> out(label);
        out.append(" [");
        int cells = (value * 10) / max_value;

        if(cells > 10)
        {
            cells = 10;
        }

        for(int i = 0; i < 10; ++i)
        {
            out.append(i < cells ? "#" : "-");
        }

        out.append("]");
        return out;
    }

    bn::string<40> seed_hex(unsigned value)
    {
        // The seed as exactly 8 uppercase hex digits — the shareable
        // challenge code two players compare on their title screens.
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

    // Dusk-lake backdrop: deep blue-violet, unmistakably not a blank screen.
    bn::bg_palettes::set_transparent_color(bn::color(3, 3, 9));

    bn::sprite_text_generator text_gen(common::variable_8x8_sprite_font);
    text_gen.set_left_alignment();

    constexpr int text_x = -104;
    constexpr int line_count = 6;
    text_line lines[line_count];  // y slots: -60 -40 -20 0 20 40
    constexpr int line_y[line_count] = {-60, -40, -20, 0, 20, 40};

    auto clear_lines = [&]()
    {
        for(int i = 0; i < line_count; ++i)
        {
            lines[i].clear();
        }
    };

    // Seed-select (growth cut 2): the dialed challenge seed. Boot value is
    // the v0.2 constant, only the title dial changes it, and reset_run()
    // reads it — so with no dial input every run is the boot run, and a
    // dialed value survives game over (START reruns the SAME lake).
    unsigned seed_sel = seed_constant;

    // --- run state (all plain ints; reset_run() restores the exact boot run)
    rng_t rng(seed_constant);
    unsigned state = st_title;
    unsigned frames = 0;        // monotonic since boot, never reset
    int lures = start_lures;
    int catches = 0;
    int score = 0;
    int charge = 0;
    int depth = 0;
    int target = 0;
    int bite_delay = 0;
    int weight = 0;
    int line_start = 0;
    int line = 0;
    int tension = 0;
    int surge = 0;
    int phase_timer = 0;
    int result_code = 0;        // 0 none · 1 catch · 2 snap
    int result_timer = 0;

    // Audio decision layer state (presentation-only: never read by the
    // sim, deliberately NOT reset by reset_run — mute is a player
    // preference, and the click timer restarts with every reel anyway).
    bool muted = false;
    int click_timer = 0;

    auto reset_run = [&]()
    {
        rng.s = seed_sel;
        lures = start_lures;
        catches = 0;
        score = 0;
        charge = 0;
        depth = 0;
        target = 0;
        bite_delay = 0;
        weight = 0;
        line_start = 0;
        line = 0;
        tension = 0;
        surge = 0;
        phase_timer = 0;
        result_code = 0;
        result_timer = 0;
        state = st_charge;
        clear_lines();
    };

    while(true)
    {
        bool a_held = bn::keypad::a_held();

        if(bn::keypad::b_pressed())
        {
            muted = ! muted;    // gates play() only — counters still bump
        }

        switch(state)
        {

        case st_title:
            if(bn::keypad::start_pressed())
            {
                reset_run();
            }
            else
            {
                // The seed dial (edge-triggered so one press is one step —
                // a challenge code is a repeatable press sequence). 32-bit
                // wrap; 0 is xorshift32's dead state, so it is skipped in
                // whichever direction the dial was turning.
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
            break;

        case st_charge:
            if(a_held)
            {
                if(charge < max_charge)
                {
                    ++charge;
                }
            }
            else if(charge > 0)
            {
                // Cast. Both RNG words are consumed HERE, in this order —
                // the pinned event the determinism contract promises.
                target = min_depth + charge;
                bite_delay = 20 + rng.range(80);
                weight = 2 + target / 12 + rng.range(2 + target / 10);
                depth = 0;
                state = st_sink;
                clear_lines();
            }
            break;

        case st_sink:
            if(depth < target)
            {
                depth += sink_rate;

                if(depth > target)
                {
                    depth = target;
                }
            }
            else if(bite_delay > 0)
            {
                --bite_delay;
            }
            else
            {
                // BITE — hooking is automatic (forgiving by design; the
                // game is the fight, not the reflex).
                line_start = target * line_per_depth;
                line = line_start;
                tension = 0;
                surge = 0;
                phase_timer = 45 + rng.range(45);  // opening rest
                state = st_fight;
                clear_lines();

                if(! muted)
                {
                    bn::sound_items::dc_bite.play(bn::fixed(0.7));
                }

                gl::count_audio(1);      // bites
            }
            break;

        case st_fight:
            if(phase_timer > 0)
            {
                --phase_timer;
            }
            else
            {
                surge = surge ? 0 : 1;
                phase_timer = surge ? 30 + rng.range(30) : 45 + rng.range(45);
            }

            if(a_held)
            {
                line -= reel_line;
                tension += reel_tension_base + weight / 8
                        + (surge ? surge_tension_extra : 0);
            }
            else
            {
                line += surge ? run_surge : run_rest;

                if(line > line_start)
                {
                    line = line_start;
                }

                tension -= slack_tension;

                if(tension < 0)
                {
                    tension = 0;
                }
            }

            if(tension >= tension_snap)
            {
                tension = tension_snap;
                result_code = 2;  // SNAP
                --lures;
                result_timer = result_frames;
                state = st_result;
                clear_lines();

                if(! muted)
                {
                    bn::sound_items::dc_snap.play(bn::fixed(0.9));
                }

                gl::count_audio(3);      // snaps
            }
            else if(line <= 0)
            {
                line = 0;
                result_code = 1;  // CATCH
                ++catches;
                score += weight;
                result_timer = result_frames;
                state = st_result;
                clear_lines();

                if(! muted)
                {
                    bn::sound_items::dc_catch.play(bn::fixed(0.9));
                }

                gl::count_audio(2);      // catches
            }
            break;

        case st_result:
            if(--result_timer <= 0)
            {
                charge = 0;
                depth = 0;
                tension = 0;
                state = lures > 0 ? st_charge : st_over;
                clear_lines();
            }
            break;

        case st_over:
            if(bn::keypad::start_pressed())
            {
                reset_run();  // same seed -> the identical run, by contract
            }
            break;

        }

        // --- THE audio cut: reel clicks that speed up with tension.
        // While reeling (fight + A held), a ratchet click fires every
        // click_interval frames; the interval shrinks linearly with the
        // tension the sim just computed, and the click's pitch rises with
        // it. Reads sim state only — never writes it.

        int click_interval = 0;

        if(state == st_fight && a_held)
        {
            click_interval = click_max_interval
                    - ((click_max_interval - click_min_interval) * tension)
                      / tension_snap;

            if(click_timer <= 0)
            {
                if(! muted)
                {
                    // Pitch rides tension too: 1.0x at slack -> 2.0x at
                    // the snap point (deterministic fixed-point math).
                    bn::fixed speed = 1 + bn::fixed(tension) / tension_snap;
                    bn::sound_items::dc_click.play(bn::fixed(0.5), speed, 0);
                }

                gl::count_audio(0);      // reel clicks
                click_timer = click_interval;
            }

            --click_timer;
        }
        else
        {
            click_timer = 0;    // the first reeling frame always clicks
        }

        // --- draw (text only; lines re-render only when their string changes)

        switch(state)
        {

        case st_title:
        {
            lines[0].set(text_gen, text_x, line_y[0], "DEEPCAST");
            lines[1].set(text_gen, text_x, line_y[1], "A QUIET LAKE AT DUSK");
            bn::string<40> seed_line("SEED ");
            seed_line.append(seed_hex(seed_sel));
            lines[2].set(text_gen, text_x, line_y[2], seed_line);
            lines[3].set(text_gen, text_x, line_y[3],
                         "HOLD A: CAST DEEP, REEL SOFT");
            lines[4].set(text_gen, text_x, line_y[4], "3 LURES. PRESS START");
            lines[5].set(text_gen, text_x, line_y[5],
                         "B: MUTE  DPAD L R: DIAL SEED");
            break;
        }

        case st_charge:
        {
            bn::string<40> status("LURES ");
            status.append(bn::to_string<8>(lures));
            status.append("  SCORE ");
            status.append(bn::to_string<8>(score));
            lines[0].set(text_gen, text_x, line_y[0], status);
            lines[1].set(text_gen, text_x, line_y[1],
                         charge > 0 ? "CHARGING..." : "HOLD A TO CAST");
            lines[2].set(text_gen, text_x, line_y[2],
                         bar_string("CAST", charge, max_charge));
            break;
        }

        case st_sink:
        {
            bn::string<40> line0("DEPTH ");
            line0.append(bn::to_string<8>(depth));
            line0.append("M OF ");
            line0.append(bn::to_string<8>(target));
            line0.append("M");
            lines[0].set(text_gen, text_x, line_y[0], line0);
            lines[1].set(text_gen, text_x, line_y[1],
                         depth < target ? "THE LURE SINKS..." : "WAITING...");
            break;
        }

        case st_fight:
        {
            bn::string<40> line0("DEPTH ");
            line0.append(bn::to_string<8>(target));
            line0.append("M  WT ");
            line0.append(bn::to_string<8>(weight));
            lines[0].set(text_gen, text_x, line_y[0], line0);
            lines[1].set(text_gen, text_x, line_y[1],
                         bar_string("TENSION", tension, tension_snap));
            bn::string<40> line2("LINE ");
            line2.append(bn::to_string<8>(line));
            lines[2].set(text_gen, text_x, line_y[2], line2);
            lines[3].set(text_gen, text_x, line_y[3],
                         surge ? "IT SURGES! LET IT RUN"
                               : "IT RESTS. REEL, GENTLY");
            break;
        }

        case st_result:
            if(result_code == 1)
            {
                bn::string<40> line0("CATCH! +");
                line0.append(bn::to_string<8>(weight));
                lines[0].set(text_gen, text_x, line_y[0], line0);
                bn::string<40> line1("SCORE ");
                line1.append(bn::to_string<8>(score));
                lines[1].set(text_gen, text_x, line_y[1], line1);
            }
            else
            {
                lines[0].set(text_gen, text_x, line_y[0], "SNAP! THE LINE BREAKS");
                lines[1].set(text_gen, text_x, line_y[1], "LURE LOST");
            }
            break;

        case st_over:
        {
            lines[0].set(text_gen, text_x, line_y[0], "DUSK FALLS. LURES GONE");
            bn::string<40> line1("SCORE ");
            line1.append(bn::to_string<8>(score));
            lines[1].set(text_gen, text_x, line_y[1], line1);
            bn::string<40> line2("CATCHES ");
            line2.append(bn::to_string<8>(catches));
            lines[2].set(text_gen, text_x, line_y[2], line2);
            bn::string<40> line3("SEED ");
            line3.append(seed_hex(seed_sel));
            lines[3].set(text_gen, text_x, line_y[3], line3);
            lines[4].set(text_gen, text_x, line_y[4], "PRESS START");
            break;
        }

        }

        // --- telemetry mailbox: every slot, every frame

        ++frames;
        dc_telemetry[0] = 0x44454550u;  // 'DEEP'
        dc_telemetry[1] = 0x43415354u;  // 'CAST'
        dc_telemetry[2] = state;
        dc_telemetry[3] = frames;
        dc_telemetry[4] = seed_sel;
        dc_telemetry[5] = unsigned(charge);
        dc_telemetry[6] = unsigned(depth);
        dc_telemetry[7] = unsigned(target);
        dc_telemetry[8] = unsigned(tension);
        dc_telemetry[9] = unsigned(line);
        dc_telemetry[10] = unsigned(weight);
        dc_telemetry[11] = unsigned(surge);
        dc_telemetry[12] = unsigned(lures);
        dc_telemetry[13] = unsigned(catches);
        dc_telemetry[14] = unsigned(score);
        dc_telemetry[15] = unsigned(result_code);

        // Audio evidence hook: per-frame words next to the cumulative
        // trigger counters (slots 0-3 bump at the play sites above).
        gl_audio_hook[4] = unsigned(click_interval);
        gl_audio_hook[5] = muted ? 1u : 0u;

        bn::core::update();
    }
}
