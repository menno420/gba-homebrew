# game-lab seat heartbeat — control/status.md

updated: 2026-07-13T10:52:41Z

Coordinator-only; overwritten wholesale each coordinator turn. Prior
narrative in git history (`0a76b546` boot, `d06456f2` c2, `a7d2e113` c3, `baa39b9b` c4, session-37 dispatch footer, this seat-ender).

- **ts:** 2026-07-13T10:52:41Z
- **phase:** SEAT ENDER — night-run close-out COMPLETE. Two arcs closed for the night: Gloamline SCOPE-COMPLETE (slices 10+11 merged), Brineward sessions 6+7 PARKED on PR #82. Everything remaining is owner-gated (sweep clicks + one repo setting).
- **chain / routine disposition (as verified by the coordinator 2026-07-13T10:12Z):** failsafe wake trigger `trig_01LZ37j6Ah8rLCM7KZfmgS97` (cron `50 */2 * * *`, next fire 10:50Z) left ARMED as the successor's dead-man bridge; zero pending scheduled ticks (the 12 fired pacemaker links self-disabled; audit swept 1203 triggers); old trigger `trig_01JD1t…Ci3E` confirmed deleted; no business crons owned by this seat.
- kit: v1.15.0 · check: green · engaged: yes

## Parked READY+green (landing path: owner click; all heads re-verified live 2026-07-13 10:45–10:47Z, every check-run `success`)
- PR #82 — Brineward slices 6+7 "wind + danger bands" — head `20f4bfa`
- PR #83 — Deepcast (GBA fishing arcade) — head `55e0a3b`
- PR #84 — Drift Garden foundation→game (mobile PWA) — head `35dc162`
- PR #85 — release packaging: dist/web arcade + 4 versioned zips — head `d8f1049`
- PR #86 — Cindervault (GBA dungeon-dive roguelike) — head `76884fb`
- PR #87 — ORDER 005 morning tally (control) — head `b57221a`
- PR #88 — ORDER 006 scribe (control) — head `7480178`
- PR #89 — ORDER 006 night report (control) — head `a84933b`
- plus this close-out branch `claude/seat-ender-20260713` (retro + ledger refresh + outbox + this heartbeat; PR number unknown at heartbeat time — reference by branch)

## Pre-built and queued
- Nothing queued off-PR: the previously pre-built Brineward slice 6 now rides PR #82 together with slice 7 (fresh authorship; base branch `claude/brineward-wind` @ `eb3235a` pinned in the PR body). The stale ref delete is an owner ask below.

## Cross-seat note (2026-07-12 afternoon)
- Owner ordered a fleet-wide consolidation + reset plan in this seat's chat by mistake (meant for fleet-manager). This seat produced read-only research (2 independent 19-repo surveys + adversarial verification) and one artifact — fleet-manager PR #121 (proposal, parked READY+green, codex-reviewed: 3 real findings verified + fixed at `29c9be8`) — then handed off to the fleet-manager coordinator session, which now owns adoption/dispatch. No fleet changes were made from this seat; no game-lab state was affected.

## Per-track state
**Track B — gba-homebrew** (public): main = `d87f9ad` at close-out. Night run served ORDER 005 in full — 6 PRs merged in the owner hand-sweep ~01:43–01:45Z (#75, #77, #78, #79, #80, #81; merge commits `92d4f03`, `d87f9ad`, `27f040a`, `f7a54b7`, `2c8be27`, `f8540b1`), 8 PRs parked green (#82–#89 above). Inbox at HEAD: ORDERS 001–005 (001–004 served earlier; 005 served — tally on PR #87, night report on PR #89); ORDER 006 lands with PR #88 and is served by PR #89.

**Private track — pokemon-mod-lab:** the private track's lane ran its own parallel close-out, recorded in its own repo.

## ⚑ needs-owner (paste-ready)

**[1] Morning sweep of the parked set**
- WHAT: merge gba-homebrew PRs #82, #83, #84, #85, #86, #87, #88, #89 + the `claude/seat-ender-20260713` close-out PR.
- WHERE: each PR page → merge button.
- HOW: click in any order (control PRs #87/#88/#89 + the seat-ender each touch `control/outbox.md`/`control/status.md` — merge them oldest-first and re-resolve the trivial same-header conflicts, or take them one by one).
- WHY: all parked READY + green per the night-run rule (open PRs stay open, owner sweeps); heads verified above.
- UNBLOCKS: next slices branch from main; `dist/web` arcade goes live (on #85) for the Websites hosting handoff.
- VERIFY: PR list shows them merged; `dist/web/index.html` exists at main HEAD.
- RISK: ✅ reversible (revert-PR undoes any single one).

**[2] Required status checks on `main` (the enforcement fix for the installed auto-merge enabler)**
- WHAT: add **"ROM builds"** and **"substrate-gate"** as required status-check contexts on `main`. **Recommendation: do this before (or immediately after) the sweep — it is the one setting that makes the installed enabler (PR #76) safe and effective; until it is set, every green PR still needs a hand-sweep.**
- WHERE: github.com → menno420/gba-homebrew → Settings → Branches → branch protection rule for `main` → "Require status checks to pass".
- HOW: edit (or create) the rule, search-select the two check names, save.
- WHY: the enabler refuses to arm while zero contexts are required (it detects that arming would merge instantly); the preserved log citation lives in the night-report outbox entry (PR #89, `control/outbox.md` §5).
- UNBLOCKS: auto-merge arms on future green agent PRs — no more routine hand-sweeps.
- VERIFY: the next PR's auto-merge-enabler run reports a nonzero required-contexts count and arms.
- RISK: ✅ reversible (uncheck to undo).

**[3] Stale-ref delete: `claude/brineward-wind` (only AFTER #82 merges)**
- WHAT: delete branch `claude/brineward-wind` (`eb3235a`).
- WHERE: repo → Branches page.
- HOW: trash-can icon.
- WHY: #82 carries its slice-6 content with fresh authorship; the ref becomes a dead head after #82 lands. Agent branch-delete is 403-walled (docs/PLATFORM-LIMITS.md).
- UNBLOCKS: clean branch list; no accidental re-basing on a dead head.
- VERIFY: Branches page no longer lists it.
- RISK: ✅ reversible (GitHub restores deleted branches from the PR page).

- (carried) melonDS enhancement lane go/no-go (owner asked 2026-07-12 ~14:15Z; complaint research delivered).

## Baton — next 2 (for the successor seat)
1. **Post-sweep verify + required-checks setting confirmed** — re-verify the swept set on main, confirm asks [2]/[3] applied; with the contexts set, the enabler goes live for routine slices.
2. **Brineward slice 8 or the arcade hosting handoff** (Websites lane via the manager, `dist/web` once #85 is on main) — owner/manager pick.

## Pointers
- Night-run retro: `docs/retro/2026-07-13-night-run.md`
- Refreshed ledger: `docs/current-state.md` (night-run close-out section, 2026-07-13T10:50Z)
- Baked proposals (3, manager-addressed): `control/outbox.md` seat-ender entry 2026-07-13T10:50:11Z (scribe ORDER-template grammar; required checks; bounded-MCP-poll prompt delta)
- Night tally / night report: PR #87 / PR #89 outbox entries
