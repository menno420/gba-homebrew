# Session 41 — Cindervault: turn-based dungeon-dive roguelike, GBA (breadth program, night run)

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/cindervault`, started 04:02:32Z per `date -u`)
- mission: game #6 of tonight's breadth program — a NEW original GBA micro-game distinct in genre from every sibling (Lumen Drift GBA gravity drifter, Gloamline NDS survival, Brineward NDS naval PR #82, Undertow web reflex arcade, Tiltstone web gravity puzzle, Drift Garden mobile idle-garden PR #84, Deepcast GBA fishing tension arcade PR #83): **Cindervault**, a seeded turn-based dungeon-dive roguelike. Dive 5 floors of a seeded dungeon; every step burns torchlight (torch 0 = DARKNESS), embers refill the torch, bump-combat monsters chase one step per player turn (player HP 0 = SLAIN), reach the stairs on floor 5 to open the vault (WIN). Turns advance ONLY on player input — no frame-time in the logic. Deliverables: `games/cindervault/` (Butano game + CONCEPT.md), `dist/cindervault.gba` + README row, headless proof via `tools/headless-screenshot.py` with a `cv_telemetry` mailbox.
- session number: 41 chosen because open PR #85's card (`.sessions/2026-07-13-release-packaging.md`) self-identifies as session 40 (the current max — #83 is 39, #82 is 38), PR #84's drift-garden card is unnumbered, and no card on main or in an open PR claims 41.
- provenance: ORDER 005 (owner night-run directive, 2026-07-13, scribed verbatim in `control/inbox.md` @ HEAD d87f9ad): "THE BREADTH PROGRAM starts now: multiple NEW small games tonight — each a playable prototype slice + a one-page concept (genre, loop, platform, sellability guess)." Dispatched breadth-program session; no merge action, no labels. Base: main @ d87f9ad.
- heartbeat: SKIPPED by dispatch constraint (`control/status.md` intentionally untouched this session, no `control/outbox.md` writes); recorded here instead.
- 📊 Model: Fable 5 (family-level self-report from this session's own harness, per ORDER 003)

## What this session did

1. This card + `control/claims/claude-cindervault.md`, born red (claim before build; commit `9f7f241`). Checked collisions first at main tip d87f9ad: open PRs are only #82 (Brineward bands), #83 (Deepcast), #84 (Drift Garden), #85 (release packaging) — no dungeon-crawler/roguelike sibling, no `cindervault` anywhere in claims or branches. PR #86 opened READY at that commit.
2. Toolchain: `tools/setup-toolchain.sh` (pinned devkitARM r68 via the leseratte10 mirror + Butano 21.7.1) ran clean, no walls — the official devkitPro infra stays Cloudflare-403 behind the proxy, the mirror recipe is the sanctioned route. Headless: `apt-get install -y mgba-sdl` + the preinstalled `mgba==0.10.2` binding — `import mgba.core` OK. No SRAM in this game, so the `core.load_save()` segfault wall (deferred savedata flush; PLATFORM-LIMITS.md) never comes near.
3. **Cindervault v0.1** (`games/cindervault/`, commit `5907d00`): single `src/main.cpp` over the shared `games/common/butano-game.mak` fragment; text-only presentation — the 13x8 dungeon is glyph rows in Butano's common FIXED 8x8 font (fixed pitch keeps the grid square; ~4 packed sprites per row, well inside the sprite budget), HUD/title/end text in the common VARIABLE 8x8 font (the one `--assert-text` can read). Deterministic core: integer math only, one xorshift32 PRNG at fixed seed `0xC1DE5EED` (shown on the title card), RNG consumed ONLY inside floor generation, turns advance only on input edges. Floors are carved by a seeded random walk from the spawn tile, so spawn→stairs is connected BY CONSTRUCTION (stairs = the carved tile farthest from spawn); 3 embers + 2-4 monsters per floor placed only on carved tiles ('m' 1-2 HP, one 3-HP 'M' from floor 3). Wall bumps cost nothing; every consumed turn burns 1 torch; ember +25; monsters chase one greedy step (deterministic tie-break: the longer axis, horizontal on ties) and bite 1 HP when adjacent. Player bump hits for 2 — hit-for-1 was route-searched first and proved unsurvivable against equal-speed chasers, so the constant is a design decision recorded here, not a fudge. Score = 100 x floors cleared + 25 x embers + 10 x kills + remaining torch on a win.
4. Telemetry mailbox `cv_telemetry` (C linkage, `volatile unsigned[16]`, written every frame): [0]=0x43494E44 'CIND', [1]=0x56414C54 'VALT', [2] state (0 title / 1 playing / 2 win / 3 lose), [3] seed, [4] floor, [5] hp, [6] torch, [7] embers, [8] kills, [9] score, [10] turn count, [11] player x, [12] player y, [13] lose reason (1 darkness / 2 slain), [14] monsters alive on floor, [15] frames since boot.
5. **Route derivation, the transcription way (Deepcast's listen-then-script loop, one step further):** a host-side Python mirror of the PRNG + generation + turn logic proposed win/lose input routes (a Dijkstra bot that clears adjacent biters, sweeps embers, then takes the stairs), and the emulator watch-log CSV then verified the mirror **turn-for-turn: 0 mismatches across all 93 win-route turns and all 220 lose-route turns**. Every assert below is pinned FROM the observed CSV, never hand-computed. The one iteration this exposed: the first HUD text pin failed because the HUD's last glyph clipped at the 240px screen edge (`SCORE 220` ink reached x=239, template needed 244) — HUD respaced/moved left, rebuilt, all green.
6. **Headless proof — WIN run** (full seeded route: title → floor 1 [2 kills, 3 embers] → … → floor 5 → VAULT REACHED): **64 `--assert-watch` pins + 8 `--assert-text` font-pixel pins + 4 non-blank screenshots + `--require-distinct`, exit 0** — title magic+seed, state 0→1, floor progression 1→2→3→4→5 (with exact score/turn/ember/kill counts at each descent), an ember pickup (torch 214→238, embers 0→1), the first kills (kills 0→1→2, monsters 2→1→0), hp attrition to 2 on floor 5, win transition ([2]=2, score exactly **1518** = 500 + 25x15 + 10x14 + 503 torch, turn 93).
7. **Headless proof — LOSE run** (darkness, deterministically: clear floor 1's two monsters in 5 turns, then A-wait out the torch): **24 `--assert-watch` pins + 3 `--assert-text` pins, exit 0** — torch 220→136@turn84→0, state 3, lose reason [13]=1, `DARKNESS. THE TORCH IS OUT` rendered, score 20.
8. **Determinism:** each proof run twice back-to-back — outputs byte-identical (`diff` clean, both runs). Two `make clean` rebuilds byte-identical; `dist/cindervault.gba` **120,984 bytes**, sha256 `d3ad8c78693fec8467273a78c174da722a314a9c03494562a3b042d9b1a9a52e`; the win proof re-run on the clean-rebuilt ROM produced byte-identical output again (proof holds at the dist bytes). README row added.
9. `games/cindervault/CONCEPT.md` (sibling template): pitch / genre (+ explicit distinct-from-all-seven-siblings disclaimer) / core loop / target platform / honest sellability / growth path.
10. `python3 bootstrap.py check --strict` pre-flip: exit 1 purely on this card's in-progress badge ("HOLD (by design)… nothing to investigate") plus the pre-existing advisory on `order-005-scribe.md`'s claim format (not this session's file; this session's claim parses clean). Re-run green after the flip.
11. Card completed + badge flipped + `control/claims/claude-cindervault.md` deleted in this same close commit (session close per control/README.md). CI "ROM builds": green at the born-red head (run 29223032646) and at the game head (run 29223821217). No merge action, no labels, no control/status or outbox writes; PR #86 parked for the owner's morning sweep.

## Harness evidence (verbatim; `$S` = this session's scratchpad dir)

WIN-run invocation (from the repo root; ROM/ELF at the clean-rebuilt tree of commit `5907d00`):

```
python3 tools/headless-screenshot.py games/cindervault/cindervault.gba "$S/cv-win.png" \
  --frames 1000 \
  --keys 240-242:START \
  --keys-pattern 300-314:6:2:DOWN --keys 318-320:RIGHT --keys-pattern 324-332:6:2:DOWN \
  --keys 336-338:RIGHT --keys-pattern 342-374:6:2:LEFT --keys 378-380:DOWN \
  --keys-pattern 384-392:6:2:LEFT --keys 396-398:DOWN --keys 402-404:LEFT \
  --keys-pattern 408-416:6:2:UP --keys-pattern 420-428:6:2:LEFT --keys-pattern 432-440:6:2:UP \
  --keys-pattern 444-452:6:2:RIGHT --keys-pattern 456-494:6:2:LEFT --keys 498-500:RIGHT \
  --keys 504-506:DOWN --keys-pattern 510-518:6:2:RIGHT --keys 522-524:UP \
  --keys 528-530:RIGHT --keys-pattern 534-554:6:2:DOWN --keys 558-560:RIGHT \
  --keys-pattern 564-572:6:2:DOWN --keys-pattern 576-590:6:2:RIGHT --keys-pattern 594-608:6:2:DOWN \
  --keys-pattern 612-620:6:2:RIGHT --keys-pattern 624-632:6:2:UP --keys 636-638:RIGHT \
  --keys-pattern 642-656:6:2:UP --keys 660-662:DOWN --keys-pattern 666-692:6:2:RIGHT \
  --keys 696-698:UP --keys 702-704:RIGHT --keys-pattern 708-722:6:2:UP \
  --keys 726-728:RIGHT --keys 732-734:LEFT --keys 738-740:UP --keys 744-746:DOWN \
  --keys 750-752:LEFT --keys-pattern 756-770:6:2:UP --keys-pattern 774-782:6:2:LEFT \
  --keys 786-788:UP --keys 792-794:RIGHT --keys-pattern 798-812:6:2:UP \
  --keys-pattern 816-848:6:2:LEFT --keys 852-854:UP \
  --shot 100:"$S/cv-title.png" --shot 400:"$S/cv-floor2.png" --shot 728:"$S/cv-floor5.png" \
  --require-distinct \
  --elf games/cindervault/cindervault.elf --watch cv:cv_telemetry:16 \
  --assert-text "100:CINDERVAULT" --assert-text "100:SEED C1DE5EED" \
  --assert-text "250:FLR 1 HP 10 TORCH 220 SCORE 0" \
  --assert-text "400:FLR 2 HP 10 TORCH 303 SCORE 220" \
  --assert-text "990:VAULT REACHED" --assert-text "990:SCORE 1518" \
  --assert-text "990:EMBERS 15  KILLS 14  TORCH 503" --assert-text "990:PRESS START" \
  --assert-watch 100:cv:0:eq:1128877636 --assert-watch 100:cv:1:eq:1447119956 \
  --assert-watch 100:cv:2:eq:0 --assert-watch 100:cv:3:eq:3252575981 \
  --assert-watch 100:cv:15:gt:0 \
  --assert-watch 250:cv:2:eq:1 --assert-watch 250:cv:4:eq:1 --assert-watch 250:cv:5:eq:10 \
  --assert-watch 250:cv:6:eq:220 --assert-watch 250:cv:7:eq:0 --assert-watch 250:cv:8:eq:0 \
  --assert-watch 250:cv:9:eq:0 --assert-watch 250:cv:10:eq:0 --assert-watch 250:cv:11:eq:6 \
  --assert-watch 250:cv:12:eq:1 --assert-watch 250:cv:14:eq:2 \
  --assert-watch 320:cv:8:eq:1 --assert-watch 320:cv:14:eq:1 --assert-watch 320:cv:6:eq:216 \
  --assert-watch 330:cv:8:eq:2 --assert-watch 330:cv:14:eq:0 \
  --assert-watch 340:cv:7:eq:1 --assert-watch 340:cv:6:eq:238 \
  --assert-watch 362:cv:7:eq:3 --assert-watch 362:cv:6:eq:284 \
  --assert-watch 380:cv:4:eq:2 --assert-watch 380:cv:9:eq:195 --assert-watch 380:cv:10:eq:14 \
  --assert-watch 400:cv:4:eq:2 --assert-watch 400:cv:6:eq:303 --assert-watch 400:cv:9:eq:220 \
  --assert-watch 494:cv:4:eq:3 --assert-watch 494:cv:9:eq:390 --assert-watch 494:cv:10:eq:33 \
  --assert-watch 494:cv:7:eq:6 --assert-watch 494:cv:8:eq:4 \
  --assert-watch 620:cv:4:eq:4 --assert-watch 620:cv:5:eq:8 --assert-watch 620:cv:9:eq:595 \
  --assert-watch 620:cv:10:eq:54 \
  --assert-watch 728:cv:4:eq:5 --assert-watch 728:cv:5:eq:5 --assert-watch 728:cv:9:eq:800 \
  --assert-watch 728:cv:10:eq:72 --assert-watch 728:cv:14:eq:4 \
  --assert-watch 790:cv:5:eq:2 --assert-watch 790:cv:14:eq:1 \
  --assert-watch 860:cv:2:eq:2 --assert-watch 860:cv:4:eq:5 --assert-watch 860:cv:5:eq:2 \
  --assert-watch 860:cv:6:eq:503 --assert-watch 860:cv:7:eq:15 --assert-watch 860:cv:8:eq:14 \
  --assert-watch 860:cv:9:eq:1518 --assert-watch 860:cv:10:eq:93 --assert-watch 860:cv:11:eq:1 \
  --assert-watch 860:cv:12:eq:1 --assert-watch 860:cv:13:eq:0 --assert-watch 860:cv:14:eq:0 \
  --assert-watch 990:cv:2:eq:2 --assert-watch 990:cv:9:eq:1518 \
  --assert-watch 990:cv:0:eq:1128877636 --assert-watch 990:cv:1:eq:1447119956 \
  --assert-watch 990:cv:3:eq:3252575981
```

Output (exit 0; `$S` substituted for the absolute scratchpad path, nothing else edited):

```
watch cv: u32 words 16 at 0x03001d14
assert-watch: cv[0] = 1128877636 eq 1128877636 OK (frame 100)
assert-watch: cv[1] = 1447119956 eq 1447119956 OK (frame 100)
assert-watch: cv[2] = 0 eq 0 OK (frame 100)
assert-watch: cv[3] = 3252575981 eq 3252575981 OK (frame 100)
assert-watch: cv[15] = 92 gt 0 OK (frame 100)
assert-text: 'CINDERVAULT' found at (16, 26) (frame 100)
assert-text: 'SEED C1DE5EED' found at (16, 46) (frame 100)
assert-watch: cv[2] = 1 eq 1 OK (frame 250)
assert-watch: cv[4] = 1 eq 1 OK (frame 250)
assert-watch: cv[5] = 10 eq 10 OK (frame 250)
assert-watch: cv[6] = 220 eq 220 OK (frame 250)
assert-watch: cv[7] = 0 eq 0 OK (frame 250)
assert-watch: cv[8] = 0 eq 0 OK (frame 250)
assert-watch: cv[9] = 0 eq 0 OK (frame 250)
assert-watch: cv[10] = 0 eq 0 OK (frame 250)
assert-watch: cv[11] = 6 eq 6 OK (frame 250)
assert-watch: cv[12] = 1 eq 1 OK (frame 250)
assert-watch: cv[14] = 2 eq 2 OK (frame 250)
assert-text: 'FLR 1 HP 10 TORCH 220 SCORE 0' found at (4, 116) (frame 250)
assert-watch: cv[8] = 1 eq 1 OK (frame 320)
assert-watch: cv[14] = 1 eq 1 OK (frame 320)
assert-watch: cv[6] = 216 eq 216 OK (frame 320)
assert-watch: cv[8] = 2 eq 2 OK (frame 330)
assert-watch: cv[14] = 0 eq 0 OK (frame 330)
assert-watch: cv[7] = 1 eq 1 OK (frame 340)
assert-watch: cv[6] = 238 eq 238 OK (frame 340)
assert-watch: cv[7] = 3 eq 3 OK (frame 362)
assert-watch: cv[6] = 284 eq 284 OK (frame 362)
assert-watch: cv[4] = 2 eq 2 OK (frame 380)
assert-watch: cv[9] = 195 eq 195 OK (frame 380)
assert-watch: cv[10] = 14 eq 14 OK (frame 380)
assert-watch: cv[4] = 2 eq 2 OK (frame 400)
assert-watch: cv[6] = 303 eq 303 OK (frame 400)
assert-watch: cv[9] = 220 eq 220 OK (frame 400)
assert-text: 'FLR 2 HP 10 TORCH 303 SCORE 220' found at (4, 116) (frame 400)
assert-watch: cv[4] = 3 eq 3 OK (frame 494)
assert-watch: cv[9] = 390 eq 390 OK (frame 494)
assert-watch: cv[10] = 33 eq 33 OK (frame 494)
assert-watch: cv[7] = 6 eq 6 OK (frame 494)
assert-watch: cv[8] = 4 eq 4 OK (frame 494)
assert-watch: cv[4] = 4 eq 4 OK (frame 620)
assert-watch: cv[5] = 8 eq 8 OK (frame 620)
assert-watch: cv[9] = 595 eq 595 OK (frame 620)
assert-watch: cv[10] = 54 eq 54 OK (frame 620)
assert-watch: cv[4] = 5 eq 5 OK (frame 728)
assert-watch: cv[5] = 5 eq 5 OK (frame 728)
assert-watch: cv[9] = 800 eq 800 OK (frame 728)
assert-watch: cv[10] = 72 eq 72 OK (frame 728)
assert-watch: cv[14] = 4 eq 4 OK (frame 728)
assert-watch: cv[5] = 2 eq 2 OK (frame 790)
assert-watch: cv[14] = 1 eq 1 OK (frame 790)
assert-watch: cv[2] = 2 eq 2 OK (frame 860)
assert-watch: cv[4] = 5 eq 5 OK (frame 860)
assert-watch: cv[5] = 2 eq 2 OK (frame 860)
assert-watch: cv[6] = 503 eq 503 OK (frame 860)
assert-watch: cv[7] = 15 eq 15 OK (frame 860)
assert-watch: cv[8] = 14 eq 14 OK (frame 860)
assert-watch: cv[9] = 1518 eq 1518 OK (frame 860)
assert-watch: cv[10] = 93 eq 93 OK (frame 860)
assert-watch: cv[11] = 1 eq 1 OK (frame 860)
assert-watch: cv[12] = 1 eq 1 OK (frame 860)
assert-watch: cv[13] = 0 eq 0 OK (frame 860)
assert-watch: cv[14] = 0 eq 0 OK (frame 860)
assert-watch: cv[2] = 2 eq 2 OK (frame 990)
assert-watch: cv[9] = 1518 eq 1518 OK (frame 990)
assert-watch: cv[0] = 1128877636 eq 1128877636 OK (frame 990)
assert-watch: cv[1] = 1447119956 eq 1447119956 OK (frame 990)
assert-watch: cv[3] = 3252575981 eq 3252575981 OK (frame 990)
assert-text: 'EMBERS 15  KILLS 14  TORCH 503' found at (16, 66) (frame 990)
assert-text: 'PRESS START' found at (16, 106) (frame 990)
assert-text: 'SCORE 1518' found at (16, 46) (frame 990)
assert-text: 'VAULT REACHED' found at (16, 26) (frame 990)
ran 1000 frames (keys: ...as invoked above...), saved 4 240x160 PNG(s)
$S/cv-title.png: unique colors: 3 · gray variance: 2677.4
$S/cv-floor2.png: unique colors: 3 · gray variance: 3268.6
$S/cv-title.png vs $S/cv-floor2.png: 7093 pixels differ
$S/cv-floor5.png: unique colors: 3 · gray variance: 3291.4
$S/cv-floor2.png vs $S/cv-floor5.png: 1690 pixels differ
$S/cv-win.png: unique colors: 3 · gray variance: 1888.2
$S/cv-floor5.png vs $S/cv-win.png: 6051 pixels differ
PASS: all frames non-blank — ROM booted and rendered · consecutive shots distinct — input changed the scene
```

LOSE-run invocation (same tree):

```
python3 tools/headless-screenshot.py games/cindervault/cindervault.gba "$S/cv-lose.png" \
  --frames 1750 \
  --keys 240-242:START \
  --keys-pattern 300-314:6:2:DOWN --keys 318-320:RIGHT --keys 324-326:DOWN \
  --keys-pattern 330-1616:6:2:A \
  --shot 800:"$S/cv-lose-mid.png" --require-distinct \
  --elf games/cindervault/cindervault.elf --watch cv:cv_telemetry:16 \
  --assert-text "1740:DARKNESS. THE TORCH IS OUT" \
  --assert-text "1740:SCORE 20" --assert-text "1740:PRESS START" \
  --assert-watch 100:cv:0:eq:1128877636 --assert-watch 100:cv:1:eq:1447119956 \
  --assert-watch 100:cv:2:eq:0 --assert-watch 100:cv:3:eq:3252575981 \
  --assert-watch 250:cv:2:eq:1 --assert-watch 250:cv:5:eq:10 \
  --assert-watch 250:cv:6:eq:220 --assert-watch 250:cv:14:eq:2 \
  --assert-watch 330:cv:8:eq:2 --assert-watch 330:cv:14:eq:0 --assert-watch 330:cv:6:eq:214 \
  --assert-watch 800:cv:2:eq:1 --assert-watch 800:cv:5:eq:10 \
  --assert-watch 800:cv:6:eq:136 --assert-watch 800:cv:10:eq:84 \
  --assert-watch 1620:cv:2:eq:3 --assert-watch 1620:cv:13:eq:1 --assert-watch 1620:cv:6:eq:0 \
  --assert-watch 1620:cv:10:eq:220 --assert-watch 1620:cv:5:eq:10 --assert-watch 1620:cv:8:eq:2 \
  --assert-watch 1620:cv:9:eq:20 \
  --assert-watch 1740:cv:2:eq:3 --assert-watch 1740:cv:13:eq:1 --assert-watch 1740:cv:9:eq:20
```

Output (exit 0):

```
watch cv: u32 words 16 at 0x03001d14
assert-watch: cv[0] = 1128877636 eq 1128877636 OK (frame 100)
assert-watch: cv[1] = 1447119956 eq 1447119956 OK (frame 100)
assert-watch: cv[2] = 0 eq 0 OK (frame 100)
assert-watch: cv[3] = 3252575981 eq 3252575981 OK (frame 100)
assert-watch: cv[2] = 1 eq 1 OK (frame 250)
assert-watch: cv[5] = 10 eq 10 OK (frame 250)
assert-watch: cv[6] = 220 eq 220 OK (frame 250)
assert-watch: cv[14] = 2 eq 2 OK (frame 250)
assert-watch: cv[8] = 2 eq 2 OK (frame 330)
assert-watch: cv[14] = 0 eq 0 OK (frame 330)
assert-watch: cv[6] = 214 eq 214 OK (frame 330)
assert-watch: cv[2] = 1 eq 1 OK (frame 800)
assert-watch: cv[5] = 10 eq 10 OK (frame 800)
assert-watch: cv[6] = 136 eq 136 OK (frame 800)
assert-watch: cv[10] = 84 eq 84 OK (frame 800)
assert-watch: cv[2] = 3 eq 3 OK (frame 1620)
assert-watch: cv[13] = 1 eq 1 OK (frame 1620)
assert-watch: cv[6] = 0 eq 0 OK (frame 1620)
assert-watch: cv[10] = 220 eq 220 OK (frame 1620)
assert-watch: cv[5] = 10 eq 10 OK (frame 1620)
assert-watch: cv[8] = 2 eq 2 OK (frame 1620)
assert-watch: cv[9] = 20 eq 20 OK (frame 1620)
assert-watch: cv[2] = 3 eq 3 OK (frame 1740)
assert-watch: cv[13] = 1 eq 1 OK (frame 1740)
assert-watch: cv[9] = 20 eq 20 OK (frame 1740)
assert-text: 'DARKNESS. THE TORCH IS OUT' found at (16, 26) (frame 1740)
assert-text: 'PRESS START' found at (16, 106) (frame 1740)
assert-text: 'SCORE 20' found at (16, 46) (frame 1740)
ran 1750 frames (keys: 240-242:START, 318-320:RIGHT, 324-326:DOWN, 300-314:6:2:DOWN, 330-1616:6:2:A), saved 2 240x160 PNG(s)
$S/cv-lose-mid.png: unique colors: 3 · gray variance: 3059.3
$S/cv-lose.png: unique colors: 3 · gray variance: 1850.3
$S/cv-lose-mid.png vs $S/cv-lose.png: 5979 pixels differ
PASS: all frames non-blank — ROM booted and rendered · consecutive shots distinct — input changed the scene
```

Determinism lines (both runs, verbatim):

```
diff win-proof-1.txt win-proof-2.txt  -> (empty)  WIN PROOF TWICE-RUN IDENTICAL
diff lose-proof-1.txt lose-proof-2.txt -> (empty) LOSE PROOF TWICE-RUN IDENTICAL
d3ad8c78693fec8467273a78c174da722a314a9c03494562a3b042d9b1a9a52e  games/cindervault/cindervault.gba   (clean build 1)
d3ad8c78693fec8467273a78c174da722a314a9c03494562a3b042d9b1a9a52e  games/cindervault/cindervault.gba   (clean build 2)
REBUILD BYTE-IDENTICAL
PROOF HOLDS AT DIST BYTES (identical output on the clean-rebuilt ROM)
```

## 💡 Session idea

Deepcast's "listen-then-script" proof loop got a force multiplier tonight: because Cindervault is turn-based, a **host-side Python mirror of the game core** (PRNG + generation + turn resolution, ~200 lines) could not only propose routes but be verified against the emulator **turn-for-turn from one watch-log CSV** — 313 turns, 0 mismatches, in a single exploratory emulator run each for win and lose. Once the mirror is trusted, route design becomes pure host-side search (the winning route here came from a 20-line Dijkstra bot; the doomed hit-for-1 balance was discovered and fixed BEFORE ever re-running the emulator). The repeatable recipe for any future turn-based title: (1) write the mirror alongside main.cpp, (2) one emulator run to certify the mirror, (3) search routes on the host at Python speed, (4) one final emulator run to transcribe pins. It converts proof-authoring from emulator-iteration-bound to search-bound — and doubles as a design-balance tool for free.

## Previous-session review

- Session 39 (`.sessions/2026-07-13-deepcast.md`, PR #83) pinned 75 watch asserts by reading a seeded timeline out of a `--watch-log` CSV and scripting inputs that play WELL against it — this session reused the loop wholesale and can confirm the claim in its 💡 note: the committed route demonstrably plays the intended strategy (sweep embers, bump biters, take the stairs), not input soup. One place Cindervault had to go beyond it: Deepcast's proof frames are continuous-time (charge/tension curves), so its pins had to be re-tuned per timeline read; a turn-based core lets the whole timeline be predicted by a host mirror and merely CONFIRMED by the emulator, which is why this session's proof needed only one pin-repair cycle (the HUD screen-edge clip) end to end.
- A genuine catch its precedent handed this session for free: Deepcast's card documents `--assert-text` as reading the VARIABLE 8x8 font specifically — which is why Cindervault's map rows (fixed font, unreadable by the harness) were kept off the assert surface and all asserted strings render in the variable font. Without that line in the sibling card, the first proof draft would likely have tried to assert the map glyphs.
- Small divergence worth flagging for the sweep: Deepcast left its claim file in place at close ("Tiltstone precedent"); this session deletes its claim in the close commit per the current control/README.md session-close rule. The two conventions now coexist in the repo's history; the sweep may want to pick one.
