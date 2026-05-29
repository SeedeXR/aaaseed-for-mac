// src/ui/qt/aaa_settings_model.h -- c152-N
//
// User-facing app settings : theme, editor font, lint debounce, auto-
// save interval, default project location. Backed by QSettings under
// the "ui/" group ; QML reads + writes via Q_PROPERTY bindings.

#pragma once

#include <QObject>
#include <QSettings>
#include <QString>

namespace aaa { namespace ui { namespace qt6 {

class SettingsModel : public QObject
{
    Q_OBJECT

    // Visual theme. v1 ships "dark" only ; "light" / "system" reserved
    // for a future light-mode pass.
    Q_PROPERTY( QString theme           READ theme           WRITE setTheme           NOTIFY themeChanged )
    Q_PROPERTY( int     editorFontSize  READ editorFontSize  WRITE setEditorFontSize  NOTIFY editorFontSizeChanged )
    Q_PROPERTY( int     editorTabWidth  READ editorTabWidth  WRITE setEditorTabWidth  NOTIFY editorTabWidthChanged )
    Q_PROPERTY( bool    editorWordWrap  READ editorWordWrap  WRITE setEditorWordWrap  NOTIFY editorWordWrapChanged )
    Q_PROPERTY( bool    editorShowLineNumbers READ editorShowLineNumbers
                                              WRITE setEditorShowLineNumbers
                                              NOTIFY editorShowLineNumbersChanged )
    Q_PROPERTY( int     lintDebounceMs  READ lintDebounceMs  WRITE setLintDebounceMs  NOTIFY lintDebounceMsChanged )
    Q_PROPERTY( bool    autoSave        READ autoSave        WRITE setAutoSave        NOTIFY autoSaveChanged )
    Q_PROPERTY( int     autoSaveIntervalSec
                                READ autoSaveIntervalSec
                                WRITE setAutoSaveIntervalSec
                                NOTIFY autoSaveIntervalSecChanged )
    Q_PROPERTY( QString defaultProjectDir
                                READ defaultProjectDir
                                WRITE setDefaultProjectDir
                                NOTIFY defaultProjectDirChanged )

public:
    explicit SettingsModel( QObject* parent = nullptr );

    QString theme()                  const { return theme_; }
    int     editorFontSize()         const { return editor_font_size_; }
    int     editorTabWidth()         const { return editor_tab_width_; }
    bool    editorWordWrap()         const { return editor_word_wrap_; }
    bool    editorShowLineNumbers()  const { return editor_show_line_numbers_; }
    int     lintDebounceMs()         const { return lint_debounce_ms_; }
    bool    autoSave()               const { return auto_save_; }
    int     autoSaveIntervalSec()    const { return auto_save_interval_sec_; }
    QString defaultProjectDir()      const { return default_project_dir_; }

    void setTheme(            QString const& v );
    void setEditorFontSize(   int v );
    void setEditorTabWidth(   int v );
    void setEditorWordWrap(   bool v );
    void setEditorShowLineNumbers( bool v );
    void setLintDebounceMs(   int v );
    void setAutoSave(         bool v );
    void setAutoSaveIntervalSec( int v );
    void setDefaultProjectDir( QString const& v );

    // Resets ALL settings to defaults. Persists immediately.
    Q_INVOKABLE void resetToDefaults();

signals:
    void themeChanged();
    void editorFontSizeChanged();
    void editorTabWidthChanged();
    void editorWordWrapChanged();
    void editorShowLineNumbersChanged();
    void lintDebounceMsChanged();
    void autoSaveChanged();
    void autoSaveIntervalSecChanged();
    void defaultProjectDirChanged();

private:
    void load();
    void save();

    QString theme_                 = QStringLiteral( "dark" );
    int     editor_font_size_      = 13;
    int     editor_tab_width_      = 4;
    bool    editor_word_wrap_      = false;
    bool    editor_show_line_numbers_ = false;
    int     lint_debounce_ms_      = 350;
    bool    auto_save_             = false;
    int     auto_save_interval_sec_ = 60;
    QString default_project_dir_;
};

} } } // namespace aaa::ui::qt6
