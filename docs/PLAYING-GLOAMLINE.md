# Playing Gloamline (Nintendo DS)

> **Status:** `owner-guidance` — download-and-play guide for the committed
> ROM in [`dist/`](../dist/); refreshed in the same PR whenever the game or
> the shipped ROM changes (written 2026-07-11, arc slice 3; refreshed same
> date for arc slice 4 — shove + waves — and slice 5 — barricades; refreshed
> 2026-07-12 for arc slice 6 — the scavenge interlude — and slice 7 —
> lantern-oil light pressure).

**Download → open in a DS emulator → play.** No build step needed. Slice 7
makes the light itself a resource: your lantern burns oil every night
frame, and when the tank runs low the lamplight gutters and the dead in
the dark stop hesitating. Scavenge oil flasks at dawn or outrun a faster
gloam.

## 1. Get the ROM

Download [`dist/gloamline.nds`](../dist/gloamline.nds) (≈114 KB — on the
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
counter for how many are up, an `O` percentage for your lantern oil, a
`SHV` light for your shove and a `PK` count for your planks (in the
interlude the top line becomes the SCAVENGE clock and remaining-cache
count). The BOTTOM screen is the watch-map: your position (`P`), every
Shambler (`Z`), every barricade (`#`), every plank cache (`*`), every
oil flask (`o`), the dawn bar filling left to right and the OIL gauge
draining under it. Buttons only — no touch needed.

**Controls**

| Button | Action |
|---|---|
| START (title / after death / at dawn) | Start — retry — next night |
| SELECT (at dawn) | **Scavenge** the yard before the next night |
| START (while scavenging) | Leave early — the night begins |
| D-pad | Move, 8 directions |
| A | **Shove** the nearest Shambler in reach |
| B | **Barricade**: place at your feet — or repair the one in reach |

**Goal:** stay out of reach for the 60 seconds of the night. The HUD
counts down to dawn (`DAWN 0:58`); the moment it hits zero, dawn breaks
and the night is yours. Then the next night begins — same yard, a fresh
fence-line spawn. **NTS** (nights survived) on the HUD is your score.

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
- **Barricades are bought time, not safety.** Press B to drive planks
  into the yard at your feet (you start each run with 6; +2 at every
  dawn, pocket caps at 9, and they carry over — your fortifications
  persist night to night). The dead cannot step into a barricade's
  reach: they stop and CHEW, and every blocked lunge splinters it a
  little more (about four seconds of one Shambler's chewing per plank —
  a crowd eats walls fast). A half-chewed barricade visibly splinters;
  at zero it breaches and the slot is bare ground again. Press B next
  to a standing barricade to repair it to full for one plank (a full
  one is never repaired — no plank is ever wasted). You always walk
  freely through your own barricades — you can never seal yourself in,
  and the dead can never be walled out forever. Build corridors, funnel
  the crowd, repair under pressure.
- **Scavenge at dawn — planks are earned now.** When dawn breaks you have
  a choice on the dawn card: START skips straight to the next night (the
  old flat +2-plank grant), or SELECT takes the **scavenge interlude** —
  20 seconds of dawn light in the same yard. You walk back to the
  lamppost, the dawn light drives the dead back to the fence line — but
  they turn right around and keep coming — and five plank caches lie
  scattered where the night left them. Walk over a cache to pocket a
  plank (a full pocket leaves it lying — nothing is wasted). Choosing to
  scavenge means **no flat grant**: what you carry out is what you get,
  so the interlude out-earns the skip only if you actually loot. Shove
  and barricades work in the interlude, your fortifications stand, and
  one touch still kills — daylight is bought planks, not safety. START
  leaves early; when the light runs out the next night begins on its own.
- **The lantern burns — keep it fed.** Your lantern holds three nights
  of oil when a run starts (`O100` on the HUD, the OIL gauge on the
  map), and it burns all night, every night — daylight costs nothing.
  While the tank is healthy the lamplight holds the whole yard's
  respect. When it runs LOW (under about one night's worth left — you
  will see the yard itself dim), the light circle around you starts to
  gutter, and the dead **beyond your lamplight stop hesitating**: their
  dead man's stagger vanishes in the dark, and the gloam presses about
  a third faster until they step into what light you have left. The
  lantern never goes fully out — but a night-3-and-later run on a dry
  tank is a running fight. The ONLY refill is the scavenge interlude:
  two **oil flasks** (`o` on the map, brass bottles in the yard) lie
  among the plank caches, each worth one full night of burning. Walk
  over one to pour it in — a full tank leaves the flask on the ground
  (nothing is ever wasted), and the START skip grants no oil, ever.
  Skipping every dawn is now a choice with a price: the light runs out
  in night 3.
- **One touch kills.** A cold-hands card shows your night, your seed and
  your death count; START restarts instantly. Dying while scavenging
  counts all the same — greed has a price on the moor.
- Every run is **reproducible**: the SEED on the HUD fully determines
  every spawn of every night (and everything after it, given your
  inputs). Same seed + same moves = same night, every time.

Next slices per the [concept](concepts/gloamline-concept.md):
original synthesized audio, best-nights saves, and watch-map polish.
