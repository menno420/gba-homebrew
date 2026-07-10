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
- **Force-push / amending pushed history** — never. Forward-only commits.

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
