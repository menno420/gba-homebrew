#!/usr/bin/env bash
# One-command headless GAME smoke for games/mobile-foundation (Drift Garden).
# Same scratch-dir playwright-core pattern as run.sh: installs NOTHING into
# the repo, never runs `playwright install` — the container's Chromium
# (PLAYWRIGHT_BROWSERS_PATH, default /opt/pw-browsers) is used directly.
set -euo pipefail

REPO="$(cd "$(dirname "$0")/../.." && pwd)"
SCRATCH="${SMOKE_SCRATCH:-${TMPDIR:-/tmp}/mobile-foundation-smoke-deps}"
PORT="${SMOKE_PORT:-8902}"

if [ ! -d "$SCRATCH/node_modules/playwright-core" ]; then
  mkdir -p "$SCRATCH"
  (cd "$SCRATCH" && npm i playwright-core --no-audit --no-fund --no-save --loglevel=error)
fi

python3 -m http.server "$PORT" --directory "$REPO/games/mobile-foundation" &
SRV=$!
trap 'kill "$SRV" 2>/dev/null || true' EXIT
sleep 1

SMOKE_URL="http://127.0.0.1:$PORT/index.html" \
NODE_PATH="$SCRATCH/node_modules" \
node "$REPO/tools/mobile-foundation-smoke/game-smoke.mjs"
