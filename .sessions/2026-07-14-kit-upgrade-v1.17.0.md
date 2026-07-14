# Session — substrate-kit upgrade v1.16.0 → v1.17.0

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/kit-upgrade-v1.17.0`, PR #138)
- mission: upgrade the vendored substrate-kit from v1.16.0 to v1.17.0
  (distribution wave lane, Q-0261.3 scope — kit files only, no
  control/ edits, no domain work, host workflows restored byte-identical
  where regenerated).
- **📊 Model:** fable-5 · medium · mechanical refactor — kit upgrade wave

## What shipped

- Asset three-way verified before use: sha256
  `0d08b8aa9efc30178cf8e8befcfa28dd2b65e02106cc9ba6d520133017955521`,
  995,446 bytes (downloaded asset == release.json == the release's
  bootstrap.py.sha256 asset == coordinator-verified release fact).
- Two-command flow: `python3 bootstrap.py.new upgrade` then
  `python3 bootstrap.py upgrade --apply-docs`. The docs pass printed
  verbatim: `upgrade: apply-docs: no template-improved docs to apply —
  every planted doc is already current or consumer-owned.` — v1.17.0
  changed no doc templates (report: consumer-edited: 5 · unchanged: 19),
  so no `## Applied (--apply-docs)` section exists this release; that is
  the correct outcome, not a skipped pass.
- Banked exactly one new archive `.substrate/backup/bootstrap-1.16.0.py`
  (sha256 `bba34e21…9170` = the v1.16.0 dist sha); all pre-existing
  banks byte-identical.
- Host-workflow rail honored (same class as the v1.16.0 wave): the
  upgrade regenerated the live `auto-merge-enabler.yml`, dropping the
  host "Skip arming while the PR's own in-diff session card is
  in-progress" step; restored byte-identical to origin/main (sha256
  `a26a45a8…b455` before == after). rom-builds.yml, headless-boot.yml,
  substrate-gate.yml untouched (gate carve-out scan: ran, 0 found —
  kit-owned, already current). All four live workflow files verified
  byte-identical to pre-upgrade origin/main.
- v1.17.0's one payload — the scheduled branch-sweep workflow — arrived
  STAGED-ONLY at `.substrate/ci/branch-sweep.yml` and was deliberately
  NOT installed live (wiring it is an `adopt --wire-enforcement` lane
  decision, not wave scope).
- `check --strict` at close: green except this card's own designed
  born-red hold pre-flip; reading-path stayed rendered (the v1.16.0
  slot-fill did not recur, as expected).
- Lane-owed (reported, untouched, carried forward from the v1.16.0
  card): heartbeat `kit:` bump in control/status.md; the still-unapplied
  v1.14.0 control/README.md delta (recover from PR #71's report in git
  history); reconciling the live enabler with the kit enabler template
  while preserving the host skip-arming step; deciding whether to wire
  the staged branch-sweep workflow live.

## 💡 Session idea

The branch-sweep workflow template lands staged-only, but THIS repo is
the fleet's canonical branch-litter case (merged claude/* heads never
auto-delete — see the stacked-PR landing memory; the stale
`claude/kit-upgrade-v1.16.0` branch was still on origin during this
wave). The lane should fast-track wiring `.substrate/ci/branch-sweep.yml`
live here first, run one `workflow_dispatch` with `dry_run` to audit the
candidate list, and report the deletions in its session card — this repo
gives the kit its best before/after evidence for the sweep's value.

## ⟲ Previous-session review

The v1.16.0 wave session (PR #137) left an exemplary card: its explicit
sha256-before/after note on the enabler restore and its lane-owed list
made this session's two riskiest steps (enabler byte-restore, delta
carry-forward) mechanical lookups instead of re-derivations. One
improvement it suggests: it recorded the pre-regen enabler bank name
(`auto-merge-enabler.pre-regen-a26a45a8.yml`) but not that the bank is
IDEMPOTENT across waves (same sha8 → no new file, git-silent) — this
session confirmed that empirically; noted here so the next wave doesn't
hunt for a "missing" new bank.
