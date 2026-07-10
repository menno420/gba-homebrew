#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# gba-homebrew (game-lab Track B) — idempotent toolchain setup
#
# Installs the PINNED GBA toolchain this repo builds with:
#   * devkitARM r68 (binutils 2.46.0, gcc 16.1.0, newlib 4.6.0) — from the leseratte10
#     community mirror (https://wii.leseratte10.de/devkitPro/), because the
#     official devkitPro package infra is Cloudflare-403 behind the fleet
#     proxy (documented wall — docs/PLATFORM-LIMITS.md; do not re-probe).
#   * devkitarm-rules 1.6.0-4, general-tools 1.4.4, gba-tools 1.2.0,
#     grit 0.9.2, mmutil 1.9.2 — same mirror.
#   * devkitarm-crtls v1.2.7 — built from devkitPro's GitHub source (the
#     mirror carries no crtls package).
#   * Butano 21.7.1 (zlib license) — pinned clone into third_party/butano.
#
# ⚠ SUPPLY-CHAIN CAVEAT: the leseratte10 mirror is UNSIGNED community
#   infrastructure (packages are not devkitPro-signed). Acceptable for this
#   sandboxed lab lane; re-verify against official artifacts before anything
#   built through it is ever distributed by the owner.
#
# Works both in the fleet container (agent proxy; curl honours the
# preconfigured HTTPS_PROXY/CURL_CA_BUNDLE env) and on GitHub-hosted runners
# (no proxy). Idempotent: a pin-stamped marker file skips completed work, so
# CI can cache /opt/devkitpro + third_party/butano and re-run this cheaply.
#
# Usage:  tools/setup-toolchain.sh
# After:  export DEVKITPRO=/opt/devkitpro
#         export DEVKITARM=$DEVKITPRO/devkitARM
#         export PATH="$DEVKITARM/bin:$DEVKITPRO/tools/bin:$PATH"
# ---------------------------------------------------------------------------
set -euo pipefail

# --- pins ------------------------------------------------------------------
MIRROR="https://wii.leseratte10.de/devkitPro"
R68_DIR="devkitARM/r68%20%282026-06-10%29"
PKG_URLS=(
    "$MIRROR/$R68_DIR/devkitARM-r68-1-any.pkg.tar.zst"
    "$MIRROR/$R68_DIR/devkitarm-binutils-2.46.0-1-linux_x86_64.pkg.tar.zst"
    "$MIRROR/$R68_DIR/devkitarm-gcc-16.1.0-1-linux_x86_64.pkg.tar.zst"
    # newlib is not re-published in the r68 dir; r68 pairs with the newlib
    # 4.6.0 package from the r67 dir (same upstream package repo).
    "$MIRROR/devkitARM/r67%20%282026-01%29/devkitarm-newlib-4.6.0.20260123-5-any.pkg.tar.zst"
    "$MIRROR/devkitARM/devkitarm-rules/devkitarm-rules-1.6.0-4-any.pkg.tar.zst"
    "$MIRROR/other-stuff/general-tools/general-tools-1.4.4-1-linux_x86_64.pkg.tar.zst"
    "$MIRROR/other-stuff/gba-tools/gba-tools-1.2.0-1-linux_x86_64.pkg.tar.xz"
    "$MIRROR/other-stuff/grit/grit-0.9.2-1-linux_x86_64.pkg.tar.xz"
    "$MIRROR/other-stuff/mmutil/mmutil-1.9.2-1-linux_x86_64.pkg.tar.xz"
)
CRTLS_REPO="https://github.com/devkitPro/devkitarm-crtls.git"
CRTLS_TAG="v1.2.7"
BUTANO_REPO="https://github.com/GValiente/butano.git"
BUTANO_TAG="21.7.1"
BUTANO_SHA="112a1827c9c6d9e6041a7e93e66f04c4561a6415"
TOOLCHAIN_STAMP="gba-lab-toolchain-r68-crtls${CRTLS_TAG}-v1"

DKP="${DEVKITPRO:-/opt/devkitpro}"
REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUTANO_DIR="$REPO_ROOT/third_party/butano"

# --- helpers ---------------------------------------------------------------
SUDO=""
if [ "$(id -u)" != "0" ] && command -v sudo >/dev/null 2>&1; then
    SUDO="sudo"
fi

log() { printf '>> %s\n' "$*"; }

fetch() { # fetch <url> <dest> — 3 tries with backoff
    local url="$1" dest="$2" delay=2 try
    for try in 1 2 3; do
        if curl -sSfL --connect-timeout 20 -o "$dest" "$url"; then return 0; fi
        [ "$try" = 3 ] && { echo "FATAL: download failed: $url" >&2; return 1; }
        log "retry $try for $url (sleep $delay)"; sleep "$delay"; delay=$((delay * 2))
    done
}

# --- devkitARM + tools -----------------------------------------------------
if [ -f "$DKP/.$TOOLCHAIN_STAMP" ]; then
    log "devkitARM toolchain already installed ($DKP) — skipping"
else
    if ! command -v zstd >/dev/null 2>&1; then
        log "installing zstd (needed for .pkg.tar.zst)"
        $SUDO apt-get update -qq && $SUDO apt-get install -y -qq zstd
    fi
    WORK="$(mktemp -d)"
    trap 'rm -rf "$WORK"' EXIT
    mkdir -p "$WORK/stage"
    for url in "${PKG_URLS[@]}"; do
        pkg="$WORK/$(basename "$url")"
        log "fetching $(basename "$url")"
        fetch "$url" "$pkg"
        tar -C "$WORK/stage" -xf "$pkg" \
            --exclude=.PKGINFO --exclude=.BUILDINFO --exclude=.MTREE \
            --warning=no-unknown-keyword
    done
    log "installing packages into $DKP"
    $SUDO mkdir -p "$DKP"
    $SUDO cp -a "$WORK/stage/opt/devkitpro/." "$DKP/"

    log "building devkitarm-crtls $CRTLS_TAG from source (no mirror package)"
    git clone --quiet --depth 1 --branch "$CRTLS_TAG" "$CRTLS_REPO" "$WORK/crtls"
    make -C "$WORK/crtls" --no-print-directory -s \
        DEVKITPRO="$DKP" DEVKITARM="$DKP/devkitARM" \
        PATH="$DKP/devkitARM/bin:$PATH" >/dev/null
    # crtls' install target hardcodes /opt/devkitpro via DESTDIR; copy manually
    $SUDO mkdir -p "$DKP/devkitARM/arm-none-eabi/lib"
    (cd "$WORK/crtls" && $SUDO cp -a ./*.specs ./*.ld ./*.mem ./*.o thumb armv6k \
        "$DKP/devkitARM/arm-none-eabi/lib/")

    $SUDO touch "$DKP/.$TOOLCHAIN_STAMP"
    log "devkitARM toolchain installed"
fi

# --- Butano (pinned vendor clone, gitignored) -------------------------------
if [ -d "$BUTANO_DIR/.git" ]; then
    have_sha="$(git -C "$BUTANO_DIR" rev-parse HEAD)"
    if [ "$have_sha" = "$BUTANO_SHA" ]; then
        log "Butano $BUTANO_TAG already present ($BUTANO_DIR) — skipping"
    else
        log "Butano checkout at wrong SHA ($have_sha) — re-pinning to $BUTANO_TAG"
        git -C "$BUTANO_DIR" fetch --quiet --depth 1 origin "refs/tags/$BUTANO_TAG:refs/tags/$BUTANO_TAG" || true
        git -C "$BUTANO_DIR" checkout --quiet "$BUTANO_SHA"
    fi
else
    log "cloning Butano $BUTANO_TAG into $BUTANO_DIR"
    git clone --quiet --depth 1 --branch "$BUTANO_TAG" "$BUTANO_REPO" "$BUTANO_DIR"
fi
actual_sha="$(git -C "$BUTANO_DIR" rev-parse HEAD)"
if [ "$actual_sha" != "$BUTANO_SHA" ]; then
    echo "FATAL: Butano SHA mismatch: want $BUTANO_SHA got $actual_sha" >&2
    exit 1
fi

log "toolchain ready. Export before building:"
log "  export DEVKITPRO=$DKP DEVKITARM=$DKP/devkitARM"
log "  export PATH=\"$DKP/devkitARM/bin:$DKP/tools/bin:\$PATH\""
