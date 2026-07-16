# Session — Underroot arc slice 10: Seed dial + balance (pick/scan the year seed; a difficulty/escalation pass)

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/underroot-slice-10`, stacked on
  `claude/underroot-slice-9` @ 19660ac; started ~18:40Z, `date -u`)
- mission: **Underroot arc slice 10 — Seed dial + balance** (docs/arcs/UNDERROOT.md
  slice-10 row): pick/scan the year seed with a runtime SEED DIAL, thread the
  dialed seed through the whole `f(seed,season,index)` schedule, and land a
  difficulty/escalation pass whose balance invariant — **a fair seed is always
  survivable with the reference plan** — is proven headless-side. Plus slice-9's
  forward note: **gate the START record-commit on a realized winter**
  (`season == UR_WINTER`), so the persisted best is an outcome the colony
  actually reached, not a spring forecast. Determinism-first throughout: the
  dial→seed wiring, the reference plan and its winter score are pure integer
  functions, mirrored line-for-line in `tools/check-underroot.py`.
- **📊 Model:** Claude Opus 4.8 family · high · task-class: gameplay increment
  (seed selection / balance + save-gate)
- landing posture: **DRAFT, stacked on #163**, base `claude/underroot-slice-9`.
  Under the standing 2026-07-16 LANDING WALL (PR ready-flips + auto-merge
  classifier-denied); honest draft-park is the terminal state. Card stays
  `in-progress` — no ready-flip, no merge / auto-merge calls from this session;
  PRs #153–#163 untouched. Land order: #153 → #155…#163 → this.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-10.md`, first commit `0b663bc`), PR
   opened DRAFT immediately, base `claude/underroot-slice-9`.
2. **The seed dial** in `games/underroot-nds/source/ur_sim.{h,c}`:
   - `ur_dial_seed(dial)` — the pure year seed each of `UR_DIAL_COUNT` (8) dial
     positions selects. **Dial 0 is UR_SEED itself**, so the whole slices 1..9
     skeleton/marquee/save proof is bit-identical at the dial's home; other
     positions hash off `UR_SEED ^ UR_DIAL_SALT` ('DIAL') into the bounded seed
     space `[1, UR_DIAL_MASK+1]`, giving 8 DISTINCT deterministic years
     `[1370, 746, 2171, 2630, 2191, 3704, 894, 2575]`.
   - `main.c` scans the dial with **LEFT/RIGHT** (a `keysDown` edge steps it one
     position, wrapping), guarded by `pad_seen_idle` (the KEYINPUT boot-trap
     note) so a boot-all-pressed emulator can't phantom-scan. The dialed
     `run_seed` then feeds the WHOLE `f(seed,season,index)` schedule (hawks,
     patches, forage, the whole economy chain) — every prior `UR_SEED` call site
     in the loop now reads `run_seed`. A player-visible `seed N (dial D/8) fair`
     readout on the meadow.
3. **The balance / difficulty pass** in `ur_sim.{h,c}`:
   - `ur_ref_plan` — a canonical REFERENCE survivable plan: a row-1 harvest
     corridor across every column (all patch drops reachable for any seed), that
     whole row designated granary (capacity 64, over any patch total), and NO
     nursery (population 0 → winter demands nothing → the banked store IS the
     score).
   - `ur_ref_score(seed)` / `ur_seed_fair(seed)` — the reference plan's winter
     score is exactly the reachable winter meadow food, always `>= UR_FAIR_FLOOR`
     (18) `> 0`, so **no dial seed is a death-trap** (the balance guarantee). The
     score still VARIES by seed (dial ref scores `[38,36,35,42,34,30,32,34]`), so
     the dial spans a real difficulty range 30..42 (the escalation). The
     reference plan is a survivability WITNESS — a skilled deep-banked colony
     scores far higher (the slice-8 marquee).
4. **The winter-gated record commit** in `main.c` (slice-9's forward note): the
   START commit now requires `season == UR_WINTER`. Slice 8 evaluates the winter
   exam as a live FORECAST in the growing seasons, so a spring START would bank a
   projected number the run never realized; gating the commit to winter — the
   one season the exam IS the realized outcome — means the persisted `best_score`
   is a score the colony genuinely reached. `best_seed` now records the dialed
   `run_seed`.
5. **Telemetry** appended additively: `UR_T_DIAL` (49) / `RUNSEED` (50) / `FAIR`
   (51) / `REFSCORE` (52), pushing `UR_T_SPARE` → 53 and widening the mailbox to
   `UR_T_WORDS = 54`. Every pre-slice-10 index (0..48) is **bit-identical** — the
   additive-layer discipline slices 7/8/9 kept, one slice further. The
   fairness witness is recomputed only on a dial change (a bounded BFS sweep),
   never per frame.
6. **Host mirror** `tools/check-underroot.py`: line-for-line `ur_dial_seed` /
   `ur_ref_plan` / `ur_ref_score` / `ur_seed_fair` + `prove_balance`
   (**4153 cases**) — (a) wiring purity: `ur_dial_seed` deterministic/wrapping,
   dial 0 == UR_SEED, 8 distinct seeds, each drives the hawk/patch schedule
   purely; (b) the fairness floor: every dial seed AND every seed in
   `[0, UR_DIAL_MASK+1]` scores `>= UR_FAIR_FLOOR` (`ur_seed_fair == 1`
   everywhere); (c) the escalation spread; (d) the reference-plan shape (all
   patches reachable, pop 0). Plus a `marquee_winter_fixture` pinning the
   survivor plan's REALIZED winter score (store 21 → wstore 9, wpop 3, SURVIVE →
   **3003**, season 3) and the new golden bytes
   `encode(3003, winter, UR_SEED) = 5653525501000000bb0b0000030000005a0500000000000000000000a01b3b12`.
   `ur_sim.c` was host-compiled (`-std=c11 -Wall -Wextra -Werror`, clean) and
   **cross-checked bit-equal** to the Python mirror on `ur_dial_seed` /
   `ur_ref_score` / `ur_seed_fair` for all 8 dial seeds + wrap.
7. **CI** (`nds-underroot-build`): the mailbox watch widened to
   `t:ur_telemetry:54`; **proof 1** pins the dial home defaults (t[49]=0 dial,
   t[50]=1370 run seed, t[51]=1 fair, t[52]=38 ref score); **proof 5** rewritten
   winter-gated — a SPRING START is BLOCKED (t[47] stays 0 in spring), the year
   clock runs to WINTER (t[3]=3), and a WINTER START commits the REALIZED score
   (best t[43]=3003, furthest season t[44]=3, seed t[45]=1370, one write
   t[47]=1); the exported battery is byte-identical to
   `underroot-save.py expect --score 3003 --season 3 --seed 1370`; **proof 6**
   (new) scans the dial (two RIGHTs → dial 2, seed 2171) and asserts the meadow's
   patch-sum FOLLOWED the dialed seed (t[16]=32) and the fairness witness tracks
   it (t[51]=1, t[52]=35).
8. Rails held: PUBLIC repo, original IP only, zero pokemon-mod-lab content; no
   `.substrate/` writes; claim/card cite the branch/base SHA (19660ac);
   timestamps `date -u`.

## 💡 Session idea

**The reference plan is a *survivability* witness (pop 0, so it only ever scores
the banked store); the natural slice-11 successor is a *skill* witness — a pure
`ur_par_score(seed)` that scores a deep, populated colony, turning the dial's
`REFSCORE` from "is this year winnable?" into "what does a good run on this year
look like?"** Slice 10 proves the balance FLOOR: every dialable seed clears
winter with the store-only reference plan, so the game is never unwinnable. But
that plan deliberately raises no population — its score is the meadow food, never
the `surviving_pop*1000` the headline chases — so it says nothing about the
*ceiling* a seed rewards. A companion pure function that builds a deep,
buried-route, nursery-fed colony (the shape the slice-8 marquee hand-built and
slice-6 predation demands) and returns its winter score would be a **par** for
the year: the two witnesses together bracket every seed — floor (survivable at
all) and par (what banking a margin buys) — both legible and both provable in the
`prove_balance` style for free, since each is a pure `f(seed)` with no forward
dependence. That makes the seed dial not just a difficulty *choice* but a
difficulty *ladder with a known top*, the raw material for slice 11's audio/HUD
polish to surface ("this year's best is N") without any runtime search. It is the
same move slice 10 made on slice 9's idea — from *gating* the record on a real
winter to *characterizing* what that winter can hold.

## Known / honest gaps

- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as slices
  1–9), so `games/underroot-nds` is **not compiled for the ARM target locally** —
  the ROM build + boot/dig/year/winter/save/dial telemetry proofs are green only
  when CI `nds-underroot-build` runs. What *did* run locally: the host mirror
  (stdlib-only, all proofs green incl. the 4153-case `prove_balance`), a
  host-`gcc` compile of `ur_sim.c` (`-std=c11 -Wall -Wextra -Werror`, clean), and
  a `gcc` harness linking `ur_sim.c` that cross-checked `ur_dial_seed` /
  `ur_ref_score` / `ur_seed_fair` bit-equal to the Python mirror for all 8 dial
  seeds + wrap. `main.c`'s new dial-scan input, `run_seed` wiring, winter-gate,
  and four telemetry writes were audited **by eye** against the header/call sites
  (the slice-5/6 lesson: a cross-compile-only signature mismatch is what host gcc
  can't see across the libnds TU). `KEY_LEFT`/`KEY_RIGHT`/`keysDown` are the same
  libnds symbols already linked for the stylus verbs and the START commit — no
  new libnds surface.
- **The winter-commit ROM proof is pinned from the host mirror, not a read ROM
  run.** Proof 5's winter values (store 21, wstore 9, score 3003 at season 3) are
  what `marquee_winter_fixture` computes; the ROM shares `ur_sim.c`, and the
  winter *timing* reuses proof 3's already-green pin (emulator frame 3800 ⇒
  t[3]=3), so pressing START there and asserting the commit is a small increment
  over the proven winter drive. If CI surfaces a boot-latency straddle at the
  winter START frame, it is a normal follow-up-commit fix (no force-push).
- **The realized winter still banks the WINTER patches, not an autumn harvest.**
  Gating the commit to `season == UR_WINTER` makes the persisted score the score
  the sim computes *at* the winter frame; `ur_store` is not abundance-scaled, so
  that score is the reachable winter-patch food (self-consistent and
  deterministic), not a store frozen at the last growing season. A true
  harvest-season store model (which season's patches feed winter) is a larger
  semantic change than slice 10's scope and is left as an honest deferral — the
  gate closes the spring-*forecast* over-optimism slice 9 named without dragging
  in a new season model.
- **The reference plan is a survivability WITNESS, not a score maximizer.**
  `ur_ref_score` proves a seed is *beatable* (floor), deliberately with pop 0; it
  is not the highest score a seed can yield. The idea above proposes the par
  companion. This is called out honestly in the header and the `prove_balance`
  comments so no reader mistakes "survivable" for "optimal".
- **`furthest_season` is now exercised at WINTER in the ROM (season 3), all
  seasons in the mirror.** The winter-gate means the ROM commit pins
  `best_season = 3`; the full improves-on-further-season truth table (0..3) is
  proven exhaustively host-side (`prove_save`), the slice-8/9 pattern.
- **The slice-5 `ur_nurse` signedness asymmetry** (unsigned `col,row` vs
  `ur_designate`'s `int32_t` + `< 0` guard) that slices 6→7→8→9 tracked stays a
  one-line follow-up. Slice 10 adds **no** new grid-coordinate entry points (its
  new functions take a dial index / a seed, or build the fixed reference plan via
  the existing `ur_dig`/`ur_designate`), so — as in slices 7–9 — there was
  nothing to align.
- **`substrate-gate` red is inherited, not from this slice.** `main` carries 5
  known orphan-doc failures (#151); the check is expected-red on this branch too.
  What must be green — the "NDS Underroot build" job — is this slice's
  responsibility.
- **Stacked on an unlanded base.** Slice 9 (PR #163) is draft-parked at the
  standing wall, so this branch stacks on `claude/underroot-slice-9`, not `main`.
  It lands only after the stack ahead does; order
  #153 → #155 → #156 → #157 → #158 → #159 → #160 → #161 → #162 → #163 → this.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-9.md` (**PR #163**, head
  `19660ac`, EEPROM best — best `SCORE` / furthest season persisted via the card
  backup). It shipped the fixed 32-byte record + `ur_save_*` / `ur_record_improves`
  + `prove_save` (1756 cases) + the `--battery-in/out` roundtrip (fresh chip → one
  write → bytes == mirror encode; power cycle loads; equal run writes nothing;
  corrupt/version-bump = fresh table). **Slice 9's close-out idea was the exact
  hinge this slice needed**: it foresaw that the record then commits a winter
  *forecast*, not a *realized* outcome, and asked that "the moment slice 10's seed
  dial lets a run actually cross into winter, the SCORE commit should gate on
  `season == UR_WINTER`". Slice 10 does exactly that — the seed dial lands, the
  commit is gated to winter, and the persisted best (3003, season 3) is now a
  score the colony genuinely reached, not a spring projection.
- **Concrete strength observed reviewing the slice-9 code:** slice 9 was
  scrupulously **additive** — the six save words were appended (43..48), pushing
  only `UR_T_SPARE`, so no slice-1..8 telemetry index moved. That is exactly what
  let slice 10 land cleanly: I appended FOUR more words (49..52) and pushed only
  `UR_T_SPARE` → 53, so every pre-slice-10 `--assert-watch` index (0..48) is
  bit-identical and no earlier proof needed re-pinning. The one place slice 10
  *changed* a slice-9 proof — proof 5's save roundtrip — was unavoidable and
  intended: the winter gate deliberately moves the committed record from the
  spring forecast (2008, season 0) to the realized winter (3003, season 3), so
  the golden bytes and the START-press frame move with it; the change is the
  behavior slice 9 asked for, not drift.
- **Concrete decision slice 9 handed forward, now honored honestly:** slice 9
  persisted the forecast faithfully and NAMED the over-optimism class rather than
  silently carrying it. Slice 10 closes it with the smallest gate that fits
  (`season == UR_WINTER`), and is transparent (see the honest gaps) that this
  banks the winter-patch food, not an autumn-frozen store — closing the
  *forecast* gap slice 9 flagged without inventing a harvest-season model beyond
  the slice-10 scope.
- **Its `📊 Model:` three-field form** is mirrored here at **family** level
  (`Claude Opus 4.8 family · high · task-class: gameplay increment`). Slice 9's
  honest gaps (no local NDS build; telemetry pinned from the host mirror; one
  battery scenario per behaviour) carry forward unchanged and true here.

## PR / CI (filled at close-out)

- PR: **#164** — https://github.com/menno420/gba-homebrew/pull/164 (DRAFT,
  base `claude/underroot-slice-9`; impl commit `27bea03`, card commit is the
  branch tip). Draft-parked under the landing wall — no ready-flip, no
  auto-merge.
- CI (`nds-underroot-build` + the gates): recorded in the session report /
  status; substrate-gate red is the inherited #151 orphan set, not this slice.
