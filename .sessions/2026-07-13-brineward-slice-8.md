# Session 42 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/brineward-slice-8`, 12:58Z → 13:21Z)
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
  noted in the PR body per rule 2. PR #91, base `main` per the repo's
  stacked-slice convention (PR #82 itself).
- session number 42 claimed (38 = slice 7 on this branch's base; 39 =
  Deepcast PR #83, 40 = release packaging PR #85, 41 = Cindervault
  PR #86, per those branches' committed cards; the other night-run
  siblings ran unnumbered slug cards).
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- night-run posture (ORDER 005 rule 2): PR #91 parked READY against
  main; no merge or auto-merge API calls from this session — the
  owner sweeps in the morning.

## 💡 Session idea

The gate family grows its cheapest member yet. Slice 5 gated behind
TIME, slice 6 behind a SEED BUCKET, slice 7 behind an INPUT VERB —
slice 8 needed NO gate at all: **a render-like slice (audio, and one
day VFX) feeds nothing back into the sim, so zero re-pins comes free
by construction** — *if* two disciplines hold. (1) The decision layer
is pure and mirrored as ever, but (2) — the extractable rule — **the
main.c GLUE model goes INTO the committed mirror, not into session
scratch**: `AudioGlue` in check-brine.py is a line-for-line object
model of the cue ladder/channel countdown/drone flips, and the SAME
object drives both the host story checks and the emulator watch-log
prediction. Slice 7's mirror-of-main.c lived in scratch, so only the
sim was drift-guarded; now a future edit that re-times a cue fails
`check_audio_duel_story` in 2 seconds instead of a 40-minute emulator
diff hunt. Corollary discovered en route: **delta-derived events fire
on state re-init unless you model it** — begin_duel releases the old
water's live rakes, which the ball-count delta reads as one splash on
the restart frame. The model predicted it, the emulator matched it,
so it shipped as a flagged quirk with a pin on it (proof 19), not a
special case in C. Rule: when a delta rule misfires benignly and
deterministically, pin the misfire honestly — a special-case branch
is a second surface to drift.

## Previous-session review

- Session 38 (slice 7, this branch's base): its guard recipe was
  written FOR this slice and every line held — the mailbox extended
  38→46 with words 0-37 frozen (verified by the 394 carried asserts
  re-running unchanged); the scrape cue fires off `groundings`, the
  player-only counter, exactly as its "no reef cue off enemy
  positions" rail demanded; no committed key file was touched, so the
  seed-511/390/1478 derived anchors never wobbled; B stays reserved.
- Its slipped-row rule confirmed a THIRD time, predictively: all 7
  mismatched rows across the three new stories (169,740 values
  diffed) were water-begin/card console-clear frames — 2 in the gale
  story, 2 in the bite story, 3 in the idle loss — and no pin was
  placed on any of them.
- One economy its two-story recorder lesson didn't anticipate: the
  audio slice needed NO recorder work at all — all three proofs replay
  stories already in `proof/*.txt`. Reusing the committed corpus is a
  rung below even the input-verb gate on the cost ladder.

## What this session did

1. Claim + born-red card first (`8937211`), PR #91 opened READY
   immediately after, stacked per ORDER 005 rule 2.
2. **Pure decision layer** (`bw_sim.{h,c}` + mirror, lockstep):
   ambience tiers CALM/BREEZE/GALE/MAW — `bw_amb_tier(maw_up, wind)`,
   the slice-6 wind level as the drone ladder, the Maw (shadow
   included) overriding; one-shot cue table ids 1..10 in priority
   order (cannon, splash, scoop, crack, scrape, wreck, dock chime,
   MAWRISE, bite, sunk) with `{freq, len, duty|NOISE, vol, freq2}`
   rows — `freq2` is the two-note contract (C5→G5 dock chime,
   square-by-contract, the only nonzero row, asserted).
3. **ARM9 glue** (`main.c`): `soundEnable()` + two hardware channels —
   a drone that free-runs between tier flips and cuts on
   title/card/port, and a cue channel (highest id wins a contested
   frame, re-tunes at len/2 on two-note cues). Event detection is
   frame-start deltas (reload jumps = broadsides, hull drops = cracks,
   ball-count drops = splashes, `groundings` = scrapes, wins/maws =
   wrecks, `maw.bit` edge = the bite, state edge = the rattle).
   Telemetry 38→46. Audio writes NOTHING into the sim.
4. **Host mirror** `check-brine.py`: 4 new checks (30 total, ~1.8 s) —
   tables (one two-note row exactly, square-by-contract, monotone
   drone ladder, clamped accessors), exhaustive tier truth table, the
   full fight-cue ladder on the seed-126 anchor (both ships thump,
   wreck on the sink step, 3 scoops, chime note-by-note frame-exact),
   the Maw story + the sinking rattle. Plus the committed `AudioGlue`
   main.c-glue model (the idea above).
5. **Proofs 17-19** (CI `nds-brineward-build` only; Gloamline job and
   the GBA gate untouched; /16→/19 step renames): 17 = the gale gets a
   voice (proof 14's committed story — gale drone 78 Hz, thump/crack/
   splash/wreck/scoop on camera, the chime's C5→G5 re-tune pinned at
   786/800/815, the START breeze water dropping the drone to 62 Hz);
   18 = the Maw is heard (proof 11's story — the drone flips to the
   104 Hz throb THE FRAME the shadow stirs at 1205, MAWRISE on the
   surface frame, the bite outranking its own crack with the drone
   flipping home on the same row); 19 = dying silences the sea (proof
   3's idle loss — the SUNK rattle on the card with the drone at 0,
   the restart re-arming it, the restart-splash quirk pinned
   honestly). **All 19 proofs / 538 asserts green in-container (394
   carried with ZERO re-pins + 144 new); all three stories
   watch-log-diffed first: 43,700 + 68,540 + 57,500 = 169,740 values,
   0 mismatches outside the 7 slipped console-clear rows, none
   pinned.**
6. **Ship**: `dist/brineward.nds` 120,320 B, sha256 `cb78bf51…7590`,
   byte-deterministic (two clean builds identical); PLAYING-BRINEWARD
   gets "The sound" section + refreshed hashes; dist/README row;
   3 proof PNGs (`slice8-audio-{chime,maw,card}.png`; R/B-swap quirk
   as ever). All five recorders re-derive byte-identical (13 files).

## Guard recipe (for the next Brineward slice — saves, roadmap item 7)

- Words 0-45 of the mailbox are now pinned; extend 46→N, never re-map.
- The audio tables are LOAD-BEARING for proofs 17-19: any retune of
  `BW_AMB_ROWS`/`BW_CUE_ROWS` re-pins those proofs' Hz/len literals
  (check_audio_tables + the story checks fail first, loudly). Mix
  changes are owner-taste — land them provable-first as one-constant
  table edits, re-pin the three audio proofs in the same PR, and
  expect ZERO other re-pins (audio still feeds nothing back).
- The cue ladder's id order IS priority — inserting a cue mid-table
  re-numbers everything above it; append new cues, never insert.
- The restart-splash quirk (proof 19, frame 1210 pin) is DELIBERATE —
  a "fix" that gates cues on at-sea states is a re-pin of proofs
  17-19 and a drift surface; don't "clean it up" casually.
- Saves (roadmap item 7): Gloamline slices 9/22-23 already proved the
  whole EEPROM path headlessly INCLUDING the libnds-vs-DeSmuME HOLD-
  bit fix — port `gl_save_*`'s shape (fixed blob, checksum, decode-
  to-zero-on-corrupt, write-only-on-improvement), don't re-derive it.
  Brineward's record candidates: banked gold, tiers, charted band.
- Audio decisions are NOT save material — the drone/cue state is
  power-on-transient by design; a future save slice must not
  serialize slots 38-45.

## Session enders

- `python3 tools/check-brine.py` — 30 checks green (~1.8 s).
- All FIVE recorders re-derive byte-identical to committed (13 key
  files).
- From-source rebuild == committed `dist/brineward.nds` bit-for-bit
  (sha256 `cb78bf51ff693a6d6773dac4ab3193417a9b5e30b907e311be99412
  84f257590`); two clean builds identical.
- All 19 headless proofs + 538 asserts green in-container (the same
  commands CI runs, extracted from rom-builds.yml).
- `rom-builds.yml`: every hunk inside `nds-brineward-build`; YAML
  parses; no assert line of proofs 1-16 touched.
- `python3 bootstrap.py check --strict` — exit 0 (the born-red hold
  cleared by this flip commit).
- Claim `control/claims/brineward-slice-8.md` retired with this flip.
