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

## ORDER 005 · 2026-07-13T00:47:05Z · status: new
priority: P1
executor: gba-homebrew seat (tonight's night run)
do: execute the owner night-run directive quoted verbatim below (everything from "DIRECT ORDER" to the end of this section is the owner's text, unaltered by the scribe).
why: direct owner order, received live 2026-07-13 (night run), landed verbatim per top-precedence owner turn; these header fields were added by the scribe to satisfy the inbox order grammar — the order text itself is untouched.
done-when: the directive's MORNING clause is met — by ~06:00Z the tally (slices landed or opened / new game starts with concept+prototype / sellables routed) is posted in heartbeat + outbox.

DIRECT ORDER — GAME LAB (owner, 2026-07-13, night run). Land this verbatim in your inbox (top-precedence owner turn), then execute all night.

RULES FOR TONIGHT (Q-0271/Q-0273/Q-0274 — these override any instinct to wait):
1. I am away until morning; that is the system's normal state. Silence = consent = done. Never hold or re-confirm finished work.
2. OPEN PRs STAY OPEN — new rule for tonight: land on green where auto-merge arms; where it doesn't (your enabler arm-step is a known open diagnosis), leave the PR OPEN and take the next slice. That includes your pre-built queue: PUSH Brineward 6 as a new PR now and keep pushing finished slices as open PRs — do not hold work behind unmerged parents; branch from the open head and note the base in the PR body. I sweep them all in the morning.
3. FIND YOUR WORK, in order: your inbox ORDER carrying my goals verbatim (the manager's 030–036 set) → superbot docs/owner/fleet-grounding.md §9 → your track plans at HEAD → your generative rung. An empty queue means GENERATE, never idle.
4. NO STALLS UNDER ANY CIRCUMSTANCES: probe before declaring a wall (attempt once, verbatim error; PLATFORM-LIMITS.md is your walls file); genuinely-owner-only item → six-field owner-queue entry → CONTINUE same turn; design uncertainty → SIM-REQUEST via outbox → CONTINUE.
5. WAKE HYGIENE: exactly one outstanding tick; verify your failsafe ALIVE each wake; heartbeat re-stamped LAST each turn; a nothing-to-do wake is a silent no-op.
6. QUALITY FLOOR: proofs/asserts green per slice; pokemon visibility check every wake (private, patches-not-ROMs, never public).
MORNING: by ~06:00Z post your tally (slices landed or opened / new game starts with concept+prototype / sellables routed) in your heartbeat + outbox.

YOUR SEAT TONIGHT (mass production — beyond GBA/NDS):
1. Current tracks keep shipping: next Gloamline + Brineward slices as open PRs (rule 2); pokemon private-track slice behind its playtest gate.
2. THE BREADTH PROGRAM starts now: multiple NEW small games tonight — each a playable prototype slice + a one-page concept (genre, loop, platform, sellability guess).
3. Platforms deliberately mixed: at least one WEB BROWSER game (target the arcade — route a hosting note to Websites via the manager) and at least one MOBILE-GAME foundation (framework pick + build pipeline + running skeleton, or the evidenced wall).
4. Sellability candidates → Venture Lab marker; keep the release-to-one-click packaging habit for everything finished.
