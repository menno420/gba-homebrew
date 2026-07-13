# Session 43 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/brineward-saves`, 18:22Z → 18:50Z)
- mission: **BRINEWARD arc slice 9 — SAVES.** The final named roadmap
  beat in `docs/concepts/brineward-concept.md` @ main `97e0117`
  (loot/gold → port+upgrades → the Maw → wind → danger bands → audio
  → **saves**), item 7 verbatim: "saves: banked gold / upgrades /
  chart persist (SRAM pattern already proven on GBA; NDS save path is
  a known open question below)." The open question was closed by
  Gloamline's slice 9 in this repo; this slice PORTS that proven
  shape per the session-42 guard recipe ("don't re-derive it").
- authorized by ORDER 005 (`control/inbox.md` @ HEAD `97e0117`); no
  new unserved order at HEAD; collision scan clean at claim time.
- session number 43 claimed (42 = slice 8, PR #91, merged `97e0117`).
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #94 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler (armed since the
  owner's required-contexts click ~17:44Z) decides on green.
- erratum, flagged honestly: the slice commit `8f47ee9` message says
  "6 new host checks" — it is **2** new checks (30 → 32); the count
  everywhere else (this card, the PR body) is correct. Forward-only
  git, so the message stands corrected here.

## 💡 Session idea

**Hermetic-by-default is a proof-harness LAW, not a nicety — and the
leak that teaches it will be in GLOBAL state, not in your diff.** The
slice's only surprise was 14 carried proofs failing in sequence while
each passed alone: DeSmuME keeps an implicit battery per ROM basename
at `~/.config/desmume/<rom>.dsv` — global across runs AND working
directories — so the first proof that banked gold silently fed its
ledger into every later proof's boot. Gloamline never noticed because
its contaminated words (best-nights) were unasserted outside its
explicit-battery proofs — the hazard was ALWAYS there, green by luck.
Rule extracted: **any emulator harness must force every implicit
persistence surface to a known state on every run** (the fix: no
`--battery-in` now means "import a factory-fresh 0xFF chip", which is
what every committed pin had silently assumed). Corollary for the
next persistence-adjacent slice (any platform): before trusting a
proof matrix, run it twice IN SEQUENCE in one directory — a suite
that only ever ran with per-proof isolation has never actually tested
its own independence.

## Previous-session review

- Session 42 (slice 8, audio): its guard recipe was again this
  session's spec and held on every line — words 0-45 stayed frozen
  (mailbox extended 46→50; the 538 carried asserts re-ran unchanged),
  audio state was kept OUT of the save blob as it demanded, no cue
  was inserted mid-table, and the restart-splash quirk was left
  untouched (proof 19 carried verbatim).
- Its headline idea (commit the glue model into the mirror, not
  scratch) paid off immediately: the AudioGlue-carrying scratch model
  extended to the save gate in ~20 lines, and every save-story pin
  was a prediction, not a transcription — 104,000 more watch-log
  values diffed with zero unexplained mismatches.
- One miss worth naming: session 42's proofs never ran twice in one
  directory, so the .dsv hazard sailed through it silently — see the
  idea above; the corollary is now the standing counter-move.

## What this session did

1. Claim + born-red card first (`a9ddb01`), PR #94 opened READY
   immediately after, cut from main @ `97e0117`.
2. **Pure layer** (`bw_sim.{h,c}` + mirror, lockstep):
   `bw_save_checksum/encode/decode` — one 32-byte blob, 8 LE u32
   words {magic 'BRSV', version 1, gold, hull|cannon<<8|sail<<16,
   best_band, 0, 0, checksum-of-0..6}; decode returns 0 on wrong
   magic/version, bad checksum, out-of-range tiers/band, blank/0xFF —
   the fresh ledger stands, never a crash.
3. **ARM9 glue** (`main.c`): Gloamline's HOLD-bit-safe bounded-poll
   SPI helpers ported verbatim in shape (READ/WREN/PAGE-PROGRAM/RDSR,
   BW_SAVE_POLL_BOUND 4096 — the measured no-frame-slip sizing); one
   bounded backup read at power-on restores gold/tiers/chart into
   Score; wear gate writes ONLY when the persisted tuple changes (a
   bank, a port buy/repair, a deeper charted band — sinking writes
   NOTHING); title greets a returning captain ("ledger: Ng banked,
   charted M"). Telemetry 46→50 (46 saveok / 47 writes / 48 version /
   49 spare), words 0-45 frozen.
4. **Host mirror** `check-brine.py`: 2 new checks (32 total) —
   roundtrip over 486 ledgers (gold ladder x every tier combo x every
   band, twice-encoded byte-identical) and rejects (blank/zero chips,
   wrong magic, future version with VALID checksum, out-of-range
   fields with valid checksum, and ALL 256 single-bit flips of a
   valid record). Plus `tools/brine-save.py` (gloam-save's sibling,
   same-mirror lockstep: inspect/expect/blank/corrupt/version-bump).
5. **Harness root-cause fix** (`tools/nds-headless-check.py`,
   decide-and-flag, in-slice): hermetic blank battery whenever
   `--battery-in` is absent — see the idea; both NDS suites re-ran
   green after it (Brineward 22/22 here; Gloamline's 28 verify in
   this PR's CI job).
6. **Proofs 20-22** (nds-brineward-build only; /19→/22 renames; no
   carried assert line touched): 20 = fresh chip, the committed
   seed-127 bank story writes ONCE on the bank frame, exported
   battery byte-identical to mirror encode(15g, 0/0/0, band 0);
   21 = three runs — power cycle restores on the title + idle writes
   nothing (battery cmp-identical), the same story ACCUMULATES
   (15→30g), buys hull II from restored gold, charts band 1 (writes
   1/2/3 pinned) and the deep-water sinking writes NOTHING (ledger
   intact on the card), second cycle boots THAT ledger; 22 = corrupt
   chip (flipped checksum bit) = fresh ledger + the game plays on
   (control seed-118 rakes land on schedule), future-version with
   valid checksum = reset. **All 22 proofs / 616 asserts green
   in-container (538 carried with ZERO re-pins + 78 new); save
   stories watch-log-diffed first: 35,500 + 14,500 + 39,500 + 14,500
   + 20,500 values, 0 mismatches outside the known console-clear/
   one-frame-lag rows (water-begin, port entry/exit, sunk card),
   none pinned — which also proves the bank-frame page program costs
   no frame slip.**
7. **Ship**: `dist/brineward.nds` 121,344 B, sha256 `39c5adcb…aa8f`,
   byte-deterministic (two clean builds identical); PLAYING-BRINEWARD
   gets "The ledger survives"; dist/README row; 3 proof PNGs
   (`slice9-save-{banked,port,title}.png`); recorders re-derive
   byte-identical (13 files); heartbeat appended at the END of
   control/status.md (coordinator convention).

## Guard recipe (for the next Brineward slice — NONE NAMED: the tree is complete)

- **The concept doc's LATER SLICES list is now FULLY SHIPPED (items
  1-7).** Like Gloamline at its slice 9, further Brineward arc work
  is owner-gated — do not invent slices beyond the concept doc; the
  generative rung is new-game breadth, not un-named Brineward beats.
- Words 0-49 of the mailbox are pinned; extend 50→N, never re-map.
- BW_SAVE_VERSION must bump on ANY blob layout change; the version
  gate + 256-bit-flip rejection are load-bearing for proofs 20-22.
- The wear gate compares the LIVE tuple to the as-persisted tuple —
  adding a new persisted field means: blob word, version bump,
  encode/decode + mirror, wear-gate compare, and re-pins of proofs
  20-22 ONLY (the gate keeps everything else zero-re-pin).
- The hermetic-battery harness default is LOAD-BEARING for every NDS
  proof now — removing it re-opens cross-proof contamination.
- The two save proofs' A/START/SELECT continuation frames (650/670/
  700/720) ride the committed salvage story's pier drift window
  (mains 615-683, measured) — a salvage-brain retune re-times them.

## Session enders

- `python3 tools/check-brine.py` — 32 checks green (~2 s).
- All FIVE recorders re-derive byte-identical to committed (13 key
  files).
- From-source rebuild == committed `dist/brineward.nds` bit-for-bit
  (sha256 `39c5adcb1ecc65e8c79b27e6917a742282d16a9e77c701682b5c5d4a
  d653aa8f`); two clean builds identical.
- All 22 headless proofs + 616 asserts green in-container (the same
  commands CI runs, extracted from rom-builds.yml), run SEQUENTIALLY
  in one directory (the new independence bar).
- `rom-builds.yml`: every hunk inside `nds-brineward-build`; YAML
  parses; no assert line of proofs 1-19 touched.
- `python3 bootstrap.py check --strict` — green at the flip.
- Claim `control/claims/brineward-saves.md` retired with this flip.
