# gba-homebrew (game-lab Track B) · status

updated: 2026-07-11T19:01:38Z (session 18 overwrite, rebased onto the
session-19 merge: the BRINEWARD parallel-arc section below is session
19's record, preserved verbatim)
phase: **GLOAMLINE ARC — session 18 (slice 4): SHOVE + WAVES SHIPPED.**
Gloamline now fights back: download **`dist/gloamline.nds`** (108,032 B,
sha256
`aaa09bb6cfcd3d1941391963ddf2cee8cbee10eeee91bd56971dff12e14be3a6`,
byte-deterministic build) — **`docs/PLAYING-GLOAMLINE.md`** is the guide.
Night N spawns a deterministic WAVE (1, 3, 5, ... +2/night, plateau at
the concept's 24-sprite cap from night 13), trickling in over the
night's first 40 s — spawn timing AND placement are pure functions of
`(seed, night, index)`. The player gets the **SHOVE** (A): the nearest
Shambler within 24 px is knocked 40 px back with a 45-frame stun; any
attempt arms a 90-frame cooldown (HUD `SHV` light) — a pressure valve,
not a weapon. Watch-map shows EVERY zombie; HUD counts the crowd.
evidence (all run green in-container this session): (1) host mirror
`tools/check-gloam.py` (lockstep with `gl_sim.c`, same commit): 23,552
spawns pure/on-fence/safe-radius incl. every scheduled wave index to the
cap; 24-crowd + hash-driven-shove containment over 20k frames; wave
ramp/cap/plateau + spawn-window shape nights 1–64; 3,837 shove cases
deterministic/never-closer/contained (224 wall-free cases push exactly
+40 px). (2) **`tools/gloam-route.py` NEW** (slice-3 scratch promoted per
its guard recipe): full-game mirror sim + lookahead kiting autopilot;
derives survive routes and proves them at every ±6-frame movement skew —
it reproduced slice 3's evidence exactly (22.6 px nominal) and every
emulator value it predicted for the new proofs matched exactly.
(3) **7 pinned headless proofs in CI** (`nds-rom-build`, 77 asserts):
the 4 slice-3 proofs stayed green UNCHANGED (night-1 bit-equivalence —
zero re-pins) + shove (dist 4517→14757 = exactly +40 px, stun, cooldown,
mash blocked, second shove, death still comes), night-2 waves (schedule
3, alive 1→2→3, BOTH nights survived to dawn via
`proof/night2-route-keys.txt`, ≥21.8 px margin at all 13 skews), and the
**24-Shambler frame budget** on a CI-only `make GL_STRESS=1` build:
steady state ≤53 scanlines (mean 51.9) vs the 71-line vblank budget,
lone night-start redraw spike 133 < the 263-line 60 fps ceiling —
honest-method note: DeSmuME scanline model (py-desmume 0.0.9), not
hardware. Telemetry mailbox grew 16→24 words (slots 0-15 untouched).
Screenshots: `games/gloamline-nds/proof/slice4-{shove,waves-yard,
stress-crowd}.png`. GBA gates untouched (Lumen Drift v1.3 +
`dist/lumen-drift.gba` unchanged).
arc plan (owner has green-lit continuous expansion): concept (PR #50) →
toolchain (PR #51) → skeleton (PR #52) → **shove + waves (THIS PR)** →
next slice = **BARRICADES**, then the between-nights scavenge interlude,
lantern-oil light pressure, synthesized audio set, best-nights saves,
watch-map polish (order per `docs/concepts/gloamline-concept.md`).
parallel arc — BRINEWARD (session 19, coordinator-assigned, this
section's writer): a SECOND original NDS game enters the repo at
concept stage — **Brineward**, single-player pirate naval-action
(broadside-duel core, out-and-back run economy, port upgrades, sea
monsters; genre-inspired reframe of browser naval-MMO loops, 100%
original IP). Shipped this slice: `docs/concepts/brineward-concept.md`
(title web-collision-checked clean 2026-07-11: no game/brand named
Brineward; Keelfire + Squallmark also swept clean as runners-up) +
session-19 card. Concept ONLY — no game dir, no toolchain/CI changes,
zero Gloamline or Lumen Drift files touched; session 18
(gloamline-shove-waves) ran live in parallel, session number 19 and
branch `claude/brineward-concept` chosen against its claim. Next
Brineward slice per the concept doc: v0 walking-skeleton scaffold
(sail + one enemy sloop broadside duel) in `games/brineward-nds/` with
its own `nds-brineward-build` CI job. Inbox re-read at HEAD before
this write (main `bc92ad1`): no new orders (001/002 done, 003/004
executed — record in git history).
honest gaps for the next slice: shove whiffs cost the full cooldown
(documented design choice — owner-taste check welcome); wave plateau
nights (13+) have no headless survive proof (routes proven through
night 2; the autopilot can derive deeper routes but crowd-13 survival
may need the shove in-route — the sim supports it, unexplored); perf
evidence is emulator-model, not hardware; no audio; no saves; night
ramp beyond +2/night unexplored for feel.
health: green (`python3 bootstrap.py check --strict` exit 0 at
close-out; host proof + all 7 headless proofs green in-container; GBA
loop untouched).
kit: v1.12.0 · engaged: yes
boot: synced to origin/main `bc92ad1` (PR #52). Coordinator mid-slice
update acknowledged: a sibling session runs a separate PIRATE arc in
this repo (own dirs) — this lane touched only Gloamline/Lumen
Drift/control files and kept the claims convention so the sibling sees
the lane. Inbox re-read at branch time: no new orders.
last-shipped: this session's PR (slice 4 shove + waves — see
phase/evidence).
blockers: none
📊 Model: fable-5 (ORDER 003, family-level self-report from this
session's own harness/environment banner).
review-queue: EMPTY.
lane position (honest): **the arc's game is now a horde game.** Two
shipped playables in dist/ (Lumen Drift GBA scope-complete v1.3;
Gloamline NDS slice 4). Every future slice inherits proven rails:
pure-sim mirror proof, 24-word telemetry watches, gloam-route.py route
derivation, pinned replay proofs, stress-build perf probe, dist
convention. The hourly wake trigger continues between directed slices.
repo-name note (cosmetic, owner's call, carried): repo named
`gba-homebrew`, now also hosts NDS (and a sibling's pirate arc);
nothing depends on a rename.
kit-follow-ups carried (still unclaimed, low priority): `upgrade
--apply-docs` (14 template-improved docs), migrate/retire legacy root
`claims/` (only its README remains), auto-merge enabler wiring decision
(`adopt --wire-enforcement`), upstream the PR #5 born-red-card gate rule
to substrate-kit.

⚑ needs-owner (carried):
- **⚑ owner-click: create the Lumen Drift GitHub Release** (worker seat
  gets 403 on tags/releases). Suggested tag **`lumen-drift-v1.3`**. Exact
  clicks: repo → Releases → "Draft a new release" → "Choose a tag" → type
  `lumen-drift-v1.3` (create on publish, target `main`) → title
  `Lumen Drift v1.3` → attach `dist/lumen-drift.gba` from the merged tree
  (167,996 B; sha256
  `195a86795e57e2fa0059a96782f1ac7a147cbcebc0cb28a96f353e5d9babae94` —
  paste it in the notes) → point the notes at `docs/PLAYING.md` and the
  v1.3 entry in `docs/current-state.md` → Publish. (No Gloamline release
  ask yet: pre-v1; this repo's release convention has only shipped at
  scope-complete — revisit when the arc matures.)
- **⚑ owner-click: merged-branch cleanup** — worker seat gets 403 on
  branch-delete; merged `claude/*` branches (through PR #52 and this PR
  once merged, plus earlier kit-upgrade and wake branches) can be deleted
  in one sweep from the branches page.
- **⚑ graze tuning wants owner hands-on validation** — the
  400-frames-per-graze refund and the 6px shell are reasoned and
  machine-proven (CI asserts the refund and the lane geometry), but
  whether grazing FEELS right — risk-priced, readable, not exploitable —
  needs real hands on a real run. Owner-gated polish on a complete,
  shipped game. (Same invitation applies to Gloamline hand-feel:
  Shambler speed/stagger AND now shove push/stun/cooldown numbers await
  owner taste.)
- ⚑ (**awareness, no click needed**) — the routine model-attribution
  mismatch (ORDER 003 record, session 9) belongs in whatever the
  fleet-manager coordinator compiles for a report to Anthropic, alongside
  pokemon-mod-lab's parallel finding and the `websites` PR #59 precedent.
notes: worker seat (no tag/release/branch-delete perms — queued above).
Walls carried unchanged: api.github.com curl proxy-walled (GitHub MCP is
the merge path); mGBA python core.load_save() segfault (--savefile
bus-copy); devkitPro official infra Cloudflare-403 (mirror+pins for GBA;
N/A for NDS — Wonderful/BlocksDS hosts direct); py-desmume
KEYINPUT-at-reset quirk (keypad_update(0) before first cycle — respected
by every proof this session). dist/ convention: player-facing games ship
committed with provenance (TWO rows live: lumen-drift.gba v1.3,
gloamline.nds slice 4); engine-test skeletons stay CI-artifact-only; CI
sha256-logs every from-source build and prints the committed-dist hash
next to it.
