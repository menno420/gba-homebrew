# claims/ — claim before build

> **Status:** `binding` — seeded 2026-07-10 per the gen-2 blueprint §1 (the
> gen-1 shared-ground races: the games kit-adoption collision, kit ORDER-005
> double execution).

## Convention

**Claim before build, one file per claim.** Before starting any work that
touches a shared surface (kit adoption, interface files, `.substrate/`,
control protocol files, the toolchain tree another session may be building
against):

1. Check this directory for an existing claim on the surface — if claimed,
   coordinate or pick other work; don't duplicate.
2. Create `claims/<branch-or-scope>.md` with one bullet:
   `branch · scope · expected files/area · date (date -u)`.
3. Land the claim (it can ride your heartbeat/WIP commit), then re-read the
   inbox and claims at HEAD before you build.
4. **Delete your claim file at session close.**

*One file per claim, not one shared list* — two writers on one file is the
only way this merge-conflicts (fleet R9); per-file claims measured 0% conflict
vs ~98% for a shared append ledger.

## Arbitration

First-declared + claim-filed wins shared-surface conflicts (fleet R10) — a
deterministic tiebreak beats re-litigating every collision.

## Pre-resolved at seed (no race possible)

- **substrate-kit adoption** is assigned to the lane's FIRST session as a
  session-1 duty ([`../docs/conventions.md`](../docs/conventions.md) rule 9) —
  no later session should claim or re-run adoption.
- **devkitARM mirror-route toolchain install** is ORDER 001 work, assigned to
  the first session executing that order — no later session should re-install
  the toolchain from scratch without a claim.
