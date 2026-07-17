// Brineward — pure simulation layer (arc slice 2, walking skeleton).
//
// EVERYTHING in this header + bw_sim.c is a pure function of its arguments:
// fixed-point integers only, no wall clock, no runtime RNG, no globals, no
// libnds. That is the concept doc's determinism plan
// (docs/concepts/brineward-concept.md § "Determinism + host-provability"),
// same discipline as Gloamline's gl_sim layer.
//
// MIRROR RULE (keep in lockstep): tools/check-brine.py carries a
// line-for-line Python mirror of every function and constant here. Any
// change to this file MUST land in check-brine.py in the same PR — same
// discipline as gl_sim.c <-> check-gloam.py.
//
// SIGNED-DIVISION RULE (mirror safety): negative operands only ever meet
// division as arithmetic right shifts (>> 8), which gcc/ARM and Python
// both evaluate as floor division — never `/` on a possibly-negative
// value, so the C and the Python mirror agree bit-for-bit.

#ifndef BW_SIM_H
#define BW_SIM_H

#include <stdint.h>

// --- fixed point: 8.8 (1 px = 256 units) -----------------------------------
#define BW_ONE 256

// --- the sea (one open-water screen off the Graywake breakwater) ------------
// Text-console frame: HUD rows 0-1, swell rows 2 and 23. Positions are
// sprite CENTERS (16x16 ships), clamped to the water rectangle.
#define BW_SEA_X_MIN (16 * BW_ONE)
#define BW_SEA_X_MAX (239 * BW_ONE)
#define BW_SEA_Y_MIN (32 * BW_ONE)
#define BW_SEA_Y_MAX (175 * BW_ONE)

#define BW_PLAYER_START_X (128 * BW_ONE)
#define BW_PLAYER_START_Y (140 * BW_ONE)
#define BW_PLAYER_START_HDG 0            // due north, bow to open water

// --- headings ----------------------------------------------------------------
// Heading is 0..1023 (BW_CIRCLE units per full turn), 0 = north, clockwise.
// Direction vectors are 256-brad quantized: brad = heading >> 2, and
// (dx, dy) = (bw_sin(brad), -bw_cos(brad)) in 1/256 units (screen y down).
#define BW_CIRCLE 1024

// --- sail trim (the momentum-sailing knob) -----------------------------------
// 0 = battle sail (slow, tight turn), 1 = half sail, 2 = full sail.
#define BW_TRIM_BATTLE 0
#define BW_TRIM_HALF 1
#define BW_TRIM_FULL 2
// target speed per trim, units/frame (0.375 / 0.625 / 0.875 px/f)
#define BW_SPEED_BATTLE 96
#define BW_SPEED_HALF 160
#define BW_SPEED_FULL 224
// turn rate per trim, heading units/frame (full turn in 2.8s / 4.3s / 8.5s)
#define BW_TURN_BATTLE 6
#define BW_TURN_HALF 4
#define BW_TURN_FULL 2
// momentum: speed eases toward the trim target this fast (units/frame^2)
#define BW_ACCEL 2

// --- broadsides ----------------------------------------------------------------
#define BW_MAX_BALLS 16                  // shared pool, both ships
#define BW_BALLS_PER_SIDE 3              // one broadside = a 3-ball rake
#define BW_BALL_SPEED 640                // 2.5 px/frame
#define BW_BALL_LIFE 48                  // frames -> 120 px range
#define BW_BALL_MUZZLE 10                // px abeam of the hull at spawn
#define BW_BALL_SPREAD 6                 // px between rake balls (along keel)
#define BW_HIT_RANGE (6 * BW_ONE)        // chebyshev ball<->hull center
// range falloff: damage 30 at the muzzle easing to 10 at max range
#define BW_DMG_NEAR 30
#define BW_DMG_FALL 20                   // lost linearly over BW_BALL_LIFE

#define BW_HULL_MAX 100
#define BW_RELOAD_PLAYER 90              // frames per battery
#define BW_RELOAD_ENEMY 150              // the rum-runner's crews are slower

// --- enemy sloop ---------------------------------------------------------------
#define BW_SPAWN_DIST 90                 // px from the player anchorage
#define BW_SPAWN_MIN_DIST (32 * BW_ONE)  // chebyshev floor after sea clamp
#define BW_ENGAGE_RANGE (110 * BW_ONE)   // beyond: intercept; inside: circle
#define BW_FIRE_RANGE (112 * BW_ONE)     // AI holds fire past this
// AI alignment arcs, as |cross| : |dot| ratios of the beam-line test:
// fire when 12*|dot| < |cross| (~4.8 deg off the beam); stop correcting
// the turn when 16*|dot| < |cross| so the helm settles INSIDE the arc.
#define BW_AI_FIRE_ARC 12
#define BW_AI_TURN_ARC 16

// --- duel outcome ----------------------------------------------------------------
#define BW_DUEL_RUNNING 0
#define BW_DUEL_PLAYER_SUNK 1            // simultaneous sinking counts as loss
#define BW_DUEL_ENEMY_SUNK 2

// --- loot / gold (arc slice 3: roadmap item 1) -------------------------------
// A sunk rum-runner breaks up into flotsam: BW_LOOT_DROPS crates on a
// BW_LOOT_RING around the wreck at fixed third-of-circle angles (pure
// f(wreck position) — deliberately NOT frame-hashed, so a +-frame input
// skew moves the crates only as far as it moves the wreck). Sail within
// BW_SCOOP_RANGE to scoop; the hold caps at BW_HOLD_CAP crates. Banking:
// lie alongside the Graywake pier (BW_PIER_*, the breakwater south of
// the anchorage) within BW_DOCK_RANGE and the hold empties into banked
// gold. Banked gold is safe forever; sinking forfeits the unbanked hold
// (main.c simply does not carry it into the next duel).
#define BW_MAX_LOOT 8
#define BW_LOOT_DROPS 3                  // crates per sunk ship
#define BW_LOOT_VALUE 5                  // gold per crate (band-0 waters)
#define BW_HOLD_CAP 8                    // crates the sloop can carry
#define BW_LOOT_RING 18                  // px, crate ring radius off the wreck
#define BW_SCOOP_RANGE (10 * BW_ONE)     // chebyshev hull<->crate
#define BW_PIER_X (128 * BW_ONE)
#define BW_PIER_Y (172 * BW_ONE)
#define BW_DOCK_RANGE (12 * BW_ONE)      // chebyshev hull<->pier

// --- port + upgrades (arc slice 4: roadmap item 2) ---------------------------
// Banked gold buys PERSISTENT player-ship upgrades at the Graywake port
// (main.c hangs the menu off the same pier berth the hold banks at).
// Three tracks, tiers I/II/III (0-based 0..2), concept-doc shaped:
//   hull    — max HP 100/150/220 (the doc's exact ladder);
//   cannons — battery reload 90/70/55 frames AND balls per rake 3/3/4;
//   sails   — trim-target speed +0/+24/+48 units/f and turn +0/+1/+2.
// Tier 0 IS the slice-2 sloop: every tier-0 table entry equals the
// legacy constant, so all recorded routes and carried proof pins stay
// bit-valid (check-brine.py asserts this identity). The ENEMY never
// upgrades — the tables apply to the player only. Prices triple per
// step (one banked band-0 wreck = 15g buys the first tier of one
// track); repair prices the MISSING hull at BW_REPAIR_PER_GOLD points
// per gold, rounded against the player. All one-constant owner-tunables
// (tables live in bw_sim.c next to the trim tables).
#define BW_UP_TIERS 3
#define BW_REPAIR_PER_GOLD 4             // hull points one gold repairs
// port menu rows (bw_port_buy's ledger; main.c renders the same order)
#define BW_PORT_ROW_HULL 0
#define BW_PORT_ROW_CANNON 1
#define BW_PORT_ROW_SAIL 2
#define BW_PORT_ROW_REPAIR 3
#define BW_PORT_ROWS 4

// --- the Maw (arc slice 5: roadmap item 3 — first sea monster) ----------------
// The break-the-geometry enemy class (concept doc: "a maw that surfaces
// under you (watch the shadow)"; roadmap: "shadow telegraph -> surface ->
// lunge"). The wreck's blood draws it: BW_MAW_PATIENCE salvage frames
// after the rum-runner goes under, a shadow rises AT THE WRECK (pure
// f(wreck position), same rationale as the crate ring — no frame hash)
// and homes under the player; after a fixed telegraph it SURFACES where
// the shadow lies, winds up, then LUNGES at the player's latched bearing
// — a bite costs BW_MAW_BITE hull, then it sounds and stalks again.
// It has no batteries and no beam to keep: the broadside rules do not
// apply to it — but they apply to YOU: while it is up (surface/lunge)
// the salvage-water batteries wake and a rake can wound it. Slay it
// (BW_MAW_HULL) and it breaks up like any wreck — richer: BW_MAW_LOOT_
// DROPS crates worth BW_MAW_LOOT_VALUE each (the doc's "monster drops
// more"). The pier is SANCTUARY (session-28 guard recipe): the shadow
// never enters, it never surfaces, and it never begins a lunge at a
// player inside BW_MAW_HARBOR of the berth — the port can always be
// reached. The Maw exists only in the salvage water: bw_duel_init
// resets it and bw_duel_step never steps it, so every duel-phase route
// and pin is bit-identical to slice 4. All one-constant owner-tunables.
#define BW_MAW_PATIENCE 600              // quiet salvage frames before the stir
#define BW_MAW_RESTIR 240                // frames until it stalks again
#define BW_MAW_SHADOW_FRAMES 150         // telegraph: the shadow homes this long
#define BW_MAW_SURFACE_FRAMES 60         // windup on the surface before the lunge
#define BW_MAW_LUNGE_FRAMES 55           // committed lunge: ~112 px of reach —
                                         //   a full-sail beam dodge clears it
#define BW_MAW_SHADOW_SPEED 140          // units/frame — full sail outruns it
                                         //   even diagonally (224*sin45 ~ 158);
                                         //   a battle-sail scooper (96) does not
#define BW_MAW_LUNGE_SPEED 520           // units/frame — no sail outruns THIS
#define BW_MAW_HULL 120                  // ~two clean rakes of the tier-0 battery
#define BW_MAW_HIT_RANGE (12 * BW_ONE)   // it is a big target (ball chebyshev)
#define BW_MAW_BITE_RANGE (12 * BW_ONE)  // jaws chebyshev
#define BW_MAW_BITE 35                   // hull points a bite costs
#define BW_MAW_HARBOR (40 * BW_ONE)      // pier sanctuary radius (chebyshev)
#define BW_MAW_LOOT_DROPS 3
#define BW_MAW_LOOT_VALUE 15             // per crate — monster salvage is richer
// Maw lifecycle states
#define BW_MAW_DOWN 0                    // beneath the water, biding
#define BW_MAW_SHADOW 1                  // the telegraph: dark water, homing
#define BW_MAW_SURFACE 2                 // up and vulnerable, winding up
#define BW_MAW_LUNGE 3                   // committed charge at the latched bearing

// --- wind + sailing feel (arc slice 6: roadmap item 4) -------------------------
// The concept doc's owner-taste slice: "a slow-rotating wind vector,
// trim interacts with it". Each water gets a WEATHER from its seed
// (pure f(seed), printed on the HUD like the seed itself): dead calm,
// a breeze, or a gale. The wind VECTOR rotates slowly all run long
// (one heading unit every 2^BW_WIND_TURN_SHIFT frames — a full box of
// the compass in ~2:16), and the point of sail scales the trim's
// target speed: running with the wind fills the canvas (+push),
// beating into it fights the hull (-push), abeam is neutral —
// cos-shaped between, and the more canvas set, the more the weather
// matters (full sail catches 4/4 of the push, half 2/4, battle 1/4:
// the trim interaction the doc asks for; shortening sail is now ALSO
// the storm verb). The wind moves HULLS only — iron balls and the Maw
// (which swims UNDER the water) don't care; the broadside and
// telegraph contracts are untouched by weather.
//
// PIN-CARRY RULE (decide-and-flag): BW_WIND_SALT is chosen so every
// committed proof/recorder anchor seed (116, 117, 118, 119, 126, 127,
// 558, 728) maps to CALM — a calm water's wind term is exactly zero,
// so every slice-2..5 recorded route and emulator pin is bit-identical
// and carries verbatim (the Maw-slice gate pattern: the new mechanic
// provably never fires inside a committed story). check-brine.py
// asserts the calm-anchor identity loudly, so a salt or table retune
// fails the mirror before it silently re-times a route.
//
// Escapability rails survive the weather (asserted in the mirror):
// a gale-beating full sail still outruns the shadow on its worst
// (diagonal) axis ((224-24)*181>>8 = 141 > 140), and a gale-running
// battle-sail scooper still does NOT (96+6 = 102 < 140).
#define BW_WIND_CALM 0
#define BW_WIND_BREEZE 1
#define BW_WIND_GALE 2
#define BW_WIND_STATES 3
#define BW_WIND_SALT 0x57499826u         // level = f(seed); calm anchors, see above
#define BW_WIND_DIR_SALT 0x57492881u     // base bearing = f(seed)
#define BW_WIND_TURN_SHIFT 3             // 1 heading unit per 8 frames (slow box)

// --- danger bands + reefs (arc slice 7: roadmap item 5) -------------------------
// The concept doc's spatial-difficulty beat ("band spawn tables,
// reefs/islands, deepest band = charted-waters score"), reframed for
// the one-screen sea the arc has proven: a WATER carries a danger band
// 0..2, and pressing SELECT in the salvage water puts out one band
// DEEPER (carrying hold, dents, and tiers, exactly like START — the
// doc's "one more fight or turn for home?" now has a third answer:
// deeper). START from salvage stays in the same band; sinking respawns
// at port in band 0. The deepest band ever sailed is the CHARTED score
// (main.c's Score, on the HUD cards like wins/maws).
//
// Deeper water is richer and worse, all one-constant tables indexed by
// band with band 0 == the slice-6 constants EXACTLY (the pin-carry
// identity, asserted loudly in check-brine.py): the rum-runners run
// heavier hulls and faster gun crews, crates are worth more (the doc's
// ~5g band 0 -> ~25g band 2 shape), the seeded weather worsens one
// level per band (min-clamped at gale — the doc's "the worse the
// weather"; the slice-6 escape rails already hold at gale, so no new
// speed constant is needed), and REEFS stud the water: fixed rocks at
// pure f(seed) positions (hash + bounded rehash for anchorage/pier
// clearance, then a fixed far-north fallback shelf — every water is
// provably sailable out of the anchorage and into the berth). A reef
// scrapes the PLAYER's keel only — the rum-runners and the Maw know
// these waters (decide-and-flag: no AI change, the hazard is the
// player's to chart) — costing BW_REEF_DMG hull ONCE per contact (a
// latch clears when clear of the rock; groundings can sink you).
//
// THE GATE this slice (the time-gate's and seed-bucket's sibling):
// the new mechanic sits behind a fresh input verb (SELECT) that NO
// committed story presses, and behind band tables whose band-0 row is
// the legacy constants — every slice-2..6 route and pin carries
// verbatim (bw_water_init(d, seed, 0) is asserted state-identical to
// bw_duel_init(d, seed) in the mirror).
#define BW_BANDS 3
#define BW_MAX_REEFS 5
#define BW_REEF_SALT 0x8EEF5EEDu         // reef x stream = f(seed)
#define BW_REEF_Y_SALT 0xB0A7B0A7u       // reef y draws its own hash off x's
#define BW_REEF_TRIES 8                  // bounded rehash before the shelf
#define BW_REEF_RANGE (8 * BW_ONE)       // chebyshev keel<->rock = a scrape
#define BW_REEF_DMG 15                   // hull points one scrape costs
#define BW_REEF_CLEAR (28 * BW_ONE)      // rocks keep off the anchorage
                                         //   (pier clearance: BW_MAW_HARBOR)
#define BW_REEF_SHELF_X 40               // fallback shelf, px: x = SHELF_X +
#define BW_REEF_SHELF_STEP 40            //   k*STEP, y = SHELF_Y — far north,
#define BW_REEF_SHELF_Y 44               //   provably clear of both rails

// --- synthesized audio (arc slice 8: roadmap item 6) ---------------------------
// The concept doc's "synthesized audio set" ("cannon thump (noise burst
// + low sine), hull crack, splash hiss, a two-note dock chime. No
// sampled or licensed audio, ever"), on the Gloamline slice-8 proven
// pattern: playback lives on the NDS hardware PSG channels (square +
// noise generators, serviced by the default BlocksDS ARM7 core over
// the libnds sound FIFO) — every sound is synthesized in code from the
// pure parameters below; NO sampled or binary audio asset exists
// anywhere in this repo. The DECISION layer is pure and mirrored
// (check-brine.py, lockstep as ever): bw_amb_tier says which ambience
// the water wants this frame, bw_cue_* say what each one-shot cue
// sounds like, and the cue ids double as the priority order (on a
// frame with several events the HIGHEST id wins the one SFX channel).
// Playback itself (FIFO writes) is render-like ARM9 glue in main.c —
// audio feeds NOTHING back into the sim, so every pre-slice-8 route
// and pin holds bit-identically (zero re-pins by construction). All
// numbers are decide-and-flag owner-tunables (one table row per sound
// in bw_sim.c). Session-38 guard-recipe rail honored: the reef cue
// fires off the player-only groundings counter, never off enemy
// positions.
//
// Ambience tiers: the WEATHER sings in the rigging (the slice-6 wind
// level IS the drone ladder — calm swell, a breeze in the canvas, the
// gale's howl), and the Maw's presence overrides the lot: while
// anything is up or under the keel (shadow included — hearing the
// telegraph matters), the deep throbs. Drone only on live water;
// the title, the sunk card and the port berth are silent (dying
// silences the sea; the tavern keeps its own counsel).
#define BW_AMB_CALM 0                    // calm water: the low swell hum
#define BW_AMB_BREEZE 1                  // a breeze fills the canvas
#define BW_AMB_GALE 2                    // the gale howls in the rigging
#define BW_AMB_MAW 3                     // dark water: it is beneath you
#define BW_AMB_TIERS 4
//
// One-shot cue ids (0 = none). ID ORDER IS PRIORITY: sinking > bite >
// the Maw rising > the dock chime > a wreck going under > the keel
// scraping > a hull cracking > a crate scooped > a spent rake's
// splash > the guns' own thump.
#define BW_CUE_NONE 0
#define BW_CUE_CANNON 1                  // a broadside thumps (either ship)
#define BW_CUE_SPLASH 2                  // a spent rake hisses into the swell
#define BW_CUE_SCOOP 3                   // a crate thuds aboard
#define BW_CUE_CRACK 4                   // a ball cracks a hull (any hull)
#define BW_CUE_SCRAPE 5                  // a reef grates the keel (player-only
                                         //   groundings counter, per the rail)
#define BW_CUE_WRECK 6                   // a ship or the Maw goes under
#define BW_CUE_DOCK 7                    // the two-note Graywake chime
#define BW_CUE_MAWRISE 8                 // the Maw breaks the surface
#define BW_CUE_BITE 9                    // the jaws close on the hull
#define BW_CUE_SUNK 10                   // your own ship goes down
#define BW_CUE_COUNT 11                  // ids 0..10
//
// bw_cue_duty(cue) == BW_CUE_ON_NOISE routes the cue to the hardware
// noise channel (soundPlayNoise) instead of a square wave; otherwise
// the value is the libnds DutyCycle register code (0..7). A nonzero
// bw_cue_freq2(cue) makes the cue TWO-NOTE: the channel re-tunes to
// freq2 after the first half of bw_cue_len (the dock chime — a noise
// cue has no pitch to re-tune, so freq2 cues are square by contract,
// asserted in the mirror).
#define BW_CUE_ON_NOISE 255

// --- the Graywake ledger save (arc slice 9: roadmap item 7) ---------------------
// The concept doc's final named beat: "saves: banked gold / upgrades /
// chart persist". The doc flagged the NDS save path as an open
// question; Gloamline has since CLOSED it in this repo (its slice 9
// proved the whole cartridge-EEPROM battery path headlessly, including
// the libnds-vs-DeSmuME chip-select/HOLD-bit protocol trap and the
// bounded-poll rule — see gl_sim.h § best-nights save and Gloamline's
// main.c save_* helpers, which main.c here ports verbatim in shape).
// The persisted record is ONE fixed 32-byte blob: 8 little-endian u32
// words { magic, version, banked gold, packed upgrade tiers
// (hull | cannon<<8 | sail<<16), charted best band, 2 spares (0),
// checksum-of-words-0..6 } — deterministic serialization, one type-2
// EEPROM page write. SAFETY BY CONSTRUCTION: bw_save_decode returns 0
// (and the caller keeps the fresh ledger) on ANY bad blob — wrong
// magic, wrong version, bad checksum, out-of-range tiers/band,
// blank/0xFF chip — a corrupt save can reset the ledger, NEVER crash
// or hang the game. Writes are wear-disciplined: the ARM9 writes ONLY
// when the persisted tuple CHANGES (a pier bank, a port purchase or
// repair, a deeper band charted) — never per frame, and never on a
// sinking (a sinking forfeits the HOLD; nothing banked changes).
// Audio state (slots 38-45) is deliberately NOT save material — the
// drone/cue state is power-on-transient (session-42 guard recipe).
#define BW_SAVE_MAGIC 0x42525356u            // 'BRSV'
#define BW_SAVE_VERSION 1u                   // bump on any layout change
#define BW_SAVE_WORDS 8
#define BW_SAVE_BYTES 32                     // one type-2 EEPROM page
#define BW_SAVE_ADDR 0                       // blob offset in the backup
#define BW_SAVE_EEPROM_TYPE 2                // libnds addrtype (see card.h)
#define BW_SAVE_SALT 0x4C454447u             // 'LEDG' checksum stream
// Iteration cap on every card-SPI wait in main.c's save write path —
// Gloamline's measured sizing carried verbatim: ~4096 RDSR polls
// outlast a real EEPROM's ~5 ms page program with margin, while a
// chip/emulator that never answers (DeSmuME reads the status busy
// forever) costs only a bounded sub-frame stall — an expired poll is
// harmless by construction (the page data commits on chip-deselect).
#define BW_SAVE_POLL_BOUND 4096u

// --- the Grasper (arc B3 «bestiary» cut 1 — the SECOND sea monster) --------------
// The concept doc promised a monster CLASS and shipped one member (the
// Maw); the Grasper is the second — "arms that grapple and hold you
// still while cutters close in" (docs/concepts/brineward-concept.md
// § Core loop). Cut 1 builds the Grasper ITSELF: the grapple-and-HOLD
// verb. (The reserved-B ram/brace break-free verb is cut 2; the cutters
// closing on a held ship are cut 3; the hold-cap economy is cut 4 — see
// games/brineward-nds/ARC-BESTIARY.md.)
//
// It is the Maw's sibling in the salvage water, with the OPPOSITE
// break-the-geometry gimmick: where the Maw is a telegraphed one-shot
// LUNGE you dodge, the Grasper REACHES up out of the deep and, if the
// arms close on you, SEIZES the sloop and PINS it still — momentum,
// helm and way all dead — for a fixed hold, taking one seize's worth of
// hull. In cut 1 there is no break-free verb: the lesson is spatial,
// exactly the Maw's — keep sea room off the reaching arms (a full-sail
// beam clears the reach; a battle-sail scooper does not). While the
// arms are up the salvage batteries wake and a rake can wound it; slay
// it and it breaks up richer, like any monster (and the seize releases).
//
// THE GATE (the pin-carry rule, the Maw/wind/band sibling): a water
// holds a Grasper OR a Maw, never both — bw_has_grasper(seed) buckets
// the seed, and BW_GRASPER_SALT is pinned so EVERY committed anchor and
// every host-checked salvage seed is a NON-grasper (Maw) water. In a
// non-grasper water bw_grasper_step is a pure no-op and bw_maw_step is
// unchanged, so every slice-2..9 recorded route and emulator pin is
// bit-identical and carries verbatim (check-brine.py asserts the
// no-grasper identity loudly). The Grasper lives only in the salvage
// water: bw_duel_init resets it and bw_duel_step never steps it, so
// every duel-phase route and pin is bit-identical too.
#define BW_GRASPER_SALT 0x10000042u      // grasper-water bucket = f(seed); pinned
                                         //   so every committed seed is a Maw water
#define BW_GRASPER_BUCKET 15             // has-grasper when hash & BUCKET == 0 (~1/16)
#define BW_GRASPER_PATIENCE 480          // quiet salvage frames before the arms stir
#define BW_GRASPER_RESTIR 240            // frames until the arms reach again
#define BW_GRASPER_REACH_FRAMES 150      // telegraph: the arms rise and home this long
#define BW_GRASPER_REACH_SPEED 120       // units/frame — full sail (224, even on the
                                         //   diagonal 158) outruns the reach; a
                                         //   battle-sail scooper (96) does not
#define BW_GRASPER_GRAB_RANGE (12 * BW_ONE)  // arms<->hull chebyshev at the seize test
#define BW_GRASPER_HOLD_FRAMES 90        // frames the seized sloop is pinned still
#define BW_GRASPER_GRAB_BITE 20          // hull the seize costs, ONCE
#define BW_GRASPER_HULL 90               // ~ a rake and a half of the tier-0 battery
#define BW_GRASPER_HIT_RANGE (14 * BW_ONE)   // a big, sprawling target (ball chebyshev)
#define BW_GRASPER_HARBOR (40 * BW_ONE)  // pier sanctuary radius (chebyshev)
#define BW_GRASPER_LOOT_DROPS 3          // crates the slain arms shed (monster-rich value)
// Grasper lifecycle states
#define BW_GRASPER_DOWN 0                // beneath the water, biding
#define BW_GRASPER_REACH 1               // the telegraph: arms up, homing (vulnerable)
#define BW_GRASPER_HOLD 2                // it has you: the sloop is pinned still (vulnerable)

// Cut 2 «Ram/brace» — the concept's reserved B verb, built as the
// BREAK-FREE wrench. A seized sloop can brace (edge-triggered B) to
// wrench loose from the hold early, at a cost: the FIRST brace that would
// actually shorten the hold pays BW_GRASPER_BRACE_HULL hull ONCE and the
// arms slip BW_GRASPER_BRACE_FRAMES frames later (a real tempo cost, not
// a teleport). A brace with nothing left to shorten (already inside the
// last BRACE_FRAMES) does nothing, and a Maw water / B-silent route never
// reaches the branch — so every slice-2..9 and cut-1 route carries
// verbatim (the input-verb gate, the SELECT precedent).
#define BW_GRASPER_BRACE_HULL 10         // hull the wrench-loose costs, ONCE
#define BW_GRASPER_BRACE_FRAMES 12       // the arms slip this many frames after a brace

// --- state -----------------------------------------------------------------------
typedef struct
{
    int32_t x, y;                        // 8.8 fixed, sprite center
    int32_t heading;                     // 0..1023
    int32_t trim;                        // 0..2
    int32_t speed;                       // units/frame, eases to trim target
    int32_t hull;                        // 0..BW_HULL_MAX
    int32_t reload_l, reload_r;          // frames until each battery is ready
} BwShip;

typedef struct
{
    int32_t x, y;                        // 8.8 fixed
    int32_t vx, vy;                      // units/frame
    int32_t age;                         // frames since fired
    int32_t owner;                       // 0 = player, 1 = enemy
    int32_t live;
} BwBall;

typedef struct
{
    int32_t x, y;                        // 8.8 fixed, crate center
    int32_t value;                       // gold when scooped (slice 5: the
    int32_t live;                        //   Maw's crates are richer)
} BwLoot;

typedef struct
{
    int32_t x, y;                        // 8.8 fixed (shadow or surfaced body)
    int32_t vx, vy;                      // latched lunge velocity, units/frame
    int32_t state;                       // BW_MAW_*
    int32_t hull;                        // 0..BW_MAW_HULL
    int32_t timer;                       // frames in the current state
    uint32_t wake;                       // d->frame at which the next stir fires
    int32_t stirs;                       // times it has risen this water
    int32_t slain;                       // 1: dead — never stirs again
    int32_t bit;                         // this lunge already landed its bite
} BwMaw;

typedef struct
{
    int32_t x, y;                        // 8.8 fixed (the reaching arms' point)
    int32_t gx, gy;                      // latched hold point while it has you
    int32_t state;                       // BW_GRASPER_*
    int32_t hull;                        // 0..BW_GRASPER_HULL
    int32_t timer;                       // frames in the current state
    uint32_t wake;                       // d->frame at which the next reach fires
    int32_t stirs;                       // times the arms have risen this water
    int32_t slain;                       // 1: dead — never reaches again
} BwGrasper;

typedef struct
{
    int32_t x, y;                        // 8.8 fixed, rock center (slice 7)
    int32_t live;
    int32_t scraped;                     // this contact already took its toll
} BwReef;

typedef struct
{
    BwShip player;
    BwShip enemy;
    BwBall balls[BW_MAX_BALLS];
    BwLoot loot[BW_MAX_LOOT];            // flotsam afloat (slice 3)
    BwMaw maw;                           // the salvage-water stalker (slice 5)
    BwGrasper grasper;                   // the OTHER salvage-water terror (bestiary
                                         //   cut 1) — a grasper water has this OR
                                         //   the Maw, never both
    BwReef reefs[BW_MAX_REEFS];          // this water's rocks (slice 7)
    int32_t hold;                        // crates aboard, 0..BW_HOLD_CAP
    int32_t hold_gold;                   // unbanked gold those crates are worth
    int32_t up_hull;                     // player upgrade tiers, 0..2
    int32_t up_cannon;                   //   (slice 4; enemy never upgrades;
    int32_t up_sail;                     //    caller re-injects, like hold)
    int32_t wind_level;                  // this water's weather, BW_WIND_*
    int32_t wind_base;                   //   (slice 6; both pure f(seed))
    int32_t band;                        // this water's danger band, 0..2
    int32_t grasper_water;               // 1: this seed's deep holds a Grasper,
                                         //   not a Maw (bestiary cut 1; f(seed))
    int32_t groundings;                  //   (slice 7) reef scrapes this water
    uint32_t frame;                      // duel frames stepped so far
    int32_t over;                        // BW_DUEL_*
} BwDuel;

// Player inputs for one duel frame. turn in {-1,0,+1}; trim_delta in
// {-1,0,+1} (edge-triggered by the caller); fire_* edge-triggered.
// brace edge-triggered (cut 2): the break-free B verb. Its zero value is
// the legacy behaviour exactly, so a route that never presses B is
// bit-identical — no slice-2..9 or cut-1 story presses it.
typedef struct
{
    int32_t turn;
    int32_t trim_delta;
    int32_t fire_l;                      // port battery
    int32_t fire_r;                      // starboard battery
    int32_t brace;                       // cut 2: break-free wrench (0 = legacy)
} BwInputs;

// --- pure functions ----------------------------------------------------------------

// Deterministic 32-bit hash (Gloamline's gl_hash, the lane's no-runtime-RNG
// primitive).
uint32_t bw_hash(uint32_t a, uint32_t b);

// Quarter-table sine/cosine, 256 brads per circle, result -256..256.
int32_t bw_sin(int32_t brad);
int32_t bw_cos(int32_t brad);

// Chebyshev distance between two centers (the range metric).
int32_t bw_chebyshev(int32_t ax, int32_t ay, int32_t bx, int32_t by);

// The wind right now (slice 6): the heading the wind blows TOWARD
// (0..1023 — sailors would name it by the opposite bearing), rotating
// slowly off the water's seeded base; and the push (units/frame at
// full canvas) of this water's weather — 0 in a calm, so a calm water
// is bit-identical to slice 5.
int32_t bw_wind_heading(const BwDuel *d);
int32_t bw_wind_push(const BwDuel *d);

// Reset a duel: player sloop at the anchorage, enemy sloop on a
// BW_SPAWN_DIST ring at hash angle f(seed), bow toward the player.
// This IS the band-0 water: bw_water_init(d, seed, 0) is state-
// identical (the slice-7 pin-carry identity).
void bw_duel_init(BwDuel *d, uint32_t seed);

// Reset a WATER: bw_duel_init plus the danger band (slice 7) — band
// clamped to 0..BW_BANDS-1, the band tables applied to the rum-runner,
// the seeded weather worsened one level per band (min-clamped at
// gale), and the water's reefs laid at pure f(seed) positions.
void bw_water_init(BwDuel *d, uint32_t seed, int32_t band);

// Does this seed's deep hold a Grasper (bestiary cut 1)? Pure f(seed):
// 1 in a grasper water, 0 in a Maw water. Pinned so every committed
// anchor and host-checked salvage seed is 0 (the pin-carry rule).
int32_t bw_has_grasper(uint32_t seed);

// The rum-runner of a band: hull and battery reload (band 0 = the
// slice-2 sloop; main.c scales the ledger bar off the hull).
int32_t bw_band_enemy_hull(int32_t band);
int32_t bw_band_enemy_reload(int32_t band);

// Enemy sail AI: pure f(enemy, player). Intercept beyond BW_ENGAGE_RANGE
// (full sail, bow on), circle inside it (battle sail, hold the player
// abeam), fire the facing battery when aligned, ranged, and reloaded.
void bw_ai(const BwShip *e, const BwShip *p, BwInputs *out);

// One duel frame: player inputs + enemy AI + both ships step + broadsides
// spawn + balls fly + hits land + sink check (the sink frame drops the
// wreck's flotsam) + scoop pass. Sets d->over.
void bw_duel_step(BwDuel *d, const BwInputs *in);

// One salvage frame (after the enemy went under): the player sails on,
// lingering balls fly out (a late rake can still strike — sinking during
// salvage flips d->over to BW_DUEL_PLAYER_SUNK), the Maw stalks (slice
// 5: shadow telegraph -> surface -> lunge; a bite or a lingering rake
// can drag the salvage down), crates are scooped. In a grasper water
// (bestiary cut 1) the Grasper reaches instead of the Maw and can SEIZE
// and pin the sloop. Batteries stay cold while the water is quiet —
// they WAKE while a monster is up (no fire verb without a foe).
void bw_salvage_step(BwDuel *d, const BwInputs *in);

// Pier pass, run once per live-water frame by the caller: if the player
// lies alongside the Graywake pier with cargo aboard, the hold empties.
// Returns the gold banked this frame (0 if not docked or nothing held).
int32_t bw_dock_step(BwDuel *d);

// --- port + upgrades (slice 4) -----------------------------------------------

// The player sloop's max hull / battery reload at an upgrade tier, and
// the gold price of buying INTO a tier (tier 0 is free — the start ship).
int32_t bw_up_hull_max(int32_t tier);
int32_t bw_up_reload(int32_t tier);
int32_t bw_up_price(int32_t tier);

// Gold to repair the player hull to its tier max (0 = already sound).
// Prices BW_REPAIR_PER_GOLD missing points per gold, rounded UP.
int32_t bw_repair_cost(const BwDuel *d);

// Pure port ledger: attempt to buy menu row `row` (BW_PORT_ROW_*) with
// `gold` banked. Returns the gold actually spent — 0 means REFUSED
// (unknown row, tier already maxed, hull already sound, or gold short of
// the price; a refusal changes nothing). An upgrade row bumps the tier;
// the repair row refills the hull to its tier max.
int32_t bw_port_buy(BwDuel *d, int32_t row, int32_t gold);

// --- synthesized audio (slice 8) -----------------------------------------------

// Which ambience the water wants this frame: the Maw up (or shadowing)
// overrides everything; otherwise the weather is the drone ladder
// (wind_level clamped to 0..2). The caller gates on live water — the
// title/cards/port are silent and never ask.
uint32_t bw_amb_tier(int32_t maw_up, int32_t wind_level);

// Ambience drone row of a tier: frequency (Hz), duty (DutyCycle code
// 0..7), volume (0..127). Out-of-range tiers return the CALM row.
uint32_t bw_amb_freq(uint32_t tier);
uint32_t bw_amb_duty(uint32_t tier);
uint32_t bw_amb_vol(uint32_t tier);

// One-shot cue row of a cue id: frequency (Hz — the noise-generator
// rate for BW_CUE_ON_NOISE cues), length (frames the ARM9 holds the
// channel open), duty (DutyCycle code 0..7, or BW_CUE_ON_NOISE for the
// noise channel), volume (0..127), and freq2 (nonzero = the two-note
// contract: re-tune to this after len/2 frames — the dock chime).
// Pure lookups; BW_CUE_NONE and out-of-range ids are the all-zero row.
uint32_t bw_cue_freq(uint32_t cue);
uint32_t bw_cue_len(uint32_t cue);
uint32_t bw_cue_duty(uint32_t cue);
uint32_t bw_cue_vol(uint32_t cue);
uint32_t bw_cue_freq2(uint32_t cue);

// --- the Graywake ledger save (slice 9) ------------------------------------------

// Checksum of a save blob's words 0..BW_SAVE_WORDS-2 (a bw_hash fold on
// the BW_SAVE_SALT stream). Pure; word BW_SAVE_WORDS-1 is not read.
uint32_t bw_save_checksum(const uint32_t words[BW_SAVE_WORDS]);

// Serialize the ledger into a BW_SAVE_BYTES blob: { BW_SAVE_MAGIC,
// BW_SAVE_VERSION, gold, hull|cannon<<8|sail<<16, best_band, 0, 0,
// checksum }, little-endian words. Deterministic byte-for-byte.
void bw_save_encode(uint32_t gold, int32_t up_hull, int32_t up_cannon,
                    int32_t up_sail, uint32_t best_band,
                    uint8_t out[BW_SAVE_BYTES]);

// Deserialize a blob. Returns 1 and fills the outputs only for a
// well-formed record (magic, version, checksum, every tier < BW_UP_TIERS
// and band < BW_BANDS); returns 0 untouched on ANYTHING else — the
// caller keeps its fresh ledger.
int bw_save_decode(const uint8_t in[BW_SAVE_BYTES], uint32_t *gold,
                   int32_t *up_hull, int32_t *up_cannon,
                   int32_t *up_sail, uint32_t *best_band);

#endif // BW_SIM_H
