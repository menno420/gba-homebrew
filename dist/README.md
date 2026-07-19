# `dist/` — download-and-play ROM builds

Prebuilt, committed copies of the lane's original ROMs so the owner can
**download one file and play** — no toolchain needed. Everything here is
100% original content (original code + open-source engine/SDK: Butano for
GBA, BlocksDS/libnds for NDS), so committing the binary is publish-safe by
construction. How to play: [`docs/PLAYING.md`](../docs/PLAYING.md) (Lumen
Drift, GBA) · [`docs/PLAYING-GLOAMLINE.md`](../docs/PLAYING-GLOAMLINE.md)
(Gloamline, NDS) · [`docs/PLAYING-BRINEWARD.md`](../docs/PLAYING-BRINEWARD.md)
(Brineward, NDS).

Only player-facing games ship here (the engine-test `skeleton.gba` stays
CI-artifact-only). Each entry below is refreshed in the same PR that
changes its game, with provenance:

| File | Version / date | Size | Built from | sha256 |
|---|---|---|---|---|
| [`lumen-drift.gba`](lumen-drift.gba) | **v1.3** · session 8 slice 6 · 2026-07-11 | 167,996 bytes | source tree of the v1.3 pause/mute + graze-flash micro-polish commit (`games/lumen-drift/` at the same tree as this file) | `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94` |
| [`gloamline.nds`](gloamline.nds) | **best-night rematch** (arc slice 11) · session 2026-07-13-gloamline-rematch | 118,272 bytes | source tree of the slice-11 rematch commit (`games/gloamline-nds/` at the same tree as this file); build is byte-deterministic (two clean builds + an independent fresh-worktree build, identical hash) | `3bab5544c03d01e358fd1f8898df47031bb3f48107fb1469551ab7ed794ec061` |
| [`brineward.nds`](brineward.nds) | **bestiary + crate-cap economy** (bestiary arc CLOSER, `games/brineward-nds/` — the Grasper / Ram-brace / ambush cutters / the-hold-track crate cap, cuts 1–4 landed via #176 + #179 `37c4e99`) · **rebuilt 2026-07-19** (was slice-9 saves build, 121,344 B `39c5adcb…`, which predated the #179 arc-closer) | 123,392 bytes | CI `rom-builds.yml` run [`29636406346`](../../actions/runs/29636406346) artifact `nds-brineward-40779f8d…` (id `8427255469`, head `40779f8d`, contains #179 `37c4e99`; both game sources' last change is at-or-before this head, so current-main source); sha256 matches the run's `brineward-sha256.txt` | `cc3ef5431fec7b1f84fd995fb1701b2693499f05803902e3ebbf5f1270b4a1b4` |
| [`underroot.nds`](underroot.nds) | **v1.0** (arc slices 1–11, #155–#165, `games/underroot-nds/` — dual-screen skeleton → meadow food patches → seasons/dig/dial → SRAM saves → synthesized PSG audio + HUD polish; slice-11 closer `40779f8` #165) · **first committed 2026-07-19** (Underroot reached v1.0 with no downloadable ROM until now) | 104,960 bytes | CI `rom-builds.yml` run [`29636406346`](../../actions/runs/29636406346) artifact `underroot-nds-40779f8d…` (id `8427259533`, head `40779f8d` = #165, ancestor of `main` HEAD `9dbde10`; Underroot source last-change IS this head, so current-main source); sha256 matches the run's `underroot-sha256.txt` | `7202415374088ce3f8472bafb4d6f69684b4c79434767bd0bcf0496d5e9b4f72` |
| [`cindervault.gba`](cindervault.gba) | **v0.6 the art pass** (growth cut 5, the LAST named growth line: tile-sprite dungeon — original procedural assets, `games/cindervault/graphics/` — a torch-bearer player sprite, and a torch-radius light fade via the shared `gl_light_window.h` (radius = min(16 + torch/2, 200) px: the screen literally darkens as the torch burns, and re-opens on embers); presentation ONLY — sim, RNG and all 18 telemetry words untouched, P1-P7 carried verbatim; the art itself proven in `games/cindervault/proofs.sh` P8/P9 via a second `cv_light` mailbox + DISPCNT/palette-RAM/VRAM pins) · 2026-07-14 | 138,692 bytes | source tree of the Cindervault art-pass commit (`games/cindervault/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `db7d591824f1f1d203570e93d692d568e11682300f317159733d3ea558b9acf6` |
| [`deepcast.gba`](deepcast.gba) | **v0.6 the art pass** (growth cut 5, the LAST named growth line — the named growth path is COMPLETE: a ten-band lake background — original procedural assets, `games/deepcast/graphics/` — whose palette follows a closed-form law of the live depth word (dim = 2*band + depth/8: the whole lake deepens as the lure sinks), a silhouette fish whose frame IS the species index (band = size, rarity = shade; hidden while sinking and on a SNAP — the species keeps its secret), and a rod-bend sprite, bend = tension*7/snap — the audio ratchet's own law, honest on any line tier; presentation ONLY — sim, RNG and all three sim mailboxes untouched, P1-P7 carried verbatim; the art itself proven in `games/deepcast/proofs.sh` P8-P10 via a fourth `dc_artmeta` mailbox + DISPCNT/palette-RAM/VRAM pins) · 2026-07-14 | 159,928 bytes | source tree of the Deepcast art-pass commit (`games/deepcast/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `ab9c026b889e92466da3e18ffda966cfefe5a068d8d7546f424115d5b503e777` |
| [`clockwork-courier.gba`](clockwork-courier.gba) | **v1.0 CONCEPT COMPLETE** (growth rung 4: six synthesized cues, hook-counted + mixer-proven; every named concept item now built — rewind ghost, co-op door, ghost platform, timed chutes, three floors, audio) · session 54 · 2026-07-14 | 145,752 bytes | source tree of the Courier audio commit (`games/clockwork-courier/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `ad73419f71cee59236fcd8c3c8efe59c26ab4f7e54c7bcdbcf8d486e3afcc75c` |
| [`wickroad.gba`](wickroad.gba) | **v1.0 the best ledger** (crossroads arc cut 4 — THE ARC CLOSER, `games/wickroad/ARC-CROSSROADS.md`: the best run is now banked across power cycles in SRAM behind the house save header `gl_save.h` (POD `{best_gold, best_day_reached, best_seed, runs}` behind magic tag `WLDGR1`), the score-attack hook the seed dial's fair repeatable worlds enable. The ledger loads ONCE at boot (a fresh / foreign / erased cart reads as NO save → a zero ledger, NOT reset by `reset_run()`), is written to SRAM the instant a run ENDS (win or loss), and survives a power cycle — the persisted best gold renders on the end cards, and START from a win/lose card reruns the SAME dialed world to beat it. THE CONTRACT: the default no-save path is render-cost + timing BYTE-IDENTICAL to cut 3 — the best line is drawn ONLY when a prior save was restored (`ledger_loaded`), which never happens on a fresh cart, so every committed proof P1-P11 (all run WITHOUT `--savefile`) carries VERBATIM. Zero new RNG draws; `wr_telemetry` UNCHANGED at 57 words — the ledger lives in a THIRD witness mailbox `wr_ledger[6]` (`'WLDG'` magic + best fields + a loaded flag), the same discipline cut 2 used for `wr_art`. Proven in `games/wickroad/proofs.sh` P12, the Deepcast SRAM power-cycle pattern: fresh-cart baseline (loaded flag 0) · write + persist (P2's WIN_ROUTE banks gold 328/day 13 into a `.sav`) · power-cycle restore (a separate boot restores it on the title before any input, `.sav` untouched) · the restored `BEST GOLD 328` line renders on the win card — run twice byte-identical, watch-logs AND the written `.sav` files) · **PLUS the run-milestone flourish + persistent VETERAN/MASTER tiers + the end-card tier-up flash** (`wr_milestones.h` `run_tier_up_label`; #189/#190/#195/#201) · **rebuilt 2026-07-19** | 182,860 bytes | CI `rom-builds.yml` run [`29678377739`](../../actions/runs/29678377739) artifact `gba-roms-7c811db6…` (head `0df73464`, contains #201; `git diff HEAD 0df73464 -- games/wickroad/` is empty, so byte-identical to the `main` HEAD tree); build is byte-deterministic (sha matches the run's `rom-sha256.txt`) | `c7e2814ee1e316e4e2cb2272caaf3dd35341f42ead80923640fd8b669a2fcb20` |
| [`shoal.gba`](shoal.gba) | **v1.0 CONCEPT COMPLETE** (growth rung 5: five synthesized cues, hook-counted + mixer-proven; every named concept item now built — flock, current, reef, predators, gates, ratings, four tuned levels, audio) · session 50 · 2026-07-13 | 155,464 bytes | source tree of the Shoal audio commit (`games/shoal/` at the same tree as this file); build is byte-deterministic (two clean builds, identical hash) | `29a67ab4b5891c9133479de23781ac011ed833ba2f72776259bf2df2193b6d86` |

## Web arcade (`web/`) + release zips (`releases/`)

`web/` is the host-ready static arcade (landing page + `undertow/`,
`tiltstone/`, `drift-garden/` — runtime files only, per each game's
hosting contract); `releases/` holds deterministic versioned zips.
Both are regenerated by `tools/package-web-arcade.sh` and pinned in
`docs/RELEASES.md` (assert with `--verify`). Repinned from main by the
2026-07-14 ORDER 006 slice after the Tiltstone stack (#92-#97) landed
(Tiltstone v1.1 with `juice.js`, Undertow v1.5, Drift Garden v1.4,
arcade bundle v1.3 — superseding the #130 arcade-refresh cut, which
superseded #109, which superseded the stale #85 cut).

**Hosted arcade (GitHub Pages):** once enabled, the arcade lives at
<https://menno420.github.io/gba-homebrew/> — the `web/` landing page
plus all three games, straight from a browser or phone, no build tools.
Publishing is `.github/workflows/deploy-pages.yml`, which uploads
`dist/web/` ONLY (never ROMs or zips). A redeploy triggers on any
merged change under `dist/web/`, or manually via Actions → "Deploy web
arcade to Pages" → Run workflow. One-time owner setup: repo Settings →
Pages → Build and deployment → Source = **GitHub Actions** (applies
immediately, no save button).

**How they were built:** GBA — `tools/setup-toolchain.sh` (pinned
devkitARM r68 + Butano) then `tools/build.sh`; NDS —
`tools/setup-nds-toolchain.sh` (pinned BlocksDS 1.21.1 + Wonderful GCC)
then `make -C games/gloamline-nds`. Exact recipes in
[`docs/BUILDING.md`](../docs/BUILDING.md). CI rebuilds both ROMs from
source on every PR and logs their sha256 ("ROM builds" workflow — the NDS
job also prints the committed `dist/gloamline.nds` hash next to the
from-source one), so the committed binaries are always cross-checkable
against a from-source build.
