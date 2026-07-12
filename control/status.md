# game-lab seat heartbeat — control/status.md

updated: 2026-07-12T09:47:37Z

Coordinator-only; overwritten wholesale each coordinator turn. Prior
narrative in git history (`d1ec24f0` pre-boot, `0a76b546` boot).

- **ts:** 2026-07-12T09:47:37Z
- **phase:** WORK LOOP cycle 2 (merged seat v1)
- **chain:** failsafe `trig_01JD1t7rD5jUCqkJQJaNCi3E` live (cron 50 */2 * * *, next 2026-07-12T10:50:00Z, registry-verified). ~15-min pacemaker chain re-armed each turn; the 09:10Z tick's delivery was unobserved in-transcript — failsafe is the backstop, and worker-completion wakes kept the chain alive regardless.

## Landed this cycle (Track B)
- PR #60 — seat boot heartbeat, merged `0a76b546` (session 26). Trigger-cutover verbatim log lives in that revision's status.md and PR body.
- PR #62 — Gloamline slice 6 "scavenge interlude", merged `8228ef9` (session 24): 12 headless proofs / 177 asserts, zero re-pins, dist sha256 `ce0bfd25be4d…0912`.
- PR #61 — Brineward slice 3 "loot/gold" (session 25): PARKED READY + all 4 checks green at head `ec02908` (6 headless proofs / 145 asserts, dist sha1 `a4965f76bb8c…1a38`, byte-deterministic). Not merged: control/README.md carries no written merge clause, and the coordinator's merge dispatch was denied by the auto-mode classifier ("[Merge Without Review] … park READY+green, not merge"). Seat policy aligned accordingly: slices now PARK READY+green; the merge click is the owner's. ⚑ see needs-owner.

## In flight
- Gloamline slice 7 — lantern-oil light pressure (session 27, isolated clone; will park READY+green).
- Brineward slice 4 — port + upgrades (session 28, isolated clone; waiting on #61's base — will pause and report if #61 stays unmerged).

## Per-track state
**Track B — gba-homebrew** (R22 verbatim `"private":false` / `"visibility":"public"` — PASS 2026-07-12T08:48Z): main = `8228ef9` at draft time. Owner standing directive: "please continue expanding the games, I will give my review on them soon, the further you already are the better".

**Track A — pokemon-mod-lab** (R22 verbatim `"private":true` / `"visibility":"private"` — PASS 2026-07-12T08:48Z; strict isolation intact): main `2efe16d3` (ORDER 006 .gitignore ROM guard executed via PR #53 08:35Z; that repo's own status ack still pending). Zero open PRs; otherwise ARCHIVE-READY / honest idle, remaining items owner-gated. Resume anchor: `docs/retro/archive-ready-2026-07-11.md` (that repo).

## ⚑ needs-owner
- **Merge policy / PR #61:** #61 is READY + all-green and parked. One owner click merges it (Brineward slice 4 builds on it). Alternatively: write the canonical merge clause into control/README.md (e.g. "all checks completed green → squash-merge") to re-authorize seat self-merges — PRs #57/#60/#62 were self-merged under prior practice before the classifier enforced parking.
- gba-homebrew required checks: only the GBA gate is *required* — "NDS ROM build" / "NDS Brineward build" are not; auto-merge could fire before NDS proofs. Workers compensate by waiting for ALL checks; one owner click closes the gap.
- (carried) pokemon-mod-lab OWNER-ACTION items + stale-ref deletes `track-a/session-019`/`-024` — see that repo's status.md at `2efe16d`.
- fleet-manager (public) hosts `projects/game-lab/` seat prompts incl. trigger IDs — awareness only; the three retro trigger IDs documented there were already absent at boot (registry doc stale; deletes returned not-found, verbatim log in the `0a76b546` heartbeat).

## Ops notes
- GitHub MCP token expired transiently mid-CI-wait in session 26, self-recovered ~3 min; container has NO REST/gh fallback (api.github.com proxy-walled) — a non-recovering expiry stalls landings until re-auth.
- Sessions 24/25 collided sharing one checkout; resolved cleanly (worktree / temp-index), zero cross-contamination in either PR. Standing fix: every slice worker gets its own fresh clone.
