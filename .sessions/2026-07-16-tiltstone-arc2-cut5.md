# Session — Tiltstone arc 2, cut 5: «The monotone difficulty ramp» (arc closer)

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/tiltstone-arc2-cut5`, base
  **`claude/tiltstone-arc2-cut4`** @ `0d04b09` — stacked on cut 4; started `date -u`
  2026-07-16 21:39 UTC)
- mission: **Tiltstone arc 2 — the shareable daily** (docs/arcs/TILTSTONE.md, the
  five-cut plan opened by cut 1). This session builds **CUT 5 — «The monotone ramp»,
  the ARC CLOSER**. The daily chain today ramps by PARAMETER (`paramsFor` bumps
  colors/gems/cells with `levelIndex`) but not by MEASURED difficulty: a generated
  LV n+1 can rate EASIER than LV n — seed 42 dips from par 7 (score 7992) at LV3 to
  par 5 (5991) at LV4. Cut 5 gives generation an OPTIONAL difficulty floor: build each
  level against a floor equal to the hardest `difficulty().score` seen so far, and the
  generator re-salts until a layout clears it → the chain becomes a PROVABLY
  non-decreasing ramp (`scores[i] >= floor[i] = max(scores[0..i-1]) >= scores[i-1]`).
  Engine changes are ADDITIVE and gated behind an opt-in flag — `generateLevel` gains
  an optional third `opts` arg; `opts.floor` absent means the accept path is
  byte-identical to before (same RNG stream, same first-valid salt), so **default
  output is unchanged and every pinned level from arc-1 slices 1–5 and cut-1 §14 /
  cut-2 §15 / cut-3 §16 / cut-4 §17 re-passes byte-identical**. Two new pure functions
  (`rampFloor`, `generateRamp`), zero edits to rotation/settle/resolve/state, no
  backend — pure static-hosting scope.
- **📊 Model:** Claude Opus 4.8 family · high · feature build — Tiltstone arc-2 cut 5
  (monotone difficulty floor)
- landing posture: **DRAFT PR stacked on cut 4** (`claude/tiltstone-arc2-cut4`).
  Under the standing **2026-07-16 LANDING WALL** (PR ready-flips + auto-merge are
  classifier-denied for this seat), an honest draft-park is the terminal state.
  **The card is born red at `in-progress`** and stays a DRAFT — no ready-flip, no
  merge / auto-merge calls from this session. PRs #153–#169 untouched, no
  force-push.

## What shipped

*(what/why — cut 5 = the monotone difficulty ramp, the arc closer.)* An OPTIONAL,
opt-in difficulty floor that turns Tiltstone's total determinism into a daily chain
that is provably non-decreasing in difficulty. **Why:** cuts 1–4 built the social loop
around the determinism (share your line, solver hints, deceptiveness curation, mechanic
fingerprints); cut 5 closes the arc by improving the CURVE itself — today's chain can
sag mid-run, which undercuts the "each day a little harder" promise a daily makes. The
floor is opt-in and defaults OFF, so nothing about today's output moves. **How (all
ADDITIVE):**

1. **Born-red gate** — this card `in-progress` + claim
   `control/claims/claude-tiltstone-arc2-cut5.md` (first commit on the branch), PR
   opened **DRAFT** immediately, base `claude/tiltstone-arc2-cut4`.
2. **Pure engine** `games/web-tiltstone/engine.js` — ADDITIVE ONLY:
   - `generateLevel(seed, levelIndex, opts)` — an optional third arg. When
     `opts.floor` is a finite number, the salt loop re-salts until
     `difficulty(candidate).score >= floor`; when absent (`opts` undefined/`{}`/
     `{floor:0}`) the accept path is byte-identical to before — the same RNG stream,
     the same first-valid salt returned — so no pinned level moves. An unclearable
     floor throws a floor-named error rather than returning an under-floor level (the
     re-salt is a genuine gate, not best-effort).
   - `rampFloor(scores)` — the floor the NEXT level must clear: the running max
     difficulty score of the prefix, 0 on empty (a no-op every level clears). Pure,
     non-decreasing in the prefix — the property that makes the ramp monotone.
   - `generateRamp(seed, count, opts)` — a chain builder. With `opts.monotone`, level
     i>0 is generated against `rampFloor(scores[0..i-1])` → a provably non-decreasing
     chain; without it (default) the chain is plain `generateLevel(seed, i)` per level,
     byte-identical to today's daily chain. Returns `{ levels, scores, floors, … }`.
   - `var VERSION` bumped `1.7.0` → `1.8.0`; the two fns added to the `return {}`
     export right after `neutralizeClass, fingerprint, fingerprintTag`.
3. **dist sync** — `dist/web/tiltstone/engine.js` re-synced byte-identical to src via
   `cp` (no build script for the web target); `app.js` untouched (identical). `diff`
   empty on both.
4. **Proof** `games/web-tiltstone/smoke.mjs` **§18** — `rampFloor` truths (0 on empty,
   running-max, order-independent); a **14-seed × 6-level byte-identity sweep** proving
   floor-off generation (`opts` absent / `{}` / `{floor:0}`) equals pre-cut-5 output
   exactly; the natural (floor-off) chain pinned and shown to genuinely DIP
   (LV4 5991 < LV3 7992), so the floor is not a no-op; the **monotone chain (seed 42
   ×6)** pinned exactly — scores `[3995,3995,4993,7992,8991,9991]`, floors
   `[0,3995,3995,4993,7992,8991]`, pars `[3,3,4,7,8,9]` — asserted non-decreasing,
   every level clears its applied floor, and it dominates the natural chain pointwise
   (the floor never lowers difficulty); LV0 byte-identical to the natural LV0 (floor 0
   is a no-op); determinism run-twice; and the honest floor-exhaustion throw. Full
   smoke green (119 PASS / 0 FAIL, engine v1.8.0).
5. **Shell** — `app.js` deliberately UNTOUCHED. The monotone ramp is a CHAIN-level
   generation concern (it re-salts across a chain of levels), not a per-cavern render
   like cut 4's tag, so surfacing it in the UI needs a progression refactor
   (`newGame`-from-level, a `?floor=` opt-in) that cannot be browser-verified in this
   container. The engine mechanism is the deliverable and is headless-proven by §18;
   the shell opt-in is flagged a browser follow-up (see Known gaps), mirroring the
   "engine headless-gated / shell surface browser-only" split cuts 1–4 used.

## 💡 Session idea

**A "target curve" generalization of the floor — swap the running-max scalar for any
pure non-decreasing target function of `levelIndex` and the same re-salt gate builds
ANY provable curve, not just the flattest monotone one.** Cut 5's `rampFloor` is one
policy (each level ≥ the hardest so far). But `generateLevel`'s floor gate is
policy-agnostic — it clears any numeric floor. Feed it `targetFloor(i) = base + i*step`
and you get a provable *linear* ramp; feed it a step function and you get "easy for 3
days, then a wall". The honest bound §18 already documents (a floor no layout clears in
64 salts throws) becomes the design knob: the steepest curve a seed can sustain is
exactly discoverable by pushing the target until the throw. Combined with cut 4's
carry-map idea, a future cut could target *difficulty AND mechanic variety* on the same
gate — one predicate, many curves, zero generator edits.

## 📊 Model

Claude Opus 4.8 family · high · feature build — Tiltstone arc-2 cut 5 (monotone
difficulty floor / the arc closer).

## Known / honest gaps

- **The ramp engine is fully headless-proven; the shell opt-in is browser-only, and
  deliberately not built this session.** §18 pins `rampFloor`/`generateRamp`/the
  `generateLevel` floor across byte-identity, the natural-chain dip, a pinned monotone
  chain, pointwise dominance, determinism, and the honest throw — the load-bearing
  surface, in plain Node. Wiring a `?floor=1` opt-in into the daily progression
  (`app.js` uses per-level `newGame`, not the ramp builder) needs a small progression
  refactor that has no in-container browser proof, so it is left as a flagged follow-up
  rather than shipped blind. Default output is guaranteed unchanged precisely because
  `app.js` is untouched.
- **The floor is a genuine gate with a real ceiling.** Re-salting is bounded by
  `MAX_SALT` (64); a floor no salted layout can clear THROWS rather than silently
  returning an under-floor level. Empirically the running-max policy sustains ~6 levels
  on seed 42 (LV6 wants score 9991 and 64 salts do not reach it → throws); the pinned
  chain is chosen within that bound. This is the honest cost of "provably
  non-decreasing" — the guarantee is real only while a clearing layout exists.
- **Floor-off is the default and is byte-identical, proven, not asserted by fiat.** The
  14×6 sweep in §18 compares `generateLevel(seed, lv)` against `{}`/`{floor:0}`/`undefined`
  and `generateRamp(seed, 6)` against the per-level chain — 0 mismatches. The optional
  parameter cannot move today's output because the floor check is skipped entirely when
  no floor is set.
- **No local browser run in this container.** The pure-Node smoke ran green here
  (119 PASS / 0 FAIL); there is no shell change to browser-verify.

## Previous-session review

- Prior cut card: `.sessions/2026-07-16-tiltstone-arc2-cut4.md` (**PR #169**, head
  `0d04b09`, arc-2 cut 4 — «Mechanic-fingerprint par deltas»). It shipped three pure
  functions in `engine.js` — `neutralizeClass(grid, cls)` (a pure grid transform that
  swaps one slice-4 cell class for its neutral stand-in: ice→STONE, lock→pre-freed gem,
  grate→EMPTY), `fingerprint(level)` (re-run the solver BFS on each neutralized grid and
  read the signed par delta / dead verdict per class), and `fingerprintTag(level)` (the
  load-bearing mechanics, heaviest carrier first, or "no-mechanic") — plus the
  load-bearing **§17** smoke (base cavern no-mechanic, a hand-built lock-dead cavern,
  seed-42 lv4 "NEEDS grate", a 12-seed determinism sweep), a memoized tag on the shell
  grade line, and engine bumped `1.6.0 → 1.7.0`. It is **draft-parked** under the
  standing 2026-07-16 landing wall — its card reads `complete` in content but the PR
  (#169) stays DRAFT. Its close-out CI showed `ROM builds` green (web-only diff) and
  `substrate-gate` red (main's #151 orphans + the born-red HOLD).
- **The additive precedent this cut extends — and closes.** Cuts 1–4 each added pure
  functions that only READ the deterministic engine (encode/decode/spectate, the solver
  as a hint source, a deceptiveness scalar, the solver as a fingerprint). Cut 5 is the
  one cut the plan flagged as *touching generation* — but it holds the exact same
  discipline: the touch is a single optional parameter whose absence is byte-identical to
  today, proven by a 14×6 sweep, so the "re-pin everything, add nothing that moves an old
  value" rule that carried arc-1 and cuts 1–4 holds through the closer. The proof-split is
  mirrored too: the ramp engine is headless-gated in §18; the daily-chain shell opt-in is
  flagged browser-only (and this time not built, since it needs a progression refactor with
  no in-container proof). With §18 the full smoke runs green at 119 PASS / 0 FAIL, engine
  v1.8.0, no prior value moved — **all five arc-2 cuts are now BUILT; the arc is
  growth-complete pending owner clicks (the standing landing wall).**

## PR / CI (filled at close-out)

- PR: **#[[fill: PR number]]** — [[fill: PR URL]] (DRAFT, base
  `claude/tiltstone-arc2-cut4` — stacked on #169; land order
  #153 → #166 → #167 → #168 → #169 → this). Draft-parked under the 2026-07-16 landing
  wall — no ready-flip, no auto-merge.
- SHAs: born-red gate `[[fill: claim+card SHA]]` (claim + card), impl
  `[[fill: impl SHA]]` (engine+smoke+dist+plan-doc+current-state), card-finish = this
  commit `[[fill: card-finish SHA]]`.
- CI: `ROM builds` (the one required per-PR gate — web-only diff, no GBA/NDS ROM source
  touched, expected green), `substrate-gate` **RED** — main's known #151 orphans + the
  born-red card HOLD (cut-1's card stays `in-progress` by design; inherited/designed),
  not a cut fault; `headless-boot` [[fill: headless-boot conclusion]]. See the report
  for the polled per-check conclusions.
- Smoke: the Tiltstone pure-Node smoke is not CI-wired (as with cuts 1–4); it ran green
  locally (`node games/web-tiltstone/smoke.mjs`, exit 0, **119 PASS / 0 FAIL**, §18
  included, engine v1.8.0).
