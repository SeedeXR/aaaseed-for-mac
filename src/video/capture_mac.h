// src/video/capture_mac.h
//
// second_todo.md S2 (c153) : Video -- camera / device CAPTURE via
// AVCaptureSession, the second frame-source for the AVFoundation -> Metal
// zero-copy path. Captured frames arrive as IOSurface-backed BGRA8
// CVPixelBuffers (kCVPixelFormatType_32BGRA) ready to hand to
// aaa::video::MetalTextureBridge with no CPU copy. Mirrors the upstream
// c_capture_avfoundation surface (device enumeration + run/stop + ask_frame).
//
// Permission safety : the STATIC enumeration path (get_device_count /
// get_device_name) only lists connected devices and NEVER starts a session,
// so it does not trigger the macOS camera-permission (TCC) dialog and is safe
// to call headless / in CI. start() DOES access the camera and will prompt
// for permission ; tests gate it behind AAA_VIDEO_LIVE.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++ (.mm). std:: + AVFoundation + CoreVideo + CoreMedia +
//     Foundation only. No engine link, no o_str, no aaa_mem.
//   - Manual reference counting (-fno-objc-arc).
//   - This header is C++-clean (PIMPL) : no ObjC type crosses the boundary,
//     so a plain .cpp TU can include it.
//
// Verified SDK signatures (MacOSX.sdk AVFoundation Headers) :
//   +[AVCaptureDevice devicesWithMediaType:] (deprecated but live)
//                                            (AVCaptureDevice.h)
//   AVCaptureDevice.localizedName            (AVCaptureDevice.h)
//   +[AVCaptureDeviceInput deviceInputWithDevice:error:]
//                                            (AVCaptureDeviceInput.h)
//   -[AVCaptureVideoDataOutput setSampleBufferDelegate:queue:]
//                                            (AVCaptureVideoDataOutput.h)
//   CMSampleBufferGetImageBuffer(CMSampleBufferRef) -> CVImageBufferRef
//                                            (CMSampleBuffer.h)

#pragma once

#include <cstddef>
#include <string>

//	CoreVideo opaque CF type (see metal_texture_bridge.h).
typedef struct __CVBuffer * CVPixelBufferRef;

namespace aaa
{
namespace video
{

//	Capture -- one AVCaptureSession over one video device, vending the most
//	recent frame as a BGRA8 CVPixelBuffer for the zero-copy Metal path.
//
//	Threading : the delegate callback fires on an internal serial queue and
//	stores the latest frame under a mutex ; copy_latest_pixel_buffer() is
//	safe to call from the render thread.
class Capture
{
public:
    //	-- static enumeration (permission-safe ; no session started) --------

    //	Number of connected video-capture devices. 0 (no crash) when none /
    //	headless. Does NOT trigger the camera-permission dialog.
    static std::size_t get_device_count();

    //	localizedName (UTF-8) of the index-th video device, same order
    //	get_device_count() counts. Out-of-range index returns "".
    static std::string get_device_name( std::size_t index );

    //	-- instance (live capture ; prompts for camera permission) ----------

    Capture();
    ~Capture();

    Capture( Capture const & )             = delete;
    Capture & operator=( Capture const & ) = delete;

    //	Build a session over the index-th device with a BGRA8,
    //	Metal-compatible video-data output. Does NOT start running yet.
    //	Returns false on a bad index or if the input/output cannot be wired.
    bool open( std::size_t device_index );

    //	Start delivering frames (begins capture ; prompts for permission the
    //	first time). Returns false if not open. No-op if already running.
    bool start();

    //	Stop the session. Safe to call when not running.
    void stop();

    //	True between a successful start() and stop().
    bool is_running() const;

    //	Copy the most recently delivered frame as a +1-retained
    //	CVPixelBufferRef (caller must CFRelease), or NULL if no frame has
    //	arrived yet. Feed it to MetalTextureBridge for a zero-copy texture.
    CVPixelBufferRef copy_latest_pixel_buffer();

private:
    struct Impl;
    Impl * impl_;   //	owns all ObjC objects ; manual retain/release
};

}   //	namespace video
}   //	namespace aaa
