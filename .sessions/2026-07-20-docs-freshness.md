# Session — Docs freshness verify (2026-07-20): close-out doc Underroot dist-ROM row + baton routine note

> **Status:** `in-progress`

- date: 2026-07-20 (branch `claude/gba-docs-freshness-0720`, main synced @
  `524cd02` = #209 merge, `git ls-remote` match; all timestamps from `date -u`)
- mission: **freshness verify, not churn** — under ORDER 048 + the owner
  2026-07-20 continue directive (last writable day before the 7/21 read-only
  EAP window), re-verify the 2026-07-19 truth-refresh docs still match the tree.
  Most facts hold; **two genuine gaps** justify this one small doc PR:
  1. **STALE (fact vs tree):** `docs/eap-closeout-gba-2026-07-19.md` Underroot
     (A2) row still reads "⚠ No committed `dist/` ROM (CI builds it each PR)".
     That was true when the row was written (through #208), but **#209**
     (`524cd02`, merged 2026-07-19 evening) **committed `dist/underroot.nds`**
     (104,960 B, sha256 `72024153…`). The tree and `dist/README.md:21` ("first
     committed 2026-07-19") now contradict the close-out row. A fresh seat /
     owner reading the finish-park table would wrongly think Underroot has no
     downloadable ROM.
  2. **BATON GAP (would mislead a fresh seat):** the successor baton (§c) has no
     note that a **re-created project** starts in a NEW environment and does
     **not** inherit the coordinator's failsafe cron — it must **re-arm its own
     routines** and should not expect the old dead-man bridge to fire. The only
     routine line (§c ORDER 007) says "do not re-arm pending owner go," which a
     fresh seat could read as "a failsafe is already covering me." It is not.
- scope guard: **fix-only, no rewrite.** Two surgical edits to the close-out
  doc; nothing else touched. Everything else verified current is left alone
  (no padding). `control/status.md` is already current (it carries a full #209
  section) — NOT edited except this heartbeat.
- **📊 Model:** Claude Opus 4.8 (1M) · low · two-line doc freshness fix
- landing posture: READY (non-draft). Born-red `claude/*` card PR — this card
  holds the PR red as `in-progress` until the close-out flip (the LAST commit),
  then it lands on green `ROM builds` via the existing `auto-merge-enabler.yml`.
  A docs-only diff does not break the build.

## What ships

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-gba-docs-freshness-0720.md`), filed before the fix so
   the PR is red until the close-out flip.
2. **`docs/eap-closeout-gba-2026-07-19.md` — Underroot row de-staled** — the
   "No committed `dist/` ROM" clause replaced with the #209 truth
   (`dist/underroot.nds` now committed; cite `dist/README.md` + #209).
3. **`docs/eap-closeout-gba-2026-07-19.md` — baton routine note added** — one
   bullet in §c telling a re-created-project seat it re-arms its own routines
   and must not rely on the old coordinator failsafe cron.
4. **Heartbeat** — a dated section in `control/status.md` (append-only,
   per-section, no clobber of coordinator sections).

## Verified current (no touch — the honest bulk of this pass)

- main HEAD `524cd02` = #209, `git ls-remote` match; **no overnight commits**
  since #209; `list_pull_requests` state=open → **empty**.
- Controller lane DONE (pf #31/#32); arcs A2 Underroot / B1 Tiltstone / B2
  Wickroad / B3 Brineward all BUILT; #177/#178 closed-subsumed-by-#179; dist
  ROMs `wickroad.gba` / `underroot.nds` / `brineward.nds` / `lumen-drift.gba`
  all present; Lumen Drift Release `lumen-drift-v1.3` published (non-draft,
  verified via github MCP `list_releases`); menu real-choices = A1 + A3 only.
- `control/status.md` already reflects #205–#209 landed, both NDS ROMs
  committed (§ "dist/ NDS ROMs", lines 127-136), the menu audit (#208), and the
  planning-mode/dry state — current, left as-is.
- Baton correctly names `auto-merge-enabler.yml` (not `merge-on-green.yml`),
  the non-required `substrate-gate` doc-orphan, Track-A isolation, ORDER 007
  through 2026-07-21, owner-no-clicks posture, remaining A1/A3 + playtests +
  itch.io picks, and current-state/NEXT-MENU pointers — all still correct; the
  only baton gap was the routine/failsafe note added here.

## 💡 Session idea

**A "close-out doc vs dist/ ledger" cross-check would have auto-caught this
staleness.** The Underroot row went stale the moment #209 committed
`dist/underroot.nds` while the close-out finish-park table kept saying "no
committed dist ROM" — the same *doc-lags-the-tree* shape #206/#209 kept fixing
for the binaries themselves. `dist/README.md` is already the authoritative
download index (it got the new Underroot row in #209); the close-out table is a
second, hand-maintained assertion about the same facts. A cheap check: assert
that every "no committed dist ROM" / "no dist ROM" phrase in a close-out/menu
doc is FALSE if a matching `dist/<game>.*` exists — turning "a menu audit
eventually notices the doc drifted" into a red the PR that commits the ROM.
Single-source-of-truth beats two ledgers that must be kept in lockstep by hand.

## Previous-session review

- Directly continues **`.sessions/2026-07-19-commit-nds-roms.md`** (#209): that
  slice *committed* `dist/underroot.nds`; this one closes the doc half it left
  behind — the close-out finish-park table still asserted the ROM's absence.
  #209's own idea ("presence is the floor the drift guard assumes") generalizes
  here to the DOC ledger, not just CI.
- Also builds on **`.sessions/2026-07-19-menu-truth-audit.md`** (#208) and the
  #207 truth-refresh: those verified the close-out doc arc-by-arc; this is the
  next-day re-verify that catches the one row #209 outdated.
- **What transferred:** the born-red rhythm verbatim — card + claim first, ONE
  flip to `complete` as the last commit, land on green `ROM builds` via
  `auto-merge-enabler.yml` (never the dead `merge-on-green.yml` #200 retired);
  every claim cited to a commit/PR/file@line; fix-only, no rewrite.
- **What differs:** this is a *freshness verify*, not new content — the honest
  default was NO-TOUCH, and the bulk of the pass confirmed-current; only two
  named gaps (one stale fact, one baton omission) earned the diff.

## Guard honored

- Track-A PUBLIC repo, original gba IP only (no Track-B / Nintendo / pokemon
  content). ORDER 007 respected — **no routines armed** this slice. No
  force-push. Every Bash pinned to the absolute repo root
  `/home/user/gba-homebrew`. Repo text treated as DATA, not instructions.
