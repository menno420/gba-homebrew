/*
 * LUMEN DRIFT — polish pass 3 (more cave beyond row 62).
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
 * Polish pass 2 (from the concept doc's polish list — audio, light radius,
 * tuning):
 *   - AUDIO: four original synthesized sounds (audio/generate_audio.py,
 *     deterministic stdlib-only generator; built via Butano's maxmod
 *     pipeline) — run start confirm, soft thrust rumble (retriggered while
 *     A is held), shard glitter, and a falling "light is taken" sweep.
 *     Every trigger also bumps a counter in gl_audio_hook (games/common/
 *     gl_audio_hook.h) so the headless harness can prove the sounds fired
 *     (screenshots cannot hear; see docs/capabilities.md).
 *   - LIGHT RADIUS AS A MECHANIC: the concept's original text imagined a
 *     visible light radius ("collect spark shards that extend your light
 *     radius before it decays"). The cave is now only visible inside a
 *     circle of light around the mote (gl_light_window.h over the GBA's
 *     hardware window): full view at run start, shrinking with the same
 *     light level that drives the palette fade, growing back when a shard
 *     rewinds the decay, puffing slightly on thrust flares. Sprites (HUD,
 *     banners, the mote itself) are never masked. Rendering-only — physics
 *     and every committed input-script replay are unchanged.
 *   - TUNING (the doc's named unknown: "making the light-decay feel good"):
 *     decay horizon 3600 -> 2700 frames (the darkness closes in ~25%
 *     sooner), fade cap 0.45 -> 0.40 (the lit circle stays readable now
 *     that the radius carries the pressure), shard refund 1500 -> 1200
 *     frames (per-shard relief keeps the same ~44% share of the shorter
 *     horizon). No physics constant changed.
 *
 * Polish pass 3 (the concept doc's LAST polish-list item — "more cave
 * beyond row 62"):
 *   - The stored 64-row hardware map used to BE the world: rows 62-63 were
 *     a solid floor, so every deep run bottomed out at DEPTH 58 on the same
 *     dead end and waited for the surge. Now every row's content is a PURE
 *     function of the world row and the stored map is a sliding 64-row
 *     window over an endless deterministic cave (GBA regular backgrounds
 *     wrap, so world row N simply lives in buffer row N % 64). Rows 0-61
 *     reproduce the committed layout cell-for-cell — the canonical replay
 *     passes unchanged — and the cave now continues past the old floor:
 *     THE DEEP's squeeze runs through row 63, then 20-row ECHO BANDS cycle
 *     the three section looks forever ("ECHOES OF THE BLUFFS/GALLERY/THE
 *     DEEP" banners, cycling rims, blobs/pillars/dense-crystal signatures)
 *     over one continuous tunnel spine, so band transitions always stay
 *     passable. Depth is once again bounded only by skill and the light.
 *   - Headless telemetry: hook slots 4-6 now publish the mote's map
 *     position and the depth high-water mark every frame, so deep-run
 *     proofs assert depth numerically (--assert-watch) instead of only
 *     reading HUD text.
 *
 * v1.1 — THE ECHOES DEEPEN (depth difficulty curve):
 *   - The endless echoes were difficulty-flat: every 60-row cycle repeated
 *     at the same crystal density and width forever. Now each full echo
 *     cycle past the first raises a depth tier — crystals come more often,
 *     the gallery echoes tighten from tier 2 — plateauing at tier 3 (row
 *     244) so generation stays periodic and check-cave.py-provable. The
 *     echo banners announce the tier ("ECHOES OF THE BLUFFS II"). Rows
 *     0-123 are cell-identical to v1.0, so every pinned replay is intact.
 *
 * v1.2 — GRAZE THE LIGHT (near-miss risk/reward):
 *   - Shaving a crystal spike WITHOUT dying now pays light back: a thin
 *     graze shell just outside the kill box senses each crystal cell the
 *     mote slips past, and every first touch of a cell (once per cell per
 *     run) rewinds part of the light decay, flares the lamp, whispers a
 *     new synthesized "tss" (ld_graze) and counts on a GRAZE HUD line +
 *     the game-over card. The crystals stop being pure keep-away: they are
 *     where the light lives, and v1.1's depth tiers now double as a richer
 *     fuel field for whoever dares to fly close. World generation, physics
 *     and every kill rule are UNTOUCHED — graze is read-only sensing over
 *     the same pure layout, so the cave proof (tools/check-cave.py) and
 *     every committed replay key script stay valid.
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
#include "bn_point.h"
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

#include "gl_audio_hook.h"
#include "gl_input.h"
#include "gl_kinematics.h"
#include "gl_light_window.h"
#include "gl_run_state.h"
#include "gl_save.h"
#include "gl_tile_grid.h"

#include "common_variable_8x8_sprite_font.h"

#include "bn_window.h"
#include "bn_sound_items.h"

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

    // Cave sections (polish pass 3): the three committed sections keep
    // their exact rows, and past the old row-62 floor the cave continues
    // FOREVER — 20-row echo bands cycle the three section looks (rims,
    // obstacles, hazard density) over one continuous tunnel spine. The
    // banner fires when the depth high-water mark crosses a section's
    // first row (depth = row - spawn row 3).
    constexpr int gallery_first_row = 24;
    constexpr int deep_first_row = 44;
    constexpr int echo_first_row = 64;   // first row past the old layout
    constexpr int echo_band_rows = 20;
    constexpr const char* section_names[] = {"THE BLUFFS", "THE GALLERY",
                                             "THE DEEP"};
    constexpr const char* echo_names[] = {"ECHOES OF THE BLUFFS",
                                          "ECHOES OF THE GALLERY",
                                          "ECHOES OF THE DEEP"};

    /// Unbounded section index: 0/1/2 are the committed sections, 3+ is
    /// one index per echo band.
    [[nodiscard]] constexpr int section_index_of_row(int cy)
    {
        if(cy < gallery_first_row)
        {
            return 0;
        }

        if(cy < deep_first_row)
        {
            return 1;
        }

        if(cy < echo_first_row)
        {
            return 2;
        }

        return 3 + (cy - echo_first_row) / echo_band_rows;
    }

    /// Which of the three section looks (rim/obstacles/hazards) an index
    /// wears — echo bands cycle them.
    [[nodiscard]] constexpr int section_look(int index)
    {
        return index < 3 ? index : (index - 3) % 3;
    }

    [[nodiscard]] constexpr const char* section_name(int index)
    {
        return index < 3 ? section_names[index] : echo_names[(index - 3) % 3];
    }

    // v1.1 — THE ECHOES DEEPEN (depth difficulty curve). The endless echo
    // bands used to repeat at one fixed difficulty forever: once a player
    // survived one 60-row cycle they could survive them all, and long runs
    // went flat. Now every full echo cycle (bluffs+gallery+deep, 60 rows)
    // past the first raises a DEPTH TIER: crystal clusters come more often
    // (the crystal step shrinks by the tier) and the gallery echoes tighten
    // by one cell from tier 2 — plateauing at tier 3 (rows 244+) so the
    // generation stays periodic and tools/check-cave.py can still prove
    // every junction of the endless cave passable (its periodicity proof
    // re-anchors at the plateau row; KEEP THE MIRROR IN LOCKSTEP). Banners
    // announce each tier with a numeral ("ECHOES OF THE DEEP III"). Rows
    // 0-123 are cell-for-cell identical to v1.0 — tier 1 starts at row 124,
    // beyond both CI replays' reach, so the pinned replays stay valid.
    constexpr int echo_cycle_bands = 3;  // bands per cycle (one of each look)
    constexpr int echo_tier_cap = 3;     // plateau: "IV" from row 244 down
    constexpr const char* tier_suffixes[] = {"", " II", " III", " IV"};

    /// Depth tier of a section index: 0 for the committed sections and the
    /// first echo cycle, +1 per full echo cycle after that, capped.
    [[nodiscard]] constexpr int echo_tier(int index)
    {
        return index < 3 ? 0
                         : bn::min((index - 3) / echo_cycle_bands,
                                   echo_tier_cap);
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

    // v1.2 GRAZE: the near-miss shell. A crystal cell KILLS while the
    // hazard box overlaps it (mote center closer than hazard_half + 4px =
    // 6px to the cell center); it GRAZES while only this bigger box does
    // (center within graze_half + 4px = 10px) — i.e. the open column right
    // next to a crystal is the graze lane, one more column out pays
    // nothing. Each crystal cell grazes once per run; the reward is a
    // light-decay rewind (a third of a shard), so flying close to death is
    // the light economy's risk premium. Sensing only — no physics, no
    // world-generation change.
    constexpr bn::fixed graze_half(6);
    constexpr int graze_light_refund = 400;  // decay frames per grazed cell

    // Light tuning (polish pass 2 — the doc's named unknown is "making the
    // light-decay feel good"; pass-2 values on the left, pass-1 on the
    // right): the decay horizon closes in ~25% sooner, the fade cap eases
    // slightly because the shrinking radius now carries the pressure, and
    // the per-shard refund keeps the same ~44% share of the horizon.
    constexpr int light_horizon = 2700;       // was 3600 frames
    constexpr bn::fixed fade_cap(0.40);       // was 0.45
    constexpr int shard_light_refund = 1200;  // was 1500 decay frames/shard

    // Light radius as a mechanic: the cave is only visible inside a circle
    // of light around the mote — full view at run start, shrinking to a
    // tight pool over the same light_horizon that drives the palette fade.
    // Thrust flares puff it back out a little; shards rewind run_frames and
    // visibly grow it back. Rendering-only (physics untouched).
    constexpr int radius_full = 160;  // run start: corners just lit (>144)
    constexpr int radius_min = 30;    // horizon end: a tight pool of light
    constexpr int radius_flare_bonus = 10;

    // Audio (polish pass 2): four original synthesized sounds, generated
    // deterministically by audio/generate_audio.py. Every play() bumps a
    // gl_audio_hook counter so headless proofs can assert the triggers.
    constexpr int audio_slot_start = 0;
    constexpr int audio_slot_thrust = 1;
    constexpr int audio_slot_shard = 2;
    constexpr int audio_slot_death = 3;
    constexpr int hook_slot_x = 4;      // mote map-x (bn::fixed raw data)
    constexpr int hook_slot_y = 5;      // mote map-y (bn::fixed raw data)
    constexpr int hook_slot_depth = 6;  // depth high-water mark (cells)
    constexpr int hook_slot_tick = 7;  // game-loop iterations (headless
                                       // overrun evidence: ticks ~= frames)
    constexpr int audio_slot_graze = 8;  // v1.2: cumulative grazed crystal
                                         // cells (the ld_graze whisper
                                         // fires on every frame this grows)
    constexpr int hook_slot_light = 9;   // v1.2: run_frames after refunds —
                                         // light-decay telemetry, so a graze
                                         // refund is asserted numerically
    constexpr int thrust_retrigger_frames = 20;  // rumble cadence while A held

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

    // --- Pure cave layout (polish pass 3) -------------------------------
    // Every row's shape/obstacles/hazards/shards are a pure function of
    // the WORLD row, not of stored state, so the cave no longer ends where
    // the stored map does. Rows 0-61 reproduce the committed generation
    // rules cell-for-cell (the canonical replay passes unchanged); the old
    // "rows 62-63 are floor" cutoff is gone and rows 62+ are new content.

    struct row_layout
    {
        int center = 16;
        int half_width = 0;
        int look = 0;          // 0 bluffs / 1 gallery / 2 deep signature
        bool blobs = false;    // bluffs-style edge blobs on this row
        bool pillar = false;   // gallery-style center pillar on this row
        int crystal_step = 7;
    };

    [[nodiscard]] row_layout layout_of_row(int cy)
    {
        int index = section_index_of_row(cy);
        row_layout row;
        row.look = section_look(index);

        if(index == 0)
        {
            // THE BLUFFS: the original medium tunnel.
            row.center = 16 + wave_a[(cy / 2) % 16] + wave_b[(cy / 3) % 8];
            row.half_width = bn::clamp(5 - (cy / 20), 2, 5);
        }
        else if(index == 1)
        {
            // THE GALLERY: wide slow-swaying halls (pillars below).
            row.center = 16 + 2 * wave_b[(cy / 4) % 8];
            row.half_width = 6;
        }
        else if(index == 2)
        {
            // THE DEEP: narrow, fast sway (the +8 phase offset makes the
            // row-44 passage line up under the gallery's last row), final
            // squeeze below row 54 — which now continues through the old
            // row-62 floor instead of dead-ending on it.
            row.center = 16 + wave_a[(cy + 8) % 16] + wave_b[(cy / 2) % 8];
            row.half_width = cy < 54 ? 3 : 2;
        }
        else
        {
            // THE ECHOES (rows 64+): one continuous spine — the bluffs
            // waveform keyed on the absolute row, so every band-to-band
            // transition stays passable (adjacent centers differ by at
            // most 3 columns and every band is at least 2 wide) — while
            // each 20-row band wears one section's look: bluffs blobs, a
            // hair-narrower gallery with its pillars, or the deep's tight
            // corridor with denser crystals. v1.1: the gallery echoes
            // tighten by one more cell from depth tier 2 (cycle boundaries
            // are band boundaries, so the width never changes mid-band).
            row.center = 16 + wave_a[(cy / 2) % 16] + wave_b[(cy / 3) % 8];
            row.half_width = row.look == 1 ? (echo_tier(index) >= 2 ? 3 : 4)
                                           : 2;
        }

        row.center = bn::clamp(row.center, 4, map_cols - 5);
        row.blobs = row.look == 0 && cy > 10 && cy % 9 == 4;
        row.pillar = row.look == 1 && cy >= 26 && cy % 6 == 1;

        // v1.1 depth tiers: crystal clusters come one row-step sooner per
        // tier (tier is 0 everywhere the committed replays reach), floored
        // so even the deep echoes keep a survivable rhythm.
        row.crystal_step = bn::max((row.look == 2 ? 5 : 7) - echo_tier(index),
                                   3);
        return row;
    }

    /// Pure solidity of one world row: borders, starting chamber, tunnel
    /// carve, blobs/pillars, rest plate (the committed rules verbatim).
    void row_solid(int cy, bool (&out)[map_cols])
    {
        for(int cx = 0; cx < map_cols; ++cx)
        {
            out[cx] = true;
        }

        if(cy <= 0)
        {
            return;  // the world border above the cave mouth
        }

        // Starting chamber under the ceiling.
        if(cy <= 6)
        {
            for(int cx = 10; cx <= 22; ++cx)
            {
                out[cx] = false;
            }
        }

        if(cy >= 5)
        {
            row_layout row = layout_of_row(cy);

            for(int cx = row.center - row.half_width;
                cx <= row.center + row.half_width; ++cx)
            {
                if(cx > 0 && cx < map_cols - 1)
                {
                    out[cx] = false;
                }
            }

            // BLUFFS blobs every 9 rows, alternating sides — the passable
            // gap stays on the other side.
            if(row.blobs)
            {
                bool left = ((cy / 9) % 2) == 0;

                for(int i = 0; i < 2; ++i)
                {
                    int cx = left ? row.center - row.half_width + i
                                  : row.center + row.half_width - i;

                    if(cx > 0 && cx < map_cols - 1)
                    {
                        out[cx] = true;
                    }
                }
            }

            // GALLERY pillar: a rock pillar splits the hall — pick a side.
            if(row.pillar)
            {
                for(int cx = row.center - 1; cx <= row.center + 1; ++cx)
                {
                    if(cx > 0 && cx < map_cols - 1)
                    {
                        out[cx] = true;
                    }
                }
            }
        }

        // Rest plate straight under the spawn: a no-input run settles here.
        if(cy == 10)
        {
            for(int cx = 15; cx <= 17; ++cx)
            {
                out[cx] = true;
            }
        }
    }

    /// Pure hazards/pickups of one world row, given its solidity: crystal
    /// spike clusters + spark shards (the committed placement rules
    /// verbatim — only open cells turn deadly, so the passage keeps >= 2
    /// clear cells; shards hug the tunnel edge, scanning inward).
    ///
    /// Junction guard (review-queue row #23): a crystal cluster must never
    /// cover EVERY column this row shares open with the row above or below
    /// — at echo-band width changes the shared span can shrink to exactly
    /// the 2 cluster cells, turning the junction into an unavoidable death
    /// gate. Each crystal cell is therefore only kept if at least one
    /// crystal-free shared-open column survives toward BOTH neighbours.
    /// Neighbour solidity is itself pure, so this stays a pure function of
    /// the world row; on every already-clean row (including all committed
    /// rows 0-63 and both CI replay tiers' reach) the guard is a no-op,
    /// because any subset of a placement that ends clean is clean too.
    /// Host-side mirror + full-period proof: tools/check-cave.py (CI runs
    /// it per PR — keep the two in lockstep).
    void row_features(int cy, const bool (&solid)[map_cols],
                      bool (&crystal)[map_cols], bool (&shard)[map_cols])
    {
        for(int cx = 0; cx < map_cols; ++cx)
        {
            crystal[cx] = false;
            shard[cx] = false;
        }

        if(cy < 5)
        {
            return;
        }

        row_layout row = layout_of_row(cy);

        if(cy > 12 && cy % row.crystal_step == 2)
        {
            bool above[map_cols];
            bool below[map_cols];
            row_solid(cy - 1, above);
            row_solid(cy + 1, below);

            auto junction_open = [&](const bool (&neighbour)[map_cols]) {
                for(int jx = 1; jx < map_cols - 1; ++jx)
                {
                    if(! solid[jx] && ! neighbour[jx] && ! crystal[jx])
                    {
                        return true;
                    }
                }

                return false;
            };

            bool left = ((cy / row.crystal_step) % 2) == 0;

            for(int i = 0; i < 2; ++i)
            {
                int cx = left ? row.center - row.half_width + i
                              : row.center + row.half_width - i;

                if(cx > 0 && cx < map_cols - 1 && ! solid[cx])
                {
                    crystal[cx] = true;

                    if(! junction_open(above) || ! junction_open(below))
                    {
                        crystal[cx] = false;  // spare the junction column
                    }
                }
            }
        }

        if(cy > 6 && cy % 6 == 3)
        {
            bool left = ((cy / 6) % 2) == 0;

            for(int i = 0; i < 3; ++i)
            {
                int cx = left ? row.center - row.half_width + i
                              : row.center + row.half_width - i;

                if(cx > 0 && cx < map_cols - 1 && ! solid[cx] && ! crystal[cx])
                {
                    shard[cx] = true;
                    break;
                }
            }
        }
    }

    // The stored map is a sliding 64-row WINDOW over the endless pure
    // layout: world row N lives in buffer row N % 64, and GBA regular
    // backgrounds wrap, so the hardware shows the right cells with no
    // extra work. The only mutable state: the window position, the shards
    // collected this run, and the surge front row (so rows the ember wall
    // consumed re-bake as consumed).
    class cave_map
    {

    public:
        bn::regular_bg_map_item map_item;

        cave_map() :
            map_item(_cells[0], bn::size(map_cols, map_rows))
        {
            rebake();
        }

        [[nodiscard]] bool solid(int cx, int cy) const
        {
            if(cx < 0 || cx >= map_cols || cy < 0)
            {
                return true;  // the side/top world border is impassable rock
            }

            bool here[map_cols];
            row_solid(cy, here);
            return here[cx];
        }

        [[nodiscard]] bool crystal(int cx, int cy) const
        {
            if(cx < 0 || cx >= map_cols || cy < 0)
            {
                return false;  // out-of-map space is rock, not hazard
            }

            bool here[map_cols];
            bool crystal_row[map_cols];
            bool shard_row[map_cols];
            row_solid(cy, here);
            row_features(cy, here, crystal_row, shard_row);
            return crystal_row[cx];
        }

        [[nodiscard]] bool shard(int cx, int cy) const
        {
            if(cx < 0 || cx >= map_cols || cy < 0 || _collected(cx, cy))
            {
                return false;  // out-of-map space is rock, not pickup
            }

            bool here[map_cols];
            bool crystal_row[map_cols];
            bool shard_row[map_cols];
            row_solid(cy, here);
            row_features(cy, here, crystal_row, shard_row);
            return shard_row[cx];
        }

        /// Take the shard at (cx, cy): remembers the pickup for this run
        /// and clears its cell. Caller reloads the bg map afterwards.
        void collect_shard(int cx, int cy)
        {
            if(_taken.full())
            {
                // Recycle a slot whose shard can no longer re-bake (it is
                // outside the window; at most ~11 shards fit in 64 rows,
                // so an in-window entry always survives eviction).
                for(auto it = _taken.begin(); it != _taken.end(); ++it)
                {
                    if(! _in_window(it->y()))
                    {
                        _taken.erase(it);
                        break;
                    }
                }

                if(_taken.full())
                {
                    _taken.pop_back();  // unreachable; stay safe anyway
                }
            }

            _taken.push_back(bn::point(cx, cy));

            if(_in_window(cy))
            {
                _set_cell(cx, cy, empty_tile);
            }
        }

        /// Reset for a new run: window back to the cave mouth, collected
        /// shards respawned, surge-consumed rows restored. Caller reloads
        /// the bg map afterwards.
        void rebake()
        {
            _window_top = 0;
            _surge_row = -1000;
            _taken.clear();

            for(int cy = 0; cy < map_rows; ++cy)
            {
                _bake_row(cy);
            }
        }

        /// Slide the window so world rows [first_row, last_row] are all
        /// stored, baking rows that scroll in (from the pure layout, minus
        /// collected shards, plus the surge's consumed overlay). True if
        /// any cell changed — caller reloads the bg map.
        bool ensure_window(int first_row, int last_row)
        {
            bool changed = false;

            if(first_row < 0)
            {
                first_row = 0;
            }

            while(_window_top + map_rows - 1 < last_row)
            {
                ++_window_top;
                _bake_row(_window_top + map_rows - 1);
                changed = true;
            }

            while(_window_top > first_row)
            {
                --_window_top;
                _bake_row(_window_top);
                changed = true;
            }

            return changed;
        }

        /// The surge front's world row — rows above it re-bake consumed.
        void set_surge_row(int cy)
        {
            _surge_row = cy;
        }

        /// Overwrite one row's cells with a single tile (visual only —
        /// solidity is untouched). Used by the surge; rows outside the
        /// window are skipped (they are off-screen). rebake() restores.
        void overwrite_row(int cy, int tile_index)
        {
            if(! _in_window(cy))
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
        bn::vector<bn::point, 24> _taken;  // shards collected this run
        int _window_top = 0;   // world row living in the buffer's top row
        int _surge_row = -1000;

        [[nodiscard]] bool _in_window(int cy) const
        {
            return cy >= _window_top && cy <= _window_top + map_rows - 1;
        }

        [[nodiscard]] bool _collected(int cx, int cy) const
        {
            for(const bn::point& taken : _taken)
            {
                if(taken.x() == cx && taken.y() == cy)
                {
                    return true;
                }
            }

            return false;
        }

        void _set_cell(int cx, int cy, int tile_index)
        {
            // World row -> buffer row: the window guarantees no two stored
            // world rows share a buffer row, and the wrapping hardware bg
            // picks the right one from the scroll offset alone.
            bn::regular_bg_map_cell& cell =
                    _cells[map_item.cell_index(cx, cy % map_rows)];
            bn::regular_bg_map_cell_info cell_info(cell);
            cell_info.set_tile_index(tile_index);
            cell_info.set_palette_id(0);
            cell = cell_info.cell();
        }

        // Bake one world row's cells from the pure layout: crystal hazard /
        // spark shard on open cells; solid next to open = glowing rim (each
        // section look's rim color is its signature); deep rock otherwise;
        // open = empty. Rows at or above the surge front come back already
        // consumed.
        void _bake_row(int cy)
        {
            if(cy <= _surge_row)
            {
                for(int cx = 0; cx < map_cols; ++cx)
                {
                    _set_cell(cx, cy, cy == _surge_row ? surge_tile
                                                       : consumed_tile);
                }

                return;
            }

            bool above[map_cols];
            bool here[map_cols];
            bool below[map_cols];
            bool crystal_row[map_cols];
            bool shard_row[map_cols];
            row_solid(cy - 1, above);
            row_solid(cy, here);
            row_solid(cy + 1, below);
            row_features(cy, here, crystal_row, shard_row);

            constexpr int section_rims[] = {rim_tile, rim2_tile, rim3_tile};
            int rim = section_rims[section_look(section_index_of_row(cy))];

            for(int cx = 0; cx < map_cols; ++cx)
            {
                int tile_index = empty_tile;

                if(crystal_row[cx])
                {
                    tile_index = crystal_tile;
                }
                else if(shard_row[cx] && ! _collected(cx, cy))
                {
                    tile_index = shard_tile;
                }
                else if(here[cx])
                {
                    tile_index = _next_to_open(above, here, below, cx) ?
                            rim : rock_tile;
                }

                _set_cell(cx, cy, tile_index);
            }
        }

        [[nodiscard]] static bool _next_to_open(const bool (&above)[map_cols],
                                                const bool (&here)[map_cols],
                                                const bool (&below)[map_cols],
                                                int cx)
        {
            const bool (*rows[3])[map_cols] = {&above, &here, &below};

            for(int dy = 0; dy < 3; ++dy)
            {
                for(int dx = -1; dx <= 1; ++dx)
                {
                    int nx = cx + dx;

                    if(nx >= 0 && nx < map_cols && ! (*rows[dy])[nx])
                    {
                        return true;
                    }
                }
            }

            return false;
        }
    };

    cave_map cave;  // statics live in EWRAM: 4KB cells + the shard list

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

    // Light radius: the cave bg is hidden outside the light circle (the
    // hardware window). Sprites — HUD, banners, the mote — never mask.
    gl::light_window light;
    bn::window::outside().set_show_bg(bg, false);

    bn::sprite_ptr mote = bn::sprite_items::ld_mote.create_sprite(0, 0);
    mote.set_camera(camera);

    bn::sprite_text_generator text_generator(common::variable_8x8_sprite_font);
    bn::vector<bn::sprite_ptr, 8> depth_sprites;
    bn::vector<bn::sprite_ptr, 8> spark_sprites;
    bn::vector<bn::sprite_ptr, 8> graze_sprites;
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
    // v1.2 graze: the SAME crystal predicate swept with the bigger shell
    // box — cells it reports that the kill box does not are near-misses.
    gl::tile_grid graze_sensor(cell_px, [](int cx, int cy) {
        return cave.crystal(cx, cy);
    });

    gl::body mote_body;
    gl::run_state run;
    int section_index = 0;  // high-water section (banner fires on raise)
    int depth = -1;      // force first HUD draw
    int max_depth_cells = 0;
    int sparks = 0;
    int sparks_drawn = -1;  // force first HUD draw
    int grazes = 0;         // v1.2: crystal cells grazed this run
    int grazes_drawn = -1;  // force first HUD draw
    bn::vector<bn::point, 16> grazed;  // cells already grazed this run (a
                                       // cell pays once — no hover farming)
    int banner_frames = 0;  // section banner time left
    int flare = 0;       // frames of thrust-flare left
    int thrust_frames = 0;  // consecutive A-held frames (sound retrigger)
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
    // cave sideways or above the mouth (shared by the title's frozen view
    // and the gameplay chase). No bottom clamp any more — the cave is
    // endless downward (polish pass 3).
    auto place_camera = [&](const bn::fixed_point& world_pos) {
        bn::fixed cam_x = bn::clamp(world_pos.x(),
                                    bn::fixed(-(map_width_px - bn::display::width()) / 2),
                                    bn::fixed((map_width_px - bn::display::width()) / 2));
        bn::fixed cam_y = bn::max(world_pos.y(),
                                  bn::fixed(-(map_height_px - bn::display::height()) / 2));
        camera.set_position(cam_x, cam_y);
    };

    auto restart = [&] {
        mote_body.pos = spawn;
        mote_body.vel = bn::fixed_point(0, 0);
        max_depth_cells = 0;
        depth = -1;
        sparks = 0;
        sparks_drawn = -1;
        grazes = 0;
        grazes_drawn = -1;
        grazed.clear();
        banner_frames = 0;
        flare = 0;
        thrust_frames = 0;
        run_frames = 0;
        surge_y = surge_start;
        surge_row = -1000;
        cave.rebake();  // restore rows the surge consumed + respawn shards
        bg_map.reload_cells_ref();
        over_sprites.clear();
        section_sprites.clear();
        section_index = 0;
        mote.set_visible(true);
        run.restart();
        light.set(to_world(spawn) - camera.position(), radius_full);
        bn::sound_items::ld_start.play(bn::fixed(0.7));
        gl::count_audio(audio_slot_start);

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
        bn::sound_items::ld_death.play(bn::fixed(0.9));
        gl::count_audio(audio_slot_death);
        bn::bg_palettes::set_fade_intensity(bn::fixed(0.7));  // light is taken

        bn::string<24> score_line("DEPTH ");
        score_line += bn::to_string<6>(max_depth_cells);
        score_line += " - BEST ";
        score_line += bn::to_string<6>(run.best());

        bn::string<24> spark_line("SPARKS ");
        spark_line += bn::to_string<6>(sparks);

        bn::string<24> graze_line("GRAZE ");
        graze_line += bn::to_string<6>(grazes);

        text_generator.set_center_alignment();
        text_generator.generate(0, -24, "THE LIGHT IS TAKEN", over_sprites);
        text_generator.generate(0, -10, score_line, over_sprites);
        text_generator.generate(0, 4, spark_line, over_sprites);
        text_generator.generate(0, 18, graze_line, over_sprites);
        text_generator.generate(0, 32, "PRESS START", over_sprites);
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
        gl::count_audio(hook_slot_tick);

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

            if(thrust_frames % thrust_retrigger_frames == 0)
            {
                // Soft rumble, retriggered on a cadence while held (the
                // sample is longer than the cadence, so it overlaps into a
                // continuous burn instead of stuttering).
                bn::sound_items::ld_thrust.play(bn::fixed(0.4));
                gl::count_audio(audio_slot_thrust);
            }

            ++thrust_frames;
        }
        else
        {
            thrust_frames = 0;
        }

        mote_body.accelerate(gl::dpad_vector() * lateral);
        mote_body.damp_x(drag_x);
        mote_body.clamp_velocity(max_speed_x, max_speed_y);
        grid.move(mote_body, mote_half, mote_half);

        // Depth score = cells descended from spawn (high-water mark).
        int depth_cells = ((mote_body.pos.y() - spawn.y()) / cell_px).floor_integer();
        max_depth_cells = bn::max(max_depth_cells, depth_cells);

        // Headless telemetry (hook slots 4-6): exact mote position + the
        // depth high-water mark, readable over the bus every frame — deep
        // runs are asserted numerically, not just via HUD text.
        gl_audio_hook[hook_slot_x] = unsigned(mote_body.pos.x().data());
        gl_audio_hook[hook_slot_y] = unsigned(mote_body.pos.y().data());
        gl_audio_hook[hook_slot_depth] = unsigned(max_depth_cells);

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
            bn::sound_items::ld_shard.play(bn::fixed(0.8));
            gl::count_audio(audio_slot_shard);
        }

        if(sparks != sparks_drawn)
        {
            sparks_drawn = sparks;
            spark_sprites.clear();
            bn::string<16> label("SPARKS ");
            label += bn::to_string<6>(sparks);
            text_generator.generate(-110, -52, label, spark_sprites);
        }

        // v1.2 graze HUD (counter updates at the end of the loop, so the
        // line redraws on the frame after a graze). y = -30, NOT -42: the
        // centered section banners render at y = -40 and a line there
        // overlaps their leading glyphs (found by the deep-run replay's
        // banner text asserts failing on the first draft).
        if(grazes != grazes_drawn)
        {
            grazes_drawn = grazes;
            graze_sprites.clear();
            bn::string<16> label("GRAZE ");
            label += bn::to_string<6>(grazes);
            text_generator.generate(-110, -30, label, graze_sprites);
        }

        // Cave sections: crossing a section's first row raises a banner
        // (depth is a high-water mark, so a section never announces twice
        // per run) — now unbounded: past the old row-62 floor, every
        // 20-row echo band announces itself as it is reached.
        int reached_index = section_index_of_row(max_depth_cells + 3);

        if(reached_index > section_index)
        {
            section_index = reached_index;
            section_sprites.clear();

            // v1.1: echo banners carry the depth tier's numeral, so the
            // player is told the cave is done being polite ("ECHOES OF
            // THE DEEP III").
            bn::string<32> banner(section_name(section_index));
            banner += tier_suffixes[echo_tier(section_index)];

            text_generator.set_center_alignment();
            text_generator.generate(0, -40, banner, section_sprites);
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
            cave.set_surge_row(front_row);

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

        bn::fixed decay = bn::min(bn::fixed(run_frames) / light_horizon, fade_cap);

        if(flare > 0)
        {
            decay = bn::max(decay - bn::fixed(0.15), bn::fixed(0));
        }

        bn::bg_palettes::set_fade_intensity(decay);

        // Camera chases the mote, clamped so the view never leaves the map
        // sideways or above the mouth.
        bn::fixed_point world_pos = to_world(mote_body.pos);
        mote.set_position(world_pos);
        place_camera(world_pos);

        // Slide the stored-map window with the view: rows scrolling in
        // from below (or back in from above) bake from the pure layout
        // just before they can appear.
        {
            int camera_map_y = (camera.y() + map_height_px / 2).floor_integer();
            int first_row = (camera_map_y - bn::display::height() / 2) / cell_px - 2;
            int last_row = (camera_map_y + bn::display::height() / 2) / cell_px + 2;

            if(cave.ensure_window(first_row, last_row))
            {
                bg_map.reload_cells_ref();
            }
        }

        // Light radius (the mechanic): visibility itself shrinks with the
        // light level — full view at run start, a tight pool at horizon end;
        // shards rewind run_frames so the circle visibly grows back, and a
        // thrust flare puffs it slightly. On game over the circle simply
        // freezes with the rest of the world (the fail card is sprites).
        int radius = radius_full - bn::min(run_frames, light_horizon)
                * (radius_full - radius_min) / light_horizon;

        if(flare > 0)
        {
            radius += radius_flare_bonus;
        }

        light.set(world_pos - camera.position(), radius);

        // Hazards end the run: crystal spikes (forgiving sub-box via the
        // generic grid sensor) or the surge front catching up.
        if(crystal_sensor.overlaps(mote_body.pos.x(), mote_body.pos.y(),
                                   hazard_half, hazard_half) ||
                mote_body.pos.y() - mote_half < surge_y)
        {
            game_over();
        }
        else
        {
            // v1.2 GRAZE — alive next to death: every crystal cell inside
            // the graze shell (and this frame provably NOT inside the kill
            // box, or we would be in the branch above) pays a light-decay
            // rewind the first time this run touches it. The refund lands
            // after this frame's fade/radius were computed — the lamp
            // visibly swells on the NEXT frame, like the shard flare.
            int new_grazes = 0;

            graze_sensor.visit_overlaps(mote_body.pos.x(), mote_body.pos.y(),
                                        graze_half, graze_half,
                                        [&](int cx, int cy) {
                                            for(const bn::point& cell : grazed)
                                            {
                                                if(cell.x() == cx && cell.y() == cy)
                                                {
                                                    return;
                                                }
                                            }

                                            if(grazed.full())
                                            {
                                                // Forget the oldest graze —
                                                // re-earning a 16-cells-ago
                                                // near-miss is play, not
                                                // farming.
                                                grazed.erase(grazed.begin());
                                            }

                                            grazed.push_back(bn::point(cx, cy));
                                            ++new_grazes;
                                        });

            if(new_grazes > 0)
            {
                grazes += new_grazes;
                run_frames = bn::max(run_frames - new_grazes * graze_light_refund, 0);
                flare = 20;
                bn::sound_items::ld_graze.play(bn::fixed(0.5));

                for(int i = 0; i < new_grazes; ++i)
                {
                    gl::count_audio(audio_slot_graze);
                }
            }
        }

        // v1.2 light telemetry: run_frames AFTER any graze refund, so a
        // graze is asserted numerically (the word drops by the refund on
        // the graze frame).
        gl_audio_hook[hook_slot_light] = unsigned(run_frames);

        bn::core::update();
    }
}
