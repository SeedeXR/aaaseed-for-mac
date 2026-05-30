// src/ui/qt/aaa_native_bridge.h
//
// c154 : Qt bridge that surfaces the c153 wave-2 NATIVE feature sub-libs
// (aaaseed_midi / aaaseed_audio / aaaseed_video / aaaseed_syphon /
// aaaseed_clipboard / aaaseed_net / aaaseed_display) to the QML Studio so the
// intuitive UI and the engine share ONE device truth and the UI can reach
// every native function. This is purely ADDITIVE -- it does NOT touch the
// existing Qt-Multimedia Sound/Camera/Tasks adapters (aaa_panel_models), so
// the 54 shipping Qt::Test cases stay green.
//
// Two QObjects, both exposed as QML context properties (mirroring the
// `sound` / `camera` / `tasks` pattern in aaa_qt_main.cpp) :
//
//   nativeDevices  (NativeDevicesModel)     -- read-mostly enumeration of the
//                  native MIDI / audio / video / Syphon devices + clipboard +
//                  async HTTP, surfaced in the new "Devices" panel.
//
//   nativeDisplay  (NativeDisplayController) -- the "Display" menu backend :
//                  choose the engine output surface (Intuitive in-Studio
//                  preview vs the Native macOS window, which carries the
//                  multi-display span + zero-copy video features), persisted
//                  via QSettings, and launch the native runtime on demand.
//
// Doctrine : the adapter is plain C++ (like aaa_panel_models.cpp). Every
// native sub-lib it calls has a C++-clean header (PIMPL / std:: types), so no
// ObjC leaks into this TU ; the .mm lives inside the sub-libs. Threading : the
// async net completion is marshalled back onto the Qt thread via
// QMetaObject::invokeMethod before any signal is emitted.

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include <cstdint>

namespace aaa { namespace ui { namespace qt6 {

// --- NativeDevicesModel : enumeration + actions over the c153 sub-libs -----
class NativeDevicesModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QStringList midiInputs    READ midiInputs    NOTIFY changed )
    Q_PROPERTY( QStringList midiOutputs   READ midiOutputs   NOTIFY changed )
    Q_PROPERTY( QStringList audioOutputs  READ audioOutputs  NOTIFY changed )
    Q_PROPERTY( QStringList audioInputs   READ audioInputs   NOTIFY changed )
    Q_PROPERTY( QStringList videoDevices  READ videoDevices  NOTIFY changed )
    Q_PROPERTY( QStringList syphonServers READ syphonServers NOTIFY changed )
    Q_PROPERTY( int         screenCount   READ screenCount   NOTIFY changed )
public:
    explicit NativeDevicesModel( QObject* parent = nullptr );
    ~NativeDevicesModel() override;

    QStringList midiInputs()    const { return midi_inputs_;    }
    QStringList midiOutputs()   const { return midi_outputs_;   }
    QStringList audioOutputs()  const { return audio_outputs_;  }
    QStringList audioInputs()   const { return audio_inputs_;   }
    QStringList videoDevices()  const { return video_devices_;  }
    QStringList syphonServers() const { return syphon_servers_; }
    int         screenCount()   const { return screen_count_;   }

public slots:
    // Re-enumerate every native subsystem. Safe to call repeatedly ; never
    // touches the camera/mic (enumeration only -- no TCC prompt).
    void refresh();
    // Poll the Syphon Distributed-Notification bus for advertising servers.
    void refreshSyphon();
    // Clipboard round-trip via aaaseed_clipboard (NSPasteboard).
    void clipboardCopy( QString const& text );
    QString clipboardPaste();
    // Async HTTP GET via aaaseed_net (NSURLSession). Result is marshalled
    // back onto the Qt thread and delivered via httpResult().
    void httpGet( QString const& url );

signals:
    void changed();
    void logLine( int level, QString const& text );
    void httpResult( int status, QString const& body );

private:
    QStringList midi_inputs_;
    QStringList midi_outputs_;
    QStringList audio_outputs_;
    QStringList audio_inputs_;
    QStringList video_devices_;
    QStringList syphon_servers_;
    int         screen_count_  = 0;
    bool        midi_ready_    = false;
    void*       syphon_dir_    = nullptr;   // aaa::syphon::DirectoryMac* (opaque)
};

// --- NativeDisplayController : the "Display" menu backend -------------------
class NativeDisplayController : public QObject
{
    Q_OBJECT
    // "intuitive" (embedded Studio Engine Preview, default) | "native"
    // (standalone native macOS window via aaaseed_runtime, carrying the
    // multi-display span + zero-copy video features).
    Q_PROPERTY( QString engineDisplayMode READ engineDisplayMode
                WRITE setEngineDisplayMode NOTIFY engineDisplayModeChanged )
    Q_PROPERTY( bool multiDisplaySpan READ multiDisplaySpan
                WRITE setMultiDisplaySpan NOTIFY multiDisplaySpanChanged )
    Q_PROPERTY( int  screenCount READ screenCount NOTIFY screenCountChanged )
    Q_PROPERTY( bool nativeActive READ nativeActive NOTIFY nativeActiveChanged )
public:
    explicit NativeDisplayController( QObject* parent = nullptr );

    QString engineDisplayMode() const { return mode_; }
    bool    multiDisplaySpan()  const { return span_; }
    int     screenCount()       const { return screen_count_; }
    bool    nativeActive()      const { return native_active_; }

public slots:
    void setEngineDisplayMode( QString const& mode );   // "intuitive"|"native"
    void setMultiDisplaySpan( bool on );
    void refreshScreens();
    // Launch the native engine window (aaaseed_runtime) with the
    // AAASEED_MULTIDISPLAY env set per multiDisplaySpan. Returns false (and
    // logs) if the runtime bundle can't be located -- never crashes.
    bool launchNativeDisplay();

signals:
    void engineDisplayModeChanged();
    void multiDisplaySpanChanged();
    void screenCountChanged();
    void nativeActiveChanged();
    void logLine( int level, QString const& text );

private:
    QString mode_  = QStringLiteral( "intuitive" );
    bool    span_  = false;
    int     screen_count_  = 0;
    bool    native_active_ = false;

    QString locateRuntimeApp() const;   // "" if not found
    void    load();                     // QSettings restore
    void    save() const;               // QSettings persist
};

} } } // namespace aaa::ui::qt6
