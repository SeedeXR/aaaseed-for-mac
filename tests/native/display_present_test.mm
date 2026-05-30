// tests/native/display_present_test.mm
//
// second_todo.md S8 (c153) : CI-safe test for the sub-rect present primitive
// (the per-aux-display blit core of the fullscreen-span feature). No window /
// no multi-display hardware : we render into an offscreen texture and read the
// bytes back.
//
// Source is a 2x2 BGRA8 texture, LEFT column red / RIGHT column green -- it is
// vertically symmetric, so the result is independent of any y-orientation
// choice and the assertions isolate the horizontal sub-rect mapping + scaling.

#import <Metal/Metal.h>
#import <gtest/gtest.h>

#include "src/display/display_present_mac.h"
#include "src/display/display_layout.h"

#include <cstdint>
#include <vector>

namespace
{

id<MTLTexture> make_texture( id<MTLDevice> dev, NSUInteger w, NSUInteger h,
                             MTLTextureUsage usage )
{
    MTLTextureDescriptor * d =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                           width:w
                                                          height:h
                                                       mipmapped:NO];
    d.usage       = usage;
    d.storageMode = MTLStorageModeShared;   //	Apple Silicon : CPU-readable
    return [dev newTextureWithDescriptor:d];
}

//	BGRA : red = {0,0,255,255}, green = {0,255,0,255}.
bool is_red( std::uint8_t const * p )   { return p[2] == 255 && p[1] == 0;   }
bool is_green( std::uint8_t const * p ) { return p[1] == 255 && p[2] == 0;   }

void read_back( id<MTLTexture> t, std::vector<std::uint8_t> & out, NSUInteger w, NSUInteger h )
{
    out.assign( w * h * 4, 0 );
    [t getBytes:out.data()
      bytesPerRow:w * 4
       fromRegion:MTLRegionMake2D( 0, 0, w, h )
      mipmapLevel:0];
}

}   //	anonymous namespace

class SubRectPresent : public ::testing::Test
{
protected:
    id<MTLDevice>        dev   = nil;
    id<MTLCommandQueue>  queue = nil;
    id<MTLTexture>       src   = nil;

    void SetUp() override
    {
        dev = MTLCreateSystemDefaultDevice();
        if( dev == nil )
            GTEST_SKIP() << "no Metal device available";
        queue = [dev newCommandQueue];

        src = make_texture( dev, 2, 2, MTLTextureUsageShaderRead );
        //	left column red, right column green ; both rows identical.
        std::uint8_t px[ 16 ] = {
            0,0,255,255,  0,255,0,255,   //	row 0 : red, green
            0,0,255,255,  0,255,0,255,   //	row 1 : red, green
        };
        [src replaceRegion:MTLRegionMake2D( 0, 0, 2, 2 )
               mipmapLevel:0
                 withBytes:px
               bytesPerRow:8];
    }
};

TEST_F( SubRectPresent, IdentityPreservesColumns )
{
    aaa::display::SubRectPresenter p( dev );
    ASSERT_TRUE( p.is_valid() );

    //	Sample the 2-wide source identity into a 4-wide dest so column centers
    //	(u = 0.125 / 0.375 / 0.625 / 0.875) avoid the u=0.5 texel boundary
    //	where nearest sampling is ambiguous. Left half must stay red, right
    //	half green -- proving identity preserves left->left, right->right.
    id<MTLTexture> dst = make_texture( dev, 4, 1,
        MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead );
    ASSERT_TRUE( p.present( queue, src, dst, aaa::display::Rect{ 0, 0, 1, 1 } ) );

    std::vector<std::uint8_t> out;
    read_back( dst, out, 4, 1 );
    EXPECT_TRUE( is_red(   out.data() + 0  ) );   //	col 0 -> red
    EXPECT_TRUE( is_red(   out.data() + 4  ) );   //	col 1 -> red
    EXPECT_TRUE( is_green( out.data() + 8  ) );   //	col 2 -> green
    EXPECT_TRUE( is_green( out.data() + 12 ) );   //	col 3 -> green
}

TEST_F( SubRectPresent, RightHalfIsAllGreen )
{
    aaa::display::SubRectPresenter p( dev );
    ASSERT_TRUE( p.is_valid() );

    id<MTLTexture> dst = make_texture( dev, 2, 2,
        MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead );
    //	Sample only the right half of the source -> entirely green.
    ASSERT_TRUE( p.present( queue, src, dst, aaa::display::Rect{ 0.5, 0.0, 0.5, 1.0 } ) );

    std::vector<std::uint8_t> out;
    read_back( dst, out, 2, 2 );
    EXPECT_TRUE( is_green( out.data() + 0 ) );
    EXPECT_TRUE( is_green( out.data() + 4 ) );
}

TEST_F( SubRectPresent, LeftHalfIsAllRed )
{
    aaa::display::SubRectPresenter p( dev );
    ASSERT_TRUE( p.is_valid() );

    id<MTLTexture> dst = make_texture( dev, 2, 2,
        MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead );
    ASSERT_TRUE( p.present( queue, src, dst, aaa::display::Rect{ 0.0, 0.0, 0.5, 1.0 } ) );

    std::vector<std::uint8_t> out;
    read_back( dst, out, 2, 2 );
    EXPECT_TRUE( is_red( out.data() + 0 ) );
    EXPECT_TRUE( is_red( out.data() + 4 ) );
}

TEST_F( SubRectPresent, NilArgsReturnFalse )
{
    aaa::display::SubRectPresenter p( dev );
    ASSERT_TRUE( p.is_valid() );
    EXPECT_FALSE( p.present( queue, nil, nil, aaa::display::Rect{ 0, 0, 1, 1 } ) );
}
