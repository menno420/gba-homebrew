# `dist/` — download-and-play ROM builds

Prebuilt, committed copies of the lane's original ROMs so the owner can
**download one file and play** — no toolchain needed. Everything here is
100% original content (original code + the Butano engine), so committing
the binary is publish-safe by construction. How to play:
[`docs/PLAYING.md`](../docs/PLAYING.md).

Only player-facing games ship here (the engine-test `skeleton.gba` stays
CI-artifact-only). Each entry below is refreshed in the same PR that
changes its game, with provenance:

| File | Version / date | Size | Built from | sha256 |
|---|---|---|---|---|
| [`lumen-drift.gba`](lumen-drift.gba) | **v1.1** · session 8 slice 4 · 2026-07-11 | 164,092 bytes | source tree of the v1.1 "the echoes deepen" difficulty-curve commit (this PR — `games/lumen-drift/` at the same tree as this file) | `d7e9e91a4d4aa136d68a5899841f1a7c631b5db7591f3647ae9c4a77c09d9b4e` |

**How it was built:** `tools/setup-toolchain.sh` (pinned devkitARM r68 +
Butano) then `tools/build.sh` — the exact recipe in
[`docs/BUILDING.md`](../docs/BUILDING.md). CI rebuilds the same ROM from
source on every PR and logs its sha256 ("ROM builds" workflow), so the
committed binary is always cross-checkable against a from-source build.
