# Session — Tiltstone arc 2, cut 4: «Mechanic-fingerprint par deltas»

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/tiltstone-arc2-cut4`, base
  **`claude/tiltstone-arc2-cut3`** @ `7ad73ef` — stacked on cut 3; started `date -u`
  2026-07-16 21:20 UTC)
- mission: **Tiltstone arc 2 — the shareable daily** (docs/arcs/TILTSTONE.md, the
  five-cut plan opened by cut 1). This session builds **CUT 4 — «Mechanic
  fingerprints»**. Difficulty today reads a cavern by par (solution length, cut 2)
  and deceptiveness (undo×par, cut 3), but neither says WHICH of the slice-4
  mechanics — ice, locked gems, one-way grates — actually carries a level. Cut 4
  answers that: re-run the solver's own BFS with ONE cell class NEUTRALIZED (grate
  → all-porous, lock → pre-freed, ice → slip disabled) and read how par moves. If
  neutralizing a present class shifts par (delta ≠ 0) or kills the level (dead), that
  mechanic is load-bearing → an honest per-seed tag ("this daily NEEDS the grate").
  Engine changes are strictly ADDITIVE (three new pure functions `fingerprint` /
  `fingerprintTag` / `neutralizeClass`, zero edits to generation / rotation / settle
  / resolve / state — neutralization is a pure grid transform fed to `search` as a
  parallel input), so every pinned smoke value from arc-1 slices 1–5 and cut-1 §14 /
  cut-2 §15 / cut-3 §16 re-passes byte-identical. No generator change, no backend —
  pure static-hosting scope.
- **📊 Model:** Claude Opus 4.8 family · high · implementation — Tiltstone arc-2
  cut 4 (mechanic-fingerprint par deltas)
- landing posture: **DRAFT PR stacked on cut 3** (`claude/tiltstone-arc2-cut3`).
  Under the standing **2026-07-16 LANDING WALL** (PR ready-flips + auto-merge are
  classifier-denied for this seat), an honest draft-park is the terminal state.
  **The card is born red at `in-progress`** and stays a DRAFT — no ready-flip, no
  merge / auto-merge calls from this session. PRs #153–#168 untouched, no
  force-push.

## What shipped

*(what/why — cut 4 = mechanic-fingerprint par deltas.)* An honest, headless-proven
read of WHICH slice-4 mechanic carries a cavern. **Why:** cut 1 gave the daily its
distribution half, cut 2 a spend-gated way forward, cut 3 a deceptiveness read; cut 4
answers "what makes THIS daily tick?" — par measures the shortest line, but two par-5
caverns are not the same puzzle if one dies without its grate and the other barely
notices. **How (all ADDITIVE):**

1. **Born-red gate** — this card `in-progress` + claim
   `control/claims/claude-tiltstone-arc2-cut4.md` (first commit on the branch), PR
   opened **DRAFT** immediately, base `claude/tiltstone-arc2-cut3`.
2. **Pure engine** `games/web-tiltstone/engine.js` — a new
   `// mechanic fingerprint: per-class par deltas (arc 2, cut 4)` section, ADDITIVE
   ONLY:
   - `neutralizeClass(grid, cls)` — a pure grid transform returning a NEW grid with
     every cell of one class replaced by its neutral stand-in: `ice → STONE` (same
     dead-weight fall, no slip), `lock → GEM0+color` (pre-freed to a normal gem),
     `grate → EMPTY` (the one-way barrier removed). Byte-identical to the input on
     content with none of that class; never mutates its argument.
   - `fingerprint(level)` — `{ base, ice, lock, grate }` where `base` is the BFS
     shortest winning depth (== `par(level)`), and each class carries
     `{ present, par, delta, dead, loadBearing }`: re-run `search` on the neutralized
     grid, `delta = neutralPar − base` (or `dead` when quota is no longer reachable
     within budget). `loadBearing` = present AND (dead OR delta ≠ 0). Deterministic;
     absent classes are no-ops (delta 0, not load-bearing).
   - `fingerprintTag(level)` — the load-bearing mechanics, heaviest carrier first
     ("NEEDS grate", "NEEDS lock+grate"), or "no-mechanic" on a base cavern. Ties
     break in `FP_CLASSES` order; dead outranks any finite par shift.
   - `var VERSION` bumped `1.6.0` → `1.7.0`; the three fns added to the `return {}`
     export right after `deception, deceptionLabel`.
3. **dist sync** — `dist/web/tiltstone/{engine.js,app.js}` re-synced byte-identical
   to src via `cp` (no build script for the web target); `diff` empty.
4. **Proof** `games/web-tiltstone/smoke.mjs` **§17** — the base cavern (seed 42 lv0)
   fingerprints "no-mechanic" (all classes absent, base == par); `neutralizeClass`
   is proven a pure identity on class-free content; a **hand-built lock-dependent
   cavern** (pinned 8×8 grid, par 5) whose locked gem is a dead-weight PLUG the win
   needs — pre-freeing it kills solvability, so `lock.dead` / `loadBearing` and the
   tag reads "NEEDS lock"; a **real deep daily** (seed 42 lv4) whose grate is
   load-bearing (par drops 2 when removed → "NEEDS grate", ice/lock present but not
   tagged), pinned exactly; determinism + `base == par` across the deep 12-seed
   sweep. Full smoke green (105 PASS / 0 FAIL, engine v1.7.0).
5. **Shell** `games/web-tiltstone/app.js` — the win card's `gradeLine()` now appends
   the fingerprint tag via a new `mechTag()` helper, computed ONCE per (seed, level)
   and cached (fingerprint re-runs the BFS, too heavy for the per-frame render); a
   base cavern's "no-mechanic" adds nothing, so the tag surfaces only when a mechanic
   actually carries the daily.

## 💡 Session idea

**A cross-mechanic "carry map" that steers cut 5's monotone floor toward VARIETY, not
just height.** Cut 4 tags each cavern with which mechanic carries it; the deduped
next axis is *distribution across the daily chain*. Aggregate `fingerprintTag` over a
seed window and you get a carry-map — "this month's dailies are 60% grate-carried, 5%
ice-carried, 30% no-mechanic". Cut 5's planned monotone difficulty floor can then
re-salt not only for a higher `difficulty().score` but for an UNDER-represented
carrier, so the ramp climbs *and* rotates which mechanic it leans on — a chain that
is provably non-decreasing in difficulty and provably varied in mechanic, from one
extra pure predicate over cut 4's tag. Keeps cut 4's fingerprint the single source
and lets cut 5 own the curve.

## 📊 Model

Claude Opus 4.8 family · high · implementation — Tiltstone arc-2 cut 4
(mechanic-fingerprint par deltas).

## Known / honest gaps

- **The fingerprint engine is fully headless-proven; the tag render is browser-only.**
  `smoke.mjs` §17 pins `fingerprint`/`fingerprintTag`/`neutralizeClass` across the
  base cavern, a hand-built lock-dead cavern, a real grate-carried deep daily, and a
  12-seed determinism sweep — the load-bearing surface, in plain Node. But whether
  the `{NEEDS …}` tag reads well on the win card is owner-eye / Chromium smoke only,
  not CI-wired (as with cuts 1–3).
- **The tag is computed per-cavern, not cheap per-frame.** `fingerprint` re-runs the
  solver BFS up to three times (once per present class); the shell memoizes it per
  (seed, levelIndex) so the per-frame canvas render only reads the cache. The pure
  cost is bounded by the same BFS generation already runs at load.
- **Delta sign is honest, not folded.** Pre-freeing a lock or removing a grate often
  makes a level EASIER (negative delta) rather than harder; cut 4 reports the signed
  delta and treats any non-zero move (or a dead verdict) as load-bearing, which is the
  faithful "does this mechanic change the puzzle?" read — not a one-directional
  "par rises only" filter.
- **No local browser run in this container.** The pure-Node smoke ran green here; the
  shell wiring is additive against the existing `gradeLine()` seam (one appended
  segment, memoized).

## Previous-session review

[[fill: prior cut card review — cut 3 / PR #168]]

## PR / CI (filled at close-out)

[[fill: PR number + URL, base, born-red SHA, impl SHA, card-finish SHA]]
[[fill: CI per-check — ROM builds, substrate-gate, auto-merge-enabler]]
[[fill: smoke result — X PASS / 0 FAIL, §17 included]]
