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

## Standing default (between orders)

When the inbox has no unexecuted orders: **advance the current track's next
playable increment, keep the headless test loop honest (screenshot proofs in
PRs), groom the backlog — never idle, never undefined.**
