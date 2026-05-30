// tests/native/net_mac_test.mm
//
// Coverage for the native macOS HTTP transport (net_mac.h).
//
// The make_auth_header() tests are fully deterministic and need no
// network : they verify the Basic-scheme base64 encoding against a
// hand-computed expectation, the Bearer pass-through, and the None
// empty-string case.
//
// The get_sync() live-network test performs real I/O and is therefore
// GATED behind the AAA_NET_LIVE=1 environment variable -- CI has no
// guaranteed network. When the gate is off the test logs an explicit
// GTEST_SKIP line ; it never skips silently.

#include <gtest/gtest.h>

#include <cstdlib>
#include <string>

#include "src/net/net_mac.h"

//	"user:pass" base64-encodes to "dXNlcjpwYXNz" (RFC 4648 alphabet,
//	no padding needed here -- 9 bytes -> 12 base64 chars exactly).
//	Hand-verified : u=0x75 s=0x73 e=0x65 r=0x72 :=0x3a p=0x70 a=0x61
//	s=0x73 s=0x73.
TEST( NetMacAuth, BasicEncodesUserPass )
{
    std::string header =
        aaa::net::make_auth_header( aaa::net::AUTH_BASIC, "user:pass" );
    EXPECT_EQ( header, "Basic dXNlcjpwYXNz" );
}

//	Empty credential under Basic : base64("") == "" -> "Basic ".
TEST( NetMacAuth, BasicEmptyCredential )
{
    std::string header =
        aaa::net::make_auth_header( aaa::net::AUTH_BASIC, "" );
    EXPECT_EQ( header, "Basic " );
}

//	A second known vector : "aaa:seed" -> "YWFhOnNlZWQ=" (8 bytes, one
//	pad char).
TEST( NetMacAuth, BasicEncodesSecondVector )
{
    std::string header =
        aaa::net::make_auth_header( aaa::net::AUTH_BASIC, "aaa:seed" );
    EXPECT_EQ( header, "Basic YWFhOnNlZWQ=" );
}

//	Bearer : the token passes through verbatim after "Bearer ".
TEST( NetMacAuth, BearerPassesTokenVerbatim )
{
    std::string header =
        aaa::net::make_auth_header( aaa::net::AUTH_BEARER, "abc123.TOKEN" );
    EXPECT_EQ( header, "Bearer abc123.TOKEN" );
}

//	None : always the empty string, regardless of the credential.
TEST( NetMacAuth, NoneIsEmpty )
{
    EXPECT_EQ(
        aaa::net::make_auth_header( aaa::net::AUTH_NONE, "ignored" ),
        std::string() );
    EXPECT_EQ(
        aaa::net::make_auth_header( aaa::net::AUTH_NONE, "" ),
        std::string() );
}

//	set_auth() must not crash and is exercised here for coverage ; the
//	header it produces is observed indirectly through make_auth_header
//	(set_auth stores the same values make_auth_header consumes).
TEST( NetMacAuth, SetAuthStoresStateWithoutCrash )
{
    aaa::net::set_auth( aaa::net::AUTH_BEARER, "tok" );
    aaa::net::set_auth( aaa::net::AUTH_NONE, "" );
    SUCCEED();
}

//	-- Live network : GATED behind AAA_NET_LIVE=1 ----------------------

TEST( NetMacLive, GetSyncFetchesOverNetwork )
{
    char const * gate = std::getenv( "AAA_NET_LIVE" );
    if( gate == nullptr || std::string( gate ) != "1" )
    {
        GTEST_SKIP()
            << "AAA_NET_LIVE != 1 : skipping live get_sync() network "
               "test (CI has no guaranteed network). Set AAA_NET_LIVE=1 "
               "to run it.";
        return;
    }

    long        status = 0;
    std::string body;
    bool ok = aaa::net::get_sync( "https://example.com/",
                                  10.0, &status, &body );

    EXPECT_TRUE( ok ) << "get_sync returned false (timeout / transport)";
    EXPECT_GE( status, 200 );
    EXPECT_LT( status, 600 );
    EXPECT_GT( body.size(), 0u ) << "expected a non-empty response body";
}
