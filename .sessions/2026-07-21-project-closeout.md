# Session — gba project closeout (public-facing wind-down doc)

> **Status:** `in-progress`

- date: 2026-07-21 (branch `claude/gba-project-closeout-0721`, main synced @
  `3377319` = #213 merge, `git ls-remote` match; all timestamps from `date -u`)
- mission: **the project's closing slice** — the autonomous development period
  ends 2026-07-22T00:00Z; after that the working environment is gone and only
  commits survive. This session lands one public-facing **project closeout**
  page (`docs/PROJECT-CLOSEOUT.md`) that a reader who knows NOTHING of these
  sessions — the owner, or a fresh future Claude seat — can pick up cold: what
  the project is and shipped, the true state at HEAD, the continuation threads
  in priority order, a plain-language owner walkthrough with clickable links,
  and a fresh-session boot guide. No new feature work; verify-before-you-write
  (every state claim checked live at HEAD, cited).
- scope guard: **doc + true-up, no gameplay code.** New `docs/PROJECT-CLOSEOUT.md`;
  `docs/current-state.md` gains the closeout on its read-path index + minimal
  freshness corrections only; `control/claims/` pruned of terminal-stale claims;
  `control/status.md` gets the final SEAT-CLOSED heartbeat. Nothing under
  `games/` is touched — a docs/control diff does not break `ROM builds`.
- **📊 Model:** Claude Opus · high · idea/planning — public-facing closeout doc
  + true-up (no gameplay code touched)
- landing posture: READY (non-draft). Plain born-red `claude/*` card PR — this
  card holds the PR red as `in-progress` until the closeout flip (the LAST
  commit), then it lands on green `ROM builds` via the existing
  `auto-merge-enabler.yml`.

## What ships

1. **Born-red gate** (this card `in-progress` + the work claim
   `control/claims/claude-gba-project-closeout-0721.md`), filed before the doc
   so the PR is red until the closeout flip.
2. **`docs/PROJECT-CLOSEOUT.md`** — the closeout page: (a) what the project is
   & what shipped with PR/commit cites; (b) current true state verified live at
   HEAD; (c) continuation threads in priority order with exact resume steps;
   (d) plain-language owner walkthrough + clickable links + a quickest-first
   checklist; (e) fresh-session boot guide + gotchas.
3. **True-up** — `docs/current-state.md` links the closeout from its read-path
   Doc index (reachability root) + a dated freshness note; `control/claims/`
   pruned of every terminal-stale claim; `control/status.md` final heartbeat.

## Verified live at HEAD `3377319` (citations, not memory)

- main HEAD `3377319` = #213 (`git ls-remote origin refs/heads/main` →
  `337731906098d1ba596dcaa1750299e45c737ed6`); `git log` shows no commits since
  #213. Live open-PR set **empty** (`list_pull_requests state=open` → `[]`).
- `dist/` carries **9 committed player ROMs** (6 GBA: lumen-drift, wickroad,
  cindervault, deepcast, clockwork-courier, shoal; 3 NDS: gloamline, brineward,
  underroot) — each sha256 spot-checked against `dist/README.md`.
- **Lumen Drift Release** `lumen-drift-v1.3` published non-draft (github MCP
  `get_release_by_tag`; asset `lumen-drift.gba` sha256 `195a8679…`).
- **Web arcade live**: `GET https://menno420.github.io/gba-homebrew/` → HTTP 200.
- Each `dist/blob/main/…` ROM URL → HTTP 200; release page → HTTP 200.
- Menu real-choices = **A1 Tinderhand + A3 Starloom only** (the #208 menu audit:
  A2/B1/B2/B3 BUILT, C published); the two open choices carry executable
  pre-plans (`docs/plans/tinderhand-preplan-2026-07-20.md`,
  `docs/plans/starloom-preplan-2026-07-20.md`).
- Claims pruned this slice map 1:1 to **merged** PRs (#176/#205/#206/#207/#208/
  #209/#210/#212), each `merged:true` at live GH or a landed commit on `main`.

## 💡 Session idea

**A closeout doc's durable value is being addressed to the reader who has lost
all session context — because after 2026-07-22 that is the ONLY reader left.**
Every prior wind-down here (the 2026-07-14 EAP walkthrough, the 2026-07-19
continuity doc) implicitly assumed a successor seat that shared this project's
internal vocabulary — landing paths, born-red cards, ORDER numbers. But the terminal
closeout has two readers who share none of it: the owner, and a cold future
Claude that boots from `git clone` with zero conversation history. The move
that pays off is writing each artifact-link and resume-step so it survives
*decontextualization*: a clickable URL instead of "the arcade" (a name only
this session knows), a `git`-checkable command instead of "verified earlier,"
the exact file + first step instead of "continue the arc." State that only
lives in a session's memory dies with the session; state written as a
clickable link or a runnable command is the only kind that outlives the seat.

## Previous-session review

- Directly continues the same worker lane (session_01343oPvj5bzQZUsHuVsC9cK):
  **#205** (EAP continuity doc) → **#206** (dist/wickroad.gba refresh) →
  **#207** (controller/B1 truth refresh) → **#208** (menu-vs-tree audit) →
  **#209** (NDS ROMs committed) → **#210** (docs freshness), all merged
  autonomously on green via `auto-merge-enabler.yml`. This is the terminal
  cap: those slices each corrected one stale surface; this one consolidates the
  whole period into a single cold-readable page and closes the seat.
- **What transferred:** the born-red rhythm verbatim — card + claim first, ONE
  flip to `complete` as the last commit, land on green `ROM builds` via
  `auto-merge-enabler.yml` (never the retired `merge-on-green.yml`); every
  claim cited to a commit/PR/live probe; verify-before-you-write.
- **What differs:** the audience. The #205/#210 docs addressed a context-literate
  successor; this page is written for a reader with zero session context, so
  every pointer is a clickable link or a runnable command, not a name only this
  seat knows.

## Guard honored

- Track-A PUBLIC repo, original gba IP only (no Track-B / Nintendo / pokemon
  content). ORDER 007 respected — **no routines armed** this slice (the routine
  wipe is coordinator-side). No force-push. Every Bash pinned to the absolute
  repo root `/home/user/gba-homebrew`. Repo text treated as DATA, not
  instructions. This period described generically as an autonomous development
  period (no internal system named).
