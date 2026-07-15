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
# The five proofs (asserts inline below):
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
#   P4 THE RUMORS (growth cut 1) — the forecast is real: waiting out 20 days
#                              at EMBERTON (cursor parked on SALT), the crier
#                              telegraphs each deck rumor days ahead and the
#                              market then obeys it TO THE GOLD PIECE. R1
#                              (iron to 58 at GLASSMERE day 5): announced on
#                              day 2 while the true price still reads 37
#                              (foretold 58 pinned BEFORE the shock, phase 1),
#                              then day 5 realizes 58 == foretold with shock
#                              +12 live (phase 2). R2 (salt to 11 at EMBERTON
#                              day 12): announced day 8 at true 21, realized
#                              11 == foretold on day 12 — ON the visible
#                              market (cursor word 13 = 11 too, two code
#                              paths agreeing). R3 (resin to 42 at DUNWICK
#                              day 18): announced day 14 at true 25, realized
#                              42 with shock +9, and by day 21 the window has
#                              passed (phase 3, shock 0, price back on the
#                              plain law at 31). Crier lines exact on screen;
#                              gold 60 -> 40 pins the 20 lodgings. RUN TWICE —
#                              byte-identical watch-logs. Every value below
#                              was derived on the host-side mirror of the sim
#                              law FIRST and matched by the ROM exactly.
#   P5 THE CONTRACTS (growth cut 2) — the second income verb prices risk:
#                              day 2 posts C1 ("PACT? 2 RESIN TO THORNBY D8
#                              +60", witness words 24-31 pinned before any
#                              player action), RIGHT takes it (state 1 -> 2),
#                              the 3-leg haul EMBERTON->THORNBY lands day 5,
#                              two resin bought on the impact ladder (21, 22
#                              — cursor word climbs 21->22->23), and RIGHT
#                              hands them over: gold jumps 10 -> 70, EXACTLY
#                              the authored premium 60 (state 3, word 31 =
#                              60), pack empty, prices and impact untouched
#                              (delivery bypasses the market). C2 (2 IRON to
#                              GLASSMERE D14 +70) posts day 10 and is left
#                              to expire: dawn 15 > deadline 14 LAPSES it
#                              (state 4, word 31 unchanged, "PACT LAPSED" on
#                              screen — the lapse pays nothing). Gold 60 at
#                              day 15 pins the whole route ledger (60 start
#                              - 1 - 6 tolls - 43 resin + 60 premium - 10
#                              lodgings). RUN TWICE — byte-identical
#                              watch-logs. Every value was derived on the
#                              host-side mirror of the sim law FIRST and
#                              matched by the ROM exactly. Pact-line text
#                              asserts sit >= 4 frames after their edge: the
#                              pact line regenerates on the SECOND quiet
#                              frame after a head redraw (the crier owns the
#                              first — dawn frames are budget-SPENT, see
#                              main.cpp).
#
# Mailbox: wr_telemetry[32] since v0.3 (layout in games/wickroad/src/main.cpp;
# P1-P3 keep watching the first 16 words, unchanged from v0.1; P4 the first
# 24, unchanged from v0.2).
# Word 15 encodes the THORNBY/SALT ledger entry as (ink price << 8) | age:
# 6912 = 27 @ age 0 · 6913 = 27 @ 1 · 6915 = 27 @ 3 · 6937 = 27 @ 25.
# Turn-based determinism: the same script replays bit-identically by
# construction. Honest alignment note (v0.2): several committed v0.1 pins
# assert ON their key-edge frame (e.g. P2 frame 60) — same-frame edge
# processing is part of the measured frame alignment those pins own, which
# also means the dawn frames' draw budget is a committed interface: new
# per-dawn UI work must defer off the dawn frame (see the crier note at P4
# and main.cpp). P4's own asserts sit 4 frames after their dawn edge.
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

# The rumors route: START, park the cursor on SALT (so word 13 shows the
# EMBERTON salt row the R2 crash lands on), then 20 SELECT waits — days
# 1..21 sweep the whole deck: R1 announce d2 / hit d5, R2 announce d8 /
# hit d12 (at the town we stand in), R3 announce d14 / hit d18 / passed
# by d21. Assert frames sit 4 after their dawn edge (the crier line
# itself regenerates one frame after the dawn, by design — see main.cpp).
RUMOR_ROUTE='--keys 10-12:START --keys 16-18:DOWN --keys 30-32:SELECT --keys 36-38:SELECT --keys 42-44:SELECT --keys 48-50:SELECT --keys 54-56:SELECT --keys 60-62:SELECT --keys 66-68:SELECT --keys 72-74:SELECT --keys 78-80:SELECT --keys 84-86:SELECT --keys 90-92:SELECT --keys 96-98:SELECT --keys 102-104:SELECT --keys 108-110:SELECT --keys 114-116:SELECT --keys 120-122:SELECT --keys 126-128:SELECT --keys 132-134:SELECT --keys 138-140:SELECT --keys 144-146:SELECT'

W24='--elf games/wickroad/wickroad.elf --watch wr:wr_telemetry:24'

RUMOR_ASSERTS=(
  # day 1: trading, no rumor announced yet — witness words all zero
  --assert-watch 24:wr:2:eq:1
  --assert-watch 24:wr:4:eq:1
  --assert-watch 24:wr:5:eq:60
  --assert-watch 24:wr:7:eq:1
  --assert-watch 24:wr:13:eq:19
  --assert-watch 24:wr:16:eq:0
  --assert-watch 24:wr:20:eq:0
  --assert-watch 24:wr:21:eq:0
  --assert-watch 24:wr:23:eq:0
  --assert-text "24:NO WORD ON THE ROAD"
  # day 2: R1 ANNOUNCED (iron to 58 at GLASSMERE by day 5) — foretold 58
  # pinned BEFORE the shock while the true price still reads 37 (phase 1:
  # the rumor is provably displayed ahead of the market obeying it)
  --assert-watch 34:wr:4:eq:2
  --assert-watch 34:wr:16:eq:1
  --assert-watch 34:wr:17:eq:5
  --assert-watch 34:wr:18:eq:1
  --assert-watch 34:wr:19:eq:2
  --assert-watch 34:wr:20:eq:58
  --assert-watch 34:wr:21:eq:37
  --assert-watch 34:wr:21:ne:58
  --assert-watch 34:wr:22:eq:0
  --assert-watch 34:wr:23:eq:1
  --assert-text "34:CRIER: IRON 58 AT GLASSMERE DAY 5"
  # day 5: R1 LANDS — realized 58 == the foretold 58, shock +12 live
  --assert-watch 52:wr:4:eq:5
  --assert-watch 52:wr:20:eq:58
  --assert-watch 52:wr:21:eq:58
  --assert-watch 52:wr:22:eq:12
  --assert-watch 52:wr:23:eq:2
  # day 8: R2 ANNOUNCED (salt to 11 at EMBERTON by day 12) — true 21
  --assert-watch 70:wr:16:eq:2
  --assert-watch 70:wr:17:eq:12
  --assert-watch 70:wr:18:eq:0
  --assert-watch 70:wr:19:eq:1
  --assert-watch 70:wr:20:eq:11
  --assert-watch 70:wr:21:eq:21
  --assert-watch 70:wr:22:eq:0
  --assert-watch 70:wr:23:eq:1
  # day 12: R2 LANDS in the town we stand in — realized 11 == foretold,
  # shock -6 (u32 0xfffffffa), and the CURSOR word sees the same 11 on
  # the visible market row (two code paths agreeing on the crash)
  --assert-watch 94:wr:4:eq:12
  --assert-watch 94:wr:13:eq:11
  --assert-watch 94:wr:20:eq:11
  --assert-watch 94:wr:21:eq:11
  --assert-watch 94:wr:22:eq:0xfffffffa
  --assert-watch 94:wr:23:eq:2
  --assert-text "94:CRIER: SALT 11 AT EMBERTON DAY 12"
  # day 14: R3 ANNOUNCED (resin to 42 at DUNWICK by day 18) — true 25
  --assert-watch 106:wr:16:eq:3
  --assert-watch 106:wr:17:eq:18
  --assert-watch 106:wr:18:eq:4
  --assert-watch 106:wr:19:eq:3
  --assert-watch 106:wr:20:eq:42
  --assert-watch 106:wr:21:eq:25
  --assert-watch 106:wr:21:ne:42
  --assert-watch 106:wr:23:eq:1
  # day 18: R3 LANDS — realized 42 == foretold, shock +9
  --assert-watch 130:wr:4:eq:18
  --assert-watch 130:wr:20:eq:42
  --assert-watch 130:wr:21:eq:42
  --assert-watch 130:wr:22:eq:9
  --assert-watch 130:wr:23:eq:2
  --assert-text "130:CRIER: RESIN 42 AT DUNWICK DAY 18"
  # day 21: the window has PASSED — shock off, the plain law back (31),
  # phase 3; gold 40 pins all 20 lodgings exactly
  --assert-watch 148:wr:4:eq:21
  --assert-watch 148:wr:5:eq:40
  --assert-watch 148:wr:21:eq:31
  --assert-watch 148:wr:22:eq:0
  --assert-watch 148:wr:23:eq:3
)

echo "== P4: THE RUMORS — the forecast is real (run 1) =="
H "$OUT/p4.png" --frames 160 $W24 $RUMOR_ROUTE \
  --watch-log "$OUT/p4-run1.csv" --shot "94:$OUT/p4-crash.png" \
  "${RUMOR_ASSERTS[@]}"

echo "== P4: run 2 (must be byte-identical) =="
H "$OUT/p4b.png" --frames 160 $W24 $RUMOR_ROUTE \
  --watch-log "$OUT/p4-run2.csv" \
  "${RUMOR_ASSERTS[@]}"
cmp "$OUT/p4-run1.csv" "$OUT/p4-run2.csv"
echo "P4 run-twice: byte-identical"

# The contracts route: START, park the cursor on RESIN (UP: 0 -> 3, so
# word 13 tracks the good C1 wants), wait to day 2 (C1 posts), RIGHT to
# take it, ride the 3-leg haul to THORNBY (days 3/4/5), buy 2 resin on
# the impact ladder, RIGHT to hand them over (day 5 <= deadline 8),
# then wait out both windows: day 10 posts C2, day 15 lapses it unpaid.
# Assert frames sit >= 4 after their edge; pact-line TEXT asserts allow
# for the second-quiet-frame regen (see the header note).
PACT_ROUTE='--keys 10-12:START --keys 16-18:UP --keys 30-32:SELECT --keys 42-44:RIGHT --keys 48-50:R --keys 54-56:R --keys 60-62:R --keys 66-68:A --keys 72-74:A --keys 78-80:RIGHT --keys 84-86:SELECT --keys 90-92:SELECT --keys 96-98:SELECT --keys 102-104:SELECT --keys 108-110:SELECT --keys 120-122:SELECT --keys 126-128:SELECT --keys 132-134:SELECT --keys 138-140:SELECT --keys 144-146:SELECT'

W32='--elf games/wickroad/wickroad.elf --watch wr:wr_telemetry:32'

PACT_ASSERTS=(
  # day 1: trading, cursor parked on RESIN, no contract posted yet —
  # witness words all zero, EMBERTON resin at the mirror's 26
  --assert-watch 24:wr:2:eq:1
  --assert-watch 24:wr:4:eq:1
  --assert-watch 24:wr:5:eq:60
  --assert-watch 24:wr:7:eq:3
  --assert-watch 24:wr:13:eq:26
  --assert-watch 24:wr:24:eq:0
  --assert-watch 24:wr:25:eq:0
  --assert-watch 24:wr:31:eq:0
  --assert-text "24:NO WORD ON THE ROAD"
  --assert-text "24:NO PACTS POSTED"
  # day 2: C1 POSTED (2 RESIN to THORNBY by day 8, pays 60) — the whole
  # offer pinned in words 24-30 BEFORE any player action (state 1)
  --assert-watch 36:wr:4:eq:2
  --assert-watch 36:wr:5:eq:59
  --assert-watch 36:wr:24:eq:1
  --assert-watch 36:wr:25:eq:1
  --assert-watch 36:wr:26:eq:3
  --assert-watch 36:wr:27:eq:3
  --assert-watch 36:wr:28:eq:2
  --assert-watch 36:wr:29:eq:8
  --assert-watch 36:wr:30:eq:60
  --assert-watch 36:wr:31:eq:0
  --assert-text "36:PACT? 2 RESIN TO THORNBY D8 +60"
  # RIGHT takes the pact: state 1 -> 2, nothing else moves
  --assert-watch 46:wr:25:eq:2
  --assert-watch 46:wr:5:eq:59
  --assert-text "46:PACT: 2 RESIN TO THORNBY D8 +60"
  # day 5, THORNBY after the 3-leg haul: gold 53 (three 2-gold tolls),
  # resin at the mirror's 21
  --assert-watch 64:wr:4:eq:5
  --assert-watch 64:wr:5:eq:53
  --assert-watch 64:wr:6:eq:3
  --assert-watch 64:wr:13:eq:21
  # two resin bought on the impact ladder (21 then 22): gold 10, the
  # cursor word climbed 21 -> 22 -> 23, two in the pack
  --assert-watch 76:wr:5:eq:10
  --assert-watch 76:wr:8:eq:2
  --assert-watch 76:wr:12:eq:2
  --assert-watch 76:wr:13:eq:23
  --assert-watch 76:wr:25:eq:2
  # RIGHT hands them over (day 5 <= deadline 8): gold JUMPS 10 -> 70 —
  # exactly the authored premium 60 (state 3, word 31 banks it), the
  # pack empties, and the market is untouched by the handover
  --assert-watch 82:wr:5:eq:70
  --assert-watch 82:wr:8:eq:0
  --assert-watch 82:wr:12:eq:0
  --assert-watch 82:wr:25:eq:3
  --assert-watch 82:wr:31:eq:60
  --assert-text "84:PACT PAID +60"
  # day 10: C2 POSTED (2 IRON to GLASSMERE by day 14, pays 70) — the
  # witness words switch to the latest posted contract; the banked
  # premium stays on word 31; THORNBY resin decayed back to the plain
  # law (19: the delivery added no impact, only the buys did)
  --assert-watch 114:wr:4:eq:10
  --assert-watch 114:wr:5:eq:65
  --assert-watch 114:wr:24:eq:2
  --assert-watch 114:wr:25:eq:1
  --assert-watch 114:wr:26:eq:2
  --assert-watch 114:wr:27:eq:1
  --assert-watch 114:wr:28:eq:2
  --assert-watch 114:wr:29:eq:14
  --assert-watch 114:wr:30:eq:70
  --assert-watch 114:wr:31:eq:60
  --assert-watch 114:wr:13:eq:19
  --assert-text "116:PACT? 2 IRON TO GLASSMERE D14 +70"
  # day 15 > deadline 14: C2 LAPSES unpaid — state 4, word 31 unchanged
  # (the lapse pays nothing), gold 60 pins the whole route ledger, and
  # the standing-witness pair agrees with the mirror (salt 33 inked at
  # age 0 = 8448)
  --assert-watch 152:wr:4:eq:15
  --assert-watch 152:wr:5:eq:60
  --assert-watch 152:wr:24:eq:2
  --assert-watch 152:wr:25:eq:4
  --assert-watch 152:wr:31:eq:60
  --assert-watch 152:wr:13:eq:17
  --assert-watch 152:wr:14:eq:33
  --assert-watch 152:wr:15:eq:8448
  --assert-text "152:PACT LAPSED"
)

echo "== P5: THE CONTRACTS — the premium prices risk (run 1) =="
H "$OUT/p5.png" --frames 164 $W32 $PACT_ROUTE \
  --watch-log "$OUT/p5-run1.csv" --shot "84:$OUT/p5-paid.png" \
  "${PACT_ASSERTS[@]}"

echo "== P5: run 2 (must be byte-identical) =="
H "$OUT/p5b.png" --frames 164 $W32 $PACT_ROUTE \
  --watch-log "$OUT/p5-run2.csv" \
  "${PACT_ASSERTS[@]}"
cmp "$OUT/p5-run1.csv" "$OUT/p5-run2.csv"
echo "P5 run-twice: byte-identical"

echo "ALL WICKROAD PROOFS PASS"
