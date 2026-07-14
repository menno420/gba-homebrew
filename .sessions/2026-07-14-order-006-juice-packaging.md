# Session — ORDER 006(a): juice.js packaging follow-up (web-tiltstone v1.1, arcade bundle repin)

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/order-006-juice-packaging`, started
  11:27Z — from `date -u` at write time, BEFORE this card was written)
- mission: serve the standing packaging follow-up now that the Tiltstone
  stack (#92/#93/#95/#97) has landed on main (HEAD `b0d2274`,
  `games/web-tiltstone/juice.js` present): add `juice.js` to the
  Tiltstone stage list in `tools/package-web-arcade.sh`, bump
  web-tiltstone v1.0 → v1.1 and the arcade bundle v1.2 → v1.3,
  regenerate `dist/web/` + `dist/releases/`, re-pin `docs/RELEASES.md`,
  and prove `--verify` green. The follow-up was recorded in three
  places by #109/#130 (script header, RELEASES.md badge, #93's note) —
  this slice retires all three.
- provenance: ORDER 006 (EAP final-day closeout dispatch, #132) —
  the juice.js packaging follow-up slice, dispatched by the game-lab
  coordinator post-Tiltstone-stack landing.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.
- landing posture: PR opened READY on main; no merge/approve/
  auto-merge calls from this session — the server-side enabler decides
  on green.

## What this session did

(filled at the flip)

## 💡 Session idea

**A follow-up written into the failing path is the only follow-up that
can't be forgotten — but it should also be written into the SUCCESS
path of whoever lands the dependency.** The #109 design made
`--verify` the reminder: once juice.js landed, the Tiltstone staged
bytes would drift and the manifest assertion would fail loudly on the
next repin attempt. That worked — but only for the lane that happens
to run the packaging script. The stack-landing lane (#93/#97) merged
juice.js and walked away green, because nothing in ITS checklist said
"you just invalidated a downstream package." General rule for any
repo with committed derivatives: when a source change is known in
advance to obsolete a derivative, put the tripwire in BOTH lanes —
the derivative's verifier (done here) AND the source PR's merge note
as an explicit named follow-up with the exact command to run. The
cheapest version: the source PR body carries the downstream one-liner
(`tools/package-web-arcade.sh && --verify`), so the landing lane can
either do it in-stack or open the follow-up issue in the same breath.

## Previous-session review

- Prior work: the ORDER 006 Tiltstone stack landing —
  #92 (par + undo) ← #93 (juice) ← #95 (cell types) ← #97 (level
  packs), squash-merged onto main ending at `b0d2274`. Verified at
  HEAD: all four slices' runtime files are present
  (`engine.js`/`app.js`/`juice.js`), `index.html` loads `juice.js`
  (line 91), and `games/web-tiltstone/HOSTING.md` was correctly
  updated to name the four-file runtime contract — which is exactly
  what makes this packaging slice mechanical rather than
  investigative. Good baton discipline: the stack left the standing
  follow-up notes (#109's script header + RELEASES.md badge) intact
  and accurate, so this slice's entire scope was pre-specified. One
  nit: the stack's claim files (claude-tiltstone-juice/-cells/-packs/
  -par) are still in `control/claims/` post-merge — served claims
  another sweep will need to prune (out of this slice's scope; noted
  as a follow-up, not fixed here).
