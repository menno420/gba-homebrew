/*
 * LUMEN DRIFT — increment 2 (hazards + fail state).
 *
 * You are a mote of light falling through a dark cave. Hold A to thrust
 * against gravity, steer laterally with the D-pad, drift through the
 * narrowing tunnel. Depth reached is the score; your light slowly decays
 * (thrust flares it back). START restarts the run.
 *
 * Increment 2 makes depth meaningful against risk:
 *   - CRYSTAL SPIKES (pink) grow from the tunnel walls at fixed depths —
 *     touch one and the run ends.
 *   - THE SURGE (ember wall) descends from the cave mouth and consumes the
 *     rock behind it — stall too long and it takes your light.
 *   - Game over shows final DEPTH + BEST (best persists across restarts
 *     in RAM); START starts the next run.
 *
 * All code and assets original (graphics/generate_assets.py). The cave,
 * crystal placement and surge speed are fixed — every run is the same,
 * which keeps the headless input-script proof deterministic.
 *
 * Generic pieces (games/common/): gl_input.h (D-pad vector), gl_kinematics.h
 * (fixed-point body), gl_tile_grid.h (tile collision — reused verbatim as
 * the hazard sensor via a second predicate), gl_run_state.h (run lifecycle +
 * best score). Game-specific here: cave shape, hazard placement/tuning,
 * light-decay palette trick, HUD, game-over screen.
 */

#include "bn_core.h"
#include "bn_math.h"
#include "bn_string.h"
#include "bn_vector.h"
#include "bn_colors.h"
#include "bn_keypad.h"
#include "bn_display.h"
#include "bn_bg_palettes.h"
#include "bn_camera_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_ptr.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_map_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_regular_bg_map_cell_info.h"

#include "gl_input.h"
#include "gl_kinematics.h"
#include "gl_run_state.h"
#include "gl_tile_grid.h"

#include "common_variable_8x8_sprite_font.h"

#include "bn_sprite_items_ld_mote.h"
#include "bn_bg_palette_items_ld_palette.h"
#include "bn_regular_bg_tiles_items_ld_tiles.h"

namespace
{
    constexpr int cell_px = 8;
    constexpr int map_cols = 32;
    constexpr int map_rows = 64;
    constexpr int map_width_px = map_cols * cell_px;   // 256
    constexpr int map_height_px = map_rows * cell_px;  // 512

    constexpr int empty_tile = 0;
    constexpr int rock_tile = 1;
    constexpr int rim_tile = 2;
    constexpr int crystal_tile = 3;
    constexpr int surge_tile = 4;
    constexpr int consumed_tile = 5;

    // Physics tuning (pixels/frame): floaty mote.
    constexpr bn::fixed gravity(0.045);
    constexpr bn::fixed thrust(0.11);
    constexpr bn::fixed lateral(0.05);
    constexpr bn::fixed max_speed_x(1.25);
    constexpr bn::fixed max_speed_y(1.75);
    constexpr bn::fixed drag_x(0.98);

    constexpr bn::fixed mote_half(3);  // collision half-extent (visual glow is bigger)
    constexpr bn::fixed hazard_half(2);  // forgiving hazard hitbox (< mote_half)

    // The surge: a wall of consuming ember light that descends from the cave
    // mouth at a fixed speed — stalling is now a losing strategy. It starts
    // above the map so the opening chamber gets a grace period.
    constexpr bn::fixed surge_start(-32);      // map px (above the map top)
    constexpr bn::fixed surge_speed(0.25);     // px/frame (~1.9 rows/s; max
                                               // fall speed is 7x faster)

    constexpr bn::fixed_point spawn(16 * cell_px + 4, 3 * cell_px + 4);  // map px

    // Handcrafted tunnel waveform: the cave's spine sways with two layered
    // waves (values are column offsets from the center column).
    constexpr int wave_a[16] = {0, 2, 4, 5, 6, 5, 4, 2, 0, -2, -4, -5, -6, -5, -4, -2};
    constexpr int wave_b[8] = {0, 1, 2, 1, 0, -1, -2, -1};

    class cave_map
    {

    public:
        bn::regular_bg_map_item map_item;

        cave_map() :
            map_item(_cells[0], bn::size(map_cols, map_rows))
        {
            _generate();
        }

        [[nodiscard]] bool solid(int cx, int cy) const
        {
            if(cx < 0 || cx >= map_cols || cy < 0 || cy >= map_rows)
            {
                return true;  // the world border is impassable rock
            }

            return _solid[cy][cx];
        }

        [[nodiscard]] bool crystal(int cx, int cy) const
        {
            if(cx < 0 || cx >= map_cols || cy < 0 || cy >= map_rows)
            {
                return false;  // out-of-map space is rock, not hazard
            }

            return _crystal[cy][cx];
        }

        /// Re-bake all cells from the solidity/crystal maps (restores rows
        /// the surge overwrote). Caller reloads the bg map afterwards.
        void rebake()
        {
            _bake();
        }

        /// Overwrite one row's cells with a single tile (visual only —
        /// solidity is untouched). Used by the surge; rebake() restores.
        void overwrite_row(int cy, int tile_index)
        {
            if(cy < 0 || cy >= map_rows)
            {
                return;
            }

            for(int cx = 0; cx < map_cols; ++cx)
            {
                _set_cell(cx, cy, tile_index);
            }
        }

    private:
        alignas(int) bn::regular_bg_map_cell _cells[map_cols * map_rows];
        bool _solid[map_rows][map_cols];
        bool _crystal[map_rows][map_cols];

        void _carve(int cx, int cy)
        {
            if(cx > 0 && cx < map_cols - 1 && cy > 0 && cy < map_rows - 2)
            {
                _solid[cy][cx] = false;
            }
        }

        void _generate()
        {
            // 1. Everything is rock; no crystals yet.
            for(int cy = 0; cy < map_rows; ++cy)
            {
                for(int cx = 0; cx < map_cols; ++cx)
                {
                    _solid[cy][cx] = true;
                    _crystal[cy][cx] = false;
                }
            }

            // 2. Starting chamber under the ceiling.
            for(int cy = 1; cy <= 6; ++cy)
            {
                for(int cx = 10; cx <= 22; ++cx)
                {
                    _carve(cx, cy);
                }
            }

            // 3. The tunnel: swaying spine, narrowing with depth.
            for(int cy = 5; cy < map_rows - 2; ++cy)
            {
                int center = 16 + wave_a[(cy / 2) % 16] + wave_b[(cy / 3) % 8];
                center = bn::clamp(center, 4, map_cols - 5);

                int half_width = bn::clamp(5 - (cy / 20), 2, 5);

                for(int cx = center - half_width; cx <= center + half_width; ++cx)
                {
                    _carve(cx, cy);
                }

                // 4. Obstacle blobs every 9 rows, alternating sides — the
                //    passable gap stays on the other side.
                if(cy > 10 && cy % 9 == 4)
                {
                    bool left = ((cy / 9) % 2) == 0;

                    for(int i = 0; i < 2; ++i)
                    {
                        int cx = left ? center - half_width + i : center + half_width - i;

                        if(cx > 0 && cx < map_cols - 1)
                        {
                            _solid[cy][cx] = true;
                        }
                    }
                }

                // 4b. Crystal spikes every 7 rows, alternating walls: a
                //     2-cell cluster growing from the tunnel edge into the
                //     passage. Deadly but NOT solid — you can fly into a
                //     spike, and shouldn't. Placed after the blobs so a
                //     spike never hides inside rock; only open cells turn
                //     deadly, so the passage keeps >= 2 clear cells.
                if(cy > 12 && cy % 7 == 2)
                {
                    bool left = ((cy / 7) % 2) == 0;

                    for(int i = 0; i < 2; ++i)
                    {
                        int cx = left ? center - half_width + i : center + half_width - i;

                        if(cx > 0 && cx < map_cols - 1 && ! _solid[cy][cx])
                        {
                            _crystal[cy][cx] = true;
                        }
                    }
                }
            }

            // 5. Rest plate straight under the spawn: a no-input run settles
            //    here (keeps the headless no-input proof frame deterministic
            //    AND teaches "thrust + steer to slip past").
            for(int cx = 15; cx <= 17; ++cx)
            {
                _solid[10][cx] = true;
            }

            // 6. Bake cells.
            _bake();
        }

        void _set_cell(int cx, int cy, int tile_index)
        {
            bn::regular_bg_map_cell& cell = _cells[map_item.cell_index(cx, cy)];
            bn::regular_bg_map_cell_info cell_info(cell);
            cell_info.set_tile_index(tile_index);
            cell_info.set_palette_id(0);
            cell = cell_info.cell();
        }

        // Bake cells: crystal hazard on open cells; solid next to open =
        // glowing rim; deep rock otherwise; open = empty.
        void _bake()
        {
            for(int cy = 0; cy < map_rows; ++cy)
            {
                for(int cx = 0; cx < map_cols; ++cx)
                {
                    int tile_index = empty_tile;

                    if(_crystal[cy][cx])
                    {
                        tile_index = crystal_tile;
                    }
                    else if(_solid[cy][cx])
                    {
                        tile_index = _next_to_open(cx, cy) ? rim_tile : rock_tile;
                    }

                    _set_cell(cx, cy, tile_index);
                }
            }
        }

        [[nodiscard]] bool _next_to_open(int cx, int cy) const
        {
            for(int dy = -1; dy <= 1; ++dy)
            {
                for(int dx = -1; dx <= 1; ++dx)
                {
                    int nx = cx + dx;
                    int ny = cy + dy;

                    if(nx >= 0 && nx < map_cols && ny >= 0 && ny < map_rows &&
                            ! _solid[ny][nx])
                    {
                        return true;
                    }
                }
            }

            return false;
        }
    };

    cave_map cave;  // statics live in EWRAM: 4KB cells + 2KB solidity

    // Map-space px -> Butano world coordinates (world origin = map center).
    [[nodiscard]] bn::fixed_point to_world(const bn::fixed_point& map_pos)
    {
        return bn::fixed_point(map_pos.x() - map_width_px / 2,
                               map_pos.y() - map_height_px / 2);
    }
}

int main()
{
    bn::core::init();

    bn::bg_palettes::set_transparent_color(bn::color(1, 1, 3));  // near-black cave air
    bn::bg_palettes::set_fade_color(bn::colors::black);

    bn::regular_bg_item bg_item(bn::regular_bg_tiles_items::ld_tiles,
                                bn::bg_palette_items::ld_palette, cave.map_item);
    bn::regular_bg_ptr bg = bg_item.create_bg(0, 0);
    bn::regular_bg_map_ptr bg_map = bg.map();

    bn::camera_ptr camera = bn::camera_ptr::create(0, 0);
    bg.set_camera(camera);

    bn::sprite_ptr mote = bn::sprite_items::ld_mote.create_sprite(0, 0);
    mote.set_camera(camera);

    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 8> depth_sprites;
    bn::vector<bn::sprite_ptr, 16> hint_sprites;
    bn::vector<bn::sprite_ptr, 16> over_sprites;
    text_generator.generate(-110, -72, "LUMEN DRIFT - A THRUST", hint_sprites);

    // The SAME generic grid template drives wall collision and hazard
    // sensing — only the predicate differs.
    gl::tile_grid grid(cell_px, [](int cx, int cy) {
        return cave.solid(cx, cy);
    });
    gl::tile_grid crystal_sensor(cell_px, [](int cx, int cy) {
        return cave.crystal(cx, cy);
    });

    gl::body mote_body;
    gl::run_state run;
    int depth = -1;      // force first HUD draw
    int max_depth_cells = 0;
    int flare = 0;       // frames of thrust-flare left
    int run_frames = 0;
    bn::fixed surge_y = surge_start;
    int surge_row = -1000;  // last drawn front row (forces first draw)

    auto restart = [&] {
        mote_body.pos = spawn;
        mote_body.vel = bn::fixed_point(0, 0);
        max_depth_cells = 0;
        depth = -1;
        flare = 0;
        run_frames = 0;
        surge_y = surge_start;
        surge_row = -1000;
        cave.rebake();  // restore rows the surge consumed
        bg_map.reload_cells_ref();
        over_sprites.clear();
        mote.set_visible(true);
        run.restart();
    };

    auto game_over = [&] {
        run.fail(max_depth_cells);
        mote.set_visible(false);
        bn::bg_palettes::set_fade_intensity(bn::fixed(0.7));  // light is taken

        bn::string<24> score_line("DEPTH ");
        score_line += bn::to_string<6>(max_depth_cells);
        score_line += " - BEST ";
        score_line += bn::to_string<6>(run.best());

        text_generator.set_center_alignment();
        text_generator.generate(0, -18, "THE LIGHT IS TAKEN", over_sprites);
        text_generator.generate(0, -4, score_line, over_sprites);
        text_generator.generate(0, 14, "PRESS START", over_sprites);
        text_generator.set_left_alignment();
    };

    restart();

    while(true)
    {
        if(run.over())
        {
            // Fail state: world frozen, game-over card up. START restarts
            // (after a short grace so a mashed button can't skip the score).
            run.update();

            if(run.frames_over() > 15 && bn::keypad::start_pressed())
            {
                restart();
            }

            bn::core::update();
            continue;
        }

        if(bn::keypad::start_pressed())
        {
            restart();
        }

        // One core mechanic: gravity vs one-button thrust (+ D-pad drift).
        mote_body.accelerate(bn::fixed_point(0, gravity));

        if(bn::keypad::a_held())
        {
            mote_body.accelerate(bn::fixed_point(0, -thrust));
            flare = 12;
        }

        mote_body.accelerate(gl::dpad_vector() * lateral);
        mote_body.damp_x(drag_x);
        mote_body.clamp_velocity(max_speed_x, max_speed_y);
        grid.move(mote_body, mote_half, mote_half);

        // Depth score = cells descended from spawn (high-water mark).
        int depth_cells = ((mote_body.pos.y() - spawn.y()) / cell_px).floor_integer();
        max_depth_cells = bn::max(max_depth_cells, depth_cells);

        if(max_depth_cells != depth)
        {
            depth = max_depth_cells;
            depth_sprites.clear();
            bn::string<12> label("DEPTH ");
            label += bn::to_string<6>(depth);
            text_generator.generate(-110, -62, label, depth_sprites);
        }

        // The surge descends; rows it passes are consumed (visual only —
        // the kill line is surge_y itself). Cell rewrites happen only when
        // the front crosses a row boundary (every 32 frames at 0.25 px/f).
        surge_y += surge_speed;
        int front_row = (surge_y / cell_px).floor_integer();

        if(front_row != surge_row)
        {
            for(int cy = bn::max(surge_row, 0); cy < front_row; ++cy)
            {
                cave.overwrite_row(cy, consumed_tile);
            }

            cave.overwrite_row(front_row, surge_tile);
            surge_row = front_row;
            bg_map.reload_cells_ref();
        }

        // Light decay: the cave darkens as the run drags on; thrusting
        // flares the light back up (sprites are unaffected — the mote
        // itself never dims).
        run_frames = bn::min(run_frames + 1, 100000);  // keep bn::fixed(run_frames) in range

        if(flare > 0)
        {
            --flare;
        }

        bn::fixed decay = bn::min(bn::fixed(run_frames) / 3600, bn::fixed(0.45));

        if(flare > 0)
        {
            decay = bn::max(decay - bn::fixed(0.15), bn::fixed(0));
        }

        bn::bg_palettes::set_fade_intensity(decay);

        // Camera chases the mote, clamped so the view never leaves the map.
        bn::fixed_point world_pos = to_world(mote_body.pos);
        mote.set_position(world_pos);

        bn::fixed cam_x = bn::clamp(world_pos.x(),
                                    bn::fixed(-(map_width_px - bn::display::width()) / 2),
                                    bn::fixed((map_width_px - bn::display::width()) / 2));
        bn::fixed cam_y = bn::clamp(world_pos.y(),
                                    bn::fixed(-(map_height_px - bn::display::height()) / 2),
                                    bn::fixed((map_height_px - bn::display::height()) / 2));
        camera.set_position(cam_x, cam_y);

        // Hazards end the run: crystal spikes (forgiving sub-box via the
        // generic grid sensor) or the surge front catching up.
        if(crystal_sensor.overlaps(mote_body.pos.x(), mote_body.pos.y(),
                                   hazard_half, hazard_half) ||
                mote_body.pos.y() - mote_half < surge_y)
        {
            game_over();
        }

        bn::core::update();
    }
}
