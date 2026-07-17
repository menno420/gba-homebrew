# Session — Brineward bestiary cut 1: THE GRASPER

> **Status:** `complete`

- date: 2026-07-17 (branch `claude/brineward-bestiary-cut1`; main synced
  @ `478bf16` at 2026-07-17T00:46Z, both from `date -u`)
- mission: **Brineward arc B3 — the bestiary, cut 1 «The Grasper»** per
  the owner menu (`docs/NEXT-MENU-2026-07-15.md`) and the arc plan
  `games/brineward-nds/ARC-BESTIARY.md`: build the concept's promised
  SECOND sea monster (`docs/concepts/brineward-concept.md` § Core loop —
  "arms that grapple and hold you still while cutters close in"). Additive
  on the arc-slice-9 base: every existing Brineward host proof and
  recorded route must re-pass byte-identical.
- **📊 Model:** opus-4.8 · high effort · feature build (bestiary cut 1)
- landing posture: DRAFT, held draft (standing 07-16 landing wall). NO
  ready-flip, NO merge/approve/auto-merge from this session. Branched from
  **main** (not stacked — each bestiary cut branches from main, unlike the
  Wickroad arc's stack).

## What shipped

1. **Born-red gate** (commit `f779bf8`): this card `in-progress` +
   `control/claims/claude-brineward-bestiary-cut1.md`, filed before build.
   The card holds the PR red until the close-out flip (this LAST commit).
2. **The Grasper — pure sim** (`games/brineward-nds/source/bw_sim.{h,c}`,
   commit `ac4971d`): a `BwGrasper` (arms point + latched hold point +
   state/hull/timer/wake/stirs/slain), `bw_has_grasper(seed)`, and a
   static `bw_grasper_step` on the Maw's shape — REACH (the arms rise at
   the wreck and home under the sloop for `BW_GRASPER_REACH_FRAMES=150`),
   SEIZE (if the arms close within `BW_GRASPER_GRAB_RANGE`: one
   `BW_GRASPER_GRAB_BITE=20` hull, latch the sloop), HOLD (pin it STILL —
   position frozen, `speed=0` — for `BW_GRASPER_HOLD_FRAMES=90`), release
   → reach again `BW_GRASPER_RESTIR=240` later. Woundable while up
   (`BW_GRASPER_HULL=90`), slain → richer crates + release, pier sanctuary
   (`BW_GRASPER_HARBOR=40px`). `gcc -std=c11 -Wall -Wextra -fsyntax-only`
   clean.
3. **The additive gate — the pin-carry rule.** A water holds a Grasper OR
   a Maw, bucketed off the seed; `BW_GRASPER_SALT=0x10000042` (mask &15,
   ~1/16 of waters — 246/4096) is pinned so EVERY committed anchor and
   host-checked salvage seed is a NON-grasper (Maw) water. In a Maw water
   `bw_grasper_step` returns immediately and `bw_maw_step`'s one added
   guard (`|| d->grasper_water`) is false, so nothing changes; the Grasper
   lives only in salvage, so `bw_duel_step` never steps it. Every
   slice-2..9 route/pin AND every duel-phase route is bit-identical.
4. **Host proof** (`tools/check-brine.py`): the line-for-line mirror
   extended + **5 new checks** —
   `check_grasper_pincarry` (all 72 committed/checked salvage seeds are
   Maw waters; a Maw water runs the Grasper as a pure no-op; the
   escapability rail: full sail 224 / diagonal 158 > reach 120 > battle
   sail 96), `_holds` (the seize/hold contract frame-EXACT over 6 real
   grasper waters), `_sanctuary` (a berthed sloop is never seized, the
   pier still banks mid-prowl), `_slain` (a rake slays the risen arms in 3
   hits → 3 crates of 15g, then stays down), `_containment` (8000
   adversarial frames stay in the sea, hulls bounded). **ALL GREEN.**
   Existing host output **byte-identical** (`diff` = `17a18,22` — five
   added lines, nothing changed or removed).
5. **Committed route** (`games/brineward-nds/tools/record-grasper.py` →
   `proof/grasper-duel-keys.txt` + `grasper-seize-keys.txt`): the beam
   policy wins the duel on **seed 174** (a grasper water, win hull 73),
   then the victor LINGERS (pure idle — no salvage input) and the arms
   SEIZE her at salvage step 769 (tool frame 946), hull 73→53 (exactly the
   20-hull grab-bite), HELD STILL 90 frames, release step 859 (tool frame
   1036). Deterministic (two runs byte-identical); the seize holds at
   every input shift in [-6,+6]. **All 6 pre-existing recorders re-derive
   byte-identical.**
6. **ROM wiring** (`games/brineward-nds/source/main.c`): `bw_telemetry`
   `50 → 56` (words 0-49 pinned; new 50-54 = grasper state/x/y/hull +
   grasper-water flag), a render sprite reusing the Maw's risen 32×32 gfx
   (a grasper water shows this INSTEAD of the Maw), and top-screen +
   chart-table HUD lines ("THE GRASPER … KEEP CLEAR" / "HELD FAST! RAKE
   THE ARMS"). Wiring mirrors the Maw 1:1.
7. **CI** (`.github/workflows/rom-builds.yml`): the grasper recorder sync
   diff, ROM **proof 23** (drives the committed route, pins the seize/hold
   against `bw_telemetry` at the recorder's step↔frame map — the same
   calibrated convention as proof 2), and the host-proof comment updated.

## Honest limits

- **The NDS ROM is NOT built in this worker env** (no BlocksDS/Wonderful
  toolchain). `bw_sim.c` is host-gcc-syntax-clean, but `main.c` (libnds)
  and the ROM proofs are compiled/pinned only by CI (`nds-brineward-build`
  job). The `main.c` changes mirror the Maw's proven code 1:1 to minimize
  the unverified surface; if CI's build or proof 23 reveals a slip, it is
  fixable with a normal follow-up commit (the PR is draft).
- **The Grasper is SILENT** in cut 1 (audio is the game's deferred
  roadmap item 6). The deep's drone still keys off the Maw, so a grasper
  water is quiet while the arms are up — a known, deliberate gap, listed
  for the audio pass.
- ROM proof 23's frame pins are mirror-PREDICTED (the lane's pin rule —
  the byte-deterministic build makes the mirror model and the emulator
  agree); not emulator-confirmed in this env.

## 💡 Session idea

**An additive monster is a seed-bucketed EITHER/OR, not a bolt-on.** The
instinct for "add a second enemy without breaking the first" is to gate
the new one behind a fresh trigger and let both coexist. But coexistence
is the expensive path twice over: it perturbs every salvage-water route
(the old proofs now have to account for the newcomer's dynamics), and it
tangles each new proof scenario into a two-monster melee that is miserable
to pin frame-exactly. Making the Grasper and the Maw **mutually exclusive
per water** — a pinned seed hash chooses one or the other, and the loser's
step function early-returns — bought two things at once: (a) **bit-identity
for free**, because every committed/checked seed lands in the Maw bucket,
so `bw_grasper_step` is provably a no-op on all of them (the `diff` is
literally `17a18,22`); and (b) **single-threaded proofs**, because a
grasper water has NO Maw, so `check_grasper_holds` reasons about one
terror's timeline with nothing else moving. The general move: when you add
a variant to a slot that already has an occupant, prefer *replacement
chosen by a pinned deterministic key* over *addition* — the key's pinning
gives you the carry-verbatim guarantee, and the exclusivity gives you
clean proofs. It transfers straight to cuts 2–4 and to any "second X in a
proven system" (a second hazard, a second boss, a second daily mode).

**Guard recipe — the two-checkout split (workflow footgun).** This session
the harness working dir was `/home/user/gba-homebrew` but a bare `cd ~` in
Bash resolved to `/root/gba-homebrew` (root's HOME) — a SECOND full
checkout. Edits (absolute `/home/user/...` paths) and shell verification
(`cd ~/...`) hit different trees, so the first `check-brine` "byte-identical
diff" was green because it ran the UNEDITED file against itself. Anchor:
pin every Bash to the absolute repo root (`cd /home/user/gba-homebrew`,
never `~`), and before trusting any "byte-identical"/"no diff" result run
`realpath <file>` + `git rev-parse --show-toplevel` to confirm the shell
and the edit tool share one inode. Symptom that catches it: an edit you
KNOW you made not appearing in `grep`/`python -c "open(...).read()"` from
the shell.

## Previous-session review

- Prior lane slice: **PR #175** — *Wickroad crossroads cut 4, "the best
  ledger"* (SRAM-persisted best runs, the Wickroad arc closer; head
  `bb28fe1`, DRAFT, base = the cut-3 branch). Read at HEAD via the GitHub
  API (not merged to main; main is @`478bf16`/#152).
- **What transferred, confirmed:** #175's core discipline — *keep the
  default path byte-identical behind a gate, then append the proof* — is
  exactly cut 1's spine. #175 drew its end-card best line ONLY when a save
  restored (never on a fresh cart), so P1–P11 (run without `--savefile`)
  carried verbatim and P12 was appended; cut 1 gates ALL grasper behavior
  behind the grasper-water bucket (never a committed seed), so
  `check-brine`'s existing output carried verbatim (`17a18,22`) and 5
  checks were appended, and all 6 recorders re-derived byte-identical.
  Same shape, different substrate — the gate is the reusable idea, proven
  twice now.
- **A real divergence worth naming:** #175 witnessed its new state in a
  SEPARATE sidecar mailbox (`wr_ledger[6]`) rather than widening
  `wr_telemetry`; cut 1 WIDENED `bw_telemetry` (50→56, words 0-49 pinned).
  Both preserve the load-bearing invariant — the *frozen prefix*, not the
  word count — so both are correct; the choice is house style (Wickroad
  cut 2+ sidecars, Brineward has always appended to its one mailbox, and
  cut 1 followed Brineward's). The transferable rule sharpened: "don't
  widen the mailbox" is really "don't disturb the pinned prefix" — a sidecar
  is one way, a pinned-suffix append is another, and matching the game's
  existing convention beats importing another game's.
- **Landing-posture divergence:** #175 is a STACKED draft (land order
  #172→#173→#174→#175, base = a sibling branch). Cut 1 branches from
  **main** — the bestiary arc will land each cut off main (no stack-order
  dependency), which is simpler to draft-park under the wall but means cut
  2 must re-branch from main (or from cut 1 once it lands), a choice the
  next session should make deliberately, not by drift.
