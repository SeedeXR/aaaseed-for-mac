// Tests c_buffer_stream from code_utils/buffer/buffer_blk.h.
//
// IMPORTANT — block-stream semantics :
//   put() fills fixed-size blocks ; partial blocks are NOT readable.
//   get() returns at most ONE block per call (or less if dst is smaller).
//   When dst is smaller than blk_size, get reads the TAIL of the block,
//   intentionally dropping the beginning ("short reads intentionally drop
//   older data" — see buffer_blk.cpp:175).
//
// Designed for audio frame streaming, not for byte-oriented round-trip.

#include <gtest/gtest.h>

#include <cstring>
#include <vector>

#include "aaa_type.h"
#include "buffer/buffer_blk.h"

TEST( CodeUtilsBufferStream, EmptyStreamReturnsZeroOnGet )
{
    c_buffer_stream s;
    s.set_blk_size( 32 );
    UINT8 out[ 32 ] = { 0 };
    EXPECT_EQ( s.get( out, 32 ), 0 );
    EXPECT_EQ( s.get_count_in(),  0 );
    EXPECT_EQ( s.get_count_out(), 0 );
}

TEST( CodeUtilsBufferStream, PartialBlockIsNotReadable )
{
    c_buffer_stream s;
    s.set_blk_size( 32 );

    UINT8 const data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    s.put( data, sizeof( data ) );

    // Only 8 of 32 bytes were written ; the block is partial and get() refuses.
    UINT8 out[ 32 ] = { 0 };
    EXPECT_EQ( s.get( out, 32 ), 0 );
}

TEST( CodeUtilsBufferStream, FullBlockRoundTrips )
{
    c_buffer_stream s;
    constexpr INT32 BLK = 16;
    s.set_blk_size( BLK );

    UINT8 in[ BLK ];
    for( INT32 i = 0; i < BLK; ++i )
        in[ i ] = static_cast< UINT8 >( i + 1 );

    s.put( in, BLK );
    EXPECT_EQ( s.get_count_in(), 1 );   // one full block completed

    UINT8 out[ BLK ] = { 0 };
    INT32 const got = s.get( out, BLK );
    EXPECT_EQ( got, BLK );
    EXPECT_EQ( std::memcmp( out, in, BLK ), 0 );
    EXPECT_EQ( s.get_count_out(), 1 );
}

TEST( CodeUtilsBufferStream, GetReturnsOneBlockPerCall )
{
    // UPSTREAM CAVEAT (surfaced 2026-05-25) : c_buffer_stream::put() does NOT
    // advance its `src` pointer across loop iterations, so calling put() with
    // size > blk_size writes the same first blk_size bytes into every block.
    // The function is documented as audio-frame oriented and expected to be
    // called with size <= blk_size. This test puts one frame at a time to
    // exercise the multi-block dequeue without tripping the bug.
    c_buffer_stream s;
    constexpr INT32 BLK = 32;
    constexpr INT32 NB_BLOCKS = 4;
    s.set_blk_size( BLK );

    std::vector< UINT8 > in( BLK * NB_BLOCKS );
    for( INT32 i = 0; i < BLK * NB_BLOCKS; ++i )
        in[ i ] = static_cast< UINT8 >( i & 0xff );
    for( INT32 i = 0; i < NB_BLOCKS; ++i )
        s.put( in.data() + i * BLK, BLK );
    EXPECT_EQ( s.get_count_in(), NB_BLOCKS );

    std::vector< UINT8 > out( BLK * NB_BLOCKS, 0 );
    INT32 total_got = 0;
    for( INT32 i = 0; i < NB_BLOCKS; ++i )
    {
        INT32 const n = s.get( out.data() + total_got, BLK * NB_BLOCKS - total_got );
        EXPECT_EQ( n, BLK );
        total_got += n;
    }
    EXPECT_EQ( total_got, BLK * NB_BLOCKS );
    EXPECT_EQ( std::memcmp( out.data(), in.data(), BLK * NB_BLOCKS ), 0 );
    EXPECT_EQ( s.get_count_out(), NB_BLOCKS );
}

TEST( CodeUtilsBufferStream, ShortReadReturnsBlockTail )
{
    // When the caller passes len_dst < blk_size, the implementation copies the
    // TAIL of the block (last len_dst bytes) and drops the beginning. This is
    // not a generic byte-stream behaviour ; it is documented and load-bearing
    // for audio frame consumers that prefer the freshest samples.
    c_buffer_stream s;
    constexpr INT32 BLK = 16;
    s.set_blk_size( BLK );

    UINT8 in[ BLK ];
    for( INT32 i = 0; i < BLK; ++i )
        in[ i ] = static_cast< UINT8 >( 100 + i );
    s.put( in, BLK );

    constexpr INT32 SHORT_LEN = 4;
    UINT8 out[ SHORT_LEN ] = { 0 };
    INT32 const got = s.get( out, SHORT_LEN );
    EXPECT_EQ( got, SHORT_LEN );

    // Expect the last 4 bytes of the input : in[12..15] = 112, 113, 114, 115.
    EXPECT_EQ( out[ 0 ], 112 );
    EXPECT_EQ( out[ 1 ], 113 );
    EXPECT_EQ( out[ 2 ], 114 );
    EXPECT_EQ( out[ 3 ], 115 );
}
