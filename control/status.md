# gba-homebrew (game-lab Track B) · status

updated: 2026-07-11T13:20:00Z
phase: session 10 (bare hourly wake, no directed slice). No new orders at
the inbox-at-HEAD re-read; standing default continues. No build/gameplay
work this session; `dist/lumen-drift.gba` stays at v1.3, world
generation/physics/kill rules untouched.
health: green (`python3 bootstrap.py check --strict` not run — docs/control
only, no kit-owned or build-graph files touched, matching session 9's and
slice 7's precedent)
kit: v1.10.1 · check: green (last run session 8 slice 8) · engaged: yes
boot: synced to origin/main `e985e38` (session 9's ORDER 003+004 merge) —
unchanged at this session's start, no drift.
last-shipped: session 9's merge (`e985e38`, ORDER 003+004 execution). This
session's own PR (heartbeat + grooming, docs-only) pending.
blockers: none
orders: acked=001,002,003,004 done=001 (this repo's half), 002 (trigger
`trig_0137SkvhXEJvwepX8aVNkcSn` armed, cron `0 * * * *`), 003+004 (executed
session 9, condensed record below). Inbox re-read at HEAD before this write:
no new orders.
review-queue: EMPTY (since slice 3).
lane position (honest): the local backlog is empty or owner-gated
(unchanged since slice 6). What remains: (a) the ⚑ owner items below
(concept pick, release click, branch cleanup, graze hand-feel), (b) the
low-priority kit follow-ups. No undone game work known: v1.0 scope
complete, v1.1–v1.3 deepening/polish shipped and proven. **Lane idle
pending owner input; the hourly wake trigger continues the ritual.**
polish-debt: items 1–3 PAID (slice 6). Item 4 carried as ⚑ (owner hands-on
graze tuning below). Kit follow-ups still carried (next line).
kit-follow-ups carried (still unclaimed, low priority): `upgrade
--apply-docs` (14 template-improved docs), migrate/retire legacy root
`claims/` (only its README remains), auto-merge enabler wiring decision
(`adopt --wire-enforcement`), upstream the PR #5 born-red-card gate rule to
substrate-kit

## Shepherd check (session 10)

`list_branches` returned only `main` (`188e3acb`) — no stranded `claude/*`
work branches. `list_pull_requests(state=open)` returned zero. Nothing to
land.

## OA-4 / add_repo (session 10 corroboration)

This session needed zero `add_repo` calls — both repos were already
present in the session's tool scope at start, consistent with session 9's
direct confirmation and session 8 slice 8's evidence-based confirmation.
Treating OA-4 as the stable, closed state (per pokemon-mod-lab's parallel
status write) — no further re-confirmation needed each wake unless it
regresses.

## Condensed ORDER 003/004 record (executed session 9, merged as `e985e38`)

- **ORDER 003 (model attribution):** session 9's ground-truth self-report was
  a **Sonnet-family** model vs the routine's owner-reported configured
  **Opus-family** setting — a third distinct data point on the
  configured-vs-actual mismatch (prior wake self-reports: Fable-family). The
  actual-model side is itself inconsistent firing-to-firing. Cross-ref:
  pokemon-mod-lab status carries a parallel finding; fleet-manager
  coordinator should fold both into `docs/findings/model-matrix-2026-07.md`.
- **ORDER 004 (self-review):** key findings — no wake-fired PR existed in
  this repo before session 9 (consistent with OA-4, now CONFIRMED); order
  latency for this lane is bounded by wake cadence AND by whether the
  firing session is a bare wake vs a directed slice; no CI red on `main`, no
  build regressions, lane correctly idling on owner input.

⚑ needs-owner (all carried):
- **Concept pick** — pick 1 of the 3 committed Track B concepts (Lumen Drift-deepening / Clockwork Courier / Shoal; `docs/concepts/session-1-concepts.md`). The menu is WITH THE OWNER; slices 4–6 executed the coordinator's announced default (deepen Lumen Drift). An explicit pick still gates the remaining sessions.
- **⚑ owner-click: create the Lumen Drift GitHub Release** (worker seat gets 403 on tags/releases). Suggested tag **`lumen-drift-v1.3`**. Exact clicks: repo → Releases → "Draft a new release" → "Choose a tag" → type `lumen-drift-v1.3` (create on publish, target `main`) → title `Lumen Drift v1.3` → attach `dist/lumen-drift.gba` from the merged tree (167,996 B; sha256 `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94` — paste it in the notes) → point the notes at `docs/PLAYING.md` and the v1.3 entry in `docs/current-state.md` → Publish.
- **⚑ owner-click: merged-branch cleanup** — worker seat gets 403 on branch-delete; merged `claude/*` branches (through PR #40, the kit-upgrade branches, session 9's `claude/gracious-feynman-4xi0o4`, and slice 8's `claude/session-8-oa4-verified`) can be deleted in one sweep from the branches page.
- **⚑ graze tuning wants owner hands-on validation** — the 400-frames-per-graze refund and the 6px shell are reasoned and machine-proven (CI asserts the refund and the lane geometry), but whether grazing FEELS right — risk-priced, readable, not exploitable — needs real hands on a real run. Was slice-5 polish-debt item 4; the only non-kit work item left, and it is owner-gated by nature.
- ⚑ (**awareness, no click needed**) — the routine model-attribution mismatch (ORDER 003 record above) belongs in whatever the fleet-manager coordinator compiles for a report to Anthropic, alongside pokemon-mod-lab's parallel finding and the `websites` PR #59 precedent.
notes: worker seat (no tag/release/branch-delete perms — queued above). Platform-issues ledger: `docs/PLATFORM-LIMITS.md` carries the dated owner-reported notes section (2026-07-11 hourly-wake entry — OA-4 item verification-CLOSED, model-mismatch and OA-5 items still open) alongside the verified walls. Walls carried unchanged: api.github.com curl proxy-walled (GitHub MCP is the merge path); mGBA python core.load_save() segfault (--savefile bus-copy is the working path); devkitPro official infra Cloudflare-403 (leseratte10 mirror + SHA-256 pins). dist/ convention: player-facing games ship committed in `dist/` with provenance (v1.3 row live); skeleton stays CI-artifact-only; CI sha256-logs every from-source build for cross-checking.
