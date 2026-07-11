# gba-homebrew (game-lab Track B) · status

updated: 2026-07-11T12:20:29Z
phase: session 8 slice 8 (coordinator-relayed DOCUMENTATION slice) — closed the OA-4 verification hook that slice 7's dated platform-issues note left open in `docs/PLATFORM-LIMITS.md`: **OA-4 fix path CONFIRMED WORKING** (evidence below). No build/gameplay work this slice; `dist/lumen-drift.gba` stays at v1.3, world generation/physics/kill rules untouched.
health: green (`python3 bootstrap.py check --strict` exit 0 at close-out; docs/control only, no kit-owned or build-graph files touched)
kit: v1.10.1 · check: green (run locally this slice) · engaged: yes
boot: synced to origin/main `e985e38` (session 9's ORDER 003+004 merge — the first bare-hourly-wake PR in this repo's ledger — landed after #40 while this seat was idle).
last-shipped: this slice's PR (branch `claude/session-8-oa4-verified`) — OA-4 verification append in `docs/PLATFORM-LIMITS.md` + this status flip, docs/control only, READY with auto-merge armed at creation, merge-on-green.
blockers: none
orders: acked=001,002,003,004 done=001 (this repo's half), 002 (trigger `trig_0137SkvhXEJvwepX8aVNkcSn` armed, cron `0 * * * *`; first-fire PR confirmed — session 9), 003+004 (executed session 9; condensed record below, full text in the session-9 status revision `git show 0b13f5b:control/status.md`). Inbox re-read at HEAD before this write: no new orders.
review-queue: EMPTY (since slice 3).
lane position (honest): the local backlog is empty or owner-gated (unchanged since slice 6). What remains: (a) the ⚑ owner items below (concept pick, release click, branch cleanup, graze hand-feel), (b) the low-priority kit follow-ups. No undone game work known: v1.0 scope complete, v1.1–v1.3 deepening/polish shipped and proven. **Lane idle pending owner input; the hourly wake trigger continues the ritual.**
polish-debt: items 1–3 PAID (slice 6). Item 4 carried as ⚑ (owner hands-on graze tuning below). Kit follow-ups still carried (next line).
kit-follow-ups carried (still unclaimed, low priority): `upgrade --apply-docs` (14 template-improved docs), migrate/retire legacy root `claims/` (only its README remains), auto-merge enabler wiring decision (`adopt --wire-enforcement`), upstream the PR #5 born-red-card gate rule to substrate-kit

## OA-4 verification: CONFIRMED (2026-07-11, this slice — was PENDING since slice 7)

The owner's OA-4 fix path (both repos added to the hourly-wake routine's repo
scope; `docs/PLATFORM-LIMITS.md` dated 2026-07-11 note, item 1) is **CONFIRMED
WORKING — first verified PR-capable wake observed**. Evidence, attributed as
**coordinator-verified via GitHub API** (this seat made no writes to the
pokemon lane and did not re-verify its PR contents; a read-only `git
ls-remote` confirmed repo reachability, wake branch ref already absent —
consistent with post-merge deletion):

- pokemon-mod-lab **PR #30** — "session 025: hourly wake heartbeat + shepherd
  session-024", head branch `claude/eloquent-newton-4ihg8r`, **merged
  2026-07-11T12:06:21Z**; its body self-identifies as an hourly wake and
  states it has **working GitHub MCP write tooling** — updating ⚑
  OWNER-ACTION 4's premise (wake sessions are no longer PR-less).
- The wake even **shepherded a stranded sibling branch itself** (opened
  pokemon-mod-lab #29; that content landed via **#31** after a CI marker
  fix) — OA-4's failure mode now being cleaned up BY a wake, not caused by
  one.
- Second lane's corroboration (already on record, session 9 below): this
  repo's first bare-wake PR needed no `add_repo` call — both repos were
  pre-attached in the wake session's tool scope.
- **Branch-name quirk (new, carried):** wake branches come out
  harness-assigned as `claude/...` instead of the lane's `track-a/...`
  convention — cosmetic, but relevant when auditing branch lists for
  stranded wake work.

The full verification text lives in `docs/PLATFORM-LIMITS.md` (2026-07-11
dated note, item 1 — pending line struck through, dated verification
appended). OA-5 (add_repo classifier denials ~1-in-2) remains open/carried in
that note — distinct from OA-4 and NOT retired by this confirmation.

## Condensed ORDER 003/004 record (executed session 9, merged as `e985e38`)

- **ORDER 003 (model attribution):** session 9's ground-truth self-report was
  a **Sonnet-family** model vs the routine's owner-reported configured
  **Opus-family** setting — a third distinct data point on the
  configured-vs-actual mismatch (prior wake self-reports: Fable-family). The
  actual-model side is itself inconsistent firing-to-firing. Cross-ref:
  pokemon-mod-lab status carries a parallel finding; fleet-manager
  coordinator should fold both into `docs/findings/model-matrix-2026-07.md`.
- **ORDER 004 (self-review):** key findings — no wake-fired PR existed in
  this repo before session 9 (consistent with OA-4, now CONFIRMED above);
  order latency for this lane is bounded by wake cadence AND by whether the
  firing session is a bare wake vs a directed slice; no CI red on `main`, no
  build regressions, lane correctly idling on owner input.

⚑ needs-owner (all carried):
- **Concept pick** — pick 1 of the 3 committed Track B concepts (Lumen Drift-deepening / Clockwork Courier / Shoal; `docs/concepts/session-1-concepts.md`). The menu is WITH THE OWNER; slices 4–6 executed the coordinator's announced default (deepen Lumen Drift). An explicit pick still gates the remaining sessions.
- **⚑ owner-click: create the Lumen Drift GitHub Release** (worker seat gets 403 on tags/releases). Suggested tag **`lumen-drift-v1.3`**. Exact clicks: repo → Releases → "Draft a new release" → "Choose a tag" → type `lumen-drift-v1.3` (create on publish, target `main`) → title `Lumen Drift v1.3` → attach `dist/lumen-drift.gba` from the merged tree (167,996 B; sha256 `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94` — paste it in the notes) → point the notes at `docs/PLAYING.md` and the v1.3 entry in `docs/current-state.md` → Publish.
- **⚑ owner-click: merged-branch cleanup** — worker seat gets 403 on branch-delete; merged `claude/*` branches (through PR #40, the kit-upgrade branches, session 9's `claude/gracious-feynman-4xi0o4`, and this slice's `claude/session-8-oa4-verified` once merged) can be deleted in one sweep from the branches page.
- **⚑ graze tuning wants owner hands-on validation** — the 400-frames-per-graze refund and the 6px shell are reasoned and machine-proven (CI asserts the refund and the lane geometry), but whether grazing FEELS right — risk-priced, readable, not exploitable — needs real hands on a real run. Was slice-5 polish-debt item 4; the only non-kit work item left, and it is owner-gated by nature.
- ⚑ (**awareness, no click needed**) — the routine model-attribution mismatch (ORDER 003 record above) belongs in whatever the fleet-manager coordinator compiles for a report to Anthropic, alongside pokemon-mod-lab's parallel finding and the `websites` PR #59 precedent.
notes: worker seat (no tag/release/branch-delete perms — queued above). Platform-issues ledger: `docs/PLATFORM-LIMITS.md` carries the dated owner-reported notes section (2026-07-11 hourly-wake entry — OA-4 item now verification-CLOSED, model-mismatch and OA-5 items still open) alongside the verified walls. Walls carried unchanged: api.github.com curl proxy-walled (GitHub MCP is the merge path); mGBA python core.load_save() segfault (--savefile bus-copy is the working path); devkitPro official infra Cloudflare-403 (leseratte10 mirror + SHA-256 pins). dist/ convention: player-facing games ship committed in `dist/` with provenance (v1.3 row live); skeleton stays CI-artifact-only; CI sha256-logs every from-source build for cross-checking.
