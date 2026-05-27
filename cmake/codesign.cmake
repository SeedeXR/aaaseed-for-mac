# cmake/codesign.cmake
#
# Phase 8 distribution -- env-gated codesign + notarize wiring for
# AAASeed.app + AAASeed-<ver>.dmg.
#
# Continuation 138-B (2026-05-27, c138-B). Extends the c137-B DMG
# beachhead (`cmake/dmg.cmake` + `aaaseed_dmg`) with two POST_BUILD
# hooks that ONLY fire when the appropriate developer-id environment
# variables are exported by the operator. When the env vars are unset
# (the default for CI + local dev), both hooks are NO-OPs : the build
# stays UNSIGNED and NOT NOTARIZED exactly like c137-B.
#
# This module is the bridge between the c136-B / c137-B "ready but not
# executed" runbook in `bundle/macos/README.md` and a real signed +
# notarized DMG, gated on the operator's deliberate decision to export
# the credentials. NO actual codesign / notarize / network calls happen
# at configure time -- the hooks only RECORD the commands to run when
# the developer next builds the relevant target.
#
# Public functions :
#   aaaseed_codesign_target(TARGET)
#       Wraps <TARGET> with a POST_BUILD codesign step IF
#       $ENV{CODESIGN_IDENTITY} is set, else NO-OP (status message).
#       Identity string is read from the environment ; never hardcoded.
#
#   aaaseed_notarize_dmg()
#       Defines `aaaseed_dmg_notarize` target that depends on
#       `aaaseed_dmg` (c137-B) and invokes `xcrun notarytool submit
#       ... --wait` + `xcrun stapler staple` IF the three notary env
#       vars are all set, else NO-OP (status message).
#
# Environment variables consulted (all optional ; absent => NO-OP) :
#   CODESIGN_IDENTITY         -- full Developer ID Application string,
#                                e.g. "Developer ID Application: Foo (BAR123)".
#                                Passed verbatim to `codesign --sign`.
#   NOTARY_API_KEY_PATH       -- absolute path to the App Store Connect
#                                .p8 private key file. Passed to
#                                `xcrun notarytool --key`.
#   NOTARY_API_KEY_ID         -- 10-character key ID from App Store
#                                Connect. Passed to `--key-id`.
#   NOTARY_API_KEY_ISSUER_ID  -- issuer UUID from App Store Connect.
#                                Passed to `--issuer`.
#
# Usage in top-level CMakeLists.txt :
#   include(codesign)
#   aaaseed_codesign_target(aaaseed_app)
#   aaaseed_notarize_dmg()
#
# Runbook : see `bundle/macos/README.md` "How to sign locally" + "How
# to notarize" + "Audit" sections.
#
# Regression-guard tests in `tests/unit/codesign_packaging_test.cpp`
# assert this module references the canonical command lines + does NOT
# leak a hardcoded Developer ID string. Per c137 doctrine, the guards
# make the env-gating contract visible in test output.

if(NOT APPLE)
    message(STATUS "[codesign] non-Apple platform ; codesign + notarize targets skipped.")
    return()
endif()

#  ----------------------------------------------------------------
#  aaaseed_codesign_target(TARGET)
#
#  POST_BUILD hook on <TARGET> that runs `codesign --deep --options
#  runtime --timestamp --entitlements <bundle/macos/entitlements.plist>
#  --sign "$ENV{CODESIGN_IDENTITY}" $<TARGET_BUNDLE_DIR:TARGET>`.
#  Only attached when CODESIGN_IDENTITY env var is set at CMake
#  configure time. Otherwise emits a single STATUS message and
#  returns. The operator re-runs `cmake ..` after `export
#  CODESIGN_IDENTITY=...` to activate the hook.
#  ----------------------------------------------------------------
function(aaaseed_codesign_target TARGET)
    if(NOT TARGET ${TARGET})
        message(WARNING "[codesign] aaaseed_codesign_target : "
                        "target '${TARGET}' is not defined ; skipping.")
        return()
    endif()

    if("$ENV{CODESIGN_IDENTITY}" STREQUAL "")
        message(STATUS "[codesign] CODESIGN_IDENTITY not set -- skipping signing for ${TARGET}. "
                       "Export the Developer ID Application string + reconfigure to enable.")
        return()
    endif()

    set(_codesign_entitlements "${CMAKE_SOURCE_DIR}/bundle/macos/entitlements.plist")
    if(NOT EXISTS "${_codesign_entitlements}")
        message(WARNING "[codesign] entitlements file not found at ${_codesign_entitlements} ; "
                        "skipping codesign hook for ${TARGET}.")
        return()
    endif()

    #  Canonical Developer ID codesign invocation. --deep recurses into
    #  embedded frameworks ; --options runtime enables hardened runtime
    #  (required for notarization) ; --timestamp adds an Apple secure
    #  timestamp (also required for notarization).
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND codesign
                --deep
                --options runtime
                --timestamp
                --entitlements "${_codesign_entitlements}"
                --sign "$ENV{CODESIGN_IDENTITY}"
                "$<TARGET_BUNDLE_DIR:${TARGET}>"
        COMMENT "Codesigning $<TARGET_BUNDLE_DIR:${TARGET}> with Developer ID from \$CODESIGN_IDENTITY"
        VERBATIM
    )

    message(STATUS "[codesign] CODESIGN_IDENTITY detected -- POST_BUILD codesign hook wired on ${TARGET}.")
endfunction()


#  ----------------------------------------------------------------
#  aaaseed_notarize_dmg()
#
#  Defines a new custom target `aaaseed_dmg_notarize` that depends on
#  `aaaseed_dmg` (c137-B) and, when all three NOTARY_* env vars are
#  set, invokes `xcrun notarytool submit ... --wait` followed by
#  `xcrun stapler staple` on the produced DMG. When env vars are
#  unset the target still exists (so CI invocation does not fail)
#  but its command is a single status echo NO-OP.
#  ----------------------------------------------------------------
function(aaaseed_notarize_dmg)
    if(NOT TARGET aaaseed_dmg)
        message(WARNING "[codesign] aaaseed_notarize_dmg : aaaseed_dmg target not defined ; "
                        "include(codesign) AFTER include(dmg). Skipping.")
        return()
    endif()

    set(_dmg_path "${CMAKE_BINARY_DIR}/AAASeed-${PROJECT_VERSION}.dmg")

    if("$ENV{NOTARY_API_KEY_PATH}"      STREQUAL "" OR
       "$ENV{NOTARY_API_KEY_ID}"        STREQUAL "" OR
       "$ENV{NOTARY_API_KEY_ISSUER_ID}" STREQUAL "")
        message(STATUS "[codesign] NOTARY_API_KEY_{PATH,ID,ISSUER_ID} not all set -- "
                       "aaaseed_dmg_notarize is a NO-OP. Export the three vars + reconfigure to enable.")
        add_custom_target(aaaseed_dmg_notarize
            DEPENDS aaaseed_dmg
            COMMAND ${CMAKE_COMMAND} -E echo
                "[codesign] aaaseed_dmg_notarize : env vars not set -- NO-OP. See bundle/macos/README.md."
            COMMENT "aaaseed_dmg_notarize (NO-OP : NOTARY_API_KEY_* env vars unset)"
            VERBATIM
        )
        return()
    endif()

    #  Real notarytool + stapler invocation. notarytool --wait blocks
    #  until Apple's service returns Accepted / Invalid / Rejected.
    #  stapler attaches the notarization ticket to the DMG so Gatekeeper
    #  can verify offline.
    add_custom_target(aaaseed_dmg_notarize
        DEPENDS aaaseed_dmg

        COMMAND xcrun notarytool submit "${_dmg_path}"
                --key "$ENV{NOTARY_API_KEY_PATH}"
                --key-id "$ENV{NOTARY_API_KEY_ID}"
                --issuer "$ENV{NOTARY_API_KEY_ISSUER_ID}"
                --wait

        COMMAND xcrun stapler staple "${_dmg_path}"

        COMMENT "Submitting ${_dmg_path} to Apple notarization service + stapling ticket"
        VERBATIM
    )

    message(STATUS "[codesign] NOTARY_API_KEY_* all set -- aaaseed_dmg_notarize wired with real notarytool + stapler.")
endfunction()


message(STATUS "[codesign] codesign.cmake loaded ; call aaaseed_codesign_target(aaaseed_app) "
               "+ aaaseed_notarize_dmg() to wire hooks. Env-gated -- no-op without CODESIGN_IDENTITY / NOTARY_API_KEY_*.")
