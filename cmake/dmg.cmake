# cmake/dmg.cmake
#
# Phase 8 distribution -- DMG packaging target for AAASeed.app.
# Continuation 137-B (2026-05-27, c137-B). Extends the c136-B
# beachhead (bundle/macos/{Info.plist, entitlements.plist, README.md}
# + src/macos/aaa_bundle_meta.{h,mm}) with a CMake custom_target that
# stages the built .app bundle into a DMG-friendly directory layout
# and invokes `hdiutil create` to produce an unsigned AAASeed-<ver>.dmg.
#
# NO actual signing or notarization happens here -- those require an
# Apple Developer ID and live in `bundle/macos/README.md` as a runbook.
# This target produces an UNSIGNED, NON-NOTARIZED DMG suitable for
# local smoke tests / ad-hoc distribution / future signing pipelines.
#
# todo.md L379 reference :
#   hdiutil create -format ULMO -srcfolder dmg_staging/ \
#       -volname "AAASeed" AAASeed-<version>.dmg
#
# Fallback -format ULFO if ULMO has notarization friction
# (todo.md L379 note ; ULFO is older but more broadly accepted).
#
# Usage :
#   cmake --build out/macos-arm64-debug --target aaaseed_dmg
# Output : ${CMAKE_BINARY_DIR}/AAASeed-${PROJECT_VERSION}.dmg
#
# Staging layout (in ${CMAKE_BINARY_DIR}/dmg_staging/) :
#   AAASeed.app             -- copied from $<TARGET_BUNDLE_DIR:aaaseed_app>
#   Applications            -- symlink to /Applications (drag-to-install)
#   .background/            -- placeholder dir for future DS_Store backdrop
#   README.txt              -- placeholder one-liner (NOT the runbook)
#
# Future-icon slot : a `.VolumeIcon.icns` at the staging root will be
# picked up by Finder when present ; left absent until art ships.

if(NOT APPLE)
    message(STATUS "aaaseed_dmg : non-Apple platform ; DMG target skipped.")
    return()
endif()

if(NOT TARGET aaaseed_app)
    message(WARNING "aaaseed_dmg : aaaseed_app target not yet defined ; "
                    "include(cmake/dmg.cmake) AFTER add_subdirectory(src/ui/macos).")
    return()
endif()

set(AAASEED_DMG_STAGING_DIR "${CMAKE_BINARY_DIR}/dmg_staging")
set(AAASEED_DMG_OUTPUT      "${CMAKE_BINARY_DIR}/AAASeed-${PROJECT_VERSION}.dmg")
set(AAASEED_DMG_VOLNAME     "AAASeed-${PROJECT_VERSION}")
set(AAASEED_DMG_ASSETS_DIR  "${CMAKE_SOURCE_DIR}/cmake/dmg_assets")

#  Custom command that does the full stage -> symlink -> hdiutil flow.
#  We chain everything into a single add_custom_target so CMake sees
#  one logical "produce the DMG" action ; intermediate files live in
#  dmg_staging/ and are removed+recreated on every invocation (cheap).
#
#  Trying -format ULMO first (LZMA, smallest ; todo.md L379 preferred) ;
#  on failure we retry with -format ULFO (LZFSE, notarization-friendlier).
#  The shell `||` fallback keeps the build target green when ULMO is
#  rejected by older hdiutil versions or unsupported on some hosts.
add_custom_target(aaaseed_dmg
    DEPENDS aaaseed_app

    #  1. Clean any prior staging dir (idempotent rebuilds).
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${AAASEED_DMG_STAGING_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${AAASEED_DMG_STAGING_DIR}"

    #  2. Copy the built AAASeed.app bundle into staging. The target's
    #     bundle dir generator-expression resolves at build time to e.g.
    #     out/macos-arm64-debug/bin/aaaseed_app.app . We copy it under
    #     the canonical user-facing name "AAASeed.app".
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_BUNDLE_DIR:aaaseed_app>"
        "${AAASEED_DMG_STAGING_DIR}/AAASeed.app"

    #  3. Drag-to-Applications affordance (todo.md L380). A symlink
    #     to /Applications at the DMG root is the canonical macOS
    #     idiom -- Finder renders it as an alias the user drags onto.
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        "/Applications"
        "${AAASEED_DMG_STAGING_DIR}/Applications"

    #  4. Placeholder .background/ dir for a future DS_Store-driven
    #     backdrop image (drop a .png in here + tweak .DS_Store in a
    #     follow-up session ; out of scope for c137-B).
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${AAASEED_DMG_STAGING_DIR}/.background"

    #  5. Copy the placeholder README.txt one-liner ("Drag AAASeed
    #     to Applications to install"). Distinct from the runbook
    #     at bundle/macos/README.md ; this is what end-users see when
    #     they mount the DMG, not the build-time documentation.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${AAASEED_DMG_ASSETS_DIR}/README.txt"
        "${AAASEED_DMG_STAGING_DIR}/README.txt"

    #  6. Remove any prior DMG so hdiutil -overwrite is unambiguous.
    COMMAND ${CMAKE_COMMAND} -E rm -f "${AAASEED_DMG_OUTPUT}"

    #  7. hdiutil create -- try ULMO first, fall back to ULFO. The
    #     `||` is a shell construct so we wrap the whole thing in
    #     `sh -c` to keep semantics portable across CMake generators.
    #  hdiutil create has no -overwrite flag (step 6 already rm'd any
    #  prior output) ; passing it caused step 7 to fail on macOS 14+.
    COMMAND sh -c
        "hdiutil create -format ULMO -srcfolder '${AAASEED_DMG_STAGING_DIR}' -volname '${AAASEED_DMG_VOLNAME}' '${AAASEED_DMG_OUTPUT}' \
         || hdiutil create -format ULFO -srcfolder '${AAASEED_DMG_STAGING_DIR}' -volname '${AAASEED_DMG_VOLNAME}' '${AAASEED_DMG_OUTPUT}'"

    COMMENT "Staging AAASeed.app and producing ${AAASEED_DMG_OUTPUT} (unsigned)"
    VERBATIM
)

message(STATUS "aaaseed_dmg : target wired. "
               "Build with `cmake --build <bin> --target aaaseed_dmg`. "
               "Output : ${AAASEED_DMG_OUTPUT}")
