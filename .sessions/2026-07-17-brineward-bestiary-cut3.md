# Session — Brineward bestiary cut 3: THE AMBUSH (converging cutters)

> **Status:** complete

- date: 2026-07-17 (branch `claude/brineward-bestiary-cut3`, STACKED on
  `claude/brineward-bestiary-cut2` @ `b1d9d12`; all timestamps from
  `date -u`)
- mission: **Brineward arc B3 — the bestiary, cut 3 «The ambush»** per the
  arc plan `games/brineward-nds/ARC-BESTIARY.md`: complete the concept's
  full sentence (`docs/concepts/brineward-concept.md` § Core loop — "arms
  that grapple and hold you still **while cutters close in**"). While the
  Grasper HOLDS the sloop, light enemy **cutters** converge on the pinned
  ship, making the hold *lethal* — the tempo loss cut 1 only hinted at.
  Cut 2's braced break-out is the counter: brace early and the hold ends
  before the cutters reach → they disperse → «ambush survived». Additive
  on cuts 1+2: every existing Brineward host proof and recorded route
  (slice-2..9 AND the grasper routes) must re-derive byte-identical,
  behind a NEW seed sub-bucket (the ambush-water gate).
- **📊 Model:** Opus family · high effort · feature build (cut 3 «The ambush» — converging cutters compose the grasper hold)
- landing posture: DRAFT, held draft (standing 07-16 landing wall). NO
  ready-flip, NO merge/approve/auto-merge from this session. **Stacked**:
  base = `claude/brineward-bestiary-cut2` (PR #177), NOT main.

## What shipped

1. **Born-red gate** (commit `8b698ac`): this card `in-progress` +
   `control/claims/claude-brineward-bestiary-cut3.md`, filed before build.
   The card holds the PR red until this close-out flip (the LAST commit).
2. **The ambush — pure sim** (`bw_sim.{h,c}`, commit `1aa24a7`): a
   `BwCutter` struct (`x,y,bit`) and a `BwGrasper.cutters[BW_CUTTER_COUNT=3]`
   array. In the `BW_GRASPER_HOLD` branch, *only in an ambush water*
   (`d->ambush_water`): at the seize, 3 cutters spawn at fixed integer
   offsets around the latched pin (`BW_CUTTER_DX/DY`, NOT sea-clamped so
   closing time is pin-position-independent); each HOLD frame each live
   cutter homes one clamped step per axis toward the pin at
   `BW_CUTTER_SPEED=256` (the REACH homing idiom) and, inside
   `BW_CUTTER_HIT_RANGE=6px`, BITES once for `BW_CUTTER_BITE=8` hull
   (clamped ≥0, the Maw/brace once-guard) and STOPS. Cutters disperse
   (all zeroed) at release / brace-out (via `bw_grasper_sound`) and at a
   slay. Tuned so each cutter bites at ~43/47/51 frames: a full unbraced
   hold takes `20 + 3×8 = 44` hull (the pin lethal), but a braced hold
   ends in ~20 frames before any cutter reaches → 0 bites. `gcc -std=c11
   -Wall -Wextra -fsyntax-only` clean.
3. **The ambush gate — a fresh seed sub-bucket** (the mirror-diff /
   pin-carry rule). `bw_grasper_ambush(seed) = bw_has_grasper(seed) &&
   ((bw_hash(seed, BW_AMBUSH_SALT=0x10000045) & BW_AMBUSH_BUCKET=3) == 0)`,
   stored as `d->ambush_water` at init. Salt/bucket **pinned** so NONE of
   the committed/host-checked grasper seeds ambush — the seize + break-free
   anchor **seed 174** and the first-six grasper waters the host checks
   drive `[47,60,66,83,88,90]` are all non-ambush. Cutters spawn/step ONLY
   in an ambush water, so every prior grasper route keeps zero cutters and
   carries **bit-identical**, and a Maw water stays a pure no-op. 52/246
   grasper waters ambush; the first is seed 194.
4. **Host proof** (`tools/check-brine.py`, commit `1aa24a7`/`040f96b`):
   the line-for-line mirror extended + **1 new check** —
   `check_grasper_ambush`: (a) an UNBRACED hold in an ambush water draws
   all 3 cutters in (first bite at hold frame 43), each exactly 8 hull, the
   pin lethal (44 hull), dispersed at release; (b) an EARLY brace ends the
   hold (21 frames) before any cutter reaches → 0 bites, hull only the
   seize + brace cost, cutters dispersed: «ambush survived»; (c) a
   non-ambush grasper water spawns ZERO cutters through a whole hold (the
   bit-identity spot-check). **ALL GREEN.** All **7 pre-existing recorders
   re-derive byte-identical** (grasper seize/break-free + duel/salvage/maw/
   wind/bands, re-run locally: `diff`-clean — no committed proof file
   changed).
5. **Committed route** (`games/brineward-nds/tools/record-ambush.py` →
   `proof/ambush-survived-keys.txt`, commit `040f96b`): the break-free
   route's sibling on **seed 439** (an ambush water). The committed duel
   win, the lingering seize (which springs 3 cutters), then ONE early B
   wrench at tool frame 1236 ends the hold in 20 frames (release 1248) —
   before the first cutter would reach (~1271) — so **0 cutter bites**, the
   24 hull the pin would have cost is dodged whole, and the cutters
   disperse: ambush survived, hull 73 → 53 (seize) → 43 (wrench).
   Deterministic (two runs byte-identical); anchor + duel come from
   `record-grasper.record_story`, so the route never leans on a private
   seed choice.
6. **ROM wiring** (`games/brineward-nds/source/main.c`, commit `646ed40`):
   render the 3 converging cutters (OAM, reuse the enemy sloop gfx — they
   ARE light sloops) while the Grasper HOLDs an ambush water; the held-fast
   HUD and chart rows show a CUTTERS warning in an ambush hold. One new
   telemetry **witness** word `BW_T_CUTTERS=55` (a free spare slot — does
   not shift any existing index): converging cutters alive right now, 0 in
   every non-ambush water, 0→N at a seize, N→0 on disperse — so ROM proof
   25 can witness the ambush *composition* on the survived route, not just
   a break-free-identical hull trace. Wiring mirrors the proven Maw/reef
   sprite + grasper HUD idioms 1:1.
7. **CI** (`.github/workflows/rom-builds.yml`, commit `bd5d02d`): the
   ambush recorder sync diff, and ROM **proof 25** (drives the committed
   ambush route, pins `t:55` 0→3→0, the seize/brace hull, and the early
   release against `bw_telemetry` at the recorder's step↔frame map, step j
   at frame j+442 — the same calibrated convention as proofs 23/24).
   Existing recorders' sync diffs unchanged.

## Honest limits

- **The NDS ROM is NOT built in this worker env** (no BlocksDS/Wonderful
  toolchain). `bw_sim.c` is host-gcc-syntax-clean and every rule is
  host-proven in `check-brine.py`, but `main.c` (libnds) and ROM proof 25
  are compiled/pinned only by CI (`nds-brineward-build`). Proof 25's frame
  pins are **mirror-PREDICTED** (the lane's rule — the byte-deterministic
  build makes the mirror and emulator agree), not emulator-confirmed here.
  The pins for seed 439 shift ~265 frames later than proofs 23/24 (seed
  174) because the START span, not the sim, is later; the seize/brace/
  release tool frames (1228/1236/1248) come straight from the recorder's
  own print. Any slip is fixable with a normal follow-up commit (draft).
- **The cutters are AI-dumb by design.** They home straight at the latched
  pin and bite once — they do not steer, fire, or persist past the hold.
  That is the cut's whole scope (the concept's "cutters close in" is a
  *pressure on the hold*, not a second duel); a cutter that fights back is
  a different, bigger feature and belongs in its own cut, not a rider here.
- **The Grasper and its cutters stay SILENT** (audio is the game's deferred
  roadmap item 6). The ambush is a HUD line, sprites, and a hull tick, no
  cue — a known gap for the audio pass, same as cut 1's silent Grasper and
  cut 2's silent wrench.
- **The lethal (unbraced) route is host-proven only.** The committed ROM
  route is the *survived* story (the arc plan's "a recorded «ambush
  survived» route"); the lethal full-hold's extra 24 hull is proven in
  `check_grasper_ambush` over the mirror, not driven as a second ROM route.
  A recorded «ambush lethal» route would be a clean follow-up if wanted.

## 💡 Session idea

**Compose a new mechanic out of an existing state's *timeline*, not a new
state machine.** The instinct for "monsters close in while you're held" is
to build converging-AI as its own subsystem — a spawner, a schedule, a
lifecycle — and bolt it onto the game loop. But the hold ALREADY is a
timeline: a 90-frame clock with a known start (the seize) and a known end
(release). Cut 3's cutters are just *readers* of that clock — they spawn on
its start edge, home every frame it runs, and are zeroed on its end edge
(the same `bw_grasper_sound` release that cut 2's brace fast-forwards). So
the ambush needed **no new state machine, no scheduler, no second loop** —
three position structs that live and die with the hold, stepped inside the
one branch that already owns it. The composition then falls out for free:
cut 2's brace, which ends the clock early, *is automatically* the counter
to cut 3's cutters, because they read the same clock — the brace races the
cutters without either feature knowing about the other. The general move:
when a new mechanic is "X happens *while* Y," don't model X's lifecycle —
find Y's existing start/run/end edges and hang X off them; the mechanics
that already act on Y's timeline become X's counters and combos with zero
extra wiring. (This is the same "clock, not state" lesson cut 2 learned for
the brace, now applied one level up: cut 2 mutated the clock, cut 3 reads
it.)

**Guard recipe — a fixed-offset spawn ring must NOT sea-clamp, or a
corner pin collapses it.** `record-ambush.py` first tripped: every
recorded seize lands at a sea CORNER (the idle victor drifts to a
sea-clamp edge before the arms close), and clamping each cutter's spawn
`(gx+dx, gy+dy)` back into the sea pulled all three onto the pin → a
cutter bit at hold frame 1, no braced route could survive. Anchor: the
host check forces a *mid-sea* pin (`_ambush_to_seize` sets the player at
128,96) so it never saw the corner case the recorder hits. Fix: spawn the
cutters at the RAW offset (a cutter may start off the pin's ring, even
off-screen, and converge inward), so closing time is pin-position-
independent — a corner seize is exactly as dangerous as a mid-sea one, and
both the check and the recorder agree. The lesson: an invariant proven
only at a manufactured central case can hide an edge-collapse; when a
spawn is *relative to a runtime position*, test it at the sea corners the
real route reaches, not just the center the check picks. Test target:
`check_grasper_ambush` (central) vs `record-ambush.py` (drifts to corner)
in lockstep.

## Guard recipe

**Pin the new seed sub-bucket against the OLD routes' seeds, and let the
host proof be the guard.** Cut 3 gates on `bw_grasper_ambush`, a fresh
salt sub-bucket *over* cut 1's grasper bucket. The whole bit-identity rail
is: NO committed/host-checked grasper seed may fall into the ambush bucket
(else its check/route would take cutter hull and diverge). The recipe:
enumerate the exact grasper seeds every existing check + recorder touches
(`_grasper_seeds(6)` = `[47,60,66,83,88,90]` for `check_grasper_holds`,
`_grasper_seeds(1)[0]=47` for sanctuary/slain/containment/break-free, and
the recorder anchor **174**), then iterate `BW_AMBUSH_SALT`/`BUCKET` until
`all(bw_grasper_ambush(s)==0 for s in those)` AND at least one *other*
grasper water ambushes (for the new check + recorder). `0x10000045`/`3`
lands it (52 ambush waters, none of the protected set). `python3
tools/check-brine.py` is the guard: if any protected seed became an
ambush water, its pre-existing check FAILS on the extra cutter hull. Green
== the pin holds. (Ran it after every edit; never pushed red.)

## Previous-session review

- Prior lane slice: **PR #177** — *Brineward bestiary cut 2: Ram/brace —
  the break-free B verb* (head `b1d9d12`, DRAFT, base =
  `claude/brineward-bestiary-cut1`). Read at HEAD via the git tree and the
  GitHub API (`pull_request_read`); it is DRAFT, `mergeable_state:
  unstable` (substrate-gate red **by design** — main's #151 doc orphans +
  the born-red HOLD card, not a build failure). Cut 3 branched from its tip
  `b1d9d12` (confirmed) and does not stack on sand: the Brineward host
  proof + recorder byte-identity are green at that tip.
- **What transferred, confirmed:** cut 2's core idea — *the hold is a
  clock, not a state* — is cut 3's foundation. Cut 2 MUTATED the clock (the
  brace fast-forwards `timer` to its last stretch); cut 3 READS the clock
  (the cutters spawn on its seize edge, home while it runs, die on its
  release edge). Because both act on the *same* release path
  (`bw_grasper_sound`), cut 2's brace is automatically cut 3's counter with
  zero cross-wiring — the composition the arc plan promised ("the braced
  break is the counter") fell out of reusing the timeline, not from a new
  `if`. The gate spine also transferred verbatim: keep the default path
  byte-identical behind a gate, then append the proof — cut 1 gated on a
  seed bucket, cut 2 on a zero-valued input, cut 3 on a *sub-bucket of cut
  1's bucket*.
- **A real divergence worth naming (state surface):** cut 2's headline
  rule was "**add nothing you can already observe**" — it added zero
  monster state and zero telemetry, because the break was visible through
  the existing hull + grasper-state words. Cut 3 could NOT hold that line
  and was right not to: the cutters are genuinely new *positional* state
  (three homing bodies), so they needed real struct fields — AND I added
  one telemetry witness word (`BW_T_CUTTERS`) on purpose, because without
  it the survived ROM route is byte-identical to a plain break-free
  (same hull trace) and proof 25 could not *witness the ambush at all*.
  The sharpened rule: *add nothing you can already observe — but a
  composition whose whole point is invisible on the winning route needs
  exactly one witness to prove it happened.* Cut 2 observed a
  timer's early end (already witnessed); cut 3 observes bodies that
  spawn-and-vanish on a route that dodges their effect, which the old
  witnesses cannot see.
- **Landing-posture divergence (deliberate):** like cut 2 on cut 1, cut 3
  is **stacked** (base = `claude/brineward-bestiary-cut2`, PR #177), not
  re-branched from main — the ambush only means anything atop the hold it
  makes lethal AND the brace that counters it, so the two-deep stack is the
  honest dependency. Land order is #176 → #177 → this PR.
- **Guard recipe honored:** cut 2's "assert pinned-still on `timer >= 1`,
  never at the seize frame" held — `check_grasper_ambush` asserts the
  pinned position (`x==gx, y==gy`) each HOLD step and never trips on the
  seize frame's residual way, reusing the `check_grasper_holds` idiom. And
  cut 1's two-checkout footgun stayed shut: every Bash pinned to
  `/home/user/gba-homebrew`, recorder byte-identity re-derived from that
  one tree.
