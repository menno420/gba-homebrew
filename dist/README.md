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
| [`gloamline.nds`](gloamline.nds) | **watch-map polish** (arc slice 10) · session 37 · 2026-07-12 | 118,272 bytes | source tree of the slice-10 watch-map commit (`games/gloamline-nds/` at the same tree as this file); build is byte-deterministic (two clean builds + an independent fresh-worktree build, identical hash) | `2ec0afa40a8d27224b95d46053fba0ee0b857453d0d983d3932ec2e79aac0749` |
| [`brineward.nds`](brineward.nds) | **the Maw** (arc slice 5) · session 31 · 2026-07-12 | 116,224 bytes | source tree of the slice-5 Maw commit (`games/brineward-nds/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `6e571941c4d286e76d1a05b18cb1c498b0ccfc20da5c62c95bc2d22a26d2af7c` |

**How they were built:** GBA — `tools/setup-toolchain.sh` (pinned
devkitARM r68 + Butano) then `tools/build.sh`; NDS —
`tools/setup-nds-toolchain.sh` (pinned BlocksDS 1.21.1 + Wonderful GCC)
then `make -C games/gloamline-nds`. Exact recipes in
[`docs/BUILDING.md`](../docs/BUILDING.md). CI rebuilds both ROMs from
source on every PR and logs their sha256 ("ROM builds" workflow — the NDS
job also prints the committed `dist/gloamline.nds` hash next to the
from-source one), so the committed binaries are always cross-checkable
against a from-source build.
