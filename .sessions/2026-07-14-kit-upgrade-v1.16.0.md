# Session — substrate-kit upgrade v1.15.0 → v1.16.0

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/kit-upgrade-v1.16.0`)
- mission: upgrade the vendored substrate-kit from v1.15.0 to v1.16.0
  (distribution wave lane, Q-0261.3 scope — kit files only, no
  control/ edits, no domain work, host workflows untouched).
- 📊 Model: fable-5

## What is about to happen

Download the v1.16.0 release asset, three-way sha256 verify, run the
mandatory two-command upgrade (`python3 bootstrap.py.new upgrade` then
`python3 bootstrap.py upgrade --apply-docs`), verify
`check --strict`, commit batched, flip this card `complete` last.
