#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# DEEPCAST — the committed headless proof suite (mGBA,
# tools/headless-screenshot.py), ported with the audio slice (growth cut 1).
# The prototype's proof (.sessions/2026-07-13-deepcast.md) lived only in
# that session's container; per the Shoal/Courier/Cindervault convention the
# input scripts ARE the pin, so from now on they are committed here.
#
# Pin provenance: the prototype card's route, re-run against THIS build with
# --watch-log and every literal transcribed from the observed CSV (the
# card's 💡 listen-then-script method). Enabling maxmod did not shift the
# boot clock for this game — every prototype game-state literal (state
# transitions, tension/line/weight/score words, frame 2492 at shot 2500)
# carried VERBATIM; the audio pins are new.
#
# Run from the repo root after building (make -C games/deepcast):
#     bash games/deepcast/proofs.sh
# Artifacts land in $DC_PROOF_OUT (default /tmp/deepcast-proofs).
#
# The three proofs (asserts inline below):
#   P1 boot/title            — mailbox magics, title state, seed 0xDEE9CA57
#                              on the card, all title lines incl. the new
#                              "B: MUTE" verb; the title is SILENT (mixer
#                              memory all-zero) and the ledger is zeroed.
#   P2 the seeded run        — the committed core-loop route: cast 72m
#     (core loop + THE cut)    (wt 11) -> bite -> fight smart (reel the
#                              rests, yield the surges) -> CATCH +11 ->
#                              three hold-only deep casts SNAP at exactly
#                              tension 600 (incl. the max-charge 172m/wt-30
#                              clamp) -> one shallow 27m CATCH +7 -> DUSK
#                              (score 18, catches 2). AUDIO: reel clicks
#                              only while reeling (interval word 0 on yield
#                              frames), the click interval SHRINKING with
#                              tension along one fight (16 at tension 0 ->
#                              5 at 588 — the concept's named cut), pitch
#                              rides the same word; the bite/catch/snap
#                              ledger counts every event (final 67 clicks /
#                              5 bites / 2 catches / 3 snaps) and the
#                              maxmod mixer-memory watch proves voicing at
#                              click/catch/snap frames (dusk silent again).
#                              RUN TWICE — the two watch-logs must be
#                              byte-identical (determinism contract).
#   P3 the mute discriminator — the IDENTICAL route with one B press at the
#                              title: the mixer stays all-zero for all 2520
#                              frames' sampled asserts, while the decision
#                              ledger and EVERY game-state word land on the
#                              same literals as P2 — audio is a pure
#                              decision layer; mute gates voicing only.
#                              RUN TWICE — byte-identical watch-logs.
#   P4 the dialed lake        — growth cut 2 (seed-select score-attack):
#     (seed-select cut)         UP/RIGHT/R at the title dial the seed
#                              +1/+0x100/+0x10000 (each step pinned in
#                              telemetry word dc[4]) to 0xDEEACB58, shown
#                              glyph-exact on the title. Same charge input
#                              as P2's first cast, DIFFERENT lake: at frame
#                              471 (the default lake's bite frame) this
#                              lake is still sinking; it bites at 489, and
#                              a later 72m cast pulls a wt-16 fish where
#                              the default stream gave 11. Four casts to
#                              dusk: score 6 / catches 1, ledger 21 clicks
#                              / 4 bites / 1 catch / 3 snaps, and the dusk
#                              card names its seed ("SEED DEEACB58") so
#                              the score is attributable. The audio laws
#                              carry to any seed (first reeling frame
#                              clicks at interval 16, cues voiced).
#                              RUN TWICE — byte-identical watch-logs.
#   P5 dial away and back     — UP/RIGHT/R then L/LEFT/DOWN returns the
#                              dial to 0xDEE9CA57 (each magnitude down-step
#                              exercised), the title shows the default seed
#                              again, and the FULL P2 route then lands on
#                              EVERY P2 literal — dialing is fully
#                              reversible and the default lake is untouched
#                              by the feature.
# ---------------------------------------------------------------------------
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."

ROM=games/deepcast/deepcast.gba
OUT="${DC_PROOF_OUT:-/tmp/deepcast-proofs}"
mkdir -p "$OUT"

H() { python3 tools/headless-screenshot.py "$ROM" "$@"; }
W='--elf games/deepcast/deepcast.elf --watch dc:dc_telemetry:16'
# Audio evidence (docs/capabilities.md method): cumulative decision
# counters + per-frame click-interval/mute words + the maxmod
# mixer-memory activity word (count of nonzero u32 words — 0 exactly
# when the mixer is silent).
WA='--watch h:gl_audio_hook:6 --watch-nonzero mix:maxmod_mixing_buffer:0x420'

echo "== P1: boot + title (silent, ledger zeroed) =="
H "$OUT/p1.png" --frames 120 $W $WA \
  --assert-watch 100:dc:0:eq:1145390416 \
  --assert-watch 100:dc:1:eq:1128354644 \
  --assert-watch 100:dc:2:eq:0 \
  --assert-watch 100:dc:3:gt:0 \
  --assert-watch 100:dc:4:eq:3739863639 \
  --assert-watch 100:dc:12:eq:3 \
  --assert-watch 100:h:0:eq:0 \
  --assert-watch 100:h:1:eq:0 \
  --assert-watch 100:h:2:eq:0 \
  --assert-watch 100:h:3:eq:0 \
  --assert-watch 100:h:4:eq:0 \
  --assert-watch 100:h:5:eq:0 \
  --assert-watch 100:mix:0:eq:0 \
  --assert-text "100:DEEPCAST" \
  --assert-text "100:A QUIET LAKE AT DUSK" \
  --assert-text "100:SEED DEE9CA57" \
  --assert-text "100:HOLD A: CAST DEEP, REEL SOFT" \
  --assert-text "100:3 LURES. PRESS START" \
  --assert-text "100:B: MUTE" \
  --assert-text "100:B: MUTE  DPAD L R: DIAL SEED"

# The committed core-loop route (the prototype card's seeded run, carried):
# START at 240; 60f charge -> 72m cast (wt 11); duty-cycle reel through the
# two rests, hands off both surges -> CATCH +11 at tension peak 278; three
# hold-only deep casts SNAP at exactly 600 (the middle one max-charge
# 172m/wt 30 — the clamp); one shallow 27m CATCH +7 between them; dusk at
# lures 0 with score 18, catches 2.
CORE_ROUTE='--keys 240-244:START --keys 300-360:A --keys-pattern 471-536:4:3:A --keys-pattern 591-690:4:3:A --keys 780-860:A --keys 862-1400:A --keys 1402-1700:A --keys 1742-1790:A --keys 1875-1955:A --keys 1957-2300:A'

P2_ASSERTS=(
  # charge/cast: state walk + both RNG words landing (target 72, wt 11)
  --assert-watch 240:dc:2:eq:1
  --assert-watch 240:dc:12:eq:3
  --assert-watch 240:dc:14:eq:0
  --assert-watch 355:dc:5:eq:56
  --assert-watch 360:dc:2:eq:2
  --assert-watch 360:dc:7:eq:72
  --assert-watch 360:dc:10:eq:11
  --assert-watch 430:dc:6:eq:72
  # sink is silent reel-wise: no clicks before the bite
  --assert-watch 430:h:0:eq:0
  --assert-watch 430:h:4:eq:0
  # frame 471 BITE: fight starts, line 432 — bite counted, and the FIRST
  # reeling frame clicks at the slack interval (16 frames, tension 0)
  --assert-watch 471:dc:2:eq:3
  --assert-watch 471:dc:8:eq:0
  --assert-watch 471:dc:9:eq:432
  --assert-watch 471:h:1:eq:1
  --assert-watch 471:h:0:eq:1
  --assert-watch 471:h:4:eq:16
  # frame 560 SURGE, hands off: yielding NEVER clicks (interval word 0)
  --assert-watch 560:dc:11:eq:1
  --assert-watch 560:dc:9:eq:258
  --assert-watch 560:h:4:eq:0
  # frame 620 reeling a rest at tension 98: interval already down 16 -> 15,
  # 25 clicks banked, and the mixer is VOICING one
  --assert-watch 620:dc:11:eq:0
  --assert-watch 620:dc:8:eq:98
  --assert-watch 620:dc:9:eq:210
  --assert-watch 620:h:0:eq:25
  --assert-watch 620:h:4:eq:15
  --assert-watch 620:mix:0:gt:0
  --assert-text "620:IT RESTS. REEL, GENTLY"
  # frame 680 CATCH +11 (tension peak 278): catch counted, cue voiced
  --assert-watch 680:dc:2:eq:4
  --assert-watch 680:dc:15:eq:1
  --assert-watch 680:dc:13:eq:1
  --assert-watch 680:dc:14:eq:11
  --assert-watch 680:dc:9:eq:0
  --assert-watch 680:dc:8:eq:278
  --assert-watch 680:h:2:eq:1
  --assert-watch 680:h:0:eq:40
  --assert-watch 680:mix:0:gt:0
  # cast 2 (hold-only 92m): THE NAMED CUT along one fight — the click
  # interval shrinks with tension, 16 at tension 0 (frame 941) -> 5 at
  # tension 588 (frame 1025), then the line SNAPS at exactly 600
  --assert-watch 860:dc:2:eq:2
  --assert-watch 860:dc:7:eq:92
  --assert-watch 860:dc:10:eq:10
  --assert-watch 941:dc:2:eq:3
  --assert-watch 941:dc:8:eq:0
  --assert-watch 941:dc:9:eq:552
  --assert-watch 941:h:1:eq:2
  --assert-watch 941:h:4:eq:16
  --assert-watch 1025:dc:8:eq:588
  --assert-watch 1025:h:4:eq:5
  --assert-watch 1025:h:0:eq:49
  --assert-watch 1027:dc:2:eq:4
  --assert-watch 1027:dc:15:eq:2
  --assert-watch 1027:dc:8:eq:600
  --assert-watch 1027:dc:12:eq:2
  --assert-watch 1029:h:3:eq:1
  --assert-watch 1029:mix:0:gt:0
  # cast 3 (max-charge clamp: 160f -> 172m, wt 30) snaps too
  --assert-watch 1400:dc:2:eq:2
  --assert-watch 1400:dc:5:eq:160
  --assert-watch 1400:dc:7:eq:172
  --assert-watch 1400:dc:10:eq:30
  --assert-watch 1529:dc:2:eq:3
  --assert-watch 1529:dc:9:eq:1032
  --assert-watch 1529:h:1:eq:3
  --assert-watch 1593:dc:8:eq:576
  --assert-watch 1593:h:4:eq:5
  --assert-watch 1594:dc:2:eq:4
  --assert-watch 1594:dc:15:eq:2
  --assert-watch 1594:dc:12:eq:1
  --assert-watch 1594:h:3:eq:2
  # cast 4 (shallow 27m) CATCHES +7
  --assert-watch 1700:dc:2:eq:2
  --assert-watch 1700:dc:7:eq:27
  --assert-watch 1700:dc:10:eq:7
  --assert-watch 1774:dc:2:eq:4
  --assert-watch 1774:dc:15:eq:1
  --assert-watch 1774:dc:13:eq:2
  --assert-watch 1774:dc:14:eq:18
  --assert-watch 1774:h:2:eq:2
  # cast 5 (92m again off the re-walked stream) snaps -> lures 0, dusk
  --assert-watch 1955:dc:2:eq:2
  --assert-watch 1955:dc:7:eq:92
  --assert-watch 1955:dc:10:eq:18
  --assert-watch 2069:dc:2:eq:3
  --assert-watch 2069:h:1:eq:5
  --assert-watch 2144:dc:2:eq:4
  --assert-watch 2144:dc:15:eq:2
  --assert-watch 2144:dc:12:eq:0
  --assert-watch 2144:h:3:eq:3
  --assert-watch 2234:dc:2:eq:5
  --assert-watch 2234:dc:14:eq:18
  --assert-watch 2234:dc:13:eq:2
  # dusk: the full ledger (67 clicks / 5 bites / 2 catches / 3 snaps),
  # unmuted, mixer silent again, mailbox magics + seed still pinned
  --assert-watch 2500:dc:2:eq:5
  --assert-watch 2500:dc:3:eq:2492
  --assert-watch 2500:dc:0:eq:1145390416
  --assert-watch 2500:dc:1:eq:1128354644
  --assert-watch 2500:dc:4:eq:3739863639
  --assert-watch 2500:h:0:eq:67
  --assert-watch 2500:h:1:eq:5
  --assert-watch 2500:h:2:eq:2
  --assert-watch 2500:h:3:eq:3
  --assert-watch 2500:h:4:eq:0
  --assert-watch 2500:h:5:eq:0
  --assert-watch 2500:mix:0:eq:0
  --assert-text "2500:DUSK FALLS. LURES GONE"
  --assert-text "2500:SCORE 18"
  --assert-text "2500:CATCHES 2"
  # growth cut 2: the dusk card names the seed the score was fished on
  --assert-text "2500:SEED DEE9CA57"
)

echo "== P2: the seeded run — core loop + reel clicks that speed up with tension (run 1) =="
H "$OUT/p2.png" --frames 2520 $W $WA $CORE_ROUTE \
  --watch-log "$OUT/p2-run1.csv" \
  --shot "620:$OUT/p2-fight.png" --shot "680:$OUT/p2-catch.png" \
  --require-distinct \
  "${P2_ASSERTS[@]}"

echo "== P2: run 2 (must be byte-identical) =="
H "$OUT/p2b.png" --frames 2520 $W $WA $CORE_ROUTE \
  --watch-log "$OUT/p2-run2.csv" \
  "${P2_ASSERTS[@]}"
cmp "$OUT/p2-run1.csv" "$OUT/p2-run2.csv"
echo "P2 run-twice: byte-identical"

# P3: the SAME route with one B press at the title. Decisions unchanged,
# voicing gated: the ledger and every game-state word land on P2's exact
# literals while the mixer memory never goes nonzero.
MUTE_ROUTE='--keys 200-202:B '$CORE_ROUTE

P3_ASSERTS=(
  --assert-watch 300:h:5:eq:1
  # the loud frames of P2, silent — same decisions counted
  --assert-watch 620:mix:0:eq:0
  --assert-watch 620:h:0:eq:25
  --assert-watch 620:h:4:eq:15
  --assert-watch 680:mix:0:eq:0
  --assert-watch 680:h:2:eq:1
  --assert-watch 1029:mix:0:eq:0
  --assert-watch 1029:h:3:eq:1
  --assert-watch 1595:mix:0:eq:0
  --assert-watch 2145:mix:0:eq:0
  # the sim never heard the mute: every game-state word identical to P2
  --assert-watch 680:dc:14:eq:11
  --assert-watch 1027:dc:8:eq:600
  --assert-watch 1774:dc:14:eq:18
  --assert-watch 2500:dc:2:eq:5
  --assert-watch 2500:dc:3:eq:2492
  --assert-watch 2500:dc:13:eq:2
  --assert-watch 2500:dc:14:eq:18
  --assert-watch 2500:h:0:eq:67
  --assert-watch 2500:h:1:eq:5
  --assert-watch 2500:h:2:eq:2
  --assert-watch 2500:h:3:eq:3
  --assert-watch 2500:h:5:eq:1
  --assert-watch 2500:mix:0:eq:0
  --assert-text "2500:SCORE 18"
)

echo "== P3: the mute discriminator — same decisions, zero voicing (run 1) =="
H "$OUT/p3.png" --frames 2520 $W $WA $MUTE_ROUTE \
  --watch-log "$OUT/p3-run1.csv" \
  "${P3_ASSERTS[@]}"

echo "== P3: run 2 (must be byte-identical) =="
H "$OUT/p3b.png" --frames 2520 $W $WA $MUTE_ROUTE \
  --watch-log "$OUT/p3-run2.csv" \
  "${P3_ASSERTS[@]}"
cmp "$OUT/p3-run1.csv" "$OUT/p3-run2.csv"
echo "P3 run-twice: byte-identical"

# ---------------------------------------------------------------------------
# P4/P5 — growth cut 2, seed-select score-attack. The dial: on the title,
# UP/DOWN adjusts the seed +-1, LEFT/RIGHT +-0x100, L/R +-0x10000
# (edge-triggered, 32-bit wrap, xorshift dead state 0 skipped). Pins from
# an observed run of THIS route (the listen-then-script method):
# 0xDEE9CA57=3739863639 · +1=3739863640 · +0x101=3739863896 ·
# +0x10101 = 0xDEEACB58 = 3739929432.
# ---------------------------------------------------------------------------

DIAL_UP='--keys 150-152:UP --keys 154-156:RIGHT --keys 158-160:R'
DIAL_BACK='--keys 170-172:L --keys 174-176:LEFT --keys 178-180:DOWN'

# The dialed-lake route: same START + first-cast charge input as P2, then
# four scripted casts (snap / shallow catch / snap / snap) to dusk.
P4_ROUTE=$DIAL_UP' --keys 240-244:START --keys 300-360:A --keys 362-600:A --keys 700-715:A --keys 717-900:A --keys 990-1050:A --keys 1052-1250:A --keys 1350-1410:A --keys 1412-1700:A'

P4_ASSERTS=(
  # the dial, step by step in the telemetry seed word (boot value first)
  --assert-watch 149:dc:4:eq:3739863639
  --assert-watch 153:dc:4:eq:3739863640
  --assert-watch 157:dc:4:eq:3739863896
  --assert-watch 200:dc:4:eq:3739929432
  # the dialed seed on the title, glyph-exact; dialing itself is silent
  --assert-text "200:SEED DEEACB58"
  --assert-watch 200:h:0:eq:0
  --assert-watch 200:mix:0:eq:0
  # same charge input as P2 -> same 72m cast, DIFFERENT lake: at 471 (the
  # default lake's pinned bite frame) this lake is still SINKING, no bite
  # counted; it bites at 489 instead — and the audio law carries to any
  # seed (first reeling frame clicks at the slack interval 16)
  --assert-watch 360:dc:2:eq:2
  --assert-watch 360:dc:7:eq:72
  --assert-watch 471:dc:2:eq:2
  --assert-watch 471:h:1:eq:0
  --assert-watch 489:dc:2:eq:3
  --assert-watch 489:dc:9:eq:432
  --assert-watch 489:h:1:eq:1
  --assert-watch 489:h:4:eq:16
  # hold-only fight 1 snaps at exactly 600 (frame 546; default lake at 620
  # is still mid-fight — this one is already on the result card)
  --assert-watch 546:dc:2:eq:4
  --assert-watch 546:dc:15:eq:2
  --assert-watch 546:dc:8:eq:600
  --assert-watch 546:dc:12:eq:2
  --assert-watch 546:h:3:eq:1
  --assert-watch 548:mix:0:gt:0
  --assert-watch 620:dc:2:eq:4
  # cast 2 shallow 27m (wt 6) CATCHES +6 — the dialed lake's score
  --assert-watch 715:dc:2:eq:2
  --assert-watch 715:dc:7:eq:27
  --assert-watch 715:dc:10:eq:6
  --assert-watch 841:dc:2:eq:4
  --assert-watch 841:dc:15:eq:1
  --assert-watch 841:dc:13:eq:1
  --assert-watch 841:dc:14:eq:6
  --assert-watch 841:dc:8:eq:198
  --assert-watch 841:h:2:eq:1
  --assert-watch 843:mix:0:gt:0
  # cast 3 (72m again) snaps
  --assert-watch 1050:dc:2:eq:2
  --assert-watch 1050:dc:7:eq:72
  --assert-watch 1109:dc:2:eq:3
  --assert-watch 1109:h:1:eq:3
  --assert-watch 1182:dc:2:eq:4
  --assert-watch 1182:dc:15:eq:2
  --assert-watch 1182:dc:12:eq:1
  --assert-watch 1182:h:3:eq:2
  # cast 4: the SAME 72m cast the default stream answers with wt 11 pulls
  # a wt-16 fish here — same formula, different world
  --assert-watch 1410:dc:2:eq:2
  --assert-watch 1410:dc:7:eq:72
  --assert-watch 1410:dc:10:eq:16
  --assert-watch 1473:dc:2:eq:3
  --assert-watch 1473:h:1:eq:4
  --assert-watch 1539:dc:2:eq:4
  --assert-watch 1539:dc:15:eq:2
  --assert-watch 1539:dc:12:eq:0
  --assert-watch 1539:h:3:eq:3
  # dusk: full ledger, seed word still the dialed one, and the dusk card
  # NAMES the seed — the score is attributable to its lake
  --assert-watch 1720:dc:2:eq:5
  --assert-watch 1720:dc:13:eq:1
  --assert-watch 1720:dc:14:eq:6
  --assert-watch 1720:dc:4:eq:3739929432
  --assert-watch 1720:h:0:eq:21
  --assert-watch 1720:h:1:eq:4
  --assert-watch 1720:h:2:eq:1
  --assert-watch 1720:h:3:eq:3
  --assert-watch 1720:mix:0:eq:0
  --assert-text "1720:DUSK FALLS. LURES GONE"
  --assert-text "1720:SCORE 6"
  --assert-text "1720:CATCHES 1"
  --assert-text "1720:SEED DEEACB58"
)

echo "== P4: the dialed lake — seed-select score-attack, same input, different world (run 1) =="
H "$OUT/p4.png" --frames 1750 $W $WA $P4_ROUTE \
  --watch-log "$OUT/p4-run1.csv" \
  --shot "200:$OUT/p4-title.png" \
  "${P4_ASSERTS[@]}"

echo "== P4: run 2 (must be byte-identical) =="
H "$OUT/p4b.png" --frames 1750 $W $WA $P4_ROUTE \
  --watch-log "$OUT/p4-run2.csv" \
  "${P4_ASSERTS[@]}"
cmp "$OUT/p4-run1.csv" "$OUT/p4-run2.csv"
echo "P4 run-twice: byte-identical"

# P5: dial away, dial back, and the ENTIRE default run must reproduce —
# every P2 literal (game state, audio ledger, click-interval law, dusk
# card) on the same frames. One run: P2 already proved this route's
# run-twice identity; this proves the dial can't leave a mark.
P5_ROUTE=$DIAL_UP' '$DIAL_BACK' '$CORE_ROUTE

echo "== P5: dial away and back — the default lake reproduces on every P2 literal =="
H "$OUT/p5.png" --frames 2520 $W $WA $P5_ROUTE \
  --assert-watch 165:dc:4:eq:3739929432 \
  --assert-watch 171:dc:4:eq:3739863896 \
  --assert-watch 175:dc:4:eq:3739863640 \
  --assert-watch 200:dc:4:eq:3739863639 \
  --assert-text "200:SEED DEE9CA57" \
  "${P2_ASSERTS[@]}"

echo "ALL DEEPCAST PROOFS PASS"
