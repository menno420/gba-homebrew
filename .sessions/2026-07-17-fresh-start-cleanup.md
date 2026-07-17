# Session — fresh-start cleanup: claims, docs, arc-landing mission, next-tasks

> **Status:** `complete`

- date: 2026-07-17 (branch `claude/fresh-start-cleanup`; `date -u`)
- mission: **owner-authorized fresh-start cleanup for the owner-driven
  relaunch.** The Claude Code Projects EAP goes read-only 2026-07-21; the owner
  is winding down autonomy and recreating the projects. This pass makes `main`
  tell the truth about the current state and retires the EAP autonomy/merge
  doctrine — WITHOUT touching any of the 24 deliberately-open arc PRs.
- **📊 Model:** Claude Opus 4.8 · high · docs-only — fresh-start cleanup
- landing posture: **owner-driven.** Plain non-draft PR opened for owner
  review; no merge / auto-merge / ready-flip from this session — the owner
  merges server-side. Scope is docs + a stale-claim delete only; no `games/`
  source, no `dist/` ROMs, no workflow files, no open PR touched.

## What shipped

1. **Claims:** deleted the stale claim `control/claims/claude-gate-orphan-fix.md`
   (its PR **#153** merged 2026-07-17 → retire-on-land). **Kept** both claims
   READMEs and `control/claims/claude-overnight-menu-2026-07-16.md` — the
   latter maps to the deliberately-open **#171**, so by the "verify merged"
   rule it is NOT stale. (Recon digest had said only one non-README claim was
   on `main`; it had missed the overnight-menu one, which stays.)
2. **current-state.md:** corrected the stale facts — EAP read-only 2026-07-21;
   `main` read-path gate repaired by #153 on 2026-07-17; the In-flight section
   rewritten from "open-PR list is empty" to the real 24-open-PR state (four
   finished arcs + #154 + #171, held open by design); autonomy wind-down +
   project recreation; NEXT-TASKS / ROUTINES / NEXT-MENU relinked in the Doc
   index (read-path reachability preserved).
3. **Instructions (merge doctrine):** rewrote the agent-auto-merge / born-red /
   "READY never draft" / self-merge-grant doctrine to the owner-driven model
   ("the owner reviews and merges server-side") in `CONSTITUTION.md` (autonomy
   rails), `docs/conventions.md` (§"PR state and merge authority" 1–4 + §10),
   and `docs/PLATFORM-LIMITS.md` (the self-merge wall marked SUPERSEDED
   2026-07-16 with the ready-flip / auto-merge / PR-write denial recorded).
4. **NEXT-TASKS.md (new):** mission #1 = land the four finished arcs in stacked
   order (Underroot #155→#165, Tiltstone #166→#170, Wickroad #172→#175,
   Brineward #176→#179) + #154 + #171, with the dist-binary conflict recipe
   (take the arc branch's ROM, let ROM-builds re-verify); plus the owner-console
   items (Pages go-live, branch-protection required checks, #171 menu curation)
   and the autonomy wind-down context.
5. **Scaffolding:** `docs/ROUTINES.md` banner-deprecated (`Status: historical`,
   failsafe-cron deletion noted). Left `control/inbox.md` untouched (pure-append
   protected by `substrate-gate`) and `control/status.md` untouched
   (machine-readable heartbeat grammar) — narrative lives in current-state.md /
   NEXT-TASKS.md instead. No workflow / `games/` / `dist/` deletions.
6. **Verify:** `python3 bootstrap.py check --strict` run on this branch before
   push (read-path reachability green with NEXT-TASKS / ROUTINES linked from
   the current-state root).

## 💡 Session idea

**A one-shot `scripts/relaunch-preflight.py` that asserts the "owner-driven"
invariants** — no `enable-auto-merge` steps armed, no self-arming routine in
the trigger registry, every merge-doctrine doc free of the "arm auto-merge on
green" phrase, and the failsafe cron absent — would turn this hand-done
wind-down into a repeatable check the re-created project runs once at boot,
so the autonomy machinery can't silently creep back in.

## Previous-session review

`.sessions/2026-07-16-gate-orphan-fix.md` (PR #153) did the right root-cause
thing — it fixed the read-path gate at its actual root (a `readpath_docs` root,
current-state.md) rather than re-expanding the condensable heartbeat, and its
💡 idea (teach the condense ritual / preflight to run `check --strict` before
opening the ender PR) is exactly the class this session generalizes. What it
could not see from inside the EAP frame: the whole born-red / auto-merge
apparatus it dutifully followed (born-red card, "no self-merge — automation
decides") was itself the thing the classifier had already walled and that this
relaunch retires. System improvement surfaced: the born-red gate reddens every
docs PR by design, which is pure friction now that a human performs the merge —
`substrate-gate` should drop to advisory (queued as an owner-console decision in
NEXT-TASKS.md) rather than stay a standing red on every PR the owner reviews.
