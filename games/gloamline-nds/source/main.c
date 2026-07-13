// Gloamline — arc slice 11: BEST-NIGHT REMATCH (on the slice-10
// watch-map polish; the first slice past the concept doc's completed
// LATER-SLICES tree, on the owner's continue order).
//
// Slice 9 wrote the promise into its own player text ("because every
// run is seed-deterministic, the recorded seed means your best night
// is *literally replayable* — start a run when the frame counter
// matches...") and left it a wink: a human cannot time a frame
// counter. This slice makes it a verb. SELECT at the TITLE SCREEN or
// the DEATH CARD — when a record exists — starts a run on the
// RECORDED best seed instead of the frame-counter latch (the pure
// gl_run_seed; the offer gate is the pure gl_rematch_available), so
// the very night the record was set replays spawn for spawn:
// determinism is the feature, now on a button. The death card is the
// natural seat (TITLE is unreachable after the first START — a
// title-only rematch would be once per power-on); with NO record
// SELECT stays completely inert on both screens (the moor keeps no
// empty boasts — the same rule as the title/card BEST lines). A
// rematch run wears a '*' after its SEED on the HUD and the cards —
// one console character, render-only. START keeps the old
// fresh-latch path bit-identical everywhere, and a rematch feeds
// NOTHING new into the sim (the recorded seed is just a seed), so
// every pre-slice-11 pin holds bit-identically: no committed route
// presses SELECT at the title or the death card. Beating your record
// ON the rematch improves the record as any dawn does (same seed —
// the strictly-better rule and the wear discipline are untouched).
// No new audio cue (the nightfall toll already rings every path into
// a night, rematch included). Telemetry appends slots 72-79 (rematch
// flag, rematches this power-on, 6 spares) with slots 0-71 frozen;
// the mailbox grows 72 -> 80 words.
//
// Below this line the slice-10 story still applies verbatim:
//
// Gloamline — arc slice 10: WATCH-MAP POLISH (on the slice-9
// best-nights save).
//
// The concept doc's LAST later-slice cut ("watch-map polish"),
// honoring its own watch-map words ("Touch optional, never required
// (tap the map to drop a marker at most); the game is 100% playable
// on buttons"). Three additions, all bottom-screen:
// (1) THE CHALK MARK — X chalks a '!' mark on the watch-map at the
// lamplighter's own position and clears it again (buttons-first,
// BINDING); tapping the map plot is the optional stylus alias that
// drops or moves the mark to the tapped cell. The map cell geometry
// moved VERBATIM into the pure layer (gl_map_col/gl_map_row) and the
// tap placement is the pure gl_mark_of_touch/gl_mark_of_cell —
// mid-span inverse with an EXACT cell round-trip (the mark renders in
// the very cell you tapped; proved host-side for every plot cell and
// every LCD pixel). The mark is chalk ON THE MAP, not a thing in the
// yard: NOTHING in the sim reads it (the dead ignore chalk), so every
// pre-slice-10 pin holds bit-identically. It persists across nights
// within a run (chalk doesn't wash off at dawn) and wipes on a fresh
// run. (2) THE WATCH LINE — "OUT n" under the plot counts tonight's
// dead still out in the gloam (pure gl_gloam_out), plus "! MARK d"
// (the mark's range in map cells) while the chalk is down; redrawn
// only when a value flips (frame-budget rail). (3) THE RECORD ON THE
// MAP HEADER — "BEST n" rides the watch-map during play (pure render
// of the slice-9 state). Telemetry appends slots 64-71 (mark
// on/x/y/dist, marks chalked, gloam-out) with slots 0-63 frozen; the
// mailbox grows 64 -> 72 words. No new audio cue (chalk is silent —
// a new id would outrank or renumber the pinned priority ranking).
//
// Below this line the slice-9 story still applies verbatim:
//
// Gloamline — arc slice 9: SAVE-FILE BEST-NIGHTS (on the slice-8
// synthesized audio).
//
// The concept doc's "save-file best-nights": the lamplighter's best-run
// record — most nights survived in one run, and the seed of that run so
// the owner can replay it (determinism is the feature) — persists
// across power cycles on the cartridge backup chip, read/written with
// the libnds card-EEPROM calls over the card SPI bus (the battery save
// DeSmuME emulates as a .dsv file). The record blob, its checksum, the
// decode ("corrupt/blank/future-version save = fresh table, NEVER a
// crash") and the update rule ("strictly better only") are all pure
// gl_sim.h/.c functions, mirrored three ways like every other rule.
// This file only adds the I/O glue at two state edges, both OFF the
// per-frame path: ONE bounded EEPROM read at power-on (before the main
// loop — a bad blob falls back to the fresh table), and ONE page write
// on a dawn that strictly improves the record (wear discipline: never
// per frame, never at death — a death cannot improve a best-nights
// record; the write rides the dawn-card state flip, a frame that
// already rebuilds the console). Saves feed NOTHING back into the
// night's sim — the record is displayed (title / cards) and persisted,
// so every pre-slice-9 pin holds bit-identically. Telemetry appends
// slots 56-63 (best nights, best seed, save-loaded flag, writes this
// power-on, format version) with slots 0-55 frozen. Honest rail:
// headless CI proves the DeSmuME battery path exactly; a real
// cartridge's backup chip (type, size, wear) is owner-hardware-only —
// GL_SAVE_EEPROM_TYPE is a decide-and-flag constant, not a probe.
//
// Below this line the slice-8 story still applies verbatim:
//
// Gloamline — arc slice 8: SYNTHESIZED AUDIO (on the slice-7 lantern
// oil).
//
// The concept doc's "synthesized audio set": the night gets a voice,
// 100% synthesized in code — no sampled or binary audio asset exists
// anywhere in this repo. Playback is the NDS hardware PSG channels
// (square + noise generators), driven by the default BlocksDS ARM7
// core over the libnds sound FIFO — per session 27's guard recipe the
// ARM9 (1 modeled scanline of headroom) spends only a handful of
// integer compares per frame; the tone generation itself runs in
// hardware on the ARM7 side. Two voices: (1) an AMBIENCE DRONE, a low
// square wave whose pitch/duty/volume track the lantern via the pure
// gl_amb_tier — calm in the daylight interlude, the gloam hum on a lit
// night, higher/wider once the light gutters, a throb while the dark
// press is live on the nearest of the dead; the FIFO is touched ONLY
// when the tier flips (the channel free-runs between flips), and the
// drone cuts out on the cards (dying silences the moor). (2) a CUE
// channel for one-shot chiptune SFX: nightfall toll, shove thump,
// plank knock, breach splinter, cache/flask pickups, the dawn bell,
// the death rattle — parameters are pure gl_cue_* table rows, and on a
// frame with several events the highest cue id wins the channel (id
// order IS the priority order, mirrored). Audio feeds NOTHING back
// into the sim — it is render-like output, so every pre-slice-8 pin
// holds bit-identically. Telemetry appends slots 48-55 (tier, drone
// freq, last cue, cue count, ...) with slots 0-47 frozen; headless CI
// proves the DECISION chain exactly and honestly cannot hear the
// result (owner playtest for the mix).
//
// Below this line the slice-7 story still applies verbatim:
//
// Gloamline — arc slice 7: LANTERN-OIL LIGHT PRESSURE (on the slice-6
// scavenge interlude).
//
// The concept doc's stated pressure ("lantern oil — light radius
// shrinks as it burns, dark edges spawn faster; the Lumen-Drift-style
// decay-you-can-buy-back pressure"): the lantern burns GL_OIL_BURN oil
// every NIGHT frame. While the tank holds GL_OIL_LOW or more the light
// is full and NOTHING changes — a fresh lantern (GL_OIL_MAX = 3 nights
// of burn) outlasts every pre-slice-7 proof window, so every old pin
// holds bit-identically. Once the oil runs LOW the light radius
// gutters toward GL_LIGHT_R_MIN (never out — the pressure is speed,
// not an unwinnable blackout), the yard's backdrop dims with it, and
// the dead beyond the lamplight stop hesitating: gl_dark_press cancels
// the 1-in-4 stagger skip outside the light circle, so the gloam
// presses ~33% faster where you cannot see. Buy-back: GL_FLASK_COUNT
// oil flasks lie in the scavenge interlude on their own salted pure
// schedule, each worth GL_OIL_FLASK (one night), capped at GL_OIL_MAX
// and NEVER consumed on a full tank (no flask is wasted). The START
// skip path grants NO oil — like the planks, the interlude is the only
// source. Daylight burns nothing and presses nobody. Fifth
// code-authored sprite (the oil flask); watch-map gains 'o' flask
// marks + an OIL gauge bar (the concept doc's promised bottom-screen
// oil gauge), redrawn only when a gauge cell changes (frame-budget
// rail); the HUD shows the tank as O<percent>.
//
// Below this line the slice-6 story still applies verbatim:
//
// The concept doc's next feature cut (docs/concepts/gloamline-concept.md
// slice order) and slice 5's own flag ("the scavenge interlude NEXT
// slice becomes the real plank source"): at the dawn card, SELECT
// enters a timed daylight phase in the same yard. The lamplighter
// returns to the lamppost, the dawn light drives the leftover dead
// back to the fence line (each to its own night spawn point — pure,
// proven outside the safe radius), and GL_CACHE_COUNT plank caches lie
// scattered on a pure f(seed, night, index) schedule. Walk within
// reach of a cache to pocket a plank — never on a full pocket (no
// cache is wasted; it stays on the ground). START leaves early; when
// the dawn light runs out the next night begins. Taking the interlude
// REPLACES the flat gl_planks_at_dawn() skip grant: loot what you
// carry out. The dead keep walking and CONTACT STILL KILLS — the
// interlude is bought planks, not safety. START at the dawn card keeps
// the old skip-straight-to-night path (with the flat grant)
// bit-identical, so every pre-slice-6 proof holds unchanged.
// Everything else is slice 5: barricades (B place/repair, chew-through
// + breach, persistence), waves, SHOVE (A), top screen = the yard,
// bottom = watch-map (zombie dots + barricade marks + cache marks +
// dawn bar), contact = death -> card -> instant restart. All game
// rules live in the pure layer (gl_sim.h/.c, mirrored by
// tools/check-gloam.py); this file is input, state machine, rendering,
// and the telemetry mailbox.
//
// GL_STRESS (make GL_STRESS=1 — CI perf proof, NEVER shipped): night 1
// spawns the full 24-Shambler cap at frame 0, PRE-PLACES the full
// 8-barricade ring around the player start (so the crowd chews and the
// block/draw paths all run), and contact does not kill, so an idle
// headless run measures the honest worst-case frame cost via the
// GL_T_VLINES / GL_T_VLMAX scanline telemetry. Every sim/render code
// path is identical to the shipped build.
//
// Determinism: seed = frame counter latched at the START press that begins
// a run (printed on the HUD and cards, so any human run is reproducible);
// scripted CI presses START on a fixed frame, so the whole run is a pure
// function of the input script. No wall clock, no runtime RNG.
//
// Telemetry mailbox (the gl_audio_hook concept ported to NDS): 80 u32
// words at the exported symbol `gl_telemetry`, rewritten every frame, so
// headless proofs (tools/nds-headless-check.py --elf/--watch) can assert
// game state numerically. Layout below at GL_T_*. Slots 0-39 keep their
// slice-3/4/5/6 meanings EXACTLY (the pinned CI asserts read them);
// slice 7 appends 40-47 (lantern oil), slice 8 appends 48-55 (audio
// decisions), slice 9 appends 56-63 (the best-nights save record),
// slice 10 appends 64-71 (the watch-map chalk mark + gloam-out),
// slice 11 appends 72-79 (the best-night rematch) — each with every
// older slot frozen. On a multi-zombie night the
// ZX/ZY/DIST/NSTUN slots describe the NEAREST Shambler (identical to
// slice 3 whenever one zombie is up); BX/BY/BHP likewise describe the
// intact barricade nearest the player.
//
// 100% original content: code, text, and the four code-authored sprites.

#include <nds.h>

#include <stdio.h>

#include "gl_sim.h"

// --- telemetry mailbox -------------------------------------------------------
#define GL_T_MAGIC0 0   // 0x474C4F41 'GLOA'
#define GL_T_MAGIC1 1   // 0x4D4C4E45 'MLNE'
#define GL_T_FRAME 2    // global frame counter (every vblank)
#define GL_T_STATE 3    // 0 title / 1 playing / 2 dead / 3 dawn / 4 scavenge
#define GL_T_PX 4       // player x, 8.8 fixed (sprite center)
#define GL_T_PY 5       // player y
#define GL_T_ZX 6       // nearest Shambler x
#define GL_T_ZY 7       // nearest Shambler y
#define GL_T_DAWN 8     // frames left until dawn
#define GL_T_NIGHT 9    // current night (1-based)
#define GL_T_NIGHTS 10  // nights survived
#define GL_T_DEATHS 11  // deaths this power-on
#define GL_T_SEED 12    // seed of the current run
#define GL_T_DIST 13    // Chebyshev player<->nearest Shambler, 8.8 fixed
#define GL_T_RUNFRAME 14// frames into the current night
#define GL_T_NFRAMES 15 // GL_NIGHT_FRAMES (build-flag visibility)
#define GL_T_ZALIVE 16  // Shamblers on the yard (spawned this night)
#define GL_T_ZTOTAL 17  // tonight's full wave size (schedule length)
#define GL_T_SHOVECD 18 // shove cooldown frames left (0 = ready)
#define GL_T_SHOVES 19  // shoves CONNECTED this power-on
#define GL_T_NSTUN 20   // stun frames left on the nearest Shambler
#define GL_T_VLINES 21  // scanlines this frame's work took (from vblank)
#define GL_T_VLMAX 22   // worst GL_T_VLINES since power-on (<71 = the
                        // whole frame fits inside vblank; <263 = 60 fps)
#define GL_T_SPARE 23   // reserved, always 0
#define GL_T_PLANKS 24  // planks in pocket
#define GL_T_BARR 25    // intact barricades on the yard
#define GL_T_BHP 26     // hp of the intact barricade nearest the player
#define GL_T_BREACH 27  // barricades breached (chewed to 0) this power-on
#define GL_T_PLACES 28  // barricades PLACED this power-on
#define GL_T_REPAIRS 29 // barricades REPAIRED this power-on
#define GL_T_BX 30      // nearest intact barricade x, 8.8 fixed (0 = none)
#define GL_T_BY 31      // nearest intact barricade y
#define GL_T_SCAVLEFT 32// scavenge frames left (0 outside the interlude)
#define GL_T_CACHES 33  // plank caches still on the ground
#define GL_T_SCAVENGED 34 // planks scavenged (caches pocketed) this power-on
#define GL_T_SCAVS 35   // interludes entered this power-on
#define GL_T_CX 36      // nearest remaining cache x, 8.8 fixed (0 = none)
#define GL_T_CY 37      // nearest remaining cache y
#define GL_T_CDIST 38   // Chebyshev player<->nearest cache (0 = none)
#define GL_T_SPARE2 39  // reserved, always 0
#define GL_T_OIL 40     // lantern oil left (burn units)
#define GL_T_LRADIUS 41 // lantern light radius, 8.8 fixed (pure f(oil))
#define GL_T_PRESS 42   // 1 = dark press live on the NEAREST Shambler
#define GL_T_FLASKS 43  // oil flasks still on the ground (interlude)
#define GL_T_OILGRAB 44 // flasks pocketed this power-on
#define GL_T_FX 45      // nearest remaining flask x, 8.8 fixed (0 = none)
#define GL_T_FY 46      // nearest remaining flask y
#define GL_T_FDIST 47   // Chebyshev player<->nearest flask (0 = none)
#define GL_T_ATIER 48   // ambience tier the drone plays (see GL_T_ADRONE)
#define GL_T_ACUE 49    // last one-shot cue id fired (0 = none yet)
#define GL_T_ACUES 50   // one-shot cues fired this power-on
#define GL_T_ACUEFR 51  // global frame the last cue fired on
#define GL_T_ADRONE 52  // drone frequency now, Hz (0 = drone silent)
#define GL_T_AFLIPS 53  // drone restarts/stops (tier flips) this power-on
#define GL_T_ASFXL 54   // frames left on the one-shot cue channel
#define GL_T_SPARE3 55  // reserved, always 0
#define GL_T_BEST 56    // best nights survived on record (live table)
#define GL_T_BESTSEED 57// seed of the record run (replay the best night)
#define GL_T_SAVEOK 58  // 1 = power-on read decoded a valid save blob
#define GL_T_SAVEWR 59  // record writes to the backup this power-on
#define GL_T_SAVEVER 60 // save format version (build-flag visibility)
#define GL_T_SPARE4 61  // reserved, always 0
#define GL_T_SPARE5 62  // reserved, always 0
#define GL_T_SPARE6 63  // reserved, always 0
#define GL_T_MARKON 64  // 1 = the chalk mark is on the watch-map
#define GL_T_MARKX 65   // mark yard x, 8.8 fixed (last chalked; raw)
#define GL_T_MARKY 66   // mark yard y
#define GL_T_MARKDIST 67// Chebyshev player<->mark, 8.8 (0 when off)
#define GL_T_MARKS 68   // marks chalked this power-on (X or touch)
#define GL_T_OUT 69     // tonight's dead still out in the gloam
#define GL_T_SPARE7 70  // reserved, always 0
#define GL_T_SPARE8 71  // reserved, always 0
#define GL_T_REMATCH 72 // 1 = this run replays the recorded best seed
#define GL_T_REMATCHES 73 // rematch runs started this power-on
#define GL_T_SPARE9 74  // reserved, always 0
#define GL_T_SPARE10 75 // reserved, always 0
#define GL_T_SPARE11 76 // reserved, always 0
#define GL_T_SPARE12 77 // reserved, always 0
#define GL_T_SPARE13 78 // reserved, always 0
#define GL_T_SPARE14 79 // reserved, always 0

volatile uint32_t gl_telemetry[80];

enum
{
    STATE_TITLE = 0,
    STATE_PLAYING = 1,
    STATE_DEAD = 2,
    STATE_DAWN = 3,
    STATE_SCAVENGE = 4,
};

// --- original sprite art (code-authored, 4bpp, shared 16-color palette) ------
// '.' = transparent; digits = palette index. Player = the lamplighter
// (indices 1-5: coat, skin, lantern brass, lantern glow, hat/boots).
// Shambler = the walking dead (indices 6-9: rot, pale flesh, shadow, eye).
static const char PLAYER_ART[16][17] = {
    "................",
    "....55555.......",
    "...5555555......",
    ".....222........",
    ".....222........",
    "....11111.......",
    "...1111111......",
    "..11111111.44...",
    "..1.11111.4334..",
    "..1.11111.4334..",
    "..2.11111..44...",
    "....11111...3...",
    "....11.11.......",
    "....11.11.......",
    "...555.555......",
    "................",
};

static const char SHAMBLER_ART[16][17] = {
    "................",
    ".....777........",
    "....79777.......",
    "....77777.......",
    ".....866........",
    "....66666.......",
    "..66666666666...",
    ".66.666666..66..",
    ".7..666666...7..",
    "....666666......",
    "....66.666......",
    "....66..66......",
    "...866...66.....",
    "...88....868....",
    "....8.....88....",
    "................",
};

// Barricade = crossed moor-planks on two posts (indices a-c = 10-12:
// plank body, plank top-light, dark outline). Vertically asymmetric on
// purpose: a half-broken barricade renders v-flipped (splintered).
static const char BARRICADE_ART[16][17] = {
    "................",
    "..cc........cc..",
    ".cbbc......cbbc.",
    "cbaabbbbbbbbaabc",
    "cbaaaaaaaaaaaabc",
    ".caaacaaaacaaac.",
    "..cc..c..c..cc..",
    "..cc........cc..",
    "cbbabbbbbbbbabbc",
    "caaaaaaaaaaaaaac",
    ".ccacc.cc.ccacc.",
    "..ca........ac..",
    "..ca........ac..",
    "..ca........ac..",
    "..cc........cc..",
    "................",
};

// Plank cache = three loose moor-planks dropped in a crossed stack
// (reuses the barricade palette indices a-c — same wood, not yet a
// wall). Fourth code-authored sprite.
static const char CACHE_ART[16][17] = {
    "................",
    "................",
    "................",
    "................",
    "...cccccccccc...",
    "...cbbbbbbbbc...",
    "...caaaaaaaac...",
    "..cccccccccc....",
    "..cbbbbbbbbc....",
    "..caaaaaaaac....",
    "....cccccccccc..",
    "....cbbbbbbbbc..",
    "....caaaaaaaac..",
    "................",
    "................",
    "................",
};

// Oil flask = a squat brass lantern-bottle with a glowing fill line
// (reuses the lantern palette indices 3-5: brass, glow, dark cork).
// Fifth code-authored sprite.
static const char FLASK_ART[16][17] = {
    "................",
    "................",
    "......55........",
    "......55........",
    ".....3333.......",
    "......33........",
    ".....3333.......",
    "....333333......",
    "...33333333.....",
    "...33444433.....",
    "...34444443.....",
    "...34444443.....",
    "...33444433.....",
    "....333333......",
    ".....3333.......",
    "................",
};

static void load_sprite_gfx(u16 *gfx, const char art[16][17])
{
    int out = 0;
    for (int ty = 0; ty < 2; ty++)
        for (int tx = 0; tx < 2; tx++)
            for (int y = 0; y < 8; y++)
                for (int half = 0; half < 2; half++)
                {
                    u16 v = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        char c = art[ty * 8 + y][tx * 8 + half * 4 + i];
                        // '.' = transparent, '0'-'9' = 0-9, 'a'+ = 10+
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
    SPRITE_PALETTE[1] = RGB15(6, 6, 9);      // coat: gloam blue-grey
    SPRITE_PALETTE[2] = RGB15(27, 22, 17);   // skin
    SPRITE_PALETTE[3] = RGB15(31, 26, 10);   // lantern brass/flame
    SPRITE_PALETTE[4] = RGB15(31, 30, 21);   // lantern glow
    SPRITE_PALETTE[5] = RGB15(4, 3, 5);      // hat + boots
    SPRITE_PALETTE[6] = RGB15(11, 15, 10);   // shambler rot green
    SPRITE_PALETTE[7] = RGB15(21, 22, 18);   // shambler pale flesh
    SPRITE_PALETTE[8] = RGB15(5, 6, 5);      // shambler shadow
    SPRITE_PALETTE[9] = RGB15(25, 7, 6);     // shambler eye
    SPRITE_PALETTE[10] = RGB15(16, 10, 4);   // barricade plank body
    SPRITE_PALETTE[11] = RGB15(23, 16, 8);   // barricade plank top-light
    SPRITE_PALETTE[12] = RGB15(7, 4, 2);     // barricade dark outline
}

// --- watch-map (bottom screen) geometry ---------------------------------------
// Slice 10: the cell geometry moved VERBATIM into the pure layer
// (gl_sim.h GL_MAP_* + gl_map_col/gl_map_row), so the chalk mark's
// touch placement is provable — the render below is bit-identical.
#define MAP_COL0 GL_MAP_COL0
#define MAP_ROW0 GL_MAP_ROW0
#define MAP_COLS_N GL_MAP_COLS
#define MAP_ROWS_N GL_MAP_ROWS

// --- run state -----------------------------------------------------------------
// GL_STRESS swaps ONLY the wave schedule (full cap, all at frame 0) and
// disarms death — see the header comment. Shipped builds use the pure
// gl_wave_count/gl_spawn_frame schedule untouched.
#ifdef GL_STRESS
#define WAVE_TOTAL(night) ((uint32_t)GL_ZOMBIE_CAP)
#define SPAWN_FRAME(night, index) 0u
#else
#define WAVE_TOTAL(night) gl_wave_count(night)
#define SPAWN_FRAME(night, index) gl_spawn_frame(night, index)
#endif

typedef struct
{
    uint32_t seed;
    uint32_t night;
    uint32_t dawn_left;
    uint32_t run_frame;
    int32_t px, py;
    int32_t zx[GL_ZOMBIE_CAP], zy[GL_ZOMBIE_CAP];
    uint32_t zstun[GL_ZOMBIE_CAP];
    uint32_t z_count;                // spawned so far; they never despawn
    uint32_t wave_total;             // tonight's schedule length
    uint32_t shove_cd;               // frames until the next shove attempt
    // Barricades (slice 5): fixed slots, hp 0 = free/breached. They
    // persist across nights within a run; a fresh run clears them.
    int32_t bx[GL_BARRICADE_CAP], by[GL_BARRICADE_CAP];
    uint32_t bhp[GL_BARRICADE_CAP];
    uint32_t planks;
    // Scavenge interlude (slice 6): plank caches on the ground (up = 1,
    // pocketed = 0) + the dawn-light clock. Both live only inside the
    // interlude — start_night() zeroes them, so this state is inert on
    // the START skip path (every pre-slice-6 proof is bit-identical).
    int32_t cx[GL_CACHE_COUNT], cy[GL_CACHE_COUNT];
    uint32_t cache_up[GL_CACHE_COUNT];
    uint32_t scav_left;
    // Lantern oil (slice 7): the tank burns every NIGHT frame; the
    // interlude's flasks (up = 1, pocketed = 0) are the only refill.
    uint32_t oil;
    int32_t fx[GL_FLASK_COUNT], fy[GL_FLASK_COUNT];
    uint32_t flask_up[GL_FLASK_COUNT];
    // Watch-map chalk mark (slice 10): MAP state only — nothing in the
    // sim reads it (the dead ignore chalk). Persists across nights
    // within a run (start_night leaves it standing); start_run wipes it.
    uint32_t mark_on;
    int32_t mark_x, mark_y;
} Run;

// Spawn every zombie whose scheduled frame has arrived (index order — the
// schedule is non-decreasing in index, so one pass per frame suffices).
static void spawn_due(Run *run)
{
    while (run->z_count < run->wave_total
           && SPAWN_FRAME(run->night, run->z_count) <= run->run_frame)
    {
        uint32_t i = run->z_count;
        gl_spawn_of_night(run->seed, run->night, i, &run->zx[i], &run->zy[i]);
        run->zstun[i] = 0;
        run->z_count++;
    }
}

static void start_night(Run *run)
{
    run->dawn_left = GL_NIGHT_FRAMES;
    run->run_frame = 0;
    run->px = GL_PLAYER_START_X;
    run->py = GL_PLAYER_START_Y;
    run->z_count = 0;
    run->wave_total = WAVE_TOTAL(run->night);
    run->shove_cd = 0;
    // Scavenge state is night-inert: caches and flasks exist only
    // inside the interlude, and the dawn-light clock reads 0 outside.
    for (int i = 0; i < GL_CACHE_COUNT; i++)
        run->cache_up[i] = 0;
    for (int i = 0; i < GL_FLASK_COUNT; i++)
        run->flask_up[i] = 0;
    run->scav_left = 0;
    // Barricades deliberately NOT reset: they persist across nights.
#ifdef GL_STRESS
    // Perf build only: pre-place the full ring so the crowd chews and
    // every block/draw path is on the measured frame.
    {
        static const int8_t ring[GL_BARRICADE_CAP][2] = {
            {24, 0}, {-24, 0}, {0, 24}, {0, -24},
            {24, 24}, {24, -24}, {-24, 24}, {-24, -24},
        };
        for (int i = 0; i < GL_BARRICADE_CAP; i++)
        {
            run->bx[i] = run->px + ring[i][0] * GL_ONE;
            run->by[i] = run->py + ring[i][1] * GL_ONE;
            run->bhp[i] = GL_BARRICADE_HP;
        }
    }
#endif
    spawn_due(run);                  // index 0 spawns at frame 0, as slice 3
}

static void start_run(Run *run, uint32_t seed)
{
    run->seed = seed;
    run->night = 1;
    run->planks = GL_PLANK_STOCK;
    run->oil = GL_OIL_MAX;           // a fresh lantern
#ifdef GL_STRESS
    // Perf build only: start with the light already failing so the
    // dark-press step path, the guttering radius math, the backdrop
    // dimming and the oil-gauge redraws are all ON the measured
    // frames — the honest worst case for the vcount probe.
    run->oil = GL_OIL_LOW / 2;
#endif
    for (int i = 0; i < GL_BARRICADE_CAP; i++)
        run->bhp[i] = 0;             // fresh run: bare yard
    run->mark_on = 0;                // the chalk wipes on a fresh run
    run->mark_x = 0;
    run->mark_y = 0;
    start_night(run);
}

// SELECT at the dawn card: the scavenge interlude of the night just
// survived (slice 6). The lamplighter returns to the lamppost (so the
// spawn safe radius that guarded the night now guards the interlude's
// first frames), the dawn light drives every leftover Shambler back to
// its own night spawn point (pure — the host proof already shows those
// are outside the safe radius), and the night's plank caches appear on
// their own pure schedule. Barricades stand untouched.
static void begin_scavenge(Run *run)
{
    run->px = GL_PLAYER_START_X;
    run->py = GL_PLAYER_START_Y;
    for (uint32_t i = 0; i < run->z_count; i++)
    {
        gl_spawn_of_night(run->seed, run->night, i,
                          &run->zx[i], &run->zy[i]);
        run->zstun[i] = 0;
    }
    for (int i = 0; i < GL_CACHE_COUNT; i++)
    {
        gl_cache_of_interlude(run->seed, run->night, (uint32_t)i,
                              &run->cx[i], &run->cy[i]);
        run->cache_up[i] = 1;
    }
    for (int i = 0; i < GL_FLASK_COUNT; i++)
    {
        gl_flask_of_interlude(run->seed, run->night, (uint32_t)i,
                              &run->fx[i], &run->fy[i]);
        run->flask_up[i] = 1;
    }
    run->scav_left = GL_SCAVENGE_FRAMES;
    run->shove_cd = 0;
}

// --- the shared verbs + the dead's step (PLAYING and SCAVENGE) -----------------
// Extracted verbatim from the slice-5 PLAYING case so the interlude
// runs the exact same rules; the emulator re-run of every pre-slice-6
// pinned proof is the no-drift evidence for this refactor.

// The shove: any A press with a cold cooldown is an ATTEMPT (arming
// the cooldown, hit or whiff); it CONNECTS only if the nearest
// Shambler is within GL_SHOVE_RANGE — knockback via the pure gl_shove,
// plus GL_SHOVE_STUN frames frozen.
static void do_shove_verb(Run *run, uint32_t down, uint32_t *shoves)
{
    if (run->shove_cd > 0)
        run->shove_cd--;
    if ((down & KEY_A) && run->shove_cd == 0)
    {
        run->shove_cd = GL_SHOVE_COOLDOWN;
        uint32_t nearest = 0;
        int32_t best = INT32_MAX;
        for (uint32_t i = 0; i < run->z_count; i++)
        {
            int32_t d = gl_chebyshev(run->px, run->py,
                                     run->zx[i], run->zy[i]);
            if (d < best) { best = d; nearest = i; }
        }
        if (run->z_count > 0
            && gl_shove(run->px, run->py,
                        &run->zx[nearest], &run->zy[nearest]))
        {
            run->zstun[nearest] = GL_SHOVE_STUN;
            (*shoves)++;
        }
    }
}

// The barricade verb (B): repair the intact barricade in reach (if it
// needs it), else place a new one at the lamplighter's feet — 1 plank
// either way. No cooldown: the plank pocket is the limiter.
static void do_barricade_verb(Run *run, uint32_t down,
                              uint32_t *places, uint32_t *repairs)
{
    if ((down & KEY_B) && run->planks > 0)
    {
        int32_t best_b = INT32_MAX;
        int near_b = -1;
        for (int i = 0; i < GL_BARRICADE_CAP; i++)
        {
            if (run->bhp[i] == 0)
                continue;
            int32_t d = gl_chebyshev(run->px, run->py,
                                     run->bx[i], run->by[i]);
            if (d < best_b) { best_b = d; near_b = i; }
        }
        if (near_b >= 0 && best_b <= GL_BARRICADE_RANGE)
        {
            if (run->bhp[near_b] < GL_BARRICADE_HP)
            {                            // repair to full, 1 plank
                run->bhp[near_b] = GL_BARRICADE_HP;
                run->planks--;
                (*repairs)++;
            }                            // full one in reach: no waste
        }
        else
        {
            for (int i = 0; i < GL_BARRICADE_CAP; i++)
                if (run->bhp[i] == 0)
                {                        // place in the first free slot
                    run->bx[i] = run->px;
                    run->by[i] = run->py;
                    run->bhp[i] = GL_BARRICADE_HP;
                    run->planks--;
                    (*places)++;
                    break;
                }
        }
    }
}

// The chalk-mark verb (slice 10): the stylus alias places or moves the
// mark to the tapped map cell (the pure gl_mark_of_touch — a tap off
// the plot drops nothing); X clears a standing mark, else chalks one
// at the lamplighter's own position. Buttons-first (BINDING per the
// concept doc) — touch is never required. Chalk is MAP state only:
// nothing in the sim reads it, so this verb cannot move a pin.
static void do_mark_verb(Run *run, int x_pressed, int touch_down,
                         int tx, int ty, uint32_t *marks)
{
    if (touch_down)
    {
        int32_t mx, my;
        if (gl_mark_of_touch(tx, ty, &mx, &my))
        {
            run->mark_x = mx;
            run->mark_y = my;
            run->mark_on = 1;
            (*marks)++;
        }
    }
    else if (x_pressed)
    {
        if (run->mark_on)
        {
            run->mark_on = 0;        // X wipes the standing mark
        }
        else
        {
            run->mark_x = run->px;   // X chalks at the lamplighter's feet
            run->mark_y = run->py;
            run->mark_on = 1;
            (*marks)++;
        }
    }
}

// The dead's step (plus the frame it costs): propose the pure chase
// step; an intact barricade blocks any step that would ENTER its
// radius — the blocked attempt chews it for exactly 1 hp instead
// (hp 0 = breached, slot free). First blocking slot wins:
// deterministic. A stagger frame proposes no move, so it neither
// enters nor chews — UNLESS the dark press cancels it (slice 7): with
// the light failing, a body beyond the lamplight strides anyway (and
// a pressed stride that is blocked chews, like any real step).
// oil_for_press is the oil level the press sees: the real tank at
// night (PLAYING), GL_OIL_MAX in the daylight interlude — the dawn
// light presses nobody, and at healthy oil gl_dark_press is
// identically 0, so the pre-slice-7 step is bit-identical.
static void step_the_dead(Run *run, uint32_t *breaches,
                          uint32_t oil_for_press)
{
    for (uint32_t i = 0; i < run->z_count; i++)
    {
        if (run->zstun[i] > 0)
        {
            run->zstun[i]--;             // stunned: no step this frame
            continue;
        }
        int32_t nx = run->zx[i], ny = run->zy[i];
        if (!gl_shambler_staggers(i /* zombie_id */, run->run_frame)
            || gl_dark_press(oil_for_press,
                             gl_chebyshev(run->px, run->py,
                                          run->zx[i], run->zy[i])))
            gl_shambler_stride(&nx, &ny, run->px, run->py);
        int blocker = -1;
        for (int j = 0; j < GL_BARRICADE_CAP; j++)
            if (run->bhp[j] > 0
                && gl_barricade_blocks(run->bx[j], run->by[j],
                                       run->zx[i], run->zy[i], nx, ny))
            {
                blocker = j;
                break;
            }
        if (blocker >= 0)
        {
            if (--run->bhp[blocker] == 0)
                (*breaches)++;
        }
        else
        {
            run->zx[i] = nx;
            run->zy[i] = ny;
        }
    }
    run->run_frame++;
}

// 1 if the cold hands reach the lamplighter this frame.
static int the_cold_hands(const Run *run)
{
    int touched = 0;
    for (uint32_t i = 0; i < run->z_count && !touched; i++)
        touched = gl_contact(run->px, run->py, run->zx[i], run->zy[i]);
    return touched;
}

// --- slice-9 backup I/O (bounded card-SPI EEPROM read + page program) ----------
// The record's ONLY two backup touches: one read at power-on, one page
// program on an improving dawn. Both are the standard SPI-EEPROM
// command flows (READ 0x03 / WREN 0x06 / PAGE PROGRAM 0x02 / RDSR 0x05
// with GL_SAVE_EEPROM_TYPE-2 = 2-byte addressing), hand-rolled instead
// of libnds's cardReadEeprom/cardWriteEeprom for two measured reasons:
//
// 1. CHIP-SELECT DISCIPLINE. The AUXSPI protocol (GBATEK; DeSmuME
//    models it faithfully) releases the chip select AFTER the next
//    byte once CARD_SPI_HOLD (bit 6) is dropped — so a command must
//    clear HOLD BEFORE its last byte. libnds's cardRead/WriteEeprom
//    instead "deselects" by writing 0x0040 (bit 6 stays set) after the
//    last byte: real chips tolerate it, but DeSmuME's backup device
//    (the proof carrier) never sees a select edge, keeps the previous
//    command open, and swallows every later command byte as more read
//    traffic — measured in this slice as (a) the record write landing
//    NOWHERE (the battery bytes stayed 0xFF while the ROM thought it
//    had written) and (b) libnds's unbounded write-in-progress poll
//    then reading save bytes (0xFF, WIP bit set) forever: the ROM
//    froze solid on the first dawn write.
// 2. BOUNDED WAITS. Every wait below gives up after
//    GL_SAVE_POLL_BOUND iterations — a chip (or emulator) that never
//    answers costs a bounded sub-frame stall on one boot/dawn-card
//    frame, never a hang. An expired program poll is harmless: the
//    page data is already clocked in and the chip finishes programming
//    internally; the next backup command is minutes away (the next
//    improving dawn) or a power cycle.

static void save_spi_byte(uint8_t b)
{
    REG_AUXSPIDATA = b;
    for (uint32_t i = 0;
         i < GL_SAVE_POLL_BOUND && (REG_AUXSPICNT & CARD_SPI_BUSY); i++) { }
}

static uint8_t save_spi_read(void)
{
    save_spi_byte(0);
    return REG_AUXSPIDATA;
}

static void save_select(void)                // begin a command window
{
    REG_AUXSPICNT = CARD_ENABLE | CARD_SPI_ENABLE | CARD_SPI_HOLD;
}

static void save_final_byte(void)            // CS releases after the NEXT
{                                            // byte (drop HOLD first —
    REG_AUXSPICNT = CARD_ENABLE | CARD_SPI_ENABLE;   // the protocol rule)
}

static void save_read_backup(uint8_t blob[GL_SAVE_BYTES])
{
    save_select();
    save_spi_byte(0x03);                     // READ + 2 address bytes
    save_spi_byte((GL_SAVE_ADDR >> 8) & 0xFF);
    save_spi_byte(GL_SAVE_ADDR & 0xFF);
    for (int i = 0; i < GL_SAVE_BYTES - 1; i++)
        blob[i] = save_spi_read();
    save_final_byte();
    blob[GL_SAVE_BYTES - 1] = save_spi_read();
}

static void save_write_backup(const uint8_t blob[GL_SAVE_BYTES])
{
    // WRITE ENABLE — its own one-byte chip-select window
    save_select();
    save_final_byte();
    save_spi_byte(0x06);
    // PAGE PROGRAM: GL_SAVE_BYTES is exactly one 32-byte page at
    // GL_SAVE_ADDR 0, so no page-crossing loop is needed.
    save_select();
    save_spi_byte(0x02);
    save_spi_byte((GL_SAVE_ADDR >> 8) & 0xFF);
    save_spi_byte(GL_SAVE_ADDR & 0xFF);
    for (int i = 0; i < GL_SAVE_BYTES - 1; i++)
        save_spi_byte(blob[i]);
    save_final_byte();
    save_spi_byte(blob[GL_SAVE_BYTES - 1]);  // CS falls: the chip programs
    // READ STATUS: wait out the program cycle — BOUNDED
    save_select();
    save_spi_byte(0x05);
    for (uint32_t i = 0;
         i < GL_SAVE_POLL_BOUND && (save_spi_read() & 0x01); i++) { }
    save_final_byte();
    save_spi_read();                         // terminating dummy byte
}

// --- top-screen scenes -----------------------------------------------------------
static PrintConsole top_console;
static PrintConsole bottom_console;

static void draw_fence(void)
{
    consoleSelect(&top_console);
    printf("\x1b[2;0H################################");
    for (int row = 3; row <= 22; row++)
        printf("\x1b[%d;0H#\x1b[%d;31H#", row, row);
    printf("\x1b[23;0H################################");
}

static void draw_title(uint32_t best_nights, uint32_t best_seed)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[6;10HGLOAMLINE\n");
    printf("\x1b[8;5Hthe last lamplighter\n");
    printf("\x1b[9;5Hholds the fence line\n");
    printf("\x1b[12;6Hmove: +pad (8-way)\n");
    printf("\x1b[13;6Hshove: A (cooldown)\n");
    printf("\x1b[14;6Hbarricade: B (planks)\n");
    printf("\x1b[15;6Hscavenge at dawn: SELECT\n");
    printf("\x1b[16;6Hdon't get touched\n");
    printf("\x1b[17;6Hsurvive to dawn\n");
    printf("\x1b[18;6Hkeep the lantern fed\n");
    printf("\x1b[19;9HPRESS START\n");
    // Slice 9: the record survived the power cycle — show it. A fresh
    // (or reset) table shows nothing: the moor keeps no empty boasts.
    // Slice 11: with a record standing, SELECT walks its very night
    // again (the same no-empty-boasts gate hides the offer too).
    if (best_nights > 0)
    {
        printf("\x1b[20;3HPRESS SELECT: rematch\n");
        printf("\x1b[21;3Hbest %lu night(s)  seed %lu",
               (unsigned long)best_nights, (unsigned long)best_seed);
    }
}

static void draw_death_card(const Run *run, uint32_t deaths,
                            uint32_t best_nights, uint32_t rematch_on)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[7;7HTHE COLD HANDS\n");
    printf("\x1b[8;9HFOUND YOU\n");
    printf("\x1b[11;7Hnight %lu", (unsigned long)run->night);
    printf("\x1b[12;7Hseed %lu%s", (unsigned long)run->seed,
           rematch_on ? "*" : "");        // slice 11: the rematch's star
    printf("\x1b[13;7Hdeaths %lu", (unsigned long)deaths);
    if (best_nights > 0)                 // slice 9: what you're chasing
        printf("\x1b[14;7Hbest %lu night(s)", (unsigned long)best_nights);
    printf("\x1b[16;5HPRESS START: retry\n");
    if (best_nights > 0)                 // slice 11: chase it again now
        printf("\x1b[17;5HPRESS SELECT: rematch\n");
}

static void draw_dawn_card(const Run *run, uint32_t nights,
                           uint32_t best_nights, uint32_t best_seed,
                           uint32_t rematch_on)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[7;9HDAWN BREAKS\n");
    printf("\x1b[10;5Hnight %lu survived", (unsigned long)run->night);
    printf("\x1b[11;5Hnights total %lu", (unsigned long)nights);
    printf("\x1b[12;5Hseed %lu%s", (unsigned long)run->seed,
           rematch_on ? "*" : "");        // slice 11: the rematch's star
    // Slice 9: the record (already updated for THIS dawn if it
    // improved — the caller writes the backup first, so what the card
    // shows is what the cartridge now holds).
    if (best_nights > 0)
        printf("\x1b[13;5Hbest %lu night(s) seed %lu",
               (unsigned long)best_nights, (unsigned long)best_seed);
    printf("\x1b[15;3HPRESS START: night %lu",
           (unsigned long)(run->night + 1));
    printf("\x1b[17;3HPRESS SELECT: scavenge");
}

static void draw_scavenge_hud(const Run *run, uint32_t nights,
                              uint32_t rematch_on)
{
    unsigned int secs = run->scav_left / 60;
    unsigned long caches = 0;
    for (int i = 0; i < GL_CACHE_COUNT; i++)
        caches += run->cache_up[i];
    consoleSelect(&top_console);
    printf("\x1b[0;1HSCAVENGE %u:%02u  CACHE %lu\x1b[K",
           secs / 60, secs % 60, caches);
    printf("\x1b[1;1HSEED %lu%s NTS %lu SHV %c PK %lu\x1b[K",
           (unsigned long)run->seed, rematch_on ? "*" : "",
           (unsigned long)nights,
           run->shove_cd == 0 ? '+' : '.', (unsigned long)run->planks);
}

static void draw_hud(const Run *run, uint32_t nights, uint32_t rematch_on)
{
    unsigned int secs = run->dawn_left / 60;
    consoleSelect(&top_console);
    // O<pct> = the lantern's oil tank, 0-100 (the full gauge lives on
    // the watch-map).
    printf("\x1b[0;1HNIGHT %lu  DAWN %u:%02u  Z %lu O%lu\x1b[K",
           (unsigned long)run->night, secs / 60, secs % 60,
           (unsigned long)run->z_count,
           (unsigned long)(run->oil * 100u / GL_OIL_MAX));
    // Slice 11: a rematch run wears a '*' after its seed — one console
    // character inside a line this HUD already reprints every frame
    // (render-only; a fresh-latch run's line is byte-identical).
    printf("\x1b[1;1HSEED %lu%s NTS %lu SHV %c PK %lu\x1b[K",
           (unsigned long)run->seed, rematch_on ? "*" : "",
           (unsigned long)nights,
           run->shove_cd == 0 ? '+' : '.', (unsigned long)run->planks);
}

// --- bottom-screen watch-map -------------------------------------------------------
// Erase-then-redraw marks: the player + EVERY zombie + every intact
// barricade + every remaining plank cache + the chalk mark (slice 10),
// so the map is the yard radar the concept promises. prev arrays
// remember last frame's cells.
static int map_prev_row[2 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP
                        + GL_CACHE_COUNT + GL_FLASK_COUNT];
static int map_prev_col[2 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP
                        + GL_CACHE_COUNT + GL_FLASK_COUNT];
static int map_prev_n = 0;
// Oil gauge (slice 7): cells drawn last time, so the bar is redrawn
// ONLY when a cell flips (frame-budget rail; -1 = force a redraw).
static int map_prev_oil_cells = -1;
// Dawn bar (slice 8): same flip-only treatment — reprinting ~30
// console cells every frame was the last full-line reprint on the
// per-frame path, and the audio glue tipped the measured worst stress
// frame from 70 to 71 scanlines; a dawn cell flips every 180 frames,
// so this buys the line back (render-only; no pin reads the bar).
static int map_prev_dawn_cells = -1;
static uint32_t map_prev_dawn_night = 0;
// Watch line (slice 10): OUT count + the chalk mark's range, same
// flip-only treatment as the gauges (values change rarely; the range
// in map cells moves about once per 5 frames at player speed).
static int map_prev_watch_out = -1;
static int map_prev_watch_mark = -2;     // -2 = force, -1 = mark off

static void draw_watch_map_frame(uint32_t best_nights)
{
    consoleSelect(&bottom_console);
    consoleClear();
    printf("\x1b[0;1HWATCH-MAP");
    // Slice 10: the record rides the map header during play — what
    // you're chasing, visible where you're looking. A fresh table
    // shows nothing (the moor keeps no empty boasts, as the title).
    if (best_nights > 0)
        printf("  BEST %lu", (unsigned long)best_nights);
    printf("\x1b[2;1HP you Z dead # wall * wd o oil");
    printf("\x1b[4;1H+----------------------------+");
    for (int row = MAP_ROW0; row < MAP_ROW0 + MAP_ROWS_N; row++)
        printf("\x1b[%d;1H|\x1b[%d;30H|", row, row);
    printf("\x1b[19;1H+----------------------------+");
    map_prev_n = 0;
    map_prev_oil_cells = -1;         // consoleClear wiped the gauge
    map_prev_dawn_cells = -1;        // ... and the dawn bar
    map_prev_dawn_night = 0;
    map_prev_watch_out = -1;         // ... and the watch line
    map_prev_watch_mark = -2;
}

static void draw_watch_map(const Run *run, int state)
{
    consoleSelect(&bottom_console);

    // dawn bar: 20 cells fill as the night burns down. Redrawn ONLY
    // when a cell (or the night number) flips — about every 180
    // frames (slice 8: the last full-line per-frame reprint, retired
    // to buy the audio glue's scanline back).
    int filled = (int)((GL_NIGHT_FRAMES - run->dawn_left) * 20
                       / GL_NIGHT_FRAMES);
    if (filled != map_prev_dawn_cells || run->night != map_prev_dawn_night)
    {
        printf("\x1b[21;1HDAWN [");
        for (int i = 0; i < 20; i++)
            putchar(i < filled ? '=' : '-');
        printf("] N%lu", (unsigned long)run->night);
        map_prev_dawn_cells = filled;
        map_prev_dawn_night = run->night;
    }

    // oil gauge (slice 7, the concept doc's bottom-screen gauge):
    // 20 cells drain as the lantern burns. Redrawn ONLY when a cell
    // flips (about every 540 frames) — the frame-budget rail.
    int oil_cells = (int)(run->oil * 20 / GL_OIL_MAX);
    if (oil_cells != map_prev_oil_cells)
    {
        printf("\x1b[22;1HOIL  [");
        for (int i = 0; i < 20; i++)
            putchar(i < oil_cells ? '#' : '.');
        putchar(']');
        map_prev_oil_cells = oil_cells;
    }

    if (state != STATE_PLAYING && state != STATE_SCAVENGE)
        return;

    // watch line (slice 10): OUT = tonight's dead still out in the
    // gloam (pure gl_gloam_out), plus the chalk mark's range in map
    // cells while it is down. Redrawn ONLY when a value flips.
    int watch_out = (int)gl_gloam_out(run->wave_total, run->z_count);
    int watch_mark = run->mark_on
        ? (int)(gl_chebyshev(run->px, run->py, run->mark_x, run->mark_y)
                / (GL_MAP_CELL_PX * GL_ONE))
        : -1;
    if (watch_out != map_prev_watch_out
        || map_prev_watch_mark == -2
        || (watch_mark < 0) != (map_prev_watch_mark < 0))
    {
        // full line: OUT flipped, or the mark went on/off
        if (watch_mark >= 0)
            printf("\x1b[3;1HOUT %2d   ! MARK %2d\x1b[K",
                   watch_out, watch_mark);
        else
            printf("\x1b[3;1HOUT %2d\x1b[K", watch_out);
    }
    else if (watch_mark != map_prev_watch_mark)
    {
        // only the range moved (about every 5 frames at player
        // speed): rewrite just its two digits — measured at full
        // stress, the full-line reprint here cost the 71-line vblank
        // budget its last scanline (72), the digits alone do not.
        printf("\x1b[3;17H%2d", watch_mark);
    }
    map_prev_watch_out = watch_out;
    map_prev_watch_mark = watch_mark;

    for (int i = 0; i < map_prev_n; i++)
        printf("\x1b[%d;%dH ", map_prev_row[i], map_prev_col[i]);
    map_prev_n = 0;

    // the chalk mark draws FIRST (chalk sits under everything — any
    // body or thing sharing the cell wins it)
    if (run->mark_on)
    {
        int m_row = gl_map_row(run->mark_y), m_col = gl_map_col(run->mark_x);
        printf("\x1b[%d;%dH!", m_row, m_col);
        map_prev_row[map_prev_n] = m_row;
        map_prev_col[map_prev_n++] = m_col;
    }
    for (int i = 0; i < GL_CACHE_COUNT; i++)
    {
        if (!run->cache_up[i])
            continue;
        int c_row = gl_map_row(run->cy[i]), c_col = gl_map_col(run->cx[i]);
        printf("\x1b[%d;%dH*", c_row, c_col);
        map_prev_row[map_prev_n] = c_row;
        map_prev_col[map_prev_n++] = c_col;
    }
    for (int i = 0; i < GL_FLASK_COUNT; i++)
    {
        if (!run->flask_up[i])
            continue;
        int f_row = gl_map_row(run->fy[i]), f_col = gl_map_col(run->fx[i]);
        printf("\x1b[%d;%dHo", f_row, f_col);
        map_prev_row[map_prev_n] = f_row;
        map_prev_col[map_prev_n++] = f_col;
    }
    for (int i = 0; i < GL_BARRICADE_CAP; i++)
    {
        if (run->bhp[i] == 0)
            continue;
        int b_row = gl_map_row(run->by[i]), b_col = gl_map_col(run->bx[i]);
        printf("\x1b[%d;%dH#", b_row, b_col);
        map_prev_row[map_prev_n] = b_row;
        map_prev_col[map_prev_n++] = b_col;
    }
    for (uint32_t i = 0; i < run->z_count; i++)
    {
        int z_row = gl_map_row(run->zy[i]), z_col = gl_map_col(run->zx[i]);
        printf("\x1b[%d;%dHZ", z_row, z_col);
        map_prev_row[map_prev_n] = z_row;
        map_prev_col[map_prev_n++] = z_col;
    }
    int p_row = gl_map_row(run->py), p_col = gl_map_col(run->px);
    printf("\x1b[%d;%dHP", p_row, p_col);   // player wins a shared cell
    map_prev_row[map_prev_n] = p_row;
    map_prev_col[map_prev_n++] = p_col;
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
    u16 *shambler_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16,
                                       SpriteColorFormat_16Color);
    u16 *barricade_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16,
                                        SpriteColorFormat_16Color);
    u16 *cache_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16,
                                    SpriteColorFormat_16Color);
    u16 *flask_gfx = oamAllocateGfx(&oamMain, SpriteSize_16x16,
                                    SpriteColorFormat_16Color);
    load_sprite_gfx(player_gfx, PLAYER_ART);
    load_sprite_gfx(shambler_gfx, SHAMBLER_ART);
    load_sprite_gfx(barricade_gfx, BARRICADE_ART);
    load_sprite_gfx(cache_gfx, CACHE_ART);
    load_sprite_gfx(flask_gfx, FLASK_ART);

    // Slice 8: power the sound block. Every voice below is a hardware
    // PSG/noise channel serviced by the default ARM7 core — synthesized
    // square waves and noise, no sample data anywhere.
    soundEnable();

    // Slice 9: the best-nights record — ONE bounded read of the
    // cartridge backup at power-on (card SPI EEPROM; DeSmuME emulates
    // it as the battery .dsv). A missing, blank, corrupt or
    // future-version blob decodes to 0 and the fresh table stands —
    // never a crash, never a hang. GL_SAVE_EEPROM_TYPE is the assumed
    // decide-and-flag addressing constant, not a boot-time chip probe.
    uint32_t best_nights = 0;
    uint32_t best_seed = 0;
    uint32_t save_ok = 0;
    uint32_t save_writes = 0;
    {
        uint8_t blob[GL_SAVE_BYTES];
        save_read_backup(blob);
        save_ok = (uint32_t)gl_save_decode(blob, &best_nights, &best_seed);
    }

    Run run = {0};
    run.dawn_left = GL_NIGHT_FRAMES;         // title-screen dawn bar: empty
    int state = STATE_TITLE;
    uint32_t frame = 0;
    uint32_t deaths = 0;
    uint32_t nights_survived = 0;
    uint32_t shoves = 0;           // shoves CONNECTED this power-on
    uint32_t places = 0;           // barricades placed this power-on
    uint32_t repairs = 0;          // barricades repaired this power-on
    uint32_t breaches = 0;         // barricades chewed to 0 this power-on
    uint32_t scavenged = 0;        // planks scavenged this power-on
    uint32_t scavs = 0;            // interludes entered this power-on
    uint32_t oil_grabs = 0;        // oil flasks pocketed this power-on
    uint32_t marks = 0;            // chalk marks placed this power-on
    uint32_t rematch_on = 0;       // this run replays the recorded seed
    uint32_t rematches = 0;        // rematch runs started this power-on
    uint32_t vlines_max = 0;       // worst frame cost seen, in scanlines
    bool pad_seen_idle = false;    // KEYINPUT boot-trap guard (session 16)
    // Slice-8 audio state (ARM9 bookkeeping only — the sound itself
    // runs on the ARM7/hardware channels). The drone free-runs on its
    // channel between tier flips; the cue channel is opened for
    // gl_cue_len frames then killed.
    int amb_ch = -1;               // drone channel handle (-1 = silent)
    uint32_t amb_tier = 0;         // tier the drone is playing
    uint32_t amb_flips = 0;        // drone restarts/stops this power-on
    int cue_ch = -1;               // one-shot cue channel handle
    uint32_t cue_left = 0;         // frames left on the cue channel
    uint32_t last_cue = 0;         // last cue id fired
    uint32_t cues = 0;             // cues fired this power-on
    uint32_t cue_frame = 0;        // global frame of the last cue

    draw_title(best_nights, best_seed);
    draw_watch_map_frame(best_nights);

    gl_telemetry[GL_T_MAGIC0] = 0x474C4F41u;   // 'GLOA'
    gl_telemetry[GL_T_MAGIC1] = 0x4D4C4E45u;   // 'MLNE'
    gl_telemetry[GL_T_NFRAMES] = GL_NIGHT_FRAMES;
    gl_telemetry[GL_T_SPARE] = 0;

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        frame++;

        uint32_t held = keysHeld();
        uint32_t down = keysDown();
        // Only trust START once the pad has been seen idle: an emulator
        // whose KEYINPUT reads all-pressed (active-low zero) before its
        // first input latch must not skip the title at boot.
        if (held == 0)
            pad_seen_idle = true;
        bool start = pad_seen_idle && (down & KEY_START);

        // Slice 10: the stylus (optional — the game never needs it).
        // One read on the tap edge; the chalk-mark verb consumes it in
        // PLAYING/SCAVENGE only.
        touchPosition touch = {0};
        int touch_down = 0;
        if (down & KEY_TOUCH)
        {
            touchRead(&touch);
            touch_down = 1;
        }

        // Slice-8 audio event capture: remember where the frame began,
        // so counter deltas + state flips below say which cues fire.
        int prev_state = state;
        uint32_t prev_shoves = shoves;
        uint32_t prev_places = places;
        uint32_t prev_repairs = repairs;
        uint32_t prev_breaches = breaches;
        uint32_t prev_scavenged = scavenged;
        uint32_t prev_oil_grabs = oil_grabs;

        switch (state)
        {
        case STATE_TITLE:
            if (start)
            {
                rematch_on = 0;             // the fresh-latch path, as ever
                start_run(&run, frame);     // latch the frame-counter seed
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame(best_nights);
            }
            else if (pad_seen_idle && (down & KEY_SELECT)
                     && gl_rematch_available(best_nights))
            {                               // slice 11: the rematch — the
                rematch_on = 1;             // recorded seed, on a button
                rematches++;
                start_run(&run, gl_run_seed(1, best_nights, best_seed,
                                            frame));
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame(best_nights);
            }
            break;

        case STATE_PLAYING:
        {
            run.oil = gl_oil_burn(run.oil);  // the lantern burns
            spawn_due(&run);                 // tonight's wave trickles in
            gl_player_step(&run.px, &run.py,
                           held & KEY_UP, held & KEY_DOWN,
                           held & KEY_LEFT, held & KEY_RIGHT);
            do_shove_verb(&run, down, &shoves);
            do_barricade_verb(&run, down, &places, &repairs);
            do_mark_verb(&run, pad_seen_idle && (down & KEY_X),
                         touch_down, touch.px, touch.py, &marks);
            step_the_dead(&run, &breaches, run.oil);

            int touched = the_cold_hands(&run);
#ifdef GL_STRESS
            touched = 0;                     // perf build: measure, don't die
#endif
            if (touched)
            {
                deaths++;
                state = STATE_DEAD;
                oamClear(&oamMain, 0, 1 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP
                                      + GL_CACHE_COUNT + GL_FLASK_COUNT);
                draw_death_card(&run, deaths, best_nights, rematch_on);
            }
            else if (--run.dawn_left == 0)
            {
                nights_survived = run.night;
                // Slice 9: the record moves ONLY here — a dawn that
                // strictly beats it. One page write to the backup, on
                // a state-flip frame that already rebuilds the console
                // (off the per-frame path); equal-or-worse dawns and
                // deaths write NOTHING (EEPROM wear discipline).
                if (gl_record_improves(best_nights, nights_survived))
                {
                    best_nights = nights_survived;
                    best_seed = run.seed;
                    uint8_t blob[GL_SAVE_BYTES];
                    gl_save_encode(best_nights, best_seed, blob);
                    save_write_backup(blob);
                    save_writes++;
                }
                state = STATE_DAWN;
                oamClear(&oamMain, 0, 1 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP
                                      + GL_CACHE_COUNT + GL_FLASK_COUNT);
                draw_dawn_card(&run, nights_survived, best_nights,
                               best_seed, rematch_on);
            }
            else
            {
                draw_hud(&run, nights_survived, rematch_on);
            }
            break;
        }

        case STATE_SCAVENGE:
        {
            if (start)                       // to the fence — night comes
            {
                run.night++;                 // loot is the grant: no +2
                start_night(&run);
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame(best_nights);
                break;
            }
            gl_player_step(&run.px, &run.py,
                           held & KEY_UP, held & KEY_DOWN,
                           held & KEY_LEFT, held & KEY_RIGHT);
            do_shove_verb(&run, down, &shoves);
            do_barricade_verb(&run, down, &places, &repairs);
            do_mark_verb(&run, pad_seen_idle && (down & KEY_X),
                         touch_down, touch.px, touch.py, &marks);

            // Pocket a cache the frame you reach it (after the player
            // moves, before the dead do) — but NEVER waste one: a full
            // pocket leaves the cache on the ground.
            for (int i = 0; i < GL_CACHE_COUNT; i++)
                if (run.cache_up[i] && run.planks < GL_PLANK_MAX
                    && gl_cache_grab(run.px, run.py, run.cx[i], run.cy[i]))
                {
                    run.cache_up[i] = 0;
                    run.planks = gl_planks_after_grab(run.planks);
                    scavenged++;
                }

            // Same rule for the oil flasks: never on a full tank —
            // no flask is wasted, it stays on the ground.
            for (int i = 0; i < GL_FLASK_COUNT; i++)
                if (run.flask_up[i] && run.oil < GL_OIL_MAX
                    && gl_cache_grab(run.px, run.py, run.fx[i], run.fy[i]))
                {
                    run.flask_up[i] = 0;
                    run.oil = gl_oil_after_flask(run.oil);
                    oil_grabs++;
                }

            // daylight is not safety — but it presses nobody
            step_the_dead(&run, &breaches, GL_OIL_MAX);

            if (the_cold_hands(&run))
            {
                deaths++;
                state = STATE_DEAD;
                oamClear(&oamMain, 0, 1 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP
                                      + GL_CACHE_COUNT + GL_FLASK_COUNT);
                draw_death_card(&run, deaths, best_nights, rematch_on);
            }
            else if (--run.scav_left == 0)   // dawn light spent
            {
                run.night++;                 // loot is the grant: no +2
                start_night(&run);
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame(best_nights);
            }
            else
            {
                draw_scavenge_hud(&run, nights_survived, rematch_on);
            }
            break;
        }

        case STATE_DEAD:
            if (start)                       // instant restart, fresh seed
            {
                rematch_on = 0;              // the star comes off
                start_run(&run, frame);
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame(best_nights);
            }
            else if (pad_seen_idle && (down & KEY_SELECT)
                     && gl_rematch_available(best_nights))
            {                                // slice 11: chase it again
                rematch_on = 1;              // right now — the death card
                rematches++;                 // is the rematch's seat
                start_run(&run, gl_run_seed(1, best_nights, best_seed,
                                            frame));
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame(best_nights);
            }
            break;

        case STATE_DAWN:
            if (start)                       // same run, next night — the
            {                                // old skip path, bit-identical
                run.night++;
                run.planks = gl_planks_at_dawn(run.planks);
                start_night(&run);
                state = STATE_PLAYING;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame(best_nights);
            }
            else if (pad_seen_idle && (down & KEY_SELECT))
            {                                // the scavenge interlude
                scavs++;
                begin_scavenge(&run);
                state = STATE_SCAVENGE;
                consoleSelect(&top_console);
                consoleClear();
                draw_fence();
                draw_watch_map_frame(best_nights);
            }
            break;
        }

        if (state == STATE_PLAYING || state == STATE_SCAVENGE)
        {
            oamSet(&oamMain, 0, run.px / GL_ONE - 8, run.py / GL_ONE - 8,
                   0, 0, SpriteSize_16x16, SpriteColorFormat_16Color,
                   player_gfx, -1, false, false, false, false, false);
            for (uint32_t i = 0; i < run.z_count; i++)
                oamSet(&oamMain, 1 + (int)i,
                       run.zx[i] / GL_ONE - 8, run.zy[i] / GL_ONE - 8,
                       0, 0, SpriteSize_16x16, SpriteColorFormat_16Color,
                       shambler_gfx, -1, false,
                       run.zstun[i] > 0 /* hflip = reeling from the shove */,
                       false, false, false);
            for (int i = 0; i < GL_BARRICADE_CAP; i++)
            {
                int oam_id = 1 + GL_ZOMBIE_CAP + i;
                if (run.bhp[i] == 0)
                {
                    oamSetHidden(&oamMain, oam_id, true);
                    continue;
                }
                oamSet(&oamMain, oam_id,
                       run.bx[i] / GL_ONE - 8, run.by[i] / GL_ONE - 8,
                       1 /* under player + dead */, 0, SpriteSize_16x16,
                       SpriteColorFormat_16Color, barricade_gfx, -1, false,
                       false,
                       run.bhp[i] <= GL_BARRICADE_HP / 2
                       /* vflip = splintered: half hp or less */,
                       false, false);
            }
            for (int i = 0; i < GL_CACHE_COUNT; i++)
            {
                int oam_id = 1 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP + i;
                if (state != STATE_SCAVENGE || !run.cache_up[i])
                {
                    oamSetHidden(&oamMain, oam_id, true);
                    continue;
                }
                oamSet(&oamMain, oam_id,
                       run.cx[i] / GL_ONE - 8, run.cy[i] / GL_ONE - 8,
                       1 /* under player + dead */, 0, SpriteSize_16x16,
                       SpriteColorFormat_16Color, cache_gfx, -1, false,
                       false, false, false, false);
            }
            for (int i = 0; i < GL_FLASK_COUNT; i++)
            {
                int oam_id = 1 + GL_ZOMBIE_CAP + GL_BARRICADE_CAP
                             + GL_CACHE_COUNT + i;
                if (state != STATE_SCAVENGE || !run.flask_up[i])
                {
                    oamSetHidden(&oamMain, oam_id, true);
                    continue;
                }
                oamSet(&oamMain, oam_id,
                       run.fx[i] / GL_ONE - 8, run.fy[i] / GL_ONE - 8,
                       1 /* under player + dead */, 0, SpriteSize_16x16,
                       SpriteColorFormat_16Color, flask_gfx, -1, false,
                       false, false, false, false);
            }
        }
        // The lamplight itself (slice 7): the yard's backdrop carries a
        // faint lantern-blue that dims with the light radius — full
        // light = 5/31 blue, guttering = 1/31, gone everywhere else.
        // One palette write per frame; pure render of pure state.
        int32_t lradius = gl_light_radius(run.oil);
        BG_PALETTE[0] = (state == STATE_PLAYING)
            ? RGB15(0, 0, (lradius / GL_ONE) / 16)
            : RGB15(0, 0, 0);

        oamUpdate(&oamMain);
        draw_watch_map(&run, state);

        // Nearest Shambler (the one that can kill you) for the mailbox;
        // arrays persist through the death/dawn cards, so the freeze-frame
        // numbers stay readable there — exactly slice-3 behavior when one
        // zombie is up.
        uint32_t near_i = 0;
        int32_t near_d = 0;
        uint32_t near_stun = 0;
        if (run.z_count > 0)
        {
            near_d = INT32_MAX;
            for (uint32_t i = 0; i < run.z_count; i++)
            {
                int32_t d = gl_chebyshev(run.px, run.py,
                                         run.zx[i], run.zy[i]);
                if (d < near_d) { near_d = d; near_i = i; }
            }
            near_stun = run.zstun[near_i];
        }

        // Remaining plank cache nearest the player (same idiom).
        uint32_t cache_up_n = 0;
        int nearc_i = -1;
        int32_t nearc_d = INT32_MAX;
        for (int i = 0; i < GL_CACHE_COUNT; i++)
        {
            if (!run.cache_up[i])
                continue;
            cache_up_n++;
            int32_t d = gl_chebyshev(run.px, run.py, run.cx[i], run.cy[i]);
            if (d < nearc_d) { nearc_d = d; nearc_i = i; }
        }

        // Remaining oil flask nearest the player (same idiom).
        uint32_t flask_up_n = 0;
        int nearf_i = -1;
        int32_t nearf_d = INT32_MAX;
        for (int i = 0; i < GL_FLASK_COUNT; i++)
        {
            if (!run.flask_up[i])
                continue;
            flask_up_n++;
            int32_t d = gl_chebyshev(run.px, run.py, run.fx[i], run.fy[i]);
            if (d < nearf_d) { nearf_d = d; nearf_i = i; }
        }

        // Intact barricade nearest the player (mirrors the ZX/ZY idiom).
        uint32_t barr_up = 0;
        int nearb_i = -1;
        int32_t nearb_d = INT32_MAX;
        for (int i = 0; i < GL_BARRICADE_CAP; i++)
        {
            if (run.bhp[i] == 0)
                continue;
            barr_up++;
            int32_t d = gl_chebyshev(run.px, run.py, run.bx[i], run.by[i]);
            if (d < nearb_d) { nearb_d = d; nearb_i = i; }
        }

        // --- slice-8 audio (decision layer pure + mirrored; playback =
        // hardware channels via the ARM7 sound FIFO; nothing feeds back
        // into the sim) -------------------------------------------------
        int press_now = (state == STATE_PLAYING && run.z_count > 0
                         && gl_dark_press(run.oil, near_d)) ? 1 : 0;

        // One-shot cue channel: close it when its frames run out.
        if (cue_left > 0 && --cue_left == 0)
        {
            soundKill(cue_ch);
            cue_ch = -1;
        }

        // Which cue does this frame want? Highest id wins the channel
        // (the id order IS the priority order — gl_sim.h).
        uint32_t cue = GL_CUE_NONE;
        if (shoves > prev_shoves)
            cue = GL_CUE_SHOVE;
        if (places > prev_places || repairs > prev_repairs)
            cue = GL_CUE_PLANK;
        if (scavenged > prev_scavenged)
            cue = GL_CUE_CACHE;
        if (oil_grabs > prev_oil_grabs)
            cue = GL_CUE_FLASK;
        if (breaches > prev_breaches)
            cue = GL_CUE_BREACH;
        if (state == STATE_PLAYING && prev_state != STATE_PLAYING)
            cue = GL_CUE_NIGHTFALL;          // every path into a night
        if (state == STATE_DAWN && prev_state == STATE_PLAYING)
            cue = GL_CUE_DAWN;
        if (state == STATE_DEAD && prev_state != STATE_DEAD)
            cue = GL_CUE_DEATH;
        if (cue != GL_CUE_NONE)
        {
            if (cue_ch >= 0)
                soundKill(cue_ch);           // a new cue preempts
            cue_ch = (gl_cue_duty(cue) == GL_CUE_ON_NOISE)
                ? soundPlayNoise((u16)gl_cue_freq(cue),
                                 (u8)gl_cue_vol(cue), 64)
                : soundPlayPSG((DutyCycle)gl_cue_duty(cue),
                               (u16)gl_cue_freq(cue),
                               (u8)gl_cue_vol(cue), 64);
            cue_left = gl_cue_len(cue);
            last_cue = cue;
            cues++;
            cue_frame = frame;
        }

        // Ambience drone: on during the night and the interlude, off on
        // the title/cards (dying silences the moor). The FIFO is touched
        // ONLY when the wanted tier flips or the drone starts/stops —
        // between flips the hardware channel free-runs at zero ARM9 cost.
        if (state == STATE_PLAYING || state == STATE_SCAVENGE)
        {
            uint32_t tier = gl_amb_tier(state == STATE_PLAYING, run.oil,
                                        press_now);
            if (amb_ch < 0 || tier != amb_tier)
            {
                if (amb_ch >= 0)
                    soundKill(amb_ch);
                amb_ch = soundPlayPSG((DutyCycle)gl_amb_duty(tier),
                                      (u16)gl_amb_freq(tier),
                                      (u8)gl_amb_vol(tier), 64);
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

        // Frame-cost probe: swiWaitForVBlank returns at scanline 192 (the
        // top of vblank); REG_VCOUNT here says how many scanlines this
        // frame's whole update took. <71 = everything fit inside vblank,
        // <263 = still 60 fps (the next vblank wasn't missed).
        uint32_t vlines = (REG_VCOUNT + 263u - 192u) % 263u;
        if (vlines > vlines_max)
            vlines_max = vlines;

        // Telemetry mailbox: rewritten every frame, read headlessly.
        gl_telemetry[GL_T_FRAME] = frame;
        gl_telemetry[GL_T_STATE] = (uint32_t)state;
        gl_telemetry[GL_T_PX] = (uint32_t)run.px;
        gl_telemetry[GL_T_PY] = (uint32_t)run.py;
        gl_telemetry[GL_T_ZX] =
            (uint32_t)(run.z_count > 0 ? run.zx[near_i] : 0);
        gl_telemetry[GL_T_ZY] =
            (uint32_t)(run.z_count > 0 ? run.zy[near_i] : 0);
        gl_telemetry[GL_T_DAWN] = run.dawn_left;
        gl_telemetry[GL_T_NIGHT] = run.night;
        gl_telemetry[GL_T_NIGHTS] = nights_survived;
        gl_telemetry[GL_T_DEATHS] = deaths;
        gl_telemetry[GL_T_SEED] = run.seed;
        gl_telemetry[GL_T_DIST] = (uint32_t)near_d;
        gl_telemetry[GL_T_RUNFRAME] = run.run_frame;
        gl_telemetry[GL_T_ZALIVE] = run.z_count;
        gl_telemetry[GL_T_ZTOTAL] = run.wave_total;
        gl_telemetry[GL_T_SHOVECD] = run.shove_cd;
        gl_telemetry[GL_T_SHOVES] = shoves;
        gl_telemetry[GL_T_NSTUN] = near_stun;
        gl_telemetry[GL_T_VLINES] = vlines;
        gl_telemetry[GL_T_VLMAX] = vlines_max;
        gl_telemetry[GL_T_PLANKS] = run.planks;
        gl_telemetry[GL_T_BARR] = barr_up;
        gl_telemetry[GL_T_BHP] = nearb_i >= 0 ? run.bhp[nearb_i] : 0;
        gl_telemetry[GL_T_BREACH] = breaches;
        gl_telemetry[GL_T_PLACES] = places;
        gl_telemetry[GL_T_REPAIRS] = repairs;
        gl_telemetry[GL_T_BX] = (uint32_t)(nearb_i >= 0 ? run.bx[nearb_i]
                                                        : 0);
        gl_telemetry[GL_T_BY] = (uint32_t)(nearb_i >= 0 ? run.by[nearb_i]
                                                        : 0);
        gl_telemetry[GL_T_SCAVLEFT] = run.scav_left;
        gl_telemetry[GL_T_CACHES] = cache_up_n;
        gl_telemetry[GL_T_SCAVENGED] = scavenged;
        gl_telemetry[GL_T_SCAVS] = scavs;
        gl_telemetry[GL_T_CX] = (uint32_t)(nearc_i >= 0 ? run.cx[nearc_i]
                                                        : 0);
        gl_telemetry[GL_T_CY] = (uint32_t)(nearc_i >= 0 ? run.cy[nearc_i]
                                                        : 0);
        gl_telemetry[GL_T_CDIST] = (uint32_t)(nearc_i >= 0 ? nearc_d : 0);
        gl_telemetry[GL_T_SPARE2] = 0;
        gl_telemetry[GL_T_OIL] = run.oil;
        gl_telemetry[GL_T_LRADIUS] = (uint32_t)lradius;
        gl_telemetry[GL_T_PRESS] = (uint32_t)press_now;
        gl_telemetry[GL_T_FLASKS] = flask_up_n;
        gl_telemetry[GL_T_OILGRAB] = oil_grabs;
        gl_telemetry[GL_T_FX] = (uint32_t)(nearf_i >= 0 ? run.fx[nearf_i]
                                                        : 0);
        gl_telemetry[GL_T_FY] = (uint32_t)(nearf_i >= 0 ? run.fy[nearf_i]
                                                        : 0);
        gl_telemetry[GL_T_FDIST] = (uint32_t)(nearf_i >= 0 ? nearf_d : 0);
        gl_telemetry[GL_T_ATIER] = amb_ch >= 0 ? amb_tier : 0;
        gl_telemetry[GL_T_ACUE] = last_cue;
        gl_telemetry[GL_T_ACUES] = cues;
        gl_telemetry[GL_T_ACUEFR] = cue_frame;
        gl_telemetry[GL_T_ADRONE] =
            amb_ch >= 0 ? gl_amb_freq(amb_tier) : 0;
        gl_telemetry[GL_T_AFLIPS] = amb_flips;
        gl_telemetry[GL_T_ASFXL] = cue_left;
        gl_telemetry[GL_T_SPARE3] = 0;
        gl_telemetry[GL_T_BEST] = best_nights;
        gl_telemetry[GL_T_BESTSEED] = best_seed;
        gl_telemetry[GL_T_SAVEOK] = save_ok;
        gl_telemetry[GL_T_SAVEWR] = save_writes;
        gl_telemetry[GL_T_SAVEVER] = GL_SAVE_VERSION;
        gl_telemetry[GL_T_SPARE4] = 0;
        gl_telemetry[GL_T_SPARE5] = 0;
        gl_telemetry[GL_T_SPARE6] = 0;
        gl_telemetry[GL_T_MARKON] = run.mark_on;
        gl_telemetry[GL_T_MARKX] = (uint32_t)run.mark_x;
        gl_telemetry[GL_T_MARKY] = (uint32_t)run.mark_y;
        gl_telemetry[GL_T_MARKDIST] = (uint32_t)(run.mark_on
            ? gl_chebyshev(run.px, run.py, run.mark_x, run.mark_y) : 0);
        gl_telemetry[GL_T_MARKS] = marks;
        gl_telemetry[GL_T_OUT] = gl_gloam_out(run.wave_total, run.z_count);
        gl_telemetry[GL_T_SPARE7] = 0;
        gl_telemetry[GL_T_SPARE8] = 0;
        gl_telemetry[GL_T_REMATCH] = rematch_on;
        gl_telemetry[GL_T_REMATCHES] = rematches;
        gl_telemetry[GL_T_SPARE9] = 0;
        gl_telemetry[GL_T_SPARE10] = 0;
        gl_telemetry[GL_T_SPARE11] = 0;
        gl_telemetry[GL_T_SPARE12] = 0;
        gl_telemetry[GL_T_SPARE13] = 0;
        gl_telemetry[GL_T_SPARE14] = 0;
    }

    return 0;
}
