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
    //   run_tier_label(200) -> "VETERAN" (still, until a higher tier is added)
    // Pure and self-contained (no Butano/GBA/std includes, no state, no RNG);
    // the stdlib mirror is tools/check-run-tier.py — keep the two in lockstep
    // (any threshold/label change lands in BOTH in the same PR).
    inline const char* run_tier_label(int lifetime_runs)
    {
        // Descending by threshold: return the FIRST (highest) tier met. Starts
        // with a single tier; add higher rows ABOVE this one (e.g. 100/MASTER)
        // to keep first-match-wins correct.
        struct tier { int at_least; const char* label; };
        static const tier tiers[] = {
            { 50, "VETERAN" },
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
}

#endif // WR_MILESTONES_H
