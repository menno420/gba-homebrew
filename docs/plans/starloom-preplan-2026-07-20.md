# Pre-plan — A3 Starloom (browser/PWA daily constellation puzzle)

**This document does NOT select this option. The owner's letter choice still
governs (see `docs/NEXT-MENU-2026-07-15.md`). This pre-plan only makes the
option instantly executable if picked.**

> **Status:** `reference` — an executable slice ladder for menu option **A3**,
> written 2026-07-20 (main `dd70656`). It is a PLAN, not a build: no
> `games/starloom*` exists yet (UNBUILT per the 2026-07-19 audit,
> `docs/NEXT-MENU-2026-07-15.md` § REMAINING REAL CHOICES). Facts pinned at
> `dd70656`; `docs/current-state.md` wins as the repo moves.
>
> **A3 is a WEB game — the GBA ROM / mGBA-headless proof template does NOT
> apply.** Proofs are the web mold: pure-Node `smoke.mjs` + a real-Chromium
> `tools/web-smoke-starloom.mjs` + the Pages deploy pipeline. There is **no
> committed ROM**.

## Why this pre-plan exists (and B1 got none)

A3 is one of only two genuine open choices at the 2026-07-19 audit. Menu A3
and OVERNIGHT NEW-02 (`docs/planning/OVERNIGHT-MENU-2026-07-16.md` lines
497–502) give a rich pitch + named idioms but stop at "**~6 slices**
(Undertow/Tiltstone precedent)" — a slice **count**, not an enumerated ladder.
Contrast menu B1, which names its five cuts, so a B1 pre-plan was correctly
SKIPPED as a restatement (`.sessions/2026-07-19-truth-refresh-b1-preplan.md`,
#207). A3 is not slice-granular, so this pre-plan supplies the missing ladder.

## Design intent

Starloom is a one-minute daily: a shareable line you draw once a day. See the
full pitch in menu A3 (`docs/NEXT-MENU-2026-07-15.md` § A) and OVERNIGHT
NEW-02 — not restated here. In one paragraph for orientation: you **thread one
continuous line** through a seeded star field within a **thread budget** while
**drift currents deterministically tug** the line; there is a share URL, and a
**ghost of yesterday's best weave** to beat. Every seed is
**provably-solvable** by construction (the Tiltstone idiom,
`games/web-tiltstone/` — a solver verifies a within-budget solution exists), and
each feature draws from its own **side-band mulberry32 stream** so adding a
feature never shifts an existing one (the Undertow header idiom,
`games/web-undertow/game.js`). It fits the platform: pure-JS, deterministic,
installable **offline via the Drift Garden PWA shell**
(`games/mobile-foundation/`), packaged by `tools/package-web-arcade.sh`, and —
since PR #149 — **hosted the moment `dist/web/` changes** via the merged Pages
deploy (`.github/workflows/deploy-pages.yml`, live at
`https://menno420.github.io/gba-homebrew/`). It joins Undertow / Tiltstone /
Drift Garden as the fourth web-arcade title.

## Ordered slice ladder (~6 slices, dependency-ordered)

Each slice is **one born-red card PR** (card `in-progress` → flip to
`complete` as the last commit), landing on green `ROM builds` via
`auto-merge-enabler.yml` (the required check is repo-wide even for web-only
diffs). Proof surface for a WEB game (mirrors Tiltstone, per the #207 web-vs-ROM
note): a pure-Node **engine smoke** `games/web-starloom/smoke.mjs` (the
CI-honest, dependency-free gate — the `games/web-tiltstone/smoke.mjs` mold) +
a real-**Chromium** shell smoke `tools/web-smoke-starloom.mjs` (the
`tools/web-smoke-tiltstone.mjs` / `web-smoke-undertow.mjs` mold; needs
playwright + a Chromium binary, NOT vendored) + the **Pages deploy check**
(`deploy-pages.yml` fires on `dist/web/**` push → live URL). **No ROM, no
headless-boot** — those are ROM-game proofs.

### Slice 1 — engine core: seeded field + threading + provable solvability
- **Scope:** `games/web-starloom/engine.js` (the same file the page ships) —
  the pure seeded **star-field generator**, the **continuous-line** threading
  model, and the **thread budget**, with a generator that is
  **provably-solvable** (a solver finds a within-budget weave). No render shell
  yet.
- **Proof:** `games/web-starloom/smoke.mjs` (`node smoke.mjs`, pure, no deps)
  asserting: pinned initial field for a fixed seed; determinism (same seed
  twice → byte-identical state trace); purity (threading never mutates its
  input state); and a **generation sweep** (N consecutive daily seeds all
  produce solvable fields whose solver line actually wins) — the Tiltstone
  smoke.mjs §6/§7/§8 mold.

### Slice 2 — render / input shell
- **Scope:** `index.html` + canvas render + pointer/drag input that threads
  the line; the shell adds nothing over the engine (`app.js` thin).
- **Proof:** `tools/web-smoke-starloom.mjs` (real Chromium, the
  `web-smoke-tiltstone.mjs` mold): the page loads with **zero console / page
  errors**, exposes a test API, a real drag input changes both the rendered
  canvas pixels AND the engine state behind them, and the page state **matches
  a pure-Node engine run** of the same seed + inputs (the shell adds nothing).

### Slice 3 — drift currents
- **Scope:** deterministic **drift** that tugs the line per index, drawn from
  its **own side-band mulberry32 stream** (the Undertow header idiom) so the
  field stream from slice 1 is byte-unchanged.
- **Proof:** `smoke.mjs` asserts drift is a pure function of `(seed, index)`,
  that the drift stream is independent (field generation trace unchanged with
  drift on), and re-runs the solvability sweep *with drift active*.
  `web-smoke-starloom.mjs` shows the drifted line rendering without errors and
  still matching the pure-Node engine.

### Slice 4 — daily seed + share URL
- **Scope:** a pure `date → seed` mapping (one puzzle a day) and a
  `?seed=N`/replay share URL that boots the exact pinned field.
- **Proof:** `smoke.mjs` daily-seed mapping test (pure date→integer, the
  Tiltstone §9 mold) + share **encode/decode round-trip**.
  `web-smoke-starloom.mjs` boots a real share URL and asserts the field is
  byte-identical to the pure-Node generator for that seed.

### Slice 5 — ghost of yesterday's best weave
- **Scope:** persist the best weave to `localStorage` and render **yesterday's
  ghost** behind today's line to beat.
- **Proof:** `smoke.mjs` pure ghost-serialize/deserialize round-trip (the
  best-line encoding is a pure function). `web-smoke-starloom.mjs` asserts the
  ghost survives a page reload and renders behind the live line.

### Slice 6 — PWA offline shell + package + Pages go-live (arc closer)
- **Scope:** wire the **Drift Garden PWA shell** (`games/mobile-foundation/`)
  for installable-offline play, add Starloom to `tools/package-web-arcade.sh`'s
  stage list (per a `games/web-starloom/HOSTING.md` contract), rebuild
  `dist/web/`, and pin its zip sha256 in `docs/RELEASES.md`. **No ROM.**
- **Proof:** `web-smoke-starloom.mjs` installable/offline check (service worker
  registers, page works offline); `tools/package-web-arcade.sh --verify`
  (deterministic rebuild + manifest assertion, exit 1 on drift); and the
  **Pages deploy check** — the `dist/web/**` change triggers
  `deploy-pages.yml`, and `GET https://menno420.github.io/gba-homebrew/`
  returns HTTP 200 with Starloom listed (the #208 arcade-live verification
  mold). Hosting is automatic since #149 (no owner Settings click needed — that
  was clicked to bring the arcade live).

## Risks / rails

- **Track-A isolation (⚠):** PUBLIC repo, original gba IP only — Starloom is an
  original concept; no Track-B / Nintendo / pokemon content.
- **Binary policy:** A3 is a WEB game — **no ROM is ever committed.** It ships
  into `dist/web/` via `package-web-arcade.sh` and hosts via the Pages
  pipeline; the "download" is the hosted URL, not a `dist/*.gba`. Do **not**
  apply the A1/Wickroad ROM-commit step here.
- **Determinism for daily-puzzle seeding:** the daily seed is a pure
  `date → seed` mapping and the field/drift are pure `f(seed, index)` schedules
  with **per-feature side-band mulberry32 streams** — no runtime RNG, no
  `Math.random()` in gameplay. This is what makes "everyone gets the same daily"
  and the share URL honest, and what the `smoke.mjs` determinism/solvability
  sweeps depend on.
- **Substrate-gate born-red flow:** every slice is one born-red card PR (card
  `in-progress` → flip `complete` last commit); lands on green `ROM builds` via
  `auto-merge-enabler.yml`. The pre-existing non-required `substrate-gate`
  doc-orphan red is unrelated.
- **Scope-per-slice discipline:** one slice = one PR = one card. The engine
  (slice 1) must land before any shell (slice 2); drift (slice 3) must not
  shift the field stream; keep each born-red diff bounded.
- **Proof-surface honesty:** `smoke.mjs` (pure Node) is the CI-honest gate;
  `web-smoke-starloom.mjs` needs playwright + a Chromium binary (NOT vendored,
  do not commit `node_modules`) so it is a local/shell proof, not a required
  per-PR check — cite it as such (the Tiltstone convention).

## Continuity note

Nothing lands today beyond this plan. **ALL six slices carry to the re-created
project** — the read-only EAP window lands 2026-07-21, so if A3 is picked the
build happens in the successor project. The landing path there is the **same
`auto-merge-enabler.yml` enabler shape** (born-red `claude/*` card PR → flip →
lands on green `ROM builds`), and the same **Pages deploy** (`deploy-pages.yml`)
hosts it on `dist/web/` change; a re-created project re-arms its own routines
and does not inherit the old coordinator failsafe cron.
