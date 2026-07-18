#!/usr/bin/env python3
"""Wickroad host proof (stdlib-only, <1s) — the persistent run-count tier tag.

The check-run-milestones.py sibling for Wickroad's TITLE-screen tier tag: a
line-for-line Python mirror of the pure decision function in
games/wickroad/src/wr_milestones.h (wr::run_tier_label), proving the tier rule
for EVERY reachable lifetime run count instead of the handful a play session
would hit.

The rule under test: the ledger's CURRENT stored lifetime run count (the value
#186 renders as RUNS n) maps to the persistent tier label the player has EARNED
— the highest threshold their total meets — or to nothing below the first tier.
This is a >= LEVEL, not an == crossing (that is run_milestone_label's job for
the end card), so the boundary is INCLUSIVE at each threshold and the label
PERSISTS above it: 49 -> none, 50 -> "VETERAN", 200 -> "VETERAN". Deterministic,
no state, no RNG.

MIRROR RULE (keep in lockstep): run_tier_label below mirrors
games/wickroad/src/wr_milestones.h exactly. Any change to the C++ MUST land
here in the same PR — same discipline as tools/check-run-milestones.py.

Usage: python3 tools/check-run-tier.py             # exit 0 = green
"""

import sys

# --- the pure decision (mirror wr::run_tier_label, wr_milestones.h) ---------
# Descending by threshold: the first (highest) tier the count meets wins. Add
# higher rows ABOVE this one to keep first-match-wins correct.
TIERS = (
    (50, "VETERAN"),
)


def run_tier_label(lifetime_runs):
    # Return the earned tier label for this lifetime count, or None (C++ nullptr).
    for at_least, label in TIERS:
        if lifetime_runs >= at_least:
            return label
    return None


# --- proof ------------------------------------------------------------------
def prove_tiers():
    # Enumerate the whole reachable count range and assert the exact rule:
    # only counts >= 50 emit "VETERAN", and every count below emits nothing.
    checks = 0
    for runs in range(0, 201):
        label = run_tier_label(runs)
        recomputed = run_tier_label(runs)
        assert label == recomputed, "run_tier_label not deterministic"
        if runs >= 50:
            assert label == "VETERAN", \
                f"count {runs} tier wrong: {label!r}"
        else:
            assert label is None, \
                f"count {runs} earned a tier it should not: {label!r}"
        checks += 1

    # Spell out the boundary the prose promises (no off-by-one at the rim):
    # 49 is still below, 50 is the inclusive first tier, and it persists above.
    for runs in range(0, 50):
        assert run_tier_label(runs) is None
    assert run_tier_label(49) is None
    assert run_tier_label(50) == "VETERAN"
    assert run_tier_label(51) == "VETERAN"
    assert run_tier_label(200) == "VETERAN"

    # Count 0 (no run ever ended) and negatives are never a tier — the title
    # only calls with best.runs >= 0, but the helper is total.
    assert run_tier_label(0) is None
    assert run_tier_label(-1) is None

    return checks


def main():
    checks = prove_tiers()
    tiers = ", ".join(f"{n}->{v!r}" for n, v in sorted(TIERS))
    print("check-run-tier: OK —")
    print(f"  tier table: {tiers}")
    print(f"  count sweep: {checks} counts (0..200) — only runs>=50 earn "
          "\"VETERAN\", every count below earns nothing (boundary 49/50/51 "
          "pinned, persists to 200; 0 and -1 total)")
    return 0


if __name__ == '__main__':
    sys.exit(main())
