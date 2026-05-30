// src/syphon/syphon_pixel_util.h
//
// second_todo.md S4 (continuation 153) : vertical-flip helper for the Syphon
// receive path. Many Syphon producers (and OpenGL-origin sources) deliver
// frames bottom-up relative to the Metal/top-left texture convention the rest
// of the Mac backend assumes. The upstream engine author's bdd_syphon exposes
// a `receiver_vertical_flip` param ; this is the native, GPU-agnostic core of
// that feature : an in-place row flip over a tightly-or-loosely packed BGRA8
// buffer (4 bytes/pixel).
//
// Kept as a pure, header-only, framework-free function so it is unit-testable
// without a GPU, an IOSurface, or a window. The IOSurface / MTLTexture wiring
// that calls it lives in the ObjC++ TUs (locks the surface base address +
// bytesPerRow and calls through). This separation follows the project's
// "test the pure logic deterministically" rule.
//
// bytes_per_row may exceed width*4 (IOSurface rows are often padded to a
// hardware alignment) ; only the first width*4 bytes of each row are pixel
// data but the whole stride is swapped so any padding travels with its row.

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace aaa
{
namespace syphon
{

//	Flip `data` vertically in place : row 0 <-> row (height-1), etc.
//	`bytes_per_row` is the stride between consecutive rows (>= width*4 for
//	BGRA8). Does nothing if data is null, height < 2, or bytes_per_row is 0.
//	Returns true if a flip was performed (height >= 2 with valid args).
inline bool flip_rows_in_place(
    std::uint8_t * data,
    std::size_t    height,
    std::size_t    bytes_per_row )
{
    if( data == nullptr || height < 2 || bytes_per_row == 0 )
        return false;

    //	One scratch row keeps the swap allocation-light and cache-friendly.
    std::vector<std::uint8_t> tmp( bytes_per_row );
    std::size_t top = 0;
    std::size_t bot = height - 1;
    while( top < bot )
    {
        std::uint8_t * row_top = data + top * bytes_per_row;
        std::uint8_t * row_bot = data + bot * bytes_per_row;
        for( std::size_t i = 0; i < bytes_per_row; ++i )
        {
            tmp[ i ]    = row_top[ i ];
            row_top[ i ] = row_bot[ i ];
            row_bot[ i ] = tmp[ i ];
        }
        ++top;
        --bot;
    }
    return true;
}

}   //	namespace syphon
}   //	namespace aaa
