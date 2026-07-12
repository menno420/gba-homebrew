# Session — substrate-kit upgrade v1.14.0 → v1.15.0

> **Status:** `complete`

- date: 2026-07-12 (branch `claude/kit-upgrade-v1.15.0`, PR #72)
- mission: upgrade the vendored substrate-kit from v1.14.0 to v1.15.0
  (distribution wave B lane, Q-0261.3 scope — kit files only, no
  control/ edits, no domain work).
- 📊 Model: fable-5

## What shipped

- Asset three-way verified before use: sha256
  `25d22af9d9d81b2a7dc6d8d500234b6aa0f3f1c6a0400284ce2381baaeac650e`,
  828,825 bytes (tag v1.15.0, release run 29198852306).
- Two-command flow: `python3 bootstrap.py.new upgrade` then
  `python3 bootstrap.py upgrade --apply-docs`; `check --strict` red
  only on this card's own designed born-red hold.
- Banked exactly one new archive `.substrate/backup/bootstrap-1.14.0.py`
  (sha256 47c1b8b9…2ee581 = the v1.14.0 dist sha); all 11 pre-existing
  banks byte-identical.
- Planted: `docs/ROUTINES.md` (trigger/wake doctrine) and
  `docs/seat-digest.md` (generated seat digest, refreshed at upgrade
  time). Applied via --apply-docs: CONSTITUTION.md,
  docs/AGENT_ORIENTATION.md, docs/SKILLS.md. Carve-out scan: ran, 0
  found; live gate kit-owned, already current. Capability seed: no
  refresh needed (CAPABILITIES template identical; v1.14.0 fence
  present).
- Lane-owed (reported, untouched): control/README.md + control/status.md
  diverged (heartbeat-grammar negative example + version-truth
  deference deltas preserved in .substrate/upgrade-report.md);
  heartbeat `kit:` bump. NOTE: the v1.14.0 control/README.md delta
  (OWNER-ACTION `RISK:` line + owner-assist output standard) is STILL
  unapplied and was dropped from the fresh report by the wholesale
  overwrite — recover it from PR #71's report in git history.

## 💡 Session idea

The upgrade-report wholesale overwrite silently dropped gba-homebrew's
still-unapplied v1.14.0 control/README.md delta this wave (second live
sighting of the class after superbot-games' AGENT_ORIENTATION loss).
Guard recipe for the kit lane: an "Outstanding deltas carried forward"
section in `run_upgrade`'s report writer (src/engine/upgrade.py) that
re-emits any prior-report template-delta section whose target doc is
still classified diverged AND whose delta hunk still fails to apply
cleanly — test target: a two-upgrade fixture asserting the v(N-1)
delta survives into the v(N) report until applied.

## ⟲ Previous-session review

The previous kit-lane session here (PR #71, v1.14.0) was clean: exact
recipe, evidence block, and its report correctly flagged
control/README.md diverged. What it could have done better: it left no
breadcrumb outside the report itself that the diverged delta was
unapplied, so this session only caught the carry-over by re-opening
PR #71's report per the playbook's preflight rule — the report-file
overwrite class above is the concrete system improvement that would
retire that manual preflight step.
