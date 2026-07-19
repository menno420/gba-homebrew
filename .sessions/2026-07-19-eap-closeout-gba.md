# Session — gba EAP close-out / continuity doc

> **Status:** `in-progress`

- date: 2026-07-19 (branch `claude/eap-closeout-gba-0719`, main synced @
  `97ac85d`; all timestamps from `date -u`)
- mission: **planning slice, not execution** — the gba execution backlog is
  dry (every remaining arc gate is owner-side), so per the owner's universal
  continue directive (2026-07-19: "when execution is dry, PLAN") this session
  writes a consolidated EAP wind-down / continuity doc. Repos reportedly go
  read-only 2026-07-21 (ORDER 007 EAP extension), so a single owner-action +
  successor-baton page is the timely planning deliverable. pml already has its
  equivalent (pml#86); this is the gba half.
- **📊 Model:** Claude Opus family · high effort · planning / continuity doc
  (no gameplay code touched)
- landing posture: READY (non-draft). Plain born-red `claude/*` card PR — the
  session card holds the PR red as `in-progress` until this close-out flip
  (the LAST commit), then it lands on green `ROM builds` via the existing
  `auto-merge-enabler.yml`. Docs + control + card only, no workflow file.

## What shipped

1. **Born-red gate** (this card `in-progress` + the work claim
   `control/claims/claude-eap-closeout-gba-0719.md`), filed before the doc so
   the PR is red until the close-out flip.
2. **`docs/eap-closeout-gba-2026-07-19.md`** — the continuity doc, three
   sections: (a) Finish/Park enumeration of every arc/lane with a citation;
   (b) OWNER ACTIONS in plain non-technical language (What / Where / Why /
   How-you'll-know), one block per owner-gated item; (c) Successor baton — the
   facts a fresh re-created-project seat needs that git alone won't surface
   (landing path, substrate-gate, Track-A rail, ORDER 007, owner posture).
3. **Heartbeat** — a dated planning-mode section appended to
   `control/status.md` pointing at the new doc + this PR (per-section, no
   clobber of other sections).

## Research verified at HEAD `97ac85d` (citations, not vibes)

- **Landed this session's prior slices (all merged via the server enabler,
  `github-actions[bot]`):** #200 (control workflow-name fix, `21ea4dd`
  base→merged 2026-07-18T21:09Z), #201 (Wickroad tier-up flash, merge `6d1d97a`,
  2026-07-18T21:22Z), #202 (control record #201 boot-proof + outbox,
  2026-07-18T21:30Z), #204 (control close-subsumed sweep of #177/#178 claims,
  2026-07-18T23:26Z). Live open-PR set at HEAD: **empty** (`list_pull_requests
  state=open` → `[]`).
- **web-arcade Pages go-live is DONE, not pending** — the docs
  (`dist/README.md` "Hosted arcade", `docs/NEXT-MENU-2026-07-15.md` § C) still
  say "one owner Settings click"; that is **stale**. Live check 2026-07-19:
  `GET https://menno420.github.io/gba-homebrew/` → **HTTP 200**, serving
  `<title>menno420 game-lab — web arcade</title>` with the Undertow/Tiltstone
  arcade. Set up by #149 (merged 2026-07-15T21:28Z). The site is up.
- **The committed `dist/wickroad.gba` predates the tier-up flash.** It is the
  crossroads cut-4 build (`git log -- dist/wickroad.gba` → `7e30756` #175,
  2026-07-17, sha256 `d740b73…`), whereas the tier system landed 2026-07-18:
  #189 milestone flourish, #190 VETERAN tag, #195 MASTER tier, #201 tier-up
  flash — all touch `games/wickroad/src/wr_milestones.h` but **none rebuilt the
  committed ROM**. So the downloadable binary does NOT contain the
  VETERAN/MASTER tiers or the tier-up flash; a from-source rebuild (CI does
  this every PR) is needed to playtest them. Flagged in the doc's owner action.
- **#171 is a squash-merged PR ON MAIN**, not a branch-only doc — commit
  `2a34653` "Overnight planning menu — 2026-07-16 (#171)" (owner-merged
  2026-07-17T10:31:54Z) landed `docs/planning/OVERNIGHT-MENU-2026-07-16.md`
  (84 proposals). current-state.md's "lives only on #171's branch" is stale.
- **GRANDMASTER≥200 is unbuilt** — the current tier table is VETERAN≥50 /
  MASTER≥100 (`wr_milestones.h:62-63`); GRANDMASTER≥200 is a forward seam gated
  on playtest evidence that players reach 200 lifetime runs
  (`control/status.md` next-2-tasks). No `GRANDMASTER` symbol exists in the
  tree.

## Honest limits

- **Planning only — zero gameplay/build code touched.** Nothing here is
  execution; the deliverable is a doc + a heartbeat + this card.
- **Controller Slice-3 blocker (product-forge #29) is cross-repo** — not in
  this repo (here #29 is the merged ORDER-000 toolchain PR). Its state is
  carried from `control/status.md`'s reconcile, unverified in-repo this session.
- **The `dist/wickroad.gba` staleness is a real gap I could not fix in a
  docs-only planning slice** — refreshing the committed ROM is an execution
  slice (rebuild + re-pin `dist/README.md` sha256), left as a cited follow-up.

## 💡 Session idea

**A wind-down doc's highest-value cells are the ones where the repo's own
docs are confidently WRONG.** The mechanical enumeration — every arc's state
with a PR cite — is real but low-leverage: git already carries it, and a
successor can reconstruct it. The load-bearing findings were the three places
where the committed docs assert a stale truth: (1) Pages "pending one owner
click" when the site is already live (HTTP 200); (2) "download `dist/wickroad.gba`
to see the tier flash" when that binary predates the flash by a day; (3) "#171
lives only on its branch" when it squash-merged to main. Each is a place a
trusting owner or successor would take a wrong action — click a button already
clicked, playtest a ROM that can't show the feature, hunt a branch for a file
that's on main. The general move for a continuity/hand-off doc under a
deadline: don't just transcribe state — **diff the repo's self-description
against live ground truth (a live HTTP probe, a `git log -- <path>`, the actual
merge commit) and lead with the deltas**, because a stale doc doesn't fail
loudly, it fails by being believed. The enumeration is the floor; the
corrections are the value.

## Previous-session review

- Prior lane slices (same session_01343oPvj5bzQZUsHuVsC9cK): the Wickroad
  tier-up flash + control sweeps **#200 → #201 → #202 → #204**, all merged
  autonomously on green via `auto-merge-enabler.yml` (`github-actions[bot]` as
  `merged_by`), 2026-07-18. Read at HEAD via `pull_request_read`; every one is
  terminal (state closed/merged), matching the empty live open-PR set.
- **What transferred:** those slices established the fact this doc leans on —
  the agent landing path is functional (READY `claude/*` + server enabler,
  no owner click), so a born-red control/docs card PR like this one lands on
  green by itself. #200/#202's whole point was correcting the *stale
  `merge-on-green.yml` name* to `auto-merge-enabler.yml`; this session honors
  that correction in the successor baton (the baton names the real workflow so
  the next seat doesn't chase the dead name — the exact drift #200 fixed).
- **A divergence worth naming:** the prior slices were *execution* (new
  gameplay logic + proofs); this one is *planning* under the "execution is
  dry → PLAN" directive. The continuity is the landing discipline (born-red
  card, claim-first, one flip to complete), not the payload.
- **Guard honored:** every Bash pinned to the absolute repo root
  `/home/user/gba-homebrew` (never a bare `~`), so no phantom second-checkout
  tree — the two-checkout footgun the Brineward cards flagged stays shut.
