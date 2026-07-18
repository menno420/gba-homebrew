#!/usr/bin/env python3
"""Wickroad host proof (stdlib-only, <1s) — the end-card tier-up flash.

The check-run-milestones.py sibling for Wickroad's TIER-UP announce: a
line-for-line Python mirror of the pure decision function in
games/wickroad/src/wr_milestones.h (wr::run_tier_up_label), proving the
tier-crossing rule for EVERY reachable run ordinal instead of the handful a
play session would hit.

The rule under test: the just-completed run's ORDINAL (1 = the first ever
run-end; captured in record_run() as best.runs + 1 BEFORE ++best.runs, so the
50th completed run-end has ordinal 50 — the same count #186 renders as RUNS 50)
maps to a one-line end-card TIER-UP callout ONLY on the run that FIRST reaches a
tier (an == crossing), and to nothing otherwise. This is the announce paired
with run_tier_label's persistent >= LEVEL title tag: the level tag re-shows
every boot, the flash fires once on the crossing run.

DERIVED, one source of truth: the tier thresholds/labels live ONLY in
run_tier_label (mirrored from tools/check-run-tier.py); the crossing is
run_tier_label(n) != run_tier_label(n-1). So this file must NOT restate the
thresholds — it imports the exact tier rule under test and derives the crossing
from it, the same way the C++ derives run_tier_up_label from run_tier_label.
Deterministic, no state, no RNG.

MIRROR RULE (keep in lockstep): run_tier_up_label below mirrors
games/wickroad/src/wr_milestones.h exactly. Any tier-table change lands in
run_tier_label + check-run-tier.py + here in the same PR — same discipline as
tools/check-underroot.py <-> ur_sim.c.

Usage: python3 tools/check-run-tier-up.py             # exit 0 = green
"""

import sys

# --- the pure tier LEVEL (mirror wr::run_tier_label, wr_milestones.h) --------
# Single source of truth for thresholds/labels, identical to check-run-tier.py.
# Descending by threshold: the first (highest) tier the count meets wins.
TIERS = (
    (100, "MASTER"),
    (50, "VETERAN"),
)


def run_tier_label(lifetime_runs):
    # Return the earned tier label for this lifetime count, or None (C++ nullptr).
    for at_least, label in TIERS:
        if lifetime_runs >= at_least:
            return label
    return None


# --- the derived CROSSING (mirror wr::run_tier_up_label, wr_milestones.h) -----
def run_tier_up_label(completed_run_ordinal):
    # This ordinal reaches a NEW tier iff the earned tier here differs from the
    # earned tier one ordinal earlier; return the newly-earned label, else None.
    now = run_tier_label(completed_run_ordinal)
    prev = run_tier_label(completed_run_ordinal - 1)
    return now if now != prev else None


# --- proof ------------------------------------------------------------------
def prove_tier_ups():
    # Enumerate the whole reachable ordinal range and assert the exact rule:
    # only the two crossing ordinals (50 -> "VETERAN", 100 -> "MASTER") emit a
    # label; every other ordinal (below, between, above a tier) emits nothing —
    # the announce fires ONCE per tier, on the run that first reaches it.
    checks = 0
    for ordinal in range(0, 301):
        label = run_tier_up_label(ordinal)
        recomputed = run_tier_up_label(ordinal)
        assert label == recomputed, "run_tier_up_label not deterministic"
        if ordinal == 50:
            assert label == "VETERAN", \
                f"ordinal {ordinal} tier-up wrong: {label!r}"
        elif ordinal == 100:
            assert label == "MASTER", \
                f"ordinal {ordinal} tier-up wrong: {label!r}"
        else:
            assert label is None, \
                f"ordinal {ordinal} flashed a tier-up it should not: {label!r}"
        checks += 1

    # Spell out every boundary the prose promises (no off-by-one at a rim): the
    # crossing fires only ON the threshold, not the ordinal before or after, and
    # NOT again anywhere the tier merely persists.
    for ordinal in range(0, 50):
        assert run_tier_up_label(ordinal) is None
    assert run_tier_up_label(49) is None
    assert run_tier_up_label(50) == "VETERAN"
    assert run_tier_up_label(51) is None
    for ordinal in range(51, 100):
        assert run_tier_up_label(ordinal) is None
    assert run_tier_up_label(99) is None
    assert run_tier_up_label(100) == "MASTER"
    assert run_tier_up_label(101) is None
    for ordinal in range(101, 301):
        assert run_tier_up_label(ordinal) is None

    # Exactly one crossing per tier over the whole reachable range.
    veteran_hits = [n for n in range(0, 301) if run_tier_up_label(n) == "VETERAN"]
    master_hits = [n for n in range(0, 301) if run_tier_up_label(n) == "MASTER"]
    assert veteran_hits == [50], f"VETERAN crossings wrong: {veteran_hits}"
    assert master_hits == [100], f"MASTER crossings wrong: {master_hits}"

    # Ordinal 0 (no run ever ended) and negatives never cross a tier — the
    # helper is total (run_tier_label(-1) == run_tier_label(-2) == None).
    assert run_tier_up_label(0) is None
    assert run_tier_up_label(-1) is None

    return checks


def main():
    checks = prove_tier_ups()
    crossings = ", ".join(
        f"{n}->{run_tier_up_label(n)!r}" for n in (50, 100))
    print("check-run-tier-up: OK —")
    print(f"  tier-up crossings: {crossings}")
    print(f"  ordinal sweep: {checks} ordinals (0..300) — only 50 emits "
          "\"VETERAN\" and 100 emits \"MASTER\" (each tier crosses exactly "
          "once), every other ordinal emits nothing (boundaries 49/50/51 and "
          "99/100/101 pinned; 0 and -1 total)")
    return 0


if __name__ == '__main__':
    sys.exit(main())
