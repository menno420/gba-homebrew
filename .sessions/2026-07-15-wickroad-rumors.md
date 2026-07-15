# Session — Wickroad growth cut 1: RUMORS (the ledger becomes a forecasting tool)

> **Status:** `complete`

- date: 2026-07-15 (branch `claude/wickroad-rumors`, PR #142; started
  04:05Z, flipped 04:24Z — both from `date -u` at write time)
- mission: **Wickroad growth cut 1 — "Rumors"** per
  `games/wickroad/CONCEPT.md`: a deterministic rumor/event deck — town
  criers telegraph future price shocks ("iron soars at Glassmere by
  day 5"), the shock then actually happens, so the ledger becomes a
  forecasting tool, not just a memory. One contained slice on the v0.1
  prototype (PR #129): single `main.cpp`, closed-form turn sim,
  mirror-first pins, new proof in the committed `proofs.sh`, dist v0.2.
- also served: ORDER 007 acknowledgment (EAP extended through
  2026-07-21; 2026-07-14 dormancy orders superseded; NO routines
  re-armed pending the owner's per-seat go) — landed on the heartbeat
  (control/status.md dispatch section, commit 019a0e8).
- **📊 Model:** Fable-family · medium · feature build — wickroad growth cut 1
- landing posture: PR #142 opened READY on main at the born-red commit;
  no merge/approve/auto-merge calls from this session — the server-side
  enabler decides on green.

## What shipped

1. Born-red gate (commit 2e03cda): this card `in-progress` +
   `control/claims/claude-wickroad-rumors.md` (bootstrap-rendered),
   PR #142 opened READY immediately; claim retired at the heartbeat
   (019a0e8) per claims README rule 4.
2. **The rumor law** (commit 7b5a621, `games/wickroad/src/main.cpp`):
   a FIXED three-rumor deck — (announce day, hit day, town, good,
   delta) — with each shock one more closed-form term of the same
   clamped price law, live for a 3-day window from its hit day.
   Deterministic by construction: zero new RNG draws (the v0.1 world
   is bit-identical), announce/display a pure function of the day.
   The foretold price is computed FROM the sim law (drift at the hit
   day + shock, impact-free), so telegraph == realization exactly
   when the player's own impact there is zero. Deck targets avoid the
   committed P2/P3 routes' pinned markets, so P1-P3 carried verbatim.
3. **The crier line** (trading UI, house glyph style) with its regen
   DEFERRED one frame past dawn redraws: probing showed v0.1's dawn
   frames ride the frame budget's edge (a 2-frame iteration already
   existed at P2 frame 66/67), and an added line on announce-day dawns
   tipped iterations over, silently missing later 2-frame key presses.
   The deferral is a pure function of the input history — determinism
   untouched, and the committed v0.1 pins keep their frame alignment.
4. **Telemetry**: `wr_telemetry` 16 -> 24 — rumor witness words (id,
   hit day/town/good, foretold, live realized, applied shock, phase
   1-telegraphed/2-landing/3-passed); words 0-15 unchanged.
5. **Proof P4 THE RUMORS** (`games/wickroad/proofs.sh`): a 20-day wait
   route sweeps the whole deck — each foretold (58/11/42) pinned
   BEFORE its shock with the true price still disobeying (37/21/25,
   `ne` asserts), realized == foretold at each hit day (the R2 crash
   also witnessed on the visible market via cursor word 13 = 11), the
   window passing (shock 0, plain law 31, phase 3), crier lines exact
   on screen, gold 60 -> 40 pinning 20 lodgings. Run twice —
   watch-logs byte-identical. Suite total P1-P4: **278 watch + 34 text
   assertion passes**, exit 0. Every pin derived on the host-side
   Python mirror of the sim law FIRST; the ROM matched the mirror
   exactly on the first probe — zero route iterations on hardware.
6. **Ship**: `dist/wickroad.gba` v0.2, 122,404 B, sha256 `ebcc10f1f5a
   6686664e51463b3a22b77d1651ccb69f3116075c7aef8bbe5fdea`, two clean
   builds byte-identical, suite re-run green at the dist bytes;
   dist/README row updated; CONCEPT.md cut 1 marked **SERVED**.
7. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold; guard-fires
   telemetry committed with the slice) + pre-existing model-line-shape
   advisories on 2026-07-14 cards (not this lane's files, advisory,
   left alone).

## 💡 Session idea

**When a proof suite's pins encode FRAME ALIGNMENT, treat the frame
budget as a committed interface — and put new draw work on the frames
the pins don't own.** The v0.1 pins assert sim state on the SAME frame
as a key edge, which silently pins "one loop iteration fits one frame,
except where measured otherwise." Adding one text line to dawn redraws
broke P2 two hundred frames downstream (a 2-frame press fell inside a
2-frame iteration) with zero diff in sim code. The diagnosing move
worth repeating: watch the ROM's own frame counter against the harness
frame index (lag = iterations lost) before touching any pin. The fix
worth repeating: defer cosmetic regeneration to the first quiet frame
after a state-change redraw — scheduling stays a pure function of the
input history, so determinism survives and the budget-owning frames
stay at their measured cost. Corollary for future cuts (contracts,
hazards): the frame budget on dawn frames is now SPENT; any new
per-dawn UI must ride the deferral pattern, not the dawn frame.

## Previous-session review

- Prior lane slice: `.sessions/2026-07-14-wickroad.md` (PR #129, the
  prototype) — its 💡 ("write the sim twice; make the mirror the route
  planner") held up EXACTLY as promised: this session's mirror,
  extended with the rumor law, predicted all 8 pin frames × 8 rumor
  words and the ROM matched on the first probe, zero iterations. One
  improvement named honestly: the prototype card's claim that "every
  assert frame sits >= 4 frames after its key edge" is not true of its
  own P2 (several pins sit ON the edge frame, e.g. frame 60) — that
  undocumented same-frame dependency is what made the frame-budget
  regression (idea above) possible. This slice documents the real
  contract in the P4 comment block instead of restating the wrong one.
- Also read: `.sessions/2026-07-14-seat-dormant.md` (PR #139) — its
  dormancy record did its job (this rebooted session knew exactly what
  was parked and why), and ORDER 007's supersession is now
  acknowledged on the heartbeat above it, keeping the SEAT DORMANT
  text intact as history.
