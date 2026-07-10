# 2026-07-10 — kit upgrade: substrate-kit v1.6.0 → v1.7.0 (distribution wave)

> **Status:** `in-progress`

- **📊 Model:** Claude Fable 5 (claude-fable-5) · distribution worker · kit upgrade v1.6.0 → v1.7.0 (owner directive Q-0261.3 wave)
- **📊 Time:** started Fri Jul 10 20:15:41 UTC 2026 (`date -u`)

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
