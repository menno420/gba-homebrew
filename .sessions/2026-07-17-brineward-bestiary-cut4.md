# Session — Brineward bestiary cut 4: THE HOLD TRACK (crate-cap economy)

> **Status:** complete

- date: 2026-07-17 (branch `claude/brineward-bestiary-cut4`, STACKED on
  `claude/brineward-bestiary-cut3` @ `02f0e57`; all timestamps from
  `date -u`)
- mission: **Brineward arc B3 — the bestiary, cut 4 «The hold track»** —
  the ARC CLOSER — per the arc plan
  `games/brineward-nds/ARC-BESTIARY.md`: build the concept's "**Hold cap
  starts at 8 crates**" (`docs/concepts/brineward-concept.md` §
  Progression) as a real *progression* beat — a FOURTH Graywake port
  upgrade line that raises `BW_HOLD_CAP` in tiers, so deeper, richer,
  Grasper-haunted water is worth the hold to carry it home. The
  port-ledger pattern (slice 4, `check_port_buy`): **tier 0 IS the legacy
  `BW_HOLD_CAP` (8) exactly** — the pin-carry identity — tiers strictly
  raise it, prices triple. Additive on cuts 1–3 and all of slice 2..9:
  every existing Brineward host proof and recorded route re-derived
  **byte-identical** because a fresh tier-0 row is the legacy constant
  and no committed route buys the new upgrade.
- **📊 Model:** Opus family · high effort · feature build (cut 4 «The hold track» — the crate-cap port economy, arc closer)
- landing posture: DRAFT, held draft (standing 07-16 landing wall). NO
  ready-flip, NO merge/approve/auto-merge from this session. **Stacked**:
  base = `claude/brineward-bestiary-cut3` (PR #178), NOT main. Arc closer:
  land order is #176 → #177 → #178 → this.

## What shipped

1. **Born-red gate** (commit `f67fd7a`): this card `in-progress` +
   `control/claims/claude-brineward-bestiary-cut4.md`, filed before build.
   The card holds the PR red until this close-out flip (the LAST commit).
2. **The hold track — pure sim** (`bw_sim.{h,c}`, commit `4b8b470`): the
   crate-cap economy as a fourth port line. `BW_UP_HOLD_OF[BW_HOLD_TIERS=3]
   = {BW_HOLD_CAP, 12, 16}` — tier 0 IS the legacy cap EXACTLY (the
   pin-carry identity), tiers strictly RAISE it; `BW_UP_HOLD_COST =
   {0,15,45}` triples per step (the concept-doc ladder, same shape as the
   other three tracks). `BW_PORT_ROW_HOLD = 4` **APPENDED** (rows 0..3 and
   REPAIR keep their indices, so every prior port proof/route holds);
   `BW_PORT_ROWS = 5`. `bw_port_buy` owns the new row with its own table +
   ceiling; `bw_up_hold_max/_price` accessors. `bw_loot_step` caps at
   `BW_UP_HOLD_OF[d->up_hold]` (tier 0 = `BW_HOLD_CAP`, so a stock hold
   scoops bit-identical to slice 3). New `up_hold` field, reset in
   `bw_duel_init` like the other tiers (caller re-injects). Save: `up_hold`
   packs into the record word's reserved high byte
   (`hull|cannon<<8|sail<<16|hold<<24`) — a stock hold (tier 0) serializes
   byte-identical to the slice-9 blob; decode validates `hold <
   BW_HOLD_TIERS`. `gcc -std=c11 -Wall -Wextra -fsyntax-only` clean.
3. **Host proof** (`tools/check-brine.py`, commit `d87c3d8`): the
   line-for-line mirror extended — `check_upgrade_tables` and
   `check_port_buy` now cover the 4th (hold) track table-aware (tier-0
   identity, strict raise, triple price, no cross-talk, the water
   untouched), and **1 new check** — `check_hold_track`: (a) the tiered
   cap fills to EXACTLY its ceiling, tier 0's cap == `BW_HOLD_CAP`; (b) the
   PAYOFF — a deep haul the STOCK hold banks 40g of (leaving 5 crates
   afloat) a bought hold banks 60g of (leaving 1): worth the hold to carry
   it home. `save_roundtrip` now sweeps the hold byte (1458 ledgers);
   `save_rejects` tests an out-of-range hold tier where it used to test the
   now-claimed reserved byte. **ALL GREEN.** All **8 pre-existing recorders
   re-derive byte-identical** (duel/salvage/maw/wind/bands/grasper/
   breakfree/ambush, re-run locally: `diff`-clean).
4. **Committed route** (`record-bighold.py` → `proof/bighold-keys.txt`,
   commit `956d44f`): the salvage recorder's sibling for the crate-cap
   economy — records the win + salvage + bank story on the shift-stable
   anchor (seed 127) and proves the payoff on that exact route: a sloop
   coming home already carrying a running haul meets the wreck's crates and
   overruns the stock hold — the STOCK hold (tier 0, cap 8) banks 40g and
   LEAVES 1 crate afloat; a bought hold (tier 1, cap 12) carries the whole
   9-crate haul home and banks 45g. Deterministic (two runs byte-identical).
5. **ROM wiring** (`games/brineward-nds/source/main.c`, commit `00f400e`):
   `Score.up_hold` carried like the other three tiers (begin_duel injects,
   the port buy persists, the ledger saves it via the extended
   encode/decode + wear gate + `sv_hold`). The Graywake port draws a fourth
   line — HOLD (row 4, appended; help lines shift to 19/20) showing the
   next tier's crate cap and triple price; the salvage HUD and status hold
   readouts scale to `bw_up_hold_max(up_hold)`. One new telemetry witness
   word `BW_T_HOLDCAP = 56` (array 56 → 57, the additive append cut 3 used
   for slot 55) — the live crate cap, 8 at tier 0, the bought ceiling
   above. Tier 0 is the default everywhere, so the ROM is bit-identical
   until a purchase.
6. **CI** (`.github/workflows/rom-builds.yml`, commit `c9a6a9b`): the
   `record-bighold` sync diff, and ROM **proof 26** «the hold track» —
   reuses proof 7's proven seed-127 salvage + berth window, walks the port
   cursor DOWN to the appended HOLD row (row 4, `t:23`=4), and A buys hold
   tier I: gold 15 → 0 (`t:18`) and the crate cap `BW_T_HOLDCAP` 8 → 12
   (`t:56`, the witness). BEFORE the buy the cap reads 8 == `BW_HOLD_CAP`
   (the tier-0 identity live in the ROM); START out, the next duel (seed
   748) sails with the bigger hold persisting. Watch depth 56 → 57 for the
   new slot.
7. **Arc-closer docs** (commit `c091f32`): `ARC-BESTIARY.md` status flipped
   `arc-plan` → **arc-built, growth-complete pending owner clicks** (all
   four cuts flagged built with PR refs); `docs/current-state.md` Brineward
   row notes the bestiary arc B3 built as the stacked draft chain.

## Honest limits

- **The NDS ROM is NOT built in this worker env** (no BlocksDS/Wonderful
  toolchain). `bw_sim.c` is host-gcc-syntax-clean and every rule is
  host-proven in `check-brine.py`, but `main.c` (libnds) and ROM proof 26
  are compiled/pinned only by CI (`nds-brineward-build`). Proof 26's frame
  pins are **mirror-PREDICTED** (reusing proof 7's proven seed-127 berth
  window — only the navigation depth and the bought row shift), not
  emulator-confirmed here. Any slip is fixable with a normal follow-up
  commit (draft).
- **The >8-crate «deep haul» bank is host-proven only, by design of
  scope.** Cut 4 is a PORT UPGRADE LINE — it raises the crate CAP, not the
  crate SUPPLY. A natural water still sheds at most `BW_LOOT_DROPS` crates
  (a monster's carcass a few more) — never more than the stock hold of 8.
  So the "deep haul the stock hold could not carry" is host-verified
  (`check_hold_track` manufactures the over-stock; `record-bighold` gives
  the sloop a pre-carried running haul the sim's per-water hold-reset
  cannot itself express). This is the exact sibling of cut 3's host-proven
  «lethal» route: the ROM witnesses the ECONOMY (buy it, cap 8→12, it
  persists across power cycles), the host proves the payoff. **The natural
  loot-scaling that would let an ordinary deep water overflow the stock
  hold is a clean follow-up beat** — the honest edge of this cut.
- **The hold upgrade stays SILENT** (audio is the game's deferred roadmap
  item 6). A buy is a HUD line and a telemetry tick, no cue — the same
  known gap as cut 1's silent Grasper, cut 2's silent wrench, cut 3's
  silent ambush.

## 💡 Session idea

**A save format's *reserved* byte is a pre-paid extension slot — claim it
and the whole persistence chain extends for free with zero migration.**
Cut 4 needed to persist a fourth tier. The instinct is to grow the record:
add a word, bump `BW_SAVE_VERSION`, and write a migration path for old
saves (or accept that every existing ledger resets on the version bump).
But slice 9 had, on purpose, packed the three tiers into ONE word's low
three bytes and reserved the high byte as `must-be-0` (a decode gate:
`(w[3]>>24)!=0` → reset). That reserved byte was a *pre-paid slot*: cut 4
packed `up_hold<<24` into it, and because tier 0 = 0, a stock ledger
serializes **byte-identical** to the slice-9 blob — same magic, same
version, same checksum, same 32 bytes. No version bump, no migration, no
reset: an existing save loads verbatim and simply reads its new field as
tier 0. The only decode change was tightening the gate from "high byte
must be 0" to "high byte must be a valid hold tier" — the reject that used
to fire on ANY nonzero high byte now fires on an out-of-range one, so the
single-bit-flip and corruption guards still hold (a flip breaks the
checksum first anyway). The general move: when you reserve space in a
serialized format, reserve it as *a field whose zero value is the current
behavior*, gated by a "must be the zero/legacy value" decode rule — then a
future feature claims the slot by simply relaxing that rule to "must be a
valid value," and every old record stays valid because it already carried
the legacy zero. Forward-compat isn't a migration you write later; it's a
default you *pick* when you first draw the byte map. (This is the save-
layer twin of the arc's whole spine — cut 1 reserved a seed bucket, cut 2
a zero-valued input, cut 4 a legacy-constant tier-0 row AND a
zero-reserved save byte; every gate is "the legacy value is the zero
value," so the extension is additive by construction.)

## Guard recipe

**When you APPEND to a fixed menu / row-indexed surface, append at the
END and keep every prior index nailed — do NOT insert "where it reads
nicely."** Cut 4 adds a HOLD line to the Graywake port. The tidy place is
before REPAIR (upgrades together, repair last). But `BW_PORT_ROW_REPAIR =
3` is a live index: ROM proof 8 navigates `DOWN×3` to land the cursor on
row 3 and asserts `t:23 == 3` is REPAIR, and `main.c` draws each row at
screen line `9 + 2*row`. Insert HOLD at row 3 and REPAIR slides to 4 —
proof 8's cursor now lands on HOLD, its repair-cost assertions read the
wrong row, and the pin breaks even though nothing about repair changed.
Fix: `BW_PORT_ROW_HOLD = 4` (a NEW last row), `BW_PORT_ROWS = 5`, rows 0..3
untouched; the menu's `DOWN` clamp (`< BW_PORT_ROWS-1`) simply reaches one
further, and no existing proof presses `DOWN` a fourth time so none reach
it. The recipe: a row/slot index that any committed route or emulator pin
navigates by COUNT is part of the frozen surface — extend it only past the
last used index, never by insertion, and verify by grepping every
`DOWN`/`row`/`t:23` assertion in the proof set before you renumber.
(Verified: the port ROM proofs 7 and 8 press `DOWN` at most 3 times and
assert rows 0..3 — proof 26's 4th `DOWN` to row 4 is the only one that
reaches the new line, so proofs 7/8 carry verbatim. Same lesson as cut 3's
telemetry-append rule: `BW_T_HOLDCAP = 56` was appended past the last used
slot 55, shifting no existing index.)

## Previous-session review

- Prior lane slice: **PR #178** — *Brineward bestiary cut 3: «The ambush»
  (converging cutters)* (head `02f0e57`, DRAFT, base =
  `claude/brineward-bestiary-cut2`). Read at HEAD via the git tree and the
  GitHub API (`pull_request_read`): confirmed DRAFT, `mergeable_state:
  unstable` (substrate-gate red **by design** — main's #151 doc orphans +
  the born-red HOLD card, not a build failure), 7 commits, +915/-5 over 10
  files. Cut 4 branched from its tip `02f0e57` (confirmed `git rev-parse`,
  matched the expected head) and does not stack on sand: the Brineward host
  proof + all 8 recorder byte-identities are green at that tip.
- **What transferred, confirmed:** cut 3's gate spine is cut 4's spine one
  more turn — *keep the default path byte-identical behind a gate whose
  zero/legacy value is the current behavior, then append the proof.* Cut 1
  gated on a seed bucket, cut 2 on a zero-valued input, cut 3 on a
  sub-bucket of cut 1's bucket, cut 4 on a **legacy-constant tier-0 row**
  (`BW_UP_HOLD_OF[0] == BW_HOLD_CAP`). And cut 3's **append-past-the-last-
  index** discipline (it added telemetry slot 55 as a "free spare that
  shifts no existing index") transferred verbatim to two surfaces at once:
  `BW_T_HOLDCAP = 56` (telemetry) AND `BW_PORT_ROW_HOLD = 4` (the port
  menu) — both appended past the last used index so every prior pin/route
  carries. The host-mirror-first rule held too: every rule is proven in
  `check-brine.py` before the ROM, and the recorder re-derives in CI.
- **A real divergence worth naming (what the ROM can witness):** cut 3
  sharpened cut 2's "add nothing you can already observe" into "a
  composition invisible on the winning route needs exactly ONE witness" —
  and added `BW_T_CUTTERS` because the survived route is byte-identical to
  a plain break-free without it. Cut 4 inherits that lesson but hits its
  *limit*: the thing cut 4 most wants the ROM to witness — a >8-crate bank
  — the ROM **cannot** naturally reach, because the crate supply caps below
  the stock hold. So the witness word `BW_T_HOLDCAP` witnesses the
  *economy* (the cap the port sells), not the *payoff* (the deep haul),
  and the payoff moves to a host proof. The sharpened rule: *a witness can
  only pin what the winning route can reach; when the feature's payoff is
  unreachable on any natural ROM route, witness the CAUSE (the purchase and
  its effect on state) and prove the EFFECT (the bank) host-side.* Cut 3
  witnessed a composition that HAPPENS on its route; cut 4 witnesses a
  capability that its route BUYS but cannot yet spend.
- **Landing-posture divergence (deliberate):** like every cut on the one
  before, cut 4 is **stacked** (base = `claude/brineward-bestiary-cut3`,
  PR #178), not re-branched from main — the hold-cap economy is the arc's
  progression payoff and reads as the closer only atop the three monster
  cuts it lets you carry the loot of. Four-deep stack, land order #176 →
  #177 → #178 → this. This is the **arc closer**: `ARC-BESTIARY.md` is
  flipped to growth-complete, the arc awaits only owner landing clicks.
- **Guard recipe honored:** cut 3's "append past the last used index" (it
  cited cut 1's two-checkout footgun staying shut) held on both new
  surfaces here; every Bash pinned to `/home/user/gba-homebrew`, and the
  8-recorder byte-identity re-derived from that one tree before every
  commit — never pushed red.
