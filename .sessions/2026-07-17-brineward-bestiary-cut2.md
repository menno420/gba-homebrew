# Session — Brineward bestiary cut 2: RAM/BRACE (the break-free B verb)

> **Status:** `complete`

- date: 2026-07-17 (branch `claude/brineward-bestiary-cut2`, STACKED on
  `claude/brineward-bestiary-cut1` @ `a3f3a0a`; main synced @ `478bf16`,
  all from `date -u`)
- mission: **Brineward arc B3 — the bestiary, cut 2 «Ram/brace»** per the
  arc plan `games/brineward-nds/ARC-BESTIARY.md`: build the concept's
  RESERVED B button (`docs/concepts/brineward-concept.md` § Controls
  sketch — "(reserved: later ram/brace verb)") as the **break-free** verb.
  A seized sloop can BRACE to wrench loose from the Grasper's hold early,
  at a cost. Additive on cut 1: every existing Brineward host proof and
  recorded route (slice-2..9 AND cut 1's grasper routes) must re-derive
  byte-identical, by the input-verb gate — no committed story presses B.
- **📊 Model:** Opus family · high effort · feature build (bestiary cut 2
  «Ram/brace»)
- landing posture: DRAFT, held draft (standing 07-16 landing wall). NO
  ready-flip, NO merge/approve/auto-merge from this session. **Stacked**:
  base = `claude/brineward-bestiary-cut1` (PR #176), NOT main.

## What shipped

1. **Born-red gate** (commit `9bf0386`): this card `in-progress` +
   `control/claims/claude-brineward-bestiary-cut2.md`, filed before build.
   The card holds the PR red until this close-out flip (the LAST commit).
2. **The break-free wrench — pure sim** (`bw_sim.{h,c}`, commit
   `c25a2de`): a fresh `BwInputs.brace` (edge-triggered B) and, in the
   `BW_GRASPER_HOLD` branch, the wrench: the FIRST brace that would
   actually shorten the hold pays `BW_GRASPER_BRACE_HULL=10` hull ONCE and
   fast-forwards the hold clock to its last stretch
   (`timer = BW_GRASPER_HOLD_FRAMES - BW_GRASPER_BRACE_FRAMES`, i.e. 78),
   so the arms slip `BW_GRASPER_BRACE_FRAMES=12` frames later. A brace with
   nothing left to shorten (already inside the last 12 frames) and every
   B-silent frame do nothing; self-limiting even if B is held (once
   fast-forwarded the guard is false, so the cost lands exactly once).
   `bw_grasper_step` now takes the inputs. `gcc -std=c11 -Wall -Wextra
   -fsyntax-only` clean.
3. **The additive gate — the input-verb rule** (the SELECT precedent,
   `.sessions/2026-07-13-brineward-bands.md`). `brace`'s zero value IS the
   legacy behaviour exactly, and NO slice-2..9 or cut-1 route presses B,
   so every recorded route and pin re-derives **bit-identical**. In a Maw
   water `bw_grasper_step` is still a pure no-op, so B never reaches the
   branch there — proven by `check_grasper_breakfree`'s idle==brace
   trajectory over a whole Maw-water salvage.
4. **Host proof** (`tools/check-brine.py`): the line-for-line mirror
   extended + **1 new check** — `check_grasper_breakfree`: (a) a B-silent
   hold runs the FULL 90 frames (the carry identity), (b) an early brace
   shortens the hold to its cost frames — the arms slip exactly 12 after
   the wrench, at exactly 10 hull, ONCE (a 2nd brace is inert, the sloop
   pinned STILL until it slips), (c) a late brace buys nothing (full-length
   release stands, no cost), (d) a Maw water ignores B entirely. **ALL
   GREEN.** All **6 pre-existing recorders re-derive byte-identical** (the
   input-verb gate, re-run locally: duel-win / salvage / maw / wind / bands
   / grasper all `diff`-clean).
5. **Committed route** (`games/brineward-nds/tools/record-breakfree.py` →
   `proof/grasper-breakfree-keys.txt`): the seize route's TWIN on **seed
   174** — the same duel win and the same lingering seize (its duel prefix
   is byte-identical to `grasper-duel-keys.txt`), then ONE B press at tool
   frame 956 wrenches loose: hull 73 → 53 (the seize's 20) → 43 (the
   wrench's 10), broke free at step 791 (tool frame 968), a **22-frame
   hold** against the full 90 (proof 23's idle route was still held at
   frame 1020). Deterministic (two runs byte-identical); the anchor and
   its duel come from `record-grasper.record_story`, so the route never
   leans on a private seed choice.
6. **ROM wiring** (`games/brineward-nds/source/main.c`): `KEY_B →
   in.brace` in the SALVAGE input (edge-triggered `keysDown`, so exactly
   one brace reaches the sim per press), and the held-fast HUD hint now
   reads `HELD FAST! B WRENCH / RAKE`. No new telemetry word — the break
   is observable through the existing hull (`BW_T_PHULL=7`) and grasper
   state (`BW_T_GRASPSTATE=50`) words. Wiring mirrors the salvage input
   1:1.
7. **CI** (`.github/workflows/rom-builds.yml`): the break-free recorder
   sync diff, and ROM **proof 24** (drives the committed route, pins the
   10-hull wrench cost and the EARLY release against `bw_telemetry` at the
   recorder's step↔frame map — the same calibrated convention as proof
   23). Existing recorders' sync diffs unchanged.

## Honest limits

- **The NDS ROM is NOT built in this worker env** (no BlocksDS/Wonderful
  toolchain). `bw_sim.c` is host-gcc-syntax-clean and every rule is
  host-proven in `check-brine.py`, but `main.c` (libnds) and ROM proof 24
  are compiled/pinned only by CI (`nds-brineward-build`). The `main.c`
  change is one input field + one HUD string, mirroring the proven salvage
  input; proof 24's frame pins are mirror-PREDICTED (the lane's rule — the
  byte-deterministic build makes the mirror and the emulator agree), not
  emulator-confirmed here. Any slip is fixable with a normal follow-up
  commit (the PR is draft).
- **The bow-on shove is DEFERRED.** The arc plan's cut-2 line says the B
  verb "doubles as a low bow-on shove against a ship at close quarters,"
  but the plan's own *proof strategy* scopes cut 2 to the break-free (a
  braced break shortens the hold; no B-silent route changes; a recorded
  break-free route). I built exactly that — the headline, fully proven —
  and left the shove for a follow-up, because a shove lives in
  `bw_duel_step` (the hottest, most-pinned path) and warrants its own
  proof rather than a rushed rider on this cut. Guard recipe below.
- **The Grasper stays SILENT** (audio is the game's deferred roadmap item
  6). The wrench is a HUD line and a hull tick, no cue — a known gap for
  the audio pass, same as cut 1's silent Grasper.

## 💡 Session idea

**A break-out verb is a clock fast-forward, not a new state.** The
instinct for "let the player escape a timed hold early, at a cost" is to
add an `escaping` sub-state (or a `braced` flag on the monster) and a
second exit path. But the hold already HAS an exit — `timer >=
HOLD_FRAMES` — so the whole verb collapses into mutating that one clock to
its last stretch: `timer = HOLD_FRAMES - BRACE_FRAMES`. The existing
release condition then fires `BRACE_FRAMES` later for free, the "reaches
again" wake and the pinned-still freeze all reuse the untouched HOLD
machinery, and — the quiet win — the **monotone guard makes the mutation
idempotent**: `if brace and timer < HOLD_FRAMES - BRACE_FRAMES` lands the
cost EXACTLY once even if B is held down, with no edge-tracking field to
store or serialize. So cut 2 added **one input field and zero monster
state**: no new `BwGrasper` member, no new telemetry word, no new release
path. The general move: to add a *skip / cancel / rush* verb over a
countdown, prefer clamping the existing clock to a boundary value over
forking a parallel sub-state — the state's own exit condition does the
rest, and a monotone guard buys idempotence without bookkeeping. Transfers
to any early-release over a timer (a cancel-cast, a skip-cutscene, an
early-eject), and straight to cut 3's cutters (the pin's lethality is a
clock the brace already races).

**Guard recipe — assert "pinned still" on `timer >= 1`, never at the
seize frame.** `check_grasper_breakfree` first tripped asserting
`d.player.speed == 0` on the frame the sloop is seized. Anchor:
`bw_grasper_step` runs AFTER `bw_ship_step`, and the seize step is the
`REACH → HOLD` transition (it latches `gx,gy` and docks hull but does NOT
run the HOLD freeze that same step), so at the seize frame `speed` still
carries the last way — the freeze zeros it only from the first HOLD-branch
step on. Fix mirrors `check_grasper_holds`: assert position (`x==gx,
y==gy`) before stepping, and gate any `speed==0` assertion on
`grasper.timer >= 1` (the recorder's `_scan_seize` rule). Test target:
`check_grasper_breakfree` / `check_grasper_holds` in `tools/check-brine.py`.

## Previous-session review

- Prior lane slice: **PR #176** — *Brineward bestiary cut 1, «The
  Grasper»* (the second sea monster; head `a3f3a0a`, DRAFT, base = main).
  Read at HEAD via the git tree and the GitHub API; **its CI backfill was
  verified GREEN before I stacked**: all three build jobs on `a3f3a0a`
  passed — ROM builds (job `87782951750`), NDS Brineward build (job
  `87782951752`, incl. "proof 23 — the Grasper seizes and holds"), NDS
  ROM/Gloamline (job `87782951775`); `substrate-gate` red is by design
  (main's #151 doc orphans + the born-red HOLD), not a build failure. Cut
  2 does not stack on sand.
- **What transferred, confirmed:** cut 1's spine — *keep the default path
  byte-identical behind a gate, then append the proof* — is cut 2's spine
  too, but through a DIFFERENT gate. Cut 1 gated on a **seed bucket** (a
  water is Grasper OR Maw, every committed seed a Maw water, so
  `bw_grasper_step` is a no-op on all of them). Cut 2 gates on a **fresh
  input verb** (`brace`'s zero value is legacy, no committed route presses
  B), which is the more surgical of the two: it needed no seed pinning at
  all, just a field that defaults to the old behaviour. Both delivered the
  same guarantee — all 6 recorders `diff`-clean, `check-brine` existing
  output untouched — proving the gate is the reusable idea across two
  substrates (a hash bucket, a zero-valued input).
- **A real divergence worth naming (state surface):** cut 1 WIDENED
  `bw_telemetry` (50→56) for the new monster; cut 2 added **zero** new
  state — no `BwGrasper` field, no telemetry word — because the break is
  fully observable through the existing hull + grasper-state words. That
  sharpens cut 1's "match the game's convention" into a stronger rule:
  *add nothing you can already observe.* A verb that only changes WHEN an
  existing state ends needs no new witness — the existing witnesses see
  the earlier end.
- **Landing-posture divergence (deliberate):** cut 1's own card predicted
  "cut 2 must re-branch from main (or from cut 1 once it lands), a choice
  the next session should make deliberately, not by drift." The
  coordinator chose **stacked on cut 1** (base = `claude/brineward-
  bestiary-cut1`, PR #176) — the break-free verb only means anything atop
  the Grasper it breaks, so the stack order is the honest dependency. This
  is the deliberate call cut 1 asked for, not drift; land order is
  #176 → this PR.
- **Guard recipe honored:** cut 1's two-checkout footgun (a bare `cd ~`
  resolving to `/root/gba-homebrew`, a second tree). Every Bash this
  session was pinned to the absolute repo root `cd /home/user/gba-homebrew`
  (never `~`), and the recorder byte-identity was re-derived from that one
  tree — no phantom "green against an unedited file."
