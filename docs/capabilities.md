# gba-homebrew capability manifest (game-lab Track B)

> **Status:** `living-ledger` — this lane's copy of the fleet capability
> manifest (`menno420/fleet-manager` `docs/capabilities.md`, carried over at
> seed 2026-07-10). **Read this before declaring anything impossible.** A new
> capability or wall discovered = append it here the same session. Verified
> walls with exact error text live in [`PLATFORM-LIMITS.md`](PLATFORM-LIMITS.md)
> — **probing a documented wall twice is a bug.**

## CAN — capabilities sessions routinely deny having (with the recipe)

### Build + test GBA ROMs entirely in-container (toolchain scout 2026-07-09)
Sessions would assume a GBA toolchain/emulator loop is impossible headless. It
isn't — all three routes proven; full evidence record mirrored at
[`findings/gba-toolchain-proof-2026-07-09.md`](findings/gba-toolchain-proof-2026-07-09.md):

- **devkitARM/Butano (THIS track):** official devkitPro installers are WALLED
  (Cloudflare-403 — see PLATFORM-LIMITS.md); use the **leseratte10 community
  mirror** (`https://wii.leseratte10.de/devkitPro/`) — extract the devkitARM
  r68 linux_x86_64 `.pkg.tar.zst` packages (`devkitARM-r68`,
  `devkitarm-binutils`, `devkitarm-gcc`) into `/opt/devkitpro`, then build
  make-rules + crt0 from devkitPro's GitHub sources (devkitarm-rules,
  devkitarm-crtls). Butano `sprites` example compiles in **17.5s**.
  ⚠ unsigned community infra — supply-chain caveat.
- **pokeemerald (Track A, PRIVATE repo only):** `apt install
  binutils-arm-none-eabi` + **agbcc** built per pokeemerald's `INSTALL.md`
  (clone pret/agbcc → `build.sh` → `install.sh <pokeemerald>`) →
  byte-identical retail build; full 1m20s, incremental 2.0s. That material
  NEVER enters this public repo.
- **Headless emulator loop:** `apt install mgba-sdl` + `pip install
  mgba==0.10.2` — the pip binding **must stay pinned to the system libmgba
  0.10.x** (0.10.2 ↔ libmgba 0.10.x verified; drifting either side breaks the
  ABI). Boot ROM → run N frames → dump PNG at **~290 fps** headless; scripted
  button injection verified changes live in-game — the full agents-only
  verify loop (no human, no display) is proven end-to-end.
- **Headless battery-save persistence (SRAM power cycles, session 5):** a GBA
  savefile is just the raw contents of the cartridge save memory — for a
  Butano `bn::sram` ROM (Butano embeds the `SRAM_V113` marker, which is how
  mGBA detects the save type) that's **32KB mapped at GBA bus address
  `0xE000000`**, so byte N of a `.sav` file IS SRAM byte N. The harness's
  `tools/headless-screenshot.py --savefile game.sav` uses exactly that
  mapping: after `core.reset()` it copies the file into the region via
  `core.memory.sram.u8` (= power-on with that battery; missing file = 32KB
  of `0xFF`, factory-erased SRAM) and after the last frame snapshots the
  region back to the file. Boot the same savefile in two separate emulator
  processes + `--assert-text` the title's BEST line to prove power-cycle
  persistence headlessly. ⚠ do NOT use the binding's native
  `core.load_save()` + VFile instead — it segfaults after the game's first
  SRAM write (verbatim error in [`PLATFORM-LIMITS.md`](PLATFORM-LIMITS.md)).
- **Headless AUDIO evidence (session 6):** screenshots cannot hear, but the
  binding's `core.memory.u32[bus_address]` reads the WHOLE emulated bus
  (the `GBAMemory` object spans 4GB from base 0 via `busRead32` — the same
  reads work on IWRAM/EWRAM/IO, not just the named regions). Two memory
  facts make sound assertable, both resolved from the game ELF's symbol
  table (`--elf`, a stdlib ELF32 parser in the harness):
  1. an **in-ROM trigger-counter hook** (`games/common/gl_audio_hook.h`, a
     volatile `unsigned[8]` at the unmangled symbol `gl_audio_hook`): the
     game bumps one cumulative counter per sound it plays — counters prove
     WHICH sound fired and how many times, robust to replay offsets;
  2. **Butano/maxmod's mixing buffer** (mangled local symbol
     `…maxmod_mixing_buffer…`, matched by unique substring; 0x420 bytes in
     IWRAM for this ROM): maxmod zero-fills it when no voice is active, so
     the **count of nonzero u32 words is 0 exactly when the mixer is
     silent** and >0 while any effect is actually being voiced (empirically
     verified: silent title = 0, one effect ≈ 264 nonzero words, back to 0
     the frame after the last voice ends). Register-level sanity on top:
     `busRead32(0x04000084)` (SOUNDCNT_X) reads 128 = master enable on.
  Harness flags: `--watch NAME:ADDR:NWORDS` (u32 dump), `--watch-nonzero
  NAME:ADDR:NBYTES` (mixer activity), `--watch-log CSV` (committable
  per-frame evidence artifact), `--assert-watch FRAME:NAME:IDX:OP:VALUE`.
  Asserting hook counters AND mixer activity at must-play vs must-be-silent
  frames = the audio proof (live in `.github/workflows/headless-boot.yml`;
  evidence log `docs/proof/session-6-audio-watch-log.csv`). A spare hook
  slot doubles as a game-loop tick counter — comparing it against hardware
  frames measures vblank overruns (how the session-6 constant +1 replay
  shift was diagnosed: the run-start rebake stall grew one frame, zero
  overruns elsewhere).

### View video / audio files (.mp4, .webm, .mov, .mp3, …)
Sessions claim they can't view an .mp4. They CAN:

```bash
ffprobe -v error -show_format -show_streams <file>          # inspect codecs/duration first
ffmpeg -i <file> -vf "fps=0.5" -q:v 2 frames_%03d.jpg       # extract frames into the scratchpad
```

Then `Read` the extracted frames as images — one frame every 2 seconds at
`fps=0.5`; raise/lower the fps for denser/sparser sampling. Audio tracks:
extract with `ffmpeg -i <file> -vn audio.wav` and process from there.

### View images and PDFs
`Read` them directly — the Read tool renders images visually and reads PDFs
page-by-page (`pages` parameter). No conversion step needed.

### Use provisioned secrets
Sessions forget tokens exist. **CHECK THE ENVIRONMENT FIRST:**

```bash
printenv | grep -iE 'token|key|railway|discord'
```

Confirm **presence only** (names, not values) — **never echo full secret
values into logs, files, or transcripts**. Use them via the env var.

### First commit to an empty repo
`git push` to a truly empty repo fails through the proxy tooling. Make the
first commit via the **Contents API** (`create_or_update_file` / `push_files`)
— that creates `main`, and normal git works from then on. (Fleet playbook R13;
this repo was bootstrapped exactly this way, 2026-07-10.)

### Arm auto-merge while checks are pending
GitHub refuses to arm auto-merge on an already-green PR — arm it **at PR
creation, in the checks-pending window** (`enable_pr_auto_merge`). This is the
sanctioned merge path where a check can go pending; on a no-CI or born-red
repo REST merge-on-green is PRIMARY (fleet playbook R5/R12/R21). See the
self-merge wall in [`PLATFORM-LIMITS.md`](PLATFORM-LIMITS.md).

### Run any repo's own checkers locally
Clone (or fetch) the repo and run its own gates — `bootstrap.py check
--strict`, the ROM build, pytest suites — before pushing. Nothing restricts a
session to the repo it was launched for.

### Read files from other public repos without extra scope
`WebFetch` on `https://raw.githubusercontent.com/<owner>/<repo>/<ref>/<path>`
works cross-repo for public content — no token scope or `add_repo` needed for
read-only single-file pulls.

### Spawn subagents for parallel work
The Agent tool runs research/implementation/verification workers concurrently
— fan out independent lanes instead of serializing them. (Worker agents
themselves don't re-spawn; the manager/coordinator tier does.)

### Watch something over time
Use **blocking foreground waits** — `until [ $(date +%s) -ge $end ]; do sleep
5; done` — never background timers. Background timers silently drop the final
report. (Fleet playbook R4.)

### YouTube transcripts
`youtube_transcript_api` is IP-blocked from datacenter IPs — sessions conclude
transcripts are impossible. They aren't:

```bash
yt-dlp --skip-download --write-auto-sub <video-url>
```

works via its android-vr endpoint; then parse the resulting `.vtt` file.
(Discovered 2026-07-09, transcript+miner task.)

### Reproduce this repo's full GBA toolchain with one script (session 1, 2026-07-10)
`tools/setup-toolchain.sh` is the pinned, idempotent install (devkitARM r68 +
Butano 21.7.1 + crtls v1.2.7 + grit/mmutil/gba-tools/general-tools); it works
both in-container (proxy; curl honours the preset `CURL_CA_BUNDLE`) and on
GitHub-hosted runners (set `DEVKITPRO` to a user-writable dir there — the CI
workflows use `/home/runner/devkitpro` so `actions/cache` can restore without
sudo). Gotchas learned installing it:

- **newlib is NOT in the mirror's r68 directory** — r68 pairs with
  `devkitarm-newlib-4.6.0.20260123-5` from the **r67** directory. Without it
  the Butano build dies with `fatal error: assert.h: No such file or
  directory`.
- **crtls has no mirror package** — build `devkitPro/devkitarm-crtls` from
  source (its Makefile includes `$(DEVKITARM)/base_rules`, which in turn
  needs `DEVKITPRO` set) and copy `*.specs *.ld *.o thumb/ armv6k/` into
  `$DEVKITARM/arm-none-eabi/lib`.
- Skeleton ROM clean build: **~12.5s** on 4 cores (`make -j$(nproc)`);
  toolchain install from a warm mirror: **~9-13s**.
- **Headless harness:** `tools/headless-screenshot.py` boots a ROM in mGBA,
  runs N frames, dumps a PNG, and fails if the frame is blank (unique-color +
  variance floor). Call `mgba.log.silence()` first — the binding otherwise
  floods stderr with per-DMA debug lines.

### Read public GitHub repos when api.github.com is walled
In-container, `api.github.com` REST calls to repos outside the session are
proxy-blocked (wall recorded in PLATFORM-LIMITS.md) — but plain
`git ls-remote` / `git clone` of any public `github.com/<owner>/<repo>.git`
**works**. Use git for cross-repo tag/SHA pins instead of the REST API.

## DISCOVERY RULE

Before declaring anything impossible:

1. **Check this file** and [`PLATFORM-LIMITS.md`](PLATFORM-LIMITS.md).
2. **Check `printenv`** (presence-grep above — the capability may be a
   provisioned credential).
3. **Attempt it once and capture the exact error** — verbatim, not paraphrased.

A new capability or wall discovered = **append it here the same session.** An
unrecorded discovery is a reminder the owner will have to give again.
