# Playing Brineward (Nintendo DS) — the Maw

> **Status:** `owner-guidance` — arc slice 5 (the first sea monster on
> the slice-4 port/upgrades economy). Broadside duels off the Graywake
> breakwater: sail, out-turn the rum-runner, sink her, scoop the
> flotsam, bank and SPEND the gold at the Graywake port — but don't
> linger over the wreck's blood, because the water goes dark and THE
> MAW comes up under your keel. Concept + roadmap:
> [`concepts/brineward-concept.md`](concepts/brineward-concept.md).

## Get the ROM

- **Download and play:** [`dist/brineward.nds`](../dist/brineward.nds)
  (116,224 bytes, sha256
  `6e571941c4d286e76d1a05b18cb1c498b0ccfc20da5c62c95bc2d22a26d2af7c`)
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
| START | begin / restart after sinking / put out again / leave port |
| UP / DOWN / A / B | in port: pick a row / buy / walk back out |

**Sail trim is the whole helm:** full sail is fast but turns wide;
battle sail is slow but turns tight. Speed eases toward the trim target
(momentum), so plan your turns a second early.

**Broadsides fire square off your sides, never forward.** The entire
fight is positional: bring your side to bear while staying off theirs.
Damage falls off with range — point-blank rakes hit for triple what
long-distance balls do. The rum-runner's crew reloads slower than yours;
your edge is cadence, if you can hold the angle.

## The loot loop (new in slice 3)

- **Sink her and the wreck breaks up:** 3 flotsam crates bob on a ring
  around the wreck. Sail through a crate to scoop it — 5 gold each in
  these home waters. The hold carries at most 8 crates; overflow stays
  afloat.
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

- **Crate value is flat 5g** — danger bands, which scale loot with
  distance from home, are roadmap slice 5; until then the upgrade
  treadmill is a grind of home-water wrecks by design.
- **Economy numbers are decide-and-flag owner-tunables** — 3 crates per
  wreck, 5g each, hold cap 8, 10 px scoop reach, 12 px pier berth,
  upgrade prices 15g/45g, repair at 4 hull per gold, and every tier
  table (hull 100/150/220, reload 90/70/55, rake 3/3/4, sail bonuses):
  all one-constant changes in `bw_sim.{h,c}`, machine-proven but not
  taste-tested.
- **One enemy, one fight at a time** — one rum-runner and one Maw; no
  danger bands yet (the Maw's numbers — patience 600 frames, telegraph
  150, windup 60, lunge 55 at speed 520, bite 35, hull 120, 40 px
  sanctuary, 15g crates — are all one-constant owner-tunables in
  `bw_sim.h`, machine-proven but not taste-tested).
- **You can sail away forever** — both ships top out at the same speed,
  so fleeing beyond the enemy's engage range stalls the duel instead of
  ending it. The pier now gives "home" a meaning; the run economy will
  close the loop.
- **No audio** (synthesized pipeline is a later slice, like Gloamline).
- **No saves** — tallies and banked gold reset at power-off; NDS save
  persistence is still an unprobed question for both arcs (roadmap
  slice 7).
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
a slain Maw never stirs again). Then TWELVE headless DeSmuME proofs
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
Proof screenshots:
[`games/brineward-nds/proof/`](../games/brineward-nds/proof/).
