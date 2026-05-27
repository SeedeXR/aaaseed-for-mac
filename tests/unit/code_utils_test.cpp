// Unit tests for the first batch of code_utils sources compiled on Mac:
//   md5, checksum, strnum, id_unique.
//
// Each test pins a known-input → known-output invariant. These become the
// regression baseline for the code_utils subsystem.

#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>
#include <string>

#include "aaa_type.h"
#include "md5.h"
#include "checksum.h"
#include "strnum.h"
#include "id_unique.h"

namespace
{
    // Hex-encode a 16-byte MD5 digest into a 32-char lowercase string.
    std::string hex32( unsigned char const ( &digest )[ 16 ] )
    {
        char out[ 33 ];
        for( int i = 0; i < 16; ++i )
            std::snprintf( out + i * 2, 3, "%02x", digest[ i ] );
        return std::string( out, 32 );
    }
}

TEST( CodeUtilsMd5, EmptyInputProducesRfcVector )
{
    MD5_CTX ctx;
    MD5Init( &ctx );
    unsigned char digest[ 16 ];
    MD5Final( digest, &ctx );

    EXPECT_EQ( hex32( digest ), "d41d8cd98f00b204e9800998ecf8427e" );
}

TEST( CodeUtilsMd5, AbcProducesRfcVector )
{
    MD5_CTX ctx;
    MD5Init( &ctx );
    char const* msg = "abc";
    MD5Update( &ctx, reinterpret_cast< unsigned char const* >( msg ), 3 );
    unsigned char digest[ 16 ];
    MD5Final( digest, &ctx );

    EXPECT_EQ( hex32( digest ), "900150983cd24fb0d6963f7d28e17f72" );
}

TEST( CodeUtilsMd5, QuickBrownFoxProducesRfcVector )
{
    MD5_CTX ctx;
    MD5Init( &ctx );
    char const* msg = "The quick brown fox jumps over the lazy dog";
    MD5Update( &ctx, reinterpret_cast< unsigned char const* >( msg ),
               static_cast< unsigned int >( std::strlen( msg ) ) );
    unsigned char digest[ 16 ];
    MD5Final( digest, &ctx );

    EXPECT_EQ( hex32( digest ), "9e107d9d372bb6826bd81d3542a419d6" );
}

TEST( CodeUtilsChecksum, ResetReturnsToInitialValue )
{
    c_checksum c;
    UINT32 const initial = c.get();

    c.add_int32( 12345 );
    EXPECT_NE( c.get(), initial );

    c.reset();
    EXPECT_EQ( c.get(), initial );
}

TEST( CodeUtilsChecksum, DifferentInputProducesDifferentChecksum )
{
    c_checksum a;
    c_checksum b;

    a.add_int32( 100 );
    b.add_int32( 101 );

    EXPECT_NE( a.get(), b.get() );
}

TEST( CodeUtilsChecksum, SameInputProducesSameChecksum )
{
    c_checksum a;
    c_checksum b;

    a.add_int32( 42 );
    a.add_int32( 1729 );

    b.add_int32( 42 );
    b.add_int32( 1729 );

    EXPECT_EQ( a.get(), b.get() );
}

TEST( CodeUtilsStrnum, MakeFormatsIntegerLeftPadded )
{
    char buf[ 16 ] = { 0 };
    strnum::make( buf, 5, 42 );
    EXPECT_STREQ( buf, "00042" );
}

TEST( CodeUtilsStrnum, MakeWithBlankUsesSpacePadding )
{
    char buf[ 16 ] = { 0 };
    strnum::make_with_blank( buf, 5, 42 );
    EXPECT_STREQ( buf, "   42" );
}

TEST( CodeUtilsIdUnique, U32SuccessiveIdsAreDistinct )
{
    id_unique::c_u32 a;
    id_unique::c_u32 b;
    id_unique::c_u32 c;

    EXPECT_NE( a.get(), b.get() );
    EXPECT_NE( b.get(), c.get() );
    EXPECT_NE( a.get(), c.get() );
}

TEST( CodeUtilsIdUnique, U64SuccessiveIdsAreDistinct )
{
    id_unique::c_u64 a;
    id_unique::c_u64 b;
    id_unique::c_u64 c;

    EXPECT_NE( a.get(), b.get() );
    EXPECT_NE( b.get(), c.get() );
    EXPECT_NE( a.get(), c.get() );
}

TEST( CodeUtilsIdUnique, ChangeAdvancesId )
{
    id_unique::c_u32 a;
    UINT32 const first = a.get();
    a.change();
    EXPECT_NE( a.get(), first );
}
