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
- **State:** best score persists via `localStorage` when available
  (guarded — private-mode/blocked storage degrades to in-memory best).

## Optional query parameters

- `?seed=N` — integer seed; all gameplay randomness is derived from it, so
  the same seed + same inputs is the identical run (daily-challenge ready).
- `?headless=1` — does not start the render loop; exposes the sim to
  `window.UNDERTOW` for automated tests. Not intended for players.

Packaged: this game ships in `dist/web/undertow/` with a versioned zip in
`dist/releases/` — see `docs/RELEASES.md`.
