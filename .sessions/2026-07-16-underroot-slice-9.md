# Session — Underroot arc slice 9: EEPROM best (best SCORE / furthest season)

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/underroot-slice-9`, stacked on
  `claude/underroot-slice-8` @ ec27203; started ~18:22Z, `date -u`)
- mission: **Underroot arc slice 9 — EEPROM best** (docs/arcs/UNDERROOT.md
  slice-9 row): the best `SCORE` and furthest season persist across power
  cycles on the cartridge backup chip, read/written over the card SPI bus (the
  battery DeSmuME emulates as a `.sav`). One fixed 32-byte record — 8
  little-endian u32 words `{ magic, version, best_score, furthest_season,
  run_seed, 0, 0, checksum }` — so serialization is fully deterministic. Safety
  by construction: a corrupt / blank / future-version blob decodes to the fresh
  all-zero table (NEVER a crash or hang). Writes are wear-disciplined: the
  record commits ONLY on a discrete player action (START) and ONLY when the run
  strictly improves the record. The `gl_save` precedent (Gloamline slice 9, PRs
  #50–#75) is reused verbatim for the hand-rolled bounded card-SPI I/O.
- **📊 Model:** Claude Opus family · high · task-class: gameplay increment
  (persistence / save layer)
- landing posture: **DRAFT, stacked on #162**, base `claude/underroot-slice-8`.
  Under the standing 2026-07-16 LANDING WALL (PR ready-flips + auto-merge
  classifier-denied); honest draft-park is the terminal state. Card stays
  `in-progress` — no ready-flip, no merge / auto-merge calls from this session;
  PRs #153–#162 untouched. Land order: land #153 → #155…#162 → this.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-9.md`, first commit `7763716`), PR
   opened DRAFT immediately, base `claude/underroot-slice-8`.
2. **The best-score save record** in
   `games/underroot-nds/source/ur_sim.{h,c}`:
   - The record: 8 little-endian u32 words `{ UR_SAVE_MAGIC ('URSV'),
     UR_SAVE_VERSION 1, best_score, furthest_season, run_seed, 0, 0, checksum }`
     — exactly one 32-byte type-2 EEPROM page.
   - `ur_save_checksum` — a `ur_hash` fold on the `UR_SAVE_SALT` stream over
     words 0..6; `ur_save_encode` / `ur_save_decode` — deterministic
     serialize / parse. Decode returns 1 ONLY if magic, version AND checksum all
     match; returns 0 (outputs untouched, the caller's fresh table stands) on
     ANY other 32-byte input — never a crash.
   - `ur_record_improves(best_score, best_season, score, season)` — 1 iff a
     higher score OR a further season; a tie in BOTH writes nothing (the EEPROM
     wear discipline).
   - Six telemetry words `UR_T_BEST` (43) / `BESTSEASON` (44) / `BESTSEED` (45)
     / `SAVEOK` (46) / `SAVEWR` (47) / `SAVEVER` (48), pushing `UR_T_SPARE` → 49
     and widening the mailbox to `UR_T_WORDS = 50`.
3. **The ARM9 backup I/O + commit** in `main.c`:
   - Bounded hand-rolled card-SPI (`save_read_backup` / `save_write_backup`:
     READ 0x03 / WREN 0x06 / PAGE PROGRAM 0x02 / RDSR 0x05, 2-byte addressing)
     copied **verbatim** from the proven Gloamline slice-9 path — the two
     measured fixes (drop `CARD_SPI_HOLD` before the last byte so DeSmuME sees
     the chip-select edge; every wait bounded by `UR_SAVE_POLL_BOUND` so a chip
     that never answers is a sub-frame stall, never a hang) carried across
     unchanged.
   - One backup read at power-on decodes the record (`save_ok`); a **discrete
     START press** commits the live winter `SCORE`/season to the record ONLY
     when `ur_record_improves` — one bounded page program per improving commit,
     never per frame, never on a tie. Guarded by `pad_seen_idle` (the KEYINPUT
     boot-trap note) so a boot-all-pressed emulator can't phantom-commit.
   - Player-visible: a `best N` readout on the meadow's title row.
4. **Host mirror** `tools/check-underroot.py`: line-for-line
   `ur_save_checksum`/`encode`/`decode`/`record_improves` + `prove_save`
   (**1756 cases**) — (a) encode/decode roundtrip byte-exact over a value grid
   incl. extremes + a **golden-bytes pin** `encode(2008, spring, UR_SEED) =
   5653525501000000d8070000000000005a0500000000000000000000789dc7fa`; (b)
   every blank (all-00/all-FF), every single-byte corruption, and a wrong-magic
   and a future-version blob EACH WITH A RECOMPUTED VALID checksum reject to the
   fresh table; (c) `ur_record_improves` strictly-better-or-further truth table.
   `ur_sim.c` was host-compiled (`-std=c11 -Wall -Wextra -Werror`, clean) and
   **cross-checked bit-equal** to the Python mirror on the golden bytes, the
   decode roundtrip, the all-FF reject and the improves truth samples.
5. **Save tool** `tools/underroot-save.py` (the `gloam-save.py` sibling):
   inspect / expect / blank / corrupt / version-bump a record inside a RAW
   battery image, all through the check-underroot mirror so a C-encoder drift is
   caught at byte level.
6. **CI** (`nds-underroot-build`): the mailbox watch widened to
   `t:ur_telemetry:50`; the boot proof gains the fresh-table save zeros
   (t[46]=0 save-loaded, t[43]=0 best, t[44]=0 season, t[47]=0 writes, t[48]=1
   version); a new **proof 5** drives the `--battery-in/out` cycle:
   - **5a** fresh 0xFF chip → the survivor marquee plan climbs to SCORE 2008
     (t[42], settled 390) with STILL no write (t[47]=0), then **START at 395
     commits ONE write** (best t[43]=2008, season t[44]=0, seed t[45]=1370,
     writes t[47]=1); the exported battery is byte-identical to
     `underroot-save.py expect --score 2008 --season 0 --seed 1370`.
   - **5b** power cycle with 5a's battery loads the record on the title
     (t[46]=1, best 2008 before any input); an equal (empty) run presses START
     but improves nothing (t[47] stays 0) — the battery is `cmp`-identical
     (zero wear).
   - **5c** a checksum-flipped save AND a future-version save each boot to the
     fresh table (t[46]=0, best 0) with no crash — the version gate rejects on
     its own.
7. Rails held: PUBLIC repo, original IP only, zero pokemon-mod-lab content; no
   `.substrate/` writes; claim/card cite the branch/base SHA (ec27203);
   timestamps `date -u`.

## 💡 Session idea

**The record today commits a winter *forecast*, not a *realized* outcome — so
the moment slice 10's seed dial lets a run actually cross into winter, the
SCORE commit should gate on `season == UR_WINTER`, and that gate is provable
by construction.** Slice 8 evaluates the winter exam as a live forecast in the
growing seasons (the marquee digs and reads the verdict in spring), and slice 9
faithfully persists exactly that number: a START press in spring banks the
*predicted* winter score. That is honest for a one-screen replay, but it opens a
latent over-optimism class the instant the year clock is allowed to run a full
loop with a fixed plan (slice 10): the meadow's patch layout is
season-dependent (`ur_patch(seed, season, index)`), so a plentiful-summer
forecast can bank a score that the *actual* winter — with its own predation and
its own realized store — never delivers, and the EEPROM would then advertise a
best no run ever achieved. The fix is small and self-contained: gate the commit
to `season == UR_WINTER` (the one season where the forecast *is* the realized
outcome, abundance 0, nothing left to change), so the persisted `best_score` is
an outcome the colony genuinely reached, never a projection. And it is provable
in the `prove_winter` style for free: `ur_winter_score` at the winter season is
a pure `f(dig plan, gran, nurs, seed)` with no forward dependence, so a host
sweep can assert `committed_score == simulate(plan, seed).winter_score` for the
committed frame — turning "the record is achievable" from a trust into a proof.
This is the natural successor to slice 8's own idea (pin `best_score(seed) > 0`
as the *fairness* invariant): there the guard proves a seed is *beatable*; here
the guard proves the number the save *brags about* is one the game actually
handed out — the two halves of making the survival score, the arc's headline,
both legible and honest.

## Known / honest gaps

- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as slices
  1–8), so `games/underroot-nds` is **not compiled for the ARM target
  locally** — the ROM build + boot/dig/year/winter/save telemetry proofs are
  green only when CI `nds-underroot-build` runs. What *did* run locally: the
  host mirror (stdlib-only, all proofs green incl. the 1756-case `prove_save`),
  a host-`gcc` compile of `ur_sim.c` (`-std=c11 -Wall -Wextra -Werror`, clean),
  a `gcc` harness linking `ur_sim.c` that cross-checked the golden bytes /
  decode / all-FF reject / improves samples bit-equal to the Python mirror, and
  the full `underroot-save.py` inspect/expect/blank/corrupt/version-bump cycle
  on a crafted battery image. `main.c`'s new save I/O + boot read + START commit
  + six telemetry writes were audited **by eye** against the header/call sites
  (the slice-5/6 lesson: a cross-compile-only signature mismatch is what host
  gcc can't see across the libnds TU). The card-SPI I/O is copied **verbatim**
  from the CI-proven Gloamline slice-9 path precisely to avoid an ARM-only trap;
  `keysDown` and the `REG_AUXSPI*`/`CARD_*` macros are the same libnds symbols
  Gloamline links.
- **The committed score is a FORECAST (see the idea above).** In the marquee
  the record is committed in spring off the live winter forecast; that is what
  the proof-5 pins assert. Gating the commit to the winter season waits on the
  slice-10 seed dial (a fixed plan opens different patches per season), the same
  time-deferred discipline slices 7–8 kept.
- **The record moves on a discrete START, not automatically.** Underroot has no
  game-over card (unlike Gloamline's dawn/death), so the commit is a player
  action rather than a state-machine transition. This is the frame-exact,
  wear-disciplined trigger the ROM proof can pin (one write per START edge that
  improves); an auto-commit on the climbing live forecast would write on every
  new high, which is neither pin-able frame-exactly from the host mirror nor
  needed. A future slice with a realized winter (idea above) can auto-commit at
  the winter boundary.
- **`furthest_season` is exercised at spring in the ROM, all seasons in the
  mirror.** The proof-5 replay lives in spring (season 0), so the ROM pins
  `best_season` at 0; the full 0..3 (and the improves-on-further-season path)
  are proven exhaustively host-side — the slice-8 pattern (winter forecast
  proven for all seasons host-side, driven at spring in the ROM).
- **`substrate-gate` red is inherited, not from this slice.** `main` carries 5
  known orphan-doc failures (#151); the check is expected-red on this branch
  too. What must be green — the "NDS Underroot build" job (host proof + ROM
  build + boot/dig/year/winter/save lockstep) — is this slice's responsibility.
- **Stacked on an unlanded base.** Slice 8 (PR #162) is draft-parked at the
  standing wall, so this branch stacks on `claude/underroot-slice-8`, not
  `main`. It lands only after the stack ahead does; order
  #153 → #155 → #156 → #157 → #158 → #159 → #160 → #161 → #162 → this.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-8.md` (**PR #162**,
  head `ec27203`, winter survival + the survival score). It shipped the pure
  `ur_winter_store` / `ur_winter_drain` / `ur_winter_survives` /
  `ur_winter_leftover` / `ur_winter_score` layer + `prove_winter` (1377 cases)
  and the marquee lockstep (a survivor plan clears winter score 2008, a
  too-small plan starves score 0). **Slice 8's close-out idea was the exact
  hinge this slice needed**: it foresaw that the survival SCORE is now "a single
  monotone integer over the whole dig plan… the arc's first natural fitness
  function", and asked that a later slice pin `best_score(seed)` as a balance
  tripwire. Slice 9 builds the *persistence* half of that: the record is now a
  durable `(best_score, furthest_season)` on the card, and this slice's own idea
  escalates slice 8's exactly — from *pinning* the best score in CI to
  *gating the persisted best on a realized winter* so the number the save brags
  about is honest.
- **Concrete strength observed reviewing the slice-8 code:** slice 8 was
  scrupulously **additive** — the six winter words were appended (37..42), no
  slice-1..7 telemetry index moved, and the winter functions took
  `store`/`pop`/`season` (never grid coordinates), so nothing upstream shifted.
  That is exactly what let slice 9 land cleanly: I appended six MORE words
  (43..48) and pushed only `UR_T_SPARE`, so every pre-slice-9 `--assert-watch`
  index (0..42) is bit-identical and no earlier proof needed re-pinning — the
  same additive-layer discipline, one slice further.
- **Concrete decision slice 8 handed forward, now inherited honestly:** slice
  8's card flagged that "the winter exam is a live-season FORECAST, evaluated
  where the marquee digs (spring)", explicitly deferring a run that crosses into
  the winter season to the slice-10 seed dial. Slice 9 does **not** try to
  resolve that (it would drag in the season-dependent patch-layout prediction
  slice 8 rightly avoided); instead it persists the forecast faithfully and
  **names the resulting over-optimism class as this slice's idea** — the commit
  should gate on `season == UR_WINTER` once a realized winter exists. The
  deferral is carried forward truthfully, not silently.
- **Concrete nit carried, still open:** the slice-5 `ur_nurse` signedness
  asymmetry (unsigned `col,row` vs `ur_designate`'s `int32_t` + `< 0` guard)
  slice 6→7→8 tracked stays a one-line follow-up. Slice 9 adds **no** new grid
  coordinate entry points (its new functions take `store`/`season`/blob bytes,
  not cells), so — as in slices 7–8 — there was nothing to align. Slice 8's
  honest gaps carry forward unchanged and true here (no local NDS build;
  telemetry pinned from the host mirror not a read ROM run; one battery scenario
  per behaviour). Its `📊 Model:` three-field form is mirrored here at **family**
  level (`Claude Opus family · high · task-class: gameplay increment`).

## PR / CI (filled at close-out)

- PR: **#163** — https://github.com/menno420/gba-homebrew/pull/163 (DRAFT,
  base `claude/underroot-slice-8`, head `bd55af6`)
- CI (`nds-underroot-build` + the gates): recorded in the session report /
  status; substrate-gate red is the inherited #151 orphan set, not this slice.
