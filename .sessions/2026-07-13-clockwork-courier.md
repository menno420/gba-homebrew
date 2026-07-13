# Session 44 — game-lab Track B

> **Status:** `complete`

- date: 2026-07-13 (branch `claude/clockwork-courier`, 19:00Z → 19:18Z)
- mission: **CLOCKWORK COURIER — playable prototype slice** (GBA, the
  breadth-game series pattern of Deepcast PR #83 / Cindervault
  PR #86). Selection per ORDER 005 rule 3's precedence ("your inbox
  ORDER carrying my goals verbatim → superbot fleet-grounding §9
  [out-of-repo, unreadable from this container] → **your track plans
  at HEAD** → your generative rung"): the track plan at HEAD,
  `docs/concepts/session-1-concepts.md` @ `8bac80a`, is the committed
  ORDER-001 concept queue — Lumen Drift (BUILT, first-complete) plus
  two named unbuilt concepts. Clockwork Courier (risk MEDIUM) is the
  queue's lower-risk next item; Shoal (risk MEDIUM-HIGH) stays queued
  behind its own doc-mandated first-session profiling gate. Not an
  invention: concept, hook, scope ladder and risk note are committed
  at HEAD.
- dedup at claim time: no courier anywhere in games/, open PRs, or
  claims; only historical pick-menu mentions in old session cards.
- session number 44 claimed (43 = Brineward slice 9, PR #94, merged
  `8bac80a`).
- **📊 Model:** Claude 5 family (Fable) — family-level self-report
  from this session's own harness banner, per ORDER 003.
- landing posture: PR #96 READY on main; no merge/approve/auto-merge
  calls from this session — the server-side enabler decides on green.

## 💡 Session idea

**Probe your own puzzle for the counter-solve BEFORE pinning it —
the exploit run costs one command and the fix costs one level cell.**
The kinematics probe (walk right + jump beside the closed door)
accidentally LANDED ON the door's one-cell cap and strolled to the
chute — the ghost was optional, the whole hook decorative. The fix
was one map glyph (a second cap cell, chosen so max jump rise 26.6 px
provably cannot clear the row-4 overlap), and proof 2 now pins the
refusal in BOTH modes (grounded clamp AND mid-air drift at the same x
literal). Rule extracted for every puzzle-shaped slice: **the proof
suite must contain the strongest cheat you can script, asserted to
FAIL** — a puzzle proven only along its intended solution is proven
solvable, not proven to be a puzzle. Second find (engine-shaped): a
pose-playback ghost makes the concept's scary bet (exact replay
determinism) TRIVIAL by construction and lets the harness pin the
replay to the same literals as the original walk 301 frames apart —
re-simulation ghosts would have made that a drift surface. Record
poses, not inputs, whenever the ghost only needs to be SEEN and
WEIGHED, not fought.

## Previous-session review

- Session 43 (Brineward slice 9, saves): its guard recipe held where
  it applied — this session invented no Brineward beat (the tree is
  complete, exactly as the recipe ordered) and took the generative
  rung instead; its hermetic-battery harness rule is untouched (this
  is a GBA slice on mGBA — different harness, and note the mGBA tool
  already had per-run --savefile hermeticity, which is where the NDS
  fix's "what every pin silently assumed" phrasing came from).
- Its 💡 (run the suite twice IN SEQUENCE in one directory) was
  applied here in spirit: the delivery proof runs twice back-to-back
  and must produce a byte-identical final frame — the GBA suite's
  cheapest independence+determinism check, now part of the enders.
- One echo of its erratum lesson: every count in this card was read
  off the run output, not from memory, before committing.

## What this session did

1. Claim + born-red card first (`7a914da`), PR #96 opened READY
   immediately after, cut from main @ `8bac80a`.
2. **The game** (`games/clockwork-courier/`, ~500-line single
   main.cpp over the shared `games/common/butano-game.mak`, text-
   glyph presentation from Butano common fonts, no local assets):
   courier platformer (LEFT/RIGHT 1.25 px/f, A jump −2.8125 px/f,
   gravity, AABB tile collision, all 8.8 fixed-point) on one
   handcrafted 16x9 single-screen tower; **the committed concept's
   hook**: R (only on a FULL 300-frame pose tape, one ghost at a
   time) snaps you back 5 seconds and spawns a ghost that replays
   your recorded POSES; the pressure switch counts the ghost's
   weight; the door opens on the switch and NEVER crushes (holds
   while occupied); parcel on a two-jump ledge; chute stamps the
   delivery clock; START restarts. No RNG anywhere; no lose state
   (the tower is patient). `cc_telemetry[16]` mailbox every frame.
3. **Proofs** (mGBA headless, `tools/headless-screenshot.py`, all
   local per the series convention — no CI edits; the required "ROM
   builds" job auto-discovers the Makefile): **4 proofs / 75 asserts
   (69 watch + 6 text)** — (1) boot + mailbox + title text; (2) walk
   speed exact, the closed door refuses at x=23808 grounded AND
   mid-jump (t[5]=9964 apex with x unmoved), door never opens; (3)
   THE REWIND CONTRACT — buffer fills to exactly 300, R snaps the
   player from the switch (6208) to 17088 with the ghost born there,
   the ghost's replay pinned to IDENTICAL literals as the original
   walk 301 frames apart (15488@436, 10688@451), the ghost holds the
   switch/door while the player stands elsewhere, and the 5 seconds
   spend themselves on schedule (ghost off + door shut at 730); (4)
   THE DELIVERY — the full co-op solve (step jump y=10240, parcel
   carried at (3008,6144), door crossed at 28288 with the ghost still
   holding, state 2 at run-frame 479 exactly, win-card text pinned
   verbatim incl. "CLOCK 7s (479 FRAMES)"), instant restart resets
   the run — and the whole solve **run twice back-to-back is
   byte-identical**.
4. **Exploit closed before pinning** (the idea above): the one-cell
   door cap was jumpable; two-cell cap now provably clears nothing.
5. **Ship**: `dist/clockwork-courier.gba` 121,056 B, sha256
   `ed877798…40a8`, byte-deterministic (two clean builds identical);
   CONCEPT.md one-pager (genre, loop, platform, honest sellability
   guess + the deliberate cuts); dist/README row; heartbeat appended
   at the END of control/status.md.

## Guard recipe (for the next breadth/courier session)

- The concept queue at HEAD now holds ONE unbuilt item: **Shoal** —
  and its own doc text gates it: "profile in the FIRST session, pivot
  cheap if the budget says no" (50 boids vs CPU/OAM). Do not skip the
  profiling gate; Lumen Drift's Butano profiler recipe is the tool.
- Courier growth rungs (CONCEPT.md's deliberate cuts, in order):
  ghost-as-platform (one-way top collision), multiple parcels/timed
  chute windows, more levels, audio via the proven generate_audio.py
  pipeline. The pins to protect: proofs 1-4's literals assume the
  level array, walk/jump constants and the 300-frame tape verbatim —
  any tuning re-pins all four (they are one session's cheap re-derive,
  but say so in the PR).
- The door-crush rule (stays open while occupied) is load-bearing for
  proof 4's return leg; a "realistic" crushing door re-times the solve.
- mGBA harness quoting: `--assert-text` values with spaces MUST be
  shell-quoted ('250:PRESS START') — unquoted they parse as extra args.

## Session enders

- 4 headless proofs / 75 asserts green (boot, kinematics+refusal,
  rewind contract, delivery+restart), delivery proof run twice
  back-to-back byte-identical.
- From-source rebuild == committed `dist/clockwork-courier.gba`
  bit-for-bit (sha256 `ed877798cee2b31b2beb4ac6dd2d449886c8f4e196ea1
  2f40fe6b9ca839c40a8`); two clean builds identical.
- No CI workflow edits; `python3 bootstrap.py check --strict` — green
  at the flip.
- Claim `control/claims/clockwork-courier.md` retired with this flip.
