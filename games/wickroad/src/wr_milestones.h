#ifndef WR_MILESTONES_H
#define WR_MILESTONES_H

// Wickroad — the run-count milestone decision (follow-on to #184/#185/#186).
//
// PURE and SELF-CONTAINED by design: no Butano / GBA / std includes, no state,
// no RNG. This is the whole "does this run-end cross a lifetime milestone?"
// logic, extracted so it is trivially inspectable and host-testable. The
// stdlib mirror is tools/check-run-milestones.py — keep the two in lockstep
// (any threshold/label change lands in BOTH in the same PR, the same
// discipline as tools/check-underroot.py <-> ur_sim.c).
//
// Contract: given the just-completed run's ORDINAL (1 = the first ever
// run-end), return the milestone callout string for that ordinal, or nullptr
// when it crosses no threshold. The ordinal is the SRAM run counter's value
// AFTER this run-end is banked (== best.runs post-increment in record_run),
// which the caller supplies as best.runs + 1 captured BEFORE the increment
// (compare-before-increment, mirroring #185's NEW RECORD capture). So the 10th
// completed run-end shows "10TH RUN" — the same count #186 renders as RUNS 10.

namespace wr
{
    inline const char* run_milestone_label(int completed_run_ordinal)
    {
        switch(completed_run_ordinal)
        {
        case 10: return "10TH RUN";
        case 25: return "25TH RUN";
        case 50: return "50TH RUN";
        default: return nullptr;
        }
    }

    // Wickroad — the persistent run-count TIER (follow-on to #189).
    //
    // A SEPARATE concern from run_milestone_label above, and deliberately NOT an
    // overload of it: run_milestone_label is an ordinal-CROSSING decision for
    // the transient end-card flash (does THIS run-end land exactly on 10/25/50?),
    // while this is a persistent LEVEL keyed on the CURRENT stored lifetime run
    // total — the value #186 renders as RUNS n — so it re-shows every boot on
    // the TITLE screen instead of flashing once.
    //
    // Contract: given the ledger's current lifetime run count, return the tier
    // label the player has EARNED (the highest threshold their total meets), or
    // nullptr below the first tier. It is a >= level, not an == crossing, so the
    // boundary is INCLUSIVE at each threshold and the label persists above it:
    //   run_tier_label(49)  -> nullptr   (below the first tier)
    //   run_tier_label(50)  -> "VETERAN" (first tier reached, inclusive)
    //   run_tier_label(99)  -> "VETERAN" (still, below the next tier)
    //   run_tier_label(100) -> "MASTER"  (higher tier reached, inclusive)
    //   run_tier_label(300) -> "MASTER"  (still, until a higher tier is added)
    // Pure and self-contained (no Butano/GBA/std includes, no state, no RNG);
    // the stdlib mirror is tools/check-run-tier.py — keep the two in lockstep
    // (any threshold/label change lands in BOTH in the same PR).
    inline const char* run_tier_label(int lifetime_runs)
    {
        // Descending by threshold: return the FIRST (highest) tier met. Rows
        // stay ordered high-to-low so first-match-wins yields the highest
        // earned tier; add any further rows ABOVE these to keep that correct.
        struct tier { int at_least; const char* label; };
        static const tier tiers[] = {
            { 100, "MASTER"  },
            { 50,  "VETERAN" },
        };

        for(const tier& t : tiers)
        {
            if(lifetime_runs >= t.at_least)
            {
                return t.label;
            }
        }

        return nullptr;
    }

    // Wickroad — the TIER-UP flash decision (follow-on to #190/#195).
    //
    // The end-card announce paired with run_tier_label's persistent TITLE tag:
    // where run_tier_label is a >= LEVEL that re-shows every boot, this is an
    // == CROSSING (the run_milestone_label shape) that fires ONCE, on the
    // run-end whose banked lifetime ordinal FIRST reaches a tier — the
    // "you just earned this" beat, the tier analog of #185's NEW RECORD.
    //
    // DERIVED, not a second table: the tier thresholds/labels live in exactly
    // ONE place (run_tier_label above), so they cannot drift. This ordinal
    // reaches a NEW tier iff the earned tier at this ordinal differs from the
    // earned tier at the previous ordinal. run_tier_label returns a pointer
    // into its own static tier table (or nullptr), so the SAME threshold
    // always yields the SAME pointer and a different (higher) tier yields a
    // different one — pointer inequality is therefore exactly the crossing
    // test, and it returns the newly-earned tier's own label. Off a boundary,
    // both ordinals share a tier (or both are below the first) -> nullptr.
    //   run_tier_up_label(49)  -> nullptr   (49 and 48 both below the first tier)
    //   run_tier_up_label(50)  -> "VETERAN" (50 reaches VETERAN, 49 had not)
    //   run_tier_up_label(51)  -> nullptr   (51 and 50 both VETERAN)
    //   run_tier_up_label(100) -> "MASTER"  (100 reaches MASTER, 99 was VETERAN)
    //   run_tier_up_label(101) -> nullptr   (101 and 100 both MASTER)
    // Pure and self-contained (no Butano/GBA/std includes, no state, no RNG);
    // the stdlib mirror is tools/check-run-tier-up.py — keep the two in
    // lockstep (any tier-table change lands in run_tier_label + BOTH mirrors in
    // the same PR).
    inline const char* run_tier_up_label(int completed_run_ordinal)
    {
        const char* now = run_tier_label(completed_run_ordinal);
        const char* prev = run_tier_label(completed_run_ordinal - 1);
        return now != prev ? now : nullptr;
    }
}

#endif // WR_MILESTONES_H
