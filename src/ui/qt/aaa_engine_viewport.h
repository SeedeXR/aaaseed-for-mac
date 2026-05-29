// src/ui/qt/aaa_engine_viewport.h
//
// c152-G : in-window engine viewport. A Q_OBJECT that owns an
// AAASeedInputView (the MTKView subclass driving the engine) and
// adds it as a subview of the Qt main window's NSView. The Qt scene
// graph composes around it ; macOS's native view layering handles
// the pixel-level z-order automatically.
//
// QML usage :
//   property var viewport : enginePreview        // singleton-style
//   onCompleted : viewport.attachToWindow(...)   // see EnginePreviewPanel.qml
//
// Lifetime : owned by the Q_OBJECT ; detach() removes the view from
// the parent and stops the render delegate. The destructor enforces
// detach.

#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QString>
#include <QTimer>

#include <memory>
#include <string>

namespace aaa { namespace ui { namespace qt6 {

// Q_OBJECT lives in C++ ; ObjC++ implementation in aaa_engine_viewport.mm
class EngineViewport : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool    running     READ running     NOTIFY runningChanged )
    Q_PROPERTY( QString projectPath READ projectPath NOTIFY projectPathChanged )

public:
    explicit EngineViewport( QObject* parent = nullptr );
    ~EngineViewport() override;

    bool    running()     const { return running_; }
    QString projectPath() const { return project_path_; }

public slots:
    // Attach the engine view to the given Qt window at the given
    // logical-points rectangle. window is a QQuickWindow* passed from
    // QML (use `Window.window` from inside the QML scene). Re-attach
    // safely re-parents.
    void attachToWindow( QQuickWindow* window,
                          int x, int y, int w, int h );

    // Reposition without re-attaching. Cheap.
    void setBounds( int x, int y, int w, int h );

    // Load a project .aaaproj.lua. Triggers a runner reload ; the
    // engine continues drawing throughout. If projectPath is empty,
    // unloads back to the bundled hello_world.lua sample.
    void loadProject( QString const& path );

    // Detach the engine view from its parent + tear down the runner.
    // Idempotent.
    void detach();

signals:
    void runningChanged();
    void projectPathChanged();
    void logLine( int level, QString const& text );

private:
    // ObjC++ side state lives behind a void* so the .h stays pure C++.
    // The impl uses an opaque struct -- see aaa_engine_viewport.mm.
    void*    impl_         = nullptr;
    bool     running_      = false;
    QString  project_path_;
};

} } } // namespace aaa::ui::qt6
