# gba-homebrew (game-lab Track B) · status

updated: 2026-07-11T17:26:58Z
phase: **NEW ARC — session 15 (directed concept slice).** The owner PICKED:
an **original zombie-survival game**, with a mid-slice owner correction
retargeting it to the **NINTENDO DS** (not GBA). This supersedes the
deepen-LD / Clockwork Courier / Shoal concept menu — the long-carried
concept-pick ⚑ is **RESOLVED by owner input**. This slice committed the
decision-ready concept doc: **`docs/concepts/gloamline-concept.md`**
(working title **Gloamline**, horde-defense loop, walking-skeleton cut,
pure-logic engine-reuse inventory, determinism plan). Lumen Drift is
untouched and stays scope-complete at v1.3 (`dist/lumen-drift.gba`
unchanged).
arc plan: slice 1 = concept doc (THIS PR) → **slice 2 (NEXT) = NDS
toolchain feasibility** — libnds or BlocksDS + headless melonDS/DeSmuME;
done-when: minimal .nds built and boot-verified with a screenshot, OR a
six-field OWNER-ACTION naming the exact missing packages → slice 3 =
walking skeleton (player moves, one Shambler chases, dawn timer to
survive, HUD count) → later: shove verb + waves, barricades, scavenge
interlude, lantern-oil light pressure, audio, save persistence.
health: green (docs/control-only slice; `python3 bootstrap.py check
--strict` attempted at close-out — result in the session card /
PR; no kit-owned or build-graph files touched)
kit: v1.11.0 · check: green (last verified kit-upgrade-v1.11.0 slice,
PR #44) · engaged: yes
boot: synced to origin/main `e1bde50` (PR #48's session-14 wake merge,
landed mid-slice; this branch rebased onto it — the wake took the
session-14 number, so this slice's card is `2026-07-11-session-15.md`).
last-shipped: PR #48 (`e1bde50`, session 14 wake). This session's own PR
(Gloamline concept doc + card + this status) pending, auto-merge armed.
blockers: none
orders: acked=001,002,003,004 done=001 (this repo's half), 002 (trigger
`trig_0137SkvhXEJvwepX8aVNkcSn` armed, cron `0 * * * *`), 003+004 (executed
session 9, condensed record below). Inbox re-read at HEAD before this write:
no new orders. This slice is coordinator-assigned work, claimed via
`control/claims/` (claim released at close-out), not an ORDER id.
📊 Model: fable-5 (ORDER 003, family-level self-report from this
session's own harness/environment banner).
review-queue: EMPTY (since slice 3).
lane position (honest): **the lane has a picked direction again.** Owner
input resolved the concept gate; the backlog is the arc plan above. Next
session takes slice 2 (NDS toolchain feasibility). The hourly wake
trigger continues between directed slices.
repo-name note (cosmetic, owner's call): this repo is named `gba-homebrew`
but will host an NDS game per the owner's direction — suggested layout
`games/gloamline-nds/`; nothing depends on a rename.
kit-follow-ups carried (still unclaimed, low priority): `upgrade
--apply-docs` (14 template-improved docs), migrate/retire legacy root
`claims/` (only its README remains), auto-merge enabler wiring decision
(`adopt --wire-enforcement`), upstream the PR #5 born-red-card gate rule to
substrate-kit

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

⚑ needs-owner (carried; concept pick now RESOLVED):
- ~~**Concept pick**~~ — **RESOLVED 2026-07-11 by owner input**: original
  zombie-survival, target NDS (correction mid-slice). The old 3-concept
  menu (`docs/concepts/session-1-concepts.md`) is superseded; Lumen Drift
  stays complete as shipped.
- **⚑ owner-click: create the Lumen Drift GitHub Release** (worker seat gets 403 on tags/releases). Suggested tag **`lumen-drift-v1.3`**. Exact clicks: repo → Releases → "Draft a new release" → "Choose a tag" → type `lumen-drift-v1.3` (create on publish, target `main`) → title `Lumen Drift v1.3` → attach `dist/lumen-drift.gba` from the merged tree (167,996 B; sha256 `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94` — paste it in the notes) → point the notes at `docs/PLAYING.md` and the v1.3 entry in `docs/current-state.md` → Publish.
- **⚑ owner-click: merged-branch cleanup** — worker seat gets 403 on branch-delete; merged `claude/*` branches (through PR #40, the kit-upgrade branches, session 9's `claude/gracious-feynman-4xi0o4`, slice 8's `claude/session-8-oa4-verified`, the v1.10.1/v1.11.0 kit-upgrade branches, and the wake branches through PR #48) can be deleted in one sweep from the branches page.
- **⚑ graze tuning wants owner hands-on validation** — the 400-frames-per-graze refund and the 6px shell are reasoned and machine-proven (CI asserts the refund and the lane geometry), but whether grazing FEELS right — risk-priced, readable, not exploitable — needs real hands on a real run. Was slice-5 polish-debt item 4; now owner-gated polish on a complete, shipped game.
- ⚑ (**awareness, no click needed**) — the routine model-attribution mismatch (ORDER 003 record above) belongs in whatever the fleet-manager coordinator compiles for a report to Anthropic, alongside pokemon-mod-lab's parallel finding and the `websites` PR #59 precedent.
notes: worker seat (no tag/release/branch-delete perms — queued above). Platform-issues ledger: `docs/PLATFORM-LIMITS.md` carries the dated owner-reported notes section (2026-07-11 hourly-wake entry — OA-4 item verification-CLOSED, model-mismatch and OA-5 items still open) alongside the verified walls. Walls carried unchanged: api.github.com curl proxy-walled (GitHub MCP is the merge path); mGBA python core.load_save() segfault (--savefile bus-copy is the working path); devkitPro official infra Cloudflare-403 (leseratte10 mirror + SHA-256 pins) — NOTE for arc slice 2: the mGBA-specific walls do not transfer to NDS; melonDS/DeSmuME feasibility is exactly what slice 2 must establish. dist/ convention: player-facing games ship committed in `dist/` with provenance (v1.3 row live); skeleton stays CI-artifact-only; CI sha256-logs every from-source build for cross-checking.
