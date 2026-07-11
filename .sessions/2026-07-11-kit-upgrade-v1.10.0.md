# 2026-07-11 — kit upgrade: substrate-kit v1.9.0 → v1.10.0 (distribution wave)

> **Status:** `in-progress`

- **📊 Model:** Claude Fable 5 (`fable-5`) · distribution worker · kit upgrade v1.9.0 → v1.10.0 (distribution wave)
- **📊 Time:** started Sat Jul 11 07:21 UTC 2026

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.10.0 release asset via
`upgrade --apply-docs` (single invocation — this wave the doc re-renders
carried since v1.7.0 land per directive), regenerate kit-owned artifacts,
verify `python3 bootstrap.py check --strict` green at close. **No domain
work; no control/inbox.md or control/status.md writes** (the lane's own
next heartbeat updates the `kit:` line — the wave seat does not write the
heartbeat).
