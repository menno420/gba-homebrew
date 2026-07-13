# Session 38 (gloamline-rematch) — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/gloamline-rematch`, based on
  `claude/gloamline-watch-map` @ `253ff64` — open PR #75; base stated
  per the night-run "don't hold work behind unmerged parents" rule)
- authority: owner night-run order via coordinator dispatch (2026-07-13
  night run) — "ship the next Gloamline slice as an OPEN pull request;
  the arc continues beyond the concept tree." At dispatch time the
  matching inbox ORDER (expected ORDER 005, landed by a parallel scribe
  session) is NOT yet on main: `control/inbox.md` at HEAD `0e08695`
  ends at "## ORDER 004 · 2026-07-11T10:01Z · status: new" —
  re-fetched once at 2026-07-13T00:56Z before this card, still absent,
  so per the dispatch's own terms: authority: owner night-run order via
  coordinator dispatch; inbox ORDER landing in parallel.
- session numbering / collision avoidance: the card FILENAME is
  slug-keyed (`2026-07-13-gloamline-rematch.md`) so it cannot collide;
  the "38" in the title is by count only — highest taken number is 36
  on main (`0e08695`) and 37 on the one open branch
  (`claude/gloamline-watch-map`), checked at session start. Two other
  sessions (a scribe and a tally session) are active tonight; if
  another card also counts itself 38, the slug disambiguates.
- mission: **Gloamline arc slice 11 — BEST-NIGHT REMATCH.** The concept
  tree went scope-complete at slice 10; tonight's order explicitly
  continues beyond it. This slice pays the debt slice 9 wrote into its
  own player-facing text: "because every run is seed-deterministic,
  the recorded seed means your best night is *literally replayable* —
  start a run when the frame counter matches..."
  (docs/PLAYING-GLOAMLINE.md, slice-9 save notes) — which is a wink,
  not a feature: a human cannot time a frame counter. Scope:
  (1) **SELECT at the title screen** — when a record exists — starts a
  run on the RECORDED best seed instead of the frame-counter latch:
  the very night the record was set, spawn for spawn, replayable on
  purpose; (2) **SELECT at the death card** does the same (the natural
  "chase it again right now" seat — TITLE is unreachable after the
  first START, so title-only would make the rematch once-per-power-on);
  (3) with NO record, SELECT stays completely inert on both screens
  (the moor keeps no empty boasts — same rule as the title/card BEST
  lines); (4) a rematch run wears a `*` after its SEED on the HUD and
  the cards; START anywhere keeps the old fresh-latch path
  bit-identical. Pure layer: `gl_rematch_available(best_nights)` +
  `gl_run_seed(rematch, best_nights, best_seed, latched)` in
  gl_sim.h/.c, three-way lockstep (gl_sim.c <-> tools/check-gloam.py
  <-> tools/gloam-route.py — the route mirror gains the SELECT rematch
  edges + `--best/--best-seed/--save-ok` power-on state flags so every
  pin is mirror-predicted first); telemetry appends slots 72-79
  (REMATCH flag, REMATCHES count, 6 spares) with slots 0-71 frozen
  (mailbox 72 -> 80 words); 2 new pinned headless proofs (the rematch
  chain title -> death card -> fresh START on the proof-20 battery
  record; the no-record inertness chain on a factory-fresh chip);
  zero re-pins targeted (9th consecutive hold) — no committed route
  presses SELECT at the title or the death card, and a non-rematch
  run's sim is bit-identical. No save-format change; no new audio cue
  (the nightfall toll already rings every path into a night, rematch
  included). 100% original content.
- 📊 Model: Claude Fable 5 (family-level self-report from this
  session's own harness/environment banner, per ORDER 003)

## 💡 Session idea

A new verb that only CHOOSES which pure world to enter can be proved
as a TIME-SHIFT of pins you already own. The rematch's headless proof
re-uses proof 3's exact chase literals (17501 → 15077 → contact 2405)
ten frames later, unedited — because idle-chase state is a pure
function of (seed, run_frame) alone, the global frame enters only
through the seed latch, and the rematch's whole point is removing the
latch. So the proof script became: move the press, keep every value;
and that diff (press frame moved, literals identical, TWICE in one
run — the death card rematch dies to the same 2405 again) is itself
the strongest possible evidence that the verb feeds nothing back into
the sim. Transferable rule for the lane: when a slice adds seed/world
SELECTION rather than any rule change inside the world, don't derive
new routes — re-pin an existing proof's literals at a shifted anchor
and let the shift BE the feature's evidence. Corollary used here: pin
the counterfactual too (the nightfall-cue timestamp t[51] = 128 shows
what the latch WOULD have read while t[12] = 118 shows what the
rematch chose — one assert pair proves the choice happened).

## Previous-session review

- Session 37 (slice 10, PR #75 — this branch's base) set the rails and
  they all held a NINTH time: telemetry append-only with older slots
  frozen (72 → 80 this session), mirror-predict-then-pin (proofs 27/28
  matched their gloam-route predictions exactly on the first emulator
  run), the ±6-skew envelope (not needed here — both new scripts are
  skew-immune by construction: idle runs + START/SELECT anchors only),
  and the born-red card gate. Its anomaly log PAID OFF CONCRETELY this
  session: the "GL_STRESS `make clean` discipline" guard recipe
  diagnosed in one look why proof 7 went red on my first local replay
  (a stress build over warm non-stress objects builds a normal ROM) —
  a symptom-only note would have cost a re-derivation.
- Session 37's card also flagged "prefer touch (not X) for any future
  pin that depends on a mark position mid-route" — noted, not needed
  here (the rematch pins are position-free by design).
- At session start: main = `0e08695` (auto-merge-enabler install,
  PR #76), PR #75 open at `253ff64` (base of this branch, untouched
  by this session per the night order), control/claims/ empty on main.

## What this session did

**SLICE 11 SHIPPED on PR #80 (opened READY; left OPEN per the
night-run order — no merge calls, no arming, no labels from this
session).** All 28 proofs ran green in-container against the final ROM
before the push; both new proofs matched their mirror predictions
exactly on the first emulator run.

1. Born-red card + claim first (`61012de`); branch cut FROM `253ff64`
   (PR #75's open head — nothing waits on a merge; base stated in the
   PR body).
2. **Pure layer** (`3c433a0`, three-way lockstep in one commit):
   gl_rematch_available + gl_run_seed; check-gloam proof 16 (32 gate
   cases, 128 seed-choice truth-table cases, 132 spawn-for-spawn
   replay checks; 1.7 s); gloam-route gained the SELECT rematch edges,
   --best/--best-seed/--save-ok and --run-through; all 7 committed
   routes byte-identical old-mirror-vs-new (zero drift).
3. **main.c glue + proofs** (`d49dcf2`): SELECT branches at
   TITLE/DEAD, the '*' seed star (render-only), gated PRESS SELECT
   offers, telemetry 72 -> 80 (slots 72-73 REMATCH/REMATCHES, 0-71
   frozen); rom-builds.yml proofs 27-28 (**28 proofs / 517 asserts**;
   all 464 pre-slice-11 asserts byte-identical — **zero re-pins, the
   trick's NINTH hold**).
4. **Ship** (`f3afd5b`): dist/gloamline.nds **118,272 B, sha256
   `3bab5544c03d01e358fd1f8898df47031bb3f48107fb1469551ab7ed794ec061`**,
   byte-deterministic (two clean builds + an independent
   fresh-worktree build identical); PLAYING-GLOAMLINE rematch notes;
   current-state ledger; dist/README row; screenshots
   `slice11-rematch-{offer,run,card}.png`.
5. CI on head `f3afd5b`: "ROM builds" run 29216933696 SUCCESS — jobs
   "ROM builds" 86714531345, "NDS ROM build" 86714531326 (all 28
   proofs incl. 27/28), "NDS Brineward build" 86714531334 all green;
   auto-merge-enabler run 29216933669 job 86714531317 SUCCESS (the
   owner-installed enabler at `0e08695` armed native auto-merge — this
   session neither armed nor fought it, per the order); substrate-gate
   run 29216933661 job 86714531136 FAILURE = the designed born-red
   hold (local `bootstrap.py check --strict` showed the hold as the
   only finding), flipped by this commit.

## Anomaly log (for the coordinator + next wake)

- **GL_STRESS `make clean` discipline bit again and the guard recipe
  paid off**: first local stress build reused warm non-stress objects
  (proof 7 red locally: t[16]=1 not 24); session 37's anomaly-log
  recipe named the cause in one look. Keep carrying it.
- **api.github.com REST is silent-empty through this container's
  proxy even for the in-session repo** (curl returned nothing, exit
  0) — a curl-based CI poll loop therefore never fires; use the
  github MCP tools for check-run polling (matches the sessions-27/30
  "gh absent, MCP for GitHub ops" precedent).
- **The enabler landed tonight** (`0e08695`, PR #76): claude/* PRs get
  native auto-merge armed at open; the required check is the GBA "ROM
  builds" job only, so the PLATFORM-LIMITS "auto-merge can fire before
  nds-rom-build completes" footgun is now armed by default on every
  agent PR. Observed tonight: #80 stayed unmerged with substrate-gate
  red on the born-red hold, so in practice the gate red held the
  merge back — evidence the required-check set now includes
  substrate-gate, or auto-merge is waiting out the full suite; either
  way the owner sweep decides.
- **What green does NOT verify (carried to the PR):** the rematch '*'
  star's HUD cost is bounded by the existing per-frame HUD reprint
  (one character in an already-reprinted line) but was NOT re-measured
  under GL_STRESS — the stress build cannot reach a rematch (no
  record at power-on); the pinned proof-7 stress scenario is
  untouched and re-ran green. Real-hardware touch/backup rails carry
  from slices 9/10 unchanged; nights 13+ wave-plateau gap carries
  from slice 4.
- Session numbering: card filename is slug-keyed
  (2026-07-13-gloamline-rematch.md); title counts itself 38 with the
  collision caveat recorded at the top of this card.
