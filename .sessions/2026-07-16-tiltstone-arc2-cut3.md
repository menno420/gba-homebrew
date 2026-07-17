# Session — Tiltstone arc 2, cut 3: «Undo×par deception curation»

> **Status:** complete

- date: 2026-07-16 (branch `claude/tiltstone-arc2-cut3`, base
  **`claude/tiltstone-arc2-cut2`** @ `cde0275` — stacked on cut 2; started, `date -u`)
- mission: **Tiltstone arc 2 — the shareable daily** (docs/arcs/TILTSTONE.md, the
  five-cut plan opened by cut 1). This session builds **CUT 3 — «Undo×par deception
  curation»**. Difficulty today is solution-length only (par). Cut 3 adds an honest
  *deceptiveness* read for a CLEARED cavern from the `(undos, overshoot)` pair:
  overshoot (turns past par) is visible forward effort; undos are hidden in-place
  backtracking — the thing that makes a level deceptive. A par win that ate many
  undos reads HARD-deceptive; par+2 with zero undos reads medium-honest. Engine
  changes are strictly ADDITIVE (two new pure functions `deception` /
  `deceptionLabel`, zero edits to generation / rotation / settle / resolve / state),
  so every pinned smoke value from arc-1 slices 1–5 and cut-1 §14 / cut-2 §15
  re-passes byte-identical. No generator change, no backend — pure static-hosting
  scope.
- **📊 Model:** Claude Opus 4.8 family · high · implementation — Tiltstone arc-2
  cut 3 (undo×par deception)
- landing posture: **DRAFT PR stacked on cut 2** (`claude/tiltstone-arc2-cut2`).
  Under the standing **2026-07-16 LANDING WALL** (PR ready-flips + auto-merge are
  classifier-denied for this seat), an honest draft-park is the terminal state.
  **The card is born red at `in-progress`** and stays a DRAFT — no ready-flip, no
  merge / auto-merge calls from this session. PRs #153–#167 untouched, no
  force-push.

## What shipped

*(what/why — cut 3 = undo×par deception curation.)* An honest, headless-proven
deceptiveness read over a cleared cavern. **Why:** cut 1 gave the daily its
distribution half and cut 2 a spend-gated way forward; cut 3 answers "how tricky
was that cavern, really?" — par measures the shortest line, but two par wins are not
equal if one ate six undos. Undos are hidden in-place backtracking (deceptive
effort); overshoot past par is visible forward effort (honest). **How (all
ADDITIVE):**

1. **Born-red gate** — this card `in-progress` + claim
   `control/claims/claude-tiltstone-arc2-cut3.md` (first commit on the branch), PR
   opened **DRAFT** immediately, base `claude/tiltstone-arc2-cut2`.
2. **Pure engine** `games/web-tiltstone/engine.js` — a new
   `// undo×par deception curation (arc 2, cut 3)` section, ADDITIVE ONLY:
   - `deception(undos, used, par)` — scalar `max(0, undos*2 - overshoot)` where
     `overshoot = max(0, used - par)`. Undos weigh double (hidden effort); visible
     overshoot discounts the deception. Pure integers, no I/O.
   - `deceptionLabel(undos, used, par)` — `"<tier>-<honesty>"`: tier from total
     effort `undos + overshoot` (`clean` / `medium` / `HARD`), honesty from the
     deception scalar (`honest` / `tricky` / `deceptive`).
   - `var VERSION` bumped `1.5.0` → `1.6.0`; both fns added to the `return {}`
     export right after `hintFrom, hintedGrade`.
3. **dist sync** — `dist/web/tiltstone/{engine.js,app.js}` re-synced byte-identical
   to src via `cp` (no build script for the web target); `diff` empty.
4. **Proof** `games/web-tiltstone/smoke.mjs` **§16** — a pinned truth table (7 rows:
   clean-honest, HARD-deceptive, medium-honest, medium-deceptive, HARD-tricky, two
   medium-tricky), plus determinism, monotonicity in undos, and 0-floor (negative
   undos and pure-overshoot both floor to 0). Full smoke green (93 PASS / 0 FAIL,
   engine v1.6.0).
5. **Shell** `games/web-tiltstone/app.js` — the win card's `gradeLine()` now renders
   `[E.deceptionLabel(undos, used, par)]` next to the grade, and every clear records
   the `(undos, overshoot, label)` triple through a new guarded `saveDeception()`
   helper (same try/catch style as `saveBest` — private-mode safe, degrades silent).

## 💡 Session idea

**A per-daily deception histogram feeding cut 5's monotone floor.** Cut 3 stores one
`(undos, overshoot)` triple per clear; the deduped next axis is *aggregation over
time*. Bucket each daily's deception scalar into a small local histogram
(`clean / tricky / deceptive` counts), surface it as a "your week was N% deceptive"
strip on the results screen, and — the composable part — feed the running
deceptiveness read into cut 5's planned monotone difficulty floor: a seed that
clears the community as HARD-deceptive can raise the floor it contributes, so the
generator's honest-difficulty signal learns from *actual* play, not just par. Keeps
cut 3's pure scalar the single source and lets cut 5 own the curve without cut 3
reaching into it.

## 📊 Model

Claude Opus 4.8 family · high · implementation — Tiltstone arc-2 cut 3 (undo×par
deception).

## Known / honest gaps

- **The deception engine is fully headless-proven; the label render is browser-only.**
  `smoke.mjs` §16 pins `deception`/`deceptionLabel` across the truth table plus
  determinism / monotonicity / 0-floor — the load-bearing surface, in plain Node.
  But whether the `[label]` reads well on the win card and the recorded triple is
  ever surfaced back to the player is owner-eye / Chromium smoke only, not CI-wired
  (as with cut 1 / cut 2).
- **No local browser run in this container.** The pure-Node smoke ran green here; the
  shell wiring is additive against the existing seams — `gradeLine()` gained one
  bracketed segment, and `saveDeception()` is guarded so blocked storage degrades
  silently (the recorded pair is write-only this cut; no read-back UI yet).

## Previous-session review

- Prior cut card: `.sessions/2026-07-16-tiltstone-arc2-cut2.md` (**PR #167**, head
  `46293b5`, arc-2 cut 2 — «Hints from the solver»). It shipped two pure functions in
  `engine.js` — `hintFrom(state)` (re-runs the same BFS `search` from the CURRENT
  board to surface the ONE next rotation of a shortest winning line, honest after
  detours/undos, `null` on terminal/junk) and `hintedGrade(used, par, hints)`
  (spend-gates the grade so each hint dings the card like one over-par turn) — plus
  the load-bearing **§15** smoke (pristine hint == stored first move, hint-follow
  wins in exactly par across a 12-seed sweep, off-line honesty, the spend-gate truth
  table), a shell Hint button + run-scoped hint counter, engine held at v1.5.0. It is
  **draft-parked** under the standing 2026-07-16 landing wall — its card reads
  `complete` in content but the PR stays DRAFT.
- **A clean additive precedent this cut echoes.** Cut 2's honesty came from *reuse*
  (`hintFrom` re-runs the solver's own BFS rather than parroting the stored line);
  cut 3 leans on the same discipline — `deception`/`deceptionLabel` are two NEW pure
  integer functions with zero edits to generation/rotation/settle/resolve/state, so
  the full smoke (arc-1 §1–§13 + cut-1 §14 + cut-2 §15 + new §16) runs green with the
  version bumped to v1.6.0 and no prior value moved. The proof-split is mirrored too:
  the scalar/label functions are headless-gated in §16, the win-card render + record
  are flagged browser-only. One honest divergence from cut 2's `games/*`-only claim:
  the stale `dist/web/tiltstone/{engine,app}.js` copies (last synced pre-arc-2 at
  v1.4.0) were re-synced byte-identical this cut, bringing dist current with src.

## PR / CI (filled at close-out)

- PR: **#168** — https://github.com/menno420/gba-homebrew/pull/168 (DRAFT, base
  `claude/tiltstone-arc2-cut2` — stacked on #167; born-red gate `ba4737a`, impl
  `bd7567b` engine+smoke+shell+dist, card-finish = this commit). Draft-parked under
  the 2026-07-16 landing wall — no ready-flip, no auto-merge.
- head SHA (impl push): `bd7567b658dc3776cbb008d7c6e7d814cc732e9a`.
- CI: `ROM builds` (the one required per-PR gate — web-only diff, no GBA/NDS ROM
  source touched, expected green; passed on the born-red commit, re-running on impl),
  `substrate-gate` **RED** — main's known #151 orphans + the born-red card HOLD
  (inherited/designed), not a cut fault; `auto-merge-enabler` **skipped** (DRAFT). The
  Tiltstone pure-Node smoke is not CI-wired (as with cut 1 / cut 2); it ran green
  locally (`node games/web-tiltstone/smoke.mjs`, exit 0, 93 assertions, §16 included).
