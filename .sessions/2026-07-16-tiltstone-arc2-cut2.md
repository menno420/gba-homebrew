# Session — Tiltstone arc 2, cut 2: «Hints from the solver»

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/tiltstone-arc2-cut2`, base
  **`claude/tiltstone-arc2-cut1`** @ `baf2ee3` — stacked on cut 1; started 20:30Z,
  `date -u`)
- mission: **Tiltstone arc 2 — the shareable daily** (docs/arcs/TILTSTONE.md, the
  five-cut plan opened by cut 1). This session builds **CUT 2 — «Hints from the
  solver»**. The solver already stores each cavern's shortest winning line
  (`level.solution`) — the arc-1 card noted it "doubles as a free hint system".
  Cut 2 makes that real, honestly: an opt-in **Hint** button surfaces the ONE next
  rotation of a shortest winning line *from the board as it stands* (re-running the
  same BFS `search` from `state.grid` against the remaining budget/quota, so it is
  honest after detours and undos, never a stale parrot of the stored line), and it
  is **spend-gated** — each hint taken dings the win card exactly like one over-par
  turn. Engine changes are strictly ADDITIVE (two new pure functions `hintFrom` /
  `hintedGrade`, zero edits to generation / rotation / settle / resolve / state), so
  every pinned smoke value from arc-1 slices 1–5 and cut-1 §14 re-passes
  byte-identical. No backend — pure static-hosting scope.
- **📊 Model:** [[fill: family-level model line, resolved at close-out]]
- landing posture: **DRAFT PR stacked on cut 1** (`claude/tiltstone-arc2-cut1`).
  Under the standing **2026-07-16 LANDING WALL** (PR ready-flips + auto-merge are
  classifier-denied for this seat), an honest draft-park is the terminal state.
  **The card is born red at `in-progress`** and stays a DRAFT — no ready-flip, no
  merge / auto-merge calls from this session. PRs #153–#166 untouched, no
  force-push.

## What shipped

*(what/why — cut 2 = solver hints.)* An opt-in, spend-gated hint layer over the
existing deterministic solver. **Why:** cut 1 gave the daily its distribution half
(share your line); cut 2 gives a stuck player a way forward that costs something —
the missing "out of ideas, never out of luck" nudge the CONCEPT names, without
trivializing par. **How (all ADDITIVE):**

1. **Born-red gate** — this card `in-progress` + claim
   `control/claims/claude-tiltstone-arc2-cut2.md` (first commit on the branch), PR
   opened **DRAFT** immediately, base `claude/tiltstone-arc2-cut1`.
2. **Pure engine** `games/web-tiltstone/engine.js` — a new
   `// solver hints (arc 2, cut 2)` section, ADDITIVE ONLY:
   - `hintFrom(state)` — the next rotation (`'L'`/`'R'`) of a shortest winning line
     FROM THE CURRENT board, or `null` on a terminal / unwinnable / malformed board.
     Re-runs the same BFS `search` from `state.grid` against the remaining budget for
     the remaining quota, so it stays honest after detours/undos.
   - `hintedGrade(used, par, hints)` — folds the hint count into `used` and reuses
     `grade`, so each hint dings the card like one over-par turn; `hints<=0`
     reproduces plain `grade`.
3. **Proof** `games/web-tiltstone/smoke.mjs` **§15** — pristine hint == stored
   solution first move, null on terminal/junk, hint-follow wins in exactly par
   (seed 42 + 12-seed sweep), off-line honesty (re-solve the actual board after a
   detour or honest null), and the `hintedGrade` spend-gating truth table.
4. **Shell** `games/web-tiltstone/app.js` + `index.html` — a run-scoped `hints`
   counter (reset with every run), a `doHint()` opt-in nudge (does NOT auto-rotate),
   the win grade spend-gated via `hintedGrade`, `h`/`H` + `#btn-hint` wiring, test
   API `getHints`/`hint`.

## 💡 Session idea

[[fill: one genuine forward idea, resolved at close-out]]

## 📊 Model

[[fill: family-level model line, resolved at close-out]]

## Known / honest gaps

- **The hint engine is fully headless-proven; the Hint-button UX is browser-only.**
  `smoke.mjs` §15 proves `hintFrom`/`hintedGrade` are deterministic and that
  following hints wins in exactly par — the load-bearing surface, in plain Node. But
  whether the button reads well and the message line is clear is owner-eye / Chromium
  smoke only, not CI-wired (as with cut 1).
- **No local browser run in this container.** The pure-Node smoke ran green here;
  the shell code is written against the existing seams (unchanged signatures) and is
  guarded so a terminal board degrades to an honest "no hint" message.

## Previous-session review

[[fill: honest review of cut 1 / PR #166, resolved at close-out]]

## PR / CI (filled at close-out)

- PR: [[fill: PR number + URL, base branch, born-red SHA, impl SHA, card-finish SHA]]
- CI: [[fill: rom-builds + substrate-gate posture at close-out]]
