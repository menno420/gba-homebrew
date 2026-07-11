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
| [`lumen-drift.gba`](lumen-drift.gba) | session 8 slice 3 · 2026-07-11 | 163,572 bytes | source tree of the row #23 crystal-gate fix commit (this PR — `games/lumen-drift/` at the same tree as this file) | `88d8146a066d15ee89533e0af80c216eea040ce2027e810182a54f2a5216b697` |

**How it was built:** `tools/setup-toolchain.sh` (pinned devkitARM r68 +
Butano) then `tools/build.sh` — the exact recipe in
[`docs/BUILDING.md`](../docs/BUILDING.md). CI rebuilds the same ROM from
source on every PR and logs its sha256 ("ROM builds" workflow), so the
committed binary is always cross-checkable against a from-source build.
