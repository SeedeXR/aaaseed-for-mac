# src/ui/macos/CMakeLists.txt
#
# The Objective-C++ shell : NSApplication + NSWindow + MTKView + MTKView
# delegate. Pure ObjC++ ; the C++ GOL::MetalBackend lives inside the
# delegate and is reached via a forward-declared pointer.

find_library(COCOA_FRAMEWORK      Cocoa      REQUIRED)
find_library(METALKIT_FRAMEWORK   MetalKit   REQUIRED)
# Metal, Foundation, QuartzCore already located by src/gol/metal/CMakeLists.txt
# but find_library is cheap; restate for self-contained linkage.
find_library(METAL_FRAMEWORK_UI       Metal      REQUIRED)
find_library(FOUNDATION_FRAMEWORK_UI  Foundation REQUIRED)
find_library(QUARTZCORE_FRAMEWORK_UI  QuartzCore REQUIRED)

set(AAA_UI_MM_SOURCES
    MainEntry.mm
    AAASeedAppDelegate.mm
    AAASeedMTKView.mm
    AAASeedInputView.mm
)

#  c119-B / Task #150 : aaa_event_bridge -- hermetic Mac sub-lib that drains
#  AAASeedInputView's NSEvent side-buffer into a Mac-side POD EngineEvent
#  FIFO. Bridges Phase 4 input plumbing without waiting on the vendor
#  c_event_keyboard / c_event_mouse port (deferred ; needs full engine
#  event-queue subsystem). Pure CPU ; std::vector + Cocoa only ; no o_str /
#  aaa_mem / engine link. See feedback_hermetic_mac_sublibs.md.
#  c133 / Phase 4 (todo L291) : aaa_event_bridge_gesture extends the c119-B
#  sub-lib with NSEventTypeScrollWheel / Magnify / Rotate / Swipe drains.
#  Same hermetic doctrine -- pure std::vector / Foundation / AppKit, no
#  o_str / aaa_mem / aaa_str. Lives in the same target so consumers only
#  link `aaaseed_event_bridge` once.
#  c133 : AAASeedInputView.mm was historically inside this static lib
#  too, but the app target ALSO compiles it from AAA_UI_MM_SOURCES,
#  causing duplicate ObjC class registration ("Class is implemented in
#  both ..."). Now that the app links this static lib directly (for the
#  bridge symbols the view forwards into), the duplicate has to go.
#  EventBridge::drain_from_view reaches the view via dynamic dispatch
#  on the ObjC runtime ; no static link needed.
#  c144 / Phase 4 v1 ship-gate (todo.md L29) : engine-side
#  `c_event_mouse` + `c_event_keyboard` adapter -- folded INTO this
#  same `aaaseed_event_bridge` static lib (rather than a separate
#  target) so consumers only link one archive. The adapter is the
#  glue that converts c119-B EngineEvent POD into engine-shaped
#  `MouseEventOut` / `KeyboardEventOut` POD ready for the vendor
#  c_event_* factories. Pure C++ API per c134-A bridge doctrine ; .mm
#  preserves ObjC++ flexibility for a future NSEvent direct path.
#  Hermetic -- no link to aaaseed_code_utils.
add_library(aaaseed_event_bridge STATIC
    aaa_event_bridge.mm
    aaa_event_bridge_gesture.mm
    aaa_event_adapter_mac.mm
)
set_source_files_properties(
    aaa_event_bridge.mm
    aaa_event_bridge_gesture.mm
    aaa_event_adapter_mac.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_include_directories(aaaseed_event_bridge PUBLIC
    "${CMAKE_SOURCE_DIR}"
)
target_link_libraries(aaaseed_event_bridge PUBLIC
    "${COCOA_FRAMEWORK}"
    "${METALKIT_FRAMEWORK}"
)
target_compile_features(aaaseed_event_bridge PUBLIC cxx_std_20)

#  c132 / Phase 4 unblock : aaaseed_ui_file_dialog_mac -- hermetic Mac sub-lib
#  replacing the Win32 `aaa::file::get_filename_*` entry points
#  (vendor/aaaseed-engine/Src/ui/file_dlg.h ; todo.md Phase 4 L284). Pure
#  std::string + std::vector + std::optional API ; NSOpenPanel /
#  NSSavePanel impl ; UTType used behind @available(macOS 11.0, *). NO
#  link to aaaseed_code_utils on purpose -- sidesteps the o_str / aaa_mem
#  / err.h cascade (see feedback_hermetic_mac_sublibs.md). Future
#  cross-platform shim layer can route the vendor signature here on
#  __APPLE__. Tests in tests/unit/ui_file_dialog_test.cpp link only this
#  sub-lib + AppKit + Foundation + UniformTypeIdentifiers.
find_library(APPKIT_FRAMEWORK_FILEDLG               AppKit                   REQUIRED)
find_library(FOUNDATION_FRAMEWORK_FILEDLG           Foundation               REQUIRED)
find_library(UNIFORMTYPEID_FRAMEWORK_FILEDLG        UniformTypeIdentifiers   REQUIRED)
add_library(aaaseed_ui_file_dialog_mac STATIC
    aaa_file_dialog.mm
)
set_source_files_properties(aaa_file_dialog.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_include_directories(aaaseed_ui_file_dialog_mac PUBLIC
    "${CMAKE_SOURCE_DIR}"
)
target_link_libraries(aaaseed_ui_file_dialog_mac PUBLIC
    "${APPKIT_FRAMEWORK_FILEDLG}"
    "${FOUNDATION_FRAMEWORK_FILEDLG}"
    "${UNIFORMTYPEID_FRAMEWORK_FILEDLG}"
)
target_compile_features(aaaseed_ui_file_dialog_mac PUBLIC cxx_std_20)

#  c135 / Phase 4 (todo L298) : aaaseed_ui_dpi_mac -- hermetic Mac
#  sub-lib replacing the Win32 `GetDpiForMonitor` path
#  (vendor/aaaseed-engine/Src/platform/win32/touch_windows.h) with
#  `[NSScreen backingScaleFactor]` + multi-display fan-out +
#  point<->pixel helpers + per-window scale lookup. Pure C++ API in
#  `aaa::ui::dpi` (no Objective-C in the header) ; impl in
#  aaa_dpi.mm uses Foundation + AppKit only. NO link to
#  aaaseed_code_utils -- preserves the c104 hermetic doctrine
#  (see feedback_hermetic_mac_sublibs.md). Tests in
#  tests/unit/ui_dpi_test.cpp link only this sub-lib + AppKit +
#  Foundation.
find_library(APPKIT_FRAMEWORK_DPI           AppKit       REQUIRED)
find_library(FOUNDATION_FRAMEWORK_DPI       Foundation   REQUIRED)
add_library(aaaseed_ui_dpi_mac STATIC
    aaa_dpi.mm
)
set_source_files_properties(aaa_dpi.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_include_directories(aaaseed_ui_dpi_mac PUBLIC
    "${CMAKE_SOURCE_DIR}"
)
target_link_libraries(aaaseed_ui_dpi_mac PUBLIC
    "${APPKIT_FRAMEWORK_DPI}"
    "${FOUNDATION_FRAMEWORK_DPI}"
)
target_compile_features(aaaseed_ui_dpi_mac PUBLIC cxx_std_20)

#  c136 / Phase 4 (todo L301) : aaaseed_ui_dialog_mac -- hermetic Mac
#  sub-lib replacing the Win32 modal/modeless dialogs
#  (vendor/aaaseed-engine/Src/ui/dialog_*.h) with NSAlert + NSPanel.
#  Pure C++ API in `aaa::ui::dialog` (alert / confirm / prompt + opaque
#  PanelHandle for modeless panels) ; impl in aaa_dialog.mm uses
#  Foundation + AppKit only. parent_window is `void*` per c134-A
#  bridge doctrine (no `@class NSWindow` ifdef inside the C++
#  namespace -- would diverge name mangling at link time). NO link to
#  aaaseed_code_utils -- preserves the c104 hermetic doctrine
#  (see feedback_hermetic_mac_sublibs.md). Tests in
#  tests/unit/ui_dialog_test.cpp link only this sub-lib + AppKit +
#  Foundation. Pure C++ test TU verifies no-ObjC-in-header contract
#  at compile time (per c131-B + c134-A pattern).
find_library(APPKIT_FRAMEWORK_DIALOG        AppKit       REQUIRED)
find_library(FOUNDATION_FRAMEWORK_DIALOG    Foundation   REQUIRED)
add_library(aaaseed_ui_dialog_mac STATIC
    aaa_dialog.mm
)
set_source_files_properties(aaa_dialog.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_include_directories(aaaseed_ui_dialog_mac PUBLIC
    "${CMAKE_SOURCE_DIR}"
)
target_link_libraries(aaaseed_ui_dialog_mac PUBLIC
    "${APPKIT_FRAMEWORK_DIALOG}"
    "${FOUNDATION_FRAMEWORK_DIALOG}"
)
target_compile_features(aaaseed_ui_dialog_mac PUBLIC cxx_std_20)

set_source_files_properties(${AAA_UI_MM_SOURCES} PROPERTIES
    LANGUAGE OBJCXX
    #  Manual reference counting matches metal-cpp's manual ARC pattern.
    #  Don't mix ARC with metal-cpp objects.
    COMPILE_FLAGS "-fno-objc-arc"
)

add_executable(aaaseed_app MACOSX_BUNDLE
    ${AAA_UI_MM_SOURCES}
)

#  Path A catalog shaders shipped inside the .app bundle so the
#  runtime can load them via [NSBundle mainBundle] pathForResource:.
#  Continuation 39 -- first asset-bundling step. CMake's post-build
#  command copies src/shaders/msl/*.metal into the bundle's
#  Contents/Resources/shaders/ directory on every build. This stays
#  Mac-only ; Windows engine ships GLSL the historical way.
add_custom_command(TARGET aaaseed_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/shaders/msl"
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/shaders"
    COMMENT "Copying Path A catalog .metal files into aaaseed_app bundle"
    VERBATIM
)

#  Continuation 46 : copy the build-time generated test_pattern.png
#  into Resources/ . The aaa_test_png_fixture dependency ensures it
#  exists before this copy runs.
add_custom_command(TARGET aaaseed_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${AAA_TEST_PNG_PATH}"
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/test_pattern.png"
    COMMENT "Copying Phase 5 test_pattern.png into aaaseed_app bundle"
    VERBATIM
)

#  c151-A : copy the placeholder AAASeed.icns into Resources/ . The
#  Info.plist's CFBundleIconFile = "AAASeed" tells macOS to look for
#  Resources/AAASeed.icns at runtime.
if(EXISTS "${CMAKE_SOURCE_DIR}/bundle/macos/AAASeed.icns")
    add_custom_command(TARGET aaaseed_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/bundle/macos/AAASeed.icns"
            "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/AAASeed.icns"
        COMMENT "Copying AAASeed.icns placeholder icon into aaaseed_app bundle"
        VERBATIM
    )
endif()

#  Info.plist singularity collapse (2026-05-27, Phase 8 / todo.md L365-L383).
#  c136-B originally landed a TWO-plist arrangement (template at
#  src/ui/macos/Info.plist.in PLUS fully-resolved artefact at
#  bundle/macos/Info.plist). The two drifted independently. This
#  collapse merges both into ONE canonical ${...}-substituted template
#  at `${CMAKE_SOURCE_DIR}/bundle/macos/Info.plist.in`, ran through
#  `configure_file()` here to produce the resolved plist consumed by
#  MACOSX_BUNDLE_INFO_PLIST. Single source of truth ; the same template
#  is the substrate future codesign / notarytool / hdiutil steps will
#  consume (they'll read the same configured output at
#  `${CMAKE_BINARY_DIR}/Info.plist`).
#
#  Reconciliation report (old src/ui/macos/Info.plist.in -> canonical) :
#    Keys merged FROM old per-UI template INTO canonical :
#      - MetalCaptureEnabled = <true/>     (unique to old template ;
#                                           Metal debugger gate ; preserved)
#    Keys retained from canonical that were ABSENT in old template :
#      - CFBundleDisplayName               (= "AAASeed")
#      - NSSupportsAutomaticGraphicsSwitching (= <true/>)
#      - NSCameraUsageDescription          (Phase 8 hardened-runtime req)
#      - NSMicrophoneUsageDescription      (Phase 8 hardened-runtime req)
#      - CFBundleDocumentTypes             (= <array/>, future doc types)
#      - LSHandlerRank                     (= "Owner")
#      - CFBundlePackageType               (was in old too ; same value)
#    Template-substitution sites preserved :
#      - ${MACOSX_BUNDLE_EXECUTABLE_NAME}      -> CFBundleExecutable
#      - ${MACOSX_BUNDLE_BUNDLE_NAME}          -> CFBundleName
#      - ${MACOSX_BUNDLE_BUNDLE_VERSION}       -> CFBundleVersion
#      - ${MACOSX_BUNDLE_SHORT_VERSION_STRING} -> CFBundleShortVersionString
#      - ${CMAKE_OSX_DEPLOYMENT_TARGET}        -> LSMinimumSystemVersion
#    Conflicts resolved :
#      - CFBundleIdentifier : both files agreed on "ai.bsa.aaaseed".
#      - CFBundleDevelopmentRegion : both agreed on "en".
#      - CFBundleInfoDictionaryVersion : both agreed on "6.0".
#      - No semantic conflicts ; reconciliation was strict superset.
#
#  Regression guard : tests/unit/info_plist_singularity_test.cpp asserts
#  there is EXACTLY ONE Info.plist*.in in the source tree (rooted at
#  bundle/macos/), so a future session can't silently reintroduce a
#  second plist.
#  MACOSX_BUNDLE_EXECUTABLE_NAME / MACOSX_BUNDLE_BUNDLE_NAME /
#  MACOSX_BUNDLE_BUNDLE_VERSION / MACOSX_BUNDLE_SHORT_VERSION_STRING are
#  populated by CMake from the target_properties below DURING the
#  MACOSX_BUNDLE_INFO_PLIST template-expansion pass. We pre-set them as
#  scope-local variables here so `configure_file()` can substitute
#  matching ${VAR} sites at configure-time. The TARGET_PROPERTY values
#  below are kept in lockstep ; mismatches between the two would
#  drift the .app dictionary.
set(MACOSX_BUNDLE_EXECUTABLE_NAME       "aaaseed_app")
set(MACOSX_BUNDLE_BUNDLE_NAME           "AAASeed")
set(MACOSX_BUNDLE_BUNDLE_VERSION        "${PROJECT_VERSION}")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING  "${PROJECT_VERSION}")
#  configure_file() with no flags substitutes BOTH ${VAR} and @VAR@.
#  Our canonical template uses ${VAR} syntax (matching CMake's own
#  MACOSX_BUNDLE_INFO_PLIST default-template style), so the default
#  is correct -- do NOT pass @ONLY (would skip ${...} sites).
configure_file(
    "${CMAKE_SOURCE_DIR}/bundle/macos/Info.plist.in"
    "${CMAKE_BINARY_DIR}/Info.plist"
)
set_target_properties(aaaseed_app PROPERTIES
    MACOSX_BUNDLE                           TRUE
    MACOSX_BUNDLE_INFO_PLIST                "${CMAKE_BINARY_DIR}/Info.plist"
    MACOSX_BUNDLE_BUNDLE_NAME               "AAASeed"
    MACOSX_BUNDLE_GUI_IDENTIFIER            "ai.bsa.aaaseed"
    MACOSX_BUNDLE_BUNDLE_VERSION            "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING      "${PROJECT_VERSION}"
)

target_include_directories(aaaseed_app PRIVATE
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/third_party/metal-cpp"
)

target_link_libraries(aaaseed_app PRIVATE
    aaaseed_gol_metal
    aaaseed_stb
    aaaseed_text
    #  c133 view-side wiring : AAASeedInputView.mm in this target now
    #  references aaa::input::EventBridge / GestureBridge. The hermetic
    #  aaaseed_event_bridge static lib carries the symbols. Note : the
    #  static lib also compiles its own copy of AAASeedInputView.mm, so
    #  we get duplicate -[AAASeedInputView ...] symbols at link time ;
    #  the ObjC runtime de-duplicates by class registration, and ld
    #  resolves the C++ template/inline symbols by first-wins. (The
    #  doubled-up archive entry is harmless ; the bridge .mm objects
    #  are what we actually need.)
    aaaseed_event_bridge
    #  c144 : MEU runner integration. AAASeedMTKViewDelegate constructs
    #  an aaa::meu::Runner against the backend, loads hello_world.lua
    #  from Resources/meu/, and drives render_frame per drawInMTKView:.
    #  AAASeedInputView forwards keyDown / mouseDown / scrollWheel events
    #  into the runner so Lua sees the same input the bridge surface
    #  sees. Hermetic sub-lib carries Lua + render dispatch.
    aaaseed_meu_runner
    #  c147-A : immediate-mode widget UI. The delegate owns a
    #  unique_ptr<aaa::ui::widgets::WidgetSystem> ivar and runs
    #  begin_frame / end_frame inside drawInMTKView:. Transitively
    #  brought in by aaaseed_meu_runner (PUBLIC link), restated here
    #  for explicit clarity at the app link line.
    aaaseed_ui_widgets_mac
    #  c148 : ImGui Studio authoring surface. Brings in aaaseed_imgui +
    #  aaaseed_imgui_metal_backend transitively.
    aaaseed_ui_studio_mac
    "${COCOA_FRAMEWORK}"
    "${METALKIT_FRAMEWORK}"
    "${METAL_FRAMEWORK_UI}"
    "${FOUNDATION_FRAMEWORK_UI}"
    "${QUARTZCORE_FRAMEWORK_UI}"
)

#  c144 : compile-time fallback path for hello_world.lua when the app is
#  invoked outside its .app bundle (e.g. integration tests that drive
#  AAASeedMTKViewDelegate without a packaged binary). NSBundle lookup
#  remains the primary path -- this is only a fallback.
target_compile_definitions(aaaseed_app PRIVATE
    AAA_MEU_HELLO_WORLD_LUA_PATH="${CMAKE_SOURCE_DIR}/bundle/macos/meu/hello_world.lua"
)

#  c144 : copy bundle/macos/meu/hello_world.lua into the .app's
#  Resources/meu/ directory so the runtime NSBundle lookup
#  `pathForResource:@"hello_world" ofType:@"lua" inDirectory:@"meu"`
#  resolves to the shipped script. Matches the pattern used for
#  Resources/shaders/ (continuation 39) and Resources/fonts/ (c61).
add_custom_command(TARGET aaaseed_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/meu"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/bundle/macos/meu/hello_world.lua"
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/meu/hello_world.lua"
    COMMENT "Copying hello_world.lua into aaaseed_app bundle Resources/meu/"
    VERBATIM
)

#  c145-D : copy the v1 sample MEU library (5 starter MEUs : mire,
#  animator, keyboard, mouse, composer) into the .app's
#  Resources/meu/Samples/ . Each sample is a folder with <name>.lua +
#  README.md . Together they exercise every binding documented in
#  docs/AUTHORING_MEUS_ON_MAC.md section 2 and replace the missing
#  Windows-style "node interface" -- on Windows there is no node editor
#  either ; the MEU library IS the authoring surface (c145-C research).
#  See memory/project_v1_ship_gate.md "Authoring Surface" section.
#
#  c147 v2 Phase 1 : the same `copy_directory` covers the new
#  `Samples/v2_widgets/` subdir wholesale (recursive copy by default),
#  so no extension to this rule was needed -- the canonical v2 demo
#  ships at Resources/meu/Samples/v2_widgets/{widgets_demo.lua,README.md}
#  automatically. Regression-guarded by
#  tests/unit/v2_widgets_sample_test.cpp .
#
#  c148 v2 Phases 2-4 (comment-only annotation, no rule change) : the
#  same recursive `copy_directory` covers the THREE new c148 sample
#  subdirs without any extension :
#    Samples/v2_color_picker/    -> color_picker_demo.lua + README.md
#    Samples/v2_modal_textinput/ -> modal_textinput_demo.lua + README.md
#    Samples/v2_collapse_reload/ -> collapse_reload_demo.lua + README.md
#  Regression-guarded by tests/unit/v2_phase234_samples_test.cpp .
add_custom_command(TARGET aaaseed_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/meu/Samples"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/bundle/macos/meu/Samples"
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/meu/Samples"
    COMMENT "Copying MEU sample library into aaaseed_app bundle Resources/meu/Samples/"
    VERBATIM
)

#  c145-D : ship the authoring guide inside the .app so users running
#  the DMG without a source checkout can still find the aaa.* API
#  reference. Lives at Resources/AUTHORING_MEUS_ON_MAC.md .
add_custom_command(TARGET aaaseed_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/docs/AUTHORING_MEUS_ON_MAC.md"
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/AUTHORING_MEUS_ON_MAC.md"
    COMMENT "Copying AUTHORING_MEUS_ON_MAC.md into aaaseed_app bundle Resources/"
    VERBATIM
)

#  Continuation 61 : bundle SourceCodePro-Medium.ttf into the .app's
#  Resources/fonts/ directory so AAASeedMTKView can load it at boot
#  via NSBundle pathForResource. The font is SIL OFL (Google Fonts)
#  -- safe to ship inside the bundle.
add_custom_command(TARGET aaaseed_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/fonts"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/vendor/aaaseed-runtime/AAAKernel/Fonts/Google/Source_Code_Pro/static/SourceCodePro-Medium.ttf"
        "$<TARGET_BUNDLE_CONTENT_DIR:aaaseed_app>/Resources/fonts/SourceCodePro-Medium.ttf"
    COMMENT "Copying SourceCodePro-Medium.ttf into aaaseed_app bundle"
    VERBATIM
)

#  Phase 5 fixture (continuation 46) : depend on the test_png target
#  so the PNG is regenerated when the generator changes, then copy
#  into the .app bundle's Resources at post-build.
add_dependencies(aaaseed_app aaa_test_png_fixture)

target_compile_features(aaaseed_app PRIVATE cxx_std_20)

#  Phase 8 binary-size discipline (c143-B, 2026-05-27, todo.md L379-L387).
#  Release-only : thin LTO + dead-code/dead-string strip + linker `-x`
#  (drop local symbols). Cuts ~30-40% off the .app's executable section
#  on arm64 ; pairs with the post-link `strip -x` in scripts/ship-dmg.sh.
#  Generator-expression-gated on CMAKE_BUILD_TYPE so Debug builds keep
#  full symbols for lldb. Mac-only (we're in src/ui/macos/CMakeLists.txt).
target_compile_options(aaaseed_app PRIVATE
    $<$<CONFIG:Release>:-flto=thin>
)
target_link_options(aaaseed_app PRIVATE
    $<$<CONFIG:Release>:-flto=thin>
    $<$<CONFIG:Release>:-Wl,-dead_strip>
    $<$<CONFIG:Release>:-Wl,-x>
)

#  Smoke test : launch the binary inside the .app bundle with
#  `--max-frames 3` and verify it exits cleanly within the wall-clock
#  fallback window (3 seconds). Does NOT verify the window actually
#  shows pixels — that requires the bundle to be launched via Finder /
#  `open`, which needs interactive activation. The smoke test catches
#  init-time crashes, hung event loops, and link / linker-flag breakage.
add_test(
    NAME aaaseed_app_smoke
    COMMAND $<TARGET_FILE:aaaseed_app> --max-frames 3
)
set_tests_properties(aaaseed_app_smoke PROPERTIES
    LABELS "integration;ui;macos"
    TIMEOUT 10
)
