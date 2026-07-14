#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# CINDERVAULT — the committed headless proof suite (mGBA,
# tools/headless-screenshot.py), ported with the item-layer slice (growth
# cut 1) and extended with the species slice (growth cut 2). The
# prototype's proofs (.sessions/2026-07-13-cindervault.md) lived only in
# that session's container; per the Shoal/Courier convention the input
# scripts ARE the pin, so from now on they are committed here.
#
# Pin provenance (the prototype card's 💡 recipe, now with the mirror
# COMMITTED): every route below was derived by the route bot in
# games/cindervault/tools/mirror.py and every literal transcribed from the
# mirror's per-turn trace — after the mirror was certified against a real
# emulator watch-log of the SAME routes turn-for-turn (0 mismatches across
# all 185 turn states of the three routes, this build). Nothing is
# hand-computed.
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
# Run from the repo root after building (make -C games/cindervault):
#     bash games/cindervault/proofs.sh
# Artifacts land in $CV_PROOF_OUT (default /tmp/cindervault-proofs).
#
# The three proofs (asserts inline below):
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

echo "ALL CINDERVAULT PROOFS PASS"
