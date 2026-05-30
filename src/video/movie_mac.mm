// src/video/movie_mac.mm
//
// second_todo.md S2 (c153) : AVPlayer-backed movie playback + CVPixelBuffer
// frame tap. See movie_mac.h for the API + the SDK-cited signatures +
// doctrine. Pure ObjC++, manual reference counting (-fno-objc-arc).

#import "src/video/movie_mac.h"

#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreVideo/CoreVideo.h>

namespace aaa
{
namespace video
{

Movie::Movie()
    : player_( nil ), item_( nil ), output_( nil ), is_open_( false )
{
}

Movie::~Movie() { close(); }

void Movie::close()
{
    if( output_ != nil && item_ != nil )
        [item_ removeOutput:output_];
    [output_ release]; output_ = nil;
    [item_   release]; item_   = nil;
    [player_ release]; player_ = nil;
    is_open_ = false;
}

bool Movie::open( char const * path )
{
    close();
    if( path == nullptr )
        return false;

    @autoreleasepool
    {
        NSString * ns = [NSString stringWithUTF8String:path];
        if( ns == nil )
            return false;
        NSURL * url = [NSURL fileURLWithPath:ns];
        if( url == nil )
            return false;

        AVPlayerItem * item = [AVPlayerItem playerItemWithURL:url];
        if( item == nil )
            return false;
        [item retain];

        //	Request BGRA8 IOSurface-backed, Metal-compatible output so the
        //	frames drop straight into MetalTextureBridge with no copy.
        NSDictionary * attrs = @{
            (id)kCVPixelBufferPixelFormatTypeKey      : @(kCVPixelFormatType_32BGRA),
            (id)kCVPixelBufferMetalCompatibilityKey   : @YES,
            (id)kCVPixelBufferIOSurfacePropertiesKey  : @{},
        };
        AVPlayerItemVideoOutput * output =
            [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:attrs];
        if( output == nil )
        {
            [item release];
            return false;
        }
        [item addOutput:output];

        AVPlayer * player = [AVPlayer playerWithPlayerItem:item];
        if( player == nil )
        {
            [item removeOutput:output];
            [output release];
            [item release];
            return false;
        }
        [player retain];

        //	Bounded wait for the item to reach ReadyToPlay. A nonexistent
        //	path / unsupported format ends in Failed (or never leaves
        //	Unknown) ; we time out and report false.
        NSDate * deadline = [NSDate dateWithTimeIntervalSinceNow:2.0];
        while( item.status == AVPlayerItemStatusUnknown
               && [deadline timeIntervalSinceNow] > 0.0 )
        {
            [[NSRunLoop currentRunLoop]
                runMode:NSDefaultRunLoopMode
                beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.02]];
        }

        if( item.status != AVPlayerItemStatusReadyToPlay )
        {
            [item removeOutput:output];
            [output release];
            [item release];
            [player release];
            return false;
        }

        player_  = player;   //	ownership transferred (+1 each)
        item_    = item;
        output_  = output;
        is_open_ = true;
        return true;
    }
}

bool Movie::is_open() const { return is_open_; }

void Movie::play()  { if( player_ != nil ) player_.rate = 1.0f; }
void Movie::pause() { if( player_ != nil ) player_.rate = 0.0f; }

void Movie::stop()
{
    if( player_ == nil )
        return;
    player_.rate = 0.0f;
    [player_ seekToTime:kCMTimeZero];
}

void Movie::set_rate( double rate )
{
    if( player_ != nil )
        player_.rate = (float) rate;
}

void Movie::set_volume( double volume )
{
    if( player_ == nil )
        return;
    if( volume < 0.0 ) volume = 0.0;
    if( volume > 1.0 ) volume = 1.0;
    player_.volume = (float) volume;
}

double Movie::position_seconds() const
{
    if( item_ == nil )
        return 0.0;
    CMTime t = [item_ currentTime];
    Float64 s = CMTimeGetSeconds( t );
    return ( isfinite( s ) && s > 0.0 ) ? (double) s : 0.0;
}

double Movie::duration_seconds() const
{
    if( item_ == nil )
        return 0.0;
    CMTime d = [item_ duration];
    Float64 s = CMTimeGetSeconds( d );
    return ( isfinite( s ) && s > 0.0 ) ? (double) s : 0.0;
}

bool Movie::is_playing() const
{
    return ( player_ != nil ) && ( player_.rate != 0.0f );
}

CVPixelBufferRef Movie::copy_current_pixel_buffer()
{
    if( output_ == nil || item_ == nil )
        return nullptr;

    CMTime t = [item_ currentTime];
    if( CMTIME_IS_INVALID( t ) )
        return nullptr;
    if( ![output_ hasNewPixelBufferForItemTime:t] )
        return nullptr;

    //	CF_RETURNS_RETAINED : caller owns the +1 and must CFRelease.
    return [output_ copyPixelBufferForItemTime:t itemTimeForDisplay:nullptr];
}

}   //	namespace video
}   //	namespace aaa
