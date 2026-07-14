# Session 43 — Tiltstone slice 2: PAR SCORING + UNDO (web arcade)

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/tiltstone-par`, started 13:24:31Z, flipped 13:31Z per `date -u`, base main `d87f9ad`)
- mission: the Tiltstone concept doc's growth-path items 2 + 3 — its named
  retention hook: **par scoring** (the generator's BFS solver already knows a
  shortest winning line; grade the player's turn count against it on every
  cleared cavern) and **undo** (engine states are immutable snapshots — a
  history stack in the shell, free by construction). Engine changes are pure
  and ADDITIVE (`par`, `grade` — zero edits to generation/rotation, so every
  pinned smoke value from PR #81 must re-pass byte-identical). PR #92.
- provenance: work-ladder rung 3 (no unexecuted order in `control/inbox.md`
  at HEAD `d87f9ad` — ORDER 005 served per PR #87/#89; the seat-ender baton's
  two items are owner-gated or actively claimed). Standing owner direction:
  "continue expanding the games". Collision check at session start: Brineward
  slice 8 is actively built by session 42 (PR #91, branch
  `claude/brineward-slice-8`, pushing live at 13:19Z) — untouched here;
  Gloamline is scope-complete + owner-gated; Tiltstone is on main (PR #81
  merged `f8540b1`), its prototype claim's session closed complete, and no
  open PR or claim covers a Tiltstone slice 2. `control/claims/claude-tiltstone-par.md`
  filed in the first commit (claim-before-build).
- landing posture: PR #92 opened READY at the born-red commit; **no merge, no
  auto-merge arming, no labels from this session** — parked green for the
  owner sweep alongside #82–#91 (dispatch rail; matches the night-run set's
  posture and the documented "[Self-Approval]…Merge Without Review" wall).
- 📊 Model: Claude (Fable family)

## What this session did

1. Claim + this card born red (`bcb9e29`), PR #92 opened READY immediately.
2. **Engine** (`games/web-tiltstone/engine.js`, v1.0.0 → 1.1.0, additive
   only): pure `par(level)` — the stored solver line's length, documented and
   ASSERTED minimal (BFS `search` pushes records shortest-first, so the first
   quota-meeting record is a shortest winning line) — and pure
   `grade(used, par)` → PERFECT / GREAT / GOOD / CLEARED (diff 0/1/2/3+,
   defensively clamped at 0). Zero edits to generation, rotation, settle,
   merge, search, or state transitions.
3. **Shell** (`app.js` + `index.html`): history stack of immutable prior
   states + honest per-attempt undo counter; U key + Undo button (disabled
   when empty or won); undo works from a BURIED card (step back instead of a
   full restart) while a WON card stays frozen, mirroring the engine's own
   terminal rule; `PAR n` HUD span; win card and win message carry
   "PAR p — YOU u — LABEL (k undos)"; the BURIED card advertises U; test API
   gained `undo`/`getUndos`/`getHistoryLength`. The solver's LINE stays
   hidden — only its length is revealed (the par mechanic).
4. **Pure-Node engine smoke** (`games/web-tiltstone/smoke.mjs`) — 24/24
   green, exit 0 at `ce0d60a` (re-run at the final tree, exit 0). All 19
   PR-#81 assertions re-passed with byte-identical pinned values (grid pin,
   quota=7 budget=10 salt=0 best=11, solution "RRR", 12/12 sweep worst
   salt=6). The 5 new:
   ```
   PASS: par == stored solution length — par=3 solution="RRR"
   PASS: par is MINIMAL: BFS finds no shorter winning line (seed 42) — search min win depth=3
   PASS: grade truth table: 0->PERFECT 1->GREAT 2->GOOD 3+->CLEARED — [PERFECT,GREAT,GOOD,CLEARED,CLEARED], underflow clamps to diff 0
   PASS: replaying the solver line is graded PERFECT — used=3 par=3
   PASS: 12-seed sweep: every stored par is the true BFS minimum — 12/12 minimal
   ```
5. **Browser smoke** (`tools/web-smoke-tiltstone.mjs`, real Chromium at
   `/opt/pw-browsers/chromium`, `NODE_PATH=/opt/node22/lib/node_modules`) —
   12/12 green, exit 0, first run (re-run at the final tree, exit 0); the 6
   PR-#81 checks untouched and green. The 6 new:
   ```
   PASS: HUD shows the engine's par — "PAR 3" (engine par=3)
   PASS: U undoes the rotation (used 1 -> 0, grid byte-identical to initial) — used=0 undos=1 histLen=0
   PASS: undo on empty history is a no-op (undo count unchanged) — used=0 undos=1
   PASS: solver line by real key presses wins the level — solution="RRR" -> status=won used=3
   PASS: win message carries the grade (par turns after the undo -> PERFECT + undo count) — msg="QUOTA MET — PAR 3 — YOU 3 — PERFECT (1 undo). N for the next level."
   PASS: undo after a win is a no-op (won card frozen) — status=won used=3
   ```
   Screenshot: `docs/proof/session-43-tiltstone-par-win-card.png` (the graded
   win card, PAR HUD, disabled Undo on won). Smokes run locally; CI wiring
   deferred like #79/#81 (no Makefile in the dir — the required "ROM builds"
   job is unaffected by construction; local build of GBA/NDS ROMs neither
   needed nor attempted this session).
6. `CONCEPT.md` growth path items 2+3 marked ✅ SHIPPED (docs follow
   reality). `HOSTING.md` unchanged — the runtime file set is identical.
7. Heartbeat: dated session-43 section APPENDED to `control/status.md`
   (`7df9148`), body untouched; notes the expected trivial conflict with the
   unmerged seat-ender rewrite (PR #90) and flags the two stale claims from
   merged sessions (`claude-tiltstone-slice.md`, `order-005-scribe.md`) for
   the sweeper.
8. `python3 bootstrap.py check --strict` pre-flip: exit 1 ONLY on this
   card's designed born-red hold + the pre-existing `order-005-scribe.md`
   advisory (advisory-only, on main since PR #77). Final inbox re-read at
   HEAD `d87f9ad` before this flip: unchanged, no new orders.

## 💡 Session idea

Undo count as a difficulty probe: the shell now records take-backs per
attempt, and the engine knows par — so `(undos, used − par)` per cleared
level is a free, privacy-less telemetry pair that curates level packs better
than solution length alone (a level that wins at par but eats 6 undos is
HARD-deceptive; one that wins at par+2 with 0 undos is honest-medium). Store
the pair in the existing guarded localStorage next to best-level and the
future "level packs" growth item gets its difficulty rating for free — no
backend, no new proof surface. (Deduped: prior cards' ideas cover the
generate→solve→re-salt pattern itself, hint systems, and par as a baseline —
this is specifically the undo×par CROSS as a curation metric.)

## Previous-session review

- The Tiltstone prototype card (`.sessions/2026-07-13-web-tiltstone.md`,
  PR #81) predicted this slice precisely — its 💡 idea said the stored
  solution "doubles as … a par-scoring baseline" — and its proof-style call
  (primary proof browser-free in pure Node, browser smoke reduced to
  shell-faithfulness) held up perfectly under extension: adding 5 engine + 6
  browser asserts took no restructuring. One gap it left that this session
  closed: the prototype card claimed the solution is minimal by BFS order
  but never ASSERTED it — the new minimality checks (seed 42 + the 12-seed
  sweep) turn that latent claim into a pin, which par scoring now
  load-bears. One process wart it left behind: its claim file was
  deliberately parked "for the sweeper" but survived the sweep (PR #81
  merged, claim still at HEAD) — flagged in the session-43 heartbeat rather
  than deleted here, since pruning another session's claim mid-parked-sweep
  is the sweeper's call.
