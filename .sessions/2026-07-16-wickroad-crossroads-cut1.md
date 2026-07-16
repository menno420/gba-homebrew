# Session — Wickroad crossroads cut 1: THE JUNCTION

> **Status:** `in-progress`

- date: 2026-07-16 (branch `claude/wickroad-crossroads-cut1`, PR
  [[fill: PR#]]; started 22:04:36Z from `date -u` at write time)
- mission: **Wickroad crossroads cut 1 — "THE JUNCTION"** per
  `docs/NEXT-MENU-2026-07-15.md` § B2 and the new arc plan
  `games/wickroad/ARC-CROSSROADS.md`: the second arc finally builds the
  idea `CONCEPT.md` named inside a served cut — the map stops being one
  linear road. Refactor the bare linear town index into a small static
  adjacency table (the spine 0↔1↔…↔6 stays bit-identical), hang ONE
  off-spine branch town — **WYRMHOLLOW** (index 7) — off the mid-spine
  junction **DUNWICK** (4), and add the first new travel verb since
  v0.1: the **L+R chord** ("take the fork"), purely additive because no
  committed route presses both shoulders on one frame. RNG: +8 draws
  appended strictly after town 6's at the wider-map freeze point (old
  world bit-identical → P1-P8 carry verbatim). Telemetry `[52] → [56]`
  (words 52-55 appended). Proven by new proof P9. One contained slice
  on the v0.6 audio build (PR #146). Original assets only (WYRMHOLLOW
  is an original name).
- **📊 Model:** [[fill: model family · effort · feature build — wickroad
  crossroads cut 1]]
- landing posture: PR opened DRAFT and held draft per the standing
  2026-07-16 landing wall — no ready-flip, no merge/approve/auto-merge
  calls from this session; card stays in-progress by design (born-red
  HOLD).

## What shipped

1. Born-red gate (commit [[fill: SHA1]]): this card `in-progress` +
   `control/claims/claude-wickroad-crossroads-cut1.md` +
   `games/wickroad/ARC-CROSSROADS.md` (the crossroads arc plan), PR
   [[fill: PR#]] opened DRAFT immediately and held draft.
2. **The junction** (commit [[fill: SHA2]],
   `games/wickroad/src/main.cpp`): [[fill: adjacency table, WYRMHOLLOW
   town 7 off DUNWICK 4, L+R fork verb, RNG +8 appended, telemetry
   52-55 — one honest paragraph.]]
3. **RNG** — [[fill: town_count 7 → 8, spine_town_count 7, +8 draws
   (4 base + 4 phase) appended strictly after town 6 at the freeze
   point; old draws keep their order → world bit-identical.]]
4. **Telemetry**: `wr_telemetry` 52 → 56 — [[fill: words 52-55: on-
   branch flag / fork-edge id / branch price witness / reserved; words
   0-51 byte-unchanged.]]
5. **Proof P9 THE JUNCTION** (`games/wickroad/proofs.sh`): [[fill:
   route + pins + suite totals; P1-P8 verbatim; P9 twice byte-
   identical; ALL WICKROAD PROOFS PASS.]]
6. **Ship** (commit [[fill: SHA]]): `dist/wickroad.gba` v0.7,
   [[fill: bytes]] B, sha256 [[fill: sha256]], two clean builds byte-
   identical; dist/README row updated; CONCEPT.md junction-served note;
   docs/current-state.md Wickroad row → v0.7.
7. CI on the head SHA: [[fill: ROM builds result + run-id; substrate-
   gate red by design.]]

## 💡 Session idea

[[fill: ONE genuine lesson — e.g. the adjacency-refactor-under-byte-
identical-constraint discipline, or the all-buttons-consumed →
chord-verb pattern.]]

## Previous-session review

[[fill: review the prior lane slice — Tiltstone arc 2 cut 5 / PR #170
(its `.sessions/*.md` card) or the newest Wickroad card
`.sessions/2026-07-15-wickroad-audio.md` — one honest paragraph on its
💡 / what it did.]]
