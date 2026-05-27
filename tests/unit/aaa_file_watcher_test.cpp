// tests/unit/aaa_file_watcher_test.cpp
//
// c149-A v3 Feature 3 : FSEvents hot-reload watcher unit tests. Pure
// hermetic .cpp -- only links aaaseed_meu_file_watcher + Foundation +
// CoreServices (no widget / runner deps).

#include "src/meu/aaa_file_watcher_mac.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>

namespace
{
std::string make_tmp_file( char const* stem )
{
    auto const p =
        std::filesystem::temp_directory_path() / ( std::string( "aaaseed_fw_" ) + stem + ".lua" );
    std::ofstream{ p } << "-- placeholder\n";
    return p.string();
}

void cleanup( std::string const& p )
{
    std::error_code ec;
    std::filesystem::remove( p, ec );
}
}   //  anonymous

TEST( FileWatcher, FileWatcherCtorDtorSmoke )
{
    aaa::meu::FileWatcher w;
    EXPECT_FALSE( w.is_watching() );
    EXPECT_TRUE( w.watched_path().empty() );
}

TEST( FileWatcher, WatchInvalidPathReturnsFalse )
{
    aaa::meu::FileWatcher w;
    EXPECT_FALSE( w.watch( "", []{} ) );
    EXPECT_FALSE( w.is_watching() );

    EXPECT_FALSE( w.watch( "/no/such/dir/file.lua", []{} ) );
    EXPECT_FALSE( w.is_watching() );
}

TEST( FileWatcher, WatchValidPathReturnsTrue )
{
    std::string const path = make_tmp_file( "valid" );
    {
        aaa::meu::FileWatcher w;
        EXPECT_TRUE( w.watch( path, []{} ) );
        EXPECT_TRUE( w.is_watching() );
    }
    cleanup( path );
}

TEST( FileWatcher, IsWatchingReflectsState )
{
    std::string const path = make_tmp_file( "state" );
    {
        aaa::meu::FileWatcher w;
        EXPECT_FALSE( w.is_watching() );
        ASSERT_TRUE( w.watch( path, []{} ) );
        EXPECT_TRUE( w.is_watching() );
        EXPECT_EQ( w.watched_path(), path );
    }
    cleanup( path );
}

TEST( FileWatcher, UnwatchClearsState )
{
    std::string const path = make_tmp_file( "clear" );
    {
        aaa::meu::FileWatcher w;
        ASSERT_TRUE( w.watch( path, []{} ) );
        EXPECT_TRUE( w.is_watching() );
        w.unwatch();
        EXPECT_FALSE( w.is_watching() );
        EXPECT_TRUE( w.watched_path().empty() );

        //  Re-call unwatch is a no-op (idempotent).
        w.unwatch();
        EXPECT_FALSE( w.is_watching() );
    }
    cleanup( path );
}
