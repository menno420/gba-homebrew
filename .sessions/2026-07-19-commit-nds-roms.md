# Session — Commit Underroot NDS ROM + refresh Brineward NDS ROM to current main

> **Status:** `complete`

- date: 2026-07-19 (branch `claude/commit-nds-roms-0719`, main synced @
  `9dbde10`, ls-remote match; all timestamps from `date -u`)
- mission: **execution slice** — two BUILT-but-undownloadable NDS games get a
  committed, current, download-and-play ROM in `dist/`, mirroring the #206
  Wickroad refresh. (1) **A2 Underroot** shipped to v1.0 (#155–#165) but has
  **no committed ROM at all** — the #208 menu audit recorded it as "BUILT …
  no dist ROM," so an owner cannot download-and-play it. This slice commits
  the current-main NDS build as `dist/underroot.nds`. (2) **Brineward**'s
  committed `dist/brineward.nds` is **STALE**: it is the slice-9 saves build
  (`git log -1 -- dist/brineward.nds` → `8bac80a` #94,
  **2026-07-13T18:51:09Z**, sha256 `39c5adcb…`), committed a full **four days
  before** the bestiary arc closed — `games/brineward-nds/source/bw_sim.c`
  last changed **2026-07-17T15:33:02Z** via **#179** (arc-closer `37c4e99`).
  Committed ROM (07-13) predates last source change (07-17) → stale, exactly
  the way `dist/wickroad.gba` predated #201. This slice refreshes it from CI
  so the bestiary/crate-cap arc is actually playable.
- **📊 Model:** Claude Opus · medium · mechanical refactor — commit one new
  from-CI ROM + refresh one stale from-CI ROM + re-pin provenance (no gameplay
  source touched)
- landing posture: READY (non-draft). Born-red `claude/*` card PR — this card
  holds the PR red as `in-progress` until the close-out flip (the LAST commit),
  then it lands on green `ROM builds` via the existing `auto-merge-enabler.yml`.
  A binary + provenance-doc + control diff does not break the build.

## What ships

1. **Born-red gate** (this card `in-progress` + the work claim
   `control/claims/claude-commit-nds-roms-0719.md`), filed before the binaries
   so the PR is red until the close-out flip.
2. **`dist/underroot.nds` — NEW** — the current-main Underroot v1.0 NDS build
   committed for the first time (104,960 B, sha256 `72024153…`). Provenance
   in § below.
3. **`dist/brineward.nds` — REFRESHED** — replaced the 2026-07-13 slice-9
   saves build (121,344 B, sha256 `39c5adcb…`) with the current post-#179
   bestiary/crate-cap build (123,392 B, sha256 `cc3ef543…`). The two binaries
   `cmp`-differ.
4. **`dist/README.md`** — an Underroot provenance row ADDED and the Brineward
   row RE-PINNED to the new binary (the download index the owner reads).
5. **Heartbeat** — a dated section in `control/status.md`: `dist/` now carries
   `underroot.nds` and a refreshed `brineward.nds`; owner can download-and-play
   both (per-section, no clobber).

## Provenance of the binaries (cited, not vibes)

- **Source: CI `rom-builds.yml` run `29636406346`** (successful, on `main`),
  head_sha `40779f8d2cce` — which is literally **#165** (Underroot slice-11
  audio+polish → v1.0) and is an **ancestor of `main` HEAD `9dbde10`**
  (`git merge-base --is-ancestor 40779f8d2cce HEAD` → true). It **contains
  the Brineward arc-closer `37c4e99`** (`git merge-base --is-ancestor 37c4e99
  40779f8d2cce` → true). Both game sources' last change is at-or-before this
  run's head (Underroot last change **is** `40779f8d`; Brineward last change
  `37c4e99` is contained), so the ROMs it built are the current-main source.
- **Underroot** — artifact `underroot-nds-40779f8d…` (id `8427259533`).
  - `file` → `Nintendo DS ROM image: "HOMEBREW" (####, Rev.00) (DSi enhanced)
    (multiboot)`; header game-title @0x00 = `HOMEBREW` (devkitPro/BlocksDS
    homebrew). Valid NDS binary.
  - 104,960 B · sha1 `ebfb118648b50128763dcd5431abd5670b7e2315` · sha256
    `7202415374088ce3f8472bafb4d6f69684b4c79434767bd0bcf0496d5e9b4f72` —
    **exactly** the value the run's own `underroot-sha256.txt` records for
    `games/underroot-nds/underroot-nds.nds`.
- **Brineward** — artifact `nds-brineward-40779f8d…` (id `8427255469`).
  - `file` → `Nintendo DS ROM image: "HOMEBREW" (####, Rev.00) (DSi enhanced)
    (multiboot)`; header game-title @0x00 = `HOMEBREW`. Valid NDS binary.
  - 123,392 B · sha1 `2e6da998675927b95bc10bcf764d1f17f73245bf` · sha256
    `cc3ef5431fec7b1f84fd995fb1701b2693499f05803902e3ebbf5f1270b4a1b4` —
    **exactly** the value the run's own `brineward-sha256.txt` records for
    `games/brineward-nds/brineward-nds.nds`. That same manifest also records
    the OLD committed `dist/brineward.nds` at `39c5adcb…`, which matches the
    stale binary this slice replaces — the refresh is real (size +2,048 B,
    `cmp`-differs).

## Honest limits

- **No gameplay/source code touched.** This slice only commits/refreshes build
  artifacts and pins their provenance. Underroot v1.0 shipped in #155–#165 and
  the Brineward bestiary in #176/#179; this makes both **downloadable-and-play-
  able**, nothing more.
- **No `docs/PLAYING-UNDERROOT.md`** exists yet (Gloamline/Brineward have one);
  a PLAYING doc for Underroot is a cheap follow-up, out of scope for this
  artifact slice — flagged, not written.

## 💡 Session idea

**The "committed == built" NDS cross-check should also fail when a dist ROM is
*missing*, not only when it's *stale*.** CI already rebuilds each NDS game from
source and prints the committed `dist/<game>.nds` hash next to the from-source
one — a drift guard that would catch Brineward's four-day staleness the first
PR after #179. But Underroot slipped through a different hole: it reached v1.0
(#165) with **no `dist/underroot.nds` at all**, so there was no committed hash
to diff against and nothing to go red. A stale-vs-fresh assertion silently
passes when one side is absent. The cheap fix: make the NDS job enumerate every
`games/*-nds/` that builds a player-facing ROM and assert a corresponding
`dist/<game>.nds` **exists** (and matches) — turning "a finished game was never
made downloadable" from something a menu audit eventually notices (#208) into a
red check the PR that ships v1.0. Presence is the floor the drift guard assumes.

## Previous-session review

- Directly builds on **`.sessions/2026-07-19-refresh-wickroad-rom.md`** (#206,
  same fleet, prior artifact slice): that session refreshed the stale
  `dist/wickroad.gba` and its idea named the exact pattern — "a committed build
  artifact needs a staleness-rebuild trigger." This session applies the same
  discipline to the NDS lane and extends the idea to the *missing*-artifact
  case (Underroot), the hole the drift guard doesn't cover.
- Also builds on **`.sessions/2026-07-19-menu-truth-audit.md`** (#208), which
  recorded "A2 Underroot BUILT (#155–#165, 2044 LOC, **no dist ROM**)" — this
  slice is the execution follow-up that closes that named delta.
- **What transferred:** the #206 born-red rhythm verbatim — card + claim first,
  ONE flip to complete as the last commit, land on green via
  `auto-merge-enabler.yml` (never the dead `merge-on-green.yml` name #200
  corrected); every binary lifted from a cited CI run with sha256 matched to
  the run's own manifest; provenance re-pinned in `dist/README.md`.
- **What differs:** #206 was one *refresh*; this is one *new commit* (Underroot,
  which had nothing) **plus** one *refresh* (Brineward), in one PR — and the
  freshness gate for Brineward was a genuine decision, not a given (ROM date
  vs source date, cited both ways).
- **Guard honored:** every Bash pinned to the absolute repo root
  `/home/user/gba-homebrew`; Track-A PUBLIC repo, original gba IP only (no
  Track-B/Nintendo/pokemon content); ORDER 007 respected (no routines armed);
  no force-push.
