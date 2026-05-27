// Tests the POSIX primitives used by the Mac branches of
//   Src/code_utils/file/aaa_dir.cpp
//   Src/code_utils/file/aaa_file.cpp
//
// These tests exercise the underlying syscalls (mkdir / rmdir / opendir /
// readdir / closedir / open / read / write / fstat / unlink / lseek)
// without pulling in c_file / c_dir which transitively need mem / err / o_str
// initialization machinery that lives outside this unit. The intent is to
// pin down the POSIX contract our Mac branches rely on : if any of these
// tests fails on a future SDK / libc revision, the file-subsystem Mac branch
// will fail too in the same way.

#include <gtest/gtest.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "aaa_type.h"

namespace
{
//  Builds a unique path under the system temp dir so concurrent CTest runs
//  do not collide. The caller owns cleanup.
std::string make_unique_tmp_path( const char* tag )
{
    const char* tmpdir = std::getenv( "TMPDIR" );
    if( !tmpdir || !*tmpdir )
        tmpdir = "/tmp";
    char buf[1024];
    std::snprintf( buf, sizeof( buf ), "%s/aaaseed_%s_%ld_%d",
        tmpdir, tag, long(std::time( nullptr )), int(::getpid()) );
    return std::string( buf );
}
}

// -------- aaa_dir.cpp Mac branch primitives --------------------------------

TEST( CodeUtilsDir, MkdirThenRmdirRoundTrip )
{
    std::string const dir = make_unique_tmp_path( "mkdir" );

    //  mkdir(0755) -- same call the Mac branch makes via _mkdir.
    ASSERT_EQ( ::mkdir( dir.c_str(), 0755 ), 0 ) << "errno = " << errno;

    //  stat -> S_ISDIR : same checks c_file::is_existing_dir() runs.
    struct stat st{};
    ASSERT_EQ( ::stat( dir.c_str(), &st ), 0 );
    EXPECT_TRUE( S_ISDIR( st.st_mode ) );

    //  rmdir : same syscall the Mac branch uses via aaa_remove_tree_recursive.
    ASSERT_EQ( ::rmdir( dir.c_str() ), 0 );

    //  Gone.
    EXPECT_NE( ::stat( dir.c_str(), &st ), 0 );
    EXPECT_EQ( errno, ENOENT );
}

TEST( CodeUtilsDir, MkdirReturnsEexistOnSecondCall )
{
    std::string const dir = make_unique_tmp_path( "mkdir_eexist" );
    ASSERT_EQ( ::mkdir( dir.c_str(), 0755 ), 0 );

    //  Second mkdir must fail with EEXIST. The Mac branch of c_dir::make
    //  tolerates EEXIST as success.
    EXPECT_NE( ::mkdir( dir.c_str(), 0755 ), 0 );
    EXPECT_EQ( errno, EEXIST );

    ASSERT_EQ( ::rmdir( dir.c_str() ), 0 );
}

TEST( CodeUtilsDir, GetcwdReadsBack )
{
    char buf[4096];
    char* p = ::getcwd( buf, sizeof( buf ) );
    ASSERT_NE( p, nullptr ) << "getcwd failed, errno = " << errno;
    EXPECT_GT( std::strlen( p ), size_t( 0 ) );
}

TEST( CodeUtilsDir, ChdirRoundTrip )
{
    char before[4096];
    ASSERT_NE( ::getcwd( before, sizeof( before ) ), nullptr );

    //  /tmp is reliably present on macOS.
    ASSERT_EQ( ::chdir( "/tmp" ), 0 );

    char now[4096];
    ASSERT_NE( ::getcwd( now, sizeof( now ) ), nullptr );
    //  On macOS /tmp is a symlink to /private/tmp ; accept either.
    EXPECT_TRUE( std::string( now ) == "/tmp"
              || std::string( now ) == "/private/tmp" );

    //  Restore so we don't leak cwd state into other tests.
    ASSERT_EQ( ::chdir( before ), 0 );
}

TEST( CodeUtilsDir, OpendirReaddirClosedirSeesEntries )
{
    //  Create a temp dir with a single known child file.
    std::string const dir = make_unique_tmp_path( "dirscan" );
    ASSERT_EQ( ::mkdir( dir.c_str(), 0755 ), 0 );
    std::string const child = dir + "/hello.txt";
    {
        int fd = ::open( child.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644 );
        ASSERT_GE( fd, 0 );
        ::close( fd );
    }

    DIR* d = ::opendir( dir.c_str() );
    ASSERT_NE( d, nullptr );

    bool found = false;
    struct dirent* e = nullptr;
    while( (e = ::readdir( d )) != nullptr )
    {
        if( std::string( e->d_name ) == "hello.txt" )
        {
            found = true;
            break;
        }
    }
    ::closedir( d );
    EXPECT_TRUE( found );

    //  Cleanup.
    ASSERT_EQ( ::unlink( child.c_str() ), 0 );
    ASSERT_EQ( ::rmdir( dir.c_str() ), 0 );
}

TEST( CodeUtilsDir, RecursiveRemoveWalksTree )
{
    //  Mirrors the structure aaa_remove_tree_recursive must handle.
    std::string const root = make_unique_tmp_path( "rmtree" );
    std::string const sub  = root + "/sub";
    std::string const f1   = root + "/f1.txt";
    std::string const f2   = sub  + "/f2.txt";

    ASSERT_EQ( ::mkdir( root.c_str(), 0755 ), 0 );
    ASSERT_EQ( ::mkdir( sub.c_str(),  0755 ), 0 );
    for( auto const& path : { f1, f2 } )
    {
        int fd = ::open( path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644 );
        ASSERT_GE( fd, 0 );
        ::close( fd );
    }

    //  Direct rmdir on a non-empty dir must fail with ENOTEMPTY.
    EXPECT_NE( ::rmdir( root.c_str() ), 0 );
    EXPECT_EQ( errno, ENOTEMPTY );

    //  Unlink + rmdir manually in dependency order.
    ASSERT_EQ( ::unlink( f2.c_str() ),  0 );
    ASSERT_EQ( ::unlink( f1.c_str() ),  0 );
    ASSERT_EQ( ::rmdir(  sub.c_str() ), 0 );
    ASSERT_EQ( ::rmdir(  root.c_str() ),0 );
}


// -------- aaa_file.cpp Mac branch primitives -------------------------------

TEST( CodeUtilsFile, OpenWriteReadCloseRoundTrip )
{
    std::string const path = make_unique_tmp_path( "file" ) + ".bin";

    //  open / write -- the calls behind the new Mac branch.
    int fd = ::open( path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644 );
    ASSERT_GE( fd, 0 ) << "errno = " << errno;

    char const payload[] = "AAASeed-Mac";
    size_t const len = sizeof( payload ) - 1;
    ssize_t written = ::write( fd, payload, len );
    EXPECT_EQ( written, ssize_t( len ) );
    ASSERT_EQ( ::close( fd ), 0 );

    //  Read back.
    fd = ::open( path.c_str(), O_RDONLY );
    ASSERT_GE( fd, 0 );
    char readbuf[64] = { 0 };
    ssize_t read_n = ::read( fd, readbuf, sizeof( readbuf ) - 1 );
    EXPECT_EQ( read_n, ssize_t( len ) );
    EXPECT_STREQ( readbuf, payload );

    //  fstat + st_size -- same path GetFileSize -> fstat in get_size( FILE* ).
    struct stat st{};
    ASSERT_EQ( ::fstat( fd, &st ), 0 );
    EXPECT_EQ( INT64( st.st_size ), INT64( len ) );
    EXPECT_TRUE( S_ISREG( st.st_mode ) );
    ASSERT_EQ( ::close( fd ), 0 );

    //  unlink -> stat must report ENOENT.
    ASSERT_EQ( ::unlink( path.c_str() ), 0 );
    EXPECT_NE( ::stat( path.c_str(), &st ), 0 );
    EXPECT_EQ( errno, ENOENT );
}

TEST( CodeUtilsFile, LseekRepositionsAndReadsLater )
{
    std::string const path = make_unique_tmp_path( "lseek" ) + ".bin";

    int fd = ::open( path.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644 );
    ASSERT_GE( fd, 0 );

    char const payload[] = "0123456789";
    ssize_t const total = sizeof( payload ) - 1;
    ASSERT_EQ( ::write( fd, payload, total ), total );

    //  Seek to offset 4, expect to read "456789".
    off_t pos = ::lseek( fd, 4, SEEK_SET );
    EXPECT_EQ( pos, off_t( 4 ) );
    char buf[16] = { 0 };
    ssize_t n = ::read( fd, buf, sizeof( buf ) - 1 );
    EXPECT_EQ( n, ssize_t( 6 ) );
    EXPECT_STREQ( buf, "456789" );

    //  SEEK_END must report EOF.
    off_t end = ::lseek( fd, 0, SEEK_END );
    EXPECT_EQ( end, off_t( total ) );

    ASSERT_EQ( ::close( fd ), 0 );
    ASSERT_EQ( ::unlink( path.c_str() ), 0 );
}

TEST( CodeUtilsFile, FopenFcloseRoundTrip )
{
    //  Engine's c_file::FOPEN ultimately calls fopen() ; fileno + fstat is
    //  the Mac path inside get_size( FILE* ).
    std::string const path = make_unique_tmp_path( "fopen" ) + ".txt";

    FILE* fp = std::fopen( path.c_str(), "wb" );
    ASSERT_NE( fp, nullptr );
    char const payload[] = "hello";
    size_t const len = sizeof( payload ) - 1;
    EXPECT_EQ( std::fwrite( payload, 1, len, fp ), len );
    EXPECT_EQ( std::fclose( fp ), 0 );

    fp = std::fopen( path.c_str(), "rb" );
    ASSERT_NE( fp, nullptr );

    //  Mac branch of c_file::get_size( FILE* ) : fstat( fileno( file ) ).
    struct stat st{};
    ASSERT_EQ( ::fstat( ::fileno( fp ), &st ), 0 );
    EXPECT_EQ( INT64( st.st_size ), INT64( len ) );

    EXPECT_EQ( std::fclose( fp ), 0 );
    ASSERT_EQ( ::unlink( path.c_str() ), 0 );
}

TEST( CodeUtilsFile, RenameMovesFile )
{
    //  Engine's MoveFile -> rename mapping.
    std::string const src = make_unique_tmp_path( "rename_src" ) + ".bin";
    std::string const dst = make_unique_tmp_path( "rename_dst" ) + ".bin";

    int fd = ::open( src.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644 );
    ASSERT_GE( fd, 0 );
    ASSERT_GE( ::write( fd, "x", 1 ), 1 );
    ASSERT_EQ( ::close( fd ), 0 );

    ASSERT_EQ( std::rename( src.c_str(), dst.c_str() ), 0 ) << "errno = " << errno;

    struct stat st{};
    EXPECT_NE( ::stat( src.c_str(), &st ), 0 );
    EXPECT_EQ( ::stat( dst.c_str(), &st ), 0 );

    ASSERT_EQ( ::unlink( dst.c_str() ), 0 );
}
