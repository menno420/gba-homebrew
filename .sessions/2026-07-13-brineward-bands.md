# Session 38 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/brineward-bands`)
- mission: **BRINEWARD arc slice 7 — DANGER BANDS + REEFS.** The next
  roadmap beat after wind in `docs/concepts/brineward-concept.md`
  (loot/gold → port+upgrades → the Maw → wind → **danger bands +
  regions** → audio → saves): "band spawn tables, reefs/islands,
  deepest band = charted-waters score". Authorized by ORDER 005 (owner
  night-run directive — scribed via PR #77, at `control/inbox.md` HEAD
  since the ~01:44Z sweep merge): "Current tracks keep shipping: next
  Gloamline + Brineward slices as open PRs (rule 2)" and rule 2's "do
  not hold work behind unmerged parents; branch from the open head and
  note the base in the PR body" — this branch is cut from
  `claude/brineward-wind` @ `eb3235a` (the UNPUBLISHED slice 6), so
  the PR carries slices 6+7 together; `origin/main` (post-sweep
  `d87f9ad`) was merged in mid-session so the PR diff is clean of the
  merged sibling work (two conflicts resolved: `dist/README.md`
  both-rows — main's Gloamline slice-11 row kept next to the slice-7
  Brineward row; `.substrate/guard-fires.jsonl` union-appended).
- session number 38 claimed (34 = Brineward slice 6, this branch's
  base; 35/36 = coordinator work-loop heartbeats; 37 = the Gloamline
  watch-map sibling, PR #75; the 2026-07-13 night-run siblings ran
  unnumbered slug cards).
- **📊 Model:** fable-5 (family-level self-report from this session's
  own harness/environment banner, per ORDER 003)
- night-run posture (ORDER 005): PR opens READY against main; no merge
  or auto-merge API calls from this session — the installed
  auto-merge-enabler workflow (PR #76) does whatever it does
  server-side, and the owner sweeps in the morning.

## 💡 Session idea

Slice 5 gated its new mechanic behind TIME (600 quiet frames), slice 6
behind a SEED BUCKET (the calm-anchor salt). Slice 7 needed neither
scan nor salt: **gate the new mechanic behind a fresh input verb that
no committed story presses, plus band tables whose row 0 is the legacy
constants.** SELECT appears in no committed key file, and
`bw_water_init(d, seed, 0)` is asserted STATE-IDENTICAL to
`bw_duel_init(d, seed)` over 512 seeds — so every slice-2..6 route and
pin carries with zero re-pins *by construction*, no 19k-candidate salt
scan required. Rule extracted: **the input-verb gate is the cheapest
member of the gate family — when the next mechanic can hang off a verb
the corpus never speaks, take that gate first and pin the row-0
identity as its rail.** Second find, for the recorder kit: when one
anchor cannot carry two continuations through every alignment shift
(the deeper duel is chaos-sensitive: 5 of 520 gaps carry the bank
story, 52 carry the reef story, intersection EMPTY — measured), split
the stories onto their own SELECT presses instead of weakening the
shift rule: two 2-file stories beat one fragile 3-file story.

## Previous-session review

- Session 34 (slice 6, this branch's base): its guard recipe was this
  session's spec and every rule held: words 0-33 stayed pinned
  (extended 34→38: 34 band, 35 charted-best, 36 reefs, 37 groundings);
  the protected anchor seeds 116/117/118/119/126/127/558/728 + wind
  anchors 128/161 were never touched (no salt, no wind-table change);
  the new constants satisfy the check_wind_tables escape-rail
  inequalities trivially (no speed constant added; band weather is
  min-clamped at the PROVEN gale); the session-34 pin method was
  applied wholesale — mirror-of-main.c model first (validated against
  proof 4's committed pins exactly), full-story watch-log diffs
  (140,562 values, 0 mismatches), then every pin a row lookup.
- Session 34's slipped-row rule confirmed AGAIN, predictively: the
  only slipped rows in both new stories were the water-begin
  console-clear frames (tool 128/392/513/1480 — every begin_duel), and
  no pin was placed on or beside them.
- The SELECT frame-law assumption (same keysDown edge as the proof-6
  START) was verified empirically before any pin: the model's water-2
  law (press [C,C+5) ⇒ seed2 = C-2, step j at tool seed2+3+j) matched
  the emulator bit-for-bit across both stories.

## What this session did

1. Heartbeat first (born-red card `39557ad`), claim file second
   (`15b4415`); overlap check at HEAD: no live claim or open PR on
   Brineward slice 6/7 (PRs #75/#77/#78/#79 all elsewhere).
2. **Pure sim** (`bw_sim.{h,c}` + mirror, lockstep): `BwDuel` gains
   `band`, `groundings`, `reefs[5]`; `bw_water_init(d, seed, band)` =
   `bw_duel_init` + the band tables — enemy hull {100,130,160}, reload
   {150,120,100}, crate value {5,12,25} (the doc's ~5g→~25g shape),
   maw crates {15,30,50}, reef count {0,3,5}, weather +1 level per
   band min-clamped at gale; row 0 = the legacy constants, asserted.
   Reefs: pure f(seed) positions (hash + ≤8 rehashes clear of the
   anchorage 28 px and pier harbor 40 px, then a provably-clear
   fallback shelf); `bw_reefs_step` scrapes the PLAYER only
   (decide-and-flag: the rum-runners and the Maw know these waters)
   for 15 hull once per contact (latch), groundings can sink.
3. **The verb** (`main.c`): SELECT in the salvage water puts out one
   band deeper (same carry rules as START; START now re-sails the SAME
   band; title/sunk sail band 0); `Score.best_band` = the
   charted-waters score, on the ledger (`band N water · charted M`)
   and the sunk card; 16x16 code-authored reef rock sprites (palette
   13/14); mailbox 34→38.
4. **Host mirror** `check-brine.py`: 5 new checks (26 total, ~4 s) —
   band tables (512-seed band-0 state identity, monotone
   harder+richer, weather min-clamp, band clamp), reef spawns (4096
   seeds x 3 bands: counts exact, in-sea, both clearance rails,
   deterministic, shelf rails), reef grounding (scrape/latch/re-arm/
   sink frame-exact, enemy-on-rock untouched, salvage scrapes too),
   band duels (idle sunk everywhere; tier-0 policy wins all 8 probed
   band-1 waters; band 2 wins 12/16 and every loss — seeds 0/3/6/10 —
   flips to a win with cannons II: the deeps are priced in gold, not
   walled), band containment (8000 adversarial band-2 frames, 4 real
   scrapes taken, everything bounded). Port fingerprint extended:
   `bw_port_buy` provably never touches band/reefs/groundings.
5. **Recorder** `record-bands.py`: two deeper stories on the committed
   seed-126 anchor, each with its own scanned drift gap → SELECT press
   (bank story: seed 511 — deeper duel won at hull 77, 3x12g scooped,
   36g banked, no grounding, Maw quiet + 30-frame pin margin; reef
   story: seed 390 — won untouched, then a deliberate grounding: ONE
   15-hull toll across a 33-frame contact, latch re-armed in clear
   water, second crossing tolls again, 100→85→70). Four key files;
   water-2 skeletons hold at every shift in [-6,+6]; byte-identical
   re-derive asserted in CI alongside the other four recorders.
6. **Proofs 15/16** (CI `nds-brineward-build` only; job at line 885,
   every hunk ≥ 907; Gloamline's job and the GBA gate untouched):
   15 = SELECT sails deeper — band word 0→1, charted word flips, 3
   rocks on camera, t[10]=130 heavier foe, worsened weather word,
   hull carried through the verb, 12g-crate scoop arithmetic
   (t[17] 12→36), 36g banked, t[37]=0, and the final START keeps the
   depth (seed-1478 water still band 1). 16 = the rocks are real —
   t[37] 0→1 with hull 100→85 on the pinned frame, THREE latch pins
   (mid-contact and clear-water), 1→2 with 85→70 on re-entry, Maw
   quiet end to end. **All 16 proofs / 394 asserts green in-container
   (318 carried with ZERO re-pins + 76 new); both new stories
   watch-log-diffed first: 59,052 + 81,510 values, 0 mismatches.**
7. **Ship**: `dist/brineward.nds` 118,272 B, sha256 `91ac3eff…531f`,
   byte-deterministic (two clean builds identical pre-merge; rebuild
   == dist bit-for-bit re-verified on the post-merge tree);
   PLAYING-BRINEWARD rewritten for the deeps; dist/README Brineward
   row; 3 proof PNGs (`slice7-bands-{duel,banked,scrape}.png`;
   R/B-swap quirk as ever).

## Guard recipe (for the next Brineward slice — audio, roadmap item 6)

- Words 0-37 of the mailbox are now pinned; extend 38→N, never re-map.
- New protected anchors: the band stories ride seed 126 (already
  protected) and derive seeds 511/390/1478 from the SELECT/START press
  frames in `bands-*.txt` — a change to ANY committed key file's
  timing re-times them (the recorder re-derive fails first).
- The band tables' row 0 is LOAD-BEARING (check_band_tables fails
  loudly if any row-0 entry drifts from its legacy constant); the
  weather worsening must stay min-clamped at BW_WIND_GALE or the
  slice-6 escape-rail proof no longer covers the deeps.
- Reefs are player-only ON PURPOSE (flagged in PLAYING + h-file); an
  audio slice must not add a reef cue that fires off enemy positions.
- SELECT is now spoken for (deeper). B remains the reserved verb.
- The two-story split is the recorder pattern when one anchor can't
  carry two continuations through the shift window — don't force a
  third continuation onto an existing anchor; give it its own press.

## Session enders

- `python3 tools/check-brine.py` — 26 checks green (~4 s), post-merge.
- All FIVE recorders re-derive byte-identical to committed (13 key
  files), post-merge.
- From-source rebuild == committed `dist/brineward.nds` bit-for-bit on
  the post-merge tree (sha256 `91ac3eff823593c6ef5f0f743f696562ba13ec
  76111d84afcd44cc401fe0531f`); two clean builds identical.
- All 16 headless proofs + 394 asserts green in-container (the same
  commands CI runs, extracted from rom-builds.yml); proofs 15/16
  re-run green against the post-merge tool (main's watch-map slice
  changed `tools/nds-headless-check.py`).
- `rom-builds.yml`: every hunk inside `nds-brineward-build`; YAML
  valid post-merge.
- Zero Gloamline/Lumen-Drift/web/mobile files touched by this slice's
  commits; `control/` untouched except this session's own claim file
  (created at start, deleted at this close per claims README).
- `python3 bootstrap.py check --strict` exit 0 (one advisory
  claims-format nag on the merged-in `order-005-scribe.md` claim — not
  this session's file, left for its owner/sweep).
- Card flipped complete as the deliberate last step.
