// src/ui/qt/aaa_qt_main.cpp
//
// c152-A : entry point for the Qt6 + QML port of the AAASeed Studio.
// Phase 0 scope :
//   - Boots a QGuiApplication and a QQmlApplicationEngine.
//   - Registers `StudioModel` as a singleton-style context property
//     so the QML scene can drive project I/O end-to-end.
//   - Loads `qrc:/aaa/ui/qt/qml/Main.qml`.
//
// Out of scope for Phase 0 (tracked in docs/developer/qt6-migration.md) :
//   - Engine integration (Qt window owning the Metal render).
//     For now the Qt app boots WITHOUT an engine -- it's the project
//     shell only. The existing ImGui aaaseed_app remains the
//     full-featured authoring binary.
//   - Lua bindings (aaa.studio.*) -- depend on a Runner attached.
//   - Remaining 12 panels.
//
// Build : the target `aaaseed_app_qt` is sibling to `aaaseed_app`.
// Run :   out/.../bin/aaaseed_app_qt.app

#include "src/ui/qt/aaa_studio_model.h"
#include "src/ui/qt/aaa_panel_models.h"
#include "src/ui/qt/aaa_engine_viewport.h"
#include "src/ui/qt/aaa_lua_helper.h"
#include "src/ui/qt/aaa_settings_model.h"
#include "src/ui/studio/aaa_studio.h"

#include <QColor>
#include <QGuiApplication>
#include <QPalette>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QUrl>
#include <qqml.h>

#include <memory>

int main( int argc, char** argv )
{
    QGuiApplication app( argc, argv );
    QGuiApplication::setApplicationName( "AAASeed Studio" );
    QGuiApplication::setOrganizationName( "SeedeXR" );
    QGuiApplication::setOrganizationDomain( "bsa.ai" );

    // c152-N : "Fusion" supports full per-control customization (dark
    // theme bg colors on GroupBox / TextArea / etc). The macOS native
    // style ignores `background:` overrides and warns at runtime.
    QQuickStyle::setStyle( QStringLiteral( "Fusion" ) );

    // Application-wide dark palette so non-stylized QWindow chrome
    // (file/folder dialogs, message boxes) also picks up the dark
    // look.
    QPalette pal;
    pal.setColor( QPalette::Window,         QColor( "#11181c" ) );
    pal.setColor( QPalette::WindowText,     QColor( "#cce7e7" ) );
    pal.setColor( QPalette::Base,           QColor( "#0e1418" ) );
    pal.setColor( QPalette::AlternateBase,  QColor( "#13202a" ) );
    pal.setColor( QPalette::Text,           QColor( "#cce7e7" ) );
    pal.setColor( QPalette::Button,         QColor( "#1f4e58" ) );
    pal.setColor( QPalette::ButtonText,     QColor( "#cce7e7" ) );
    pal.setColor( QPalette::Highlight,      QColor( "#33a6b3" ) );
    pal.setColor( QPalette::HighlightedText, QColor( "#11181c" ) );
    pal.setColor( QPalette::ToolTipBase,    QColor( "#0e1418" ) );
    pal.setColor( QPalette::ToolTipText,    QColor( "#cce7e7" ) );
    pal.setColor( QPalette::PlaceholderText, QColor( "#7a8c8c" ) );
    QGuiApplication::setPalette( pal );

    // Phase 0 : pure data-model Studio (no backend, no runner). Later
    // phases pass real GOL::Backend + meu::Runner pointers.
    auto studio = std::make_unique< aaa::ui::studio::Studio >(
        /*backend=*/ nullptr, /*runner=*/ nullptr );

    aaa::ui::qt6::StudioModel model( studio.get() );

    // c152-B : register the list-model classes as uncreatable QML
    // types so QML files can use `Connections { target: ... }` with
    // them as targets without explicitly importing.
    qmlRegisterUncreatableType< aaa::ui::qt6::ConsoleListModel >(
        "aaa.ui.qt6", 1, 0, "ConsoleListModel",
        QStringLiteral( "expose via studio.consoleModel" ) );
    qmlRegisterUncreatableType< aaa::ui::qt6::NodeListModel >(
        "aaa.ui.qt6", 1, 0, "NodeListModel",
        QStringLiteral( "expose via studio.nodeModel" ) );
    qmlRegisterUncreatableType< aaa::ui::qt6::AssetListModel >(
        "aaa.ui.qt6", 1, 0, "AssetListModel",
        QStringLiteral( "expose via studio.assetModel" ) );

    // c152-D : three additional panel adapters surfaced to QML.
    aaa::ui::qt6::SoundDeviceModel sound;
    aaa::ui::qt6::CameraController camera;
    aaa::ui::qt6::BinaryTaskModel  tasks;
    // c152-G : in-window engine viewport.
    aaa::ui::qt6::EngineViewport   viewport;
    QObject::connect( &viewport, &aaa::ui::qt6::EngineViewport::logLine,
                       &model,    &aaa::ui::qt6::StudioModel::logLine );

    // Route panel logs into the Studio Console.
    QObject::connect( &camera, &aaa::ui::qt6::CameraController::logLine,
                       &model,  &aaa::ui::qt6::StudioModel::logLine );
    QObject::connect( &tasks,  &aaa::ui::qt6::BinaryTaskModel::logLine,
                       &model,  &aaa::ui::qt6::StudioModel::logLine );
    QObject::connect( &sound,  &aaa::ui::qt6::SoundDeviceModel::logLine,
                       &model,  &aaa::ui::qt6::StudioModel::logLine );

    qmlRegisterUncreatableType< aaa::ui::qt6::SoundDeviceModel >(
        "aaa.ui.qt6", 1, 0, "SoundDeviceModel",
        QStringLiteral( "expose via context property" ) );
    qmlRegisterUncreatableType< aaa::ui::qt6::CameraController >(
        "aaa.ui.qt6", 1, 0, "CameraController",
        QStringLiteral( "expose via context property" ) );
    qmlRegisterUncreatableType< aaa::ui::qt6::BinaryTaskModel >(
        "aaa.ui.qt6", 1, 0, "BinaryTaskModel",
        QStringLiteral( "expose via context property" ) );

    // c152-M : Lua helper -- syntax highlighting attach + linter +
    // asset-type classifier.
    aaa::ui::qt6::LuaHelper luaHelper;
    qmlRegisterUncreatableType< aaa::ui::qt6::LuaHelper >(
        "aaa.ui.qt6", 1, 0, "LuaHelper",
        QStringLiteral( "expose via luaHelper context property" ) );

    // c152-N : user-facing settings (theme, editor font, etc.).
    aaa::ui::qt6::SettingsModel settings;
    qmlRegisterUncreatableType< aaa::ui::qt6::SettingsModel >(
        "aaa.ui.qt6", 1, 0, "SettingsModel",
        QStringLiteral( "expose via settings context property" ) );

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(
        QStringLiteral( "studio" ), &model );
    engine.rootContext()->setContextProperty(
        QStringLiteral( "sound"  ), &sound );
    engine.rootContext()->setContextProperty(
        QStringLiteral( "camera" ), &camera );
    engine.rootContext()->setContextProperty(
        QStringLiteral( "tasks"  ), &tasks );
    engine.rootContext()->setContextProperty(
        QStringLiteral( "viewport" ), &viewport );
    engine.rootContext()->setContextProperty(
        QStringLiteral( "luaHelper" ), &luaHelper );
    engine.rootContext()->setContextProperty(
        QStringLiteral( "settings" ), &settings );

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QGuiApplication::exit( -1 ); },
        Qt::QueuedConnection );

    engine.load( QUrl( QStringLiteral( "qrc:/aaa/ui/qt/qml/Main.qml" ) ) );
    if( engine.rootObjects().isEmpty() )
        return -1;

    return QGuiApplication::exec();
}
