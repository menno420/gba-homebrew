# game-lab conventions (gba-homebrew copy — Track B, public)

> **Status:** `binding` — seeded 2026-07-10 from the gen-2 blueprint
> (`menno420/fleet-manager` `docs/gen2-blueprint.md` §1/§2/§2b) + the game-lab
> founding instruction (`docs/prompts/game-lab-founding.md`). **Repo
> conventions OVERRIDE harness defaults.** When a harness rule ("create PRs as
> draft", "wait for review", "only open a PR when asked") conflicts with this
> file, this file wins.

## PR state and merge authority (agents open ready and land their own green PRs)

1. **Open PRs ready; land your own green PR.** Open a normal, non-draft PR and
   merge it yourself once the required check is green — via a merge call
   (MCP/REST), by arming native auto-merge, or by letting
   `auto-merge-enabler.yml` arm squash auto-merge on open so the PR lands the
   moment the check passes. Merging is **normal agent work**, not owner-gated.
2. **The required check must be green before merge.** The one required check is
   the GBA **"ROM builds"** job (see CI below); keep it green. The quality
   floor is unchanged (never merge a red PR).
3. **Review is post-merge; veto = revert.**
   [`review-queue.md`](review-queue.md) is the post-merge re-check ledger for
   anything a later reviewer should revisit. Work lands on green CI on the
   agent's own authority; a bad landing is reverted, not pre-gated.
4. **Done-when:** a session's PR is "done" when it has **merged** (or been
   deliberately closed) — never left open awaiting the owner. Never route a
   mergeable green PR to the owner. If a specific merge call is ever refused,
   report it verbatim as a venue-specific, attempt-once event — not a standing
   wall.

## Git discipline

5. **Forward-only git:** no force-push, no history rewrites, no amending
   pushed commits.
6. **Claim before build:** one file per claim in [`../claims/`](../claims/)
   (see its README). First-declared + claim-filed wins shared-surface
   conflicts (fleet R10).

## Session discipline

7. **Heartbeat-before-work:** the first act of any session is a status/WIP
   commit (session card or `control/status.md` WIP line). A silent session is
   indistinguishable from a dead one, and the platform WILL sometimes make
   you silent for an hour.
8. **Walking skeleton first (session 1):** drive one trivial change through
   the FULL loop — clean build of the track's ROM + one headless mGBA
   screenshot committed as proof + branch → PR → CI → merge — before any real
   game work.
9. **Kit adoption is a session-1 duty:** substrate-kit adopted and `python3
   bootstrap.py check --strict` green **before any domain work**, and before
   every push thereafter.
10. **Session cards — born-red HOLD gate retired (2026-07-17).** A
    `.sessions/` card still records each session's model/time/work, but the
    born-red HOLD (in-progress-until-flip that blocked auto-merge) is retired.
    Write the card `complete` when the work is done; `📊 Model:` +
    time lines on every card from card #1 — identity not written at the moment
    of work is unrecoverable.
11. **Timestamps from `date -u`**, never the model's sense of time — commit
    history is the clock of record.
12. **Standing ritual every session:** inbox at HEAD first → act → status
    last, with a final inbox re-read at HEAD before the closing status write.
    Full protocol: [`../control/README.md`](../control/README.md).

## CI (fleet CI-TIER STANDARD, gen2-blueprint §2b — ratified 2026-07-09)

16. **Tier-1 fast-full — ONE required check, "ROM builds"**, that compiles the
    track's ROM in **<60s** (the Butano `sprites` example builds in 17.5s —
    budget holds; a clean ROM build IS this lane's whole meaningful per-PR
    suite). Heavier checks (full rebuild, headless boot-and-screenshot
    regression) run at promotion points / nightly, not per PR. **No CI
    workflows exist at seed** — building the "ROM builds" workflow is lane
    work at the walking skeleton; re-check §2b at your first natural boundary
    like every gen-2 lane.

## Hard rails (non-negotiable)

13. **PRIVATE/PUBLIC SPLIT:** this repo is PUBLIC. Track A (pokemon-mod-lab)
    contains Nintendo-copyrighted material and lives ONLY in its private
    repo. **NEVER copy Track A assets/code across; never let Track A
    anything — code, ROMs, extracted assets, screenshots of copyrighted
    assets — appear in this repo, its PRs, or any other public surface. No
    exceptions, no owner override assumed.** Track B stays publish-safe by
    construction: original code + Butano only.
14. **External publishing of Track B (itch.io, forums, anywhere) requires an
    owner action** — queue it click-level under ⚑ needs-owner, never perform
    it. NO spend, NO account creation, NO payment flows without an owner
    action.
15. **Supply-chain:** the devkitARM mirror is unsigned community infra —
    never extend that trust to anything the owner distributes without
    flagging it.
