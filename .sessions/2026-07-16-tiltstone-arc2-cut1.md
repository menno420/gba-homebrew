# Session — Tiltstone arc 2, cut 1: «Share your line»

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/tiltstone-arc2-cut1`, base **main** @
  `478bf16`; started 20:04Z, `date -u`)
- mission: **Tiltstone arc 2 — the shareable daily** (docs/NEXT-MENU-2026-07-15.md
  § B1, the owner-recommended arc). Arc 2 is five explicitly-deduped, unbuilt
  ender 💡s that turn Tiltstone's total determinism into the daily/social hook the
  CONCEPT itself names as the game's real value (§ Sellability, "the Wordle-shaped
  hook"). This session opens the arc with the highest-value cut, **CUT 1 —
  «Share your line»**: `?seed=N&level=L&replay=RRLR` playback. Determinism is
  total, so a cleared cavern is fully described by `(seed, levelIndex, line)` — the
  new code turns that triple back into the full scored, animated `resolveTrace`
  playback for free. Only the player's OWN line is ever encoded; the solver's line
  stays hidden, so par bragging rights survive. Engine changes are strictly
  ADDITIVE (new pure functions only, zero edits to generation / rotation / settle /
  resolve / state), so every pinned smoke value from slices 1–5 re-passes
  byte-identical. No backend — pure static-hosting scope.
- **📊 Model:** Claude Opus 4.8 family · high · task-class: web-gameplay feature —
  Tiltstone arc-2 cut 1 (shareable line)
- landing posture: **DRAFT PR on main**. Under the standing **2026-07-16 LANDING
  WALL** (PR ready-flips + auto-merge are classifier-denied for this seat), an
  honest draft-park is the terminal state — matching how the Underroot arc (PRs
  #155–#165) parked. **The card stays `in-progress`** — flipping to `complete` is
  what would release auto-merge, so it stays red under the wall; no ready-flip, no
  merge / auto-merge calls from this session. PRs #153–#165 untouched, no
  force-push.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-tiltstone-arc2-cut1.md`, first commit on the branch), PR
   opened **DRAFT** immediately, base `main`.
2. **The arc-2 plan doc** `docs/arcs/TILTSTONE.md` — the five cuts in build order,
   each with its ender-card provenance and a headless proof strategy, CUT 1 marked
   BUILT and the other four planned. Follows the `docs/arcs/UNDERROOT.md` arc-doc
   convention (Status badge, per-slice rows).
3. **The pure share layer** in `games/web-tiltstone/engine.js` (v1.4.0 → 1.5.0,
   ADDITIVE ONLY — a new `// shareable line (arc 2, cut 1)` section, zero edits to
   any existing function):
   - `isReplayLine(s)` — is `s` a legal rotation line (`/^[LR]*$/`, within a
     64-char hostile-URL cap)?
   - `normalizeLine(s)` — uppercases, drops every non-L/R char, caps length, so a
     hand-typed or arrow-glyph share degrades to its legal core rather than
     throwing.
   - `encodeShare({seed, levelIndex, line})` → the canonical query fragment
     `seed=N&level=L&replay=RRLR` (`level` omitted when 0 — the daily case — so the
     URL stays short); throws on an illegal line so a share is never silently
     corrupted at creation.
   - `decodeShare(src)` — decodes a `?a=b&c=d` / `a=b&c=d` string, a
     URLSearchParams-like object, or a plain `{seed,level,replay}` bag → `{seed,
     levelIndex, line}` with the line normalized, or `null` when there is no usable
     `seed`+`replay` pair; malformed `%`-escapes degrade to null, never throw.
   - `spectate(seed, levelIndex, line)` — the payoff: generates the cavern and
     applies the line one rotation at a time, capturing per step the
     `resolveTrace` phases (what the shell tweens) and the post-rotation state,
     stopping early if the game reaches terminal (a won cavern freezes, extra
     rotations ignored — exactly the live shell). Pure — reuses
     `newGame`/`rotate`/`resolveTrace`, so the reconstructed run is byte-identical
     to the sharer's own play.
4. **Shell** `games/web-tiltstone/app.js` — the player's own line is recorded (a
   string appended on each `act`, popped on `undo`, cleared on reset / restart /
   pack-stage), a **Share button** copies the `?seed&level&replay` URL of the
   current line (guarded clipboard, honest fallback message showing the URL), and a
   boot `?replay=` triggers **spectate mode**: the shared line auto-plays
   rotation-by-rotation on the existing juice animation to its win/lose card. Test
   API gained `getLine`/`getShareURL`/`isSpectating`/`spectate`.
5. **`index.html`**: a **Share** button on the controls row + a `?replay=` /
   spectate hint line.
6. **Pure-Node smoke** `games/web-tiltstone/smoke.mjs` — a new **section 14
   (arc 2 cut 1)**: encode/decode round-trips, illegal-line rejection +
   normalization, `level=0` omission, `decodeShare` on all three input shapes +
   null on junk, and the load-bearing proof — `spectate(seed, level,
   solverLine).final` is byte-identical to `replay(newGame(...), line)` and each
   step's trace grid equals the authoritative post-rotation grid, across the
   seed-42 solver line and the 12-seed sweep; a shared winning line replays to
   `won`; the solver's own line is never emitted by any share function. All
   slice-1..5 assertions re-pass byte-identical under v1.5.0.
7. **`CONCEPT.md`** growth path gains an "Arc 2 — the shareable daily" block; cut 1
   marked ✅ SHIPPED.
8. Rails held: PUBLIC repo, original code only (no Nintendo-derived material),
   zero pokemon-mod-lab content, no `.substrate/` writes; claim/card cite the base
   SHA (`478bf16`); all timestamps `date -u`.

## 💡 Session idea

**A share is a challenge, not just a replay — the deduped next axis is the
_ghost race_.** CUT 1 makes `(seed, levelIndex, line)` a spectatable artifact; the
natural extension keeps the determinism spine but flips the verb from *watch* to
*beat*: because the shared line's grade is a pure function of its length against
the level's par (`grade(line.length, par(level))`), a received share carries a
target the receiver can be scored against **without a backend** — load the shared
seed, play your own line, and the win card reads "you matched their line / beat it
by 2 / they beat you by 1" purely from the two integers. It is the exact
Wordle-share loop (you don't watch someone's board, you get told the shape of
their result and try to top it) and it composes with cut 2's hint idea (a share
could optionally carry "solver par N" as the ceiling) without either owning the
other. (Deduped against the arc's five ender cards: this is neither the raw
playback of cut 1, nor the solver-hint of cut 2, nor the undo×par curation of
cut 3, nor mechanic-fingerprinting of cut 4, nor difficulty-gated generation of
cut 5 — it is _competitive scoring of one shared line against another_, a
distribution-plus-scoring cross none of them names.)

## Known / honest gaps

- **The pure share layer is fully headless-proven; the shell spectate/Share UX is
  browser-only.** `smoke.mjs` section 14 proves the encode/decode/spectate
  functions are deterministic and byte-identical to a live replay — that is the
  load-bearing surface and it runs in plain Node. But *whether the Share button's
  clipboard copy fires and the spectate animation reads well* is exercised only by
  `tools/web-smoke-tiltstone.mjs` (real Chromium) and by eye — that browser smoke
  is **not wired into CI** (as it wasn't for slices 3–5), so it is a local/owner
  check, not a gate.
- **No local browser run in this container.** The pure-Node smoke ran green here
  (`node games/web-tiltstone/smoke.mjs`, exit 0); the Chromium browser smoke was
  not run in-container. The shell code is written against the existing
  `act`/`startReplay`/test-API seams (unchanged signatures) and is guarded so a
  blocked clipboard or a malformed `?replay=` degrades silently for players.
- **Clipboard is best-effort.** `navigator.clipboard` is guarded; where it is
  absent or blocked (insecure context, `file://` on some browsers) the Share
  button falls back to writing the full URL into the message line for manual copy —
  the URL is always *shown*, the auto-copy is the bonus.

## Previous-session review

- Prior arc-neighbour card: `.sessions/2026-07-16-underroot-slice-11.md`
  (**PR #165**, head `82e5c08`, Underroot arc slice 11 — audio + polish → v1.0).
  It closed the 11-slice Underroot A2 arc with a pure PSG cue + ambience decision
  layer (`ur_sim.{h,c}`), a host mirror (`tools/check-underroot.py` `prove_audio`,
  11 cases), additive telemetry 54–58, and CI proof 7 — and it draft-parked under
  this same 2026-07-16 landing wall.
- **What this cut borrowed from #165, verbatim:** the draft-park convention.
  #165's card stays `in-progress` on purpose ("honest draft-park is the terminal
  state … flipping to complete is what would release auto-merge"), its PR is DRAFT,
  and its close-out is nonetheless *complete in content* (What shipped, 💡, gaps,
  prev-review, Model). This card matches that shape exactly — a fully written
  close-out held red at `in-progress` by the wall, not by unfinished work.
- **Concrete strength observed reviewing the #165 diff:** its **strict-additive
  telemetry discipline** (`UR_T_SPARE` stayed 53; only new indices appended) is the
  same contract that makes *this* cut safe on the web side — every engine change is
  a NEW exported pure function with zero edits to the existing generation /
  rotation / settle / resolve surface, so all of slices 1–5's pinned smoke values
  re-pass byte-identical under v1.5.0 (the web analogue of "no earlier
  `--assert-watch` index moved"). #165's honest-gaps habit (proving the pure
  decision layer while flagging the taste/visual parts as owner-eye-only) is
  mirrored here: the share *functions* are gated, the share *UX* is flagged
  browser-only.
- **Its `📊 Model:` three-field form** (`Claude Opus 4.8 family · high · task-class`)
  is mirrored here at family level. #165's "no local NDS build → rely on CI"
  posture is the same shape as this cut's "browser smoke not in CI → pure-Node smoke
  is the gate" posture, one platform over.

## PR / CI (filled at close-out)

- PR: **#166** — https://github.com/menno420/gba-homebrew/pull/166 (DRAFT, base
  `main`; impl commits `c654e01` engine+smoke / `645fc74` shell+html / `f2b50f8`
  docs, born-red gate `f7d634a`). Draft-parked under the landing wall — no
  ready-flip, no auto-merge.
- CI: `rom-builds` (the one required per-PR gate — web-only diff, no GBA/NDS ROM
  source touched, so expected green), `headless-boot` (workflow_dispatch only,
  does not run per-PR), `substrate-gate` **RED (inherited)** — main's 5 known
  #151-orphaned docs + the born-red card HOLD, not a cut fault. The Tiltstone
  pure-Node smoke is not CI-wired (as slices 3–5); it ran green locally.
