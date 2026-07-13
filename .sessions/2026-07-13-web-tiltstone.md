# Session — Tiltstone: turn-based gravity puzzle, web (breadth program, night run)

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/tiltstone-slice`, started 01:14:41Z, closed 01:22:24Z per `date -u`)
- mission: game #3 of tonight's breadth program — a NEW original web game distinct in genre from Undertow (PR #79, web arcade action) and the mobile foundation (PR #78): **Tiltstone**, a turn-based gravity puzzle. Rotate the cavern 90° CW/CCW; stones and gems fall; 3+ same-color orthogonal gems collect; hit the gem quota inside the rotation budget. Deliverables: `games/web-tiltstone/` (page + PURE engine module + one-page concept + hosting note), pure-Node engine smoke `games/web-tiltstone/smoke.mjs`, browser smoke `tools/web-smoke-tiltstone.mjs`. PR #81 parked per tonight's rule 2 (ORDER 005): opened READY, this session takes no merge action and applies no labels — the repo's auto-merge-enabler or the owner's morning sweep lands it.
- provenance: ORDER 005 (owner night-run directive, 2026-07-13, scribed verbatim in PR #77 → `control/inbox.md` on branch `claude/order-005-scribe` @ 0f4a1f7): "THE BREADTH PROGRAM starts now: multiple NEW small games tonight — each a playable prototype slice + a one-page concept (genre, loop, platform, sellability guess)." Base: main @ 0e08695.
- heartbeat: SKIPPED by dispatch constraint (no `control/status.md` / `control/outbox.md` writes this session — the coordinator routes hosting asks); recorded here instead.
- 📊 Model: Fable 5 (family-level self-report from this session's own harness, per ORDER 003)

## What this session did

1. This card + `control/claims/claude-tiltstone-slice.md`, born red (claim before build; commit 93b00f0). Checked `control/claims/` at HEAD + all open PRs first: no sibling game #3, no puzzle-genre overlap, "tiltstone" greps clean at 0e08695 — name kept.
2. **Tiltstone engine** (`games/web-tiltstone/engine.js` @ 090b579): ALL rules in one pure, DOM-free, dual-mode (browser global / Node `require`) module — seeded gen (mulberry32, used ONLY at gen; zero randomness afterwards), 90° rotation of the whole cavern, per-column gravity settle around walls, 3+ orthogonal same-color collect with cascade chains, win (quota) / lose (budget) — every function pure, states immutable. **Provable-solvability is built into generation**: `generateLevel(seed, levelIndex)` runs a BFS over the rotation tree (branching 2, depth ≤ budget 10, dedup) and only ships a salted layout whose quota (60% of proven-best, min 3) its own solver reaches; the winning line rides the level object as proof material. Daily seed = pure `"YYYY-MM-DD"` → `YYYYMMDD` mapping (engine never reads the clock).
3. **Page shell** (`index.html` + `app.js`): thin canvas render + input over the engine — no game rules, no animation loop (turn-based → repaint on demand). ←/A = CCW, →/D = CW, R restart, N next level, plus clickable buttons, Daily button, free-play seed box; `?seed=N&level=L`; colorblind-safe gem shapes; guarded localStorage best-level; `window.TILTSTONE` test API. Zero dependencies, no external requests, plain scripts (no ES-module `file://` CORS trap).
4. `CONCEPT.md` (genre, loop, platform, honest near-zero sellability as-is / daily-seed hook as the one differentiator, growth path) + `HOSTING.md` (static-only, CSP-safe, path-agnostic).
5. **Pure-Node engine smoke** (`games/web-tiltstone/smoke.mjs`, no browser, no deps) — ALL 19 GREEN, exit 0. Full run at 090b579:
   ```
   PASS: seed 42 level 0: pinned initial grid — grid matches 8x8 pin
   PASS: seed 42: pinned level params — quota=7 budget=10 salt=0 best=11
   PASS: initial state is at rest and merge-free — settle(grid)==grid, 0 groups
   PASS: rotation orientation (CW moves [0][0] -> [0][7], CCW -> [7][0])
   PASS: gravity settle: wall support vs open column — on-wall row=3, open-column row=7
   PASS: merge collects 3+ and CASCADES a chain — collected=6 events=[1:c0x3,2:c1x3]
   PASS: rotation decrements budget (used 0 -> 1) — used=1/10
   PASS: grid at rest after rotation (gravity settled)
   PASS: a merge/collect event fires on the solver line — first collect after rotation #3: c2x3@chain1,c1x3@chain1,c0x4@chain2
   PASS: collected count tracks events — collected=10 quota=7
   PASS: WIN: replaying the generator's solver line reaches 'won' — solution="RRR" -> status=won collected=10/7 used=3/10
   PASS: terminal state is frozen (rotate after win is a no-op)
   PASS: LOSS: anti-play burns the budget to 'lost' — line="RRLRLRLRLR" -> status=lost collected=0/7 used=10/10
   PASS: determinism: same seed + same rotations -> identical full state trace
   PASS: different seed -> different level
   PASS: purity: rotate() never mutates its input state
   PASS: generation sweep: 12 seeds, every solver line wins its own level — 12/12 won, worst salt=6
   PASS: level chain: (seed 42, level 1) generates solvable too — quota=7 best=11 solution="RLL"
   PASS: daily seed is a pure date mapping — 2026-07-13 -> 20260713
   SMOKE PASS: all assertions green (engine v1.0.0, seed=42, solution="RRR")
   ```
6. **Browser smoke** (`tools/web-smoke-tiltstone.mjs`, real Chromium at `/opt/pw-browsers/chromium` via the global playwright install, `NODE_PATH=/opt/node22/lib/node_modules`) — ALL 6 GREEN, exit 0, first run, no walls:
   ```
   PASS: page loads with zero console/page errors — clean
   PASS: test API exposed with the URL seed — seed=42 status=playing quota=7 budget=10
   PASS: ArrowRight rotates: engine state advanced (used 0 -> 1) — HUD says "TURNS 1/10"
   PASS: rotation changes the rendered canvas pixels — dataURL 34738B -> 36210B, differs
   PASS: rotation changes the grid
   PASS: page state == pure-Node engine state (same seed, same input)
   BROWSER SMOKE PASS: page shell is a faithful window onto the engine
   ```
   Smokes run locally; CI wiring deferred like Undertow's (workflows are kit-managed; no Makefile in `games/web-tiltstone/`, so the ROM-builds workflow skips the dir — `.github/workflows/` untouched).
7. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this card's in-progress badge ("HOLD (by design)… nothing to investigate"); no other findings. Re-run green after this flip (see PR).
8. PR #81 opened READY at the born-red commit per repo convention (conventions.md rule 1); no merge call, no auto-merge arming, no labels from this session — ORDER 005 rule 2 posture ("land on green where auto-merge arms" — the enabler workflow is the repo's own sanctioned lander; note this dispatch differs deliberately from #78/#79's `do-not-automerge` posture, and conventions.md rule 3 says never apply that label). Claim file kept in place while the PR is open tonight (the 72h expiry + morning sweep prune it; flagged for the sweeper).

## 💡 Session idea

The generate→solve→re-salt loop (BFS over a branching-factor-2 turn tree, ≤2^10 sims of a 64-cell grid, ~25ms/level) is a reusable pattern for every future puzzle title: **never ship a seed you can't beat yourself, and keep the winning line as a proof artifact.** It collapses "is procedural gen fair?" from a design debate into an assert — and the stored solution doubles as a free hint system, a par-scoring baseline, and a difficulty rating (solution length + slack) for curating level packs.

## Previous-session review

- The Undertow session (`.sessions/2026-07-13-web-undertow.md`, PR #79) set tonight's web-game conventions this session copied wholesale: `games/web-<name>/` layout, CONCEPT/HOSTING with `reference` badges, browser smoke under `tools/`, born-red choreography, and the honest-sellability register. Its card's key warning — the flip-to-complete push is exactly the event that re-triggers the auto-merge-enabler — is why this session double-checked its own posture; unlike #79's dispatch (hold open, label applied), this dispatch sanctions the enabler landing #81 on green, which also happens to match conventions.md rule 3.
- Divergence worth flagging: #79/#78 applied `do-not-automerge`; this session was explicitly instructed not to label. Both postures are owner-sanctioned tonight (ORDER 005 rule 2 covers both "land on green where auto-merge arms" and "leave OPEN"), but the morning sweep should expect the mixed state.
- Improvement over #79's proof style: Undertow's determinism evidence lives behind a browser; Tiltstone's primary proof is browser-free (pure engine in Node) with the browser smoke reduced to "the shell is a faithful window" — cheaper, tighter, and CI-wireable without Chromium.
