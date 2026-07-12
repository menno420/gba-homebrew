# Playing Brineward (Nintendo DS) — port + upgrades

> **Status:** `owner-guidance` — arc slice 4 (port/upgrades on the
> slice-3 economy). Broadside duels off the Graywake breakwater: sail,
> out-turn the rum-runner, sink her, scoop the flotsam, bank the gold at
> the pier — then SPEND it: hull, cannons and sails upgrade tracks and
> paid repairs at the Graywake port. Concept + roadmap:
> [`concepts/brineward-concept.md`](concepts/brineward-concept.md).

## Get the ROM

- **Download and play:** [`dist/brineward.nds`](../dist/brineward.nds)
  (112,128 bytes, sha256
  `9f354a3744377e49a5bd78c532ae411c316e64d9e11e50d95f54e1d484c64322`)
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
  Graywake pier, HUD (hull / battery reloads / trim / seed / salvage).
- Bottom screen: the Graywake ledger — hull bars for both ships, reload
  bars, trim, sunk/lost tallies, hold + banked gold, range to the foe.
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
- **One enemy, one fight at a time** — no sea monsters, no danger bands
  yet.
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
ways; containment at max tiers). Then TEN headless DeSmuME proofs
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
Proof screenshots:
[`games/brineward-nds/proof/`](../games/brineward-nds/proof/).
