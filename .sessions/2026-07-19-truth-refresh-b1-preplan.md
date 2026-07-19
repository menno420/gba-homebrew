# Session — Controller-lane truth refresh + B1 (Tiltstone arc 2) pre-plan judgment

> **Status:** `complete`

- date: 2026-07-19 (branch `claude/gba-truth-refresh-b1-preplan-0719`, main
  synced @ `01a72c5`; all timestamps from `date -u`)
- mission: **planning/continuity slice, not execution** — the gba execution
  backlog is dry (every remaining arc gate is owner-side), so per the owner's
  universal continue directive (2026-07-19: "when execution is dry, PLAN") this
  session does a two-part wind-down docs pass: (1) **truth refresh** — the
  Controller lane FINISHED after the close-out doc landed, and `control/status.md`
  needs every lane's state true as of now; (2) **B1 pre-plan judgment** — decide
  whether Tiltstone arc 2 (menu option B1) needs a fresh pre-plan doc to be
  instantly executable if the owner picks it.
- **📊 Model:** Claude Opus · high · idea/planning — wind-down truth refresh +
  B1 executability judgment (no gameplay code touched)
- landing posture: READY (non-draft). Plain born-red `claude/*` card PR — this
  card holds the PR red as `in-progress` until the close-out flip (the LAST
  commit), then it lands on green `ROM builds` via the existing
  `auto-merge-enabler.yml`. Docs + control + card only, no workflow file.

## What shipped

1. **Born-red gate** (this card `in-progress` + the work claim
   `control/claims/claude-gba-truth-refresh-b1-preplan-0719.md`), filed before
   the docs so the PR is red until the close-out flip.
2. **Slice 1 — truth refresh** — `docs/eap-closeout-gba-2026-07-19.md` +
   `docs/arcs/TILTSTONE.md` + a dated `control/status.md` heartbeat (details
   below).
3. **Slice 2 — B1 pre-plan judgment** — SKIP-with-reason (details below); no
   pre-plan file written, by design, not omission.

## Slice 1 — truth refresh (details)

- **Controller row → DONE.** The close-out doc's Controller row read
  "UNBLOCKED (reserved for Controller lane)". Corrected to **DONE**: the
  Android/Gradle controller lane is complete — product-forge **#31 MERGED**
  (app module wired into the Gradle build, `:app:assembleDebug` green; merge
  commit `f527ca0`, `github-actions[bot]`, 2026-07-19T07:57:25Z; coordinator
  relays CI run 29679129919) and product-forge **#32 MERGED** (the
  `assembleDebug` CI lane went live; owner-merged `menno420`, merge `e3fc844`,
  2026-07-19T09:05:57Z — a workflow-file PR, so owner-merge-only). Both
  **verified in-repo via github MCP `pull_request_read`** on
  `menno420/product-forge`. Only remaining Controller item = **owner hardware
  playtest** (phone + physical BT-HID receiver).
- **Tiltstone row → LANDED (high-value stale-doc correction).** The close-out
  doc + `TILTSTONE.md` + NEXT-MENU § B1 all said Tiltstone arc 2 is
  "draft-parked, not merged, carried as owner menu option B1." **Ground truth
  disagrees:** the arc-2 engine work is fully **on main** — `engine.js` at
  **v1.8.0** carries all five cuts' pure functions (`encodeShare`/`decodeShare`/
  `spectate`, `hintFrom`/`hintedGrade`, `deception`, `fingerprint`,
  `rampFloor`/`generateRamp`), landed via **`c654e01`** (cut 1, share layer +
  smoke §14), **`46293b5`** (cut 2, solver hints + smoke §15), and **`207e391`
  «arc 2 cut 5 the monotone ramp (arc closer) (#170)»** (which also carries the
  cut-3 deception + cut-4 fingerprint functions). All three are ancestors of
  main HEAD `01a72c5` (`git merge-base --is-ancestor` → true). `node
  games/web-tiltstone/smoke.mjs` → **SMOKE PASS: all assertions green (engine
  v1.8.0)**. Corrected the close-out doc's Tiltstone row + stale-doc watch and
  `TILTSTONE.md`'s status badge to match main.
- **Landed PRs this pass folded in:** #205 (the close-out doc, merged) and #206
  (`dist/wickroad.gba` refreshed to the post-#201 build) — added to the
  close-out doc's landed-PR list and the HEAD stamp refreshed to `01a72c5`.
- **`control/status.md`:** a dated **append-only** worker heartbeat (does NOT
  touch coordinator sections — one-writer rule) stating each lane's true state:
  Controller DONE (owner hardware playtest only), Brineward
  closed-subsumed-by-#179, Wickroad post-#201 with `dist/wickroad.gba`
  refreshed (#206), Tiltstone arc-2 LANDED on main, landed #205 (doc) + #206
  (ROM), and the planning-mode note + PR pointer.

## Slice 2 — B1 pre-plan judgment: SKIP-with-reason

**Judgment: a B1 pre-plan adds nothing, because B1 is already BUILT and
LANDED on main — writing "a plan to make B1 instantly executable if picked"
would be planning already-completed work, which is misleading, not helpful.**

- The menu framing (NEXT-MENU § B1 = "Tiltstone: the shareable daily", 5 cuts)
  is a decision option; a pre-plan would only add value if that arc were
  unbuilt. It is not: the five deterministic cuts' engine + host proofs are on
  main (evidence above), engine v1.8.0, `smoke.mjs` green.
- **What genuinely remains for Tiltstone is minor and already named in
  `docs/arcs/TILTSTONE.md`:** (a) the cut-5 daily-chain **browser-shell**
  opt-in (a progression refactor with no in-container proof — `app.js` is
  deliberately untouched so default output stays byte-identical); (b) the
  optional **touch-controls** cut (polish per CONCEPT § Sellability, not a
  determinism feature). Neither is owner-gated; both are small agent
  follow-ups, already documented — so a fresh pre-plan file would restate the
  arc doc, i.e. padding.
- Honest note on proof surface (why the generic pre-plan template did not
  apply): Tiltstone is a **web** game (`games/web-tiltstone/`). Its proof
  surface is the pure-Node `smoke.mjs` (the CI-honest gate) + the Chromium
  `tools/web-smoke-tiltstone.mjs` (local shell/UX) — **not** the GBA/NDS
  `tools/check-*.py` host-mirror + ROM build + headless-boot pattern, which
  cover the C/C++ ROM games. Applying that template to Tiltstone would have
  mis-described its proofs.

## Honest limits

- **Planning only — zero gameplay/build code touched.** Deliverable is a docs
  truth-refresh + a heartbeat + this card. No pre-plan file written (by
  judgment, § above).
- **product-forge #31/#32 are cross-repo** — verified via github MCP
  `pull_request_read`, not built here; CI run 29679129919 is coordinator-relayed
  (the PR merges themselves are in-repo-verified).
- **The stale Tiltstone framing survives in NEXT-MENU § B1.** I corrected the
  wind-down/continuity surfaces (close-out doc, TILTSTONE.md, status.md) but
  left the owner-facing decision menu itself unedited — a next pass could retire
  the B1 option outright (flagged, out of this slice's bounded diff).

## 💡 Session idea

**A "pick your next thing to build" menu is a liability document: each option
is an assertion "this is still unbuilt" that silently decays as work lands, and
nothing re-validates it — so a menu option should ship with a one-command
falsifier, not just prose.** This session's headline finding was that NEXT-MENU
§ B1 ("Tiltstone: the shareable daily") offers an arc whose entire engine is
already on `main`, green, at v1.8.0 — the menu rotted from a to-do into a
did-that without any edit, because a decision menu is written once and then
trusted, while the code under it keeps moving. Reading three prose docs
(NEXT-MENU says "pick it", TILTSTONE.md says "growth-complete pending clicks",
close-out says "draft-parked menu option") would never resolve the
contradiction — they're all confident and all stale. What resolved it in one
step was running the arc's OWN proof: `node games/web-tiltstone/smoke.mjs` →
SMOKE PASS at engine v1.8.0. The general move: every "unbuilt option" in a
build-menu should carry a **falsifier command** — the smoke/test/grep that
would go green (or the symbol that would already exist) IF the option were
secretly already done — so re-offering it is a 5-second check, not a
three-doc archaeology dig. A menu without falsifiers is a snapshot that ages
into misinformation; a menu where each row says "still open UNLESS `X` passes"
audits itself. (The neighbouring rot here — Pages "pending a click" when it's
live, `dist/wickroad.gba` "predates the flash" when #206 refreshed it — is the
same disease: a claim with no attached probe outlives the fact it described.)

## Previous-session review

- Prior lane slices (same session_01343oPvj5bzQZUsHuVsC9cK): the EAP close-out
  doc **#205** and the `dist/wickroad.gba` refresh **#206**, plus the earlier
  #200→#201→#202→#204 chain — all merged autonomously on green via
  `auto-merge-enabler.yml` (`github-actions[bot]`). Read at HEAD; the close-out
  card `.sessions/2026-07-19-eap-closeout-gba.md` and the refresh card
  `.sessions/2026-07-19-refresh-wickroad-rom.md` are both `complete`, matching
  the empty live open-PR set at `01a72c5`.
- **What transferred:** the close-out card's own thesis — *"a wind-down doc's
  highest-value cells are the ones where the repo's own docs are confidently
  WRONG"* — was the exact lens that surfaced this slice's biggest correction.
  That card led with three doc-vs-ground-truth deltas (Pages live, stale ROM,
  #171 on main); this slice found a **fourth of the same species** (Tiltstone
  arc-2 "draft-parked" while its engine is on main and green) and closed one of
  its three (#206 resolved the stale `dist/wickroad.gba`). The landing
  discipline transferred verbatim: born-red card + claim first, one flip to
  complete, land on green via `auto-merge-enabler.yml` (never the dead
  `merge-on-green.yml` name #200 corrected).
- **A divergence worth naming:** the close-out card *diffed docs against a live
  HTTP probe / `git log`*; this slice sharpened that into *diffing a
  build-menu's "unbuilt" claim against the code's own passing smoke* — the
  falsifier-command idea above. The close-out asked "is the doc's fact still
  true?"; this asked "is the doc's *to-do* already done?" — a subtly different
  and, for a wind-down, higher-stakes question, because a stale to-do wastes a
  whole re-picked arc, not just one wrong click.
- **Judgment divergence (deliberate):** the coordinator's Slice-2 brief
  anticipated *writing* a B1 pre-plan (and pre-drafted its section headers).
  Ground truth inverted the premise — B1 is built — so the value-first call was
  to **SKIP with citation** rather than pad a plan for done work. Decide, don't
  stall: made the call, cited it, moved on.
- **Guard honored:** every Bash pinned to the absolute repo root
  `/home/user/gba-homebrew` (never a bare `~`), so the two-checkout footgun the
  Brineward cards flagged stays shut; Track-A isolation held (original IP only,
  public repo — no Track-B content); ORDER 007 respected (no routines armed);
  no force-push.
