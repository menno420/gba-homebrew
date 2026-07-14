#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# CLOCKWORK COURIER — the committed headless proof suite (mGBA,
# tools/headless-screenshot.py). The Shoal proofs.sh pattern: the input
# scripts are part of the pin — "carried byte-for-byte" stays a
# re-runnable command, never an archaeology project (the prototype's
# session-44 scripts died with their container; rung 1 re-derived them
# against the byte-verified v0.1 dist and committed them here, and the
# re-derivation walked straight into the same literals the #96 card
# documented: door clamp 23808, parcel (3008,6144), switch 6208).
#
# Run from the repo root after building (make -C games/clockwork-courier):
#     bash games/clockwork-courier/proofs.sh
# Artifacts land in $CC_PROOF_OUT (default /tmp/courier-proofs).
#
# The eight proofs:
#   P1 boot/title           — magics, title state, every title line (incl.
#                             rung 1's "AND YOU CAN STAND ON IT").
#   P2 kinematics + refusal — walk clamp at the closed door (23808),
#                             max-jump apex pinned (9922 = feet 38.76 px,
#                             THE number every reachability claim rests
#                             on), door refuses grounded AND mid-air.
#   P3 the rewind contract  — tape fills to exactly 300; R snaps the
#                             player to the tape start and the ghost
#                             replays the walk to the SAME literal the
#                             player recorded 301 frames earlier; the
#                             ghost holds switch+door from afar; the five
#                             seconds spend themselves and the door shuts.
#   P4 the delivery         — the co-op solve: step (40) -> platform (24)
#                             -> parcel (3008,6144) -> switch (6208) ->
#                             rewind -> the ghost holds the door -> chute:
#                             state 2 at rf 599, card text exact, START
#                             restarts. RUN TWICE — byte-identical.
#   P5 GHOST-AS-PLATFORM    — rung 1's mechanic used in a full solve: the
#                             delivery detours inside the chute corridor,
#                             a second rewind parks a ghost there, the
#                             courier lands on its HEAD (one-way top
#                             collision, on_ghost word pinned), boosts to
#                             the BELL LEDGE (feet 8192 = 32 px — a height
#                             P2's apex proves unreachable from ground),
#                             then delivers with REWINDS 2. RUN TWICE —
#                             byte-identical.
#   P7 THE RUSH ORDER       — rung 2 ("multiple parcels/chutes with
#                             timing windows"): SELECT re-runs P4's exact
#                             route with a second parcel waiting on the
#                             step and a chute that keeps hours (open 60
#                             frames in every 240, pure run_frames
#                             arithmetic). Both parcels are picked
#                             (carried word 2), the courier stands at the
#                             chute through the SHUT window for 100+
#                             pinned frames delivering NOTHING (the
#                             refusal: missing the window is a wait, not
#                             a shortcut), then rf 720 — the first open
#                             frame — delivers both at once. RUN TWICE —
#                             byte-identical. START from the rush card
#                             restarts the CLASSIC run (mode word 0).
#   P6 the counter-solve    — the strongest scripted cheats FAIL: a
#                             max-jump+drift at the ledge peaks at 9922
#                             (38.76 px, short of 32 px) with the flank
#                             clamping x at 27648; jumping under the ledge
#                             bumps at 12408; the door-side hop bumps at
#                             14452; the ledge is never stood on and the
#                             run never ends. A puzzle proven only along
#                             its solution is proven solvable, not proven
#                             to be a puzzle.
#
# RUNG-4 AUDIO + CLOCK VERDICT: enabling maxmod did NOT move any clock
# on this engine (full-row cross-ROM diff vs the v0.4 dist: 0 differing
# rows over 700 frames at the SAME frame indices — Courier's transition
# frames are light, unlike Shoal's 50-sprite spawn spike). Every carried
# literal stands verbatim; the audio asserts are pure additions.
#
# CROSS-ROM CARRY (rung 2): the classic run lives on START and the rush
# order on SELECT, so P1-P6 carry by construction — re-proven by
# replaying P4's script on the v0.2 dist (sha256 689bc792…6f00) and the
# rung-2 build: 0 differing rows over 700 frames, all 17 rung-1 words.
#
# CROSS-ROM CARRY (rung 1): P2/P3/P4's scripts replay on the v0.1 dist
# (sha256 ed877798…40a8) and this build with IDENTICAL telemetry — 0
# differing rows over 1800 frames, all 16 words, no clock shift — because
# the new geometry ((12,1)-(12,3) wall, (14,4) ledge) sits where no
# carried trajectory flies. Zero game-state re-pins.
# ---------------------------------------------------------------------------
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."

ROM=games/clockwork-courier/clockwork-courier.gba
OUT="${CC_PROOF_OUT:-/tmp/courier-proofs}"
mkdir -p "$OUT"

H() { python3 tools/headless-screenshot.py "$ROM" "$@"; }
W='--elf games/clockwork-courier/clockwork-courier.elf --watch t:cc_telemetry:21'
# Rung 4 audio evidence (docs/capabilities.md): cumulative decision
# counters + the maxmod mixer-memory activity word (count of nonzero
# u32 words — 0 exactly when the mixer is silent; one voice ~ 264).
WA='--watch h:gl_audio_hook:8 --watch-nonzero mix:maxmod_mixing_buffer:0x420'

echo "== P1: boot + title =="
H "$OUT/p1.png" --frames 70 $W $WA --keys 30-32:B \
  --assert-watch 60:h:6:eq:1 \
  --assert-watch 60:mix:0:eq:0 \
  --assert-watch 60:t:0:eq:1129073495 \
  --assert-watch 60:t:1:eq:1129272658 \
  --assert-watch 60:t:2:eq:0 \
  --assert-text "60:CLOCKWORK COURIER" \
  --assert-text "60:R REWINDS YOU 5 SECONDS" \
  --assert-text "60:YOUR GHOST REPLAYS YOU" \
  --assert-text "60:IT CAN HOLD THE SWITCH" \
  --assert-text "60:PRESS START" \
  --assert-text "60:AND YOU CAN STAND ON IT"

echo "== P2: kinematics + the door refuses =="
H "$OUT/p2.png" --frames 400 $W \
  --keys 10-12:START --keys 30-32:A --keys 120-260:RIGHT --keys 262-264:A \
  --assert-watch 20:t:4:eq:17408 \
  --assert-watch 47:t:5:eq:9922 \
  --assert-watch 47:t:6:eq:0 \
  --assert-watch 100:t:5:eq:16384 \
  --assert-watch 260:t:4:eq:23808 \
  --assert-watch 260:t:5:eq:16384 \
  --assert-watch 260:t:11:eq:0 \
  --assert-watch 260:t:12:eq:0 \
  --assert-watch 279:t:4:eq:23808 \
  --assert-watch 279:t:5:eq:9922 \
  --assert-watch 350:t:4:eq:23808 \
  --assert-watch 350:t:2:eq:1 \
  --assert-watch 350:t:12:eq:0

echo "== P3: the rewind contract =="
H "$OUT/p3.png" --frames 700 $W \
  --keys 10-12:START --keys 20-52:LEFT --keys 330-332:R --keys 340-366:RIGHT \
  --assert-watch 39:t:4:eq:11008 \
  --assert-watch 52:t:4:eq:7168 \
  --assert-watch 52:t:11:eq:1 \
  --assert-watch 52:t:12:eq:1 \
  --assert-watch 315:t:7:eq:300 \
  --assert-watch 331:t:4:eq:13888 \
  --assert-watch 331:t:8:eq:1 \
  --assert-watch 331:t:9:eq:13888 \
  --assert-watch 331:t:14:eq:1 \
  --assert-watch 340:t:9:eq:11008 \
  --assert-watch 355:t:9:eq:7168 \
  --assert-watch 355:t:11:eq:1 \
  --assert-watch 355:t:12:eq:1 \
  --assert-watch 500:t:8:eq:1 \
  --assert-watch 500:t:12:eq:1 \
  --assert-watch 634:t:8:eq:0 \
  --assert-watch 634:t:11:eq:0 \
  --assert-watch 634:t:12:eq:0 \
  --assert-watch 634:t:4:eq:22208

DELIVERY_ROUTE='--keys 10-12:START --keys 20-26:LEFT --keys 28-30:A --keys 34-46:LEFT --keys 70-72:A --keys 70-96:LEFT --keys 110-140:LEFT --keys 160-184:RIGHT --keys 196-210:LEFT --keys 530-532:R --keys 540-614:RIGHT --keys 660-662:START'

P4_ASSERTS=(
  --assert-watch 60:h:0:eq:1
  --assert-watch 150:h:1:eq:1
  --assert-watch 540:h:2:eq:1
  --assert-watch 540:h:3:eq:1
  --assert-watch 540:mix:0:gt:0
  --assert-watch 620:h:4:eq:1
  --assert-watch 620:mix:0:gt:0
  --assert-watch 650:mix:0:eq:0
  --assert-watch 680:h:0:eq:2
  --assert-watch 58:t:4:eq:14208
  --assert-watch 58:t:5:eq:10240
  --assert-watch 100:t:4:eq:5888
  --assert-watch 100:t:5:eq:6144
  --assert-watch 142:t:4:eq:3008
  --assert-watch 142:t:5:eq:6144
  --assert-watch 142:t:13:eq:1
  --assert-watch 220:t:4:eq:6208
  --assert-watch 220:t:11:eq:1
  --assert-watch 531:t:14:eq:1
  --assert-watch 531:t:4:eq:6208
  --assert-watch 531:t:8:eq:1
  --assert-watch 531:t:9:eq:6208
  --assert-watch 600:t:4:eq:25728
  --assert-watch 600:t:12:eq:1
  --assert-watch 612:t:2:eq:2
  --assert-watch 612:t:15:eq:599
  --assert-text "640:PARCEL DELIVERED"
  --assert-text "640:CLOCK 9s (599 FRAMES)"
  --assert-text "640:REWINDS 1"
  --assert-text "640:THE TOWER TICKS ON"
  --assert-watch 680:t:2:eq:1
  --assert-watch 680:t:14:eq:0
)

echo "== P4: the delivery — the co-op solve (run 1) =="
H "$OUT/p4.png" --frames 700 $W $WA $DELIVERY_ROUTE \
  --watch-log "$OUT/p4-run1.csv" --shot "640:$OUT/p4-card.png" \
  "${P4_ASSERTS[@]}"

echo "== P4: run 2 (must be byte-identical) =="
H "$OUT/p4b.png" --frames 700 $W $WA $DELIVERY_ROUTE \
  --watch-log "$OUT/p4-run2.csv" \
  "${P4_ASSERTS[@]}"
cmp "$OUT/p4-run1.csv" "$OUT/p4-run2.csv"
echo "P4 run-twice: byte-identical"

# Ghost-as-platform: the same delivery until the door, then the courier
# parks INSIDE the corridor (108.0 — one pixel-step clear of the closed
# door's never-crush cells), refills the tape standing there, rewinds a
# second ghost into existence under the BELL LEDGE, climbs onto its head
# (one-way top collision), boosts onto the ledge (feet 32 px — P2's apex
# 38.76 px proves no unboosted jump lands there), steps off across the
# ghost's head again, and delivers. Every span before frame 1066 is
# load-bearing.
PLATFORM_ROUTE='--keys 10-12:START --keys 20-26:LEFT --keys 28-30:A --keys 34-46:LEFT --keys 70-72:A --keys 70-96:LEFT --keys 110-140:LEFT --keys 160-184:RIGHT --keys 196-210:LEFT --keys 530-532:R --keys 540-607:RIGHT --keys 920-922:R --keys 930-932:A --keys 980-982:A --keys 992-997:RIGHT --keys 1040-1046:LEFT --keys 1060-1074:RIGHT'

P5_ASSERTS=(
  --assert-watch 610:t:4:eq:27648
  --assert-watch 610:t:13:eq:1
  --assert-watch 921:t:14:eq:2
  --assert-watch 921:t:8:eq:1
  --assert-watch 921:t:9:eq:27648
  --assert-watch 963:t:16:eq:1
  --assert-watch 963:t:5:eq:14336
  --assert-watch 963:t:6:eq:1
  --assert-watch 1002:t:5:eq:8192
  --assert-watch 1002:t:6:eq:1
  --assert-watch 1002:t:4:eq:28288
  --assert-watch 1002:t:16:eq:0
  --assert-watch 1030:t:5:eq:8192
  --assert-watch 1030:t:6:eq:1
  --assert-watch 1060:t:16:eq:1
  --assert-watch 1066:t:2:eq:2
  --assert-watch 1066:t:15:eq:1054
  --assert-watch 1066:t:14:eq:2
  --assert-text "1100:PARCEL DELIVERED"
  --assert-text "1100:CLOCK 17s (1054 FRAMES)"
  --assert-text "1100:REWINDS 2"
)

echo "== P5: GHOST-AS-PLATFORM — stand on your past self (run 1) =="
H "$OUT/p5.png" --frames 1150 $W $PLATFORM_ROUTE \
  --watch-log "$OUT/p5-run1.csv" --shot "1015:$OUT/p5-ledge.png" \
  "${P5_ASSERTS[@]}"

echo "== P5: run 2 (must be byte-identical) =="
H "$OUT/p5b.png" --frames 1150 $W $PLATFORM_ROUTE \
  --watch-log "$OUT/p5-run2.csv" \
  "${P5_ASSERTS[@]}"
cmp "$OUT/p5-run1.csv" "$OUT/p5-run2.csv"
echo "P5 run-twice: byte-identical"

echo "== P6: the counter-solve fails — the ledge needs the ghost =="
H "$OUT/p6.png" --frames 900 $W \
  --keys 10-12:START --keys 20-26:LEFT --keys 28-30:A --keys 34-46:LEFT \
  --keys 70-72:A --keys 70-96:LEFT --keys 110-140:LEFT --keys 160-184:RIGHT \
  --keys 196-210:LEFT --keys 530-532:R --keys 540-607:RIGHT \
  --keys 700-702:A --keys 710-724:RIGHT \
  --keys 760-762:RIGHT --keys 770-772:A \
  --keys 820-824:LEFT --keys 830-832:A \
  --assert-watch 717:t:5:eq:9922 \
  --assert-watch 717:t:4:eq:27648 \
  --assert-watch 717:t:6:eq:0 \
  --assert-watch 750:t:5:eq:16384 \
  --assert-watch 776:t:5:eq:12408 \
  --assert-watch 815:t:5:eq:16384 \
  --assert-watch 832:t:5:eq:14452 \
  --assert-watch 880:t:5:eq:16384 \
  --assert-watch 880:t:2:eq:1 \
  --assert-watch 880:t:16:eq:0


RUSH_ROUTE='--keys 10-12:SELECT --keys 20-26:LEFT --keys 28-30:A --keys 34-46:LEFT --keys 70-72:A --keys 70-96:LEFT --keys 110-140:LEFT --keys 160-184:RIGHT --keys 196-210:LEFT --keys 530-532:R --keys 540-614:RIGHT --keys 800-802:START'

P7_ASSERTS=(
  --assert-watch 60:h:1:eq:1
  --assert-watch 150:h:1:eq:2
  --assert-watch 260:h:3:eq:1
  --assert-watch 260:h:5:eq:1
  --assert-watch 540:h:2:eq:1
  --assert-watch 540:h:5:eq:2
  --assert-watch 540:mix:0:gt:0
  --assert-watch 735:h:4:eq:1
  --assert-watch 735:h:5:eq:3
  --assert-watch 735:mix:0:gt:0
  --assert-watch 830:h:0:eq:2
  --assert-watch 830:mix:0:eq:0
  --assert-watch 20:t:17:eq:1
  --assert-watch 20:t:19:eq:1
  --assert-watch 75:t:19:eq:0
  --assert-watch 58:t:13:eq:1
  --assert-watch 58:t:4:eq:14208
  --assert-watch 58:t:5:eq:10240
  --assert-watch 142:t:13:eq:2
  --assert-watch 142:t:4:eq:3008
  --assert-watch 220:t:4:eq:6208
  --assert-watch 531:t:14:eq:1
  --assert-watch 640:t:4:eq:29888
  --assert-watch 640:t:19:eq:0
  --assert-watch 640:t:18:eq:0
  --assert-watch 640:t:2:eq:1
  --assert-watch 700:t:19:eq:0
  --assert-watch 700:t:18:eq:0
  --assert-watch 700:t:2:eq:1
  --assert-watch 700:t:13:eq:2
  --assert-watch 725:t:18:eq:0
  --assert-watch 725:t:2:eq:1
  --assert-text "700:GHOST OUT 2 HELD SHUT"
  --assert-watch 735:t:2:eq:2
  --assert-watch 735:t:18:eq:2
  --assert-watch 735:t:15:eq:720
  --assert-watch 735:t:14:eq:1
  --assert-watch 735:t:13:eq:0
  --assert-text "760:PARCEL DELIVERED"
  --assert-text "760:CLOCK 12s (720 FRAMES)"
  --assert-text "760:REWINDS 1"
  --assert-text "760:RUSH ORDER: 2 PARCELS"
  --assert-watch 830:t:2:eq:1
  --assert-watch 830:t:17:eq:0
)

echo "== P7: THE RUSH ORDER — catch the window, or wait for it (run 1) =="
H "$OUT/p7.png" --frames 850 $W $WA $RUSH_ROUTE \
  --watch-log "$OUT/p7-run1.csv" --shot "700:$OUT/p7-shut.png" \
  "${P7_ASSERTS[@]}"

echo "== P7: run 2 (must be byte-identical) =="
H "$OUT/p7b.png" --frames 850 $W $WA $RUSH_ROUTE \
  --watch-log "$OUT/p7-run2.csv" \
  "${P7_ASSERTS[@]}"
cmp "$OUT/p7-run1.csv" "$OUT/p7-run2.csv"
echo "P7 run-twice: byte-identical"

# The tower chain: every span before frame 2146 is load-bearing (LV1
# probes + boost solve -> L -> LV2 platform/door-thread/window ->
# L -> LV3 probe + boost + switch ghost + window -> L wraps to LV1).
TOWER_ROUTE='--keys 10-12:L --keys 20-22:A --keys 60-62:A --keys 64-74:LEFT --keys 420-422:R --keys 430-432:A --keys 480-482:A --keys 490-497:LEFT --keys 510-514:LEFT --keys 540-560:RIGHT --keys 580-645:RIGHT --keys 660-662:L --keys 672-710:LEFT --keys 714-716:A --keys 718-728:LEFT --keys 745-762:RIGHT --keys 1010-1012:R --keys 1020-1096:RIGHT --keys 1180-1182:L --keys 1191-1221:LEFT --keys 1225-1227:A --keys 1501-1503:R --keys 1511-1529:LEFT --keys 1541-1543:A --keys 1591-1593:A --keys 1601-1608:LEFT --keys 1621-1625:LEFT --keys 1641-1657:RIGHT --keys 1821-1823:R --keys 1831-1989:RIGHT --keys 2171-2173:L'

P8_ASSERTS=(
  --assert-watch 60:h:0:eq:1
  --assert-watch 520:h:1:eq:1
  --assert-watch 520:h:2:eq:1
  --assert-watch 620:h:4:eq:1
  --assert-watch 620:h:3:eq:0
  --assert-watch 620:mix:0:gt:0
  --assert-watch 655:mix:0:eq:0
  --assert-watch 1080:h:0:eq:2
  --assert-watch 1080:h:1:eq:2
  --assert-watch 1080:h:2:eq:2
  --assert-watch 1080:h:3:eq:3
  --assert-watch 1080:h:5:eq:1
  --assert-watch 1150:h:4:eq:2
  --assert-watch 1150:h:5:eq:2
  --assert-watch 1201:h:0:eq:3
  --assert-watch 1830:h:2:eq:4
  --assert-watch 1830:h:3:eq:7
  --assert-watch 2146:h:4:eq:3
  --assert-watch 2146:h:5:eq:6
  --assert-watch 2146:h:3:eq:8
  --assert-watch 2200:h:0:eq:4
  --assert-watch 2200:mix:0:eq:0
  --assert-watch 2290:h:6:eq:0
  --assert-watch 20:t:17:eq:2
  --assert-watch 20:t:20:eq:0
  --assert-watch 37:t:5:eq:9922
  --assert-watch 77:t:5:eq:9922
  --assert-watch 77:t:4:eq:15168
  --assert-watch 470:t:16:eq:1
  --assert-watch 470:t:5:eq:14336
  --assert-watch 505:t:5:eq:8192
  --assert-watch 505:t:6:eq:1
  --assert-watch 505:t:4:eq:14528
  --assert-watch 505:t:13:eq:0
  --assert-watch 512:t:13:eq:1
  --assert-watch 620:t:2:eq:2
  --assert-watch 620:t:15:eq:597
  --assert-watch 620:t:14:eq:1
  --assert-text "640:PARCEL DELIVERED"
  --assert-text "640:CLOCK 9s (597 FRAMES)"
  --assert-text "640:L1 THE HIGH SHELF"
  --assert-text "640:L: NEXT SHIFT"
  --assert-watch 670:t:20:eq:1
  --assert-watch 670:t:17:eq:2
  --assert-watch 744:t:13:eq:1
  --assert-watch 744:t:4:eq:3008
  --assert-watch 1061:t:12:eq:1
  --assert-watch 1100:t:4:eq:28608
  --assert-watch 1100:t:19:eq:0
  --assert-watch 1100:t:13:eq:1
  --assert-watch 1100:t:2:eq:1
  --assert-watch 1130:t:19:eq:0
  --assert-watch 1130:t:2:eq:1
  --assert-watch 1150:t:2:eq:2
  --assert-watch 1150:t:15:eq:480
  --assert-watch 1150:t:18:eq:1
  --assert-text "1170:CLOCK 8s (480 FRAMES)"
  --assert-text "1170:L2 THE NIGHT SHIFT"
  --assert-watch 1201:t:20:eq:2
  --assert-watch 1242:t:5:eq:9922
  --assert-watch 1242:t:4:eq:7808
  --assert-watch 1581:t:16:eq:1
  --assert-watch 1581:t:5:eq:14336
  --assert-watch 1622:t:13:eq:1
  --assert-watch 1622:t:5:eq:8192
  --assert-watch 1622:t:4:eq:5888
  --assert-watch 1720:t:4:eq:10368
  --assert-watch 1720:t:12:eq:1
  --assert-watch 1830:t:14:eq:2
  --assert-watch 2100:t:19:eq:0
  --assert-watch 2100:t:13:eq:1
  --assert-watch 2100:t:4:eq:27328
  --assert-watch 2100:t:2:eq:1
  --assert-watch 2146:t:2:eq:2
  --assert-watch 2146:t:15:eq:960
  --assert-watch 2146:t:14:eq:2
  --assert-text "2160:CLOCK 16s (960 FRAMES)"
  --assert-text "2160:L3 THE FULL ROUND"
  --assert-watch 2200:t:20:eq:0
  --assert-watch 2200:t:17:eq:2
  --assert-watch 2200:t:2:eq:1
)

echo "== P8: THE TOWER SHIFTS — three floors, one chain (run 1) =="
H "$OUT/p8.png" --frames 2300 $W $WA $TOWER_ROUTE \
  --watch-log "$OUT/p8-run1.csv" --shot "2160:$OUT/p8-lv3-card.png" \
  "${P8_ASSERTS[@]}"

echo "== P8: run 2 (must be byte-identical) =="
H "$OUT/p8b.png" --frames 2300 $W $WA $TOWER_ROUTE \
  --watch-log "$OUT/p8-run2.csv" \
  "${P8_ASSERTS[@]}"
cmp "$OUT/p8-run1.csv" "$OUT/p8-run2.csv"
echo "P8 run-twice: byte-identical"

echo "ALL COURIER PROOFS PASS"
