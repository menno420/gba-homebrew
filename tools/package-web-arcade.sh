#!/usr/bin/env bash
# package-web-arcade.sh — rebuild dist/web/ (host-ready static arcade) and
# dist/releases/ (deterministic versioned zips) from the game sources at the
# current tree, and (with --verify) assert the committed manifest.
#
# ORDER 006 repin (2026-07-14, supersedes the #130 arcade-refresh cut):
# the #92<-#93<-#95<-#97 Tiltstone stack has landed on main, so the standing
# #93 follow-up is served — juice.js is now on the Tiltstone stage list
# (per the updated games/web-tiltstone/HOSTING.md contract) and Tiltstone
# ships at v1.1 (arcade bundle v1.3). Undertow v1.5 and Drift Garden v1.4
# are unchanged from the #130 cut (PRs #110/#114/#118/#123/#126 and
# #111/#115/#119/#124 respectively).
#
# Deterministic by construction: fixed source epoch on every staged file and
# directory, zip member list sorted with LC_ALL=C, `zip -X` (no platform
# extra fields). Two runs at the same tree produce byte-identical zips — so
# the sha256s pinned in docs/RELEASES.md are re-derivable with one command:
#
#   tools/package-web-arcade.sh            # rebuild dist/web + dist/releases
#   tools/package-web-arcade.sh --verify   # rebuild, then assert every pin
#                                          # in docs/RELEASES.md (exit 1 on
#                                          # any drift)
#
# Runtime file sets are the per-game hosting contracts, verbatim:
#   games/web-undertow/HOSTING.md      -> index.html, game.js
#   games/web-tiltstone/HOSTING.md     -> index.html, engine.js, juice.js,
#       app.js
#   games/mobile-foundation/README.md  -> index.html, game.js,
#       manifest.webmanifest, sw.js, icon-192.png, icon-512.png
# (.md docs and smoke scripts are repo material, not runtime — never staged.)
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WEB="$ROOT/dist/web"
REL="$ROOT/dist/releases"
SOURCE_EPOCH="2026-07-14 00:00:00 UTC"

# --- clean rebuild ----------------------------------------------------------
rm -rf "$WEB" "$REL"
mkdir -p "$WEB/undertow" "$WEB/tiltstone" "$WEB/drift-garden" "$REL"

# --- stage runtime files ONLY (per each game's hosting contract) ------------
cp "$ROOT/games/web-undertow/index.html" "$ROOT/games/web-undertow/game.js" \
   "$WEB/undertow/"
cp "$ROOT/games/web-tiltstone/index.html" "$ROOT/games/web-tiltstone/engine.js" \
   "$ROOT/games/web-tiltstone/juice.js" "$ROOT/games/web-tiltstone/app.js" \
   "$WEB/tiltstone/"
cp "$ROOT/games/mobile-foundation/index.html" \
   "$ROOT/games/mobile-foundation/game.js" \
   "$ROOT/games/mobile-foundation/manifest.webmanifest" \
   "$ROOT/games/mobile-foundation/sw.js" \
   "$ROOT/games/mobile-foundation/icon-192.png" \
   "$ROOT/games/mobile-foundation/icon-512.png" \
   "$WEB/drift-garden/"

# --- arcade landing page (heredoc so every run regenerates it identically) --
cat > "$WEB/index.html" <<'HTML'
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>menno420 game-lab — web arcade</title>
<style>
  body { margin: 0; background: #10141c; color: #d8dee8;
         font-family: Georgia, 'Times New Roman', serif; }
  main { max-width: 40em; margin: 0 auto; padding: 2.5em 1.25em 4em; }
  h1 { font-size: 1.6em; letter-spacing: 0.04em; color: #f0f4fa; }
  p.lede { color: #9aa5b5; }
  ul.games { list-style: none; padding: 0; }
  ul.games li { margin: 1.4em 0; padding: 1em 1.2em;
                background: #171d29; border: 1px solid #2a3242;
                border-radius: 6px; }
  ul.games a { color: #7fd4ff; font-size: 1.15em; text-decoration: none; }
  ul.games a:hover { text-decoration: underline; }
  ul.games span.desc { display: block; margin-top: 0.35em;
                       color: #b9c2d0; font-size: 0.95em; }
  footer { margin-top: 3em; color: #6b7484; font-size: 0.85em;
           border-top: 1px solid #2a3242; padding-top: 1em; }
</style>
</head>
<body>
<main>
<h1>Web arcade</h1>
<p class="lede">Three original games. No accounts, no downloads, no
external requests &mdash; each one is plain HTML and vanilla JS.</p>
<ul class="games">
  <li>
    <a href="undertow/">Undertow</a>
    <span class="desc">One-hand arcade survival: free-dive a bottomless
    trench whose walls squeeze tighter the deeper you go &mdash; depth is
    the only score, and the same seed always plays out identically.</span>
  </li>
  <li>
    <a href="tiltstone/">Tiltstone</a>
    <span class="desc">Turn-based gravity puzzle: your only verb is turning
    the whole cavern 90&deg;. Everything falls, three gems of a kind
    collect, and every seed is provably winnable.</span>
  </li>
  <li>
    <a href="drift-garden/">Drift Garden</a>
    <span class="desc">Ambient touch strategy in a pocket garden: plant
    drifting motes, shepherd them into pollinating pairs, and harvest the
    day&rsquo;s quota before dusk sends the wisps hunting. Seeded rounds
    &mdash; the same seed is the same garden. Installable, offline-capable
    PWA.</span>
  </li>
</ul>
<footer>menno420 game-lab &mdash; built from main</footer>
</main>
</body>
</html>
HTML

# --- pin every staged file + dir to the fixed source epoch ------------------
find "$WEB" -exec touch -d "$SOURCE_EPOCH" {} +

# --- deterministic zip helper -----------------------------------------------
make_zip() {
  local stage="$1" out="$2"
  rm -f "$out"
  if command -v zip >/dev/null 2>&1; then
    (cd "$stage" && find . -type f | LC_ALL=C sort | TZ=UTC zip -X -q "$out" -@)
  else
    python3 - "$stage" "$out" <<'PY'
import os, sys, zipfile
stage, out = sys.argv[1], sys.argv[2]
names = []
for root, dirs, files in os.walk(stage):
    for f in files:
        p = os.path.join(root, f)
        names.append(os.path.relpath(p, stage))
names.sort()
with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
    for n in names:
        info = zipfile.ZipInfo(n, date_time=(2026, 7, 14, 0, 0, 0))
        info.external_attr = 0o644 << 16
        with open(os.path.join(stage, n), "rb") as fh:
            z.writestr(info, fh.read(), zipfile.ZIP_DEFLATED)
PY
  fi
}

# --- versioned release zips ---------------------------------------------------
# Tiltstone becomes v1.1: the #92<-#93<-#95<-#97 stack landed (par+undo,
# juice, cell types, level packs) — its staged runtime grew a file
# (juice.js) and every member changed. Undertow stays v1.5 and Drift
# Garden v1.4 (staged bytes unchanged since the #130 cut). The arcade
# bundle bumps to v1.3 (its Tiltstone members changed).
make_zip "$WEB/undertow"     "$REL/web-undertow-v1.5.zip"
make_zip "$WEB/tiltstone"    "$REL/web-tiltstone-v1.1.zip"
make_zip "$WEB/drift-garden" "$REL/drift-garden-v1.4.zip"
make_zip "$WEB"              "$REL/web-arcade-v1.3.zip"

# --- provenance printout -------------------------------------------------------
echo "dist/web/ + dist/releases/ rebuilt (source epoch: $SOURCE_EPOCH)"
(cd "$ROOT" && sha256sum dist/releases/*.zip)

# --- --verify: assert every pin in docs/RELEASES.md ---------------------------
if [ "${1:-}" = "--verify" ]; then
  fail=0
  while read -r want path; do
    [ -z "$want" ] && continue
    got="$(cd "$ROOT" && sha256sum "$path" | cut -d' ' -f1)"
    if [ "$got" != "$want" ]; then
      echo "MANIFEST DRIFT: $path" >&2
      echo "  pinned $want" >&2
      echo "  actual $got" >&2
      fail=1
    fi
  done < <(grep -E '^    [0-9a-f]{64}  ' "$ROOT/docs/RELEASES.md" | sed 's/^    //')
  if [ "$fail" = 1 ]; then
    echo "VERIFY: FAIL — regenerate docs/RELEASES.md or investigate drift" >&2
    exit 1
  fi
  echo "VERIFY: every docs/RELEASES.md pin matches the rebuilt tree"
fi
