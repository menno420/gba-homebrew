# control/inbox.md — MANAGER-WRITTEN, append-only

> One writer: the fleet manager. The lane NEVER edits this file. Orders stay
> `status: new` in this file — the lane diffs the inbox against its own
> `control/status.md` to see what is unexecuted. Protocol: [`README.md`](README.md).

---

## ORDER 001 — Track B walking skeleton + game concepts

- **status:** new
- **issued:** 2026-07-10 (fleet manager, at lane seed; drafted in
  `menno420/fleet-manager` `docs/prompts/game-lab-founding.md` § ORDER 001)
- **task (verbatim from the founding draft):** install the mirror-route
  toolchain, build and run one Butano example headlessly (screenshot
  committed), then propose **3 original game concepts** with the same
  scope-estimate format (sessions to first-playable, systems touched, risk).
- **done-when (verbatim; joint across both tracks):** both walking skeletons
  merged on green + the 6 concepts with estimates committed for owner pick.
  *(This repo's half: Track B's walking skeleton merged on green + its 3
  original game concepts with estimates committed. Track A's half lives in
  `menno420/pokemon-mod-lab` `control/inbox.md`.)*

---

## ORDER 002 (status: new, P1): SELF-ARM YOUR WAKE ROUTINE

The owner has verified 2026-07-10 that Project sessions can create routines
that fire inside their own Project. Create yours: cadence hourly, prompt:
'Read control/inbox.md at HEAD and run the standing ritual from your
instructions.' Record in control/status.md: the exact mechanism used (tool
name or UI path) + confirmation of the first successful fire, OR the exact
refusal/error text if it fails on your surface. Done-when: routine armed and
mechanism documented in status, or failure documented verbatim with a ⚑ owner
fallback ask.

---

## Standing default (between orders)

When the inbox has no unexecuted orders: **advance the current track's next
playable increment, keep the headless test loop honest (screenshot proofs in
PRs), groom the backlog — never idle, never undefined.**

## ORDER 003 · 2026-07-11T03:30:31Z · status: new
priority: P3
from: fleet-manager manager — ORDER 010 per-lane relay (provenance: fm control/inbox.md ORDER 010 + fm docs/findings/model-matrix-2026-07.md; relayed via fm PR #63)
executor: gba-homebrew lane coordinator — next fired session
do: Model-attribution ground truth (fleet standing rule, family-level names only per Q-0262): (1) confirm the session-card template carries a `📊 Model:` line — add it if missing; (2) every fired session records the model family its own harness/environment reports (e.g. fable-5, opus-4.8, sonnet-5) on that line in its committed session card — the Routines screen is NOT a reliable attribution surface; (3) replace the card's "ID withheld" null convention with the real family name the harness reports (family-level is allowed and required; only exact model IDs are withheld).
why: the fleet model matrix (fm docs/findings/model-matrix-2026-07.md) found per-session self-report in commits is the only reliable attribution; cross-surface disagreement is evidenced (websites PR #59 squash 2c89e96: Routines screen fable-5 vs the fired card's claude-sonnet-5).
done-when: the next fired session's committed card carries a real family-level `📊 Model:` line and the template (if any) includes it.

## ORDER 004 · 2026-07-11T10:01Z · status: new
priority: P1
executor: gba-homebrew seat (next wake)
do: quick self-review of this lane covering roughly the last 24h (2026-07-10 ~20:00Z → now): (1) anything that WENT WRONG — red CI runs, guard/classifier denials, walls hit, drift found, mistakes made or corrected — each with a citation (PR/run/commit); (2) anything REQUIRING OWNER ATTENTION — owner-only asks, pending vetoes, risky decisions taken decide-and-flag, spend/publish items — click-level and plain language; (3) one-line current health (what shipped, what's next). Commit the review as a dated "Self-review 2026-07-11" section in control/status.md (or this lane's report convention); mirror ⚑ owner-attention items on the heartbeat so the manager sweep collects them.
why: owner-requested fleet-wide self-review (2026-07-11), relayed by the fleet-manager coordinator on the owner's in-session instruction.
done-when: the self-review section is on main within this lane's next two wakes.
provenance: filed by fleet-manager on coordinator direction (cse_012o8pySy5K3AV6JWoPKryZL), owner-directed.
