# 2026-07-10 — kit upgrade: substrate-kit v1.6.0 → v1.7.0 (distribution wave)

> **Status:** `complete`

- **📊 Model:** Claude Fable 5 (claude-fable-5) · distribution worker · kit upgrade v1.6.0 → v1.7.0 (owner directive Q-0261.3 wave)
- **📊 Time:** started Fri Jul 10 20:15:41 UTC 2026 · closed Fri Jul 10 20:19:30 UTC 2026 (`date -u`)

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.7.0 release asset, regenerate kit-owned
staged artifacts under `.substrate/`, bump the `substrate.config.json` pin,
verify `python3 bootstrap.py check --strict` green. **No domain work; no
control/inbox.md or control/status.md writes** (the lane's own next heartbeat
updates the `kit:` line — the wave seat does not write the heartbeat).
**The live hand-fixed `.github/workflows/substrate-gate.yml` is NOT touched**
— the kit change making it kit-owned (kit PR #130) is post-v1.7.0; at v1.7.0
only the staged copy `.substrate/ci/substrate-gate.yml` refreshes.

## Shipped

- **PR #26** — substrate-kit **v1.6.0 → v1.7.0** via the pinned release asset
  (sha256 `00f4f4cd39351b17389b9abab3be88fcb0c9f4dee9ad8f1639ad1fc67fdb5238`,
  self-verified against `release.json` by the upgrade engine; inputs
  self-cleaned after the run).
  - `bootstrap.py` v1.6.0 → v1.7.0; `substrate.config.json`
    `kit_version: 1.7.0`; staged `.substrate/` artifacts regenerated
    (claude/CLAUDE.md, 7 skills, 3 agents, hooks templates, ci staged copies);
    backup + `upgrade-report.md` banked (`--rollback` available).
  - Live `.github/workflows/substrate-gate.yml` verified byte-identical
    before/after (sha256 `f1a94be8…` unchanged) — kit PR #130 ships it
    kit-owned only in the NEXT release.
  - `python3 bootstrap.py check --strict` green at close (the only mid-session
    finding was this card's own born-red badge, by design).
- **Not taken, deliberately:** `--apply-docs`. The report classifies 14 planted
  docs `template-improved` (consumer-untouched) and `control/README.md`
  `diverged` (adds the `adopt --lane` shared-repo paragraph — manual merge).
  Both are lane decisions, not wave scope; a later
  `python3 bootstrap.py upgrade --apply-docs` applies the 14 post-hoc from the
  banked archive, no rollback needed. See `.substrate/upgrade-report.md`.

## Follow-ups for the lane (decide-and-flag)

- Run `python3 bootstrap.py upgrade --apply-docs` to take the 14
  template-improved doc re-renders; hand-merge the `control/README.md`
  divergence (template delta is in the upgrade report).
- Update the `kit:` line in `control/status.md` to `v1.7.0 · check: green ·
  engaged: yes` on the next heartbeat (wave seat does not write the heartbeat).

## 💡 Session idea

The upgrade engine's `check --strict` red on a mid-session born-red card is
correct locally but noisy in an upgrade PR: teach the kit's upgrade verb (or
the checklist) a `--session-log <card>` pass-through so the "green before
shipping" verification step can assert *everything except the card I am
required to keep in-progress* — today the operator must eyeball that the only
finding is their own card, which is exactly the kind of judgment call a
checker should make.

## ⟲ Previous-session review

Reviewed session 7 (PRs #22/#23) from its card + merged history: exemplary
close-out — evidence-dense card, guard recipe extended with the negative-shift
case, claim released, status overwritten. One workflow improvement it
surfaces: the card's CI timings and proof pointers are hand-transcribed; the
kit's telemetry-backfill (v1.7.0 #91) now lands model/time lines at
card-commit, so the lane should lean on it rather than hand-writing the 📊
lines — this upgrade just delivered that mechanism.
