// tests/qt/aaa_settings_model_test.cpp -- c152-N
//
// Coverage : SettingsModel round-trip via QSettings, clamping, and
// resetToDefaults().

#include "src/ui/qt/aaa_settings_model.h"

#include <QCoreApplication>
#include <QSettings>
#include <QSignalSpy>
#include <QtTest/QtTest>

class SettingsModelTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void defaults_areSane();
    void setters_persistAcrossInstances();
    void setters_clampOutOfRangeValues();
    void resetToDefaults_returnsToBaseline();
    void changeSignals_fireOnSetter();
};

void SettingsModelTest::initTestCase()
{
    // Use a temp-org so we don't trash the real user prefs.
    QCoreApplication::setOrganizationName( "SettingsTest" );
    QCoreApplication::setApplicationName( "AAASeedStudioTests" );
    QSettings().clear();
}

void SettingsModelTest::cleanupTestCase()
{
    QSettings().clear();
}

void SettingsModelTest::defaults_areSane()
{
    aaa::ui::qt6::SettingsModel s;
    QCOMPARE( s.theme(), QStringLiteral( "dark" ) );
    QVERIFY( s.editorFontSize() >= 9 && s.editorFontSize() <= 28 );
    QVERIFY( s.editorTabWidth() >= 1 && s.editorTabWidth() <= 8 );
    QVERIFY( s.lintDebounceMs() >= 50 );
    QVERIFY( !s.defaultProjectDir().isEmpty() );
}

void SettingsModelTest::setters_persistAcrossInstances()
{
    {
        aaa::ui::qt6::SettingsModel s;
        s.setTheme( "light" );
        s.setEditorFontSize( 18 );
        s.setEditorTabWidth( 2 );
        s.setEditorWordWrap( true );
        s.setEditorShowLineNumbers( true );
        s.setLintDebounceMs( 500 );
        s.setAutoSave( true );
        s.setAutoSaveIntervalSec( 120 );
        s.setDefaultProjectDir( "/tmp/aaa-test-projects" );
    }
    aaa::ui::qt6::SettingsModel s2;
    QCOMPARE( s2.theme(),                  QStringLiteral( "light" ) );
    QCOMPARE( s2.editorFontSize(),         18 );
    QCOMPARE( s2.editorTabWidth(),         2 );
    QCOMPARE( s2.editorWordWrap(),         true );
    QCOMPARE( s2.editorShowLineNumbers(),  true );
    QCOMPARE( s2.lintDebounceMs(),         500 );
    QCOMPARE( s2.autoSave(),               true );
    QCOMPARE( s2.autoSaveIntervalSec(),    120 );
    QCOMPARE( s2.defaultProjectDir(),      QStringLiteral( "/tmp/aaa-test-projects" ) );
}

void SettingsModelTest::setters_clampOutOfRangeValues()
{
    aaa::ui::qt6::SettingsModel s;
    s.setEditorFontSize( 99 );
    QCOMPARE( s.editorFontSize(), 28 );        // clamped to max
    s.setEditorFontSize( 1 );
    QCOMPARE( s.editorFontSize(), 9 );         // clamped to min
    s.setEditorTabWidth( 99 );
    QCOMPARE( s.editorTabWidth(), 8 );
    s.setLintDebounceMs( 1 );
    QCOMPARE( s.lintDebounceMs(), 50 );
    s.setAutoSaveIntervalSec( 99999 );
    QCOMPARE( s.autoSaveIntervalSec(), 3600 );
}

void SettingsModelTest::resetToDefaults_returnsToBaseline()
{
    aaa::ui::qt6::SettingsModel s;
    s.setEditorFontSize( 22 );
    s.setEditorWordWrap( true );
    s.setAutoSave( true );
    s.resetToDefaults();
    QCOMPARE( s.theme(),          QStringLiteral( "dark" ) );
    QCOMPARE( s.editorFontSize(), 13 );
    QCOMPARE( s.editorWordWrap(), false );
    QCOMPARE( s.autoSave(),       false );
}

void SettingsModelTest::changeSignals_fireOnSetter()
{
    aaa::ui::qt6::SettingsModel s;
    QSignalSpy themeSpy( &s, &aaa::ui::qt6::SettingsModel::themeChanged );
    QSignalSpy fontSpy(  &s, &aaa::ui::qt6::SettingsModel::editorFontSizeChanged );
    s.setTheme( "system" );
    s.setEditorFontSize( 17 );
    QCOMPARE( themeSpy.count(), 1 );
    QCOMPARE( fontSpy.count(),  1 );

    // Setting the SAME value is a no-op (no signal).
    s.setTheme( "system" );
    QCOMPARE( themeSpy.count(), 1 );
}

QTEST_GUILESS_MAIN( SettingsModelTest )
#include "aaa_settings_model_test.moc"
