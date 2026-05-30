// tests/native/video_metal_bridge_test.mm
//
// second_todo.md S2 (c153) : THE headline CI-safe test for the zero-copy
// CVPixelBuffer -> MTLTexture bridge. No camera, no video file : we
// synthesize a known BGRA8 CVPixelBuffer, wrap it via MetalTextureBridge, and
// assert the resulting MTLTexture has the right dims/format AND that reading
// its bytes back yields the same pattern (proving the texture aliases the
// pixel-buffer's unified-memory bytes).

#import <CoreVideo/CoreVideo.h>
#import <Metal/Metal.h>
#import <gtest/gtest.h>

#include "src/video/metal_texture_bridge.h"

#include <cstdint>
#include <vector>

namespace
{

constexpr std::size_t kW = 4;
constexpr std::size_t kH = 4;

//	Create a Metal-compatible BGRA8 CVPixelBuffer filled with a known pattern
//	(pixel i -> B=i, G=i+1, R=i+2, A=255). Caller CFReleases. Returns NULL on
//	failure.
CVPixelBufferRef make_pattern_buffer()
{
    NSDictionary * attrs = @{
        (id)kCVPixelBufferMetalCompatibilityKey  : @YES,
        (id)kCVPixelBufferIOSurfacePropertiesKey : @{},
    };
    CVPixelBufferRef pb = nullptr;
    CVReturn rc = CVPixelBufferCreate(
        kCFAllocatorDefault, kW, kH,
        kCVPixelFormatType_32BGRA,
        (__bridge CFDictionaryRef) attrs, &pb );
    if( rc != kCVReturnSuccess || pb == nullptr )
        return nullptr;

    CVPixelBufferLockBaseAddress( pb, 0 );
    auto *      base = (std::uint8_t *) CVPixelBufferGetBaseAddress( pb );
    std::size_t bpr  = CVPixelBufferGetBytesPerRow( pb );
    for( std::size_t y = 0; y < kH; ++y )
        for( std::size_t x = 0; x < kW; ++x )
        {
            std::size_t    i = y * kW + x;
            std::uint8_t * p = base + y * bpr + x * 4;
            p[ 0 ] = (std::uint8_t)( i );        //	B
            p[ 1 ] = (std::uint8_t)( i + 1 );    //	G
            p[ 2 ] = (std::uint8_t)( i + 2 );    //	R
            p[ 3 ] = 255;                        //	A
        }
    CVPixelBufferUnlockBaseAddress( pb, 0 );
    return pb;
}

}   //	anonymous namespace

TEST( VideoMetalBridge, WrapsPixelBufferZeroCopy )
{
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if( device == nil )
    {
        GTEST_SKIP() << "no Metal device available (headless GPU-less runner)";
    }

    CVPixelBufferRef pb = make_pattern_buffer();
    ASSERT_NE( pb, nullptr );

    aaa::video::MetalTextureBridge bridge( device );
    ASSERT_TRUE( bridge.is_valid() );

    id<MTLTexture> tex = bridge.texture_from_pixel_buffer( pb );
    ASSERT_NE( tex, nil );
    EXPECT_EQ( (std::size_t) tex.width,  kW );
    EXPECT_EQ( (std::size_t) tex.height, kH );
    EXPECT_EQ( tex.pixelFormat, MTLPixelFormatBGRA8Unorm );

    //	Read the texture bytes back. On unified memory the texture aliases the
    //	IOSurface pages, so the pattern we wrote into the pixel buffer must
    //	survive verbatim.
    std::vector<std::uint8_t> out( kW * kH * 4, 0 );
    [tex getBytes:out.data()
        bytesPerRow:kW * 4
         fromRegion:MTLRegionMake2D( 0, 0, kW, kH )
        mipmapLevel:0];

    bool pattern_ok = true;
    for( std::size_t i = 0; i < kW * kH; ++i )
    {
        std::uint8_t const * p = out.data() + i * 4;
        if( p[ 0 ] != (std::uint8_t)( i )     ||
            p[ 1 ] != (std::uint8_t)( i + 1 ) ||
            p[ 2 ] != (std::uint8_t)( i + 2 ) ||
            p[ 3 ] != 255 )
        {
            pattern_ok = false;
            break;
        }
    }
    EXPECT_TRUE( pattern_ok ) << "zero-copy texture did not alias the pixel-buffer bytes";

    bridge.flush();
    CVPixelBufferRelease( pb );
}

TEST( VideoMetalBridge, InvalidInputsReturnNil )
{
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if( device == nil )
        GTEST_SKIP() << "no Metal device available";

    aaa::video::MetalTextureBridge bridge( device );
    EXPECT_EQ( bridge.texture_from_pixel_buffer( nullptr ), nil );
}

TEST( VideoMetalBridge, NilDeviceIsInvalid )
{
    aaa::video::MetalTextureBridge bridge( (id<MTLDevice>) nil );
    EXPECT_FALSE( bridge.is_valid() );
    EXPECT_EQ( bridge.texture_from_pixel_buffer( nullptr ), nil );
}
