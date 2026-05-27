// First Metal smoke test for the AAASeed Mac port.
//
// Proves we can use metal-cpp from pure C++ on this machine — no Objective-C++,
// no .mm files. This is the foundation the GOL::MetalBackend builds on.
//
// Per metal-cpp's README, the implementation symbols (selectors, classes) are
// emitted in exactly ONE translation unit per build. That TU is
// `src/gol/metal/metal_backend.cpp` (the canonical place since 2026-05-26).
// This file just includes the header declarations and links against the
// `aaaseed_gol_metal` static lib which carries the impl.

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <gtest/gtest.h>

#include <cstdio>

TEST( Metal, MtlCreateSystemDefaultDeviceReturnsNonNull )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    MTL::Device* pDevice = MTL::CreateSystemDefaultDevice();
    ASSERT_NE( pDevice, nullptr ) << "MTL::CreateSystemDefaultDevice() returned nullptr "
                                     "— machine has no Metal-capable GPU?";

    NS::String* pName = pDevice->name();
    ASSERT_NE( pName, nullptr );
    char const* name_utf8 = pName->utf8String();
    ASSERT_NE( name_utf8, nullptr );
    EXPECT_GT( std::char_traits< char >::length( name_utf8 ), 0u );

    std::printf( "[      INFO ] MTLDevice name : %s\n", name_utf8 );

    pDevice->release();
    pPool->release();
}

TEST( Metal, MtlDeviceCommandQueueRoundTrip )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    MTL::Device* pDevice = MTL::CreateSystemDefaultDevice();
    ASSERT_NE( pDevice, nullptr );

    MTL::CommandQueue* pQueue = pDevice->newCommandQueue();
    ASSERT_NE( pQueue, nullptr ) << "MTL::Device::newCommandQueue() returned nullptr";

    pQueue->release();
    pDevice->release();
    pPool->release();
}

TEST( Metal, MtlDeviceReportsAppleSiliconFeatures )
{
    NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();

    MTL::Device* pDevice = MTL::CreateSystemDefaultDevice();
    ASSERT_NE( pDevice, nullptr );

    //  Apple Silicon GPUs all support TBDR. The hasUnifiedMemory() bit is
    //  what tells us this is an Apple-architecture GPU (unified memory) and
    //  not, e.g., an external eGPU. On the Mac dev machine we expect this
    //  to be true.
    EXPECT_TRUE( pDevice->hasUnifiedMemory() )
        << "Expected unified-memory GPU on Apple Silicon. If this is an Intel "
           "Mac with discrete GPU this assertion is allowed to fail.";

    //  Apple7 = M1, Apple8 = M2, Apple9 = M3, Apple10 = M4. The dev machine
    //  should be one of these.
    bool const isApple7  = pDevice->supportsFamily( MTL::GPUFamilyApple7 );
    bool const isApple8  = pDevice->supportsFamily( MTL::GPUFamilyApple8 );
    bool const isApple9  = pDevice->supportsFamily( MTL::GPUFamilyApple9 );
    EXPECT_TRUE( isApple7 || isApple8 || isApple9 )
        << "Expected Apple7+ (M1+) GPU family on this Mac.";

    pDevice->release();
    pPool->release();
}
