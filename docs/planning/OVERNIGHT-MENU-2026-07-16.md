# OVERNIGHT MENU — 2026-07-16: the veto-ready proposal menu

> **Status:** `owner-guidance` — a wide, deliberately-unfiltered planning
> menu. **Nothing here is built.** Every row is a proposal the owner
> *vetoes by id* (strike what you don't want; the veto is the filter, not a
> pre-filter by the seat). Grounded against main HEAD
> `478bf167892751f907b02d26b59ec07596f602e2` (#152 merge), generated
> `2026-07-16` (`date -u`: Thu Jul 16 21:51:45 UTC 2026). Facts cited
> `file@478bf16` / PR # / concept doc; [current-state.md](../current-state.md)
> wins as the repo moves.

## How to read this

Skim the **summary table**, strike ids you don't want, keep the rest. Ids
are stable and grouped by category prefix so a veto is one token. Effort is
**S** (sub-slice / hours), **M** (1–3 slices), **L** (arc-sized, 4+ slices)
— every number is a precedent transfer from a completed arc of the same
shape, not a guess (the NEXT-MENU honesty rule,
[.sessions/2026-07-15-next-menu.md](../../.sessions/2026-07-15-next-menu.md)).

**Today's unlanded stack** (open DRAFT PRs at HEAD — do NOT touch): #153–#154
docs fixes; **#155–#165 Underroot v1.0** (arc A2, NDS dual-screen colony sim,
11 stacked slices); **#166–#170 Tiltstone arc 2** (5 cuts: share/hints/
curation/fingerprints/monotone ramp). Proposals that build on those are
marked **⧗ depends on today's stack (#155–#170) landing**. This is the
PUBLIC Track B repo — **original IP only, Butano/BlocksDS, never any
Nintendo-derived material** (README.md@478bf16 HARD RAIL).

At HEAD the repo has **11 games**; Underroot is the 12th and lands with the
#155–#165 stack (current-state.md@478bf16 lists eleven; Underroot is not yet
on `main`). Per-game growth cuts for all 12 games are below.

---

## SUMMARY TABLE (veto by id)

| id | title | S/M/L | one-liner |
|---|---|---|---|
| **Per-game — Underroot (NDS colony, ⧗ stack)** ||||
| UND-01 | Multi-year endless winters | M | seasons loop past year 1, winters escalate — Cindervault endless-depth shape |
| UND-02 | Second predator on the schedule | M | a burrowing/night hunter beside the hawk, pure f(seed,season,index) |
| UND-03 | Forager caste / role assignment | M | bottom-screen verb to earmark nurses vs foragers |
| UND-04 | Burrow cross-section art pass | M | diegetic tunnels/chambers replacing the skeleton render |
| UND-05 | Lifetime almanac (EEPROM) | S | seasons-survived / species record, extends the slice-9 save |
| **Per-game — Tiltstone (web puzzle, ⧗ stack)** ||||
| TIL-01 | Daily streak + calendar | S | local streak tracking the concept already names as free |
| TIL-02 | Mobile touch (swipe-to-rotate) | M | the paid-path input the concept names as missing |
| TIL-03 | Handcrafted level-pack expansion | M | more curated packs atop the generator |
| TIL-04 | Static seed-of-the-day board | M | shareable daily results without a backend (arcade lane) |
| TIL-05 | Colorblind gem shapes | S | pattern/shape glyphs so gem kind is non-color-coded |
| **Per-game — Lumen Drift (GBA drifter)** ||||
| LUM-01 | Dialable daily-seed mode | S | endless cave is already deterministic — add the GBA seed dial |
| LUM-02 | Mote + shard art/particle pass | M | sprites/particles over today's palette tricks |
| LUM-03 | Second hazard family | M | a new obstacle beyond crystal spikes + the surge |
| **Per-game — Gloamline (NDS horde)** ||||
| GLO-01 | Sprint/dodge verb | M | the concept's named later-slice movement verb |
| GLO-02 | Lantern toggle verb | S | the second named-later verb; light as a spendable |
| GLO-03 | Scavenge interlude expansion | M | the between-nights loop the concept slots in later |
| GLO-04 | A second zombie type | M | a runner/spitter on the hash-stagger schedule |
| **Per-game — Brineward (NDS naval)** ||||
| BRI-01 | The Grasper (2nd monster) | M | the grapple-and-hold enemy the concept promised, never built |
| BRI-02 | Ram/brace B verb | S | the literally-"reserved" B button gets its verb |
| BRI-03 | Hold-cap economy track | M | the "hold cap starts at 8 crates" progression, unbuilt |
| BRI-04 | NDS chart/save hardening | M | close the concept's flagged save-persistence open question |
| **Per-game — Undertow (web diver)** ||||
| UTW-01 | Static daily leaderboard | M | "the only version with a reason to return" (concept) |
| UTW-02 | Weekly-seed rotation | S | a rotating featured seed on the title screen |
| UTW-03 | A fourth channel hazard | M | a current/whirlpool on a fourth side-band RNG stream |
| **Per-game — Drift Garden (mobile PWA)** ||||
| DGN-01 | Species-unlock cut | M | the wallet-spend the concept names as still unbuilt |
| DGN-02 | TWA/Play packaging prep | M | non-spend Bubblewrap config prep (concept step 2) |
| DGN-03 | Share-your-garden PNG export | S | render the garden to a shareable image |
| DGN-04 | iOS install polish | S | apple-touch meta + Safari add-to-home path |
| **Per-game — Deepcast (GBA fishing)** ||||
| DPC-01 | Lifetime catch log (SRAM) | M | the twice-named follow-up the concept defers |
| DPC-02 | A fifth depth band + species | M | deeper water, new rarity tier |
| DPC-03 | Almanac / bestiary screen | S | a browse view of caught species |
| **Per-game — Cindervault (GBA roguelike)** ||||
| CIN-01 | Third item slot type | M | a ring/scroll beyond lantern/blade — one more decision |
| CIN-02 | Milestone vault-guardian | M | a boss encounter at floor-5/10 vaults |
| CIN-03 | Daily-run streak persistence | S | SRAM streak on the dialed-seed score attack |
| **Per-game — Clockwork Courier (GBA puzzle)** ||||
| CLK-01 | Sprite art pass | M | tiles/sprites over the glyph render (concept sellability) |
| CLK-02 | 10-level campaign | M | the level count the concept names for an itch title |
| CLK-03 | Two-ghost puzzles | L | two rewinds → two cooperating past-selves |
| **Per-game — Shoal (GBA boids)** ||||
| SHO-01 | Sprite art pass (⚠ CPU) | M | tiles over glyph — risk-gated by the 82.35% worst frame |
| SHO-02 | Levels beyond L4 | S | more tuned waters on the shipped knob triples |
| SHO-03 | Tide/current modifier | M | a global drift field as a new difficulty axis |
| **Per-game — Wickroad (GBA trading)** ||||
| WIK-01 | The junction (branch roads) | M | the "honest cut" the concept named — new travel verb |
| WIK-02 | Sprite art pass | M | market-table art over the house glyph style |
| WIK-03 | SRAM best ledger | S | the only GBA title with zero persistence gets a best |
| WIK-04 | Seed dial | S | dialable year seed on the Deepcast/Cindervault precedent |
| **Engine / shared-tech** ||||
| ENG-01 | Shared audio-synth wrapper | M | de-dupe the per-game generate_audio.py pipeline |
| ENG-02 | Shared NDS host-sim harness lib | M | unify check-gloam/check-brine/check-underroot |
| ENG-03 | Unified save-format + migration | M | one versioned header across SRAM + EEPROM |
| ENG-04 | Shared seed-dial widget | S | one 8-hex picker instead of three re-implementations |
| ENG-05 | Package the light-window helper | S | promote gl_light_window.h to a documented shared system |
| ENG-06 | Sim-core convention audit | S | one xorshift32/fixed-point header, audited + doc'd |
| ENG-07 | Telemetry-mailbox schema std | M | a shared layout convention for the per-game mailboxes |
| **Tooling / CI** ||||
| CI-01 | ROM-size budget gate | S | fail a PR that bloats a dist ROM past a budget |
| CI-02 | Screenshot-diff regression | M | golden-PNG diff per game on every PR |
| CI-03 | Auto-discover proofs.sh | S | one gate step that runs every games/*/proofs.sh |
| CI-04 | Reachability orphan reporter | M | stop the recurring substrate-gate orphan reds |
| CI-05 | Path-filter the NDS jobs | S | build an NDS ROM only when its dir changed (CI minutes) |
| CI-06 | Asset-determinism gate | S | run-twice byte identity on generate_assets.py |
| CI-07 | Two-build hash attestation gate | S | promote the manual determinism claim to a check |
| CI-08 | Web-smoke matrix | S | real-Chromium smoke wired for every present+future web game |
| **Web arcade (Pages lane)** ||||
| ARC-01 | Go-live the arcade | S | the one owner Settings click (#149 merged, zero runs) |
| ARC-02 | Per-game landing pages | M | screenshots + controls + play button per game |
| ARC-03 | Static leaderboards | M | high-score board from localStorage + shared seed URLs |
| ARC-04 | Auto screenshot gallery | S | generate galleries from the headless captures |
| ARC-05 | Browser-playable ROMs (wasm) | L | ⚠ first third-party runtime dep — owner ruling first |
| ARC-06 | Unified arcade nav/shell | S | one index across the web games + future titles |
| ARC-07 | Installable arcade PWA | M | wrap the arcade itself as an offline PWA |
| **Release / packaging** ||||
| REL-01 | Lumen Drift v1.3 release | S | ⚑ the tag+Release click; agent stages, owner fires |
| REL-02 | Batch-release concept-complete ROMs | M | Releases for Shoal/Courier/Cindervault/Deepcast/Wickroad |
| REL-03 | Per-game changelog automation | S | changelogs from the PR trails |
| REL-04 | Flashcart / Pocket packaging | M | EverDrive folders + Analogue Pocket manifests |
| REL-05 | Versioning + tag-scheme doc | S | a written per-game semantic-version convention |
| REL-06 | Release-notes generator | S | notes tied to dist/README provenance rows |
| REL-07 | NDS ROM releases | M | Gloamline/Brineward Releases with save caveats |
| **New-game concepts** ||||
| NEW-01 | Tinderhand (GBA card) | M | press-your-luck ember-stoking deck (NEXT-MENU A1) |
| NEW-02 | Starloom (web/PWA) | M | daily constellation-weaving puzzle (NEXT-MENU A3) |
| NEW-03 | Lamplight relay (NDS) | M | dual-screen signal-relay tower-keeper concept |
| NEW-04 | Metronome Vault (GBA) | M | a timing/rhythm game on the maxmod pipeline |
| NEW-05 | Cryptglyph (web daily) | M | a Wordle-shaped deduction daily for the arcade |
| NEW-06 | Ledgerfall (GBA tactics) | M | a tiny turn-based tactics micro-game |
| NEW-07 | Tidepool (mobile PWA) | M | a second title on the mobile-foundation shell |
| NEW-08 | Glyphwright (NDS stylus) | M | a stylus drawing/tracing puzzle, touch-native |
| **Cross-game systems** ||||
| XG-01 | Compilation / launcher cart | L | a menu ROM that boots the GBA titles — the "bundle cart" |
| XG-02 | Cross-game achievements | M | a shared meta-score/achievement layer |
| XG-03 | Shared music bank / audio identity | M | a house sound palette across titles |
| XG-04 | Web arcade passport | M | one localStorage profile spanning the web games |
| XG-05 | Shared HUD / house-glyph kit | S | a common HUD/theme component |
| XG-06 | Unified score-attack format | S | one on-cart score record all GBA titles emit |

**Proposal count: 84** (see the per-category tally at the foot of this doc.)

---

## a. Per-game growth cuts

### Underroot — NDS dual-screen burrow-colony survival (⧗ arrives with #155–#165)
Underroot is the 12th game, landing at **v1.0** with today's stack: a
dual-screen colony sim where the top screen is the meadow (food, hawk
shadows on a `f(seed, season, index)` schedule) and the bottom screen is
the stylus-dug burrow; v1.0 = winter survival + score + EEPROM best + seed
dial + audio (PRs #155–#165 titles; concept A2 in
[docs/NEXT-MENU-2026-07-15.md](../NEXT-MENU-2026-07-15.md)). Every cut below
is **⧗ depends on today's stack landing** — none can start until #165 is on
`main`.

- **UND-01 · Multi-year endless winters** — after year 1, loop the seasons
  with escalating winter severity so v1.0's survival score becomes the
  *first* milestone, not the end. Pitch: the Cindervault endless-depth shape
  (games/cindervault/CONCEPT.md@478bf16 growth cut 3) transplanted to the
  season clock. **Effort M.** Risk low/reversible (new mode, boot run
  bit-identical). Unblocks a score-attack ladder. **⧗ #155–#170.**
- **UND-02 · Second predator on the schedule** — a burrowing or night hunter
  beside the hawk, spawned from the same pure `f(seed, season, index)`
  schedule (no runtime RNG). **Effort M.** Risk medium (touches the sim);
  reversible. Unblocks predator variety + harder late seasons. **⧗.**
- **UND-03 · Forager caste / role assignment** — a bottom-screen verb to
  earmark burrow occupants as nurses vs foragers, changing food-vs-population
  balance. **Effort M.** Risk medium; new verb, gated behind a menu. Unblocks
  a strategy layer. **⧗.**
- **UND-04 · Burrow cross-section art pass** — diegetic tunnel/chamber tiles
  and colony sprites over the skeleton render (the Cindervault/Deepcast
  art-pass method, presentation-only, all sim pins carried). **Effort M.**
  Risk low (render-only). Unblocks owner-facing polish + a release. **⧗.**
- **UND-05 · Lifetime almanac (EEPROM)** — extend the slice-9 EEPROM save
  with a persistent record (furthest season, species seen). **Effort S.**
  Risk low; additive save-version bump. Unblocks a meta-progression hook.
  **⧗.**

### Tiltstone — web turn-based gravity puzzle (⧗ arc 2 = #166–#170)
Tiltstone arc 2 lands the five deduped cuts (share-your-line, solver hints,
undo×par curation, mechanic fingerprints, monotone ramp; PRs #166–#170,
concept B1). Post-arc-2 the daily/social hook is live; these extend it.

- **TIL-01 · Daily streak + calendar** — local streak tracking + a
  month-calendar of solved days; the concept names it as available today
  without a backend (games/web-tiltstone/CONCEPT.md@478bf16 §Growth path 6).
  **Effort S.** Risk low (localStorage, render-only). Unblocks retention.
  **⧗ #166–#170.**
- **TIL-02 · Mobile touch (swipe-to-rotate)** — the concept lists mobile
  touch as a required paid-path input (CONCEPT.md §Sellability). **Effort M.**
  Risk low; additive input path. Unblocks the mobile audience. **⧗.**
- **TIL-03 · Handcrafted level-pack expansion** — more curated packs beyond
  GRANITE GAUNTLET / DEEP CUTS, on the shipped `curatePack` machinery.
  **Effort M.** Risk low. Unblocks content depth. **⧗.**
- **TIL-04 · Static seed-of-the-day board** — a shareable results board with
  no backend (hashes of daily results in the URL / arcade lane). **Effort M.**
  Risk low. Unblocks the social loop the concept calls the game's real value.
  Cross-links **ARC-03**. **⧗.**
- **TIL-05 · Colorblind gem shapes** — per-kind shape/pattern glyphs so gem
  identity is not color-only. **Effort S.** Risk low. Unblocks accessibility.
  **⧗.**

### Lumen Drift — GBA one-button gravity drifter (v1.3, scope-complete)
Scope-complete + 3 polish passes; polish list "EXHAUSTED"
(docs/concepts/session-1-concepts.md@478bf16 §1). Further work is
new-concept material.

- **LUM-01 · Dialable daily-seed mode** — the endless cave is already a pure
  function of the world row; add a GBA seed dial (the Deepcast/Cindervault
  8-hex picker) for "same cave, same run" score attack. **Effort S.** Risk
  low; boot run unchanged. Unblocks a shareable challenge + a reason to
  return.
- **LUM-02 · Mote + shard art/particle pass** — sprites/particles over the
  current palette-trick light effect. **Effort M.** Risk low (render-only).
  Unblocks a stronger release/demo.
- **LUM-03 · Second hazard family** — a new obstacle class beyond crystal
  spikes + the descending surge, banded into the echo cave. **Effort M.**
  Risk medium (sim); reversible per-band. Unblocks depth variety.

### Gloamline — NDS zombie horde-defense (concept tree complete, 28 proofs)
Concept tree COMPLETE + best-night rematch. The concept itself names two
deferred verbs and an interlude (docs/concepts/gloamline-concept.md@478bf16).

- **GLO-01 · Sprint/dodge verb** — "Sprint/dodge … : later slices" (concept
  §Player verbs). **Effort M.** Risk medium; new verb + spacing rebalance.
  Unblocks a higher skill ceiling.
- **GLO-02 · Lantern toggle verb** — the second named-later verb; light as a
  spendable resource (the concept's oil-pressure idiom). **Effort S/M.** Risk
  medium. Unblocks a risk/reward light economy.
- **GLO-03 · Scavenge interlude expansion** — the between-nights scavenge
  loop the concept slots in later without reworking the core. **Effort M.**
  Risk medium. Unblocks pacing variety.
- **GLO-04 · A second zombie type** — a runner or spitter on the deterministic
  hash-stagger schedule (no runtime RNG). **Effort M.** Risk medium.
  Unblocks wave variety.

### Brineward — NDS pirate broadside duel (arc slice 9)
The concept promised a monster *class* and shipped one member; a reserved B
verb and a hold-cap track were designed but never built
(docs/concepts/brineward-concept.md@478bf16; NEXT-MENU B3).

- **BRI-01 · The Grasper (2nd monster)** — "arms that grapple and hold you
  still while cutters close in" — designed, only the Maw exists in `bw_sim.c`.
  **Effort M.** Risk medium (new enemy AI). Unblocks the monster-class the
  concept sells. (NEXT-MENU **B3**.)
- **BRI-02 · Ram/brace B verb** — the B button is literally "(reserved: later
  ram/brace verb)" (concept §Controls sketch). **Effort S.** Risk low; the
  input-verb-gate rule keeps it zero-re-pin. Unblocks a defensive option.
- **BRI-03 · Hold-cap economy track** — "Hold cap starts at 8 crates" was
  never built as a progression (concept §Progression). **Effort M.** Risk
  medium. Unblocks the greed/logistics curve.
- **BRI-04 · NDS chart/save hardening** — close the concept's flagged
  open question: NDS save persistence unproven, chart persistence pending
  (concept §Honest open questions). **Effort M.** Risk medium; needs a
  headless save-flash feasibility probe first. Unblocks durable progression.

### Undertow — web endless diver (v1.5, growth complete)
Named growth path COMPLETE (games/web-undertow/CONCEPT.md@478bf16). The
concept names determinism-backed return-play as the one differentiator worth
building.

- **UTW-01 · Static daily leaderboard** — "the only version of this game with
  a reason to return" (concept §Sellability); shareable seed URLs already
  exist. **Effort M.** Risk low (static). Unblocks retention. Cross-links
  **ARC-03**.
- **UTW-02 · Weekly-seed rotation** — a rotating featured seed on the title
  screen. **Effort S.** Risk low. Unblocks a lightweight event cadence.
- **UTW-03 · A fourth channel hazard** — a current/whirlpool on a fourth
  side-band RNG stream (the jellyfish precedent keeps wall+pocket layout
  byte-identical). **Effort M.** Risk medium (sim); ghost records re-version
  cleanly. Unblocks variety.

### Drift Garden — mobile ambient PWA (v1.4, growth complete)
Named growth path complete; the concept explicitly leaves species-unlocks
and store-packaging as bolt-ons (games/mobile-foundation/CONCEPT.md@478bf16).

- **DGN-01 · Species-unlock cut** — "species unlocks would ride the same
  wallet" (concept §Core loop) — named, unbuilt. **Effort M.** Risk low
  (wallet-spend, render-side). Unblocks collection depth.
- **DGN-02 · TWA/Play packaging prep** — the non-spend half of concept
  packaging step 2 (Bubblewrap/`assetlinks.json` config prep; the AAB build
  is walled, PLATFORM-LIMITS). **Effort M.** Risk low; no spend, no account.
  Unblocks a store path (owner does the spend later). Cross-links **REL**.
- **DGN-03 · Share-your-garden PNG export** — render the garden to a
  shareable image. **Effort S.** Risk low. Unblocks organic sharing.
- **DGN-04 · iOS install polish** — apple-touch meta + Safari add-to-home
  path (concept packaging step 4). **Effort S.** Risk low. Unblocks iOS
  installs.

### Deepcast — GBA fishing tension (v0.6, growth complete)
Growth path COMPLETE; the concept twice defers a lifetime catch log
(games/deepcast/CONCEPT.md@478bf16).

- **DPC-01 · Lifetime catch log (SRAM)** — the twice-named follow-up (the
  session log is session-scope by design; a lifetime log needs its own
  save-version slice). **Effort M.** Risk low; additive on the existing
  `DCLINE1` SRAM bank. Unblocks a collection meta. (NEXT-MENU runner-up.)
- **DPC-02 · A fifth depth band + species** — deeper than THE ABYSS with a
  new rarity tier on the side-band species stream. **Effort M.** Risk low
  (side-band, main stream untouched). Unblocks content depth.
- **DPC-03 · Almanac / bestiary screen** — a browse view of caught species.
  **Effort S.** Risk low. Unblocks a reason to chase rares.

### Cindervault — GBA turn-based roguelike (v0.6, growth complete)
Growth path COMPLETE (games/cindervault/CONCEPT.md@478bf16). The item and
species systems are plug-in shaped.

- **CIN-01 · Third item slot type** — a ring/scroll beyond lantern/blade —
  "one more decision, zero new verbs" in the concept's own words. **Effort
  M.** Risk low (the single-slot swap already exists). Unblocks build variety.
- **CIN-02 · Milestone vault-guardian** — a boss encounter at the floor-5/10
  vaults. **Effort M.** Risk medium (new monster behavior). Unblocks a
  climax beat.
- **CIN-03 · Daily-run streak persistence** — an SRAM streak on the dialed-
  seed score attack. **Effort S.** Risk low. Unblocks return-play.

### Clockwork Courier — GBA rewind-ghost puzzle (v1.0, concept complete)
CONCEPT COMPLETE; the concept names sprite-art + 10 levels as the itch-title
cut (games/clockwork-courier/CONCEPT.md@478bf16 §Sellability).

- **CLK-01 · Sprite art pass** — tiles/sprites over the glyph render.
  **Effort M.** Risk low (render-only; no RNG anywhere). Unblocks a saleable
  build.
- **CLK-02 · 10-level campaign** — beyond the current three tower floors,
  the level count the concept names. **Effort M.** Risk low (handcrafted
  maps, replay determinism by construction). Unblocks content depth.
- **CLK-03 · Two-ghost puzzles** — two rewinds spawn two cooperating past-
  selves. **Effort L.** Risk medium (IWRAM budget for two pose rings — size
  early). Unblocks a deeper puzzle language.

### Shoal — GBA boids herding (v1.0, concept complete)
CONCEPT COMPLETE at 82.35% worst frame (games/shoal/CONCEPT.md@478bf16) —
CPU headroom is thin, so art is deliberately risk-flagged.

- **SHO-01 · Sprite art pass** — tiles over glyph. **Effort M.** **Risk HIGH
  / gated:** the four-level chain's worst frame is 82.35% of one frame
  (3371/4096); any art must respect the `t[5] < 4096` rail (NEXT-MENU calls
  Shoal art "risky"). Reversible if the budget says no. Unblocks a
  demo-quality build.
- **SHO-02 · Levels beyond L4** — more tuned waters on the shipped
  knob-triple discipline. **Effort S.** Risk low (data-only). Unblocks
  content.
- **SHO-03 · Tide/current modifier** — a global drift field as a new
  difficulty axis. **Effort M.** Risk medium (sim + budget). Unblocks a
  fresh mechanic.

### Wickroad — GBA market-route trading (v0.6, growth complete)
Growth path COMPLETE; the concept names "the junction" as the honest cut it
left, and it is the only GBA title with zero SRAM persistence
(games/wickroad/CONCEPT.md@478bf16; NEXT-MENU B2).

- **WIK-01 · The junction (branch roads)** — branch the one road with a new
  travel verb at the wider-map cut's named RNG freeze point. **Effort M.**
  Risk medium (new verb grammar). Unblocks the concept's biggest named idea.
  (NEXT-MENU **B2**.)
- **WIK-02 · Sprite art pass** — market-table art over the house glyph style.
  **Effort M.** Risk low (render-only). Unblocks a saleable build.
- **WIK-03 · SRAM best ledger** — a persistent best (days/gold) via
  `gl_save.h`; Wickroad is the only GBA title with zero persistence.
  **Effort S.** Risk low. Unblocks score attack.
- **WIK-04 · Seed dial** — a dialable year seed on the Deepcast/Cindervault
  precedent. **Effort S.** Risk low. Unblocks "same road, same prices"
  sharing.

## b. Engine / shared-tech

- **ENG-01 · Shared audio-synth wrapper** — every game reimplements a
  `generate_audio.py` sibling (Lumen, Deepcast, Shoal, Courier, Wickroad,
  Gloamline …). Extract one deterministic stdlib-only synth library +
  maxmod-hook counter convention. **Effort M.** Risk low (build-time tooling;
  ROMs re-verified). Unblocks faster audio cuts on new games.
- **ENG-02 · Shared NDS host-sim harness lib** — `check-gloam.py`,
  `check-brine.py` (and a future `check-underroot.py`) share the lockstep
  host-mirror structure. Factor the common driver. **Effort M.** Risk low.
  Unblocks cheaper NDS proofs.
- **ENG-03 · Unified save-format + migration** — one versioned header across
  GBA SRAM (`gl_save.h`) and NDS EEPROM, with a migration story (several
  games bumped save versions ad hoc). **Effort M.** Risk medium (touches
  persistence). Unblocks XG-02 and safe save evolution.
- **ENG-04 · Shared seed-dial widget** — Deepcast, Cindervault (and any GBA
  daily cut) each re-implement the 8-hex UP/DOWN/L/R picker. Package one.
  **Effort S.** Risk low. Unblocks LUM-01/WIK-04/CIN-03 cheaply.
- **ENG-05 · Package the light-window helper** — `games/common/gl_light_
  window.h` is shared by Lumen + Cindervault; document it as a first-class
  shared system with proofs. **Effort S.** Risk low. Unblocks reuse.
- **ENG-06 · Sim-core convention audit** — one audited xorshift32 +
  fixed-point header, with a short "how our determinism works" doc. **Effort
  S.** Risk low (docs + light refactor). Unblocks onboarding + fewer subtle
  RNG-order bugs.
- **ENG-07 · Telemetry-mailbox schema standard** — each game invents its own
  `*_telemetry[N]` layout; a shared schema convention (word 0 = state, …)
  makes the harness generic. **Effort M.** Risk low. Unblocks CI-02/CI-03.

## c. Tooling / CI

- **CI-01 · ROM-size budget gate** — dist ROMs run 118–168 KB
  (dist/@478bf16); fail a PR that bloats one past a per-game budget. **Effort
  S.** Risk low. Unblocks bloat safety.
- **CI-02 · Screenshot-diff regression** — a golden-PNG per game diffed on
  each PR (the headless harness already emits PNGs). **Effort M.** Risk
  medium (flaky-pixel tuning). Unblocks visual regressions caught pre-merge.
- **CI-03 · Auto-discover proofs.sh** — one gate step that runs every
  `games/*/proofs.sh` instead of per-game wiring (five games already commit
  one). **Effort S.** Risk low. Unblocks zero-config proofs on new games.
- **CI-04 · Reachability orphan reporter** — the substrate-gate keeps going
  red on inherited doc orphans (#151 → #153 fix; see today's #153). A clear
  orphan report + suggested read-path fix. **Effort M.** Risk low. Unblocks
  fewer spurious gate reds.
- **CI-05 · Path-filter the NDS jobs** — build an NDS ROM only when its game
  dir changed; the Brineward concept flags "does a second live arc strain CI
  minutes?" as an open question. **Effort S.** Risk low. Unblocks CI-minute
  savings.
- **CI-06 · Asset-determinism gate** — run `generate_assets.py` twice and
  assert byte identity (the art-pass method claims determinism per game).
  **Effort S.** Risk low. Unblocks trustable art regen.
- **CI-07 · Two-build hash attestation gate** — the "two clean builds,
  identical hash" claim in dist/README is stated per row but not gated;
  promote it to a check. **Effort S.** Risk low. Unblocks provable
  determinism.
- **CI-08 · Web-smoke matrix** — the real-Chromium smokes
  (`web-smoke-undertow.mjs`, `web-smoke-tiltstone.mjs`) are per-game; a
  matrix that auto-includes every present + future web game. **Effort S.**
  Risk low. Unblocks zero-config web proofs.

## d. Web arcade (the Pages lane)

The Pages deploy workflow (`deploy-pages.yml`) merged as #149 but has **zero
runs** — go-live is one owner Settings click (control/status.md@478bf16 ⚑).

- **ARC-01 · Go-live the arcade** — surface the exact owner action (Settings
  → Pages → Source "GitHub Actions", then run "Deploy web arcade to Pages").
  **Effort S.** ⚑ **owner-gated** (a click, not agent work). Unblocks a
  public URL for every web title + Tiltstone/Undertow daily hooks.
- **ARC-02 · Per-game landing pages** — a page per game with screenshots,
  controls, and a play/download button. **Effort M.** Risk low. Unblocks
  discoverability.
- **ARC-03 · Static leaderboards** — a high-score board from localStorage +
  the shareable seed URLs the web games already emit (no backend). **Effort
  M.** Risk low. Unblocks TIL-04/UTW-01. 
- **ARC-04 · Auto screenshot gallery** — generate galleries from the headless
  capture PNGs the harness already produces. **Effort S.** Risk low.
  Unblocks marketing surface.
- **ARC-05 · Browser-playable ROMs (wasm)** — run the GBA/NDS ROMs in-page
  via a third-party wasm emulator. **Effort L.** ⚠ **first third-party
  runtime dependency — owner ruling required first** (NEXT-MENU C R5). Risk
  high/reversible. Unblocks "play the cartridges in a browser".
- **ARC-06 · Unified arcade nav/shell** — one index/nav spanning the web
  games + future titles. **Effort S.** Risk low. Unblocks a coherent arcade.
- **ARC-07 · Installable arcade PWA** — wrap the arcade shell itself as an
  offline-installable PWA (the Drift Garden shell precedent). **Effort M.**
  Risk low. Unblocks an installable arcade.

## e. Release / packaging chains

Zero tags/Releases exist yet; the worker seat 403s on direct tag pushes and
uses the `workflow_dispatch` release path (docs/CAPABILITIES.md@478bf16).

- **REL-01 · Lumen Drift v1.3 release** — the release-ready title
  (scope-complete, committed dist ROM + provenance). Agent stages notes; the
  tag+Release is ⚑ the owner click (or the dispatch path). **Effort S.**
  ⚑ owner-gated. Unblocks the first public Release. (NEXT-MENU C R3.)
- **REL-02 · Batch-release concept-complete ROMs** — Releases for Shoal,
  Clockwork Courier, Cindervault, Deepcast, Wickroad (all committed in dist/
  with sha256 provenance). **Effort M.** ⚑ owner-gated clicks. Unblocks a
  distributable catalog.
- **REL-03 · Per-game changelog automation** — generate changelogs from the
  per-game PR trails. **Effort S.** Risk low. Unblocks release hygiene.
- **REL-04 · Flashcart / Pocket packaging** — EverDrive folder layout +
  Analogue Pocket `.json` manifests for the GBA ROMs (original IP, publish-
  safe). **Effort M.** Risk low. Unblocks real-hardware play.
- **REL-05 · Versioning + tag-scheme doc** — a written per-game semantic
  version + tag convention (versions today live only in prose). **Effort S.**
  Risk low. Unblocks consistent releases.
- **REL-06 · Release-notes generator** — notes tied to the dist/README
  provenance rows + sha256s. **Effort S.** Risk low. Unblocks one-command
  notes.
- **REL-07 · NDS ROM releases** — Gloamline + Brineward Releases, with the
  save-persistence caveats stated. **Effort M.** ⚑ owner-gated. Unblocks NDS
  distribution.

## f. New-game concepts (original IP)

Planning-only concept + hook; none built. Two carry over from the standing
menu (NEXT-MENU A1/A3 — A2 Underroot is being built tonight).

- **NEW-01 · Tinderhand (GBA)** — press-your-luck ember-stoking card game:
  BANK or STOKE after every flip, buy deck-thinning charms, survive ten cold
  nights; deck order is a pure `card_of(seed, night, index)` (zero runtime
  RNG). Hook: the whole game is one nerve — quit while you're warm. No card
  game on the roster. **Effort M** (~6 slices, Shoal/Courier precedent).
  (NEXT-MENU **A1**.)
- **NEW-02 · Starloom (web/PWA)** — daily constellation-weave: thread one
  continuous line through a seeded star field within a thread budget while
  drift currents tug it; provably-solvable seeds, share URL, ghost of
  yesterday. Hook: a one-minute daily with a shareable line. **Effort M**
  (~6 slices, Undertow/Tiltstone precedent). (NEXT-MENU **A3**.)
- **NEW-03 · Lamplight Relay (NDS)** — a dual-screen tower-keeper: top screen
  is the storm-lashed coast (ships on a `f(seed, night, index)` schedule),
  bottom screen is the lamp-room where you aim/fuel the beam; guide ships
  home before the oil runs out. Hook: dual-screen split of danger and
  control (the Gloamline/Underroot idiom). **Effort M** (~11 slices, NDS
  precedent).
- **NEW-04 · Metronome Vault (GBA)** — a timing/rhythm dungeon where every
  action fires on the beat of the maxmod music track; miss the beat and the
  vault reacts. Hook: rhythm as the whole verb, on a proven audio pipeline.
  **Effort M.**
- **NEW-05 · Cryptglyph (web daily)** — a Wordle-shaped deduction daily: crack
  a seeded glyph-cipher in six guesses; deterministic daily seed, shareable
  emoji-grid result. Hook: the arcade's own daily social game. **Effort M.**
- **NEW-06 · Ledgerfall (GBA)** — a tiny turn-based tactics micro-game on a
  small grid (3–4 units, deterministic AI), score = turns. Hook: bite-size
  tactics, fully replay-checkable. **Effort M.**
- **NEW-07 · Tidepool (mobile PWA)** — a second title on the mobile-foundation
  shell: a gentle tide-timing collection game (portrait, offline-first).
  Hook: proves the mobile foundation carries a second game. **Effort M.**
- **NEW-08 · Glyphwright (NDS stylus)** — a stylus tracing/drawing puzzle
  where you draw sigils to solve rooms; touch-native, buttons for undo. Hook:
  the first genuinely stylus-first title (all others are buttons-first).
  **Effort M** (save-flash + touch scripting probe first).

## g. Cross-game systems

- **XG-01 · Compilation / launcher cart** — a GBA menu ROM that boots into
  each GBA title (multiboot or a bundled compilation cart) — named across
  several concepts' sellability as "one game in a bundled original-homebrew
  compilation cart" (Deepcast/Cindervault/Shoal CONCEPTs). **Effort L.** Risk
  medium (multiboot mechanics — probe early). Unblocks a single distributable
  artifact for the whole GBA catalog.
- **XG-02 · Cross-game achievements** — a shared meta-score/achievement layer
  riding the unified save format (ENG-03). **Effort M.** Risk medium.
  Unblocks a portfolio-wide meta.
- **XG-03 · Shared music bank / audio identity** — a house sound palette
  (a shared motif set) across titles, on the ENG-01 synth wrapper. **Effort
  M.** Risk low. Unblocks a recognizable lane identity.
- **XG-04 · Web arcade passport** — one localStorage profile spanning
  Undertow/Tiltstone/Drift Garden (and future web titles): shared streaks,
  cosmetics, seeds. **Effort M.** Risk low. Unblocks cross-title retention.
- **XG-05 · Shared HUD / house-glyph kit** — the common font/HUD style
  (`games/common/`) promoted to a documented shared component. **Effort S.**
  Risk low. Unblocks consistent presentation + faster new games.
- **XG-06 · Unified score-attack format** — one on-cartridge score record
  (seed + score + game id) all GBA titles emit, so an off-cart leaderboard is
  honest across games (the "seed makes an off-cart leaderboard honest"
  precedent, Cindervault/Deepcast). **Effort S.** Risk low. Unblocks a
  cross-game leaderboard.

---

## Category tally

| category | prefix | count |
|---|---|---|
| Per-game — Underroot | UND | 5 |
| Per-game — Tiltstone | TIL | 5 |
| Per-game — Lumen Drift | LUM | 3 |
| Per-game — Gloamline | GLO | 4 |
| Per-game — Brineward | BRI | 4 |
| Per-game — Undertow | UTW | 3 |
| Per-game — Drift Garden | DGN | 4 |
| Per-game — Deepcast | DPC | 3 |
| Per-game — Cindervault | CIN | 3 |
| Per-game — Clockwork Courier | CLK | 3 |
| Per-game — Shoal | SHO | 3 |
| Per-game — Wickroad | WIK | 4 |
| Engine / shared-tech | ENG | 7 |
| Tooling / CI | CI | 8 |
| Web arcade | ARC | 7 |
| Release / packaging | REL | 7 |
| New-game concepts | NEW | 8 |
| Cross-game systems | XG | 6 |
| **Total** | | **84** |

Per-game subtotal: **44** across all 12 games. Nothing above is built; the
owner vetoes by id and the seat takes the survivors as a future dispatch.
