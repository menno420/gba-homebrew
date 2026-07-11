# 2026-07-11 — kit upgrade: substrate-kit v1.9.0 → v1.10.0 (distribution wave)

> **Status:** `complete`

- **📊 Model:** Claude Fable 5 (`fable-5`) · distribution worker · kit upgrade v1.9.0 → v1.10.0 (distribution wave)
- **📊 Time:** started Sat Jul 11 07:21 UTC 2026 · closed Sat Jul 11 07:26 UTC 2026

## Scope

Kit-upgrade ONLY (hard scope per the wave directive): replace the vendored
`bootstrap.py` with the pinned v1.10.0 release asset via
`upgrade --apply-docs` (single invocation — this wave the doc re-renders
carried since v1.7.0 land per directive), regenerate kit-owned artifacts,
verify `python3 bootstrap.py check --strict` green at close. **No domain
work; no control/inbox.md or control/status.md writes** (the lane's own
next heartbeat updates the `kit:` line — the wave seat does not write the
heartbeat).

## What shipped

- **PR #37** — substrate-kit **v1.9.0 → v1.10.0** via the pinned release
  asset (tag `v1.10.0` @ commit 1b5db16, release run 29142780212; sha256
  `ba69fc5cf21619cc85e4c733ebe3d9eda8803e678f810fcc39b94d60c2f3b5a4` —
  verified locally against the wave pin AND self-verified by the upgrade
  engine against the adjacent `release.json`; staged inputs self-cleaned).
  - `bootstrap.py` v1.9.0 → v1.10.0; `substrate.config.json` +
    `.substrate/state.json` `kit_version: 1.10.0`; staged `.substrate/`
    artifacts regenerated.
  - **Backup:** exactly ONE new archive —
    `.substrate/backup/bootstrap-1.9.0.py` (sha256 `55181082…`, byte-equal
    to the pre-upgrade dist); all four pre-existing banks byte-identical
    before/after (hash-verified).
  - **`--apply-docs` (single invocation, riding the upgrade):** the 14
    template-improved doc re-renders carried since v1.7.0 applied
    (CONSTITUTION, decisions, architecture, ownership, runtime_contracts,
    repo-navigation-map, helper-policy, collaboration-model,
    ai-project-workflow, owner-profile, AGENT_ORIENTATION, question-router,
    CAPABILITIES, .session-journal). The `## Carve-out scan` section
    SURVIVED --apply-docs in `.substrate/upgrade-report.md` (the v1.9.0
    rewrite bug is fixed in v1.10.0; scan: ran, 0 found — no pre-regen
    bank, correct).
  - **Live gate regen (kit-owned):** the added-card lane now routes an
    in-progress ADDED card to the born-red **HOLD** (`session-card-hold`
    finding, appended after the allowlist pass — never allowlistable; the
    superbot-games #40 card-only-loophole fix), and the locked-door branch
    additionally runs `--simulate-added-card` so the added-card verdict
    stays observable on gate-touching PRs. Gate is **LIVE** here.
  - **First exercise, live:** first commit (card-only, d3255d6) ran GREEN
    under the OLD v1.9.0 gate's advisory lane (run 29144377427); the regen
    commit (0d38516) engaged the locked-door HOLD under the NEW gate —
    run 29144443174 printed the locked-door line, the new
    `simulate-added-card … would HOLD (born-red …)` advisory, and the
    `HOLD (by design)` banner + `##[notice]` annotation. Hold semantics
    tightened, never loosened, inside this PR — as designed.
  - **Model-attribution doctrine retro-append:** NO-OP branch —
    `.sessions/README.md` byte-identical before/after (sha256 `cae82297…`
    unchanged); the v1.9.0 session regenerated it from the compose that
    already embeds the doctrine, and the v1.10.0 detection phrase matches.
  - **Sibling-card sweep (mtime-lottery guard):** all 12 sibling cards in
    `.sessions/` grade exit 0 individually via
    `check --strict --session-log <card>` — zero backfills needed.
  - `python3 tools/check-cave.py` green locally (host-side passability
    proof from the ROM-builds lane); the ROM build itself needs the
    devkitARM toolchain — covered by the required `ROM builds` CI check.
  - `python3 bootstrap.py check --strict` green at close (the only
    mid-session finding was this card's own born-red badge, by design);
    `check --simulate-added-card <this card>` exercised locally: advisory
    "would HOLD (born-red …)", exit 0.

## Follow-ups for the lane (decide-and-flag)

- Update the `kit:` line in `control/status.md` to `v1.10.0 · check:
  green` on the next heartbeat (wave seat does not write the heartbeat;
  self-report has been chronic-stale for 4+ regens).
- Legacy root `claims/` still draws the `claims-legacy-location` nudge —
  migrate/fold into `control/claims/` (carried from the v1.8.0/v1.9.0
  cards; third consecutive wave re-listing, see review below).
- Staged auto-merge enabler remains staged-only
  (`.substrate/ci/auto-merge-enabler.yml`); wiring it live is the lane's
  opt-in (`adopt --wire-enforcement`).

## ⚑ Flags

- ⚑ Self-initiated: none — everything this session was directed wave work
  (the `--apply-docs` doc re-renders were named in the wave directive, so
  the v1.9.0 card's apply-docs follow-up closes as directed, not
  self-initiated).

## 💡 Session idea

The v1.9.0 card's "Follow-ups for the lane" list has now been re-listed
verbatim across three consecutive kit-upgrade cards with zero lane pickup
(status heartbeat, legacy `claims/`). Kit-sized fix: teach `bootstrap.py
check` a cheap advisory that greps the NEWEST complete kit-upgrade card
for a "Follow-ups for the lane" section and, when the SAME bullet
(normalized) also appears in the previous kit-upgrade card, emits a
`followup-carried` advisory naming the bullet and both cards — turning
silent follow-up rot into a per-session nudge the lane actually sees,
without making anything red.

## ⟲ Previous-session review

Reviewed the v1.9.0 kit-upgrade session (PR #36, card
`2026-07-11-kit-upgrade-v1.9.0.md`): exemplary evidence discipline — its
deliberate "Not taken: --apply-docs" call correctly deferred a known-buggy
path (the carve-out rewrite) instead of shipping it, and this wave landed
the same docs safely once the kit fixed the bug. Its 💡 session idea
(`--simulate-added-card`) shipped in kit v1.10.0 *and was exercised live
by this very PR's gate run* — a clean idea-to-shipped loop in under a day,
which is the workflow working exactly as designed. Improvement it
surfaces: the loop worked because a kit-lane agent happened to read that
card; ideas recorded on adopter session cards have no routed path into the
kit's own `docs/ideas/` intake — the 💡 above proposes the complementary
guard for follow-ups, and idea-routing across repos remains the wider gap.
