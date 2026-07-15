# Session — Wickroad growth cut 4: A WIDER MAP + PACK UPGRADES

> **Status:** `complete`

- date: 2026-07-15 (branch `claude/wickroad-wider-map`, PR #145; started
  05:54Z, flipped 06:21Z — both from `date -u` at write time)
- mission: **Wickroad growth cut 4 — "A wider map + pack upgrades"** per
  `games/wickroad/CONCEPT.md`: 7 towns, mules that grow capacity — gold
  buys logistics, the classic Taipan curve. One contained slice on the
  v0.4 hazards build (PR #144): single `main.cpp`, the world widened
  from the SAME seed (16 world-init draws appended strictly after the
  v0.4 stream — the counted RNG delta; prior cuts drew zero), mirror-
  first pins, new proof P7 in the committed `proofs.sh`, dist v0.5.
  The map deliberately stays ONE road — no branch fork (L/R is a
  committed verb grammar; a junction would need a new travel verb) —
  the honest cut named in CONCEPT.md's SERVED note.
- **📊 Model:** Fable-family · medium · feature build — wickroad growth cut 4
- landing posture: PR #145 opened READY on main at the born-red commit;
  no merge/approve/auto-merge calls from this session — the server-side
  enabler decides on green.

## What shipped

1. Born-red gate (commit becb3ad): this card `in-progress` +
   `control/claims/claude-wickroad-wider-map.md`, PR #145 opened READY
   immediately; claim retired at the heartbeat (c321cef) per claims
   README rule 4.
2. **The wider map** (commit 1e0188d, `games/wickroad/src/main.cpp`):
   the road runs on past DUNWICK to HOLLOWFEN (the drovers' fair) and
   MIRGATE (the far iron source) — `town_count` 5 -> 7, same closed-
   form price law and produce/crave rotation (HOLLOWFEN sells salt
   cheap/craves resin, MIRGATE sells iron cheap/craves tallow), each
   new town with its own aging ledger ink. The RNG stream order is a
   committed interface: the five legacy towns draw base-then-phase
   EXACTLY as v0.1-v0.4 (40 draws), the two new towns' 16 draws append
   strictly after — the old world is bit-identical and P1-P6 carried
   verbatim on the first post-change run.
3. **The mules**: START — free in the trading state (every committed
   route only presses it on the title and end cards) — buys a mule at
   the Hollowfen fair on a fixed authored ladder (30 then 55, then
   sold out), each mule growing the pack by 4 (8 -> 12 -> 16). The
   fair posts the next price on the existing town/pack line (no new
   always-on regen frames); the two NEW ledger rows regenerate on
   quiet frames 4 and 5 after a head redraw (crier 1, pact 2, road 3 —
   the #142-#144 stagger extended), so the measured dawn workload
   holds and the 256-item sprite pool needed no bump.
4. **Telemetry**: `wr_telemetry` 40 -> 48 — wider-map witness words
   (capacity, mules owned, posted fair price, cumulative mule spend,
   the MIRGATE-iron stale-ink pair, visited-towns bitmask, free pack
   space); words 0-39 byte-for-byte unchanged.
5. **Proof P7 THE WIDER MAP + THE MULES** (`games/wickroad/proofs.sh`):
   the committed P2 prefix re-banks gold 269 at day 9 under the widened
   map (the bit-identity proof), the fair posts 30 on arrival (word 42
   + "MULE 30" on the town line), the purchase edge to the gold piece
   (267 -> 237, capacity word 8 -> 12, ladder walks to 55), NINE salt
   in the pack — past the old cap of 8 (words 8/47), MIRGATE arrival
   with mask 0x7F (all seven towns on one route) and its ledger inked
   at 8192 (iron 32 @ age 0), the 9-sell haul to gold 280 (authored
   under the 300 win), the ride back where the new region's ink
   provably LIES (true 35 vs word 45 = 8193 — the cut-0 hook stretched
   to the new map), and the second mule selling the fair out (gold 223,
   capacity 16, word 42 -> 0, spend word 85). Hazard words 0 all route;
   word 31 = 0 (no pact taken). Suite totals P1-P7: **655 watch + 68
   text assertion passes**, exit 0; P2-P7 each run twice, watch-logs
   byte-identical; the whole suite run twice end-to-end, CSVs
   byte-identical across runs. Every pin derived on the host-side
   Python mirror FIRST (a route simulator replays the exact key
   schedule against the law); the ROM matched every watch pin on the
   first route probe.
6. **One measured text-side fact** (documented in proofs.sh): the
   three first-probe failures were all text asserts on FIXED-font
   ledger rows — `--assert-text` templates are built from the VARIABLE
   font bmp (`tools/headless-screenshot.py` FONT_BMP), so fixed-font
   rows are not text-assertable (P1-P6 never asserted one either); the
   new towns' ledger states are pinned exactly via watch words 44/45
   instead.
7. **Ship** (commit 1e0188d): `dist/wickroad.gba` v0.5, 125,016 B,
   sha256 `ad4e477bb027e2b0bf54f0b7090c372cb248a34986b48fffa4dfb854
   aee8e31a`, two clean builds byte-identical, suite re-run green at
   the dist bytes; dist/README row updated; CONCEPT.md cut 4 marked
   **SERVED**; docs/current-state.md Wickroad row -> v0.5 (close-out
   commit c321cef).
8. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold); re-run post-
   flip green (see PR). The check's auto-appended
   `.substrate/guard-fires.jsonl` delta is deliberately NOT committed
   by this session (dispatch rail: no writes under `.substrate/`,
   deny-wins — noted here so the divergence from the #142/#143 pattern
   is explained, not mysterious).

## 💡 Session idea

**A committed RNG stream is append-only — treat the draw ORDER like a
wire format, and growth cuts can widen a seeded world without
re-rolling it.** The naive way to add towns 6 and 7 was to bump
`town_count` and let the existing `for t < town_count` init loops run
wider — which silently shifts every phase draw for the OLD towns eight
positions down the stream and re-rolls the entire committed world (all
~60 pinned prices, every route, every proof). The wire-format move:
freeze the legacy loops at `legacy_town_count` exactly as shipped, and
append the new entities' draws after the last legacy draw, documenting
the count as the cut's RNG delta. The old world stays bit-identical BY
CONSTRUCTION (P1-P6 passed on the first post-change run — zero re-pin),
and "zero new draws" generalizes honestly to "N draws, appended,
counted". Corollary: any loop that consumes a shared deterministic
stream is secretly a serialization format — version it like one, and
name the freeze point in the code where the next session will widen it
again.

## Previous-session review

- Prior lane slice: `.sessions/2026-07-15-wickroad-hazards.md` (PR
  #144, growth cut 3) — its 💡 ("in a deterministic system even the
  jitter is a constant — measure it once and pin against it; the
  longest STATE of a line has to be proven to fit") held up and
  directly shaped this slice: P7's text asserts were placed off the
  measured head-redraw parity from the start (edge+4 on
  immediate-regen lines, end-of-route frames for deferred ones) and
  every new on-screen state was width-checked against the 240px edge
  before authoring (the fair line's longest state "AT HOLLOWFEN
  PACK 9/12  MULE 55" fits with margin) — zero clipping or parity
  failures this cut, against two last cut. One honest improvement:
  that card's display rules covered geometry (pixels, frames) but not
  the harness's FONT domain — this cut's three first-probe failures
  were all attempts to text-assert FIXED-font rows, which the
  variable-font template reader structurally cannot see. The widened
  rule: the committed display interface is cycles AND allocator AND
  pixels AND the assert-tooling's own reach — check what the harness
  can READ before pinning what the screen shows.
