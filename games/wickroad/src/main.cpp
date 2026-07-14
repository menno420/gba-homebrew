/*
 * WICKROAD (game-lab Track B, breadth program game #11)
 *
 * Market-route trading loop, original IP — a NEW one-page concept
 * promoted to a playable prototype slice in the same session (the
 * Clockwork Courier #96 / Shoal #98 breadth grammar). Five towns on
 * one road, four goods, a pack that holds eight, and a 30-day clock:
 * reach 300 gold before the winter pass closes. THE HOOK is the
 * LEDGER: prices are only visible in the town you are standing in —
 * every other market shows what you wrote down the last time you were
 * there, stamped with how many days old the ink is. You navigate an
 * economy you remember, not one you see.
 *
 * PROTOTYPE RULES (deliberate cuts documented in CONCEPT.md):
 *   - Turn-based: nothing moves but on a key edge. A buys one unit of
 *     the cursor good, B sells one, L/R travel one town down the road
 *     (a day passes, 2 gold toll), SELECT waits a day (1 gold
 *     lodging, floored at 0 — the road never softlocks).
 *   - MARKET IMPACT: each unit you buy lifts that town's price of
 *     that good by 1 (cap +9); each unit you sell drops it by 1
 *     (cap -9); impact decays 1 toward 0 at every dawn. Cornering a
 *     market prices itself in — hammering A/B in place always loses
 *     gold (sell pays price-1).
 *   - Win the moment gold reaches 300; lose at the dawn of day 31.
 *     START on the title or an end card restarts the same seeded
 *     world.
 *
 * DETERMINISM CONTRACT (headless proof relies on all of this):
 *   - One xorshift32 PRNG at fixed seed 0x5749434B ('WICK'), consumed
 *     ONLY at world init, in a fixed order (base prices then phases).
 *   - Integer math only. Prices are a CLOSED FORM of (day, impact):
 *     price = clamp(base + tri12(day + phase) * step + impact),
 *     where tri12 is a triangle wave over a 12-day period — no
 *     per-frame simulation, no accumulated float drift, nothing.
 *   - The sim steps only on key edges latched that frame; the same
 *     input script replays bit-identically.
 *
 * Telemetry mailbox for the headless harness (tools/headless-screenshot.py
 * --elf --watch wr:wr_telemetry:16), volatile, C-linkage, every frame:
 *   [0] 0x5749434B 'WICK' magic     [8]  pack used (0..8)
 *   [1] 0x524F4144 'ROAD' magic     [9]  cargo TALLOW
 *   [2] state (0 title, 1 trading,  [10] cargo SALT
 *       2 ledger-balanced win,      [11] cargo IRON
 *       3 the pass closed)          [12] cargo RESIN
 *   [3] frames since boot           [13] live price of the cursor
 *   [4] day (1..30)                      good in the current town
 *   [5] gold                        [14] TRUE price of SALT at
 *   [6] town (0..4)                      THORNBY (the stale-ink
 *   [7] cursor good (0..3)               witness word)
 *                                   [15] ledger word for THORNBY
 *                                        SALT: (last-seen price << 8)
 *                                        | ink age in days (0 while
 *                                        THORNBY is unvisited)
 * Words 14/15 exist to make the hook itself assertable: the harness
 * pins frames where the ledger and the world DISAGREE (t14 != t15>>8
 * with age > 0) — the ink provably ages while the road moves on.
 *
 * Presentation is deliberately trivial (breadth-program slice): the
 * market table and the ledger are fixed-font glyph lines; headers,
 * title and end cards use the common variable 8x8 font (the one
 * --assert-text can read). All code original; fonts are Butano's
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
    // Headless telemetry mailbox — layout in the header comment.
    volatile unsigned wr_telemetry[16];
}

namespace
{
    enum state_t : unsigned
    {
        st_title = 0,
        st_trading = 1,
        st_balanced = 2,             // gold hit the target: the ledger
        st_closed = 3,               //   balances / the pass closed
    };

    constexpr unsigned seed_constant = 0x5749434Bu;  // 'WICK'; init-only

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

    // --- the road (all owner-tunable integers) ------------------------------
    constexpr int town_count = 5;
    constexpr int good_count = 4;

    constexpr const char* town_names[town_count] = {
        "EMBERTON", "GLASSMERE", "SALTCOMBE", "THORNBY", "DUNWICK",
    };

    constexpr const char* good_names[good_count] = {
        "TALLOW", "SALT", "IRON", "RESIN",
    };

    constexpr char good_letters[good_count] = {'T', 'S', 'I', 'R'};

    constexpr int price_lo[good_count] = {6, 10, 22, 14};
    constexpr int price_hi[good_count] = {26, 34, 60, 44};
    constexpr int drift_step[good_count] = {1, 2, 3, 2};
    constexpr int produce_gap[good_count] = {6, 8, 12, 10};

    constexpr int start_gold = 60;
    constexpr int gold_target = 300;
    constexpr int last_day = 30;
    constexpr int pack_capacity = 8;
    constexpr int travel_toll = 2;
    constexpr int lodging = 1;
    constexpr int impact_cap = 9;

    // The stale-ink witness pair (mailbox words 14/15).
    constexpr int witness_town = 3;      // THORNBY
    constexpr int witness_good = 1;      // SALT

    // Triangle wave over a 12-day period: 0..11 -> -3..3..-2 (integer,
    // closed-form — the whole "simulation" of the market).
    constexpr int tri12(int m)
    {
        m %= 12;
        return m <= 6 ? m - 3 : 9 - m;
    }

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

    void append_pad(bn::string<40>& text, const char* word, int width)
    {
        int len = 0;

        for(const char* c = word; *c; ++c)
        {
            text.append(*c);
            ++len;
        }

        for(; len < width; ++len)
        {
            text.append(' ');
        }
    }
}

int main()
{
    bn::core::init();

    // Candlelit backdrop: unmistakably not blank.
    bn::bg_palettes::set_transparent_color(bn::color(3, 2, 6));

    bn::sprite_text_generator map_gen(common::fixed_8x8_sprite_font);
    map_gen.set_left_alignment();
    bn::sprite_text_generator ui_gen(common::variable_8x8_sprite_font);
    ui_gen.set_left_alignment();

    constexpr int ui_x = -110;
    text_line ui_lines[4];               // headers / title / end cards
    text_line table_lines[good_count];   // the market table (fixed font)
    text_line ledger_lines[town_count - 1];  // the other four towns
    text_line title_lines[4];            // extra title/card rows

    auto clear_lines = [&]()
    {
        for(auto& line : ui_lines)
        {
            line.clear();
        }

        for(auto& line : table_lines)
        {
            line.clear();
        }

        for(auto& line : ledger_lines)
        {
            line.clear();
        }

        for(auto& line : title_lines)
        {
            line.clear();
        }
    };

    // --- world + run state (reset_run() restores the exact boot world) -----
    unsigned state = st_title;
    unsigned frames = 0;                 // monotonic since boot
    int base[town_count][good_count];
    int phase[town_count][good_count];
    int impact[town_count][good_count];
    int ledger[town_count][good_count];
    int ledger_day[town_count];          // 0 = never visited
    unsigned day = 0;
    int gold = 0;
    int town = 0;
    int cursor = 0;
    int cargo[good_count] = {0, 0, 0, 0};

    auto pack_used = [&]() -> int
    {
        return cargo[0] + cargo[1] + cargo[2] + cargo[3];
    };

    // The price law: a pure function of (town, good, day, impact).
    auto price = [&](int t, int g) -> int
    {
        int p = base[t][g] + tri12(int(day) + phase[t][g]) * drift_step[g]
              + impact[t][g];

        return p < price_lo[g] ? price_lo[g]
             : p > price_hi[g] ? price_hi[g] : p;
    };

    // Standing in a town, you SEE its market: today's prices go in the
    // ledger, in ink, dated today.
    auto refresh_ledger = [&]()
    {
        for(int g = 0; g < good_count; ++g)
        {
            ledger[town][g] = price(town, g);
        }

        ledger_day[town] = int(day);
    };

    auto reset_run = [&]()
    {
        // The SAME world every run: the PRNG restarts at the fixed
        // seed and is consumed only here, in a fixed order.
        rng_t rng(seed_constant);

        for(int t = 0; t < town_count; ++t)
        {
            for(int g = 0; g < good_count; ++g)
            {
                base[t][g] = (price_lo[g] + price_hi[g]) / 2 - 3
                           + rng.range(7);
            }
        }

        for(int t = 0; t < town_count; ++t)
        {
            for(int g = 0; g < good_count; ++g)
            {
                phase[t][g] = rng.range(12);
            }
        }

        // Every town produces one good cheap and craves another dear —
        // a fixed rotation, so profitable routes exist by construction.
        for(int t = 0; t < town_count; ++t)
        {
            base[t][t % good_count] -= produce_gap[t % good_count];
            base[t][(t + 2) % good_count] += produce_gap[(t + 2) % good_count];
        }

        for(int t = 0; t < town_count; ++t)
        {
            ledger_day[t] = 0;

            for(int g = 0; g < good_count; ++g)
            {
                impact[t][g] = 0;
                ledger[t][g] = 0;
            }
        }

        day = 1;
        gold = start_gold;
        town = 0;
        cursor = 0;

        for(int g = 0; g < good_count; ++g)
        {
            cargo[g] = 0;
        }

        refresh_ledger();
    };

    // A day passes: impact decays everywhere, and either the pass
    // closes or you wake up reading today's local prices.
    auto dawn = [&]()
    {
        ++day;

        for(int t = 0; t < town_count; ++t)
        {
            for(int g = 0; g < good_count; ++g)
            {
                if(impact[t][g] > 0)
                {
                    --impact[t][g];
                }
                else if(impact[t][g] < 0)
                {
                    ++impact[t][g];
                }
            }
        }

        if(int(day) > last_day)
        {
            state = st_closed;
            clear_lines();
            return;
        }

        refresh_ledger();
    };

    while(true)
    {
        bool start = bn::keypad::start_pressed();

        switch(state)
        {

        case st_title:
        case st_balanced:
        case st_closed:
            if(start)
            {
                reset_run();
                clear_lines();
                state = st_trading;
            }
            break;

        case st_trading:
        {
            // --- the verbs (turn-based: one key edge = one action) --------
            if(bn::keypad::up_pressed())
            {
                cursor = (cursor + good_count - 1) % good_count;
            }

            if(bn::keypad::down_pressed())
            {
                cursor = (cursor + 1) % good_count;
            }

            if(bn::keypad::a_pressed())      // BUY one unit
            {
                int cost = price(town, cursor);

                if(gold >= cost && pack_used() < pack_capacity)
                {
                    gold -= cost;
                    ++cargo[cursor];

                    if(impact[town][cursor] < impact_cap)
                    {
                        ++impact[town][cursor];
                    }

                    refresh_ledger();        // you watched it move
                }
            }

            if(bn::keypad::b_pressed())      // SELL one unit (pays price-1)
            {
                if(cargo[cursor] > 0)
                {
                    gold += price(town, cursor) - 1;
                    --cargo[cursor];

                    if(impact[town][cursor] > -impact_cap)
                    {
                        --impact[town][cursor];
                    }

                    refresh_ledger();

                    if(gold >= gold_target)
                    {
                        state = st_balanced;
                        clear_lines();
                    }
                }
            }

            if(state == st_trading && bn::keypad::l_pressed()
               && town > 0 && gold >= travel_toll)
            {
                gold -= travel_toll;
                --town;
                dawn();
            }

            if(state == st_trading && bn::keypad::r_pressed()
               && town < town_count - 1 && gold >= travel_toll)
            {
                gold -= travel_toll;
                ++town;
                dawn();
            }

            if(state == st_trading && bn::keypad::select_pressed())
            {
                gold = gold > lodging ? gold - lodging : 0;
                dawn();
            }

            break;
        }

        }

        // --- draw ------------------------------------------------------------

        switch(state)
        {

        case st_title:
            ui_lines[0].set(ui_gen, ui_x, -70, "WICKROAD");
            ui_lines[1].set(ui_gen, ui_x, -52, "ONE ROAD - FIVE MARKETS");
            ui_lines[2].set(ui_gen, ui_x, -40, "BUY LOW - SELL HIGH");
            ui_lines[3].set(ui_gen, ui_x, -28, "300 GOLD BEFORE DAY 30");
            title_lines[0].set(ui_gen, ui_x, -8, "YOUR LEDGER REMEMBERS");
            title_lines[1].set(ui_gen, ui_x, 4, "BUT THE INK AGES");
            title_lines[2].set(ui_gen, ui_x, 28, "PRESS START");
            title_lines[3].set(ui_gen, ui_x, 48,
                               "A BUY  B SELL  L/R GO  SEL WAIT");
            break;

        case st_trading:
        {
            bn::string<40> head("DAY ");
            head.append(bn::to_string<8>(day));
            head.append("/30  GOLD ");
            head.append(bn::to_string<8>(gold));
            ui_lines[0].set(ui_gen, ui_x, -70, head);

            bn::string<40> where("AT ");
            where.append(town_names[town]);
            where.append("  PACK ");
            where.append(bn::to_string<8>(pack_used()));
            where.append("/8");
            ui_lines[1].set(ui_gen, ui_x, -58, where);

            for(int g = 0; g < good_count; ++g)
            {
                bn::string<40> row;
                row.append(g == cursor ? '>' : ' ');
                append_pad(row, good_names[g], 7);
                row.append(bn::to_string<8>(price(town, g)));
                row.append("  x");
                row.append(bn::to_string<8>(cargo[g]));
                table_lines[g].set(map_gen, ui_x, -42 + 10 * g, row);
            }

            ui_lines[2].set(ui_gen, ui_x, 2, "LEDGER - THE INK AGES");

            int slot = 0;

            for(int t = 0; t < town_count; ++t)
            {
                if(t == town)
                {
                    continue;
                }

                bn::string<40> row;
                append_pad(row, town_names[t], 10);

                if(ledger_day[t] == 0)
                {
                    row.append("-- -- -- --");
                }
                else
                {
                    for(int g = 0; g < good_count; ++g)
                    {
                        row.append(good_letters[g]);
                        row.append(bn::to_string<8>(ledger[t][g]));
                        row.append(' ');
                    }

                    int age = int(day) - ledger_day[t];
                    row.append(bn::to_string<8>(age < 9 ? age : 9));
                    row.append('D');
                }

                ledger_lines[slot].set(map_gen, ui_x, 14 + 10 * slot, row);
                ++slot;
            }

            ui_lines[3].set(ui_gen, ui_x, 62,
                            "A BUY  B SELL  L/R GO  SEL WAIT");
            break;
        }

        case st_balanced:
        {
            ui_lines[0].set(ui_gen, ui_x, -60, "THE LEDGER BALANCES");
            bn::string<40> line1("GOLD ");
            line1.append(bn::to_string<8>(gold));
            ui_lines[1].set(ui_gen, ui_x, -40, line1);
            bn::string<40> line2("DAY ");
            line2.append(bn::to_string<8>(day));
            line2.append(" OF 30");
            ui_lines[2].set(ui_gen, ui_x, -28, line2);
            ui_lines[3].set(ui_gen, ui_x, -16, "THE ROAD MADE YOU");
            title_lines[0].set(ui_gen, ui_x, 8, "PRESS START");
            break;
        }

        case st_closed:
        {
            ui_lines[0].set(ui_gen, ui_x, -60, "THE PASS CLOSES");
            bn::string<40> line1("GOLD ");
            line1.append(bn::to_string<8>(gold));
            line1.append(" OF 300");
            ui_lines[1].set(ui_gen, ui_x, -40, line1);
            ui_lines[2].set(ui_gen, ui_x, -28, "30 DAYS SPENT");
            ui_lines[3].set(ui_gen, ui_x, -16, "WINTER TAKES THE ROAD");
            title_lines[0].set(ui_gen, ui_x, 8, "PRESS START");
            break;
        }

        }

        // --- telemetry mailbox: every slot, every frame ---------------------

        ++frames;
        wr_telemetry[0] = 0x5749434Bu;   // 'WICK'
        wr_telemetry[1] = 0x524F4144u;   // 'ROAD'
        wr_telemetry[2] = state;
        wr_telemetry[3] = frames;
        wr_telemetry[4] = day;
        wr_telemetry[5] = unsigned(gold);
        wr_telemetry[6] = unsigned(town);
        wr_telemetry[7] = unsigned(cursor);
        wr_telemetry[8] = unsigned(pack_used());
        wr_telemetry[9] = unsigned(cargo[0]);
        wr_telemetry[10] = unsigned(cargo[1]);
        wr_telemetry[11] = unsigned(cargo[2]);
        wr_telemetry[12] = unsigned(cargo[3]);
        wr_telemetry[13] = state == st_title ? 0u
                                             : unsigned(price(town, cursor));
        wr_telemetry[14] = state == st_title ? 0u
                                             : unsigned(price(witness_town,
                                                              witness_good));
        wr_telemetry[15] = ledger_day[witness_town] == 0 ? 0u
            : unsigned((ledger[witness_town][witness_good] << 8)
                       | ((int(day) - ledger_day[witness_town]) & 0xFF));

        bn::core::update();
    }
}
