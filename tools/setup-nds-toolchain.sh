#!/usr/bin/env bash
# ---------------------------------------------------------------------------
# gba-homebrew (game-lab Track B) — idempotent NDS toolchain setup
#
# Installs the PINNED Nintendo DS toolchain the Gloamline arc builds with
# (arc slice 2 feasibility gate, 2026-07-11):
#   * BlocksDS 1.21.1 (open-source NDS SDK: libnds, ndstool, grit, mmutil,
#     default ARM7 cores) — from the BlocksDS pacman repo
#     (https://blocksds.skylyrac.net/packages/).
#   * Wonderful toolchain arm-none-eabi GCC 16.1.1 + binutils 2.46.1 +
#     picolibc — from the Wonderful pacman repo
#     (https://wonderful.asie.pl/packages/).
#   * wf-bootstrap (wf-pacman + wf-patchelf runtime) — the Wonderful
#     bootstrap tarball; wf-patchelf's post-transaction hook adjusts the
#     dynamic-loader paths of the prebuilt host binaries, which is why the
#     install goes through wf-pacman -U instead of a plain tar extract.
#
# NOTE (probed 2026-07-11): unlike the official devkitPro infra (Cloudflare
# 403 — docs/PLATFORM-LIMITS.md), the Wonderful + BlocksDS package hosts are
# DIRECTLY reachable through the fleet proxy — no mirror needed.
#
# ⚠ SUPPLY-CHAIN POSTURE: same trust-on-first-use discipline as
#   tools/setup-toolchain.sh — the repos are community infrastructure, so
#   every artifact is PINNED BY SHA-256 below (hashes recorded 2026-07-11
#   from the install verified working in this container: .nds built AND
#   boot-verified headlessly) and the install FAILS on any mismatch. The
#   bootstrap URL is a rolling artifact upstream; when upstream rotates it,
#   this script fails closed and the pin must be re-verified deliberately.
#   (CI cache keys hash this script, so changing a pin correctly
#   invalidates the cached toolchain.)
#
# The Wonderful packages are prefix-bound to /opt/wonderful (package
# payload paths + loader patching), so unlike the GBA script the install
# root is NOT relocatable. On GitHub runners, sudo mkdir + chown makes it
# writable for the runner user; the cache step caches /opt/wonderful.
#
# Usage:  tools/setup-nds-toolchain.sh
# After:  export WONDERFUL_TOOLCHAIN=/opt/wonderful
#         export BLOCKSDS=/opt/wonderful/thirdparty/blocksds/core
#         (the games/gloamline-nds Makefile defaults to those paths)
# ---------------------------------------------------------------------------
set -euo pipefail

# --- pins ------------------------------------------------------------------
WF_BOOTSTRAP_URL="https://wonderful.asie.pl/bootstrap/wf-bootstrap-x86_64.tar.gz"
WF_BOOTSTRAP_SHA256="923bcd6d0e6c89bc5d19d9a85d1dd0b749b2aa46dc4d470e106b3d7d547e96a3"

WF_REPO="https://wonderful.asie.pl/packages/rolling/linux/x86_64"
BDS_REPO="https://blocksds.skylyrac.net/packages/rolling/linux/x86_64"
# NOTE: the ':' in the gcc epoch ('1:16.1.1...') must be URL-encoded (%3A) in
# the fetch URL but stays literal in the on-disk filename.
PKG_URLS=(
    "$WF_REPO/runtime-zlib-1.3.2-1-x86_64.pkg.tar.xz"
    "$WF_REPO/runtime-zstd-1.5.7-1-x86_64.pkg.tar.xz"
    "$WF_REPO/toolchain-gcc-arm-none-eabi-binutils-2.46.1-1-x86_64.pkg.tar.xz"
    "$WF_REPO/toolchain-gcc-arm-none-eabi-gcc-1%3A16.1.1.r228552.d6ba7128f65-2-x86_64.pkg.tar.xz"
    "$WF_REPO/toolchain-gcc-arm-none-eabi-gcc-libs-1%3A16.1.1.r228552.d6ba7128f65-2-x86_64.pkg.tar.xz"
    "$WF_REPO/toolchain-gcc-arm-none-eabi-libstdcxx-picolibc-16.1.1.r228552.d6ba7128f65-1-any.pkg.tar.xz"
    "$WF_REPO/toolchain-gcc-arm-none-eabi-picolibc-generic-1.8.11.r26127.2a7b920f5-1-any.pkg.tar.xz"
    "$BDS_REPO/blocksds-toolchain-1.21.1-1-x86_64.pkg.tar.xz"
)
# SHA-256 pins (decoded basename -> hash), recorded 2026-07-11 from the
# known-good install this container built and boot-verified with.
declare -A PKG_SHA256=(
    ["runtime-zlib-1.3.2-1-x86_64.pkg.tar.xz"]="40abdd299da2249cb356c8a7bf15ffdb8fdda95e4a67aa8e0f5f982a381d6282"
    ["runtime-zstd-1.5.7-1-x86_64.pkg.tar.xz"]="ef93cdacf6c921375786f81286b55682c7468b3eb9c86c860576fb2d727afa7c"
    ["toolchain-gcc-arm-none-eabi-binutils-2.46.1-1-x86_64.pkg.tar.xz"]="0fd4778950696e62709928f379f7b49318da34a09cb7a0e06d3db6a4df4d99c6"
    ["toolchain-gcc-arm-none-eabi-gcc-1:16.1.1.r228552.d6ba7128f65-2-x86_64.pkg.tar.xz"]="8f571e37875665d355026cb77a02c103651f9db0ae1c21eab4041282169e7906"
    ["toolchain-gcc-arm-none-eabi-gcc-libs-1:16.1.1.r228552.d6ba7128f65-2-x86_64.pkg.tar.xz"]="3f707fd42d2e54714c3451e3a5d55658d9d1efe0fea669707511b357714064d2"
    ["toolchain-gcc-arm-none-eabi-libstdcxx-picolibc-16.1.1.r228552.d6ba7128f65-1-any.pkg.tar.xz"]="df9b6c1bb3acb9e7c79bae645e9991c2619e961cb53ebfb249f586bb4e16ec5d"
    ["toolchain-gcc-arm-none-eabi-picolibc-generic-1.8.11.r26127.2a7b920f5-1-any.pkg.tar.xz"]="42849a6d147f24100ebeb8556dab8c1921ac98bc4a19f96d279532134f7e24c6"
    ["blocksds-toolchain-1.21.1-1-x86_64.pkg.tar.xz"]="2fff228c254aa6b286a59ddabba5984da7b6942e394e7a692022ec0393975bfa"
)
TOOLCHAIN_STAMP="nds-lab-toolchain-blocksds1.21.1-gcc16.1.1-v1"

WF="/opt/wonderful"   # prefix-bound by the packages — not relocatable

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

# --- bootstrap + pinned packages --------------------------------------------
if [ -f "$WF/.$TOOLCHAIN_STAMP" ]; then
    log "NDS toolchain already installed ($WF) — skipping"
else
    WORK="$(mktemp -d)"
    trap 'rm -rf "$WORK"' EXIT

    log "fetching wf-bootstrap"
    fetch "$WF_BOOTSTRAP_URL" "$WORK/wf-bootstrap.tar.gz"
    got="$(sha256sum "$WORK/wf-bootstrap.tar.gz" | cut -d' ' -f1)"
    if [ "$got" != "$WF_BOOTSTRAP_SHA256" ]; then
        echo "FATAL: SHA-256 mismatch for wf-bootstrap-x86_64.tar.gz" >&2
        echo "  want $WF_BOOTSTRAP_SHA256" >&2
        echo "  got  $got" >&2
        echo "  Upstream rotates this rolling artifact; re-verify the new one" >&2
        echo "  deliberately before updating the pin — do NOT bypass." >&2
        exit 1
    fi
    log "sha256 OK: wf-bootstrap-x86_64.tar.gz"
    $SUDO mkdir -p "$WF"
    if [ -n "$SUDO" ]; then
        $SUDO chown -R "$(id -u):$(id -g)" "$WF"
    fi
    tar xzf "$WORK/wf-bootstrap.tar.gz" -C "$WF"

    pkgs=()
    for url in "${PKG_URLS[@]}"; do
        name="$(basename "$url" | sed 's/%3A/:/g')"
        pkg="$WORK/$name"
        log "fetching $name"
        fetch "$url" "$pkg"
        want="${PKG_SHA256[$name]:-}"
        if [ -z "$want" ]; then
            echo "FATAL: no SHA-256 pin recorded for $name — refusing artifact" >&2
            exit 1
        fi
        got="$(sha256sum "$pkg" | cut -d' ' -f1)"
        if [ "$got" != "$want" ]; then
            echo "FATAL: SHA-256 mismatch for $name" >&2
            echo "  want $want" >&2
            echo "  got  $got" >&2
            echo "  The repo served a different artifact than the pinned" >&2
            echo "  known-good one — do NOT bypass; investigate first." >&2
            exit 1
        fi
        log "sha256 OK: $name"
        pkgs+=("$pkg")
    done

    log "installing ${#pkgs[@]} pinned packages via wf-pacman -U"
    "$WF/bin/wf-pacman" -U --noconfirm "${pkgs[@]}"

    touch "$WF/.$TOOLCHAIN_STAMP"
    log "NDS toolchain installed"
fi

"$WF/toolchain/gcc-arm-none-eabi/bin/arm-none-eabi-gcc" --version | head -1
cat "$WF/thirdparty/blocksds/core/version.txt"
log "toolchain ready. Export before building:"
log "  export WONDERFUL_TOOLCHAIN=$WF"
log "  export BLOCKSDS=$WF/thirdparty/blocksds/core"
