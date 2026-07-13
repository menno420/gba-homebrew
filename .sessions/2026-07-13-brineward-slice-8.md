# Session 42 — game-lab Track B

> **Status:** `in-progress`

- date: 2026-07-13 (branch `claude/brineward-slice-8`, started 12:58Z)
- mission: **BRINEWARD arc slice 8 — SYNTHESIZED AUDIO SET.** The next
  roadmap beat after danger bands in
  `docs/concepts/brineward-concept.md` (loot/gold → port+upgrades →
  the Maw → wind → danger bands → **audio** → saves): "synthesized
  audio set" — the doc's stated direction: "cannon thump (noise burst
  + low sine), hull crack, splash hiss, a two-note dock chime. No
  sampled or licensed audio, ever." Authorized by ORDER 005 (owner
  night-run directive, `control/inbox.md` @ HEAD `d87f9ad`): "Current
  tracks keep shipping: next Gloamline + Brineward slices as open PRs
  (rule 2)" — this branch is cut from the open PR #82 head
  `claude/brineward-bands` @ `20f4bfa` (slices 6+7, unmerged), base
  noted in the PR body per rule 2.
- session number 42 claimed (38 = slice 7 on this branch's base; 39 =
  Deepcast PR #83, 40 = release packaging PR #85, 41 = Cindervault
  PR #86, per those branches' committed cards; the other night-run
  siblings ran unnumbered slug cards).
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- night-run posture (ORDER 005 rule 2): PR opens READY against main;
  no merge or auto-merge API calls from this session — the owner
  sweeps in the morning.

## Plan (the Gloamline slice-8 pattern, ported to the brine)

1. Pure decision layer in `bw_sim.{h,c}` + the check-brine.py mirror
   in lockstep: ambience tiers (the WEATHER sings in the rigging —
   calm/breeze/gale — and the Maw's presence overrides it) + one-shot
   cue tables (id order = priority order), incl. the two-note dock
   chime (a `freq2` column — the cue channel re-tunes mid-cue).
2. Playback = ARM9 glue in `main.c` over the libnds sound FIFO
   (hardware PSG square + noise channels, ARM7-serviced) — audio
   feeds NOTHING back into the sim, so every pre-slice-8 pin holds
   bit-identically. Telemetry extends 38 → 46 (words 0-37 frozen,
   per the session-38 guard recipe).
3. Headless proofs 17-19 reuse COMMITTED stories only (no new
   routes): the gale bank story (proof 14's), the Maw bite story
   (proof 11's), and the idle loss (proof 3's) — every pin a row
   lookup in a mirror-of-main.c vs emulator watch-log diff.
4. Rails from the session-38 guard recipe: no reef cue off enemy
   positions (the scrape cue fires off `groundings`, a player-only
   counter); no committed key file touched; B stays reserved.

(Close-out, idea, and previous-session review land at the flip.)
