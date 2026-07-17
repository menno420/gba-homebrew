# COORDINATOR ENDED (ender heartbeat, 2026-07-17)

updated: 2026-07-17T12:15:01Z (`date -u`; coordinator ender heartbeat for the
2026-07-17 coordinator session)

kit: v1.17.0

## Seat state at close

- Seat ender executed; the coordinator chain is closed. This is a wholesale
  coordinator heartbeat (neutral facts + pointers). control/inbox.md is the
  manager's file and is untouched here.
- Failsafe cron "Game Lab failsafe wake"
  `trig_0123fLkN1pzY6uNN3Y7ksYaW` (`50 */2 * * *`, enabled; last fired
  2026-07-17T10:50:10Z, next fire 2026-07-17T12:50Z) LEFT ARMED as the
  successor's dead-man bridge.
- NO pacemaker send_later one-shot is bound to this coordinator seat; none
  were deleted. 4 pending pacemakers seen belong to sibling sessions and were
  left untouched.

## Day ledger 2026-07-17 (pointers)

- #153 merged — gate-orphan / docs read-path fix; squash landed as `8da00cd`,
  merged 2026-07-17T09:17:04Z by menno420.
- #171 merged — overnight planning menu (84 proposals); merge commit
  `2a34653` = current main HEAD, merged 2026-07-17T10:31:54Z by
  github-actions[bot]. Menu:
  `docs/planning/OVERNIGHT-MENU-2026-07-16.md`.
- #180 closed UNMERGED — prior coordinator heartbeat; closed
  2026-07-17T10:17:10Z (head `f397306`). Not open.

## Open PRs (25) — landing paths

- #154 — READY. Builds green; substrate-gate FAIL (designed born-red hold);
  `mergeable_state=dirty` (real merge conflict — base is stale `8da00cd`, two
  merges behind main). Landing path: **owner rebase/resolve conflict, THEN
  owner merge button**. NOT a clean one-click; NOT a merge-on-green candidate
  as-is.
- #155→#165 (11 drafts) — **Underroot v1.0** stack (bottom-first). Each:
  builds green, substrate-gate born-red hold, auto-merge skipped. Path: owner
  ready-click → merge-on-green watcher lands.
- #166→#170 (5 drafts) — **Tiltstone arc 2** stack. Same path.
- #172→#175 (4 drafts) — **Wickroad crossroads** stack. Same path.
- #176→#179 (4 drafts) — **Brineward bestiary** stack. Same path.

## Wall map (verified landing paths)

- Agent mark-ready is classifier-walled (probe denied 2026-07-17 ~10:33Z).
- Card-flip is flaky (allowed on #171, denied on #154).
- Only working landing paths: the owner's direct ready-click (drafts) and the
  owner's merge button (#154 after rebase).

## ⚑ needs-owner (queue)

1. #154 — rebase + merge button.
2. Stack ready-clicks #155–#179 (Underroot / Tiltstone / Wickroad /
   Brineward).
3. Required-checks console — substrate-gate + nds-rom-build are NOT in the
   required set.
4. Veto picks on the two planning menus: gba #171 (84-proposal), pml #94
   (33-proposal).
5. Wickroad audio playtest — mix levels, timbre on hardware,
   coin-repetition fatigue.

## Cleanup (stale claim files on main HEAD — NOT fixed in this PR)

- `control/claims/claude-gate-orphan-fix.md` (PR #153 merged) — terminal/stale.
- `control/claims/claude-overnight-menu-2026-07-16.md` (PR #171 merged) —
  terminal/stale.

## next-2-tasks baton

1. Act on the veto survivors from the two planning menus.
2. Prune the stale claim files listed above.

## Dispatched slice 2026-07-16 — Underroot slice 2: meadow food patches (append-only)

- updated: 2026-07-16T15:43:58Z (`date -u`); session card
  `.sessions/2026-07-16-underroot-slice-2.md`, branch
  `claude/underroot-slice-2`, draft-parked stacked on PR #155
  (`claude/underroot-slice-1`).
- What: a pure `f(seed, season, index)` food-patch layer on the meadow —
  positions + amounts on the foraging apron below the hawk lanes, rendered
  on the top screen, with a host↔ROM patch-count + total-food (patch_n=6
  patch_sum=34) lockstep at pinned frames. Host mirror
  `tools/check-underroot.py` runs green locally; `nds-underroot-build`
  green pending CI.
