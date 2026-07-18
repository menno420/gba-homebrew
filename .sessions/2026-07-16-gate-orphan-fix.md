# Session — gate-red orphan fix: restore read-path reachability lost in #151

> **Status:** `complete`

- date: 2026-07-16 (branch `claude/gate-orphan-fix`; started 01:28Z, `date -u`)
- mission: **turn `python3 bootstrap.py check --strict` green on main again.**
  The check has failed since #151 (squash `d38887c`): the coordinator-ender
  condensing of control/status.md (−297 lines) removed the only links into
  `docs/eap-closeout-walkthrough-2026-07-14.md` and
  `docs/concepts/session-1-concepts.md`, transitively orphaning three more
  docs — 5 `[reachable]` findings total. The check passed at `588aa4e`
  (pre-#151). Docs-only slice; DRAFT PR for owner review, no merge automation.
- **📊 Model:** Claude (Fable family) · medium · docs-only — gate-red orphan fix
- landing posture: PR opened as DRAFT deliberately; the owner reviews, marks
  ready, and lands it. No merge/approve/auto-merge calls from this session.

## What shipped

1. Born-red gate (commit `7474791`): this card in-progress; claim
   `control/claims/claude-gate-orphan-fix.md` (`6de22bd`).
2. **The fix** (`276343e`, docs/current-state.md only): a "Doc index"
   section linking the 5 orphaned docs. Mechanism chosen from the gate's
   actual roots: `check_reachable` in bootstrap.py seeds from
   `readpath_docs` (substrate.config.json: AGENT_ORIENTATION.md,
   current-state.md) plus every `docs/**/README.md`; current-state.md IS
   a root, so control/status.md needed no re-expansion — it stays
   condensed as #151 left it (this slice only appends a dated section,
   repo convention).
3. Claim retirement (`2250c50`): control/claims/claude-current-state-rows.md
   deleted — its work merged as PR #152 (`478bf16`, 2026-07-16T01:20:02Z);
   claims README rule: retire the claim once the work lands.
4. Dated worker section appended to control/status.md (`7acb609`).
5. Verify: `python3 bootstrap.py check --strict` on this branch — the 5
   `[reachable]` findings are gone; the only remaining red before the
   final card flip was the designed born-red hold on this card itself.
   (A pre-existing `[model-line-shape]` advisory on
   `.sessions/2026-07-14-wickroad.md` is never exit-affecting and is out
   of scope — follow-up candidate.)
6. Rails held: no writes committed under `.substrate/` (the check's
   guard-fire telemetry delta was left uncommitted in the working tree),
   docs-only diff, no dist/ or ROM rebuilds, no merge automation.

## 💡 Session idea

**The reachability gate treats control/status.md as a link source but the
repo treats it as condensable — those two facts are a standing time bomb.**
Any future ender condense can silently re-orphan docs. Guard recipe: teach
the condense ritual (or a preflight in `scripts/preflight.py`, which the
check already looks for and currently skips as missing) to run
`python3 bootstrap.py check --strict` on the condensed tree *before*
opening the ender PR; anchor: `check_reachable()` /
`_default_readpath_docs()` in bootstrap.py — links that must survive belong
in a `readpath_docs` root like docs/current-state.md, not in a file whose
convention is periodic shrinkage.

## Previous-session review

- `.sessions/2026-07-16-current-state-rows.md` (PR #152, `478bf16`) — its
  💡 idea ("condensation should move receipts, never delete them") is
  exactly the failure this session repaired, one layer down: #151's
  condense dropped not just an ORDER ack but the read-path links, and that
  session's own card flagged the ack loss without running the strict check
  that would have caught the orphans. Its row-level citation discipline
  (every fact from the live API) held up: the #152 merge SHA/timestamp it
  predicted for its own claim retirement matched what this session
  verified. One gap inherited and closed here: it deferred its claim
  retirement "to the next control-lane pass" — this was that pass.
