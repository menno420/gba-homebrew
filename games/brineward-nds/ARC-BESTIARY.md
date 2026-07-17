# Brineward — arc B3: the bestiary

> **Status:** `arc-plan` — the second Brineward arc (owner menu B3,
> `docs/NEXT-MENU-2026-07-15.md`). The base is arc slice 9 (loot, port,
> upgrades, the Maw, wind, danger bands, audio, the Graywake ledger save).
> This doc plans the arc in build order and pins each cut's proof
> strategy; `docs/current-state.md` wins as the repo moves.

## Why this arc

The concept doc promises a monster **class** and shipped one member. From
`docs/concepts/brineward-concept.md`:

- **§ Core loop / Monsters:** "a maw that surfaces under you (watch the
  shadow), **arms that grapple and hold you still while cutters close
  in**." Only the Maw was built (`bw_sim.c` slice 5).
- **§ Controls sketch:** the B button is literally "**(reserved: later
  ram/brace verb)**" — a designed, unbuilt verb.
- **§ Progression:** "**Hold cap starts at 8 crates**" — the hold-cap
  economy track was never built as a *progression* beat.

The bestiary arc builds the promised second monster and the systems the
concept sketched around it. Four cuts, in build order:

## The four cuts (build order)

### Cut 1 — «The Grasper» — the second sea monster  ✅ (this PR)

The break-the-geometry **sibling** of the Maw, with the OPPOSITE gimmick:
where the Maw is a telegraphed one-shot LUNGE you dodge, the Grasper
**reaches** up out of the deep and, if the arms close on you, **seizes**
the sloop and **pins it still** — momentum, helm, and way all dead — for a
fixed hold, taking one seize's worth of hull, then the arms slip and reach
again. Cut 1 is the Grasper ITSELF: the grapple-and-HOLD verb, woundable
while up and slayable for richer crates, pier sanctuary intact. Cut 1's
escape is **spatial** (the Maw's lesson): keep sea room off the reach — a
full-sail beam clears it, a battle-sail scooper does not.

- **Additive gate (the pin-carry rule):** a water holds a Grasper OR a
  Maw, never both — `bw_has_grasper(seed)` buckets the seed, and
  `BW_GRASPER_SALT` is pinned so every committed anchor and every
  host-checked salvage seed is a NON-grasper (Maw) water. In a Maw water
  `bw_grasper_step` is a pure no-op and `bw_maw_step` is unchanged, so
  every slice-2..9 route and pin carries **bit-identical**. The Grasper
  lives only in the salvage water; `bw_duel_step` never steps it, so duel
  routes carry too.
- **Proof:** five host checks in `tools/check-brine.py`
  (`check_grasper_pincarry / _holds / _sanctuary / _slain / _containment`)
  prove the pin-carry identity, the frame-exact seize/hold contract, the
  escapability rail, the pier sanctuary, the slay-and-shed, and
  adversarial containment — for whole input families. A committed route
  (`record-grasper.py` → `proof/grasper-*-keys.txt`) drives the seize on a
  real grasper-water seed; a ROM proof (`rom-builds.yml` proof 23) pins it
  against `bw_telemetry`. Existing host output AND every committed route
  re-derive **byte-identical**.

### Cut 2 — «Ram/brace» — the reserved B verb

Build the concept's reserved B button as the **break-free** verb: a
braced ram that lets a seized sloop wrench loose from the Grasper's hold
early (at a cost), and doubles as a low bow-on shove against a ship at
close quarters. Per the input-verb-gate rule
(`.sessions/2026-07-13-brineward-bands.md`, the SELECT precedent): a fresh
input verb that NO committed story presses re-pins **nothing** — every
slice-2..9 and cut-1 route carries verbatim because none of them press B.

- **Proof strategy:** B is edge-triggered in `BwInputs` (a new field or a
  reuse gated so its zero-value is the legacy behavior); host checks prove
  a braced break shortens the hold to its cost frames and that no
  B-silent route changes. A recorded «break free» route shows a held
  sloop wrenching loose. Zero re-pins by the gate.

### Cut 3 — «The ambush» — the Grasper holds while cutters close in

Complete the concept's full sentence: while the Grasper **holds** you,
**cutters close in** — light enemy sloops that converge on the pinned
ship, making the hold *lethal* (the tempo loss cut 1 only hinted at). The
ambush composes cut 1's hold with cut 3's converging AI.

- **Proof strategy:** the mirror-diff method
  (`.sessions/2026-07-12-session-31.md`): the ambush fires only in a
  grasper water at a seize, so the gate keeps every non-ambush route
  bit-identical; host checks prove the cutters converge on a held sloop
  and that the braced break (cut 2) is the counter. A recorded «ambush
  survived» route.

### Cut 4 — «The hold track» — the hold-cap economy

Build the concept's "Hold cap starts at 8 crates" as a **progression**
track: a fourth port upgrade line that raises `BW_HOLD_CAP` in tiers, so
deeper, richer, Grasper-haunted water is worth the hold to carry it home.

- **Proof strategy:** the port-ledger pattern (slice 4,
  `check_port_buy`): tier 0 IS the legacy `BW_HOLD_CAP` exactly (the
  pin-carry identity), tiers strictly raise it, prices triple; the
  hold-cap host checks re-run with tier 0 and carry byte-identical. A
  recorded «bigger hold» route banks a deep haul the stock hold could not.

## Standing rails (every cut)

- **Bit-identity first:** every cut sits behind a gate (a seed bucket, a
  fresh input verb, or a legacy-constant tier-0 row) so every prior route
  and emulator pin carries verbatim. The host mirror asserts the identity
  loudly; the route recorders re-derive byte-identical in CI.
- **Host-provable before the ROM:** the frame-exact contract lives in
  `tools/check-brine.py` (the `bw_sim.c` line-for-line mirror, lockstep
  rule); the ROM proof confirms the wiring surfaces it.
- **Original assets/text only** (PUBLIC repo). Audio stays deferred (the
  game's roadmap item 6) until an owner-taste pass; the Grasper is silent
  in cut 1 and the deep's drone still keys off the Maw.
- **Draft-park:** open DRAFT, leave it draft under the standing 07-16
  landing wall; born-red HOLD by design; substrate-gate red = #151 doc
  orphans on main + the born-red card.
