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
#   P6 species + catch log    — growth cut 3 (species tables per depth band
#     (species cut)             with named rarities; a catch log): the
#                              default lake's first catch is the MIDWATER
#                              MYTHIC (MOON EEL, species 7 — one side-band
#                              word per cast, stream seeded from
#                              seed^0x51DEF157, main stream untouched);
#                              SELECT opens the catch log ("CATCH LOG 1",
#                              the entry glyph-exact) and SELECT closes it
#                              back to the charge screen. A shallow cast
#                              lands THE SHALLOWS COMMON (MUD BREAM wt 9,
#                              packed log entry 9), a deeps cast hooks
#                              IRON GAR (THE DEEPS on the sink card,
#                              species 9 in the mailbox) and SNAPS — a
#                              snapped line never logs (fl[2] stays 2,
#                              slot fl[6] stays 0). The log ring, the
#                              side-band stream state, and the species /
#                              rarity words are pinned in the dc_fishlog
#                              mailbox at every step.
#                              RUN TWICE — byte-identical watch-logs.
#   P7 line upgrades + SRAM   — growth cut 4 (line upgrades bought with
#     (upgrade cut)             score: thicker line = higher snap threshold,
#                              slower reel). Run A (NO savefile = the fresh
#                              cart): the WORN LINE baseline — bank 0 tier 0
#                              snap 600 reel 5 on the title, and the default
#                              lake's first cast fought hold-only reels 5
#                              line/frame (bite+29 -> 432-5*29 = 287) and
#                              SNAPS at exactly 600 on frame 542. Run B
#                              (boot 1, factory-fresh savefile): the full P2
#                              route banks its dusk score (BANK 18 deposited
#                              at dusk, glyph-exact on the new dusk line), R
#                              opens the LINE SHOP, A buys BRAIDED LINE for
#                              15 (bank 18 -> 3, snap 600 -> 750, reel 5 ->
#                              4, all glyph-exact), a second A is REFUSED
#                              (STEEL costs 40 > bank 3 — nothing moves),
#                              SELECT returns to dusk. RUN TWICE from two
#                              factory-fresh savefiles — watch-logs AND the
#                              two written savefiles byte-identical. Run C
#                              (boot 2, a SEPARATE emulator boot on run B's
#                              savefile — the Lumen Drift two-boot pattern):
#                              the purchase SURVIVED the power cycle (title
#                              BANK 3 / BRAIDED on the title shop, tier 1 /
#                              snap 750 / reel 4 in dc_linemeta), and the
#                              SAME hold-only fight as run A is the
#                              TRADEOFF WITNESS, arithmetic on the same
#                              fight: same bite (frame 471, line 432), same
#                              tension trajectory (203 at 500, 573 at 540 —
#                              tension math untouched), but line 316 =
#                              432-4*29 where the worn line had 287 =
#                              432-5*29 (slower reel), still FIGHTING on
#                              frame 542 at tension 623 (past the worn
#                              line's 600 snap — higher threshold), and the
#                              snap lands at exactly 750 on frame 548. RUN
#                              TWICE — byte-identical watch-logs, savefile
#                              untouched (no dusk = no SRAM write).
#                              Fresh-cart identity: P1-P6 above run with NO
#                              savefile and are UNMODIFIED from v0.4 — a
#                              zeroed/erased cart boots exactly like v0.4
#                              (tier 0 IS the v0.4 constants).
#   P8 lake gradient + rod    — growth cut 5 (the art pass): the P2 route
#     (art cut)                 with dc_artmeta + DISPCNT/palette-RAM/VRAM
#                              watches — every P2 pin re-lands while the
#                              gradient palette follows depth (0x6A05 ->
#                              0x44E2 -> 0x1421 in palette RAM), the
#                              silhouette follows the species word, and
#                              the rod bend follows tension*7/snap.
#                              RUN TWICE — byte-identical watch-logs.
#   P9 species silhouettes    — the P6 route with the art watched, every
#     (art cut)                 P6 pin carried: three species across three
#                              bands drive frame 7 / 0 / 9; the fish is
#                              hidden while sinking and on the SNAP card
#                              (the species keeps its secret).
#                              RUN TWICE — byte-identical watch-logs.
#   P10 the honest gauge      — P7's two-SRAM-state pattern re-used on the
#     (art cut)                 rod law: the SAME hold-only fight bends the
#                              worn rod 2/6 and the braided rod 1/5 at the
#                              same tension words — the gauge follows
#                              lm[3], so it is honest on any line.
#                              RUN TWICE per leg — byte-identical logs.
#
# Growth cut 5 (the art pass — lake gradient by depth, silhouette fish,
# rod-bend tension sprite) is PRESENTATION ONLY: game state, RNG word order
# and every dc_telemetry / dc_fishlog / dc_linemeta word are byte-identical
# to cut 4, so P1-P7 below carried VERBATIM (zero re-derived pins, zero
# adjusted text asserts — the text HUD stays alongside the art). What the
# cut adds is proven in P8/P9/P10 through a FOURTH mailbox (dc_artmeta:
# presentation words only) plus direct hardware-state pins — DISPCNT, BG
# palette RAM (the gradient law's exact BGR555 halfwords, rewritten as the
# line sinks), OBJ palette RAM (the committed fish/rod art's colors) and
# the lake map's VRAM screenblock — all read at GBA bus addresses by the
# same watch machinery (the Cindervault P8/P9 method). The laws under
# proof (closed forms of already-pinned sim words, cross-checked offline):
#
#   gradient band i (0 surface..9 bottom) at depth d (the dc[6] word):
#       dim = 2*i + d/8
#       BGR555 = max(1,5-dim/3) | max(1,16-dim)<<5 | max(4,26-dim)<<10
#     (d=0 band0 = 0x6A05 dusk blue; d=72 -> 0x44E2; d=172 -> 0x1421 —
#      the whole lake literally darkens as the line sinks)
#   fish frame  = the dc_fishlog species word fl[0] (band*4 + tier: band
#       sets silhouette SIZE, tier its SHADE) while fighting and on the
#       CATCH card; 255 (hidden) everywhere else — a SNAP card never
#       shows the fish, the species keeps its secret
#   rod bend    = tension * 7 / line_snap[tier]  (dc[8] over lm[3] — the
#       same tension-over-snap law the audio ratchet reads, so the rod is
#       an honest analog gauge on ANY line tier)
#
# Growth cut 3 also threads species witnesses through the EXISTING routes
# additively: P1 pins the boot sentinels (species/rarity 255, log 0, the
# derived side seed 2402761472 live on the title), P2/P5 pin the default
# lake's species on both catch cards (MOON EEL MYTHIC / DUSK PERCH
# UNCOMMON, glyph-exact) + the full log ring at dusk, P3 pins that mute
# doesn't touch the log, and P4 pins that the DIALED lake draws from a
# DIFFERENT side-band stream (derived seed 2402564623: its shallow catch
# is MUD BREAM COMMON where the default lake's was DUSK PERCH UNCOMMON).
# All pins below were transcribed from observed --watch-log runs and
# cross-checked against an offline xorshift32 replica of the side stream.
# ---------------------------------------------------------------------------
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."

ROM=games/deepcast/deepcast.gba
OUT="${DC_PROOF_OUT:-/tmp/deepcast-proofs}"
mkdir -p "$OUT"

H() { python3 tools/headless-screenshot.py "$ROM" "$@"; }
W='--elf games/deepcast/deepcast.elf --watch dc:dc_telemetry:16 --watch fl:dc_fishlog:16'
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
  --assert-watch 100:fl:0:eq:255 \
  --assert-watch 100:fl:1:eq:255 \
  --assert-watch 100:fl:2:eq:0 \
  --assert-watch 100:fl:3:eq:0 \
  --assert-watch 100:fl:12:eq:2402761472 \
  --assert-watch 100:fl:13:eq:1179210568 \
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
  # frame 680 CATCH +11 (tension peak 278): catch counted, cue voiced —
  # and (growth cut 3) the catch card NAMES the fish: the 72m cast is
  # MIDWATER, the default lake's first side-band word rolls MYTHIC ->
  # species 7 MOON EEL, logged as ring entry 0 (7*256+11 = 1803)
  --assert-watch 680:fl:0:eq:7
  --assert-watch 680:fl:1:eq:3
  --assert-watch 680:fl:2:eq:1
  --assert-watch 680:fl:4:eq:1803
  --assert-text "690:MOON EEL"
  --assert-text "690:MYTHIC"
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
  # the 172m cast sinks through THE ABYSS (band named on the sink card);
  # its fish is TRENCH MAW (species 13, UNCOMMON) — it will snap, so it
  # must NEVER reach the log
  --assert-text "1450:THE ABYSS"
  --assert-watch 1450:fl:0:eq:13
  --assert-watch 1450:fl:1:eq:1
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
  # the shallow catch card: THE SHALLOWS' UNCOMMON, DUSK PERCH (species
  # 1), ring entry 1 packed 1*256+7 = 263 — and the two snaps between
  # the catches left no mark on the log (fl[2] went 1 -> 2 directly)
  --assert-watch 1774:fl:0:eq:1
  --assert-watch 1774:fl:1:eq:1
  --assert-watch 1774:fl:2:eq:2
  --assert-watch 1774:fl:5:eq:263
  --assert-text "1780:DUSK PERCH"
  --assert-text "1780:UNCOMMON"
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
  # dusk (growth cut 3): the run logged exactly its two catches — ring
  # slots 0/1 pinned, slot 2 empty, five casts consumed five side-band
  # words (stream state 489620607), last cast was SILVER TROUT COMMON
  --assert-watch 2500:fl:0:eq:4
  --assert-watch 2500:fl:1:eq:0
  --assert-watch 2500:fl:2:eq:2
  --assert-watch 2500:fl:4:eq:1803
  --assert-watch 2500:fl:5:eq:263
  --assert-watch 2500:fl:6:eq:0
  --assert-watch 2500:fl:12:eq:489620607
  --assert-watch 2500:fl:13:eq:1179210568
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
  # mute never touches the catch log either
  --assert-watch 2500:fl:2:eq:2
  --assert-watch 2500:fl:4:eq:1803
  --assert-watch 2500:fl:5:eq:263
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
  # the dialed seed on the title, glyph-exact; dialing itself is silent —
  # and (growth cut 3) the species side-band stream FOLLOWS the dial
  # live: fl[12] is already the dialed lake's derived side seed
  --assert-text "200:SEED DEEACB58"
  --assert-watch 200:h:0:eq:0
  --assert-watch 200:mix:0:eq:0
  --assert-watch 200:fl:12:eq:2402564623
  --assert-watch 200:fl:0:eq:255
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
  # the dialed lake's shallow catch is MUD BREAM, COMMON (species 0,
  # ring entry 0*256+6 = 6) where the DEFAULT lake's shallow catch was
  # DUSK PERCH, UNCOMMON — same input grammar, different world, down to
  # the fish's name
  --assert-watch 841:fl:0:eq:0
  --assert-watch 841:fl:1:eq:0
  --assert-watch 841:fl:2:eq:1
  --assert-watch 841:fl:4:eq:6
  --assert-text "850:MUD BREAM"
  --assert-text "850:COMMON"
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
  # dusk on the dialed lake: one catch logged, four side-band words
  # consumed from the DIALED stream, last cast TWILIGHT PIKE UNCOMMON
  --assert-watch 1720:fl:0:eq:5
  --assert-watch 1720:fl:1:eq:1
  --assert-watch 1720:fl:2:eq:1
  --assert-watch 1720:fl:4:eq:6
  --assert-watch 1720:fl:5:eq:0
  --assert-watch 1720:fl:12:eq:1019259156
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

# ---------------------------------------------------------------------------
# P6 — growth cut 3, species tables per depth band + the catch log. Default
# seed. Cast 1 is P2's exact first cast (72m, MIDWATER -> the side stream's
# first word rolls MYTHIC: MOON EEL, wt 11); SELECT then opens the catch
# log between casts and SELECT closes it. Cast 2 is shallow (42m, THE
# SHALLOWS -> COMMON: MUD BREAM, wt 9). Cast 3 is one continuous hold that
# charges 132 frames -> 144m, THE DEEPS -> UNCOMMON: IRON GAR (species 9)
# — and SNAPS at exactly 600, proving a snapped line never logs. The log
# is then reopened with both entries glyph-exact. Pins transcribed from an
# observed --watch-log run of THIS route (the listen-then-script method)
# and cross-checked against an offline xorshift32 replica of the side
# stream (derived seed 0xDEE9CA57^0x51DEF157 = 2402761472; words
# 1656154699 -> 1074094900 -> 1611740581).
# ---------------------------------------------------------------------------

P6_ROUTE='--keys 240-244:START --keys 300-360:A --keys-pattern 471-536:4:3:A --keys-pattern 591-690:4:3:A --keys 800-802:SELECT --keys 860-862:SELECT --keys 900-930:A --keys 940-1260:A --keys 1380-1520:A --keys 1560-1562:SELECT --keys 1650-1652:SELECT'

P6_ASSERTS=(
  # cast 1 = P2's first cast: the side-band word is drawn AT the cast —
  # species 7 (MIDWATER MYTHIC, MOON EEL), stream advanced one word;
  # the sink card names the band
  --assert-watch 365:dc:7:eq:72
  --assert-watch 365:dc:10:eq:11
  --assert-watch 365:fl:0:eq:7
  --assert-watch 365:fl:1:eq:3
  --assert-watch 365:fl:12:eq:1656154699
  --assert-text "430:MIDWATER"
  # the catch card names the fish, glyph-exact, and logs ring entry 0
  # (7*256 + 11 = 1803)
  --assert-watch 680:dc:2:eq:4
  --assert-watch 680:dc:15:eq:1
  --assert-watch 680:fl:2:eq:1
  --assert-watch 680:fl:4:eq:1803
  --assert-text "690:CATCH! +11"
  --assert-text "690:MOON EEL"
  --assert-text "690:MYTHIC"
  # SELECT between casts opens the CATCH LOG (state 6, open flag up, no
  # RNG consumed — fl[12] unmoved); the one entry is glyph-exact;
  # SELECT closes it back to the charge screen (which advertises the key)
  --assert-watch 798:dc:2:eq:1
  --assert-watch 798:fl:3:eq:0
  --assert-text "798:SELECT: CATCH LOG"
  --assert-watch 830:dc:2:eq:6
  --assert-watch 830:fl:3:eq:1
  --assert-watch 830:fl:12:eq:1656154699
  --assert-text "830:CATCH LOG 1"
  --assert-text "830:MOON EEL MYTHIC 11"
  --assert-text "830:SELECT: BACK"
  --assert-watch 880:dc:2:eq:1
  --assert-watch 880:fl:3:eq:0
  # cast 2: 30 charge frames -> 42m, THE SHALLOWS; the second side-band
  # word rolls COMMON -> MUD BREAM (species 0), wt 9 off the main stream
  --assert-watch 932:dc:2:eq:2
  --assert-watch 932:dc:7:eq:42
  --assert-watch 932:dc:10:eq:9
  --assert-watch 932:fl:0:eq:0
  --assert-watch 932:fl:1:eq:0
  --assert-watch 932:fl:12:eq:1074094900
  --assert-text "945:THE SHALLOWS"
  # bite at 986, hold-only catch at 1037 (tension 357): ring entry 1 is
  # the packed 0*256 + 9 = 9, card glyph-exact
  --assert-watch 986:dc:2:eq:3
  --assert-watch 986:dc:9:eq:252
  --assert-watch 1037:dc:2:eq:4
  --assert-watch 1037:dc:15:eq:1
  --assert-watch 1037:dc:13:eq:2
  --assert-watch 1037:dc:14:eq:20
  --assert-watch 1037:dc:8:eq:357
  --assert-watch 1037:fl:2:eq:2
  --assert-watch 1037:fl:5:eq:9
  --assert-text "1040:CATCH! +9"
  --assert-text "1040:MUD BREAM"
  --assert-text "1040:COMMON"
  # cast 3: the same hold keeps charging out of the result card — 132
  # frames -> 144m, THE DEEPS; the third side-band word rolls UNCOMMON
  # -> IRON GAR (species 9), wt 16
  --assert-watch 1260:dc:2:eq:2
  --assert-watch 1260:dc:7:eq:144
  --assert-watch 1260:dc:10:eq:16
  --assert-watch 1260:fl:0:eq:9
  --assert-watch 1260:fl:1:eq:1
  --assert-watch 1260:fl:12:eq:1611740581
  --assert-text "1300:THE DEEPS"
  # hold-only fight SNAPS at exactly 600 — the snapped IRON GAR never
  # reaches the log: entry count stays 2, ring slot 2 stays empty
  --assert-watch 1375:dc:2:eq:3
  --assert-watch 1444:dc:2:eq:4
  --assert-watch 1444:dc:15:eq:2
  --assert-watch 1444:dc:8:eq:600
  --assert-watch 1444:dc:12:eq:2
  --assert-watch 1444:fl:2:eq:2
  --assert-watch 1444:fl:6:eq:0
  # the reopened log still shows exactly the two LANDED fish
  --assert-watch 1590:dc:2:eq:6
  --assert-watch 1590:fl:3:eq:1
  --assert-watch 1590:fl:2:eq:2
  --assert-watch 1590:fl:4:eq:1803
  --assert-watch 1590:fl:5:eq:9
  --assert-text "1590:CATCH LOG 2"
  --assert-text "1590:MOON EEL MYTHIC 11"
  --assert-text "1590:MUD BREAM COMMON 9"
  --assert-watch 1680:dc:2:eq:1
  --assert-watch 1680:fl:3:eq:0
  # the mailbox magic + main-stream words never flinched: same lures /
  # catches / score as the sim said at the cards
  --assert-watch 1700:dc:13:eq:2
  --assert-watch 1700:dc:14:eq:20
  --assert-watch 1700:dc:12:eq:2
  --assert-watch 1700:fl:13:eq:1179210568
)

echo "== P6: species tables + catch log — the fish have names, the log remembers (run 1) =="
H "$OUT/p6.png" --frames 1750 $W $WA $P6_ROUTE \
  --watch-log "$OUT/p6-run1.csv" \
  --shot "830:$OUT/p6-log1.png" --shot "1590:$OUT/p6-log2.png" \
  --require-distinct \
  "${P6_ASSERTS[@]}"

echo "== P6: run 2 (must be byte-identical) =="
H "$OUT/p6b.png" --frames 1750 $W $WA $P6_ROUTE \
  --watch-log "$OUT/p6-run2.csv" \
  "${P6_ASSERTS[@]}"
cmp "$OUT/p6-run1.csv" "$OUT/p6-run2.csv"
echo "P6 run-twice: byte-identical"

# ---------------------------------------------------------------------------
# P7 — growth cut 4, line upgrades bought with score + the SRAM bank. All
# pins transcribed from observed --watch-log runs of THESE routes (the
# listen-then-script method). The tension math is untouched by the cut
# (reel gain 6 + weight/8 + surge 18, slack decay 9), so runs A and C share
# every tension literal while the LINE literals diverge by exactly the
# reel-rate delta — the arithmetic witness the concept's tradeoff names.
# gl_save.h payload: magic "DCLINE1", {int bank; int tier;}. mGBA wall
# (docs/PLATFORM-LIMITS.md, quoted not re-probed): core.load_save()
# segfaults this binding — --savefile is the bus-copy path, per the Lumen
# Drift two-boot job in .github/workflows/headless-boot.yml.
# ---------------------------------------------------------------------------

WL='--watch lm:dc_linemeta:8'
P7_FIGHT='--keys 240-244:START --keys 300-360:A --keys 471-700:A'

# Run A — fresh cart (NO savefile): the WORN LINE baseline. lm magic
# 'LINE' = 1279872581; tier 0 / bank 0 / snap 600 / reel 5 / next cost 15.
P7A_ASSERTS=(
  --assert-watch 100:lm:0:eq:1279872581
  --assert-watch 100:lm:1:eq:0
  --assert-watch 100:lm:2:eq:0
  --assert-watch 100:lm:3:eq:600
  --assert-watch 100:lm:4:eq:5
  --assert-watch 100:lm:5:eq:0
  --assert-watch 100:lm:6:eq:15
  --assert-watch 100:dc:4:eq:3739863639
  --assert-text "100:SELECT: LINE SHOP  BANK 0"
  # the default lake's first cast (72m wt 11), fought hold-only: bite at
  # 471 with line 432; the worn line reels 5/frame — bite+29 frames held
  # -> line 432-5*29 = 287; tension 203 at the same frame
  --assert-watch 471:dc:2:eq:3
  --assert-watch 471:dc:9:eq:432
  --assert-watch 500:dc:8:eq:203
  --assert-watch 500:dc:9:eq:287
  --assert-watch 540:dc:8:eq:573
  --assert-watch 540:dc:9:eq:87
  # the worn line SNAPS at exactly 600 on frame 542
  --assert-watch 542:dc:2:eq:4
  --assert-watch 542:dc:15:eq:2
  --assert-watch 542:dc:8:eq:600
  --assert-watch 542:dc:9:eq:77
  --assert-watch 542:dc:12:eq:2
)

echo "== P7 run A: the worn-line baseline on a fresh cart (no savefile) =="
H "$OUT/p7a.png" --frames 700 $W $WA $WL $P7_FIGHT \
  --watch-log "$OUT/p7a-run1.csv" \
  "${P7A_ASSERTS[@]}"

# Run B — boot 1 on a factory-fresh savefile: the P2 route to dusk (score
# 18), the dusk deposit, the shop purchase, the refused purchase.
P7B_ROUTE="$CORE_ROUTE --keys 2560-2562:R --keys 2600-2602:A --keys 2620-2622:A --keys 2660-2662:SELECT"

P7B_ASSERTS=(
  # factory-fresh SRAM (0xFF) loads as NO save: worn line, bank 0
  --assert-watch 100:lm:1:eq:0
  --assert-watch 100:lm:2:eq:0
  --assert-text "100:SELECT: LINE SHOP  BANK 0"
  # dusk deposits the run's score into the bank the frame dusk falls
  --assert-watch 2234:dc:2:eq:5
  --assert-watch 2234:dc:14:eq:18
  --assert-watch 2234:lm:2:eq:18
  --assert-text "2500:SCORE 18"
  --assert-text "2500:R: LINE SHOP  BANK 18"
  # R opens the LINE SHOP (state 7, open flag up), glyph-exact: the worn
  # line, the next tier's price, and the doc's tradeoff published
  --assert-watch 2559:dc:2:eq:5
  --assert-watch 2559:lm:5:eq:0
  --assert-watch 2565:dc:2:eq:7
  --assert-watch 2565:lm:5:eq:1
  --assert-text "2590:LINE SHOP"
  --assert-text "2590:BANK 18"
  --assert-text "2590:LINE: WORN LINE"
  --assert-text "2590:NEXT: BRAIDED LINE COST 15"
  --assert-text "2590:SNAP 600>750  REEL 5>4"
  --assert-text "2590:A: BUY  SELECT: BACK"
  # A BUYS the braided line: bank 18-15=3, tier 1, snap 750, reel 4,
  # next cost 40 — bought with score, the concept's exact words
  --assert-watch 2605:lm:1:eq:1
  --assert-watch 2605:lm:2:eq:3
  --assert-watch 2605:lm:3:eq:750
  --assert-watch 2605:lm:4:eq:4
  --assert-watch 2605:lm:6:eq:40
  # the second A is REFUSED (STEEL costs 40 > bank 3): nothing moves
  --assert-watch 2640:lm:1:eq:1
  --assert-watch 2640:lm:2:eq:3
  --assert-text "2640:BANK 3"
  --assert-text "2640:LINE: BRAIDED LINE"
  --assert-text "2640:NEXT: STEEL LINE COST 40"
  --assert-text "2640:SNAP 750>900  REEL 4>3"
  # SELECT returns to dusk; the dusk line now shows the spent bank
  --assert-watch 2665:dc:2:eq:5
  --assert-watch 2665:lm:5:eq:0
  --assert-text "2700:R: LINE SHOP  BANK 3"
)

rm -f "$OUT/p7-boot1a.sav" "$OUT/p7-boot1b.sav"

echo "== P7 run B: boot 1 — dusk banks the score, the shop spends it (run 1) =="
H "$OUT/p7b.png" --frames 2720 $W $WA $WL $P7B_ROUTE \
  --savefile "$OUT/p7-boot1a.sav" \
  --watch-log "$OUT/p7b-run1.csv" \
  --shot "2590:$OUT/p7-shop-before.png" --shot "2640:$OUT/p7-shop-after.png" \
  --require-distinct \
  "${P7B_ASSERTS[@]}"

echo "== P7 run B: run 2 from a second factory-fresh savefile (must be byte-identical, savefile too) =="
H "$OUT/p7b2.png" --frames 2720 $W $WA $WL $P7B_ROUTE \
  --savefile "$OUT/p7-boot1b.sav" \
  --watch-log "$OUT/p7b-run2.csv" \
  "${P7B_ASSERTS[@]}"
cmp "$OUT/p7b-run1.csv" "$OUT/p7b-run2.csv"
cmp "$OUT/p7-boot1a.sav" "$OUT/p7-boot1b.sav"
echo "P7 run B run-twice: watch-logs AND written savefiles byte-identical"

# Run C — boot 2, a SEPARATE emulator boot on run B's savefile (the Lumen
# Drift two-boot pattern): the purchase survived the power cycle, and the
# SAME hold-only fight as run A carries the tradeoff arithmetic.
P7C_ROUTE="--keys 120-122:SELECT --keys 180-182:SELECT $P7_FIGHT"

P7C_ASSERTS=(
  # TWO-BOOT PERSISTENCE WITNESS: bank 3 / tier 1 / snap 750 / reel 4
  # restored from SRAM on a separate boot, on the title before any input
  --assert-watch 110:lm:1:eq:1
  --assert-watch 110:lm:2:eq:3
  --assert-watch 110:lm:3:eq:750
  --assert-watch 110:lm:4:eq:4
  --assert-watch 110:dc:4:eq:3739863639
  --assert-text "110:SELECT: LINE SHOP  BANK 3"
  # the title shop shows the owned line, glyph-exact
  --assert-watch 150:dc:2:eq:7
  --assert-watch 150:lm:5:eq:1
  --assert-text "150:BANK 3"
  --assert-text "150:LINE: BRAIDED LINE"
  --assert-text "150:NEXT: STEEL LINE COST 40"
  --assert-text "150:SNAP 750>900  REEL 4>3"
  --assert-watch 200:dc:2:eq:0
  # THE TRADEOFF WITNESS, arithmetic on the SAME fight as run A: same
  # lake (seed pinned above), same bite (frame 471, line 432), same
  # tension trajectory (203 at 500, 573 at 540 — tension math untouched
  # by the cut) — but the braided line reels 4/frame, not 5: line
  # 432-4*29 = 316 at frame 500 (run A: 287 = 432-5*29) and 156 at 540
  # (run A: 87)
  --assert-watch 471:dc:2:eq:3
  --assert-watch 471:dc:9:eq:432
  --assert-watch 500:dc:8:eq:203
  --assert-watch 500:dc:9:eq:316
  --assert-watch 540:dc:8:eq:573
  --assert-watch 540:dc:9:eq:156
  # on frame 542 — the WORN line's snap frame — the braided line is
  # still FIGHTING at tension 623, past the old 600 threshold
  --assert-watch 542:dc:2:eq:3
  --assert-watch 542:dc:8:eq:623
  # and it SNAPS at exactly 750 on frame 548: higher snap threshold AND
  # slower reel, the doc's tradeoff, both halves pinned on one fight
  --assert-watch 548:dc:2:eq:4
  --assert-watch 548:dc:15:eq:2
  --assert-watch 548:dc:8:eq:750
  --assert-watch 548:dc:9:eq:124
  --assert-watch 548:dc:12:eq:2
)

echo "== P7 run C: boot 2 — the purchase survives the power cycle; the tradeoff on the same fight (run 1) =="
H "$OUT/p7c.png" --frames 700 $W $WA $WL $P7C_ROUTE \
  --savefile "$OUT/p7-boot1a.sav" \
  --watch-log "$OUT/p7c-run1.csv" \
  --shot "150:$OUT/p7-title-shop.png" \
  "${P7C_ASSERTS[@]}"

echo "== P7 run C: run 2 (must be byte-identical; no dusk = no SRAM write, savefile untouched) =="
H "$OUT/p7c2.png" --frames 700 $W $WA $WL $P7C_ROUTE \
  --savefile "$OUT/p7-boot1a.sav" \
  --watch-log "$OUT/p7c-run2.csv" \
  "${P7C_ASSERTS[@]}"
cmp "$OUT/p7c-run1.csv" "$OUT/p7c-run2.csv"
cmp "$OUT/p7-boot1a.sav" "$OUT/p7-boot1b.sav"
echo "P7 run C run-twice: byte-identical; savefile untouched by two more boots"

# ---------------------------------------------------------------------------
# P8/P9/P10 — growth cut 5, the art pass (lake gradient by depth, silhouette
# fish, rod-bend tension sprite; presentation ONLY — P1-P7 above carried
# verbatim). The proof reads the art back off the hardware:
#   dc_artmeta (ELF symbol, 8 words): 'DART' magic · gradient depth word ·
#     band-0/band-9 BGR555 (the gradient law) · fish frame (255 hidden) ·
#     rod bend frame (255 hidden) · fish-flipped flag.
#   0x04000000 DISPCNT: 0xF840 — the lake bg's enable bit is on in every
#     state (the lake IS the screen; v0.5 had no bg at all).
#   0x050001E0 BG palette RAM (bank 15, entries 0/1): band-0's halfword in
#     the high half — 0x6A050000 at depth 0, 0x44E20000 at 72, 0x14210000
#     at 172: the gradient law IN palette RAM, moving with the sink.
#   0x050001F4 BG palette RAM (entries 10/11): band-9 + the surface glint
#     0x7BB8 — 0x7BB82021 at depth 0, 0x7BB81021 once dim saturates.
#   0x050003C4 OBJ palette RAM: 0x67162E78 = rod tip (24,19,11) + line
#     (22,24,25) BGR555 — the committed dc_rod art.
#   0x050003E4 OBJ palette RAM: 0x4E2E3D8A = the UNCOMMON + RARE fish
#     shades — the committed dc_fish art.
#   0x06000800 map screenblock: 0xF00BF00B = surface-shimmer cells (tile
#     11, palette bank 15); 0x06000020: 0x11111111 = band tile 1's solid
#     4bpp rows — the lake baked into VRAM.
# All pins transcribed from observed --watch-log runs of THESE routes and
# cross-checked against an offline replica of the gradient/rod laws.
# ---------------------------------------------------------------------------

AW='--watch aw:dc_artmeta:8 --watch io:0x04000000:1 --watch grad0:0x050001E0:1 --watch grad9:0x050001F4:1 --watch rodpal:0x050003C4:1 --watch fishpal:0x050003E4:1 --watch vmap:0x06000800:1 --watch vtile:0x06000020:1'

P8_ART_ASSERTS=(
  # title: the lake at depth 0 — dusk-blue gradient (band0 0x6A05, band9
  # 0x2021), no fish, no rod; the committed art's colors + cells in RAM
  --assert-watch 100:aw:0:eq:0x44415254
  --assert-watch 100:aw:1:eq:0
  --assert-watch 100:aw:2:eq:0x6A05
  --assert-watch 100:aw:3:eq:0x2021
  --assert-watch 100:aw:4:eq:255
  --assert-watch 100:aw:5:eq:255
  --assert-watch 100:io:0:eq:0xF840
  --assert-watch 100:grad0:0:eq:0x6A050000
  --assert-watch 100:grad9:0:eq:0x7BB82021
  --assert-watch 100:rodpal:0:eq:0x67162E78
  --assert-watch 100:fishpal:0:eq:0x4E2E3D8A
  --assert-watch 100:vmap:0:eq:0xF00BF00B
  --assert-watch 100:vtile:0:eq:0x11111111
  # charge: the rod comes up, at rest (bend 0)
  --assert-watch 240:aw:5:eq:0
  # the 72m sink: the gradient law lands in the mailbox AND palette RAM —
  # the lake is literally darker at depth 72 (band0 0x6A05 -> 0x44E2)
  --assert-watch 430:aw:1:eq:72
  --assert-watch 430:aw:2:eq:0x44E2
  --assert-watch 430:grad0:0:eq:0x44E20000
  --assert-watch 430:grad9:0:eq:0x7BB81021
  # the bite: the SILHOUETTE appears — frame 7 = the species word fl[0]
  # (MOON EEL: MIDWATER size, MYTHIC pale shade); rod still at rest
  --assert-watch 471:aw:4:eq:7
  --assert-watch 471:aw:5:eq:0
  # the surge: the fish FLIPS to run (aw[6] rides dc[11])
  --assert-watch 560:aw:6:eq:1
  # reeling a rest at tension 98: rod bend 1 = 98*7/600 — the analog
  # gauge moves with the same word the TENSION bar and the ratchet read
  --assert-watch 620:aw:5:eq:1
  --assert-watch 620:aw:6:eq:0
  # the CATCH card shows the landed fish (frame 7 stays), rod down
  --assert-watch 680:aw:4:eq:7
  --assert-watch 680:aw:5:eq:255
  # cast 2's fight at tension 588: rod bend 6 = 588*7/600 (near-snap);
  # the result card then hides both sprites
  --assert-watch 1025:aw:5:eq:6
  --assert-watch 1027:aw:4:eq:255
  --assert-watch 1027:aw:5:eq:255
  # the max-charge 172m cast: mid-sink the lake keeps darkening (depth
  # 100 -> 0x3881), and at the ABYSS bite the palette RAM holds the law
  # at 172 (0x1421) with TRENCH MAW's big band-3 silhouette on the line
  --assert-watch 1450:aw:1:eq:100
  --assert-watch 1450:aw:2:eq:0x3881
  --assert-watch 1529:aw:1:eq:172
  --assert-watch 1529:aw:2:eq:0x1421
  --assert-watch 1529:grad0:0:eq:0x14210000
  --assert-watch 1529:aw:4:eq:13
  --assert-watch 1593:aw:5:eq:6
  # dusk: the lake surfaces (depth word 0 -> the title's exact palette),
  # no fish, no rod — and the whole route's sim words landed on P2's
  # literals below (the art cannot be felt by the sim)
  --assert-watch 2500:aw:1:eq:0
  --assert-watch 2500:aw:2:eq:0x6A05
  --assert-watch 2500:aw:4:eq:255
  --assert-watch 2500:aw:5:eq:255
  --assert-watch 2500:grad0:0:eq:0x6A050000
  --assert-watch 2500:io:0:eq:0xF840
)

echo "== P8: THE LAKE GRADIENT + ROD BEND — the P2 route with the hardware watched (run 1) =="
H "$OUT/p8.png" --frames 2520 $W $WA $AW $CORE_ROUTE \
  --watch-log "$OUT/p8-run1.csv" \
  --shot "100:$OUT/p8-title.png" --shot "430:$OUT/p8-sink72.png" \
  --shot "620:$OUT/p8-fight.png" --shot "690:$OUT/p8-catch.png" \
  --shot "1540:$OUT/p8-abyss.png" \
  --require-distinct \
  "${P8_ART_ASSERTS[@]}" \
  "${P2_ASSERTS[@]}"

echo "== P8: run 2 (must be byte-identical) =="
H "$OUT/p8b.png" --frames 2520 $W $WA $AW $CORE_ROUTE \
  --watch-log "$OUT/p8-run2.csv" \
  "${P8_ART_ASSERTS[@]}" \
  "${P2_ASSERTS[@]}"
cmp "$OUT/p8-run1.csv" "$OUT/p8-run2.csv"
echo "P8 run-twice: byte-identical"

# P9 — THE SPECIES SILHOUETTES: the P6 route (three species across three
# depth bands) with the art watched, every P6 pin carried. The fish frame
# IS the species word: 7 (MIDWATER MYTHIC — mid size, palest shade), 0
# (SHALLOWS COMMON — smallest, darkest), 9 (DEEPS UNCOMMON — big); it is
# HIDDEN while sinking (the species keeps its secret until the card), and
# the SNAP card never shows it (fl[0] stays 9 while aw[4] drops to 255).
P9_ART_ASSERTS=(
  # cast 1 sinking at depth 10: gradient mid-law 0x65E5, fish HIDDEN
  # (fl[0] already knows species 7 — the player must not)
  --assert-watch 365:aw:1:eq:10
  --assert-watch 365:aw:2:eq:0x65E5
  --assert-watch 365:aw:4:eq:255
  --assert-watch 680:aw:4:eq:7
  --assert-watch 680:aw:5:eq:255
  # the catch log screen: no rod, no fish, surface palette
  --assert-watch 830:aw:4:eq:255
  --assert-watch 830:aw:5:eq:255
  --assert-watch 830:grad0:0:eq:0x6A050000
  # cast 2, THE SHALLOWS at 42m: gradient 0x5564 in mailbox AND palette
  # RAM; MUD BREAM's silhouette (frame 0) through fight and catch card
  --assert-watch 986:aw:1:eq:42
  --assert-watch 986:aw:2:eq:0x5564
  --assert-watch 986:grad0:0:eq:0x55640000
  --assert-watch 986:aw:4:eq:0
  --assert-watch 1037:aw:4:eq:0
  --assert-watch 1037:aw:5:eq:255
  # cast 3, THE DEEPS at 144m: the law saturates green (0x2021), IRON
  # GAR's big silhouette (frame 9) bends the rod (168*7/600 = 1 at 1400)
  --assert-watch 1375:aw:1:eq:144
  --assert-watch 1375:aw:2:eq:0x2021
  --assert-watch 1375:grad0:0:eq:0x20210000
  --assert-watch 1375:aw:4:eq:9
  --assert-watch 1400:aw:5:eq:1
  # the SNAP card: the fish vanishes UNREVEALED — the sim's species word
  # still says 9 (P6 pins it), the screen never showed a name
  --assert-watch 1444:aw:4:eq:255
  --assert-watch 1444:aw:5:eq:255
  # the reopened log: sprites down, lake at the surface
  --assert-watch 1590:aw:4:eq:255
  --assert-watch 1590:grad0:0:eq:0x6A050000
)

echo "== P9: THE SPECIES SILHOUETTES — the P6 route with the art watched (run 1) =="
H "$OUT/p9.png" --frames 1750 $W $WA $AW $P6_ROUTE \
  --watch-log "$OUT/p9-run1.csv" \
  --shot "1400:$OUT/p9-irongar.png" \
  --require-distinct \
  "${P9_ART_ASSERTS[@]}" \
  "${P6_ASSERTS[@]}"

echo "== P9: run 2 (must be byte-identical) =="
H "$OUT/p9b.png" --frames 1750 $W $WA $AW $P6_ROUTE \
  --watch-log "$OUT/p9-run2.csv" \
  "${P9_ART_ASSERTS[@]}" \
  "${P6_ASSERTS[@]}"
cmp "$OUT/p9-run1.csv" "$OUT/p9-run2.csv"
echo "P9 run-twice: byte-identical"

# P10 — THE HONEST GAUGE: the rod-bend law follows the CURRENT line's snap
# threshold (lm[3]), proven exactly like P7 proved the tradeoff — the SAME
# hold-only fight under two SRAM states, the savefile the only treatment.
# Worn line (leg A, no savefile): tension 203 -> bend 2, 573 -> bend 6.
# Braided line (leg C, P7 run B's savefile): the SAME tension words bend
# the rod LESS — 203 -> 1, 573 -> 5 — and on frame 542 (the worn line's
# snap frame) the braided rod is still up at bend 5 under tension 623.
# P7's own A/C sim asserts are carried verbatim in both legs.
echo "== P10 leg A: the worn-line rod on a fresh cart (run 1) =="
P10A_ASSERTS=(
  --assert-watch 500:aw:5:eq:2
  --assert-watch 540:aw:5:eq:6
  --assert-watch 500:aw:4:eq:7
  --assert-watch 542:aw:4:eq:255
  --assert-watch 542:aw:5:eq:255
)
H "$OUT/p10a.png" --frames 700 $W $WA $WL $AW $P7_FIGHT \
  --watch-log "$OUT/p10a-run1.csv" \
  "${P10A_ASSERTS[@]}" \
  "${P7A_ASSERTS[@]}"

echo "== P10 leg A: run 2 (must be byte-identical) =="
H "$OUT/p10a2.png" --frames 700 $W $WA $WL $AW $P7_FIGHT \
  --watch-log "$OUT/p10a-run2.csv" \
  "${P10A_ASSERTS[@]}" \
  "${P7A_ASSERTS[@]}"
cmp "$OUT/p10a-run1.csv" "$OUT/p10a-run2.csv"
echo "P10 leg A run-twice: byte-identical"

P10C_ASSERTS=(
  --assert-watch 500:aw:5:eq:1
  --assert-watch 540:aw:5:eq:5
  --assert-watch 542:aw:5:eq:5
  --assert-watch 542:aw:4:eq:7
  --assert-watch 548:aw:4:eq:255
  --assert-watch 548:aw:5:eq:255
)

echo "== P10 leg C: the braided-line rod on P7's savefile — same fight, less bend (run 1) =="
H "$OUT/p10c.png" --frames 700 $W $WA $WL $AW $P7C_ROUTE \
  --savefile "$OUT/p7-boot1a.sav" \
  --watch-log "$OUT/p10c-run1.csv" \
  --shot "540:$OUT/p10-braided-bend.png" \
  "${P10C_ASSERTS[@]}" \
  "${P7C_ASSERTS[@]}"

echo "== P10 leg C: run 2 (must be byte-identical; no dusk = no SRAM write) =="
H "$OUT/p10c2.png" --frames 700 $W $WA $WL $AW $P7C_ROUTE \
  --savefile "$OUT/p7-boot1a.sav" \
  --watch-log "$OUT/p10c-run2.csv" \
  "${P10C_ASSERTS[@]}" \
  "${P7C_ASSERTS[@]}"
cmp "$OUT/p10c-run1.csv" "$OUT/p10c-run2.csv"
cmp "$OUT/p7-boot1a.sav" "$OUT/p7-boot1b.sav"
echo "P10 leg C run-twice: byte-identical; savefile untouched"

echo "ALL DEEPCAST PROOFS PASS"
