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

## DISCOVERY RULE

Before declaring anything impossible:

1. **Check this file** and [`PLATFORM-LIMITS.md`](PLATFORM-LIMITS.md).
2. **Check `printenv`** (presence-grep above — the capability may be a
   provisioned credential).
3. **Attempt it once and capture the exact error** — verbatim, not paraphrased.

A new capability or wall discovered = **append it here the same session.** An
unrecorded discovery is a reminder the owner will have to give again.
