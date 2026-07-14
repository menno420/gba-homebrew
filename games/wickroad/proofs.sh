#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# WICKROAD — the committed headless proof suite (mGBA, tools/headless-screenshot.py).
#
# Committed from day one (the Shoal oracle lesson: input scripts are part of
# the pin — a proof that lives only in a session container dies with it).
#
# Run from the repo root after building (tools/build.sh or make -C games/wickroad):
#     bash games/wickroad/proofs.sh
# Artifacts land in $WICKROAD_PROOF_OUT (default /tmp/wickroad-proofs).
#
# The three proofs (asserts inline below):
#   P1 boot/title            — magics, title state, every title line incl.
#                              the hook line ("BUT THE INK AGES") and the
#                              full verb help; witness words zero on title.
#   P2 THE WICK RUN (win)    — the committed 13-day trading route: tallow
#                              EMBERTON->SALTCOMBE, iron SALTCOMBE->DUNWICK
#                              three times over, gold 60 -> 328 with every
#                              leg's day/gold/cargo pinned exact. The
#                              market-impact law is pinned in the gold
#                              deltas (iron buys climb 22,23,24 / 26..30 /
#                              32..37 as the route corners the market).
#                              Win card text exact ("THE LEDGER BALANCES",
#                              "GOLD 328", "DAY 13 OF 30"); START restarts
#                              day 1 / gold 60 / empty pack. RUN TWICE —
#                              byte-identical watch-logs (determinism
#                              contract).
#   P3 THE STALE INK + THE PASS CLOSES — the hook and the fail state on one
#                              route: the buy-impact ladder lifts EMBERTON
#                              tallow 9->10->11->12 (one per unit) and two
#                              dawns decay it back through 12,10; THORNBY's
#                              ledger is inked at 27 on day 6 and then AGES
#                              while the true price moves (day 9: ink says
#                              27 at age 3, the world says 33 — the ledger
#                              provably lies); waiting out the winter closes
#                              the pass at dawn 31 (state 3, gold floored at
#                              0, three tallow still in the pack), card text
#                              exact, START restarts. RUN TWICE —
#                              byte-identical watch-logs.
#
# Mailbox: wr_telemetry[16] (layout in games/wickroad/src/main.cpp).
# Word 15 encodes the THORNBY/SALT ledger entry as (ink price << 8) | age:
# 6912 = 27 @ age 0 · 6913 = 27 @ 1 · 6915 = 27 @ 3 · 6937 = 27 @ 25.
# Turn-based determinism: every assert frame sits >= 4 frames after its
# key edge; the same script replays bit-identically by construction.
# ---------------------------------------------------------------------------
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."

ROM=games/wickroad/wickroad.gba
OUT="${WICKROAD_PROOF_OUT:-/tmp/wickroad-proofs}"
mkdir -p "$OUT"

H() { python3 tools/headless-screenshot.py "$ROM" "$@"; }
W='--elf games/wickroad/wickroad.elf --watch wr:wr_telemetry:16'

echo "== P1: boot + title =="
H "$OUT/p1.png" --frames 70 $W \
  --assert-watch 60:wr:0:eq:1464419147 \
  --assert-watch 60:wr:1:eq:1380925764 \
  --assert-watch 60:wr:2:eq:0 \
  --assert-watch 60:wr:4:eq:0 \
  --assert-watch 60:wr:5:eq:0 \
  --assert-watch 60:wr:13:eq:0 \
  --assert-watch 60:wr:14:eq:0 \
  --assert-watch 60:wr:15:eq:0 \
  --assert-text "60:WICKROAD" \
  --assert-text "60:ONE ROAD - FIVE MARKETS" \
  --assert-text "60:BUY LOW - SELL HIGH" \
  --assert-text "60:300 GOLD BEFORE DAY 30" \
  --assert-text "60:YOUR LEDGER REMEMBERS" \
  --assert-text "60:BUT THE INK AGES" \
  --assert-text "60:PRESS START" \
  --assert-text "60:A BUY  B SELL  L/R GO  SEL WAIT"

# The committed 13-day winning route (win at the 5th iron sale, frame 452):
# day 1 EMBERTON buy 5 tallow -> day 3 SALTCOMBE sell, buy 3 iron -> day 5
# DUNWICK sell, buy 8 tallow -> day 7 SALTCOMBE sell, buy 5 iron -> day 9
# DUNWICK sell -> day 11 SALTCOMBE buy 6 iron -> day 13 DUNWICK: the 5th
# sale banks gold 328 >= 300. START at 470 restarts. Every span is
# load-bearing up to frame 452.
WIN_ROUTE='--keys 10-12:START --keys 30-32:A --keys 36-38:A --keys 42-44:A --keys 48-50:A --keys 54-56:A --keys 60-62:R --keys 66-68:R --keys 72-74:B --keys 78-80:B --keys 84-86:B --keys 90-92:B --keys 96-98:B --keys 102-104:DOWN --keys 108-110:DOWN --keys 114-116:A --keys 120-122:A --keys 126-128:A --keys 132-134:R --keys 138-140:R --keys 144-146:B --keys 150-152:B --keys 156-158:B --keys 162-164:UP --keys 168-170:UP --keys 174-176:A --keys 180-182:A --keys 186-188:A --keys 192-194:A --keys 198-200:A --keys 204-206:A --keys 210-212:A --keys 216-218:A --keys 222-224:L --keys 228-230:L --keys 234-236:B --keys 240-242:B --keys 246-248:B --keys 252-254:B --keys 258-260:B --keys 264-266:B --keys 270-272:B --keys 276-278:B --keys 282-284:DOWN --keys 288-290:DOWN --keys 294-296:A --keys 300-302:A --keys 306-308:A --keys 312-314:A --keys 318-320:A --keys 324-326:R --keys 330-332:R --keys 336-338:B --keys 342-344:B --keys 348-350:B --keys 354-356:B --keys 360-362:B --keys 366-368:L --keys 372-374:L --keys 378-380:A --keys 384-386:A --keys 390-392:A --keys 396-398:A --keys 402-404:A --keys 408-410:A --keys 414-416:R --keys 420-422:R --keys 426-428:B --keys 432-434:B --keys 438-440:B --keys 444-446:B --keys 450-452:B --keys 470-472:START'

WIN_ASSERTS=(
  --assert-watch 20:wr:2:eq:1
  --assert-watch 20:wr:4:eq:1
  --assert-watch 20:wr:5:eq:60
  --assert-watch 20:wr:6:eq:0
  --assert-watch 20:wr:13:eq:9
  --assert-watch 60:wr:4:eq:2
  --assert-watch 60:wr:5:eq:3
  --assert-watch 60:wr:6:eq:1
  --assert-watch 60:wr:9:eq:5
  --assert-watch 102:wr:4:eq:3
  --assert-watch 102:wr:5:eq:86
  --assert-watch 102:wr:6:eq:2
  --assert-watch 102:wr:8:eq:0
  --assert-watch 132:wr:5:eq:20
  --assert-watch 132:wr:7:eq:2
  --assert-watch 132:wr:11:eq:3
  --assert-watch 162:wr:4:eq:5
  --assert-watch 162:wr:5:eq:154
  --assert-watch 162:wr:6:eq:4
  --assert-watch 162:wr:8:eq:0
  --assert-watch 222:wr:5:eq:46
  --assert-watch 222:wr:9:eq:8
  --assert-watch 282:wr:4:eq:7
  --assert-watch 282:wr:5:eq:158
  --assert-watch 282:wr:6:eq:2
  --assert-watch 324:wr:5:eq:18
  --assert-watch 324:wr:11:eq:5
  --assert-watch 366:wr:4:eq:9
  --assert-watch 366:wr:5:eq:269
  --assert-watch 366:wr:6:eq:4
  --assert-watch 366:wr:14:eq:33
  --assert-watch 366:wr:15:eq:7937
  --assert-watch 414:wr:4:eq:11
  --assert-watch 414:wr:5:eq:52
  --assert-watch 414:wr:11:eq:6
  --assert-watch 448:wr:4:eq:13
  --assert-watch 448:wr:5:eq:274
  --assert-watch 448:wr:11:eq:2
  --assert-watch 448:wr:13:eq:55
  --assert-watch 460:wr:2:eq:2
  --assert-watch 460:wr:5:eq:328
  --assert-watch 460:wr:4:eq:13
  --assert-watch 460:wr:11:eq:1
  --assert-text "282:DAY 7/30  GOLD 158"
  --assert-text "466:THE LEDGER BALANCES"
  --assert-text "466:GOLD 328"
  --assert-text "466:DAY 13 OF 30"
  --assert-text "466:THE ROAD MADE YOU"
  --assert-watch 490:wr:2:eq:1
  --assert-watch 490:wr:4:eq:1
  --assert-watch 490:wr:5:eq:60
  --assert-watch 490:wr:6:eq:0
  --assert-watch 490:wr:8:eq:0
)

echo "== P2: THE WICK RUN — the committed route wins (run 1) =="
H "$OUT/p2.png" --frames 500 $W $WIN_ROUTE \
  --watch-log "$OUT/p2-run1.csv" --shot "466:$OUT/p2-win.png" \
  "${WIN_ASSERTS[@]}"

echo "== P2: run 2 (must be byte-identical) =="
H "$OUT/p2b.png" --frames 500 $W $WIN_ROUTE \
  --watch-log "$OUT/p2-run2.csv" \
  "${WIN_ASSERTS[@]}"
cmp "$OUT/p2-run1.csv" "$OUT/p2-run2.csv"
echo "P2 run-twice: byte-identical"

# The stale-ink / pass-closes route: buy 3 tallow at EMBERTON (impact
# ladder), wait two dawns (decay), ride to THORNBY (ink its ledger),
# step back to SALTCOMBE, wait out the winter, restart from the card.
LOSE_ROUTE='--keys 10-12:START --keys 30-32:A --keys 36-38:A --keys 42-44:A --keys 48-50:SELECT --keys 54-56:SELECT --keys 60-62:R --keys 66-68:R --keys 72-74:R --keys 78-80:L --keys 84-86:SELECT --keys 90-92:SELECT --keys 96-98:SELECT --keys 102-104:SELECT --keys 108-110:SELECT --keys 114-116:SELECT --keys 120-122:SELECT --keys 126-128:SELECT --keys 132-134:SELECT --keys 138-140:SELECT --keys 144-146:SELECT --keys 150-152:SELECT --keys 156-158:SELECT --keys 162-164:SELECT --keys 168-170:SELECT --keys 174-176:SELECT --keys 180-182:SELECT --keys 186-188:SELECT --keys 192-194:SELECT --keys 198-200:SELECT --keys 204-206:SELECT --keys 210-212:SELECT --keys 216-218:SELECT --keys 222-224:SELECT --keys 240-242:START'

LOSE_ASSERTS=(
  --assert-watch 28:wr:13:eq:9
  --assert-watch 34:wr:13:eq:10
  --assert-watch 40:wr:13:eq:11
  --assert-watch 46:wr:13:eq:12
  --assert-watch 46:wr:5:eq:30
  --assert-watch 46:wr:9:eq:3
  --assert-watch 52:wr:4:eq:2
  --assert-watch 52:wr:13:eq:12
  --assert-watch 58:wr:4:eq:3
  --assert-watch 58:wr:13:eq:10
  --assert-watch 78:wr:4:eq:6
  --assert-watch 78:wr:6:eq:3
  --assert-watch 78:wr:14:eq:27
  --assert-watch 78:wr:15:eq:6912
  --assert-watch 84:wr:4:eq:7
  --assert-watch 84:wr:6:eq:2
  --assert-watch 84:wr:15:eq:6913
  --assert-watch 96:wr:4:eq:9
  --assert-watch 96:wr:14:eq:33
  --assert-watch 96:wr:14:ne:27
  --assert-watch 96:wr:15:eq:6915
  --assert-watch 226:wr:2:eq:3
  --assert-watch 226:wr:4:eq:31
  --assert-watch 226:wr:5:eq:0
  --assert-watch 226:wr:9:eq:3
  --assert-watch 226:wr:15:eq:6937
  --assert-text "236:THE PASS CLOSES"
  --assert-text "236:GOLD 0 OF 300"
  --assert-text "236:30 DAYS SPENT"
  --assert-text "236:WINTER TAKES THE ROAD"
  --assert-watch 260:wr:2:eq:1
  --assert-watch 260:wr:4:eq:1
  --assert-watch 260:wr:5:eq:60
  --assert-watch 260:wr:8:eq:0
  --assert-watch 260:wr:15:eq:0
)

echo "== P3: THE STALE INK + THE PASS CLOSES (run 1) =="
H "$OUT/p3.png" --frames 280 $W $LOSE_ROUTE \
  --watch-log "$OUT/p3-run1.csv" --shot "236:$OUT/p3-card.png" \
  "${LOSE_ASSERTS[@]}"

echo "== P3: run 2 (must be byte-identical) =="
H "$OUT/p3b.png" --frames 280 $W $LOSE_ROUTE \
  --watch-log "$OUT/p3-run2.csv" \
  "${LOSE_ASSERTS[@]}"
cmp "$OUT/p3-run1.csv" "$OUT/p3-run2.csv"
echo "P3 run-twice: byte-identical"

echo "ALL WICKROAD PROOFS PASS"
