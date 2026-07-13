# Session — Undertow: web-arcade prototype (breadth program, night run)

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/web-undertow`)
- mission: ship ONE new original web browser game for the owner's web arcade — playable single-page HTML5 canvas prototype (games/web-undertow/), one-page concept doc, hosting note, and a locally-run headless smoke proof (tools/web-smoke-undertow.mjs). PR parked OPEN for the morning sweep (no merges, no auto-merge arming tonight — `do-not-automerge` label applied because auto-merge-enabler.yml now arms claude/* READY PRs).
- provenance: dispatched on the owner's 2026-07-13 night-run breadth order, relayed live via the game-lab coordinator. At claim time, control/inbox.md at HEAD (0e08695) carries ORDERS 001–004 only; the sibling-scribe landing of the night order was not yet visible. Proceeding because the work is contained + reversible (new dir + one tool script, PR left open, nothing merged).
- heartbeat: SKIPPED by dispatch constraint (no control/status.md writes this session) — recorded here instead.
- 📊 Model: fable-5 (family-level self-report from this session's own harness, per ORDER 003)

## What this session did

1. This card + `control/claims/claude-web-undertow.md`, born red (claim before build).
2. **Undertow** (`games/web-undertow/index.html` + `game.js`): vertical trench-diver — steer left/right (arrows/A,D/touch halves) to stay in a procedurally drifting, narrowing channel; wall touch ends the run; depth in meters is score; descent speed ramps (clamped) and channel narrows (clamped). Deterministic core: seeded mulberry32 (`?seed=N`), fixed-timestep 60 Hz sim over a RAF accumulator, no wall-clock reads in the step; `window.UNDERTOW` test API (`stepFrames`/`reset(seed)`/`setInput` + getters); `?headless=1` disables RAF for automated driving. Zero dependencies, no external requests, works from `file://`.
3. `games/web-undertow/CONCEPT.md` (pitch, loop, honest near-zero sellability as paid title / reasonable free arcade filler, deterministic daily-seed as the one differentiator) + `HOSTING.md` (static-only requirements, query params).
4. `tools/web-smoke-undertow.mjs` written and run locally against real Chromium — ALL GREEN. Key output (seed=7):
   ```
   PASS: UNDERTOW API exposed — version=1.0.0
   PASS: initial state is 'title' — state=title, seed=7
   PASS: Space keydown -> 'playing' — state=playing
   PASS: stepFrames(180) increases depth/score — score=37m rows=23 state=playing
   PASS: no-input run reaches 'gameover' within bound — state=gameover crashFrame=823 finalScore=194m
   PASS: determinism: same seed + same input => identical run — runA(crashFrame=823, score=194m) vs runB(crashFrame=823, score=194m)
   PASS: steering changes the outcome (hold-left run differs) — hold-left crashFrame=37 vs no-input crashFrame=823
   PASS: gameover -> Space restarts a fresh run — state=playing score=0m
   SMOKE PASS: all assertions green (seed=7, crashFrame=823, finalScore=194m)
   ```
   Web smoke runs locally; CI wiring deferred (workflows are kit-managed, adding a node job is not a contained change). No Makefile in games/web-undertow/, so rom-builds skips the dir.
5. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this card's in-progress badge (the designed born-red hold — "HOLD (by design)… nothing to investigate"); no other findings.
6. PR #79 opened **draft** → `do-not-automerge` label applied (label existed repo-side) → **then** marked READY. Label-before-ready ordering is load-bearing: auto-merge-enabler.yml arms claude/* READY PRs, and the label carve-out is the opt-out.
7. CI round 1 (head a46c474): ROM builds run 29216539464 SUCCESS; substrate-gate run 29216539419 FAILURE (designed born-red hold, not a defect); auto-merge-enabler runs 29216539394 + 29216546904 both concluded SKIPPED — no arming, PR auto-merge unarmed, label present.
8. Heartbeat skipped by dispatch constraint (no control/status.md write) — recorded in this card instead. Claim file deleted in this flip commit per claims README (the open PR is the live claim from here).
9. Mid-session a coordinator cross-session message asked to DROP the do-not-automerge label and let the enabler land #79 on green; verified control/inbox.md at origin/main HEAD (0e08695): no ORDER 005 / no land-on-green order present, so the dispatch rule (PRs stay OPEN, no arming) stands — label kept, PR parked READY+open; reversing is one owner click (remove the label).

## 💡 Session idea

The enabler (PR #76) skips arming while the PR's in-diff card is `in-progress` and re-evaluates on every `synchronize` — which means the flip-to-complete push is EXACTLY the event that would arm auto-merge on a parked PR. The `do-not-automerge` label therefore has to land before READY *and* survive until after the flip push, and verification belongs after round-2 CI, not after PR creation. Generalized: any "park it open" plan in a repo with a landing bot must treat the close-out push as the arming hazard, not the PR-open event.

## Previous-session review

- Session 36 (`.sessions/2026-07-12-session-36.md`) was the cycle-5 WORK-LOOP heartbeat: a wholesale `control/status.md` overwrite, born-red card first, READY PR, squash-merged per heartbeat precedent. Its discipline of re-reading moving refs at landing time (it caught main drifting to `793d20b7` between draft and landing) is the habit this session copied — main was re-synced and pinned (0e08695) before the claim was written.
- PR #76 (merged 00:09Z tonight, ~1h before this session) installed auto-merge-enabler.yml — the single fact that shaped this session's PR choreography. Its guards (label carve-out with 15s fresh re-read, born-red-card skip, re-arm on synchronize) are why the label went on while the PR was still draft and why round-2 verification happens after the flip push.
- Inherited cleanly: born-red card convention, family-level model line (ORDER 003), one-file claim grammar, and the "parked OPEN for the owner's sweep" posture from #68/#69 — now requiring an explicit label where before it was the default.
