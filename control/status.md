# gba-homebrew (game-lab Track B) · status

updated: 2026-07-11T21:03:45Z (session 23, bare hourly wake heartbeat —
appends the "Shepherd + grooming (session 23)" section at file end
only; every Gloamline/Brineward/Lumen-Drift section above remains
session 20/21's record, untouched)
phase: **GLOAMLINE ARC — session 21 (slice 5): BARRICADES SHIPPED +
OWNER WRAP-UP/ARCHIVE-PREP EXECUTED.** The project chat is being
archived (owner order, 2026-07-11): the durable hand-off is
**`docs/retro/archive-ready-2026-07-11.md`** (true state, owner rulings
of 2026-07-11 incl. the standing directive verbatim, resume
instructions, ⚑ queue) and the ORDER-004 self-review is
**`docs/retro/self-review-2026-07-11.md`**. The game: download
**`dist/gloamline.nds`** (110,080 B, sha256
`25ae4f81b665e60cdfa6498cb26933cfced11a5bf3f71302b389af9b17b43cdc`,
byte-deterministic build) — **`docs/PLAYING-GLOAMLINE.md`** is the
guide. Slice 5 adds BARRICADES: B places a barricade at the
lamplighter's feet (1 plank; 6-plank run stock, +2/dawn capped 9 — the
scavenge interlude NEXT slice becomes the real source) or repairs the
intact one in reach to full (never wasting a plank on a full one). The
dead cannot ENTER an intact barricade's 16 px radius — a blocked step
chews exactly 1 hp (240 hp; 0 = breach, slot free); a body inside may
always leave; a stagger frame never chews. No-trap invariants BY
CONSTRUCTION: the player is never a block-predicate argument (can't
seal themselves in) and blocked dead always chew (never walled out
forever). Barricades persist across nights; watch-map `#` marks; HUD
`PK` planks; third original sprite (vflip = splintered at half hp);
telemetry 24→32 words (slots 0-23 untouched).
evidence (all run green in-container this session): (1) host mirror
`tools/check-gloam.py` (lockstep with `gl_sim.c`, same commit):
containment now drives hash-driven B verbs vs the 24-crowd; 8192
block-predicate cases deterministic/enter-only/never-pinning; no-seal
(player trajectory bit-identical with barricades up); eventual pressure
(128 full-cap walled-in runs ALL breached to contact, worst frame 1152
vs bound 4240); plank economy. 1.5 s. (2) `tools/gloam-route.py`
full-game mirror + rollout track the new step order — slice-3/4 routes
reproduce exactly (22.6/22.1 px), zero drift; every emulator value the
mirror predicted for the new proofs matched exactly. (3) **9 pinned
headless proofs in CI** (`nds-rom-build`, 112 asserts): ALL 77
slice-3/4 asserts unchanged (night-1-bit-identical by construction —
zero re-pins, barricade state inert until B) + proof 8 barricade
lifecycle against proof 3 as its control (same seed 118: control dies
frame 250, barricaded player alive at 400 with the Shambler pinned at
16.1 px; chew math exact 228@250/122@400; breach → death by 690 — no
free lunch) + proof 9 repair economy (no waste on full; 1 plank → full
hp; block keeps holding) + proof 7 frame budget re-measured WITH the
8-barricade ring + 24 crowd on the `GL_STRESS=1` build: steady ≤68
scanlines (mean 56.3) vs the 71-line vblank budget, night-start spike
140 < 263 — DeSmuME timing model (py-desmume 0.0.9), NOT hardware;
margin tightened from slice 4's 53 (barricades cost ~4-15 lines).
Screenshots:
`games/gloamline-nds/proof/slice5-{barricade-hold,breach-death,stress-ring}.png`.
GBA gates untouched (Lumen Drift v1.3 + `dist/lumen-drift.gba`
unchanged).
arc plan (owner standing directive, verbatim in the archive-ready
retro: "please continue expanding the games, I will give my review on
them soon, the further you already are the better"): concept (PR #50) →
toolchain (PR #51) → skeleton (PR #52) → shove + waves (PR #54) →
**barricades (THIS PR)** → next slice = **between-nights SCAVENGE
INTERLUDE** (the real plank source), then lantern-oil light pressure,
synthesized audio set, best-nights saves, difficulty-curve/watch-map
polish (order per `docs/concepts/gloamline-concept.md`).
parallel arc — BRINEWARD (session 20, coordinator-assigned, this
section's writer): **WALKING SKELETON SHIPPED — Brineward is
PLAYABLE.** Download **`dist/brineward.nds`** (108,032 B, sha256
`89e68dc2dd926050fdd6202a6d3b9bd8f2d82e10047453de7b41ac7b462dc475`,
byte-deterministic) — **`docs/PLAYING-BRINEWARD.md`** is the guide.
The concept doc's v0 cut, whole: momentum sailing (heading + 3-state
sail trim, the ship always makes way), ONE rum-runner sloop on an
intercept-and-circle AI, L/R broadside batteries (3-ball rakes, per-
battery reload, range falloff) on BOTH ships, sink-or-be-sunk → card →
START instant restart, bottom screen = ship-status ledger v0. Rails:
pure sim `games/brineward-nds/source/bw_sim.c` mirrored line-for-line
by `tools/check-brine.py` (sine identities; 4096 spawns deterministic/
safe; idle player ALWAYS sunk ≤1177 f over 64 seeds; the beam-holding
policy ALWAYS wins over 16 seeds; 20k-frame containment);
`bw_telemetry[16]` mailbox; NEW CI job **`nds-brineward-build`**
(Gloamline's job + the GBA gate untouched, verified append-only)
running the host proof, a route-drift diff, the deterministic build +
sha256 cross-log, and 4 pinned headless proofs (55 numeric asserts,
green in-container AND on the PR's CI run): boot; exact sail
kinematics (positions/heading/trim matched to the unit); duel-LOSS
idle (hull 100→71→42→0, enemy unscratched, instant restart, fresh
seed); duel-WIN via a recorded route
(`games/brineward-nds/tools/record-duel-win.py`, closed-loop policy →
open-loop replay, wins at every ±6-frame alignment shift, player hull
UNTOUCHED, victory card, next duel starts). Archive-prep (owner
wrap-up order 2026-07-11) honored for THIS arc: chat-only context in
`docs/retro/archive-ready-2026-07-11-brineward.md` (inspiration/owner
ruling, "expand the games" directive, roadmap state, exact resume
recipe) — the Gloamline lane's repo-level retro is separate and its
own. Two py-desmume quirks recorded in PLATFORM-LIMITS (BGRX
screenshots; affine OAM has no hide bit). Sessions 18 and 21
(Gloamline slices 4+5) merged mid-slice; rebases conflicted only on
dist/README.md's table (both-rows resolution) and this file (own
sections re-applied) — the claims convention held, zero game-file
collisions. Brineward roadmap next (per concept doc, NOT started):
loot/gold → port+upgrades → the Maw → wind feel → danger bands →
audio → saves. Inbox re-read at HEAD before this write (main
`ba9ec91`): no new orders (001-004 executed — record in git history
and session cards).
honest gaps for the next slice: plank economy is a placeholder stock
(scavenging is the designed source — next slice); barricade numbers
(16 px radius, 240 hp, 6+2/9 planks, repair-over-place) are
decide-and-flag, one-constant owner-tunable in `gl_sim.h`; camping
inside your own barricade ring is possible but self-defeating (the
crowd chews through — eventual-pressure proven; still worth an owner
feel-check); wave plateau nights (13+) still have no headless survive
proof; perf evidence is emulator-model, not hardware (steady 68/71 = 3
modeled scanlines of margin at absolute worst case); committed
screenshots carry a py-desmume BGR channel swap (colors correct on
hardware; see self-review); no audio; no saves.health: green (`python3 bootstrap.py check --strict` exit 0 at
close-out; host proof + all 9 headless proofs green in-container; GBA
loop untouched; ORDER 001-004 all executed — 004 as
`docs/retro/self-review-2026-07-11.md` this session).
kit: v1.12.0 · engaged: yes
boot: synced to origin/main `c3331da` (PR #54). Sibling PIRATE lane
live on `origin/claude/brineward-skeleton` (their session card 20 —
this card renumbered to 21; their claim seen and respected, zero
Brineward files touched). Inbox re-read at branch time: no new orders
beyond 004 (executed this session). Owner wrap-up/archive order
received mid-slice via coordinator and executed (finish-not-park: the
slice was proofs-green when it arrived).
last-shipped: this session's PR (slice 5 barricades + archive-prep —
see phase/evidence).
blockers: none
📊 Model: fable-5 (ORDER 003, family-level self-report from this
session's own harness/environment banner).
review-queue: EMPTY.
lane position (honest): **the horde game now has a build verb.** Two
shipped playables in dist/ (Lumen Drift GBA scope-complete v1.3;
Gloamline NDS slice 5). Every future slice inherits proven rails:
pure-sim mirror proof, 32-word telemetry watches, gloam-route.py route
derivation, pinned replay proofs, stress-build perf probe, dist
convention. Archive-ready: a fresh session resumes from
`docs/retro/archive-ready-2026-07-11.md` § resume with no chat history
needed. The hourly wake trigger continues between directed slices.
repo-name note (cosmetic, owner's call, carried): repo named
`gba-homebrew`, now also hosts NDS (and a sibling's pirate arc);
nothing depends on a rename.
kit-follow-ups carried (still unclaimed, low priority): `upgrade
--apply-docs` (14 template-improved docs), migrate/retire legacy root
`claims/` (only its README remains), auto-merge enabler wiring decision
(`adopt --wire-enforcement`), upstream the PR #5 born-red-card gate rule
to substrate-kit.

⚑ needs-owner (carried + one NEW):
- **⚑ owner-click: create the Lumen Drift GitHub Release** (worker seat
  gets 403 on tags/releases). Suggested tag **`lumen-drift-v1.3`**. Exact
  clicks: repo → Releases → "Draft a new release" → "Choose a tag" → type
  `lumen-drift-v1.3` (create on publish, target `main`) → title
  `Lumen Drift v1.3` → attach `dist/lumen-drift.gba` from the merged tree
  (167,996 B; sha256
  `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94` —
  paste it in the notes) → point the notes at `docs/PLAYING.md` and the
  v1.3 entry in `docs/current-state.md` → Publish. (No Gloamline release
  ask yet: pre-v1; this repo's release convention has only shipped at
  scope-complete — revisit when the arc matures.)
- **⚑ owner-click: merged-branch cleanup** — worker seat gets 403 on
  branch-delete; merged `claude/*` branches (through PR #54 and this PR
  once merged, plus earlier kit-upgrade and wake branches) can be deleted
  in one sweep from the branches page.
- **⚑ owner-click (NEW): add `NDS ROM build` to the required checks**
  (repo Settings → Rulesets/Branch protection). Today only the GBA "ROM
  builds" job is required, so an armed auto-merge can fire before the
  NDS proofs finish (observed on PR #54's rebase-push; recorded in
  `docs/PLATFORM-LIMITS.md`). Until clicked, lane discipline is:
  verify the post-merge `main` run is green and say so.
- **⚑ graze tuning wants owner hands-on validation** — the
  400-frames-per-graze refund and the 6px shell are reasoned and
  machine-proven (CI asserts the refund and the lane geometry), but
  whether grazing FEELS right — risk-priced, readable, not exploitable —
  needs real hands on a real run. Owner-gated polish on a complete,
  shipped game. (Same invitation applies to Gloamline hand-feel:
  Shambler speed/stagger, shove push/stun/cooldown AND now barricade
  radius/hp/plank numbers await owner taste.)
- ⚑ (**awareness, no click needed**) — the routine model-attribution
  mismatch (ORDER 003 record, session 9) belongs in whatever the
  fleet-manager coordinator compiles for a report to Anthropic, alongside
  pokemon-mod-lab's parallel finding and the `websites` PR #59 precedent.
notes: worker seat (no tag/release/branch-delete perms — queued above).
Walls carried unchanged: api.github.com curl proxy-walled (GitHub MCP is
the merge path); mGBA python core.load_save() segfault (--savefile
bus-copy); devkitPro official infra Cloudflare-403 (mirror+pins for GBA;
N/A for NDS — Wonderful/BlocksDS hosts direct); py-desmume
KEYINPUT-at-reset quirk (keypad_update(0) before first cycle — respected
by every proof this session); py-desmume screenshot buffer is
BGR-swapped (colors correct on hardware — new, session 21). dist/
convention: player-facing games ship committed with provenance (TWO rows
live: lumen-drift.gba v1.3, gloamline.nds slice 5); engine-test
skeletons stay CI-artifact-only; CI sha256-logs every from-source build
and prints the committed-dist hash next to it.

## Shepherd + grooming (session 22, bare hourly wake — no directed slice)

- **Inbox re-read at HEAD:** ORDERS 001-004 all acked/done (see above);
  no new orders — standing default (advance current arc / groom
  backlog / never idle) continues, but per this lane's convention a
  bare hourly wake does the heartbeat-only pass (sessions 8-14
  precedent) — actual next-slice game-dev work (the scavenge interlude)
  is left for a directed/interactive session, not this bare wake.
- **Shepherd check:** `list_pull_requests(state=open)` → zero; `main`
  at `75ef9ba` (session 20's Brineward-skeleton merge, PR #55) at
  session start, unchanged at this write. Nothing stranded.
- **Grooming:** `docs/review-queue.md` re-read — still EMPTY, no new
  lead. No `docs/backlog.md` in this repo (review-queue is the
  convention here).
- **Visibility:** `menno420/gba-homebrew` is `public` (verified via
  `list_repos`) — expected; R22 (private-visibility guard) applies to
  the pokemon-mod-lab track only, not this one.
- **Health:** green — no code/build change this session; both game
  gates (`dist/lumen-drift.gba` v1.3, `dist/gloamline.nds` slice 5,
  `dist/brineward.nds` slice 2) untouched.
- 📊 Model: claude-sonnet-5 (ORDER 003, family-level self-report from
  this session's own harness/environment banner).

## Shepherd + grooming (session 23, bare hourly wake — no directed slice)

- **Inbox re-read at HEAD:** ORDERS 001-004 all acked/done; no new
  orders — standing default continues, bare hourly wake stays
  heartbeat-only (sessions 8-14, 22 precedent); next-slice game-dev
  work (the scavenge interlude) is left for a directed/interactive
  session.
- **Shepherd check:** `list_pull_requests(state=open)` → zero; `main`
  at `dcfa865` (session 22's merge, PR #57) at session start, unchanged
  at this write. Nothing stranded.
- **Grooming:** `docs/review-queue.md` re-read — still EMPTY, no new
  lead.
- **Visibility:** `menno420/gba-homebrew` is `public` (verified via
  `list_repos`) — expected; R22 applies to the pokemon-mod-lab track
  only.
- **Health:** green — no code/build change this session; all three
  game gates (`dist/lumen-drift.gba` v1.3, `dist/gloamline.nds` slice 5,
  `dist/brineward.nds` slice 2) untouched.
- 📊 Model: sonnet-5 (ORDER 003, family-level self-report from this
  session's own harness/environment banner).
