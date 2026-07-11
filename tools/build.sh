#!/usr/bin/env bash
# One-command build: compile every games/*/ ROM and print size + sha256.
# Toolchain must be installed first (tools/setup-toolchain.sh). Honours
# $DEVKITPRO (default /opt/devkitpro). Recipe doc: docs/BUILDING.md.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"
export DEVKITARM="$DEVKITPRO/devkitARM"
export PATH="$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH"

if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    echo "FATAL: arm-none-eabi-gcc not found — run tools/setup-toolchain.sh first" >&2
    exit 1
fi

built=0
for dir in "$REPO_ROOT"/games/*/; do
    [ -f "$dir/Makefile" ] || continue
    make -C "$dir" -j"$(nproc)"
    rom="$dir$(basename "$dir").gba"
    test -f "$rom"
    built=$((built + 1))
done
# Same floor as .github/workflows/rom-builds.yml (-ge 2): the repo ships two
# games, and the local tool should fail exactly where CI would if one of
# them silently vanished from the build (review-queue row #29).
test "$built" -ge 2

echo
echo "== ROMs built =="
for rom in "$REPO_ROOT"/games/*/*.gba; do
    printf '%8d bytes  %s\n' "$(stat -c %s "$rom")" "${rom#"$REPO_ROOT"/}"
done
(cd "$REPO_ROOT" && sha256sum games/*/*.gba)
