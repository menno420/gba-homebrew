# Session — Undertow growth cut 3: ghost replays of your best run

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/undertow-ghost`, started 04:10Z,
  flipped 04:19Z — both from `date -u` at write time)
- mission: build the next named growth cut from
  `games/web-undertow/CONCEPT.md` — "Ghost replays of your best run —
  nearly free because the sim is deterministic: store the input
  timeline, replay it against the same seed". Record every run's input
  timeline; persist the best run per seed to guarded `localStorage` as a
  compact input log; on later runs of the SAME seed, a translucent ghost
  diver replays it in lockstep — a second sim instance fed the stored
  inputs. HARD CONSTRAINTS: the live sim must be byte-identical with the
  ghost on vs off (same crashFrame/depth), and a stored timeline must
  replay to the original run's depth exactly — both asserted by the
  smoke.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-undertow-ghost.md`, born red
   (claim before build), cut from main @ `932cfd1`.
2. **Baseline first**: ran the v1.2.0 smoke before touching product code
   — 22/22 PASS, fixed point for seed 7 pinned at crashFrame=823 / 194m.
3. **Ghost replays** (`games/web-undertow/game.js` v1.2.0 → v1.3.0):
   every run records its effective steer (-1/0/+1) per frame,
   run-length encoded ("120n45l30r"); at crash, the run persists as the
   seed's ghost when it beats the stored best (`undertow.ghost.<seed>`,
   guarded like the best score and skin). On the next dive of the same
   seed a translucent ghost diver replays the stored timeline in
   lockstep and drifts up-screen once it crashes. Title line ("ghost:
   your best N m dives with you") + a small HUD "ghost N m" marker;
   `?ghost=0` opts out, render-side only, read once at boot.
4. **Fidelity by construction**: the sim core was factored into
   `makeSim()` / `rowIn(sim, i)` / `physStep(sim, steer)` — a sim
   instance owns its RNG and lazily generated rows, and the live diver
   and the ghost execute the SAME `physStep`. The ghost's instance never
   touches the live run's state or RNG streams (zero-contact identity);
   the refactor's math is identical to v1.2.0 — the pre-change fixed
   point (crashFrame=823 / 194m) carried verbatim. New test API:
   `getGhostInfo` / `getGhostRun` / `verifyGhost` (offline replay probe,
   no freeze guard) / `clearGhost`.
5. `CONCEPT.md` ghost growth line marked BUILT (the PR #110/#114
   convention); `HOSTING.md` documents `?ghost=0` and the persisted
   ghost record.
6. `tools/web-smoke-undertow.mjs` extended 22 → 31 assertions in a fresh
   (clean-storage) context: a crashed run persists depth + crashFrame +
   a 49-byte RLE log; **replay fidelity** — the stored timeline replayed
   through a fresh sim reproduces crashFrame=823 / 194m exactly;
   **live-run identity** — the run with the ghost active is identical to
   the cleared-storage baseline, and the lockstep ghost lands on its
   recorded numbers mid-run; best-per-seed keeper (a 7m hold-left run
   never overwrites 194m); reload persistence + re-arm; per-seed keying;
   `?ghost=0` opt-out with an unchanged run. Run twice against real
   Chromium: 31/31 PASS both runs, outputs byte-identical (`diff`
   empty). Assertions 4–20 now double as extra identity coverage — they
   execute with ghost records written and replayed underneath and still
   land on every v1.2.0 literal.
7. `python3 bootstrap.py check --strict` pre-flip: the designed born-red
   hold on this card + the pre-existing `claims-format` advisory on
   `order-005-scribe.md` (not this lane's file, advisory, left alone).
   `.substrate/guard-fires.jsonl` telemetry committed.
8. PR #118 opened READY immediately (no draft phase); no
   merge/approve/auto-merge calls from this session — the server-side
   enabler decides on green. Heartbeat appended to `control/status.md`
   (append-only dispatch section, PR number confirmed first).
9. Claim `control/claims/claude-undertow-ghost.md` retired with this
   flip per the claims README (the open PR is the live claim from here).

## 💡 Session idea

**When two executions must agree exactly, don't write two
implementations and test them into agreement — make them the same
function, and spend the test budget on the seam.** The ghost could have
been a hand-copied replica of the update step; every future physics
tweak would then need a matching edit or the replay silently desyncs.
Instead the refactor made "one diver's outcome" a value (`makeSim`:
own RNG + own rows) and "one frame of physics" a single `physStep`
both divers call — replay fidelity is now a property of the code shape,
not of test discipline. The tests that remain guard what construction
CAN'T guarantee: the seam between executions (record exactly the steer
applied, per frame; storage round-trip; per-seed keying) and the
refactor itself (the pre-change crashFrame=823 / 194m fixed point,
re-asserted after — the cosmetics card's baseline idea, reused). The
smell to watch for: an assertion whose failure would be fixed by
copying code from A to B — that assertion wants to be a shared
function instead.

## Previous-session review

- The Deepcast seed-select session (`.sessions/2026-07-14-deepcast-daily.md`,
  PR #117) kept its boot seed untouched so every v0.2 proof pin carried,
  and proved the dial's round trip ("dial away and back → the full P2
  route lands on every P2 literal") — the same discipline this card
  leaned on (an untouched default path, a pre-change fixed point
  re-asserted after the feature); its out-of-scope note for the
  leaderboard half (the PR #97 precedent) is the honest-scoping pattern
  this lane's static-hosting-only share links already follow; no erratum
  found to carry.
