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
    if( pDevice == nullptr )
    {
        pPool->release();
        GTEST_SKIP() << "No Metal device available ; runner is headless / "
                        "virtualized without GPU passthrough.";
    }

    //  Apple Silicon GPUs all support TBDR + unified memory. On a virtualized
    //  CI runner that exposes a non-Apple-Silicon Metal device (e.g. swiftshader
    //  or paravirtualized), skip rather than fail -- the assertion only applies
    //  to real Apple-architecture hardware.
    bool const isApple7  = pDevice->supportsFamily( MTL::GPUFamilyApple7 );
    bool const isApple8  = pDevice->supportsFamily( MTL::GPUFamilyApple8 );
    bool const isApple9  = pDevice->supportsFamily( MTL::GPUFamilyApple9 );
    bool const isApple10 = pDevice->supportsFamily( MTL::GPUFamilyApple10 );
    bool const isAppleSilicon =
        pDevice->hasUnifiedMemory() &&
        ( isApple7 || isApple8 || isApple9 || isApple10 );

    if( !isAppleSilicon )
    {
        pDevice->release();
        pPool->release();
        GTEST_SKIP() << "Metal device is not Apple Silicon (no unified memory "
                        "or family < Apple7) ; skipping Apple-Silicon-only "
                        "feature assertions. This is expected on Intel Macs "
                        "and on virtualized CI runners.";
    }

    //  Now on real Apple Silicon -- assert the expected feature set.
    EXPECT_TRUE( pDevice->hasUnifiedMemory() );
    EXPECT_TRUE( isApple7 || isApple8 || isApple9 || isApple10 );

    pDevice->release();
    pPool->release();
}
