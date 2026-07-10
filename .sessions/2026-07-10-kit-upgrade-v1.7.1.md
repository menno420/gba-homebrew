# 2026-07-10 — kit upgrade: substrate-kit v1.7.0 → v1.7.1 (distribution wave)

> **Status:** `complete`

- **📊 Model:** Claude Fable 5 (claude-fable-5) · distribution worker · kit upgrade v1.7.0 → v1.7.1 (owner directive Q-0261.3 wave)
- **📊 Time:** started Fri Jul 10 21:49:20 UTC 2026 · closed Fri Jul 10 21:53 UTC 2026 (`date -u`)

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.7.1 release asset (sha256
`2aa4fedd…3118486d`, verified), regenerate kit-owned artifacts, bump the
`substrate.config.json` pin, verify `python3 bootstrap.py check --strict`
green. **No domain work; no control/inbox.md or control/status.md writes**
(the lane's own next heartbeat updates the `kit:` line — the wave seat does
not write the heartbeat).
**v1.7.1 makes the live `.github/workflows/substrate-gate.yml` kit-owned**:
the upgrade regenerates it in place and banks the pre-regen copy to
`.substrate/backup/`. This repo's live gate is hand-fixed but functionally
identical to the v1.7.0 staged template (comment-wording diff only, verified
pre-upgrade) — expect a functionally-identical regen plus the v1.7.1
`--inbox-base` wiring; the diff is recorded below at close-out.

## What shipped

- **PR #27** — substrate-kit **v1.7.0 → v1.7.1** via the pinned release asset
  (sha256 `2aa4feddf7de7e20b00f46866826985ca8fd11f40bc51ebe261bbdef3118486d`,
  self-verified against `release.json` by the upgrade engine; inputs
  self-cleaned after the run).
  - `bootstrap.py` v1.7.0 → v1.7.1; `substrate.config.json`
    `kit_version: 1.7.1`; staged `.substrate/` artifacts regenerated.
  - **Live gate regen (kit-owned as of v1.7.1):**
    `.github/workflows/substrate-gate.yml` regenerated in place, now
    byte-identical to the staged `.substrate/ci/substrate-gate.yml`.
    Functional delta vs the previous hand-fixed gate is ONLY the new
    v1.7.1 *inbox append-only gate* step (`check --strict --status-only
    --inbox-base "$basefile"`, line 78) — every hand-fixed behavior
    (control fast lane, control-status gate, added/modified session-card
    split) survived identically; the rest of the diff is comment wording
    plus the KIT-OWNED header.
  - **No pre-regen bank, and that is correct:** the engine banks
    `.substrate/backup/substrate-gate.pre-regen-*.yml` ONLY when it detects
    host-added-job carve-outs (`gate_carveouts`, bootstrap.py ~L9407); this
    repo's hand-fix was comment-only → zero carve-outs → no bank. The
    pre-regen copy is preserved in git history (main @ `bc73da7`).
  - **Carve-outs: none** — `upgrade-report.md` has no carve-out section.
  - **#137 single-backup pass:** `bootstrap-1.7.0.py` was already banked
    byte-identical (pre-#137 verb banked the new dist during v1.6.0→v1.7.0);
    the verb reported `already banked`, created no new archive, and no
    `bootstrap-1.7.1.py` exists — backup file unmodified in git, as the
    recipe predicts.
  - `python3 bootstrap.py check --strict` green at close (the only
    mid-session finding was this card's own born-red badge, by design).
- **Not taken, deliberately (unchanged from the v1.7.0 session):**
  `--apply-docs`. The report classifies 14 planted docs `template-improved`;
  applying them is a lane decision, not wave scope
  (`python3 bootstrap.py upgrade --apply-docs` works post-hoc). Note
  `control/README.md` now classifies `consumer-edited` (the lane merged the
  divergence since the v1.7.0 report) — 5 consumer-edited / 14
  template-improved / 0 diverged.

## Follow-ups for the lane (decide-and-flag)

- Run `python3 bootstrap.py upgrade --apply-docs` to take the 14
  template-improved doc re-renders (carried over from the v1.7.0 card).
- Update the `kit:` line in `control/status.md` to `v1.7.1` on the next
  heartbeat (wave seat does not write the heartbeat).

## ⚑ Flags

- ⚑ Self-initiated: none — owner-directed wave work only (Q-0261.3).
- Live gate hand-ownership ended this PR: future gate fixes arrive via kit
  upgrades; host-specific CI belongs in a separate workflow file.

## 💡 Session idea

The engine banks the pre-regen live gate only when `gate_carveouts` finds
host-added jobs/steps — a comment-only or wording-level hand-fix is silently
overwritten with no bank, and an adopter who *documented an incident* in gate
comments (as this repo did, PRs #2/#3 forensics) loses that text with no
pointer. Cheap fix in the kit: when the live gate differs from template@new
but has zero carve-outs, print a one-line `note: hand edits (comments only)
overwritten; previous copy at <git ref>` in the report — evidence-preserving
without banking noise.

## ⟲ Previous-session review

Reviewed the v1.7.0 kit-upgrade session (PR #26, card
`2026-07-10-kit-upgrade-v1.7.0.md`): clean three-commit shape, correct
refusal to touch the then-hand-owned live gate, and its "kit PR #130 ships
the gate kit-owned NEXT release" prediction was exactly right — this session
executed it. One improvement it surfaces: that card's follow-ups
(`--apply-docs`, status heartbeat) were still unclaimed by any lane session
when this wave seat arrived. Follow-ups written into a wave card have no
routing mechanism into the lane's backlog; the lane should sweep
`.sessions/*kit-upgrade*` cards for open follow-ups on its next heartbeat.
