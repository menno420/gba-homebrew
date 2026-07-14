# Session — Undertow growth cut 2: cosmetics (diver skins + bubble trails)

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/undertow-cosmetics`, started 02:43Z,
  flipped 02:48Z — both from `date -u` at write time)
- mission: build the next-lowest-risk named growth cut from
  `games/web-undertow/CONCEPT.md` — "Cosmetics (diver skins, bubble
  trails) — pure render-side, zero sim risk". A small set of diver skins
  (body/visor colors + a bubble-trail style each), selectable via a
  `?skin=` URL param and a `C` key-cycle on the title/gameover screens,
  persisted to `localStorage` with the same guarded-access pattern the
  lane already uses for the best score. HARD CONSTRAINT from the concept
  line itself: pure render-side — the sim, RNG draw order, crash frames,
  and depth results for a given seed must be BYTE-IDENTICAL with any
  skin/trail active vs. default, and the smoke must prove it.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-undertow-cosmetics.md`, born red
   (claim before build), cut from main @ `46b5fd4`.
2. **Skins** (`games/web-undertow/game.js` v1.1.0 → v1.2.0): a `SKINS`
   table — `classic`, `abyss`, `ember`, `ghost`, each a diver body/visor
   color pair plus a bubble-trail draw style (filled bubbles / stroked
   rings / streak ellipses). Selection resolves at boot: `?skin=ID` →
   guarded `localStorage` (`undertow.skin`) → default; `C` on the
   title/gameover screens cycles and persists (guarded, like the best
   score). Every selection event happens OUTSIDE the sim step, matching
   the game's existing boundary-read discipline. Title screen draws a
   skin swatch + "C to change". New test API: `getSkin` / `getSkinIds`.
3. **Zero sim risk, by construction**: skins are consumed only inside
   `render()` (and the title HUD). The diff to `update()` is empty; both
   RNG streams (gameplay `rng`, visual `vrng`) and their draw order are
   byte-for-byte untouched — the trail styles restyle the SAME
   deterministic particles the sim already produced.
4. `CONCEPT.md` cosmetics growth line marked BUILT (the PR #110/#111
   convention); `HOSTING.md` documents `?skin=` + the `C` cycle and the
   persisted-state bullet now covers the skin.
5. `tools/web-smoke-undertow.mjs` extended 17 → 22 assertions:
   `?skin=ember` selects; **sim identity across skins** — the
   non-default-skin run crashes on the identical frame at the identical
   depth as the default skin for the same seed (crashFrame=823 / 194m,
   also unchanged from the pre-change v1.1.0 baseline run); `C` cycles
   ember → ghost and persists; the persisted skin survives a reload
   without the param; unknown `?skin=nope` falls back. Run twice locally
   against real Chromium: 22/22 PASS both runs, outputs byte-identical
   (`diff` empty).
6. `python3 bootstrap.py check --strict` pre-flip: the designed born-red
   hold on this card + the pre-existing `claims-format` advisory on
   `order-005-scribe.md` (not this lane's file, advisory, left alone).
7. PR #114 opened READY immediately (no draft phase); no
   merge/approve/auto-merge calls from this session — the server-side
   enabler decides on green. Heartbeat appended to `control/status.md`
   (append-only dispatch section, PR number confirmed first).
8. Claim `control/claims/claude-undertow-cosmetics.md` retired with this
   flip per the claims README (the open PR is the live claim from here).
9. Follow-up carried (PR body + heartbeat): `dist/web`'s Undertow copy
   trails main again after the v1.2 bump — the next
   `tools/package-web-arcade.sh` refresh re-pins it; `--verify` red until
   then is the session-55 designed reminder.

## 💡 Session idea

**When a feature's contract is "changes nothing", make the OLD baseline
the assertion, not just self-consistency.** The obvious test for
"cosmetics don't touch the sim" is to run skin-A and skin-B in the new
build and compare — but that check passes even if the cosmetics patch
quietly shifted the sim for ALL skins equally (say, by reordering a
vrng draw). This smoke pins the invariant twice: across skins within
the new build (ember ≡ default) AND against the pre-change v1.1.0
numbers (crashFrame=823 / 194m for seed 7, recorded in the PR before
the first product edit). Generalized: a "pure render / pure refactor /
zero behavior change" claim needs a fixed point measured BEFORE the
change and re-asserted after — internal equivalence alone can't see a
uniform drift. Cheap here because determinism makes the fixed point one
integer; that's another return on keeping sims seeded.

## Previous-session review

- The Deepcast audio session (`.sessions/2026-07-14-deepcast-audio.md`,
  PR #113) held "enabling the mixer did not shift the boot clock — every
  prototype game-state literal carried verbatim", which is exactly the
  fixed-point discipline this card's idea names: its committed literals
  were the pre-change baseline this smoke imitates (crashFrame=823
  carried verbatim across the skin layer), and its run-twice,
  byte-identical proof style is reused here unchanged; no erratum found
  to carry.
