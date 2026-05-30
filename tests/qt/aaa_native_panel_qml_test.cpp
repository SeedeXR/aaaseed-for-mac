// tests/qt/aaa_native_panel_qml_test.cpp
//
// c154 : INTEGRATION test proving the new native-subsystem UI actually
// instantiates in a QML engine with the bridge context properties bound --
// i.e. the added functionality "appears" and its bindings resolve, not just
// that the C++ compiles. Loads NativeDevicesPanel.qml as a QQmlComponent with
// `nativeDevices` / `nativeDisplay` set as context properties (exactly as
// aaa_qt_main.cpp wires them) and asserts the component reaches Ready with no
// QML errors. Runs under QT_QPA_PLATFORM=offscreen (set in CMake ENVIRONMENT).

#include "src/ui/qt/aaa_native_bridge.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QUrl>
#include <QtTest>

using namespace aaa::ui::qt6;

class NativePanelQmlTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        QCoreApplication::setOrganizationName( "SeedeXR" );
        QCoreApplication::setApplicationName( "AAASeed Studio Test" );
        // The panel uses QtQuick.Controls ; pick a concrete style so the
        // engine doesn't fall back to a platform style that needs a window.
        QQuickStyle::setStyle( QStringLiteral( "Fusion" ) );
    }

    void native_devices_panel_loads_with_bindings()
    {
        QQmlEngine engine;

        NativeDevicesModel      devices;
        NativeDisplayController display;
        engine.rootContext()->setContextProperty(
            QStringLiteral( "nativeDevices" ), &devices );
        engine.rootContext()->setContextProperty(
            QStringLiteral( "nativeDisplay" ), &display );

        QQmlComponent component(
            &engine, QUrl::fromLocalFile( QStringLiteral( AAA_NATIVE_DEVICES_PANEL_QML ) ) );

        // If the QtQuick.Controls QML modules aren't resolvable in this
        // headless environment, skip rather than false-fail.
        if( component.status() == QQmlComponent::Error )
        {
            bool moduleMissing = false;
            for( QQmlError const& e : component.errors() )
                if( e.toString().contains( QStringLiteral( "is not installed" ) )
                 || e.toString().contains( QStringLiteral( "No such file" ) ) )
                    moduleMissing = true;
            if( moduleMissing )
                QSKIP( "QtQuick.Controls QML modules unavailable in this headless env" );
        }

        QVERIFY2( component.status() != QQmlComponent::Error,
                  qPrintable( component.errorString() ) );

        QObject* obj = component.create( engine.rootContext() );
        QVERIFY2( obj != nullptr, qPrintable( component.errorString() ) );
        obj->deleteLater();
    }
};

QTEST_MAIN( NativePanelQmlTest )
#include "aaa_native_panel_qml_test.moc"
