#!/usr/bin/env bash
#
# scripts/verify-qt-dmg.sh -- c152-P
#
# Post-receipt structural verification for AAASeed-Studio-X.Y.Z.dmg.
# Inverse of scripts/ship-qt-dmg.sh : where ship-qt-dmg.sh BUILDS the
# DMG, this script CONFIRMS a received DMG is structurally valid
# (downloaded from a release, copied off a USB, etc.).
#
# What we check (structural only -- NOT signature):
#   - hdiutil verify (checksum + filesystem integrity)
#   - mount read-only
#   - AAASeed Studio.app exists
#   - AAASeed-Studio executable is present + arm64
#   - Qt frameworks bundled (QtCore, QtGui, QtQml, QtQuick, QtMultimedia)
#   - macdeployqt qt.conf exists in Resources/
#   - aaaseed_runtime.app is nested inside the Studio Resources/
#   - sample/starter.aaaproj.lua is bundled
#   - AAASeed.icns is bundled
#   - clean detach
#
# Signature verification (codesign --verify, spctl --assess) is a
# SEPARATE concern -- this script is the structural half.
#
# Exit codes:
#    0  success
#   64  EX_USAGE -- bad arguments
#   65  hdiutil verify failed
#   66  hdiutil attach failed
#   67  AAASeed Studio.app missing
#   68  Studio executable missing or not arm64
#   69  Qt framework missing
#   70  runtime binary missing
#   71  bundled assets missing (starter project / icon)
#   75  hdiutil detach failed
#
# Usage:  ./scripts/verify-qt-dmg.sh /path/to/AAASeed-Studio-X.Y.Z.dmg

set -euo pipefail

QUIET=0
while [[ $# -gt 0 ]]; do
    case "$1" in
        --quiet) QUIET=1; shift ;;
        -h|--help)
            sed -n '2,30p' "$0"
            exit 0 ;;
        *)  break ;;
    esac
done

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 [--quiet] /path/to/AAASeed-Studio-X.Y.Z.dmg" >&2
    exit 64
fi

DMG="$1"
say() { if [[ "$QUIET" == "0" ]]; then echo "[verify-qt-dmg] $*"; fi; }
die() { echo "[verify-qt-dmg] FAIL : $1" >&2; exit "$2"; }

if [[ ! -f "$DMG" ]]; then
    die "DMG not found : $DMG" 64
fi

# ── 1. checksum ───────────────────────────────────────────────────────────
say "1/8  hdiutil verify"
hdiutil verify "$DMG" >/dev/null 2>&1 \
    || die "hdiutil verify failed" 65

# ── 2. mount ──────────────────────────────────────────────────────────────
MOUNT_PT="$( mktemp -d -t aaaverify )"
say "2/8  mount → $MOUNT_PT"
hdiutil attach -nobrowse -readonly -mountpoint "$MOUNT_PT" "$DMG" >/dev/null 2>&1 \
    || die "hdiutil attach failed" 66
trap 'hdiutil detach "$MOUNT_PT" >/dev/null 2>&1 || true; rmdir "$MOUNT_PT" 2>/dev/null || true' EXIT

APP_PATH="$MOUNT_PT/AAASeed Studio.app"
EXE="$APP_PATH/Contents/MacOS/AAASeed-Studio"

# ── 3. app exists ─────────────────────────────────────────────────────────
say "3/8  AAASeed Studio.app present"
[[ -d "$APP_PATH" ]] || die "AAASeed Studio.app missing" 67

# ── 4. executable + arm64 slice ───────────────────────────────────────────
say "4/8  arm64 Mach-O present"
[[ -x "$EXE" ]] || die "Studio binary missing or not executable" 68
ARCHS="$( lipo -archs "$EXE" 2>/dev/null || file "$EXE" )"
echo "$ARCHS" | grep -q "arm64" \
    || die "Studio binary lacks arm64 slice : $ARCHS" 68

# ── 5. Qt frameworks bundled ──────────────────────────────────────────────
say "5/8  Qt frameworks bundled"
for fw in QtCore QtGui QtQml QtQuick QtMultimedia; do
    fpath="$APP_PATH/Contents/Frameworks/${fw}.framework"
    [[ -d "$fpath" ]] || die "$fw.framework missing from bundle" 69
done

# ── 6. qt.conf written by macdeployqt ─────────────────────────────────────
say "6/8  qt.conf landed"
[[ -f "$APP_PATH/Contents/Resources/qt.conf" ]] \
    || die "qt.conf missing from Resources" 69

# ── 7. nested runtime + starter project + icon ────────────────────────────
say "7/8  runtime app + starter project + icon"
RT_EXE="$APP_PATH/Contents/Resources/runtime/aaaseed_runtime.app/Contents/MacOS/aaaseed_runtime"
[[ -x "$RT_EXE" ]] || die "aaaseed_runtime not bundled inside Studio" 70
[[ -f "$APP_PATH/Contents/Resources/sample/starter.aaaproj.lua" ]] \
    || die "starter.aaaproj.lua missing" 71
[[ -f "$APP_PATH/Contents/Resources/AAASeed.icns" ]] \
    || die "AAASeed.icns missing" 71

# ── 8. detach ─────────────────────────────────────────────────────────────
say "8/8  detach"
hdiutil detach "$MOUNT_PT" >/dev/null 2>&1 \
    || die "hdiutil detach failed" 75
trap - EXIT
rmdir "$MOUNT_PT" 2>/dev/null || true

say "OK : DMG is structurally valid."
exit 0
