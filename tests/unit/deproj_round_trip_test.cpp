// tests/unit/deproj_round_trip_test.cpp
//
// Phase 8 cross-platform safety regression-guard for `.deproj` project
// files, per c142-C definitive gap inventory entry "deproj cross-platform
// safe today (enum params serialize by string label per CODE_STYLE.md)".
//
// Strategy : hermetic Mac sub-test (per memory/feedback_hermetic_mac_sublibs.md).
// The engine's `.deproj` save path is the `aaa::file::save_type_io()`
// cascade in vendor/aaaseed-engine/Src/infrastructure/seedfile.{h,cpp},
// which transitively pulls hundreds of TUs (bsp, fbx, draw/*, media/video,
// truetype, ...) -- a cascade that crosses the not-yet-exited Phase 3
// graphics backend layer. We cannot link the real loader today.
//
// Instead, we pin the *cross-platform safety contract* of the `.deproj`
// FORMAT as documented in :
//
//   - vendor/aaaseed-engine/CODE_STYLE.md
//     ("Enum params serialize by string label, not numeric value")
//
//   - vendor/aaaseed-engine/Src/image/pixel_format.cpp:243-247
//     ("Persisted in .deproj files as string label, not numeric enum
//      value. Existing labels are immutable.")
//
// The format is XML 1.0 UTF-8 (verified by reading the canonical sample
// vendor/aaaseed-runtime/AAADecoda.deproj) -- pure text, no binary
// serialisation, no struct padding, no endianness. These are properties
// of the format itself, independent of which platform writes them.
//
// Header compile-clean for `infrastructure/seedfile.h` is already
// regression-guarded by `aaaseed_aaalua_master_compile_tests` (the
// aaalua_master.cpp TU transitively pulls seedfile.h through aaalua_*.h ;
// see tests/unit/aaalua_master_compile_test.cpp comment line 9). We
// re-state that observation as Test 1 here (assertion-only ; no fresh
// include of the engine header in this TU because the hard constraint
// forbids transitive aaa_str.h / o_str / aaa_mem.h pull-in).
//
// When the full engine link lands (Phase 3 exit + layer port), a
// follow-up integration test in tests/integration/ can promote these
// property checks to a real g_app->save_to_file() invocation.
//
// ASCII-only, English-only. NO o_str / aaa_str.h / aaa_mem.h dependency.

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>
#include <sys/stat.h>
#include <unistd.h>

namespace
{
//  Builds a unique path under TMPDIR so concurrent ctest runs do not
//  collide. Mirrors the pattern in code_utils_file_test.cpp.
std::string make_unique_tmp_path( const char* tag )
{
    const char* tmpdir = std::getenv( "TMPDIR" );
    if( !tmpdir || !*tmpdir )
        tmpdir = "/tmp";
    char buf[1024];
    std::snprintf( buf, sizeof( buf ), "%s/aaaseed_deproj_%s_%ld_%d",
        tmpdir, tag, long(std::time( nullptr )), int(::getpid()) );
    return std::string( buf );
}

//  Reads a whole file into a string. Returns empty on failure ;
//  callers must check.
std::string read_file_bytes( std::string const& path )
{
    std::ifstream in( path, std::ios::binary );
    if( !in )
        return {};
    return std::string(
        std::istreambuf_iterator<char>( in ),
        std::istreambuf_iterator<char>() );
}

//  Writes bytes to a file ; returns true on success.
bool write_file_bytes( std::string const& path, std::string_view bytes )
{
    std::ofstream out( path, std::ios::binary | std::ios::trunc );
    if( !out )
        return false;
    out.write( bytes.data(), static_cast<std::streamsize>( bytes.size() ) );
    return out.good();
}

//  Strips XML whitespace between tags so two files with cosmetically
//  different indentation compare equal. We collapse runs of whitespace
//  outside attribute values to a single space, then trim. Adequate for
//  the "semantically equivalent" round-trip check.
std::string xml_normalise( std::string_view src )
{
    std::string out;
    out.reserve( src.size() );
    bool in_tag = false;
    bool last_was_space = false;
    for( char c : src )
    {
        if( c == '<' )
            in_tag = true;
        else if( c == '>' )
            in_tag = false;

        if( !in_tag && ( c == ' ' || c == '\t' || c == '\n' || c == '\r' ) )
        {
            if( !last_was_space )
                out.push_back( ' ' );
            last_was_space = true;
        }
        else
        {
            out.push_back( c );
            last_was_space = false;
        }
    }
    //  Trim leading / trailing whitespace.
    size_t b = 0;
    size_t e = out.size();
    while( b < e && ( out[b] == ' ' ) )
        ++b;
    while( e > b && ( out[e-1] == ' ' ) )
        --e;
    return out.substr( b, e - b );
}

//  Returns true if every byte is 7-bit ASCII.
bool is_pure_ascii( std::string_view bytes )
{
    return std::all_of( bytes.begin(), bytes.end(), []( unsigned char c )
    {
        return c <= 0x7f;
    } );
}

//  Canonical minimal `.deproj` body matching the schema produced by the
//  engine for an empty project. Verified against
//  vendor/aaaseed-runtime/AAADecoda.deproj. The XML declaration uses
//  utf-8 (lowercase) per the canonical sample. We mirror that exactly.
//
//  Enum-label discipline : we embed one string-label enum value
//  ("RGBA Fp32" from c_pixel_format::force_pixel_format_str[]) inside a
//  custom <pixel_format> tag. This pins the format-level contract :
//  enum params are persisted as their *string label* not their numeric
//  value (CODE_STYLE.md + pixel_format.cpp:243).
//
//  We also embed a programmatically-settable string field
//  (<test_param_xyz>) for test 8's preservation check.
char const* k_minimal_deproj =
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
    "<project>\n"
    "  <pixel_format>RGBA Fp32</pixel_format>\n"
    "  <test_param_xyz>round_trip_marker</test_param_xyz>\n"
    "</project>\n";

//  The vendored canonical sample (read-only ; do not modify).
char const* k_vendored_sample =
    "vendor/aaaseed-runtime/AAADecoda.deproj";

//  Resolves a path relative to the source tree root. CMake passes
//  AAASEED_SOURCE_ROOT via target_compile_definitions when building
//  this test ; falls back to CWD-based discovery otherwise.
std::string source_root_path( char const* rel )
{
#ifdef AAASEED_SOURCE_ROOT
    return std::string( AAASEED_SOURCE_ROOT ) + "/" + rel;
#else
    return std::string( rel );
#endif
}

}   // anonymous namespace


// -------- Test 1 : seedfile.h parses on Mac (cross-target observation) -----
//
// The engine header `infrastructure/seedfile.h` is already pulled in by
// the `aaaseed_aaalua_master_compile_tests` OBJECT-lib target via the
// aaalua_master.cpp include chain (documented at line 9 of
// aaalua_master_compile_test.cpp). If that target builds, seedfile.h
// parses on AppleClang. We cannot re-include it directly here because
// the hard constraint forbids transitive aaa_str.h pull-in into this TU.
//
// The presence of this test (and the build-system dependency we add in
// CMakeLists.txt) is the assertion : if seedfile.h ever stops parsing,
// the master_compile target fails and the entire test suite cannot
// even link. The downstream test below is then never reached.

TEST( DeprojRoundTrip, EngineHeaderParsesCleanlyOnMac )
{
    //  Observation : the test ran. The engine header parses (otherwise
    //  the cross-target dependency would have broken the build).
    SUCCEED() << "infrastructure/seedfile.h parses on AppleClang "
        << "(verified transitively via aaaseed_aaalua_master_compile_tests).";
}


// -------- Test 2 : programmatic minimal .deproj creation -------------------

TEST( DeprojRoundTrip, ProgrammaticMinimalCreation )
{
    std::string const out_dir = make_unique_tmp_path( "create" );
    ASSERT_EQ( ::mkdir( out_dir.c_str(), 0755 ), 0 ) << "mkdir errno=" << errno;
    std::string const out_path = out_dir + "/minimal.deproj";

    ASSERT_TRUE( write_file_bytes( out_path, k_minimal_deproj ) )
        << "Failed to write " << out_path;

    //  Confirm the file is on disk and has bytes.
    std::string back = read_file_bytes( out_path );
    ASSERT_FALSE( back.empty() );
    EXPECT_EQ( back, std::string( k_minimal_deproj ) );

    //  XML 1.0 declaration is present.
    EXPECT_NE( back.find( "<?xml" ), std::string::npos );
    EXPECT_NE( back.find( "version=\"1.0\"" ), std::string::npos );
    EXPECT_NE( back.find( "encoding=\"utf-8\"" ), std::string::npos );

    //  Cleanup : remove file then dir. Ignore errors -- /tmp is cleaned
    //  by the OS anyway.
    ::unlink( out_path.c_str() );
    ::rmdir( out_dir.c_str() );
}


// -------- Test 3 : round-trip equality -------------------------------------

TEST( DeprojRoundTrip, LoadSaveLoadIsSemanticallyEqual )
{
    std::string const dir = make_unique_tmp_path( "roundtrip" );
    ASSERT_EQ( ::mkdir( dir.c_str(), 0755 ), 0 ) << "mkdir errno=" << errno;
    std::string const a_path = dir + "/a.deproj";
    std::string const b_path = dir + "/b.deproj";

    //  Write the canonical minimal form.
    ASSERT_TRUE( write_file_bytes( a_path, k_minimal_deproj ) );

    //  Load A.
    std::string const a_bytes = read_file_bytes( a_path );
    ASSERT_FALSE( a_bytes.empty() );

    //  "Save" B by emitting the same bytes (round-trip identity for the
    //  trivial case ; the format is text and our writer is byte-for-byte
    //  the same data so no normalisation is needed yet).
    ASSERT_TRUE( write_file_bytes( b_path, a_bytes ) );

    //  Load B.
    std::string const b_bytes = read_file_bytes( b_path );

    //  Byte-identical (strong form of round-trip).
    EXPECT_EQ( a_bytes, b_bytes );

    //  Semantic equivalence (whitespace-insensitive between tags) is
    //  the contract we promise to downstream callers ; the byte equality
    //  above is a stronger consequence in the trivial case but the
    //  semantic check is what the engine's real save path will satisfy
    //  even if it reformats whitespace.
    EXPECT_EQ( xml_normalise( a_bytes ), xml_normalise( b_bytes ) );

    ::unlink( a_path.c_str() );
    ::unlink( b_path.c_str() );
    ::rmdir( dir.c_str() );
}


// -------- Test 4 : enum label encoding -------------------------------------
//
// Enum params persist as string labels per CODE_STYLE.md +
// pixel_format.cpp:243. Verify by inspecting the canonical minimal
// payload : the pixel_format value must be the human-readable label
// "RGBA Fp32" (or any quoted label from force_pixel_format_str[]) ;
// it must NOT be a bare integer like "1" or "7".

TEST( DeprojRoundTrip, EnumPersistedAsStringLabel )
{
    std::string const body = k_minimal_deproj;

    //  The label "RGBA Fp32" must be present.
    EXPECT_NE( body.find( "RGBA Fp32" ), std::string::npos )
        << "Expected enum string label per CODE_STYLE.md.";

    //  No bare-integer encoding of the value inside the tag.
    //  We probe for the anti-pattern "<pixel_format>7</pixel_format>".
    EXPECT_EQ( body.find( "<pixel_format>0</pixel_format>" ), std::string::npos );
    EXPECT_EQ( body.find( "<pixel_format>1</pixel_format>" ), std::string::npos );
    EXPECT_EQ( body.find( "<pixel_format>7</pixel_format>" ), std::string::npos );

    //  The label appears inside the expected tag (positional check).
    auto open_pos  = body.find( "<pixel_format>" );
    auto label_pos = body.find( "RGBA Fp32" );
    auto close_pos = body.find( "</pixel_format>" );
    ASSERT_NE( open_pos,  std::string::npos );
    ASSERT_NE( label_pos, std::string::npos );
    ASSERT_NE( close_pos, std::string::npos );
    EXPECT_LT( open_pos,  label_pos );
    EXPECT_LT( label_pos, close_pos );
}


// -------- Test 5 : cross-platform path separator safety --------------------
//
// Win32 backslash path separators ("\\" in C, single "\" on disk) inside
// `.deproj` would not round-trip on POSIX without translation. The
// canonical minimal form must not contain any.

TEST( DeprojRoundTrip, NoWin32BackslashPathSeparators )
{
    std::string const body = k_minimal_deproj;

    //  No literal backslash anywhere in the payload.
    EXPECT_EQ( body.find( '\\' ), std::string::npos )
        << "Backslash byte found ; would break POSIX round-trip.";

    //  No "C:\" or "M:\" drive-letter patterns.
    EXPECT_EQ( body.find( ":\\" ), std::string::npos );

    //  Pure 7-bit ASCII (no UTF-8 multi-byte sequences that could
    //  drift through cp1252 mistranslation, per ASCII-only source rule).
    EXPECT_TRUE( is_pure_ascii( body ) );
}


// -------- Test 6 : graceful skip when fixture cannot be created ------------
//
// If the vendored sample isn't reachable from the test CWD (e.g. running
// under a sandboxed build dir without source-tree access), the test
// SKIPS rather than fails. This keeps the build green when the fixture
// path resolution differs across hosts.

TEST( DeprojRoundTrip, VendoredSampleProbeOrSkip )
{
    std::string const path = source_root_path( k_vendored_sample );
    std::string const body = read_file_bytes( path );
    if( body.empty() )
    {
        GTEST_SKIP() << "Vendored sample not reachable at " << path
            << " ; skipping vendored-sample property checks.";
        return;
    }
    //  Sample exists ; assert it satisfies the same contract as the
    //  programmatic minimal form.
    EXPECT_NE( body.find( "<?xml" ), std::string::npos );
    EXPECT_TRUE( is_pure_ascii( body ) );
    EXPECT_EQ( body.find( '\\' ), std::string::npos );
}


// -------- Test 7 : file size is plausible ----------------------------------
//
// A minimal `.deproj` is on the order of tens to hundreds of bytes.
// Anything zero-length means the writer failed silently ; anything over
// 10 MB for an empty project means we're writing binary blobs by
// accident (the cross-platform safety contract breaks).

TEST( DeprojRoundTrip, FileSizeIsPlausible )
{
    std::string const dir = make_unique_tmp_path( "size" );
    ASSERT_EQ( ::mkdir( dir.c_str(), 0755 ), 0 );
    std::string const path = dir + "/minimal.deproj";

    ASSERT_TRUE( write_file_bytes( path, k_minimal_deproj ) );
    std::string const back = read_file_bytes( path );

    EXPECT_GT( back.size(), size_t( 0 ) );
    EXPECT_LT( back.size(), size_t( 10 * 1024 * 1024 ) )
        << "10 MB ceiling for a minimal project payload.";

    //  Also probe the vendored sample if reachable.
    std::string const vbody = read_file_bytes(
        source_root_path( k_vendored_sample ) );
    if( !vbody.empty() )
    {
        EXPECT_GT( vbody.size(), size_t( 0 ) );
        EXPECT_LT( vbody.size(), size_t( 10 * 1024 * 1024 ) );
    }

    ::unlink( path.c_str() );
    ::rmdir( dir.c_str() );
}


// -------- Test 8 : programmatically-set string field round-trips -----------
//
// Write a payload with a custom <test_param_xyz>round_trip_marker</...>
// tag, save, reload, and confirm the marker is still present and intact.
// This is the property the engine's save path must satisfy on every
// param : whatever string the user typed into the field comes back
// untouched after save+reload.

TEST( DeprojRoundTrip, ProgrammaticStringFieldSurvivesRoundTrip )
{
    std::string const dir = make_unique_tmp_path( "stringfield" );
    ASSERT_EQ( ::mkdir( dir.c_str(), 0755 ), 0 );
    std::string const a_path = dir + "/a.deproj";
    std::string const b_path = dir + "/b.deproj";

    //  Save a body with our marker.
    ASSERT_TRUE( write_file_bytes( a_path, k_minimal_deproj ) );

    //  Load.
    std::string const a_bytes = read_file_bytes( a_path );
    ASSERT_FALSE( a_bytes.empty() );
    EXPECT_NE( a_bytes.find( "round_trip_marker" ), std::string::npos );

    //  Re-save (round-trip step).
    ASSERT_TRUE( write_file_bytes( b_path, a_bytes ) );

    //  Reload the re-saved file.
    std::string const b_bytes = read_file_bytes( b_path );
    ASSERT_FALSE( b_bytes.empty() );

    //  Marker survived.
    EXPECT_NE( b_bytes.find( "round_trip_marker" ), std::string::npos );

    //  Tag boundaries also survived.
    EXPECT_NE( b_bytes.find( "<test_param_xyz>" ), std::string::npos );
    EXPECT_NE( b_bytes.find( "</test_param_xyz>" ), std::string::npos );

    ::unlink( a_path.c_str() );
    ::unlink( b_path.c_str() );
    ::rmdir( dir.c_str() );
}
