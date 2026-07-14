# Session — Undertow growth cut 1: daily seeded run + shareable score URL

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/undertow-daily`, started 01:40Z,
  flipped 01:42Z — both from `date -u` at write time)
- mission: build Undertow's lowest-risk named growth cut from
  `games/web-undertow/CONCEPT.md` — "Daily seeded run with a shareable
  score URL (`?seed=YYYYMMDD`)". A `?daily=1` mode derives the seed from
  the UTC calendar date as YYYYMMDD (read ONCE at boot, never inside the
  sim step — the sim stays wall-clock-free), plus a share mechanism: the
  gameover screen offers a copyable challenge link (`?seed=N&depth=M`,
  seed pinned explicitly so the link replays the same run on any day;
  `depth` is render-only, a title-screen score to beat). Static hosting
  only — no server, no leaderboard; the URL itself is the share.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-undertow-daily.md`, born red
   (claim before build), cut from main @ `6cb85ee`.
2. **Daily dive** (`games/web-undertow/game.js` v1.0.0 → v1.1.0):
   `dailySeed(d)` maps a UTC date to YYYYMMDD; `?daily=1` selects it at
   boot. Explicit `?seed=N` WINS over `?daily=1` — a shared link must
   replay the same run whenever it is opened, so share links always pin
   the seed, never the mode. The date feeds seed selection only; no new
   clock reads anywhere near the sim step (the update loop is untouched).
3. **Share mechanism**: gameover `S` copies `?seed=N&depth=M` (guarded
   `navigator.clipboard` with a visible-link fallback drawn on the
   canvas); `?depth=M` is render-only — "challenge: beat M m" on the
   title screen. Title shows "daily dive YYYYMMDD" in daily mode. New
   test API: `getMode` / `getChallengeDepth` / `getShareURL` /
   `getShareFeedback` / `dailySeedFor`.
4. `CONCEPT.md` growth-path line marked BUILT; `HOSTING.md` documents
   `?daily=1` / `?depth=M` and the share flow (arcade tile → `?daily=1`).
5. `tools/web-smoke-undertow.mjs` extended with 9 assertions: share-URL
   contents, `S` feedback, `dailySeedFor` derivation, `?daily=1` boot
   under an INJECTED FAKE `Date` (2030-06-07 → 20300607), daily run ≡
   explicit `?seed=20300607` run (same crashFrame=458/score=100m —
   the date-only-feeds-seed-selection proof), seed-beats-daily
   precedence, `?depth=123` render-only (crashFrame identical to bare
   seed, 823). Run twice locally against real Chromium: 17/17 PASS both
   runs, identical output.
6. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this
   card's in-progress badge (the designed born-red hold) + the
   pre-existing `claims-format` advisory on `order-005-scribe.md` (not
   this lane's file, advisory, left alone).
7. PR #110 opened READY immediately (no draft phase, no labels); no
   merge/approve/auto-merge calls from this session — the server-side
   enabler decides on green. Heartbeat appended to `control/status.md`
   (append-only dispatch section, PR number confirmed first).
8. Claim `control/claims/claude-undertow-daily.md` retired with this
   flip per the claims README (the open PR is the live claim from here).
9. Follow-up recorded (PR body + heartbeat): `dist/web`'s Undertow copy
   now trails main — the next `tools/package-web-arcade.sh` refresh
   bumps web-undertow to v1.1 and re-pins `docs/RELEASES.md`
   (`--verify` red until then is the session-55 designed reminder).

## 💡 Session idea

**Test the calendar by injecting the clock at the boundary, not by
adding a test-only date param to the product.** The determinism rule
here ("date feeds seed selection at boot, never the sim") is exactly
the kind of claim that rots if the game grows a `?today=` backdoor to
make it testable — the backdoor becomes a second date source someone
eventually reads mid-run. Instead the smoke stubs `Date` itself via
`addInitScript` BEFORE the page loads and asserts the real boot path
(`?daily=1` → seed 20300607), then proves equivalence against a plain
`?seed=20300607` page with the REAL clock: if the sim ever grew a
second clock read, the two runs would diverge and the equivalence
check — not a code reviewer — catches it. Generalized: when a rule is
"X is read only at the boundary", the test that keeps it true is
boundary injection + an equivalence run against a build where X never
varies, and the product ships zero test affordances for it.

## Previous-session review

- Session 55 (`.sessions/2026-07-14-web-arcade-pack.md`, PR #109) shipped
  the packaging refresh whose manifest-as-machine-assertion idea directly
  shaped this card's follow-up line: this PR changes `game.js`, so
  `--verify` going red on the next package run is the designed reminder
  working, and the version-tracks-content rule from that card is why
  this slice bumps game.js to v1.1.0 (different bytes, different claim).
  Its timestamp erratum (run `date -u` BEFORE writing the born-red card)
  was applied here from the first commit — both card timestamps were
  measured, not guessed. Its stand-down posture ("new breadth is
  owner-routed") is honored: this slice is not breadth — it is a named
  growth-path cut on an existing shipped game, dispatched on a live
  owner directive.
