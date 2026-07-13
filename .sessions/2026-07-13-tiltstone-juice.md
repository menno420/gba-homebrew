# Session 44 — Tiltstone slice 3: JUICE (web arcade)

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/tiltstone-juice`, started 18:15:28Z,
  flipped 18:29:58Z per `date -u`, base `claude/tiltstone-par` @ `a072be8`
  — PR #92 is OPEN, so this slice stacks on it per the standing default;
  once #92 merges the diff collapses to slice 3 alone)
- mission: the Tiltstone concept doc's growth-path item 1 — **Juice**:
  "animated falls/merges (the engine already emits per-chain collect events;
  the shell just needs tweening), synth audio." Engine changes stay pure and
  ADDITIVE (a fall/collect TRACE surface — zero edits to generation,
  rotation, settle, resolve, or state transitions, so every pinned smoke
  value from PR #81 and PR #92 must re-pass byte-identical). Audio is 100%
  WebAudio-synthesized — no sampled or licensed audio, ever (concept doc
  rule shared with the NDS tracks).
- provenance: coordinator dispatch (Tiltstone lane, next slice per the
  concept doc roadmap). Inbox re-read at HEAD `48be770`: ORDERS 001–005
  only, all served (005 tally posted per PR #87/#89) — no unexecuted order,
  standing default applies. Collision check at session start: open PRs
  #82–#92 scanned — only #92 (this lane's own slice 2) touches Tiltstone;
  the only Tiltstone claim at HEAD is `claude-tiltstone-slice.md` from the
  MERGED prototype (PR #81, flagged stale by session 43). No collision.
  `control/claims/claude-tiltstone-juice.md` filed in this commit
  (claim-before-build).
- landing posture: PR opened READY at this born-red commit; **no merge, no
  auto-merge arming, no labels from this session** — the repo's auto-merge
  enabler may arm and land it server-side on green; that is expected. No
  merge/approve/auto-merge calls from this session.
- 📊 Model: Claude (Fable family)

## What this session did

1. Claim + this card born red (`85da0f5`), PR #93 opened READY immediately
   (base branch `claude/tiltstone-par` so the diff shows slice 3 alone;
   GitHub retargets to main when #92 merges). Substrate-gate red at the
   born-red commit verified verbatim in run 29273800286: "HOLD (by design)
   … nothing to investigate" — the designed hold, as documented.
2. **Engine** (`engine.js`, v1.1.0 → 1.2.0, additive only): pure
   `settleMoves(g)` — settle plus a `{ v, from, to }` move list, grid
   asserted byte-identical to `settle` and reconstructible from the moves —
   and pure `resolveTrace(g)` — resolve plus an ordered phase script
   (fall / collect-per-group with chain numbers and intermediate grids),
   grid/collected/events asserted exactly `resolve`'s. Zero edits to any
   existing function.
3. **Juice** (`juice.js`, new, dual-mode, v1.0.0): pure `timeline(phases)`
   (sequential non-overlapping ms schedule; empty falls produce no step;
   fall time scales with the longest drop, capped) + pure `cueFor(name,
   chain)` synth table (rotate / land / collect with chain-rising pitch
   capped at ×4 / win / lose / undo — single-oscillator sweeps, 100%
   synthesized, no sampled audio ever) + guarded `makeAudio()` WebAudio
   player whose honest log records every cue whether audible or not.
4. **Shell** (`app.js` + `index.html`): engine state stays authoritative
   and IMMEDIATE; the replay (board sweep with fit-scale, gems easing down
   their columns, groups popping with rings per chain, terminal win/lose
   cue at schedule end) is cosmetic, cancelled by any new input with
   unfired cues FLUSHED so the per-move cue sequence is deterministic, and
   skipped under `prefers-reduced-motion` (cues still fire in order). Mute
   button + M key, persisted via guarded localStorage. Render split into
   `drawBoardTo`/`drawCellTo`/`updateHud` so tweens reuse the exact cell
   painters. Test API gained `getCueLog`/`isAnimating`/`isMuted`/`render`.
5. **Pure-Node smoke** (`smoke.mjs`) — **31/31 green, exit 0** at
   `bc5133d` (re-run at the final tree). All 24 PR-#81/#92 assertions
   re-passed with byte-identical pinned values (grid pin, quota=7
   budget=10 salt=0 best=11, solution "RRR", 12/12 sweeps). The 7 new:
   settleMoves identity + reconstruction, resolveTrace ≡ resolve on the
   solver line and across 38 turns of the 12-seed sweep, phase
   well-formedness, timeline determinism (6 steps / 950ms on the
   collecting rotation), cue schedule `[land,collect,collect,land,
   collect@x2,land]`, cue table (f0 x1=523 x2=654 x3=817 cap=1277).
6. **Browser smoke** (`tools/web-smoke-tiltstone.mjs`, real Chromium at
   `/opt/pw-browsers/chromium`) — **18/18 green, exit 0**, first run
   (re-run at the final tree). The 12 carried checks untouched and green.
   The 6 new: page cue log EQUALS the pure-Node composition of the exact
   inputs performed (`[rotate,land,undo,rotate,land,rotate,land,rotate,
   land,collect,collect,land,collect@x2,land,win]`); the replay settles to
   the plain render of the authoritative state (forced re-render is a
   byte-identical no-op); a frame sampled mid-flight differs from both the
   pre-input and settled boards; reduced-motion applies input instantly
   with zero animation and the same cue order; mute toggles, persists
   ("1" in guarded storage) and the log keeps recording while muted.
   Screenshots: `docs/proof/session-44-tiltstone-juice-win.png` +
   `…-win-mid.png` (the chain-1 triangle group mid-pop). Smokes run
   locally; CI wiring deferred like #79/#81/#92 (no ROM job touched).
7. `CONCEPT.md` growth item 1 marked ✅ SHIPPED; `HOSTING.md` runtime set
   now `index.html + engine.js + juice.js + app.js`. Follow-up flagged for
   the packaging lane: PR #85's `tools/package-web-arcade.sh` staging list
   needs `juice.js` once it lands.
8. Heartbeat: dated session-44 section APPENDED to `control/status.md`
   (`2f46353`), body untouched. Final inbox re-read at HEAD `48be770`
   before this flip: ORDERS 001–005 only, all served — unchanged.

## 💡 Session idea

`(seed, rotation line)` IS a replay file — determinism is total, so a
cleared cavern is fully described by two integers and a short string, and
the new `resolveTrace` turns that pair back into the full animated,
scored playback for free. A share URL like `?seed=N&replay=RRLR` could
play back anyone's clear as a spectated run (sweeps, falls, pops, cues),
which is the missing social half of the daily-seed hook: today players
can share the CAVERN, but not their LINE. Only the player's own line is
ever encoded — the solver's stays hidden, so par bragging rights survive.
(Deduped against prior cards: slice 1's idea was par-as-baseline from the
stored solution, slice 2's was the undo×par cross as a difficulty/curation
metric — this is playback/sharing of the player's OWN line, a different
axis: distribution, not scoring.)

## Previous-session review

Session 43 (PR #92, par + undo) set this slice up almost perfectly: its
insistence that engine states stay immutable snapshots is exactly why the
undo stack survived a full animation layer with zero changes, and its
"additive-only, re-pin everything" discipline gave slice 3 a mechanical
green bar to clear (all 24 of its asserts re-passed byte-identical under
v1.2.0). Its 💡 idea (undo×par curation) remains unclaimed and honest.
Two small warts it left: (1) `app.js` drew everything inside one
`render()` monolith writing straight to the page context, so slice 3 had
to cut the `drawBoardTo`/`drawCellTo` seam before it could tween anything
— a seam slice 2 could have pre-cut when it touched the win-card branch;
(2) its browser checks implicitly assume input effects are visible
SYNCHRONOUSLY (state read right after `keyboard.press`), which is a real
constraint on any future animation work — honored here by keeping engine
state immediate and animation purely cosmetic, but worth stating in the
card, which it didn't. Both cosmetic; the slice itself re-verified clean.
