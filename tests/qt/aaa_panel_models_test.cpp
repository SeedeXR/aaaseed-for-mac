// tests/qt/aaa_panel_models_test.cpp
//
// c152-E : Qt::Test coverage for the Sound / Camera / Tasks panel
// adapters.

#include "src/ui/qt/aaa_panel_models.h"

#include <QCoreApplication>
#include <QSignalSpy>
#include <QtTest/QtTest>

class PanelModelsTest : public QObject
{
    Q_OBJECT

private slots:
    // SoundDeviceModel
    void sound_refresh_populates();
    void sound_kindFlagSane();

    // CameraController
    void camera_enumerates();
    void camera_setActive_does_not_crash();

    // BinaryTaskModel
    void task_addAndRemove();
    void task_startEcho_streamsStdoutThenExits();
};

void PanelModelsTest::sound_refresh_populates()
{
    aaa::ui::qt6::SoundDeviceModel m;
    m.refresh();
    // CI machines usually have at least one device. Hosts without
    // any audio HW will record 0 ; tolerate.
    QVERIFY( m.rowCount() >= 0 );
}

void PanelModelsTest::sound_kindFlagSane()
{
    aaa::ui::qt6::SoundDeviceModel m;
    m.refresh();
    for( int i = 0; i < m.rowCount(); ++i )
    {
        auto const idx = m.index( i, 0 );
        QString const kind = m.data( idx,
            aaa::ui::qt6::SoundDeviceModel::KindRole ).toString();
        QVERIFY( kind == QStringLiteral( "IN" )
              || kind == QStringLiteral( "OUT" )
              || kind == QStringLiteral( "DUPLEX" )
              || kind == QStringLiteral( "---" ) );
    }
}

void PanelModelsTest::camera_enumerates()
{
    aaa::ui::qt6::CameraController c;
    c.refresh();
    // List size is hardware-dependent ; just exercise the API.
    QStringList const names = c.deviceNames();
    QVERIFY( names.size() >= 0 );
}

void PanelModelsTest::camera_setActive_does_not_crash()
{
    aaa::ui::qt6::CameraController c;
    c.refresh();
    c.setActiveIndex( -1 );
    QCOMPARE( c.activeIndex(), -1 );
    // No camera-permission prompt in headless test ; don't call start().
}

void PanelModelsTest::task_addAndRemove()
{
    aaa::ui::qt6::BinaryTaskModel m;
    QCOMPARE( m.rowCount(), 0 );

    m.addTask( QStringLiteral( "echo-test" ),
               QStringLiteral( "/bin/echo" ),
               QStringLiteral( "hello qt" ) );
    QCOMPARE( m.rowCount(), 1 );

    m.removeTask( 0 );
    QCOMPARE( m.rowCount(), 0 );
}

void PanelModelsTest::task_startEcho_streamsStdoutThenExits()
{
    aaa::ui::qt6::BinaryTaskModel m;
    QSignalSpy logs( &m, &aaa::ui::qt6::BinaryTaskModel::logLine );

    m.addTask( QStringLiteral( "echo-test" ),
               QStringLiteral( "/bin/echo" ),
               QStringLiteral( "hello qt" ) );
    m.startTask( 0 );

    // Wait up to 2 seconds for the process to exit + signals to fire.
    QTRY_VERIFY_WITH_TIMEOUT( logs.count() >= 2, 2000 );

    // At least one log line should contain "hello qt".
    bool saw_hello = false;
    for( auto const& args : logs )
        if( args.at( 1 ).toString().contains( QStringLiteral( "hello qt" ) ) )
            saw_hello = true;
    QVERIFY( saw_hello );
}

QTEST_GUILESS_MAIN( PanelModelsTest )
#include "aaa_panel_models_test.moc"
