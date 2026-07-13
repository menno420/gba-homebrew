# Playing Gloamline (Nintendo DS)

> **Status:** `owner-guidance` — download-and-play guide for the committed
> ROM in [`dist/`](../dist/); refreshed in the same PR whenever the game or
> the shipped ROM changes (written 2026-07-11, arc slice 3; refreshed same
> date for arc slice 4 — shove + waves — and slice 5 — barricades; refreshed
> 2026-07-12 for arc slice 6 — the scavenge interlude — and slice 7 —
> lantern-oil light pressure; refreshed same date for slice 8 —
> synthesized audio — and slice 10 — watch-map polish).

**Download → open in a DS emulator → play.** No build step needed. Slice 8
gives the night a voice: a low moor-drone that climbs as your lantern
fails and the dark presses, and chiptune cues for everything you do —
all synthesized on the DS's own square-wave and noise channels, no
samples of anything. Turn the sound on. Slice 9 makes the moor remember:
your best run — most nights survived, and the seed that made it — now
persists across power cycles in the cartridge's battery save. Slice 10
finishes the concept's watch-map: chalk a mark on it (X, or just tap the
map), read how many of tonight's dead are still out in the gloam, and
see your record while you play. Slice 11 puts your best night on a
button: with a record standing, SELECT at the title or after a death
starts a **rematch** — the very night your record was set, spawn for
spawn.

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
  `.nds` files. Since slice 9 the game keeps a tiny battery save (one
  32-byte record — your best run) via standard SPI-EEPROM commands on
  the cartridge backup; most flashcarts map this to an ordinary `.sav`
  file automatically. If your cart exposes a save-size choice, any size
  works (the game assumes the common 64Kbit-class EEPROM addressing).
  Honest note: CI verifies this persistence in DeSmuME's emulated
  battery only — on real carts it is untested; if the save doesn't
  stick, the game still plays perfectly (a missing/odd save just reads
  as "no record yet"). In desktop emulators there is nothing to
  configure — melonDS/DeSmuME create the battery file themselves.

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
oil flask (`o`), your chalk mark (`!`), the watch line (`OUT` — how
many of tonight's dead are still out in the gloam — plus your mark's
range), your best-night record in the header, the dawn bar filling
left to right and the OIL gauge draining under it. Buttons always
suffice — the only touch in the game is an optional shortcut for the
chalk mark.

**Controls**

| Button | Action |
|---|---|
| START (title / after death / at dawn) | Start — retry — next night |
| SELECT (title / after death) | **Rematch** your best night (record required) |
| SELECT (at dawn) | **Scavenge** the yard before the next night |
| START (while scavenging) | Leave early — the night begins |
| D-pad | Move, 8 directions |
| A | **Shove** the nearest Shambler in reach |
| B | **Barricade**: place at your feet — or repair the one in reach |
| X | **Chalk mark** on the watch-map at your position — X again wipes it |
| Stylus (optional) | Tap the watch-map to drop/move the mark to that spot |

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
- **The moor has a voice — listen to your lantern.** A low drone hums
  under every night (and calmer under the dawn light of the interlude).
  While the tank is healthy it stays put; when the oil runs LOW the
  drone climbs and widens with the guttering light, and when the dead
  beyond your lamplight start pressing it becomes a throb — you can
  HEAR the stagger-cancel arrive before the crowd does, and the drone
  falls back the moment every walker is inside your light again. On
  top of that, every beat of the loop has its own chiptune cue: the
  night tolls in low, a shove lands with a thump, a plank knocks, a
  breach splinters, cache and flask pickups clink bright (standing on
  a flask with a full tank is pointedly silent — nothing is wasted,
  and the silence tells you so), dawn rings a bell, and the cold hands
  end the run with a rattle as the drone cuts out dead. Every sound is
  synthesized in code on the DS hardware channels — 100% original, no
  sampled audio anywhere.
- **One touch kills.** A cold-hands card shows your night, your seed and
  your death count; START restarts instantly. Dying while scavenging
  counts all the same — greed has a price on the moor.
- **The moor remembers your best night.** Your record — the most nights
  survived in a single run, and the seed of that run — is written to
  the cartridge's battery save the moment a dawn beats it, and greets
  you on the title screen (`best N night(s) seed S`) every time you
  power on. The dawn and death cards show it too, so you always know
  what you're chasing. Only a strictly better dawn moves it; deaths
  and equal runs touch nothing. And because every run is seed-
  deterministic, the recorded seed means your best night is *literally
  replayable* — start a run when the frame counter matches... or just
  accept the moor's challenge. A corrupt or foreign save can never
  crash the game — it simply reads as a fresh record.
- **The watch-map is now a watch instrument.** Chalk a `!` mark on it —
  press X to mark the spot you're standing on (a barricade line to fall
  back to, a corner you swore you'd never get pinned in), press X again
  to wipe it; or, if you like the stylus, just tap the map and the mark
  lands in that very square (tap somewhere else to move it — the border
  and gauges ignore taps). The mark is chalk, not a thing in the yard:
  the dead ignore it completely, it stays through dawns and interludes
  for the whole run, and a fresh run starts with a clean map. While
  your chalk is down the watch line reads `! MARK n` — how many map
  squares between you and it. Next to that, `OUT n` counts how many of
  tonight's dead are still out in the gloam — `OUT 0` means everything
  the night will send is already inside the fence, so what you see is
  what you fight. And your best-night record rides the map header
  (`BEST n`) — what you're chasing, visible where you're looking.
- **Rematch the moor's best night.** With a record standing, the title
  screen and the death card both offer `PRESS SELECT: rematch` — SELECT
  starts a run on the RECORDED seed instead of a fresh one, so the very
  night your record was set comes back spawn for spawn: same fence
  climbs, same schedule, your inputs against your own ghost of a night.
  A rematch run wears a `*` after its seed on the HUD and the cards so
  you always know which fight you're in; START anywhere still deals a
  fresh night, and beating your record ON the rematch counts like any
  other dawn. No record, no offer — the moor keeps no empty boasts.
- Every run is **reproducible**: the SEED on the HUD fully determines
  every spawn of every night (and everything after it, given your
  inputs). Same seed + same moves = same night, every time. (The
  rematch is this promise on a button — no frame-counter timing
  required.)

The concept doc's slice list is complete: skeleton → shove + waves →
barricades → scavenge interlude → lantern oil → synthesized audio →
best-nights saves → watch-map polish. Slice 11 (the best-night
rematch) is the first cut past that tree, on the owner's
continue-the-arc order: it pays off the save slice's own promise that
the recorded seed makes your best night literally replayable.
