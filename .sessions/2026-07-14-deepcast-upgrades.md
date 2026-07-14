# Session — Deepcast growth cut 4: line upgrades bought with score + SRAM bank

> **Status:** `complete`

- date: 2026-07-14 (branch `claude/deepcast-upgrades`, started 06:41Z,
  flipped 07:05Z — both from `date -u` at write time)
- mission: build Deepcast's next named growth cut from
  `games/deepcast/CONCEPT.md` — "Line upgrades bought with score (thicker
  line = higher snap threshold, slower reel) — a run-to-run meta without
  breaking determinism" — as the game's first persistent meta: each run's
  dusk score deposits into a BANK that survives power cycles (the
  `games/common/include/gl_save.h` magic-checked SRAM slot, tag `DCLINE1`
  — the Lumen Drift `LDRIFT1` pattern, and the SRAM follow-up the species
  cut named), and the LINE SHOP (SELECT on the title, R at dusk —
  edge-triggered, glyph text, the existing UI grammar) spends the bank on
  three line tiers with the doc's exact tradeoff: WORN (snap 600, reel
  5/frame — the v0.2 constants), BRAIDED (750/4, cost 15), STEEL (900/3,
  cost 40). Tension gain, slack decay and every RNG stream untouched: a
  run is a pure function of (seed, line tier), and a fresh/erased cart
  loads as no save = tier 0 = v0.4 bit-exact.
- provenance: dispatched on a LIVE OWNER directive — "See if there is
  anything else you can come up with or improve, try to continue with as
  much as you can" — owner live in the coordinator chat ~00:58Z
  2026-07-14, relayed via the game-lab coordinator.
- 📊 Model: Claude (Fable family) — family-level self-report from this
  session's own harness, per ORDER 003.

## What this session did

1. This card + `control/claims/claude-deepcast-upgrades.md`, born red
   (claim before build), cut from main @ `e6b7693` (PR #124 merged).
   PR #125 opened READY.
2. Toolchain certified before touching code: devkitARM (leseratte10
   mirror pin) + Butano present, mgba binding under Python 3.11 imported
   clean; main's unmodified tree rebuilt FROM CLEAN (scratch worktree)
   to the committed `dist/deepcast.gba` v0.4 hash `f87b7975…` EXACTLY.
3. **THE cut** (`games/deepcast/src/main.cpp`): `gl::save_slot<dc_save>`
   restores `{bank, tier}` at boot (no save = 0/0); dusk deposits the
   run's score exactly once (the st_result -> st_over edge) and saves;
   `st_shop` (state 7) opens from the title (SELECT — the title's one
   free key) and from dusk (R), A buys the next tier if the bank covers
   it (edge-triggered; an unaffordable press changes nothing), saves
   immediately. The fight reads `line_snap[tier]` / `line_reel[tier]`
   where v0.4 read the constants 600 / 5; the click-speed audio law
   scales with the current line's snap threshold so the ratchet stays an
   honest tension bar on any line. New `dc_linemeta` 8-word mailbox
   ('LINE' magic, tier, bank, snap, reel rate, shop flag, next cost) +
   a 7th text row (title/dusk: the shop key + live BANK).
4. **Carry proven before any proof changed** (the species card's oracle
   idea, reused deliberately): the whole committed v0.4 P1-P6 suite ran
   green against the new build UNMODIFIED — every pin carried, zero
   re-derived; tier 0 IS the v0.4 math, and no-savefile runs are the
   fresh cart. Then `games/deepcast/proofs.sh` grew +P7, three routes,
   all literals transcribed from observed --watch-log runs: run A the
   worn baseline on a fresh cart (bank 0 / snap 600 / reel 5 pinned,
   hold-only fight snaps at exactly 600 on frame 542, line 287 = 432-5k
   at bite+29); run B boot 1 on a factory-fresh savefile (dusk deposits
   18 the frame dusk falls, R opens the shop glyph-exact, A buys BRAIDED
   for 15 -> bank 3 / snap 750 / reel 4, a second A is REFUSED — STEEL
   costs 40 — nothing moves; RUN TWICE from two factory-fresh savefiles,
   watch-logs AND written savefiles byte-identical); run C boot 2, a
   SEPARATE emulator boot on run B's savefile (the Lumen Drift two-boot
   pattern; --savefile bus-copy path — the load_save() segfault wall
   quoted from PLATFORM-LIMITS.md, not re-probed): bank 3 / tier 1 /
   750 / 4 restored on the title before any input, and run A's EXACT
   fight replayed as the tradeoff witness — same bite (471, line 432),
   same tension trajectory (203@500, 573@540), line 316 = 432-4k vs 287,
   still FIGHTING at 623 on the worn line's snap frame, snap at exactly
   750 on 548. RUN TWICE, savefile untouched (no dusk = no SRAM write).
   407 committed assert flags, 887 green assert lines, exit 0.
5. `dist/deepcast.gba` v0.5 + README row: two clean builds
   byte-identical, 134,392 bytes, sha256 `c8ed6f11600f7cd7f0ee7a8a2858
   973644e6575360debd6346ec24e988634da9`; the full suite re-ran green at
   these exact bytes. CONCEPT.md's line-upgrades line marked **BUILT**
   (lifetime catch log noted as the remaining SRAM follow-up).
6. `python3 bootstrap.py check --strict` pre-flip: red purely on this
   card's in-progress badge (the designed born-red hold; telemetry
   `.substrate/guard-fires.jsonl` committed) + the pre-existing
   `claims-format` advisory on `order-005-scribe.md` (not this lane's
   file, advisory, left alone).
7. Heartbeat appended to `control/status.md` (append-only dispatch
   section, PR number confirmed first). No merge/approve/auto-merge
   calls from this session — the server-side enabler decides on green.
8. Claim `control/claims/claude-deepcast-upgrades.md` retired with this
   flip — the file is DELETED per the claims README rule 4 ("delete
   your own claim file at session close"; the open PR is the live
   claim from here).

## 💡 Session idea

**Prove a balance tradeoff as arithmetic on a shared route, with the
save file as the only treatment.** The concept line names a comparison
("higher snap threshold, slower reel" — implicitly: than before), and
comparisons are where proofs usually go soft: two different routes, two
different fights, and the assert becomes "trust me, this one is
tankier". P7 instead runs ONE input script under two SRAM states — no
savefile (worn) vs the bought line (braided) — so every divergence in
the watch-log is CAUSED by the purchase and nothing else: the bite
frame and tension trajectory are pinned EQUAL (the cut didn't touch
what it promised not to), while the line words differ by exactly the
reel-rate delta (432-5k vs 432-4k) and the fight outlives the old snap
frame by exactly the threshold delta. Generalized: when a feature is a
parameter change, make the input script the control variable and the
persisted state the treatment — the diff of two watch-logs is then the
feature's effect, computable by hand from the constants in the source.

## Previous-session review

- Prior slice: PR #124 (Drift Garden biomes, growth cut 4 — the wallet
  buys the wind, completing that game's named growth path). Its
  discipline transferred directly: the wallet-meta shape (persistent
  currency, spend-gated unlocks, deny-then-buy-then-rebuy proven, the
  default option pinned byte-exact as identity) is exactly this cut's
  shape one platform over — bank for wallet, SRAM for localStorage,
  line tiers for biomes — and its "read the meta ONCE at a fixed edge"
  rule (round start there, cast/fight reads of `meta.tier` here with
  deposits only at the st_over edge) is what kept the determinism
  contract provable. The difference worth recording: web storage is
  free, SRAM needed the two-boot emulator pattern (Lumen Drift's) to
  prove the power cycle, so the persistence half of the proof cost two
  extra runs here where #124's cost one reload assertion.
