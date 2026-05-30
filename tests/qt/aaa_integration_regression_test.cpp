// tests/qt/aaa_integration_regression_test.cpp
//
// c154 : REGRESSION guards for the native-feature integration. These are
// cheap, deterministic file-content checks that fail loudly if a future edit
// silently drops:
//   1. the camera/microphone usage descriptions from the bundle Info.plist
//      template (without which the shipped DMG app is killed by macOS TCC on
//      camera/mic access and never prompts) ;
//   2. the "Display" menu or the native "Devices" tab/panel wiring from the
//      QML Studio ;
//   3. the NativeDevicesPanel.qml entry from the Qt resource (.qrc).
//
// Paths are injected via compile definitions (see tests/qt/CMakeLists.txt).

#include <QFile>
#include <QString>
#include <QtTest>

namespace {
QString readAll( char const* path )
{
    QFile f( QString::fromUtf8( path ) );
    if( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return QString();
    return QString::fromUtf8( f.readAll() );
}
}

class IntegrationRegressionTest : public QObject
{
    Q_OBJECT
private slots:
    void info_plist_template_declares_camera_and_mic()
    {
        QString const plist = readAll( AAA_INFO_PLIST_IN );
        QVERIFY2( !plist.isEmpty(), "Info.plist.in not readable" );
        QVERIFY2( plist.contains( QStringLiteral( "NSCameraUsageDescription" ) ),
                  "Info.plist.in must declare NSCameraUsageDescription" );
        QVERIFY2( plist.contains( QStringLiteral( "NSMicrophoneUsageDescription" ) ),
                  "Info.plist.in must declare NSMicrophoneUsageDescription" );
    }

    void entitlements_grant_camera_and_audio_input()
    {
        QString const ent = readAll( AAA_ENTITLEMENTS_PLIST );
        QVERIFY2( !ent.isEmpty(), "entitlements.plist not readable" );
        QVERIFY( ent.contains( QStringLiteral( "com.apple.security.device.camera" ) ) );
        QVERIFY( ent.contains( QStringLiteral( "com.apple.security.device.audio-input" ) ) );
    }

    void qt_cmake_wires_info_plist_into_studio()
    {
        // The Studio app MUST consume the canonical template, else its
        // auto-generated plist omits the usage strings.
        QString const cmake = readAll( AAA_QT_CMAKELISTS );
        QVERIFY2( cmake.contains( QStringLiteral( "MACOSX_BUNDLE_INFO_PLIST" ) ),
                  "aaaseed_app_qt must set MACOSX_BUNDLE_INFO_PLIST" );
        QVERIFY( cmake.contains( QStringLiteral( "Info.qt.plist" ) ) );
    }

    void main_qml_has_display_menu_and_devices_tab()
    {
        QString const qml = readAll( AAA_MAIN_QML );
        QVERIFY2( !qml.isEmpty(), "Main.qml not readable" );
        QVERIFY2( qml.contains( QStringLiteral( "title: qsTr(\"Display\")" ) ),
                  "Main.qml must define the Display menu" );
        QVERIFY2( qml.contains( QStringLiteral( "nativeDisplay.engineDisplayMode" ) ),
                  "Display menu must bind nativeDisplay.engineDisplayMode" );
        QVERIFY2( qml.contains( QStringLiteral( "multiDisplaySpan" ) ),
                  "Display menu must expose the multi-display span toggle" );
        QVERIFY2( qml.contains( QStringLiteral( "NativeDevicesPanel" ) ),
                  "Main.qml must host the NativeDevicesPanel (Devices tab)" );
        QVERIFY2( qml.contains( QStringLiteral( "TabButton { text: qsTr(\"Devices\") }" ) ),
                  "Main.qml must add the Devices tab button" );
    }

    void qrc_lists_native_devices_panel()
    {
        QString const qrc = readAll( AAA_QT_QRC );
        QVERIFY2( qrc.contains( QStringLiteral( "qml/panels/NativeDevicesPanel.qml" ) ),
                  ".qrc must bundle NativeDevicesPanel.qml" );
    }

    void main_cpp_registers_bridge_context_properties()
    {
        QString const cpp = readAll( AAA_QT_MAIN_CPP );
        QVERIFY( cpp.contains( QStringLiteral( "\"nativeDevices\"" ) ) );
        QVERIFY( cpp.contains( QStringLiteral( "\"nativeDisplay\"" ) ) );
    }
};

QTEST_GUILESS_MAIN( IntegrationRegressionTest )
#include "aaa_integration_regression_test.moc"
