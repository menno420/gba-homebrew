# Session 40 (release-packaging) — game-lab Track B

> **Status:** `complete`

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

1. This card + `control/claims/claude-release-packaging.md`, born red
   (claim before build; commit `d4f1590`). Checked `control/claims/` and
   the open PRs first: no overlap (tiltstone claim is stale post-merge,
   order-005-scribe is the scribe lane; #82/#83/#84 are game slices, not
   packaging). PR #85 opened READY at the born-red commit.
2. **`tools/package-web-arcade.sh`** (commit `5b2e239`): clean-rebuilds
   `dist/web/` + `dist/releases/` from the tree — stages runtime files
   ONLY per each game's hosting contract (Undertow: `index.html`+
   `game.js`; Tiltstone: `index.html`+`engine.js`+`app.js`; Drift Garden
   foundation: `index.html`+`game.js`+`manifest.webmanifest`+`sw.js`+
   both icons), generates the arcade landing page from an in-script
   heredoc (inline styles, no JS, no external requests, honest
   foundation-build note on Drift Garden), pins source epoch
   `2026-07-13 00:00:00 UTC` on every staged file/dir, zips with
   `LC_ALL=C`-sorted member lists under `zip -X` (python3-zipfile
   fallback included but unused — `/usr/bin/zip` present), prints
   sha256s. `sw.js` precache verified relative (`./`-prefixed), so the
   PWA works under the `drift-garden/` mount.
3. **Determinism proven**: two consecutive runs → `diff` of the two
   sha256 lists empty, byte-identical; then re-verified post-commit (a
   third run leaves `git status` clean on `dist/**`). Hashes below.
4. **Docs** (commit `5ff7598`): `docs/RELEASES.md` (living-ledger; table
   per artifact with version/bytes/sha256/source `main @ d87f9ad`/regen
   command; Lumen Drift v1.3 row with its still-pending owner Release
   click; gloamline/brineward "no cut yet" note; explicit
   excluded-because-unmerged list) + `docs/RELEASE-HOWTO.md`
   (owner-guidance; one-click steps for static host / itch.io / GitHub
   Release; automated-vs-owner-only stated plainly, 403 wall cited,
   workflow_dispatch release.yml named as a future slice and NOT
   created). Reachability via the new current-state ledger entry;
   pointer lines appended to both web `HOSTING.md`s +
   `games/mobile-foundation/README.md`. `dist/README.md` gained the
   web/releases section.
5. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge ("HOLD (by design)… nothing to
   investigate"); only other output is the pre-existing
   `claims-format` advisory on `control/claims/order-005-scribe.md`
   (advisory, never exit-affecting, not this session's file — left
   alone). CI at `5ff7598`: ROM builds green-track, substrate-gate red
   ONLY on `session-card-hold` (job log verified verbatim), enabler
   run green. No merge/label/arm action taken, per dispatch.

## Outcomes — what shipped

- `dist/web/` — host-ready static arcade (12 files: landing page +
  `undertow/` + `tiltstone/` + `drift-garden/`), upload-as-is.
- `dist/releases/` — four deterministic zips, pinned in
  `docs/RELEASES.md`:
  - `web-undertow-v1.0.zip` · 4,890 B ·
    `662f0e04a82dfa760f62bd2b47cd1054782a66d18467d0b23026b5978160dfcd`
  - `web-tiltstone-v1.0.zip` · 8,749 B ·
    `24c65a116e4a908fbb2046e22ea7a1c66e4098f142fb272f26eacb01b593b603`
  - `drift-garden-pwa-v0.1.zip` · 6,241 B ·
    `d446f90a6bbc75b88f9d4cf8e83024e1fddb0b211d68627b3791fc96a03e2604`
  - `web-arcade-v1.0.zip` · 21,340 B ·
    `6f92498cc1c852bcee87a99bf096088322bf4f73f61fe40b983ed486660a8a33`
- `docs/RELEASES.md` + `docs/RELEASE-HOWTO.md` — the ledger and the
  owner's one-click publish steps.
- Session closed 03:42:40Z per `date -u`; claim file deleted at this
  flip per `control/claims/README.md`.

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
