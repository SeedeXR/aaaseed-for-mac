// src/ui/qt/aaa_native_bridge.cpp
//
// c154 : implementation of the Qt <-> native-sub-lib bridge. See
// aaa_native_bridge.h for scope + doctrine. Plain C++ : every native header
// included here is C++-clean (PIMPL / std::), so no ObjC leaks into this TU.

#include "src/ui/qt/aaa_native_bridge.h"

#include "src/midi/midi_mac.h"
#include "src/audio/audio_mac.h"
#include "src/video/capture_mac.h"
#include "src/syphon/syphon_directory_mac.h"
#include "src/clipboard/clipboard_mac.h"
#include "src/net/net_mac.h"
#include "src/display/display_mac.h"
#include "src/display/display_layout.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QProcess>
#include <QProcessEnvironment>
#include <QSettings>

#include <string>
#include <vector>

namespace aaa { namespace ui { namespace qt6 {

namespace {
QString qs( std::string const& s ) { return QString::fromUtf8( s.c_str(), int( s.size() ) ); }
}

// --- NativeDevicesModel -----------------------------------------------------

NativeDevicesModel::NativeDevicesModel( QObject* parent )
    : QObject( parent )
{
    midi_ready_ = aaa::midi::init();
    syphon_dir_ = new aaa::syphon::DirectoryMac();
    refresh();
}

NativeDevicesModel::~NativeDevicesModel()
{
    delete static_cast< aaa::syphon::DirectoryMac* >( syphon_dir_ );
    syphon_dir_ = nullptr;
    if( midi_ready_ )
        aaa::midi::deinit();
}

void NativeDevicesModel::refresh()
{
    midi_inputs_.clear();
    midi_outputs_.clear();
    audio_outputs_.clear();
    audio_inputs_.clear();
    video_devices_.clear();

    if( midi_ready_ )
    {
        std::size_t const ni = aaa::midi::get_in_device_count();
        for( std::size_t i = 0; i < ni; ++i )
            midi_inputs_ << qs( aaa::midi::get_in_device_name( i ) );
        std::size_t const no = aaa::midi::get_out_device_count();
        for( std::size_t i = 0; i < no; ++i )
            midi_outputs_ << qs( aaa::midi::get_out_device_name( i ) );
    }

    std::size_t const ao = aaa::audio::get_output_device_count();
    for( std::size_t i = 0; i < ao; ++i )
        audio_outputs_ << qs( aaa::audio::get_output_device_name( i ) );
    std::size_t const ai = aaa::audio::get_input_device_count();
    for( std::size_t i = 0; i < ai; ++i )
        audio_inputs_ << qs( aaa::audio::get_input_device_name( i ) );

    std::size_t const vn = aaa::video::Capture::get_device_count();
    for( std::size_t i = 0; i < vn; ++i )
        video_devices_ << qs( aaa::video::Capture::get_device_name( i ) );

    screen_count_ = int( aaa::display::enumerate_screens().size() );

    refreshSyphon();   // also emits changed()
    emit logLine( 0, QStringLiteral( "Native devices refreshed: %1 MIDI-in, "
                                     "%2 audio-out, %3 video, %4 screens" )
                         .arg( midi_inputs_.size() )
                         .arg( audio_outputs_.size() )
                         .arg( video_devices_.size() )
                         .arg( screen_count_ ) );
}

void NativeDevicesModel::refreshSyphon()
{
    syphon_servers_.clear();
    if( syphon_dir_ )
    {
        auto* dir = static_cast< aaa::syphon::DirectoryMac* >( syphon_dir_ );
        dir->poll( 0.1 );
        for( auto const& name : dir->server_names() )
            syphon_servers_ << qs( name );
    }
    emit changed();
}

void NativeDevicesModel::clipboardCopy( QString const& text )
{
    std::string const utf8 = text.toStdString();
    if( aaa::clipboard::copy( utf8 ) )
        emit logLine( 0, QStringLiteral( "Copied %1 chars to clipboard" )
                             .arg( text.size() ) );
    else
        emit logLine( 2, QStringLiteral( "Clipboard copy failed" ) );
}

QString NativeDevicesModel::clipboardPaste()
{
    return qs( aaa::clipboard::paste() );
}

void NativeDevicesModel::httpGet( QString const& url )
{
    QPointer< NativeDevicesModel > guard( this );
    std::string const u = url.toStdString();
    emit logLine( 0, QStringLiteral( "HTTP GET %1 ..." ).arg( url ) );
    aaa::net::get( u, [guard]( long status, std::string body ) {
        // The completion fires on an NSURLSession background queue. Marshal
        // back onto the Qt thread before emitting any signal.
        QString const qbody = QString::fromUtf8( body.c_str(), int( body.size() ) );
        int const     code  = int( status );
        QMetaObject::invokeMethod(
            qApp,
            [guard, code, qbody]() {
                if( guard )
                    emit guard->httpResult( code, qbody );
            },
            Qt::QueuedConnection );
    } );
}

// --- NativeDisplayController ------------------------------------------------

NativeDisplayController::NativeDisplayController( QObject* parent )
    : QObject( parent )
{
    load();
    refreshScreens();
}

void NativeDisplayController::setEngineDisplayMode( QString const& mode )
{
    QString const m = ( mode == QStringLiteral( "native" ) )
                          ? QStringLiteral( "native" )
                          : QStringLiteral( "intuitive" );
    if( m == mode_ )
        return;
    mode_ = m;
    save();
    emit engineDisplayModeChanged();
    emit logLine( 0, QStringLiteral( "Engine display mode: %1" ).arg( mode_ ) );
}

void NativeDisplayController::setMultiDisplaySpan( bool on )
{
    if( on == span_ )
        return;
    span_ = on;
    save();
    emit multiDisplaySpanChanged();
}

void NativeDisplayController::refreshScreens()
{
    int const n = int( aaa::display::enumerate_screens().size() );
    if( n != screen_count_ )
    {
        screen_count_ = n;
        emit screenCountChanged();
    }
}

QString NativeDisplayController::locateRuntimeApp() const
{
    QString const dir = QCoreApplication::applicationDirPath();
    QStringList const candidates = {
        // Inside a shipped Studio bundle : Contents/MacOS -> Contents/Resources/runtime
        dir + QStringLiteral( "/../Resources/runtime/aaaseed_runtime.app/Contents/MacOS/aaaseed_runtime" ),
        // Dev tree : sibling .app in the same bin/ dir (Studio.app/Contents/MacOS -> bin)
        dir + QStringLiteral( "/../../../aaaseed_runtime.app/Contents/MacOS/aaaseed_runtime" ),
        // Plain sibling binary next to the Studio executable.
        dir + QStringLiteral( "/aaaseed_runtime" ),
    };
    for( QString const& c : candidates )
    {
        QFileInfo fi( c );
        if( fi.exists() && fi.isFile() )
            return fi.absoluteFilePath();
    }
    return QString();
}

bool NativeDisplayController::launchNativeDisplay()
{
    QString const exe = locateRuntimeApp();
    if( exe.isEmpty() )
    {
        emit logLine( 2, QStringLiteral(
            "Native display: aaaseed_runtime not found next to the Studio. "
            "Build the runtime target or run from a packaged DMG." ) );
        return false;
    }

    QProcess proc;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if( span_ )
        env.insert( QStringLiteral( "AAASEED_MULTIDISPLAY" ), QStringLiteral( "1" ) );
    proc.setProcessEnvironment( env );
    proc.setProgram( exe );

    bool const ok = proc.startDetached();
    if( ok )
    {
        native_active_ = true;
        emit nativeActiveChanged();
        emit logLine( 0, QStringLiteral( "Launched native engine display%1" )
                             .arg( span_ ? QStringLiteral( " (multi-display span)" )
                                         : QString() ) );
    }
    else
    {
        emit logLine( 2, QStringLiteral( "Native display: failed to launch %1" ).arg( exe ) );
    }
    return ok;
}

void NativeDisplayController::load()
{
    QSettings s;
    s.beginGroup( QStringLiteral( "display" ) );
    mode_ = s.value( QStringLiteral( "engineDisplayMode" ),
                     QStringLiteral( "intuitive" ) ).toString();
    if( mode_ != QStringLiteral( "native" ) )
        mode_ = QStringLiteral( "intuitive" );
    span_ = s.value( QStringLiteral( "multiDisplaySpan" ), false ).toBool();
    s.endGroup();
}

void NativeDisplayController::save() const
{
    QSettings s;
    s.beginGroup( QStringLiteral( "display" ) );
    s.setValue( QStringLiteral( "engineDisplayMode" ), mode_ );
    s.setValue( QStringLiteral( "multiDisplaySpan" ), span_ );
    s.endGroup();
}

} } } // namespace aaa::ui::qt6
