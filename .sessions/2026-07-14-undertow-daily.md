# Session — Undertow growth cut 1: daily seeded run + shareable score URL

> **Status:** `in-progress`

- date: 2026-07-14 (branch `claude/undertow-daily`, started 01:40Z — `date -u` at write time)
- mission: build Undertow's lowest-risk named growth cut from
  `games/web-undertow/CONCEPT.md` — "Daily seeded run with a shareable
  score URL (`?seed=YYYYMMDD`)". A `?daily=1` mode derives the seed from
  the UTC calendar date as YYYYMMDD (read ONCE at boot, never inside the
  sim step — the sim stays wall-clock-free), plus a share mechanism: the
  gameover screen offers a copyable challenge link (`?seed=N&depth=M`,
  seed pinned explicitly so the link replays the same run on any day;
  `depth` is render-only, a title-screen score to beat). Static hosting
  only — no server, no leaderboard; the URL itself is the share.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-undertow-daily.md`, born red
   (claim before build), cut from main @ `6cb85ee`.
2. (work lands in later commits — this card flips at close-out)
