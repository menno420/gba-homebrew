// Underroot — arc slice 1: DUAL-SCREEN SKELETON.
//
// The two screens ARE the game (docs/arcs/UNDERROOT.md):
//   TOP    = the meadow. A placeholder grass field with the hawk shadow
//            sweeping across it on the DETERMINISTIC pure schedule
//            ur_hawk_x/ur_hawk_active/ur_hawk_y = f(seed, season, index)
//            (ur_sim.c). Slice 1 runs one fixed seed (UR_SEED), fixed
//            season (spring), so the boot proof can assert exact values.
//   BOTTOM = the burrow cross-section. A tile-snap dig grid: hold the
//            stylus to paint DUG cells (a drawn tunnel), soil elsewhere.
//            The burrow is a GRAPH — ur_burrow_size BFS-counts the cells
//            reachable from the entrance mouth, so a disconnected pocket is
//            visibly NOT part of the colony (the slice-3 forager-pathing
//            seed).
//
// Everything the sim decides is pure (ur_sim.c) and mirrored host-side
// (tools/check-underroot.py); this file is libnds boot + render + input +
// the telemetry mailbox only. The mailbox is a fixed array of words at the
// exported symbol `ur_telemetry`, rewritten every frame, so a headless
// DeSmuME run (tools/nds-headless-check.py) can ELF-resolve it and assert
// the pure schedule and the dig accounting against these very functions.

#include <nds.h>
#include <stdio.h>

#include "ur_sim.h"

// The telemetry mailbox — external linkage so the ELF carries the symbol.
volatile uint32_t ur_telemetry[UR_T_WORDS];

static PrintConsole top_console;
static PrintConsole bottom_console;

// --- meadow (top screen): grass + the deterministic hawk shadow ------------
static const char *season_name(uint32_t season)
{
    switch (season)
    {
    case UR_SPRING: return "spring";
    case UR_SUMMER: return "summer";
    case UR_AUTUMN: return "autumn";
    default:        return "winter";
    }
}

static void draw_meadow(uint32_t frame, int hawk_on, int32_t hawk_x,
                        int32_t hawk_y, uint32_t dug, uint32_t con,
                        const ur_patch_t *patches, uint32_t food_total,
                        ur_forage_t forage, uint32_t store, uint32_t cap,
                        uint32_t pop, uint32_t exposed, uint32_t lost,
                        uint32_t surv, uint32_t season, uint32_t day,
                        uint32_t abund, uint32_t sfood, uint32_t wstore,
                        uint32_t wsurv, uint32_t wscore)
{
    consoleSelect(&top_console);
    consoleClear();
    printf("\x1b[0;0HUNDERROOT  the meadow");
    // The year clock (slice 7): the live season name + the day the frame clock
    // is on. Season advances SPRING->SUMMER->AUTUMN->WINTER on the pure counter.
    printf("\x1b[1;0H%s  day %lu  frame %lu",
           season_name(season), (unsigned long)day, (unsigned long)frame);

    // A placeholder grass field, rows 3..15 (the hawk lane band y 24..119).
    for (int row = 3; row < 16; row++)
    {
        printf("\x1b[%d;0H", row);
        for (int col = 0; col < 32; col++)
            printf(",");
    }

    // Food patches (slice 2): a '*' at each patch's meadow cell, drawn on the
    // foraging apron BELOW the hawk lane band (console rows 15+). Positions
    // are the pure f(seed, season, index) ur_patch schedule. The nearest
    // REACHABLE patch (slice 3) — the one the forager routes to — is marked
    // 'o' instead, so the drawn tunnel's consequence is visible.
    for (int i = 0; i < UR_PATCH_COUNT; i++)
    {
        int pc = patches[i].x / 8;       // px -> 8px console column
        int pr = patches[i].y / 8;       // px -> 8px console row
        if (pc < 0) pc = 0; else if (pc > 31) pc = 31;
        if (pr < 15) pr = 15; else if (pr > 23) pr = 23;
        char glyph = (forage.index != UR_ROUTE_NONE &&
                      (uint32_t)i == forage.index) ? 'o' : '*';
        printf("\x1b[%d;%dH%c", pr, pc, glyph);
    }

    // The hawk shadow: one glyph at its pure-scheduled meadow cell.
    if (hawk_on)
    {
        int hc = hawk_x / 8;             // px -> 8px console column
        int hr = hawk_y / 8;             // px -> 8px console row
        if (hc < 0) hc = 0; else if (hc > 31) hc = 31;
        if (hr < 3) hr = 3; else if (hr > 15) hr = 15;
        printf("\x1b[%d;%dHV", hr, hc);
        printf("\x1b[17;0Hhawk x=%ld", (long)hawk_x);
    }
    else
    {
        printf("\x1b[17;0Hsky clear");
    }

    // Meadow food total (all patches) and the colony's BANKED store / granary
    // capacity (slice 4): the reachable haul banked up to the connected
    // granaries' capacity — unreachable granaries bank nothing.
    // The connected nurseries convert the banked store into POP new foragers
    // (slice 5): shown alongside the meadow food and the banked store.
    printf("\x1b[18;0Hfood %lu  store %lu/%lu  pop %lu",
           (unsigned long)food_total, (unsigned long)store, (unsigned long)cap,
           (unsigned long)pop);
    // Forager route (slice 3): the nearest reachable patch and its round-trip
    // route length over the drawn tunnel graph, or a prompt to dig one open.
    if (forage.index != UR_ROUTE_NONE)
        printf("\x1b[19;0Hforager -> patch %lu route %lu",
               (unsigned long)forage.index, (unsigned long)forage.route);
    else
        printf("\x1b[19;0Hno forage route (dig to a patch)");
    printf("\x1b[20;0Hburrow  dug %lu  con %lu",
           (unsigned long)dug, (unsigned long)con);
    // Hawk predation (slice 6): the exposed (shallow) cells on the forager
    // route, the foragers the hawks catch there, and the survivors that carry
    // into winter. A deeper route exposes fewer cells and loses fewer foragers.
    printf("\x1b[21;0Hhawk  exposed %lu  lost %lu  surv %lu",
           (unsigned long)exposed, (unsigned long)lost, (unsigned long)surv);
    // The year clock's economy (slice 7): the meadow abundance scale for the
    // live season (a numerator over UR_ABUND_UNIT — plentiful summer, thinning
    // autumn, ZERO winter) and the seasonal meadow haul it scales the reachable
    // food to.
    printf("\x1b[22;0Hseason  abund %lu/%d  food %lu",
           (unsigned long)abund, UR_ABUND_UNIT, (unsigned long)sfood);
    // The winter survival exam (slice 8): the store carried into winter (banked
    // minus the brood the nurseries ate), whether the colony SURVIVES the drain,
    // and the survival SCORE it chases. Zero meadow food in winter, so this is
    // the whole game's verdict — clears winter or starves on the drawn tunnels.
    printf("\x1b[23;0Hwinter  store %lu  %s  score %lu",
           (unsigned long)wstore, wsurv ? "SURVIVE" : "starve",
           (unsigned long)wscore);
}

// --- burrow (bottom screen): the tile-snap dig grid ------------------------
static void draw_burrow(const uint8_t grid[UR_CELLS], const uint8_t gran[UR_CELLS],
                        const uint8_t nurs[UR_CELLS])
{
    consoleSelect(&bottom_console);
    consoleClear();
    printf("\x1b[0;0HTHE BURROW  cross-section");
    printf("\x1b[1;0H#soil .dug Ggran Nnurs Mmouth");

    // The grid, one char per cell, drawn from console row 3, col 4. A
    // designated nursery cell (slice 5) reads 'N'; a designated granary cell
    // (slice 4) reads 'G'; a plain dug cell '.'.
    for (int r = 0; r < UR_GRID_H; r++)
    {
        printf("\x1b[%d;4H", r + 3);
        for (int c = 0; c < UR_GRID_W; c++)
        {
            int idx = r * UR_GRID_W + c;
            char ch;
            if (c == UR_ENTRANCE_COL && r == UR_ENTRANCE_ROW)
                ch = 'M';
            else if (grid[idx] && nurs[idx])
                ch = 'N';
            else if (grid[idx] && gran[idx])
                ch = 'G';
            else
                ch = grid[idx] ? '.' : '#';
            printf("%c", ch);
        }
    }
}

static uint32_t dug_total(const uint8_t grid[UR_CELLS])
{
    uint32_t n = 0;
    for (int i = 0; i < UR_CELLS; i++)
        n += grid[i];
    return n;
}

// --- slice-9 backup I/O (bounded card-SPI EEPROM read + page program) ----------
// The record's ONLY two backup touches: one read at power-on, one page program
// on a commit that improves the record. Both are the standard SPI-EEPROM command
// flows (READ 0x03 / WREN 0x06 / PAGE PROGRAM 0x02 / RDSR 0x05 with
// UR_SAVE_EEPROM_TYPE-2 = 2-byte addressing), hand-rolled instead of libnds's
// cardReadEeprom/cardWriteEeprom for two measured reasons carried verbatim from
// the Gloamline slice-9 precedent:
//
// 1. CHIP-SELECT DISCIPLINE. The AUXSPI protocol releases the chip select AFTER
//    the next byte once CARD_SPI_HOLD (bit 6) is dropped — so a command must
//    clear HOLD BEFORE its last byte. libnds "deselects" by writing 0x0040 (bit
//    6 stays set): real chips tolerate it, but DeSmuME's backup device (the
//    proof carrier) never sees a select edge, keeps the previous command open,
//    and swallows every later byte as read traffic — the record write lands
//    NOWHERE and the unbounded WIP poll then hangs the ROM.
// 2. BOUNDED WAITS. Every wait below gives up after UR_SAVE_POLL_BOUND
//    iterations — a chip (or emulator) that never answers costs a bounded
//    sub-frame stall on one boot/commit frame, never a hang. An expired program
//    poll is harmless: the page data is already clocked in and the chip finishes
//    programming internally; the next backup command is a power cycle away.

static void save_spi_byte(uint8_t b)
{
    REG_AUXSPIDATA = b;
    for (uint32_t i = 0;
         i < UR_SAVE_POLL_BOUND && (REG_AUXSPICNT & CARD_SPI_BUSY); i++) { }
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

static void save_read_backup(uint8_t blob[UR_SAVE_BYTES])
{
    save_select();
    save_spi_byte(0x03);                     // READ + 2 address bytes
    save_spi_byte((UR_SAVE_ADDR >> 8) & 0xFF);
    save_spi_byte(UR_SAVE_ADDR & 0xFF);
    for (int i = 0; i < UR_SAVE_BYTES - 1; i++)
        blob[i] = save_spi_read();
    save_final_byte();
    blob[UR_SAVE_BYTES - 1] = save_spi_read();
}

static void save_write_backup(const uint8_t blob[UR_SAVE_BYTES])
{
    // WRITE ENABLE — its own one-byte chip-select window
    save_select();
    save_final_byte();
    save_spi_byte(0x06);
    // PAGE PROGRAM: UR_SAVE_BYTES is exactly one 32-byte page at UR_SAVE_ADDR 0,
    // so no page-crossing loop is needed.
    save_select();
    save_spi_byte(0x02);
    save_spi_byte((UR_SAVE_ADDR >> 8) & 0xFF);
    save_spi_byte(UR_SAVE_ADDR & 0xFF);
    for (int i = 0; i < UR_SAVE_BYTES - 1; i++)
        save_spi_byte(blob[i]);
    save_final_byte();
    save_spi_byte(blob[UR_SAVE_BYTES - 1]);  // CS falls: the chip programs
    // READ STATUS: wait out the program cycle — BOUNDED
    save_select();
    save_spi_byte(0x05);
    for (uint32_t i = 0;
         i < UR_SAVE_POLL_BOUND && (save_spi_read() & 0x01); i++) { }
    save_final_byte();
    save_spi_read();                         // terminating dummy byte
}

int main(void)
{
    videoSetMode(MODE_0_2D);
    videoSetModeSub(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    vramSetBankC(VRAM_C_SUB_BG);

    consoleInit(&top_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0,
                true /* main engine */, true);
    consoleInit(&bottom_console, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0,
                false /* sub engine */, true);

    uint8_t grid[UR_CELLS];
    ur_fresh_grid(grid);
    // The granary designation layer (slice 4): a parallel 0/1 mask over the
    // dig grid, nothing designated at boot.
    uint8_t gran[UR_CELLS];
    ur_fresh_gran(gran);
    // The nursery designation layer (slice 5): a parallel 0/1 mask over the
    // dig grid, nothing designated at boot. Connected nurseries convert the
    // granary-banked store into population.
    uint8_t nurs[UR_CELLS];
    ur_fresh_nurs(nurs);

    // The seed dial (slice 10): the runtime-selectable year seed. The dial rests
    // at position 0 (== UR_SEED, so the boot skeleton is the pinned meadow); the
    // player scans it with LEFT/RIGHT to pick a different year. run_seed is the
    // dialed seed that feeds the WHOLE f(seed,season,index) schedule below. The
    // reference-plan fairness witness (ur_seed_fair / ur_ref_score) is recomputed
    // ONLY on a dial change (never per frame) — it is a bounded burrow BFS sweep,
    // fine as a one-shot on a button press but not worth every vblank.
    uint32_t dial = 0;
    uint32_t run_seed = ur_dial_seed(dial);
    uint32_t ref_score = ur_ref_score(run_seed);
    uint32_t seed_fair = ref_score > 0 ? 1u : 0u;

    // The meadow's food patches: pure f(seed, season, index). Recomputed each
    // frame for the LIVE season (slice 7's year clock), so the meadow's patch
    // layout and total food advance with the seasons. At boot (frame 0 -> day 0
    // -> spring) this is the slice-2 pinned spring meadow.
    ur_patch_t patches[UR_PATCH_COUNT];
    for (int i = 0; i < UR_PATCH_COUNT; i++)
        patches[i] = ur_patch(run_seed, UR_SPRING, (uint32_t)i);
    uint32_t food_total = ur_patch_total(run_seed, UR_SPRING);

    uint32_t frame = 0;
    int32_t last_col = -1;
    int32_t last_row = -1;
    bool pad_seen_idle = false;          // KEYINPUT boot-trap guard (PL note)
    bool burrow_dirty = true;            // redraw the grid on next frame

    // The best-score record (slice 9): read the backup ONCE at power-on and
    // decode it. A corrupt / blank / future-version blob decodes to 0 and the
    // fresh all-zero table stands — never a crash, never a hang. The record then
    // persists across power cycles; a run commits it (a discrete START press)
    // ONLY when it strictly improves. UR_SAVE_EEPROM_TYPE is the assumed
    // decide-and-flag addressing constant, not a boot-time chip probe.
    uint32_t best_score = 0;
    uint32_t best_season = 0;
    uint32_t best_seed = 0;
    uint32_t save_ok = 0;
    uint32_t save_writes = 0;
    uint8_t save_blob[UR_SAVE_BYTES];
    save_read_backup(save_blob);
    save_ok = (uint32_t)ur_save_decode(save_blob, &best_score, &best_season,
                                       &best_seed);

    ur_telemetry[UR_T_MAGIC0] = 0x554E4452u;   // 'UNDR'
    ur_telemetry[UR_T_MAGIC1] = 0x524F4F54u;   // 'ROOT'
    ur_telemetry[UR_T_GW] = UR_GRID_W;
    ur_telemetry[UR_T_GH] = UR_GRID_H;
    ur_telemetry[UR_T_PATCHN] = UR_PATCH_COUNT;
    ur_telemetry[UR_T_PATCHSUM] = food_total;
    ur_telemetry[UR_T_SPARE] = 0;

    draw_burrow(grid, gran, nurs);

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        frame++;

        uint32_t held = keysHeld();
        uint32_t down = keysDown();
        // Don't trust the pad (incl. KEY_TOUCH) until it has read idle once:
        // an emulator whose KEYINPUT boots all-pressed must not phantom-dig
        // at frame 1 (the py-desmume boot-trap, docs/PLATFORM-LIMITS.md).
        if (held == 0)
            pad_seen_idle = true;

        int touch_now = 0;
        if (pad_seen_idle && (held & KEY_TOUCH))
        {
            touchPosition touch = {0};
            touchRead(&touch);
            int32_t col, row;
            if (ur_cell_of_touch(touch.px, touch.py, &col, &row))
            {
                touch_now = 1;
                last_col = col;
                last_row = row;
                // The stylus verbs: hold L and tap a DUG cell to designate it a
                // NURSERY (slice 5); hold R for a GRANARY (slice 4); a bare tap
                // digs (slice 1). All are idempotent, and all redraw the burrow
                // on a change.
                if (held & KEY_L)
                {
                    if (ur_nurse(grid, nurs, (uint32_t)col, (uint32_t)row))
                        burrow_dirty = true;
                }
                else if (held & KEY_R)
                {
                    if (ur_designate(grid, gran, col, row))
                        burrow_dirty = true;
                }
                else if (ur_dig(grid, col, row))
                    burrow_dirty = true;
            }
        }

        // The seed dial (slice 10): LEFT/RIGHT scan the year seed. A discrete
        // d-pad edge steps the dial one position (wrapping over UR_DIAL_COUNT);
        // the dialed run seed re-feeds the whole schedule from this frame on, and
        // the reference-plan fairness witness is recomputed for the new seed (a
        // one-shot BFS sweep, only on the change). Guarded by pad_seen_idle so a
        // boot-all-pressed emulator cannot phantom-scan at frame 1 (the KEYINPUT
        // boot-trap note). LEFT/RIGHT are otherwise unused, so this never
        // collides with the stylus verbs or the START commit.
        if (pad_seen_idle && (down & (KEY_LEFT | KEY_RIGHT)))
        {
            if (down & KEY_RIGHT)
                dial = (dial + 1u) % (uint32_t)UR_DIAL_COUNT;
            if (down & KEY_LEFT)
                dial = (dial + (uint32_t)UR_DIAL_COUNT - 1u)
                       % (uint32_t)UR_DIAL_COUNT;
            run_seed = ur_dial_seed(dial);
            ref_score = ur_ref_score(run_seed);
            seed_fair = ref_score > 0 ? 1u : 0u;
        }

        // The year clock (slice 7): the day the frame counter is on and the
        // season that day falls in. EVERYTHING below reads this LIVE season
        // (the meadow, the patches, the forage/economy chain) rather than a
        // pinned spring — so the whole world advances through the year. At the
        // frames the slices 1-6 proofs assert (all within spring day 0) this is
        // season UR_SPRING, so every pinned value is bit-identical to before.
        uint32_t day = ur_day(frame);
        uint32_t season = ur_season_of_day(day);
        // The live meadow: patches move with the season (f(seed, season, index)).
        for (int i = 0; i < UR_PATCH_COUNT; i++)
            patches[i] = ur_patch(run_seed, season, (uint32_t)i);
        food_total = ur_patch_total(run_seed, season);

        int hawk_on = ur_hawk_active(frame);
        int32_t hawk_x = ur_hawk_x(run_seed, season, frame);
        int32_t hawk_y = ur_hawk_y(run_seed, season, frame);
        uint32_t dug = dug_total(grid);
        uint32_t con = ur_burrow_size(grid);
        // The forager route (slice 3): the nearest reachable patch over the
        // drawn tunnel graph — pure f(dig plan, seed, season).
        ur_forage_t forage = ur_forage(grid, run_seed, season);
        // The food store (slice 4): connected granary cells, their capacity,
        // and the reachable meadow haul banked up to it — pure f(dig plan,
        // gran plan, seed, season).
        uint32_t gran_n = ur_gran_count(grid, gran);
        uint32_t gran_con = ur_gran_connected(grid, gran);
        uint32_t cap = ur_gran_capacity(grid, gran);
        uint32_t store = ur_store(grid, gran, run_seed, season);
        // The population (slice 5): connected nurseries brood the banked store
        // into new foragers on a pure bounded schedule — pure f(dig plan, gran
        // plan, nurs plan, seed, season).
        uint32_t nurs_n = ur_nurs_count(grid, nurs);
        uint32_t nurs_con = ur_nurs_connected(grid, nurs);
        uint32_t pop = ur_pop(grid, gran, nurs, run_seed, season);
        uint32_t pop_food = ur_pop_food(grid, gran, nurs, run_seed, season);
        // Hawk predation (slice 6): the hawks catch foragers on the EXPOSED
        // (shallow) cells of the forager route; deep cells are safe. Pure
        // f(dig plan, gran plan, nurs plan, seed, season).
        uint32_t exposed = ur_forage_exposed(grid, run_seed, season);
        uint32_t lost = ur_predation(grid, gran, nurs, run_seed, season);
        uint32_t surv = ur_survivors(grid, gran, nurs, run_seed, season);
        // The year clock's economy (slice 7): the meadow abundance scale, the
        // seasonal haul it scales the reachable food to, and the per-hawk-pass
        // predation over the whole season (the slice-6-deferred temporal scale).
        uint32_t abund = ur_abundance(season);
        uint32_t sfood = ur_season_food(grid, run_seed, season);
        uint32_t hawk_pass = ur_hawk_passes(season);
        uint32_t spred = ur_season_predation(grid, gran, nurs, run_seed, season);
        uint32_t ssurv = ur_season_survivors(grid, gran, nurs, run_seed, season);
        // The winter survival exam (slice 8): the store carried into winter (the
        // banked granary store minus the brood the nurseries drew), the foragers
        // carried in (the season survivors), the winter drain that appetite
        // demands, and the SURVIVE verdict + SCORE. Pure f(dig plan, gran plan,
        // nurs plan, seed, season) — the arc's headline number.
        uint32_t wstore = ur_winter_store(grid, gran, nurs, run_seed, season);
        uint32_t wpop = ssurv;
        uint32_t wdrain = ur_winter_drain(wpop);
        int wsurv = ur_winter_survives(wstore, wpop);
        uint32_t wleft = ur_winter_leftover(wstore, wpop);
        uint32_t wscore = ur_winter_score(wstore, wpop);

        // The best-score record commit (slice 9 + slice-10 winter gate): pressing
        // START banks the run's survival SCORE and season into the persistent
        // record — but ONLY when the live season is WINTER, and ONLY when it
        // strictly improves (a higher score OR a further season). The WINTER gate
        // (slice 10, honoring slice-9's forward note) is the correctness half:
        // slice 8 evaluates the winter exam as a live forecast in the growing
        // seasons, so a spring START would bank a PROJECTED number the run never
        // realized; gating the commit to season == UR_WINTER — the one season the
        // exam IS the realized outcome — means the persisted best is a score the
        // colony actually reached. A discrete player action + a strict-improve
        // gate is the EEPROM wear discipline: one bounded page program per
        // improving winter commit, never per frame, never on a tie, never in a
        // growing season. (Guarded by pad_seen_idle so a boot-all-pressed emulator
        // cannot phantom-commit at frame 1 — the same KEYINPUT boot-trap note.)
        if (pad_seen_idle && (down & KEY_START) && season == UR_WINTER
            && ur_record_improves(best_score, best_season, wscore, season))
        {
            if (wscore > best_score)
                best_score = wscore;
            if (season > best_season)
                best_season = season;
            best_seed = run_seed;
            ur_save_encode(best_score, best_season, best_seed, save_blob);
            save_write_backup(save_blob);
            save_writes++;
        }

        // Top screen animates every frame (the hawk sweeps); the burrow is
        // redrawn only when a dig changed it (frame-budget discipline).
        draw_meadow(frame, hawk_on, hawk_x, hawk_y, dug, con, patches,
                    food_total, forage, store, cap, pop, exposed, lost, surv,
                    season, day, abund, sfood, wstore, (uint32_t)wsurv, wscore);
        // The persisted best-score record (slice 9): the headline number the
        // colony chases, carried across power cycles. START banks an improving
        // run. (Drawn after draw_meadow's consoleClear on the free right of the
        // title row.)
        consoleSelect(&top_console);
        printf("\x1b[0;22Hbest %lu", (unsigned long)best_score);
        // The seed dial (slice 10): the picked year seed and whether it is a fair
        // (survivable) year — scan LEFT/RIGHT to pick another. Drawn on row 2
        // (above the grass field) so the player sees which year they are playing.
        printf("\x1b[2;0Hseed %lu (dial %lu/%d) %s",
               (unsigned long)run_seed, (unsigned long)dial, UR_DIAL_COUNT,
               seed_fair ? "fair" : "harsh");
        if (burrow_dirty)
        {
            draw_burrow(grid, gran, nurs);
            burrow_dirty = false;
        }

        ur_telemetry[UR_T_FRAME] = frame;
        ur_telemetry[UR_T_SEASON] = season;    // LIVE season (slice 7 year clock)
        ur_telemetry[UR_T_SEED] = run_seed;
        ur_telemetry[UR_T_HAWKON] = (uint32_t)hawk_on;
        ur_telemetry[UR_T_HAWKX] = hawk_on ? (uint32_t)hawk_x : 0;
        ur_telemetry[UR_T_HAWKY] = hawk_on ? (uint32_t)hawk_y : 0;
        ur_telemetry[UR_T_DUG] = dug;
        ur_telemetry[UR_T_CON] = con;
        ur_telemetry[UR_T_TCOL] = (uint32_t)last_col;
        ur_telemetry[UR_T_TROW] = (uint32_t)last_row;
        ur_telemetry[UR_T_TOUCH] = (uint32_t)touch_now;
        ur_telemetry[UR_T_ROUTEI] = forage.index;
        ur_telemetry[UR_T_ROUTED] = forage.dist;
        ur_telemetry[UR_T_ROUTELEN] = forage.route;
        ur_telemetry[UR_T_GRANN] = gran_n;
        ur_telemetry[UR_T_GRANCON] = gran_con;
        ur_telemetry[UR_T_CAP] = cap;
        ur_telemetry[UR_T_STORE] = store;
        ur_telemetry[UR_T_NURSN] = nurs_n;
        ur_telemetry[UR_T_NURSCON] = nurs_con;
        ur_telemetry[UR_T_POP] = pop;
        ur_telemetry[UR_T_POPFOOD] = pop_food;
        ur_telemetry[UR_T_EXPOSED] = exposed;
        ur_telemetry[UR_T_LOST] = lost;
        ur_telemetry[UR_T_SURV] = surv;
        ur_telemetry[UR_T_DAY] = day;
        ur_telemetry[UR_T_ABUND] = abund;
        ur_telemetry[UR_T_SFOOD] = sfood;
        ur_telemetry[UR_T_HAWKPASS] = hawk_pass;
        ur_telemetry[UR_T_SPRED] = spred;
        ur_telemetry[UR_T_SSURV] = ssurv;
        ur_telemetry[UR_T_WSTORE] = wstore;
        ur_telemetry[UR_T_WPOP] = wpop;
        ur_telemetry[UR_T_WDRAIN] = wdrain;
        ur_telemetry[UR_T_WSURV] = (uint32_t)wsurv;
        ur_telemetry[UR_T_WLEFT] = wleft;
        ur_telemetry[UR_T_WSCORE] = wscore;
        // The best-score record (slice 9): the live persisted table + the
        // power-on decode flag + the writes this power-on + the format version.
        ur_telemetry[UR_T_BEST] = best_score;
        ur_telemetry[UR_T_BESTSEASON] = best_season;
        ur_telemetry[UR_T_BESTSEED] = best_seed;
        ur_telemetry[UR_T_SAVEOK] = save_ok;
        ur_telemetry[UR_T_SAVEWR] = save_writes;
        ur_telemetry[UR_T_SAVEVER] = UR_SAVE_VERSION;
        // The seed dial + balance (slice 10): the live dial position, the dialed
        // run seed feeding the schedule, and the reference-plan fairness witness
        // (is this seed survivable? and its reference winter score) recomputed on
        // the last dial change — the ROM-side confirmation of the host balance
        // proof that no dialed seed is a death-trap.
        ur_telemetry[UR_T_DIAL] = dial;
        ur_telemetry[UR_T_RUNSEED] = run_seed;
        ur_telemetry[UR_T_FAIR] = seed_fair;
        ur_telemetry[UR_T_REFSCORE] = ref_score;
        ur_telemetry[UR_T_SPARE] = 0;
    }

    return 0;
}
