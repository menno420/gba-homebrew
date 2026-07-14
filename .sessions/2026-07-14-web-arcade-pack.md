# Session 55 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/web-arcade-pack`, 00:05Z → 00:36Z)
- **⚑ Self-initiated:** rung-4 slice on coordinator direction — no
  inbox ORDER names it. Scope: SUPERSEDE the stale release-packaging
  PR #85 (conflicted against main; packaged the pre-#84 Drift Garden
  foundation and pre-dates every merge after `d87f9ad`). Per
  etiquette, #85 (another lane's PR) was not touched, closed, or
  commented on — this PR states "supersedes #85 pending owner
  disposition" and the owner disposes of it.
- inbox check: control/inbox.md at HEAD read fully — ORDERS 001-005
  only; no new unserved ORDER. No preemption.
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #109 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.
- erratum, self-caught at the flip: the born-red card's "started"
  time was pre-written as 00:20Z against a real clock of 00:05Z —
  the sessions-45/47 erratum, third occurrence; corrected here.
  Timestamps come from `date -u` AT WRITE TIME, and the pre-written
  guess happened in the one file where the rule wasn't a checklist
  line yet: the born-red card itself. Rule extended: run `date -u`
  BEFORE writing the card, not just the heartbeat.

## 💡 Session idea

**A packaging artifact goes stale the moment its sources move — so
make the manifest a MACHINE assertion, not a ledger humans promise to
update.** PR #85 was correct the hour it was written and wrong within
the day: Drift Garden grew from a foundation into a game, versions it
pinned became fiction, and the PR sat conflicted because nothing
could TELL anyone it had rotted. The refresh ships the antidote:
`docs/RELEASES.md` pins every zip AND every staged file sha256, and
`tools/package-web-arcade.sh --verify` re-derives the whole tree and
fails loudly on any drift — one command turns "is the package
current?" from archaeology into a yes/no. The general rule, the
lane's committed-oracle pattern applied to artifacts instead of
sims: **any committed derivative (package, dist, generated doc)
needs a committed one-command re-derivation that ASSERTS equality —
otherwise its correctness has an expiry date nobody can read.**
Smaller find: version numbers should track CONTENT identity, not
effort — Undertow/Tiltstone stayed v1.0 (their staged bytes are the
same cut #85 zipped), Drift Garden jumped foundation-v0.1 → v1.0
(different program), and the bundle bumped v1.0 → v1.1 (one member
changed). The zip name is a claim about bytes; keep it honest.

## Previous-session review

- Session 54 (Courier audio): its front-load-the-check idea shaped
  this slice's proof order — determinism (run-twice cmp) and the
  manifest assertion were built and run BEFORE the docs were written,
  so the pinned numbers were measured, never promised. Its "record
  the negative result explicitly" rule shows up here as the #93
  juice.js disposition: checked first (the Tiltstone stack is still
  open, no juice.js on main), recorded as a pending follow-up in the
  script header, the ledger, and the PR body — instead of guessed at.
- Its CONCEPT-COMPLETE baton is why this slice exists at all: with
  both GBA concepts fully built, the one loose end worth a contained
  slice was the rotting package.
- Its ts discipline was still not enough for the born-red card (the
  erratum above) — the rule now covers ALL timestamps, first commit
  included.

## What this session did

1. Claim + born-red card first (`903fac0`, with the ⚑ flag), PR #109
   opened READY immediately after, cut from main @ `982b23a`.
2. **The refreshed package** (`tools/package-web-arcade.sh`):
   - **Undertow v1.0** — `index.html`, `game.js` (contract verbatim).
   - **Tiltstone v1.0** — `index.html`, `engine.js`, `app.js`; **no
     `juice.js`** — checked first: the #92←#93←#95←#97 stack is still
     open and main has no juice; the #93 staging follow-up (add
     juice.js + bump versions) is recorded in the script header, the
     ledger, and the PR body.
   - **Drift Garden v1.0** — the playable seeded round game (#84,
     merge `7ffcf1c`): `index.html`, `game.js`,
     `manifest.webmanifest`, `sw.js`, both icons; the landing page
     copy rewritten to match (plant, pollinate, harvest before dusk).
   - **web-arcade v1.1** — the bundle (its Drift Garden member
     changed; the two unchanged games kept v1.0).
3. **Determinism, proven**: fixed source epoch (2026-07-14 UTC),
   `LC_ALL=C`-sorted member lists, `zip -X`; the script run twice →
   all four zips byte-identical (`cmp`); no timestamp normalization
   hacks needed beyond the epoch pin.
4. **The manifest, pinned AND asserted**: `docs/RELEASES.md` carries
   sha256 for all 4 zips + all 12 staged runtime files;
   `tools/package-web-arcade.sh --verify` rebuilds and asserts every
   pin (exit 1 on drift) — run green against the committed tree.
5. **Ship**: `dist/web/` (host-ready arcade) + `dist/releases/`
   committed per repo convention; dist/README gains the web/releases
   section; heartbeat appended — the lane's CLOSING dispatch, ending
   with the stand-down line.

## Guard recipe (the lane stands down — for whoever wakes it)

- The state of the world is in the closing heartbeat: Shoal v1.0 and
  Clockwork Courier v1.0 CONCEPT-COMPLETE with committed proof
  oracles; the web arcade package current with an asserted manifest;
  ORDERS 001-005 served; the EAP night ORDER never arrived (#104
  outbox ask stands); open non-lane PRs: Tiltstone stack
  #92/#93/#95/#97, control #85/#87/#88/#89/#90.
- When the Tiltstone stack lands: add `juice.js` to the Tiltstone
  stage list, bump web-tiltstone to v1.1 and the bundle to v1.2,
  regenerate, re-pin the manifest (`--verify` will fail until you
  do — that failure is the reminder working as designed).
- #85's owner disposition: close as superseded (this PR carries its
  current-form content), or merge-and-fix — either way `--verify`
  arbitrates what is true afterward.
- New gba/web breadth is owner-routed: one-page concept first.

## Session enders

- Package script run twice → four byte-identical zips (cmp, all);
  `--verify` green against the committed tree (16 pins asserted);
  the committed dist/web + dist/releases ARE the script's output at
  this tree (regenerated in-place before commit).
- No CI workflow edits; web game dirs carry no Makefile (the ROM
  builds job's game discovery is untouched).
- `python3 bootstrap.py check --strict` — green at the flip.
- Claim `control/claims/web-arcade-pack.md` retired with this flip.
