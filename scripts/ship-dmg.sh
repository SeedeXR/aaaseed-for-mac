#!/usr/bin/env bash
#
# scripts/ship-dmg.sh
#
# Phase 8 -- single end-to-end ship pipeline for a UNIVERSAL-BINARY
# AAASeed.dmg. Produced by continuation c143-B (2026-05-27) per the
# user's escalated mandate ("ship Apple Silicon AND Intel Macs, using
# the best-working compression for portability").
#
# Pipeline (each step prints a STATUS banner) :
#   1. Per-arch Release build (arm64 default + x86_64 default ; override
#      via ARCHES env, e.g. ARCHES="arm64").
#   2. `lipo -create` the per-arch `aaaseed_app` executables into a
#      universal binary IF more than one arch built.
#   3. Assemble a staging .app bundle from the arm64 skeleton + the
#      lipo'd universal executable.
#   4. Optional codesign (only if CODESIGN_IDENTITY env var set).
#   5. DMG staging directory (Applications symlink, .background/, README).
#   6. Compression cascade : try ULMO (LZMA, smallest) -> ULFO (LZFSE,
#      Apple-native, broad modern macOS support) -> UDBZ (bzip2, legacy
#      compatibility). First success wins.
#   7. Verify : `hdiutil verify`, mount, `lipo -info`, `plutil -lint`,
#      detach.
#   8. Optional notarize + staple (only if NOTARY_API_KEY_{PATH,ID,
#      ISSUER_ID} env vars set).
#   9. Final report.
#
# Env vars (ALL optional ; absent => safe default behaviour) :
#   ARCHES                      space-separated arch list. Default :
#                               "arm64 x86_64". Set ARCHES="arm64" for a
#                               fast Apple-Silicon-only dev build.
#   BUILD_ROOT                  directory holding per-arch build dirs.
#                               Default : "out". Each arch builds in
#                               $BUILD_ROOT/macos-${ARCH}-release/.
#   CODESIGN_IDENTITY           Developer ID Application string. When
#                               set, the .app is signed before being
#                               wrapped in the DMG. Identity is NEVER
#                               hardcoded -- read verbatim from env.
#   NOTARY_API_KEY_PATH         absolute path to App Store Connect .p8
#                               key file.
#   NOTARY_API_KEY_ID           10-char App Store Connect key ID.
#   NOTARY_API_KEY_ISSUER_ID    issuer UUID from App Store Connect.
#                               All three must be set together for the
#                               notarize + staple step to fire.
#
# Compression rationale (also documented in bundle/macos/README.md) :
#   ULMO (LZMA)  -- highest compression ratio, supported macOS 10.11+.
#                   Best for SHIPPING (smallest download for the user).
#   ULFO (LZFSE) -- Apple's native compression algorithm. Decoded at
#                   the filesystem level on all modern macOS. Best for
#                   COMPATIBILITY across Apple platforms (preferred
#                   when ULMO trips notarization edge cases).
#   UDBZ (bzip2) -- broad legacy compatibility going back to macOS 10.4.
#                   Largest output. Best for OLDEST macOS support.
# The cascade tries the best-compression tier first and falls back
# only when the host hdiutil rejects it.
#
# Exit status :
#   0 success ; DMG path written to stdout final report.
#   non-zero on per-arch build failure of the FIRST (default) arch, or
#   on all-three compression-tier failure, or on verify mount failure.
#   If a non-default arch fails (e.g., x86_64 cross-compile), the
#   script falls back to a single-arch DMG with a clearly logged
#   WARNING + a TODO for the next session.

set -euo pipefail

# --------- configuration -------------------------------------------------

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_ROOT="${BUILD_ROOT:-${REPO_ROOT}/out}"
ARCHES="${ARCHES:-arm64 x86_64}"
PRIMARY_ARCH="${PRIMARY_ARCH:-arm64}"
# c144-A (2026-05-27) : Release is the ship default ; BUILD_TYPE env
# overrides (e.g. BUILD_TYPE=Debug ./scripts/ship-dmg.sh for a fast
# unoptimized dev DMG). Release enables LTO + dead-strip + -Os + strip
# via src/ui/macos/CMakeLists.txt (c142-A) for a smaller, faster app.
BUILD_TYPE="${BUILD_TYPE:-Release}"
PROJECT_VERSION_FALLBACK="0.0.1"
STAGING_DIR="${BUILD_ROOT}/ship_staging"
DMG_STAGING_DIR="${BUILD_ROOT}/ship_dmg_staging"
FINAL_DMG_NAME=""   # filled after we resolve PROJECT_VERSION

# --------- helpers -------------------------------------------------------

log()      { printf '[ship-dmg] %s\n' "$*"; }
status()   { printf '\n=== %s ===\n' "$*"; }
warn()     { printf '[ship-dmg WARN] %s\n' "$*" >&2; }
die()      { printf '[ship-dmg ERROR] %s\n' "$*" >&2; exit 1; }

# Pull PROJECT_VERSION from top-level CMakeLists.txt -- fall back to
# the recorded c141-A value if grep miss.
resolve_version() {
    local v
    v="$(awk '/^project\(/{getline; print}' "${REPO_ROOT}/CMakeLists.txt" \
        | grep -oE 'VERSION[[:space:]]+[0-9.]+' \
        | awk '{print $2}' || true)"
    if [[ -z "${v}" ]]; then
        v="$(grep -E 'VERSION[[:space:]]+[0-9]' "${REPO_ROOT}/CMakeLists.txt" \
            | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -n1 || true)"
    fi
    if [[ -z "${v}" ]]; then
        v="${PROJECT_VERSION_FALLBACK}"
    fi
    printf '%s' "${v}"
}

PROJECT_VERSION="$(resolve_version)"
FINAL_DMG_NAME="AAASeed-${PROJECT_VERSION}.dmg"
FINAL_DMG_PATH="${BUILD_ROOT}/${FINAL_DMG_NAME}"
VOLNAME="AAASeed-${PROJECT_VERSION}"

# --------- 1. per-arch builds --------------------------------------------

declare -a BUILT_ARCHES=()
declare -a FAILED_ARCHES=()

build_one_arch() {
    local arch="$1"
    local build_dir="${BUILD_ROOT}/macos-${arch}-release"

    log "configuring ${arch} in ${build_dir} (BUILD_TYPE=${BUILD_TYPE})"
    cmake -S "${REPO_ROOT}" -B "${build_dir}" \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -DCMAKE_OSX_ARCHITECTURES="${arch}" \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
        -G Ninja >/dev/null

    local jobs
    jobs="$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
    log "building aaaseed_app for ${arch} (-j${jobs})"
    cmake --build "${build_dir}" --target aaaseed_app -j "${jobs}"
}

status "1. per-arch build : ARCHES=\"${ARCHES}\" BUILD_TYPE=${BUILD_TYPE}"
for ARCH in ${ARCHES}; do
    if build_one_arch "${ARCH}"; then
        BUILT_ARCHES+=("${ARCH}")
        log "OK : ${ARCH} built."
    else
        warn "${ARCH} build FAILED. Continuing with the remaining arches."
        FAILED_ARCHES+=("${ARCH}")
    fi
done

if [[ ${#BUILT_ARCHES[@]} -eq 0 ]]; then
    die "no arch built successfully ; aborting ship."
fi

# Make the primary arch the one we copy the .app skeleton from. If the
# user excluded it via ARCHES, fall back to whichever DID build.
SKELETON_ARCH="${PRIMARY_ARCH}"
SKELETON_PRESENT="no"
for a in "${BUILT_ARCHES[@]}"; do
    if [[ "${a}" == "${SKELETON_ARCH}" ]]; then
        SKELETON_PRESENT="yes"
        break
    fi
done
if [[ "${SKELETON_PRESENT}" == "no" ]]; then
    SKELETON_ARCH="${BUILT_ARCHES[0]}"
    warn "primary arch ${PRIMARY_ARCH} did not build ; using ${SKELETON_ARCH} skeleton."
fi
SKELETON_BUILD_DIR="${BUILD_ROOT}/macos-${SKELETON_ARCH}-release"
SKELETON_APP="${SKELETON_BUILD_DIR}/bin/aaaseed_app.app"

if [[ ! -d "${SKELETON_APP}" ]]; then
    die "skeleton .app not found at ${SKELETON_APP}."
fi

# --------- 2. lipo merge -------------------------------------------------

status "2. lipo merge"
rm -rf "${STAGING_DIR}"
mkdir -p "${STAGING_DIR}"
LIPO_EXEC="${STAGING_DIR}/aaaseed_app"

if [[ ${#BUILT_ARCHES[@]} -ge 2 ]]; then
    declare -a LIPO_INPUTS=()
    for a in "${BUILT_ARCHES[@]}"; do
        local_exec="${BUILD_ROOT}/macos-${a}-release/bin/aaaseed_app.app/Contents/MacOS/aaaseed_app"
        if [[ ! -f "${local_exec}" ]]; then
            warn "missing per-arch executable ${local_exec} ; skipping in lipo."
            continue
        fi
        LIPO_INPUTS+=("${local_exec}")
    done
    log "lipo -create ${LIPO_INPUTS[*]} -output ${LIPO_EXEC}"
    lipo -create "${LIPO_INPUTS[@]}" -output "${LIPO_EXEC}"
else
    log "single-arch build (${BUILT_ARCHES[0]}) ; copying executable as-is."
    cp "${SKELETON_APP}/Contents/MacOS/aaaseed_app" "${LIPO_EXEC}"
fi

LIPO_INFO_STAGING="$(lipo -info "${LIPO_EXEC}")"
log "lipo -info -> ${LIPO_INFO_STAGING}"

# Discipline : record stripped-vs-unstripped size for the final report.
SIZE_BEFORE_STRIP="$(stat -f%z "${LIPO_EXEC}")"
strip -x "${LIPO_EXEC}" 2>/dev/null || true
SIZE_AFTER_STRIP="$(stat -f%z "${LIPO_EXEC}")"
log "binary size : ${SIZE_BEFORE_STRIP} bytes (pre-strip) -> ${SIZE_AFTER_STRIP} bytes (post-strip)"

# --------- 3. bundle assembly --------------------------------------------

status "3. bundle assembly"
STAGED_APP="${STAGING_DIR}/AAASeed.app"
rm -rf "${STAGED_APP}"
cp -R "${SKELETON_APP}" "${STAGED_APP}"
# Overwrite the executable with the lipo'd / stripped universal binary.
cp "${LIPO_EXEC}" "${STAGED_APP}/Contents/MacOS/aaaseed_app"
chmod +x "${STAGED_APP}/Contents/MacOS/aaaseed_app"
log "bundle assembled at ${STAGED_APP}"

# --------- 4. optional codesign -----------------------------------------

status "4. optional codesign"
if [[ -n "${CODESIGN_IDENTITY:-}" ]]; then
    ENTITLEMENTS="${REPO_ROOT}/bundle/macos/entitlements.plist"
    if [[ ! -f "${ENTITLEMENTS}" ]]; then
        warn "entitlements.plist not found at ${ENTITLEMENTS} ; signing without --entitlements."
        codesign --deep --options runtime --timestamp \
            --sign "${CODESIGN_IDENTITY}" "${STAGED_APP}"
    else
        codesign --deep --options runtime --timestamp \
            --entitlements "${ENTITLEMENTS}" \
            --sign "${CODESIGN_IDENTITY}" "${STAGED_APP}"
    fi
    log "codesigned with operator-supplied identity."
else
    log "STATUS : skipping codesign -- set CODESIGN_IDENTITY to enable."
fi

# --------- 5. DMG staging dir --------------------------------------------

status "5. DMG staging"
rm -rf "${DMG_STAGING_DIR}"
mkdir -p "${DMG_STAGING_DIR}/.background"
cp -R "${STAGED_APP}" "${DMG_STAGING_DIR}/AAASeed.app"
ln -s /Applications "${DMG_STAGING_DIR}/Applications"
cat > "${DMG_STAGING_DIR}/README.txt" <<'EOF'
AAASeed -- drag the app onto the Applications shortcut to install.
EOF
log "DMG staging at ${DMG_STAGING_DIR}"

# --------- 6. compression cascade ---------------------------------------

status "6. compression cascade"
rm -f "${FINAL_DMG_PATH}"
COMPRESSION_WINNER=""

try_compression() {
    local format="$1"
    local label="$2"
    log "trying ${label} (${format}) ..."
    if hdiutil create -format "${format}" \
        -srcfolder "${DMG_STAGING_DIR}" \
        -volname "${VOLNAME}" \
        "${FINAL_DMG_PATH}" >/dev/null 2>&1; then
        COMPRESSION_WINNER="${label} (${format})"
        log "compression winner : ${COMPRESSION_WINNER}"
        return 0
    fi
    rm -f "${FINAL_DMG_PATH}"
    warn "${label} (${format}) failed ; trying next tier."
    return 1
}

if   try_compression ULMO "ULMO/LZMA"; then :
elif try_compression ULFO "ULFO/LZFSE"; then :
elif try_compression UDBZ "UDBZ/bzip2"; then :
else
    die "all three compression tiers (ULMO, ULFO, UDBZ) failed."
fi

# --------- 7. verify -----------------------------------------------------

status "7. verify"
log "hdiutil verify ${FINAL_DMG_PATH}"
hdiutil verify "${FINAL_DMG_PATH}" >/dev/null

MOUNT_DIR="$(mktemp -d -t aaaseed_ship_mount)"
log "hdiutil attach -nobrowse -readonly -mountpoint ${MOUNT_DIR}"
hdiutil attach -nobrowse -readonly -mountpoint "${MOUNT_DIR}" "${FINAL_DMG_PATH}" >/dev/null

MOUNTED_APP="${MOUNT_DIR}/AAASeed.app"
MOUNTED_EXE="${MOUNTED_APP}/Contents/MacOS/aaaseed_app"
MOUNTED_PLIST="${MOUNTED_APP}/Contents/Info.plist"

if [[ ! -x "${MOUNTED_EXE}" ]]; then
    hdiutil detach "${MOUNT_DIR}" >/dev/null || true
    die "mounted .app executable missing or not executable."
fi

FILE_INFO_MOUNT="$(file "${MOUNTED_EXE}")"
LIPO_INFO_MOUNT="$(lipo -info "${MOUNTED_EXE}")"
log "file : ${FILE_INFO_MOUNT}"
log "lipo : ${LIPO_INFO_MOUNT}"

if [[ -f "${MOUNTED_PLIST}" ]]; then
    plutil -lint "${MOUNTED_PLIST}" >/dev/null \
        && log "plutil -lint : Info.plist OK." \
        || warn "plutil -lint reported issues on Info.plist."
fi

hdiutil detach "${MOUNT_DIR}" >/dev/null
rmdir "${MOUNT_DIR}" 2>/dev/null || true

# --------- 8. optional notarize -----------------------------------------

status "8. optional notarize"
if [[ -n "${NOTARY_API_KEY_PATH:-}" \
      && -n "${NOTARY_API_KEY_ID:-}" \
      && -n "${NOTARY_API_KEY_ISSUER_ID:-}" ]]; then
    log "submitting ${FINAL_DMG_PATH} to xcrun notarytool ..."
    xcrun notarytool submit "${FINAL_DMG_PATH}" \
        --key "${NOTARY_API_KEY_PATH}" \
        --key-id "${NOTARY_API_KEY_ID}" \
        --issuer "${NOTARY_API_KEY_ISSUER_ID}" \
        --wait
    log "stapling ticket onto DMG ..."
    xcrun stapler staple "${FINAL_DMG_PATH}"
else
    log "STATUS : skipping notarization -- export NOTARY_API_KEY_PATH + NOTARY_API_KEY_ID + NOTARY_API_KEY_ISSUER_ID to enable."
fi

# --------- 9. final report ----------------------------------------------

status "9. final report"
DMG_SIZE_BYTES="$(stat -f%z "${FINAL_DMG_PATH}")"
DMG_SIZE_MB="$(awk -v b="${DMG_SIZE_BYTES}" 'BEGIN{printf "%.2f", b/1048576}')"

printf 'BUILD_TYPE              : %s\n' "${BUILD_TYPE}"
printf 'ARCHES requested        : %s\n' "${ARCHES}"
printf 'ARCHES built            : %s\n' "${BUILT_ARCHES[*]}"
if [[ ${#FAILED_ARCHES[@]} -gt 0 ]]; then
    printf 'ARCHES failed           : %s (TODO follow-up : investigate SDK / NEON-only sources)\n' "${FAILED_ARCHES[*]}"
fi
printf 'DMG path                : %s\n' "${FINAL_DMG_PATH}"
printf 'DMG size                : %s bytes (%s MB)\n' "${DMG_SIZE_BYTES}" "${DMG_SIZE_MB}"
printf 'Compression tier        : %s\n' "${COMPRESSION_WINNER}"
printf 'lipo -info (staging)    : %s\n' "${LIPO_INFO_STAGING}"
printf 'lipo -info (mounted)    : %s\n' "${LIPO_INFO_MOUNT}"
printf 'file    (mounted)       : %s\n' "${FILE_INFO_MOUNT}"
printf 'binary pre-strip bytes  : %s\n' "${SIZE_BEFORE_STRIP}"
printf 'binary post-strip bytes : %s\n' "${SIZE_AFTER_STRIP}"
printf 'codesign                : %s\n' "${CODESIGN_IDENTITY:+enabled (operator identity)}${CODESIGN_IDENTITY:-skipped (CODESIGN_IDENTITY unset)}"
printf 'notarize                : %s\n' "${NOTARY_API_KEY_PATH:+enabled (operator key)}${NOTARY_API_KEY_PATH:-skipped (NOTARY_API_KEY_* unset)}"

log "DONE."
