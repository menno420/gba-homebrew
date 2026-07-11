# 2026-07-11 — kit upgrade: substrate-kit v1.8.0 → v1.9.0 (distribution wave)

> **Status:** `in-progress`

- **📊 Model:** Claude Fable 5 (claude-fable-5) · distribution worker · kit upgrade v1.8.0 → v1.9.0 (distribution wave)
- **📊 Time:** started Fri Jul 11 2026 (UTC) · in progress

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.9.0 release asset (sha256
`55181082c796657c8e5e14750d248cea2df9e69a9aa896dd8a8c7f1adfb9cc90`),
regenerate kit-owned artifacts, take the v1.9.0 plants, verify
`python3 bootstrap.py check --strict` green. **No domain work; no
control/inbox.md or control/status.md writes** (the lane's own next
heartbeat updates the `kit:` line — the wave seat does not write the
heartbeat).

## What is about to happen

Canonical upgrade: stage verified asset as `bootstrap.py.new` +
`release.json` in repo root → `python3 bootstrap.py.new upgrade`
(self-verifies sha256+version, self-cleans inputs) → verify payload
(`.ignore`/`.gitattributes` plants, SessionStart handoff-push,
model-attribution doctrine, explicit carve-out section, exactly one new
backup `bootstrap-1.8.0.py`, live gate regen) → `check --strict` → ship.
