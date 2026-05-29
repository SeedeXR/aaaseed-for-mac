// src/ui/qt/aaa_settings_model.cpp -- c152-N

#include "src/ui/qt/aaa_settings_model.h"

#include <QStandardPaths>

namespace aaa { namespace ui { namespace qt6 {

SettingsModel::SettingsModel( QObject* parent )
    : QObject( parent )
{
    // Sensible default : ~/Documents
    default_project_dir_ = QStandardPaths::writableLocation(
        QStandardPaths::DocumentsLocation );
    load();
}

void SettingsModel::load()
{
    QSettings s;
    s.beginGroup( QStringLiteral( "ui" ) );
    theme_                 = s.value( "theme",            theme_                ).toString();
    editor_font_size_      = s.value( "editorFontSize",   editor_font_size_     ).toInt();
    editor_tab_width_      = s.value( "editorTabWidth",   editor_tab_width_     ).toInt();
    editor_word_wrap_      = s.value( "editorWordWrap",   editor_word_wrap_     ).toBool();
    editor_show_line_numbers_ = s.value( "editorShowLineNumbers",
                                          editor_show_line_numbers_ ).toBool();
    lint_debounce_ms_      = s.value( "lintDebounceMs",   lint_debounce_ms_     ).toInt();
    auto_save_             = s.value( "autoSave",         auto_save_            ).toBool();
    auto_save_interval_sec_ = s.value( "autoSaveIntervalSec",
                                        auto_save_interval_sec_ ).toInt();
    default_project_dir_   = s.value( "defaultProjectDir", default_project_dir_ ).toString();
    s.endGroup();
}

void SettingsModel::save()
{
    QSettings s;
    s.beginGroup( QStringLiteral( "ui" ) );
    s.setValue( "theme",                  theme_ );
    s.setValue( "editorFontSize",         editor_font_size_ );
    s.setValue( "editorTabWidth",         editor_tab_width_ );
    s.setValue( "editorWordWrap",         editor_word_wrap_ );
    s.setValue( "editorShowLineNumbers",  editor_show_line_numbers_ );
    s.setValue( "lintDebounceMs",         lint_debounce_ms_ );
    s.setValue( "autoSave",               auto_save_ );
    s.setValue( "autoSaveIntervalSec",    auto_save_interval_sec_ );
    s.setValue( "defaultProjectDir",      default_project_dir_ );
    s.endGroup();
}

// ── Setter boilerplate with clamping where needed ────────────────────────

void SettingsModel::setTheme( QString const& v )
{
    if( v == theme_ ) return;
    theme_ = v;
    save();
    emit themeChanged();
}

void SettingsModel::setEditorFontSize( int v )
{
    v = qBound( 9, v, 28 );
    if( v == editor_font_size_ ) return;
    editor_font_size_ = v;
    save();
    emit editorFontSizeChanged();
}

void SettingsModel::setEditorTabWidth( int v )
{
    v = qBound( 1, v, 8 );
    if( v == editor_tab_width_ ) return;
    editor_tab_width_ = v;
    save();
    emit editorTabWidthChanged();
}

void SettingsModel::setEditorWordWrap( bool v )
{
    if( v == editor_word_wrap_ ) return;
    editor_word_wrap_ = v;
    save();
    emit editorWordWrapChanged();
}

void SettingsModel::setEditorShowLineNumbers( bool v )
{
    if( v == editor_show_line_numbers_ ) return;
    editor_show_line_numbers_ = v;
    save();
    emit editorShowLineNumbersChanged();
}

void SettingsModel::setLintDebounceMs( int v )
{
    v = qBound( 50, v, 2000 );
    if( v == lint_debounce_ms_ ) return;
    lint_debounce_ms_ = v;
    save();
    emit lintDebounceMsChanged();
}

void SettingsModel::setAutoSave( bool v )
{
    if( v == auto_save_ ) return;
    auto_save_ = v;
    save();
    emit autoSaveChanged();
}

void SettingsModel::setAutoSaveIntervalSec( int v )
{
    v = qBound( 5, v, 3600 );
    if( v == auto_save_interval_sec_ ) return;
    auto_save_interval_sec_ = v;
    save();
    emit autoSaveIntervalSecChanged();
}

void SettingsModel::setDefaultProjectDir( QString const& v )
{
    if( v == default_project_dir_ ) return;
    default_project_dir_ = v;
    save();
    emit defaultProjectDirChanged();
}

void SettingsModel::resetToDefaults()
{
    // Use hardcoded defaults rather than a temp instance (which would
    // re-load() from QSettings -- i.e. the very values we want to
    // discard).
    setTheme(            QStringLiteral( "dark" ) );
    setEditorFontSize(   13 );
    setEditorTabWidth(   4 );
    setEditorWordWrap(   false );
    setEditorShowLineNumbers( false );
    setLintDebounceMs(   350 );
    setAutoSave(         false );
    setAutoSaveIntervalSec( 60 );
    setDefaultProjectDir( QStandardPaths::writableLocation(
        QStandardPaths::DocumentsLocation ) );
}

} } } // namespace aaa::ui::qt6
