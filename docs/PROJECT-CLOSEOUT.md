# gba-homebrew — Project Closeout

> **Status:** `reference`
>
> Public-facing wind-down page for the `menno420/gba-homebrew` project. Written
> for two readers who start with **zero** prior context: the **owner**, and a
> **fresh future Claude session** that boots from `git clone` with no
> conversation history. Everything below was verified live at main HEAD
> `3377319` (#213) on 2026-07-21 (`date -u`), not recalled from memory — each
> state claim carries a command, a PR/commit, or a live probe you can re-run.
> This project was built over an **autonomous development period**: a series of
> autonomous agent sessions that opened PRs, proved them in CI, and landed them
> on green. That period ends 2026-07-22; after it, only what is committed here
> survives. This page is the durable handoff.

---

## (a) What this project is & what was accomplished

**What it is.** `gba-homebrew` is an original-IP homebrew game lab: a single
repository that builds real, playable **Game Boy Advance** (Butano engine) and
**Nintendo DS** (BlocksDS/libnds) ROMs, a hostable **web arcade** of browser
games, and an Android **phone-controller** companion lane — all from source,
all proven by committed, replayable CI harnesses. Every asset is 100% original
(original code + open-source engine/SDK), so committing the built binaries is
publish-safe by construction.

Every major shipped thing, each cited to the PR/commit that landed it
(verified present at HEAD `3377319`):

**Games & arcs**

- **Lumen Drift** (GBA, gravity cave drifter) — the founding game, taken to
  **v1.3** across sessions 1–8 (PRs **#2–#29**: endless echo cave, depth tiers,
  graze risk/reward, pause/mute, SRAM best, synthesized audio). It is the only
  title with a published **GitHub Release** (`lumen-drift-v1.3`).
- **Wickroad** (GBA, market-route trading with an aging ledger) — named growth
  cuts 1–5 (**#142–#146**) to v1.0, then the **crossroads arc** cuts 1–4
  (**#172–#175**, arc closer `7e30756` #175: branch roads, authored sprite art,
  a world-seed dial, and an SRAM best-run ledger). Then the end-card polish
  stack: run-milestone flourish (**#189**), persistent **VETERAN** tier
  (**#190**), **MASTER** tier (**#195**), and the **tier-up flash** that
  announces a tier the run it is first crossed (**#201**, merge `6d1d97a`). The
  downloadable `dist/wickroad.gba` was refreshed to this post-#201 build in
  **#206** so the download actually shows the banner.
- **Brineward** (NDS, pirate broadside duel) — the **bestiary arc**: cut 1 «The
  Grasper» landed **#176** (merge `c338301`), and the arc closer **#179**
  (`37c4e99`) carried cuts 2–4 (Ram/brace verb, the ambush, the hold-track
  economy) together; the intermediate PRs **#177/#178** were closed as subsumed
  by #179. `dist/brineward.nds` was committed/refreshed to this build in **#209**.
- **Underroot** (NDS, dual-screen colony — game #12, the marquee) — reached
  **v1.0** across arc slices 1–11 (**#155–#165**, closer `40779f8` #165:
  dual-screen skeleton → meadow food patches → seasons/dig/dial → SRAM saves →
  synthesized PSG audio + HUD polish). `dist/underroot.nds` was first committed
  in **#209**.
- **Tiltstone** (web, turn-based gravity puzzle) — v1.1, then **arc 2 «the
  shareable daily»**: the pure engine reached **v1.8.0** on `main` with all five
  cuts (share-a-line replay, solver hints, undo×par deception, mechanic
  fingerprints, monotone difficulty ramp) — `c654e01`, `46293b5`, and `207e391`
  (**#170**, arc closer, carrying cuts 3+4); `node games/web-tiltstone/smoke.mjs`
  passes.
- **The Android / Gradle controller lane** (cross-repo, in `menno420/product-forge`)
  — a phone-as-BT-HID-controller companion app: capability engine + Android HID
  transport (**pf #27/#28**), an app module wired into a Gradle build with
  `:app:assembleDebug` green (**pf #31**), and an assembleDebug CI lane
  (**pf #32**). Remaining item is an owner hardware playtest (phone + physical
  BT-HID receiver).
- **The rest of the catalogue** (all original IP, all with committed ROMs or web
  builds): **Gloamline** (NDS zombie horde-defense, concept tree complete,
  **#50–#75**), **Undertow** (web one-hand trench diver, v1.5), **Drift Garden**
  (mobile ambient PWA, v1.4), **Deepcast** (GBA fishing, v0.6), **Cindervault**
  (GBA dungeon dive, v0.6), **Clockwork Courier** (GBA rewind-ghost platformer,
  v1.0), **Shoal** (GBA flock-herding puzzler, v1.0).

**Web arcade**

- A host-ready static arcade in `dist/web/` (landing page + Undertow, Tiltstone,
  Drift Garden), packaged into deterministic versioned zips in `dist/releases/`
  and pinned in [`RELEASES.md`](RELEASES.md). Published via the Pages deploy
  workflow (`.github/workflows/deploy-pages.yml`, **#149**) — **live** (see §b).

**Committed downloadable ROMs**

- `dist/` carries **9 player ROMs** (6 GBA + 3 NDS) so the owner can download one
  file and play with no toolchain. The full provenance table — version, size,
  build source, sha256 — is [`dist/README.md`](../dist/README.md). Exact
  contents + hashes are enumerated in §b.

**CI proof harnesses** (the evidence bar every slice cleared)

- **`ROM builds`** (`.github/workflows/rom-builds.yml`) — the ONE required
  status check: rebuilds every `games/*/Makefile` ROM from source on each PR and
  logs sha256s, so committed binaries are always cross-checkable.
- **Headless-boot** (`headless-boot.yml`) — builds the skeleton + Lumen Drift and
  boots them headless in mGBA with scripted-input replay assertions.
- **Host mirrors** — each GBA game ships `games/<game>/proofs.sh`; the NDS games
  mirror their whole sim host-side (`tools/check-brine.py`, `tools/check-gloam.py`,
  `tools/check-underroot.py`), driven in CI.
- **Web smoke** — pure-Node engine smokes plus real-Chromium browser smokes
  (`tools/web-smoke-undertow.mjs`, `tools/web-smoke-tiltstone.mjs`).

**Autonomous landing infrastructure**

- **`.github/workflows/auto-merge-enabler.yml`** — the merge-on-green backstop.
  On a READY (non-draft) `claude/*` PR it arms GitHub-native squash auto-merge,
  so the PR merges itself the moment the required `ROM builds` check goes green —
  no human click. This is how the whole autonomous period landed its work. (It
  is **not** named `merge-on-green.yml`; that earlier name was retired.)

---

## (b) Current true state (verified live at HEAD)

All checks below were run on 2026-07-21 (`date -u`) against main HEAD.

- **HEAD SHA:** `3377319` (#213). Confirm:
  `git ls-remote origin refs/heads/main` → `337731906098d1ba596dcaa1750299e45c737ed6`.
  `git log --oneline -1` → `3377319 control: 2026-07-21 final-day roll-up stamp (#213)`.
- **Open PRs: 0.** `list_pull_requests state=open` → `[]` (github MCP, live).
- **`dist/` committed ROMs** (`ls dist/`; each sha256 = `sha256sum`, matches
  [`dist/README.md`](../dist/README.md)):

  | ROM | Platform | Size (bytes) | sha256 |
  |---|---|---|---|
  | `lumen-drift.gba` | GBA | 167,996 | `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94` |
  | `wickroad.gba` | GBA | 182,860 | `c7e2814ee1e316e4e2cb2272caaf3dd35341f42ead80923640fd8b669a2fcb20` |
  | `cindervault.gba` | GBA | 138,692 | `db7d591824f1f1d203570e93d692d568e11682300f317159733d3ea558b9acf6` |
  | `deepcast.gba` | GBA | 159,928 | `ab9c026b889e92466da3e18ffda966cfefe5a068d8d7546f424115d5b503e777` |
  | `clockwork-courier.gba` | GBA | 145,752 | `ad73419f71cee59236fcd8c3c8efe59c26ab4f7e54c7bcdbcf8d486e3afcc75c` |
  | `shoal.gba` | GBA | 155,464 | `29a67ab4b5891c9133479de23781ac011ed833ba2f72776259bf2df2193b6d86` |
  | `gloamline.nds` | NDS | 118,272 | `3bab5544c03d01e358fd1f8898df47031bb3f48107fb1469551ab7ed794ec061` |
  | `brineward.nds` | NDS | 123,392 | `cc3ef5431fec7b1f84fd995fb1701b2693499f05803902e3ebbf5f1270b4a1b4` |
  | `underroot.nds` | NDS | 104,960 | `7202415374088ce3f8472bafb4d6f69684b4c79434767bd0bcf0496d5e9b4f72` |

  All 9 are downloadable now; each `.../blob/main/dist/<rom>` URL returns HTTP 200.
- **Web arcade: LIVE.** `GET https://menno420.github.io/gba-homebrew/` → **HTTP 200**.
- **Lumen Drift Release: published.** `get_release_by_tag lumen-drift-v1.3` →
  `draft:false`, published 2026-07-18T20:07:16Z, asset `lumen-drift.gba`
  (167,996 B, sha256 `195a8679…`). Page:
  <https://github.com/menno420/gba-homebrew/releases/tag/lumen-drift-v1.3> → HTTP 200.
- **Menu — remaining real choices: A1 Tinderhand + A3 Starloom only.** The
  2026-07-19 menu-vs-tree audit (**#208**) verified every option in
  [`NEXT-MENU-2026-07-15.md`](NEXT-MENU-2026-07-15.md) against the tree:
  **A2** (Underroot), **B1** (Tiltstone arc 2), **B2** (Wickroad crossroads),
  **B3** (Brineward bestiary) are all **BUILT**; **C** (Lumen Drift release +
  arcade) is **DONE/published**. Only **A1 (Tinderhand)** and **A3 (Starloom)**
  are genuine unbuilt proposals — no `games/tinderhand*` / `games/starloom*` dir
  exists (both are pure proposals with executable pre-plans; see §c).

---

## (c) Continuation — open threads in priority order

Each thread names the exact file and the first resume step. Nothing here is
blocked on missing code inside the repo except where an **owner verdict** is
noted.

**1. Build the next game — A1 Tinderhand and/or A3 Starloom (the two open
choices).** Both have complete, executable slice ladders already written; a
fresh session can start slice 1 immediately once the owner picks a letter.

- **A1 Tinderhand** (GBA press-your-luck card game) —
  [`docs/plans/tinderhand-preplan-2026-07-20.md`](plans/tinderhand-preplan-2026-07-20.md).
  *First resume step:* open the pre-plan, execute **slice 1** (the walking
  skeleton) as a born-red `claude/*` PR — host `tools/check-tinderhand.py` mirror
  + `rom-builds.yml` build + `headless-boot.yml`, per the pre-plan's per-slice
  proof; the arc closer commits `dist/tinderhand.gba` with sha256 provenance.
- **A3 Starloom** (browser/PWA daily constellation puzzle) —
  [`docs/plans/starloom-preplan-2026-07-20.md`](plans/starloom-preplan-2026-07-20.md).
  *First resume step:* open the pre-plan, execute **slice 1** — this is a **web**
  game, so its proof mold is `node smoke.mjs` + `tools/web-smoke-starloom.mjs`
  (mirroring `web-smoke-tiltstone.mjs`) + the Pages deploy check; it ships to
  `dist/web/`, **no ROM**. (The GBA ROM/headless template does not apply.)

  Neither pre-plan selects its option — the owner's letter choice governs.

**2. Wickroad GRANDMASTER≥200 tier** — behind an owner playtest verdict. The
tier table today is VETERAN≥50 / MASTER≥100 (`games/wickroad/src/wr_milestones.h`);
GRANDMASTER≥200 is the same one-row pattern, but it is **gated on evidence that
players actually reach 200 lifetime runs** (a playtest/telemetry signal), so it
is not a headless-only pickup. *First resume step:* get the owner's Wickroad
playtest verdict (thread 3), then, if warranted, add the `runs>=200 → GRANDMASTER`
row + host boundary test in `tools/check-run-tier.py`.

**3. Playtest verdicts for the downloadable games** — owner action, no code. The
downloadable ROMs (`wickroad.gba`, `underroot.nds`, `brineward.nds`) and the
released `lumen-drift.gba` are waiting on a one-line owner "feels good / change
X" verdict to unblock further polish. *First resume step:* see the owner
checklist in §d; a verdict routes to the relevant game's arc doc.

**4. (Optional) Publish Lumen Drift to itch.io** — an off-repo owner action. The
Release is already published on GitHub (§b); itch.io is a wider-distribution
stretch, not a code task. *First resume step:* upload the released
`lumen-drift.gba` to an itch.io page (owner-side).

**5. (Optional) Browser-shell / touch-controls tails** — small agent follow-ups,
not owner-gated: the Tiltstone cut-5 daily-chain browser-shell opt-in and an
optional touch-controls cut, both named in
[`docs/arcs/TILTSTONE.md`](arcs/TILTSTONE.md). *First resume step:* pick one up
as an additive, byte-identical-by-default pure-engine slice with a `smoke.mjs`
proof.

---

## (d) Owner walkthrough — plain language

No installs, no toolchain. Every item below is a link you can click.

- **Play the web arcade** (nothing to install — runs in your browser):
  <https://menno420.github.io/gba-homebrew/>
- **Download a ROM and play it** in any GBA/NDS emulator (grab the file, open it
  in the emulator):
  - Wickroad (GBA, trading): <https://github.com/menno420/gba-homebrew/blob/main/dist/wickroad.gba>
  - Underroot (NDS, colony): <https://github.com/menno420/gba-homebrew/blob/main/dist/underroot.nds>
  - Brineward (NDS, pirate duel): <https://github.com/menno420/gba-homebrew/blob/main/dist/brineward.nds>
  - Lumen Drift (GBA, cave drifter): <https://github.com/menno420/gba-homebrew/blob/main/dist/lumen-drift.gba>
  - (The other five — cindervault, deepcast, clockwork-courier, shoal, gloamline —
    live alongside these in the `dist/` folder: <https://github.com/menno420/gba-homebrew/tree/main/dist>)
- **The published Lumen Drift release** (a proper download page):
  <https://github.com/menno420/gba-homebrew/releases/tag/lumen-drift-v1.3>
- **Pick the next game** — the two remaining choices and what they'd be:
  [the menu](NEXT-MENU-2026-07-15.md), with ready-to-build plans for
  [A1 Tinderhand](plans/tinderhand-preplan-2026-07-20.md) and
  [A3 Starloom](plans/starloom-preplan-2026-07-20.md).

**Your checklist (quickest first):**

1. **Open the arcade link** — nothing to install, it just plays in your browser.
2. **Reply `A1` or `A3`** to pick the next game to build (Tinderhand or Starloom).
3. **Play the downloadable ROMs** in an emulator and give a one-line verdict
   ("feels good" / "change X") for each.
4. **(optional)** Publish the Lumen Drift release to itch.io for wider reach.

---

## (e) Working this repo with a fresh session

If you are a new Claude session picking this up cold, boot in this order:

1. **Boot route (read these, in order):** [`README.md`](../README.md) →
   [`CONSTITUTION.md`](../CONSTITUTION.md) → [`docs/current-state.md`](current-state.md)
   → [`docs/CAPABILITIES.md`](CAPABILITIES.md). That is the working agreement,
   the living state ledger, and the capability/wall ledger.
2. **Verify the checkout:** `python3 bootstrap.py check --strict`. This is the
   substrate gate; it prints findings and appends a guard-fire telemetry ledger
   (commit that delta with your session — do not revert it).
3. **How PRs land:** open a PR **READY (non-draft)** on a `claude/*` branch. The
   **`auto-merge-enabler.yml`** workflow arms GitHub-native **squash** auto-merge;
   GitHub merges it the moment the required **`ROM builds`** check goes green — no
   human click. (The workflow is **not** `merge-on-green.yml` — that name was
   retired; don't chase it.)

**Gotchas (each has bitten a prior session):**

- **Born-red HOLD.** An **in-progress** `.sessions/` card intentionally holds the
  substrate gate **red** until you flip it to `complete` — that flip is the LAST
  commit of the slice, so the PR only goes green (and auto-merges) once the work
  is actually done. This is a feature, not a failure.
- **`substrate-gate` is NON-required and is red for a known reason.** It reds on
  two **pre-existing** doc-orphans — [`arcs/TILTSTONE.md`](../arcs/TILTSTONE.md)
  and [`arcs/UNDERROOT.md`](../arcs/UNDERROOT.md) are missing their Status badge
  (and a dateless-wall note on `docs/PLATFORM-LIMITS.md`). That red does **not**
  block merges (only `ROM builds` is required). Don't let it panic you — but make
  sure **your own** new doc has a Status badge and is reachable from a read-path
  root (this file is linked from `docs/current-state.md`), so you don't *add* a
  finding.
- **devkitARM installs via the leseratte10 mirror.** The official host 403s from
  the CI/agent environment; the toolchain setup scripts pull from the mirror
  instead (see [`docs/BUILDING.md`](BUILDING.md) / `tools/setup-toolchain.sh`).
- **mGBA `load_save()` segfaults.** Do not use it in headless proofs; use the
  `--savefile` bus-copy path to load a `.sav` instead (the committed SRAM
  power-cycle proofs use this).
- **Track-A isolation rail.** This is a **public** repo of **original** GBA/NDS IP
  only — nothing Nintendo-derived, no `pokemon`, no cross-track content, ever.

---

*This closeout was written 2026-07-21 as the project's final content slice; the
autonomous development period ended 2026-07-22. Any state above can be re-checked
with the command, PR, or link cited next to it.*
