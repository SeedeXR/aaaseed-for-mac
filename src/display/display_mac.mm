// src/display/display_mac.mm
//
// second_todo.md S8 (c153) : live NSScreen enumeration + borderless aux
// windows with CAMetalLayers. See display_mac.h for the API, scope honesty,
// and doctrine. Pure ObjC++, manual reference counting (-fno-objc-arc).

#import "src/display/display_mac.h"

#import <AppKit/AppKit.h>
#import <QuartzCore/CAMetalLayer.h>

#include <vector>

namespace aaa
{
namespace display
{

std::vector<Rect> enumerate_screens()
{
    std::vector<Rect> out;
    @autoreleasepool
    {
        NSArray<NSScreen *> * screens = [NSScreen screens];
        if( screens == nil )
            return out;
        for( NSScreen * s in screens )
        {
            NSRect f = s.frame;
            out.push_back( Rect{ (double) f.origin.x,
                                 (double) f.origin.y,
                                 (double) f.size.width,
                                 (double) f.size.height } );
        }
    }
    return out;
}

namespace
{

struct Aux
{
    NSWindow *     window = nil;
    CAMetalLayer * layer  = nil;
    Rect           rect;
};

void configure_aux_window( NSWindow * w )
{
    //	Borderless, covers its whole screen, joins every Space, sits above
    //	normal windows -- matches the upstream MultiWindow aux config.
    [w setLevel:NSScreenSaverWindowLevel];
    [w setOpaque:YES];
    [w setHasShadow:NO];
    [w setCollectionBehavior:( NSWindowCollectionBehaviorCanJoinAllSpaces
                             | NSWindowCollectionBehaviorFullScreenAuxiliary
                             | NSWindowCollectionBehaviorStationary
                             | NSWindowCollectionBehaviorIgnoresCycle )];
    [w setIgnoresMouseEvents:YES];
}

}   //	anonymous namespace

struct MultiDisplay::Impl
{
    std::vector<Aux> aux;
    bool             enabled = false;

    void teardown()
    {
        for( Aux & a : aux )
        {
            if( a.window != nil )
            {
                [a.window orderOut:nil];
                [a.window release];
            }
            [a.layer release];
        }
        aux.clear();
        enabled = false;
    }
};

MultiDisplay::MultiDisplay() : impl_( new Impl() ) {}

MultiDisplay::~MultiDisplay()
{
    impl_->teardown();
    delete impl_;
}

std::size_t MultiDisplay::enable()
{
    if( impl_->enabled )
        return impl_->aux.size();

    @autoreleasepool
    {
        std::vector<Rect> screens = enumerate_screens();
        std::size_t       primary = primary_index( screens );

        NSArray<NSScreen *> * ns_screens = [NSScreen screens];
        for( std::size_t i = 0; i < screens.size(); ++i )
        {
            if( i == primary )
                continue;
            if( i >= (std::size_t) ns_screens.count )
                continue;

            NSScreen * screen = [ns_screens objectAtIndex:i];
            NSRect     frame  = screen.frame;

            NSWindow * w = [[NSWindow alloc]
                initWithContentRect:frame
                          styleMask:NSWindowStyleMaskBorderless
                            backing:NSBackingStoreBuffered
                              defer:NO
                             screen:screen];
            configure_aux_window( w );

            CAMetalLayer * layer = [[CAMetalLayer alloc] init];
            layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
            layer.framebufferOnly = YES;
            layer.frame = NSRectToCGRect( frame );

            NSView * content = [w contentView];
            [content setWantsLayer:YES];
            [content setLayer:layer];

            [w orderFrontRegardless];

            Aux a;
            a.window = w;       //	owns the +1
            a.layer  = layer;   //	owns the +1
            a.rect   = screens[ i ];
            impl_->aux.push_back( a );
        }
        impl_->enabled = true;
        return impl_->aux.size();
    }
}

void MultiDisplay::disable()        { impl_->teardown(); }
bool MultiDisplay::is_enabled() const  { return impl_->enabled; }
std::size_t MultiDisplay::aux_count() const { return impl_->aux.size(); }

void * MultiDisplay::aux_metal_layer( std::size_t index ) const
{
    if( index >= impl_->aux.size() )
        return nullptr;
    return (void *) impl_->aux[ index ].layer;
}

Rect MultiDisplay::aux_rect( std::size_t index ) const
{
    if( index >= impl_->aux.size() )
        return Rect{};
    return impl_->aux[ index ].rect;
}

}   //	namespace display
}   //	namespace aaa
