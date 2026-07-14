# Session 46 — Tiltstone slice 5: LEVEL PACKS (web arcade)

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/tiltstone-packs`, started 19:04:11Z,
  flipped per `date -u` at the flip commit; base `claude/tiltstone-cells`
  @ `6238f3a` — PR #95 is OPEN, so this slice stacks on it per the
  standing default; GitHub retargets as the parents merge and the diff
  collapses to slice 5 alone; full chain: PR #97 ← #95 ← #93 ← #92 ← main)
- mission: the Tiltstone concept doc's growth-path item 5 — **Level
  packs**: "curate seeds the solver rates hard (long solutions, low
  slack) into named packs." Engine changes stay pure and ADDITIVE
  (v1.3.0 → v1.4.0): every pinned smoke value from PR #81/#92/#93/#95
  re-passed byte-identical; the pack data itself is PINNED in the engine
  and the smoke re-runs the curation to prove the pin honest.
- provenance: coordinator dispatch (Tiltstone lane, next slice per the
  concept doc roadmap). Inbox re-read at HEAD `8bac80a`: ORDERS 001–005
  only, all served — no unexecuted order, standing default applies.
  Collision check at session start: open PRs #85, #87–#90, #92, #93,
  #95 scanned — only #92/#93/#95 (this lane's own slices 2–4) touch
  Tiltstone; #96 (Clockwork Courier, opened mid-session) is the GBA
  breadth lane, no overlap. The only other Tiltstone claim at HEAD is
  `claude-tiltstone-slice.md` from the MERGED prototype (PR #81, stale
  — sweeper's job, untouched here).
  `control/claims/claude-tiltstone-packs.md` filed in the born-red
  commit (claim-before-build).
- landing posture: PR #97 opened READY at the born-red commit `35e874d`;
  **no merge, no auto-merge arming, no labels from this session** — the
  repo's auto-merge enabler may arm and land it server-side on green;
  that is expected. The substrate-gate red at the born-red push is the
  designed hold, cleared by this flip commit.
- 📊 Model: Claude (Fable family)

## What this session did

1. Claim + this card born red (`35e874d`), PR #97 opened READY
   immediately (base branch `claude/tiltstone-cells` so the diff shows
   slice 5 alone).
2. **Engine** (`engine.js`, v1.3.0 → 1.4.0, pure + additive — zero edits
   to any existing function): `difficulty(level)` rates what the solver
   already measured at generation time — par dominates (longer shortest
   line = harder), SLACK (`best - quota`: spare collectible gems beyond
   the demand) breaks ties downward, `score = par*1000 +
   max(0, 999-slack)`, labels MILD/TRICKY/STIFF/CRUEL by par.
   `curatePack(def)` scans the def's seed window deterministically
   (score desc, seed asc tiebreak) and keeps the `take` hardest.
   `PACK_DEFS` ships two recipes over the SAME 32-seed window (1..32):
   **GRANITE GAUNTLET** (levelIndex 0, base rules) and **DEEP CUTS**
   (levelIndex 4, slice-4 cells live). `PACKS` pins curatePack's own
   output verbatim (so the page never runs 64 BFS curations at load);
   `packById(id)` resolves the shipped ids. The pin is never
   hand-editable in practice: the smoke re-derives both packs and
   asserts JSON byte-equality.
3. **Shell** (`app.js` + `index.html`): pack picker (select + "Play
   pack", options straight from `E.PACKS`); `startPackStage` enters a
   stage = the generator on the curated seed at the pack's depth; N
   advances within the pack (final won card stays frozen; button
   disables); R / Restart restarts the STAGE (free-play restart
   unchanged); explicit seed/level loads exit pack mode; `hud-pack`
   line names the pack + stage; win message points at the next stage or
   "PACK CLEAR"; guarded per-pack progress persistence
   (`tiltstone-pack-<id>` = highest stage cleared); `?pack=ID&stage=S`
   (1-based) boots straight into a stage. Test API gains
   `loadPack`/`getPack`. Engine state stays authoritative and IMMEDIATE.
4. **Pure-Node smoke** (`smoke.mjs`) — **58/58 green, exit 0**. All 50
   PR-#81/#92/#93/#95 assertions re-passed with byte-identical pinned
   values (grid pin, quota=7 budget=10 salt=0 best=11, solution "RRR",
   12/12 sweeps, cue table f0 523/654/817/1277, slice-4 cell truths).
   The 8 new: difficulty pinned on seed 42 (par=3 slack=4 score=3995
   TRICKY); the difficulty law (labels by par, par dominates, low slack
   breaks ties); PACKS pin == curatePack re-run byte-for-byte for both
   packs (granite-gauntlet: seeds 1/3/8/10/29/32 · deep-cuts:
   12/7/18/20/4/21); curatePack deterministic; structure (take honored,
   seeds in-window, sorted hardest-first); every entry's solver line
   wins + stored rating == fresh generation (12/12 + 12/12); pinned
   pars granite 766555 / deep 887777, all ≥ 5 vs baseline par 3;
   packById resolves + unknown → null.
5. **Browser smoke** (`tools/web-smoke-tiltstone.mjs`, real Chromium) —
   **31/31 green, exit 0**. The 24 carried checks untouched and green
   (cue-log pins byte-identical). The 7 new: picker lists the pinned
   packs (free play = no active pack); a REAL select+click enters DEEP
   CUTS stage 1 (seed 12, HUD "DEEP CUTS 1/6"); the stage grid == the
   pure-Node generator byte-for-byte; stage 1 won by real key presses
   of its own solver line ("LLRRRRRR" → PERFECT, message points at
   stage 2/6); N advances to stage 2 = the next curated seed (12 → 7),
   byte-identical to pure Node; `?pack=granite-gauntlet&stage=2` boots
   straight into the stage (HUD "GRANITE GAUNTLET 2/6"); zero
   console/page errors end-to-end. Screenshots:
   `docs/proof/session-46-tiltstone-packs.png` (GRANITE GAUNTLET stage
   2 via the ?pack= boot) + `…-mid.png`.
6. `CONCEPT.md` growth item 5 marked ✅ SHIPPED (items 1–5 now all
   shipped; the only remaining rung, item 6 daily leaderboard, is
   flagged out-of-scope for static hosting in the doc itself — the
   Tiltstone concept tree is effectively scope-complete pending owner
   direction). `HOSTING.md` gained the `?pack=` query-param note; the
   runtime file set is unchanged (same four files — no packaging-lane
   follow-up needed).
7. `python3 bootstrap.py check --strict` — exit 0 at the flip tree
   (known pre-existing advisory `[claims-format] control/claims/
   order-005-scribe.md: no parseable claim bullet`, never
   exit-affecting). `.substrate/guard-fires.jsonl` restored before every
   commit (lane precedent: never committed). Heartbeat: dated dispatch
   section APPENDED to `control/status.md` in this commit, body
   untouched. Final inbox re-read at HEAD `8bac80a` before this flip:
   ORDERS 001–005 only, all served — unchanged.

## 💡 Session idea

`difficulty()` is cheap at generation time (the BFS already ran), which
makes the free-play level CHAIN self-balancing for free: `generateLevel`
could accept an optional difficulty floor and re-salt until
`difficulty(level).score` clears it, giving the daily chain a MONOTONE
ramp (LV n+1 provably rated ≥ LV n) instead of today's flat ramp of
parameter bumps — same determinism (the floor is a pure function of
levelIndex), zero new solver cost, and the pins for existing levels
survive by gating the floor behind a levelIndex the current chain never
reaches. (Deduped against prior cards: slice 1 = par-as-baseline, slice
2 = undo×par difficulty metric, slice 3 = shareable replay files via
resolveTrace, slice 4 = per-seed mechanic-attribution fingerprinting via
neutralized-BFS par deltas — this is difficulty-GATED GENERATION, a new
axis: using the rating to steer what gets generated, not to describe or
attribute what already exists.)

## Previous-session review

Session 45 (PR #95, new cell types) set this slice up cleanly: its
`difficulty` inputs (par, best, quota) were all already ON the level
object because slice 45 kept generation's solver bookkeeping intact, so
rating + curation needed zero engine surgery — only new pure functions;
and its level-4 generation meant DEEP CUTS could curate genuinely
different caverns (par 7–8) than the base game. One wart: its session
card's 💡 idea (mechanic-attribution fingerprints) is exactly the
curation signal packs want, but implementing it would have tripled this
slice's BFS cost (three neutralized re-searches per seed × 64 seeds),
so packs shipped on the cheaper par/slack rating — the fingerprint
remains the natural slice-6 upgrade if the owner wants themed packs
("grate-heavy", "ice-heavy") rather than difficulty-tiered ones. Its
24-carried-pins discipline again made the "additive only" claim provable
the moment the old suites went green.
