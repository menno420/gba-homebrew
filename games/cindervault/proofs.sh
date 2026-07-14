#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# CINDERVAULT — the committed headless proof suite (mGBA,
# tools/headless-screenshot.py), ported with the item-layer slice (growth
# cut 1), extended with the species slice (growth cut 2) and the endless
# depth-banding slice (growth cut 3). The prototype's proofs
# (.sessions/2026-07-13-cindervault.md) lived only in that session's
# container; per the Shoal/Courier convention the input scripts ARE the
# pin, so from now on they are committed here.
#
# Pin provenance (the prototype card's 💡 recipe, now with the mirror
# COMMITTED): every route below was derived by the route bot in
# games/cindervault/tools/mirror.py and every literal transcribed from the
# mirror's per-turn trace — after the mirror was certified against a real
# emulator watch-log of the SAME routes turn-for-turn (0 mismatches across
# all 414 turn states of the four routes, this build, with the cut-3
# verifier upgraded to full state-SEQUENCE matching: every distinct
# emulator state must be the mirror's next predicted state, nothing
# extra, nothing missing). Nothing is hand-computed.
#
# Growth cut 2 (named species per depth) touched NO floor generation RNG —
# species is a pure function of the floor number — so every fixed-seed
# spawn/layout pin below carried unchanged. What DID change is monster
# behavior on floors 2+ (that is the cut), so the two P3 hp pins moved
# (turn 38: hp 6 -> 9; end HUD: HP 5 -> 8 — wisps flit, hounds bite as a
# pack of one) and were re-derived through the re-certified mirror. P1 and
# P2 (floor 1 = CINDER RAT, the unchanged baseline chase) carried verbatim
# — which is itself the rat's species witness.
#
# Growth cut 3 (endless depth banding) is reachable ONLY through a SELECT
# press on the win screen, and floors 1-5 read the same parameter tables
# as before, so a run that never presses SELECT is bit-identical to cut 2:
# P1-P4 carried VERBATIM below, zero re-derived pins. Past the vault every
# floor draws (monsters, bigs, embers, carve length) from the depth-band
# table and the five species recur on the five-depth cycle. The bigs and
# carve parameters are pinned STRUCTURALLY through the RNG word order: a
# big rolls no HP word and every carve step consumes one, so changing
# either would shift every ember/monster/item placement P5's blind route
# then walks onto at exact turns (same law that pins item spawns, cut 1).
#
# Growth cut 4 (seed-select score-attack) changes WHICH stream a run draws
# from — so the carry is proven the Deepcast way: the feature's DEFAULT is
# the old constant (cv[3] boots at 0xC1DE5EED and only the title dial
# moves it), P1-P5 carried VERBATIM (zero re-derived pins — a run that
# never dials is bit-identical to cut 3), P7's dial-away-and-back run
# re-lands every P2 literal, and the dialed vault's own pins (P6) were
# derived through the mirror re-certified AT THE DIALED SEED (mirror.py
# grew a --seed parameter, and the verifier's word set grew cv[3]: the
# emulator's live seed word must now match the mirror's on every turn
# state of every certified route).
#
# Growth cut 5 (the art pass — tile sprites + torch-radius light fade) is
# PRESENTATION ONLY: game state, RNG word order and every cv_telemetry
# word are byte-identical to cut 4, so P1-P7 below carried VERBATIM (zero
# re-derived pins, zero adjusted text asserts — the HUD/title/end text
# never moved). What the cut adds is proven in P8/P9 through a SECOND
# mailbox (cv_light: the light circle's live radius + center, presentation
# words only) plus direct hardware-state pins — DISPCNT (the dungeon bg's
# enable bit flips with play state), BG palette RAM (the committed art's
# exact BGR555 colors), sprite palette RAM (the torch-bearer's), and the
# bg map's VRAM screenblock (the wall-row cells) — all read at GBA bus
# addresses by the same watch machinery. The light law under proof:
#
#     light_radius_px = min(16 + torch / 2, 200)      (200 = full screen)
#
# so the circle SHRINKS as the torch burns (P8: 126 -> 122 -> 120 -> 117
# -> 115 on the lantern route's own carried torch pins; 97 -> 48 on the
# deep-burn leg) and RE-OPENS on ember pickup (P9: 126 -> 155 through the
# dialed vault's +3-ember sweep) — the burn economy, made diegetic and
# then read back off the hardware.
#
# Run from the repo root after building (make -C games/cindervault):
#     bash games/cindervault/proofs.sh
# Artifacts land in $CV_PROOF_OUT (default /tmp/cindervault-proofs).
#
# The nine proofs (asserts inline below):
#   P1 boot/title            — mailbox magics, seed 0xC1DE5EED on the card,
#                              title state and text.
#   P2 THE LANTERN (item cut) — fixed-seed spawn pins (px/py, 2 monsters),
#                              clear floor 1, walk onto the lantern at turn
#                              8 (pickup activates, zero new verbs; the
#                              pickup turn burns nothing), then wait it
#                              out: 20 held turns burn EXACTLY 10 torch
#                              (213 -> 203, halved), the lantern gutters
#                              out at turn 27 (slot empty), and full burn
#                              resumes (198 by turn 32). RUN TWICE — the
#                              two watch-logs must be byte-identical
#                              (determinism contract).
#   P3 THE BLADE (item cut)  — same opening, then the one more decision:
#                              lantern picked at turn 8, REPLACED by the
#                              blade at turn 10 (single slot, item 1 -> 2),
#                              blade carried down two floors (dodging the
#                              floor lanterns; walking over floor 2's own
#                              blade is a harmless re-pick), and on floor 3
#                              three bump kills in three consecutive turns
#                              (kills 4->7, monsters 3->0) — so no monster
#                              took two bumps, and the 3-HP 'M' in that
#                              trio necessarily died in ONE bump: bumps-
#                              for-3 evidence (bump-2 needs two hits).
#                              RUN TWICE — byte-identical watch-logs.
#   P4 THE SPECIES (cut 2)   — one full descent past every named species,
#                              one arithmetic witness per quirk, ending in
#                              the OPEN VAULT (the winnability re-check):
#                              SOOT WISP first blood waits for the even
#                              beat (adjacent flit turn bites nothing);
#                              ASH HOUNDs ring the player two-at-once yet
#                              drain exactly -1/turn (one pack bite per
#                              phase; baseline drains -2); VAULT WRAITH
#                              draws first blood at range 2 on a pinned
#                              player tile, one turn EARLIER than any
#                              one-step chaser could reach adjacency from
#                              the floor's spawns; HOARD SENTINELs hold
#                              their posts through eight pinned turns
#                              (hp AND monsters-alive constant while the
#                              torch proves the turns are real), wake only
#                              when the leash is crossed — and the fourth
#                              guard still stands as the vault opens.
#                              Counterfactual (mirror --baseline): the
#                              SAME 109 inputs under the all-rat policy
#                              are first-bitten at turn 17 (vs 22) and die
#                              SLAIN on floor 3 at turn 39; the species
#                              world wins at turn 109, hp 3, score 1032.
#                              RUN TWICE — byte-identical watch-logs.
#   P5 THE DESCENT (cut 3)   — past the vault: the P4 route wins at turn
#                              109 exactly as pinned (the milestone
#                              stands, "SELECT DELVES DEEPER" on the win
#                              screen), then SELECT continues the SAME
#                              run — floor 6 from the RNG state the vault
#                              left behind, turn counter still 109 (a
#                              menu edge, no turn), hp/torch/items/kills
#                              carried, and the score drops 1032 -> 770:
#                              EXACTLY the 262 +torch win bonus, the
#                              price of not stopping. Band 1 (floors
#                              6-8): FOUR monsters per floor (cv[14]=4 on
#                              each entry vs 2-4 on floors 1-5), TWO
#                              embers per floor (the sweep visits every
#                              ember tile: +25 torch at turns 123/129,
#                              embers 6 -> 8 — and a third would shift
#                              every later pin), species recurring up the
#                              cycle (FOE CINDER RAT at 6, SOOT WISP at
#                              7, ASH HOUND at 8), and each floor cleared
#                              at ZERO hp cost by entry-choke fighting
#                              (kills 12 -> 24, hp 3 throughout: movers
#                              cannot bite, staggered arrivals die on
#                              arrival). Floor 7 re-proves the item layer
#                              at depth: lantern swap (slot 2 -> 1,
#                              countdown 19) then blade back (1 -> 2).
#                              Band 2 (floor 9): FIVE monsters (cv[14]=5
#                              — the band boundary in one word), FOE
#                              VAULT WRAITH, and the toll: five crossing
#                              grasp lines beat every greedy policy — the
#                              committed line is the beam-search OPTIMUM
#                              and still pays exactly 2 hp (3 -> 1,
#                              turns 206/207). Ends on floor 10 among
#                              FIVE HOARD SENTINELS frozen outside their
#                              leash — three pinned turns of hp AND guard
#                              count constant while the turn counter
#                              proves the run is live — at score 1495
#                              (900 floors + 325 embers + 270 kills, no
#                              torch bonus: endless is score-attack).
#                              Survivable, but escalating. RUN TWICE —
#                              byte-identical watch-logs.
#
#   P6 THE DIALED VAULT      — growth cut 4 (seed-select score-attack —
#     (cut 4)                  the concept's "daily seed" line, GBA-shaped
#                              per the Deepcast precedent): UP/RIGHT/R at
#                              the title dial the seed +1/+0x100/+0x10000
#                              (each step pinned in telemetry word cv[3],
#                              which now publishes the LIVE dial) to
#                              0xC1DF5FEE, shown glyph-exact on the title.
#                              START dives the DIALED vault: floor 1
#                              differs from the default seed's in the same
#                              telemetry words P2 pins at the same frame
#                              (spawn (1,4) vs (6,1)), while every law
#                              carries — the rats die at bump arithmetic,
#                              the ember economy pays +25, the wisp flit
#                              law holds on floor 2 (hp constant on odd
#                              turns, -2 on even beats with two adjacent).
#                              The route ends SLAIN at turn 38 and the
#                              death card — the score-attack card — NAMES
#                              the seed ("SEED C1DF5FEE"), so the score
#                              220 is attributable to its vault.
#                              RUN TWICE — byte-identical watch-logs.
#   P7 dial away and back     — UP/RIGHT/R then L/LEFT/DOWN returns the
#                              dial to 0xC1DE5EED (each magnitude
#                              down-step pinned in cv[3]), the title shows
#                              the default seed again, and the FULL P2
#                              route then lands on EVERY P2 literal —
#                              dialing is fully reversible and the default
#                              vault is untouched by the feature (the
#                              Deepcast P5 precedent).
#   P8 THE TORCHLIGHT (cut 5) — the art pass, read off the hardware. Leg
#                              A reruns the FULL P2 route with the light
#                              and hardware watches added: every P2 pin
#                              still lands (the sim cannot feel the art),
#                              the light circle sits on the player's tile
#                              center, and its radius follows the torch
#                              through five carried torch pins (220->126,
#                              213->122, 208->120, 203->117, 198->115 px
#                              — strictly shrinking); DISPCNT flips
#                              0xF040 (title: no bg) -> 0xF840 (playing:
#                              the dungeon bg's enable bit), the BG/OBJ
#                              palette RAM words hold the committed art's
#                              exact BGR555 colors, and the bg map's
#                              screenblock holds the wall-row cells. Leg
#                              B (THE DEEP BURN) rides the same opening
#                              then waits ~160 turns: torch 163 -> radius
#                              97, torch 64 -> radius 48 — at 48px the
#                              screen is VISIBLY mostly dark (the shots
#                              are the PR evidence). BOTH legs RUN TWICE
#                              — watch-log CSVs (now including the light
#                              words and the hardware reads) must be
#                              byte-identical.
#   P9 THE EMBER RELIGHT      — the other direction of the law, on the
#      (cut 5)                  P6 dialed-vault route with every P6 pin
#                              carried: full radius 200 on the dialed
#                              title (the fade only lives in the dungeon),
#                              126 px on entry (torch 220), 155 px at turn
#                              16 (torch 279 after the +3-ember sweep —
#                              the light RE-OPENS when embers feed the
#                              torch), and 200 again on the death card.
#                              RUN TWICE — byte-identical watch-logs.
#
# Item-spawn determinism is pinned structurally: both routes walk blind,
# fixed key scripts onto the item tiles of seed 0xC1DE5EED and assert the
# slot word flips at the exact scripted turn — a shifted spawn fails loud.
# Species assignment by depth is pinned the same way: the routes descend
# blind and assert each floor's "FOE <NAME>" HUD line plus each quirk's
# arithmetic signature at exact scripted turns.
# ---------------------------------------------------------------------------
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."

ROM=games/cindervault/cindervault.gba
OUT="${CV_PROOF_OUT:-/tmp/cindervault-proofs}"
mkdir -p "$OUT"

H() { python3 tools/headless-screenshot.py "$ROM" "$@"; }
W='--elf games/cindervault/cindervault.elf --watch cv:cv_telemetry:18'

echo "== P1: boot + title =="
H "$OUT/p1.png" --frames 120 $W \
  --assert-watch 100:cv:0:eq:1128877636 \
  --assert-watch 100:cv:1:eq:1447119956 \
  --assert-watch 100:cv:2:eq:0 \
  --assert-watch 100:cv:3:eq:3252575981 \
  --assert-watch 100:cv:15:gt:0 \
  --assert-text "100:CINDERVAULT" \
  --assert-text "100:SEED C1DE5EED" \
  --assert-text "100:EVERY STEP BURNS THE TORCH" \
  --assert-text "100:PRESS START" \
  --assert-text "100:PRESS START  DPAD L R: DIAL SEED"

# Mirror route (games/cindervault/tools/mirror.py --design lantern):
#   DDDRDRRRWWWWWWWWWWWWWWWWWWWWWWWW
# Turn N is pressed at frame 300+6(N-1) and asserted at press+4.
LANTERN_ROUTE='--keys 240-242:START --keys-pattern 300-314:6:2:DOWN --keys 318-320:RIGHT --keys 324-326:DOWN --keys-pattern 330-344:6:2:RIGHT --keys-pattern 348-488:6:2:A'

P2_ASSERTS=(
  # fixed-seed floor 1: spawn (6,1), 2 monsters, full torch, empty slot
  --assert-watch 250:cv:2:eq:1
  --assert-watch 250:cv:4:eq:1
  --assert-watch 250:cv:5:eq:10
  --assert-watch 250:cv:6:eq:220
  --assert-watch 250:cv:11:eq:6
  --assert-watch 250:cv:12:eq:1
  --assert-watch 250:cv:14:eq:2
  --assert-watch 250:cv:16:eq:0
  --assert-watch 250:cv:17:eq:0
  --assert-text "250:ITEM -"
  # floor 1's named species on the HUD (growth cut 2) — and P2's pins all
  # carrying verbatim IS the CINDER RAT witness: the baseline chase.
  --assert-text "250:FOE CINDER RAT"
  # floor cleared by turn 5 (both biters bumped down)
  --assert-watch 328:cv:8:eq:2
  --assert-watch 328:cv:14:eq:0
  # turn 7: about to step onto the lantern; torch 213
  --assert-watch 340:cv:6:eq:213
  --assert-watch 340:cv:16:eq:0
  # turn 8: LANTERN PICKED UP by walking onto it — slot 1, countdown 19,
  # and the pickup turn itself burned NOTHING (torch still 213)
  --assert-watch 346:cv:16:eq:1
  --assert-watch 346:cv:17:eq:19
  --assert-watch 346:cv:6:eq:213
  --assert-text "346:ITEM LANTERN 19"
  # turn 17: half-burn arithmetic mid-flight (5 burned over 9 held turns)
  --assert-watch 400:cv:6:eq:208
  --assert-watch 400:cv:17:eq:10
  --assert-text "400:ITEM LANTERN 10"
  # turn 27: the lantern gutters out — slot empty, and EXACTLY 10 torch
  # burned over its 20 held turns (213 at turn 7 -> 203 at turn 27)
  --assert-watch 460:cv:16:eq:0
  --assert-watch 460:cv:17:eq:0
  --assert-watch 460:cv:6:eq:203
  --assert-text "460:ITEM -"
  # turns 28-32: full burn resumed (1 per turn: 203 -> 198)
  --assert-watch 490:cv:6:eq:198
  --assert-watch 490:cv:10:eq:32
  --assert-text "490:FLR 1 HP 10 TORCH 198 SCORE 20"
)

echo "== P2: THE LANTERN — halved burn for 20 turns (run 1) =="
H "$OUT/p2.png" --frames 500 $W $LANTERN_ROUTE \
  --watch-log "$OUT/p2-run1.csv" \
  --shot "250:$OUT/p2-start.png" --shot "350:$OUT/p2-lantern.png" \
  --require-distinct \
  "${P2_ASSERTS[@]}"

echo "== P2: run 2 (must be byte-identical) =="
H "$OUT/p2b.png" --frames 500 $W $LANTERN_ROUTE \
  --watch-log "$OUT/p2-run2.csv" \
  "${P2_ASSERTS[@]}"
cmp "$OUT/p2-run1.csv" "$OUT/p2-run2.csv"
echo "P2 run-twice: byte-identical"

# Mirror route (games/cindervault/tools/mirror.py --design blade):
#   DDDRDRRRDRLLDLLLLLLLUUUULLRRRRRUURRDRRURR
BLADE_ROUTE='--keys 240-242:START --keys-pattern 300-314:6:2:DOWN --keys 318-320:RIGHT --keys 324-326:DOWN --keys-pattern 330-344:6:2:RIGHT --keys 348-350:DOWN --keys 354-356:RIGHT --keys-pattern 360-368:6:2:LEFT --keys 372-374:DOWN --keys-pattern 378-416:6:2:LEFT --keys-pattern 420-440:6:2:UP --keys-pattern 444-452:6:2:LEFT --keys-pattern 456-482:6:2:RIGHT --keys-pattern 486-494:6:2:UP --keys-pattern 498-506:6:2:RIGHT --keys 510-512:DOWN --keys-pattern 516-524:6:2:RIGHT --keys 528-530:UP --keys-pattern 534-542:6:2:RIGHT'

P3_ASSERTS=(
  # turn 8: lantern in the slot first...
  --assert-watch 346:cv:16:eq:1
  --assert-watch 346:cv:17:eq:19
  # turn 10: ...REPLACED by the blade (single slot — the one more decision)
  --assert-watch 358:cv:16:eq:2
  --assert-watch 358:cv:17:eq:0
  --assert-text "358:ITEM BLADE"
  # turn 20: stairs taken, blade carried onto floor 2
  --assert-watch 418:cv:4:eq:2
  --assert-watch 418:cv:6:eq:201
  --assert-watch 418:cv:9:eq:120
  --assert-watch 418:cv:16:eq:2
  # turn 34: floor 3 — three monsters, slot 0 the 3-HP 'M'
  --assert-watch 502:cv:4:eq:3
  --assert-watch 502:cv:14:eq:3
  --assert-watch 502:cv:16:eq:2
  # turn 38: about to engage — kills 4, all three still alive. hp re-pinned
  # 6 -> 9 for growth cut 2 (wisps flit, hounds bite as a pack of one; the
  # route and every other pin carried) — re-derived via the mirror.
  --assert-watch 526:cv:8:eq:4
  --assert-watch 526:cv:14:eq:3
  --assert-watch 526:cv:5:eq:9
  # turns 39-41: three kills in three consecutive turns (no monster took
  # two bumps -> the 3-HP 'M' died in ONE bump: blade bumps for 3)
  --assert-watch 532:cv:8:eq:5
  --assert-watch 532:cv:14:eq:2
  --assert-watch 538:cv:8:eq:6
  --assert-watch 538:cv:14:eq:1
  --assert-watch 544:cv:8:eq:7
  --assert-watch 544:cv:14:eq:0
  --assert-watch 544:cv:16:eq:2
  --assert-text "544:ITEM BLADE"
  # HP 5 -> 8 for growth cut 2 (same re-derivation as the turn-38 pin)
  --assert-text "544:FLR 3 HP 8 TORCH 255 SCORE 345"
)

echo "== P3: THE BLADE — slot swap, carried two floors, one-bumps the M (run 1) =="
H "$OUT/p3.png" --frames 560 $W $BLADE_ROUTE \
  --watch-log "$OUT/p3-run1.csv" \
  --shot "360:$OUT/p3-swap.png" --shot "502:$OUT/p3-floor3.png" \
  --require-distinct \
  "${P3_ASSERTS[@]}"

echo "== P3: run 2 (must be byte-identical) =="
H "$OUT/p3b.png" --frames 560 $W $BLADE_ROUTE \
  --watch-log "$OUT/p3-run2.csv" \
  "${P3_ASSERTS[@]}"
cmp "$OUT/p3-run1.csv" "$OUT/p3-run2.csv"
echo "P3 run-twice: byte-identical"

# Mirror route (games/cindervault/tools/mirror.py --design species), 109
# inputs, WIN at turn 109:
#   DDDRDDDLLLLLWWWWWWWWWWWLRURUURRUURRDRRWWURRRDDDRRDRRWWWUWDDUWDDDRR
#   DDDRDRRRRRRWWWWWWWWUUUURDDDUURUURRRRDDURRRR
SPECIES_ROUTE='--keys 240-242:START --keys-pattern 300-314:6:2:DOWN --keys 318-320:RIGHT --keys-pattern 324-338:6:2:DOWN --keys-pattern 342-368:6:2:LEFT --keys-pattern 372-434:6:2:A --keys 438-440:LEFT --keys 444-446:RIGHT --keys 450-452:UP --keys 456-458:RIGHT --keys-pattern 462-470:6:2:UP --keys-pattern 474-482:6:2:RIGHT --keys-pattern 486-494:6:2:UP --keys-pattern 498-506:6:2:RIGHT --keys 510-512:DOWN --keys-pattern 516-524:6:2:RIGHT --keys-pattern 528-536:6:2:A --keys 540-542:UP --keys-pattern 546-560:6:2:RIGHT --keys-pattern 564-578:6:2:DOWN --keys-pattern 582-590:6:2:RIGHT --keys 594-596:DOWN --keys-pattern 600-608:6:2:RIGHT --keys-pattern 612-626:6:2:A --keys 630-632:UP --keys 636-638:A --keys-pattern 642-650:6:2:DOWN --keys 654-656:UP --keys 660-662:A --keys-pattern 666-680:6:2:DOWN --keys-pattern 684-692:6:2:RIGHT --keys-pattern 696-710:6:2:DOWN --keys 714-716:RIGHT --keys 720-722:DOWN --keys-pattern 726-758:6:2:RIGHT --keys-pattern 762-806:6:2:A --keys-pattern 810-830:6:2:UP --keys 834-836:RIGHT --keys-pattern 840-854:6:2:DOWN --keys-pattern 858-866:6:2:UP --keys 870-872:RIGHT --keys-pattern 876-884:6:2:UP --keys-pattern 888-908:6:2:RIGHT --keys-pattern 912-920:6:2:DOWN --keys 924-926:UP --keys-pattern 930-950:6:2:RIGHT'

P4_ASSERTS=(
  # ---- floor 2: SOOT WISP (flits on odd turn counts — half-rate) ----
  # turn 12: onto floor 2 untouched; the depth's name on the HUD
  --assert-watch 370:cv:4:eq:2
  --assert-watch 370:cv:5:eq:10
  --assert-watch 370:cv:14:eq:2
  --assert-text "370:FOE SOOT WISP"
  # turn 21: a wisp stands ADJACENT (it arrived on even turn 20 — the
  # mirror certifies both) and the player has waited in place since turn
  # 13 — yet hp is still 10: odd turn counts are flit turns, no bite.
  # Counterfactual: the all-rat mirror replay of the SAME inputs is at
  # hp 5 here (first blood turn 17).
  --assert-watch 424:cv:5:eq:10
  --assert-watch 424:cv:10:eq:21
  # turn 22: first blood lands exactly on the even beat...
  --assert-watch 430:cv:5:eq:9
  # turn 23: ...and the next odd turn is bite-free again (the staircase)
  --assert-watch 436:cv:5:eq:9
  # turn 24: the answering bump kills the wisp on ITS strike beat — the
  # bump resolves before the monster phase, so hp still 9
  --assert-watch 442:cv:8:eq:3
  --assert-watch 442:cv:14:eq:1
  --assert-watch 442:cv:5:eq:9
  # ---- floor 3: ASH HOUND (one pack bite per phase) ----
  # turn 34: onto floor 3 (blade picked crossing floor 2's cut tile)
  --assert-watch 502:cv:4:eq:3
  --assert-watch 502:cv:14:eq:3
  --assert-watch 502:cv:16:eq:2
  --assert-watch 502:cv:5:eq:9
  --assert-text "502:FOE ASH HOUND"
  # turns 39-40: TWO hounds ring the player both turns (mirror-certified,
  # standing on the floor's blade tile) — yet the drain is EXACTLY -1 per
  # turn: one pack bite per phase. Baseline chase drains -2 per turn here.
  --assert-watch 532:cv:5:eq:7
  --assert-watch 532:cv:14:eq:3
  --assert-watch 538:cv:5:eq:6
  --assert-watch 538:cv:14:eq:3
  # turns 41-43: the pack bladed down in three consecutive kills — and hp
  # holds at 5 through turns 42-43 (each bump lands before the phase, and
  # the dead cannot bite); the 3-HP alpha 'M' one-bumped last
  --assert-watch 544:cv:8:eq:4
  --assert-watch 544:cv:14:eq:2
  --assert-watch 544:cv:5:eq:5
  --assert-watch 550:cv:8:eq:5
  --assert-watch 550:cv:14:eq:1
  --assert-watch 550:cv:5:eq:5
  --assert-watch 556:cv:8:eq:6
  --assert-watch 556:cv:14:eq:0
  --assert-watch 556:cv:5:eq:5
  # ---- floor 4: VAULT WRAITH (cold grasp: aligned, range 2) ----
  # turn 52: onto floor 4 at the pinned entry tile (2,2), hp 5
  --assert-watch 610:cv:4:eq:4
  --assert-watch 610:cv:14:eq:3
  --assert-watch 610:cv:5:eq:5
  --assert-watch 610:cv:11:eq:2
  --assert-watch 610:cv:12:eq:2
  --assert-text "610:FOE VAULT WRAITH"
  # turns 53-54: standing still, untouched — nothing can reach adjacency
  --assert-watch 622:cv:5:eq:5
  # turn 55: FIRST BLOOD AT RANGE — hp drops on the pinned tile with the
  # nearest wraith two tiles away on the same row and NOTHING adjacent
  # (mirror-certified). From this floor's spawns the earliest a one-step
  # chaser could land a bite is turn 56 (arrive adjacent 55, bite 56):
  # turn-55 blood is reachable ONLY with the range-2 grasp.
  --assert-watch 628:cv:5:eq:4
  --assert-watch 628:cv:11:eq:2
  --assert-watch 628:cv:12:eq:2
  --assert-watch 628:cv:14:eq:3
  # turn 58: the grasper baited off its line and bladed — and the matador
  # run costs no further hp (hp 4 all the way to floor 5)
  --assert-watch 646:cv:8:eq:7
  --assert-watch 646:cv:14:eq:2
  --assert-watch 646:cv:5:eq:4
  # ---- floor 5: HOARD SENTINEL (holds post outside leash 4) ----
  # turn 77: onto floor 5 at the pinned entry tile, four guards up
  --assert-watch 760:cv:4:eq:5
  --assert-watch 760:cv:14:eq:4
  --assert-watch 760:cv:5:eq:4
  --assert-watch 760:cv:11:eq:2
  --assert-watch 760:cv:12:eq:5
  --assert-text "760:FOE HOARD SENTINEL"
  # turns 78-85: eight pinned turns standing still — hp AND guard count
  # frozen while the burning torch proves the turns are real (267 -> 260).
  # Baseline chasers spawned 7-10 away would be biting by turn 84.
  --assert-watch 766:cv:5:eq:4
  --assert-watch 766:cv:14:eq:4
  --assert-watch 766:cv:6:eq:267
  --assert-watch 790:cv:5:eq:4
  --assert-watch 790:cv:14:eq:4
  --assert-watch 808:cv:5:eq:4
  --assert-watch 808:cv:14:eq:4
  --assert-watch 808:cv:6:eq:260
  --assert-watch 808:cv:10:eq:85
  # turn 94: still untouched crossing the floor, right up to the leash
  --assert-watch 862:cv:5:eq:4
  --assert-watch 862:cv:8:eq:9
  --assert-watch 862:cv:14:eq:4
  # turns 95-96: the leash crossed — the woken guards move, bite once, and
  # are bladed down in consecutive turns (kills 9 -> 11)
  --assert-watch 868:cv:5:eq:3
  --assert-watch 868:cv:8:eq:10
  --assert-watch 868:cv:14:eq:3
  --assert-watch 874:cv:8:eq:11
  --assert-watch 874:cv:14:eq:2
  # turn 104: the stairs-side guard falls too (kills 12)
  --assert-watch 922:cv:8:eq:12
  --assert-watch 922:cv:14:eq:1
  # turn 109: THE VAULT OPENS — and the last guard NEVER woke: it still
  # holds its post (monsters alive 1) as the run banks 1032
  --assert-watch 952:cv:2:eq:2
  --assert-watch 952:cv:4:eq:5
  --assert-watch 952:cv:5:eq:3
  --assert-watch 952:cv:6:eq:262
  --assert-watch 952:cv:8:eq:12
  --assert-watch 952:cv:9:eq:1032
  --assert-watch 952:cv:14:eq:1
  --assert-text "952:VAULT REACHED"
  --assert-text "952:SCORE 1032"
)

echo "== P4: THE SPECIES — five depths, five quirks, and the open vault (run 1) =="
H "$OUT/p4.png" --frames 980 $W $SPECIES_ROUTE \
  --watch-log "$OUT/p4-run1.csv" \
  --shot "370:$OUT/p4-wisp.png" --shot "628:$OUT/p4-grasp.png" \
  --shot "808:$OUT/p4-sentinel.png" \
  --require-distinct \
  "${P4_ASSERTS[@]}"

echo "== P4: run 2 (must be byte-identical) =="
H "$OUT/p4b.png" --frames 980 $W $SPECIES_ROUTE \
  --watch-log "$OUT/p4-run2.csv" \
  "${P4_ASSERTS[@]}"
cmp "$OUT/p4-run1.csv" "$OUT/p4-run2.csv"
echo "P4 run-twice: byte-identical"

# Mirror route (games/cindervault/tools/mirror.py --design endless), 228
# inputs = the 109-input species route (P4, byte-for-byte — same spans up
# to frame 950) + SELECT on the win screen + the 118-input deep leg
# (band-1 entry chokes + sweeps, the floor-7 item two-step, the floor-9
# beam-searched flee line, three closing waits among the sentinels):
#   DDDRDDDLLLLLWWWWWWWWWWWLRURUURRUURRDRRWWURRRDDDRRDRRWWWUWDDUWDDDRR
#   DDDRDRRRRRRWWWWWWWWUUUURDDDUURUURRRRDDURRRR S WWWRWWUWRRRDRRUURRRR
#   URRWWWWDWWWDWUDDWWWWWWWLUUULLLURRRDDDLLRRRRULULLLLLLLLDWDDUWUURLLU
#   LDDDDLLLDL RURUUWUDULLLLLLLLLU WWW
ENDLESS_ROUTE='--keys 240-242:START --keys-pattern 300-314:6:2:DOWN --keys 318-320:RIGHT --keys-pattern 324-338:6:2:DOWN --keys-pattern 342-368:6:2:LEFT --keys-pattern 372-434:6:2:A --keys 438-440:LEFT --keys 444-446:RIGHT --keys 450-452:UP --keys 456-458:RIGHT --keys-pattern 462-470:6:2:UP --keys-pattern 474-482:6:2:RIGHT --keys-pattern 486-494:6:2:UP --keys-pattern 498-506:6:2:RIGHT --keys 510-512:DOWN --keys-pattern 516-524:6:2:RIGHT --keys-pattern 528-536:6:2:A --keys 540-542:UP --keys-pattern 546-560:6:2:RIGHT --keys-pattern 564-578:6:2:DOWN --keys-pattern 582-590:6:2:RIGHT --keys 594-596:DOWN --keys-pattern 600-608:6:2:RIGHT --keys-pattern 612-626:6:2:A --keys 630-632:UP --keys 636-638:A --keys-pattern 642-650:6:2:DOWN --keys 654-656:UP --keys 660-662:A --keys-pattern 666-680:6:2:DOWN --keys-pattern 684-692:6:2:RIGHT --keys-pattern 696-710:6:2:DOWN --keys 714-716:RIGHT --keys 720-722:DOWN --keys-pattern 726-758:6:2:RIGHT --keys-pattern 762-806:6:2:A --keys-pattern 810-830:6:2:UP --keys 834-836:RIGHT --keys-pattern 840-854:6:2:DOWN --keys-pattern 858-866:6:2:UP --keys 870-872:RIGHT --keys-pattern 876-884:6:2:UP --keys-pattern 888-908:6:2:RIGHT --keys-pattern 912-920:6:2:DOWN --keys 924-926:UP --keys-pattern 930-950:6:2:RIGHT --keys 954-956:SELECT --keys-pattern 960-974:6:2:A --keys 978-980:RIGHT --keys-pattern 984-992:6:2:A --keys 996-998:UP --keys 1002-1004:A --keys-pattern 1008-1022:6:2:RIGHT --keys 1026-1028:DOWN --keys-pattern 1032-1040:6:2:RIGHT --keys-pattern 1044-1052:6:2:UP --keys-pattern 1056-1076:6:2:RIGHT --keys 1080-1082:UP --keys-pattern 1086-1094:6:2:RIGHT --keys-pattern 1098-1118:6:2:A --keys 1122-1124:DOWN --keys-pattern 1128-1142:6:2:A --keys 1146-1148:DOWN --keys 1152-1154:A --keys 1158-1160:UP --keys-pattern 1164-1172:6:2:DOWN --keys-pattern 1176-1214:6:2:A --keys 1218-1220:LEFT --keys-pattern 1224-1238:6:2:UP --keys-pattern 1242-1256:6:2:LEFT --keys 1260-1262:UP --keys-pattern 1266-1280:6:2:RIGHT --keys-pattern 1284-1298:6:2:DOWN --keys-pattern 1302-1310:6:2:LEFT --keys-pattern 1314-1334:6:2:RIGHT --keys 1338-1340:UP --keys 1344-1346:LEFT --keys 1350-1352:UP --keys-pattern 1356-1400:6:2:LEFT --keys 1404-1406:DOWN --keys 1410-1412:A --keys-pattern 1416-1424:6:2:DOWN --keys 1428-1430:UP --keys 1434-1436:A --keys-pattern 1440-1448:6:2:UP --keys 1452-1454:RIGHT --keys-pattern 1458-1466:6:2:LEFT --keys 1470-1472:UP --keys 1476-1478:LEFT --keys-pattern 1482-1502:6:2:DOWN --keys-pattern 1506-1520:6:2:LEFT --keys 1524-1526:DOWN --keys 1530-1532:LEFT --keys 1536-1538:RIGHT --keys 1542-1544:UP --keys 1548-1550:RIGHT --keys-pattern 1554-1562:6:2:UP --keys 1566-1568:A --keys 1572-1574:UP --keys 1578-1580:DOWN --keys 1584-1586:UP --keys-pattern 1590-1640:6:2:LEFT --keys 1644-1646:UP --keys-pattern 1650-1664:6:2:A'

P5_ASSERTS=(
  # turn 109 (pre-SELECT): the vault milestone STANDS, exactly as P4 pins
  # it — and the win screen now offers the delve
  --assert-watch 952:cv:2:eq:2
  --assert-watch 952:cv:6:eq:262
  --assert-watch 952:cv:9:eq:1032
  --assert-text "952:VAULT REACHED"
  --assert-text "952:SELECT DELVES DEEPER"
  # SELECT (input 110): the SAME run continues — floor 6, turn count
  # still 109 (a menu edge consumes no turn), hp/torch/embers/kills/blade
  # all carried, entry tile pinned, and the score drops 1032 -> 770:
  # EXACTLY the 262 +torch win bonus, back out of the live formula
  --assert-watch 958:cv:2:eq:1
  --assert-watch 958:cv:4:eq:6
  --assert-watch 958:cv:10:eq:109
  --assert-watch 958:cv:5:eq:3
  --assert-watch 958:cv:6:eq:262
  --assert-watch 958:cv:7:eq:6
  --assert-watch 958:cv:8:eq:12
  --assert-watch 958:cv:9:eq:770
  --assert-watch 958:cv:11:eq:2
  --assert-watch 958:cv:12:eq:3
  --assert-watch 958:cv:16:eq:2
  # BAND 1, floor 6: FOUR monsters (floors 1-5 spawn 2-4; cv[14] IS the
  # band table's first column) and the species cycle restarts at depth
  --assert-watch 958:cv:14:eq:4
  --assert-text "958:FOE CINDER RAT"
  --assert-text "958:FLR 6 HP 3 TORCH 262 SCORE 770"
  # turn 119: all four rats dead at the entry choke — kills 12 -> 16 and
  # hp STILL 3 (arrivals cannot bite; the answering bump lands first)
  --assert-watch 1018:cv:8:eq:16
  --assert-watch 1018:cv:14:eq:0
  --assert-watch 1018:cv:5:eq:3
  # turns 123/129: the ember sweep visits EVERY ember tile the floor has
  # — exactly TWO pickups (+25 torch each): band 1's ember economy (a
  # third ember would shift every pin below this line)
  --assert-watch 1042:cv:7:eq:7
  --assert-watch 1042:cv:6:eq:273
  --assert-watch 1078:cv:7:eq:8
  --assert-watch 1078:cv:6:eq:292
  --assert-watch 1078:cv:10:eq:129
  # turn 132: floor 7 — four monsters again, and the cycle walks on
  --assert-watch 1096:cv:4:eq:7
  --assert-watch 1096:cv:14:eq:4
  --assert-watch 1096:cv:5:eq:3
  --assert-text "1096:FOE SOOT WISP"
  # turn 153: four wisps down, still untouched (kills 16 -> 20, hp 3)
  --assert-watch 1222:cv:8:eq:20
  --assert-watch 1222:cv:14:eq:0
  --assert-watch 1222:cv:5:eq:3
  # turns 159/163: the item layer works at depth — endless floors spawn
  # both items; the single slot swaps blade -> lantern -> blade
  --assert-watch 1258:cv:16:eq:1
  --assert-watch 1258:cv:17:eq:19
  --assert-text "1258:ITEM LANTERN 19"
  --assert-watch 1282:cv:16:eq:2
  --assert-watch 1282:cv:17:eq:0
  --assert-text "1282:ITEM BLADE"
  # turn 183: floor 8 — the third band-1 floor, four hounds
  --assert-watch 1402:cv:4:eq:8
  --assert-watch 1402:cv:14:eq:4
  --assert-watch 1402:cv:9:eq:1150
  --assert-text "1402:FOE ASH HOUND"
  # turn 190: the pack desynced and dismantled — kills 20 -> 24, and ALL
  # of band 1 was cleared at zero hp cost (hp 3 since the vault)
  --assert-watch 1444:cv:8:eq:24
  --assert-watch 1444:cv:14:eq:0
  --assert-watch 1444:cv:5:eq:3
  # turn 205: floor 9 — BAND 2 begins: FIVE monsters on entry (the band
  # boundary observable in one telemetry word), entry tile pinned
  --assert-watch 1534:cv:4:eq:9
  --assert-watch 1534:cv:14:eq:5
  --assert-watch 1534:cv:5:eq:3
  --assert-watch 1534:cv:11:eq:9
  --assert-watch 1534:cv:12:eq:6
  --assert-text "1534:FOE VAULT WRAITH"
  # turns 206-207: the wraith toll — five crossing grasp lines cost the
  # beam-search-OPTIMAL flee line exactly 2 hp, one per turn
  --assert-watch 1540:cv:5:eq:2
  --assert-watch 1546:cv:5:eq:1
  # turn 224: floor 10 on hp 1 — five sentinels, score 1495 with NO torch
  # bonus (900 floors + 325 embers + 270 kills): endless is score-attack
  --assert-watch 1648:cv:4:eq:10
  --assert-watch 1648:cv:14:eq:5
  --assert-watch 1648:cv:5:eq:1
  --assert-watch 1648:cv:9:eq:1495
  --assert-text "1648:FOE HOARD SENTINEL"
  # turns 225-227: the guards hold their posts outside the leash — hp AND
  # guard count frozen across three pinned turns while the turn counter
  # proves the run is live. Survivable, but escalating.
  --assert-watch 1654:cv:5:eq:1
  --assert-watch 1654:cv:14:eq:5
  --assert-text "1654:FLR 10 HP 1 TORCH 328 SCORE 1495"
  --assert-watch 1666:cv:2:eq:1
  --assert-watch 1666:cv:5:eq:1
  --assert-watch 1666:cv:14:eq:5
  --assert-watch 1666:cv:10:eq:227
  --assert-watch 1666:cv:9:eq:1495
)

echo "== P5: THE DESCENT — past the vault, two bands deep, alive (run 1) =="
H "$OUT/p5.png" --frames 1700 $W $ENDLESS_ROUTE   --watch-log "$OUT/p5-run1.csv"   --shot "958:$OUT/p5-delve.png" --shot "1534:$OUT/p5-band2.png"   --shot "1648:$OUT/p5-floor10.png"   --require-distinct   "${P5_ASSERTS[@]}"

echo "== P5: run 2 (must be byte-identical) =="
H "$OUT/p5b.png" --frames 1700 $W $ENDLESS_ROUTE   --watch-log "$OUT/p5-run2.csv"   "${P5_ASSERTS[@]}"
cmp "$OUT/p5-run1.csv" "$OUT/p5-run2.csv"
echo "P5 run-twice: byte-identical"

# ---------------------------------------------------------------------------
# P6/P7 — growth cut 4, seed-select score-attack. The dial: on the title,
# UP/DOWN adjusts the seed +-1, LEFT/RIGHT +-0x100, L/R +-0x10000
# (edge-triggered, 32-bit wrap, xorshift dead state 0 skipped; the dial
# lives ONLY on the title — these keys are dungeon verbs everywhere else).
# Pins from an observed run of THIS route through the re-certified mirror
# (mirror.py --seed 0xC1DF5FEE --cmds ... --verify: 0 mismatches across
# all 39 turn states, with cv[3] added to the verifier's word set):
# 0xC1DE5EED=3252575981 · +1=3252575982 · +0x101=3252576238 ·
# +0x10101 = 0xC1DF5FEE = 3252641774.
# ---------------------------------------------------------------------------

DIAL_UP='--keys 150-152:UP --keys 154-156:RIGHT --keys 158-160:R'
DIAL_BACK='--keys 170-172:L --keys 174-176:LEFT --keys 178-180:DOWN'

# The dialed-vault route (mirror.py --seed 0xC1DF5FEE: kill_all +
# sweep_embers + descend + walk into the wisps' bite ring + stand until
# SLAIN), 38 inputs:
#   DDRURRRUUUURRRDRURURRRDDDRRRRRWWWWWWWW
P6_ROUTE=$DIAL_UP' --keys 240-242:START --keys-pattern 300-308:6:2:DOWN --keys 312-314:RIGHT --keys 318-320:UP --keys-pattern 324-338:6:2:RIGHT --keys-pattern 342-362:6:2:UP --keys-pattern 366-380:6:2:RIGHT --keys 384-386:DOWN --keys 390-392:RIGHT --keys 396-398:UP --keys 402-404:RIGHT --keys 408-410:UP --keys-pattern 414-428:6:2:RIGHT --keys-pattern 432-446:6:2:DOWN --keys-pattern 450-476:6:2:RIGHT --keys-pattern 480-524:6:2:A'

P6_ASSERTS=(
  # the dial, step by step in the telemetry seed word (boot value first;
  # cv[3] publishes the LIVE dial, so each keypress is its own literal)
  --assert-watch 149:cv:3:eq:3252575981
  --assert-watch 153:cv:3:eq:3252575982
  --assert-watch 157:cv:3:eq:3252576238
  --assert-watch 200:cv:3:eq:3252641774
  # the dialed seed on the title, glyph-exact; dialing consumes nothing
  --assert-text "200:SEED C1DF5FEE"
  --assert-watch 200:cv:2:eq:0
  # START dives the DIALED vault: floor 1 differs from the default
  # seed's IN THE WORDS P2 PINS AT THIS SAME FRAME — spawn (1,4) here
  # vs (6,1) there — while the laws carry: 2 monsters, full torch,
  # empty slot, and floor 1 is still CINDER RAT (species is a function
  # of depth, not seed)
  --assert-watch 250:cv:2:eq:1
  --assert-watch 250:cv:3:eq:3252641774
  --assert-watch 250:cv:4:eq:1
  --assert-watch 250:cv:5:eq:10
  --assert-watch 250:cv:6:eq:220
  --assert-watch 250:cv:11:eq:1
  --assert-watch 250:cv:12:eq:4
  --assert-watch 250:cv:14:eq:2
  --assert-watch 250:cv:16:eq:0
  --assert-text "250:FOE CINDER RAT"
  --assert-text "250:FLR 1 HP 10 TORCH 220 SCORE 0"
  # turn 8: this vault's rats roll 2 HP (default floor 1 rolls 1s) —
  # both die to the bump-2 arithmetic, and the exchange costs 2 hp
  --assert-watch 346:cv:8:eq:2
  --assert-watch 346:cv:14:eq:0
  --assert-watch 346:cv:5:eq:8
  # turn 16: the ember economy carries to any seed — three sweeps at
  # +25 each (torch 212 -> 279 net of burn), score 95
  --assert-watch 394:cv:7:eq:3
  --assert-watch 394:cv:6:eq:279
  --assert-watch 394:cv:9:eq:95
  # turn 21: floor 2 of the dialed vault, entry pinned, two wisps up
  --assert-watch 424:cv:4:eq:2
  --assert-watch 424:cv:11:eq:1
  --assert-watch 424:cv:12:eq:2
  --assert-watch 424:cv:14:eq:2
  --assert-text "424:FOE SOOT WISP"
  # turns 31-32: the wisp flit law holds at any seed — odd turn 31 is
  # bite-free (hp 7 flat), even turn 32 costs -2 (two adjacent wisps,
  # one bite each)
  --assert-watch 484:cv:5:eq:7
  --assert-watch 484:cv:10:eq:31
  --assert-watch 490:cv:5:eq:5
  --assert-watch 490:cv:10:eq:32
  # turn 38: SLAIN — and the death card (endless runs end only here:
  # the score-attack card) NAMES the seed, so score 220 is attributable
  # to vault C1DF5FEE
  --assert-watch 526:cv:2:eq:3
  --assert-watch 526:cv:13:eq:2
  --assert-watch 526:cv:9:eq:220
  --assert-watch 526:cv:10:eq:38
  --assert-watch 526:cv:7:eq:4
  --assert-watch 526:cv:8:eq:2
  --assert-watch 526:cv:3:eq:3252641774
  --assert-text "526:SLAIN IN THE DEEP"
  --assert-text "526:SCORE 220"
  --assert-text "526:SEED C1DF5FEE"
)

echo "== P6: THE DIALED VAULT — seed-select score-attack, same laws, different world (run 1) =="
H "$OUT/p6.png" --frames 560 $W $P6_ROUTE \
  --watch-log "$OUT/p6-run1.csv" \
  --shot "200:$OUT/p6-title.png" --shot "250:$OUT/p6-floor1.png" \
  --require-distinct \
  "${P6_ASSERTS[@]}"

echo "== P6: run 2 (must be byte-identical) =="
H "$OUT/p6b.png" --frames 560 $W $P6_ROUTE \
  --watch-log "$OUT/p6-run2.csv" \
  "${P6_ASSERTS[@]}"
cmp "$OUT/p6-run1.csv" "$OUT/p6-run2.csv"
echo "P6 run-twice: byte-identical"

# P7: dial away, dial back, and the ENTIRE default run must reproduce —
# every P2 literal (spawn, lantern pickup turn, half-burn arithmetic,
# gutter-out, end HUD) on the same frames. One run: P2 already proved
# this route's run-twice identity; this proves the dial can't leave a
# mark (the Deepcast P5 precedent).
P7_ROUTE=$DIAL_UP' '$DIAL_BACK' '$LANTERN_ROUTE

echo "== P7: dial away and back — the default vault reproduces on every P2 literal =="
H "$OUT/p7.png" --frames 500 $W $P7_ROUTE \
  --assert-watch 165:cv:3:eq:3252641774 \
  --assert-watch 171:cv:3:eq:3252576238 \
  --assert-watch 175:cv:3:eq:3252575982 \
  --assert-watch 200:cv:3:eq:3252575981 \
  --assert-text "200:SEED C1DE5EED" \
  "${P2_ASSERTS[@]}"

# ---------------------------------------------------------------------------
# P8/P9 — growth cut 5, the art pass (tile sprites + torch-radius light
# fade; presentation ONLY — P1-P7 above carried verbatim). The proof reads
# the art back off the hardware:
#   cv_light (ELF symbol, 4 words): 'LITE' magic · radius px · center x ·
#     center y — the law is radius = min(16 + torch/2, 200), center = the
#     player's tile center in screen pixels (x = 8*px + 72, y = 8*py + 10).
#   0x04000000 DISPCNT: 0xF040 with no dungeon on screen, 0xF840 playing
#     (bit 11 = the dungeon bg enabled).
#   0x050001EC BG palette RAM: 0x1A5F4A33 = ember-bright (248,144,48) +
#     wall-lit-edge (152,136,144) as BGR555 — the committed cv_tiles art.
#   0x050003E0 OBJ palette RAM: 0x5FBF7C1F = torch-bearer face/flame-core
#     (248,232,184) + transparent-magenta slot — the committed cv_player.
#   0x06000800 bg map screenblock: 0xF002F002 = two wall cells (tile 2,
#     palette bank 15) — the dungeon's top wall row, baked into VRAM.
# ---------------------------------------------------------------------------

LIGHT_W='--watch lw:cv_light:4 --watch io:0x04000000:1 --watch bgpal:0x050001EC:1 --watch objpal:0x050003E0:1 --watch vmap:0x06000800:1'

P8_LIGHT_ASSERTS=(
  # title: no dungeon, full-screen light, center = display center
  --assert-watch 100:lw:0:eq:0x4C495445
  --assert-watch 100:lw:1:eq:200
  --assert-watch 100:lw:2:eq:120
  --assert-watch 100:lw:3:eq:80
  --assert-watch 100:io:0:eq:0xF040
  # entering the vault: the dungeon bg's DISPCNT bit is ON, the art's
  # exact colors sit in palette RAM, the wall row sits in the map's VRAM
  # screenblock — and the light snaps to the law: torch 220 -> 126 px,
  # centered on the P2-pinned spawn (6,1) -> screen (120, 18)
  --assert-watch 250:io:0:eq:0xF840
  --assert-watch 250:bgpal:0:eq:0x1A5F4A33
  --assert-watch 250:objpal:0:eq:0x5FBF7C1F
  --assert-watch 250:vmap:0:eq:0xF002F002
  --assert-watch 250:lw:1:eq:126
  --assert-watch 250:lw:2:eq:120
  --assert-watch 250:lw:3:eq:18
  # the shrink, on P2's own carried torch pins (turns 7/17/27/32): every
  # step is min(16 + torch/2, 200) and every step is SMALLER
  --assert-watch 340:lw:1:eq:122
  --assert-watch 400:lw:1:eq:120
  --assert-watch 460:lw:1:eq:117
  --assert-watch 490:lw:1:eq:115
)

echo "== P8a: THE TORCHLIGHT — the P2 route with the hardware watched (run 1) =="
H "$OUT/p8.png" --frames 500 $W $LIGHT_W $LANTERN_ROUTE \
  --watch-log "$OUT/p8-run1.csv" \
  --shot "250:$OUT/p8-floor1.png" \
  --require-distinct \
  "${P8_LIGHT_ASSERTS[@]}" \
  "${P2_ASSERTS[@]}"

echo "== P8a: run 2 (must be byte-identical) =="
H "$OUT/p8b.png" --frames 500 $W $LIGHT_W $LANTERN_ROUTE \
  --watch-log "$OUT/p8-run2.csv" \
  "${P8_LIGHT_ASSERTS[@]}" \
  "${P2_ASSERTS[@]}"
cmp "$OUT/p8-run1.csv" "$OUT/p8-run2.csv"
echo "P8a run-twice: byte-identical"

# Leg B — THE DEEP BURN: the P2 opening (both rats dead by turn 5), then
# ~160 waited turns. No embers on the path, so the torch runs down and
# the light closes in: this is "the screen literally darkens as the torch
# burns down" (CONCEPT.md), pinned at two depths and photographed.
BURN_ROUTE='--keys 240-242:START --keys-pattern 300-314:6:2:DOWN --keys 318-320:RIGHT --keys 324-326:DOWN --keys-pattern 330-344:6:2:RIGHT --keys-pattern 348-1300:6:2:A'

P8_BURN_ASSERTS=(
  # mid-burn: torch 163 -> radius 97 (16 + 163/2)
  --assert-watch 700:cv:6:eq:163
  --assert-watch 700:lw:1:eq:97
  # deep burn: torch 64 -> radius 48, run still alive and untouched (the
  # floor was cleared at turn 5; waiting is safe — only the light moved),
  # light still centered on the resting tile (9,4) -> screen (144, 42)
  --assert-watch 1290:cv:2:eq:1
  --assert-watch 1290:cv:5:eq:10
  --assert-watch 1290:cv:6:eq:64
  --assert-watch 1290:lw:1:eq:48
  --assert-watch 1290:lw:2:eq:144
  --assert-watch 1290:lw:3:eq:42
  --assert-watch 1290:io:0:eq:0xF840
)

echo "== P8b: THE DEEP BURN — torch 220 -> 64, radius 126 -> 48 (run 1) =="
H "$OUT/p8c.png" --frames 1310 $W $LIGHT_W $BURN_ROUTE \
  --watch-log "$OUT/p8-burn1.csv" \
  --shot "700:$OUT/p8-burn-mid.png" --shot "1290:$OUT/p8-burn-low.png" \
  "${P8_BURN_ASSERTS[@]}"

echo "== P8b: run 2 (must be byte-identical) =="
H "$OUT/p8d.png" --frames 1310 $W $LIGHT_W $BURN_ROUTE \
  --watch-log "$OUT/p8-burn2.csv" \
  "${P8_BURN_ASSERTS[@]}"
cmp "$OUT/p8-burn1.csv" "$OUT/p8-burn2.csv"
echo "P8b run-twice: byte-identical"

# P9 — THE EMBER RELIGHT: the P6 dialed-vault route, every P6 pin carried,
# plus the light law's other direction: embers feed the torch, so the
# circle RE-OPENS (126 -> 155 px through the turn-16 three-ember sweep),
# and the fade only lives in the dungeon (200 px on the dialed title AND
# on the death card).
P9_ASSERTS=(
  --assert-watch 200:lw:1:eq:200
  --assert-watch 250:lw:1:eq:126
  # turn 16: torch 279 (P6's own pin) -> radius 155: the relight
  --assert-watch 394:lw:1:eq:155
  # SLAIN: the death card lifts the fade (full radius, no dungeon bg)
  --assert-watch 526:lw:1:eq:200
  --assert-watch 526:io:0:eq:0xF040
)

echo "== P9: THE EMBER RELIGHT — the dialed vault with the light watched (run 1) =="
H "$OUT/p9.png" --frames 560 $W $LIGHT_W $P6_ROUTE \
  --watch-log "$OUT/p9-run1.csv" \
  --shot "394:$OUT/p9-relight.png" \
  --require-distinct \
  "${P9_ASSERTS[@]}" \
  "${P6_ASSERTS[@]}"

echo "== P9: run 2 (must be byte-identical) =="
H "$OUT/p9b.png" --frames 560 $W $LIGHT_W $P6_ROUTE \
  --watch-log "$OUT/p9-run2.csv" \
  "${P9_ASSERTS[@]}" \
  "${P6_ASSERTS[@]}"
cmp "$OUT/p9-run1.csv" "$OUT/p9-run2.csv"
echo "P9 run-twice: byte-identical"

echo "ALL CINDERVAULT PROOFS PASS"
