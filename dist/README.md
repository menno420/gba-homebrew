# `dist/` — download-and-play ROM builds

Prebuilt, committed copies of the lane's original ROMs so the owner can
**download one file and play** — no toolchain needed. Everything here is
100% original content (original code + open-source engine/SDK: Butano for
GBA, BlocksDS/libnds for NDS), so committing the binary is publish-safe by
construction. How to play: [`docs/PLAYING.md`](../docs/PLAYING.md) (Lumen
Drift, GBA) · [`docs/PLAYING-GLOAMLINE.md`](../docs/PLAYING-GLOAMLINE.md)
(Gloamline, NDS) · [`docs/PLAYING-BRINEWARD.md`](../docs/PLAYING-BRINEWARD.md)
(Brineward, NDS).

Only player-facing games ship here (the engine-test `skeleton.gba` stays
CI-artifact-only). Each entry below is refreshed in the same PR that
changes its game, with provenance:

| File | Version / date | Size | Built from | sha256 |
|---|---|---|---|---|
| [`lumen-drift.gba`](lumen-drift.gba) | **v1.3** · session 8 slice 6 · 2026-07-11 | 167,996 bytes | source tree of the v1.3 pause/mute + graze-flash micro-polish commit (`games/lumen-drift/` at the same tree as this file) | `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94` |
| [`gloamline.nds`](gloamline.nds) | **best-night rematch** (arc slice 11) · session 2026-07-13-gloamline-rematch | 118,272 bytes | source tree of the slice-11 rematch commit (`games/gloamline-nds/` at the same tree as this file); build is byte-deterministic (two clean builds + an independent fresh-worktree build, identical hash) | `3bab5544c03d01e358fd1f8898df47031bb3f48107fb1469551ab7ed794ec061` |
| [`brineward.nds`](brineward.nds) | **saves — the ledger survives** (arc slice 9) · session 43 · 2026-07-13 | 121,344 bytes | source tree of the slice-9 saves commit (`games/brineward-nds/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `39c5adcb1ecc65e8c79b27e6917a742282d16a9e77c701682b5c5d4ad653aa8f` |
| [`cindervault.gba`](cindervault.gba) | **v0.6 the art pass** (growth cut 5, the LAST named growth line: tile-sprite dungeon — original procedural assets, `games/cindervault/graphics/` — a torch-bearer player sprite, and a torch-radius light fade via the shared `gl_light_window.h` (radius = min(16 + torch/2, 200) px: the screen literally darkens as the torch burns, and re-opens on embers); presentation ONLY — sim, RNG and all 18 telemetry words untouched, P1-P7 carried verbatim; the art itself proven in `games/cindervault/proofs.sh` P8/P9 via a second `cv_light` mailbox + DISPCNT/palette-RAM/VRAM pins) · 2026-07-14 | 138,692 bytes | source tree of the Cindervault art-pass commit (`games/cindervault/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `db7d591824f1f1d203570e93d692d568e11682300f317159733d3ea558b9acf6` |
| [`deepcast.gba`](deepcast.gba) | **v0.6 the art pass** (growth cut 5, the LAST named growth line — the named growth path is COMPLETE: a ten-band lake background — original procedural assets, `games/deepcast/graphics/` — whose palette follows a closed-form law of the live depth word (dim = 2*band + depth/8: the whole lake deepens as the lure sinks), a silhouette fish whose frame IS the species index (band = size, rarity = shade; hidden while sinking and on a SNAP — the species keeps its secret), and a rod-bend sprite, bend = tension*7/snap — the audio ratchet's own law, honest on any line tier; presentation ONLY — sim, RNG and all three sim mailboxes untouched, P1-P7 carried verbatim; the art itself proven in `games/deepcast/proofs.sh` P8-P10 via a fourth `dc_artmeta` mailbox + DISPCNT/palette-RAM/VRAM pins) · 2026-07-14 | 159,928 bytes | source tree of the Deepcast art-pass commit (`games/deepcast/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `ab9c026b889e92466da3e18ffda966cfefe5a068d8d7546f424115d5b503e777` |
| [`clockwork-courier.gba`](clockwork-courier.gba) | **v1.0 CONCEPT COMPLETE** (growth rung 4: six synthesized cues, hook-counted + mixer-proven; every named concept item now built — rewind ghost, co-op door, ghost platform, timed chutes, three floors, audio) · session 54 · 2026-07-14 | 145,752 bytes | source tree of the Courier audio commit (`games/clockwork-courier/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `ad73419f71cee59236fcd8c3c8efe59c26ab4f7e54c7bcdbcf8d486e3afcc75c` |
| [`wickroad.gba`](wickroad.gba) | **v0.3 contracts** (growth cut 2: dated delivery orders — a fixed authored contract deck on the rumor-deck pattern; RIGHT takes the posted pact and hands the goods over at the destination by the deadline for a premium that PRICES RISK (it beats the spot value by a margin that grows with the haul and the deadline's tightness), a missed deadline lapses the pact unpaid; zero new RNG, zero price-law changes — delivery bypasses the market — so the v0.2 rumors world is bit-identical and P1-P4 carry verbatim; the verb itself is proven in `games/wickroad/proofs.sh` P5 — offer/acceptance/delivery with the gold jump == the exact authored premium, and the lapse paying nothing — on top of the carried P1-P4) · 2026-07-15 | 122,696 bytes | source tree of the Wickroad contracts commit (`games/wickroad/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `6e395a1c42cfb62cbb469b3c443d3c8f36eaaa68e61bbec0e77c83af5c5d3c21` |
| [`shoal.gba`](shoal.gba) | **v1.0 CONCEPT COMPLETE** (growth rung 5: five synthesized cues, hook-counted + mixer-proven; every named concept item now built — flock, current, reef, predators, gates, ratings, four tuned levels, audio) · session 50 · 2026-07-13 | 155,464 bytes | source tree of the Shoal audio commit (`games/shoal/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `29a67ab4b5891c9133479de23781ac011ed833ba2f72776259bf2df2193b6d86` |

## Web arcade (`web/`) + release zips (`releases/`)

`web/` is the host-ready static arcade (landing page + `undertow/`,
`tiltstone/`, `drift-garden/` — runtime files only, per each game's
hosting contract); `releases/` holds deterministic versioned zips.
Both are regenerated by `tools/package-web-arcade.sh` and pinned in
`docs/RELEASES.md` (assert with `--verify`). Repinned from main by the
2026-07-14 ORDER 006 slice after the Tiltstone stack (#92-#97) landed
(Tiltstone v1.1 with `juice.js`, Undertow v1.5, Drift Garden v1.4,
arcade bundle v1.3 — superseding the #130 arcade-refresh cut, which
superseded #109, which superseded the stale #85 cut).

**How they were built:** GBA — `tools/setup-toolchain.sh` (pinned
devkitARM r68 + Butano) then `tools/build.sh`; NDS —
`tools/setup-nds-toolchain.sh` (pinned BlocksDS 1.21.1 + Wonderful GCC)
then `make -C games/gloamline-nds`. Exact recipes in
[`docs/BUILDING.md`](../docs/BUILDING.md). CI rebuilds both ROMs from
source on every PR and logs their sha256 ("ROM builds" workflow — the NDS
job also prints the committed `dist/gloamline.nds` hash next to the
from-source one), so the committed binaries are always cross-checkable
against a from-source build.
