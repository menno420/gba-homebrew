# Tiltstone — hosting notes

> **Status:** `reference`

What the arcade needs to serve this game:

- **Files:** `index.html` + `engine.js` + `juice.js` + `app.js` only (the
  `.md` docs and `smoke.mjs` are repo material, not runtime). Copy the
  directory as-is.
- **Entry point:** `index.html`.
- **No build step.** Plain HTML5 canvas + vanilla JS, zero dependencies.
- **No server-side code.** Any static file host works; opening
  `index.html` via `file://` also works (plain scripts, no ES-module CORS
  trap).
- **No external requests.** No CDN, no fonts, no analytics — safe under a
  strict CSP (`default-src 'none'; script-src 'self'; style-src
  'unsafe-inline'` is sufficient; styles are inline in the page).
- **Path-prefix agnostic.** Scripts are referenced relatively, so the game
  works under any mount point (`/arcade/tiltstone/`, a hashed folder, etc.).
- **No special headers** needed (no SharedArrayBuffer, no workers, no wasm).
- **State:** best-level-per-seed persists via `localStorage` when available
  (guarded — private-mode/blocked storage degrades silently).

## Optional query parameters

- `?seed=N` — integer seed; the level chain is a pure function of it. No
  seed = today's UTC daily seed (`YYYYMMDD` as a number), same for everyone.
- `?level=L` — start at level index L of the seed's chain (default 0).
