# 2026-07-10 — kit upgrade: substrate-kit v1.7.0 → v1.7.1 (distribution wave)

> **Status:** `in-progress`

- **📊 Model:** Claude Fable 5 (claude-fable-5) · distribution worker · kit upgrade v1.7.0 → v1.7.1 (owner directive Q-0261.3 wave)
- **📊 Time:** started Fri Jul 10 21:49:20 UTC 2026 (`date -u`)

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
