/*
 * LUMEN DRIFT — polish pass 1 (title screen + SRAM best persistence).
 *
 * You are a mote of light falling through a dark cave. Hold A to thrust
 * against gravity, steer laterally with the D-pad, drift through the
 * narrowing tunnel. Depth reached is the score; your light slowly decays
 * (thrust flares it back). START restarts the run.
 *
 * Increment 2 made depth meaningful against risk:
 *   - CRYSTAL SPIKES (pink) grow from the tunnel walls at fixed depths —
 *     touch one and the run ends.
 *   - THE SURGE (ember wall) descends from the cave mouth and consumes the
 *     rock behind it — stall too long and it takes your light.
 *   - Game over shows final DEPTH + BEST (best persists across restarts
 *     in RAM); START starts the next run.
 *
 * Increment 3 completes the concept doc's committed scope:
 *   - SPARK SHARDS (glinting diamonds) sit at the tunnel edges, next to
 *     walls and crystals — collecting one rewinds the light decay (the
 *     cave brightens back up) and counts on the SPARKS HUD, so hugging
 *     the dangerous edges pays. Shards respawn on restart.
 *   - THREE CAVE SECTIONS with distinct looks and layouts as you descend:
 *     THE BLUFFS (teal rims, the increment-1 tunnel), THE GALLERY (amber
 *     rims, wide halls split by rock pillars) and THE DEEP (violet rims,
 *     narrow fast-swaying passage, denser crystals). A banner names each
 *     new section as you reach it.
 *
 * Polish pass 1 (post-first-complete, from the concept doc's polish list):
 *   - TITLE SCREEN: the game boots to a "LUMEN DRIFT" card over the cave
 *     mouth showing the BEST depth and PRESS START; START begins the run.
 *     Physics only starts on that press, so scripted proof replays stay
 *     deterministic — every pre-title input script is reproduced by holding
 *     START at boot and shifting all frame spans by a constant offset.
 *   - SRAM BEST PERSISTENCE: the best depth survives power cycles via
 *     bn::sram behind a magic tag (gl_save.h) — fresh/foreign SRAM reads as
 *     BEST 0, an improved best is written once per game over. In-RAM run
 *     behavior is unchanged.
 *
 * All code and assets original (graphics/generate_assets.py). The cave,
 * shard/crystal placement and surge speed are fixed — every run is the
 * same, which keeps the headless input-script proof deterministic.
 *
 * Generic pieces (games/common/): gl_input.h (D-pad vector), gl_kinematics.h
 * (fixed-point body), gl_tile_grid.h (tile collision — reused verbatim as
 * hazard AND pickup sensors via extra predicates; visit_overlaps reports
 * which shard cells were touched), gl_run_state.h (run lifecycle + best
 * score), gl_stage.h (threshold-based section progression), gl_save.h
 * (magic-checked SRAM slot). Game-specific here: cave shape, hazard/shard
 * placement/tuning, light-decay palette trick, HUD, title and game-over
 * screens.
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
#include "gl_save.h"
#include "gl_stage.h"
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
    constexpr int shard_tile = 6;
    constexpr int rim2_tile = 7;   // gallery (section 2) rim
    constexpr int rim3_tile = 8;   // deep (section 3) rim

    // Cave sections: row bands with distinct rims + tunnel layouts. The
    // banner fires when the depth high-water mark crosses a section's first
    // row (section_depth = first_row - spawn_row, spawn row 3.5 -> 21/41).
    constexpr int gallery_first_row = 24;
    constexpr int deep_first_row = 44;
    constexpr int section_depths[] = {21, 41};
    constexpr const char* section_names[] = {"THE BLUFFS", "THE GALLERY",
                                             "THE DEEP"};

    [[nodiscard]] constexpr int section_of_row(int cy)
    {
        return cy < gallery_first_row ? 0 : (cy < deep_first_row ? 1 : 2);
    }

    // Physics tuning (pixels/frame): floaty mote.
    constexpr bn::fixed gravity(0.045);
    constexpr bn::fixed thrust(0.11);
    constexpr bn::fixed lateral(0.05);
    constexpr bn::fixed max_speed_x(1.25);
    constexpr bn::fixed max_speed_y(1.75);
    constexpr bn::fixed drag_x(0.98);

    constexpr bn::fixed mote_half(3);  // collision half-extent (visual glow is bigger)
    constexpr bn::fixed hazard_half(2);  // forgiving hazard hitbox (< mote_half)
    constexpr bn::fixed pickup_half(4);  // generous shard pickup box (> mote_half)

    constexpr int shard_light_refund = 1500;  // decay frames one shard rewinds (25s)

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

        [[nodiscard]] bool shard(int cx, int cy) const
        {
            if(cx < 0 || cx >= map_cols || cy < 0 || cy >= map_rows)
            {
                return false;  // out-of-map space is rock, not pickup
            }

            return _shard[cy][cx];
        }

        /// Take the shard at (cx, cy): clears the pickup and its cell.
        /// Caller reloads the bg map afterwards.
        void collect_shard(int cx, int cy)
        {
            _shard[cy][cx] = false;
            _set_cell(cx, cy, empty_tile);
        }

        /// Re-bake all cells from the solidity/crystal/shard maps (restores
        /// rows the surge overwrote AND respawns collected shards). Caller
        /// reloads the bg map afterwards.
        void rebake()
        {
            for(int cy = 0; cy < map_rows; ++cy)
            {
                for(int cx = 0; cx < map_cols; ++cx)
                {
                    _shard[cy][cx] = _shard_home[cy][cx];
                }
            }

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
        bool _shard[map_rows][map_cols];
        bool _shard_home[map_rows][map_cols];  // placement baseline for rebake()

        void _carve(int cx, int cy)
        {
            if(cx > 0 && cx < map_cols - 1 && cy > 0 && cy < map_rows - 2)
            {
                _solid[cy][cx] = false;
            }
        }

        void _generate()
        {
            // 1. Everything is rock; no crystals or shards yet.
            for(int cy = 0; cy < map_rows; ++cy)
            {
                for(int cx = 0; cx < map_cols; ++cx)
                {
                    _solid[cy][cx] = true;
                    _crystal[cy][cx] = false;
                    _shard[cy][cx] = false;
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

            // 3. The tunnel: swaying spine, one waveform + width per cave
            //    section (rows above gallery_first_row keep increment 1's
            //    exact shape, so the committed early-game proof scripts and
            //    the no-input settle point still hold).
            for(int cy = 5; cy < map_rows - 2; ++cy)
            {
                int section = section_of_row(cy);
                int center;
                int half_width;

                if(section == 0)
                {
                    // THE BLUFFS: the original medium tunnel.
                    center = 16 + wave_a[(cy / 2) % 16] + wave_b[(cy / 3) % 8];
                    half_width = bn::clamp(5 - (cy / 20), 2, 5);
                }
                else if(section == 1)
                {
                    // THE GALLERY: wide slow-swaying halls (pillars below).
                    center = 16 + 2 * wave_b[(cy / 4) % 8];
                    half_width = 6;
                }
                else
                {
                    // THE DEEP: narrow, fast sway (the +8 phase offset makes
                    // the row-44 passage line up under the gallery's last
                    // row), final squeeze below row 54.
                    center = 16 + wave_a[(cy + 8) % 16] + wave_b[(cy / 2) % 8];
                    half_width = cy < 54 ? 3 : 2;
                }

                center = bn::clamp(center, 4, map_cols - 5);

                for(int cx = center - half_width; cx <= center + half_width; ++cx)
                {
                    _carve(cx, cy);
                }

                // 4. Section obstacles.
                //    BLUFFS: blobs every 9 rows, alternating sides — the
                //    passable gap stays on the other side.
                if(section == 0 && cy > 10 && cy % 9 == 4)
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

                //    GALLERY: a rock pillar splits the hall — pick a side.
                if(section == 1 && cy >= 26 && cy % 6 == 1)
                {
                    for(int cx = center - 1; cx <= center + 1; ++cx)
                    {
                        if(cx > 0 && cx < map_cols - 1)
                        {
                            _solid[cy][cx] = true;
                        }
                    }
                }

                // 4b. Crystal spikes, alternating walls: a 2-cell cluster
                //     growing from the tunnel edge into the passage. Deadly
                //     but NOT solid — you can fly into a spike, and
                //     shouldn't. Placed after the blobs/pillars so a spike
                //     never hides inside rock; only open cells turn deadly,
                //     so the passage keeps >= 2 clear cells. THE DEEP packs
                //     them denser (every 5 rows vs every 7).
                int crystal_step = section == 2 ? 5 : 7;

                if(cy > 12 && cy % crystal_step == 2)
                {
                    bool left = ((cy / crystal_step) % 2) == 0;

                    for(int i = 0; i < 2; ++i)
                    {
                        int cx = left ? center - half_width + i : center + half_width - i;

                        if(cx > 0 && cx < map_cols - 1 && ! _solid[cy][cx])
                        {
                            _crystal[cy][cx] = true;
                        }
                    }
                }

                // 4c. Spark shards every 6 rows, alternating sides, hugging
                //     the tunnel edge (scanning inward past rock/crystal
                //     cells) — collecting rewards flying close to the walls
                //     and spikes that increment 2 taught you to fear.
                if(cy > 6 && cy % 6 == 3)
                {
                    bool left = ((cy / 6) % 2) == 0;

                    for(int i = 0; i < 3; ++i)
                    {
                        int cx = left ? center - half_width + i : center + half_width - i;

                        if(cx > 0 && cx < map_cols - 1 && ! _solid[cy][cx] &&
                                ! _crystal[cy][cx])
                        {
                            _shard[cy][cx] = true;
                            break;
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

            // 6. Freeze the shard placement as the rebake() baseline.
            for(int cy = 0; cy < map_rows; ++cy)
            {
                for(int cx = 0; cx < map_cols; ++cx)
                {
                    _shard_home[cy][cx] = _shard[cy][cx];
                }
            }

            // 7. Bake cells.
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

        // Bake cells: crystal hazard / spark shard on open cells; solid next
        // to open = glowing rim (each section's rim color is its signature);
        // deep rock otherwise; open = empty.
        void _bake()
        {
            constexpr int section_rims[] = {rim_tile, rim2_tile, rim3_tile};

            for(int cy = 0; cy < map_rows; ++cy)
            {
                for(int cx = 0; cx < map_cols; ++cx)
                {
                    int tile_index = empty_tile;

                    if(_crystal[cy][cx])
                    {
                        tile_index = crystal_tile;
                    }
                    else if(_shard[cy][cx])
                    {
                        tile_index = shard_tile;
                    }
                    else if(_solid[cy][cx])
                    {
                        tile_index = _next_to_open(cx, cy) ?
                                section_rims[section_of_row(cy)] : rock_tile;
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

    cave_map cave;  // statics live in EWRAM: 4KB cells + 8KB cell masks

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
    bn::vector<bn::sprite_ptr, 8> spark_sprites;
    bn::vector<bn::sprite_ptr, 16> hint_sprites;
    bn::vector<bn::sprite_ptr, 24> over_sprites;
    bn::vector<bn::sprite_ptr, 8> section_sprites;
    bn::vector<bn::sprite_ptr, 24> title_sprites;

    // The SAME generic grid template drives wall collision, hazard sensing
    // AND shard pickup — only the predicate differs.
    gl::tile_grid grid(cell_px, [](int cx, int cy) {
        return cave.solid(cx, cy);
    });
    gl::tile_grid crystal_sensor(cell_px, [](int cx, int cy) {
        return cave.crystal(cx, cy);
    });
    gl::tile_grid shard_sensor(cell_px, [](int cx, int cy) {
        return cave.shard(cx, cy);
    });

    gl::body mote_body;
    gl::run_state run;
    gl::stage_track section_track(section_depths, 2);
    int depth = -1;      // force first HUD draw
    int max_depth_cells = 0;
    int sparks = 0;
    int sparks_drawn = -1;  // force first HUD draw
    int banner_frames = 0;  // section banner time left
    int flare = 0;       // frames of thrust-flare left
    int run_frames = 0;
    bn::fixed surge_y = surge_start;
    int surge_row = -1000;  // last drawn front row (forces first draw)
    bool on_title = true;

    // SRAM persistence (gl_save.h): BEST depth survives power cycles behind
    // a magic tag — fresh/foreign/erased SRAM loads as no save (BEST 0).
    gl::save_slot<int> best_slot("LDRIFT1");
    int saved_best = 0;

    if(best_slot.load(saved_best))
    {
        run.restore_best(saved_best);
    }

    // Camera chases a world position, clamped so the view never leaves the
    // map (shared by the title's frozen view and the gameplay chase).
    auto place_camera = [&](const bn::fixed_point& world_pos) {
        bn::fixed cam_x = bn::clamp(world_pos.x(),
                                    bn::fixed(-(map_width_px - bn::display::width()) / 2),
                                    bn::fixed((map_width_px - bn::display::width()) / 2));
        bn::fixed cam_y = bn::clamp(world_pos.y(),
                                    bn::fixed(-(map_height_px - bn::display::height()) / 2),
                                    bn::fixed((map_height_px - bn::display::height()) / 2));
        camera.set_position(cam_x, cam_y);
    };

    auto restart = [&] {
        mote_body.pos = spawn;
        mote_body.vel = bn::fixed_point(0, 0);
        max_depth_cells = 0;
        depth = -1;
        sparks = 0;
        sparks_drawn = -1;
        banner_frames = 0;
        flare = 0;
        run_frames = 0;
        surge_y = surge_start;
        surge_row = -1000;
        cave.rebake();  // restore rows the surge consumed + respawn shards
        bg_map.reload_cells_ref();
        over_sprites.clear();
        section_sprites.clear();
        section_track.reset();
        mote.set_visible(true);
        run.restart();

        if(hint_sprites.empty())
        {
            text_generator.generate(-110, -72, "LUMEN DRIFT - A THRUST",
                                    hint_sprites);
        }
    };

    auto game_over = [&] {
        run.fail(max_depth_cells);

        if(run.best() > saved_best)
        {
            // New all-time best: persist it right away (power-off safe).
            saved_best = run.best();
            best_slot.save(saved_best);
        }
        mote.set_visible(false);
        section_sprites.clear();
        banner_frames = 0;
        bn::bg_palettes::set_fade_intensity(bn::fixed(0.7));  // light is taken

        bn::string<24> score_line("DEPTH ");
        score_line += bn::to_string<6>(max_depth_cells);
        score_line += " - BEST ";
        score_line += bn::to_string<6>(run.best());

        bn::string<24> spark_line("SPARKS ");
        spark_line += bn::to_string<6>(sparks);

        text_generator.set_center_alignment();
        text_generator.generate(0, -24, "THE LIGHT IS TAKEN", over_sprites);
        text_generator.generate(0, -10, score_line, over_sprites);
        text_generator.generate(0, 4, spark_line, over_sprites);
        text_generator.generate(0, 20, "PRESS START", over_sprites);
        text_generator.set_left_alignment();
    };

    // Boot to the title card: world frozen at the cave mouth, mote resting
    // at the spawn, BEST restored from SRAM. START begins the first run.
    {
        mote_body.pos = spawn;
        bn::fixed_point world_pos = to_world(spawn);
        mote.set_position(world_pos);
        place_camera(world_pos);

        bn::string<16> best_line("BEST ");
        best_line += bn::to_string<6>(run.best());

        text_generator.set_center_alignment();
        text_generator.generate(0, -40, "LUMEN DRIFT", title_sprites);
        text_generator.generate(0, -8, best_line, title_sprites);
        text_generator.generate(0, 24, "PRESS START", title_sprites);
        text_generator.set_left_alignment();
    }

    while(true)
    {
        if(on_title)
        {
            // Title: nothing moves until START — physics starts on the
            // press, so scripted proof replays stay deterministic (press
            // START at a fixed frame = constant offset for every span).
            if(bn::keypad::start_pressed())
            {
                on_title = false;
                title_sprites.clear();
                restart();
            }

            bn::core::update();
            continue;
        }

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

        // Spark shards: the pickup box sweeps the generic grid sensor;
        // every touched shard cell is collected — score up, light decay
        // rewound (the cave visibly brightens back).
        int collected = 0;

        shard_sensor.visit_overlaps(mote_body.pos.x(), mote_body.pos.y(),
                                    pickup_half, pickup_half,
                                    [&](int cx, int cy) {
                                        cave.collect_shard(cx, cy);
                                        ++collected;
                                    });

        if(collected > 0)
        {
            sparks += collected;
            run_frames = bn::max(run_frames - collected * shard_light_refund, 0);
            flare = 30;
            bg_map.reload_cells_ref();
        }

        if(sparks != sparks_drawn)
        {
            sparks_drawn = sparks;
            spark_sprites.clear();
            bn::string<16> label("SPARKS ");
            label += bn::to_string<6>(sparks);
            text_generator.generate(-110, -52, label, spark_sprites);
        }

        // Cave sections: crossing a section's first row raises a banner
        // (depth is a high-water mark, so a section never announces twice
        // per run).
        if(section_track.update(max_depth_cells) && section_track.stage() > 0)
        {
            section_sprites.clear();
            text_generator.set_center_alignment();
            text_generator.generate(0, -40, section_names[section_track.stage()],
                                    section_sprites);
            text_generator.set_left_alignment();
            banner_frames = 150;
        }

        if(banner_frames > 0)
        {
            --banner_frames;

            if(banner_frames == 0)
            {
                section_sprites.clear();
            }
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
        place_camera(world_pos);

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
