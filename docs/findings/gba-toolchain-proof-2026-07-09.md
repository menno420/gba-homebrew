# GBA toolchain proof — both game-lab tracks verified in-container

> **Status:** `reference`
>
> Ground truth from the **toolchain scout session, 2026-07-09** (same night as
> the owner's game-lab decision). Every claim below was executed and verified
> in a real fleet container; the boot/run/screenshot **screenshots live in the
> scout session's chat**, not in any repo — this doc is the committed
> distillation (playbook R2: verify against repos/evidence, and this IS the
> evidence record). Consumed by
> [`../prompts/game-lab-founding.md`](https://github.com/menno420/fleet-manager/blob/main/docs/prompts/game-lab-founding.md) and
> [`../../environments/archetype-gba-lab.sh`](https://github.com/menno420/fleet-manager/blob/main/environments/archetype-gba-lab.sh).

## Verdicts

| Track | Verdict | One-line proof |
|---|---|---|
| **A — Pokémon Emerald mod** (pret/pokeemerald decomp) | **GO** | byte-identical retail ROM built **first try**; text mod rebuilt + verified in-game |
| **B — original GBA homebrew** (Butano on devkitARM) | **GO, with supply-chain caveat** | Butano `sprites` example built in **17.5s** via the leseratte10 community mirror route |
| **Headless test loop** (mGBA, both tracks) | **GO** | boot → run N frames → PNG at **~290 fps**; scripted input drove title → New Game → intro and verified a dialogue mod in-game |

## Track A — pokeemerald (private lane)

- **Deps:** `apt install binutils-arm-none-eabi` is the **only extra apt
  dep** on the fleet container; **agbcc** built per pokeemerald's
  `INSTALL.md` (clone pret/agbcc → `build.sh` → `install.sh <pokeemerald>`).
- **Build:** **byte-identical retail build on the first try** (rom.sha1
  compare green). Full build **1m20s**; incremental rebuild after a text
  mod **2.0s** — the edit-compile-test loop is interactive-speed.
- **RAIL:** pokeemerald + built ROMs contain Nintendo-copyrighted material —
  **PRIVATE repo only** (`pokemon-mod-lab`); never publish or commit ROMs /
  extracted assets anywhere public. Build inputs (a retail base ROM where
  needed) are owner-provided into the private surface only.

## Track B — Butano / devkitARM (public lane)

- **Wall:** the **official devkitPro installers Cloudflare-403** behind the
  fleet proxy (verified; recipe appended to `docs/capabilities.md` per R18).
- **Working route:** the **leseratte10 community mirror**
  (`https://wii.leseratte10.de/devkitPro/`) — extract the **devkitARM r68**
  packages (`devkitARM-r68`, `devkitarm-binutils`, `devkitarm-gcc`,
  linux_x86_64 `.pkg.tar.zst`) into `/opt/devkitpro`, then **build
  make-rules + crt0 from devkitPro's GitHub sources**
  (devkitarm-rules, devkitarm-crtls).
- **Build:** Butano `sprites` example compiles in **17.5s**.
- **⚠ Supply-chain caveat:** the mirror is **unsigned community
  infrastructure** — packages are not devkitPro-signed. Acceptable for a
  sandboxed lab lane; re-verify against official checksums if the route ever
  feeds anything the owner distributes. Butano itself + all lane code is
  original → **publish-safe** (publishing still owner-gated per lane rails).

## Headless emulator loop (both tracks)

- **Deps:** `apt install mgba-sdl` + `pip install mgba==0.10.2` — the pip
  binding **must stay pinned to the system libmgba 0.10.x** (0.10.2 ↔
  libmgba 0.10.x verified; drifting either side breaks the ABI).
- **Loop:** boot ROM → run N frames → dump PNG at **~290 fps** headless —
  fast enough to screenshot every session and to script regression checks.
- **Input injection:** scripted button presses drove title → New Game →
  intro cutscene and **verified a dialogue mod live in-game** — the full
  agents-only verify loop (no human, no display) is proven end-to-end.

## Caveats index (the three to remember)

1. **Mirror supply-chain** — leseratte10 is unsigned community infra (above).
2. **mgba pin** — `mgba==0.10.2` against system libmgba 0.10.x only.
3. **Proxy 403** — official devkitPro package infra is unreachable
   in-container; the mirror route is the documented recipe (don't re-probe
   the wall — R18/capabilities entry).

## Provenance

- **Toolchain scout session, 2026-07-09** — all timings/verdicts measured
  there; screenshots (retail-match diff, in-game dialogue mod, headless PNG
  dumps) live in that session's chat. Owner decision the same night:
  game-lab launches on both tracks
  ([`../prompts/game-lab-founding.md`](https://github.com/menno420/fleet-manager/blob/main/docs/prompts/game-lab-founding.md),
  [`../owner-queue.md`](https://github.com/menno420/fleet-manager/blob/main/docs/owner-queue.md) launch item).
