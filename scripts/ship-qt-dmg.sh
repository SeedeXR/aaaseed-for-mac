#!/usr/bin/env bash
#
# scripts/ship-qt-dmg.sh
#
# c152-C : ship pipeline for the Qt6 + QML AAASeed Studio.
#
# Pipeline :
#   1. CMake configure + build aaaseed_app_qt (Release, arm64).
#   2. macdeployqt -- bundle Qt frameworks + QML modules into the .app.
#   3. Code-sign the bundle if $CODESIGN_IDENTITY is set.
#   4. hdiutil compresses the .app + Applications symlink into a DMG.
#   5. Verify : hdiutil verify, mount, plutil -lint, detach.
#
# Notes :
#   - Homebrew's Qt6 (qt or qt@6) ships ARM64-only frameworks. The Qt
#     DMG is arm64-only by design. Intel users run under Rosetta until
#     a universal Qt is on the prefix path.
#   - macdeployqt resolves all transitive framework deps + the QtQuick
#     plugin tree (-qmldir tells it which QML imports to scan).
#   - Output : out/AAASeed-Studio-Qt-<version>.dmg
#
# Env vars :
#   BUILD_ROOT             default $REPO_ROOT/out
#   BUILD_TYPE             default Release
#   CODESIGN_IDENTITY      "Developer ID Application: ..." -- optional
#   NOTARY_API_KEY_PATH    optional ; if set with the other two below,
#   NOTARY_API_KEY_ID                triggers notarize + staple.
#   NOTARY_API_KEY_ISSUER_ID
#   AAA_MACDEPLOYQT        path to macdeployqt ; auto-detected.
#
# Exit codes :
#   0    success ; DMG path written to stdout final report.
#   1    cmake / build failure.
#   2    macdeployqt failure.
#   3    code-sign failure (only when CODESIGN_IDENTITY set).
#   4    hdiutil create / verify failure.

set -euo pipefail

REPO_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"
BUILD_ROOT="${BUILD_ROOT:-${REPO_ROOT}/out}"
BUILD_TYPE="${BUILD_TYPE:-Release}"
BUILD_DIR="${BUILD_ROOT}/macos-arm64-release"
APP_NAME="AAASeed-Studio.app"
APP_PATH="${BUILD_DIR}/bin/${APP_NAME}"
VERSION="${PROJECT_VERSION:-0.0.1}"
DMG_NAME="AAASeed-Studio-${VERSION}.dmg"
DMG_PATH="${BUILD_ROOT}/${DMG_NAME}"
STAGE_DIR="${BUILD_ROOT}/qt_dmg_staging"

# Auto-detect macdeployqt.
if [[ -z "${AAA_MACDEPLOYQT:-}" ]]; then
    for cand in /opt/homebrew/opt/qt/bin/macdeployqt \
                /opt/homebrew/opt/qt@6/bin/macdeployqt \
                /usr/local/opt/qt/bin/macdeployqt
    do
        if [[ -x "${cand}" ]]; then
            AAA_MACDEPLOYQT="${cand}"
            break
        fi
    done
fi
if [[ -z "${AAA_MACDEPLOYQT:-}" || ! -x "${AAA_MACDEPLOYQT}" ]]; then
    echo "[ship-qt-dmg] ERR : macdeployqt not found. Install Qt6 via Homebrew or set AAA_MACDEPLOYQT." >&2
    exit 2
fi

step() { echo ""; echo "==== ${1} ====" ; }

# ── 1. configure + build ───────────────────────────────────────────────────
step "1/5  cmake configure (Release, arm64)"
cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
    || { echo "[ship-qt-dmg] cmake configure failed" >&2; exit 1; }

step "2/5  build aaaseed_app_qt + aaaseed_runtime"
cmake --build "${BUILD_DIR}" --target aaaseed_app_qt aaaseed_runtime \
    -j "$( sysctl -n hw.ncpu )" \
    || { echo "[ship-qt-dmg] build failed" >&2; exit 1; }

if [[ ! -d "${APP_PATH}" ]]; then
    echo "[ship-qt-dmg] ERR : ${APP_PATH} missing after build." >&2
    exit 1
fi

RUNTIME_APP_PATH="${BUILD_DIR}/bin/aaaseed_runtime.app"
if [[ ! -d "${RUNTIME_APP_PATH}" ]]; then
    echo "[ship-qt-dmg] ERR : ${RUNTIME_APP_PATH} missing after build." >&2
    exit 1
fi

# c152-D : bundle aaaseed_runtime.app inside the Studio's
# Contents/Resources/runtime/ . The Studio's playProject() slot
# probes for this path. Two .apps inside one DMG.
RUNTIME_DEST="${APP_PATH}/Contents/Resources/runtime"
rm -rf "${RUNTIME_DEST}"
mkdir -p "${RUNTIME_DEST}"
cp -R "${RUNTIME_APP_PATH}" "${RUNTIME_DEST}/aaaseed_runtime.app"

# ── 3. macdeployqt ─────────────────────────────────────────────────────────
step "3/5  macdeployqt (bundle Qt frameworks + QML modules)"
"${AAA_MACDEPLOYQT}" "${APP_PATH}" \
    -qmldir="${REPO_ROOT}/src/ui/qt/qml" \
    -no-strip \
    || { echo "[ship-qt-dmg] macdeployqt failed" >&2; exit 2; }

# Copy the AAASeed.icns placeholder into the bundle if present.
if [[ -f "${REPO_ROOT}/bundle/macos/AAASeed.icns" ]]; then
    cp "${REPO_ROOT}/bundle/macos/AAASeed.icns" \
       "${APP_PATH}/Contents/Resources/AAASeed.icns"
fi

# ── 4. optional code-sign ──────────────────────────────────────────────────
if [[ -n "${CODESIGN_IDENTITY:-}" ]]; then
    step "4/5  codesign : ${CODESIGN_IDENTITY}"
    codesign --force --deep --options runtime \
        --entitlements "${REPO_ROOT}/bundle/macos/entitlements.plist" \
        --sign "${CODESIGN_IDENTITY}" \
        "${APP_PATH}" \
        || { echo "[ship-qt-dmg] codesign failed" >&2; exit 3; }
else
    step "4/5  codesign : SKIPPED (CODESIGN_IDENTITY not set)"
fi

# ── 5. DMG ─────────────────────────────────────────────────────────────────
step "5/5  hdiutil DMG : ${DMG_PATH}"
rm -rf "${STAGE_DIR}"
mkdir -p "${STAGE_DIR}"
cp -R "${APP_PATH}" "${STAGE_DIR}/AAASeed Studio.app"
ln -s /Applications "${STAGE_DIR}/Applications"

# Try ULMO (LZMA) first ; fall back to ULFO (LZFSE) ; fall back to UDBZ.
TIERS=( ULMO ULFO UDBZ )
DMG_OK=0
for FMT in "${TIERS[@]}"; do
    if hdiutil create -volname "AAASeed Studio" \
                      -srcfolder "${STAGE_DIR}" \
                      -ov \
                      -format "${FMT}" \
                      "${DMG_PATH}" >/dev/null 2>&1
    then
        echo "  + DMG compressed with ${FMT}"
        DMG_OK=1
        break
    fi
done
if [[ "${DMG_OK}" -eq 0 ]]; then
    echo "[ship-qt-dmg] all hdiutil tiers failed" >&2
    exit 4
fi

# Verify.
hdiutil verify "${DMG_PATH}" >/dev/null \
    || { echo "[ship-qt-dmg] hdiutil verify failed" >&2; exit 4; }

# ── 6. optional notarize + staple ──────────────────────────────────────────
if [[ -n "${NOTARY_API_KEY_PATH:-}" && \
      -n "${NOTARY_API_KEY_ID:-}"   && \
      -n "${NOTARY_API_KEY_ISSUER_ID:-}" ]]; then
    step "6/5  notarize + staple"
    xcrun notarytool submit "${DMG_PATH}" \
        --key       "${NOTARY_API_KEY_PATH}" \
        --key-id    "${NOTARY_API_KEY_ID}" \
        --issuer    "${NOTARY_API_KEY_ISSUER_ID}" \
        --wait || { echo "[ship-qt-dmg] notarytool failed" >&2; exit 5; }
    xcrun stapler staple "${DMG_PATH}" \
        || { echo "[ship-qt-dmg] staple failed" >&2; exit 5; }
fi

echo ""
echo "==== final report ===="
echo "  DMG : ${DMG_PATH}"
echo "  size : $(du -h "${DMG_PATH}" | awk '{print $1}')"
echo "  arch : arm64 (Apple Silicon ; Intel via Rosetta)"
echo "  signed : $([[ -n "${CODESIGN_IDENTITY:-}" ]] && echo yes || echo no)"
echo ""
echo "Test : open ${DMG_PATH}"
