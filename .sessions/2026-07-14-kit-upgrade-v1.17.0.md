# Session — substrate-kit upgrade v1.16.0 → v1.17.0

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/kit-upgrade-v1.17.0`)
- mission: upgrade the vendored substrate-kit from v1.16.0 to v1.17.0
  (distribution wave lane, Q-0261.3 scope — kit files only, no
  control/ edits, no domain work, host workflows restored byte-identical
  if regenerated).
- **📊 Model:** fable-5 · medium · mechanical refactor — kit upgrade wave

About to: verify the v1.17.0 release asset three-way (sha256
`0d08b8aa9efc30178cf8e8befcfa28dd2b65e02106cc9ba6d520133017955521`,
995,446 bytes), run the two-command upgrade
(`python3 bootstrap.py.new upgrade` + `python3 bootstrap.py upgrade
--apply-docs`), restore any host workflow the regen touches, gate on
`check --strict`, flip this card complete last.
