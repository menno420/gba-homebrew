# Session — Cindervault growth cut 5: the art pass (tile sprites + torch-radius light fade)

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/cindervault-art`, started 07:20Z,
  flipped 07:44Z — both from `date -u` at write time)
- mission: build Cindervault's LAST named growth cut from
  `games/cindervault/CONCEPT.md` — "Real art pass: tile sprites for
  walls/floor, a torch-radius light fade (the mechanic made diegetic —
  the screen literally darkens as the torch burns down)". Presentation
  ONLY: game state, RNG word order and every `cv_telemetry` word
  untouched, so ALL prior proofs must pass verbatim. The fade reuses the
  shared `games/common/include/gl_light_window.h` (Lumen Drift's
  hardware-window light circle); the tiles follow Lumen Drift's dynamic
  regular-bg idiom with original procedural indexed-BMP assets. This
  completes Cindervault's named growth path (#112 items, #116 species,
  #120 endless, #122 seed dial, this cut).
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-cindervault-art.md`, born red
   (claim before build), cut from main @ `0912a1c` (PR #126 merged).
   PR #127 opened READY.
2. Toolchain certified before touching code: the unmodified tree rebuilt
   to the committed v0.5 dist hash `0555a8c9…` EXACTLY (pinned devkitARM
   via the leseratte10 mirror — walls quoted, not re-probed;
   preinstalled Python 3.11 mgba binding reused).
3. **The assets** (`games/cindervault/graphics/generate_assets.py` +
   committed BMPs/JSONs, the Lumen Drift convention: stdlib-only,
   byte-reproducible, original art): 9 BG tiles — floor, brick wall,
   glowing ember, steel `>` stairs, lantern, blade, small/big monster —
   plus the 8x8 torch-bearer sprite and a 16-color BG palette. No color
   is exact black or white, so the harness's font-pixel text matcher
   cannot be confused by construction.
4. **The cut** (`games/cindervault/src/main.cpp` + Makefile): the glyph
   rows are REPLACED by a dynamic 32x32-cell regular background baked
   every play frame from the SAME `tiles[][]`/`monsters[]` the glyphs
   read; the player is a sprite. The torch drives the light window:
   `light_radius_px = min(16 + torch/2, 200)`, centered on the player's
   tile — the light closes in as the torch burns, re-opens +12px per
   ember, and lifts to full on title/win/lose. Sprites ride over the
   hardware window, so the HUD text and the torch-bearer stay lit (they
   carry the flame). A SECOND mailbox `cv_light[4]` ('LITE', radius,
   center x/y) publishes the presentation; `cv_telemetry`'s 18 words
   stay untouched by contract.
5. **The proofs** (`games/cindervault/proofs.sh`): **P1–P7 carried
   VERBATIM — zero re-derived pins, zero adjusted text asserts** (the
   old map glyphs were never text-asserted; the HUD never moved). +P8
   THE TORCHLIGHT: the full P2 route re-run with light + hardware
   watches — every P2 pin re-lands, and the radius follows five carried
   torch pins (220→126, 213→122, 208→120, 203→117, 198→115 px, strictly
   shrinking) while DISPCNT flips 0xF040→0xF840 (the dungeon bg's
   enable bit), BG palette RAM 0x050001EC holds 0x1A5F4A33 (ember-
   bright + wall-lit-edge in BGR555), OBJ palette RAM 0x050003E0 holds
   0x5FBF7C1F (the torch-bearer), and VRAM screenblock 0x06000800 holds
   0xF002F002 (the wall row's cells) — the art read back off the
   machine. P8b THE DEEP BURN (new route: P2 opening + ~160 waits):
   torch 163→radius 97, torch 64→radius 48 — visibly mostly dark,
   photographed. +P9 THE EMBER RELIGHT: the P6 dialed route with every
   P6 pin carried — radius 126→155 through the +3-ember sweep (the law's
   other direction), 200 on the dialed title AND the death card. **P8a,
   P8b, P9 each RUN TWICE: watch-log CSVs (including the hardware
   reads) byte-identical.** Suite exit 0: 622 watch + 104 text asserts
   (v0.5: 446 + 78, all carried).
6. `dist/cindervault.gba` v0.6 + README row: two clean builds
   byte-identical, 138,692 bytes, sha256 `db7d591824f1f1d203570e93d692
   d568e11682300f317159733d3ea558b9acf6`; the proof suite re-ran green
   at these exact bytes. CONCEPT.md's art line marked **BUILT** — the
   named growth path is COMPLETE (sellability note updated: the
   presentation is no longer placeholder).
7. Before/after evidence committed under `docs/proof/cindervault-art-*`
   (same sim state, torch 65: before = fully lit glyphs, after = a 48px
   light circle) and embedded in the PR body.
8. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold; telemetry
   `.substrate/guard-fires.jsonl` committed) + the pre-existing
   `claims-format` advisory on `order-005-scribe.md` (not this lane's
   file, advisory, left alone). Re-run green after the flip.
9. Heartbeat appended to `control/status.md` (append-only dispatch
   section, PR number confirmed first). No merge/approve/auto-merge
   calls from this session — the server-side enabler decides on green.
10. Claim `control/claims/claude-cindervault-art.md` retired with this
    flip per the claims README (the open PR is the live claim from
    here).

## 💡 Session idea

**Give presentation its own mailbox, then pin the hardware, not the
code: palette RAM, DISPCNT and the map's screenblock make an art pass as
provable as a sim change — without spending a single sim-telemetry
word.** The trap on an art cut is to "prove" it by widening the game's
telemetry mailbox (breaking the very carry contract the cut promises) or
to not prove it at all (screenshots only, unpinnable). The split that
worked: the sim mailbox stays frozen at its 18 words — that IS the
carry-verbatim witness — and a second, presentation-only symbol
publishes the derived quantities (radius, center) whose LAW the proof
asserts against already-pinned sim words (radius = min(16 + torch/2,
200) evaluated at five carried torch pins). Then go one level below the
code: the emulator's watch machinery reads GBA bus addresses just as
happily as ELF symbols, so the committed BMP's exact BGR555 colors in
palette RAM, the bg-enable bit flipping in DISPCNT with play state, and
the wall row's cells in the map's VRAM screenblock pin that the art is
IN THE MACHINE — a witness no source-level assert can fake, and one
that run-twice byte-identity extends to the whole render path for free.
The general form: presentation features should be proven as (frozen sim
words) x (law-checked derived words) x (raw hardware reads), never by
touching the sim's contract.

## Previous-session review

- Prior slice: `.sessions/2026-07-14-undertow-hazards.md` (PR #126,
  Undertow jellyfish hazards — that lane's OWN growth-path completion) —
  holds up: a sim change on a third side-band stream, with the honest
  fixed-point split (carry the seeds the feature provably cannot touch,
  re-derive the ones it legitimately moves, and say which is which).
  This cut sits at the exact opposite pole of that split: presentation
  only, so there was nothing to re-derive — the carry is TOTAL (P1–P7
  verbatim, 446+78 asserts) and the new proof surface had to come from
  somewhere other than the sim: the hardware. Its 💡 (a drifting
  creature should be a closed-form read of time, not state) transferred
  here almost literally: the light circle owns NO state — radius is a
  closed-form read of the torch and center a closed-form read of the
  player tile, so every consumer (window driver, mailbox, proof) reads
  the same expression and cannot disagree, and the "does the light
  track play?" proof is arithmetic on carried pins instead of an
  induction over frames. No erratum found in #126's entry to carry.
