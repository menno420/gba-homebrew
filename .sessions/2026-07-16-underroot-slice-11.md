# Session — Underroot arc slice 11: audio + polish → v1.0

> **Status:** in-progress

- date: 2026-07-16 (branch `claude/underroot-slice-11`, stacked on
  `claude/underroot-slice-10` @ 9bed7a3; started ~19:36Z, `date -u`)
- mission: **Underroot arc slice 11 — Audio + polish → v1.0** (docs/arcs/UNDERROOT.md
  slice-11 row): synthesized PSG cues (forager return, hawk cry, winter toll), a
  per-season ambience drone, and a v1.0-polished dual-screen HUD, all proven by a
  pure/PSG-legal audio-decision layer mirrored line-for-line host-side (the
  Gloamline `gl_amb`/`gl_cue` pattern), ROM cue telemetry, and final dual-screen
  shots. The **arc closer**: slices 1–11 all built. Determinism-first throughout —
  the cue/ambience tables and their accessors are pure integer lookups, mirrored
  byte-identical in `tools/check-underroot.py`; the telemetry is strictly additive
  (indices 54–58), so no earlier index moves.
- **📊 Model:** Claude Opus 4.8 family · high · task-class: gameplay-audio + arc-closer
- landing posture: **DRAFT, stacked on #164**, base `claude/underroot-slice-10`.
  Under the standing 2026-07-16 LANDING WALL (PR ready-flips + auto-merge
  classifier-denied); honest draft-park is the terminal state. Card stays
  `in-progress` — no ready-flip, no merge / auto-merge calls from this session;
  PRs #153–#164 untouched. Land order: #153 → #155…#164 → this.

## What shipped

1. **Born-red gate** (this card `in-progress` + claim
   `control/claims/claude-underroot-slice-11.md`, first commit `515ff45`), PR
   opened DRAFT immediately, base `claude/underroot-slice-10`.
2. **The pure PSG cue + ambience decision layer** in
   `games/underroot-nds/source/ur_sim.{h,c}` (commit `bf91f5d`) — the Gloamline
   `gl_amb`/`gl_cue` precedent verbatim:
   - `ur_cue_freq/len/duty/vol(cue)` over a 4-row table
     `{freq, len, duty(or UR_CUE_ON_NOISE), vol}`: `NONE`(0,0,0,0) the no-op,
     `FORAGE`(988,10,2,42) forager return, `HAWK`(1500,18,noise,78) the shriek,
     `WINTER`(262,64,0,66) the toll. **The cue ids ARE the priority order** — on
     a frame with several events the HIGHEST id wins the single SFX channel
     (`WINTER` > `HAWK` > `FORAGE`). Out-of-range id → row-0 no-op.
   - `ur_amb_freq/duty/vol(tier)` over a 4-row per-season drone table
     `{freq, duty, vol}`: spring(180,1,22) < summer(200,1,22) < autumn(230,1,24)
     < winter(270,0,26) — **freq STRICTLY climbs** spring→winter (the burrow
     hums higher as the year closes). Out-of-range tier → spring.
   - Pure integer lookups, no globals / no hardware — so every pre-slice-11 pin
     holds bit-for-bit and the host mirror recomputes them identically.
3. **`main.c` integration + v1.0 HUD polish** (commit `b8cfa7d`):
   - `soundEnable()` at boot; the season ambience drone started once at boot and
     retuned (`soundKill` + `soundPlayPSG`) only on a season flip — the FIFO is
     touched only when the tier changes, between flips the channel free-runs.
   - A **per-frame highest-id-wins cue decision** in the loop's per-frame block:
     `store > prev_store` → `FORAGE`, `hawk_on && !prev_hawk_on` → `HAWK`,
     `season != prev_season && season == UR_WINTER` → `WINTER`; the winning cue
     preempts the channel via `soundPlayNoise`/`soundPlayPSG` off the pure table.
     Rising-edge trackers `prev_store/prev_hawk_on/prev_season` are updated at the
     END of the block; `prev_hawk_on` is **seeded with the boot hawk state**
     (`ur_hawk_active(0)`) so a shadow already mid-crossing at power-on does not
     phantom-cry a fresh crossing at frame 1 (only a NEW crossing tolls).
   - HUD: `UNDERROOT v1.0` / `THE BURROW v1.0` title tags on both screens, a
     row-16 audio indicator that flashes `~ FORAGE`/`~ HAWK`/`~ WINTER` while the
     cue channel is open (ASCII `~`, the console font has no note glyph) + a
     steady season-ambience label.
4. **Telemetry** appended **strictly additively**: `UR_T_ACUE` (54, last cue id)
   / `ACUES` (55, cumulative cues) / `ACUEFR` (56, last-cue frame) / `ASFXL` (57,
   sfx frames left) / `AMB` (58, ambience tier == season), widening the mailbox to
   `UR_T_WORDS = 59`. `UR_T_SPARE` stays **53** — every pre-slice-11 index (0..53)
   is bit-identical, no earlier `--assert-watch` moves.
5. **Host mirror** `tools/check-underroot.py` (commit `9855d3b`): line-for-line
   `UR_CUE_ROWS`/`UR_AMB_ROWS` + `ur_cue_*`/`ur_amb_*` (**byte-identical** to the C
   — cross-checked by a `gcc` harness linking `ur_sim.c`, `diff` clean over cue
   ids 0..9 and tiers 0..9) and `prove_audio` (**11 cases**, the `check-gloam.py`
   section-13 pattern): (a) cue row 0 no-op, cues 1..3 deterministic + PSG-legal
   (`50<=freq<=4000`, `1<=len<=90`, `0<=duty<=7 or ==noise`, `1<=vol<=127`),
   priority ranking `[FORAGE,HAWK,WINTER]==1..3`, out-of-range id → row-0; (b)
   ambience each tier deterministic, freq strictly climbs, duty/vol legal,
   out-of-range tier → spring. `python3 tools/check-underroot.py` exits **0**; no
   existing golden/`ci_fixture` number moved (audio adds no sim quantity).
6. **CI proof 7** in `.github/workflows/rom-builds.yml`, job `nds-underroot-build`
   (commit `d2de5fd`): additive — proofs 1–6 and their `--watch t:ur_telemetry:54`
   untouched. Proof 7 widens the watch to `t:ur_telemetry:59` and asserts the
   audio telemetry across an undug year run — boot spring (`t[54]=0, t[55]=0,
   t[58]=0`), summer ambience retune (`t[58]=1`), and the **winter edge** (day 14
   = `UR_DAY_FRAMES*14` = ROM frame 3584): `t[58]=3` winter ambience, `t[54]=3`
   last cue == the WINTER toll (it wins over the SIMULTANEOUS hawk edge at that
   frame — id 3 > 2), `t[55] >= 1`. `--shot`s a final polished dual-screen PNG
   `ur-v1-final.png` (+ spring/summer audio shots), all added to the artifact
   upload list.
7. **Docs — arc growth-complete** (commit `c438bba`): `docs/arcs/UNDERROOT.md`
   status → slices 1–11 all BUILT, arc growth-complete pending owner playtest +
   landing clicks; slice-11 table row marked BUILT. `docs/current-state.md` header
   count eleven→twelve, NDS tally 2→3, new table row 12 (Underroot v1.0 arc, PRs
   #155–#164 + this closer) + a Recently-shipped arc-summary paragraph mirroring
   the Gloamline entry — honest that it is draft-parked under the wall.
8. Rails held: PUBLIC repo, original IP / synthesized audio only, zero
   pokemon-mod-lab content; no `.substrate/` writes; claim/card cite the branch/
   base SHA (9bed7a3); timestamps `date -u`.

## 💡 Session idea

**A post-v1.0 companion for the burrow: a colony you can leave running — an
"overwinter journal" that plays back the year as audio.** Slice 11 makes the
year *audible* per-frame (forager chirps, hawk shrieks, the winter toll, a drone
that climbs with the season), but the sound is ephemeral — it happens and is
gone. The natural next move keeps the determinism spine: because the whole run is
a pure `f(seed, dig_plan)`, a fixed year can be *replayed as a score* — a pure
`ur_journal(seed, dig_plan) → sequence of (frame, cue_id)` that enumerates every
cue the year WOULD fire (the store-growth frames, the hawk-pass edges, the winter
crossing) without running the ROM. That turns the audio layer from a live
side-effect into a **provable artifact**: the host mirror could assert the exact
cue timeline for a committed dig plan (the way slice 8's marquee asserts the exact
winter verdict), and the ROM could *play the journal back* on a title-screen
"listen to your best year" — the audio analogue of slice 9's replayable-best-seed
promise and slice 10's par idea. It is the same recurring Underroot move: take a
thing the player experiences once and make it a pure, legible, replayable
function of the seed and the tunnels they drew.

## Known / honest gaps

- **The audio is proven pure/PSG-legal + telemetry-verified, but the actual
  SOUND is owner-playtest-only.** `prove_audio` (11 cases) proves the decision
  layer is deterministic and inside the hardware ranges, and proof 7 proves the
  ROM fires the right cue ids / ambience tiers at the right frames — but *whether
  a 988 Hz duty-2 square wave is a pleasing forager chirp* is a taste call only
  the owner's ears can make on hardware/emulator. The numbers are decide-and-flag
  owner-tunables (one table row per sound); retuning them is a one-line change
  that re-runs green.
- **The HUD polish is visual / shot-only.** The title tags, the `~ CUE` flash and
  the ambience label are text-console changes proven only by the dual-screen
  screenshots (`ur-v1-final.png` et al.), not by telemetry asserts (the existing
  proofs assert the mailbox, not screen text — deliberately, so text polish is
  safe). A rendering regression in the HUD would not turn a proof red; it is
  caught by eye on the committed shots.
- **No local NDS build.** BlocksDS/`DEVKITPRO` absent in-container (as slices
  1–10), so `games/underroot-nds` is **not compiled for the ARM target locally** —
  the ROM build + the 7 headless proofs are green only when CI `nds-underroot-build`
  runs. What *did* run locally: the host mirror (stdlib-only, all proofs green
  incl. the 11-case `prove_audio`), a host-`gcc` compile of `ur_sim.c`
  (`-std=c11 -Wall -Wextra -Werror`, clean), and a `gcc` harness linking
  `ur_sim.c` that cross-checked `ur_cue_*`/`ur_amb_*` **byte-identical** to the
  Python mirror. `main.c`'s new `soundEnable`/`soundPlayPSG`/`soundPlayNoise`/
  `soundKill` calls were audited **by eye** against Gloamline's working slice-8
  usage (identical signatures, same libnds `DutyCycle`/`u16`/`u8` surface) since
  host gcc can't see across the libnds TU (the slice-5/6 lesson).
- **The winter-toll ROM timing reuses proof 3's already-green winter pin.** Proof
  7 asserts the WINTER cue at emulator frame 3800 (the same frame proof 3 already
  pins `t[3]=3`), so the winter-edge assertion is a small increment over the
  proven year drive. If CI surfaces a boot-latency straddle at the winter edge, it
  is a normal follow-up-commit fix (no force-push). The hawk-edge / winter-edge
  coincidence at ROM frame 3584 (both `3584 % 512 == 0` and day 14) is handled by
  the highest-id-wins rule and asserted, not left implicit.
- **`substrate-gate` red is inherited + expected under the born-red HOLD.** This
  card stays `in-progress` (draft-park), so the substrate gate is expected-red by
  design; `main` also carries the known orphan-doc failures (#151). What must be
  GREEN is the `NDS Underroot build` job — this slice's responsibility.
- **Stacked on an unlanded base.** The whole Underroot stack (#155…#164) is
  draft-parked at the standing 2026-07-16 wall, so this branch stacks on
  `claude/underroot-slice-10`, not `main`. It lands only after the stack ahead
  does; order #153 → #155 → … → #164 → this.

## Previous-session review

- Prior lane card: `.sessions/2026-07-16-underroot-slice-10.md` (**PR #164**, head
  `9bed7a3`, Seed dial + balance). It shipped `ur_dial_seed` + the reference
  survivable plan (`ur_ref_plan`/`ur_ref_score`/`ur_seed_fair`) + `prove_balance`
  (4153 cases) proving no dial seed is a death-trap, and honored slice 9's forward
  note by **gating the START record-commit on a realized winter**
  (`season == UR_WINTER`).
- **Slice 10's close-out idea was the hinge for THIS slice's forward idea.** It
  proposed a *par* companion to the survivability witness — a pure `ur_par_score`
  characterizing what a good run on a seed looks like — turning `REFSCORE` from
  "is this year winnable?" into "what does a good run look like?", explicitly so
  *slice 11's audio/HUD polish could surface it* ("this year's best is N"). Slice
  11 lands the audio/HUD it pointed at, and its own close-out idea extends the same
  determinism-as-feature spine one rung further (a pure, replayable cue *journal*).
- **Concrete strength observed reviewing the slice-10 code:** slice 10 was
  scrupulously **additive** — it appended words 49..52 and pushed only
  `UR_T_SPARE` → 53, so no slice-1..9 index moved. That discipline is exactly what
  let slice 11 land cleanly: I appended FIVE more words (54..58) and left
  `UR_T_SPARE` at 53, so every pre-slice-11 `--assert-watch` index (0..53) is
  bit-identical and no earlier proof (proofs 1–6) needed re-pinning — the additive
  telemetry contract slices 7→8→9→10 kept, one final slice further.
- **Its `📊 Model:` three-field form** is mirrored here at **family** level
  (`Claude Opus 4.8 family · high · task-class: gameplay-audio + arc-closer`).
  Slice 10's honest gaps (no local NDS build; ROM telemetry pinned from the host
  mirror; HUD/text not telemetry-asserted) carry forward unchanged and true here.

## PR / CI (filled at close-out)

- PR: **[[fill:PR#]]** — [[fill:PR-URL]] (DRAFT, base `claude/underroot-slice-10`;
  impl commits `bf91f5d`/`b8cfa7d`/`9855d3b`/`d2de5fd`/`c438bba`, card commit is
  the branch tip). Draft-parked under the landing wall — no ready-flip, no
  auto-merge.
- CI (`nds-underroot-build` + the gates): recorded in the session report / status;
  substrate-gate red is expected under the born-red HOLD (draft-park), not a slice
  fault. What must be green — the "NDS Underroot build" job — is this slice's
  responsibility.
