# Playing Brineward (Nintendo DS) — loot + gold

> **Status:** `owner-guidance` — arc slice 3 (loot/gold on the slice-2
> skeleton). Broadside duels off the Graywake breakwater: sail, out-turn
> the rum-runner, sink her, scoop the flotsam, bank the gold at the pier
> — or sail straight into the next fight with the hold aboard and risk
> it. Concept + roadmap:
> [`concepts/brineward-concept.md`](concepts/brineward-concept.md).

## Get the ROM

- **Download and play:** [`dist/brineward.nds`](../dist/brineward.nds)
  (110,080 bytes, sha256
  `40ecf251f5c64d723107b9a1a20e9a0d41e86798bebde3c2bf1c724122607886`)
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
| START | begin / restart after sinking / put out again from salvage |

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

- **Gold has nowhere to go yet** — banking is the score; the port MENU
  (repairs, hull/cannon/sail upgrade tracks) is the NEXT slice, per the
  concept's roadmap. Crate value is flat 5g (danger bands, which scale
  it, are roadmap slice 5).
- **Loot numbers are decide-and-flag owner-tunables** — 3 crates per
  wreck, 5g each, hold cap 8, 10 px scoop reach, 12 px pier berth: all
  one-constant changes in `bw_sim.h`, machine-proven but not
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
determinism, both duel outcomes reachable, 20k-frame containment, and
the slice-3 loot rails (every win drops exactly 3 crates on the wreck
ring; a salvage policy always scoops and banks; the hold cap and pier
window hold; salvage containment). Then six headless DeSmuME proofs
assert the ROM's `bw_telemetry` mailbox numerically: boot, exact sail
kinematics, an idle player sunk + instant restart, a recorded route
(from `games/brineward-nds/tools/record-duel-win.py`) that WINS the
duel with the player hull untouched and leaves 3 crates afloat, a
recorded salvage story (`games/brineward-nds/tools/record-salvage.py`)
that scoops all 3 and banks 15g at the pier with the gold persisting
into the next duel, and a carry run where sinking forfeits the unbanked
hold while banked gold stays safe. Proof screenshots:
[`games/brineward-nds/proof/`](../games/brineward-nds/proof/).
