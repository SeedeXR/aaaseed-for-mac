#include <gtest/gtest.h>

#include <cstdint>
#include <string>

TEST( SanityTest, ArithmeticHolds )
{
    EXPECT_EQ( 2 + 2, 4 );
    EXPECT_NE( 2 + 2, 5 );
}

TEST( SanityTest, Cpp20StandardLibraryIsAvailable )
{
    std::string s = "AAASeed";
    EXPECT_EQ( s.length(), 7u );
    EXPECT_TRUE( s.starts_with( "AAA" ) );
}

TEST( SanityTest, AppleSiliconArm64Detected )
{
#if defined( __aarch64__ ) || defined( __arm64__ )
    SUCCEED() << "Compiled for arm64 (Apple Silicon) as expected.";
#else
    FAIL() << "Expected arm64 target. Mac v1 ships arm64-only per project_context.md.";
#endif
}

TEST( SanityTest, FixedWidthIntegerSizes )
{
    EXPECT_EQ( sizeof( std::int32_t ), 4u );
    EXPECT_EQ( sizeof( std::int64_t ), 8u );
    EXPECT_EQ( sizeof( std::uint8_t ), 1u );
}
