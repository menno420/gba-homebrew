# Session 44 — Tiltstone slice 3: JUICE (web arcade)

> **Status:** `in-progress`

- date: 2026-07-13 (branch `claude/tiltstone-juice`, started 18:15:28Z per
  `date -u`, base `claude/tiltstone-par` @ `a072be8` — PR #92 is OPEN, so
  this slice stacks on it per the standing default; once #92 merges the diff
  collapses to slice 3 alone)
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

## Plan

1. Engine (additive, v1.1.0 → 1.2.0): pure `settleMoves(g)` (settle plus a
   per-cell move list) and `resolveTrace(g)` (resolve plus ordered
   fall/collect phases with intermediate grids) — final results asserted
   byte-identical to `settle`/`resolve` in the smoke.
2. New `games/web-tiltstone/juice.js` (dual-mode like engine.js): pure
   timeline builder (phases → keyframed schedule) + pure synth cue table
   (rotate / land / collect-by-chain / win / lose / undo — WebAudio recipes,
   data only) + a guarded browser-only player with mute.
3. Shell: engine state stays authoritative and IMMEDIATE (smokes and the
   "shell adds nothing" property depend on it); animation is a cosmetic
   replay of the trace, cancelled by any new input; honors
   `prefers-reduced-motion`; mute button persisted via guarded localStorage.
4. Extended engine smoke (trace consistency incl. the 12-seed sweep) +
   extended browser smoke (cue log order, animation settles to the true
   final render, reduced-motion skip, mute toggle) + screenshot proof.
