# Playing Lumen Drift

> **Status:** `owner-guidance` — download-and-play guide for the committed
> ROM in [`dist/`](../dist/); refreshed in the same PR whenever the game or
> the shipped ROM changes (written 2026-07-11, session 8 slice 3).

**Download → open in an emulator → play.** No build step needed.

## 1. Get the ROM

Download [`dist/lumen-drift.gba`](../dist/lumen-drift.gba) (≈160 KB —
on the GitHub file page, use the **Download raw file** button). Verify if
you like: the sha256 is pinned in [`dist/README.md`](../dist/README.md).

## 2. Open it in mGBA

- **Desktop (Windows/macOS/Linux):** install [mGBA](https://mgba.io/downloads.html),
  then File → Load ROM… → pick `lumen-drift.gba`. That's it.
- **Phone:** any GBA emulator that loads `.gba` files works — mGBA has an
  Android build, and iOS users can load the file in Delta. Copy the file to
  the device (or download it straight from GitHub) and open it from the
  emulator's file picker.
- **Real hardware:** the ROM runs from a GBA flashcart; saves use plain
  SRAM.

The game saves your best depth to cartridge SRAM automatically — in mGBA
that battery file is created next to the ROM, nothing to configure.

## 3. How to play

You are a **mote of light** drifting down an endless cave. The cave is
only visible inside your glow, and the glow is slowly failing.

**Controls**

| Button | Action |
|---|---|
| START | Start a run (and restart after game over) |
| A (hold) | Thrust upward against gravity |
| D-pad ← → | Steer sideways |

**Goal:** descend as deep as you can. **DEPTH** on the HUD is your score
(in cave rows below the spawn); **BEST** is your record, and it survives
power-off. **SPARKS** counts the shards you've collected this run.

**What's happening:**

- Your **light shrinks** over time. Grab the glittering **spark shards**
  hugging the tunnel walls to rewind the decay and brighten back up.
- **Crystals** (the spiky clusters in the passage) kill on touch.
- A **surge** of consuming ember light descends from the cave mouth at a
  fixed pace — stall too long and it swallows you. Keep moving down.
- The cave has three handcrafted sections — THE BLUFFS, THE GALLERY,
  THE DEEP — then continues forever through echoing variations of them.
  A banner announces each one as you reach it.

One more run: the cave is identical every time — every death is a lesson
you can spend on the next descent.
