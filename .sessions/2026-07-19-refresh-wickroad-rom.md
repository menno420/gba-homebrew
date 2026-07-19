# Session — Refresh dist/wickroad.gba to the post-#201 tier-up build

> **Status:** `complete`

- date: 2026-07-19 (branch `claude/refresh-wickroad-dist-rom`, main synced @
  `5d8b9a0`; all timestamps from `date -u`)
- mission: **execution slice** — the committed downloadable `dist/wickroad.gba`
  is STALE. It is the crossroads cut-4 build (`git log -1 -- dist/wickroad.gba`
  → `7e30756` #175, **2026-07-17T15:13:39Z**, sha256 `d740b73…`), built a full
  day BEFORE the Wickroad tier system landed. The end-card tier-up flash
  (`wr::run_tier_up_label` in `games/wickroad/src/wr_milestones.h`) landed
  2026-07-18T21:22:11Z via **PR #201** (merge `6d1d97a`) — so the binary an
  owner downloads to play-test the tier-up banner **cannot show it**. This
  slice swaps the stale ROM for a current from-source build so the feature is
  actually playable, then lands on green.
- **📊 Model:** Claude Opus · medium · mechanical refactor — swap the stale
  committed ROM artifact for its current from-CI build + re-pin provenance (no
  gameplay source touched)
- landing posture: READY (non-draft). Born-red `claude/*` card PR — this card
  holds the PR red as `in-progress` until the close-out flip (the LAST commit),
  then it lands on green `ROM builds` via the existing `auto-merge-enabler.yml`.
  A binary + provenance-doc + control diff does not break the build.

## What shipped

1. **Born-red gate** (this card `in-progress` + the work claim
   `control/claims/claude-refresh-wickroad-dist-rom.md`), filed before the
   binary so the PR is red until the close-out flip.
2. **`dist/wickroad.gba` refreshed** — replaced the 2026-07-17 crossroads
   cut-4 build (180,008 B, sha256 `d740b73…`) with the current post-#201 build
   (182,860 B, sha256 `c7e2814e…`). Provenance in § below; the two binaries
   `cmp`-differ.
3. **`dist/README.md`** — the wickroad sha256/size/date/built-from provenance
   row re-pinned to the new binary (the download index the owner reads).
4. **`docs/eap-closeout-gba-2026-07-19.md`** — Controller Slice-3 enumeration
   row corrected: product-forge #29 MERGED 2026-07-19T07:41:57Z (`20be749`),
   Slice 3 is now UNBLOCKED-but-reserved for the original Controller lane
   (folded-in one-line factual correction; collision guard).
5. **Heartbeat** — a dated section in `control/status.md`: dist/wickroad.gba
   refreshed to the post-#201 build, owner play-test now uses the current
   binary (per-section, no clobber).

## Provenance of the new binary (cited, not vibes)

- **Source: CI `rom-builds.yml` run `29678377739`** (successful), artifact
  `gba-roms-7c811db6…` (artifact id `8439735515`), head_sha `0df73464` — which
  **contains #201** (`git merge-base --is-ancestor 6d1d97a 0df73464` → true).
- **Byte-equivalence to main:** `git diff --stat HEAD 0df73464 -- games/wickroad/`
  is **EMPTY** — no commit after #201 (`6d1d97a`) touches Wickroad source, so
  the ROM this CI run built from `games/wickroad/` is byte-identical to what
  current `main` HEAD (`5d8b9a0`) builds. The build is byte-deterministic.
- **Verified GBA ROM:** `file` → `Game Boy Advance ROM image: "WICKROAD"
  (GLWR01, Rev.00)`; Nintendo logo header @0x04 = `24ffae51…` (valid);
  title @0xA0 = `WICKROAD`.
- **sha256 match to CI provenance:** the extracted binary hashes
  `c7e2814ee1e316e4e2cb2272caaf3dd35341f42ead80923640fd8b669a2fcb20`, exactly
  the value the run's own `rom-sha256.txt` records for
  `games/wickroad/wickroad.gba`.
- **Differs from the stale committed ROM:** old `d740b73…` vs new `c7e2814e…`
  (`cmp` differs at byte 465; +2,852 B for the tier-up code/labels).

## Honest limits

- **No gameplay/source code touched.** This slice only swaps a stale build
  artifact for its current from-CI build and re-pins its provenance. The
  Wickroad tier logic itself shipped in #189/#190/#195/#201; this makes it
  downloadable-and-playable, nothing more.
- **GRANDMASTER≥200 remains unbuilt** — a forward seam gated on owner
  play-test evidence; out of scope here.

## 💡 Session idea

**A committed build artifact needs a "staleness rebuild" trigger, not just a
provenance stamp.** The repo already does the hard part — every dist ROM row in
`dist/README.md` pins a sha256 and a "built from" tree, and CI rebuilds from
source on every PR. But nothing links the two: four separate PRs (#189/#190/
#195/#201) all edited `wr_milestones.h` and none re-committed `dist/wickroad.gba`,
so the downloadable binary silently drifted a full day behind the source the
owner was being told to play-test. The provenance stamp records *what* the
binary is; it can't notice that *what it should be* has moved. The cheap fix is
a CI assertion: on `main`, diff each committed `dist/<game>.gba` against a fresh
from-source build and fail (or auto-open a refresh PR) when they differ — the
same "committed == built" cross-check the NDS job already runs, extended to the
GBA dist artifacts. That converts "someone eventually notices the download is
stale" into a red check the first PR that touches the source. Provenance is the
floor; a drift assertion is the guardrail.

## Previous-session review

- Directly builds on **`.sessions/2026-07-19-eap-closeout-gba.md`** (same
  fleet, prior slice): that planning/continuity doc *flagged this exact gap* —
  "The committed `dist/wickroad.gba` predates the tier-up flash… a from-source
  rebuild is needed to play-test them," and listed it under "Honest limits" as
  a cited follow-up it could not fix in a docs-only slice ("refreshing the
  committed ROM is an execution slice"). This session is that execution
  follow-up: the prior card named the debt; this one pays it.
- **What transferred:** the prior slice's discipline — born-red card + claim
  first, one flip to complete, land on green via `auto-merge-enabler.yml`
  (never the dead `merge-on-green.yml` name #200 corrected). Honored verbatim.
- **What differs:** that slice was *planning* (diff the repo's self-description
  against ground truth); this is *execution* acting on one of its three named
  deltas — the ROM that "can't show the feature it's advertised for." The
  continuity is: a good hand-off doc surfaces the gap; the value is closing it.
- **Guard honored:** every Bash pinned to the absolute repo root
  `/home/user/gba-homebrew`; Track-A only; ORDER 007 respected (no routines
  armed); no force-push.
