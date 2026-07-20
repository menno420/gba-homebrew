# Pre-plan — A1 Tinderhand (GBA press-your-luck card game)

**This document does NOT select this option. The owner's letter choice still
governs (see `docs/NEXT-MENU-2026-07-15.md`). This pre-plan only makes the
option instantly executable if picked.**

> **Status:** `reference` — an executable slice ladder for menu option **A1**,
> written 2026-07-20 (main `dd70656`). It is a PLAN, not a build: no
> `games/tinderhand*` exists yet (UNBUILT per the 2026-07-19 audit,
> `docs/NEXT-MENU-2026-07-15.md` § REMAINING REAL CHOICES). Facts pinned at
> `dd70656`; `docs/current-state.md` wins as the repo moves.

## Why this pre-plan exists (and B1 got none)

The 2026-07-19 menu audit leaves A1 as one of only two genuine open choices.
Menu A1 and OVERNIGHT NEW-01 (`docs/planning/OVERNIGHT-MENU-2026-07-16.md`
lines 491–496) give a rich *pitch* but stop at "**~6 slices** (Shoal/Courier
precedent)" — a slice **count**, not an enumerated ladder. Contrast menu B1,
which names its five cuts, so a B1 pre-plan was correctly SKIPPED as a
restatement (`.sessions/2026-07-19-truth-refresh-b1-preplan.md`, #207). A1 is
not slice-granular, so this pre-plan supplies the missing ladder.

## Design intent

Tinderhand is a single-verb nerve game: the whole title is *quit while you're
warm*. See the full pitch in menu A1 (`docs/NEXT-MENU-2026-07-15.md` § A) and
OVERNIGHT NEW-01 — not restated here. In one paragraph for orientation: you
flip cards from a hearth deck (kindling stokes the fire, ash smothers it) and
choose **BANK** or **STOKE** after every flip; bank too early and the cold
wins, stoke once too often and the hand busts. Between the ten nights of
rising cold you spend banked warmth on **deck-thinning charms**. The deck order
is a pure `card_of(seed, night, index)` schedule with **zero runtime RNG** (the
Gloamline idiom, `games/gloamline-nds/source/gl_sim.h`), so every proof is a
scripted route with a pinned outcome, exactly as Wickroad commits its route
suite (`games/wickroad/proofs.sh`). It fits the GBA: tiny state, a fixed seed,
committed headless proofs, SRAM best via `gl_save.h`, maxmod audio watches
(`docs/capabilities.md`). It is the first card game on a roster of eleven, and
its determinism makes it fully CI-provable — no frame-timing luck to chase.

## Ordered slice ladder (~6 slices, dependency-ordered)

Each slice is **one born-red card PR** (card `in-progress` → flip to
`complete` as the last commit), landing on green `ROM builds` via
`auto-merge-enabler.yml`. Proof surface for a GBA ROM game: a stdlib-only
**host mirror** (`tools/check-*.py` mold, run in `rom-builds.yml` before the
build so a logic regression fails fast) + the **ROM build** itself
(`rom-builds.yml`, the one required <60s check) + **headless-boot** (mGBA
screenshot, `headless-boot.yml`, `workflow_dispatch` — the heavy proof) where a
screen surface is coverable + a committed **`proofs.sh`** scripted-route suite
in the Wickroad mold.

### Slice 1 — skeleton + the pure deck function
- **Scope:** `games/tinderhand/` skeleton (Makefile in the Butano/GBA mold),
  boot → title screen (hook line + verb help), and the deterministic deck
  kernel `card_of(seed, night, index)` returning kindling/ash values with zero
  runtime RNG. No player loop yet.
- **Proof:** host mirror `tools/check-tinder.py` (stdlib-only) asserting the
  deck schedule is a pure function of `(seed, night, index)` and byte-identical
  across two runs — moves in lockstep with the C deck function (the
  `tools/check-cave.py` mirror rule). ROM build (`rom-builds.yml`).
  headless-boot title screenshot non-blank (`headless-boot.yml`).

### Slice 2 — the flip / BANK / STOKE loop (one night)
- **Scope:** the core press-your-luck verb for a single night — flip the next
  scheduled card, apply kindling/ash to the fire gauge, and let the player
  **BANK** (lock the round's warmth) or **STOKE** (flip again, risking a bust).
  Bust and end-of-round card text.
- **Proof:** committed `games/tinderhand/proofs.sh` (Wickroad mold) with the
  first pinned routes: **P1** boot/title, **P2** a scripted STOKE-to-bust route
  (every flip's card/fire/state pinned exact), **P3** a BANK-safe route; each
  route **run twice → byte-identical watch-logs** (the determinism contract).
  ROM build. headless-boot screenshot of the hand state.

### Slice 3 — ten nights of rising cold
- **Scope:** the night structure and the escalating cold threshold (each night
  demands more banked warmth to survive; night 10 is the win). Loss when the
  cold beats your bank; win card on surviving night 10.
- **Proof:** host mirror extended to the cold curve (`check-tinder.py` asserts
  the per-night threshold schedule) + a scripted **10-night win route** pinned
  end-to-end in `proofs.sh` (P4), run twice byte-identical. ROM build.
  headless-boot of the win card.

### Slice 4 — the between-round charm shop
- **Scope:** spend banked warmth between nights on **deck-thinning charms**
  (remove an ash card, upgrade a kindling, etc.); the buy economy and its
  effect on the next night's deck.
- **Proof:** host mirror of each charm's effect on the deck composition +
  a scripted **buy route** in `proofs.sh` (P5) proving the purchased charm
  changes exactly the pinned deck slots. ROM build. headless-boot of the shop
  screen.

### Slice 5 — SRAM best-night persistence
- **Scope:** persist the best night reached / best banked total to SRAM
  (`gl_save.h` mold) and surface it on the title screen; survive power-cycle.
- **Proof:** `proofs.sh` SRAM route (P6) — play a route, save, reload,
  assert the best survives (the `gl_save.h` battery-in/out pattern) — plus a
  host-mirror check of the save encoding. ROM build. (No new screen logic that
  headless-boot must cover beyond the title best line.)

### Slice 6 — maxmod audio + polish → v1.0 (arc closer)
- **Scope:** maxmod audio cues (stoke crackle, bust, night-survive sting),
  final polish, and **commit `dist/tinderhand.gba`** with sha256 provenance
  (the Wickroad/Lumen-Drift binary-policy mold) + a `dist/README.md` row.
- **Proof:** the full `proofs.sh` suite green (all routes P1–P6 + an audio
  watch asserting the cue log per route), ROM build, a final headless-boot
  screenshot. `dist/tinderhand.gba` sha256 matches the `rom-builds.yml` run's
  `rom-sha256.txt` (the #206/#209 provenance discipline).

## Risks / rails

- **Track-A isolation (⚠):** PUBLIC repo, original gba IP only — Tinderhand is
  an original concept; no Track-B / Nintendo / pokemon content.
- **Binary policy:** A1 is a GBA ROM game — the **arc-closer slice commits
  `dist/tinderhand.gba`** with sha256 provenance lifted from the CI
  `rom-builds.yml` run (never a container-local build), with a `dist/README.md`
  provenance row (the #206/#209 mold). Intermediate slices do NOT commit a ROM.
- **Substrate-gate born-red flow:** every slice is one born-red card PR (card
  `in-progress` → flip `complete` last commit); lands on green `ROM builds` via
  `auto-merge-enabler.yml`. The pre-existing non-required `substrate-gate`
  doc-orphan red (Tiltstone/Underroot badge orphans) is unrelated.
- **Determinism:** the deck is a pure `card_of(seed, night, index)` with zero
  runtime RNG — the whole proof strategy (scripted routes, run-twice
  byte-identical) depends on this; no `rand()` may enter gameplay.
- **Scope-per-slice discipline:** one slice = one PR = one card. The flip/bank
  loop (slice 2) must not smuggle in the charm shop (slice 4); keep the
  born-red diff bounded.
- **Proof-surface honesty:** `headless-boot.yml` is `workflow_dispatch` (heavy,
  not per-PR); the per-PR required gate is `ROM builds` only. Cite the boot
  proof as a regenerate-on-demand artifact, not a required check.

## Continuity note

Nothing lands today beyond this plan. **ALL six slices carry to the re-created
project** — the read-only EAP window lands 2026-07-21, so if A1 is picked the
build happens in the successor project. The landing path there is the **same
`auto-merge-enabler.yml` enabler shape** (born-red `claude/*` card PR → flip →
lands on green `ROM builds`); a re-created project re-arms its own routines and
does not inherit the old coordinator failsafe cron.
