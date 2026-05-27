#!/usr/bin/env bash
#
# scripts/verify-dmg.sh
#
# Phase 8 -- one-script post-receipt verification for AAASeed-X.Y.Z.dmg.
# The inverse of `scripts/ship-dmg.sh` : where ship-dmg.sh BUILDS the
# DMG, verify-dmg.sh CONFIRMS a received DMG is structurally valid
# (downloaded over the web, copied off a USB stick, attached to an
# email -- whichever channel the user got it through). Produced by
# continuation c145 alongside SHIP_CHECKLIST.md.
#
# This is STRUCTURAL verification only :
#   - hdiutil verify (DMG checksum + filesystem integrity).
#   - mount read-only, no Finder browse.
#   - presence of AAASeed.app inside.
#   - Applications symlink for drag-and-drop install.
#   - the bundle executable is present + executable.
#   - the executable is a Mach-O universal binary.
#   - both arm64 AND x86_64 slices are present (universal-binary mandate).
#   - the Info.plist passes plutil -lint.
#   - the MEU runner has its bundled hello_world.lua script.
#   - the shader catalog has at least 100 .metal files.
#   - clean detach.
#
# Signature verification (codesign --verify, spctl --assess) is a
# SEPARATE concern handled by Gatekeeper / future v1.1 ship pipelines.
# This script is the structural half ; do not add `codesign` to it.
# A regression-guard test in tests/unit/verify_dmg_script_test.cpp
# enforces the separation.
#
# Exit codes (each step has a distinct code so callers can branch) :
#    0  success.
#   64  EX_USAGE -- bad arguments.
#   65  hdiutil verify failed.
#   66  hdiutil attach failed.
#   67  AAASeed.app missing inside DMG.
#   68  Applications symlink missing or wrong target.
#   69  executable missing or not executable.
#   70  executable is not a Mach-O universal binary.
#   71  executable missing arm64 or x86_64 slice.
#   72  Info.plist failed plutil -lint.
#   73  meu/hello_world.lua missing from bundle Resources.
#   74  shader catalog has fewer than 100 .metal files.
#   75  hdiutil detach failed (rare, post-success cleanup).
#
# Flags :
#   --quiet    suppress per-step progress lines ; errors still print.
#
# Usage :
#   ./scripts/verify-dmg.sh out/AAASeed-0.0.1.dmg
#   ./scripts/verify-dmg.sh --quiet out/AAASeed-0.0.1.dmg

set -euo pipefail

# --------- argument parsing ----------------------------------------------

QUIET="no"
DMG_PATH=""

for arg in "$@"; do
    case "${arg}" in
        --quiet)  QUIET="yes" ;;
        -h|--help)
            cat <<EOF
Usage : $(basename "$0") [--quiet] <path-to-dmg>

Structural verification for AAASeed-X.Y.Z.dmg. Exits 0 on success,
non-zero with a distinct code per failure (see top of script).
EOF
            exit 0
            ;;
        *)
            if [[ -z "${DMG_PATH}" ]]; then
                DMG_PATH="${arg}"
            else
                printf '[verify-dmg ERROR] unexpected extra argument : %s\n' "${arg}" >&2
                exit 64
            fi
            ;;
    esac
done

# --------- helpers -------------------------------------------------------

progress() {
    if [[ "${QUIET}" == "no" ]]; then
        printf '[verify-dmg] %s\n' "$*"
    fi
}

err() {
    printf '[verify-dmg ERROR] %s\n' "$*" >&2
}

# --------- step 1 : validate the argument --------------------------------

if [[ -z "${DMG_PATH}" ]]; then
    err "missing DMG path."
    err "Usage : $(basename "$0") [--quiet] <path-to-dmg>"
    exit 64
fi

if [[ ! -r "${DMG_PATH}" ]]; then
    err "DMG path not readable : ${DMG_PATH}"
    exit 64
fi

if [[ ! -f "${DMG_PATH}" ]]; then
    err "DMG path is not a regular file : ${DMG_PATH}"
    exit 64
fi

case "${DMG_PATH}" in
    *.dmg) ;;
    *)
        err "DMG path must end in .dmg : ${DMG_PATH}"
        exit 64
        ;;
esac

DMG_BASENAME="$(basename "${DMG_PATH}")"
progress "verifying ${DMG_BASENAME}"

# --------- step 2 : hdiutil verify ---------------------------------------

progress "step 2 : hdiutil verify"
if ! hdiutil verify "${DMG_PATH}" >/dev/null 2>&1; then
    err "hdiutil verify FAILED on ${DMG_PATH}"
    err "DMG is corrupt or its internal checksum no longer matches."
    exit 65
fi

# --------- step 3 : hdiutil attach (mount read-only, no browse) ---------

progress "step 3 : hdiutil attach (read-only, nobrowse)"
MOUNT_DIR="$(mktemp -d -t aaaseed_verify_mount)"

# Once the mount succeeds we install a trap that detaches it on ANY
# subsequent exit, so failures from steps 5-12 do not leak a mount.
detach_mount() {
    if [[ -n "${MOUNTED:-}" ]]; then
        hdiutil detach "${MOUNT_DIR}" >/dev/null 2>&1 || true
    fi
    rmdir "${MOUNT_DIR}" 2>/dev/null || true
}

if ! hdiutil attach -nobrowse -readonly -mountpoint "${MOUNT_DIR}" \
        "${DMG_PATH}" >/dev/null 2>&1; then
    err "hdiutil attach FAILED on ${DMG_PATH}"
    err "Could not mount the DMG read-only."
    rmdir "${MOUNT_DIR}" 2>/dev/null || true
    exit 66
fi

MOUNTED="yes"
trap detach_mount EXIT

# --------- step 4 : AAASeed.app present ---------------------------------

APP_PATH="${MOUNT_DIR}/AAASeed.app"
progress "step 4 : AAASeed.app present"
if [[ ! -d "${APP_PATH}" ]]; then
    err "AAASeed.app missing inside DMG at ${APP_PATH}"
    exit 67
fi

# --------- step 5 : Applications symlink --------------------------------

APPLINK="${MOUNT_DIR}/Applications"
progress "step 5 : Applications symlink -> /Applications"
if [[ ! -L "${APPLINK}" ]]; then
    err "Applications shortcut missing or not a symlink at ${APPLINK}"
    err "Drag-and-drop install relies on this symlink."
    exit 68
fi
APPLINK_TARGET="$(readlink "${APPLINK}")"
if [[ "${APPLINK_TARGET}" != "/Applications" ]]; then
    err "Applications symlink points to '${APPLINK_TARGET}', expected '/Applications'."
    exit 68
fi

# --------- step 6 : executable present + executable bit -----------------

EXE_PATH="${APP_PATH}/Contents/MacOS/aaaseed_app"
progress "step 6 : Contents/MacOS/aaaseed_app is executable"
if [[ ! -x "${EXE_PATH}" ]]; then
    err "Bundle executable missing or not executable : ${EXE_PATH}"
    exit 69
fi

# --------- step 7 : Mach-O universal binary -----------------------------

progress "step 7 : file(1) reports Mach-O universal binary"
FILE_INFO="$(file -b "${EXE_PATH}")"
if [[ "${FILE_INFO}" != *"Mach-O universal binary"* ]]; then
    err "Executable is NOT a Mach-O universal binary."
    err "file reported : ${FILE_INFO}"
    exit 70
fi

# --------- step 8 : lipo -info has arm64 AND x86_64 ---------------------

progress "step 8 : lipo -info reports arm64 AND x86_64"
LIPO_INFO="$(lipo -info "${EXE_PATH}" 2>&1)"
if [[ "${LIPO_INFO}" != *"arm64"* ]]; then
    err "lipo -info missing arm64 slice."
    err "lipo reported : ${LIPO_INFO}"
    exit 71
fi
if [[ "${LIPO_INFO}" != *"x86_64"* ]]; then
    err "lipo -info missing x86_64 slice."
    err "lipo reported : ${LIPO_INFO}"
    exit 71
fi

# --------- step 9 : plutil -lint Info.plist -----------------------------

PLIST_PATH="${APP_PATH}/Contents/Info.plist"
progress "step 9 : plutil -lint Info.plist"
if [[ ! -f "${PLIST_PATH}" ]]; then
    err "Info.plist missing at ${PLIST_PATH}"
    exit 72
fi
if ! plutil -lint "${PLIST_PATH}" >/dev/null 2>&1; then
    err "plutil -lint reported issues on ${PLIST_PATH}"
    exit 72
fi

# --------- step 10 : meu/hello_world.lua present ------------------------

LUA_PATH="${APP_PATH}/Contents/Resources/meu/hello_world.lua"
progress "step 10 : Resources/meu/hello_world.lua present"
if [[ ! -f "${LUA_PATH}" ]]; then
    err "MEU Lua bundle missing : ${LUA_PATH}"
    exit 73
fi

# --------- step 11 : shader catalog has >= 100 .metal files -------------

SHADERS_DIR="${APP_PATH}/Contents/Resources/shaders"
progress "step 11 : Resources/shaders/ has at least 100 .metal files"
if [[ ! -d "${SHADERS_DIR}" ]]; then
    err "Shader catalog directory missing : ${SHADERS_DIR}"
    exit 74
fi
# `find ... | wc -l` is portable across BSD + GNU find. -maxdepth 1 keeps
# the count to the top level of the shaders dir (subdirs out of scope).
SHADER_COUNT="$(find "${SHADERS_DIR}" -maxdepth 1 -type f -name '*.metal' 2>/dev/null | wc -l | tr -d ' ')"
if [[ "${SHADER_COUNT}" -lt 100 ]]; then
    err "Shader catalog has only ${SHADER_COUNT} .metal files (expected at least 100)."
    exit 74
fi

# --------- step 12 : detach ---------------------------------------------

progress "step 12 : hdiutil detach"
if ! hdiutil detach "${MOUNT_DIR}" >/dev/null 2>&1; then
    err "hdiutil detach FAILED on ${MOUNT_DIR}"
    err "Mount may still be present ; run 'hdiutil detach ${MOUNT_DIR}' manually."
    MOUNTED=""   # prevent the trap from retrying
    exit 75
fi
MOUNTED=""
rmdir "${MOUNT_DIR}" 2>/dev/null || true
trap - EXIT

# --------- success report ------------------------------------------------

printf 'OK : %s verified (universal binary, %s shaders, MEU runner present).\n' \
    "${DMG_BASENAME}" "${SHADER_COUNT}"
exit 0
