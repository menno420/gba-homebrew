# Session 50 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/shoal-audio`, 21:58Z → 22:52Z)
- mission: **SHOAL growth rung 5 — AUDIO, the LAST named cut.** The
  final item of `games/shoal/CONCEPT.md` @ main `751341c`
  ("Deliberately still cut: audio"). **With this rung the committed
  concept is FULLY BUILT** — every named item of the ORDER-001 pitch
  and its S1-S3 scope ladder is on cartridge behind committed
  headless proofs.
- dedup at claim time: no shoal claim, no open Shoal PR; inbox at
  HEAD unchanged — nothing new unserved.
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #103 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.

## 💡 Session idea

**An audio engine is never render-only on a machine this small — its
IDLE cost is a per-frame tax, and the tax lands exactly on the frame
you already couldn't afford.** The prediction ("audio is render-like,
zero sim cost") was half right: the five cues themselves are pure
decisions, and the worst gameplay frame moved only 3369 → 3371
(82.30% → 82.35%). But merely ENABLING maxmod added a constant
~1-1.5% every frame — and the one frame in Shoal that runs >100%
(the 50-sprite spawn spike at run start) crossed its next vblank
boundary in every water at once. The interesting part is WHERE it
didn't: card→run transitions (same 50-sprite generation!) did not
shift, because their frames start from a lighter base — so the
boot-lag map went calm 1→2, hungry 2→3, gated/levels 2→3 for
TITLE-starts only, and the P7 chain's later segments kept their old
clocks. Rules extracted: (1) **price an audio engine as BASE LOAD,
not per-cue cost** — measure the worst frame with the mixer merely
present before designing the cue set; (2) **defer cues off spike
frames by a one-frame pending flag** (the start cue now fires on the
first herding frame — a spike frame must never gain even a voice
start); (3) the #102 ±1-row-offset method scales to a suite-wide
event: re-base every clock, carry every literal verbatim, and let
the one residual row (the transition frame itself) name the
mechanism.

## Previous-session review

- Session 49 (Shoal levels): its guard recipe scoped this session
  almost line by line — "audio: expect CPU-exact drift only, but
  re-measure the CHAIN, not the calm gate" (done: the chain was the
  before/after instrument, and the L4 worst frame was the binding
  number), and its boot-lag trap warning ("ANY code in the
  transition path can flip one of these") fired exactly as
  documented, with its prescribed remedy applied suite-wide.
- One prediction it got subtly wrong, worth recording: it filed the
  trap under "code in the input-transition or first-draw path" —
  the actual trigger was a GLOBAL constant cost (the mixer's
  per-frame tax), no transition code touched at all. The trap is
  about the spike frame's total budget, not about who added the
  cycles.
- Its P7-chain design (one deterministic run, four levels) paid off
  here: one instrument measured the whole game's budget before and
  after, and the audio-decision ledger (136 banked / 15 eaten /
  4 wins / 5 starts) could be pinned across all four levels in the
  same proof.

## What this session did

1. Claim + born-red card first (`b9b86d7`), PR #103 opened READY
   immediately after, cut from main @ `751341c`.
2. **The audio set** (`games/shoal/audio/generate_audio.py` — the
   Lumen Drift pipeline: stdlib-only, byte-exact regeneration
   verified, no samples ever; maxmod soundbank via `AUDIO := audio`):
   `sh_start` (a low water bloom, A3→E4), `sh_save` (one tiny bright
   plink per banking frame — it layers as the school streams home),
   `sh_eaten` (a dull wet thud, pitch-dropping into dark noise),
   `sh_win` (a warm three-note resolve over a tide swell), `sh_lose`
   (a falling minor pair into a cold wash). The water's palette:
   sine-heavy, soft attacks — herding smoke, not shooting.
3. **Pure decision layer**: cues fire from events the sim already
   computed (run start — deferred one frame off the spawn spike, the
   💡 above; banked/eaten counts from the existing return values;
   win/lose on the state transitions). B toggles MUTE (gates play()
   ONLY — the counters always bump, so the decision layer stays
   provable muted or not; HUD shows a MUTE tag; title gains
   "B: MUTE"). Nothing writes back into the sim.
4. **Evidence**: every trigger rides `gl_audio_hook` (slots: starts /
   fish banked cumulative / fish eaten / wins / losses / mute flag)
   and the maxmod mixer-memory nonzero watch proves actual voicing
   at the win and lose cards (silent title pinned at 0). The P7
   campaign pins the full ledger: banked 40→76→108→136, eaten
   0→3→8→15, wins 1..4, starts 1..5 across the four levels + wrap.
5. **Budget, before → after** (P7 chain, per-level herding frames):
   L1 37.3%/75.1% → 37.4%/75.1% · L2 33.4%/75.5% → 33.8%/76.0% ·
   L3 37.1%/82.1% → 37.2%/82.2% · **L4 46.5%/82.30% (3369) →
   46.7%/82.35% (3371)**. The `t[5] < 4096` rail holds everywhere —
   **no scope-down was needed** (the mixer's idle tax ≈ +0.1-0.5pp
   on means; cue voicing never lands on the worst frames).
6. **The clock re-base** (zero game-state re-pins): the mixer's
   constant cost tipped the TITLE→run spawn spike +1 vblank in every
   water; card→run transitions measured UNCHANGED. Every post-press
   span and post-transition assert frame in P2-P7 moved +1 (P7's
   later segments kept their old clocks — measured, not assumed,
   after a blanket +1 wrongly shifted seg2 and the empirical rf0
   scan corrected it); **every game-state literal carried verbatim**.
   CPU exacts re-derived: P2 2159/2853, P3 3126, P4 2989, P5 3019,
   P6 3187, P7 chain-max 3371.
7. **Proofs**: **7 proofs / 179 asserts (P1 14 · P2 23 · P3 15 ·
   P4 36 · P5 9 · P6 19 · P7 63)**, all green; P4/P6/P7 run-twice
   byte-identical. New audio asserts: 33 (the decision ledger, the
   mute flag both states, mixer voicing at win/lose, title silence).
8. **Ship**: `dist/shoal.gba` **v1.0 CONCEPT COMPLETE** 155,464 B
   (the soundbank is ~27KB of it), sha256 `29a67ab4…6d86`,
   byte-deterministic (two clean builds identical; the suite re-run
   green against the exact dist bytes); WAV regeneration byte-stable;
   CONCEPT.md closes the ladder ("FULLY BUILT"); dist/README v1.0
   row; heartbeat appended with live ts (one follow-up commit
   corrected the assert count 180→179 after recounting from the log
   — numbers come from measurements, not from arithmetic in prose).

## Guard recipe (Shoal is CONCEPT-COMPLETE — for whoever touches it next)

- There is no next named cut. Anything further (sprite art, more
  levels, sellability packaging per the CONCEPT's honest guess) is
  NEW scope: one-page concept first, owner-routed.
- The budget map at v1.0: worst frame 82.35% (L4, two hunters + two
  walls + pushed school + mixer base load). The remaining ~17.6% is
  the whole future budget — sprite art (replacing text glyphs)
  changes render cost, re-measure the CHAIN first.
- Boot-lag map at v1.0: TITLE→run = calm 2 / hungry 3 / gated+levels
  3; card→run = unchanged from v0.5. The suite's clocks encode
  these; the ±1 method + empirical rf0 scan (grep t[15]==0 resets)
  is the repair kit, and per-segment shifts must be MEASURED, never
  assumed cumulative.
- The audio decision layer is the pattern for any new cue: event →
  hook slot (cumulative) → play() gated by mute only — and never
  fire a cue on a frame that already skips vblanks.
- `games/shoal/proofs.sh` is the single oracle: 7 proofs cover every
  water, every level, win/loss/idle, budget rails, determinism
  run-twice, and the audio ledger. Run it before AND after any
  change; it takes ~4 minutes.

## Session enders

- 7 headless proofs / 179 asserts green via the committed suite;
  P4, P6, and the P7 four-level chain each run twice back-to-back
  byte-identical; every game-state literal carried verbatim through
  the suite-wide clock re-base (cross-ROM +1-offset diffs vs the
  rebuilt v0.5 dist: 1 residual row per water — the transition frame
  itself).
- From-source rebuild == committed `dist/shoal.gba` bit-for-bit
  (sha256 `29a67ab4b5891c9133479de23781ac011ed833ba2f72776259bf2df2
  193b6d86`); two clean builds identical; the suite re-run against
  the dist copy itself; `audio/generate_audio.py` re-run leaves the
  committed WAVs byte-identical.
- IWRAM symbols map-verified unmoved (0x03000000 / 0x030006d8 /
  0x03000a30); `gl_audio_hook` and the maxmod mixing buffer resolve
  in the shipped ELF (0x0300292c / 0x03002a0c).
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/shoal-audio.md` retired with this flip.
