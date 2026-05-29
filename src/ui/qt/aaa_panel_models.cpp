// src/ui/qt/aaa_panel_models.cpp
//
// c152-D : Sound + Camera + Binary Manager implementations.

#include "src/ui/qt/aaa_panel_models.h"

#include <QAudioFormat>
#include <QBuffer>
#include <QByteArray>
#include <QCameraDevice>
#include <QFileInfo>
#include <QMediaDevices>

#include <cmath>

namespace aaa { namespace ui { namespace qt6 {

// ── SoundDeviceModel ───────────────────────────────────────────────────────

SoundDeviceModel::SoundDeviceModel( QObject* parent )
    : QAbstractListModel( parent )
    , devices_( new QMediaDevices( this ) )
{
    QObject::connect( devices_, &QMediaDevices::audioInputsChanged,
                       this,   &SoundDeviceModel::refresh );
    QObject::connect( devices_, &QMediaDevices::audioOutputsChanged,
                       this,   &SoundDeviceModel::refresh );
    refresh();
}

int SoundDeviceModel::rowCount( QModelIndex const& parent ) const
{
    if( parent.isValid() ) return 0;
    return entries_.size();
}

QVariant SoundDeviceModel::data( QModelIndex const& index, int role ) const
{
    int const row = index.row();
    if( row < 0 || row >= entries_.size() ) return QVariant();
    auto const& e = entries_[ row ];
    switch( role )
    {
        case NameRole:    return e.name;
        case KindRole:    return e.is_input && e.is_output ? QStringLiteral( "DUPLEX" )
                              : e.is_input  ? QStringLiteral( "IN" )
                              : e.is_output ? QStringLiteral( "OUT" )
                              :               QStringLiteral( "---" );
        case DefaultRole: return e.is_default;
        case IdRole:      return e.id;
        default:          return QVariant();
    }
}

QHash< int, QByteArray > SoundDeviceModel::roleNames() const
{
    return {
        { NameRole,    "name"      },
        { KindRole,    "kind"      },
        { DefaultRole, "isDefault" },
        { IdRole,      "deviceId"  },
    };
}

void SoundDeviceModel::refresh()
{
    beginResetModel();
    entries_.clear();
    auto const def_in  = QMediaDevices::defaultAudioInput();
    auto const def_out = QMediaDevices::defaultAudioOutput();
    for( auto const& d : QMediaDevices::audioInputs() )
    {
        Entry e;
        e.id         = QString::fromUtf8( d.id() );
        e.name       = d.description();
        e.is_input   = true;
        e.is_default = ( d == def_in );
        entries_.append( std::move( e ) );
    }
    for( auto const& d : QMediaDevices::audioOutputs() )
    {
        Entry e;
        e.id         = QString::fromUtf8( d.id() );
        e.name       = d.description();
        e.is_output  = true;
        e.is_default = ( d == def_out );
        entries_.append( std::move( e ) );
    }
    endResetModel();
    emit changed();
}

void SoundDeviceModel::playTestTone()
{
    QAudioFormat fmt;
    fmt.setSampleRate( 44100 );
    fmt.setChannelCount( 1 );
    fmt.setSampleFormat( QAudioFormat::Int16 );

    QAudioDevice const out = QMediaDevices::defaultAudioOutput();
    if( out.isNull() )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "No default audio output." ) );
        return;
    }
    if( !out.isFormatSupported( fmt ) )
        fmt = out.preferredFormat();

    constexpr int   durationMs = 600;
    constexpr float frequency  = 440.0f;
    int const sampleRate  = fmt.sampleRate();
    int const totalFrames = sampleRate * durationMs / 1000;

    QByteArray buf;
    buf.resize( totalFrames * 2 );  // Int16
    qint16* data = reinterpret_cast< qint16* >( buf.data() );
    for( int i = 0; i < totalFrames; ++i )
    {
        // 30 ms fade-in / fade-out so the tone doesn't click.
        float const fade = std::min< float >( 1.0f,
            std::min< float >( i, totalFrames - i ) / ( sampleRate * 0.03f ) );
        double const phase = 2.0 * M_PI * frequency * i / sampleRate;
        float  const sample = 0.5f * fade * static_cast< float >( std::sin( phase ) );
        data[ i ] = static_cast< qint16 >( sample * 32767.0f );
    }

    if( tone_sink_ ) { tone_sink_->stop(); tone_sink_.reset(); }
    tone_sink_ = std::make_unique< QAudioSink >( out, fmt );
    QBuffer* qbuf = new QBuffer( tone_sink_.get() );
    qbuf->setData( buf );
    qbuf->open( QIODevice::ReadOnly );

    QObject::connect( tone_sink_.get(), &QAudioSink::stateChanged,
        this, [ this ]( QAudio::State s )
        {
            if( s == QAudio::IdleState || s == QAudio::StoppedState )
            {
                if( tone_sink_ )
                {
                    tone_sink_->stop();
                    tone_sink_.reset();
                }
            }
        } );
    tone_sink_->start( qbuf );
    emit logLine( /*INFO=*/0,
        QStringLiteral( "Playing 440 Hz on %1." ).arg( out.description() ) );
}

// c152-K : input-level meter. QAudioSource pulls 16-bit mono samples ;
// every 50 ms the timer reads the IODevice and computes peak amplitude.
// Stops cleanly on stopMonitoring() / destruction.
void SoundDeviceModel::startMonitoring()
{
    if( input_source_ ) return;

    QAudioDevice const in = QMediaDevices::defaultAudioInput();
    if( in.isNull() )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "No default audio input." ) );
        return;
    }
    QAudioFormat fmt;
    fmt.setSampleRate( 44100 );
    fmt.setChannelCount( 1 );
    fmt.setSampleFormat( QAudioFormat::Int16 );
    if( !in.isFormatSupported( fmt ) )
        fmt = in.preferredFormat();

    input_source_ = std::make_unique< QAudioSource >( in, fmt );
    input_io_     = input_source_->start();
    if( !input_io_ )
    {
        input_source_.reset();
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Failed to open audio input." ) );
        return;
    }
    if( !meter_timer_ )
    {
        meter_timer_ = new QTimer( this );
        meter_timer_->setInterval( 50 );
        QObject::connect( meter_timer_, &QTimer::timeout, this, [ this ]
        {
            if( !input_io_ ) return;
            QByteArray const chunk = input_io_->readAll();
            if( chunk.isEmpty() ) return;
            qint16 const* s = reinterpret_cast< qint16 const* >( chunk.constData() );
            int const n = chunk.size() / 2;
            int peak = 0;
            for( int i = 0; i < n; ++i )
            {
                int const v = std::abs( static_cast< int >( s[ i ] ) );
                if( v > peak ) peak = v;
            }
            double const level = std::min( 1.0, peak / 32767.0 );
            if( std::abs( level - input_level_ ) > 0.005 )
            {
                input_level_ = level;
                emit inputLevelChanged();
            }
        } );
    }
    meter_timer_->start();
    emit monitoringChanged();
    emit logLine( /*INFO=*/0,
        QStringLiteral( "Listening to %1." ).arg( in.description() ) );
}

void SoundDeviceModel::stopMonitoring()
{
    if( meter_timer_ ) meter_timer_->stop();
    if( input_source_ ) input_source_->stop();
    input_source_.reset();
    input_io_ = nullptr;
    if( input_level_ != 0.0 )
    {
        input_level_ = 0.0;
        emit inputLevelChanged();
    }
    emit monitoringChanged();
}

// ── CameraController ──────────────────────────────────────────────────────

CameraController::CameraController( QObject* parent )
    : QObject( parent )
    , devices_( new QMediaDevices( this ) )
    , session_( std::make_unique< QMediaCaptureSession >() )
{
    QObject::connect( devices_, &QMediaDevices::videoInputsChanged,
                       this,   &CameraController::refresh );
    refresh();
}

CameraController::~CameraController()
{
    stop();
}

void CameraController::refresh()
{
    device_list_  = QMediaDevices::videoInputs();
    QStringList names;
    for( auto const& d : device_list_ )
        names << d.description();
    if( names == device_names_ ) return;
    device_names_ = std::move( names );
    if( active_index_ >= device_names_.size() ) setActiveIndex( -1 );
    emit devicesChanged();
}

void CameraController::setActiveIndex( int idx )
{
    if( idx == active_index_ ) return;
    bool const was_capturing = capturing_;
    if( was_capturing ) stop();
    active_index_ = idx;
    emit activeIndexChanged();
    if( was_capturing && active_index_ >= 0 ) start();
}

void CameraController::start()
{
    if( active_index_ < 0 || active_index_ >= device_list_.size() )
    {
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Camera : no device selected." ) );
        return;
    }
    camera_ = std::make_unique< QCamera >( device_list_[ active_index_ ] );
    session_->setCamera( camera_.get() );
    camera_->start();
    capturing_ = true;
    emit capturingChanged();
    emit videoSinkChanged();
    emit logLine( /*INFO=*/0,
        QStringLiteral( "Camera capture started : %1" )
            .arg( device_list_[ active_index_ ].description() ) );
}

void CameraController::bindToVideoSink( QObject* sink )
{
    if( !session_ ) return;
    auto* vs = qobject_cast< QVideoSink* >( sink );
    if( vs )
    {
        session_->setVideoSink( vs );
        emit logLine( /*INFO=*/0,
            QStringLiteral( "Camera bound to QML VideoOutput sink." ) );
    }
    else
    {
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Camera : provided sink is not a QVideoSink." ) );
    }
}

void CameraController::stop()
{
    if( camera_ )
    {
        camera_->stop();
        session_->setCamera( nullptr );
        camera_.reset();
    }
    bool const was = capturing_;
    capturing_ = false;
    if( was )
    {
        emit capturingChanged();
        emit logLine( /*INFO=*/0,
            QStringLiteral( "Camera capture stopped." ) );
    }
}

// ── BinaryTaskModel ───────────────────────────────────────────────────────

BinaryTaskModel::BinaryTaskModel( QObject* parent )
    : QAbstractListModel( parent )
{
}

BinaryTaskModel::~BinaryTaskModel()
{
    for( auto& t : tasks_ )
    {
        if( t.process )
        {
            t.process->terminate();
            t.process->waitForFinished( 200 );
            t.process->deleteLater();
        }
    }
}

int BinaryTaskModel::rowCount( QModelIndex const& parent ) const
{
    if( parent.isValid() ) return 0;
    return tasks_.size();
}

QVariant BinaryTaskModel::data( QModelIndex const& index, int role ) const
{
    int const row = index.row();
    if( row < 0 || row >= tasks_.size() ) return QVariant();
    auto const& t = tasks_[ row ];
    switch( role )
    {
        case LabelRole:    return t.label;
        case CommandRole:  return t.command;
        case ArgsRole:     return t.args;
        case RunningRole:  return t.running;
        case PidRole:      return static_cast< qlonglong >( t.pid );
        case ExitCodeRole: return t.exit_code;
        default:           return QVariant();
    }
}

QHash< int, QByteArray > BinaryTaskModel::roleNames() const
{
    return {
        { LabelRole,    "label"    },
        { CommandRole,  "command"  },
        { ArgsRole,     "args"     },
        { RunningRole,  "running"  },
        { PidRole,      "pid"      },
        { ExitCodeRole, "exitCode" },
    };
}

void BinaryTaskModel::addTask( QString const& label,
                                QString const& command,
                                QString const& args )
{
    Entry e;
    e.label   = label.isEmpty() ? command : label;
    e.command = command;
    e.args    = args;
    int const row = tasks_.size();
    beginInsertRows( QModelIndex(), row, row );
    tasks_.append( std::move( e ) );
    endInsertRows();
}

void BinaryTaskModel::wire_process( int row )
{
    if( row < 0 || row >= tasks_.size() ) return;
    auto& t = tasks_[ row ];
    QProcess* p = t.process;
    QString label = t.label;

    QObject::connect( p, &QProcess::readyReadStandardOutput, this,
        [ this, p, label ]() {
            QByteArray data = p->readAllStandardOutput();
            for( QByteArray line : data.split( '\n' ) )
            {
                if( line.isEmpty() ) continue;
                emit logLine( /*LUA=*/3,
                    QStringLiteral( "[%1] %2" ).arg( label,
                        QString::fromUtf8( line ).trimmed() ) );
            }
        } );
    QObject::connect( p, &QProcess::readyReadStandardError, this,
        [ this, p, label ]() {
            QByteArray data = p->readAllStandardError();
            for( QByteArray line : data.split( '\n' ) )
            {
                if( line.isEmpty() ) continue;
                emit logLine( /*ERR=*/2,
                    QStringLiteral( "[%1 stderr] %2" ).arg( label,
                        QString::fromUtf8( line ).trimmed() ) );
            }
        } );
    QObject::connect( p, &QProcess::finished, this,
        [ this, row ]( int code, QProcess::ExitStatus )
        {
            if( row < 0 || row >= tasks_.size() ) return;
            tasks_[ row ].running   = false;
            tasks_[ row ].exit_code = code;
            QModelIndex const idx = index( row, 0 );
            emit dataChanged( idx, idx,
                { RunningRole, ExitCodeRole } );
            emit logLine( /*INFO=*/0,
                QStringLiteral( "[%1] exited with code %2" )
                    .arg( tasks_[ row ].label ).arg( code ) );
        } );
}

void BinaryTaskModel::startTask( int row )
{
    if( row < 0 || row >= tasks_.size() ) return;
    auto& t = tasks_[ row ];
    if( t.running ) return;
    if( !QFileInfo( t.command ).isExecutable() )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Not executable : %1" ).arg( t.command ) );
        return;
    }
    if( !t.process )
    {
        t.process = new QProcess( this );
        wire_process( row );
    }
    QStringList const argv = t.args.split( QChar( ' ' ),
        Qt::SkipEmptyParts );
    t.process->start( t.command, argv );
    if( !t.process->waitForStarted( 2000 ) )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Launch failed : %1" ).arg(
                t.process->errorString() ) );
        return;
    }
    t.running = true;
    t.pid     = t.process->processId();
    QModelIndex const idx = index( row, 0 );
    emit dataChanged( idx, idx, { RunningRole, PidRole } );
    emit logLine( /*INFO=*/0,
        QStringLiteral( "[%1] launched, pid=%2" )
            .arg( t.label ).arg( t.pid ) );
}

void BinaryTaskModel::stopTask( int row )
{
    if( row < 0 || row >= tasks_.size() ) return;
    auto& t = tasks_[ row ];
    if( !t.running || !t.process ) return;
    t.process->terminate();
    emit logLine( /*INFO=*/0,
        QStringLiteral( "[%1] SIGTERM" ).arg( t.label ) );
}

void BinaryTaskModel::removeTask( int row )
{
    if( row < 0 || row >= tasks_.size() ) return;
    auto& t = tasks_[ row ];
    if( t.process )
    {
        if( t.process->state() != QProcess::NotRunning )
            t.process->terminate();
        t.process->waitForFinished( 200 );
        t.process->deleteLater();
    }
    beginRemoveRows( QModelIndex(), row, row );
    tasks_.remove( row );
    endRemoveRows();
}

} } } // namespace aaa::ui::qt6
