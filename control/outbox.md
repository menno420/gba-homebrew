# control/outbox.md — LANE-WRITTEN, append-only (lane → manager)

> One writer: the game-lab lane (Track B). Entries append at the bottom and
> are never edited after landing. The manager sweeps this file; anything
> flagged ⚑ needs manager routing or owner clicks.

---

## 2026-07-13T10:50:11Z — seat-ender BAKE proposals (game-lab, Track B; manager-addressed)

File-provenance note: `control/outbox.md` does not exist at main HEAD
`d87f9ad` — it was created on unmerged PRs #87 (head `b57221a`) and #89
(head `a84933b`) with this exact header; this branch re-creates it the
same way, so the three PRs conflict trivially on this file at sweep time
(same header, distinct appended entries — re-land the later entries under
the survivor's header). Three baked proposals out of the night-run retro
(`docs/retro/2026-07-13-night-run.md`):

1. **Scribe prompt delta — inbox ORDER template must carry the grammar.**
   Add the `priority:` / `do:` / `why:` / `done-when:` header fields to the
   scribe prompt's ORDER template so a scribe's first commit conforms to
   substrate-gate's inbox-order grammar instead of discovering it red.
   Evidence: two scribes' first commits failed the gate on exactly this
   during the night run — retro §b(4).

2. **Owner-gated config — set required status checks on `main`.**
   Add "ROM builds" (and "substrate-gate") as required status-check
   contexts on gba-homebrew `main`. This is the enforcement fix that makes
   the installed auto-merge-enabler (PR #76) safe and effective: with zero
   required contexts it correctly refuses to arm, so every green PR still
   needs a hand-sweep. Already flagged ⚑ on the heartbeat with the
   six-field paste-ready ask (and in PR #87's outbox entry). Evidence:
   retro §d (open item) + §b(2).

3. **Prompt delta for dispatched sessions — bounded MCP polling, never
   shell sleeps.** Replace "sleep N && re-check" loops with GitHub-MCP
   check-run polling under a fixed poll cap (e.g. max 3 polls, then report
   the freshest state honestly). A Gloamline session wedged on a
   background-sleep permission flag before recovering to this pattern;
   REST-over-proxy is separately unusable (silent-empty), so MCP is also
   the only reliable transport. Evidence: retro §b(3) + §b(5).
