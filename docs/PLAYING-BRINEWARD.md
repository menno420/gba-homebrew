# Playing Brineward (Nintendo DS) — walking skeleton

> **Status:** `owner-guidance` — arc slice 2 (v0, first playable). One broadside
> duel off the Graywake breakwater: sail, out-turn the rum-runner, sink
> or be sunk, press START, go again. Concept + roadmap:
> [`concepts/brineward-concept.md`](concepts/brineward-concept.md).

## Get the ROM

- **Download and play:** [`dist/brineward.nds`](../dist/brineward.nds)
  (108,032 bytes, sha256
  `89e68dc2dd926050fdd6202a6d3b9bd8f2d82e10047453de7b41ac7b462dc475`)
  in any DS emulator — melonDS and DeSmuME both work; no BIOS files
  needed for homebrew. The build is byte-deterministic and CI re-builds
  it from source on every PR, printing both hashes side by side.
- **Build from source:** `tools/setup-nds-toolchain.sh` (pinned BlocksDS
  1.21.1 + Wonderful GCC, sha256-verified downloads), then
  `make -C games/brineward-nds`. Recipes: [`BUILDING.md`](BUILDING.md).

## Controls (buttons only — v0 has no touch)

| Input | Effect |
|---|---|
| LEFT / RIGHT | turn (your ship ALWAYS makes way — there is no stop) |
| UP / DOWN | sail trim: battle -> half -> full (and back) |
| L | fire the PORT (left) battery — 3-ball rake, own reload |
| R | fire the STARBOARD (right) battery — own reload |
| START | begin / restart after sinking or victory |

**Sail trim is the whole helm:** full sail is fast but turns wide;
battle sail is slow but turns tight. Speed eases toward the trim target
(momentum), so plan your turns a second early.

**Broadsides fire square off your sides, never forward.** The entire
fight is positional: bring your side to bear while staying off theirs.
Damage falls off with range — point-blank rakes hit for triple what
long-distance balls do. The rum-runner's crew reloads slower than yours;
your edge is cadence, if you can hold the angle.

## What v0 contains

- Top screen: open water, your sloop, ONE enemy sloop
  (intercept-and-circle sail AI), cannonballs, HUD (hull / battery
  reloads / trim / seed).
- Bottom screen: the Graywake ledger — hull bars for both ships, reload
  bars, trim, sunk/lost tallies, range to the foe.
- Duel resolves both ways: sink the rum-runner (victory card) or go
  under (sunk card); START restarts instantly with a fresh seed. Tallies
  persist for the power-on.
- Determinism: the seed (printed on HUD and both cards) is the frame
  you pressed START; the same seed + same inputs replays the same duel.

## Honest gaps (v0, deliberate)

- **One enemy, one fight at a time** — no loot, no gold, no port menu,
  no upgrades, no sea monsters, no danger bands yet (roadmap slices 1-5
  in the concept doc).
- **You can sail away forever** — both ships top out at the same speed,
  so fleeing beyond the enemy's engage range stalls the duel instead of
  ending it. Fine for a duel sandbox; the run economy will give "home"
  a meaning later.
- **No audio** (synthesized pipeline is a later slice, like Gloamline).
- **No saves** — tallies reset at power-off; NDS save persistence is
  still an unprobed question for both arcs.
- **No touch input** — buttons only, per the concept's buttons-first
  rule; the chart-table bottom screen is a status panel v0, not yet the
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
determinism, both duel outcomes reachable, and 20k-frame containment.
Then four headless DeSmuME proofs assert the ROM's `bw_telemetry`
mailbox numerically: boot, exact sail kinematics, an idle player sunk +
instant restart, and a recorded route (from
`games/brineward-nds/tools/record-duel-win.py`) that WINS the duel with
the player hull untouched. Proof screenshots:
[`games/brineward-nds/proof/`](../games/brineward-nds/proof/).
