# Tiltstone — concept

> **Status:** `reference`

## Pitch

An 8×8 cavern full of loose gems, dead-weight boulders and jutting rock.
Your only verb: turn the whole cavern 90°. Everything falls. Three gems of
a kind touching after the dust settles collect — cascades chain. Hit the
gem quota before your rotation budget runs out. Every seed is **provably
winnable**: the generator refuses to ship a layout its own solver can't
beat, so a stuck player is out of ideas, never out of luck.

## Genre

Turn-based gravity puzzle (rotate-the-world family — *Fidel Dungeon
Rescue*'s discrete deliberation meets the falling-and-matching physics of
*Puzzle Bobble*/match-3, but with ONE global input and zero time pressure).
Deliberately distinct from tonight's other breadth slices: Undertow (#79)
is reflex arcade, the mobile foundation (#78) is a platform slice.

## Core loop

Read the cavern → predict where everything lands under a quarter-turn →
rotate CW or CCW → watch the fall, the collect, maybe a chain → count
remaining turns against remaining quota → repeat. Win: quota met → next,
slightly deeper level (same seed, level chain). Lose: budget spent → same
cavern again, one keypress away. A full level is 3–10 decisions; a session
is one bus stop or ten.

## Platform

Web browser (the owner's arcade target): static `index.html` + two JS
files, zero dependencies, works from `file://`. Turn-based means no
animation loop, no timing sensitivity, and the whole rule set lives in a
pure DOM-free engine module the page merely renders — the same file is the
headless proof surface in Node/CI.

## Daily seed

The seed defaults to the UTC date (`2026-07-13 → 20260713`) — everyone in
the world gets the same cavern chain each day, and `?seed=N` / the seed box
is free-play. Determinism is total: same seed + same rotations = same game.

## Sellability (honest guess)

Near-zero as a paid standalone in this state — one-mechanic browser
puzzles are a crowded free tier (comparable itch.io gravity/rotation
puzzles: hundreds of free entries, the rare paid ones at $2–3 with tiny
sales). Realistic value: solid free arcade filler with unusually good
"daily challenge" bones (deterministic, provably-solvable, shareable
seeds — the Wordle-shaped hook). A paid path would need: handcrafted level
packs on top of the generator, undo + move-count par scoring, juice
(fall/merge animation, sound), and mobile touch — at which point $2–4 on
itch/mobile is defensible but unproven. Venture Lab marker: NOT yet; revisit
if the daily hook shows organic pull.

## Growth path

1. **Juice** — ✅ SHIPPED (slice 3, session 44): the engine grew a pure
   trace surface (`settleMoves`/`resolveTrace` — byte-identical results to
   `settle`/`resolve`, asserted), and the shell replays it: board sweep,
   gems easing down their columns, collect groups popping per chain, all
   cancelled by new input and skipped under `prefers-reduced-motion`.
   Synth audio via `juice.js` (WebAudio oscillator sweeps only — no sampled
   audio, ever): rotate/land/collect (chain-rising pitch)/win/lose/undo,
   mute persisted; every cue lands in an honest log the smoke pins.
2. **Par scoring** — ✅ SHIPPED (slice 2, session 43): pure `par(level)` /
   `grade(used, par)` in the engine (PERFECT / GREAT / GOOD / CLEARED), PAR
   on the HUD, graded win card. Par is the solver's shortest line —
   BFS-minimality is asserted in the smoke, so PERFECT is exactly optimal.
3. **Undo** — ✅ SHIPPED (slice 2, session 43): history stack in the shell
   (states are immutable snapshots), U key/button, works from a BURIED card
   (won cards stay frozen); undo count rides the win card so grades stay
   honest.
4. **New cell types** — ✅ SHIPPED (slice 4, session 45): locked gems
   (fall like dead weight, never group; a collect popping adjacent frees a
   normal gem — cascades chain through it), ice (never merges; slips off
   piles, left before right, and keeps falling), one-way grates (fixed like
   walls, porous exactly downward, arrow turns with the cavern) — each a
   pure `settle()`/`resolve()` extension in the engine, entering the
   generator at level 5+ (levels 1–4 draw the identical RNG stream, so all
   prior pins hold). Rendered as caged gems / glinting ice / arrowed
   grates; the juice layer gained an `unlock` cue on the same honest log.
5. **Level packs** — ✅ SHIPPED (slice 5, session 46): pure
   `difficulty(level)` rates what the solver already measured (par
   dominates; low slack — best minus quota — breaks ties), deterministic
   `curatePack(def)` scans a seed window and keeps the hardest, and the
   engine ships the PINNED result (`PACKS`: GRANITE GAUNTLET, six
   base-rule caverns par 5–7; DEEP CUTS, six level-4 caverns par 7–8) —
   the smoke re-runs the curation and asserts the pin byte-identical, so
   the curated data is never hand-editable. Shell: pack picker, staged
   progression (N advances, R restarts the stage), `?pack=&stage=` deep
   links, guarded per-pack progress persistence.
6. **Daily leaderboard** needs a backend — out of static-hosting scope;
   local streak tracking works today.

## Arc 2 — the shareable daily

The five arc-1 ender cards left a *designed* second arc: turn total determinism
into the daily/social hook § Sellability names as the real value. Five deduped,
backend-free cuts (full plan + proof strategies: `docs/arcs/TILTSTONE.md`):

1. **Share your line** — ✅ SHIPPED (arc 2, cut 1): `?seed=N&level=L&replay=RRLR`
   — a share URL of the player's OWN rotation line. `(seed, levelIndex, line)`
   fully describes a run (determinism is total), so the existing `resolveTrace`
   replays it as a spectated run for free; the solver's line stays hidden.
   Pure `encodeShare`/`decodeShare`/`spectate` in the engine (additive), a Share
   button + `?replay=` spectate mode in the shell, proven in `smoke.mjs` § 14.
2. **Hints from the solver** — the stored winning line's next move, spend-gated.
3. **Undo×par curation** — `(undos, used − par)` as a deceptiveness rating.
4. **Mechanic fingerprints** — neutralized-BFS par deltas → which cell class
   carries a seed ("this daily NEEDS the grate").
5. **The monotone ramp** — an optional difficulty floor on `generateLevel` so the
   daily chain is a provably non-decreasing ramp.
