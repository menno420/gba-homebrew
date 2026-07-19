# Tiltstone — arc 2 design doc (the shareable daily)

> **Status:** all **5 of 5 cuts BUILT and LANDED ON `main`** (corrected
> 2026-07-19 — the earlier "draft-parked, not merged" badge was stale). The
> arc-2 engine is on `main`: `games/web-tiltstone/engine.js` **v1.8.0** carries
> every cut's pure functions, landed via `c654e01` (cut 1, smoke §14),
> `46293b5` (cut 2, smoke §15), and `207e391` «cut 5 arc closer **(#170)**»
> (which also carries the cut-3 `deception` + cut-4 `fingerprint` functions);
> `node games/web-tiltstone/smoke.mjs` → SMOKE PASS (all green). **Remaining:**
> the cut-5 daily-chain browser-shell opt-in + the optional touch-controls cut
> (below) — small agent follow-ups, neither owner-gated. Arc opened 2026-07-16.
> Source code and
> merged PRs always win over this file; it states the plan and the
> decide-and-flag design calls, updated as cuts land. Kicked off from the owner
> decision menu (docs/NEXT-MENU-2026-07-15.md § B1, the in-doc recommendation)
> → coordinator dispatch, judgment delegated to sharpen the cut list. Base game
> is Tiltstone v1.1 (arc-1 slices 1–5, `games/web-tiltstone/`): a turn-based
> gravity puzzle whose every seed is provably winnable and whose runs are
> **totally deterministic** — same seed + same rotation line = the identical
> game everywhere (browser, Node, CI). Cut 1 (this) draft-parked under the
> standing 2026-07-16 landing wall (no ready-flip / auto-merge). Precedent for
> the proof discipline: the arc-1 pure-Node smoke `games/web-tiltstone/smoke.mjs`
> and the Chromium browser smoke `tools/web-smoke-tiltstone.mjs`.

## The concept (B1, verbatim seed)

> The only game whose session cards left a *designed* second arc: five
> feature-forward ender 💡s, each explicitly deduped against the others, none
> built — together they turn total determinism into the daily/social hook the
> CONCEPT itself calls the game's real value ("'daily challenge' bones … the
> Wordle-shaped hook", `games/web-tiltstone/CONCEPT.md` § Sellability). No
> backend needed; base is v1.1, packaged + repinned (#134).

Tiltstone already ships the hard part of a daily: a UTC-date default seed, so
everyone in the world gets the same cavern chain each day, and provably-winnable
generation, so a stuck player is out of ideas, never out of luck. What it is
missing is the **social loop around** that determinism — the reasons to come
back and the reasons to tell a friend. All five cuts are pure, headless-provable
extensions of the existing deterministic engine; none needs a server, an
account, or any owner setup beyond the already-merged Pages deploy.

## Design invariant across all five cuts

**Every engine change is ADDITIVE — new pure functions only, zero edits to
generation / rotation / settle / resolve / state transitions.** That is what
keeps arc-1's pinned smoke values (the seed-42 grid, quota 7 / budget 10,
solution `RRR`, the level packs, the juice traces) byte-identical as the arc
grows, and it is the same "re-pin everything, add nothing that moves an old
value" discipline arc-1's slices 3–5 held. The proof surface is the same too:
the pure-Node `smoke.mjs` (runs in plain Node, the honest gate) plus the
Chromium `tools/web-smoke-tiltstone.mjs` (shell/UX, local — not CI-wired).

## The five cuts, in build order

Order = value-first, and each cut is self-contained (no cut depends on a later
one). Provenance is the arc-1 ender card whose deduped 💡 seeded it.

### CUT 1 — «Share your line» — BUILT (this PR)

- **Provenance:** `.sessions/2026-07-13-tiltstone-juice.md` § idea.
- **What:** `?seed=N&level=L&replay=RRLR` — a share URL of the player's OWN
  rotation line. Determinism is total, so `(seed, levelIndex, line)` fully
  describes a run; the existing `resolveTrace` turns the triple back into the
  full scored, animated playback for free. Loading a `?replay=` URL plays the
  shared line back as a spectated run to its win/lose card; a Share button copies
  the URL of your current line. Only the player's OWN line is encoded — the
  solver's stays hidden, so par bragging rights survive.
- **Why first:** it is the missing *distribution* half of the daily hook (today
  you can share the cavern, not your line) and unlocks word-of-mouth with the
  least surface — no generator change at all, just new pure encode/decode/replay
  helpers plus shell glue.
- **Proof strategy (shipped):** `smoke.mjs` § 14 — `encodeShare`/`decodeShare`
  round-trip, line validation + normalization, `decodeShare` across all three
  input shapes + null on junk, and the load-bearing assert: `spectate(seed,
  level, line).final` is byte-identical to a live `replay`, and every step's
  trace grid equals the authoritative post-rotation grid, across the seed-42
  line and the 12-seed sweep; a share carries only seed+line.

### CUT 2 — «Hints from the solver» — BUILT (this PR)

- **Provenance:** `.sessions/2026-07-13-web-tiltstone.md` § idea (the stored
  winning line "doubles as a free hint system").
- **What:** an opt-in hint that surfaces the NEXT rotation of a shortest winning
  line from the current board — not the whole solution, one nudge, spend-gated so
  it dings the grade (a hint used counts like an over-par turn on the win card).
  Honest after detours/undos: `hintFrom` re-solves from the board as it stands, so
  it is never a stale parrot of the stored line.
- **Proof strategy (shipped):** pure `hintFrom(state)` returns the next rotation
  for the current grid (re-run the BFS `search` from `state.grid` against the
  remaining budget/quota, take the first move of the shortest winning
  continuation) and `hintedGrade(used, par, hints)` folds the hint count into
  `used`; `smoke.mjs` § 15 asserts a pristine hint equals `level.solution[0]`,
  following hints wins in exactly par (seed 42 + 12-seed sweep), an off-line detour
  re-solves the actual board (or returns an honest null), the hint is never revealed
  for a won/lost/malformed terminal state, and the `hintedGrade` spend-gating truth
  table is pinned. No generator change; arc-1 pins and cut-1 § 14 re-pass
  byte-identical.

### CUT 3 — «Undo×par curation»

- **Provenance:** `.sessions/2026-07-13-tiltstone-par.md` § idea (the undo×par
  cross as a curation metric).
- **What:** record `(undos, used − par)` per cleared level in the existing guarded
  localStorage and use the pair to rate a cavern's *deceptiveness* — a level that
  wins at par but eats 6 undos is HARD-deceptive; par+2 with 0 undos is
  honest-medium. Feeds a richer difficulty label than solution length alone.
- **Proof strategy:** pure `deception(undos, used, par)` scalar with a pinned
  truth table in the smoke; the persistence path stays guarded (private-mode safe)
  and is exercised by the browser smoke. No generator change.

### CUT 4 — «Mechanic fingerprints»

- **Provenance:** `.sessions/2026-07-13-tiltstone-cells.md` § idea (neutralized-BFS
  par deltas → which mechanic carries a level).
- **What:** fingerprint a cavern by which slice-4 cell classes its shortest
  solutions actually exercise — re-run the BFS with each class neutralized (grates
  all-porous, locks pre-freed, ice slip disabled); if par rises or the level dies
  without a class, that mechanic is load-bearing → an honest per-seed tag ("this
  daily NEEDS the grate").
- **Proof strategy:** pure `fingerprint(level)` returning the per-class par-delta
  vector; assert it is deterministic and that a hand-built lock-dependent cavern
  fingerprints as lock-load-bearing. Neutralization is a pure grid transform — no
  edit to `settle`/`resolve`, a parallel BFS input only.

### CUT 5 — «The monotone ramp» — BUILT (this PR, the ARC CLOSER)

- **Provenance:** `.sessions/2026-07-13-tiltstone-packs.md` § idea (difficulty-floor
  re-salt → a provably non-decreasing daily chain).
- **What:** `generateLevel` gains an OPTIONAL difficulty floor and re-salts until
  `difficulty(level).score` clears it — the daily chain becomes a MONOTONE ramp
  (LV n+1 provably rated ≥ LV n) instead of today's flat parameter-bump ramp, which
  can actually sag mid-run (seed 42 dips par 7 → par 5 from LV3 → LV4). The floor is
  driven by a new pure `rampFloor(scores)` (running max of the prefix) and a
  `generateRamp(seed, count, {monotone})` chain builder; both are opt-in and the
  floor defaults OFF.
- **Proof strategy (shipped):** the optional floor is a NO-OP when absent — the accept
  path is byte-identical to before (same RNG stream, same first-valid salt), so every
  existing level pin survives unchanged. `smoke.mjs` **§18** proves it: `rampFloor`
  truths; a **14-seed × 6-level byte-identity sweep** (floor-off `generateLevel` ≡
  `{}` ≡ `{floor:0}` ≡ pre-cut-5); the natural chain pinned and shown to genuinely DIP
  (LV4 < LV3); the **monotone chain (seed 42 ×6)** pinned exactly and asserted
  non-decreasing (`difficulty(LV n+1).score ≥ difficulty(LV n).score`), floor-clearing,
  and pointwise-dominant over the natural chain; LV0 byte-identical to the natural LV0;
  determinism run-twice; and the honest floor-exhaustion throw. This is the one cut that
  touches generation — additively, behind an optional parameter defaulting to the
  current behaviour; engine bumped `1.7.0 → 1.8.0`, no prior value moved. The daily-chain
  shell opt-in is a flagged browser follow-up (it needs a progression refactor with no
  in-container proof); `app.js` is untouched, which is what guarantees default output is
  unchanged.

*(+1 optional touch cut per CONCEPT § Sellability — mobile touch controls — is
tracked there, not here; it is polish, not a determinism feature.)*

## Cut ledger

| Cut | Title | Provenance card | Status | PR |
|-----|-------|-----------------|--------|----|
| 1 | Share your line | tiltstone-juice | **BUILT** | #166 |
| 2 | Hints from the solver | web-tiltstone | **BUILT** | #167 |
| 3 | Undo×par curation | tiltstone-par | **BUILT** | #168 |
| 4 | Mechanic fingerprints | tiltstone-cells | **BUILT** | #169 |
| 5 | The monotone ramp (arc closer) | tiltstone-packs | **BUILT** | this |

**Arc status:** all five cuts BUILT — **growth-complete pending owner clicks**. The
cuts are a stacked draft chain under the standing 2026-07-16 landing wall; land order
**#153 → #166 (cut 1) → #167 (cut 2) → #168 (cut 3) → #169 (cut 4) → this (cut 5)**.
