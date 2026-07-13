# Session — Drift Garden: playable slice on the mobile foundation (breadth program, night run)

> **Status:** `in-progress`

- date: 2026-07-13 (branch `claude/drift-garden`, started 03:02:49Z per `date -u`)
- mission: game #5 of tonight's breadth program — evolve the `games/mobile-foundation/` skeleton (PR #78) into the playable game its own CONCEPT.md names: **Drift Garden**, a touch-first ambient garden with real stakes. Tap plants motes, drag currents herd them together, clustered mature motes cross-pollinate into rarer tiers, harvest for essence — beat the 90 s dusk quota while predator wisps eat the garden. Adds seeded determinism (mulberry32, `?seed=N` or daily UTC seed), a win/lose round structure, and a second headless smoke that proves win, lose, and byte-identical determinism. Original IP; zero deps, no build step; still an installable offline PWA.
- provenance: ORDER 005 (owner night-run directive, 2026-07-13, scribed in `control/inbox.md` via PR #77): breadth program — multiple NEW small playable slices tonight. Base: main @ d87f9ad.
- heartbeat: SKIPPED by dispatch constraint (no `control/status.md` / `control/outbox.md` writes this session); recorded here instead.
- 📊 Model: fable-5 (family-level self-report, per ORDER 003)

## What this session did

1. This card + `control/claims/claude-drift-garden.md`, born red (claim before build; commit 80fdbf3). PR #84 opened READY immediately (https://github.com/menno420/gba-homebrew/pull/84); per this dispatch: no merge action, no auto-merge arming, no labels.
2. **Game rules, evolved in place** (`games/mobile-foundation/game.js`): every foundation guarantee kept byte-compatible with the existing smoke (fixed 60 Hz timestep + accumulator + spiral guard, touch-first input with pointer fallback, `visibilitychange` pause/resume that never replays hidden time, resize/DPR, HUD DOM mirror + `window.__gameState` with all original fields). Added on top:
   - **Seeded determinism** — mulberry32; seed precedence `?seed=N` → daily UTC-date seed. Zero `Math.random` calls remain; all randomness (mote velocity/hue jitter, wisp spawn timing/position, wind re-targeting every 600 steps) draws from the one stream, only inside `update()` or deterministic input handlers, so (seed + action sequence) → identical state. `seed`/`baseSeed`/`roundsPlayed` exposed.
   - **Round structure** — `phase` ∈ playing/won/lost (state + HUD + on-canvas banner); 5400 fixed steps (90 s) of daylight with remaining seconds on the HUD and a sky that literally darkens toward dusk; essence 0 → quota 100; win on quota before dusk, lose at dusk; terminal rounds freeze the simulation and a tap replants with seed = baseSeed + roundsPlayed.
   - **Plant / harvest** — tap empty ground plants a tier-1 mote (MAX_ENTITIES 200 kept); mature motes (age ≥ 300 steps, drawn with a halo) harvest on tap within radius + 12 px slop for 5/15/40 essence by tier; tapping an immature mote does nothing (tap still counted).
   - **Cross-pollination** — two mature same-tier motes within 40 canvas px merge into one tier+1 mote (max tier 3) at their midpoint, age reset; tier drives size/brightness/hue band and harvest value.
   - **Drag = herd** — the skeleton's repel nudge became the CONCEPT's current: capped gentle attraction toward the touch point (reach 120 px) so two motes can be shepherded into pollinating — EXCEPT wisps, which the current repels hard; 3 accumulated repels dissipate a wisp.
   - **Predator wisps** — from step 600, one spawns every 900±300 steps (seeded) at a screen edge, cap 3; a wisp homes on the nearest mote and eats it on contact (`motesLost`); dark-violet body + fading trail; `wispsAlive`/`wispsDissipated` tracked.
   - **Headless drivers** on `window.__game` (existing state/start/pause/resume/spawnAt kept): `step(n)` synchronous fixed steps bypassing rAF (renders once at the end, works hidden), `tapAt(x,y)`/`dragAt(x,y)` through the same plant/harvest/current code paths as real touches, `snapshot()` JSON-safe deterministic core (frames, seed, phase, essence, taps, wisp count, mote tiers + positions rounded to 2 decimals). Plus: a page that loads with `document.hidden` true now starts paused (restored-background-tab correctness — and the hook that makes scripted rounds rAF-free).
   - Shell: `index.html` title → "Drift Garden", `sw.js` CACHE → `drift-garden-v2`, manifest description de-skeletonized. No Makefile anywhere near this (ROM CI untouched).
3. **New game smoke** (`tools/mobile-foundation-smoke/game-smoke.mjs` + `run-game.sh` wrapper, same scratch-dir playwright-core + `executablePath` pattern as `run.sh`, port 8902): 11 assertions — seeded boot, real constructed-`TouchEvent` dispatch plants (the actual handler path, not the API), scripted win (pollination produces a tier-2 from two adjacent plants, harvest 21 tier-1 motes to essence 100 at frame 310 < 5400 → won), scripted loss (no gardening, 5400 steps → lost, with ≥1 wisp alive at step 1200 and 3 motes eaten), determinism (seed 7 twice → byte-identical 116-byte snapshots; seed 8 differs), pause/resume. Full run at the work commit, verbatim (server log lines omitted):
   ```
   game-smoke: chromium = /opt/pw-browsers/chromium-1194/chrome-linux/chrome
   game-smoke: url      = http://127.0.0.1:8902/index.html
   PASS  boot-seeded  booted=true phase=playing seed=42 quota=100
   PASS  touchevent-plants  taps 0 -> 1, entities 3 -> 4, lastTap={"x":60,"y":700}
   PASS  scripted-clock  hidden-boot paused=true, frames after step(310)=310
   PASS  pollination-tier2  tier2 motes=1, entities 23 -> 22 (pair merged)
   PASS  win-before-dusk  phase=won essence=100/100 frames=310 (<5400)
   PASS  wisps-hunt  wisps alive @step1200=1, motes eaten by dusk=3
   PASS  lose-at-dusk  phase=lost essence=0 frames=5400
   PASS  deterministic-same-seed  seed 7 twice -> identical 116-byte snapshots: {"frames":1500,"seed":7,"phase":"playing","essence":0,"taps":3,"wisps":1,"motes":[{"tier":1,"x":...
   PASS  seed-changes-world  seed 7 vs seed 8 snapshots differ (116B vs 116B)
   PASS  pause-on-hidden  paused=true frames frozen at 70 (recheck 70)
   PASS  resume-on-visible  paused=false frames 70 -> 93

   game-smoke: 11/11 assertions passed
   GAME SMOKE RESULT: PASS
   ```
4. **Existing skeleton smoke unmodified and still green** (`tools/mobile-foundation-smoke/run.sh`, zero edits to it or `smoke.mjs`) — the seed motes moved up-canvas (0.35 h) so its center tap keeps planting instead of grazing a mote, but its assertions were never weakened. Same run, verbatim (server log lines omitted):
   ```
   smoke: chromium = /opt/pw-browsers/chromium-1194/chrome-linux/chrome
   smoke: url      = http://127.0.0.1:8901/index.html
   PASS  boot  booted=true entities=3
   PASS  loop-ticks  frames 1 -> 38, renders 2 -> 39
   PASS  tap-changes-state  taps 0 -> 1, entities 3 -> 4, lastTap={"x":195,"y":400}
   PASS  dom-mirror  hud entities=4
   PASS  pause-on-hidden  paused=true frames frozen at 47 (recheck 47)
   PASS  resume-on-visible  paused=false frames 47 -> 70
   
   smoke: 6/6 assertions passed
   SMOKE RESULT: PASS
   ```
   Honesty notes: the game smoke's win/lose/determinism rounds run on a page booted with visibility overridden to hidden before load, so the rAF loop never interleaves with the scripted `step()` clock — that is what makes byte-identical determinism assertable at all; the touch proof is a constructed `TouchEvent` dispatched on the canvas (the real `touchstart` handler runs), and `run.sh`'s tap is Playwright's `touchscreen.tap` as before.
5. **Docs**: CONCEPT.md status → first playable slice shipped 2026-07-13 (+ one shipped-vs-growth-path line; sellability guess kept honest — one round loop deep, still foundation-value not a revenue bet); README.md "What works" + smoke section refreshed for the game and `game-smoke.mjs`.
6. `python3 bootstrap.py check --strict` pre-flip: exits 1 solely on this card's designed in-progress badge (born-red HOLD) plus the pre-existing advisory claims-format warning on `control/claims/order-005-scribe.md` (not this session's to fix); re-run green expected at the close-out flip.
7. Close-out: work committed and pushed (see PR #84 for the proof summary), CI conclusions recorded below, then this badge flipped complete as the final commit. Claim file left in place per tonight's precedent (morning sweep prunes).

## CI (branch claude/drift-garden, head = work commit)

- recorded at close-out: see PR #84 checks — substrate-gate, ROM builds, headless boot proof; conclusions noted in the PR proof section update.

## 💡 Session idea

The hidden-boot trick (override `document.visibilityState` before load so the game wakes paused at frame 0, then drive it purely with a synchronous `step(n)` API) turns any rAF game into a deterministic state machine without forking its loop — worth extracting as the standard recipe for every future web/mobile title here: ship `step/tapAt/snapshot` from day one and determinism regression tests cost one page-load each, no replay infrastructure needed.

## Previous-session review

- `.sessions/2026-07-13-mobile-foundation.md` (PR #78) is the card this session literally builds on, and it holds up: its skeleton's guarantees were strong enough that this session's game rules dropped in WITHOUT touching `smoke.mjs`/`run.sh` — the born-green promise of "observable state surface for headless proofs" was real, not aspirational. Two things it did especially well that paid off tonight: the honesty notes about what pause/resume proof actually simulates (copied verbatim into the new smoke's posture), and documenting the scratch-dir playwright-core pattern precisely enough to reuse blind. One gap worth naming: its skeleton drew from `Math.random` and spawned seed motes dead-center — both fine for a skeleton, but the first thing a playable, deterministic, tap-precise game had to remove; a foundation card could flag "known non-goals that the first real game must fix" explicitly so the next session budgets for them.
