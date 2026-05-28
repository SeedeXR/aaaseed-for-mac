// AAASeedMTKView — the MTKViewDelegate that drives per-frame rendering.
// On each callback it acquires the MTKView's currentDrawable and routes
// through `GOL::MetalBackend::begin_window_render_pass()` /
// `present_window()`. The actual render content is currently a fixed
// red clear color (Phase 4 MVP) ; later iterations plug in real engine
// rendering.

#pragma once

#import <MetalKit/MetalKit.h>

#include "src/gol/metal/metal_backend.h"

//	c144 : MEU runner forward-decl. Full type comes in via .mm import of
//	`src/meu/aaa_meu_runner_mac.h`. Keeps the public header free of the
//	runner's <chrono> / <filesystem> baggage.
#ifdef __cplusplus
namespace aaa { namespace meu { class Runner; } }
//	c147-A : widget system forward-decl. The MTKView delegate owns the
//	WidgetSystem and hands a non-owning pointer to the MEU runner so
//	Lua `aaa.ui.*` bindings route into the same instance.
namespace aaa { namespace ui { namespace widgets { class WidgetSystem; } } }
//	c148 : Studio forward-decl.
namespace aaa { namespace ui { namespace studio { class Studio; } } }
#endif

@interface AAASeedMTKViewDelegate : NSObject< MTKViewDelegate >

- (instancetype)initWithBackend:(GOL::MetalBackend*)backend
                      maxFrames:(NSInteger)maxFrames;

//	Set to YES to ask the app to terminate after the next frame draws.
//	The delegate signals this when its frame budget runs out.
@property( nonatomic, readonly ) BOOL shouldTerminate;

//	c144 : MEU runner accessor. The delegate owns the Runner instance
//	for the app's lifetime ; AAASeedInputView reaches in to forward
//	keyboard / mouse / scroll events. Returns nullptr if the runner
//	failed to construct (e.g. backend init failure) -- callers MUST
//	null-check before dispatching.
#ifdef __cplusplus
- (aaa::meu::Runner*)meuRunner;

//	c147-A : widget system accessor. Returns nullptr if the delegate
//	hasn't initialised the system yet (e.g. backend init failure).
//	Caller MUST null-check before dispatching. Used by tests +
//	AAASeedInputView for mouse-edge forwarding.
- (aaa::ui::widgets::WidgetSystem*)widgetSystem;

//	c148 : Studio accessor. Returns nullptr before the first drawable.
- (aaa::ui::studio::Studio*)studio;
#endif

//	c147-A : mouse edge flags consumed by the widget system once per
//	frame. AAASeedInputView calls these helpers from mouseDown: /
//	mouseUp: so the per-event press/release events surface as edge
//	flags into the next widget begin_frame call. queueMousePressed and
//	queueMouseReleased latch one bool each ; drainPressedEdge and
//	drainReleasedEdge atomically read + clear them.
- (void)queueMousePressed;
- (void)queueMouseReleased;

@end
