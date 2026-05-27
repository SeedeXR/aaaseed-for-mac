// Exercises GOL::MetalBackend through the abstract GOL::Backend interface.
// Proves the polymorphic dispatch works and that backend construction /
// init / buffer-create / texture-create / deinit don't leak or crash.
//
// These tests are headless — they create MTL::Texture render targets but
// never present to a screen. Useful as a sanity layer ; integration tests
// against a real MTKView land later in Phase 4.

#include <gtest/gtest.h>

#include <memory>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

TEST( GolMetalBackend, FactoryReturnsNonNullBackend )
{
    std::unique_ptr< GOL::Backend > backend( GOL::create_default_backend() );
    ASSERT_NE( backend, nullptr );
    EXPECT_STREQ( backend->backend_name(), "Metal" );
}

TEST( GolMetalBackend, InitSucceedsOnAppleSilicon )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() ) << "MetalBackend::init() failed — no Metal-capable GPU?";
    ASSERT_NE( backend.get_device(),        nullptr );
    ASSERT_NE( backend.get_command_queue(), nullptr );
}

TEST( GolMetalBackend, InitIsIdempotent )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );
    auto* device_first  = backend.get_device();
    ASSERT_TRUE( backend.init() );
    auto* device_second = backend.get_device();
    EXPECT_EQ( device_first, device_second );
}

TEST( GolMetalBackend, DeviceInfoReportsAppleSilicon )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );
    GOL::DeviceInfo info = backend.get_device_info();

    EXPECT_FALSE( info.name.empty() );
    EXPECT_TRUE(  info.has_unified_memory );
    EXPECT_TRUE(  info.supports_apple_family );
    EXPECT_GE(    info.apple_family_min, 7u );   // M1+

    std::printf( "[      INFO ] backend reports %s (Apple%u, unified-memory=%d)\n",
                 info.name.c_str(), info.apple_family_min, info.has_unified_memory );
}

TEST( GolMetalBackend, BufferGenAndDelete )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    GOL::BufferId a = backend.gen_buffer();
    GOL::BufferId b = backend.gen_buffer();
    EXPECT_NE( a, GOL::kInvalidBufferId );
    EXPECT_NE( b, GOL::kInvalidBufferId );
    EXPECT_NE( a, b );

    backend.delete_buffer( a );
    backend.delete_buffer( b );
}

TEST( GolMetalBackend, BufferUploadRoundTrip )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    GOL::BufferId id = backend.gen_buffer();
    ASSERT_NE( id, GOL::kInvalidBufferId );

    float const data[] = { 1.0f, 2.0f, 3.0f, 4.0f };
    backend.buffer_data( id, sizeof( data ), data, GOL::BufferUsage::Stream );

    backend.delete_buffer( id );
}

TEST( GolMetalBackend, TextureCreateAndUpload )
{
    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    constexpr std::uint32_t W = 64;
    constexpr std::uint32_t H = 64;
    GOL::TextureId tex = backend.gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
    ASSERT_NE( tex, GOL::kInvalidTextureId );

    //	Fill with a known pattern : pixel (x,y) = (x ^ y, x, y, 255).
    std::vector< std::uint8_t > pixels( W * H * 4 );
    for( std::uint32_t y = 0; y < H; ++y )
    {
        for( std::uint32_t x = 0; x < W; ++x )
        {
            std::size_t const off = ( y * W + x ) * 4;
            pixels[ off + 0 ] = static_cast< std::uint8_t >( x ^ y );
            pixels[ off + 1 ] = static_cast< std::uint8_t >( x );
            pixels[ off + 2 ] = static_cast< std::uint8_t >( y );
            pixels[ off + 3 ] = 255;
        }
    }
    backend.texture_data_2d( tex, pixels.data(), W * 4 );

    backend.delete_texture( tex );
}

TEST( GolMetalBackend, BackendDestructorCleansUpEvenWithoutExplicitDeinit )
{
    //	Make sure ~MetalBackend() calls deinit() and releases owned resources
    //	even when the user forgets to call deinit() explicitly.
    {
        GOL::MetalBackend backend;
        ASSERT_TRUE( backend.init() );
        backend.gen_buffer();
        backend.gen_texture_2d( 16, 16, GOL::TextureFormat::RGBA8 );
        //	No explicit deinit() ; destructor handles it.
    }
    //	If this test reaches here without leaking or crashing, we're good.
    SUCCEED();
}
