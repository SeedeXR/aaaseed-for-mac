// src/ui/qt/aaa_panel_models.h
//
// c152-D : the three panels that were retired with aaa_studio_metal.mm
// re-implemented on the Qt side :
//   - SoundDeviceModel    : QMediaDevices → audio I/O list
//   - CameraController    : QMediaDevices::videoInputs + QCamera +
//                           QMediaCaptureSession ; preview rendered
//                           by QML's VideoOutput
//   - BinaryTaskModel     : QProcess-backed task list ; stdout/stderr
//                           streamed to the Console via signals
//
// All three are independent of the Studio data model -- they don't
// touch project state. They're surfaced on the QML side as separate
// context properties (`sound`, `camera`, `tasks`).

#pragma once

#include <QAbstractListModel>
#include <QAudioDevice>
#include <QAudioSink>
#include <QAudioSource>
#include <QCamera>
#include <QCameraDevice>
#include <QIODevice>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVector>
#include <QVideoSink>

#include <memory>

#include <memory>

namespace aaa { namespace ui { namespace qt6 {

// ── Sound : list of QAudioDevice ───────────────────────────────────────────

class SoundDeviceModel : public QAbstractListModel
{
    Q_OBJECT
    // c152-K : live input level (peak, 0.0–1.0) from the default
    // audio-input device. Updated by a 50 ms timer while monitoring
    // is active.
    Q_PROPERTY( double inputLevel    READ inputLevel    NOTIFY inputLevelChanged )
    Q_PROPERTY( bool   monitoring    READ monitoring    NOTIFY monitoringChanged )
public:
    enum Roles { NameRole = Qt::UserRole + 1, KindRole, DefaultRole, IdRole };

    explicit SoundDeviceModel( QObject* parent = nullptr );

    int rowCount( QModelIndex const& parent = QModelIndex() ) const override;
    QVariant data( QModelIndex const& index, int role ) const override;
    QHash< int, QByteArray > roleNames() const override;

    // c152-I : a 600 ms 440 Hz sine pulse on the default audio output.
    Q_INVOKABLE void playTestTone();

    double inputLevel() const { return input_level_;  }
    bool   monitoring() const { return input_source_ != nullptr; }

    // c152-K : start/stop polling the default input device for a
    // peak-amplitude level. Safe to toggle repeatedly.
    Q_INVOKABLE void startMonitoring();
    Q_INVOKABLE void stopMonitoring();

public slots:
    void refresh();
signals:
    void changed();
    void logLine( int level, QString const& text );
    void inputLevelChanged();
    void monitoringChanged();

private:
    struct Entry
    {
        QString id;
        QString name;
        bool    is_input  = false;
        bool    is_output = false;
        bool    is_default = false;
    };
    QVector< Entry > entries_;
    QMediaDevices*   devices_ = nullptr;
    // c152-I : held during a test tone ; auto-deletes via the
    // QObject child mechanism on stop. One outstanding sink at a time.
    std::unique_ptr< QAudioSink > tone_sink_;

    // c152-K : audio-input monitoring.
    std::unique_ptr< QAudioSource > input_source_;
    QIODevice*                       input_io_   = nullptr;
    QTimer*                          meter_timer_ = nullptr;
    double                           input_level_ = 0.0;
};

// ── Camera : single live capture session driven by QCamera ────────────────

class CameraController : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QStringList deviceNames READ deviceNames  NOTIFY devicesChanged )
    Q_PROPERTY( int     activeIndex   READ activeIndex    WRITE setActiveIndex
                                       NOTIFY activeIndexChanged )
    Q_PROPERTY( bool    capturing     READ capturing      NOTIFY capturingChanged )
    Q_PROPERTY( QObject* videoSink    READ videoSinkObj   NOTIFY videoSinkChanged )

public:
    explicit CameraController( QObject* parent = nullptr );
    ~CameraController() override;

    QStringList deviceNames()  const { return device_names_; }
    int         activeIndex()  const { return active_index_; }
    bool        capturing()    const { return capturing_;    }
    QObject*    videoSinkObj() const { return session_ ? session_->videoSink() : nullptr; }

public slots:
    void refresh();
    void setActiveIndex( int idx );
    void start();
    void stop();
    // c152-I : bind the session's video output to a QML VideoOutput's
    // QVideoSink. Called from QML once the VideoOutput is realized.
    void bindToVideoSink( QObject* sink );

signals:
    void devicesChanged();
    void activeIndexChanged();
    void capturingChanged();
    void videoSinkChanged();
    void logLine( int level, QString const& text );

private:
    QMediaDevices*               devices_ = nullptr;
    QList< QCameraDevice >       device_list_;
    QStringList                  device_names_;
    int                          active_index_ = -1;
    std::unique_ptr< QCamera >   camera_;
    std::unique_ptr< QMediaCaptureSession > session_;
    bool                         capturing_ = false;
};

// ── Binary Manager : QProcess-backed tasks ────────────────────────────────

class BinaryTaskModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles { LabelRole = Qt::UserRole + 1, CommandRole, ArgsRole,
                 RunningRole, PidRole, ExitCodeRole };

    explicit BinaryTaskModel( QObject* parent = nullptr );
    ~BinaryTaskModel() override;

    int rowCount( QModelIndex const& parent = QModelIndex() ) const override;
    QVariant data( QModelIndex const& index, int role ) const override;
    QHash< int, QByteArray > roleNames() const override;

    Q_INVOKABLE void addTask( QString const& label,
                              QString const& command,
                              QString const& args );
    Q_INVOKABLE void startTask( int row );
    Q_INVOKABLE void stopTask( int row );
    Q_INVOKABLE void removeTask( int row );

signals:
    // Bubble process stdout/stderr up to the Console panel.
    void logLine( int level, QString const& text );

private:
    struct Entry
    {
        QString    label;
        QString    command;
        QString    args;
        QProcess*  process = nullptr;
        bool       running = false;
        int        exit_code = 0;
        qint64     pid     = 0;
    };
    QVector< Entry > tasks_;

    void wire_process( int row );
};

} } } // namespace aaa::ui::qt6
