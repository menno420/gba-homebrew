# Undertow — hosting notes

> **Status:** `reference`

What the arcade needs to serve this game:

- **Files:** `index.html` + `game.js` only. Copy the directory as-is.
- **Entry point:** `index.html`.
- **No build step.** Plain HTML5 canvas + vanilla JS, zero dependencies.
- **No server-side code.** Any static file host works; opening
  `index.html` via `file://` also works.
- **No external requests.** No CDN, no fonts, no analytics — safe under a
  strict CSP.
- **Path-prefix agnostic.** `game.js` is referenced relatively, so the game
  works under any mount point (`/arcade/undertow/`, a hashed folder, etc.).
- **State:** best score, skin choice and the best-run ghost per seed
  persist via `localStorage` when available (guarded — private-mode/blocked
  storage degrades to in-memory values).

## Optional query parameters

- `?seed=N` — integer seed; all gameplay randomness is derived from it, so
  the same seed + same inputs is the identical run. Wins over `?daily=1`
  (a shared link must replay the same run whenever it is opened).
- `?daily=1` — daily dive: the seed derives from the UTC calendar date as
  `YYYYMMDD` (e.g. `20260714`), so everyone gets the same trench on the
  same UTC day. The date is read once at boot to pick the seed — the sim
  itself never reads the clock. Link the arcade tile to
  `index.html?daily=1` to run it as a daily challenge.
- `?depth=M` — render-only challenge target carried by share links; shown
  on the title screen as a score to beat. Never read by the sim.
- `?skin=ID` — render-only cosmetic: diver colors + bubble-trail style
  (`classic`, `abyss`, `ember`, `ghost`). Unknown IDs fall back to the
  persisted choice (then `classic`). `C` on the title/gameover screens
  cycles skins; the choice persists via guarded `localStorage`
  (`undertow.skin`). Skins never touch the sim — same seed, same run,
  whatever the skin.
- `?ghost=0` — disables the ghost replay (render-side only). By default,
  every run records its input timeline and the best run per seed persists
  via guarded `localStorage` (`undertow.ghost.<seed>`); on later runs of
  the same seed a translucent ghost diver replays that best run in
  lockstep. The ghost is a second, independent sim instance — it never
  touches the live run: same seed, same run, ghost or no ghost.
- `?headless=1` — does not start the render loop; exposes the sim to
  `window.UNDERTOW` for automated tests. Not intended for players.

## Sharing a run

On the gameover screen, `S` copies a challenge link
(`?seed=N&depth=M` — the seed pinned explicitly, plus the depth to beat).
Where the clipboard is unavailable (blocked, `file://`), the link is drawn
on the canvas instead. No server involved: the URL itself is the share.
