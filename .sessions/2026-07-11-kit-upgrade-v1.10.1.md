# 2026-07-11 — kit upgrade: substrate-kit v1.10.0 → v1.10.1 (distribution wave)

> **Status:** `complete`

- **📊 Model:** Claude Fable 5 (`fable-5`) · distribution worker · kit upgrade v1.10.0 → v1.10.1 (distribution wave)
- **📊 Time:** started Sat Jul 11 09:25 UTC 2026 · closed Sat Jul 11 09:33 UTC 2026

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.10.1 release asset (sha256
`fbe83ce35d1fb3b544ac58fc60ee2609eaa6c69c13d77883e9fdc5da6bbad158`, tag
`v1.10.1` @ commit 7e361bb, release run 29146372884), regenerate kit-owned
artifacts (live substrate-gate.yml included), verify
`python3 bootstrap.py check --strict` green at close. Heartbeat `kit:` line
bumped to v1.10.1 in the flip commit (that line only). No domain work; no
control/inbox.md writes.

## What shipped

- **PR #38** — substrate-kit **v1.10.0 → v1.10.1** via the pinned release
  asset, verified locally against the wave pin AND self-verified by the
  upgrade engine against the adjacent `release.json` (staged inputs
  self-cleaned).
  - `bootstrap.py` v1.10.0 → v1.10.1; `substrate.config.json` +
    `.substrate/state.json` `kit_version: 1.10.1`; staged `.substrate/`
    artifacts regenerated.
  - **Backup:** exactly ONE new archive —
    `.substrate/backup/bootstrap-1.10.0.py` (sha256 `ba69fc5c…`, byte-equal
    to the pre-upgrade dist); all five pre-existing banks byte-identical
    before/after (hash-verified).
  - **Live gate regen (kit-owned):** the gate now grades EVERY card in the
    PR's session-card diff (the `tail -1` multi-card shadowing fix,
    venture-lab #33 class): each ADDED card walks the added-card lane (any
    added in-progress card → HOLD), siblings MODIFIED alongside an added
    card are advisory-only, modified-only diffs keep the locked door per
    card, and a gate-touching PR routes every ADDED card through the FULL
    locked door + `--simulate-added-card`.
  - **First exercise, live:** card-only head cde53ee held under the OLD
    v1.10.0 gate (run 29147711134); regen head 8dc6d69 engaged the NEW
    gate's locked door — run 29147743695 printed
    `session gate cards: .sessions/2026-07-11-kit-upgrade-v1.10.1.md`, the
    gate-touching locked-door line, the
    `simulate-added-card … would HOLD (born-red …)` advisory, and the
    `HOLD (by design)` banner + `##[notice]`. Hold semantics tightened,
    never loosened, inside this PR — as designed.
  - **Carve-out scan:** ran, 0 found (section present in
    `.substrate/upgrade-report.md`); pre/post gate diff confirms only
    kit-template deltas — no host-added jobs/steps existed or were lost.
  - **Model-attribution doctrine (emphasis-blind in v1.10.1):** NO-OP —
    `.sessions/README.md` byte-identical before/after (sha256
    `cae82297…` unchanged); no duplicate append.
  - **Heartbeat:** `control/status.md` `kit:` line v1.8.0 → v1.10.1 (was
    two waves stale; the chronic-drift class the release checklist closes).
- **Verification:** `python3 bootstrap.py check --strict` green with this
  card complete (locked-door invocation exit 0 locally before the flip
  push); no `--apply-docs` this wave (docs already current from v1.10.0's
  pass).

## 💡 Session idea

The heartbeat `kit:` line was two versions stale (v1.8.0 while the tree ran
v1.10.0) because upgrade sessions were scoped away from status.md until this
wave. Kit idea: `bootstrap check` could emit an advisory when
`control/status.md`'s `kit:` line disagrees with `KIT_VERSION` in the tree —
the drift is machine-detectable and chronic across adopters (6 repos owed
bumps after the v1.10.0 wave).

## ⟲ Previous-session review

The v1.10.0 upgrade session (PR #37) was clean and well-evidenced — its card
documented the backup-bank hashes and the first live exercise of the
added-card lane, which made this session's verification trivially
comparable. One miss worth naming: it left the `kit:` heartbeat line at
v1.8.0 (scoped out by its directive), so the stale-heartbeat class survived
two waves; this wave's directive folds the bump into the flip commit, which
is the right durable fix — the improvement is already adopted.
