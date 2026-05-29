# cmake/aaa_qt6.cmake
#
# c152-A : Qt6 + QML integration for the next-gen UI shell.
#
# Brings in the Qt6 modules used by the Studio Qt port (aaaseed_app_qt
# target lives in src/ui/qt/CMakeLists.txt). Auto-detects Homebrew's
# Qt install (default on Apple Silicon : /opt/homebrew/opt/qt) but lets
# the caller override via the AAA_QT6_DIR cache var or the standard
# CMAKE_PREFIX_PATH.
#
# Why a separate include() rather than inlining into the root :
#   - Future Windows / Linux ports can override the Qt prefix without
#     touching the root CMakeLists.
#   - find_package(Qt6 ...) is heavy ; keeping it gated behind a
#     dedicated module makes the ImGui-only build path (the existing
#     aaaseed_app target) skip the lookup entirely.
#
# License : Qt6 is LGPL-3 by default. The aaaseed_app_qt target links
# Qt frameworks DYNAMICALLY so the user-relink requirement of LGPL is
# satisfied automatically by the .dylib bundling in the .app. See
# docs/developer/qt6-migration.md for the full disclosure.

if(NOT APPLE)
    message(FATAL_ERROR
        "aaa_qt6.cmake : the Qt6 port is currently Mac-only. "
        "Windows / Linux Qt branches are tracked in "
        "ui/notes/qt6-migration.md.")
endif()

# Auto-detect Homebrew Qt6.
if(NOT DEFINED AAA_QT6_DIR)
    if(EXISTS "/opt/homebrew/opt/qt/lib/cmake/Qt6")
        set(AAA_QT6_DIR "/opt/homebrew/opt/qt" CACHE PATH
            "Qt6 install prefix (auto-detected from Homebrew).")
    elseif(EXISTS "/opt/homebrew/opt/qt@6/lib/cmake/Qt6")
        set(AAA_QT6_DIR "/opt/homebrew/opt/qt@6" CACHE PATH
            "Qt6 install prefix (auto-detected from Homebrew, qt@6).")
    elseif(EXISTS "/usr/local/opt/qt/lib/cmake/Qt6")
        set(AAA_QT6_DIR "/usr/local/opt/qt" CACHE PATH
            "Qt6 install prefix (auto-detected from Intel Homebrew).")
    else()
        set(AAA_QT6_DIR "" CACHE PATH
            "Qt6 install prefix. Set this if find_package can't locate Qt6.")
    endif()
endif()

if(AAA_QT6_DIR)
    list(PREPEND CMAKE_PREFIX_PATH "${AAA_QT6_DIR}")
endif()

# Qt6 requires AUTOMOC/AUTORCC/AUTOUIC for Q_OBJECT macros + .qrc
# resources + .ui Designer forms. We enable them at the per-target
# level in src/ui/qt/CMakeLists.txt rather than globally so the
# existing engine targets don't pay the cost.

# Components used by aaaseed_app_qt :
#   - Core            : QObject, QString, QFile, signals/slots, JSON
#   - Gui             : QGuiApplication, native pixel formats
#   - Qml             : QQmlEngine, QML registration macros
#   - Quick           : QtQuick scene graph, QQuickWindow
#   - QuickControls2  : QtQuick.Controls 2 widgets (Button, TextField,
#                       SplitView, Menu, FileDialog, ...)
#   - Multimedia      : QtMultimedia (camera capture, audio I/O)
#   - Concurrent      : QtConcurrent::run for NSTask-equivalent process I/O
#   - Network         : reserved for future asset import via URL
#
# find_package is invoked here as a sanity check ; the target wires
# the actual link in src/ui/qt/CMakeLists.txt so dependents only see
# Qt symbols transitively.
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Multimedia
)

message(STATUS "AAASeed Qt6 : version ${Qt6_VERSION} at ${AAA_QT6_DIR}")

# The macdeployqt tool bundles Qt frameworks into the .app. We resolve
# its path here so the release pipeline can invoke it.
if(EXISTS "${AAA_QT6_DIR}/bin/macdeployqt")
    set(AAA_MACDEPLOYQT_EXECUTABLE "${AAA_QT6_DIR}/bin/macdeployqt"
        CACHE FILEPATH "macdeployqt for the Qt port's release bundle.")
endif()
