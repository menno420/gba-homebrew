# 2026-07-11 — kit upgrade: substrate-kit v1.11.0 → v1.12.0 (distribution wave)

> **Status:** `in-progress`

- **📊 Model:** Claude Fable 5 (`fable-5`) · distribution worker · kit upgrade v1.11.0 → v1.12.0 (distribution wave)
- **📊 Time:** started Sat Jul 11 17:30 UTC 2026

## Scope

Kit-upgrade ONLY (hard scope per wave directive Q-0261.3): replace the vendored
`bootstrap.py` with the pinned v1.12.0 release asset (sha256
`77c00b811429e1b526ccc7e0dcf597435c11048e16a67edba6050f516ad5e1f8`, tag
`v1.12.0` @ commit b310aba, release run 29160292286), regenerate kit-owned
artifacts (live substrate-gate.yml included), verify
`python3 bootstrap.py check --strict` green at close. No domain work; no
control/inbox.md or control/status.md writes — the heartbeat `kit:` line bump
is **lane-owed** to the resident session lane, not done here.

This repo is the wave's **first live exercise of the v1.12.0 carve-out
scanner three-way compare** (kit #210) on a repo with a LIVE substrate-gate —
the v1.11.0 wave's phantom bank
`.substrate/backup/substrate-gate.pre-regen-4f50eb4d.yml` was created by the
exact false-positive class this fix targets. Scan lines will be captured
verbatim below.

## What shipped

(in progress — filled at close-out)
