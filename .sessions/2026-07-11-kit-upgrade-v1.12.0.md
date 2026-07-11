# 2026-07-11 — kit upgrade: substrate-kit v1.11.0 → v1.12.0 (distribution wave)

> **Status:** `complete`

- **📊 Model:** Claude Fable 5 (`fable-5`) · distribution worker · kit upgrade v1.11.0 → v1.12.0 (distribution wave)
- **📊 Time:** started Sat Jul 11 17:30 UTC 2026 · closed Sat Jul 11 17:50 UTC 2026

## Scope

Kit-upgrade ONLY (hard scope per wave directive Q-0261.3): replace the vendored
`bootstrap.py` with the pinned v1.12.0 release asset (sha256
`77c00b811429e1b526ccc7e0dcf597435c11048e16a67edba6050f516ad5e1f8`, tag
`v1.12.0` @ commit b310aba, release run 29160292286), regenerate kit-owned
artifacts, verify `python3 bootstrap.py check --strict` green at close. No
domain work; no control/inbox.md or control/status.md writes — the heartbeat
`kit:` line bump is **lane-owed** to the resident session lane, not done here.

## What shipped

- **PR #49** — substrate-kit **v1.11.0 → v1.12.0** via the pinned release
  asset (689,586 bytes, sha256 verified locally before running AND
  self-verified by the upgrade engine against the adjacent `release.json`;
  both staged inputs self-cleaned).
  - `bootstrap.py` KIT_VERSION 1.11.0 → 1.12.0; `.substrate/state.json` +
    `substrate.config.json` `kit_version: 1.12.0`; staged `.substrate/`
    artifacts regenerated.
- **Carve-out scanner three-way compare (kit #210) — FIRST LIVE-GATE
  EXERCISE, and it fixed the v1.11.0 false-positive class.** Scan line from
  `.substrate/upgrade-report.md`, verbatim:
  - `- carve-out scan: .github/workflows/substrate-gate.yml — ran, 0 found`
  - Engine log, verbatim: `upgrade: carve-out scan:
    .github/workflows/substrate-gate.yml — ran, 0 found` followed by
    `upgrade: kept: .github/workflows/substrate-gate.yml (kit-owned, already
    current)`.
  - The live gate is **byte-identical to the new v1.12.0 staged template**
    (`diff` empty; `git diff` on the live gate empty — the v1.12.0 gate
    template is unchanged from v1.11.0's regen). Zero flags, **NO new
    pre-regen bank, no phantom carve-outs** — contrast the v1.11.0 upgrade
    (PR #44), where the old two-way scanner flagged the kit's own
    checkout/setup-python pin bumps as 2 "host-added steps" and banked the
    phantom `substrate-gate.pre-regen-4f50eb4d.yml`. That historical bank is
    untouched (hash-identical before/after), kept for audit.
- **Backup:** exactly ONE new dist archive —
  `.substrate/backup/bootstrap-1.11.0.py`, sha256
  `c339bd6a2eb3a139dd0106d5fd3873eb2d067f79723fccb5781d4e72a74a8d29`,
  byte-identical to the pre-upgrade dist (= the v1.11.0 release asset). All 7
  pre-existing `bootstrap-*.py` banks + the pre-regen yml `sha256sum -c` OK
  before/after; `backup/last-upgrade.json` + `backup/state.json` rewritten by
  the engine as designed.
- **Planted docs:** 2 classed template-improved + consumer-untouched
  (`CONSTITUTION.md`, `docs/AGENT_ORIENTATION.md`) → applied via
  `upgrade --apply-docs` (v1.12.0 boot-set trim: condensed program-law
  register cite; orientation drops its duplicate start-list + verify block).
  5 consumer-edited docs: "template unchanged — consumer-owned, nothing to
  apply"; 14 unchanged. Staged `.substrate/claude/CLAUDE.md` regen (13-line
  three-surface boot-set diff). The report's Carve-out scan section survived
  the `--apply-docs` rewrite (kit #176 fix, confirmed third time this wave).
- **Verification:** `python3 bootstrap.py check --strict` exit 0 with this
  card complete (HOLD-by-design while in-progress); host-side
  `python3 tools/check-cave.py` green (rows 0..20405, 0 breaks); ROM build
  left to CI ("ROM builds" required check; toolchain not local). Repo-wide
  grep: no `1.11.0` exact-pins in tests/config outside `.substrate/backup/`,
  historical `.sessions/`, and the lane-owed `control/status.md` heartbeat.

## 💡 Session idea

The v1.12.0 `AGENT_ORIENTATION.md` template replaces its boot-list and
verify block with pointers to `.claude/CLAUDE.md` § "Orientation — read
first" / § "Verifying a change" — but on repos like this one with **no live
`.claude/CLAUDE.md`** (staged `.substrate/claude/CLAUDE.md` only), both
pointers dangle: the named section exists only in the staged copy, and the
repo-specific verify line ("ROM builds" <60s compile check, conventions rule
16) now lives nowhere live except `.substrate/claude/CLAUDE.md:44`. Kit
idea: the template (or `render --live`) should resolve the pointer target —
point at the staged path when no live working agreement is wired, or treat
"live CLAUDE.md absent" as a render-time warning — so the one-home doctrine
never points at a home that isn't there.

## ⟲ Previous-session review

The v1.11.0 upgrade session (PR #44) hit the two-way scanner's pin-bump
false positive, hand-proved kit provenance (bank vs old staged template),
and filed the exact fix as its 💡 idea — the three-way compare that shipped
as kit #210 and cleared this session's scan (`ran, 0 found`, no bank). That
is the idea loop working end-to-end in one wave: friction → idea → kit fix →
verified live on the same repo that hit it. What it missed: it committed the
phantom bank "for audit" without leaving a disposition rule, so
`substrate-gate.pre-regen-4f50eb4d.yml` now sits in `.substrate/backup/` as
proven-phantom clutter with no expiry. Improvement: this card names the
disposition owner — the resident lane may groom (delete) the phantom bank
whenever it next touches `.substrate/`, citing this card + the #44 card as
the audit trail; kit-upgrade workers keep hands off per hard scope.

## Next session should know

- Kit at v1.12.0; heartbeat `kit:` line in `control/status.md` still says
  v1.11.0 — **lane-owed bump** (same debt shape session 11 paid for v1.11.0).
- `docs/AGENT_ORIENTATION.md` now points boot-set/verify at a live
  `.claude/CLAUDE.md` that doesn't exist in this repo (see 💡 above) — the
  verify command's live home is `.substrate/claude/CLAUDE.md:44` until the
  claude dir is wired or the kit fixes the pointer.
