# Playing Gloamline (Nintendo DS)

> **Status:** `owner-guidance` — download-and-play guide for the committed
> ROM in [`dist/`](../dist/); refreshed in the same PR whenever the game or
> the shipped ROM changes (written 2026-07-11, arc slice 3; refreshed same
> date for arc slice 4 — shove + waves).

**Download → open in a DS emulator → play.** No build step needed. Slice 4
is the first cut that *fights back properly*: every night sends a bigger
wave of the dead (up to a 24-strong crowd), and you get the SHOVE —
barricades and the rest arrive in later slices.

## 1. Get the ROM

Download [`dist/gloamline.nds`](../dist/gloamline.nds) (≈105 KB — on the
GitHub file page, use the **Download raw file** button). Verify if you
like: the sha256 is pinned in [`dist/README.md`](../dist/README.md).

## 2. Open it in melonDS or DeSmuME

- **Desktop (Windows/macOS/Linux):** install
  [melonDS](https://melonds.org/) (the accuracy reference — File → Open
  ROM… → pick `gloamline.nds`) or
  [DeSmuME](https://desmume.org/) (File → Open ROM…). Both run it as-is;
  no BIOS files needed with the default settings of either.
- **Phone:** any DS emulator that loads plain `.nds` files works — melonDS
  has an Android build, and iOS users can load it in Delta (which emulates
  DS). Copy the file to the device (or download it straight from GitHub)
  and open it from the emulator's file picker.
- **Real hardware:** runs from any DS flashcart that takes homebrew
  `.nds` files. No save file is used yet (best-nights persistence is a
  later slice), so there is nothing to configure.

## 3. How to play

You are the **last lamplighter** of a fenced hamlet on the moor. Each
night the gloam sends the walking dead over the fence line — night 1 sends
one Shambler, night 2 sends three, night 3 five... two more every night
until the moor maxes out at a 24-strong crowd (night 13). They trickle in
over the first 40 seconds of the night and never stop walking toward you.
Don't let a single one touch you until dawn.

**Screens:** the TOP screen is the yard — all the action, plus a `Z`
counter for how many are up and a `SHV` light for your shove. The BOTTOM
screen is the watch-map: your position (`P`), every Shambler (`Z`), and
the dawn bar filling left to right. Buttons only — no touch needed.

**Controls**

| Button | Action |
|---|---|
| START (title / after death / at dawn) | Start — retry — next night |
| D-pad | Move, 8 directions |
| A | **Shove** the nearest Shambler in reach |

**Goal:** stay out of reach for the 60 seconds of the night. The HUD
counts down to dawn (`DAWN 0:58`); the moment it hits zero, dawn breaks
and the night is yours. Then the next night begins — same yard, a fresh
fence-line spawn. **NIGHTS** on the HUD is your score.

**What's happening:**

- Every Shambler walks straight at you, always, with a dead man's
  stagger. You are twice their speed — but the yard is closed, the night
  is long and the crowd only grows, so the game is *kiting*: circle wide,
  never corner yourself, and watch the map for where the next one climbed
  the fence.
- **The shove is spacing, not a gun.** Press A with a Shambler within
  arm's reach (about a body-and-a-half): the nearest one is knocked five
  tiles back and stands reeling for most of a second. Then the `SHV`
  light goes dark for a second and a half — hit or whiff — so shove to
  open a corridor, not to win. Shoved dead come back; they always come
  back.
- **One touch kills.** A cold-hands card shows your night, your seed and
  your death count; START restarts instantly.
- Every run is **reproducible**: the SEED on the HUD fully determines
  every spawn of every night (and everything after it, given your
  inputs). Same seed + same moves = same night, every time.

Next slices per the [concept](concepts/gloamline-concept.md): barricades,
the between-nights scavenge interlude, lantern-oil light pressure,
original synthesized audio, and best-nights saves.
