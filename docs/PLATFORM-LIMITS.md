# Platform limits — verified walls (gba-homebrew copy)

> **Status:** `living-ledger` — carried over at seed 2026-07-10 from the fleet
> capability manifest (`menno420/fleet-manager` `docs/capabilities.md`, WALLED
> section) + the venture-lab gen-2 copy + the toolchain-proof findings.
> **Probing a documented wall twice is a bug.** Quote the observed error
> verbatim when appending a new wall — never paraphrase.

## Verified walls

- **Official devkitPro package installers/infra** — **Cloudflare 403** behind
  the fleet proxy (toolchain scout 2026-07-09). Don't re-probe; the working
  route is the **leseratte10 community mirror** recipe in
  [`capabilities.md`](capabilities.md) (⚠ unsigned community infra —
  supply-chain caveat).
- **Direct push to `main`** — blocked by repo ruleset. Observed at seed
  (2026-07-10), verbatim: `failed to update reference: PATCH
  https://api.github.com/repos/menno420/gba-homebrew/git/refs/heads/main:
  422 Repository rule violations found — Changes must be made through a pull
  request.` Quirk worth knowing: the very FIRST Contents-API commit to the
  empty repo (`create_or_update_file` creating `main`) went through; every
  subsequent push to `main` is ruleset-blocked. All changes land via PR; with
  no CI at seed, REST merge is the landing path (R21) — once the "ROM builds"
  check exists, merge on green.
- **Tag push, GitHub Release creation, remote branch deletion** — fail with
  **403 at the credential layer** → owner action required. Sanctioned release
  path: an Actions `workflow_dispatch` workflow — its `GITHUB_TOKEN`
  tags/releases server-side (proven by the codetool-lab-opus4.8 lane). Queue
  the owner click under ⚑ needs-owner (WHAT/WHERE/HOW/WHY/UNBLOCKS).
- **Creating/editing claude.ai environments, routines, or Projects; GitHub
  repo settings/rulesets** — no API surface for the agent → **owner clicks**
  in the claude.ai / GitHub UI. Queue click-level under ⚑ needs-owner.
- **Direct self-merge of own PRs in established repos** — blocked by the
  classifier (**"[Self-Approval]…Merge Without Review"**). **Arming auto-merge
  while checks are pending is the sanctioned path** — the wall blocks the
  direct merge call, not the arm (fleet playbook R12). On a born-red or no-CI
  repo, arming is structurally impossible — **REST merge-on-green is PRIMARY**
  there (fleet playbook R21).
- **GraphQL quota exhausts at fleet scale (~hourly)** — REST merge-on-green is
  the fallback; ready-flips (draft→ready) are GraphQL-only, so wait for quota
  reset for those. (Fleet playbook R8.) Moot here if you never draft — see
  [`conventions.md`](conventions.md): READY, never draft.
- **Auto-merge can fire BEFORE the `nds-rom-build` job completes** — this
  repo's required-check set is the GBA **"ROM builds"** job only, so an armed
  auto-merge merges the moment that job goes green (surfaced during the
  slice-4/5 sessions, 2026-07-11: the NDS proofs can still be running at
  merge time, finishing on the post-merge `main` push run). Not a wall but a
  footgun. Discipline until the owner adds `NDS ROM build` to the required
  checks (⚑ queued in `control/status.md`): after any merge, **verify the
  post-merge `main` workflow run is green and say so** in the session
  record — a merge is not "on green" until that run is.
- **Force-push / amending pushed history** — never. Forward-only commits.
- **`api.github.com` REST calls to repos OUTSIDE the session** — proxy-blocked
  in-container (session 1, 2026-07-10). Observed verbatim (GETting
  `repos/GValiente/butano/releases/latest`): `{"message":"GitHub access to
  this repository is not enabled for this session. Use add_repo to request
  access.","documentation_url":"https://docs.anthropic.com/en/docs/claude-code/github-actions"}`.
  NOT a full wall: plain `git ls-remote`/`git clone` of public repos works —
  recipe in [`capabilities.md`](capabilities.md).
- **mGBA python binding `core.load_save()` + a file VFile** — segfaults the
  pinned binding (mgba==0.10.2 on system libmgba 0.10.x) ~64 frames AFTER the
  game's first SRAM write, when mGBA's deferred savedata flush fires (attach,
  boot, SRAM reads and even the write itself are all fine — crash reproduces
  at 400 frames but not 380 with the write at ~332). Observed verbatim
  (session 5, 2026-07-10, `python3 -X faulthandler`): `Fatal Python error:
  Segmentation fault` / `Current thread 0x00007fa4bda3f080 (most recent call
  first):` / `File "/usr/local/lib/python3.11/dist-packages/mgba/core.py",
  line 269 in run_frame` — exit code 139. NOT a persistence wall: the
  harness's `--savefile` copies the file through the emulated bus instead
  (`core.memory.sram.u8` after `reset()`, snapshot back after the last
  frame) — recipe in [`capabilities.md`](capabilities.md); SRAM power-cycle
  persistence is fully proven headlessly.

- **py-desmume binding: KEYINPUT reads all-pressed at reset → BlocksDS ROMs
  "power off" at frame 1** — NOT a wall, a one-line-fix quirk (recipe below),
  recorded 2026-07-11 (NDS feasibility slice) so nobody re-derives it. The
  py-desmume==0.0.9 core reads `REG_KEYINPUT` as active-low ZERO (= every
  button held) until the frontend's first keypad latch; the BlocksDS default
  ARM7 core treats SELECT+START+L+R as its exit combo, so its main loop
  exits on frame 1 and libnds powers the console off. Observed verbatim
  (every BlocksDS .nds, including a bare `while(1) swiWaitForVBlank();`):
  `SYSTEM POWERED OFF VIA ARM7 SPI POWER DEVICE` / `Did your main()
  return?` — with a blank framebuffer ever after. **Fix:** call
  `emu.input.keypad_update(0)` BEFORE the first `emu.cycle()` (done in
  `tools/nds-headless-check.py` — keep it). Also needed headlessly:
  `SDL_VIDEODRIVER=dummy` + `SDL_AUDIODRIVER=dummy`, or DeSmuME init fails
  with `Error trying to initialize SDL: No available video device`.
- **Wonderful/BlocksDS package infra is NOT walled** (recorded 2026-07-11 so
  nobody assumes the devkitPro 403 generalizes): `wonderful.asie.pl` and
  `blocksds.skylyrac.net` both serve 200 through the fleet proxy — direct
  fetch + SHA-256 pin in `tools/setup-nds-toolchain.sh`, no mirror needed.

## Dated platform-issue notes (owner-reported, not verbatim-error walls)

> Unlike the walls above, entries here are **owner-reported findings** from
> live debug sessions — attributed as such and NOT independently re-verified
> by the recording seat. Each entry names its verification hook so a later
> session can confirm or retire it.

### 2026-07-11 — hourly-wake routine (owner debug session ~10:36–11:05Z)

Owner-debugged findings about this lane's hourly-wake routine (ORDER 002,
`trig_0137SkvhXEJvwepX8aVNkcSn`), reported by the owner from a live debug
session ~10:36–11:05Z today; recorded 2026-07-11 by the session-8 worker
seat (owner-reported facts, not claims verified by this seat):

1. **Routine ownership/editing:** routines created from inside project
   sessions cannot be created by the owner personally, but they DO appear in
   the owner's Routines screen, and their **repo scope is editable there**.
   The owner has added BOTH repos (`gba-homebrew`, `pokemon-mod-lab`) to the
   hourly-wake routine's config. This is the fix path for **OWNER-ACTION 4**
   (on record in the pokemon lane's `control/status.md` as OA-4: wake
   sessions stranding PR-less branches for lack of GitHub write access).
   ~~**Verification pending:** the first wake session that successfully opens
   a PR should be recorded here (and in `control/status.md`) when observed.~~
   **Verified 2026-07-11 (hook closed): the OA-4 fix path is CONFIRMED
   WORKING — the first verified PR-capable wake has been observed.**
   Evidence, coordinator-verified via GitHub API (this seat did not
   re-verify pokemon-lane PR contents itself and made no writes there; a
   read-only `git ls-remote` from this seat confirmed the repo is reachable,
   with the wake branch ref already absent — consistent with post-merge
   deletion):
   - pokemon-mod-lab **PR #30** — "session 025: hourly wake heartbeat +
     shepherd session-024", head branch `claude/eloquent-newton-4ihg8r`,
     **merged 2026-07-11T12:06:21Z**. Its body self-identifies as an hourly
     wake and states it has **working GitHub MCP write tooling** — updating
     ⚑ OWNER-ACTION 4's premise (wake sessions are no longer PR-less).
   - The wake even **shepherded a stranded sibling branch itself** (it
     opened pokemon-mod-lab #29; that content landed via **#31** after a CI
     marker fix) — the exact failure mode OA-4 described, now being cleaned
     up BY a wake session rather than caused by one.
   - This repo's own corroborating data point is already on record: session
     9 (the first bare-wake PR in gba-homebrew's ledger) needed no
     `add_repo` call — see `control/status.md`'s ORDER 003 finding.
   - **Branch-name quirk (new, carried):** wake branches come out with
     harness-assigned names of the form `claude/...` instead of the lane's
     `track-a/...` convention — cosmetic, but worth knowing when auditing
     branch lists for stranded wake work.
2. **Model mismatch (configured vs actual):** the owner reports the routine
   is configured to run **Opus 4.8**, but wake sessions report actually
   running a **Fable-family model**. Recorded as an observed
   configured-vs-actual mismatch; no workaround attempted.
3. **OWNER-ACTION 5** (on record in the pokemon lane's `control/status.md`
   as OA-5): wake sessions' `add_repo` step is denied by the platform's
   auto-mode classifier roughly 1-in-2 attempts today ("Unauthorized
   Persistence"), so some wake hours **silently produce no lane session** —
   separate from, and compounding, the OA-4 gap (an hour can fail before
   the repo-access question even arises).

## Mission-specific rails (not platform walls — owner rails, equally hard)

- **PRIVATE/PUBLIC SPLIT:** this repo is PUBLIC — nothing Nintendo-derived
  (Track A / pokemon-mod-lab material) may ever enter it. See
  [`conventions.md`](conventions.md) rule 13.
- **External publishing of Track B (itch.io, forums, anywhere) requires an
  owner action; NO spend, NO account creation, NO payment flows** without an
  explicit owner action — queue click-level, never perform.

## Discovery rule

Before declaring anything impossible: check this file +
[`capabilities.md`](capabilities.md) → check `printenv` → attempt once and
capture the exact error verbatim → append the finding the same session.
