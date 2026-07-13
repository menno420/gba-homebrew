# Playing Brineward (Nintendo DS) — danger bands + reefs

> **Status:** `owner-guidance` — arc slice 7 (the deeps under the
> slice-6 weather). Broadside duels off the Graywake breakwater: sail,
> out-turn the rum-runner, sink her, scoop the flotsam, bank and SPEND
> the gold at the Graywake port — don't linger over the wreck's blood
> (the Maw comes up under your keel), read the sky before you commit —
> and now the brine has DEPTH: after a win, SELECT presses one danger
> band deeper. Richer wrecks, worse weather, heavier rum-runners with
> faster crews, and rocks that scrape your keel; the deepest band you
> dare is the charted-waters score. Concept + roadmap:
> [`concepts/brineward-concept.md`](concepts/brineward-concept.md).

## Get the ROM

- **Download and play:** [`dist/brineward.nds`](../dist/brineward.nds)
  (118,272 bytes, sha256
  `91ac3eff823593c6ef5f0f743f696562ba13ec76111d84afcd44cc401fe0531f`)
  in any DS emulator — melonDS and DeSmuME both work; no BIOS files
  needed for homebrew. The build is byte-deterministic and CI re-builds
  it from source on every PR, printing both hashes side by side.
- **Build from source:** `tools/setup-nds-toolchain.sh` (pinned BlocksDS
  1.21.1 + Wonderful GCC, sha256-verified downloads), then
  `make -C games/brineward-nds`. Recipes: [`BUILDING.md`](BUILDING.md).

## Controls (buttons only — no touch yet)

| Input | Effect |
|---|---|
| LEFT / RIGHT | turn (your ship ALWAYS makes way — there is no stop) |
| UP / DOWN | sail trim: battle -> half -> full (and back) |
| L | fire the PORT (left) battery — 3-ball rake, own reload |
| R | fire the STARBOARD (right) battery — own reload |
| L / R | in the salvage water: the batteries WAKE while the Maw is up |
| A | dock: open the Graywake port (alongside the pier, no foe afloat) |
| SELECT | in the salvage water: put out one danger band DEEPER |
| START | begin / restart after sinking / put out again / leave port |
| UP / DOWN / A / B | in port: pick a row / buy / walk back out |

**Sail trim is the whole helm:** full sail is fast but turns wide;
battle sail is slow but turns tight. Speed eases toward the trim target
(momentum), so plan your turns a second early.

## The weather (new in slice 6)

- **Each water rolls its weather from the seed** (printed like the
  seed, reproducible like the seed): about a quarter of waters are
  **dead calm**, half carry a **breeze**, a quarter blow a **GALE**.
- **The wind vector turns slowly all run long** — a full box of the
  compass in about two minutes. The HUD's top row carries the gauge:
  `NE~` is a breeze out of the northeast, `NE!` is a gale, `calm` is
  a millpond; the chart table spells it out (`wind out of the NE
  GALE`).
- **Your point of sail is real speed:** running with the wind fills
  the canvas (up to +24 speed units in a gale), beating into it
  fights the hull (up to -24), abeam is neutral — and **the more
  canvas you set, the more the weather matters**: full sail catches
  all of it, half sail half, battle sail a quarter. Shortening sail
  is now also the storm verb.
- **Both ships sail the same weather** — the rum-runner's intercept
  is downwind-fast and upwind-slow exactly like you, so the weather
  is a broadside-geometry knob: force the beat on them, keep the run
  for yourself.
- **The Maw doesn't care.** It swims UNDER the water: gale or calm,
  the shadow homes at the same pace and full sail still outruns it
  (verified even beating diagonally into a gale — barely; read the
  sky before you scoop). Iron doesn't care either: cannonballs fly
  the same in any weather.

## The deeps (new in slice 7)

- **Three danger bands, 0..2.** A fresh sail from the title (or after
  sinking) is band 0 — the home water, exactly the game you knew.
  After a win, **SELECT** puts out one band deeper (carrying hold,
  dents, and tiers, exactly like START — which now re-sails the SAME
  band). There is no deeper water than band 2.
- **Deeper is richer:** crates are worth 5g / 12g / 25g by band, and
  monster salvage climbs too (15g / 30g / 50g a crate). One banked
  band-2 wreck is 75g — half a tier-III purchase in one stand.
- **Deeper is worse:** the deep-water rum-runners run heavier hulls
  (100 / 130 / 160) with faster gun crews (reload 150 / 120 / 100),
  and the seeded weather blows **one level worse per band** (band 2
  never rolls milder than a gale).
- **Rocks.** Bands 1 and 2 lay 3 and 5 reefs at seeded spots (clear of
  the anchorage and the harbor — you can always put out and always
  come home). A reef scrapes YOUR keel for 15 hull, once per contact —
  sitting on the rock costs nothing more, but leave and touch it again
  and it tolls again. Groundings can sink you. The rum-runners and the
  Maw know these waters: the rocks are yours alone to chart.
- **The chart is the score:** the deepest band you have sailed rides
  the ledger (`charted`) and the sunk card — the concept doc's
  "waters charted", alive.

**Broadsides fire square off your sides, never forward.** The entire
fight is positional: bring your side to bear while staying off theirs.
Damage falls off with range — point-blank rakes hit for triple what
long-distance balls do. The rum-runner's crew reloads slower than yours;
your edge is cadence, if you can hold the angle.

## The loot loop (new in slice 3)

- **Sink her and the wreck breaks up:** 3 flotsam crates bob on a ring
  around the wreck. Sail through a crate to scoop it — 5 gold each in
  the home waters, 12g one band down, 25g in the deeps (slice 7). The
  hold carries at most 8 crates; overflow stays afloat.
- **Bank at the Graywake pier** (the marked berth on the south
  breakwater): lie alongside and the hold empties into banked gold.
  **Banked gold is safe forever.**
- **Or don't:** press START from the salvage water and you put out again
  WITH the hold aboard. Win the next one too and the pile grows — but
  **sinking forfeits every unbanked crate** (banked gold survives; the
  tallies and the sunk card say exactly what the brine kept).
- HUD row 2 tracks the salvage (hold count, unbanked worth, BANKED
  flash); the bottom-screen ledger carries the running `hold` and
  `GOLD BANKED` lines at all times.

## The Maw (new in slice 5)

- **Linger and it comes:** ten seconds after the rum-runner goes under,
  the wreck's blood draws something. A dark SHADOW rises at the wreck
  and homes under your keel — that is the telegraph, and it is the
  whole warning you get. HUD: `THE WATER GOES DARK...`; the chart
  table: `something big is turning`.
- **Shadow -> surface -> lunge:** after a fixed telegraph it SURFACES
  where the shadow lies, winds up, and LUNGES at you. **A bite costs
  35 hull** — real dents that carry, repair-priced like any others (and
  yes, a bite can send you down; the hold goes with you).
- **Outrun the shadow at full sail** — it is slower than a full-sail
  sloop by design, and battle-sail scooping is exactly the pace it
  hunts. **Dodge the lunge crosswise**: the charge is a straight,
  committed line at where you WERE; a hull moving beam-on slides out of
  the jaws. Running dead away from a close lunge is how you get bitten.
- **Or kill it:** while it is up (surface + lunge) your batteries WAKE
  — L/R rake it like any hull (a shot at a mere shadow wastes the rake
  over its back). It takes ~two clean rakes. The carcass **breaks up
  RICHER than any rum-runner: 3 crates at 15g each** — one slain Maw
  banks 45g, a tier-III purchase in one stand.
- **The pier is sanctuary:** it never surfaces inside the harbor ring
  and never lunges at a berthed ship — banking, shopping, and repairs
  are always reachable. It gives up, sounds, and stalks again while
  you stay in its water; slay it and that water is safe until you put
  out again.
- One Maw per water: putting out to a fresh duel resets the hunt (and
  the patience clock starts at the next wreck).

## The Graywake port (new in slice 4)

- **Dock to shop:** once the water is yours (salvage, no foe), lie
  alongside the pier and press A — the GRAYWAKE PORT menu opens and the
  sea waits (no shopping under fire: mid-duel the berth still banks,
  nothing more).
- **Three upgrade tracks, tiers I/II/III, bought with banked gold,
  NEVER lost** (not even by sinking):
  - **HULL** — max hull 100 / 150 / 220 (buying raises the ceiling;
    repair fills it);
  - **CANNONS** — battery reload 90 / 70 / 55 frames, and tier III
    throws a 4-ball rake;
  - **SAILS** — a measurably swifter, tighter-turning ship at every
    trim.
- **Prices triple per tier: 15g, then 45g** — one banked band-0 wreck
  (3 crates, 15g) buys your first tier; the rest want deeper pockets.
- **Repair costs gold now:** putting out from salvage KEEPS your dents
  (the sunk-respawn refit is still free — sinking already cost you the
  hold). The REPAIR row prices the missing hull at 4 points per gold,
  rounded against you; coming home hurt is strictly cheaper than going
  under.
- UP/DOWN pick a row, A buys (a short purse or a maxed track is simply
  refused), B or START walks back to the water; START again puts out
  to sea.

## What's in the box

- Top screen: open water, your sloop, ONE enemy sloop
  (intercept-and-circle sail AI), cannonballs, flotsam crates, the
  Graywake pier, the Maw (shadow and risen frames), HUD (hull /
  battery reloads / trim / seed / salvage / Maw warnings).
- Bottom screen: the Graywake ledger — hull bars for both ships, reload
  bars, trim, sunk/lost/maws tallies, hold + banked gold, range to the
  foe, and a Maw status line reading the water.
- Duel resolves both ways: go under (sunk card — shows the forfeited
  hold and the safe banked gold) or sink the rum-runner and salvage the
  wreck; START restarts / re-sails instantly with a fresh seed. Tallies
  and banked gold persist for the power-on.
- Determinism: the seed (printed on HUD and the sunk card) is the frame
  you pressed START; the same seed + same inputs replays the same run.

## Honest gaps (deliberate, roadmap-ordered)

- **The weather is machine-proven, not taste-tested** — the concept
  doc calls this slice "owner-taste, hand-feel gated". Every number
  (breeze push 12 / gale push 24, canvas quarters 1/2/4, rotation one
  unit per 8 frames, the calm/breeze/gale seed mix) is a one-constant
  owner-tunable in `bw_sim.{h,c}`; whether a gale FEELS like weather
  on real hands is the owner's call, and the feel gate is open.
- **Economy numbers are decide-and-flag owner-tunables** — 3 crates per
  wreck, 5g each, hold cap 8, 10 px scoop reach, 12 px pier berth,
  upgrade prices 15g/45g, repair at 4 hull per gold, and every tier
  table (hull 100/150/220, reload 90/70/55, rake 3/3/4, sail bonuses):
  all one-constant changes in `bw_sim.{h,c}`, machine-proven but not
  taste-tested.
- **One enemy, one fight at a time** — one rum-runner and one Maw per
  water, whatever the band (the Maw's numbers — patience 600 frames,
  telegraph 150, windup 60, lunge 55 at speed 520, bite 35, hull 120,
  40 px sanctuary — are all one-constant owner-tunables in `bw_sim.h`,
  machine-proven but not taste-tested; the Maw itself does not yet
  scale with the band).
- **Band numbers are decide-and-flag owner-tunables** — enemy hulls
  130/160, reloads 120/100, crate values 12/25 (monster 30/50), reef
  counts 3/5, scrape 15, the one-level weather worsening: all
  one-constant tables in `bw_sim.c`, machine-proven, feel-gated on
  the owner like the wind. Reefs are player-only hazards by deliberate
  call ("the rum-runners know these waters") — flagged, not hidden.
- **You can sail away forever** — both ships top out at the same speed,
  so fleeing beyond the enemy's engage range stalls the duel instead of
  ending it. The pier now gives "home" a meaning; the run economy will
  close the loop.
- **No audio** (synthesized pipeline is a later slice, like Gloamline).
- **No saves** — tallies, banked gold, and the charted-waters score
  reset at power-off (Gloamline's slice 9 has since proven the NDS
  battery-save path in this repo; Brineward's saves are the roadmap's
  last slice, after audio).
- **No touch input** — buttons only, per the concept's buttons-first
  rule; the chart-table bottom screen is a status panel, not yet the
  concept's tap-able sea chart.
- **AI feel is provable-first, not tuned** — the rum-runner reliably
  kills an idle player and can be beaten by geometry (both properties
  machine-proven every PR), but whether it feels cunning is owner-taste
  work for a later slice.
- **Emulator screenshot hue quirk:** py-desmume's screenshot buffer
  swaps red/blue, so CI proof PNGs show a rust-brown sea; on a real
  emulator screen the water is slate blue. Display-only.

## How it's proven (every PR, `nds-brineward-build` job)

Host mirror first: `tools/check-brine.py` re-implements the pure sim
(`games/brineward-nds/source/bw_sim.c`) line-for-line and proves spawn
determinism, both duel outcomes reachable, 20k-frame containment, the
slice-3 loot rails (every win drops exactly 3 crates on the wreck
ring; a salvage policy always scoops and banks; the hold cap and pier
window hold; salvage containment), and the slice-4 port rails (tier-0
tables ARE the slice-2 sloop, so every recorded route stays bit-valid;
the port ledger's exact prices, refusals and repair math for every
reachable hull value at every tier; upgraded duels still converge both
ways; containment at max tiers), and the slice-5 Maw rails (the whole
telegraph contract frame-exact; ONE bite of exactly 35 per lunge; the
pier sanctuary; the hunter policy slays it and banks the richer crates;
a slain Maw never stirs again), and the slice-6 wind rails (the calm
wind term is identically zero and every committed anchor seed is CALM,
so every earlier route and pin carries verbatim; point-of-sail speed
deltas exact at every bearing, trim, and weather; duels still converge
both ways under forced breeze AND gale; the gale-armed salvage water
stays contained; the escape rails survive the weather — a gale-beating
full sail still outruns the shadow, a gale-running battle-sail scooper
still does not), and the slice-7 band rails (band 0 is STATE-IDENTICAL
to the un-banded water over 512 seeds, so every earlier route and pin
carries verbatim; deeper is monotonically harder and richer with the
doc's 5g -> 25g crate shape; 4096 seeds x 3 bands of reefs all land in
the sea and clear of the anchorage and pier; the scrape contract is
frame-exact — one 15-hull toll per contact, latch held on the rock,
re-armed off it, a 1-hull grounding sinks, the parked enemy untouched;
band duels converge both ways, and the four band-2 waters the tier-0
brain loses all flip to wins with cannons II — the deeps are priced in
gold, not walled; the band-2 rock water survives 8000 adversarial
frames). Then SIXTEEN headless DeSmuME proofs
assert the ROM's `bw_telemetry` mailbox numerically: boot, exact sail
kinematics, an idle player sunk + instant restart, a recorded route
(from `games/brineward-nds/tools/record-duel-win.py`) that WINS the
duel with the player hull untouched and leaves 3 crates afloat, a
recorded salvage story (`games/brineward-nds/tools/record-salvage.py`)
that scoops all 3 and banks 15g at the pier with the gold persisting
into the next duel, a carry run where sinking forfeits the unbanked
hold while banked gold stays safe, and four port stories on the same
recorded bank: the menu opens in the berth and buys HULL II with the
15g (broke thereafter, a second buy is refused, the tier persists into
the next duel), REPAIR heals 47 -> 100 for exactly 14g, SAILS II makes
the same seed + same inputs measurably faster than its own tier-0
control run, and CANNONS II reloads the battery in 70 frames, not 90.
Slice 5 adds two recorded Maw stories on their own anchor seed
(`games/brineward-nds/tools/record-maw.py`, both robust at every
alignment shift in [-6,+6]): the BAIT sails at the dark water and takes
exactly one 35-hull bite (telegraph, surfacing, lunge, and the sounding
all pinned to the frame, the bitten hull carried into the next duel),
and the HUNTER rakes the risen Maw (its hull words drop mid-windup IN
the emulator), watches it give up over the sanctuary, slays it on its
second rising, scoops wreck + monster crates, and banks 60g with the
hunter's own hull untouched.
Slice 6 adds two wind proofs: a scripted gale sail that walks the whole
point-of-sail table in the emulator — the new speed telemetry word
settles at exactly the mirror's predicted value on every leg (downwind
half sail 171 vs the calm 160, upwind half 149, upwind full 202,
upwind battle 90) while the wind-heading word ticks its slow rotation
on camera — and a recorded gale story
(`games/brineward-nds/tools/record-wind.py`, robust at every alignment
shift in [-6,+6]): the committed duel brain WINS a broadside fight
with both ships sailing the same gale, scoops the wreck, banks 15g
before the Maw ever stirs, and puts out into a fresh water that rolls
DIFFERENT weather.
Slice 7 adds two deeper-water stories on the committed duel-win anchor
(`games/brineward-nds/tools/record-bands.py`, water-2 skeletons robust
at every alignment shift in [-6,+6]): SELECT sails DEEPER — the band-1
water arrives charted (the score word flips), lays 3 rocks, rolls
worsened weather, and fields the heavier 130-hull rum-runner; the
unchanged duel brain beats it, scoops the 12g crates, and banks
exactly 36g with no grounding before the Maw stirs, and the final
START proves the depth STICKS across waters — and THE ROCKS ARE REAL:
a deliberate grounding costs exactly 15 hull ONCE for a whole ~33-frame
crossing (the latch on camera at three pinned frames), re-arms in
clear water, and tolls again on re-entry. Every pinned value in both
stories is a row lookup in a mirror-of-main.c vs emulator watch-log
diff (140,562 values, zero mismatches).
Proof screenshots:
[`games/brineward-nds/proof/`](../games/brineward-nds/proof/).
