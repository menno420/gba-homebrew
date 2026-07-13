# control/outbox.md — LANE-WRITTEN, append-only (lane → manager)

> One writer: the game-lab lane (Track B). Entries append at the bottom and
> are never edited after landing. The manager sweeps this file; anything
> flagged ⚑ needs manager routing or owner clicks.

---

## ⚑ 2026-07-13T04:46:55Z — ORDER 005 night-run MORNING tally (game-lab, Track B)

All numbers verified against live GitHub 04:42–04:46Z (Q-0120); PR heads cited.

### 1) Numbers

- **gba-homebrew: 6 merged** (owner hand-sweep 01:43–01:45Z): #75 Gloamline slice 10 · #77 ORDER 005 scribe · #78 Drift Garden PWA foundation · #79 Undertow · #80 Gloamline slice 11 · #81 Tiltstone.
- **gba-homebrew: 5 parked OPEN + green** (all three workflows success at head): #82 Brineward slices 6+7 (`20f4bfa`; carries the previously unpublished slice 6 WIND) · #83 Deepcast (`55e0a3b`) · #84 Drift Garden foundation→game (`35dc162`) · #85 release packaging (`d8f1049`) · #86 Cindervault, breadth game #6 (`76884fb`, green 04:28Z).
- **pokemon-mod-lab: 3 parked OPEN + green**: #57 ORDER 006 .gitignore (`52b408c`, independently review-approved) · #58 ORDER 007 scribe (`2ae4c40`) · #59 review-queue row #17 code-trace closure (`2d53bcf`). #60 closed-retracted, not merged.
- **New games with concept + prototype: 5** — Undertow (web), Tiltstone (web), Drift Garden (mobile PWA), Deepcast (GBA), Cindervault (GBA).
- **Sellables routed:** PR #85's four versioned zips (`web-undertow-v1.0`, `web-tiltstone-v1.0`, `drift-garden-pwa-v0.1`, `web-arcade-v1.0`) + `docs/RELEASES.md` (sha256-pinned) + `docs/RELEASE-HOWTO.md`.
- **Honest sellability guesses:** all five — free-arcade / near-zero paid.
- **Venture Lab marker:** Drift Garden foundation→game pipeline + provable-daily-seed hook (low confidence; flagged, not pitched).

### 2) HOSTING NOTE → Websites lane (via manager)

`dist/web/` (exists on main once #85 merges) is a zero-build static arcade
directory — landing page + `undertow/` + `tiltstone/` + `drift-garden/`,
entry point `index.html`. Any static host serves it as-is.

### 3) OWNER-ACTION asks (paste-ready, six-field)

**[a] Required status checks (unblocks the auto-merge enabler)**
- WHAT: add "ROM builds" + "substrate-gate" as required status checks on `main`.
- WHERE: github.com → menno420/gba-homebrew → Settings → Branches → branch protection rule for `main` → "Require status checks to pass".
- HOW: edit (or create) the rule, search-select the two check names, save.
- WHY: the auto-merge-enabler refuses to arm while zero contexts are required. Verbatim warning (auto-merge-enabler run 29222310196, job 86729758157, PR #85 head `d8f1049`): "the base branch requires no status-check CONTEXTS — arming would merge instantly. Refusing to arm; make 'ROM builds' a required check first (conventions.md rule 16)." All three probes in that run (rulesets, classic protection, branches endpoint) returned 0 contexts.
- UNBLOCKS: enabler arms on future green PRs — no more hand-sweeps for routine slices.
- VERIFY: next PR's auto-merge-enabler run logs show a nonzero required-contexts count and the arm step no longer skips.
- RISK: ✅ reversible (uncheck to undo).

**[b] Morning sweep clicks**
- WHAT: merge gba-homebrew #82, #83, #84, #85 (+ #86 if still green) and pokemon-mod-lab #57, #58, #59.
- WHERE: each PR page → "Squash and merge".
- HOW: click in any order; #85 packaged from main@`d87f9ad` only, so merging it before/after the game PRs is safe either way (repackage rides the next packaging slice).
- WHY: all are parked READY + green per tonight's rule 2 (open PRs stay open, owner sweeps in the morning); heads cited in section 1.
- UNBLOCKS: next slices branch from main instead of stacked open heads; dist/web goes live for the Websites hosting note.
- VERIFY: PR list shows them merged; `dist/web/index.html` exists at main HEAD.
- RISK: ✅ reversible (all green + cited; revert-PR undoes any single one).

**[c] Stale branch deletes (403-walled to agents)**
- WHAT: delete branches — pokemon-mod-lab: `track-a/session-019`, `track-a/session-024`, `claude/eloquent-newton-qaf1ii`, `claude/fm-r27-wake-repair`; gba-homebrew: `claude/brineward-wind` (only after #82 merges — #82 carries its slice 6).
- WHERE: each repo → Branches page.
- HOW: trash-can icon per branch.
- WHY: heads of closed/superseded PRs; agent delete attempts 403 (recorded wall).
- UNBLOCKS: clean branch list; no accidental re-basing on dead heads.
- VERIFY: Branches page no longer lists them.
- RISK: ✅ reversible (GitHub restores deleted branches from the PR page).
