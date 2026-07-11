# Building the ROMs

> **Status:** `reference` — one-command build recipe for this repo's GBA
> ROMs, written 2026-07-11 (session 8, ORDER 000). Everything here was run
> and verified in-container on that date. Pins and supply-chain posture live
> in [`../tools/setup-toolchain.sh`](../tools/setup-toolchain.sh); verified
> walls in [`PLATFORM-LIMITS.md`](PLATFORM-LIMITS.md).

## TL;DR (two commands from a fresh clone)

```sh
tools/setup-toolchain.sh          # one-time: pinned devkitARM r68 + Butano 21.7.1
tools/build.sh                    # builds every games/*/ ROM, prints size + sha256
```

`tools/build.sh` is a thin wrapper that exports the toolchain paths and runs
`make` in every game directory — the output is
`games/<game>/<game>.gba` (e.g. `games/lumen-drift/lumen-drift.gba`).

## What the setup script installs

- **devkitARM r68** (gcc 16.1.0 for `arm-none-eabi`) into `/opt/devkitpro`
  (honours `$DEVKITPRO` if you want it elsewhere — CI installs under
  `$HOME`), plus gba-tools / grit / mmutil / devkitarm-rules, all **pinned
  by SHA-256** against the unsigned community mirror (install fails on any
  mismatch), and devkitarm-crtls v1.2.7 built from devkitPro's GitHub
  source.
- **Butano 21.7.1** (zlib license) as a pinned clone in `third_party/butano`
  (gitignored).

It is idempotent — a stamp file skips completed work, so re-running is cheap.
Needs `zstd`, `curl`, `git`, `make` on the host (`apt-get install zstd` is
the only one a stock Ubuntu container tends to be missing; the script
installs it itself when it can).

## Building one game by hand

```sh
export DEVKITPRO=/opt/devkitpro DEVKITARM=/opt/devkitpro/devkitARM
export PATH="$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH"
make -C games/lumen-drift -j"$(nproc)"
```

Cold build of Lumen Drift is ~18s in-container; warm rebuilds ~2s. Build
output (`build/`, `*.gba`, `*.elf`, `*.map`, `*.sav`) is gitignored — ROMs
are **never committed**; CI builds them from source on every PR and push to
main ("ROM builds" check) and uploads them as a workflow artifact with a
logged sha256, so any ROM you hand to a device can be traced to a commit.

## Running the ROM in mGBA

- **Desktop mGBA** (owner playtesting): File → Load ROM →
  `games/lumen-drift/lumen-drift.gba`, or from a shell:
  `mgba-qt games/lumen-drift/lumen-drift.gba`. The game is one-button:
  D-pad + A, START on the title screen.
- **Headless (agents / CI)** — boot proof + screenshot without a display:

  ```sh
  sudo apt-get install -y mgba-sdl   # system libmgba 0.10.x
  pip install mgba==0.10.2           # binding MUST match system libmgba 0.10.x
  python3 tools/headless-screenshot.py games/lumen-drift/lumen-drift.gba out.png --frames 300
  ```

  Exits non-zero on a blank frame, so it doubles as a smoke test. Scripted
  input, per-frame screenshots, text/memory-watch assertions and battery
  saves: see the header of
  [`../tools/headless-screenshot.py`](../tools/headless-screenshot.py).
  ⚠ Use the `--savefile` flag for battery saves — the python binding's
  `core.load_save()` path segfaults (documented in
  [`PLATFORM-LIMITS.md`](PLATFORM-LIMITS.md)).
