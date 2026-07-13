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
W='--elf games/shoal/shoal.elf --watch t:sh_telemetry:25'

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
  --assert-text "60:R: THE GATED RUN (40)"

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
  --assert-watch 400:t:4:eq:2085 \
  --assert-watch 400:t:5:eq:2822 \
  --assert-watch 400:t:5:lt:4096 \
  --assert-watch 1199:t:6:eq:47 \
  --assert-watch 1199:t:7:eq:3 \
  --assert-watch 1199:t:8:eq:85 \
  --assert-watch 1199:t:9:eq:57 \
  --assert-watch 1199:t:5:eq:2822 \
  --assert-watch 1199:t:5:lt:4096 \
  --assert-watch 1299:t:7:eq:5 \
  --assert-watch 1299:t:16:eq:0 \
  --assert-watch 1299:t:24:eq:0

echo "== P3: hungry idle — the predator pass, carried =="
H "$OUT/p3.png" --frames 1700 $W --keys 10-12:SELECT \
  --assert-watch 320:t:16:eq:1 \
  --assert-watch 320:t:17:eq:2 \
  --assert-watch 320:t:18:eq:176 \
  --assert-watch 320:t:19:eq:40 \
  --assert-watch 320:t:20:eq:176 \
  --assert-watch 320:t:21:eq:120 \
  --assert-watch 400:t:5:eq:3058 \
  --assert-watch 400:t:5:lt:4096 \
  --assert-watch 1420:t:17:eq:8 \
  --assert-watch 1699:t:17:eq:10 \
  --assert-watch 1699:t:6:eq:37 \
  --assert-watch 1699:t:7:eq:3 \
  --assert-watch 1699:t:5:eq:3058 \
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
  --assert-watch 3540:t:5:eq:2916
  --assert-watch 3540:t:5:lt:4096
  --assert-text "3560:THE SHOAL IS HOME"
  --assert-text "3560:SAVED 40/50"
  --assert-text "3560:CLOCK 58s (3519 FRAMES)"
  --assert-text "3560:THE TIDE THANKS YOU"
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
  --assert-watch 1299:t:5:eq:2949 \
  --assert-watch 1299:t:5:lt:4096

echo "ALL SHOAL PROOFS PASS"
