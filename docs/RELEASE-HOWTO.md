# Release HOWTO — the owner's one-click publish steps

> **Status:** `owner-guidance`
>
> Exact steps, per channel, to publish what is already packaged. Artifact
> names, byte sizes and sha256s: [`RELEASES.md`](RELEASES.md). Everything
> in-repo (bundle, zips, hashes) is automated; every step below is the
> part only the owner can click.

## Channel 1 — static host (the whole arcade, zero build)

1. Upload the **`dist/web/`** directory **as-is** to any static file host
   (Netlify: drag the folder onto the deploys page; GitHub Pages, nginx,
   S3, anything — no server-side code, no special headers needed).
2. Done. The landing page is `index.html`; the games live at
   `undertow/`, `tiltstone/`, `drift-garden/` under whatever mount point
   you chose (everything is path-prefix agnostic, no external requests,
   strict-CSP safe).

Alternative: upload `dist/releases/web-arcade-v1.0.zip` to itch.io as an
HTML project (see channel 2) — same content, one file.

## Channel 2 — itch.io, per game

1. Create/edit the project → **Uploads** → upload the game's zip from
   `dist/releases/` (`web-undertow-v1.0.zip`, `web-tiltstone-v1.0.zip`,
   or `drift-garden-pwa-v0.1.zip`).
2. Tick **"This file will be played in the browser"**. The entry point is
   `index.html` at the zip root — itch.io detects it automatically.
3. Viewport suggestions: Undertow/Tiltstone 480×640 or larger;
   Drift Garden is portrait mobile-first (enable the mobile-friendly
   flag). No SharedArrayBuffer/COOP-COEP toggles needed.

## Channel 3 — GitHub Release

1. Draft a new Release on `main`; suggested tag: **`web-arcade-v1.0`**
   (create tag on publish), title `Web arcade v1.0`.
2. Attach the four zips from `dist/releases/`.
3. Paste the sha256 table rows from [`RELEASES.md`](RELEASES.md) into the
   Release notes so downloads are verifiable.
4. While you're there: the **Lumen Drift v1.3** Release is still pending —
   tag `lumen-drift-v1.3`, attach `dist/lumen-drift.gba` (spelled out in
   `docs/retro/archive-ready-2026-07-11.md`, item 1).

## Automated vs owner-only (plainly)

- **Automated, already done in-repo:** regenerating the bundle, the zips
  and the hashes — `tools/package-web-arcade.sh` rebuilds all of it
  deterministically; CI-checkable against the table in
  [`RELEASES.md`](RELEASES.md).
- **Owner-only:** any actual upload, publish or tag. Worker sessions get
  **403 on tags/releases** at the credential layer
  (`docs/PLATFORM-LIMITS.md`). The sanctioned automated path would be a
  `workflow_dispatch` release workflow whose `GITHUB_TOKEN` tags/releases
  server-side — this repo does **not** have one yet; a possible future
  slice, deliberately not created here (workflows are kit-managed).
