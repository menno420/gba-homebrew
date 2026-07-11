# gba-homebrew (game-lab Track B) · status

updated: 2026-07-11T18:25:00Z
phase: **GLOAMLINE ARC — session 17 (slice 3): WALKING SKELETON SHIPPED.**
Gloamline is PLAYABLE: download **`dist/gloamline.nds`** (107,008 B, sha256
`6d1ff00ad7a4c5631046d6c944328beba5d0bc102b279c512762a0f999aee2dc`,
byte-deterministic build) and open it in melonDS/DeSmuME —
**`docs/PLAYING-GLOAMLINE.md`** is the one-page guide. The skeleton cut per
the concept doc: top screen = the yard (fence ring, HUD: night / dawn
countdown / seed / nights survived, two original code-authored sprites),
bottom screen = watch-map v0 (live P/Z dots + dawn bar), 8-way move, ONE
Shambler chasing with deterministic hash stagger, contact = death →
cold-hands card → START instant restart, survive the 60 s night → dawn →
next night. Seed = frame latched at START, printed on HUD/cards (any run
reproducible; CI scripts a fixed press frame).
evidence: (1) all rules are pure fixed-point functions
(`games/gloamline-nds/source/gl_sim.c`) mirrored line-for-line by
**`tools/check-gloam.py`** (lockstep rule; green: 2048 spawns
pure/on-fence/safe-radius, 256 idle-player chases converge monotonically
≤258 frames, 20k-frame containment). (2) **telemetry mailbox**
`gl_telemetry[16]` + `tools/nds-headless-check.py` gained the GBA
harness's ELF-symbol watches (`--elf/--watch/--watch-log/--assert-watch`)
— the session-16 "memory-watch unproven on NDS" gap is CLOSED. (3) four
headless proofs pinned in CI (`nds-rom-build`, all run green in-container
this session, 36 numeric asserts): boot+mailbox; 8-way move (exact
positions per input span); chase→contact(dist 17501→2405)→death→instant
restart (fresh seed, deaths persists); **a FULL 60 s night survived to
dawn** (mirror-sim-derived kiting route
`games/gloamline-nds/proof/dawn-route-keys.txt`, ≥22 px margin at ±6-frame
alignments, ~5 s wall headless — no shortened test build needed). Proof
screenshots committed: `games/gloamline-nds/proof/skeleton-{title,yard,
death,dawn}.png`. Touch input deliberately out of scope (concept:
buttons-only playable). GBA gates untouched (Lumen Drift v1.3 +
`dist/lumen-drift.gba` unchanged).
arc plan: slice 1 concept (PR #50) → slice 2 toolchain feasibility
(PR #51) → slice 3 **THIS PR (skeleton SHIPPED)** → next feature slices
per `docs/concepts/gloamline-concept.md`: **shove verb + multi-zombie
waves** (spawn schedule f(seed,night,index) already in gl_sim), night
ramp/plateau, barricades, between-nights scavenge interlude, lantern-oil
light pressure, synthesized audio set, best-nights saves, watch-map
polish.
honest gaps for the next slice: one zombie only (crowd cap 24 planned);
no shove verb yet (pure kiting); no audio; no saves; no game-feel pass on
Shambler speed/stagger (numbers chosen for provability, owner taste
pending); the dawn-route derivation is a scratch script — promote a
route-recorder sibling if slice 4 needs richer routes.
health: green (`python3 bootstrap.py check --strict` exit 0 at close-out;
all four NDS proofs + host proof green in-container; GBA loop untouched)
kit: v1.12.0 · engaged: yes
boot: synced to origin/main `d45b156` (PR #51). No wake landed mid-slice;
session number 17 held. Inbox re-read at HEAD before this write: no new
orders (003/004 executed previously, record in git history).
last-shipped: this session's PR (walking skeleton — see phase/evidence).
blockers: none
📊 Model: fable-5 (ORDER 003, family-level self-report from this
session's own harness/environment banner).
review-queue: EMPTY.
lane position (honest): **the arc has a playable game.** Two shipped
playables in dist/ (Lumen Drift GBA scope-complete v1.3; Gloamline NDS
skeleton). Rails for every future Gloamline slice exist and are proven:
pure-sim mirror proof, telemetry watches, pinned replay proofs, dist
convention. The hourly wake trigger continues between directed slices.
repo-name note (cosmetic, owner's call, carried): repo named
`gba-homebrew`, now also hosts the NDS game; nothing depends on a rename.
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
  ask yet: the skeleton is pre-v1 and this repo's release convention has
  only shipped at scope-complete — revisit when the arc matures.)
- **⚑ owner-click: merged-branch cleanup** — worker seat gets 403 on
  branch-delete; merged `claude/*` branches (through PR #51 and this PR
  once merged, plus earlier kit-upgrade and wake branches) can be deleted
  in one sweep from the branches page.
- **⚑ graze tuning wants owner hands-on validation** — the
  400-frames-per-graze refund and the 6px shell are reasoned and
  machine-proven (CI asserts the refund and the lane geometry), but
  whether grazing FEELS right — risk-priced, readable, not exploitable —
  needs real hands on a real run. Owner-gated polish on a complete,
  shipped game. (Same invitation now applies to Gloamline skeleton
  hand-feel: Shambler speed/stagger numbers await owner taste.)
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
by every new proof this session). dist/ convention: player-facing games
ship committed with provenance (TWO rows live: lumen-drift.gba v1.3,
gloamline.nds skeleton); engine-test skeletons stay CI-artifact-only; CI
sha256-logs every from-source build and prints the committed-dist hash
next to it.
