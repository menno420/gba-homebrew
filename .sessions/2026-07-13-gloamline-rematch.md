# Session 38 (gloamline-rematch) — game-lab Track B

> **Status:** `in-progress`

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
