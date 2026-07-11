# 2026-07-11 — kit upgrade: substrate-kit v1.10.0 → v1.10.1 (distribution wave)

> **Status:** `in-progress`

- **📊 Model:** Claude Fable 5 (`fable-5`) · distribution worker · kit upgrade v1.10.0 → v1.10.1 (distribution wave)
- **📊 Time:** started Sat Jul 11 09:25 UTC 2026

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.10.1 release asset (sha256
`fbe83ce35d1fb3b544ac58fc60ee2609eaa6c69c13d77883e9fdc5da6bbad158`, tag
`v1.10.1` @ commit 7e361bb, release run 29146372884), regenerate kit-owned
artifacts (live substrate-gate.yml included), verify
`python3 bootstrap.py check --strict` green at close. Heartbeat `kit:` line
bump to v1.10.1 in the flip commit (that line only). No domain work; no
control/inbox.md writes.

## About to do

1. Born-red card (this commit) → open PR immediately (in-flight signal; no
   auto-merge pre-arm — card-only loophole is open under the OLD gate on
   this first head).
2. Canonical upgrade: `bootstrap.py.new` + adjacent `release.json` →
   `python3 bootstrap.py.new upgrade`.
3. Verify: multi-card grading in the regenerated live gate; exactly ONE new
   backup (`bootstrap-1.10.0.py`, sha256 `ba69fc5c…`), pre-existing banks
   untouched; carve-out section intact in `.substrate/upgrade-report.md`;
   doctrine phrase no duplicate-append in `.sessions/README.md`;
   `check --strict` exit 0.
4. Flip this card `complete` as the deliberate LAST commit; REST
   merge-commit on green.
