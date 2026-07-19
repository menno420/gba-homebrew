# Session — gba menu-vs-tree truth audit + annotations

> **Status:** `in-progress`

- date: 2026-07-19 (branch `claude/gba-menu-truth-audit-0719`, main synced @
  `41a9a9f`; all timestamps from `date -u`, session opened 20:29Z)
- mission: **audit slice, not execution** — the gba execution backlog is dry
  (every remaining arc gate is owner-side), so per the owner's universal
  continue directive (2026-07-19: "when execution is dry, PLAN") this session
  runs a full MENU-vs-TREE truth audit of `docs/NEXT-MENU-2026-07-15.md` — every
  option A1/A2/A3/B1/B2/B3/C verified against the actual tree at HEAD and given a
  BUILT / PARTIAL / UNBUILT verdict with a one-line citation — then ANNOTATES the
  existing menu in place (owner standing rule: no NEW menu), mirrors the
  corrected picture into `docs/eap-closeout-gba-2026-07-19.md`, and ends both
  with the honest REMAINING REAL CHOICES line (which letters are still genuine
  open choices vs already done/moot). The key deliverable is that line.
- **📊 Model:** Claude Opus · high · audit/planning — menu-vs-tree truth audit
  (no gameplay code touched)
- landing posture: READY (non-draft). Plain born-red `claude/*` card PR — the
  session card holds the PR red as `in-progress` until this audit flip (the LAST
  commit), then it lands on green `ROM builds` via the existing
  `auto-merge-enabler.yml`. Docs + control + card only, no workflow file.

## What shipped

1. **Born-red gate** (this card `in-progress` + the work claim
   `control/claims/claude-gba-menu-truth-audit-0719.md`), filed before the audit
   annotations so the PR is red until the close-out flip.
2. **`docs/NEXT-MENU-2026-07-15.md`** — an inline `▶ AUDIT 2026-07-19` verdict +
   citation annotation on every option (A1/A2/A3/B1/B2/B3/C) and a header note;
   the menu structure itself is untouched (annotation only, per the standing
   owner rule).
3. **`docs/eap-closeout-gba-2026-07-19.md`** — the finish/park enumeration
   updated so every arc's true built/park state shows with a citation: the stale
   Lumen Drift "zero tags/releases exist" row corrected (the v1.3 GitHub Release
   is published), and the two arcs the table omitted — Underroot (A2) and the
   Wickroad crossroads arc (B2) — added as DONE rows. Both docs end with the
   REMAINING REAL CHOICES line.
4. **Heartbeat** — a dated planning-mode section appended to `control/status.md`
   (per-section, no clobber of other sections).

## The audit (verified at HEAD `41a9a9f`, citations not vibes)

| Opt | Verdict | Citation |
|---|---|---|
| **A1 Tinderhand** (GBA card game) | **UNBUILT** | No `games/tinderhand*`, zero commits (`git log --grep tinderhand` empty). Pure menu proposal — a genuine open choice. |
| **A2 Underroot** (NDS colony) | **BUILT** | `games/underroot-nds/` 2044 LOC (`main.c`/`ur_sim.c`/`ur_sim.h`); slices 1–11 landed #155–#165 (slice-11 v1.0 audio `40779f8` #165 on main); `docs/arcs/UNDERROOT.md` "slices 1–11 all BUILT"; CI `nds-underroot-build` + 7 headless proofs. No committed `dist/` ROM; pending owner playtest. Was a Section-A *proposal* at menu time; since built. |
| **A3 Starloom** (PWA puzzle) | **UNBUILT** | No `games/starloom*`, zero commits. Pure menu proposal — a genuine open choice. |
| **B1 Tiltstone arc 2** | **BUILT** | `games/web-tiltstone/engine.js` **v1.8.0**; cuts via `c654e01` (cut1 smoke §14), `46293b5` (cut2 §15), `207e391` «cut 5 arc closer **(#170)**» (also carries cut-3 deception + cut-4 fingerprint); `node smoke.mjs` → **SMOKE PASS** (all green, v1.8.0). |
| **B2 Wickroad crossroads** | **BUILT** | All 4 cuts on main: #172 junction (`28d5980`), #173 sprite art (`1950f6e`), #174 seed dial (`f4fb8f4`), #175 «best ledger» arc closer (`7e30756`); code in `main.cpp`/`wr_milestones.h`, `dist/wickroad.gba` refreshed post-#201 (#206). |
| **B3 Brineward bestiary** | **BUILT** | Arc closer **#179** (`37c4e99`) carries all 4 cuts' code in `bw_sim.c` — grasper (cut1 `bw_has_grasper`), brace (cut2 `in->brace`), ambush (cut3 `bw_grasper_ambush`), hold track (cut4 `BW_UP_HOLD_OF`); cut1 also standalone #176 (`c338301`). #177/#178 closed unmerged as subsumed by #179 (#203/#204). `dist/brineward.nds` committed. |
| **C Lumen Drift + web arcade** | **BUILT / DONE** | **GitHub Release `lumen-drift-v1.3` PUBLISHED** 2026-07-18T20:07:16Z by owner (non-draft, ROM asset 167,996 B sha256 `195a867…` = committed ROM); tag on main `e64651c`. Web arcade Pages **LIVE** — `GET https://menno420.github.io/gba-homebrew/` → HTTP 200 (Undertow/Tiltstone/Drift Garden). Remaining: R4 itch.io external publish (owner action, not verified done) + R5 wasm stretch (unbuilt) — a polish tail, not a next-arc choice. |

**REMAINING REAL CHOICES (the key deliverable):** only **A1 (Tinderhand)** and
**A3 (Starloom)** are still genuine unbuilt proposals the owner could pick as a
next arc. A2, B1, B2, B3 are all BUILT; C's release is published and the arcade
is live (only R4 itch.io publish + R5 wasm stretch remain, both owner/stretch,
not "next arc" work).

## Honest limits

- **Audit + docs only — zero gameplay/build code touched.** The deliverable is
  annotations + a heartbeat + this card.
- **A2 Underroot / B3 Brineward NDS ROMs not re-proven from source** — no
  devkitARM toolchain in this environment; verdicts rest on symbol-traced source
  presence at HEAD, the arc docs, the merged slice/cut PRs, and (Brineward)
  committed `dist/brineward.nds`. Underroot has **no** committed dist ROM (noted).
- **C R4 (itch.io external publish) not measured** — an off-repo owner action
  with no in-repo signal; flagged unverified (its *release* half, R3, IS done).
- Only B1's proof (`node smoke.mjs`) and C's live probes (GitHub Release API +
  Pages HTTP 200) are first-hand runtime checks; the rest are tree/PR evidence.

## 💡 Session idea

**A decision menu decays fastest at exactly the options it recommends, because
those are the ones that get built — so a menu's own recommendation is the first
cell to audit as stale.** This menu recommended B1, and B1 shipped; but the same
dispatch energy that built B1 also built B2, B3, and A2, and clicked C's release
— leaving the menu offering seven "choices" of which five are already done. The
load-bearing move for auditing an owner-facing choice list is not to re-verify
each option's *definition* (the menu states those fine) but to diff each option
against the tree's HEAD and surface the **BUILT** ones loudly, because a stale
"pick one of seven" invites the owner to pick something already finished — the
worst outcome for a menu, worse than being wrong about an unbuilt option. The
REMAINING REAL CHOICES line is the whole value: it collapses a seven-way menu to
the two letters (A1, A3) that are still real, and that collapse is only visible
from the tree, never from the menu text.

## Previous-session review

- Prior lane slice: the EAP close-out / continuity doc
  `.sessions/2026-07-19-eap-closeout-gba.md` (PR #205, same
  session_01343oPvj5bzQZUsHuVsC9cK), merged via `auto-merge-enabler.yml`. Its 💡
  — "a wind-down doc's highest-value cells are the ones where the repo's own docs
  are confidently WRONG; diff the self-description against live ground truth and
  lead with the deltas" — held up exactly and drove this session: the audit's
  headline findings are all deltas the close-out either got right late (Tiltstone
  /Brineward/Pages DONE) or still had stale (the Lumen Drift **release** row said
  "zero tags/releases exist" when the v1.3 Release had already been published a
  day earlier, 2026-07-18). One gap that prior card left for this one to close:
  its finish/park table **omitted Underroot (A2) and the Wickroad crossroads arc
  (B2) entirely** — both built, neither enumerated — so a reader of the close-out
  alone would not learn A2/B2 are done. This audit adds those rows and the
  REMAINING REAL CHOICES line the close-out lacked.
- **Guard honored:** every Bash pinned to the absolute repo root
  `/home/user/gba-homebrew`, so no phantom second-checkout tree. Track-A
  isolation held (PUBLIC repo, original gba IP only — no Track-B/Nintendo/pokemon
  content touched or added); ORDER 007 honored (no routines armed).
