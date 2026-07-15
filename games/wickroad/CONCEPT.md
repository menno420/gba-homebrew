# Wickroad — concept

> **Status:** `reference` — the one-page concept for breadth game #11.
> A NEW concept (the generative rung: the ORDER-001 queue was
> exhausted at Shoal), written and promoted to a playable prototype
> slice in the same session under the established breadth grammar
> (Clockwork Courier PR #96 / Shoal PR #98 shape), on a LIVE OWNER
> directive of 2026-07-14.

## Pitch

A peddler, a pack that holds eight, and one road through five mountain
towns. Buy tallow where they render it, sell it where they burn it,
and reach 300 gold before the winter pass closes in 30 days. The hook
is the LEDGER: prices are only visible in the town you are standing
in — everywhere else you see what you wrote down the last time you
were there, stamped with how many days old the ink is. You navigate an
economy you remember, not one you see; every trip is a bet on your own
stale information.

## Genre

Market-route trading loop (turn-based economic strategy with imperfect
information — the Taipan/Dope Wars family, with the info-decay twist
promoted to being the whole read).

## Why it is distinct from the other ten (the dedup argument)

No game on the roster is an economy game: **Lumen Drift** (gravity
cave drifter), **Gloamline** (zombie horde survival), **Brineward**
(pirate naval ACTION — combat, not commerce), **Undertow** (endless-
diver reflex), **Tiltstone** (gravity puzzle), **Deepcast** (fishing
tension), **Shoal** (flock herding), **Clockwork Courier** (rewind-
ghost platformer — "courier" delivers parcels through physics, not
markets). The two nearest neighbors, named honestly: **Cindervault**
shares "turn-based on GBA" but is a spatial dungeon roguelike (move/
fight/loot verbs, no prices, no trade); **Drift Garden** has a wallet
but is an idle garden (the verb is waiting — no travel, no arbitrage,
no decisions about price). Wickroad's verb set — buy/sell/travel/wait
against drifting seeded prices, market impact and an aging ledger —
exists nowhere else in games/.

## Core loop

Read today's local prices → fill the pack where the town's produce
runs cheap → ride (a day passes, toll paid, ink ages everywhere) →
sell where your ledger says it runs dear — if the ink still tells the
truth → gold compounds → 300 gold and the ledger balances; day 31 and
the pass closes. Days used and final gold are the brag.

## Why it is provable headlessly

Turn-based and integer-only: nothing moves but on a key edge, so the
input script IS the run. One xorshift32 at fixed seed 0x5749434B
('WICK') consumed only at world init; prices are a CLOSED FORM of
(day, market impact) — no per-frame sim at all, so run-twice byte
identity is by construction and every gold/price/ledger word is an
exact integer pin. The mailbox (`wr_telemetry[16]`) carries a
dedicated stale-ink witness pair (true price vs ledger word) so the
HOOK itself is a watch assert: the harness pins frames where the
ledger provably lies. Proof suite committed from day one at
`games/wickroad/proofs.sh` (the Shoal oracle lesson).

## Target platform

Game Boy Advance (Butano, text presentation from the shared common
fonts — a market table IS text, so the house glyph style is the
natural fit, not a compromise). Ships as `dist/wickroad.gba`.

## Prototype slice (this PR)

Title, the seeded five-town/four-good world (every town produces one
good cheap and craves another dear — profitable routes exist by
construction), the full verb set (A buy / B sell / UP-DOWN cursor /
L-R travel / SELECT wait), triangle-wave price drift, market impact
(+1 per unit bought, -1 per sold, cap ±9, decays 1 per dawn — corner
a market and it prices itself in), the ledger with per-town ink age,
win at 300 gold / lose at dawn 31, instant seeded restart. Proven: a
committed 13-day winning route (gold 60 → 328), the impact ladder and
its decay, the stale-ink witness, the pass closing, both routes run
twice byte-identical.

## Growth cuts (deliberately cut; named for later sessions)

1. **Rumors** — a deterministic event deck: town criers telegraph
   future price shocks ("iron trebles in Dunwick by day N"), making
   the ledger a forecasting tool, not just a memory. **SERVED — v0.2**
   (growth cut 1: fixed three-rumor deck, shock as a closed-form term
   of the same price law, crier line + foretold/realized witness words
   16-23, proven in `proofs.sh` P4).
2. **Contracts** — dated delivery orders (X resin to Thornby by day D
   for a premium): a second income verb that prices risk. **SERVED —
   v0.3** (growth cut 2: fixed two-contract deck on the rumor-deck
   pattern — RIGHT takes the posted pact and delivers at the
   destination by the deadline, the premium is the whole payment and
   beats spot by a risk margin, a missed deadline lapses unpaid;
   delivery bypasses the market entirely, witness words 24-31, proven
   in `proofs.sh` P5).
3. **The road itself** — per-leg hazards (bandits, weather) and a
   provisions/guard-hire decision, so travel cost stops being flat.
   **SERVED — v0.4** (growth cut 3: fixed three-hazard deck on the
   rumor/contract-deck pattern — RAID stretches seize gold from an
   unguarded crossing, STORM stretches cost an unprovisioned crossing
   an extra day, LEFT hires the guard for a flat fee consumed by the
   next crossing, hazard or not; each hazard telegraphed on the ROAD
   line from its authored announce day; witness words 32-39, proven
   in `proofs.sh` P6).
4. **A wider map + pack upgrades** — branch roads, 7-8 towns, mules
   that grow capacity: gold buys logistics, the classic Taipan curve.
   **SERVED — v0.5** (growth cut 4: the road runs on past DUNWICK to
   HOLLOWFEN and MIRGATE — seven markets, each with its own aging
   ink — and START at the Hollowfen fair buys mules on a fixed
   authored price ladder, each growing the pack by four; the new
   towns' world comes from the same seed via 16 draws appended
   strictly AFTER the v0.4 stream, so the old world is bit-identical;
   the map deliberately stays ONE road — no branch fork, L/R is a
   committed verb grammar and a junction would need a new travel
   verb — the honest cut named here; witness words 40-47, proven in
   `proofs.sh` P7).
5. **Audio** — synthesized cues via the proven `generate_audio.py`
   pipeline (coin chink, dawn bell, the pass-closing wind), decisions
   hook-counted per the house method.

## Determinism

One xorshift32 at fixed seed `0x5749434B`, consumed only at world
init in a fixed order; integer math only; prices are pure functions
of (day, impact). Same input script = identical run (proven run-twice
byte-identical). Telemetry mailbox `wr_telemetry[16]` every frame.

## Sellability guess

Honest: near-zero as a paid standalone in glyph presentation — but
trading loops are famously sticky (Taipan!, Dope Wars, Space Trader
all thrived on far less hardware than a GBA), and the aging-ledger
read is a genuinely ownable twist worth testing. With rumors +
contracts + sprite art this is a plausible small itch/flash-cart
title; the prototype's job is proving the loop compounds, which the
committed 13-day route already shows.
