#!/usr/bin/env bash
# One-command headless smoke for games/mobile-foundation.
# Installs NOTHING into the repo: playwright-core goes to a scratch dir.
# Requires: node >= 18, python3, a Playwright-provisioned Chromium
# (PLAYWRIGHT_BROWSERS_PATH, default /opt/pw-browsers) or CHROMIUM_PATH.
set -euo pipefail

REPO="$(cd "$(dirname "$0")/../.." && pwd)"
SCRATCH="${SMOKE_SCRATCH:-${TMPDIR:-/tmp}/mobile-foundation-smoke-deps}"
PORT="${SMOKE_PORT:-8901}"

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
node "$REPO/tools/mobile-foundation-smoke/smoke.mjs"
