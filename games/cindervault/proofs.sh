#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# CINDERVAULT — the committed headless proof suite (mGBA,
# tools/headless-screenshot.py), ported with the item-layer slice (growth
# cut 1). The prototype's proofs (.sessions/2026-07-13-cindervault.md) lived
# only in that session's container; per the Shoal/Courier convention the
# input scripts ARE the pin, so from now on they are committed here.
#
# Pin provenance (the prototype card's 💡 recipe, now with the mirror
# COMMITTED): every route below was derived by the route bot in
# games/cindervault/tools/mirror.py and every literal transcribed from the
# mirror's per-turn trace — after the mirror was certified against a real
# emulator watch-log of the SAME routes turn-for-turn (0 mismatches across
# all 75 turn states, this build). Nothing is hand-computed.
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
#
# Item-spawn determinism is pinned structurally: both routes walk blind,
# fixed key scripts onto the item tiles of seed 0xC1DE5EED and assert the
# slot word flips at the exact scripted turn — a shifted spawn fails loud.
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
  # turn 38: about to engage — kills 4, all three still alive
  --assert-watch 526:cv:8:eq:4
  --assert-watch 526:cv:14:eq:3
  --assert-watch 526:cv:5:eq:6
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
  --assert-text "544:FLR 3 HP 5 TORCH 255 SCORE 345"
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

echo "ALL CINDERVAULT PROOFS PASS"
