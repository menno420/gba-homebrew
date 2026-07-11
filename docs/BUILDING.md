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

## NDS builds (Gloamline arc — BlocksDS)

> Added 2026-07-11 (arc slice 2 feasibility gate — everything below was run
> and verified in-container that date; feasibility **PROVEN**).

```sh
tools/setup-nds-toolchain.sh      # one-time: pinned BlocksDS 1.21.1 + Wonderful GCC 16.1.1
make -C games/gloamline-nds -j"$(nproc)"
```

Output is `games/gloamline-nds/gloamline-nds.nds` (gitignored; the
player-facing copy ships committed as `dist/gloamline.nds` since arc
slice 3 — provenance row in [`../dist/README.md`](../dist/README.md)).
The setup script installs into
`/opt/wonderful` (the packages are prefix-bound — loader paths are patched
at install time, so the root is NOT relocatable, unlike devkitARM). Every
artifact is **pinned by SHA-256** and the install fails closed on any
mismatch; pins live in the script header. Unlike the official devkitPro
infra (Cloudflare-403 wall), the Wonderful/BlocksDS package hosts are
directly reachable from the fleet container — no mirror needed.

### Running the .nds

- **Desktop melonDS or DeSmuME** (owner playtesting): open
  `games/gloamline-nds/gloamline-nds.nds`. melonDS is the accuracy
  reference; recent DeSmuME also works.
- **Headless (agents / CI)** — dual-screen boot proof + screenshots without
  a display:

  ```sh
  pip install py-desmume==0.0.9    # bundles its own DeSmuME core + SDL2
  python3 tools/nds-headless-check.py games/gloamline-nds/gloamline-nds.nds out.png \
      --frames 300 --shot 150:mid.png --require-distinct
  ```

  Dumps both screens stacked (256x384), exits non-zero if EITHER screen is
  blank; `--require-distinct` additionally proves the main loop is running.
  Since arc slice 3 the checker also reads the ROM's telemetry mailbox the
  GBA way: `--elf build/gloamline-nds.elf --watch t:gl_telemetry:24
  --assert-watch FRAME:t:IDX:OP:VALUE` (+ `--watch-log` CSV) — numeric
  game-state asserts against ELF-resolved symbols, interface ported from
  `headless-screenshot.py` (slice 4 grew the mailbox 16 -> 24 words:
  wave/shove/stun counters + the GL_T_VLINES/GL_T_VLMAX frame-cost probe).
  Survive routes are derived and skew-verified on the host mirror with
  `tools/gloam-route.py` (`derive --nights N --out FILE`, `verify
  --keys-file FILE --nights N`). `make GL_STRESS=1` builds the CI-only
  perf-stress ROM (full 24-Shambler crowd at frame 0, death disarmed) for
  the frame-budget proof — always `make clean` between shipped and stress
  builds.
  ⚠ The checker latches "no keys pressed" before frame 0 — without that,
  the py-desmume core's reset-state KEYINPUT (active-low zero = everything
  held) matches the BlocksDS default ARM7 exit combo (SELECT+START+L+R) and
  every BlocksDS ROM powers off at frame 1 (documented in
  [`PLATFORM-LIMITS.md`](PLATFORM-LIMITS.md)).

CI: the `nds-rom-build` job in `rom-builds.yml` runs the host-mirror
proof (`tools/check-gloam.py`), builds the .nds with the cached pinned
toolchain, replays nine headless proofs (boot+telemetry, 8-way move,
chase+death+restart, survive-night-1-to-dawn, shove, night-2 waves +
multi-zombie survive, 24-Shambler + 8-barricade frame-budget on the
stress build, barricade hold/breach lifecycle, barricade repair
economy — all pinned `--assert-watch` numerics), and uploads `.nds` +
sha256 +
screenshots alongside the GBA artifacts. The required GBA `ROM builds` gate is a
separate untouched job (its `games/*/` loop skips BlocksDS Makefiles).
