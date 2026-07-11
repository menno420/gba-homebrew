# gba-homebrew — game-lab Track B (public)

> **Status:** `binding` — lane identity. Seeded 2026-07-10 by the fleet manager
> from the gen-2 blueprint (`menno420/fleet-manager` `docs/gen2-blueprint.md`
> §1/§2/§2b) and the game-lab founding instruction
> (`menno420/fleet-manager` `docs/prompts/game-lab-founding.md`).

**This is Track B of the game-lab lane** — **original GBA homebrew built on
Butano, publish-safe by construction**: original code + Butano only. game-lab
is ONE lane running two GBA game tracks in two repos: this repo (Track B,
public) and `menno420/pokemon-mod-lab` (Track A, PRIVATE). One lane, per-repo
inboxes.

## ⚠ HARD RAIL — PRIVATE/PUBLIC SPLIT (non-negotiable)

This repo is public. **Never copy Track A (pokemon-mod-lab) assets or code
across** — Track A contains Nintendo-copyrighted material and lives ONLY in
its private repo. Track B stays publish-safe by construction: original code +
Butano only. **External publishing of Track B (itch.io, forums, anywhere)
still requires an owner action — queue it under ⚑ needs-owner, never perform
it.**

## Mission

Ship playable, original GBA homebrew. Agents build and verify everything
headlessly in-container — devkitARM r68 via the leseratte10 community mirror
route + make-rules/crt0 from devkitPro's GitHub sources (Butano `sprites`
example builds in 17.5s), headless mGBA loop (boot → run N frames → PNG at
~290 fps, scripted button injection) — and the owner playtests on real
devices/emulators and steers taste.

Ground truth: [`docs/findings/gba-toolchain-proof-2026-07-09.md`](docs/findings/gba-toolchain-proof-2026-07-09.md)
— every loop proven in-container by the toolchain scout session 2026-07-09.
The toolchain install is **ORDER 001 work** ([`control/inbox.md`](control/inbox.md)).
⚠ Supply-chain caveat: the devkitARM mirror is unsigned community infra —
never extend that trust to anything the owner distributes without flagging it.

## ▶ Play it

**[Lumen Drift](docs/PLAYING.md)** is playable NOW: download
[`dist/lumen-drift.gba`](dist/lumen-drift.gba) and open it in mGBA (or any
GBA emulator) — controls, goal and setup in one page:
[`docs/PLAYING.md`](docs/PLAYING.md).

## Where things live

| Path | What it is |
|---|---|
| [`dist/`](dist/) | **Download-and-play ROM builds** (sha256 + provenance in [`dist/README.md`](dist/README.md)); how to play: [`docs/PLAYING.md`](docs/PLAYING.md). |
| [`control/`](control/) | Manager↔lane coordination — `inbox.md` (manager-written orders), `status.md` (lane-written state). Protocol: [`control/README.md`](control/README.md). |
| [`docs/conventions.md`](docs/conventions.md) | Repo conventions — **override harness defaults**. Read before any PR. |
| [`docs/BUILDING.md`](docs/BUILDING.md) | One-command build recipe (`tools/setup-toolchain.sh` then `tools/build.sh`) + running the ROM in mGBA, desktop and headless. |
| [`docs/capabilities.md`](docs/capabilities.md) | What sessions CAN do (recipes, incl. the full GBA toolchain + headless mGBA loop) — read before declaring anything impossible. |
| [`docs/PLATFORM-LIMITS.md`](docs/PLATFORM-LIMITS.md) | Verified walls with exact error text — probing a documented wall twice is a bug. |
| [`docs/findings/`](docs/findings/) | Ground-truth findings mirrored from fleet-manager (the toolchain proof). |
| [`docs/review-queue.md`](docs/review-queue.md) | Post-merge review ledger — merge-then-flag, never wait-for-review. |
| [`docs/retro/QUESTIONS.md`](docs/retro/QUESTIONS.md) | Standing self-review questions, planted day 0. |
| [`claims/`](claims/) | Claim-before-build files, one per claim. |

## First session duties (in order)

1. Kit adoption: substrate-kit adopted and `python3 bootstrap.py check
   --strict` green **before any domain work**.
2. Walking skeleton: drive one trivial change through the FULL loop — clean
   build of the track's ROM + one headless mGBA screenshot committed as
   proof + branch → PR → CI → merge — before any real game work.
3. ORDER 001 in [`control/inbox.md`](control/inbox.md).
