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
# Run from the repo root after building (make -C games/cindervault):
#     bash games/cindervault/proofs.sh
# Artifacts land in $CV_PROOF_OUT (default /tmp/cindervault-proofs).
#
# The five proofs (asserts inline below):
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
  --assert-text "100:PRESS START"

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

echo "ALL CINDERVAULT PROOFS PASS"
