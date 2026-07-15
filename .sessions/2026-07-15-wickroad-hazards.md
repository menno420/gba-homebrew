# Session — Wickroad growth cut 3: THE ROAD ITSELF (per-leg hazards)

> **Status:** `complete`

- date: 2026-07-15 (branch `claude/wickroad-hazards`, PR #144; started
  05:24Z, flipped 05:43Z — both from `date -u` at write time)
- mission: **Wickroad growth cut 3 — "The road itself"** per
  `games/wickroad/CONCEPT.md`: per-leg hazards (bandits, weather) and a
  provisions/guard-hire decision, so travel cost stops being flat. One
  contained slice on the v0.3 contracts build (PR #143): single
  `main.cpp`, a fixed authored hazard deck (zero new RNG draws, the
  world stays bit-identical), mirror-first pins, new proof P6 in the
  committed `proofs.sh`, dist v0.4. New always-on UI rides the deferral
  pattern (the #142/#143 frame-budget lesson: dawn frames are SPENT,
  the crier owns quiet frame 1, the pact line quiet frame 2 — the road
  line takes quiet frame 3) and the sprite-tiles pool is audited at the
  worst redraw frame (the #143 measured-pool lesson).
- **📊 Model:** Fable-family · medium · feature build — wickroad growth cut 3
- landing posture: PR #144 opened READY on main at the born-red commit;
  no merge/approve/auto-merge calls from this session — the server-side
  enabler decides on green.

## What shipped

1. Born-red gate (commit d51606d): this card `in-progress` +
   `control/claims/claude-wickroad-hazards.md`, PR #144 opened READY
   immediately; claim retired at the heartbeat (80cedd0) per claims
   README rule 4.
2. **The hazard law** (commit 9f923fb, `games/wickroad/src/main.cpp`):
   a FIXED three-hazard deck on the rumor/contract-deck pattern —
   (announce day, window, leg, kind, seizure) — H1 RAID on the
   GLASSMERE road D15-16 seizing 12, H2 STORM on the THORNBY road
   D19-22 costing an unprovisioned crossing one whole extra day (a
   second dawn camped, 1 gold lodging), H3 RAID on the DUNWICK road
   D25-28 seizing 15. Zero new RNG draws and zero price-law changes:
   a hazard is a pure function of (leg, ARRIVAL day), and every window
   is authored after day 14 (the committed P2/P3/P5 routes' last
   travel day is 13), so the v0.3 world is bit-identical and P1-P5
   carried verbatim on the first post-change run. LEFT (the last
   unused d-pad key) HIRES THE GUARD: outfit_fee 4 covers the NEXT
   crossing against either kind, hazard or not — the fee spends either
   way, which is what makes the ROAD line's advance warning worth
   reading. Kinds price differently on purpose: bandits tax gold,
   storms tax the clock, and days are the win condition's currency.
3. **The ROAD line** (trading UI + a title hint), regen deferred to the
   THIRD quiet frame after a head redraw (crier first, pact second —
   the #142/#143 stagger, extended); in the trading screen it takes the
   row the static verb-help line held (full help stays on the title
   card), so the always-on sprite load is a wash and the 256-item pool
   holds with no bump needed.
4. **Telemetry**: `wr_telemetry` 32 -> 40 — hazard witness words
   (latest announced id, kind, leg, window first/last day, outfitted
   flag, gold seized cumulative, storm days lost cumulative); words
   0-31 byte-for-byte unchanged.
5. **Proof P6 THE ROAD ITSELF** (`games/wickroad/proofs.sh`): H1
   announced on day 13 pinned two days before its window (words 32-36
   + the ROAD line exact), the UNGUARDED crossing losing exactly the
   authored seizure (gold 45 -> 33 on one edge, word 38 = 12), LEFT
   hiring (fee 4, word 37: 0 -> 1, "+GUARD" on screen), the GUARDED
   crossing of the SAME stretch in the SAME window losing nothing
   (word 38 unchanged, guard consumed), the UNPROVISIONED storm
   crossing losing exactly one day (day 18 -> 20 on one travel edge,
   word 39 = 1), the PROVISIONED crossing of the same window losing
   none (day 20 -> 21), H3's late announcement, the cut-0 stale-ink
   pair still biting at day 23 (word 14 = 34 true vs word 15 = 7939),
   and day-23 gold 12 pinning the whole route ledger. Suite total
   P1-P6: **520 watch + 59 text assertion passes**, exit 0; P2-P6 each
   run twice, watch-logs byte-identical; the whole suite run twice
   end-to-end, CSVs byte-identical across runs. Every pin derived on
   the host-side Python mirror FIRST; the ROM matched all watch pins
   on the first probe of the route — the only two first-probe failures
   were text-side (item 6).
6. **Two measured text-side facts** (documented in proofs.sh): (a) the
   guarded road line "BANDITS: GLASSMERE RD D15-16 +GUARD" measurably
   clips its last glyph at the 240px screen edge from ui_x — fixed by
   the shorter RAID token (the deck's longest guarded line now fits
   with margin); (b) the head redraw lands 1-2 frames after its key
   edge — a DETERMINISTIC parity of the input script, measured
   frame-by-frame (day-13 flip renders at edge+4, day-23 at edge+5) —
   so one road-line text assert moved a frame later, and the parity is
   documented where the pins live.
7. **Ship** (commit d0f0238): `dist/wickroad.gba` v0.4, 123,844 B,
   sha256 `7c061301d028b9e97cee8e4f43fd556a7228c73a7680464efdf9cbf5
   722bf8d3`, two clean builds byte-identical, suite re-run green at
   the dist bytes; dist/README row updated; CONCEPT.md cut 3 marked
   **SERVED**; docs/current-state.md Wickroad row -> v0.4 (close-out
   commit 80cedd0).
8. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold); re-run post-
   flip green (see PR).

## 💡 Session idea

**In a deterministic system, even the jitter is a constant — measure
it once and pin against it instead of padding for it.** The one P6
assert that failed on the first probe failed because the head redraw
lands 1-2 frames after its key edge, and WHICH of the two it is
varies by position in the input script. The instinct from flaky-test
culture is to add slack everywhere ("assert at edge+8 to be safe").
The deterministic-house move is different: the parity is a pure
function of the committed route, so it is itself pinnable — measure
the flip frame-by-frame (two shots either side), assert exactly
there, and write the measured parity down next to the pins. The
committed suite then documents the system's true timing instead of
hiding it under padding, and a future one-frame drift (a real
regression) still fails loudly instead of vanishing into the slack.
Corollary for text lines: screen-edge clipping is part of the pin
too — the longest STATE of a line (suffixes included) has to be
proven to fit, not the longest content you happened to assert.

## Previous-session review

- Prior lane slice: `.sessions/2026-07-15-wickroad-contracts.md` (PR
  #143, growth cut 2) — its 💡 ("draw-side resource pools are part of
  the frame-budget interface; audit the engine's static pools against
  the WORST redraw frame") held up and directly shaped this slice: the
  ROAD line was deliberately made a sprite WASH (it takes the row the
  static help line held) rather than a net add, and the 256-item pool
  survived the full suite with zero halts — the #143 freeze never
  recurred. Its explicit corollary — "bump the config with a comment
  recording the measured overflow" — is why the Makefile needed no
  touch this cut: the pool was already sized to a measured peak. One
  honest improvement: that card's frame-of-mind was allocator-side
  only; this session's two first-probe failures were both DISPLAY-side
  (screen-edge clipping, edge-processing parity), so the widened rule
  this card's idea records is: the committed interface is cycles AND
  allocator headroom AND the literal pixels — measure all three where
  the pins live.
