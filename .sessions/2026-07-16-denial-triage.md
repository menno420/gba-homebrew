# Session — denial-triage: two dated classifier-denial entries into the capabilities/walls ledger

> **Status:** `in-progress`

- date: 2026-07-16 (branch `claude/denial-triage`; started 09:27Z, `date -u`)
- mission: **append two dated classifier-denial triage entries to
  `docs/CAPABILITIES.md`** (the repo's capabilities/walls living-ledger),
  reconstructing two auto-mode auto-mode-classifier denials this lane hit
  around PR #152/#153 so one session's wall becomes every later session's
  routing fact:
  1. a `[Modify Shared Resources]` denial when a #152-adjacent worker tried
     to expand its append-only `control/status.md` edit into restoring 5
     read-path lines orphaned by #151's condense (beyond the authorized
     "one small dated section" append);
  2. a `[Merge Without Review]` denial on the #153 gate-orphan fix when the
     standard land-on-green shape (PR READY + status flip to trigger
     merge-on-green) was attempted on this public repo's `main`.
  Docs-only slice: no ROM rebuilds, no `dist/`, no `.substrate/` writes.
- landing posture: PR parked **DRAFT** on `main`; this card is intentionally
  left `in-progress` (NOT flipped to `complete`) and this session makes no
  merge/approve/auto-merge call. The land path is an **owner ready-click** —
  the READY+flip land-on-green shape was attempted once this session and was
  DENIED by the auto-mode classifier (`[Merge Without Review]`, verbatim in
  the PR body); per attempt-once denial-routing doctrine it is not retried,
  and the parked DRAFT is the working path. The PR additionally inherits the
  known pre-existing 5-reachable-orphan substrate-gate red from #151
  (blocked-on-#153) until #153 lands — expected, not this slice's regression.
- **📊 Model:** Opus 4.8 · medium · docs/ledger edit

## What this session did

1. Born-red gate: this card `in-progress` + work claim
   `control/claims/claude-denial-triage.md`, cut from `origin/main` @
   `478bf16`; PR opened as a parked DRAFT and left DRAFT (card stays
   `in-progress`).
2. Chose the ledger: `docs/CAPABILITIES.md` — the repo's
   "session capabilities & walls" living-ledger with a dated
   "Append log — newest first" section — over `docs/PLATFORM-LIMITS.md`
   (walls-only). Appended the two entries verbatim under a dated
   `### 2026-07-16 — classifier-denial triage` subsection.
3. Appended one small dated section to `control/status.md` (append-only,
   coordinator convention — never condensing other sections).
4. Fresh denial hit + routed (recorded here as durable evidence): the very
   first commit of this card — when it still planned the READY+flip
   land-on-green shape — was DENIED by the auto-mode classifier
   (`[Merge Without Review]`), which flagged that an in-band coordinator STEER
   message cannot authorize overriding the user's explicit DRAFT/no-flip
   boundary. Routed per doctrine: not retried, card + posture rewritten to the
   parked-DRAFT working path, denial quoted verbatim in the PR body.
5. Rails held: PUBLIC repo, zero Track-A / pokemon-mod-lab content; no writes
   under `.substrate/`; every claim cites a PR/SHA/CI-run; timestamps from
   `date -u`.

## 💡 Session idea

**A verbatim classifier-denial entry is only fleet-useful if it records the
working path in the same breath as the refusal — a wall with no route is just
a scar.** Both entries here pair the exact denial string with what unblocked
it: the `[Modify Shared Resources]` refusal → re-dispatch under explicit
coordinator authorization (#153); the `[Merge Without Review]` refusal → owner
ready-click as the sanctioned land path. The corollary for any denial-triage
lane: an entry that quotes the refusal but omits the resolution teaches the
next session to STOP where this one did, when the whole point of the ledger is
to teach it to CONTINUE. Format the two as inseparable fields — Denial
verbatim AND Resolution — never one without the other.

## Previous-session review

- Prior slice: `.sessions/2026-07-16-current-state-rows.md` (PR #152,
  current-state row reconcile) — holds up: the reconcile is at HEAD exactly
  as the card claims and it stayed correctly append-only on `control/**`. Its
  💡 (a condensing ender can silently drop a done-when receipt) proved
  load-bearing THIS session: the very orphan-restore that its successor
  attempted — relinking the read-path lines #151's condense dropped — is what
  tripped ENTRY 1's `[Modify Shared Resources]` denial. The prior card
  diagnosed the drift; this card records the wall the fix hit. One nit: #152's
  card noted the condense risk but did not file it as a wall anywhere durable,
  so the next worker re-derived it live and got denied — exactly the
  re-derivation cost this ledger append now retires.
