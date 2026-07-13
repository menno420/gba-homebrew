# Session 2026-07-13 — mobile-game foundation (owner night order)

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/mobile-foundation`)
- mission: stand up a MOBILE-GAME FOUNDATION in one PR — framework
  decision (static PWA vs Capacitor-style shell vs native Android vs
  Godot export, evidenced by one native probe or a fresh documented
  wall), an npm-free build pipeline, a RUNNING skeleton game loop with
  touch input, and a headless smoke proof executed in this container.
  Original IP only (this repo is PUBLIC — conventions.md rule 13).
  Additive outside `games/mobile-foundation/**` + new `tools/` files +
  this card + one claim file + `docs/PLATFORM-LIMITS.md` appends; no
  workflow or existing-game changes. Owner night rules: PR stays OPEN,
  no merge, no auto-merge arming (the `do-not-automerge` label carve-out
  in `.github/workflows/auto-merge-enabler.yml` is applied at PR open to
  honor that).
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness/environment banner, per ORDER 003

## 💡 Session idea

The briefing said "there is no auto-merge enabler in this repo" — HEAD
said otherwise (PR #76 installed one between briefing and execution).
The lesson worth one line: any "the repo cannot do X" premise in a task
brief is a moving ref like a SHA — re-verify it against HEAD before
relying on it for a safety property (here: "a ready PR cannot
self-merge"). The enabler's own `do-not-automerge` carve-out was the
correct, already-provided valve; five minutes of reading the workflow
beat any improvised guard.

## Previous-session review

- `main` at session start: `0e08695` ("Install auto-merge-enabler
  landing workflow (owner directive, fm ORDER 029)", PR #76) — NEW
  since this task was briefed: the briefing said "no auto-merge enabler
  in this repo"; that is now false at HEAD. Consequence handled: this
  PR gets the workflow's documented `do-not-automerge` label at open,
  because tonight's owner order is PRs-stay-open / no merges / no
  arming — the born-red card alone would stop arming only until the
  close-out flip re-triggers `synchronize`.
- Inbox re-read at HEAD (`0e08695`): ORDERS 001–004 only. **No ORDER
  matching "mobile foundation / breadth program" is present in this
  repo's `control/inbox.md`** — this task arrives as a direct owner
  night order via the coordinator (2026-07-13 night run), not as an
  inbox ORDER. Noted per the ritual; not treated as ambiguous (owner
  orders outrank the standing default).
- Claims directory at branch time: empty except README — no collision.
- Recent sessions (24–36) are Gloamline/Brineward slices and
  heartbeats; this session opens a NEW surface (`games/mobile-foundation/`)
  touching zero existing game files.

## What this session did

1. This card + claim file, born red (first commit `bf065cb`); PR #78
   opened READY immediately
   (https://github.com/menno420/gba-homebrew/pull/78) with the
   `do-not-automerge` label applied in the next API call — verified
   effective: the enabler run on `bf065cb` concluded without arming
   (PR `auto_merge` empty on re-read) and the run on `1522fc3` was
   job-level **skipped** (run 29216394542).
2. **Framework decision** (`games/mobile-foundation/DECISION.md`):
   npm-free static PWA, picked over Capacitor shell / native Android /
   Godot export. Evidence: ONE native probe (2026-07-13T00:51:30Z),
   verbatim on the ledger — `which gradle sdkmanager godot java` found
   gradle+java only; `godot --version` → `/bin/bash: line 1: godot:
   command not found` (exit 127). Appended to `docs/PLATFORM-LIMITS.md`
   ("Native Android SDK + Godot are absent from this container").
3. **Skeleton** (`games/mobile-foundation/`): Drift Garden — original
   IP. `index.html` (viewport, fullscreen canvas, HUD, SW
   registration), `game.js` (60 Hz fixed-timestep + rAF render, touch
   handlers with pointer fallback — tap plants a mote, drag nudges;
   `visibilitychange` pause/resume that never replays hidden time;
   state mirrored to canvas HUD + `#hud` dataset +
   `window.__gameState`), `manifest.webmanifest` (standalone,
   portrait), `sw.js` (cache-first, versioned `drift-garden-v1`),
   2 python3-generated icons (760 B / 2.4 KB). Zero deps, no build
   step.
4. **Headless smoke proof** (`tools/mobile-foundation-smoke/`):
   driver probe chain per the order — node `playwright` MODULE_NOT_FOUND,
   `playwright-core` MODULE_NOT_FOUND, python `playwright`
   ModuleNotFoundError; fallback (c) worked: `npm i playwright-core`
   (1.61.1) into the session scratch dir (NOT committed), driving the
   container's Chromium `/opt/pw-browsers/chromium-1194/chrome-linux/chrome`
   via `executablePath` (never `playwright install`). Committed
   `smoke.mjs` + `run.sh`. Run twice locally, verbatim (run.sh pass):

   ```
   PASS  boot  booted=true entities=3
   PASS  loop-ticks  frames 1 -> 37, renders 2 -> 39
   PASS  tap-changes-state  taps 0 -> 1, entities 3 -> 4, lastTap={"x":195,"y":400}
   PASS  dom-mirror  hud entities=4
   PASS  pause-on-hidden  paused=true frames frozen at 47 (recheck 47)
   PASS  resume-on-visible  paused=false frames 47 -> 69
   smoke: 6/6 assertions passed
   SMOKE RESULT: PASS
   ```

   Honesty note: the tap is a real synthetic touch (Playwright
   touch-enabled mobile context, `page.touchscreen.tap`); pause/resume
   is asserted by overriding `document.visibilityState`/`.hidden` and
   dispatching a real `visibilitychange` — the game's own handler runs;
   only the browser-level trigger is simulated (headless cannot
   background a tab).
5. **CONCEPT.md**: Drift Garden target game (ambient tap-to-plant
   garden), honest sellability guess, packaging path — PWA direct
   install provable here; Play Store TWA/Bubblewrap blocked on the
   Android-SDK wall + owner actions (account, signing, spend).
6. `python3 bootstrap.py check --strict` at flip time: PASS (exit 0)
   with the designed born-red HOLD notice on this card — flipped by
   this commit.

## Walls hit (all appended/already on `docs/PLATFORM-LIMITS.md`)

- NEW: Native Android SDK + Godot absent (verbatim probe above) —
  appended this session.
- Pre-documented, quoted not re-probed: direct `api.github.com` REST
  blocked in-container (re-observed verbatim while polling CI:
  `{"message":"GitHub access is not enabled for this session. An org
  admin must connect the Claude GitHub App for this organization."}`
  — matches the ledger entry; MCP Actions API used instead).

## Close-out facts

- PR #78, branch `claude/mobile-foundation`, base `main` @ `0e08695`.
- Check runs observed pre-flip on head `1522fc3d`: substrate-gate run
  29216394529 = completed/**failure** (designed born-red hold on this
  card), ROM builds run 29216394572 = completed/**success**,
  auto-merge-enabler run 29216394542 = completed/**skipped** (label
  carve-out). This flip commit re-triggers all three; substrate-gate
  is expected green on the flip head, and the enabler stays skipped
  under `do-not-automerge` — per the 2026-07-13 owner night rules this
  PR stays OPEN, unmerged, un-armed.
- Claim file deleted in this flip commit (session close per
  `control/claims/README.md`); the open PR is the in-flight signal.
- No inbox ORDER covers this task (ORDERS 001–004 at `0e08695`); it is
  a direct owner night order relayed by the coordinator.
