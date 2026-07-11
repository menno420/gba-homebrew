# Archive-ready retro — Brineward arc · 2026-07-11 (session 20)

> **Status:** `historical` — written on the owner's wrap-up + archive-prep
> order (project chat being archived; anything not in the repo is lost).
> This file is the Brineward lane's own note; the Gloamline lane writes
> the repo-level pointer. Scope: Brineward ONLY.

## True state, one paragraph

Brineward — an original single-player pirate naval-action NDS game —
is **PLAYABLE at walking-skeleton depth and LANDED**: slice 1 (concept,
PR #53, merge `39d9630`) and slice 2 (this session's PR: the v0
skeleton) are both on main. `dist/brineward.nds` (108,032 B, sha256
`89e68dc2dd926050fdd6202a6d3b9bd8f2d82e10047453de7b41ac7b462dc475`,
byte-deterministic) boots in any DS emulator to a momentum-sailing
broadside duel against one AI sloop that provably resolves BOTH ways —
CI re-proves every PR that an idle player is sunk (restart works) and
that a recorded route sinks the enemy with the player hull untouched.
No loot, port, monsters, audio, saves, or touch yet — that is the
roadmap, not drift.

## Chat-only context, captured (verify against the concept doc first —
## most design context is already there)

- **Inspiration / owner ruling:** Brineward is an original
  single-player take on the naval-action browser-MMO genre the owner
  loved as a kid (Pirate Storm era — the LOOP is the inspiration,
  nothing else). 100% IP-clean by construction: original title
  (collision-checked 2026-07-11), art, audio, code; no external names,
  assets, or mechanics lifted verbatim. Already recorded in
  `docs/concepts/brineward-concept.md` — this bullet just marks it as
  ALSO being the owner's explicit ruling, not merely a doc convention.
- **Owner directive (chat, 2026-07-11):** "expand the games, owner
  will review soon, the further along the better" — i.e. keep advancing
  playable increments between reviews; don't park the arc waiting for
  taste passes.
- **Roadmap state at archive:** slice 1 concept DONE (PR #53, merge
  `39d9630`); slice 2 walking skeleton DONE (this PR — see
  `docs/PLAYING-BRINEWARD.md` for the honest v0 gaps); slices 3+ NOT
  STARTED, in concept-doc order: loot/gold -> port + upgrades -> first
  sea monster (the Maw) -> wind/sailing feel (owner-taste) -> danger
  bands/regions -> synthesized audio -> saves (NDS save persistence
  still unprobed in this repo, flagged in the concept doc).

## Exactly how a fresh session resumes this arc

1. **Read first:** `docs/concepts/brineward-concept.md` (design + slice
   roadmap), `docs/PLAYING-BRINEWARD.md` (what v0 is), this file,
   `control/README.md` (ritual), `docs/PLATFORM-LIMITS.md` (walls +
   the three py-desmume quirks that WILL bite: KEYINPUT frame-1 boot
   trap -> `keypad_update(0)`; screenshots are BGRX; affine OAM sprites
   have no hide bit).
2. **Branches/sessions:** work on `claude/brineward-<slice>` branches;
   session cards `.sessions/YYYY-MM-DD-session-N.md` born red, one
   claim file per branch in `control/claims/`, deleted at close.
   Sessions 19 (concept) and 20 (skeleton) are Brineward's history.
3. **Toolchain:** `tools/setup-nds-toolchain.sh` (BlocksDS 1.21.1 +
   Wonderful GCC, sha256-pinned; installs to `/opt/wonderful`), then
   `make -C games/brineward-nds`; build is byte-deterministic — refresh
   `dist/brineward.nds` + its README row in any PR that changes the
   game.
4. **Layout:** pure rules ONLY in `games/brineward-nds/source/bw_sim.c`
   (fixed-point, no libnds), mirrored line-for-line by
   `tools/check-brine.py` — **change both in the same PR or CI fails**.
   `main.c` = input/state machine/rendering/`bw_telemetry[16]` mailbox
   (word layout at the `BW_T_*` defines). Negative values are divided
   only by `>> 8` so C and Python agree bit-for-bit — keep that rule.
5. **Proofs:** CI job `nds-brineward-build` in
   `.github/workflows/rom-builds.yml` (NEVER touch Gloamline's
   `nds-rom-build` job or the required GBA "ROM builds" gate). Headless
   runs via `tools/nds-headless-check.py --elf ... --watch
   t:bw_telemetry:16 --assert-watch FRAME:t:IDX:OP:VALUE`. Frame
   mapping (empirically pinned, re-asserted by the latched seed every
   run): START held over tool frames `[A, A+5)` latches seed `A-2`;
   telemetry at tool frame `T` shows `T-A` completed duel steps; a key
   held at tool frame `T` drives duel step index `T-(A+1)`.
6. **Duel-win route:** regenerate with
   `games/brineward-nds/tools/record-duel-win.py` (closed-loop policy
   `bw_policy` in check-brine.py, open-loop verified, ±6-frame
   alignment margin required) whenever ANY sim constant changes; CI
   diffs the committed route against a fresh recording, so a stale
   `games/brineward-nds/proof/duel-win-keys.txt` fails fast. Seed 126
   was chosen by scanning press frames 116-144 for full shift
   robustness — most seeds are chaos-fragile (the enemy AI reacts
   closed-loop, so ±1 frame changes the whole fight); re-scan if the
   sim changes.

## Open questions (carried)

- NDS save persistence (slice 7) unprobed — probe DeSmuME headless
  save-flash EARLY, before promising saves (lesson from the GBA mGBA
  savefile segfault).
- Enemy AI feel is provable-first numbers; owner taste pass pending
  (same standing invitation as Gloamline's Shambler tuning).
- Two NDS CI jobs per PR now — if CI minutes bite, path-filter the two
  NDS jobs per game dir as a deliberate workflow change (concept-doc
  flag, not yet needed: warm-cache job cost is ~1 min each).
- Repo name `gba-homebrew` now hosts one GBA + two NDS games — rename
  is the owner's call, nothing depends on it.
