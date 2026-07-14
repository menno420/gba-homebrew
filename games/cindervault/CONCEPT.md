# Cindervault — concept

> **Status:** `reference`

## Pitch

Five floors down, one torch. Every step you take burns a little more of
the light you are carrying, and the vault at the bottom does not care
how brave you were when it goes out. Step, bump, wait — the dungeon only
moves when you do — grab embers to feed the torch, put down the things
in the dark by walking into them, and find the stairs before the
arithmetic finds you. Reach the bottom and the vault opens: your score
is everything you carried through it.

## Genre

Turn-based dungeon-dive roguelike (single-resource descent: the classic
roguelike food-clock promoted to being the entire game, with bump
combat and one-step monster chase). Deliberately NOT a reflex, gravity,
survival, naval, fishing or garden game — it is distinct from all seven
siblings in this repo: Lumen Drift (GBA one-button gravity drifter),
Gloamline (NDS zombie horde survival), Brineward (NDS pirate naval
action), Undertow (web endless-diver reflex), Tiltstone (web turn-based
gravity puzzle — shares turns, shares nothing else: no dungeon, no
resources, no combat), Drift Garden (mobile ambient idle-garden) and
Deepcast (GBA fishing tension arcade). Nothing else in the lane is a
dungeon crawl.

## Core loop

Read the floor (walls, embers, monsters, stairs are all visible — the
tension is arithmetic, not surprise) → decide what the torch can afford:
detour for embers and kills, or rush the stairs → step/bump/wait, one
turn at a time, while every monster steps back at you → descend →
floors get meaner (more monsters, a 3-HP brute from floor 3) while the
same torch keeps burning → descend floor 5 to open the vault, or die to
DARKNESS (torch 0) or SLAIN (HP 0). Score = 100 x floors cleared + 25 x
embers + 10 x kills + remaining torch on a win — greed and speed are
the same currency. The whole run is one fixed seed (shown on the title
screen) and turns advance only on input, so a score is a claim someone
else can replay and check.

## Target platform

Game Boy Advance (Butano). It fits the repo's identity, the required
"ROM builds" CI job auto-discovers the Makefile with zero workflow
edits, and the proven headless proof harness (mGBA + telemetry mailbox
`cv_telemetry` + font-pixel text asserts) makes every build's win path,
lose path and determinism checkable in CI. A turn-based core is the
best possible match for that harness: input edges are turns, so proofs
are exact transcriptions.

## Sellability guess (honest)

Near-zero as a paid standalone — a small 5-floor roguelike (tile-art
since growth cut 5; glyph-rendered before that) is a tech-sketch next
to free genre giants. Its realistic market is the
flash-cart / itch.io homebrew niche: a free or pay-what-you-want curio,
or one game in a bundled original-homebrew compilation cart. What the
prototype actually proves is the part that IS worth money in the genre:
a fully deterministic, replay-checkable descent with a single readable
resource clock — daily-seed score attack ("same vault, same monsters")
is free to build on top, and that loop is what retains roguelike
players. The presentation was placeholder by design until growth cut 5
(the art pass) made the torch clock diegetic.

## Growth path

- Real art pass: tile sprites for walls/floor, a torch-radius light
  fade (the mechanic made diegetic — the screen literally darkens as
  the torch burns down). **BUILT** (growth cut 5, 2026-07-14 — the
  LAST named growth line: the growth path is complete): the glyph rows
  are replaced by an 8x8 tile background (original procedural assets,
  `graphics/generate_assets.py` — floor, brick wall, glowing ember,
  steel stairs, lantern, blade, both monster breeds) and the player is
  a torch-bearer sprite; the torch drives a hardware-window light
  circle (the shared `games/common/include/gl_light_window.h`) of
  radius min(16 + torch/2, 200) px around the player — the light
  closes in as the torch burns and re-opens on every ember, while the
  torch-bearer and HUD ride over the darkness (sprites are never
  clipped: they carry the flame). Presentation ONLY — sim, RNG and all
  telemetry untouched, every prior proof pin carried verbatim; the art
  is itself proven headlessly in `games/cindervault/proofs.sh` (P8:
  radius follows five carried torch pins down to a visibly-dark 48 px,
  plus DISPCNT/palette-RAM/VRAM pins of the committed art; P9: the
  ember relight).
- Item layer: one inventory slot (a lantern that halves burn for 20
  turns, a blade that bumps for 3) picked up like embers — one more
  decision, zero new verbs. **BUILT** (growth cut 1, 2026-07-14): each
  floor spawns one lantern 'o' and one blade '/'; walking onto one
  picks it up into the single slot (pickup activates it, and picking up
  the other kind replaces the held one — that swap is the one more
  decision). Lantern: the torch burns on every second held turn for 20
  turns (exactly 10 burned — halved), then gutters out. Blade: bumps
  hit for 3 while held (a 3-HP 'M' dies in one bump). Proven in
  `games/cindervault/proofs.sh`.
- Named monster species per depth with distinct chase quirks (the
  greedy step is a plug-in policy). **BUILT** (growth cut 2,
  2026-07-14): one species per depth, each one quirk on the now
  plugged-in greedy step — floor 1 CINDER RAT (the baseline chase),
  floor 2 SOOT WISP (flits out on odd turn counts — closes and drains
  at half rate), floor 3 ASH HOUND (pack discipline: at most one hound
  bites per turn), floor 4 VAULT WRAITH (cold grasp: bites axis-aligned
  at range 2 instead of stepping), floor 5 HOARD SENTINEL (holds its
  post until the player crosses its leash of 4). Species consume no
  RNG (pure function of the floor), the depth's name is on the HUD
  ("FOE ..."), and every quirk has an arithmetic witness in
  `games/cindervault/proofs.sh` (P4 — a full descent that ends in the
  open vault).
- Daily seed + score-attack leaderboard: the deterministic core makes
  "same vault, same monsters" a shareable challenge for free. **BUILT**
  (growth cut 4, 2026-07-14), GBA-shaped per the Deepcast precedent: a
  cartridge has no clock and no server, so "daily" becomes a dialable
  seed — the title's seed line is a picker (UP/DOWN +-1, LEFT/RIGHT
  +-0x100, L/R +-0x10000, shown as 8 hex digits; none of these clash
  with the title's only other input, START). Two players who dial the
  same digits dive the SAME vault — same floors, same monsters, same
  embers — and the death card (the score-attack card: endless runs end
  only there) names the seed, so a score is a claim anyone with a
  cartridge can check. The boot seed 0xC1DE5EED is untouched: no dial
  input = the v0.4 run, bit-identical. Proven in
  `games/cindervault/proofs.sh` (P6: the dialed vault, run twice; P7:
  dial away and back reproduces every default-vault literal). The
  leaderboard half of this line is out of GBA scope — no network — per
  the Tiltstone PR #97 precedent; the seed is what makes an
  off-cartridge leaderboard honest.
- Depth banding past floor 5 for an endless mode; the vault becomes
  the first milestone instead of the end. **BUILT** (growth cut 3,
  2026-07-14): SELECT on the VAULT REACHED screen delves deeper — the
  win (and its +torch score bonus) stands as the first milestone, and
  the same run continues onto floor 6 from the RNG state the vault
  left behind, the torch bonus dropping back out of the live score:
  the price of not stopping. Past floor 5 every floor draws its
  generator parameters (monster count, big-monster count, embers,
  carve-walk length) from depth bands — one band per 3 floors, the
  deepest the floor of the world — and the five named species recur
  on the five-depth cycle (floor 6 = CINDER RAT again, floor 10 =
  HOARD SENTINEL, and around). No second win: an endless run ends
  only in DARKNESS or SLAIN — score-attack, 100 per cleared floor. A
  run that never presses SELECT is bit-identical to growth cut 2.
  Proven two bands deep in `games/cindervault/proofs.sh` (P5 — ends
  standing on floor 10 at hp 1: survivable, but escalating).
