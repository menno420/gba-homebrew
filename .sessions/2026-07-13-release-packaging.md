# Session 40 (release-packaging) — game-lab Track B

> **Status:** `in-progress`

- date: 2026-07-13 (branch `claude/release-packaging`, based on main @
  `d87f9ad`, started 03:35:57Z per `date -u`; session numbered 40 because
  38 is double-claimed by the gloamline-rematch and mobile-foundation
  cards and 39 is taken by open PR #83 — 40 is the next safe slot)
- mission: ORDER 005 item 4 — "Sellability candidates → Venture Lab
  marker; **keep the release-to-one-click packaging habit for everything
  finished**." Everything finished on main = the three web deliverables:
  Undertow (web arcade action, PR #79), Tiltstone (turn-based gravity
  puzzle, PR #81), and the Drift Garden mobile-foundation PWA (PR #78).
  Deliverables: `tools/package-web-arcade.sh` (deterministic rebuild of
  the whole packaging surface), `dist/web/` (one static dir = the whole
  arcade, host-ready as-is), `dist/releases/` (four versioned zips with
  pinned sha256s), `docs/RELEASES.md` (release ledger), and
  `docs/RELEASE-HOWTO.md` (the owner's exact one-click publish steps).
- provenance + scope fence: everything packaged derives from **main @
  `d87f9ad` only**. Open PRs #82 (Brineward 6+7), #83 (Deepcast) and #84
  (Drift Garden game slice) are unmerged and therefore EXCLUDED — the
  Drift Garden entry here is the foundation PWA currently on main, cut
  as v0.1 for that reason. No game code touched (docs pointers only);
  no workflow files created; no merge/label/auto-merge/tag/Release
  action from this session — the enabler or the owner's morning sweep
  lands the PR.
- heartbeat: SKIPPED by dispatch constraint (no `control/status.md` /
  `control/outbox.md` writes this session); recorded here instead.
- 📊 Model: fable-5 (family-level self-report from this session's own
  harness, per ORDER 003)

## What this session did

(born-red; filled at close-out)

## 💡 Session idea

Release packaging is only trustworthy if it is **deterministic enough to
re-derive**: fixed source epoch on every staged file, sorted zip member
order, `zip -X` (no extra attrs) — then any future session can run
`tools/package-web-arcade.sh` at the same tree and byte-match the
committed sha256s, which turns "are the committed release artifacts
really from this source?" into a one-command assert, exactly like the
byte-deterministic NDS builds in `dist/README.md`. The habit worth
keeping: never commit a release artifact without committing the script
that regenerates it byte-identically.

## Previous-session review

- The Tiltstone card (`.sessions/2026-07-13-web-tiltstone.md`, PR #81)
  is the model for this one: born-red claim-first choreography, honest
  no-merge/no-label posture under ORDER 005 rule 2, and — most useful
  here — its `HOSTING.md` enumerates the exact runtime file set
  (`index.html` + `engine.js` + `app.js`, docs and smokes are repo
  material, not runtime), which is precisely the contract this
  packaging session consumes; the same is true of Undertow's. That
  those hosting notes were written per-game before anyone asked for a
  bundle is why this slice is mechanical instead of archaeological.
- One honest gap it left: Tiltstone's card flags "CI wiring deferred"
  for its smokes and its claim file was deliberately left in place for
  the sweep ("flagged for the sweeper") — that claim is still present
  at `d87f9ad` alongside the merged PR, so the 72h expiry is doing the
  cleanup work the session close-out normally does. This session
  deletes its own claim at close instead.

## Close-out

(pending — filled at the flip)
