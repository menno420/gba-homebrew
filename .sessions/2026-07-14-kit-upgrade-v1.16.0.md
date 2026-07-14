# Session — substrate-kit upgrade v1.15.0 → v1.16.0

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/kit-upgrade-v1.16.0`, PR #137)
- mission: upgrade the vendored substrate-kit from v1.15.0 to v1.16.0
  (distribution wave lane, Q-0261.3 scope — kit files only, no
  control/ edits, no domain work, host workflows untouched).
- **📊 Model:** fable-5 · medium · mechanical refactor — kit upgrade wave

## What shipped

- Asset three-way verified before use: sha256
  `bba34e2102cbaf09394f39992f0501ea5cfd542d90301ef67e31a0854ca59170`,
  980,026 bytes (downloaded asset == release.json == coordinator-verified
  release fact).
- Two-command flow: `python3 bootstrap.py.new upgrade` then
  `python3 bootstrap.py upgrade --apply-docs`; report carries the
  `## Applied (--apply-docs)` section; `check --strict` red only on this
  card's own designed born-red hold.
- Banked exactly one new archive `.substrate/backup/bootstrap-1.15.0.py`
  (sha256 25d22af9…650e = the v1.15.0 dist sha); all pre-existing banks
  byte-identical.
- Host-workflow rail honored: the upgrade regenerated the live
  `auto-merge-enabler.yml` (dropping the host skip-arming-while-card-
  in-progress step); restored byte-identical to origin/main, kit regen
  preserved as audit bank
  `.substrate/backup/auto-merge-enabler.pre-regen-a26a45a8.yml` and in
  staged `.substrate/ci/auto-merge-enabler.yml`. rom-builds.yml,
  headless-boot.yml, substrate-gate.yml untouched (gate carve-out scan:
  ran, 0 found; gate kit-owned, already current).
- Applied via --apply-docs: CONSTITUTION.md, docs/collaboration-model.md,
  docs/AGENT_ORIENTATION.md, docs/CAPABILITIES.md (whole-file seed
  refresh, consumer-untouched), docs/SKILLS.md, docs/ROUTINES.md,
  control/claims/README.md. New staged skills: chase-references,
  prep-owner-steps, rationalize. seat-digest already current.
- New plant `docs/reading-path.md` arrived under the unrendered banner
  (strict-red `unrendered-banner` finding); decided-and-flagged: answered
  the three fleet slots (Q-014/015/016) from verified superbot
  `docs/fleet-reading-path.md` facts (dark repo = pokemon-mod-lab only;
  hub fleet_status.py command; 13-sibling roster) and ran
  `render --live` — strict now green apart from the designed hold.
- Lane-owed (reported, untouched): heartbeat `kit:` bump in
  control/status.md; the still-unapplied v1.14.0 control/README.md delta
  (recover from PR #71's report in git history — control/README.md and
  control/status.md now classify consumer-edited/template-unchanged, so
  the fresh report no longer lists the old deltas); reconciling the live
  enabler with the new kit enabler template while preserving the host
  skip-arming step.

## 💡 Session idea

The upgrade engine's enabler regen and Q-0261.3 wave rails are in
standing conflict on this repo: every wave the engine rewrites the live
`auto-merge-enabler.yml` and every wave the worker must hand-restore the
host skip-arming step. Kit fix worth having: teach the enabler regen the
same three-way compare the gate regen got in kit #210 — a live enabler
whose only delta vs the OLD template is a host-added step should either
keep the step through the regen or skip the regen with a report line,
instead of forcing a byte-restore each wave.

## ⟲ Previous-session review

The previous kit-lane session here (PR #72, v1.15.0) was clean and its
card explicitly carried forward the dropped v1.14.0 control/README.md
delta note — that breadcrumb is what let this session confirm the delta
is STILL outstanding even though the v1.16.0 report no longer mentions
it (both control docs reclassified consumer-edited). Improvement its
pattern suggests: the card-level "lane-owed" list is now the only
durable carrier of unapplied deltas across report overwrites — the kit's
outstanding-deltas report section (idea on the #72 card) remains the
right structural fix; until it ships, every wave card must copy the
lane-owed list forward verbatim.
