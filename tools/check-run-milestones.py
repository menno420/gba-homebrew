#!/usr/bin/env python3
"""Wickroad host proof (stdlib-only, <1s) — the run-count milestone flourish.

The check-underroot.py sibling for Wickroad's end-card milestone callout: a
line-for-line Python mirror of the pure decision function in
games/wickroad/src/wr_milestones.h (wr::run_milestone_label), proving the
threshold-crossing rule for EVERY reachable run ordinal instead of the handful
a play session would hit.

The rule under test: the just-completed run's ORDINAL (1 = the first ever
run-end; captured in record_run() as best.runs + 1 BEFORE ++best.runs, so the
10th completed run-end has ordinal 10 — the same count #186 renders as
RUNS 10) maps to a one-line end-card callout ONLY on the lifetime thresholds
10 / 25 / 50, and to nothing otherwise. Deterministic, no state, no RNG.

MIRROR RULE (keep in lockstep): run_milestone_label below mirrors
games/wickroad/src/wr_milestones.h exactly. Any change to the C++ MUST land
here in the same PR — same discipline as tools/check-underroot.py <-> ur_sim.c.

Usage: python3 tools/check-run-milestones.py          # exit 0 = green
"""

import sys

# --- the pure decision (mirror wr::run_milestone_label, wr_milestones.h) ----
MILESTONES = {
    10: "10TH RUN",
    25: "25TH RUN",
    50: "50TH RUN",
}


def run_milestone_label(completed_run_ordinal):
    # Return the callout string for this run ordinal, or None (C++ nullptr).
    return MILESTONES.get(completed_run_ordinal)


# --- proof ------------------------------------------------------------------
def prove_milestones():
    # Enumerate the whole reachable ordinal range and assert the exact rule:
    # only 10/25/50 emit a label, and each emits its own "<n>TH RUN" string;
    # every other ordinal (below, between, above) emits nothing.
    checks = 0
    for ordinal in range(1, 201):
        label = run_milestone_label(ordinal)
        recomputed = run_milestone_label(ordinal)
        assert label == recomputed, "run_milestone_label not deterministic"
        if ordinal in (10, 25, 50):
            assert label == f"{ordinal}TH RUN", \
                f"ordinal {ordinal} label wrong: {label!r}"
        else:
            assert label is None, \
                f"ordinal {ordinal} crossed a threshold it should not: {label!r}"
        checks += 1

    # Spell out the boundaries the prose promises (no off-by-one at a rim).
    for ordinal in range(1, 10):
        assert run_milestone_label(ordinal) is None
    assert run_milestone_label(10) == "10TH RUN"
    for ordinal in range(11, 25):
        assert run_milestone_label(ordinal) is None
    assert run_milestone_label(25) == "25TH RUN"
    for ordinal in range(26, 50):
        assert run_milestone_label(ordinal) is None
    assert run_milestone_label(50) == "50TH RUN"
    for ordinal in range(51, 201):
        assert run_milestone_label(ordinal) is None

    # Ordinal 0 (no run ever ended) and negatives are never a milestone —
    # record_run only calls with best.runs + 1 >= 1, but the helper is total.
    assert run_milestone_label(0) is None
    assert run_milestone_label(-1) is None

    return checks


def main():
    checks = prove_milestones()
    labels = ", ".join(f"{k}->{v!r}" for k, v in sorted(MILESTONES.items()))
    print("check-run-milestones: OK —")
    print(f"  milestone table: {labels}")
    print(f"  ordinal sweep: {checks} ordinals (1..200) — only 10/25/50 emit a "
          "label, each its own \"<n>TH RUN\", every other ordinal emits nothing "
          "(boundaries 9/10/11, 24/25/26, 49/50/51 pinned; 0 and -1 total)")
    return 0


if __name__ == '__main__':
    sys.exit(main())
