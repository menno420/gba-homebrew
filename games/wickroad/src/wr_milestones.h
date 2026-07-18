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
}

#endif // WR_MILESTONES_H
