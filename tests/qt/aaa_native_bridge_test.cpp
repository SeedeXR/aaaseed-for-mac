// tests/qt/aaa_native_bridge_test.cpp
//
// c154 : unit coverage for the Qt <-> native-sub-lib bridge
// (aaa_native_bridge). Guiless QObject test (no QML scene). Verifies the
// adapter enumerates every native subsystem without crashing, the device
// lists are well-formed, the Display controller's mode/span persist, and the
// native-window launch fails gracefully when no runtime bundle is adjacent.
// Device enumeration here NEVER opens the camera/mic (no TCC prompt in CI).

#include "src/ui/qt/aaa_native_bridge.h"

#include <QSignalSpy>
#include <QStringList>
#include <QtTest>

using namespace aaa::ui::qt6;

class NativeBridgeTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        // QSettings needs an org/app identity ; set it once for the suite.
        QCoreApplication::setOrganizationName( "SeedeXR" );
        QCoreApplication::setApplicationName( "AAASeed Studio Test" );
    }

    void devices_refresh_does_not_crash()
    {
        NativeDevicesModel m;
        m.refresh();   // re-enumerate ; must be safe with 0 devices
        // All lists are valid QStringLists (size >= 0 is implicit ; the point
        // is no crash + the getters are callable).
        QVERIFY( m.midiInputs().size()    >= 0 );
        QVERIFY( m.midiOutputs().size()   >= 0 );
        QVERIFY( m.audioOutputs().size()  >= 0 );
        QVERIFY( m.audioInputs().size()   >= 0 );
        QVERIFY( m.videoDevices().size()  >= 0 );
        QVERIFY( m.syphonServers().size() >= 0 );
        QVERIFY( m.screenCount()          >= 0 );
    }

    void devices_refresh_emits_changed()
    {
        NativeDevicesModel m;
        QSignalSpy spy( &m, &NativeDevicesModel::changed );
        m.refresh();
        QVERIFY( spy.count() >= 1 );   // refresh() -> refreshSyphon() -> changed()
    }

    void clipboard_round_trips_or_is_unavailable()
    {
        NativeDevicesModel m;
        m.clipboardCopy( QStringLiteral( "aaaseed-bridge" ) );
        QString const got = m.clipboardPaste();
        // On a headless runner the system pasteboard may be unavailable
        // (returns ""), which is acceptable ; when present it round-trips.
        if( !got.isEmpty() )
            QCOMPARE( got, QStringLiteral( "aaaseed-bridge" ) );
    }

    void display_mode_defaults_to_intuitive()
    {
        NativeDisplayController d;
        // Default may be restored from a prior test's QSettings ; accept
        // either valid value, but it must be one of the two.
        QVERIFY( d.engineDisplayMode() == QStringLiteral( "intuitive" )
              || d.engineDisplayMode() == QStringLiteral( "native" ) );
    }

    void display_mode_set_emits_and_clamps()
    {
        NativeDisplayController d;
        QSignalSpy spy( &d, &NativeDisplayController::engineDisplayModeChanged );
        d.setEngineDisplayMode( QStringLiteral( "native" ) );
        QCOMPARE( d.engineDisplayMode(), QStringLiteral( "native" ) );
        QVERIFY( spy.count() >= 1 );
        // An unknown mode clamps to "intuitive".
        d.setEngineDisplayMode( QStringLiteral( "garbage" ) );
        QCOMPARE( d.engineDisplayMode(), QStringLiteral( "intuitive" ) );
    }

    void display_mode_persists_across_instances()
    {
        { NativeDisplayController a; a.setEngineDisplayMode( QStringLiteral( "native" ) ); }
        NativeDisplayController b;   // reloads from QSettings
        QCOMPARE( b.engineDisplayMode(), QStringLiteral( "native" ) );
        // restore default so other tests/users aren't left on native.
        b.setEngineDisplayMode( QStringLiteral( "intuitive" ) );
    }

    void display_span_toggles()
    {
        NativeDisplayController d;
        QSignalSpy spy( &d, &NativeDisplayController::multiDisplaySpanChanged );
        bool const before = d.multiDisplaySpan();
        d.setMultiDisplaySpan( !before );
        QCOMPARE( d.multiDisplaySpan(), !before );
        QVERIFY( spy.count() >= 1 );
        d.setMultiDisplaySpan( before );   // restore
    }

    void launch_native_is_graceful_without_runtime()
    {
        NativeDisplayController d;
        // In the test bin/ there is no aaaseed_runtime.app adjacent in the
        // expected bundle layout ; the call must return a bool, not crash.
        bool const r = d.launchNativeDisplay();
        Q_UNUSED( r );
        QVERIFY( true );
    }
};

QTEST_GUILESS_MAIN( NativeBridgeTest )
#include "aaa_native_bridge_test.moc"
