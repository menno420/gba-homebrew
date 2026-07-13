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
| [`cindervault.gba`](cindervault.gba) | **v0.1 prototype** (breadth game #6) · session 41 · 2026-07-13 | 120,984 bytes | source tree of the Cindervault prototype commit (`games/cindervault/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `d3ad8c78693fec8467273a78c174da722a314a9c03494562a3b042d9b1a9a52e` |
| [`deepcast.gba`](deepcast.gba) | **v0.1 prototype** (breadth game #4) · session 39 · 2026-07-13 | 117,032 bytes | source tree of the Deepcast prototype commit (`games/deepcast/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `3a62c8c883a41b20b385427afcaca124c0491f66be42c94e1efa9b7286179fcf` |
| [`clockwork-courier.gba`](clockwork-courier.gba) | **v0.1 prototype** (breadth game #7, the ORDER-001 queue's Clockwork Courier) · session 44 · 2026-07-13 | 121,056 bytes | source tree of the Clockwork Courier prototype commit (`games/clockwork-courier/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `ed877798cee2b31b2beb4ac6dd2d449886c8f4e196ea12f40fe6b9ca839c40a8` |
| [`shoal.gba`](shoal.gba) | **v0.2 the hungry water** (growth rung 1: predators lock onto stragglers, SELECT verb; 50 boids + 2 hunters at 75% worst-frame CPU in IWRAM) · session 46 · 2026-07-13 | 127,196 bytes | source tree of the Shoal predator-pass commit (`games/shoal/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `e63c2f6777a85fcd540c2992cb3b30ac254562364e57afa71878185cfdfa5fdc` |

**How they were built:** GBA — `tools/setup-toolchain.sh` (pinned
devkitARM r68 + Butano) then `tools/build.sh`; NDS —
`tools/setup-nds-toolchain.sh` (pinned BlocksDS 1.21.1 + Wonderful GCC)
then `make -C games/gloamline-nds`. Exact recipes in
[`docs/BUILDING.md`](../docs/BUILDING.md). CI rebuilds both ROMs from
source on every PR and logs their sha256 ("ROM builds" workflow — the NDS
job also prints the committed `dist/gloamline.nds` hash next to the
from-source one), so the committed binaries are always cross-checkable
against a from-source build.
