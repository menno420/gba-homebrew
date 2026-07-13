#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# SHOAL — the committed headless proof suite (mGBA, tools/headless-screenshot.py).
#
# NEW with the gates slice (rung 2): earlier Shoal proofs lived only in the
# session container and were LOST with it — this session had to reconstruct
# the carried-pin story via a cross-ROM equivalence diff against the merged
# dist/shoal.gba (see .sessions/2026-07-13-shoal-gates.md). The input scripts
# are part of the pin: from now on they are committed here, so "carried
# byte-for-byte" is a re-runnable command, not an archaeology project.
#
# Run from the repo root after building (tools/build.sh or make -C games/shoal):
#     bash games/shoal/proofs.sh
# Artifacts land in $SHOAL_PROOF_OUT (default /tmp/shoal-proofs).
#
# The five proofs (asserts inline below):
#   P1 boot/title            — magics, title state, all six title lines incl.
#                              every mode verb; title CPU baseline rail.
#   P2 calm idle 1300f       — THE GATE carried: school positions/centroid/
#                              counts pinned exact (game-state words identical
#                              to the pre-gates ROM — verified cross-ROM at
#                              rung 2); CPU exact + < 4096 rail; mode 0.
#   P3 hungry idle 1700f     — predator pass carried: the opening scramble
#                              (eaten 2 by rf~300, 8 by rf~1400) then the
#                              slow starve (10 by rf~1690); dens exact;
#                              CPU exact + rail; calm words untouched.
#   P4 THE GATED RUN (win)   — the committed funnel route: compress the
#                              school into the high lane, pump it through
#                              gap 0, snake it down through gap 1, bank
#                              40/50 at rf 3519; wall-block evidence word;
#                              budget exact + rail over gate-active frames;
#                              win card text exact; START restarts CALM.
#                              RUN TWICE — the two watch-logs must be
#                              byte-identical (determinism contract).
#   P5 gates are geometry    — the same no-input run that banks 5 fish in
#                              calm water banks ZERO in gated water: the
#                              walls, and only the walls, hold the school.
#   P6 a graded loss (rung 3) — the ratings rung's other half: a run that
#                              banks 6 then feeds the remnant to the
#                              hunters SCATTERS at rf 3420 and the card
#                              grades it honestly ("RATING -"); with P4's
#                              live HUD stars (* at 25, ** at 30) and its
#                              "RATING ***" win card, all four grades
#                              (- / * / ** / ***) are pinned on screen.
#                              RUN TWICE — byte-identical watch-logs.
#   P7 the tuned levels (rung 4) — ONE deterministic chain plays all four
#                              levels back to back on the L verb: L1 THE
#                              SHALLOWS (1 gate, no hunters, goal 40) ->
#                              L2 THE HUNT (one 44px/300 hunter, goal 36)
#                              -> L3 THE NARROWS (2 gates + one 52px/360
#                              hunter, goal 32) -> L4 DEEP WATER (2 gates
#                              + two 52px/420 hunters, goal 28), each won
#                              by its own derived route, then the L-wrap
#                              back to L1. RUN TWICE — byte-identical, so
#                              every level's win route is proven twice.
#
# CPU-word pins (t[4]/t[5] exact literals) are PER-BUILD measurements — any
# code touch shifts them ~±15 data units. Re-derive them once per slice and
# keep the durable `< 4096` rails verbatim (the series contract).
# ---------------------------------------------------------------------------
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."

ROM=games/shoal/shoal.gba
ELF=games/shoal/shoal.elf
OUT="${SHOAL_PROOF_OUT:-/tmp/shoal-proofs}"
mkdir -p "$OUT"

H() { python3 tools/headless-screenshot.py "$ROM" "$@"; }
W='--elf games/shoal/shoal.elf --watch t:sh_telemetry:26'

echo "== P1: boot + title =="
H "$OUT/p1.png" --frames 70 $W \
  --assert-watch 60:t:0:eq:1397247809 \
  --assert-watch 60:t:1:eq:1280460102 \
  --assert-watch 60:t:2:eq:0 \
  --assert-watch 60:t:4:lt:500 \
  --assert-text "60:SHOAL" \
  --assert-text "60:YOU ARE A CURRENT" \
  --assert-text "60:HERD 40 FISH TO THE REEF" \
  --assert-text "60:PRESS START" \
  --assert-text "60:SELECT: HUNGRY WATER (35)" \
  --assert-text "60:R: THE GATED RUN (40)" \
  --assert-text "60:STARS = FISH SAVED" \
  --assert-text "60:L: TUNED LEVELS (4)"

echo "== P2: calm idle — THE GATE, carried =="
H "$OUT/p2.png" --frames 1300 $W --keys 10-12:START \
  --assert-watch 400:t:2:eq:1 \
  --assert-watch 400:t:6:eq:49 \
  --assert-watch 400:t:7:eq:1 \
  --assert-watch 400:t:8:eq:91 \
  --assert-watch 400:t:9:eq:59 \
  --assert-watch 400:t:10:eq:30768 \
  --assert-watch 400:t:11:eq:14690 \
  --assert-watch 400:t:15:eq:389 \
  --assert-watch 400:t:16:eq:0 \
  --assert-watch 400:t:17:eq:0 \
  --assert-watch 400:t:4:eq:2106 \
  --assert-watch 400:t:5:eq:2851 \
  --assert-watch 400:t:5:lt:4096 \
  --assert-watch 1199:t:6:eq:47 \
  --assert-watch 1199:t:7:eq:3 \
  --assert-watch 1199:t:8:eq:85 \
  --assert-watch 1199:t:9:eq:57 \
  --assert-watch 1199:t:5:eq:2851 \
  --assert-watch 1199:t:5:lt:4096 \
  --assert-watch 1299:t:7:eq:5 \
  --assert-watch 1299:t:16:eq:0 \
  --assert-watch 1299:t:24:eq:0 \
  --assert-text "1299:SAVED 5/40 -"

echo "== P3: hungry idle — the predator pass, carried =="
H "$OUT/p3.png" --frames 1700 $W --keys 10-12:SELECT \
  --assert-watch 320:t:16:eq:1 \
  --assert-watch 320:t:17:eq:2 \
  --assert-watch 320:t:18:eq:176 \
  --assert-watch 320:t:19:eq:40 \
  --assert-watch 320:t:20:eq:176 \
  --assert-watch 320:t:21:eq:120 \
  --assert-watch 400:t:5:eq:3089 \
  --assert-watch 400:t:5:lt:4096 \
  --assert-watch 1420:t:17:eq:8 \
  --assert-watch 1699:t:17:eq:10 \
  --assert-watch 1699:t:6:eq:37 \
  --assert-watch 1699:t:7:eq:3 \
  --assert-watch 1699:t:5:eq:3089 \
  --assert-watch 1699:t:5:lt:4096 \
  --assert-watch 1699:t:24:eq:0

# The committed gated-run funnel route (win at frame 3531, rf 3519):
# compress the school into the high lane off the bottom rail, pump it
# through gap 0 with left-side vertical sweeps drifting right, shepherd
# the mid-water group down through gap 1, then two more gather cycles
# for the stragglers. Every span is load-bearing up to frame 3531.
GATED_WIN_ROUTE='--keys 10-12:R --keys 12-32:RIGHT --keys 12-42:DOWN --keys 42-352:A --keys 42-72:LEFT --keys 72-110:RIGHT --keys 110-148:LEFT --keys 148-186:RIGHT --keys 186-224:LEFT --keys 224-262:RIGHT --keys 262-300:LEFT --keys 300-338:RIGHT --keys 352-384:LEFT --keys 352-410:UP --keys 410-850:A --keys 410-438:DOWN --keys 438-466:UP --keys 466-494:DOWN --keys 494-522:UP --keys 522-550:DOWN --keys 550-578:UP --keys 578-606:DOWN --keys 606-634:UP --keys 634-662:DOWN --keys 662-690:UP --keys 690-718:DOWN --keys 718-746:UP --keys 746-774:DOWN --keys 774-802:UP --keys 802-830:DOWN --keys-pattern 410-850:8:1:RIGHT --keys 850-880:LEFT --keys 850-881:DOWN --keys 881-1160:A --keys 881-911:LEFT --keys 911-949:RIGHT --keys 949-987:LEFT --keys 987-1025:RIGHT --keys 1025-1063:LEFT --keys 1063-1101:RIGHT --keys 1101-1139:LEFT --keys 1139-1160:RIGHT --keys 1160-1195:LEFT --keys 1160-1218:UP --keys 1225-1665:A --keys 1225-1253:DOWN --keys 1253-1281:UP --keys 1281-1309:DOWN --keys 1309-1337:UP --keys 1337-1365:DOWN --keys 1365-1393:UP --keys 1393-1421:DOWN --keys 1421-1449:UP --keys 1449-1477:DOWN --keys 1477-1505:UP --keys 1505-1533:DOWN --keys 1533-1561:UP --keys 1561-1589:DOWN --keys 1589-1617:UP --keys 1617-1645:DOWN --keys-pattern 1225-1665:8:1:RIGHT --keys 1800-1807:LEFT --keys 1800-1815:DOWN --keys 1815-2100:A --keys 1815-1829:DOWN --keys 1829-1854:UP --keys 1854-1879:DOWN --keys 1879-1904:UP --keys 1904-1929:DOWN --keys 1929-1954:UP --keys 1954-1979:DOWN --keys 1979-2004:UP --keys 2004-2029:DOWN --keys 2029-2054:UP --keys 2054-2079:DOWN --keys 2079-2100:UP --keys-pattern 1815-2100:6:1:RIGHT --keys 2100-2193:LEFT --keys 2100-2122:DOWN --keys 2193-2450:A --keys 2193-2231:RIGHT --keys 2231-2269:LEFT --keys 2269-2307:RIGHT --keys 2307-2345:LEFT --keys 2345-2383:RIGHT --keys 2383-2421:LEFT --keys 2421-2450:RIGHT --keys 2450-2478:LEFT --keys 2450-2510:UP --keys 2515-2955:A --keys 2515-2543:DOWN --keys 2543-2571:UP --keys 2571-2599:DOWN --keys 2599-2627:UP --keys 2627-2655:DOWN --keys 2655-2683:UP --keys 2683-2711:DOWN --keys 2711-2739:UP --keys 2739-2767:DOWN --keys 2767-2795:UP --keys 2795-2823:DOWN --keys 2823-2851:UP --keys 2851-2879:DOWN --keys 2879-2907:UP --keys 2907-2935:DOWN --keys 2935-2955:UP --keys-pattern 2515-2955:8:1:RIGHT --keys 2955-2970:DOWN --keys 2970-3100:A --keys 2970-2984:DOWN --keys 2984-3009:UP --keys 3009-3034:DOWN --keys 3034-3059:UP --keys 3059-3084:DOWN --keys 3084-3100:UP --keys-pattern 2970-3100:6:1:RIGHT --keys 3100-3172:LEFT --keys 3180-3560:A --keys 3180-3200:UP --keys 3200-3228:DOWN --keys 3228-3256:UP --keys 3256-3284:DOWN --keys 3284-3312:UP --keys 3312-3340:DOWN --keys 3340-3368:UP --keys 3368-3396:DOWN --keys 3396-3424:UP --keys 3424-3452:DOWN --keys 3452-3480:UP --keys 3480-3508:DOWN --keys 3508-3536:UP --keys 3536-3560:DOWN --keys-pattern 3180-3560:8:1:RIGHT --keys 3600-3602:START'

P4_ASSERTS=(
  --assert-watch 100:t:16:eq:2
  --assert-watch 200:t:24:eq:3
  --assert-watch 800:t:7:eq:9
  --assert-watch 1700:t:7:eq:25
  --assert-watch 2000:t:7:eq:30
  --assert-watch 3100:t:7:eq:39
  --assert-watch 3540:t:2:eq:2
  --assert-watch 3540:t:7:eq:40
  --assert-watch 3540:t:6:eq:10
  --assert-watch 3540:t:15:eq:3519
  --assert-watch 3540:t:16:eq:2
  --assert-watch 3540:t:17:eq:0
  --assert-watch 3540:t:22:eq:0
  --assert-watch 3540:t:23:eq:0
  --assert-watch 3540:t:5:eq:2988
  --assert-watch 3540:t:5:lt:4096
  --assert-text "3560:THE SHOAL IS HOME"
  --assert-text "3560:SAVED 40/50"
  --assert-text "3560:CLOCK 58s (3519 FRAMES)"
  --assert-text "3560:THE TIDE THANKS YOU"
  --assert-text "1700:SAVED 25/40 *"
  --assert-text "2000:SAVED 30/40 **"
  --assert-text "3560:RATING ***"
  --assert-watch 3650:t:2:eq:1
  --assert-watch 3650:t:16:eq:0
  --assert-watch 3650:t:17:eq:0
  --assert-watch 3650:t:6:eq:50
  --assert-watch 3650:t:7:eq:0
  --assert-watch 3650:t:24:eq:0
)

echo "== P4: THE GATED RUN — the committed funnel wins (run 1) =="
H "$OUT/p4.png" --frames 3660 $W $GATED_WIN_ROUTE \
  --watch-log "$OUT/p4-run1.csv" --shot "3560:$OUT/p4-win.png" \
  "${P4_ASSERTS[@]}"

echo "== P4: run 2 (must be byte-identical) =="
H "$OUT/p4b.png" --frames 3660 $W $GATED_WIN_ROUTE \
  --watch-log "$OUT/p4-run2.csv" \
  "${P4_ASSERTS[@]}"
cmp "$OUT/p4-run1.csv" "$OUT/p4-run2.csv"
echo "P4 run-twice: byte-identical"

echo "== P5: gates are geometry — the idle discriminator =="
# Identical absence of input to P2's calm idle; only the verb differs.
# Calm banks 5 by frame 1299 (P2); the walls hold the gated school at 0.
H "$OUT/p5.png" --frames 1300 $W --keys 10-12:R \
  --assert-watch 60:t:16:eq:2 \
  --assert-watch 60:t:24:eq:1 \
  --assert-watch 1299:t:6:eq:50 \
  --assert-watch 1299:t:7:eq:0 \
  --assert-watch 1299:t:8:eq:70 \
  --assert-watch 1299:t:9:eq:62 \
  --assert-watch 1299:t:17:eq:0 \
  --assert-watch 1299:t:5:eq:3018 \
  --assert-watch 1299:t:5:lt:4096

# The committed graded-loss route (rung 3 — ratings grade LOSSES too):
# SELECT starts the hungry water, the cursor slips under the school to
# its right flank (banking nothing on the way), then the current pins
# the school against the LEFT wall — off the reef, permanently
# straggling. Six fish leak home early; the hunters take the rest one
# straggler at a time until the 35-goal dies at eaten 16: SCATTERED at
# run-frame 3420 with saved 6 -> "RATING -". Every span before frame
# 3433 is load-bearing. (Rung 4 re-based every post-press span +1 hw
# frame: the hungry transition crossed a vblank boundary — boot-lag
# 1 -> 2 skipped frames — so the same run-frame inputs land one hw
# frame later; the sim is identical in run-frame terms.)
GRADED_LOSS_ROUTE='--keys 10-12:SELECT --keys 13-43:DOWN --keys 13-58:RIGHT --keys 61-91:UP --keys 101-3461:A --keys 101-131:DOWN --keys 161-191:UP --keys 221-251:DOWN --keys 281-311:UP --keys 341-371:DOWN --keys 401-431:UP --keys 461-491:DOWN --keys 521-551:UP --keys 581-611:DOWN --keys 641-671:UP --keys 701-731:DOWN --keys 761-791:UP --keys 821-851:DOWN --keys 881-911:UP --keys 941-971:DOWN --keys 1001-1031:UP --keys 1061-1091:DOWN --keys 1121-1151:UP --keys 1181-1211:DOWN --keys 1241-1271:UP --keys 1301-1331:DOWN --keys 1361-1391:UP --keys 1421-1451:DOWN --keys 1481-1511:UP --keys 1541-1571:DOWN --keys 1601-1631:UP --keys 1661-1691:DOWN --keys 1721-1751:UP --keys 1781-1811:DOWN --keys 1841-1871:UP --keys 1901-1931:DOWN --keys 1961-1991:UP --keys 2021-2051:DOWN --keys 2081-2111:UP --keys 2141-2171:DOWN --keys 2201-2231:UP --keys 2261-2291:DOWN --keys 2321-2351:UP --keys 2381-2411:DOWN --keys 2441-2471:UP --keys 2501-2531:DOWN --keys 2561-2591:UP --keys 2621-2651:DOWN --keys 2681-2711:UP --keys 2741-2771:DOWN --keys 2801-2831:UP --keys 2861-2891:DOWN --keys 2921-2951:UP --keys 2981-3011:DOWN --keys 3041-3071:UP --keys 3101-3131:DOWN --keys 3161-3191:UP --keys 3221-3251:DOWN --keys 3281-3311:UP --keys 3341-3371:DOWN --keys 3401-3431:UP'

P6_ASSERTS=(
  --assert-watch 100:t:16:eq:1
  --assert-watch 1200:t:7:eq:6
  --assert-watch 1200:t:17:eq:6
  --assert-watch 3450:t:2:eq:3
  --assert-watch 3450:t:7:eq:6
  --assert-watch 3450:t:17:eq:16
  --assert-watch 3450:t:15:eq:3420
  --assert-watch 3450:t:24:eq:0
  --assert-watch 3450:t:5:eq:3112
  --assert-watch 3450:t:5:lt:4096
  --assert-text "3480:THE SHOAL SCATTERED"
  --assert-text "3480:EATEN 16  SAVED 6"
  --assert-text "3480:CLOCK 57s (3420 FRAMES)"
  --assert-text "3480:KEEP THE SCHOOL TIGHT"
  --assert-text "3480:RATING -"
)

echo "== P6: a graded loss — the scattered shoal still banks its stars (run 1) =="
H "$OUT/p6.png" --frames 3520 $W $GRADED_LOSS_ROUTE \
  --watch-log "$OUT/p6-run1.csv" --shot "3480:$OUT/p6-card.png" \
  "${P6_ASSERTS[@]}"

echo "== P6: run 2 (must be byte-identical) =="
H "$OUT/p6b.png" --frames 3520 $W $GRADED_LOSS_ROUTE \
  --watch-log "$OUT/p6-run2.csv" \
  "${P6_ASSERTS[@]}"
cmp "$OUT/p6-run1.csv" "$OUT/p6-run2.csv"
echo "P6 run-twice: byte-identical"

# The tuned-levels chain (rung 4): every span is load-bearing up to the
# L4 win at frame 8740. Segment bases: L1 @ 12 (the funnel route — one
# gate, no teeth, wins rf 2881); L press 2940 -> L2 @ 2943 (blast-push +
# gather, wins rf 1050 with 3 lost to the 44px/300 hunter); L press
# 4030 -> L3 @ 4034 (the funnel vs a 52px/360 hunter, wins rf 2955,
# 5 lost); L press 7030 -> L4 @ 7034 (the funnel vs TWO 52px/420
# hunters, wins rf 1707, 7 lost); L press 8800 wraps to L1.
LEVELS_CHAIN_ROUTE='--keys 10-12:L --keys 12-32:RIGHT --keys 12-42:DOWN --keys 42-352:A --keys 42-72:LEFT --keys 72-110:RIGHT --keys 110-148:LEFT --keys 148-186:RIGHT --keys 186-224:LEFT --keys 224-262:RIGHT --keys 262-300:LEFT --keys 300-338:RIGHT --keys 352-384:LEFT --keys 352-410:UP --keys 410-850:A --keys 410-438:DOWN --keys 438-466:UP --keys 466-494:DOWN --keys 494-522:UP --keys 522-550:DOWN --keys 550-578:UP --keys 578-606:DOWN --keys 606-634:UP --keys 634-662:DOWN --keys 662-690:UP --keys 690-718:DOWN --keys 718-746:UP --keys 746-774:DOWN --keys 774-802:UP --keys 802-830:DOWN --keys-pattern 410-850:8:1:RIGHT --keys 850-880:LEFT --keys 850-881:DOWN --keys 881-1160:A --keys 881-911:LEFT --keys 911-949:RIGHT --keys 949-987:LEFT --keys 987-1025:RIGHT --keys 1025-1063:LEFT --keys 1063-1101:RIGHT --keys 1101-1139:LEFT --keys 1139-1160:RIGHT --keys 1160-1195:LEFT --keys 1160-1218:UP --keys 1225-1665:A --keys 1225-1253:DOWN --keys 1253-1281:UP --keys 1281-1309:DOWN --keys 1309-1337:UP --keys 1337-1365:DOWN --keys 1365-1393:UP --keys 1393-1421:DOWN --keys 1421-1449:UP --keys 1449-1477:DOWN --keys 1477-1505:UP --keys 1505-1533:DOWN --keys 1533-1561:UP --keys 1561-1589:DOWN --keys 1589-1617:UP --keys 1617-1645:DOWN --keys-pattern 1225-1665:8:1:RIGHT --keys 1800-1807:LEFT --keys 1800-1815:DOWN --keys 1815-2100:A --keys 1815-1829:DOWN --keys 1829-1854:UP --keys 1854-1879:DOWN --keys 1879-1904:UP --keys 1904-1929:DOWN --keys 1929-1954:UP --keys 1954-1979:DOWN --keys 1979-2004:UP --keys 2004-2029:DOWN --keys 2029-2054:UP --keys 2054-2079:DOWN --keys 2079-2100:UP --keys-pattern 1815-2100:6:1:RIGHT --keys 2100-2193:LEFT --keys 2100-2122:DOWN --keys 2193-2450:A --keys 2193-2231:RIGHT --keys 2231-2269:LEFT --keys 2269-2307:RIGHT --keys 2307-2345:LEFT --keys 2345-2383:RIGHT --keys 2383-2421:LEFT --keys 2421-2450:RIGHT --keys 2450-2478:LEFT --keys 2450-2510:UP --keys 2515-2900:A --keys 2515-2543:DOWN --keys 2543-2571:UP --keys 2571-2599:DOWN --keys 2599-2627:UP --keys 2627-2655:DOWN --keys 2655-2683:UP --keys 2683-2711:DOWN --keys 2711-2739:UP --keys 2739-2767:DOWN --keys 2767-2795:UP --keys 2795-2823:DOWN --keys 2823-2851:UP --keys 2851-2879:DOWN --keys 2879-2900:UP --keys-pattern 2515-2900:8:1:RIGHT --keys 2940-2942:L --keys 2943-3441:A --keys 3441-3451:LEFT --keys 3441-3471:DOWN --keys 3481-3731:A --keys 3481-3519:RIGHT --keys 3519-3557:LEFT --keys 3557-3595:RIGHT --keys 3595-3633:LEFT --keys 3633-3671:RIGHT --keys 3671-3709:LEFT --keys 3731-3759:LEFT --keys 3731-3791:UP --keys 3801-4026:A --keys 3801-3829:DOWN --keys 3829-3857:UP --keys 3857-3885:DOWN --keys 3885-3913:UP --keys 3913-3941:DOWN --keys 3941-3969:UP --keys 3969-3997:DOWN --keys 3997-4025:UP --keys 4025-4026:DOWN --keys-pattern 3801-4026:8:1:RIGHT --keys 4030-4032:L --keys 4034-4054:RIGHT --keys 4034-4064:DOWN --keys 4064-4374:A --keys 4064-4094:LEFT --keys 4094-4132:RIGHT --keys 4132-4170:LEFT --keys 4170-4208:RIGHT --keys 4208-4246:LEFT --keys 4246-4284:RIGHT --keys 4284-4322:LEFT --keys 4322-4360:RIGHT --keys 4374-4406:LEFT --keys 4374-4432:UP --keys 4432-4872:A --keys 4432-4460:DOWN --keys 4460-4488:UP --keys 4488-4516:DOWN --keys 4516-4544:UP --keys 4544-4572:DOWN --keys 4572-4600:UP --keys 4600-4628:DOWN --keys 4628-4656:UP --keys 4656-4684:DOWN --keys 4684-4712:UP --keys 4712-4740:DOWN --keys 4740-4768:UP --keys 4768-4796:DOWN --keys 4796-4824:UP --keys 4824-4852:DOWN --keys-pattern 4432-4872:8:1:RIGHT --keys 4872-4902:LEFT --keys 4872-4903:DOWN --keys 4903-5182:A --keys 4903-4933:LEFT --keys 4933-4971:RIGHT --keys 4971-5009:LEFT --keys 5009-5047:RIGHT --keys 5047-5085:LEFT --keys 5085-5123:RIGHT --keys 5123-5161:LEFT --keys 5161-5182:RIGHT --keys 5182-5217:LEFT --keys 5182-5240:UP --keys 5247-5687:A --keys 5247-5275:DOWN --keys 5275-5303:UP --keys 5303-5331:DOWN --keys 5331-5359:UP --keys 5359-5387:DOWN --keys 5387-5415:UP --keys 5415-5443:DOWN --keys 5443-5471:UP --keys 5471-5499:DOWN --keys 5499-5527:UP --keys 5527-5555:DOWN --keys 5555-5583:UP --keys 5583-5611:DOWN --keys 5611-5639:UP --keys 5639-5667:DOWN --keys-pattern 5247-5687:8:1:RIGHT --keys 5822-5829:LEFT --keys 5822-5837:DOWN --keys 5837-6122:A --keys 5837-5851:DOWN --keys 5851-5876:UP --keys 5876-5901:DOWN --keys 5901-5926:UP --keys 5926-5951:DOWN --keys 5951-5976:UP --keys 5976-6001:DOWN --keys 6001-6026:UP --keys 6026-6051:DOWN --keys 6051-6076:UP --keys 6076-6101:DOWN --keys 6101-6122:UP --keys-pattern 5837-6122:6:1:RIGHT --keys 6122-6215:LEFT --keys 6122-6144:DOWN --keys 6215-6472:A --keys 6215-6253:RIGHT --keys 6253-6291:LEFT --keys 6291-6329:RIGHT --keys 6329-6367:LEFT --keys 6367-6405:RIGHT --keys 6405-6443:LEFT --keys 6443-6472:RIGHT --keys 6472-6500:LEFT --keys 6472-6532:UP --keys 6537-6977:A --keys 6537-6565:DOWN --keys 6565-6593:UP --keys 6593-6621:DOWN --keys 6621-6649:UP --keys 6649-6677:DOWN --keys 6677-6705:UP --keys 6705-6733:DOWN --keys 6733-6761:UP --keys 6761-6789:DOWN --keys 6789-6817:UP --keys 6817-6845:DOWN --keys 6845-6873:UP --keys 6873-6901:DOWN --keys 6901-6929:UP --keys 6929-6957:DOWN --keys 6957-6977:UP --keys-pattern 6537-6977:8:1:RIGHT --keys 6977-6992:DOWN --keys 6992-7020:A --keys 6992-7006:DOWN --keys 7006-7020:UP --keys-pattern 6992-7020:6:1:RIGHT --keys 7030-7032:L --keys 7034-7054:RIGHT --keys 7034-7064:DOWN --keys 7064-7374:A --keys 7064-7094:LEFT --keys 7094-7132:RIGHT --keys 7132-7170:LEFT --keys 7170-7208:RIGHT --keys 7208-7246:LEFT --keys 7246-7284:RIGHT --keys 7284-7322:LEFT --keys 7322-7360:RIGHT --keys 7374-7406:LEFT --keys 7374-7432:UP --keys 7432-7872:A --keys 7432-7460:DOWN --keys 7460-7488:UP --keys 7488-7516:DOWN --keys 7516-7544:UP --keys 7544-7572:DOWN --keys 7572-7600:UP --keys 7600-7628:DOWN --keys 7628-7656:UP --keys 7656-7684:DOWN --keys 7684-7712:UP --keys 7712-7740:DOWN --keys 7740-7768:UP --keys 7768-7796:DOWN --keys 7796-7824:UP --keys 7824-7852:DOWN --keys-pattern 7432-7872:8:1:RIGHT --keys 7872-7902:LEFT --keys 7872-7903:DOWN --keys 7903-8182:A --keys 7903-7933:LEFT --keys 7933-7971:RIGHT --keys 7971-8009:LEFT --keys 8009-8047:RIGHT --keys 8047-8085:LEFT --keys 8085-8123:RIGHT --keys 8123-8161:LEFT --keys 8161-8182:RIGHT --keys 8182-8217:LEFT --keys 8182-8240:UP --keys 8247-8687:A --keys 8247-8275:DOWN --keys 8275-8303:UP --keys 8303-8331:DOWN --keys 8331-8359:UP --keys 8359-8387:DOWN --keys 8387-8415:UP --keys 8415-8443:DOWN --keys 8443-8471:UP --keys 8471-8499:DOWN --keys 8499-8527:UP --keys 8527-8555:DOWN --keys 8555-8583:UP --keys 8583-8611:DOWN --keys 8611-8639:UP --keys 8639-8667:DOWN --keys-pattern 8247-8687:8:1:RIGHT --keys 8800-8802:L'

P7_ASSERTS=(
  --assert-watch 100:t:16:eq:3
  --assert-watch 100:t:25:eq:0
  --assert-watch 100:t:24:eq:1
  --assert-watch 2904:t:2:eq:2
  --assert-watch 2904:t:7:eq:40
  --assert-watch 2904:t:17:eq:0
  --assert-watch 2904:t:15:eq:2881
  --assert-watch 2950:t:25:eq:1
  --assert-watch 2950:t:6:eq:50
  --assert-watch 4001:t:2:eq:2
  --assert-watch 4001:t:7:eq:36
  --assert-watch 4001:t:17:eq:3
  --assert-watch 4001:t:15:eq:1050
  --assert-watch 4100:t:25:eq:2
  --assert-watch 7000:t:2:eq:2
  --assert-watch 7000:t:7:eq:32
  --assert-watch 7000:t:17:eq:5
  --assert-watch 7000:t:15:eq:2955
  --assert-watch 7500:t:25:eq:3
  --assert-watch 8000:t:7:eq:15
  --assert-watch 8000:t:17:eq:4
  --assert-watch 8750:t:2:eq:2
  --assert-watch 8750:t:7:eq:28
  --assert-watch 8750:t:17:eq:7
  --assert-watch 8750:t:15:eq:1707
  --assert-watch 8750:t:5:eq:3369
  --assert-watch 8750:t:5:lt:4096
  --assert-watch 8850:t:25:eq:0
  --assert-watch 8850:t:16:eq:3
  --assert-watch 8850:t:6:eq:50
  --assert-watch 8850:t:7:eq:0
  --assert-watch 8850:t:15:eq:48
  --assert-text "2920:L1 THE SHALLOWS - L: NEXT"
  --assert-text "2920:RATING ***"
  --assert-text "2920:SAVED 40/50"
  --assert-text "4020:L2 THE HUNT - L: NEXT"
  --assert-text "4020:SAVED 36/50"
  --assert-text "7020:L3 THE NARROWS - L: NEXT"
  --assert-text "7020:SAVED 32/50"
  --assert-text "8000:L4 SAVED 15/28 *"
  --assert-text "8770:L4 DEEP WATER - L: NEXT"
  --assert-text "8770:SAVED 28/50"
  --assert-text "8770:RATING ***"
)

echo "== P7: the tuned levels — one chain, four wins, the wrap (run 1) =="
H "$OUT/p7.png" --frames 8900 $W $LEVELS_CHAIN_ROUTE \
  --watch-log "$OUT/p7-run1.csv" --shot "8770:$OUT/p7-l4-card.png" \
  "${P7_ASSERTS[@]}"

echo "== P7: run 2 (must be byte-identical) =="
H "$OUT/p7b.png" --frames 8900 $W $LEVELS_CHAIN_ROUTE \
  --watch-log "$OUT/p7-run2.csv" \
  "${P7_ASSERTS[@]}"
cmp "$OUT/p7-run1.csv" "$OUT/p7-run2.csv"
echo "P7 run-twice: byte-identical"

echo "ALL SHOAL PROOFS PASS"
