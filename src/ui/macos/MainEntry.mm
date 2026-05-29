// MainEntry.mm — process entry point for the aaaseed_app binary.
//
// Standard Cocoa pattern : create the NSApplication, install an app
// delegate, run the main event loop. `--max-frames N` quits cleanly
// after N rendered frames (used by smoke tests).

#import <Cocoa/Cocoa.h>
#import "AAASeedAppDelegate.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

int main( int argc, char const* argv[] )
{
    NSInteger maxFrames = 0;
    NSString* projectPath = nil;       // c152-D : --project <path>
    for( int i = 1; i + 1 < argc; ++i )
    {
        if( std::strcmp( argv[ i ], "--max-frames" ) == 0 )
        {
            maxFrames = std::atoi( argv[ i + 1 ] );
            ++i;
        }
        else if( std::strcmp( argv[ i ], "--project" ) == 0 )
        {
            projectPath = [NSString stringWithUTF8String: argv[ i + 1 ]];
            ++i;
        }
    }

    @autoreleasepool
    {
        NSApplication* app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];

        AAASeedAppDelegate* delegate = [[AAASeedAppDelegate alloc] init];
        delegate.maxFrames = maxFrames;
        if( projectPath )
            delegate.projectPath = projectPath;
        [app setDelegate:delegate];

        [app run];
    }
    return 0;
}
