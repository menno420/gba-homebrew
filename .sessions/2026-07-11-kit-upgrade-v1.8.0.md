# 2026-07-11 — kit upgrade: substrate-kit v1.7.1 → v1.8.0 (distribution wave)

> **Status:** `in-progress`

- **📊 Model:** Claude Fable 5 (claude-fable-5) · distribution worker · kit upgrade v1.7.1 → v1.8.0 (distribution wave)
- **📊 Time:** started Sat Jul 11 01:10:40 UTC 2026 (`date -u`)

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.8.0 release asset (sha256
`28c5dcb6…c89b9b`, verify), regenerate kit-owned artifacts (live
`substrate-gate.yml` included — carve-out scan expected), take the v1.8.0
plants (`control/claims/README.md`, `scripts/env-setup.sh`), perform the
report-prescribed manual merge for diverged `control/README.md`, bump the
`substrate.config.json` pin, verify `python3 bootstrap.py check --strict`
green. **No domain work; no control/inbox.md or control/status.md writes**
(the lane's own next heartbeat updates the `kit:` line — the wave seat does
not write the heartbeat).

Note: this PR touches the kit-owned gate workflow itself, so per the v1.8.0
mid-PR tightening rule (#156) this ADDED card rides the FULL locked door —
the PR merges only once this badge flips `complete`.
