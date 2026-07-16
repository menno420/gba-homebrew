# Session — current-state rows: reconcile the ledger with merged PRs #148–#151

> **Status:** `in-progress`

- date: 2026-07-16 (branch `claude/current-state-rows`; started
  01:14Z, `date -u`)
- mission: **reconcile stale rows in docs/current-state.md against
  live GitHub at HEAD `d38887c`** — the `## In flight` section still
  lists the NEXT-MENU doc as this-slice-open though it merged as #150
  (`588aa4e`, 2026-07-15T21:42:05Z), and the shipped ledger predates
  #148–#151. Docs-only slice: every row edit cites a merged PR / SHA /
  merged_at fetched from the live API this session, no invented state.
  Plus one appended dated section in control/status.md (append-only,
  coordinator convention).
- 📊 Model: Claude (Fable family) · standard effort · docs-reconcile
- landing posture: PR #152 opened READY at the born-red commit; no
  merge/approve/auto-merge calls from this session — the server-side
  enabler decides on green after the flip.

## What shipped

1. Born-red gate (commit a8b5224): this card `in-progress` + work
   claim `control/claims/claude-current-state-rows.md` (written by
   `bootstrap.py claim`; retire per claims README rule 4 falls to the
   next control-lane pass — this slice is append-only on control/**
   beyond its own claim), PR #152 opened READY immediately.
2. **The reconcile** (`docs/current-state.md` only), every fact
   fetched from the live API this session:
   - In-flight NEXT-MENU row resolved: it merged as #150 (`588aa4e`,
     2026-07-15T21:42:05Z); the doc link to NEXT-MENU-2026-07-15.md is
     kept in place so the read-path reachability #150 established
     still holds. The live open-PR list at write time: empty apart
     from this slice's own #152.
   - Ledger carried through #151 with one newest-first entry: probe
     #148 (`150abf1`, 2026-07-15T14:15:47Z), Pages deploy #149
     (`872627d`, 2026-07-15T21:28:17Z, go-live still owner-gated),
     decision menu #150 (`588aa4e`, 2026-07-15T21:42:05Z), coordinator
     ender heartbeat #151 (`d38887c`, 2026-07-16T00:53:41Z — main
     HEAD at reconcile time).
   - Header blockquote: one reconcile-note line added; living-ledger
     badge untouched.
3. Dated section appended to `control/status.md` (append-only): what
   this slice did + the ORDER 007 ack finding — the done-when ack was
   already recorded at `df55299` (#142, 2026-07-15), so no re-ack.
4. Rails held: no writes under `.substrate/`, no dist/ or ROM
   rebuilds, no merge/auto-merge calls, no routines touched.

## 💡 Session idea

**A coordinator ender that condenses control/status.md can silently
drop a done-when receipt — condensation should move receipts, never
delete them.** Concrete catch this session: ORDER 007's ack ("first
rebooted wake") was recorded at `df55299` (#142) but the #151 ender
heartbeat rewrote status.md (−297 lines) and the ack now survives
only in git history — a successor checking the file at HEAD would
wrongly conclude the order was never served and re-ack it. Cheap
rule: before an ender condenses a control file, grep the outgoing
text for done-when phrases from open inbox ORDERs and carry those
lines (or a one-line pointer to their commit SHA) into the condensed
version.

## Previous-session review

- Prior lane slice: `.sessions/2026-07-15-next-menu.md` (PR #150) —
  its research-at-HEAD note (gate research at `150abf1`, HEAD moved to
  `872627d` before first commit, citations re-pinned) is exactly the
  discipline this slice leaned on: every SHA it published checked out
  against the live API a day later, so reconciling its row was pure
  confirmation. The one wrinkle it left: its own "this slice's own PR"
  phrasing in current-state.md's In-flight section went stale the
  moment #150 merged — a self-referential row is guaranteed to rot on
  merge, and writing it as "open as of <date>, resolves on merge"
  would have saved this session one of its two edits.
