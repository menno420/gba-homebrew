# 2026-07-11 — kit upgrade: substrate-kit v1.8.0 → v1.9.0 (distribution wave)

> **Status:** `complete`

- **📊 Model:** Claude Fable 5 (`fable-5`) · distribution worker · kit upgrade v1.8.0 → v1.9.0 (distribution wave)
- **📊 Time:** started Fri Jul 11 05:05 UTC 2026 · closed Fri Jul 11 05:20 UTC 2026

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.9.0 release asset, regenerate kit-owned
artifacts, take the v1.9.0 plants, verify `python3 bootstrap.py check
--strict` green. **No domain work; no control/inbox.md or
control/status.md writes** (the lane's own next heartbeat updates the
`kit:` line — the wave seat does not write the heartbeat).

## What shipped

- **PR #36** — substrate-kit **v1.8.0 → v1.9.0** via the pinned release
  asset (tag `v1.9.0`, commit 2a779b5, release run 29139623697; 645,448
  bytes, sha256
  `55181082c796657c8e5e14750d248cea2df9e69a9aa896dd8a8c7f1adfb9cc90` —
  verified locally against the wave pin AND self-verified by the upgrade
  engine against the adjacent `release.json`; staged inputs self-cleaned).
  - `bootstrap.py` v1.8.0 → v1.9.0; `substrate.config.json`
    `kit_version: 1.9.0`; staged `.substrate/` artifacts regenerated.
  - **Backup:** exactly ONE new archive —
    `.substrate/backup/bootstrap-1.8.0.py` (sha256 `28c5dcb6…`, byte-equal
    to the pre-upgrade dist); all three pre-existing banks byte-identical
    before/after (hash-verified).
  - **Plants (v1.9.0):** `.ignore` + `.gitattributes` search-hygiene
    entries — both files absent pre-upgrade → fresh plants, 2 entries each
    under the append-only provenance marker.
  - **SessionStart handoff-push** verified live: `python3 bootstrap.py
    hook sessionstart` prints the `## Handoff — the previous session's
    trail (pushed; read before re-deriving)` section with the newest card
    path + status + read-first line.
  - **Model-attribution doctrine:** live `.sessions/README.md` was stale
    template output (sole diff vs the v1.9.0 compose was the template's
    own updated paragraph 1; zero host-only content) → regenerated from
    the v1.9.0 compose (`_adopt_sessions_readme`); doctrine + exact
    byte-form markers now present.
  - **Live gate regen (kit-owned):** `.github/workflows/substrate-gate.yml`
    regenerated; functional delta is the v1.9.0 `--added-card`
    grammar-lint lane on the advisory sentinel (the venture-lab #15
    class). **Carve-out scan: ran, 0 found**; explicit `## Carve-out scan`
    section in `.substrate/upgrade-report.md`; no pre-regen bank — correct,
    banking is conditional on detected carve-outs.
  - **First exercise, live gate:** the #156 mid-PR locked-door invariant
    fired on the payload commit (this PR adds a card AND touches the gate
    file) — gate run 29140851655 printed the locked-door line AND the new
    v1.9.0 `HOLD (by design)` notice verbatim, plus the Actions
    `##[notice]` annotation naming the designed hold. The card-only first
    commit (pre-regen gate) ran green under the old advisory sentinel
    (run 29140769538), so hold semantics tightened, never loosened,
    inside this PR — as designed.
  - `python3 bootstrap.py check --strict` green at close (the only
    mid-session finding was this card's own born-red badge, by design).
- **Not taken, deliberately:** `--apply-docs` (14 docs classed
  template-improved; applying them is a lane decision, not wave scope —
  and the post-hoc `upgrade --apply-docs` path has a known v1.9.0 bug:
  it rewrites `.substrate/upgrade-report.md` WITHOUT the carve-out
  section; hand-restore that section if the lane runs it).

## Follow-ups for the lane (decide-and-flag)

- Update the `kit:` line in `control/status.md` to `v1.9.0 · check:
  green` on the next heartbeat (wave seat does not write the heartbeat;
  self-report has been chronic at v1.6.0 for 3+ regens).
- `upgrade --apply-docs` for the 14 template-improved doc re-renders
  (carried since v1.7.0) — mind the carve-out-section rewrite bug above.
- Legacy root `claims/` still draws the `claims-legacy-location` nudge —
  migrate/fold into `control/claims/` (carried from the v1.8.0 card).
- Staged auto-merge enabler remains staged-only
  (`.substrate/ci/auto-merge-enabler.yml`); wiring it live is the lane's
  opt-in (`adopt --wire-enforcement`).

## ⚑ Flags

- ⚑ Self-initiated: `.sessions/README.md` regenerated from the v1.9.0
  compose (kit-owned adoption path; live copy verified stale-template with
  zero host-only content before overwrite). Everything else directed wave
  work.

## 💡 Session idea

The v1.9.0 advisory-sentinel `--added-card` grammar lint could not be
exercised on this PR — any PR that regenerates the gate file routes its
added card through the locked door instead, so the very wave that ships
the lint never observes it. Cheap kit improvement: `bootstrap.py check`
could grow a `--simulate-added-card <path>` local mode that runs exactly
the advisory-lane grammar lint, letting a distribution worker capture the
new lane's verdict locally in the same session that ships it instead of
waiting for the next organic born-red PR.

## ⟲ Previous-session review

Reviewed the v1.8.0 kit-upgrade session (PR #28, card
`2026-07-11-kit-upgrade-v1.8.0.md`): thorough — its careful
anchored-vs-unanchored judgment on the diverged `control/README.md` merge
left nothing owed this wave (the doc now classes consumer-edited /
template-unchanged), and its session idea (classify delta hunks as
anchored/unanchored) remains a good kit candidate. Improvement it
surfaces, now confirmed twice: its "Follow-ups for the lane" list
(status heartbeat, apply-docs, legacy claims/) is carried forward
verbatim for the second consecutive wave with no lane pickup — the
follow-ups section needs a routing target (e.g. an inbox row written by
the lane itself, or a check advisory when a kit-upgrade card's follow-ups
survive a full wave), not just re-listing. This card re-lists them once
more; the session idea above attacks a different gap.
