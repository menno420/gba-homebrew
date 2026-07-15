# Session — Wickroad growth cut 5: AUDIO

> **Status:** `complete`

- date: 2026-07-15 (branch `claude/wickroad-audio`, PR #146; started
  10:26Z, flipped 10:38Z — both from `date -u` at write time)
- mission: **Wickroad growth cut 5 — "Audio"** per
  `games/wickroad/CONCEPT.md`: synthesized cues via the proven
  `generate_audio.py` pipeline (coin chink, dawn bell, the pass-closing
  wind), decisions hook-counted per the house method. The LAST named
  cut — the named growth path is now COMPLETE. One contained slice on
  the v0.5 wider-map build (PR #145): stdlib-only deterministic
  generator + committed WAVs, `AUDIO := audio`, three play hooks on
  events the sim already decides, zero new RNG draws, words 0-47 of
  the mailbox byte-unchanged, new proof P8, dist v0.6.
- **📊 Model:** Fable-family · medium · feature build — wickroad growth cut 5
- landing posture: PR #146 opened READY at the born-red commit; no
  merge/approve/auto-merge calls from this session — the server-side
  enabler decides on green.

## What shipped

1. Born-red gate (commit d643790): this card `in-progress` +
   `control/claims/claude-wickroad-audio.md` (written by
   `bootstrap.py claim`), PR #146 opened READY immediately; claim
   retired at the heartbeat (51af80f) per claims README rule 4.
2. **The cues** (commit 5cc8e8d, `games/wickroad/audio/`):
   `generate_audio.py` on the Lumen Drift/Deepcast/Courier/Shoal house
   pattern — stdlib-only, byte-exact deterministic (pure math, an
   explicit LCG for the wind's noise, no `random`, no timestamps;
   regenerated three times in-session, identical hashes), mono 16-bit
   PCM @ 16384 Hz. `wr_coin` (150 ms, two detuned C7-region partials +
   an octave whisper, fast decay — built to layer on a nine-buy
   streak), `wr_dawn` (750 ms, one struck bell: hum/prime/tierce at
   1.0/2.756/5.404 over C5, high partials dying first), `wr_wind`
   (1.1 s, LCG noise through a one-pole lowpass whose cutoff gusts on
   a slow sine, under a rise-and-die swell). WAV sha256s: coin
   `25ee2c25...4150`, dawn `9ea71198...cf39`, wind `276e9087...089a`.
3. **The hooks** (`games/wickroad/src/main.cpp`): three event classes
   the sim already decides — the coin where gold changes hands on a
   TRADE (market buy, market sell, pact delivery payment; tolls,
   lodging, guard fees and the mule fair are costs, not trades —
   deliberately silent), the bell inside `dawn()` when a day rolls
   over with the road still open, the wind on the exact pass-closing
   dawn. Zero new RNG draws; nothing feeds back into the sim.
4. **Telemetry**: `wr_telemetry` 48 -> 52 — words 48/49/50 cumulative
   play-call counters for coin/bell/wind, word 51 their total;
   boot-cumulative like the frame word (deliberately NOT reset by a
   START restart — P8 pins the survival); words 0-47 byte-unchanged.
5. **P1-P7 carried verbatim on the first post-audio run** — the Shoal
   rung-5 clock re-base (enabling the audio engine tipped its spike
   frame across a vblank) was the named risk; measured absent here:
   zero re-pins, the dawn frames held their committed budget.
6. **Proof P8 THE AUDIO** (`games/wickroad/proofs.sh`): one buy + one
   sell on day 1 step the coin to exactly 2 on their key edges (gold
   60 -> 51 -> 60 on the committed tallow 9), thirty waits ring one
   bell per rolled day (10 at day 11, 29 at the close), the 30th wait
   rolls day 31 and fires the WIND exactly once (state 3, gold 30
   pinning all 30 lodgings), a START restart provably KEEPS the
   counters, and the maxmod mixer-memory nonzero watch pins the
   voicing (0 on the silent title, >0 under the coin and the wind,
   0 again once the wind dies — one-shots, nothing loops). Suite
   totals P1-P8: **755 watch + 72 text assertion passes**, exit 0;
   P2-P8 each run twice, watch-logs byte-identical; the whole suite
   run twice end-to-end, CSVs byte-identical across runs. Honest
   limit written into proofs.sh AND the PR: only play-call decisions
   and mixer-memory activity are asserted — audible output (mix,
   timbre, DAC) is owner-playtest territory, listed in the PR.
7. **Ship** (commit 5cc8e8d): `dist/wickroad.gba` v0.6, 159,484 B,
   sha256 `92df5c919f0dda62b108d0b94dc5bfb7c703ab300e71a5eaaa1ead8
   aea110947`, two clean builds byte-identical, suite re-run green at
   the dist bytes; dist/README row updated; CONCEPT.md cut 5 marked
   **SERVED** (named growth path COMPLETE) and its stale
   `wr_telemetry[16]` Determinism reference fixed to the current 52;
   docs/current-state.md Wickroad row -> v0.6, growth cuts: none open
   (close-out commit 51af80f).
8. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold); re-run
   post-flip green. The check's auto-appended
   `.substrate/guard-fires.jsonl` delta is NOT committed (dispatch
   rail: no writes under `.substrate/`, deny-wins — the #145
   precedent).

## 💡 Session idea

**Measure the platform tax BEFORE re-deriving pins — run the old
committed suite against the new build first, and let the pass/fail
pattern tell you whether the change is free.** Shoal's rung-5 card
warned that merely ENABLING the audio engine shifts frame timing
(its boot spike crossed one more vblank and every post-transition pin
moved +1). The cheap experiment this session: build with `AUDIO :=
audio` and the hooks in, then run the UNTOUCHED P1-P7 suite before
writing a single new assert. It passed verbatim — so the maxmod
per-frame mixing cost provably fits inside wickroad's committed dawn
budget, and the whole "re-base every pin" contingency evaporated as a
non-fact. The general move: when a committed interface might be
perturbed by an environmental change (a new engine, a compiler bump),
the existing proof suite IS the measurement instrument — one run
classifies the change as free or taxed, before any new work
contaminates the signal.

## Previous-session review

- Prior lane slice: `.sessions/2026-07-15-wickroad-wider-map.md` (PR
  #145, growth cut 4) — its 💡 ("a committed RNG stream is append-only
  — treat the draw ORDER like a wire format") held up as stated:
  this cut consumed zero draws, but the same wire-format discipline
  transferred directly to the MAILBOX — words 48-51 were appended
  behind the frozen 0-47 exactly the way cut 4 appended 40-47 behind
  0-39, and P1-P7 passed with zero re-pins, twice in a row now. Its
  harness-reach rule ("check what the assert tooling can READ before
  pinning what the screen shows") also paid: P8 pins voicing through
  the mixer-memory nonzero watch (the Shoal/Courier house method)
  instead of trying to assert sound from screenshots. One honest gap
  in the cut-4 card: it named the audio cut as simply "one named
  growth cut open" with no warning that enabling the audio ENGINE is
  itself a timing risk to the committed pins — the warning lives in
  Shoal's proofs.sh header, a different game's file, and only prior
  knowledge of that card surfaced it. The widened rule: when a card
  names the NEXT cut, it should also name the known platform tax the
  next session will pay (or measure), not just the feature.
