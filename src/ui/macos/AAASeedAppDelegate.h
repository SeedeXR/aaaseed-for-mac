// AAASeedAppDelegate — the NSApplicationDelegate that owns the main window
// and the Metal-backed MTKView. Pure Objective-C interface ; the underlying
// MetalBackend lives in pure C++ and is reached via a forward-declared
// C++ pointer held inside the delegate.

#pragma once

#import <Cocoa/Cocoa.h>

@interface AAASeedAppDelegate : NSObject< NSApplicationDelegate >

//	If > 0, the app auto-quits after this many frames. 0 = run until the
//	user closes the window. Used by smoke tests to make `aaaseed_app`
//	exit cleanly without a person clicking close.
@property( nonatomic, assign ) NSInteger maxFrames;

// c152-D : when set, the delegate hands this path to the MEU runner
// via load_script() instead of the bundled hello_world.lua fallback.
// Spawned by the Qt Studio's Play button : `--project <path>`.
@property( nonatomic, copy   ) NSString* projectPath;

@end
