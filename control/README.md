# control/ — manager↔lane coordination protocol

> **Status:** `binding` — seeded 2026-07-10 by the fleet manager. One writer per
> file (playbook R9); violating writer-ownership is the only way this protocol
> merge-conflicts. game-lab runs **ONE shared lane across two repos**
> (`pokemon-mod-lab` = Track A, `gba-homebrew` = Track B) with **per-repo
> inboxes**: orders for this repo's track land in THIS `inbox.md`; the lane's
> state for this repo lives in THIS `status.md`.

## The two files

| File | Writer | Semantics |
|---|---|---|
| [`inbox.md`](inbox.md) | **MANAGER ONLY** | Append-only orders. The lane NEVER edits this file — not to ack, not to mark done, not to fix typos. |
| [`status.md`](status.md) | **LANE ONLY** | Overwritten by the lane each session. The manager touches it exactly once: this bootstrap stub. |

## Inbox semantics — `status: new` stays `new`

Orders in the inbox keep `status: new` **in the file forever** — the manager
does not flip them and neither do you. To see what is unexecuted, **diff the
inbox against your own `status.md`**: any order number not listed as
acked/done in your status is new work. (Gen-1 lanes without this rule either
re-executed finished orders or waited for a status flip that never came.)

## The standing ritual (every session, every wake)

1. **FIRST:** `git fetch origin main`; read `control/inbox.md` **AT HEAD** —
   never from a stale clone (R1). Diff against your status. Claim an order
   BEFORE building: add `claimed-by:` on your status orders line, land it,
   then re-read the inbox at HEAD before you build (no double execution).
2. **HEARTBEAT BEFORE WORK:** your first commit is the session card / a
   status WIP line. A silent session is indistinguishable from a dead one,
   and the platform WILL sometimes make you silent for an hour.
3. Act on orders; an ambiguous order goes under ⚑ needs-owner in your status
   — then do the rest, don't stall.
4. **LAST:** overwrite `control/status.md` — timestamp (`date -u`), phase,
   health, last-shipped PR, blockers, orders acked/done, ⚑ needs-owner.
   **Re-read `control/inbox.md` at HEAD immediately before this final write**
   and ack anything new — the measured miss class is a lane that heartbeated
   15 minutes after an order landed without seeing it (fleet ping test,
   2026-07-09).

## Claiming work (not an ORDER) — one file per claim under `control/claims/`

Order claims cover the inbox; **work claims** cover everything else two
parallel sessions could both pick up — a coordinator-assigned slice, a
self-initiated build, a shared-surface change. Before starting such work,
create **one file per claim** — `control/claims/<branch-or-scope>.md`, a
single bullet `` - `branch-or-scope` · **scope** — detail · YYYY-MM-DD `` —
land it on main FAST (claims are `control/**` traffic and ride the CI fast
lane), re-read the directory at HEAD, build, then **delete the file at
session close**. Per-file is the measured winner over any shared list (~98%
merge-conflict rate for shared-append vs 0% per-file — superbot
`tools/sim/claim_layout_sim.py`); first claim merged to main wins a
collision; ~72h with no activity = abandoned, prune on sight. Full
convention + checker contract: `control/claims/README.md`. (`check` nags —
advisory-only — on unparseable, stale, duplicate, or legacy-located claims;
legacy homes `docs/owner/claims/` and root `claims/` are auto-detected
during the migration window, and a deliberate different home is pinned via
`substrate.config.json` → `claims_dir`.)

(Kit v1.8.0 template delta applied manually 2026-07-11 — this doc is
`diverged`; the host format blocks the template's "Grammar source of truth"
lines anchor to do not exist in this rewrite, so only this section applies.
Grammar for the status/inbox/claims formats is kit-owned:
`src/engine/grammar.py` in menno420/substrate-kit, EAP §6.8.)

## Rules that ride the protocol

- **Re-read the inbox at HEAD before acting AND before any close-out** (R19 —
  the ORDER-number race cost the fleet 2 PRs twice in one day).
- **Report progress ONLY in `status.md`** — never in the inbox, never only in
  chat (chat evaporates; git is the evidence, R2).
- **All timestamps from `date -u`**, never from the model's sense of time —
  the fleet ping sweep caught two lanes stamping local-time-as-Z.
- A no-op wake (no new orders) costs at most a heartbeat line in status —
  never a full PR round.
