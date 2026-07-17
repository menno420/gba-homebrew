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
# The nine proofs (asserts inline below):
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
#   P6 THE ROAD ITSELF (growth cut 3) — travel cost stops being flat: the
#                              ROAD line telegraphs each authored hazard days
#                              ahead (H1 pinned on day 13, two days before
#                              its window), an UNGUARDED crossing of the
#                              bandit stretch loses EXACTLY the authored
#                              seizure (gold 45 -> 33, word 38 = 12), LEFT
#                              hires the guard (fee 4, word 37: 0 -> 1) and
#                              the guarded crossing of the SAME stretch on
#                              the SAME window loses nothing (37 back to 0,
#                              38 unchanged), the unprovisioned storm
#                              crossing loses exactly ONE DAY (day 18 -> 20
#                              on one travel edge, word 39 = 1, 1 gold camp
#                              lodging), the provisioned crossing of the
#                              same storm window loses none (day 20 -> 21),
#                              and day-23 gold 12 pins the whole route
#                              ledger (60 - 15 lodgings - 14 tolls - 12
#                              seized - 2 x 4 fees - 1 camp - 2 = see
#                              inline). H3's late announcement (day 23) and
#                              the day-23 THORNBY stale-ink pair (word 14 =
#                              34 true vs word 15 = 7939: 31 inked at age 3)
#                              close the loop with the cut-0 hook. RUN
#                              TWICE — byte-identical watch-logs. Every
#                              value was derived on the host-side mirror of
#                              the sim law FIRST. ROAD-line text asserts sit
#                              >= 4 frames after their edge: the road line
#                              regenerates on the THIRD quiet frame after a
#                              head redraw (crier first, pact second — dawn
#                              frames are budget-SPENT, see main.cpp), and
#                              the head redraw itself lands 1-2 frames after
#                              the key edge (a DETERMINISTIC parity of the
#                              input script, measured frame-by-frame on this
#                              route: the day-13 flip renders at edge+4, the
#                              day-23 flip at edge+5 — hence the 168 assert.
#                              Determinism makes the parity a constant of
#                              the committed route, not noise).
#
#   P7 THE WIDER MAP + THE MULES (growth cut 4) — gold buys logistics:
#                              the road runs past DUNWICK to HOLLOWFEN and
#                              MIRGATE (seven markets; visited-mask word 46
#                              pinned reaching 0x7F on one route), and at
#                              the Hollowfen fair START buys a mule at the
#                              posted authored price. Pinned exact: the
#                              day-1 baseline (capacity 8, no mules, fair
#                              closed, MIRGATE unvisited and its true iron
#                              price 38 already witnessed), the P2-prefix
#                              bank (gold 269 at day 9 — the same committed
#                              keys, proving the old world is bit-identical
#                              under the widened map), the fair posting 30
#                              on arrival (word 42, "MULE 30" on the town
#                              line), the purchase edge (gold 267 -> 237,
#                              capacity word 8 -> 12, ladder walks to 55),
#                              a NINE-salt pack (pack used 9 > the old cap
#                              of 8 — the upgrade provably carries), the
#                              MIRGATE arrival (mask 0x7F, ledger inked at
#                              8192 = iron 32 @ age 0), the 9-sell haul to
#                              gold 280 (under the 300 win by authorship),
#                              the ride back (day 12: MIRGATE ink now LIES
#                              — true 35 vs word 45 = 8193 = 32 @ age 1 —
#                              the cut-0 hook stretches to the new region),
#                              and the second mule (gold 223, capacity 16,
#                              word 42 = 0: sold out, spend word 43 = 85).
#                              Hazard words stay 0 throughout (every window
#                              opens day 15+; the route's last travel day
#                              is 12) and word 31 stays 0 (no pact taken —
#                              every gold piece is market + mule money).
#                              RUN TWICE — byte-identical watch-logs. Every
#                              value derived on the host-side mirror of the
#                              sim law FIRST (the route simulator replays
#                              the exact key schedule against the law).
#
#   P8 THE AUDIO (growth cut 5) — the cues are decisions, hook-counted:
#                              one buy + one sell on day 1 step the coin
#                              counter (word 48) to exactly 2 on their key
#                              edges (gold 60 -> 51 -> 60, the committed
#                              day-1 tallow 9 from the P2/P3 pins), thirty
#                              waits ring the dawn bell once per rolled day
#                              (word 49 = 10 at day 11, 29 at the close)
#                              until the pass-closing dawn fires the WIND
#                              exactly once (word 50 = 1, state 3, gold 30
#                              pinning all 30 lodgings), and a START
#                              restart provably KEEPS the boot's counters
#                              (words 48-51 unchanged across the reset —
#                              they are boot-cumulative like the frame
#                              word). Voicing evidence per the house
#                              method (docs/capabilities.md): the maxmod
#                              mixer-memory nonzero watch is 0 on the
#                              silent title, >0 while the coin and the
#                              wind sound, and 0 again once the wind dies.
#                              RUN TWICE — byte-identical watch-logs.
#                              HONEST LIMIT: only play-call decisions and
#                              mixer-memory activity are asserted — actual
#                              audible output (mix levels, timbre, the
#                              DAC) is not headlessly provable and is
#                              named owner-playtest territory in the PR.
#
#   P9 THE JUNCTION (crossroads cut 1) — the road forks: walk the spine
#                              EAST to the junction DUNWICK (day 5, gold
#                              52 — the four spine tolls, no hazard, all
#                              windows open day 15+), where the fork-edge
#                              witness word 53 reads 7 and the on-branch
#                              flag word 52 is still 0; the L+R CHORD
#                              ("take the fork") rides onto WYRMHOLLOW
#                              (town 7, day 6, gold 50 — word 52 -> 1,
#                              visited mask 0x9f: bit 7 set, the branch
#                              stood in); the branch MARKET obeys the same
#                              impact law (UP to RESIN at the mirror's 18,
#                              two buys climb the cursor word 18->19->20 as
#                              gold falls 50->32->13, two sells walk it
#                              back to gold 50), and plain L returns to
#                              DUNWICK (day 7, gold 48, word 52 -> 0). The
#                              fork-edge word 53 reads 7 ONLY at the two
#                              ends of the fork road (junction + branch)
#                              and 0 at a mid-spine town (GLASSMERE) — the
#                              branch is reachable ONLY by the chord, never
#                              by a spine R. Word 54 pins the branch RESIN
#                              price against the host mirror even from
#                              across the spine. RUN TWICE — byte-identical
#                              watch-logs. Every value derived on the mirror
#                              FIRST; the ROM matched on the first probe.
#
# Mailbox: wr_telemetry[56] since the crossroads cut (layout in
# games/wickroad/src/main.cpp;
# P1-P3 keep watching the first 16 words, unchanged from v0.1; P4 the first
# 24, unchanged from v0.2; P5 the first 32, unchanged from v0.3; P6 the
# first 40, unchanged from v0.4; P7 asserts nothing past word 47, unchanged
# from v0.5 — the audio counter words 48-51 are new in v0.6 and words 0-47
# stayed byte-identical: P1-P7 passed verbatim on the first post-audio run,
# zero re-pins. Crossroads cut 1 appended words 52-55 behind the frozen
# 0-51 and +8 RNG draws behind town 6's: P1-P8 passed verbatim again,
# zero re-pins — the same append-only wire-format discipline.)
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
W52='--elf games/wickroad/wickroad.elf --watch wr:wr_telemetry:52'
W56='--elf games/wickroad/wickroad.elf --watch wr:wr_telemetry:56'
# Audio voicing evidence (growth cut 5, the Shoal/Courier house method):
# count of nonzero u32 words in the maxmod mixing buffer — 0 exactly when
# the mixer is silent.
WMIX='--watch-nonzero mix:maxmod_mixing_buffer:0x420'

echo "== P1: boot + title =="
H "$OUT/p1.png" --frames 70 $W52 \
  --assert-watch 60:wr:0:eq:1464419147 \
  --assert-watch 60:wr:1:eq:1380925764 \
  --assert-watch 60:wr:2:eq:0 \
  --assert-watch 60:wr:4:eq:0 \
  --assert-watch 60:wr:5:eq:0 \
  --assert-watch 60:wr:13:eq:0 \
  --assert-watch 60:wr:14:eq:0 \
  --assert-watch 60:wr:15:eq:0 \
  --assert-watch 60:wr:40:eq:0 \
  --assert-watch 60:wr:42:eq:0 \
  --assert-watch 60:wr:46:eq:0 \
  --assert-watch 60:wr:48:eq:0 \
  --assert-watch 60:wr:49:eq:0 \
  --assert-watch 60:wr:50:eq:0 \
  --assert-watch 60:wr:51:eq:0 \
  --assert-text "60:WICKROAD" \
  --assert-text "60:ONE ROAD - SEVEN MARKETS" \
  --assert-text "60:BUY LOW - SELL HIGH" \
  --assert-text "60:300 GOLD BEFORE DAY 30" \
  --assert-text "60:YOUR LEDGER REMEMBERS" \
  --assert-text "60:BUT THE INK AGES" \
  --assert-text "60:PRESS START" \
  --assert-text "60:A BUY  B SELL  L/R GO  SEL WAIT" \
  --assert-text "60:LEFT HIRES THE GUARD" \
  --assert-text "60:THE HOLLOWFEN FAIR SELLS MULES"

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

# The road route: START, 13 SELECT waits to day 14 (H1 announced day 13,
# window D15-16 on the GLASSMERE road), R into the window UNGUARDED (the
# bandits take their authored 12), LEFT hires the guard (fee 4), L back
# across the SAME stretch guarded (nothing taken, the guard consumed),
# R/R/R east — the last leg arrives THORNBY day 19 inside the storm
# window D19-22 UNPROVISIONED (one day camped: day 18 -> 20 on one edge,
# 1 gold lodging), LEFT again, L back across the storm provisioned (day
# 21, no delay), then two waits to day 23 (H3 announced). Route gold
# ledger, every step on the mirror: 60 - 13 lodgings = 47; - 2 toll -
# 12 seized = 33; - 4 fee = 29; - 2 toll = 27; - 2 - 2 = 23; - 2 toll -
# 1 camp = 20; - 4 fee = 16; - 2 toll = 14; - 1 - 1 = 12 at day 23.
ROAD_ROUTE='--keys 10-12:START --keys 30-32:SELECT --keys 36-38:SELECT --keys 42-44:SELECT --keys 48-50:SELECT --keys 54-56:SELECT --keys 60-62:SELECT --keys 66-68:SELECT --keys 72-74:SELECT --keys 78-80:SELECT --keys 84-86:SELECT --keys 90-92:SELECT --keys 96-98:SELECT --keys 102-104:SELECT --keys 108-110:R --keys 114-116:LEFT --keys 120-122:L --keys 126-128:R --keys 132-134:R --keys 138-140:R --keys 144-146:LEFT --keys 150-152:L --keys 156-158:SELECT --keys 162-164:SELECT'

W40='--elf games/wickroad/wickroad.elf --watch wr:wr_telemetry:40'

ROAD_ASSERTS=(
  # day 1: trading, nothing announced — hazard words all zero, the road
  # quiet on screen, cursor TALLOW at the mirror's 9 (the P2 pin)
  --assert-watch 24:wr:2:eq:1
  --assert-watch 24:wr:4:eq:1
  --assert-watch 24:wr:5:eq:60
  --assert-watch 24:wr:13:eq:9
  --assert-watch 24:wr:32:eq:0
  --assert-watch 24:wr:37:eq:0
  --assert-watch 24:wr:38:eq:0
  --assert-watch 24:wr:39:eq:0
  --assert-text "24:THE ROAD LIES QUIET"
  # day 13: H1 ANNOUNCED two days before its window — the whole hazard
  # pinned in words 32-36 while nothing has been lost yet
  --assert-watch 100:wr:4:eq:13
  --assert-watch 100:wr:5:eq:48
  --assert-watch 100:wr:32:eq:1
  --assert-watch 100:wr:33:eq:0
  --assert-watch 100:wr:34:eq:0
  --assert-watch 100:wr:35:eq:15
  --assert-watch 100:wr:36:eq:16
  --assert-watch 100:wr:37:eq:0
  --assert-watch 100:wr:38:eq:0
  --assert-text "100:RAID: GLASSMERE RD D15-16"
  # day 14, the eve: gold 47 after 13 lodgings
  --assert-watch 106:wr:4:eq:14
  --assert-watch 106:wr:5:eq:47
  # the UNGUARDED crossing, arrival day 15 inside D15-16: toll 2 then
  # the authored seizure 12 — gold 47 -> 33 on one edge, word 38 = 12
  --assert-watch 112:wr:4:eq:15
  --assert-watch 112:wr:6:eq:1
  --assert-watch 112:wr:5:eq:33
  --assert-watch 112:wr:38:eq:12
  --assert-watch 112:wr:39:eq:0
  # LEFT hires the guard: fee 4, word 37: 0 -> 1, on screen
  --assert-watch 118:wr:5:eq:29
  --assert-watch 118:wr:37:eq:1
  --assert-text "118:RAID: GLASSMERE RD D15-16 +GUARD"
  # the GUARDED crossing of the SAME stretch, arrival day 16 still
  # inside the window: toll only — word 38 unchanged, the guard consumed
  --assert-watch 124:wr:4:eq:16
  --assert-watch 124:wr:6:eq:0
  --assert-watch 124:wr:5:eq:27
  --assert-watch 124:wr:37:eq:0
  --assert-watch 124:wr:38:eq:12
  # day 17: the window is over — the same road crosses clean unguarded
  --assert-watch 130:wr:4:eq:17
  --assert-watch 130:wr:6:eq:1
  --assert-watch 130:wr:5:eq:25
  --assert-watch 130:wr:38:eq:12
  # day 18 at SALTCOMBE: H2 ANNOUNCED (storm, THORNBY road, D19-22)
  --assert-watch 136:wr:4:eq:18
  --assert-watch 136:wr:6:eq:2
  --assert-watch 136:wr:5:eq:23
  --assert-watch 136:wr:32:eq:2
  --assert-watch 136:wr:33:eq:1
  --assert-watch 136:wr:34:eq:2
  --assert-watch 136:wr:35:eq:19
  --assert-watch 136:wr:36:eq:22
  --assert-text "136:STORM: THORNBY RD D19-22"
  # the UNPROVISIONED storm crossing: ONE travel edge, TWO dawns — day
  # 18 -> 20 (word 39 = 1), toll 2 + 1 camp lodging, arrival THORNBY
  --assert-watch 142:wr:4:eq:20
  --assert-watch 142:wr:6:eq:3
  --assert-watch 142:wr:5:eq:20
  --assert-watch 142:wr:39:eq:1
  --assert-watch 142:wr:38:eq:12
  # LEFT again: provisions for the ride back
  --assert-watch 148:wr:5:eq:16
  --assert-watch 148:wr:37:eq:1
  # the PROVISIONED crossing of the same storm window: day 20 -> 21,
  # one dawn only — word 39 unchanged, the fee bought the day back
  --assert-watch 154:wr:4:eq:21
  --assert-watch 154:wr:6:eq:2
  --assert-watch 154:wr:5:eq:14
  --assert-watch 154:wr:37:eq:0
  --assert-watch 154:wr:39:eq:1
  # day 23: H3 ANNOUNCED (bandits, DUNWICK road, D25-28, late and fat);
  # gold 12 pins the whole route ledger; the cut-0 stale-ink pair still
  # bites (THORNBY salt: true 34, ink says 31 at age 3 = 7939) and the
  # cursor word matches the mirror's SALTCOMBE tallow 16
  --assert-watch 166:wr:4:eq:23
  --assert-watch 166:wr:6:eq:2
  --assert-watch 166:wr:5:eq:12
  --assert-watch 166:wr:13:eq:16
  --assert-watch 166:wr:14:eq:34
  --assert-watch 166:wr:15:eq:7939
  --assert-watch 166:wr:32:eq:3
  --assert-watch 166:wr:33:eq:0
  --assert-watch 166:wr:34:eq:3
  --assert-watch 166:wr:35:eq:25
  --assert-watch 166:wr:36:eq:28
  --assert-watch 166:wr:38:eq:12
  --assert-watch 166:wr:39:eq:1
  --assert-text "168:RAID: DUNWICK RD D25-28"
)

echo "== P6: THE ROAD ITSELF — travel cost stops being flat (run 1) =="
H "$OUT/p6.png" --frames 172 $W40 $ROAD_ROUTE \
  --watch-log "$OUT/p6-run1.csv" --shot "118:$OUT/p6-guard.png" \
  "${ROAD_ASSERTS[@]}"

echo "== P6: run 2 (must be byte-identical) =="
H "$OUT/p6b.png" --frames 172 $W40 $ROAD_ROUTE \
  --watch-log "$OUT/p6-run2.csv" \
  "${ROAD_ASSERTS[@]}"
cmp "$OUT/p6-run1.csv" "$OUT/p6-run2.csv"
echo "P6 run-twice: byte-identical"

# The wider-map route: the committed P2 prefix through the day-9 DUNWICK
# iron sale (gold 269 — the same keys, byte-for-byte, proving the legacy
# world carries under the widened map), then EAST past the old end of the
# road: R to HOLLOWFEN day 10 (fair posts MULE 30), START buys mule 1
# (capacity 12), UP parks the cursor on SALT (the fair town's cheap
# produce, 10 + the impact ladder), 9 buys fill the pack PAST the old cap
# of 8, R to MIRGATE day 11 (all seven towns visited: mask 0x7F; iron
# inked 32 at age 0), 9 sells bank gold 280 (authored under the 300 win),
# L back day 12 (MIRGATE's ink now provably lies: true 35 vs 32 @ age 1),
# START buys mule 2 (capacity 16, the fair sells out: word 42 -> 0).
# Route gold ledger, every step on the mirror: 60 -55 tallow -2 -2 tolls
# +91 sold -66 iron -2 -2 +138 sold -108 tallow -2 -2 +112 sold -134 iron
# -2 -2 +257 sold = 269 day 9; -2 toll -30 mule -126 salt = 111; -2 toll
# +171 sold = 280; -2 toll -55 mule = 223 day 12.
WIDE_ROUTE='--keys 10-12:START --keys 30-32:A --keys 36-38:A --keys 42-44:A --keys 48-50:A --keys 54-56:A --keys 60-62:R --keys 66-68:R --keys 72-74:B --keys 78-80:B --keys 84-86:B --keys 90-92:B --keys 96-98:B --keys 102-104:DOWN --keys 108-110:DOWN --keys 114-116:A --keys 120-122:A --keys 126-128:A --keys 132-134:R --keys 138-140:R --keys 144-146:B --keys 150-152:B --keys 156-158:B --keys 162-164:UP --keys 168-170:UP --keys 174-176:A --keys 180-182:A --keys 186-188:A --keys 192-194:A --keys 198-200:A --keys 204-206:A --keys 210-212:A --keys 216-218:A --keys 222-224:L --keys 228-230:L --keys 234-236:B --keys 240-242:B --keys 246-248:B --keys 252-254:B --keys 258-260:B --keys 264-266:B --keys 270-272:B --keys 276-278:B --keys 282-284:DOWN --keys 288-290:DOWN --keys 294-296:A --keys 300-302:A --keys 306-308:A --keys 312-314:A --keys 318-320:A --keys 324-326:R --keys 330-332:R --keys 336-338:B --keys 342-344:B --keys 348-350:B --keys 354-356:B --keys 360-362:B --keys 372-374:R --keys 378-380:START --keys 384-386:UP --keys 390-392:A --keys 396-398:A --keys 402-404:A --keys 408-410:A --keys 414-416:A --keys 420-422:A --keys 426-428:A --keys 432-434:A --keys 438-440:A --keys 444-446:R --keys 450-452:B --keys 456-458:B --keys 462-464:B --keys 468-470:B --keys 474-476:B --keys 480-482:B --keys 486-488:B --keys 492-494:B --keys 498-500:B --keys 504-506:L --keys 510-512:START'

WIDE_ASSERTS=(
  # day 1: capacity 8, no mules, the fair closed, only EMBERTON stood
  # in (mask 1), and MIRGATE already witnessed: true iron 38 (word 44,
  # the mirror's day-1 value), ledger word 0 — unvisited. The widened
  # ledger is on screen from the first quiet frames: both new towns'
  # unvisited rows exact.
  --assert-watch 24:wr:2:eq:1
  --assert-watch 24:wr:4:eq:1
  --assert-watch 24:wr:5:eq:60
  --assert-watch 24:wr:40:eq:8
  --assert-watch 24:wr:41:eq:0
  --assert-watch 24:wr:42:eq:0
  --assert-watch 24:wr:43:eq:0
  --assert-watch 24:wr:44:eq:38
  --assert-watch 24:wr:45:eq:0
  --assert-watch 24:wr:46:eq:1
  --assert-watch 24:wr:47:eq:8
  # (measured fact, first P7 probe: the ledger/table rows render in the
  # FIXED 8x8 font, and --assert-text templates come from the VARIABLE
  # font bmp — tools/headless-screenshot.py FONT_BMP — so fixed-font
  # rows are not text-assertable; P1-P6 never asserted one either. The
  # new towns' ledger states are pinned exactly via words 44/45 instead.)
  # day 9, the committed P2 prefix banked: gold 269 at DUNWICK, pack
  # empty — the legacy world is bit-identical under the widened map
  # (same keys, same gold, the P2 pins' own values)
  --assert-watch 366:wr:4:eq:9
  --assert-watch 366:wr:5:eq:269
  --assert-watch 366:wr:6:eq:4
  --assert-watch 366:wr:8:eq:0
  --assert-watch 366:wr:40:eq:8
  --assert-watch 366:wr:46:eq:31
  # R past the old end of the road: HOLLOWFEN day 10 — the fair posts
  # the first mule at the authored 30 (word 42 + the town line)
  --assert-watch 376:wr:4:eq:10
  --assert-watch 376:wr:5:eq:267
  --assert-watch 376:wr:6:eq:5
  --assert-watch 376:wr:40:eq:8
  --assert-watch 376:wr:41:eq:0
  --assert-watch 376:wr:42:eq:30
  --assert-watch 376:wr:44:eq:29
  --assert-watch 376:wr:45:eq:0
  --assert-watch 376:wr:46:eq:63
  --assert-text "376:AT HOLLOWFEN  PACK 0/8  MULE 30"
  # START buys mule 1: gold 267 -> 237, capacity 8 -> 12, the ladder
  # posts 55, spend word banks 30
  --assert-watch 382:wr:5:eq:237
  --assert-watch 382:wr:40:eq:12
  --assert-watch 382:wr:41:eq:1
  --assert-watch 382:wr:42:eq:55
  --assert-watch 382:wr:43:eq:30
  --assert-watch 382:wr:47:eq:12
  --assert-text "382:AT HOLLOWFEN  PACK 0/12  MULE 55"
  # nine salt on the impact ladder (10..18, the fair town's cheap
  # produce): pack used 9 — PAST the old cap of 8, the upgrade carries
  --assert-watch 442:wr:5:eq:111
  --assert-watch 442:wr:8:eq:9
  --assert-watch 442:wr:10:eq:9
  --assert-watch 442:wr:13:eq:19
  --assert-watch 442:wr:40:eq:12
  --assert-watch 442:wr:47:eq:3
  # MIRGATE day 11 — ALL SEVEN towns stood in (mask 0x7F), the far
  # market inked at 8192 = iron 32 @ age 0, no fair here (word 42 = 0)
  --assert-watch 448:wr:4:eq:11
  --assert-watch 448:wr:5:eq:109
  --assert-watch 448:wr:6:eq:6
  --assert-watch 448:wr:42:eq:0
  --assert-watch 448:wr:44:eq:32
  --assert-watch 448:wr:45:eq:8192
  --assert-watch 448:wr:46:eq:127
  --assert-text "448:AT MIRGATE  PACK 9/12"
  # the 9-sell haul: gold 280 — authored UNDER the 300 win, so the
  # route keeps trading; pack empty again
  --assert-watch 502:wr:5:eq:280
  --assert-watch 502:wr:8:eq:0
  --assert-watch 502:wr:47:eq:12
  # L back day 12: MIRGATE's ink now provably LIES — true iron 35
  # (word 44) vs ledger word 8193 = 32 @ age 1 — the cut-0 hook
  # stretches to the new region; the THORNBY pair still ages too
  # (word 15 = 7940: 31 @ age 4); hazard words untouched all route
  # (every window opens day 15+) and word 31 = 0 (no pact taken)
  --assert-watch 508:wr:4:eq:12
  --assert-watch 508:wr:5:eq:278
  --assert-watch 508:wr:6:eq:5
  --assert-watch 508:wr:14:eq:34
  --assert-watch 508:wr:15:eq:7940
  --assert-watch 508:wr:31:eq:0
  --assert-watch 508:wr:32:eq:0
  --assert-watch 508:wr:38:eq:0
  --assert-watch 508:wr:42:eq:55
  --assert-watch 508:wr:44:eq:35
  --assert-watch 508:wr:45:eq:8193
  # START buys mule 2: gold 278 -> 223, capacity 16, the fair SELLS
  # OUT (word 42 -> 0), spend word 85 — the whole Taipan curve pinned
  --assert-watch 514:wr:5:eq:223
  --assert-watch 514:wr:40:eq:16
  --assert-watch 514:wr:41:eq:2
  --assert-watch 514:wr:42:eq:0
  --assert-watch 514:wr:43:eq:85
  --assert-watch 514:wr:46:eq:127
  --assert-watch 514:wr:47:eq:16
  --assert-text "520:AT HOLLOWFEN  PACK 0/16"
)

echo "== P7: THE WIDER MAP + THE MULES — gold buys logistics (run 1) =="
H "$OUT/p7.png" --frames 530 $W52 $WIDE_ROUTE \
  --watch-log "$OUT/p7-run1.csv" --shot "520:$OUT/p7-mules.png" \
  "${WIDE_ASSERTS[@]}"

echo "== P7: run 2 (must be byte-identical) =="
H "$OUT/p7b.png" --frames 530 $W52 $WIDE_ROUTE \
  --watch-log "$OUT/p7-run2.csv" \
  "${WIDE_ASSERTS[@]}"
cmp "$OUT/p7-run1.csv" "$OUT/p7-run2.csv"
echo "P7 run-twice: byte-identical"

# The audio route (growth cut 5): START, one buy + one sell on day 1
# (the coin chink on both edges of a trade — the committed day-1 tallow
# 9: buy at 9, the impact ladder lifts it to 10, sell pays 10-1=9, gold
# 60 -> 51 -> 60), then 30 SELECT waits ring the dawn bell once per
# rolled day until the 30th wait rolls day 31 and the PASS-CLOSING WIND
# fires exactly once (state 3; gold 30 pins all 30 lodgings). START at
# 240 restarts the run — and the counters provably SURVIVE it (words
# 48-51 are boot-cumulative like the frame word, by design). The mixer
# watch pins the voicing: silent title, audible coin, audible wind,
# silence again once the wind dies (~1.1 s after the close).
# HONEST LIMIT (also in the PR): these asserts prove the play-call
# DECISIONS (hook counters) and maxmod mixer-memory activity — actual
# audible output (mix levels, timbre, the DAC path) cannot be asserted
# headlessly and is named owner-playtest territory.
AUDIO_ROUTE='--keys 10-12:START --keys 30-32:A --keys 36-38:B --keys 42-44:SELECT --keys 48-50:SELECT --keys 54-56:SELECT --keys 60-62:SELECT --keys 66-68:SELECT --keys 72-74:SELECT --keys 78-80:SELECT --keys 84-86:SELECT --keys 90-92:SELECT --keys 96-98:SELECT --keys 102-104:SELECT --keys 108-110:SELECT --keys 114-116:SELECT --keys 120-122:SELECT --keys 126-128:SELECT --keys 132-134:SELECT --keys 138-140:SELECT --keys 144-146:SELECT --keys 150-152:SELECT --keys 156-158:SELECT --keys 162-164:SELECT --keys 168-170:SELECT --keys 174-176:SELECT --keys 180-182:SELECT --keys 186-188:SELECT --keys 192-194:SELECT --keys 198-200:SELECT --keys 204-206:SELECT --keys 210-212:SELECT --keys 216-218:SELECT --keys 240-242:START'

AUDIO_ASSERTS=(
  # the title is SILENT: no cue has ever fired, the mixer is empty
  --assert-watch 8:wr:2:eq:0
  --assert-watch 8:wr:48:eq:0
  --assert-watch 8:wr:51:eq:0
  --assert-watch 8:mix:0:eq:0
  # day 1 before the trade: run started (no start cue by design — the
  # deck is coin/bell/wind, CONCEPT.md verbatim), counters still zero
  --assert-watch 24:wr:2:eq:1
  --assert-watch 24:wr:4:eq:1
  --assert-watch 24:wr:5:eq:60
  --assert-watch 24:wr:13:eq:9
  --assert-watch 24:wr:48:eq:0
  --assert-watch 24:wr:49:eq:0
  --assert-watch 24:wr:50:eq:0
  --assert-watch 24:wr:51:eq:0
  # the BUY edge: gold 60 -> 51 (the committed tallow 9), one in the
  # pack — and the COIN counter steps to 1 on the same decision; the
  # mixer is audibly voicing it 4 frames in (the chink runs ~0.15 s)
  --assert-watch 34:wr:5:eq:51
  --assert-watch 34:wr:9:eq:1
  --assert-watch 34:wr:48:eq:1
  --assert-watch 34:wr:49:eq:0
  --assert-watch 34:wr:51:eq:1
  --assert-watch 34:mix:0:gt:0
  # the SELL edge: the impact ladder pays 10-1=9 back (gold 60), the
  # pack empties — the coin steps to 2: BOTH sides of a trade chink
  --assert-watch 40:wr:5:eq:60
  --assert-watch 40:wr:8:eq:0
  --assert-watch 40:wr:48:eq:2
  --assert-watch 40:wr:51:eq:2
  # the first wait: day 2, one lodging — the DAWN BELL rings once
  --assert-watch 46:wr:4:eq:2
  --assert-watch 46:wr:5:eq:59
  --assert-watch 46:wr:48:eq:2
  --assert-watch 46:wr:49:eq:1
  --assert-watch 46:wr:50:eq:0
  --assert-watch 46:wr:51:eq:3
  # day 11 (ten waits in): the bell has rung once per rolled day
  --assert-watch 100:wr:4:eq:11
  --assert-watch 100:wr:5:eq:50
  --assert-watch 100:wr:49:eq:10
  --assert-watch 100:wr:51:eq:12
  # the 30th wait rolls day 31: the pass CLOSES — the WIND fires
  # exactly once, the bell stops at 29 (the closing dawn howls, it
  # does not ring), gold 30 pins all 30 lodgings
  --assert-watch 222:wr:2:eq:3
  --assert-watch 222:wr:4:eq:31
  --assert-watch 222:wr:5:eq:30
  --assert-watch 222:wr:48:eq:2
  --assert-watch 222:wr:49:eq:29
  --assert-watch 222:wr:50:eq:1
  --assert-watch 222:wr:51:eq:32
  --assert-watch 222:mix:0:gt:0
  --assert-text "232:THE PASS CLOSES"
  --assert-text "232:GOLD 30 OF 300"
  # START restarts the run (day 1, gold 60) — and the audio history
  # SURVIVES: the counters are boot-cumulative, not run state
  --assert-watch 260:wr:2:eq:1
  --assert-watch 260:wr:4:eq:1
  --assert-watch 260:wr:5:eq:60
  --assert-watch 260:wr:48:eq:2
  --assert-watch 260:wr:49:eq:29
  --assert-watch 260:wr:50:eq:1
  --assert-watch 260:wr:51:eq:32
  # the wind (~1.1 s from the close) has died: the mixer is silent
  # again — cues are one-shots, nothing loops
  --assert-watch 300:mix:0:eq:0
)

echo "== P8: THE AUDIO — the cues are decisions, hook-counted (run 1) =="
H "$OUT/p8.png" --frames 310 $W52 $WMIX $AUDIO_ROUTE \
  --watch-log "$OUT/p8-run1.csv" --shot "232:$OUT/p8-closed.png" \
  "${AUDIO_ASSERTS[@]}"

echo "== P8: run 2 (must be byte-identical) =="
H "$OUT/p8b.png" --frames 310 $W52 $WMIX $AUDIO_ROUTE \
  --watch-log "$OUT/p8-run2.csv" \
  "${AUDIO_ASSERTS[@]}"
cmp "$OUT/p8-run1.csv" "$OUT/p8-run2.csv"
echo "P8 run-twice: byte-identical"

# The junction route (crossroads cut 1): START, R x4 EAST along the spine
# to the junction DUNWICK (day 5, gold 52 — four 2-gold tolls, every
# hazard window opens day 15+ so the road is quiet), the L+R CHORD onto
# the branch WYRMHOLLOW (day 6, gold 50), UP parks the cursor on RESIN
# (the branch's cheap produce, town 7 % 4 = good 3), two buys on the
# impact ladder (18 then 19) and two sells back (20-1 then 19-1) — a
# clean round trip through the branch market that ends where it began
# (gold 50), then plain L back to DUNWICK (day 7, gold 48). The two
# shoulder keys on the SAME frame span (54-56:L + 54-56:R) are the chord;
# no committed route ever presses both, so the verb is purely additive.
# Route gold ledger, every step on the host mirror: 60 - 2*4 spine tolls
# = 52 at DUNWICK d5; -2 fork = 50 at WYRMHOLLOW d6; -18 -19 buys = 13;
# +19 +18 sells = 50; -2 L = 48 at DUNWICK d7.
JUNCTION_ROUTE='--keys 10-12:START --keys 30-32:R --keys 36-38:R --keys 42-44:R --keys 48-50:R --keys 54-56:L --keys 54-56:R --keys 60-62:UP --keys 66-68:A --keys 72-74:A --keys 78-80:B --keys 84-86:B --keys 90-92:L'

JUNCTION_ASSERTS=(
  # day 1 EMBERTON: trading, on the spine — the on-branch flag word 52
  # and the fork-edge word 53 are both 0 (EMBERTON is a plain spine end,
  # no fork), and word 54 already pins the branch RESIN price from across
  # the whole spine (the mirror's day-1 WYRMHOLLOW RESIN = 24)
  --assert-watch 24:wr:2:eq:1
  --assert-watch 24:wr:4:eq:1
  --assert-watch 24:wr:5:eq:60
  --assert-watch 24:wr:6:eq:0
  --assert-watch 24:wr:13:eq:9
  --assert-watch 24:wr:52:eq:0
  --assert-watch 24:wr:53:eq:0
  --assert-watch 24:wr:54:eq:24
  --assert-watch 24:wr:55:eq:0
  # R to GLASSMERE day 2: a mid-spine town — NO fork here (word 53 = 0),
  # so the branch is provably unreachable from anywhere but the junction
  --assert-watch 34:wr:4:eq:2
  --assert-watch 34:wr:5:eq:58
  --assert-watch 34:wr:6:eq:1
  --assert-watch 34:wr:52:eq:0
  --assert-watch 34:wr:53:eq:0
  # R R to DUNWICK day 5, gold 52 — THE JUNCTION: the fork-edge word 53
  # lights to 7 (a fork is live from here) while word 52 is still 0 (not
  # yet on the branch); word 54 = the mirror's day-5 branch RESIN 16;
  # visited mask 0x1f = the five spine towns 0..4
  --assert-watch 52:wr:4:eq:5
  --assert-watch 52:wr:5:eq:52
  --assert-watch 52:wr:6:eq:4
  --assert-watch 52:wr:46:eq:31
  --assert-watch 52:wr:52:eq:0
  --assert-watch 52:wr:53:eq:7
  --assert-watch 52:wr:54:eq:16
  --assert-text "52:AT DUNWICK  PACK 0/8"
  # THE FORK: the L+R chord rides onto WYRMHOLLOW (town 7) day 6, gold 50
  # — word 52 flips 0 -> 1, the fork-edge word 53 still 7 (now the branch
  # end of the same road), the visited mask sets bit 7 (0x1f | 0x80 =
  # 0x9f = 159), and the header names the branch town (variable font)
  --assert-watch 58:wr:4:eq:6
  --assert-watch 58:wr:5:eq:50
  --assert-watch 58:wr:6:eq:7
  --assert-watch 58:wr:8:eq:0
  --assert-watch 58:wr:46:eq:159
  --assert-watch 58:wr:52:eq:1
  --assert-watch 58:wr:53:eq:7
  --assert-watch 58:wr:54:eq:18
  --assert-text "58:AT WYRMHOLLOW  PACK 0/8"
  # UP parks the cursor on RESIN (good 3): the visible market word 13 =
  # the mirror's branch RESIN 18, impact-free — the branch is a real
  # market read from the same price law
  --assert-watch 64:wr:7:eq:3
  --assert-watch 64:wr:13:eq:18
  # two buys climb the impact ladder (18 then 19): gold 50 -> 32 -> 13,
  # pack used 0 -> 1 -> 2, RESIN cargo (word 12) 2, the cursor word
  # walking 18 -> 19 -> 20 exactly as any spine market
  --assert-watch 70:wr:5:eq:32
  --assert-watch 70:wr:8:eq:1
  --assert-watch 70:wr:12:eq:1
  --assert-watch 70:wr:13:eq:19
  --assert-watch 76:wr:5:eq:13
  --assert-watch 76:wr:8:eq:2
  --assert-watch 76:wr:12:eq:2
  --assert-watch 76:wr:13:eq:20
  # two sells walk it back down (20-1 then 19-1): gold 13 -> 32 -> 50,
  # pack empties — the branch market's impact decays symmetrically, the
  # round trip nets zero, the law is the same one
  --assert-watch 82:wr:5:eq:32
  --assert-watch 82:wr:8:eq:1
  --assert-watch 88:wr:5:eq:50
  --assert-watch 88:wr:8:eq:0
  # plain L rides back to the junction DUNWICK (day 7, gold 48): word 52
  # -> 0 (off the branch), word 6 = 4, the fork-edge word 53 still 7
  # (standing at the junction again); mask keeps bit 7 (WYRMHOLLOW stays
  # a visited town), word 55 reserved and 0 all route
  --assert-watch 94:wr:4:eq:7
  --assert-watch 94:wr:5:eq:48
  --assert-watch 94:wr:6:eq:4
  --assert-watch 94:wr:46:eq:159
  --assert-watch 94:wr:52:eq:0
  --assert-watch 94:wr:53:eq:7
  --assert-watch 94:wr:55:eq:0
)

echo "== P9: THE JUNCTION — the road forks, reached by the L+R chord (run 1) =="
H "$OUT/p9.png" --frames 100 $W56 $JUNCTION_ROUTE \
  --watch-log "$OUT/p9-run1.csv" --shot "58:$OUT/p9-branch.png" \
  "${JUNCTION_ASSERTS[@]}"

echo "== P9: run 2 (must be byte-identical) =="
H "$OUT/p9b.png" --frames 100 $W56 $JUNCTION_ROUTE \
  --watch-log "$OUT/p9-run2.csv" \
  "${JUNCTION_ASSERTS[@]}"
cmp "$OUT/p9-run1.csv" "$OUT/p9-run2.csv"
echo "P9 run-twice: byte-identical"

echo "ALL WICKROAD PROOFS PASS"
