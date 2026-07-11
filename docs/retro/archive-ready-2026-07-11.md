# Archive-ready note — 2026-07-11 (project chat being archived)

> **Status:** `binding-snapshot` — written on the owner's wrap-up order
> (2026-07-11, relayed by the coordinator mid-slice-5): the project chat
> is being archived and anything not in the repo is lost. This note is
> the durable hand-off. Written by session 21 (Gloamline slice 5).

## True state in one paragraph

Two playable, download-and-run games ship committed in `dist/` with
provenance, plus a third in flight in a sibling session. **Lumen Drift
(GBA)** is scope-complete at **v1.3** (167,996 B, sha256 `195a8679…`,
21-assert replay proof + deep-run telemetry proof in CI) — done pending
owner taste passes. **Gloamline (NDS)** is the active arc, five slices
in: concept (PR #50) → toolchain feasibility (PR #51) → walking skeleton
(PR #52) → shove + waves (PR #54, merged `c3331da`) → **barricades
(slice 5, PR from branch `claude/gloamline-barricades` — this session;
finished and landed per the wrap-up order's finish-or-park rule)**. The
game is a deterministic horde-defense: pure-function sim layer
(`gl_sim.c`) mirrored line-for-line by a host proof
(`tools/check-gloam.py`, LOCKSTEP RULE: same commit), a full-game mirror
sim + route autopilot (`tools/gloam-route.py`), a 32-word telemetry
mailbox read by 9 pinned headless CI proofs (112 asserts,
`nds-rom-build` job), byte-deterministic ROM builds, and
`dist/gloamline.nds` (slice 5: 110,080 B, sha256 `25ae4f81…`). The
**Brineward** pirate arc (concept merged, PR #53) runs in a PARALLEL
sibling session in its own dirs — pointer only, it writes its own retro;
nothing here speaks for it.

## Open ⚑ owner-actions (click-level)

1. **⚑ owner-click: create the Lumen Drift GitHub Release** — worker
   seat gets 403 on tags/releases. Tag `lumen-drift-v1.3` (create on
   publish, target `main`) → title `Lumen Drift v1.3` → attach
   `dist/lumen-drift.gba` from the merged tree (167,996 B; sha256
   `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94`)
   → point notes at `docs/PLAYING.md` + the v1.3 entry in
   `docs/current-state.md` → Publish.
2. **⚑ owner-click: merged-branch cleanup** — worker seat gets 403 on
   branch-delete; sweep merged `claude/*` branches from the branches
   page.
3. **⚑ owner-click (NEW, small but load-bearing): add `NDS ROM build`
   to the repo's required checks** (Settings → Rulesets/Branch
   protection). Today only the GBA "ROM builds" job is required, so an
   armed auto-merge can fire before the NDS proofs finish (recorded in
   `docs/PLATFORM-LIMITS.md`; until clicked, the lane discipline is:
   verify the post-merge `main` run is green and say so).
4. **⚑ owner hands-on: taste passes** — Lumen Drift graze tuning
   (refund pacing/shell feel) and Gloamline hand-feel (Shambler
   speed/stagger, shove push/stun/cooldown, and now barricade
   hp/radius/plank numbers) are machine-proven but await human hands.
5. **⚑ awareness (no click):** the model-attribution mismatch record
   (ORDER 003, session 9) belongs in the fleet manager's report
   compilation; family-level self-report lines are on every session
   card.

## Owner rulings of 2026-07-11 (chat-only knowledge, now durable)

- **Direction pick:** the owner picked the original zombie-survival
  arc, superseding the deepen-LD / Clockwork Courier / Shoal menu —
  already durable in `docs/concepts/gloamline-concept.md` (header).
- **Platform correction:** owner corrected the new game's target
  GBA → **Nintendo DS** mid-slice — already durable in the same concept
  header.
- **Standing directive, verbatim:** *"please continue expanding the
  games, I will give my review on them soon, the further you already
  are the better"* — this is the lane's standing mandate between
  orders (previously only paraphrased as "green-lit continuous
  expansion" in `control/status.md`; the verbatim form was chat-only
  until this note).
- **Arc topology:** Gloamline is THIS seat's active arc; the Brineward
  pirate arc runs in a parallel sibling session (its own dirs, its own
  claims/cards). Durable in `control/status.md` + this note.

## What a fresh session needs to resume (no chat history)

1. Read `docs/AGENT_ORIENTATION.md` → `docs/current-state.md` →
   `control/inbox.md` (never edit) → `control/status.md` →
   `ls control/claims/` — the standing ritual.
2. Gloamline next slice = **between-nights scavenge interlude** (the
   real plank source; slice-5 plank stock is the placeholder), then
   lantern-oil light pressure, synthesized audio, best-nights saves,
   difficulty-curve/watch-map polish — order per
   `docs/concepts/gloamline-concept.md`.
3. Non-negotiable rails: LOCKSTEP RULE (`gl_sim.c` ↔
   `tools/check-gloam.py` ↔ `tools/gloam-route.py` step order, same
   commit); night-1-bit-identical-by-construction to keep old pins
   (slice-4/5 both shipped with ZERO re-pins this way); GL_STRESS
   `make clean` discipline; `keypad_update(0)` boot trap; ±6-frame
   route skew envelope with ≥10 px margin; born-red card + claim first,
   status overwrite last, `python3 bootstrap.py check --strict` exit 0.
4. Session cards `.sessions/2026-07-11-session-{17,18,21}.md` carry the
   guard recipes (route derivation, re-pin policy, frame-cost probe
   caveats, emulator BGR-swap screenshot caveat).

## Nothing important is chat-only — confirmation

Checked this session: the owner rulings above are now durable (this
note + concept doc + status); the auto-merge/required-check process
finding is in `docs/PLATFORM-LIMITS.md`; all proof evidence, build
recipes, walls, and guard recipes live in `docs/`, `.sessions/`, and
CI. The 24h self-review (ORDER 004) is
`docs/retro/self-review-2026-07-11.md`. The Brineward sibling documents
its own lane. Nothing else of substance existed only in chat.
