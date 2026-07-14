# CONCEPT — Drift Garden (first mobile target on this foundation)

> **Status:** `first playable slice shipped 2026-07-13` — drift-garden
> session, on the mobile-foundation skeleton. Original IP; one page per
> the concept convention.

Shipped in the slice: seeded 90 s win/lose rounds (plant / herd /
cross-pollinate to tier 3 / harvest essence vs. quota) + predator wisps.
Growth cut 1 shipped 2026-07-14: daily seeded weather (UTC-date-derived
wind parameters, read once at boot). Growth cut 2 shipped 2026-07-14:
more species (two per tier, distinct visuals/values; hybrid
cross-pollination). Growth cut 3 shipped 2026-07-14: essence spending
(round harvests bank to a persistent wallet; a dusk-screen shop spends it
on render-only garden palettes). Growth cut 4 shipped 2026-07-14: biomes
(essence-purchasable gardens with distinct wind patterns, read once at
round start). **The concept's named growth path is complete** — further
work is new concept, not a listed cut.

## Genre

Ambient idle-garden / gentle arcade hybrid — the "one thumb, thirty
seconds, no fail state at first" class that carries the strongest casual
mobile retention. Portrait, offline-first, session length 30 s – 5 min.

## Core loop

1. **Tap to plant** a mote (seed of light) anywhere — it drifts, glows,
   and slowly matures (the skeleton's tap-spawn is this verb already).
2. **Drag to shepherd** — currents from your finger herd motes together;
   motes that cluster **cross-pollinate** into rarer, brighter species
   (MORE SPECIES BUILT 2026-07-14 — two species per tier with distinct
   hues/values; mixed-species parents breed the rare next-tier line).
3. **Return later** — the garden kept drifting (or resumes exactly where
   backgrounded, per the pause discipline); harvest matured motes for
   essence, spend essence on new biomes/palettes/species (ESSENCE
   SPENDING BUILT 2026-07-14 — each round's harvest banks to a persistent
   wallet, spent in the dusk-screen shop on unlockable garden palettes,
   pure render; BIOMES BUILT 2026-07-14 — the same wallet buys biomes in
   a second shop section; species unlocks would ride the same wallet).

Depth ramps: wind patterns per biome (BUILT 2026-07-14 — the last named
growth cut: four biomes, each a parameter set for how the round's wind
evolves — gust strength/cadence, a deterministic swirl, a breathing sway,
prevailing-bias coupling — bought with essence, read ONCE at round start;
the free default reproduces pre-biome rounds byte-exactly), predator
wisps to fend off with drags, daily seeded "weather" so every player's
Tuesday garden differs (BUILT 2026-07-14 — UTC date picks the day's named
wind parameters at boot; the sim step never reads the clock).
The skeleton's fixed-timestep loop makes grown gardens reproducible from
(seed, action log) — the same replay-by-construction proof discipline the
GBA/NDS arcs use.

## Sellability guess (honest, unvalidated)

Ambient/zen garden games are a proven small-but-real niche (the
Viridi/Prune/Alto's-Odyssey audience): plausible as a free PWA with a
one-time "full garden" unlock or cosmetic palettes once packaged as a TWA
with Play Billing. Realistic expectation for a solo original with zero
marketing: portfolio piece + low-hundreds installs organically; a playable
seeded slice now exists, but it is one round loop deep — the value is
still the **foundation + provable pipeline**, not a revenue bet.
No spend, no store account, no payment flow without an explicit owner
action (PLATFORM-LIMITS.md mission rails).

## Packaging path to stores

| Step | What | Provable in this container? |
|---|---|---|
| 1 | **PWA direct install** — manifest + SW, "Add to Home Screen" from any URL (needs HTTPS hosting, e.g. GitHub Pages) | YES — shell built + smoke-proven now; Pages enablement is an owner click |
| 2 | **Play Store via TWA** — Bubblewrap/PWABuilder wraps the hosted PWA into an AAB (Trusted Web Activity + `assetlinks.json`) | PARTIAL — config/manifest prep yes; the AAB build needs the Android SDK (walled here, PLATFORM-LIMITS.md 2026-07-13) |
| 3 | **Signing + listing** — Play Console account ($25), signing keys, store listing, screenshots | NO — owner action only (account creation/spend are owner-gated rails) |
| 4 | iOS | PWA installs via Safari share-sheet today; App Store needs a Mac/Xcode + $99/yr — out of scope, owner decision |

Bottom line: this container can carry the game to a **hosted, installable,
smoke-proven PWA**; store packaging is a bolt-on owner-action milestone,
not a rewrite.
