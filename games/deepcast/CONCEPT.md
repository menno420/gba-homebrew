# Deepcast — concept

> **Status:** `reference`

## Pitch

A quiet lake at dusk, three lures, one button. Hold A to charge a cast —
how long you hold IS how deep the lure goes, and deeper water holds
heavier, more valuable fish. When one bites, the whole game is the line:
reel and the tension climbs, yield and the fish takes your line back.
Snap the line and you lose a lure; land the fish and its weight is your
score. When the third lure is gone, dusk falls.

## Genre

Single-mechanic fishing arcade / tension management (push-pull risk
metering: the "reel or yield" loop of classic fishing minigames promoted
to being the entire game). Deliberately NOT a reflex game — the bite
hooks itself; all skill lives in reading the fish's rest/surge rhythm.

## Core loop

Charge a cast (risk appetite: deep = rich = long, dangerous fight) → the
lure sinks, a seeded fish bites by depth → fight: hold A to reel while it
rests, release while it surges → catch (score += weight) or snap (lure
lost) → repeat until all 3 lures are gone → final score, one button to
run it back. The same seed replays identically, so a score is a claim
someone else can check.

## Target platform

Game Boy Advance (Butano). Fully deterministic fixed-point core with a
frame-driven, seeded PRNG and a telemetry mailbox (`dc_telemetry`), so
every build is headless-provable in CI and every run is replayable from
an input script.

## Sellability guess (honest)

Near-zero as a paid standalone — a text-presentation GBA homebrew of a
known minigame mechanic sells to almost nobody. Its realistic market is
the flash-cart / itch.io homebrew niche: a free or pay-what-you-want
curio, or one game in a bundled original-homebrew compilation cart,
where "complete, polished, deterministic score-attack" is the bar it
already meets. The mechanic's depth-vs-greed decision is the one seed
worth growing; the presentation is placeholder.

## Growth path

- Real art pass: lake gradient by depth, silhouette fish, rod-bend sprite
  as the analog tension gauge (HUD bar becomes diegetic).
- Fish species tables per depth band with named rarities; a catch log.
  **BUILT** (growth cut 3, 2026-07-14): four depth bands by target depth
  (THE SHALLOWS / MIDWATER / THE DEEPS / THE ABYSS — the band is named on
  the sink card), four named species per band, one per rarity tier
  (COMMON / UNCOMMON / RARE / MYTHIC; 16 species, MUD BREAM to THE
  NAMELESS). The species is drawn at cast time from a SIDE-BAND xorshift32
  stream seeded from `seed ^ 0x51DEF157`, so the main stream's word order
  is untouched — every v0.2/v0.3 pin carries verbatim, and the seed-select
  contract extends to the fish's name (same dialed seed = same species on
  the same casts). The catch card names the landed fish; SELECT (the one
  unused key) opens a per-run CATCH LOG (ring of the last 8 landed fish —
  snapped lines never log). The log is session-scope by design: SRAM
  persistence of a lifetime log is its own growth cut (follow-up), since
  battery-save plumbing + save-versioning is a bigger slice than a data
  table. Proven in `games/deepcast/proofs.sh` P6 (species/rarity/log
  glyph-exact + `dc_fishlog` mailbox witnesses, run twice byte-identical)
  with species witnesses threaded through P1-P5.
- Line upgrades bought with score (thicker line = higher snap threshold,
  slower reel) — a run-to-run meta without breaking determinism.
- Daily seed + score-attack: the deterministic core makes a shareable
  "same lake, same fish" challenge free to build. **BUILT** (growth cut
  2, 2026-07-14), GBA-shaped: a cartridge has no clock and no server, so
  "daily" becomes a dialable seed — the title's seed line is a picker
  (UP/DOWN +-1, LEFT/RIGHT +-0x100, L/R +-0x10000, shown as 8 hex
  digits). Two players who dial the same digits fish the SAME lake —
  same bites, same weights, same surge rhythm — and the dusk score card
  names the seed, so a score is a claim anyone with a cartridge can
  check. The boot seed 0xDEE9CA57 is untouched: no dial input = the
  v0.2 run, bit-identical. Proven in `games/deepcast/proofs.sh` (P4:
  dialed lake, run twice; P5: dial away and back reproduces every
  default-lake literal). The leaderboard half of this line is out of
  GBA scope — no network — per the Tiltstone PR #97 precedent; the seed
  is what makes an off-cartridge leaderboard honest.
- Audio: reel clicks that speed up with tension — tension readable with
  eyes closed (also a nice accessibility angle). **BUILT** (growth cut 1,
  2026-07-14): while reeling, a dry ratchet click fires on a period that
  shrinks linearly with tension (every 16 frames at slack -> every 4 at
  the snap point) with its pitch rising the same way; yielding is
  silent, so the click train IS the tension bar. Plus the loop's three
  event cues (bite plunge, catch resolve, snap twang) — four original
  synthesized sounds (`audio/generate_audio.py`, deterministic, no
  samples), B mutes. Proven in `games/deepcast/proofs.sh` (decision
  ledger + mixer-memory voicing watch).
