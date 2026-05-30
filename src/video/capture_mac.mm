// src/video/capture_mac.mm
//
// second_todo.md S2 (c153) : AVCaptureSession camera capture. See
// capture_mac.h for the API, SDK-cited signatures, permission-safety notes,
// and doctrine. Pure ObjC++, manual reference counting (-fno-objc-arc).

#import "src/video/capture_mac.h"

#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreVideo/CoreVideo.h>

#include <mutex>

//	------------------------------------------------------------------
//	Private delegate : receives sample buffers on a serial queue and
//	stashes the latest CVPixelBuffer (retained) under a mutex.
//	------------------------------------------------------------------
@interface AAACaptureDelegate : NSObject < AVCaptureVideoDataOutputSampleBufferDelegate >
{
@public
    std::mutex *      mutex_;
    CVPixelBufferRef  latest_;
}
@end

@implementation AAACaptureDelegate

- (void)captureOutput:(AVCaptureOutput *)output
       didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    (void) output; (void) connection;
    CVImageBufferRef img = CMSampleBufferGetImageBuffer( sampleBuffer );
    if( img == nullptr )
        return;
    CVPixelBufferRef pb = (CVPixelBufferRef) img;
    CVPixelBufferRetain( pb );          //	+1 ; keep alive past this callback

    std::lock_guard<std::mutex> lock( *mutex_ );
    if( latest_ != nullptr )
        CVPixelBufferRelease( latest_ );
    latest_ = pb;
}

@end

namespace aaa
{
namespace video
{

namespace
{

//	Connected video devices, system order. Enumeration-only ; no session,
//	no permission prompt. devicesWithMediaType: is deprecated but live and
//	the simplest portable enumeration across the macOS versions we target.
NSArray * video_devices()
{
    return [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
}

}   //	anonymous namespace

std::size_t Capture::get_device_count()
{
    @autoreleasepool
    {
        NSArray * devs = video_devices();
        return devs ? (std::size_t) devs.count : 0;
    }
}

std::string Capture::get_device_name( std::size_t index )
{
    @autoreleasepool
    {
        NSArray * devs = video_devices();
        if( devs == nil || index >= (std::size_t) devs.count )
            return std::string();
        AVCaptureDevice * dev = [devs objectAtIndex:index];
        char const * utf8 = [dev.localizedName UTF8String];
        return utf8 ? std::string( utf8 ) : std::string();
    }
}

struct Capture::Impl
{
    AVCaptureSession *        session  = nil;
    AVCaptureDeviceInput *    input    = nil;
    AVCaptureVideoDataOutput * output  = nil;
    AAACaptureDelegate *      delegate = nil;
    dispatch_queue_t          queue    = nullptr;
    std::mutex                mutex;
    bool                      running  = false;

    void teardown()
    {
        if( session != nil && running )
            [session stopRunning];
        running = false;

        [session release];  session  = nil;
        [input release];    input    = nil;
        [output release];   output   = nil;

        if( delegate != nil )
        {
            std::lock_guard<std::mutex> lock( mutex );
            if( delegate->latest_ != nullptr )
            {
                CVPixelBufferRelease( delegate->latest_ );
                delegate->latest_ = nullptr;
            }
            [delegate release];
            delegate = nil;
        }
        if( queue != nullptr )
        {
            dispatch_release( queue );
            queue = nullptr;
        }
    }
};

Capture::Capture() : impl_( new Impl() ) {}

Capture::~Capture()
{
    impl_->teardown();
    delete impl_;
}

bool Capture::open( std::size_t device_index )
{
    impl_->teardown();

    @autoreleasepool
    {
        NSArray * devs = video_devices();
        if( devs == nil || device_index >= (std::size_t) devs.count )
            return false;
        AVCaptureDevice * dev = [devs objectAtIndex:device_index];

        NSError * err = nil;
        AVCaptureDeviceInput * input =
            [AVCaptureDeviceInput deviceInputWithDevice:dev error:&err];
        if( input == nil || err != nil )
            return false;
        [input retain];

        AVCaptureSession * session = [[AVCaptureSession alloc] init];
        AVCaptureVideoDataOutput * output =
            [[AVCaptureVideoDataOutput alloc] init];
        output.videoSettings = @{
            (id)kCVPixelBufferPixelFormatTypeKey    : @(kCVPixelFormatType_32BGRA),
            (id)kCVPixelBufferMetalCompatibilityKey : @YES,
        };
        output.alwaysDiscardsLateVideoFrames = YES;

        AAACaptureDelegate * delegate = [[AAACaptureDelegate alloc] init];
        delegate->mutex_  = &impl_->mutex;
        delegate->latest_ = nullptr;

        dispatch_queue_t queue =
            dispatch_queue_create( "com.seedexr.aaaseed.capture", DISPATCH_QUEUE_SERIAL );
        [output setSampleBufferDelegate:delegate queue:queue];

        if( ![session canAddInput:input] || ![session canAddOutput:output] )
        {
            [input release];
            [session release];
            [output release];
            [delegate release];
            dispatch_release( queue );
            return false;
        }
        [session addInput:input];
        [session addOutput:output];

        impl_->session  = session;    //	ownership transferred (+1 each)
        impl_->input    = input;
        impl_->output   = output;
        impl_->delegate = delegate;
        impl_->queue    = queue;
        return true;
    }
}

bool Capture::start()
{
    if( impl_->session == nil )
        return false;
    if( impl_->running )
        return true;
    [impl_->session startRunning];
    impl_->running = true;
    return true;
}

void Capture::stop()
{
    if( impl_->session != nil && impl_->running )
        [impl_->session stopRunning];
    impl_->running = false;
}

bool Capture::is_running() const { return impl_->running; }

CVPixelBufferRef Capture::copy_latest_pixel_buffer()
{
    if( impl_->delegate == nullptr )
        return nullptr;
    std::lock_guard<std::mutex> lock( impl_->mutex );
    if( impl_->delegate->latest_ == nullptr )
        return nullptr;
    CVPixelBufferRetain( impl_->delegate->latest_ );   //	+1 for the caller
    return impl_->delegate->latest_;
}

}   //	namespace video
}   //	namespace aaa
