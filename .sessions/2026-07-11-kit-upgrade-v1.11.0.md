# 2026-07-11 — kit upgrade: substrate-kit v1.10.1 → v1.11.0 (distribution wave)

> **Status:** `complete`

- **📊 Model:** Claude Fable 5 (`fable-5`) · distribution worker · kit upgrade v1.10.1 → v1.11.0 (distribution wave)
- **📊 Time:** started Sat Jul 11 13:16 UTC 2026 · closed Sat Jul 11 13:25 UTC 2026

## Scope

Kit-upgrade ONLY (hard scope per wave directive Q-0261.3): replace the vendored
`bootstrap.py` with the pinned v1.11.0 release asset (sha256
`c339bd6a2eb3a139dd0106d5fd3873eb2d067f79723fccb5781d4e72a74a8d29`, tag
`v1.11.0` @ commit 640f8a1, release run 29152928040), regenerate kit-owned
artifacts (live substrate-gate.yml included), verify
`python3 bootstrap.py check --strict` green at close. No domain work; no
control/inbox.md or control/status.md writes — the heartbeat `kit:` line bump
is **lane-owed** to the resident session lane, not done here.

## What shipped

- **PR #44** — substrate-kit **v1.10.1 → v1.11.0** via the pinned release
  asset, verified locally against the wave pin AND self-verified by the
  upgrade engine against the adjacent `release.json` (staged inputs
  self-cleaned).
  - `bootstrap.py` v1.10.1 → v1.11.0; `substrate.config.json` +
    `.substrate/state.json` `kit_version: 1.11.0`; staged `.substrate/`
    artifacts regenerated.
  - **HANDOFF composer (new in v1.11.0):** 28 `HANDOFF` references in the
    vendored dist; pointer constants at bootstrap.py:4885
    (`HANDOFF_POINTER_FILENAME = "HANDOFF.md"`), :4888 (marker), :4892
    (excerpt cap), :4894 (needle). Planted CLAUDE.md template
    (`.substrate/claude/CLAUDE.md:16`) carries the read-first line
    "`HANDOFF.md` at repo root (when present) — the previous session's
    trail". `HANDOFF.md` is boot-generated + untracked and NOT gitignored
    (`git check-ignore HANDOFF.md` exits 1); none exists locally and none
    was fabricated. No live root `CLAUDE.md` / `.claude/CLAUDE.md` in this
    repo — nothing host-owned to reconcile.
  - **Live gate regen (kit-owned):** actions bumped `checkout@v4→v5`
    (line 21) and `setup-python@v5→v6` (line 80) — the Node 20 deprecation
    fix; live gate byte-identical to staged `.substrate/ci/substrate-gate.yml`.
  - **Carve-out scan — false-positive class, evidence-verified:** the scan
    flagged the OLD gate's own `checkout@v4` / `setup-python@v5` pins as
    "host-added steps" (2 lines) + banked
    `.substrate/backup/substrate-gate.pre-regen-4f50eb4d.yml`. The
    pre-regen bank is **byte-identical to the OLD v1.10.1 kit-staged
    template** (`git show HEAD:.substrate/ci/substrate-gate.yml` diff
    empty), so the live gate had ZERO genuine host additions — nothing
    existed to carry and nothing was lost. Bank committed for audit.
  - **Backup:** exactly ONE new dist archive —
    `.substrate/backup/bootstrap-1.10.1.py` (sha256 `fbe83ce3…`, byte-equal
    to the pre-upgrade dist); all six pre-existing `bootstrap-*.py` banks
    hash-identical before/after.
  - **Guard-fires 10-min dedupe (new in v1.11.0):** two back-to-back
    `check --strict` runs — first appended 1 fire (47→48 lines, this card's
    designed born-red hold), second appended 0 (48→48). Dedupe works.
- **Verification:** `python3 bootstrap.py check --strict` green with this
  card complete; host-side `python3 tools/check-cave.py` green (rows
  0..20405, 0 breaks); ROM builds left to CI (toolchain not local).

## 💡 Session idea

The carve-out scanner compares live-gate steps against the NEW template, so
a template-side action-pin bump (`checkout@v4→v5`) makes every old kit pin
read as a "host-added step" — a guaranteed 2-false-positive wave on every
adopter with a live gate whenever the kit bumps its own action versions.
Kit idea: the scanner should diff live-gate steps against the OLD
(pre-upgrade) template first and only flag steps absent from BOTH templates
— old-template membership proves kit provenance, cheap to check since the
old dist is already banked adjacent in `.substrate/backup/`.

## ⟲ Previous-session review

The v1.10.1 upgrade session (PR #38) was clean: its card recorded backup
hashes and the first live exercise of the per-card locked door, which made
this session's before/after comparison mechanical. One improvement it
missed: it recorded "carve-out scan: ran, 0 found" as the expected shape
without noting that shape only holds when the template's OWN action pins
are unchanged — this session hit the pin-bump false-positive the moment the
kit bumped checkout/setup-python, and had to prove kit-provenance by hand
(pre-regen bank vs old staged template). The improvement is captured as
this session's 💡 idea + the evidence recipe above (diff the bank against
`git show <pre>:.substrate/ci/substrate-gate.yml`), so the next wave lands
it in minutes.
