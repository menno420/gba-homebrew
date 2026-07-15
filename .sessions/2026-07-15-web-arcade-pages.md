# Session — Web arcade GitHub Pages deploy

> **Status:** `complete`

- date: 2026-07-15 (branch `claude/web-arcade-pages`; started 21:19Z,
  `date -u` at write time)
- mission: make the static web arcade hostable at
  https://menno420.github.io/gba-homebrew/ with minimum owner effort —
  a GitHub Pages deployment workflow publishing `dist/web/` ONLY (no
  ROMs, no Track A material), a subpath-safety audit of the arcade
  files, a short docs note, and a READY PR parked green with a
  paste-ready owner ask. NOT merged by this session.
- **📊 Model:** Fable · medium · infra build — web arcade Pages deploy
- landing posture: PR #149 opened READY at 21:22Z (post-workflow
  commit); no merge/approve/auto-merge calls from this session — the
  owner clicks.

## What shipped

1. Born-red gate (commit 730be10): this card `in-progress` + work claim
   `control/claims/claude-web-arcade-pages.md` (written by
   `bootstrap.py claim`); claim retired at the heartbeat (76c47b8) per
   claims README rule 4.
2. **The workflow** (commit 4cf6325,
   `.github/workflows/deploy-pages.yml`): push-to-main filtered to
   `dist/web/**` + the workflow file itself, plus `workflow_dispatch`;
   `checkout@v4` → `configure-pages@v5` → `upload-pages-artifact@v3`
   (path: `dist/web`) → `deploy-pages@v4`; permissions contents:read /
   pages:write / id-token:write, concurrency group `pages`. Deliberately
   NO `enablement: true` — GITHUB_TOKEN can't enable Pages; the owner's
   one Settings click does. Publishes `dist/web/` ONLY: no ROMs, no
   zips, no Track A material — the track-isolation rail holds by
   construction (the artifact path can't reach them).
3. **Subpath audit** (site serves from `/gba-homebrew/`, not the domain
   root): `grep -rn 'src="/\|href="/\|url(/\|fetch("/\|from "/'
   dist/web/` → zero matches; manual reads of `index.html` (relative
   game links), all three game entry files (relative script tags,
   `./game.js` module import, `./sw.js` registration), the Drift Garden
   manifest (`start_url: "./index.html"`, `scope: "./"`) and service
   worker (all-`./` shell list, origin check via `self.location.origin`).
   **No absolute paths anywhere — zero fixes needed.**
4. **Docs** (same commit): "Hosted arcade (GitHub Pages)" paragraph in
   the existing Web-arcade section of `dist/README.md` — the URL, the
   redeploy triggers, the one-time owner setup. No new doc file.
5. PR #149 opened READY with Before/After, the subpath-safety audit,
   and the paste-ready OWNER ASK (~2 min: Pages Source = GitHub
   Actions, merge, verify the URL; reversible).
6. CI at the born-red head: substrate-gate red purely on this card's
   in-progress badge (the designed hold), rom-builds green untouched,
   auto-merge-enabler green (no arming by this session); headless-boot
   path-filtered out (no game sources touched).

## 💡 Session idea

**A deploy workflow's artifact `path:` is a publish-scope guarantee you
can cite, not just a build setting.** The track-isolation worry ("no
ROMs on the public site") dissolves the moment the Pages artifact step
says `path: dist/web` — nothing outside that directory CAN ship, no
matter what else lands in the repo. When a rail is "never publish X",
prefer a mechanism where X is structurally unreachable over a checklist
that says to exclude it — then cite the mechanism in the PR so the
reviewer inherits the proof instead of re-deriving it.

## Previous-session review

- Prior lane slice: `.sessions/2026-07-15-wickroad-audio.md` (PR #146,
  Wickroad growth cut 5) — its 💡 ("run the old committed suite against
  the new build FIRST and let the pass/fail pattern classify the change
  as free or taxed") transferred here in miniature: the subpath audit
  was exactly that move — measure the existing artifact against the new
  serving condition (a `/gba-homebrew/` prefix) before writing any fix,
  and the measurement came back "the change is free" (zero absolute
  paths), so no fix work contaminated the diff. The card's stated
  posture (READY on green, enabler decides, no self-merge) matched the
  observable record. One gap worth naming: the card's close-out never
  says WHERE a player can reach the web games it helped package — the
  arcade had been "host-ready" through six packaging repins without any
  session asking "ready for WHAT host?"; this slice exists because that
  question finally got asked.

- **📊 Model:** Fable · medium · infra build — web arcade Pages deploy
