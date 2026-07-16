# Session — Underroot arc slice 6: hawks predate exposed shallow route cells

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/underroot-slice-6`, stacked on
  `claude/underroot-slice-5` @ 706876f; started ~17:07Z, `date -u`)
- mission: **Underroot arc slice 6 — hawks predate.** Add the predation layer:
  the hawk shadows that sweep the meadow (slice 1) catch foragers on the
  **exposed** (shallow, near-surface) cells of the forager route, while cells
  buried below the surface are **safe**. Model, pure integers only: a route
  cell at grid row `< UR_SAFE_DEPTH` is EXPOSED, at row `>= UR_SAFE_DEPTH` is
  DEEP; the colony loses `min(exposed route cells, population)` foragers to the
  hawks and `survivors = pop − losses`. The route counted is slice 3's
  shortest dug path (`ur_forage` → the canonical BFS path to the nearest
  reachable patch's drop cell), so predation is the deterministic consequence
  of the tunnel you drew: a route that hugs the surface is hawk-food, burying
  it (digging a deeper-only path so the forager's shortest route dives below
  the exposed band) costs more digging but survives the meadow — the arc doc's
  "Deep or dead" pressure. Proven host-side (catch geometry exact at the
  exposed↔deep boundary; the two `min` regimes — exposure-bound and pop-bound;
  a disconnected/no route predates nothing) plus a ROM `ur_telemetry`
  predation lockstep on the scripted exposed route — the slice-6 row of
  `docs/arcs/UNDERROOT.md`.
- **📊 Model:** Opus 4.x · high · task-class: gameplay increment
- landing posture: **DRAFT, stacked on #159**, base `claude/underroot-slice-5`.
  The standing 2026-07-16 LANDING WALL (PR ready-flips + auto-merge
  classifier-denied — recorded in `docs/PLATFORM-LIMITS.md` and the
  #153/#155/#156/#157/#158/#159 bodies, all parked DRAFT at this same wall)
  makes honest draft-park the terminal state; the land path is an owner
  ready-click, in stack order
  #153 → #155 → #156 → #157 → #158 → #159 → this. No merge / ready / auto-merge
  calls from this session; the pre-existing PRs (#153–#159) untouched.

## What shipped

[[fill: numbered, on completion]]

## Known / honest gaps

[[fill: on completion]]

## 💡 Session idea

[[fill: on completion]]

## Previous-session review

[[fill: on completion]]
