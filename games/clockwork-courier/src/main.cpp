/*
 * CLOCKWORK COURIER (game-lab Track B, breadth program game #7)
 *
 * Rewind-ghost puzzle platformer, original IP — the ORDER-001 concept
 * queue's item 2 (docs/concepts/session-1-concepts.md), prototype
 * slice. A courier robot in a clockwork tower: LEFT/RIGHT walk, A
 * jump (fixed-point gravity + tile collision on one handcrafted
 * single-screen level). THE HOOK: R rewinds YOU 5 seconds (300
 * frames) but leaves a GHOST that replays exactly what you just did.
 * The door 'D' stays open only while the pressure switch 'o' is held
 * down — and the switch counts the ghost's weight, so the solve is to
 * act out your future partner (walk onto the switch, stand there),
 * rewind, and run the parcel 'P' through the door to the chute 'v'
 * while your past self holds it. Deliver = win card, clock stamped;
 * START restarts. No lose state — the tower is patient.
 *
 * THE RUSH ORDER (growth rung 2 — "multiple parcels/chutes with
 * timing windows", the pitch's "timed chutes"): SELECT starts a run
 * with TWO parcels (the classic ledge one plus one waiting on the
 * step) and the chute KEEPS HOURS — its shutter is open only while
 * (run_frames %% 240) < 60 (1 s in every 4). Both parcels through an
 * open window = the rush is done. Pure data + clock: no physics
 * constant moves (the rung-1 reachability rail is load-bearing), and
 * the classic START run never evaluates any of it, so every carried
 * pin holds by construction.
 *
 * PROTOTYPE RULES (deliberate cuts documented in CONCEPT.md):
 *   - One ghost at a time; R fires only when the pose buffer is FULL
 *     (300 recorded playing-frames — the HUD says REWIND READY) and
 *     no ghost is already out. A rewind empties the buffer.
 *   - The ghost replays recorded POSES (not re-simulated inputs), so
 *     replay exactness is BY CONSTRUCTION; it vanishes when its 300
 *     frames run out. It presses switches; it is not (yet) a platform.
 *   - The door never crushes: while any part of the player overlaps a
 *     door cell the door stays open regardless of the switch.
 *
 * DETERMINISM CONTRACT (headless proof relies on all of this):
 *   - No RNG anywhere: a handcrafted level + input-driven physics.
 *     Integer/fixed-point math only (positions in 8.8 pixels).
 *   - The sim steps once per frame off the input latched that frame;
 *     the same input script replays bit-identically — the concept
 *     doc's own bet ("the headless harness can literally assert it"),
 *     and the ghost contract is asserted ON CAMERA: the ghost's
 *     replayed positions pin to the SAME literals as the original
 *     walk 300 frames earlier.
 *
 * Telemetry mailbox for the headless harness (tools/headless-screenshot.py
 * --elf --watch cc:cc_telemetry:16): a volatile 16-word array with
 * C-linkage symbol `cc_telemetry`, updated every frame:
 *   [0] 0x434C4B57 'CLKW' magic      [8]  ghost active (0/1)
 *   [1] 0x434F5552 'COUR' magic      [9]  ghost x (8.8 px)
 *   [2] state (0 title, 1 playing,   [10] ghost y (8.8 px)
 *       2 delivered)                 [11] switch held (0/1)
 *   [3] frames since boot            [12] door open (0/1)
 *   [4] player x (8.8 px, feet mid)  [13] parcel carried (0/1)
 *   [5] player y (8.8 px, feet)     [14] rewinds used this run
 *   [6] on ground (0/1)              [15] run frames elapsed (the
 *   [7] pose buffer fill (0..300)         clock the chute stamps)
 *   [16] standing on the ghost (0/1) — growth rung 1
 *   [17] mode (0 classic START / 1 THE RUSH ORDER, SELECT) — rung 2
 *   [18] parcels delivered this run (rush)   [19] chute window open
 *
 * Presentation is deliberately trivial (breadth-program slice): the
 * tower is glyph rows in Butano's common FIXED 8x8 sprite font; the
 * courier '@' and ghost 'g' are single-glyph pixel-positioned text
 * sprites; HUD/title/win text uses the common variable 8x8 font (the
 * one --assert-text can read). All code original; fonts are Butano's
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
    volatile unsigned cc_telemetry[20];
}

namespace
{
    enum state_t : unsigned
    {
        st_title = 0,
        st_playing = 1,
        st_delivered = 2,
    };

    // --- the tower (one handcrafted single-screen level; NO RNG) -----------
    // 16 columns x 9 rows of 8px cells. '#' solid, 'P' parcel ledge prize,
    // 'o' the pressure switch, 'D' door cells (solid while closed), 'v'
    // the delivery chute, '.' air. The door column is capped by '#' at
    // (12,5) so the only way through is the switch.
    constexpr int map_w = 16;
    constexpr int map_h = 9;
    // The door column is capped by '#' at (12,4) AND (12,5): a max jump
    // rises 26.6 px (feet 64 -> 37.4), so the body always overlaps the
    // row-4 cap while airborne beside it — the door is provably not
    // jumpable (probe-measured: a ONE-cell cap was, and the ghost was
    // optional; the two-cell cap closes that exploit).
    constexpr char level[map_h][map_w + 1] = {
        "################",
        "#...........#..#",
        "#P..........#..#",
        "####........#..#",
        "#...........#.##",
        "#....##.....#..#",   // (6,4) hosts the rush parcel Q
        "#...........D..#",
        "#..o........D.v#",
        "################",
    };

    constexpr int cell_px = 8;

    // Named cells (must match the level art above).
    constexpr int switch_cx = 3, switch_cy = 7;
    constexpr int parcel_cx = 1, parcel_cy = 2;
    constexpr int parcel2_cx = 6, parcel2_cy = 4;   // on the step (rush)
    constexpr int window_period = 240;              // the chute's hours:
    constexpr int window_open_frames = 60;          //   1 s in every 4
    constexpr int chute_cx = 14, chute_cy = 7;
    constexpr int door_cx = 12;
    constexpr int door_cy0 = 6, door_cy1 = 7;

    // --- fixed-point physics (8.8 pixels; all owner-tunable integers) ------
    constexpr int fp = 256;
    constexpr int walk_speed = 320;      // 1.25 px/frame
    constexpr int gravity = 38;          // px/frame^2 in 1/256
    constexpr int jump_v = -720;         // 2.8125 px/frame up = ~3.3 cells
    constexpr int fall_cap = 768;        // terminal 3 px/frame
    constexpr int half_w = 3 * fp;       // hitbox: 6px wide, 8px tall
    constexpr int body_h = 8 * fp;       //   (feet-center anchored)

    // --- the rewind (the whole hook) ----------------------------------------
    constexpr int rewind_frames = 300;   // 5 seconds at 60 fps

    // Spawn: feet-center, standing on the ground row (cells y=7).
    constexpr int spawn_x = 8 * cell_px * fp + 4 * fp;   // middle of cell 8
    constexpr int spawn_y = 8 * cell_px * fp;            // ground top

    struct pose_t
    {
        int x = 0;
        int y = 0;
    };

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
}

int main()
{
    bn::core::init();

    // Brass-dark clockwork backdrop: unmistakably not blank.
    bn::bg_palettes::set_transparent_color(bn::color(4, 3, 1));

    bn::sprite_text_generator map_gen(common::fixed_8x8_sprite_font);
    map_gen.set_left_alignment();
    bn::sprite_text_generator ui_gen(common::variable_8x8_sprite_font);
    ui_gen.set_left_alignment();

    // The tower drawn cell-true: 16x8px wide = 128px centered, rows 8px
    // apart (collision matches what you see).
    constexpr int map_x = -64;
    constexpr int map_y0 = -56;          // row centers at -56, -48, ... 8
    text_line map_lines[map_h];

    constexpr int ui_count = 7;
    constexpr int ui_y[ui_count] = {-70, 24, 40, 52, 64, 76, 88};
    constexpr int ui_x = -110;
    text_line ui_lines[ui_count];

    text_line player_glyph;              // '@' at pixel position
    text_line ghost_glyph;               // 'g' at pixel position

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

        player_glyph.clear();
        ghost_glyph.clear();
    };

    // --- run state (reset_run() restores the exact boot run: no RNG) -------
    unsigned state = st_title;
    unsigned frames = 0;                 // monotonic since boot, never reset
    int px = spawn_x;
    int py = spawn_y;
    int vy = 0;
    bool on_ground = true;
    bool carried = false;
    bool p2_carried = false;             // the rush parcel (rung 2)
    unsigned delivered_n = 0;            // rush deliveries this run
    unsigned mode = 0;                   // 0 classic (START) / 1 rush (SELECT)
    bool door_open = false;
    bool switch_held = false;
    unsigned rewinds = 0;
    unsigned run_frames = 0;

    pose_t buffer[rewind_frames];
    int buf_fill = 0;                    // 0..rewind_frames (ring when full)
    int buf_head = 0;                    // next write slot

    bool ghost_active = false;
    bool on_ghost = false;               // standing on the ghost's head
                                         // (growth rung 1)
    pose_t ghost_track[rewind_frames];
    int ghost_at = 0;                    // replay cursor
    int gx = 0;
    int gy = 0;

    auto reset_run = [&]()
    {
        px = spawn_x;
        py = spawn_y;
        vy = 0;
        on_ground = true;
        carried = false;
        p2_carried = false;
        delivered_n = 0;
        door_open = false;
        switch_held = false;
        rewinds = 0;
        run_frames = 0;
        buf_fill = 0;
        buf_head = 0;
        ghost_active = false;
        on_ghost = false;
        gx = 0;
        gy = 0;
    };

    // Solid test for the PLAYER's collision (the door is solid while
    // closed; everything '#' always is).
    auto solid_cell = [&](int cx, int cy) -> bool
    {
        if(cx < 0 || cx >= map_w || cy < 0 || cy >= map_h)
        {
            return true;
        }

        char t = level[cy][cx];

        if(t == '#')
        {
            return true;
        }

        if(t == 'D' && ! door_open)
        {
            return true;
        }

        return false;
    };

    // Does the player AABB at (x, y) overlap any solid cell? Feet-center
    // anchor: box spans [x-half_w, x+half_w) x [y-body_h, y) in 8.8 px.
    auto blocked_at = [&](int x, int y) -> bool
    {
        int left = (x - half_w) / fp;
        int right = (x + half_w - 1) / fp;
        int top = (y - body_h) / fp;
        int bottom = (y - 1) / fp;

        for(int pyc = top / cell_px; pyc <= bottom / cell_px; ++pyc)
        {
            for(int pxc = left / cell_px; pxc <= right / cell_px; ++pxc)
            {
                if(solid_cell(pxc, pyc))
                {
                    return true;
                }
            }
        }

        return false;
    };

    auto feet_cell_x = [&](int x) { return (x / fp) / cell_px; };
    auto feet_cell_y = [&](int y) { return ((y - 1) / fp) / cell_px; };

    while(true)
    {
        bool start = bn::keypad::start_pressed();
        bool select = bn::keypad::select_pressed();

        switch(state)
        {

        case st_title:
        case st_delivered:
            if(start || select)
            {
                mode = select ? 1 : 0;   // SELECT: THE RUSH ORDER
                reset_run();
                clear_lines();
                state = st_playing;
            }
            break;

        case st_playing:
        {
            ++run_frames;

            // --- the ghost replays first (pure pose playback) --------------
            if(ghost_active)
            {
                gx = ghost_track[ghost_at].x;
                gy = ghost_track[ghost_at].y;
                ++ghost_at;

                if(ghost_at >= rewind_frames)
                {
                    ghost_active = false;     // its 5 seconds are spent
                }
            }

            // --- the rewind verb (R): only on a FULL buffer, one ghost ----
            if(bn::keypad::r_pressed() && ! ghost_active
               && buf_fill >= rewind_frames)
            {
                // Oldest-first copy of the ring into the ghost's track.
                for(int i = 0; i < rewind_frames; ++i)
                {
                    ghost_track[i] = buffer[(buf_head + i) % rewind_frames];
                }

                // YOU snap back 5 seconds; the ghost sets off from there.
                px = ghost_track[0].x;
                py = ghost_track[0].y;
                vy = 0;
                gx = px;
                gy = py;
                ghost_active = true;
                ghost_at = 0;
                buf_fill = 0;                 // the tape is spent too
                buf_head = 0;
                ++rewinds;
            }

            // --- ghost-as-platform, part 1: the ride (growth rung 1) ------
            // Standing on the ghost's head re-evaluates every frame AFTER
            // the ghost has replayed its pose: still active, still under
            // your feet, and the seat not squeezed into a wall -> your
            // feet FOLLOW its head. Otherwise you are dismounted and
            // normal physics resumes this same frame.
            if(on_ghost)
            {
                int seat = gy - body_h;

                if(ghost_active
                   && px - gx < 2 * half_w && gx - px < 2 * half_w
                   && ! blocked_at(px, seat))
                {
                    py = seat;
                    vy = 0;
                    on_ground = true;
                }
                else
                {
                    on_ghost = false;
                }
            }

            // --- courier physics (integer, input-driven, deterministic) ---
            int vx = 0;

            if(bn::keypad::left_held())
            {
                vx -= walk_speed;
            }

            if(bn::keypad::right_held())
            {
                vx += walk_speed;
            }

            if(bn::keypad::a_pressed() && on_ground)
            {
                vy = jump_v;
                on_ghost = false;         // jumping dismounts
            }

            if(! on_ghost)
            {
                vy += gravity;            // riders hang on the replay,
            }                             //   not on gravity

            if(vy > fall_cap)
            {
                vy = fall_cap;
            }

            // Horizontal, pixel-stepped so we never tunnel a wall.
            if(vx != 0)
            {
                int nx = px + vx;

                if(! blocked_at(nx, py))
                {
                    px = nx;
                }
            }

            // Vertical, with landing/bump resolution to the cell edge.
            if(! on_ghost)
            {
                int ny = py + vy;
                int seat = gy - body_h;

                // Ghost-as-platform, part 2: the one-way TOP landing.
                // Falling, horizontally over the ghost, feet crossing its
                // head this frame, seat clear -> stand on it. One-way by
                // construction: rising or sideways never tests this.
                if(ghost_active && vy > 0
                   && px - gx < 2 * half_w && gx - px < 2 * half_w
                   && py <= seat && ny >= seat
                   && ! blocked_at(px, seat))
                {
                    py = seat;
                    vy = 0;
                    on_ground = true;
                    on_ghost = true;
                }
                else if(! blocked_at(px, ny))
                {
                    py = ny;
                    on_ground = false;
                }
                else if(vy > 0)               // landing: snap feet to the
                {                             // top of the blocking row
                    int bottom_cell = ((ny - 1) / fp) / cell_px;
                    py = bottom_cell * cell_px * fp;
                    vy = 0;
                    on_ground = true;
                }
                else                          // head bump: stop rising
                {
                    vy = 0;
                }
            }

            // --- the clockwork reacts --------------------------------------
            bool player_on_switch = on_ground
                && feet_cell_x(px) == switch_cx
                && feet_cell_y(py) == switch_cy;
            bool ghost_on_switch = ghost_active
                && feet_cell_x(gx) == switch_cx
                && feet_cell_y(gy) == switch_cy;
            switch_held = player_on_switch || ghost_on_switch;

            // The door never crushes: it stays open while the player
            // overlaps its cells, whatever the switch says.
            bool player_in_door = false;
            {
                int left = (px - half_w) / fp / cell_px;
                int right = ((px + half_w - 1) / fp) / cell_px;
                int top = ((py - body_h) / fp) / cell_px;
                int bottom = ((py - 1) / fp) / cell_px;
                player_in_door = left <= door_cx && door_cx <= right
                    && top <= door_cy1 && door_cy0 <= bottom;
            }

            door_open = switch_held || (door_open && player_in_door);

            // Parcel + chute (the delivery). Rung 2: the rush order
            // adds a second parcel and gives the chute HOURS — the
            // classic run (mode 0) evaluates exactly the old code.
            bool window_open = (run_frames % window_period)
                               < window_open_frames;

            if(! carried && feet_cell_x(px) == parcel_cx
               && feet_cell_y(py) == parcel_cy)
            {
                carried = true;
            }

            if(mode == 1 && ! p2_carried
               && feet_cell_x(px) == parcel2_cx
               && feet_cell_y(py) == parcel2_cy)
            {
                p2_carried = true;
            }

            if(mode == 0)
            {
                if(carried && feet_cell_x(px) == chute_cx
                   && feet_cell_y(py) == chute_cy)
                {
                    state = st_delivered;
                    clear_lines();
                }
            }
            else if((carried || p2_carried) && window_open
                    && feet_cell_x(px) == chute_cx
                    && feet_cell_y(py) == chute_cy)
            {
                delivered_n += unsigned(carried) + unsigned(p2_carried);
                carried = false;
                p2_carried = false;

                if(delivered_n >= 2)
                {
                    state = st_delivered;
                    clear_lines();
                }
            }

            // --- record THIS frame's pose onto the tape --------------------
            buffer[buf_head] = pose_t{px, py};
            buf_head = (buf_head + 1) % rewind_frames;

            if(buf_fill < rewind_frames)
            {
                ++buf_fill;
            }

            break;
        }

        }

        // --- draw ----------------------------------------------------------

        switch(state)
        {

        case st_title:
            ui_lines[0].set(ui_gen, ui_x, -60, "CLOCKWORK COURIER");
            ui_lines[1].set(ui_gen, ui_x, -40, "R REWINDS YOU 5 SECONDS");
            ui_lines[2].set(ui_gen, ui_x, -28, "YOUR GHOST REPLAYS YOU");
            ui_lines[3].set(ui_gen, ui_x, -16, "IT CAN HOLD THE SWITCH");
            ui_lines[4].set(ui_gen, ui_x, 8, "PRESS START");
            ui_lines[5].set(ui_gen, ui_x, 24, "AND YOU CAN STAND ON IT");
            ui_lines[6].set(ui_gen, ui_x, 40, "SELECT: RUSH ORDER (2)");
            break;

        case st_playing:
        {
            for(int y = 0; y < map_h; ++y)
            {
                bn::string<40> row;

                for(int x = 0; x < map_w; ++x)
                {
                    char glyph = level[y][x];

                    if(glyph == 'P' && carried)
                    {
                        glyph = '.';
                    }
                    else if(y == parcel2_cy && x == parcel2_cx)
                    {
                        glyph = (mode == 1 && ! p2_carried) ? 'Q' : '.';
                    }
                    else if(glyph == 'D')
                    {
                        glyph = door_open ? '.' : 'D';
                    }
                    else if(glyph == 'o' && switch_held)
                    {
                        glyph = '_';
                    }

                    row.append(glyph);
                }

                map_lines[y].set(map_gen, map_x, map_y0 + cell_px * y, row);
            }

            // The courier and the ghost, pixel-true (cosmetic only — the
            // mailbox carries the exact fixed-point truth).
            bn::string<40> at("@");
            player_glyph.set(map_gen, map_x + px / fp - 4,
                             map_y0 + py / fp - cell_px, at);

            if(ghost_active)
            {
                bn::string<40> g("g");
                ghost_glyph.set(map_gen, map_x + gx / fp - 4,
                                map_y0 + gy / fp - cell_px, g);
            }
            else
            {
                ghost_glyph.clear();
            }

            bn::string<40> hud;

            if(ghost_active)
            {
                hud.append("GHOST OUT ");
            }
            else if(buf_fill >= rewind_frames)
            {
                hud.append("REWIND READY ");
            }
            else
            {
                hud.append("WINDING ");
                hud.append(bn::to_string<8>(buf_fill / 3));
                hud.append("% ");
            }

            if(mode == 1)
            {
                unsigned held = unsigned(carried) + unsigned(p2_carried);
                hud.append(bn::to_string<8>(held));
                hud.append(" HELD ");         // the 20-sprite text cap
                hud.append((run_frames % window_period)  // rules the HUD:
                           < window_open_frames ? "OPEN"  // keep it terse
                                                : "SHUT");
            }
            else
            {
                hud.append(carried ? "PARCEL HELD" : "PARCEL WAITS");
            }

            ui_lines[0].set(ui_gen, ui_x, ui_y[1], hud);

            bn::string<40> clock("CLOCK ");
            clock.append(bn::to_string<8>(run_frames / 60));
            clock.append("s  REWINDS ");
            clock.append(bn::to_string<8>(rewinds));
            ui_lines[1].set(ui_gen, ui_x, ui_y[2], clock);
            break;
        }

        case st_delivered:
        {
            ui_lines[0].set(ui_gen, ui_x, -60, "PARCEL DELIVERED");
            bn::string<40> line1("CLOCK ");
            line1.append(bn::to_string<8>(run_frames / 60));
            line1.append("s (");
            line1.append(bn::to_string<8>(run_frames));
            line1.append(" FRAMES)");
            ui_lines[1].set(ui_gen, ui_x, -40, line1);
            bn::string<40> line2("REWINDS ");
            line2.append(bn::to_string<8>(rewinds));
            ui_lines[2].set(ui_gen, ui_x, -28, line2);
            ui_lines[3].set(ui_gen, ui_x, -16, "THE TOWER TICKS ON");
            ui_lines[4].set(ui_gen, ui_x, 8, "PRESS START");

            if(mode == 1)
            {
                ui_lines[5].set(ui_gen, ui_x, 24, "RUSH ORDER: 2 PARCELS");
            }

            break;
        }

        }

        // --- telemetry mailbox: every slot, every frame ---------------------

        ++frames;
        cc_telemetry[0] = 0x434C4B57u;   // 'CLKW'
        cc_telemetry[1] = 0x434F5552u;   // 'COUR'
        cc_telemetry[2] = state;
        cc_telemetry[3] = frames;
        cc_telemetry[4] = unsigned(px);
        cc_telemetry[5] = unsigned(py);
        cc_telemetry[6] = on_ground ? 1u : 0u;
        cc_telemetry[7] = unsigned(buf_fill);
        cc_telemetry[8] = ghost_active ? 1u : 0u;
        cc_telemetry[9] = unsigned(gx);
        cc_telemetry[10] = unsigned(gy);
        cc_telemetry[11] = switch_held ? 1u : 0u;
        cc_telemetry[12] = door_open ? 1u : 0u;
        cc_telemetry[13] = unsigned(carried) + unsigned(p2_carried);
        cc_telemetry[14] = rewinds;
        cc_telemetry[15] = run_frames;
        // growth rung 1 (extension; words 0-15 stay pinned)
        cc_telemetry[16] = on_ghost ? 1u : 0u;
        // growth rung 2 (extension; words 0-16 stay pinned)
        cc_telemetry[17] = mode;
        cc_telemetry[18] = delivered_n;
        cc_telemetry[19] = (state == st_playing
                            && (run_frames % window_period)
                               < window_open_frames) ? 1u : 0u;

        bn::core::update();
    }
}
