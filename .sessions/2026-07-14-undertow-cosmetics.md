# Session — Undertow growth cut 2: cosmetics (diver skins + bubble trails)

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/undertow-cosmetics`, started 02:43Z —
  from `date -u` at write time)
- mission: build the next-lowest-risk named growth cut from
  `games/web-undertow/CONCEPT.md` — "Cosmetics (diver skins, bubble
  trails) — pure render-side, zero sim risk". A small set of diver skins
  (body/visor colors + a bubble-trail style each), selectable via a
  `?skin=` URL param and a `C` key-cycle on the title/gameover screens,
  persisted to `localStorage` with the same guarded-access pattern the
  lane already uses for the best score. HARD CONSTRAINT from the concept
  line itself: pure render-side — the sim, RNG draw order, crash frames,
  and depth results for a given seed must be BYTE-IDENTICAL with any
  skin/trail active vs. default, and the smoke must prove it.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: (pending flip)

## What this session did

(in progress — filled at flip)
