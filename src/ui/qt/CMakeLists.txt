# src/ui/qt/CMakeLists.txt
#
# c152-A : `aaaseed_app_qt` -- Qt6 + QML port of the AAASeed Studio.
# Sibling to the existing ImGui-based `aaaseed_app` ; both stay
# buildable side-by-side during the migration so we don't lose a
# working binary while the rewrite is in progress.
#
# Phase 0 scope :
#   - QGuiApplication + QQmlApplicationEngine boot.
#   - StudioModel Q_OBJECT bridge for the Project Manager surface.
#   - QML resource bundle (Main.qml).
#   - macdeployqt-friendly .app bundle layout.
#
# Build target naming :
#   aaaseed_app      = ImGui Studio (unchanged ; 800 KB ship binary)
#   aaaseed_app_qt   = Qt6 + QML Studio (Phase 0 today, grows panel-by-panel)

# AUTOMOC scans Q_OBJECT macros and generates the moc TUs.
# AUTORCC compiles .qrc resources into the binary.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# We don't use .ui Designer forms ; QML covers the entire UI tree.
set(CMAKE_AUTOUIC OFF)

# ── Sources ───────────────────────────────────────────────────────────────
set(AAA_QT_SOURCES
    aaa_qt_main.cpp
    aaa_studio_model.cpp
    aaa_studio_model.h           # listed so AUTOMOC sees Q_OBJECT
    aaa_panel_models.cpp         # c152-D : Sound + Camera + BinaryTasks
    aaa_panel_models.h
    aaa_engine_viewport.mm       # c152-G : in-window engine NSView
    aaa_engine_viewport.h
    aaa_lua_helper.cpp           # c152-M : Lua highlight/lint + asset type
    aaa_lua_helper.h
    aaa_settings_model.cpp       # c152-N : app settings (theme/editor/etc)
    aaa_settings_model.h
    aaa_native_bridge.cpp        # c154 : native sub-lib bridge + Display menu
    aaa_native_bridge.h
    aaa_qt_qml.qrc               # listed so AUTORCC bundles it
)

# c152-G : the engine viewport .mm uses ObjC++ + MRC ; the rest of
# the Qt target is plain C++ + ARC by default. Tag the file so the
# build system picks the right flags.
set_source_files_properties(aaa_engine_viewport.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc -Wno-unused-parameter"
)

add_executable(aaaseed_app_qt MACOSX_BUNDLE
    ${AAA_QT_SOURCES}
)

target_include_directories(aaaseed_app_qt PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/src/ui/studio"
)

target_link_libraries(aaaseed_app_qt PRIVATE
    # c152-G : the foreign-NSView engine viewport. Static lib carries
    # AAASeedMTKView + AAASeedInputView ; pulled in transitively are
    # aaaseed_meu_runner, aaaseed_gol_metal, the Mac frameworks.
    aaaseed_engine_view
    # c152-C : platform-neutral studio data model (no ObjC, no Metal).
    # Post-ImGui-retirement this is the ONLY studio lib.
    aaaseed_ui_studio
    # c152-E : direct lua link for the Studio's syntax-check scratchpad.
    # aaaseed_ui_studio links lua PUBLIC, so this is duplicate-but-explicit.
    aaaseed_lua
    # c154 : the c153 native feature sub-libs, surfaced to QML via
    # aaa_native_bridge (nativeDevices / nativeDisplay context properties).
    # All hermetic ; no engine link beyond what they already carry.
    aaaseed_midi
    aaaseed_audio
    aaaseed_video
    aaaseed_syphon
    aaaseed_clipboard
    aaaseed_net
    aaaseed_display
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Multimedia
)

target_compile_features(aaaseed_app_qt PRIVATE cxx_std_20)

# c154 : wire the CANONICAL Info.plist into the Qt Studio app. PREVIOUSLY
# aaaseed_app_qt auto-generated its plist from target properties, which OMITS
# NSCameraUsageDescription / NSMicrophoneUsageDescription -- so the shipping
# DMG app (which DOES access the camera via the Qt-Multimedia Camera panel)
# would be KILLED by macOS TCC on first camera access, and never prompt. The
# runtime (src/ui/macos) already uses bundle/macos/Info.plist.in ; we now
# configure the same template for the Studio so the DMG asks permission.
# NB: the template uses ${VAR} placeholders (matching the runtime consumer in
# src/ui/macos), so configure_file WITHOUT @ONLY is required here.
set(MACOSX_BUNDLE_EXECUTABLE_NAME       "AAASeed-Studio")
set(MACOSX_BUNDLE_BUNDLE_NAME           "AAASeed Studio")
set(MACOSX_BUNDLE_BUNDLE_VERSION        "${PROJECT_VERSION}")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING  "${PROJECT_VERSION}")
configure_file(
    "${CMAKE_SOURCE_DIR}/bundle/macos/Info.plist.in"
    "${CMAKE_BINARY_DIR}/Info.qt.plist"
)

# c152-M : Qt is now the sole UI ; drop the "(Qt)" suffix from the
# user-facing name. The OUTPUT_NAME stays binary-style.
set_target_properties(aaaseed_app_qt PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST         "${CMAKE_BINARY_DIR}/Info.qt.plist"
    MACOSX_BUNDLE_BUNDLE_NAME       "AAASeed Studio"
    MACOSX_BUNDLE_BUNDLE_VERSION    "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_GUI_IDENTIFIER    "com.seedexr.aaaseed"
    MACOSX_BUNDLE_ICON_FILE         "AAASeed.icns"
    OUTPUT_NAME                     "AAASeed-Studio"
)

# c152-L : bundle the AAASeed.icns into Resources/ so Finder, Dock,
# and Cmd-Tab pick up the hand-logo. MACOSX_BUNDLE_ICON_FILE above
# names it in Info.plist ; this command actually copies the bytes.
if(EXISTS "${CMAKE_SOURCE_DIR}/bundle/macos/AAASeed.icns")
    add_custom_command(TARGET aaaseed_app_qt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/bundle/macos/AAASeed.icns"
            "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app_qt>/Resources/AAASeed.icns"
        COMMENT "Copying AAASeed.icns into AAASeed-Studio.app"
        VERBATIM
    )
endif()

# c152-A : suppress the deprecated-warnings inside the Qt headers
# themselves (they hit on every Qt SDK refresh and are out of our
# control).
target_compile_options(aaaseed_app_qt PRIVATE
    -Wno-deprecated-declarations
)

# c152-I : bundle the starter project into the .app so the Welcome
# screen's "Open Sample" button can load it without an internet
# connection. Lands at AAASeed-Studio.app/Contents/Resources/sample/.
if(EXISTS "${CMAKE_SOURCE_DIR}/bundle/macos/sample/starter.aaaproj.lua")
    add_custom_command(TARGET aaaseed_app_qt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app_qt>/Resources/sample"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/bundle/macos/sample/starter.aaaproj.lua"
            "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app_qt>/Resources/sample/starter.aaaproj.lua"
        COMMENT "Copying starter.aaaproj.lua into AAASeed-Studio.app"
        VERBATIM
    )
endif()
