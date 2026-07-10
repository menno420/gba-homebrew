# game-lab retro questions (Track B copy) — standing self-review protocol

> **Status:** `binding` — planted at seed (2026-07-10) per the gen-2 blueprint
> §1: retro questions exist from day 0 **so self-review is continuous, not
> archaeology**. Question set = the fleet's universal core (fleet retro
> protocol, 2026-07-09) + the game-lab addendum.
>
> **Rules:** answer by ID in `docs/retro/self-review-<date>.md` (IDs make
> answers comparable across the fleet). Honest > flattering — friction, dead
> ends, and "I don't know" are the deliverable; invented certainty is not.
> Every claim linked to repo evidence (PR / commit / file) where possible.
> Self-assessments are data, not verdicts — the manager cross-checks against
> git. Don't wait for a retro order: any session that hits notable friction
> answers the relevant IDs the same session.

## A. Work & correctness

A1. What did you actually ship to main, vs what exists only on branches/drafts? Explain any gap.
A2. Which of your claims were verified against an external oracle (live run, real deploy, recorded goldens, a human) vs only your own tests?
A3. Which piece of your work are you LEAST confident in, and what concrete check would prove or disprove it?
A4. What did you build that turned out unnecessary, duplicated, or already existing somewhere you didn't look?

## B. Errors & friction

B1. List every error you hit (environment, CI, permissions, tooling, API). For each: time lost, and was it preventable by you, by better setup, or genuinely external?
B2. What did you have to figure out that was already documented somewhere you didn't find? Where SHOULD it have been for you to see it at the moment you needed it?
B3. What broke silently (no error, wrong result)? How was it eventually discovered?
B4. Which line of your Custom Instructions or orders was ambiguous, contradictory, or missing exactly when you needed it? Quote it.

## C. Efficiency

C1. Rough % of your working time spent on: orientation/reading · building · verifying · CI/merge mechanics · blocked/waiting. Biggest single time sink?
C2. What context did you rebuild from scratch that should have been durable (a file, an index, a summary)?
C3. Which tool/check/practice gave the most value per minute? Which the least?
C4. Redoing your work with what you know now: how much faster, and what's the biggest ORDERING change you'd make?

## D. Autonomy & owner input

D1. List every point you stopped for owner input or a human click. For each: truly owner-only (taste/money/irreversible), or unblockable by a pre-granted rule/scope? Name the grant.
D2. Which decisions did you route upward that you now think you should have taken decide-and-flag?
D3. Which decisions did you take while unsure you were allowed to? What written rule would have made it unambiguous?
D4. The smallest set of standing grants that would have let you ship end-to-end with zero humans: list it.
D5. Did you always know what "done" meant for your current order? Where was it undefined?

## E. Protocol & environment

E1. Did the control/ ritual (inbox first, status last, never edit inbox) fit how you actually work? Where did it cost you, or where did you skip it — and why?
E2. What should the ENVIRONMENT have contained at first boot that it didn't (deps, tools, config, data)?
E3. What should the REPO have contained at seed that it didn't (structure, CI, templates, examples, docs)?
E4. If a fresh session started on your repo tomorrow with no chat history, what would it misunderstand first, and what single document would prevent that?

## F. Redesign (the payload)

F1. Write the THREE rules you'd put into the next Project's founding instructions that weren't in yours.
F2. What should the MANAGER have done differently for you — orders too vague/too detailed, too many/few, wrong priorities, wrong timing?
F3. One capability you lacked that you'd trade almost anything for.
F4. If your Project were relaunched tomorrow "built right from the start": describe its ideal seed state in ≤10 bullet points.

## G. Addendum — GAME (game-lab)

G1. Born-right audit: which gen-2 seed conventions (self-merge grant, heartbeat-before-work, walking skeleton, inbox-diff semantics, day-0 walls file) actually prevented friction, and which were ceremony you paid without payoff? Evidence per item.
G2. Rail honesty (Track B, PUBLIC repo): did anything Nintendo-derived — code, ROMs, extracted assets, screenshots — enter this repo, its PRs, or any public surface? What guard would make the private/public split structurally safe rather than remembered?
G3. Loop honesty: are the headless mGBA screenshot proofs real, current, and tied to the PR they claim to prove — or decorative? Quote a case.
G4. Toolchain drift: did any documented recipe (leseratte10 mirror route, make-rules/crt0 build, mgba==0.10.2 pin) fail or need adaptation? Was the finding appended to `docs/capabilities.md` the same session?
