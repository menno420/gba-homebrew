# Session 45 — Tiltstone slice 4: NEW CELL TYPES (web arcade)

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/tiltstone-cells`, started 18:42:44Z,
  flipped 18:52:50Z per `date -u`, base `claude/tiltstone-juice` @
  `427f165` — PR #93 is OPEN, so this slice stacks on it per the standing
  default; GitHub retargets when the parents merge and the diff collapses
  to slice 4 alone)
- mission: the Tiltstone concept doc's growth-path item 4 — **New cell
  types**: "locked gems (collect adjacent to free), ice (slides sideways),
  one-way grates — each a pure `settle()` extension." Engine changes stay
  pure and ADDITIVE for existing content: every pinned smoke value from
  PR #81 / #92 / #93 must re-pass byte-identical (new cell codes live
  outside the gem range; level 0–3 generation draws the exact same RNG
  sequence).
- provenance: coordinator dispatch (Tiltstone lane, next slice per the
  concept doc roadmap). Inbox re-read at HEAD `97e0117` (file last touched
  `0f4a1f7`): ORDERS 001–005 only, all served (005 tally per PR #87/#89)
  — no unexecuted order, standing default applies. Collision check at
  session start: open PRs #85, #87–#90, #92–#94 scanned — only #92/#93
  (this lane's own slices 2–3) touch Tiltstone; #94 is the Brineward lane.
  The only Tiltstone claim at HEAD is `claude-tiltstone-slice.md` from the
  MERGED prototype (PR #81, stale — sweeper's job, untouched here).
  `control/claims/claude-tiltstone-cells.md` filed in the born-red commit
  (claim-before-build).
- landing posture: PR #95 opened READY at the born-red commit `7619586`;
  **no merge, no auto-merge arming, no labels from this session** — the
  repo's auto-merge enabler may arm and land it server-side on green; that
  is expected.
- 📊 Model: Claude (Fable family)

## What this session did

1. Claim + this card born red (`7619586`), PR #95 opened READY immediately
   (base branch `claude/tiltstone-juice` so the diff shows slice 4 alone).
   The substrate-gate red at the born-red push is the designed hold —
   verified in `bootstrap.py check --strict` locally: "HOLD (by design) …
   nothing to investigate" — cleared by this flip commit.
2. **Engine** (`engine.js`, v1.2.0 → 1.3.0): three new cell classes
   entirely OUTSIDE the 8 reserved gem codes — `ICE` (11), `LOCK0 + c`
   (12..19), `GRATE0 + o` (20..23, o = up/right/down/left). `settle` and
   `settleMoves` now share one `settleCore`: the v1.2.0 straight-drop
   column walk extended with grates (porous exactly when pointing DOWN —
   pieces fall through and never rest inside; every other orientation
   blocks like a wall), then deterministic ice slips (side + diagonal-below
   empty → slide, left before right, keep falling; cargo drops after it)
   iterated to a fixed point. Moves are per-piece NET movements emitted in
   the legacy order, so old content produces the byte-identical move list.
   `rotateGrid` turns a grate's arrow with the cavern (CW: up→right→down→
   left; 4×CW = identity, asserted). `findGroups` gem test is now
   `isGem(v)` — locked gems and ice never group. `resolve`/`resolveTrace`
   gained `unlockAround`: a collect popping orthogonally adjacent to a
   locked gem frees a normal gem of its color in place (event gains
   `unlocked: k` ONLY when k > 0, so lock-free event objects stay
   byte-identical; the collect phase carries the freed cells for the
   shell). Generation places ice 2 / locks 2 / grates 1 from levelIndex 4
   (LV 5+), drawn AFTER all existing RNG draws — levels 0–3 consume the
   identical stream, which is why every pinned level re-passed unchanged.
3. **Juice** (`juice.js`, v1.0.0 → 1.1.0, additive): new `unlock` synth
   cue (square sweep 392→587 Hz); `timeline` schedules it at the end of
   any collect step whose phase freed a lock. Old phases produce the
   identical schedule.
4. **Shell** (`app.js` + `index.html`): caged-gem rendering (dimmed gem +
   bars), glinting ice block, steel grate with orientation arrow (arrow
   highlighted when porous); the fall tween now interpolates x as well as
   y (a slipping ice visibly slides diagonally); unlocking cells flash a
   fading ring during the collect pop; hint line documents the LV 5+
   cells. Engine state stays authoritative and IMMEDIATE as before.
5. **Pure-Node smoke** (`smoke.mjs`) — **50/50 green, exit 0** at the flip
   tree. All 31 PR-#81/#92/#93 assertions re-passed with byte-identical
   pinned values (grid pin, quota=7 budget=10 salt=0 best=11, solution
   "RRR", 12/12 sweeps, cue table f0 x1=523 x2=654 x3=817 cap=1277). The
   19 new: encoding chars `* b ^ > v <` outside the gem range; grate
   one-way truth (through when down, rests otherwise, never rests INSIDE
   a porous grate); arrow rotation (CW down→left, CCW down→right, 4×CW
   identity); ice slips left, right-when-left-blocked, stays boxed-in,
   cargo drops, settleMoves nets the slip as ONE diagonal move that still
   reconstructs; locked gems never group, adjacent collect frees exactly
   one (`unlocked: 1`) and the freed gem cascades chain 2, non-adjacent
   collects leave locks intact with no `unlocked` field; resolveTrace ≡
   resolve on a kitchen-sink grid and the unlock cue schedules;
   paramsFor zeros before level 4; (seed 42, level 4) = salt 3 quota 14
   best 22 solution "LLLRL" contains all three cell types and its solver
   line wins; 12-seed level-4 sweep 12/12 winnable (worst salt 5).
6. **Browser smoke** (`tools/web-smoke-tiltstone.mjs`, real Chromium) —
   **24/24 green, exit 0**, first run. The 18 carried checks untouched and
   green (cue-log pin byte-identical). The 6 new: LV5 cavern generates
   with 2 ice / 2 locks / 1 grate on the board; the page's deep grid ==
   the pure-Node generator byte-for-byte; a real ArrowRight there ==
   pure-Node engine (grate turned, ice slid, everything settled); the new
   cells render (pixels change); the deep-cavern cue-log tail == the
   pure-Node timeline composition; zero console/page errors end-to-end.
   Screenshots: `docs/proof/session-45-tiltstone-cells.png` (LV5 board —
   caged gems, ice, arrowed grate visible) + `…-mid.png`.
7. `CONCEPT.md` growth item 4 marked ✅ SHIPPED (items 1–4 now all
   shipped; next unshipped rung is item 5, level packs). `HOSTING.md`
   unchanged — the runtime file set is the same four files.
8. `python3 bootstrap.py check --strict` — exit 0 at the flip tree (known
   pre-existing advisory `[claims-format] control/claims/
   order-005-scribe.md: no parseable claim bullet`, never exit-affecting).
   `.substrate/guard-fires.jsonl` restored before every commit. Heartbeat:
   dated dispatch section APPENDED to `control/status.md` in this commit,
   body untouched. Final inbox re-read at HEAD `97e0117` before this flip:
   ORDERS 001–005 only, all served — unchanged.

## 💡 Session idea

The three new cells make level DNA legible: a generated cavern can now be
FINGERPRINTED by which mechanics its shortest solutions actually exercise
— re-run the BFS with each cell class neutralized (grates all-porous,
locks pre-freed, ice slip disabled) and compare par; if par rises (or the
level dies) without, that mechanic is load-bearing for that seed. That
yields an honest per-seed mechanic tag ("this daily NEEDS the grate")
for free, straight from the solver — which is exactly the curation signal
growth item 5's level packs want, and deeper than difficulty alone.
(Deduped against prior cards: slice 1 = par-as-baseline, slice 2 =
undo×par difficulty metric, slice 3 = shareable replay files via
resolveTrace — this is mechanic ATTRIBUTION per seed, a new axis:
which rule carries the level, not how hard or how shareable it is.)

## Previous-session review

Session 44 (PR #93, juice) left slice 4 a genuinely easy landing: its
trace surface meant the new diagonal ice moves needed exactly ONE shell
line (interpolate x alongside y), and its honest cue log absorbed the new
`unlock` cue with zero new proof machinery — the browser smoke's
compose-in-pure-Node trick re-verified the whole deep-cavern audio path
for free. Its screenshots-as-proof habit also carried straight over. Two
warts: (1) it left `settle` and `settleMoves` as two hand-synchronized
copies of the same column walk — fine at two copies, but slice 4's grates
and slips would have meant FOUR synchronized loops, so this slice had to
merge them into one `settleCore` first (the refactor it deferred); (2) its
composeCues helper in the browser smoke hardcodes seed 42 level 0, so the
deep-cavern check had to inline its own composition rather than reuse it.
Both small; its 24-carried-pins discipline meant the settleCore merge was
proven byte-identical the moment the old suite went green.
