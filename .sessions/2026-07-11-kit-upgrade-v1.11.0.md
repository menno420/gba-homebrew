# 2026-07-11 — kit upgrade: substrate-kit v1.10.1 → v1.11.0 (distribution wave)

> **Status:** `in-progress`

- **📊 Model:** Claude Fable 5 (`fable-5`) · distribution worker · kit upgrade v1.10.1 → v1.11.0 (distribution wave)
- **📊 Time:** started Sat Jul 11 13:16 UTC 2026

## Scope

Kit-upgrade ONLY (hard scope per wave directive Q-0261.3): replace the vendored
`bootstrap.py` with the pinned v1.11.0 release asset (sha256
`c339bd6a2eb3a139dd0106d5fd3873eb2d067f79723fccb5781d4e72a74a8d29`, tag
`v1.11.0` @ commit 640f8a1, release run 29152928040), regenerate kit-owned
artifacts (live substrate-gate.yml included — actions bump to checkout@v5 /
setup-python@v6), verify `python3 bootstrap.py check --strict` green at close.
No domain work; no control/inbox.md or control/status.md writes — the
heartbeat `kit:` line bump is lane-owed to the resident session lane.

## What is about to happen

Born-red card first (this commit) → canonical `bootstrap.py.new upgrade`
regen commit → verify payload (HANDOFF composer in dist, planted CLAUDE.md
HANDOFF read-first line, exactly-one-new-backup `bootstrap-1.10.1.py`,
carve-out scan, gate action pins, guard-fires dedupe) → flip card `complete`
as the deliberate last commit → REST merge-commit on green. No auto-merge
pre-arm.
