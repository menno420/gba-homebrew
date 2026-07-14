# EAP closeout walkthrough — 2026-07-14 (ORDER 006)

> **Status:** `reference` — complete. The owner-facing closeout walkthrough
> for the game-lab Track B seat at the end of the EAP (2026-07-10 →
> 2026-07-14), served per `control/inbox.md` ORDER 006(b). Point-in-time
> facts are frozen at the ORDER 006 serve (main ≥ `cf107d3`, PR #134);
> the living ledgers ([`current-state.md`](current-state.md),
> [`RELEASES.md`](RELEASES.md), [`../dist/README.md`](../dist/README.md))
> win over this file as the repo moves. Depth companion: the
> [EAP project audit](audits/eap-project-audit-2026-07-14.md).

This is the one document to read to review this seat: what it did
(§A), what exists now and how to check it (§B), **every click or
decision waiting on you** (§C), a 5-minute hands-on tour (§D), and
what the next phase inherits (§E).

## A. What this seat did during the EAP

**Scale:** 2026-07-10 → 2026-07-14 (~4.3 days), 11 original games on 4
platforms, 134 PRs (median created→merged 4.1 min over the audit's 121
measured merges), every game headlessly proven with committed,
re-runnable proof suites. Full evidence-cited detail — tooling
verdicts, walls with verbatim denials, friction measurements, ceremony
cost/benefit — lives in the
[EAP project audit](audits/eap-project-audit-2026-07-14.md).

The arcs, PR-cited:

- **Lumen Drift** (GBA, gravity cave drifter) — sessions 1–8, PRs
  #2–#29: substrate adoption, pinned toolchain, walking skeleton, then
  v1.3 (endless cave, graze, SRAM best, audio). The harness
  conventions every later game inherited.
- **Gloamline** (NDS, zombie horde defense, owner-picked) — PRs
  #50–#75 + rematch slice: concept tree COMPLETE; 28 proofs / 517
  emulator asserts.
- **Brineward** (NDS, pirate broadside duel) — PRs #82/#91/#94:
  loot/ports/wind/reefs/audio/SPI-EEPROM saves.
- **Breadth night, ORDER 005** (2026-07-13) — five new prototypes in
  one night (Undertow #79, Tiltstone #81, Deepcast #83, Drift Garden
  #84, Cindervault #86), then Clockwork Courier #96 and Shoal #98
  closed the ORDER 001 concept queue; both ran to CONCEPT COMPLETE
  v1.0 (#99–#103, #105–#108).
- **Growth-path completion wave** (2026-07-14, PRs #110–#128): the
  four games with named growth paths finished them in parallel —
  Undertow v1.5, Drift Garden v1.4 (installable PWA), Cindervault
  v0.6, Deepcast v0.6 — plus the universal committed-`proofs.sh`
  convention and the deterministic web-arcade packaging machinery
  (#109, repinned #130).
- **Wickroad** (GBA, game #11) — PR #129: the generative rung — a new
  concept written and shipped as a pinned playable prototype in one
  session.
- **EAP audit** — PR #131:
  [`audits/eap-project-audit-2026-07-14.md`](audits/eap-project-audit-2026-07-14.md).
- **Today's closeout (ORDER 006(a), all verified live 2026-07-14):**
  the Tiltstone growth stack landed on main — par+undo
  [#92](https://github.com/menno420/gba-homebrew/pull/92) `d451b79`,
  juice [#93](https://github.com/menno420/gba-homebrew/pull/93)
  `7535d6c`, cell types
  [#95](https://github.com/menno420/gba-homebrew/pull/95) `1c8e0be`,
  level packs [#97](https://github.com/menno420/gba-homebrew/pull/97)
  `b0d2274` (Tiltstone is now v1.1: undo/par, animated juice + synth
  audio, locked gems / ice / one-way grates, two curated packs);
  stale [#85](https://github.com/menno420/gba-homebrew/pull/85)
  closed-with-citation (superseded by merged #109 + #130 —
  [disposition comment](https://github.com/menno420/gba-homebrew/pull/85#issuecomment-4968446028));
  and the juice.js packaging follow-up served —
  [#134](https://github.com/menno420/gba-homebrew/pull/134) `cf107d3`
  (web-tiltstone v1.1, arcade bundle v1.3, `--verify` green,
  determinism proven run-twice).

## B. Current state + how to run and verify

**What exists at HEAD** (details: [`current-state.md`](current-state.md)):
six GBA ROMs, two NDS ROMs, two browser games, one mobile PWA — all
original IP, all downloadable from [`../dist/`](../dist/README.md)
with per-file sha256 + provenance. `dist/web/` is a host-ready static
arcade; `dist/releases/` holds deterministic versioned zips pinned in
[`RELEASES.md`](RELEASES.md).

Exact commands (from a clone at main):

```sh
# 1. Repo hygiene gate (docs, cards, claims) — expect exit 0
python3 bootstrap.py check --strict

# 2. Release integrity — asserts every sha256 pin in docs/RELEASES.md
bash tools/package-web-arcade.sh --verify   # expect exit 0

# 3. Play the web arcade locally (any static server works)
cd dist/web && python3 -m http.server 8000
# then open http://localhost:8000 — landing page links all three games

# 4. Rebuild the GBA ROMs from source (pinned toolchain)
tools/setup-toolchain.sh && tools/build.sh    # per docs/BUILDING.md

# 5. Rebuild the NDS ROMs
tools/setup-nds-toolchain.sh && make -C games/gloamline-nds
```

Per-game headless proof suites: `games/<game>/proofs.sh` (GBA) and
`tools/check-gloam.py` / `tools/check-brine.py` (NDS); web smokes:
`tools/web-smoke-*.mjs`. Recipes: [`BUILDING.md`](BUILDING.md),
[`capabilities.md`](capabilities.md).

## C. OWNER ACTIONS — every pending click and decision

Each item: six fields + a bolded recommendation. Paste-ready.

**1. Prune the merged Tiltstone branches**
- WHAT: delete branches `claude/tiltstone-par`, `claude/tiltstone-juice`,
  `claude/tiltstone-cells`, `claude/tiltstone-packs` (all squash-merged
  today: #92/#93/#95/#97; remote branch delete is owner-only — agents
  get 403 at the credential layer).
- WHERE: <https://github.com/menno420/gba-homebrew/branches>
- HOW: Delete button on each row (they show as merged).
- WHY: dead branches accumulate as false "in-flight" signals.
- **Recommendation: delete all four.**
- UNBLOCKS: a clean branch list for the next phase.
- VERIFY: the branches page lists no `tiltstone-*` branches.

**2. Ratify the #85 close (or reopen)**
- WHAT: PR #85 (original release-packaging cut) was closed unmerged
  under ORDER 006 decide-and-flag authority — conflicted and fully
  superseded by merged #109 (machinery) + #130 (repin) + #134 (v1.3).
- WHERE: [disposition comment](https://github.com/menno420/gba-homebrew/pull/85#issuecomment-4968446028)
- HOW: read the comment; reopen only if you disagree.
- WHY: decide-and-flag closes need owner ratification.
- **Recommendation: ratify the close** — everything #85 packaged
  exists better on main.
- UNBLOCKS: item 3.
- VERIFY: #85 shows "Closed" with the citation comment; no reopen.

**3. Delete the `claude/release-packaging` branch**
- WHAT: #85's head branch, left intact pending your ratification.
- WHERE: <https://github.com/menno420/gba-homebrew/branches>
- HOW: Delete button, after item 2.
- WHY: same hygiene as item 1.
- **Recommendation: delete** once item 2 is ratified.
- UNBLOCKS: zero stale `claude/*` branches.
- VERIFY: branch absent from the branches page.

**4. Ratify the ORDER 006 serve (PR #133) — no merge click needed**
- WHAT: #133 carries this walkthrough + the closeout stamps. The
  server-side auto-merge enabler lands green `claude/*` PRs; this
  session does not merge/approve/arm its own PR.
- WHERE: <https://github.com/menno420/gba-homebrew/pull/133>
- HOW: read this walkthrough; that IS the review.
- WHY: EAP final-day directive — every lane terminal-or-parked-cited.
- **Recommendation: ratify by reading** — no click required unless the
  enabler failed to land it (then: review-merge on green).
- UNBLOCKS: the EAP closes clean.
- VERIFY: #133 merged; this doc reachable from the README at main.

**5. Make `nds-rom-build` a required status check (standing ⚑ ask)**
- WHAT: auto-merge fires on "ROM builds" alone; the NDS job is not a
  required context (audit §4 — #54 once merged while `nds-rom-build`
  was still running).
- WHERE: <https://github.com/menno420/gba-homebrew/settings/branches>
  (main's protection rule → required status checks).
- HOW: add `nds-rom-build` next to "ROM builds".
- WHY: protection state is owner-UI-only (agents can neither read nor
  set it); one missing required check already cost a 17.5h epoch.
- **Recommendation: add it.**
- UNBLOCKS: NDS regressions can no longer slip through an early merge.
- VERIFY: the next PR touching NDS waits for `nds-rom-build` before
  auto-merge fires.

**6. Owner-gated game decisions (no code blocked, direction wanted)**
- WHAT: (a) Tiltstone's last concept line, the **daily leaderboard**,
  needs a backend — out of static-hosting scope
  (`games/web-tiltstone/CONCEPT.md` item 6); (b) next-arc picks:
  Wickroad has five named growth cuts open, and the four
  concept-complete titles need new-concept material or your veto;
  (c) real-device playtests of the 8 ROMs + 3 web games — taste
  steering is your half of the loop.
- WHERE: reply in the coordinator chat or file a new ORDER in
  `control/inbox.md`.
- HOW: one line per decision is enough.
- WHY: all three are explicitly owner-gated in
  [`current-state.md`](current-state.md) / the concept docs.
- **Recommendation: park the leaderboard** (static-only stays free and
  deterministic); **pick Wickroad** as the next growth arc; playtest
  Tiltstone v1.1 first (§D).
- UNBLOCKS: the next phase's work queue.
- VERIFY: the decision lands as an inbox ORDER or a recorded chat
  directive.

**7. External publishing (only if/when you want it)**
- WHAT: publishing Track B anywhere (itch.io, forums, a host for
  `dist/web/`) is an owner action by hard rail
  ([`../README.md`](../README.md)); `web-arcade-v1.3.zip` is the
  one-file static bundle, pinned sha256 `5174c744…` in
  [`RELEASES.md`](RELEASES.md).
- WHERE: your host of choice; the zip is in `dist/releases/`.
- HOW: unzip onto any static host — no build step, no server code
  (per-game contracts: `games/*/HOSTING.md`).
- WHY: agents never publish externally; supply-chain caveat for ROMs
  noted in the README.
- **Recommendation: no action needed now**; the bundle is ready
  whenever you are.
- UNBLOCKS: public playtesting, sellability probes (Venture Lab).
- VERIFY: hosted page serves the arcade landing + all three games.

## D. Verify it yourself — the 5-minute tour

1. **Serve the arcade** (30s): `cd dist/web && python3 -m http.server
   8000`, open <http://localhost:8000>. Expect: a landing page linking
   **Undertow**, **Tiltstone**, **Drift Garden**.
2. **Play Tiltstone v1.1** (2 min): click Tiltstone. Expect: gems fall
   with animated slides/pops and synth audio (the #93 juice — mute
   button top-right), par + undo counters (#92), locked gems / ice /
   one-way grates on deeper levels (#95). Add
   `?pack=granite-gauntlet` to the URL for a curated pack (#97).
3. **Release integrity** (30s): `bash tools/package-web-arcade.sh
   --verify`. Expect exit 0 — every sha256 pin in
   [`RELEASES.md`](RELEASES.md) asserted against the tree.
4. **Read the pins** (1 min): open [`RELEASES.md`](RELEASES.md).
   Expect 4 zips: `web-tiltstone-v1.1.zip` (`3a8b5ce7…`),
   `web-arcade-v1.3.zip` (`5174c744…`), Undertow v1.5, Drift Garden
   v1.4 — and `juice.js` among the staged Tiltstone runtime files.
5. **Repo hygiene** (30s): `python3 bootstrap.py check --strict` at
   main. Expect exit 0 (docs badges/links/reachability, session-card
   markers, zero claims advisories).
6. *(Bonus)* download [`../dist/lumen-drift.gba`](../dist/lumen-drift.gba)
   into mGBA — [`PLAYING.md`](PLAYING.md) is the one-page setup.

## E. Handoff notes (batons for the next phase)

- **The EAP ended 2026-07-14 with this lane terminal-or-parked-cited:**
  no open work PRs remain from this seat's queue (Tiltstone stack
  merged, #85 dispositioned, packaging repinned via #134); PR #133
  (this doc) is the last in flight and lands via the enabler.
- **Where state lives:** [`current-state.md`](current-state.md)
  (living ledger), `control/status.md` (append-only dispatch history —
  the ORDER 006 closeout dispatch is the last section),
  [`../dist/README.md`](../dist/README.md) (shipped builds + hashes),
  [`RELEASES.md`](RELEASES.md) (web pins), `control/inbox.md`
  (manager orders — ORDER 006 was the final EAP order; it superseded
  the never-delivered night ORDER and answered the seat's
  2026-07-13T22:29Z outbox ask).
- **Machinery the next phase inherits, working:** the server-side
  auto-merge enabler (lands green `claude/*` PRs; refuse-to-arm guard
  documented in audit §4), deterministic one-command packaging
  (`tools/package-web-arcade.sh`), pinned GBA/NDS toolchains,
  committed per-game proof suites, the born-red card ritual, and the
  append-only status.md dispatch convention.
- **Hygiene at close:** served claims swept (the Tiltstone four +
  arcade-refresh retired in #133 with this doc; the closeout claim
  retired at its flip); remaining stale branches are the owner clicks
  in §C items 1/3.
- **What the next phase needs first:** the §C decisions — especially
  item 5 (required check) and item 6 (direction). The generative rung
  is proven (Wickroad); an empty queue means generate, never idle.
