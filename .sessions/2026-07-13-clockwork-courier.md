# Session 44 — game-lab Track B

> **Status:** `in-progress`

- date: 2026-07-13 (branch `claude/clockwork-courier`, started 19:00Z)
- mission: **CLOCKWORK COURIER — playable prototype slice** (GBA, the
  breadth-game series pattern of Deepcast PR #83 / Cindervault
  PR #86). Selection per ORDER 005 rule 3's precedence ("your inbox
  ORDER carrying my goals verbatim → superbot fleet-grounding §9
  [out-of-repo, unreadable from this container] → **your track plans
  at HEAD** → your generative rung"): the track plan at HEAD,
  `docs/concepts/session-1-concepts.md` @ `8bac80a`, is the committed
  ORDER-001 concept queue — Lumen Drift (BUILT, first-complete) plus
  two named unbuilt concepts. Clockwork Courier (risk MEDIUM) is the
  queue's lower-risk next item; Shoal (risk MEDIUM-HIGH) stays in the
  queue behind its own doc-mandated first-session profiling gate. Not
  an invention: the concept, hook, scope ladder and risk note are all
  committed at HEAD.
- dedup at claim time: games/ has no courier (deepcast, cindervault,
  drift-garden, web-undertow, web-tiltstone are the shipped breadth
  set); no open PR and no claim touches it; only historical pick-menu
  mentions in old session cards.
- session number 44 claimed (43 = Brineward slice 9, PR #94, merged
  `8bac80a`).
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR opens READY on main; no merge/approve/
  auto-merge calls from this session — the server-side enabler
  decides on green.

## Plan (the breadth-series pattern, one increment)

1. `games/clockwork-courier/` over the shared
   `games/common/butano-game.mak` fragment, text-glyph presentation
   (Butano common sprite fonts, both asset paths pinned to
   `third_party/butano/common` — the Deepcast/Cindervault convention,
   no local assets).
2. The HOOK ships first and alone: courier platformer movement
   (LEFT/RIGHT + A jump, fixed-point gravity, tile collision on a
   single-screen glyph tower), R rewinds YOU 5 seconds and leaves a
   GHOST that replays your recorded inputs from the rewound snapshot;
   stand on the ghost / let it hold the pressure switch while
   present-you runs the parcel to the chute. One handcrafted level
   whose door DEMANDS the mechanic; deliver = win card, START
   restarts.
3. Fully deterministic: no RNG at all (handcrafted level, input-driven
   physics); `volatile unsigned cc_telemetry[16]` mailbox written
   every frame for the headless pins (magic, state, frame, positions,
   ghost state/positions, switch/door/parcel/delivered, rewinds,
   buffer fill).
4. Proofs: `tools/headless-screenshot.py` scripted runs — movement
   kinematics pinned to the unit, the REWIND CONTRACT pinned (player
   snaps to the exact 300-frame-old position; the ghost's replayed
   positions equal the recorded originals at fixed offsets — the
   concept's determinism bet, asserted), and the full puzzle solve to
   the DELIVERED card (`--assert-text`), screenshots non-blank +
   distinct. `CONCEPT.md` one-pager (genre, loop, platform,
   sellability guess). `dist/clockwork-courier.gba` byte-deterministic
   + dist/README row. No CI workflow edits (the required "ROM builds"
   job auto-discovers `games/*/Makefile`).
5. Heartbeat: ONE dated section appended at the END of
   control/status.md before the flip.

(Close-out, idea, and previous-session review land at the flip.)
