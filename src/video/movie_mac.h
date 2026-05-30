// src/video/movie_mac.h
//
// second_todo.md S2 (c153) : Video -- movie playback via AVPlayer, the
// frame-source half of the AVFoundation -> Metal zero-copy path.
//
// Movie wraps an AVPlayer + AVPlayerItem + AVPlayerItemVideoOutput. The
// video output vends decoded frames as IOSurface-backed BGRA8
// CVPixelBuffers ; feed copy_current_pixel_buffer() straight into
// aaa::video::MetalTextureBridge for a zero-copy MTLTexture (no CPU
// readback on unified memory). Mirrors the upstream
// c_movie_avfoundation surface : open / play / pause / stop / set_rate /
// set_volume / position / duration.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++ (.mm). std:: + AVFoundation + CoreVideo + CoreMedia +
//     Foundation only. No engine link, no o_str, no aaa_mem.
//   - Manual reference counting (-fno-objc-arc) : retain/release the
//     AVPlayer / AVPlayerItem / AVPlayerItemVideoOutput we own ;
//     CFRelease CVPixelBuffers handed to the caller.
//   - ASCII-only, English-only comments. House style mirrors
//     src/syphon/syphon_mac.h.
//
// Verified SDK signatures (MacOSX.sdk AVFoundation / CoreMedia Headers) :
//   +[AVPlayerItem playerItemWithURL:]            (AVPlayerItem.h:93)
//   -[AVPlayerItem addOutput:]                    (AVPlayerItem.h:733)
//   -[AVPlayerItem currentTime] / duration        (AVPlayerItem.h:289/206)
//   +[AVPlayer playerWithPlayerItem:]             (AVPlayer.h:77)
//   -[AVPlayer play] / pause                      (AVPlayer.h:170/181)
//   AVPlayer.rate (float) / AVPlayer.volume (float) (AVPlayer.h:152/490)
//   -[AVPlayerItemVideoOutput initWithPixelBufferAttributes:]
//                                                 (AVPlayerItemOutput.h:180)
//   -[... hasNewPixelBufferForItemTime:]          (AVPlayerItemOutput.h:217)
//   -[... copyPixelBufferForItemTime:itemTimeForDisplay:] CF_RETURNS_RETAINED
//                                                 (AVPlayerItemOutput.h:234)
//   CMTimeGetSeconds(CMTime) -> Float64           (CMTime.h:243)
//   AVPlayerItemStatusReadyToPlay = 1             (AVPlayerItem.h:48)

#pragma once

#include <cstddef>
#include <string>

//	CoreVideo opaque CF type. Forward-declared (see metal_texture_bridge.h).
typedef struct __CVBuffer * CVPixelBufferRef;

#ifdef __OBJC__
@class AVPlayer;
@class AVPlayerItem;
@class AVPlayerItemVideoOutput;
#endif

namespace aaa
{
namespace video
{

//	Movie -- AVPlayer-backed file playback with a CVPixelBuffer frame tap.
//
//	Ownership : Movie owns its AVPlayer / AVPlayerItem /
//	AVPlayerItemVideoOutput (retained on open(), released on close()/dtor).
//	copy_current_pixel_buffer() returns a +1-retained CVPixelBufferRef the
//	CALLER must CFRelease.
class Movie
{
public:
    Movie();
    ~Movie();

    Movie( Movie const & )             = delete;
    Movie & operator=( Movie const & ) = delete;

    //	Open a local file path (UTF-8). Returns true if the asset loaded
    //	to AVPlayerItemStatusReadyToPlay within a short bounded wait,
    //	false otherwise (nonexistent path, unsupported format, timeout).
    //	A previously open movie is closed first.
    bool open( char const * path );

    //	Tear down the player/item/output. Safe to call when not open.
    void close();

    //	True between a successful open() and close()/dtor.
    bool is_open() const;

    void play();    //	rate -> 1.0
    void pause();   //	rate -> 0.0
    void stop();    //	pause + seek to start

    //	Playback rate (1.0 normal, 0.0 paused, negative = reverse if the
    //	asset supports it). No-op when not open.
    void set_rate( double rate );

    //	Audio volume in [0,1]. No-op when not open.
    void set_volume( double volume );

    //	Current playhead position in seconds. Returns 0.0 when not open or
    //	the time is invalid/indefinite.
    double position_seconds() const;

    //	Total asset duration in seconds. Returns 0.0 when not open or the
    //	duration is indefinite (e.g. live stream).
    double duration_seconds() const;

    //	True when the player rate is non-zero (i.e. playing).
    bool is_playing() const;

    //	Copy the most-recently-decoded frame as a BGRA8, IOSurface-backed,
    //	Metal-compatible CVPixelBuffer for the current playhead time.
    //	Returns NULL if not open, no new frame is available yet, or the
    //	item time is invalid. The returned buffer is +1-retained : the
    //	CALLER must CFRelease it. Feed it to MetalTextureBridge for a
    //	zero-copy MTLTexture.
    CVPixelBufferRef copy_current_pixel_buffer();

private:
#ifdef __OBJC__
    AVPlayer *                player_;
    AVPlayerItem *            item_;
    AVPlayerItemVideoOutput * output_;
#else
    void * player_;
    void * item_;
    void * output_;
#endif
    bool   is_open_;
};

}   //	namespace video
}   //	namespace aaa
