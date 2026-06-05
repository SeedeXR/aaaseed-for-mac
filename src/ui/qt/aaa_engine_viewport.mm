// src/ui/qt/aaa_engine_viewport.mm
//
// c152-G : foreign-NSView embedding of AAASeedInputView (the MTKView
// driving the engine) inside a Qt main window. ObjC++ wrapper around
// the C++ Q_OBJECT in aaa_engine_viewport.h.

#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

#include "src/ui/qt/aaa_engine_viewport.h"
#include "src/ui/macos/AAASeedMTKView.h"
#include "src/ui/macos/AAASeedInputView.h"
#include "src/gol/metal/metal_backend.h"
#include "src/meu/aaa_meu_runner_mac.h"

#include <cstdint>
#include <memory>

// Forward decl of the ObjC delegate that lives in aaaseed_engine_view ;
// the @interface is in AAASeedMTKView.h (already imported above).

namespace
{
    // Opaque struct kept behind void* on the Q_OBJECT.
    struct EngineViewportImpl
    {
        std::unique_ptr< GOL::MetalBackend >  backend;
        AAASeedInputView*                     view     = nil;   // retained
        AAASeedMTKViewDelegate*               delegate = nil;   // retained
        NSView*                               parent   = nil;   // weak (Qt owns)
    };
}

namespace aaa { namespace ui { namespace qt6 {

EngineViewport::EngineViewport( QObject* parent )
    : QObject( parent )
    , impl_( new EngineViewportImpl() )
{
}

EngineViewport::~EngineViewport()
{
    detach();
    delete static_cast< EngineViewportImpl* >( impl_ );
    impl_ = nullptr;
}

void EngineViewport::attachToWindow( QQuickWindow* window,
                                     int x, int y, int w, int h )
{
    if( !window )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "EngineViewport : null window." ) );
        return;
    }
    auto* impl = static_cast< EngineViewportImpl* >( impl_ );

    // Detach any existing view first.
    if( impl->view )
        detach();

    // The QQuickWindow's native handle is its NSView contentView.
    // WId is an opaque integer ; reinterpret_cast through uintptr_t.
    NSView* parentView = reinterpret_cast< NSView* >(
        static_cast< std::uintptr_t >( window->winId() ) );
    if( !parentView )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "EngineViewport : window has no NSView." ) );
        return;
    }

    // Spin up the engine backend.
    impl->backend = std::make_unique< GOL::MetalBackend >();
    if( !impl->backend->init() )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "EngineViewport : MetalBackend init failed." ) );
        impl->backend.reset();
        return;
    }

    id<MTLDevice> dev = (__bridge id<MTLDevice>)
        reinterpret_cast< void* >( impl->backend->get_device() );

    // Create the input view at the requested rect and the MTKView delegate.
    NSRect const frame = NSMakeRect( x, y, w, h );
    impl->view = [[AAASeedInputView alloc] initWithFrame:frame device:dev];
    //	c158 : MUST be BGRA8 -- the MEU runner compiles every catalog
    //	pipeline against TextureFormat::BGRA8 (matching the runtime
    //	MTKView). The previous RGBA8 here made every use_shader draw fail
    //	pipeline/pass validation, so the preview could never render a
    //	script even when the catalog was present.
    impl->view.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    impl->view.preferredFramesPerSecond = 60;
    impl->view.translatesAutoresizingMaskIntoConstraints = YES;

    impl->delegate = [[AAASeedMTKViewDelegate alloc]
        initWithBackend:impl->backend.get()
              maxFrames:0];
    impl->view.delegate = impl->delegate;

    // Add to the Qt window. ABOVE Qt's content -- macOS draws subviews
    // on top of the parent's compositing layer.
    [parentView addSubview:impl->view];
    impl->parent = parentView;

    [impl->view.window makeFirstResponder:impl->view];

    running_ = true;
    emit runningChanged();
    emit logLine( /*INFO=*/0,
        QStringLiteral( "Engine preview attached at (%1,%2,%3,%4)" )
            .arg(x).arg(y).arg(w).arg(h) );

    //	c158 : a script queued by loadScript() before attach loads now.
    if( !pending_script_.isEmpty() )
    {
        QString const queued = pending_script_;
        pending_script_.clear();
        loadScript( queued );
    }
}

void EngineViewport::loadScript( QString const& path )
{
    if( path.isEmpty() ) return;

    auto* impl = static_cast< EngineViewportImpl* >( impl_ );
    if( !impl->delegate || !running_ )
    {
        pending_script_ = path;
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Engine preview not attached -- script queued. "
                            "Press Start in the Engine Preview panel." ) );
        return;
    }
    auto* runner = [impl->delegate meuRunner];
    if( !runner )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Engine preview : runner missing." ) );
        return;
    }

    std::string const std_path = path.toStdString();
    if( runner->load_script( std_path ) )
    {
        runner->enable_file_watch();
        if( !runner->has_on_frame() )
            emit logLine( /*WARN=*/1,
                QStringLiteral( "Engine preview : script loaded but defines "
                                "no aaa.on_frame -- nothing will render." ) );
        else
            emit logLine( /*INFO=*/0,
                QStringLiteral( "Engine preview now running the editor "
                                "script (hot-reload on)." ) );
    }
    else
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Engine preview : script load failed for %1" )
                .arg( path ) );
    }
}

void EngineViewport::setBounds( int x, int y, int w, int h )
{
    auto* impl = static_cast< EngineViewportImpl* >( impl_ );
    if( !impl->view ) return;
    impl->view.frame = NSMakeRect( x, y, w, h );
}

void EngineViewport::loadProject( QString const& path )
{
    auto* impl = static_cast< EngineViewportImpl* >( impl_ );
    if( !impl->delegate )
    {
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Engine preview not attached ; loadProject ignored." ) );
        return;
    }
    auto* runner = [impl->delegate meuRunner];
    if( !runner )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Engine preview : runner missing." ) );
        return;
    }
    if( path.isEmpty() ) return;
    std::string const std_path = path.toStdString();
    if( runner->load_script( std_path ) )
    {
        project_path_ = path;
        emit projectPathChanged();
        emit logLine( /*INFO=*/0,
            QStringLiteral( "Engine preview loaded : %1" ).arg( path ) );
        runner->enable_file_watch();
    }
    else
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Engine preview : load_script failed for %1" )
                .arg( path ) );
    }
}

void EngineViewport::detach()
{
    auto* impl = static_cast< EngineViewportImpl* >( impl_ );
    if( impl->view )
    {
        [impl->view removeFromSuperview];
        [impl->view release];
        impl->view = nil;
    }
    if( impl->delegate )
    {
        [impl->delegate release];
        impl->delegate = nil;
    }
    impl->backend.reset();
    impl->parent  = nil;
    if( running_ )
    {
        running_ = false;
        emit runningChanged();
    }
    if( !project_path_.isEmpty() )
    {
        project_path_.clear();
        emit projectPathChanged();
    }
}

} } } // namespace aaa::ui::qt6
