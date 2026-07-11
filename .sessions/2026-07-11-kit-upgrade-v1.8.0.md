# 2026-07-11 — kit upgrade: substrate-kit v1.7.1 → v1.8.0 (distribution wave)

> **Status:** `complete`

- **📊 Model:** Claude Fable 5 (claude-fable-5) · distribution worker · kit upgrade v1.7.1 → v1.8.0 (distribution wave)
- **📊 Time:** started Sat Jul 11 01:10:40 UTC 2026 · closed Sat Jul 11 01:14 UTC 2026 (`date -u`)

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.8.0 release asset, regenerate kit-owned
artifacts, take the v1.8.0 plants, perform the report-prescribed manual
merge for diverged `control/README.md`, bump the `substrate.config.json`
pin, verify `python3 bootstrap.py check --strict` green. **No domain work;
no control/inbox.md or control/status.md writes** (the lane's own next
heartbeat updates the `kit:` line — the wave seat does not write the
heartbeat).

## What shipped

- **PR #28** — substrate-kit **v1.7.1 → v1.8.0** via the pinned release
  asset (tag `v1.8.0`, release id 352386319, commit 63c6b39; 625,066 bytes,
  sha256 `28c5dcb64b713dde8d64a513a9a1aa860b4a07bf17d832686f0009932dc89b9b`
  — verified locally against the release digest AND self-verified by the
  upgrade engine against the adjacent `release.json`; inputs self-cleaned).
  - `bootstrap.py` v1.7.1 → v1.8.0; `substrate.config.json`
    `kit_version: 1.8.0` + new `automerge` block
    (`branch_patterns: ["claude/*"]`, `required_context: substrate-gate`)
    + `claims_dir: control/claims`; staged `.substrate/` artifacts
    regenerated.
  - **Live gate regen (kit-owned):** `.github/workflows/substrate-gate.yml`
    regenerated; the functional delta is the #156 **mid-PR hold-tightening
    rule** — a PR that ADDS a session card and also touches the gate file
    routes the added card through the full locked door (this very PR ran
    under it, by design). **Carve-out scan: ran, 0 found** — the v1.8.0
    explicit-when-clean report line; `upgrade-report.md` carries the
    `## Carve-out scan` section. No pre-regen bank, and that is correct:
    banking is conditional on detected carve-outs.
  - **Plants:** `control/claims/README.md` (unified one-file-per-claim
    work-claim convention; this session dogfooded it —
    `control/claims/claude-kit-upgrade-v1.8.0.md`, deleted at close) and
    `scripts/env-setup.sh` (setup-script contract; no host script
    pre-existed — planted fresh, skip-if-exists not exercised).
  - **Auto-merge enabler: STAGED only** at
    `.substrate/ci/auto-merge-enabler.yml`. No live enabler pre-existed, so
    the kit-owned regen semantics correctly did NOT install it live;
    `adopt --wire-enforcement` is the lane's opt-in path.
  - **Backup:** exactly ONE new archive —
    `.substrate/backup/bootstrap-1.7.1.py` — no collision lines
    (`bootstrap-1.7.0.py` already banked from the prior wave, untouched).
  - **Manual merge (report-prescribed):** `control/README.md` classed
    `diverged` — applied the claims-section template delta at the host
    anchor (before "Rules that ride the protocol"), all host content
    preserved. The template's three "Grammar source of truth" delta lines
    anchor to format blocks this host rewrite does not contain, so they do
    not apply; a provenance note inside the merged section records this and
    points at the kit's `src/engine/grammar.py`.
  - `python3 bootstrap.py check --strict` green at close (the only
    mid-session finding was this card's own born-red badge, by design).
- **Not taken, deliberately (unchanged from the v1.7.0/v1.7.1 sessions):**
  `--apply-docs`. The report classifies 14 planted docs `template-improved`;
  applying them is a lane decision, not wave scope
  (`python3 bootstrap.py upgrade --apply-docs` works post-hoc).

## Follow-ups for the lane (decide-and-flag)

- Update the `kit:` line in `control/status.md` to `v1.8.0 · check: green`
  on the next heartbeat (wave seat does not write the heartbeat).
- Run `python3 bootstrap.py upgrade --apply-docs` for the 14
  template-improved doc re-renders (carried since v1.7.0).
- Legacy root `claims/` now draws an advisory `claims-legacy-location`
  nudge — migrate to `control/claims/` (only `claims/README.md` exists;
  fold its content or delete once the lane confirms nothing references it).
- Decide whether to wire the staged auto-merge enabler live
  (`adopt --wire-enforcement` + the repo-settings one-time checklist).

## ⚑ Flags

- ⚑ Self-initiated: none — directed wave work only.

## 💡 Session idea

The v1.8.0 diverged-doc report prints the full template delta as a diff,
but when the host doc is a ground-up rewrite (this repo's fleet-manager
`control/README.md`), most hunks have no anchor and the merger silently
judges applicability hunk-by-hunk. Cheap kit improvement: the report could
classify each delta hunk as `anchored` (context lines found in host doc)
vs `unanchored` — the merging agent then knows mechanically which hunks the
template expects to apply and which need judgment, and a reviewer can audit
the merge against that list instead of re-deriving it.

## ⟲ Previous-session review

Reviewed the v1.7.1 kit-upgrade session (PR #27, card
`2026-07-10-kit-upgrade-v1.7.1.md`): exemplary — its pre-upgrade
verification that the hand-fixed gate was comment-only-divergent correctly
predicted the zero-carve-out regen, and its session idea (report a pointer
when comment-only hand edits are overwritten) was effectively answered by
v1.8.0's explicit-when-clean carve-out section. One improvement it
surfaces, still open: its "Follow-ups for the lane" (apply-docs, status
heartbeat) remain unclaimed two waves later — wave cards still have no
routing into the lane's backlog. The lane should sweep
`.sessions/*kit-upgrade*` follow-ups on its next heartbeat; this card
re-lists them rather than assuming the sweep.
