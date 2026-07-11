# Self-review 2026-07-11 — game-lab Track B (ORDER 004, ~last 24h)

> **Status:** `archive` — completed self-review; answers by ID per
> [`QUESTIONS.md`](QUESTIONS.md); window 2026-07-10 ~20:00Z →
> 2026-07-11 ~19:40Z. Written by session 21 during the owner-ordered
> wrap-up (ORDER 004 asked for this within two wakes; the archive order
> pulled it forward). Every claim cites repo evidence. The Brineward
> sibling session self-reviews its own lane.

## 1. What WENT WRONG (with citations)

- **Auto-merge fired before the NDS job finished** (process drift, not
  a red build): the repo's required-check set is only the GBA "ROM
  builds" job, so slice-4's PR #54 merged on a rebase-push while
  `nds-rom-build` was still running; the NDS proofs went green on the
  post-merge `main` run instead of pre-merge. No bad code landed (the
  post-merge run WAS green), but the gate order was wrong. Now recorded
  in `docs/PLATFORM-LIMITS.md` + ⚑ owner-click queued (add
  `NDS ROM build` to required checks). Time lost: none; risk carried
  until the click.
- **Emulator screenshot channel swap discovered** (session 21): the
  py-desmume `display_buffer_as_rgbx()` buffer is effectively
  BGR-ordered — every committed Gloamline screenshot since slice 3
  shows red↔blue swapped colors (the barricade's wood-brown renders
  blue-grey; the lantern's brass glow renders cyan). Silent because
  non-blank/variance checks and numeric telemetry don't care. Palette
  values in `main.c` are correct for hardware (libnds RGB15). Evidence:
  pixel sampling in the session-21 slice; caveat noted on the session
  card. Preventable earlier by sampling one known color — cheap lesson.
- **Walls hit: none new.** All carried walls (devkitPro 403 mirror
  route, api.github.com proxy scope, mGBA load_save segfault,
  py-desmume KEYINPUT boot trap) were respected via their documented
  recipes; zero re-probes (`docs/PLATFORM-LIMITS.md`).
- **Red CI runs: none in the window** on this lane's PRs (#54 and the
  slice-5 branch ran green end to end; hourly-wake and kit-upgrade PRs
  likewise).

## 2. REQUIRING OWNER ATTENTION

Mirrored click-level in `docs/retro/archive-ready-2026-07-11.md` § open
⚑ and on the status heartbeat: (1) Lumen Drift v1.3 Release click
(sha256 `195a8679…`); (2) merged-branch sweep; (3) **add `NDS ROM
build` to required checks** (closes the auto-merge gate gap); (4) taste
passes on graze + shove/barricade hand-feel numbers; (5) awareness:
model-attribution mismatch record for the fleet report.
Decide-and-flag decisions taken in-window that the owner may veto
cheaply: barricade numbers (16 px radius, 240 hp, 6 planks +2/dawn cap
9, repair-over-place in reach), shove whiff costing full cooldown —
all one-constant changes in `gl_sim.h`.

## 3. One-line current health

Two shipped playables (`dist/lumen-drift.gba` v1.3 scope-complete,
`dist/gloamline.nds` slice 5 barricades) with 112 pinned NDS asserts +
host mirrors green; next: Gloamline scavenge interlude; no blockers.

## Selected retro-question answers (IDs per QUESTIONS.md)

- **A1:** Shipped to main in-window: PR #54 (slice 4) + slice-5 PR
  (this session's, see status). Nothing stranded on branches except
  merged-then-undeletable `claude/*` heads (⚑ cleanup).
- **A2:** External-oracle verified: every pinned CI value was
  double-derived (host mirror predicts → DeSmuME emulator confirms,
  exact match, both slices); ROM byte-determinism (two clean builds,
  identical sha256). Own-tests-only: nothing player-visible.
- **A3:** Least confident: perf headroom is DeSmuME's scanline model,
  not hardware — steady 68/71 lines leaves only 3 lines of modeled
  margin at the 24-crowd + 8-ring worst case. Concrete check: one run
  on real DS/flashcart, or melonDS cross-measure.
- **B2:** The auto-merge/required-check behavior should have been in
  PLATFORM-LIMITS from the day the NDS job was added — it took a live
  merge to surface it. Fixed this session.
- **C3:** Most value/minute: the mirror-sim → pinned-assert pipeline
  (gloam-route.py predicts a number once, CI asserts it forever).
  Least: hand-deriving kiting routes before the autopilot existed.
- **D2/D4:** Zero upward routes needed in-window beyond the standing
  grants; the ⚑ queue covers the genuinely owner-only set (releases,
  rulesets, branch deletes, taste).
- **E4:** A fresh session's first misunderstanding would be "why is an
  NDS game in gba-homebrew?" —
  `docs/retro/archive-ready-2026-07-11.md` § owner rulings now answers
  it (owner correction, repo-name note carried).
- **F1 (three rules for a next founding):** (1) put every
  required-check name in the founding doc and verify auto-merge waits
  on ALL jobs before the first arm; (2) mirror-proof pipelines
  (pure sim ↔ host mirror ↔ emulator pin) are the cheapest correctness
  rail a game lane can buy — mandate them from slice 1; (3) write the
  no-trap/no-softlock invariant into the SHAPE of a predicate (like
  `gl_barricade_blocks`), never as post-hoc patches.
