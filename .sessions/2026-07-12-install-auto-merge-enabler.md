# Install auto-merge-enabler workflow (fleet-manager ORDER 029) — named card, no session-number claim

> **Status:** `complete`

- date: 2026-07-12 (branch `claude/install-auto-merge-enabler`, started
  2026-07-12T23:59:35Z, close-out written 2026-07-13T00:07Z per `date -u`)
- mission: install a PR-landing workflow
  (`.github/workflows/auto-merge-enabler.yml`) adapted from
  idea-engine@819a8d5, per the fleet owner's live directive (fleet-manager
  coordinator chat, 2026-07-12T23:00Z) recorded as fleet-manager inbox
  ORDER 029 — uniform landing workflows fleet-wide. Executed by a
  fleet-manager-coordinator worker session in this repo; docs/workflow only,
  no game code, no build change.
- 📊 Model: fable-5 (family-level self-report from this session's own
  harness/environment, per ORDER 003)

## What shipped

- `.github/workflows/auto-merge-enabler.yml` (PR #76): arms GitHub-native
  auto-merge (squash) on `claude/*` PRs at open/reopen/ready/synchronize.
  All reference guards kept: same-repo head, non-draft, `claude/` prefix
  allowlist (all 12 most-recent PR heads, #64–#75, are `claude/*`),
  `do-not-automerge` carve-out with 15s fresh label re-read, refuse-to-arm
  on zero required status-check contexts, in-progress-session-card SKIP
  (this repo's rule-10 born-red convention makes that guard load-bearing —
  substrate-gate is NOT in the required-check set, so without it an armed
  PR would merge on "ROM builds" green mid-session).
- Host adaptations flagged in the file header: allowlist, "ROM builds"
  wording, and a classic-branch-protection fallback in the rules-count
  guard (the rulesets endpoint under-counts on a classic-protection repo;
  errors still fail safe to refuse).
- Doctrine change flagged in the PR body: green PRs no longer park for
  owner merge clicks — per the owner's explicit standing permission
  (verbatim quoted in PR #76). Opt-out = `do-not-automerge` label.
- Verified: YAML parses, every `run:` block passes `bash -n`,
  `python3 bootstrap.py check --strict` green apart from the designed
  born-red hold + a pre-existing claims-legacy-location advisory.

## 💡 Session idea

The refuse-to-arm guard reads required contexts from
`rules/branches/<base>` (rulesets) and now falls back to classic branch
protection — but nothing in the repo records WHICH mechanism actually
protects `main` here (docs say "Settings → Rulesets/Branch protection",
ambiguous). Worth one line in `docs/CAPABILITIES.md` the first time a
workflow run prints the guard's context counts: which endpoint reported
"ROM builds", so future guards (and the owner's NDS-required-check ask in
control/status.md ⚑) target the right settings surface.

## Previous-session review

Session 36 (PR #73/heartbeat cycle 5) did the right thing re-reading
moving refs at landing time rather than draft time — its own 💡 lesson.
What it (and every session since #68) could not fix from inside the
convention: PRs #68/#69 sat parked "awaiting the owner's merge clicks"
despite conventions.md rule 3 declaring exactly that state a convention
violation — the written self-merge grant existed but had no always-on
mechanism. Concrete workflow improvement: this session's enabler IS that
mechanism (arming is now automatic at open, not a per-session manual step
a cautious session can skip). Watch item for the next real PR: confirm
the enabler run arms (log line "Auto-merge enabled for PR #N") and that
the merge lands on "ROM builds" green.
