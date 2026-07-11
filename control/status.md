# gba-homebrew (game-lab Track B) · status

updated: 2026-07-11T18:05:00Z
phase: **GLOAMLINE ARC — session 16 (slice 2, NDS toolchain feasibility
gate): FEASIBILITY PROVEN.** The container builds and boot-verifies NDS
ROMs. Toolchain: **BlocksDS 1.21.1 + Wonderful arm-none-eabi GCC 16.1.1**,
installed fail-closed by the new sha256-pinned
**`tools/setup-nds-toolchain.sh`** (validated from a clean slate; the
Wonderful/BlocksDS hosts are directly reachable — the devkitPro
Cloudflare-403 wall does NOT generalize). Evidence: original dual-screen
probe **`games/gloamline-nds/`** builds deterministically
(`gloamline-nds.nds` 95,744 B, sha256
`f767523eb2f6ef096dd73f8c72b56b7c4782e0851238cc0f5e700936590ca4d7`) and
boot-verifies headlessly in py-desmume==0.0.9 via the new
**`tools/nds-headless-check.py`** — both screens non-blank at frames 150
and 300, consecutive shots distinct (live main-loop counter); screenshots
committed at `games/gloamline-nds/proof/`. CI: parallel **`nds-rom-build`**
job (pinned-toolchain cache, build, sha256 log, headless boot proof,
artifact upload); the required GBA "ROM builds" gate untouched except an
errexit-safe skip of BlocksDS Makefiles in its `games/*/` loop. Lumen
Drift untouched, scope-complete at v1.3 (`dist/lumen-drift.gba` unchanged).
arc plan: slice 1 = concept doc (PR #50, merged) → slice 2 = **THIS PR**
(feasibility PROVEN) → **slice 3 (NEXT) = walking skeleton** — one arena
screen, player 8-way move, ONE Shambler chasing, contact = death →
game-over card → instant restart, dawn timer for night 1, HUD count;
per `docs/concepts/gloamline-concept.md`. Slice-3 rails already laid:
`make -C games/gloamline-nds`, headless check with `--keys` scripting.
Slice-3 gaps knowingly open: ELF-symbol memory-watch (py-desmume
`emu.memory` verified working, no symbol-resolution tool yet), touch
input untested — see session-16 card's guard recipe (incl. the
`keypad_update(0)` boot-poweroff trap; PLATFORM-LIMITS has the verbatim
entry).
health: green (GBA jobs untouched and loop-simulated locally; NDS job
verified step-by-step in-container; `python3 bootstrap.py check --strict`
result in the session card / PR)
kit: v1.12.0 · check: green (last verified kit-upgrade-v1.12.0 slice) ·
engaged: yes
boot: synced to origin/main `4bfcf3a` (PR #50, session-15 concept slice).
No wake landed mid-slice; session number 16 held.
last-shipped: PR #50 (`4bfcf3a`, session 15). This session's own PR
(NDS feasibility: toolchain script, probe ROM, headless checker, CI job,
docs, this status) pending, auto-merge armed.
blockers: none
orders: acked=001,002,003,004 done=001 (this repo's half), 002 (trigger
`trig_0137SkvhXEJvwepX8aVNkcSn` armed, cron `0 * * * *`), 003+004
(executed session 9, condensed record in git history at `4bfcf3a`'s
status). Inbox re-read at HEAD before this write: no new orders. This
slice was coordinator-assigned, claimed via `control/claims/` (claim
released at close-out).
📊 Model: fable-5 (ORDER 003, family-level self-report from this
session's own harness/environment banner).
review-queue: EMPTY (since slice 3).
lane position (honest): **the arc is unblocked end-to-end.** Concept
picked (owner), toolchain proven (this slice), next session builds the
walking skeleton on rails that already exist. The hourly wake trigger
continues between directed slices.
repo-name note (cosmetic, owner's call): this repo is named
`gba-homebrew` but now hosts an NDS game per the owner's direction
(`games/gloamline-nds/` landed this slice); nothing depends on a rename.
kit-follow-ups carried (still unclaimed, low priority): `upgrade
--apply-docs` (14 template-improved docs), migrate/retire legacy root
`claims/` (only its README remains), auto-merge enabler wiring decision
(`adopt --wire-enforcement`), upstream the PR #5 born-red-card gate rule
to substrate-kit

⚑ needs-owner (carried):
- **⚑ owner-click: create the Lumen Drift GitHub Release** (worker seat gets 403 on tags/releases). Suggested tag **`lumen-drift-v1.3`**. Exact clicks: repo → Releases → "Draft a new release" → "Choose a tag" → type `lumen-drift-v1.3` (create on publish, target `main`) → title `Lumen Drift v1.3` → attach `dist/lumen-drift.gba` from the merged tree (167,996 B; sha256 `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94` — paste it in the notes) → point the notes at `docs/PLAYING.md` and the v1.3 entry in `docs/current-state.md` → Publish.
- **⚑ owner-click: merged-branch cleanup** — worker seat gets 403 on branch-delete; merged `claude/*` branches (through PR #50 and this PR once merged, plus the earlier kit-upgrade and wake branches) can be deleted in one sweep from the branches page.
- **⚑ graze tuning wants owner hands-on validation** — the 400-frames-per-graze refund and the 6px shell are reasoned and machine-proven (CI asserts the refund and the lane geometry), but whether grazing FEELS right — risk-priced, readable, not exploitable — needs real hands on a real run. Was slice-5 polish-debt item 4; now owner-gated polish on a complete, shipped game.
- ⚑ (**awareness, no click needed**) — the routine model-attribution mismatch (ORDER 003 record, session 9) belongs in whatever the fleet-manager coordinator compiles for a report to Anthropic, alongside pokemon-mod-lab's parallel finding and the `websites` PR #59 precedent.
notes: worker seat (no tag/release/branch-delete perms — queued above).
Walls carried unchanged: api.github.com curl proxy-walled (GitHub MCP is
the merge path); mGBA python core.load_save() segfault (--savefile
bus-copy is the working path); devkitPro official infra Cloudflare-403
(leseratte10 mirror + SHA-256 pins for GBA; N/A for NDS — Wonderful/
BlocksDS hosts are direct). NEW this slice in `docs/PLATFORM-LIMITS.md`:
py-desmume KEYINPUT-at-reset quirk (BlocksDS ROMs power off at frame 1
without `keypad_update(0)` — verbatim error + recipe recorded). dist/
convention: player-facing games ship committed in `dist/` with provenance
(v1.3 row live); the GBA skeleton AND the NDS probe stay
CI-artifact-only; CI sha256-logs every from-source build.
