/*
 * tools/make_test_png/make_test_png.c
 *
 * One-shot fixture generator. Emits a 64x64 RGBA PNG with a known
 * pattern : R ramps along x, G ramps along y, B static mid-gray, A
 * full. The .app loads this file from its Resources/ at runtime and
 * uses it as the source texture for the catalog shader -- proves the
 * Phase 5 asset pipeline (disk PNG -> stb_image -> MTLTexture -> draw)
 * end-to-end.
 *
 * Invoked at build time via cmake/aaa_test_png.cmake -- output goes
 * to ${CMAKE_BINARY_DIR}/test_assets/test_pattern.png. The .app's
 * post-build copy step then drops it into the bundle.
 *
 * Continuation 46. Pure C, links against the vendored stb_image_write
 * via the aaaseed_stb static lib.
 */

/*	`stb_image_write.h` is a header-only ; the IMPLEMENTATION is
 *	provided by the `aaaseed_stb` static lib's `stb_impl.c` so the
 *	symbols link cleanly without re-instantiating here.
 */
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char* argv[] )
{
    if( argc != 2 )
    {
        fprintf( stderr, "usage: %s <output_path.png>\n", argv[ 0 ] );
        return 1;
    }

    enum { W = 64, H = 64, C = 4 };
    unsigned char pixels[ W * H * C ];

    for( int y = 0; y < H; ++y )
    {
        for( int x = 0; x < W; ++x )
        {
            int const i = ( y * W + x ) * C;
            pixels[ i + 0 ] = (unsigned char) ( ( x * 255 ) / ( W - 1 ) ); /* R ramps -> */
            pixels[ i + 1 ] = (unsigned char) ( ( y * 255 ) / ( H - 1 ) ); /* G ramps up */
            pixels[ i + 2 ] = 128;                                          /* B mid-gray */
            pixels[ i + 3 ] = 255;
        }
    }

    int const ok = stbi_write_png( argv[ 1 ], W, H, C, pixels, W * C );
    if( !ok )
    {
        fprintf( stderr, "stbi_write_png failed for %s\n", argv[ 1 ] );
        return 2;
    }

    printf( "Wrote %dx%d RGBA test pattern to %s\n", W, H, argv[ 1 ] );
    return 0;
}
