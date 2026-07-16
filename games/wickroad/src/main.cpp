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
 * GROWTH CUT 1 — RUMORS (v0.2, CONCEPT.md growth cut 1): town criers
 * telegraph FUTURE price shocks days ahead ("CRIER: IRON 58 AT
 * GLASSMERE DAY 5") and the shock then actually happens, to the exact gold
 * piece foretold — the ledger becomes a forecasting tool, not just a
 * memory. The deck is FIXED and deterministic by construction: no new
 * RNG draws (the v0.1 world init is untouched), and the shock is a
 * term of the same closed-form price law — a pure function of the
 * day. The foretold price is computed FROM the sim law (base +
 * drift at the hit day + shock, clamped), so telegraph == realization
 * whenever the player's own market impact there is zero; proofs.sh P4
 * pins exactly that.
 *
 * GROWTH CUT 2 — CONTRACTS (v0.3, CONCEPT.md growth cut 2): dated
 * delivery orders ("PACT? 2 RESIN TO THORNBY D8 +60") — a second
 * income verb that PRICES RISK. A fixed authored deck, exactly the
 * rumor-deck pattern: zero new RNG draws and zero price-law changes
 * (delivery bypasses the market — goods leave the pack, the premium
 * lands, prices and impact untouched), so the world stays
 * bit-identical and P1-P4 carry verbatim. RIGHT (the d-pad, unused
 * until now) is the pact verb: it takes the posted contract, and
 * hands the goods over at the destination on or before the deadline.
 * Miss the deadline and the pact simply LAPSES (no forfeit — the
 * simple rule, deliberately). The pact line's regeneration is
 * deferred one frame FURTHER than the crier's (the second quiet frame
 * after a head redraw): dawn frames are budget-SPENT (the v0.2
 * measured lesson) and the crier owns the first quiet frame.
 *
 * GROWTH CUT 3 — THE ROAD ITSELF (v0.4, CONCEPT.md growth cut 3):
 * per-leg hazards (bandits, weather) and a guard-hire decision, so
 * travel cost stops being flat. A fixed authored deck, exactly the
 * rumor/contract-deck pattern: zero new RNG draws, and a hazard is a
 * pure function of (leg, arrival day) — the world stays bit-identical
 * and P1-P5 carry verbatim (every hazard window is authored AFTER day
 * 14; the committed routes' last travel day is 13). BANDIT stretches
 * seize an authored cut of gold from an unguarded crossing (floored
 * at 0, like lodging); STORM stretches cost an unprovisioned crossing
 * a whole extra day (a second dawn, plus 1 gold camp lodging) — days
 * are the real currency, so weather and bandits price differently.
 * LEFT (the last unused d-pad key) HIRES THE GUARD: a flat
 * outfit_fee buys guard + provisions for the NEXT crossing, hazard or
 * not (the fee is spent either way — hiring against a quiet road is
 * the bad bet it sounds like). Every hazard is telegraphed on the new
 * ROAD line from its authored announce day, days before the window
 * opens, so the hire is an informed decision, not a coin flip. The
 * ROAD line regenerates on the THIRD quiet frame after a head redraw
 * (the crier owns the first, the pact line the second — dawn frames
 * are budget-SPENT, the #142/#143 measured lesson); in the trading
 * screen it takes the row the static verb-help line used to occupy
 * (the full verb help stays on the title card), so the always-on
 * sprite load is a wash.
 *
 * GROWTH CUT 4 — A WIDER MAP + PACK UPGRADES (v0.5, CONCEPT.md growth
 * cut 4): the road runs on past DUNWICK to two new markets —
 * HOLLOWFEN (the drovers' fair) and MIRGATE (the far iron source) —
 * and gold buys logistics: at the Hollowfen fair START buys a MULE,
 * each mule growing the pack by 4 (8 -> 12 -> 16) at a fixed authored,
 * escalating price (30 then 55; the fair posts the next price on the
 * town line, 0 mules ridable past two). The classic Taipan curve:
 * early profits convert into carrying capacity, capacity converts
 * distance into bigger hauls. The new towns run the SAME closed-form
 * price law and produce/crave rotation (HOLLOWFEN sells salt cheap and
 * craves resin; MIRGATE sells iron cheap and craves tallow), and their
 * ledger ink ages exactly like the old five — the hook stretches with
 * the map. RNG DELTA, counted: +16 world-init draws (2 towns x 4
 * goods x base+phase), appended STRICTLY AFTER the v0.4 stream (the
 * five old towns' 40 draws keep their exact order), so the old world
 * is bit-identical and every committed pin (P1-P6) carries verbatim.
 * The map stays ONE road, deliberately (no branch fork): L/R is a
 * committed verb grammar and a junction would need a new travel verb —
 * documented as the honest cut in CONCEPT.md. Frame-budget note: dawn
 * frames are budget-SPENT (the #142-#144 measured lesson), so the two
 * NEW ledger rows regenerate on quiet frames 4 and 5 after a head
 * redraw (crier 1, pact 2, road 3 — the stagger extended); the mule
 * price rides the existing town/pack line, so the always-on line
 * count only grows by the two deferred ledger rows.
 *
 * GROWTH CUT 5 — AUDIO (v0.6, CONCEPT.md growth cut 5, the concept's
 * LAST named cut): three original synthesized cues (audio/
 * generate_audio.py — the Lumen Drift/Deepcast/Courier/Shoal house
 * pattern: stdlib-only, deterministic, no samples ever) played through
 * Butano's maxmod pipeline as pure DECISIONS on events the sim already
 * computed: a COIN CHINK when gold changes hands on a trade (market
 * buy, market sell, a pact delivery payment — tolls/lodging/fees are
 * costs, not trades, and stay silent), a DAWN BELL when a day rolls
 * over, and the PASS-CLOSING WIND when winter takes the road (the
 * dawn that would land past day 30). Nothing feeds back into the sim:
 * zero new RNG draws, zero price-law changes, the world and words
 * 0-47 of the mailbox stay byte-identical, so P1-P7 carry verbatim.
 * Every trigger bumps a cumulative hook counter into the new mailbox
 * words 48-51 (the house hook-count method, seated in wr_telemetry
 * the way cut 4 appended words) so the headless harness pins the
 * decisions; the counters count from BOOT (like the frame word) and
 * deliberately survive a START restart — proofs.sh P8 pins exactly
 * that, plus maxmod mixer-memory activity for the voicing.
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
 * --elf --watch wr:wr_telemetry:56; the v0.1/v0.2/v0.3/v0.4/v0.5/v0.6 proofs
 * keep watching the first 16/24/32/40/48/52 words unchanged), volatile,
 * C-linkage, every frame:
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
 * Rumor witness words (growth cut 1; all 0 on the title, and 0 while
 * no rumor has been announced yet):
 *   [16] latest announced rumor id  [20] FORETOLD price (the number
 *        (1-based; 0 = none)             the crier shouts — the sim
 *   [17] its hit (target) day            law at the hit day, shock
 *   [18] its target town                 in, impact-free)
 *   [19] its target good            [21] live TRUE price at the
 *                                        rumor's (town, good) — the
 *                                        realization witness
 *                                   [22] shock currently applied
 *                                        there (u32 cast; negative
 *                                        deltas read as 2^32+delta)
 *                                   [23] rumor phase: 1 telegraphed
 *                                        (day < hit), 2 landing (in
 *                                        the shock window), 3 passed
 * Words 20/21 make THE FORECAST assertable: the harness pins a frame
 * where the rumor is displayed BEFORE the shock (t21 != t20, phase 1)
 * and the hit-day frame where the market obeys it EXACTLY (t21 == t20,
 * phase 2) — planning against rumors is provably informative.
 *
 * Contract witness words (growth cut 2; all 0 on the title, and 0
 * while no contract has been posted yet; words 24-30 describe the
 * LATEST posted contract — windows are disjoint by authorship, so
 * that is the only live one):
 *   [24] latest posted contract id  [28] quantity
 *        (1-based; 0 = none)        [29] deadline day
 *   [25] its lifecycle state:       [30] premium (the whole payout)
 *        1 offered, 2 accepted,     [31] cumulative premium gold
 *        3 paid, 4 lapsed                paid across ALL contracts
 *   [26] its good                        this run
 *   [27] its destination town
 * Words 25/31 make THE SECOND INCOME VERB assertable: the harness pins
 * the offer (state 1) before any player action, the acceptance edge
 * (1 -> 2), the delivery frame where gold jumps by EXACTLY the premium
 * (state 3, word 31 += premium), and the lapse dawn (state 4, word 31
 * unchanged) — the premium provably prices the risk taken, or not.
 *
 * Hazard witness words (growth cut 3; all 0 on the title, and 0 while
 * no hazard has been announced yet; words 32-36 describe the LATEST
 * announced hazard, the latest_rumor pattern):
 *   [32] latest announced hazard   [36] window last day
 *        id (1-based; 0 = none)    [37] outfitted flag: 1 while a
 *   [33] its kind: 0 bandits,           hired guard rides the next
 *        1 storm                        crossing, else 0
 *   [34] its leg (between town     [38] gold seized by bandits this
 *        leg and leg+1)                 run, cumulative
 *   [35] window first day          [39] days lost to storms this
 *        (arrival-day check)            run, cumulative
 * Words 37/38/39 make THE ROAD DECISION assertable: the harness pins
 * the unguarded crossing losing EXACTLY the authored seizure (word 38
 * jumps, gold drops in the same frame), the guarded crossing of the
 * SAME stretch losing nothing (37: 1 -> 0, 38 unchanged), the
 * unprovisioned storm costing exactly one day (39 += 1, day += 2 on
 * one travel edge) and the provisioned one costing none — the fee
 * provably buys what it claims, or is provably wasted.
 *
 * Wider-map witness words (growth cut 4; all 0 on the title):
 *   [40] pack capacity (8 + 4 per  [44] TRUE price of IRON at
 *        mule)                          MIRGATE (the new-region
 *   [41] mules owned (0..2)            stale-ink witness word)
 *   [42] mule price posted HERE    [45] ledger word for MIRGATE
 *        (0 unless standing at the      IRON: (last-seen price << 8)
 *        Hollowfen fair with a          | ink age in days (0 while
 *        mule still buyable)            MIRGATE is unvisited)
 *   [43] gold spent on mules,      [46] visited-towns bitmask (bit
 *        cumulative                     t set once town t is stood
 *                                       in; 0x7F = all seven)
 *                                  [47] free pack space (capacity
 *                                       minus used)
 * Words 40/47 make THE UPGRADE assertable: the harness pins a buy that
 * pushes pack used PAST the old cap of 8, the purchase edges where
 * capacity jumps 8 -> 12 -> 16 and word 42 walks the authored price
 * ladder to 0 (sold out), and word 46 reaching 0x7F — all seven
 * markets stood in on one committed route. Words 44/45 replay the
 * cut-0 hook on the NEW region: MIRGATE's ink provably ages too.
 *
 * Audio hook-count words (growth cut 5; cumulative play-call counts
 * since BOOT, like the frame word [3] — deliberately NOT reset by a
 * START restart, and not zeroed on the title, so a restart provably
 * keeps the boot's audio history):
 *   [48] wr_coin plays (a trade:    [50] wr_wind plays (the pass
 *        market buy, market sell,        closed)
 *        pact delivery payment)     [51] total plays
 *   [49] wr_dawn plays (a day           ([48] + [49] + [50])
 *        rolled over, pass still
 *        open)
 * Words 48-51 make THE CUE DECISIONS assertable: the harness pins the
 * coin count stepping on exactly the buy/sell edges, the bell count
 * matching the dawns crossed, the wind firing ONCE on the pass-close
 * frame, and all four words surviving a START restart — while a
 * mixer-memory nonzero watch proves maxmod actually voiced them.
 * Actual audible output (mix levels, timbre, the DAC) is NOT
 * headlessly assertable — that is named owner-playtest territory.
 *
 * Crossroads witness words (crossroads cut 1; all 0 on the title):
 *   [52] on-branch flag: 1 while    [54] TRUE price of WYRMHOLLOW's
 *        standing on WYRMHOLLOW           produce good (RESIN) — the
 *        (town 7), else 0                 branch-town price witness,
 *   [53] fork-edge id: branch_leg         so the fork market is pinned
 *        (7) while a fork is live         from the mirror even from
 *        from the current town (at        across the spine
 *        the junction DUNWICK or on  [55] free / reserved (0)
 *        WYRMHOLLOW), else 0
 * Words 52/53 make THE FORK verb assertable: the harness pins the flag
 * flipping 0 -> 1 on the L+R chord that rides onto the branch and back
 * to 0 on the return, and word 53 reading 7 exactly at the two ends of
 * the fork road (junction + branch) and 0 elsewhere — the branch is
 * reachable ONLY by the chord, never by a spine R. Word 54 pins the
 * branch market against the host mirror. Words 0-51 are byte-unchanged.
 *
 * Presentation is deliberately trivial (breadth-program slice): the
 * market table and the ledger are fixed-font glyph lines; headers,
 * title and end cards use the common variable 8x8 font (the one
 * --assert-text can read). All code original; fonts are Butano's
 * zlib-licensed common assets (third_party/butano/common).
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

#include "common_fixed_8x8_sprite_font.h"
#include "common_variable_8x8_sprite_font.h"

extern "C"
{
    // Headless telemetry mailbox — layout in the header comment.
    volatile unsigned wr_telemetry[56];
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
    constexpr int town_count = 8;        // widened to 8 (crossroads cut 1:
                                         //   +WYRMHOLLOW, the off-spine
                                         //   branch town — was 7 in v0.5/0.6)
    constexpr int spine_town_count = 7;  // the LINEAR spine 0<->1<->...<->6
                                         //   that L/R walk (crossroads cut 1);
                                         //   town 7 hangs off it, fork-only —
                                         //   the spine travel/display bounds
                                         //   use this, not town_count, so R at
                                         //   MIRGATE never falls onto the branch
    constexpr int legacy_town_count = 5; // the v0.4 world: its 40 world-init
                                         //   draws keep their exact stream
                                         //   order (see reset_run)
    constexpr int good_count = 4;

    constexpr const char* town_names[town_count] = {
        "EMBERTON", "GLASSMERE", "SALTCOMBE", "THORNBY", "DUNWICK",
        "HOLLOWFEN", "MIRGATE", "WYRMHOLLOW",
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

    // --- the CROSSROADS (crossroads cut 1) ----------------------------------
    // The map is no longer a bare linear index. A tiny static adjacency
    // table names, per town, its spine-left / spine-right neighbours
    // (-1 = none) and its FORK neighbour (the off-spine town reached by
    // the L+R chord; -1 = none). L/R read the .left/.right columns and
    // still walk the spine 0<->1<->...<->6 BIT-IDENTICALLY (the spine
    // legs and their hazards are unchanged); only WYRMHOLLOW (7) hangs
    // off the junction DUNWICK (4), reachable ONLY by the fork verb, and
    // R at DUNWICK still follows the spine to HOLLOWFEN.
    constexpr int junction_town = 4;     // DUNWICK: the mid-spine fork
    constexpr int branch_town = 7;       // WYRMHOLLOW: the off-spine child
    constexpr int branch_leg = spine_town_count;  // 7: a leg id the hazard
                                         //   deck never uses, so the fork
                                         //   road carries no bandits/storm

    struct adj_t { int left; int right; int fork; };
    constexpr adj_t adjacency[town_count] = {
        { -1,  1, -1 },   // 0 EMBERTON  (spine start)
        {  0,  2, -1 },   // 1 GLASSMERE
        {  1,  3, -1 },   // 2 SALTCOMBE
        {  2,  4, -1 },   // 3 THORNBY
        {  3,  5,  7 },   // 4 DUNWICK    (the junction: fork -> WYRMHOLLOW)
        {  4,  6, -1 },   // 5 HOLLOWFEN
        {  5, -1, -1 },   // 6 MIRGATE    (spine end)
        {  4, -1,  4 },   // 7 WYRMHOLLOW (branch: left/fork -> DUNWICK)
    };

    // --- the MULES (growth cut 4) --------------------------------------------
    // Gold buys logistics: at the Hollowfen fair START buys a mule,
    // each mule growing the pack by mule_gain. The price ladder is
    // FIXED and authored (like every deck before it): the fair posts
    // the next price on the town line, and past mule_limit it simply
    // sells out. The new-region stale-ink witness pair (words 44/45)
    // replays the cut-0 hook on MIRGATE's iron — the far market's ink
    // ages exactly like the old five towns'.
    constexpr int stable_town = 5;       // HOLLOWFEN, the drovers' fair
    constexpr int mule_limit = 2;
    constexpr int mule_gain = 4;         // pack: 8 -> 12 -> 16
    constexpr int mule_price[mule_limit] = {30, 55};

    constexpr int wide_witness_town = 6; // MIRGATE
    constexpr int wide_witness_good = 2; // IRON

    // --- the RUMOR deck (growth cut 1) --------------------------------------
    // FIXED and deterministic by construction: no RNG is consumed (the
    // v0.1 world-init consumption order is untouched, so the world is
    // bit-identical), and each shock is a term of the same closed-form
    // price law — a pure function of the day, active for rumor_window
    // days from the hit day. Announce/display is a pure function of the
    // day too (the latest announced rumor), so nothing here adds state.
    struct rumor_t
    {
        int announce_day;    // the criers start shouting at this dawn
        int hit_day;         // the shock lands at this dawn...
        int town;
        int good;
        int delta;           // ...adding this inside the clamped price law
    };

    constexpr int rumor_count = 3;
    constexpr int rumor_window = 3;      // shock days: hit_day..hit_day+2

    // Deck order = announce order (ids are 1-based positions). Targets
    // deliberately avoid the committed P2/P3 routes' pinned markets.
    constexpr rumor_t rumor_deck[rumor_count] = {
        {2, 5, 1, 2, 12},    // R1: iron soars at GLASSMERE by day 5
        {8, 12, 0, 1, -6},   // R2: salt crashes at EMBERTON by day 12
        {14, 18, 4, 3, 9},   // R3: resin soars at DUNWICK by day 18
    };

    // --- the CONTRACT deck (growth cut 2) -----------------------------------
    // Dated delivery orders — the second income verb. FIXED and
    // authored like the rumor deck: no RNG is consumed and no
    // price-law term is added (delivery bypasses the market entirely:
    // goods leave the pack, the premium lands, prices and impact
    // untouched), so the world stays bit-identical and the committed
    // proofs carry verbatim. The premium PRICES RISK, by authorship:
    // it beats the destination's spot value of the goods by a margin
    // that grows with the haul (tolls + days of capital tied up) and
    // the tightness of the deadline window. Offer windows are disjoint
    // (each offer_day > the previous deadline_day), so at most one
    // contract is live at a time — the pact line and the witness words
    // track the latest posted one.
    struct contract_t
    {
        int offer_day;       // posted at this dawn
        int good;
        int town;            // destination
        int qty;
        int deadline_day;    // hand over ON or BEFORE this day...
        int premium;         // ...for this payout (the whole payment)
    };

    constexpr int contract_count = 2;

    constexpr contract_t contract_deck[contract_count] = {
        // C1: 2 RESIN to THORNBY by day 8, pays 60 — spot value there
        // is ~40 (2 x ~21, minus the sell fee); the margin prices a
        // 3-leg haul from the EMBERTON start and a 7-day window.
        {2, 3, 3, 2, 8, 60},
        // C2: 2 IRON to GLASSMERE by day 14, pays 70 — iron runs ~26
        // at its SALTCOMBE source, two legs out; the fatter margin
        // prices a tighter 4-day window.
        {10, 2, 1, 2, 14, 70},
    };

    // --- the HAZARD deck (growth cut 3) --------------------------------------
    // The road itself: per-leg hazards, so travel cost stops being
    // flat. FIXED and authored like the rumor and contract decks: no
    // RNG is consumed, and whether a crossing is hazarded is a pure
    // function of (leg, ARRIVAL day) — the day the dawn of the ride
    // lands you on the far side. Every window opens AFTER day 14 (the
    // committed P2/P3/P5 routes' last travel day is 13), so the world
    // and every committed pin stay bit-identical. Kinds price
    // differently on purpose: BANDITS tax gold (an authored seizure,
    // floored at 0 like lodging), STORMS tax the clock (one extra dawn
    // camped in the pass, 1 gold lodging) — and days are the win
    // condition's real currency. One outfit_fee guard-hire covers the
    // NEXT crossing against either kind, hazard or not: hiring against
    // a quiet road wastes the fee, so the ROAD line's advance warning
    // (announce_day, days before the window) is what makes the hire a
    // decision instead of a coin flip.
    struct hazard_t
    {
        int announce_day;    // the ROAD line starts warning at this dawn
        int from_day;        // hazard window: ARRIVAL day in
        int to_day;          //   [from_day, to_day], inclusive
        int leg;             // the road between town leg and leg+1
        int kind;            // 0 = bandits, 1 = storm
        int seizure;         // bandits: gold seized per unguarded
                             //   crossing (storms take a day instead)
    };

    constexpr int hazard_count = 3;
    constexpr int outfit_fee = 4;        // LEFT: guard + provisions,
                                         //   consumed by the next ride

    constexpr hazard_t hazard_deck[hazard_count] = {
        // H1: bandits on the GLASSMERE road, a tight two-day window —
        // the cheap lesson: seizure 12 vs the 4-gold guard.
        {13, 15, 16, 0, 0, 12},
        // H2: a storm over the THORNBY road, four days — the clock
        // tax: an unprovisioned crossing loses a whole day.
        {17, 19, 22, 2, 1, 0},
        // H3: bandits on the DUNWICK road, late and fat — priced so
        // an end-game iron run has to budget for it.
        {23, 25, 28, 3, 0, 15},
    };

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
    text_line ledger_lines[town_count - 1];  // the other six towns
    text_line title_lines[5];            // extra title/card rows
    text_line pact_line;                 // the contract line (cut 2)
    text_line road_line;                 // the hazard line (cut 3)

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

        pact_line.clear();
        road_line.clear();
    };

    // --- world + run state (reset_run() restores the exact boot world) -----
    unsigned state = st_title;
    unsigned frames = 0;                 // monotonic since boot
    unsigned head_quiet = 0;             // frames since the head line
                                         // last changed (regen stagger)
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
    bool contract_accepted[contract_count] = {false, false};
    bool contract_paid[contract_count] = {false, false};
    bool outfitted = false;              // a hired guard rides the
                                         //   next crossing (cut 3)
    int hazard_gold_lost = 0;            // bandit seizures, cumulative
    int storm_days_lost = 0;             // storm delays, cumulative
    int mules = 0;                       // pack upgrades owned (cut 4)
    int mule_spend = 0;                  // gold spent on mules, cumulative
    unsigned visited_mask = 0;           // bit t set once town t stood in

    // Audio hook counters (growth cut 5): cumulative play-call counts
    // since BOOT, like `frames` — deliberately NOT reset by reset_run
    // (a START restart provably keeps the boot's audio history; P8
    // pins exactly that). The cues are pure outputs: nothing here is
    // read back by the sim.
    unsigned audio_coin = 0;             // wr_coin: gold changed hands
    unsigned audio_dawn = 0;             // wr_dawn: a day rolled over
    unsigned audio_wind = 0;             // wr_wind: the pass closed

    // A trade completed — gold changed hands (market buy, market sell,
    // a pact delivery payment): the coin chink. Tolls, lodging, fees
    // and the mule fair are costs, not trades — deliberately silent.
    auto chink = [&]()
    {
        bn::sound_items::wr_coin.play(bn::fixed(0.6));
        ++audio_coin;
    };

    auto pack_used = [&]() -> int
    {
        return cargo[0] + cargo[1] + cargo[2] + cargo[3];
    };

    // The pack grows with the mule train (growth cut 4): 8 -> 12 -> 16.
    auto capacity = [&]() -> int
    {
        return pack_capacity + mule_gain * mules;
    };

    // What the Hollowfen fair posts: the next mule's authored price, 0
    // anywhere else on the road (or once the fair is sold out).
    auto stable_price = [&]() -> int
    {
        return town == stable_town && mules < mule_limit
                   ? mule_price[mules] : 0;
    };

    // The rumor shock at (t, g) today: a pure function of the day.
    auto shock = [&](int t, int g) -> int
    {
        int total = 0;

        for(const rumor_t& r : rumor_deck)
        {
            if(r.town == t && r.good == g && int(day) >= r.hit_day
               && int(day) < r.hit_day + rumor_window)
            {
                total += r.delta;
            }
        }

        return total;
    };

    // The price law: a pure function of (town, good, day, impact) —
    // the rumor shock is one more closed-form term inside the clamp.
    auto price = [&](int t, int g) -> int
    {
        int p = base[t][g] + tri12(int(day) + phase[t][g]) * drift_step[g]
              + impact[t][g] + shock(t, g);

        return p < price_lo[g] ? price_lo[g]
             : p > price_hi[g] ? price_hi[g] : p;
    };

    // What the crier promises: the price law at the hit day, shock in,
    // impact-free — pure in the init state, so telegraph == realization
    // exactly when the player's own impact there is zero at the hit day.
    auto foretell = [&](const rumor_t& r) -> int
    {
        int p = base[r.town][r.good]
              + tri12(r.hit_day + phase[r.town][r.good])
                * drift_step[r.good]
              + r.delta;

        return p < price_lo[r.good] ? price_lo[r.good]
             : p > price_hi[r.good] ? price_hi[r.good] : p;
    };

    // The crier's current story: the latest announced rumor (1-based id;
    // 0 = none yet) — a pure function of the day, like everything else.
    auto latest_rumor = [&]() -> int
    {
        int id = 0;

        for(int i = 0; i < rumor_count; ++i)
        {
            if(int(day) >= rumor_deck[i].announce_day)
            {
                id = i + 1;
            }
        }

        return id;
    };

    // The latest posted contract (1-based id; 0 = none yet) — a pure
    // function of the day, exactly the latest_rumor pattern.
    auto current_contract = [&]() -> int
    {
        int id = 0;

        for(int i = 0; i < contract_count; ++i)
        {
            if(int(day) >= contract_deck[i].offer_day)
            {
                id = i + 1;
            }
        }

        return id;
    };

    // The latest announced hazard (1-based id; 0 = none yet) — a pure
    // function of the day, exactly the latest_rumor pattern.
    auto latest_hazard = [&]() -> int
    {
        int id = 0;

        for(int i = 0; i < hazard_count; ++i)
        {
            if(int(day) >= hazard_deck[i].announce_day)
            {
                id = i + 1;
            }
        }

        return id;
    };

    // Contract lifecycle, DERIVED — the only stored contract state is
    // the two player flags, everything else is a function of the day:
    // 0 unposted, 1 offered, 2 accepted, 3 paid, 4 lapsed (the
    // deadline passed unpaid — accepted or not; the simple lapse rule,
    // no forfeit, deliberately).
    auto contract_state = [&](int i) -> int
    {
        if(contract_paid[i])
        {
            return 3;
        }

        if(int(day) > contract_deck[i].deadline_day)
        {
            return 4;
        }

        if(contract_accepted[i])
        {
            return 2;
        }

        if(int(day) >= contract_deck[i].offer_day)
        {
            return 1;
        }

        return 0;
    };

    // Premium gold banked across all paid contracts this run —
    // derived from the paid flags, nothing stored.
    auto premium_paid_total = [&]() -> int
    {
        int total = 0;

        for(int i = 0; i < contract_count; ++i)
        {
            if(contract_paid[i])
            {
                total += contract_deck[i].premium;
            }
        }

        return total;
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
        // seed and is consumed only here, in a fixed order. THE ORDER
        // IS A COMMITTED INTERFACE (growth cut 4, extended crossroads
        // cut 1): the five legacy towns draw base then phase EXACTLY
        // as in v0.1-v0.4 (40 draws), the two wider-map towns (5-6)
        // draw their 16 (base then phase) APPENDED after that, and the
        // branch town (7 = WYRMHOLLOW) draws its 8 (4 base + 4 phase,
        // mirroring town 6's pattern) APPENDED strictly after town 6 —
        // each delta counted, the old stream order frozen, so towns
        // 0-6's world is bit-identical and P1-P8 carry verbatim.
        rng_t rng(seed_constant);

        for(int t = 0; t < legacy_town_count; ++t)
        {
            for(int g = 0; g < good_count; ++g)
            {
                base[t][g] = (price_lo[g] + price_hi[g]) / 2 - 3
                           + rng.range(7);
            }
        }

        for(int t = 0; t < legacy_town_count; ++t)
        {
            for(int g = 0; g < good_count; ++g)
            {
                phase[t][g] = rng.range(12);
            }
        }

        for(int t = legacy_town_count; t < spine_town_count; ++t)
        {
            for(int g = 0; g < good_count; ++g)
            {
                base[t][g] = (price_lo[g] + price_hi[g]) / 2 - 3
                           + rng.range(7);
            }
        }

        for(int t = legacy_town_count; t < spine_town_count; ++t)
        {
            for(int g = 0; g < good_count; ++g)
            {
                phase[t][g] = rng.range(12);
            }
        }

        // The branch town (crossroads cut 1): +8 draws, appended after
        // town 6 — 4 base then 4 phase, mirroring town 6's pattern.
        // THE FREEZE POINT: any future off-spine town appends its draws
        // strictly after this, never in between (the wire-format rule).
        for(int g = 0; g < good_count; ++g)
        {
            base[branch_town][g] = (price_lo[g] + price_hi[g]) / 2 - 3
                                 + rng.range(7);
        }

        for(int g = 0; g < good_count; ++g)
        {
            phase[branch_town][g] = rng.range(12);
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

        for(int i = 0; i < contract_count; ++i)
        {
            contract_accepted[i] = false;
            contract_paid[i] = false;
        }

        outfitted = false;
        hazard_gold_lost = 0;
        storm_days_lost = 0;
        mules = 0;
        mule_spend = 0;
        visited_mask = 1u;               // EMBERTON, the starting square

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

            // The pass-closing wind (growth cut 5): the one loss cue,
            // fired on the exact dawn winter takes the road.
            bn::sound_items::wr_wind.play(bn::fixed(0.8));
            ++audio_wind;

            clear_lines();
            return;
        }

        // The dawn bell (growth cut 5): a day rolled over and the road
        // is still open. The play call is a fixed-cost maxmod channel
        // start, not text work — the dawn frame's committed draw
        // budget (the #142-#144 measured lesson) is untouched.
        bn::sound_items::wr_dawn.play(bn::fixed(0.5));
        ++audio_dawn;

        refresh_ledger();
    };

    // Growth cut 3: resolve the road just ridden. Called right after
    // a travel dawn, with `day` already the ARRIVAL day and `town` the
    // far side; `leg` is the road between town leg and leg+1. The
    // hired guard (if any) is consumed by the crossing whether the
    // road bites or not — hiring against a quiet road wastes the fee,
    // which is exactly what makes the ROAD line's warning worth
    // reading. At most one deck hazard matches any (leg, day): the
    // authored windows sit on distinct legs.
    auto cross = [&](int leg)
    {
        bool guarded = outfitted;
        outfitted = false;

        if(state != st_trading)          // the pass closed on the
        {                                //   travel dawn itself
            return;
        }

        for(const hazard_t& h : hazard_deck)
        {
            if(h.leg != leg || int(day) < h.from_day
               || int(day) > h.to_day || guarded)
            {
                continue;
            }

            if(h.kind == 0)              // bandits: the gold tax,
            {                            //   floored at 0 like lodging
                int seized = gold > h.seizure ? h.seizure : gold;
                gold -= seized;
                hazard_gold_lost += seized;
            }
            else                         // storm: the clock tax — one
            {                            //   day camped in the pass
                gold = gold > lodging ? gold - lodging : 0;
                ++storm_days_lost;
                dawn();
            }
        }
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

                if(gold >= cost && pack_used() < capacity())
                {
                    gold -= cost;
                    ++cargo[cursor];

                    if(impact[town][cursor] < impact_cap)
                    {
                        ++impact[town][cursor];
                    }

                    refresh_ledger();        // you watched it move
                    chink();                 // gold changed hands
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
                    chink();                 // gold changed hands

                    if(gold >= gold_target)
                    {
                        state = st_balanced;
                        clear_lines();
                    }
                }
            }

            // The travel verbs read the adjacency table (crossroads
            // cut 1). A ride is the same everywhere: pay the toll, move,
            // mark visited, dawn, then resolve the leg's hazards. The
            // leg id is the LOWER town index for a spine leg (so a spine
            // L/R ride is byte-identical to v0.6 — min(4,3)=3 is the
            // (3,4) leg, min(3,4)=3 the same), or branch_leg for a fork
            // ride (the fork road carries no deck hazard).
            auto ride = [&](int dest)
            {
                int leg = (int(town) == branch_town || dest == branch_town)
                        ? branch_leg
                        : (int(town) < dest ? int(town) : dest);
                gold -= travel_toll;
                town = unsigned(dest);
                visited_mask |= 1u << unsigned(town);
                dawn();
                cross(leg);
            };

            // L: walk the spine left (or ride the branch back down to the
            // junction). Guarded against the chord frame so L+R never
            // also fires a single move — byte-identical, since no
            // committed route presses both shoulders on one frame.
            if(state == st_trading && bn::keypad::l_pressed()
               && !bn::keypad::r_pressed()
               && adjacency[town].left >= 0 && gold >= travel_toll)
            {
                ride(adjacency[town].left);
            }

            // R: walk the spine right — never onto the branch (the fork
            // column is separate), so R at the junction still goes to
            // HOLLOWFEN and R at MIRGATE (spine end, .right == -1) does
            // nothing, exactly as the old `town < town_count - 1` bound.
            if(state == st_trading && bn::keypad::r_pressed()
               && !bn::keypad::l_pressed()
               && adjacency[town].right >= 0 && gold >= travel_toll)
            {
                ride(adjacency[town].right);
            }

            // THE FORK VERB (crossroads cut 1): L+R chorded on one frame —
            // "take the fork". Purely additive (no committed route chords
            // the shoulders): from the junction it rides ONTO the branch
            // (WYRMHOLLOW), from the branch it rides back to the junction.
            // Same toll + dawn() + cross(leg) as any spine ride.
            if(state == st_trading && bn::keypad::l_pressed()
               && bn::keypad::r_pressed()
               && adjacency[town].fork >= 0 && gold >= travel_toll)
            {
                ride(adjacency[town].fork);
            }

            if(state == st_trading && bn::keypad::select_pressed())
            {
                gold = gold > lodging ? gold - lodging : 0;
                dawn();
            }

            // The pact verb (growth cut 2): RIGHT on the d-pad (unused
            // until now) TAKES the posted contract, and HANDS OVER the
            // goods at the destination on or before the deadline —
            // the premium is the whole payment (delivery bypasses the
            // market: no price, no impact, no ledger change).
            if(state == st_trading && bn::keypad::right_pressed())
            {
                int cid = current_contract();

                if(cid > 0)
                {
                    const contract_t& c = contract_deck[cid - 1];
                    int cs = contract_state(cid - 1);

                    if(cs == 1)
                    {
                        contract_accepted[cid - 1] = true;
                    }
                    else if(cs == 2 && town == c.town
                            && cargo[c.good] >= c.qty)
                    {
                        cargo[c.good] -= c.qty;
                        gold += c.premium;
                        contract_paid[cid - 1] = true;
                        chink();             // the premium is a trade:
                                             //   gold changed hands

                        if(gold >= gold_target)
                        {
                            state = st_balanced;
                            clear_lines();
                        }
                    }
                }
            }

            // The road verb (growth cut 3): LEFT hires the guard —
            // outfit_fee gold buys guard + provisions for the NEXT
            // crossing, against bandits and storms alike. One hire at
            // a time; the crossing consumes it, hazard or not.
            if(state == st_trading && bn::keypad::left_pressed()
               && ! outfitted && gold >= outfit_fee)
            {
                gold -= outfit_fee;
                outfitted = true;
            }

            // The fair verb (growth cut 4): START — free in the
            // trading state; every committed route only presses it on
            // the title and end cards — BUYS A MULE at the Hollowfen
            // fair, at the posted authored price. Each mule grows the
            // pack by mule_gain; past mule_limit the fair is sold out
            // and START goes back to doing nothing.
            if(state == st_trading && start && stable_price() > 0
               && gold >= stable_price())
            {
                gold -= stable_price();
                mule_spend += stable_price();
                ++mules;
            }

            break;
        }

        }

        // --- draw ------------------------------------------------------------

        switch(state)
        {

        case st_title:
            ui_lines[0].set(ui_gen, ui_x, -70, "WICKROAD");
            ui_lines[1].set(ui_gen, ui_x, -54, "ONE ROAD - SEVEN MARKETS");
            ui_lines[2].set(ui_gen, ui_x, -42, "BUY LOW - SELL HIGH");
            ui_lines[3].set(ui_gen, ui_x, -30, "300 GOLD BEFORE DAY 30");
            title_lines[0].set(ui_gen, ui_x, -14, "YOUR LEDGER REMEMBERS");
            title_lines[1].set(ui_gen, ui_x, -2, "BUT THE INK AGES");
            title_lines[2].set(ui_gen, ui_x, 12, "PRESS START");
            title_lines[3].set(ui_gen, ui_x, 26,
                               "A BUY  B SELL  L/R GO  SEL WAIT");
            pact_line.set(ui_gen, ui_x, 40, "RIGHT TAKES A PACT");
            road_line.set(ui_gen, ui_x, 52, "LEFT HIRES THE GUARD");
            title_lines[4].set(ui_gen, ui_x, 64,
                               "THE HOLLOWFEN FAIR SELLS MULES");
            break;

        case st_trading:
        {
            bn::string<40> head("DAY ");
            head.append(bn::to_string<8>(day));
            head.append("/30  GOLD ");
            head.append(bn::to_string<8>(gold));

            // Dawn frames regenerate the whole table/ledger and ride the
            // frame budget's edge (a v0.1-measured fact the committed
            // pins depend on) — so the crier line defers its own regen
            // to the NEXT frame, which does no other text work. Purely a
            // function of the input history: determinism is untouched.
            bool head_changed = !(ui_lines[0].cached == head)
                                || ui_lines[0].dirty_clear;

            head_quiet = head_changed ? 0 : head_quiet + 1;

            ui_lines[0].set(ui_gen, ui_x, -70, head);

            // The town/pack line carries the fair's posted mule price
            // (growth cut 4): it already regenerates on every dawn and
            // purchase frame, so the extra words cost no NEW regen
            // frames — the budget-friendly seat for the new number.
            bn::string<40> where("AT ");
            where.append(town_names[town]);
            where.append("  PACK ");
            where.append(bn::to_string<8>(pack_used()));
            where.append('/');
            where.append(bn::to_string<8>(capacity()));

            if(int fair = stable_price(); fair > 0)
            {
                where.append("  MULE ");
                where.append(bn::to_string<8>(fair));
            }

            ui_lines[1].set(ui_gen, ui_x, -58, where);

            for(int g = 0; g < good_count; ++g)
            {
                bn::string<40> row;
                row.append(g == cursor ? '>' : ' ');
                append_pad(row, good_names[g], 7);
                row.append(bn::to_string<8>(price(town, g)));
                row.append("  x");
                row.append(bn::to_string<8>(cargo[g]));
                table_lines[g].set(map_gen, ui_x, -46 + 10 * g, row);
            }

            // The town crier (growth cut 1): the latest rumor, shouted on
            // every square of the road — the ledger's forecasting page.
            {
                int rid = latest_rumor();
                bn::string<40> cry;

                if(rid == 0)
                {
                    cry.append("NO WORD ON THE ROAD");
                }
                else
                {
                    const rumor_t& r = rumor_deck[rid - 1];
                    cry.append("CRIER: ");
                    cry.append(good_names[r.good]);
                    cry.append(' ');
                    cry.append(bn::to_string<8>(foretell(r)));
                    cry.append(" AT ");
                    cry.append(town_names[r.town]);
                    cry.append(" DAY ");
                    cry.append(bn::to_string<8>(r.hit_day));
                }

                if(! head_changed)
                {
                    title_lines[0].set(ui_gen, ui_x, -4, cry);
                }
            }

            // The pact line (growth cut 2): the latest posted contract
            // and its lifecycle. Deferred one frame FURTHER than the
            // crier (regen only from the SECOND quiet frame after a
            // head redraw): dawn frames are budget-SPENT (the v0.2
            // measured lesson) and the crier owns the first quiet
            // frame — staggering keeps every regeneration frame
            // single-purpose. head_quiet is a pure function of the
            // input history, so determinism is untouched.
            if(head_quiet >= 2)
            {
                int cid = current_contract();
                bn::string<40> pact;

                if(cid == 0)
                {
                    pact.append("NO PACTS POSTED");
                }
                else
                {
                    const contract_t& c = contract_deck[cid - 1];
                    int cs = contract_state(cid - 1);

                    if(cs == 3)
                    {
                        pact.append("PACT PAID +");
                        pact.append(bn::to_string<8>(c.premium));
                    }
                    else if(cs == 4)
                    {
                        pact.append("PACT LAPSED");
                    }
                    else
                    {
                        pact.append(cs == 1 ? "PACT? " : "PACT: ");
                        pact.append(bn::to_string<8>(c.qty));
                        pact.append(' ');
                        pact.append(good_names[c.good]);
                        pact.append(" TO ");
                        pact.append(town_names[c.town]);
                        pact.append(" D");
                        pact.append(bn::to_string<8>(c.deadline_day));
                        pact.append(" +");
                        pact.append(bn::to_string<8>(c.premium));
                    }
                }

                pact_line.set(ui_gen, ui_x, 74, pact);
            }

            // The ROAD line (growth cut 3): the latest announced
            // hazard and the standing guard, on the row the static
            // verb-help line held through v0.3 (the full help stays on
            // the title card — an always-on sprite wash, deliberately:
            // the #143 lesson says audit the pool at the worst redraw
            // frame before adding lines). Deferred one frame further
            // than the pact line (regen only from the THIRD quiet
            // frame after a head redraw): dawn frames are budget-SPENT
            // and the crier/pact lines own quiet frames 1 and 2.
            // head_quiet is a pure function of the input history, so
            // determinism is untouched.
            if(head_quiet >= 3)
            {
                int hid = latest_hazard();
                bn::string<40> road;

                if(hid == 0)
                {
                    road.append("THE ROAD LIES QUIET");
                }
                else
                {
                    // "RAID" is the bandits' on-screen token: the
                    // guarded line must fit the 240px screen from
                    // ui_x, and "BANDITS: GLASSMERE RD D15-16 +GUARD"
                    // measurably clips its last glyph (the first P6
                    // probe caught exactly that).
                    const hazard_t& h = hazard_deck[hid - 1];
                    road.append(h.kind == 0 ? "RAID: " : "STORM: ");
                    road.append(town_names[h.leg + 1]);
                    road.append(" RD D");
                    road.append(bn::to_string<8>(h.from_day));
                    road.append('-');
                    road.append(bn::to_string<8>(h.to_day));
                }

                if(outfitted)
                {
                    road.append(" +GUARD");
                }

                road_line.set(ui_gen, ui_x, 66, road);
            }

            ui_lines[2].set(ui_gen, ui_x, 8, "LEDGER - THE INK AGES");

            int slot = 0;

            // The ledger lists the SPINE towns (crossroads cut 1 bounds
            // this to spine_town_count, not town_count): standing on a
            // spine town this renders the other six EXACTLY as v0.6
            // (byte-identical display), and standing on the branch it
            // shows all seven spine towns — WYRMHOLLOW's own row is
            // never listed (you are standing in it, and its price is in
            // the market table). ledger_lines has town_count - 1 = 7
            // slots, room for the seven-row branch case.
            for(int t = 0; t < spine_town_count; ++t)
            {
                if(t == town)
                {
                    continue;
                }

                // The v0.4 dawn workload is a committed interface (the
                // #142-#144 frame-budget lesson): the five legacy
                // towns' rows regenerate with the dawn exactly as
                // before, and the two NEW towns' rows defer to quiet
                // frames 4 and 5 after a head redraw (crier 1, pact 2,
                // road 3 — the stagger extended). head_quiet is a pure
                // function of the input history: determinism holds.
                if(t >= legacy_town_count
                   && head_quiet < unsigned(4 + t - legacy_town_count))
                {
                    ++slot;
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

                ledger_lines[slot].set(map_gen, ui_x, 18 + 8 * slot, row);
                ++slot;
            }

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

        // Rumor witness words (growth cut 1) — layout in the header.
        {
            int rid = state == st_title ? 0 : latest_rumor();
            const rumor_t& r = rumor_deck[rid == 0 ? 0 : rid - 1];
            bool none = rid == 0;
            wr_telemetry[16] = none ? 0u : unsigned(rid);
            wr_telemetry[17] = none ? 0u : unsigned(r.hit_day);
            wr_telemetry[18] = none ? 0u : unsigned(r.town);
            wr_telemetry[19] = none ? 0u : unsigned(r.good);
            wr_telemetry[20] = none ? 0u : unsigned(foretell(r));
            wr_telemetry[21] = none ? 0u : unsigned(price(r.town, r.good));
            wr_telemetry[22] = none ? 0u : unsigned(shock(r.town, r.good));
            wr_telemetry[23] = none                          ? 0u
                             : int(day) < r.hit_day          ? 1u
                             : int(day) < r.hit_day + rumor_window ? 2u
                                                             : 3u;
        }

        // Contract witness words (growth cut 2) — layout in the header.
        {
            int cid = state == st_title ? 0 : current_contract();
            const contract_t& c = contract_deck[cid == 0 ? 0 : cid - 1];
            bool none = cid == 0;
            wr_telemetry[24] = none ? 0u : unsigned(cid);
            wr_telemetry[25] = none ? 0u
                                    : unsigned(contract_state(cid - 1));
            wr_telemetry[26] = none ? 0u : unsigned(c.good);
            wr_telemetry[27] = none ? 0u : unsigned(c.town);
            wr_telemetry[28] = none ? 0u : unsigned(c.qty);
            wr_telemetry[29] = none ? 0u : unsigned(c.deadline_day);
            wr_telemetry[30] = none ? 0u : unsigned(c.premium);
            wr_telemetry[31] = state == st_title
                                    ? 0u : unsigned(premium_paid_total());
        }

        // Hazard witness words (growth cut 3) — layout in the header.
        {
            int hid = state == st_title ? 0 : latest_hazard();
            const hazard_t& h = hazard_deck[hid == 0 ? 0 : hid - 1];
            bool none = hid == 0;
            wr_telemetry[32] = none ? 0u : unsigned(hid);
            wr_telemetry[33] = none ? 0u : unsigned(h.kind);
            wr_telemetry[34] = none ? 0u : unsigned(h.leg);
            wr_telemetry[35] = none ? 0u : unsigned(h.from_day);
            wr_telemetry[36] = none ? 0u : unsigned(h.to_day);
            wr_telemetry[37] = state == st_title ? 0u : unsigned(outfitted);
            wr_telemetry[38] = state == st_title
                                    ? 0u : unsigned(hazard_gold_lost);
            wr_telemetry[39] = state == st_title
                                    ? 0u : unsigned(storm_days_lost);
        }

        // Wider-map witness words (growth cut 4) — layout in the header.
        {
            bool title = state == st_title;
            wr_telemetry[40] = title ? 0u : unsigned(capacity());
            wr_telemetry[41] = title ? 0u : unsigned(mules);
            wr_telemetry[42] = title ? 0u : unsigned(stable_price());
            wr_telemetry[43] = title ? 0u : unsigned(mule_spend);
            wr_telemetry[44] = title ? 0u
                                     : unsigned(price(wide_witness_town,
                                                      wide_witness_good));
            wr_telemetry[45] = title || ledger_day[wide_witness_town] == 0
                ? 0u
                : unsigned((ledger[wide_witness_town][wide_witness_good] << 8)
                           | ((int(day) - ledger_day[wide_witness_town])
                              & 0xFF));
            wr_telemetry[46] = title ? 0u : visited_mask;
            wr_telemetry[47] = title ? 0u
                                     : unsigned(capacity() - pack_used());
        }

        // Audio hook-count words (growth cut 5) — layout in the header.
        // Cumulative since BOOT (not zeroed on the title, not reset by
        // a restart), so the words are monotone like the frame word.
        wr_telemetry[48] = audio_coin;
        wr_telemetry[49] = audio_dawn;
        wr_telemetry[50] = audio_wind;
        wr_telemetry[51] = audio_coin + audio_dawn + audio_wind;

        // Crossroads witness words (crossroads cut 1) — layout in the
        // header. All 0 on the title.
        {
            bool title = state == st_title;
            wr_telemetry[52] = title ? 0u
                                     : (int(town) == branch_town ? 1u : 0u);
            wr_telemetry[53] = title || adjacency[town].fork < 0
                                     ? 0u : unsigned(branch_leg);
            wr_telemetry[54] = title ? 0u
                                     : unsigned(price(branch_town,
                                                      branch_town % good_count));
            wr_telemetry[55] = 0u;       // reserved
        }

        bn::core::update();
    }
}
