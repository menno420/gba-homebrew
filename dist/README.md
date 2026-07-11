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
| [`gloamline.nds`](gloamline.nds) | **barricades** (arc slice 5) · session 21 · 2026-07-11 | 110,080 bytes | source tree of the slice-5 barricades commit (`games/gloamline-nds/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `25ae4f81b665e60cdfa6498cb26933cfced11a5bf3f71302b389af9b17b43cdc` |
| [`brineward.nds`](brineward.nds) | **walking skeleton** (arc slice 2) · session 20 · 2026-07-11 | 108,032 bytes | source tree of the slice-2 skeleton commit (`games/brineward-nds/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `89e68dc2dd926050fdd6202a6d3b9bd8f2d82e10047453de7b41ac7b462dc475` |

**How they were built:** GBA — `tools/setup-toolchain.sh` (pinned
devkitARM r68 + Butano) then `tools/build.sh`; NDS —
`tools/setup-nds-toolchain.sh` (pinned BlocksDS 1.21.1 + Wonderful GCC)
then `make -C games/gloamline-nds`. Exact recipes in
[`docs/BUILDING.md`](../docs/BUILDING.md). CI rebuilds both ROMs from
source on every PR and logs their sha256 ("ROM builds" workflow — the NDS
job also prints the committed `dist/gloamline.nds` hash next to the
from-source one), so the committed binaries are always cross-checkable
against a from-source build.
