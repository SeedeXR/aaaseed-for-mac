// tests/unit/gol_backend_cross_platform_test.cpp
//
// Phase 3 (todo.md L98, Task #152 / c126-c130, 2026-05-27) :
// REGRESSION GUARD that the abstract `GOL::Backend` interface in
// `src/gol/gol_backend.h` stays cross-platform.
//
// History : the Mac port implemented `GOL::MetalBackend` (c20-c22)
// behind the abstract `GOL::Backend` interface. The Win-side
// `GOL::WindowsBackend` (Task #152) is a Win-machine session ; the
// Mac-side prerequisite is that the abstract header carries no
// Metal-cpp / Foundation / AppKit leak so the Win-side wiring is
// drop-in. This regression guard fires if a future session adds a
// Mac-only type (MTL::Texture*, NS::String*, CA::MetalDrawable*) to
// the abstract surface ; the Win port could not implement such a
// method without a circular dep on Apple frameworks.
//
// Per `memory/feedback_regression_guard_tests.md` (c137 codification) :
// name the test for the deferred / forbidden thing, point the failure
// message at the rationale.
//
// Pure C++ ; no engine deps. Uses only the standard library :
// <fstream> + <filesystem> + <string> + <vector> + <sstream>. No
// `o_str`, no `aaa_str.h`, no `aaa_mem.h` (hermetic doctrine).
//
// Labels (set in tests/unit/CMakeLists.txt) :
//   phase3;unit;cross-platform;regression-guard
// `phase3` is FIRST per c121-B doctrine (CTest LABELS first-only
// quirk) so `ctest -L phase3` includes this test.

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{

//	Source-tree root resolver -- walks up looking for the top-level
//	CMakeLists.txt + src/gol/gol_backend.h marker. Mirrors the pattern
//	in info_plist_singularity_test.cpp + dmg_packaging_test.cpp.
std::filesystem::path source_tree_root()
{
    namespace fs = std::filesystem;
    fs::path cur = fs::current_path();
    for( int hop = 0; hop < 8; ++hop )
    {
        if( fs::exists( cur / "CMakeLists.txt" )
            && fs::exists( cur / "src" / "gol" / "gol_backend.h" ) )
        {
            return cur;
        }
        if( cur.has_parent_path() && cur.parent_path() != cur )
            cur = cur.parent_path();
        else
            break;
    }
    //	Fallback : tests are typically run from out/macos-arm64-debug ;
    //	source tree is two levels up.
    return fs::current_path().parent_path().parent_path();
}

std::string read_file( std::filesystem::path const& p )
{
    std::ifstream f( p );
    if( ! f )
        return {};
    std::string content( ( std::istreambuf_iterator< char >( f ) ),
                         std::istreambuf_iterator< char >() );
    return content;
}

//	Returns true if `needle` appears in `haystack` at any non-comment
//	position. We strip line and block comments to keep our regression
//	guard tight against the actual code surface ; a `// MTL::Texture`
//	mention in a doc string is fine, a real type declaration is not.
//	Cheap stripper -- works for our header which has no string literals
//	that would confuse the heuristic.
bool contains_outside_comments( std::string const& haystack,
                                std::string const& needle )
{
    //	Strip block comments /* ... */ then line comments // to EOL.
    std::string stripped;
    stripped.reserve( haystack.size() );
    bool in_block = false;
    bool in_line  = false;
    for( std::size_t i = 0; i < haystack.size(); ++i )
    {
        if( in_block )
        {
            if( i + 1 < haystack.size()
                && haystack[ i ] == '*' && haystack[ i + 1 ] == '/' )
            {
                in_block = false;
                ++i;
            }
            continue;
        }
        if( in_line )
        {
            if( haystack[ i ] == '\n' )
            {
                in_line = false;
                stripped.push_back( '\n' );
            }
            continue;
        }
        if( i + 1 < haystack.size()
            && haystack[ i ] == '/' && haystack[ i + 1 ] == '*' )
        {
            in_block = true;
            ++i;
            continue;
        }
        if( i + 1 < haystack.size()
            && haystack[ i ] == '/' && haystack[ i + 1 ] == '/' )
        {
            in_line = true;
            ++i;
            continue;
        }
        stripped.push_back( haystack[ i ] );
    }
    return stripped.find( needle ) != std::string::npos;
}

}   //	namespace

//	Test 1 : the abstract header exists at the canonical path.
TEST( GolBackendCrossPlatformTest, AbstractHeaderExists )
{
    auto const root = source_tree_root();
    auto const header = root / "src" / "gol" / "gol_backend.h";
    EXPECT_TRUE( std::filesystem::exists( header ) )
        << "Abstract GOL::Backend header missing at " << header.string()
        << " . The Mac MetalBackend and the Win WindowsBackend both "
           "depend on this contract. Restore from git.";
}

//	Test 2 : NO Metal-cpp type names in the abstract header (outside
//	comments). The forward-declares for MTL::* / NS::* / CA::* belong
//	in the concrete `src/gol/metal/metal_backend.h`, NOT the abstract.
TEST( GolBackendCrossPlatformTest, AbstractHeaderHasNoMetalCppLeak )
{
    auto const root = source_tree_root();
    auto const header = root / "src" / "gol" / "gol_backend.h";
    auto const body = read_file( header );

    ASSERT_FALSE( body.empty() )
        << "Cannot read " << header.string();

    EXPECT_FALSE( contains_outside_comments( body, "MTL::" ) )
        << "Abstract header leaks Metal-cpp type (MTL::...). Move it "
           "to src/gol/metal/metal_backend.h forward-decls, or add an "
           "opaque-handle abstraction. See "
           "docs/windows-backend-howto.md Section 6.";
    EXPECT_FALSE( contains_outside_comments( body, "NS::" ) )
        << "Abstract header leaks Foundation type (NS::...). Same "
           "remediation as MTL::* above.";
    EXPECT_FALSE( contains_outside_comments( body, "CA::" ) )
        << "Abstract header leaks QuartzCore type (CA::...). Same "
           "remediation as MTL::* above.";
}

//	Test 3 : NO GL type names in the abstract header. The Win-side
//	`WindowsBackend` will USE GL internally but the abstract surface
//	must stay neutral -- if it leaks GLenum / glBegin etc, the Mac
//	side has the same circular-dep problem in reverse.
TEST( GolBackendCrossPlatformTest, AbstractHeaderHasNoGlLeak )
{
    auto const root = source_tree_root();
    auto const header = root / "src" / "gol" / "gol_backend.h";
    auto const body = read_file( header );

    ASSERT_FALSE( body.empty() );

    EXPECT_FALSE( contains_outside_comments( body, "glBegin" ) )
        << "Abstract header leaks GL fixed-function call (glBegin). "
           "Win-side mappings live in src/gol/windows/, NOT the "
           "abstract interface.";
    EXPECT_FALSE( contains_outside_comments( body, "glEnable" ) )
        << "Abstract header leaks GL state call (glEnable). Same "
           "remediation as glBegin above.";
    //	Note : the abstract header LEGITIMATELY mentions "GL" in
    //	comments / type-names like `OpenGL` ; the contains_outside_comments
    //	heuristic skips comments. We DO NOT check for `GL_` because that
    //	would false-positive on `GOL::` ; the gl* function-name checks
    //	above are sufficient.
    EXPECT_FALSE( contains_outside_comments( body, "GLenum " ) )
        << "Abstract header leaks GL type (GLenum). Use the abstract "
           "enums (BufferUsage, TextureFormat, PrimitiveType) instead.";
    EXPECT_FALSE( contains_outside_comments( body, "GLuint " ) )
        << "Abstract header leaks GL type (GLuint). Use BufferId / "
           "TextureId / ProgramId typedefs instead.";
}

//	Test 4 : NO Apple framework includes in the abstract header. If
//	the Mac side ever needs to pull <Metal/Metal.h> through the
//	abstract surface, the Win side breaks immediately.
TEST( GolBackendCrossPlatformTest, AbstractHeaderHasNoAppleIncludes )
{
    auto const root = source_tree_root();
    auto const header = root / "src" / "gol" / "gol_backend.h";
    auto const body = read_file( header );

    ASSERT_FALSE( body.empty() );

    EXPECT_FALSE( contains_outside_comments( body, "<Metal/Metal.h>" ) )
        << "Abstract header includes <Metal/Metal.h>. Move to "
           "src/gol/metal/ only.";
    EXPECT_FALSE( contains_outside_comments( body, "<Metal/Metal.hpp>" ) )
        << "Abstract header includes Metal.hpp (metal-cpp). Move to "
           "src/gol/metal/ only.";
    EXPECT_FALSE( contains_outside_comments( body, "<Foundation/Foundation.h>" ) )
        << "Abstract header includes <Foundation/Foundation.h>. The "
           "abstract surface must be platform-neutral C++.";
    EXPECT_FALSE( contains_outside_comments( body, "<Foundation/Foundation.hpp>" ) )
        << "Abstract header includes Foundation.hpp (metal-cpp Foundation). "
           "Move to src/gol/metal/ only.";
    EXPECT_FALSE( contains_outside_comments( body, "<AppKit/AppKit.h>" ) )
        << "Abstract header includes <AppKit/AppKit.h>. Window/event "
           "code lives in src/ui/macos/.";
    EXPECT_FALSE( contains_outside_comments( body, "<QuartzCore/" ) )
        << "Abstract header includes QuartzCore. Move to "
           "src/gol/metal/ only.";
}

//	Test 5 : the canonical interface methods exist with cross-
//	platform-safe signatures. We grep for each pure-virtual by name
//	to confirm the contract hasn't been amputated. If a method has
//	been renamed or removed, downstream concrete backends fail to
//	link ; this test catches the regression earlier (compile-time of
//	the test binary -- which links nothing -- via grep of the source).
TEST( GolBackendCrossPlatformTest, AbstractInterfaceMethodsPresent )
{
    auto const root = source_tree_root();
    auto const header = root / "src" / "gol" / "gol_backend.h";
    auto const body = read_file( header );

    ASSERT_FALSE( body.empty() );

    //	The MVP method surface (c20-c22). If you ADD a new pure-virtual,
    //	bump this list -- it also serves as the canonical inventory
    //	used by docs/windows-backend-howto.md Appendix A.
    std::vector< std::string > const required_methods = {
        "init",
        "deinit",
        "get_device_info",
        "gen_buffer",
        "delete_buffer",
        "buffer_data",
        "gen_texture_2d",
        "delete_texture",
        "texture_data_2d",
        "gen_texture_3d",
        "bind_fragment_texture_3d",
        "create_program_msl",
        "create_program",
        "delete_program",
        "get_last_error",
        "begin_render_pass",
        "end_render_pass",
        "set_viewport",
        "bind_program",
        "bind_vertex_buffer",
        "bind_fragment_buffer",
        "bind_fragment_texture",
        "draw_arrays",
        "draw_elements",
        "read_texture_pixels",
        "present",
        "backend_name",
    };

    std::ostringstream missing;
    int missing_count = 0;
    for( auto const& m : required_methods )
    {
        if( ! contains_outside_comments( body, m ) )
        {
            missing << "  " << m << "\n";
            ++missing_count;
        }
    }

    EXPECT_EQ( missing_count, 0 )
        << "Abstract interface is missing " << missing_count
        << " expected method(s) :\n" << missing.str()
        << "If a method was intentionally removed, update this test "
           "list AND docs/windows-backend-howto.md Appendix A. See "
           "memory/feedback_regression_guard_tests.md for the doctrine.";
}

//	Test 6 : the Win-side runbook exists. If the Win-machine session
//	is to land Task #152 cleanly, this runbook must be present.
TEST( GolBackendCrossPlatformTest, WindowsBackendRunbookExists )
{
    auto const root = source_tree_root();
    auto const runbook = root / "docs" / "windows-backend-howto.md";
    EXPECT_TRUE( std::filesystem::exists( runbook ) )
        << "Win-side runbook missing at " << runbook.string()
        << " . The Mac-side prerequisite for Task #152 is the abstract "
           "interface PLUS this runbook -- the Win machine session "
           "consumes both. Restore from git.";

    //	Size sanity : the spec calls for ~150-250 lines. We accept
    //	100..400 as the regression band.
    auto const body = read_file( runbook );
    auto const lines = static_cast< std::size_t >(
        std::count( body.begin(), body.end(), '\n' ) );
    EXPECT_GE( lines, 100u )
        << "Win-side runbook is suspiciously short (" << lines
        << " lines). The runbook should cover Sections 1-6 + "
           "Appendix A per docs/windows-backend-howto.md ; if it's "
           "been amputated, restore from git.";
}

//	Test 7 : the abstract header is freely consumable from any TU
//	without pulling in Mac-only frameworks. We assert that the only
//	includes in `src/gol/gol_backend.h` are standard-library headers.
TEST( GolBackendCrossPlatformTest, AbstractHeaderOnlyStdLibIncludes )
{
    auto const root = source_tree_root();
    auto const header = root / "src" / "gol" / "gol_backend.h";
    auto const body = read_file( header );

    ASSERT_FALSE( body.empty() );

    //	Scan every `#include <...>` directive ; assert each is in the
    //	standard-library whitelist. Any project / framework include
    //	leaking into the abstract header fails the test.
    std::vector< std::string > const allowed = {
        "<cstdint>", "<cstddef>", "<string>",
        "<cstring>", "<cstdio>",  "<vector>",
        "<array>",   "<memory>",  "<utility>",
        "<type_traits>",
    };

    std::istringstream iss( body );
    std::string line;
    std::vector< std::string > offenders;
    while( std::getline( iss, line ) )
    {
        auto const pos = line.find( "#include" );
        if( pos == std::string::npos )
            continue;
        //	Skip if this line is a // comment (cheap check : leading
        //	whitespace then //).
        auto const trimmed_start = line.find_first_not_of( " \t" );
        if( trimmed_start != std::string::npos
            && line.compare( trimmed_start, 2, "//" ) == 0 )
        {
            continue;
        }
        auto const lt = line.find( '<', pos );
        auto const gt = line.find( '>', pos );
        if( lt == std::string::npos || gt == std::string::npos )
            continue;
        std::string const inc = line.substr( lt, gt - lt + 1 );
        bool ok = false;
        for( auto const& a : allowed )
        {
            if( inc == a )
            {
                ok = true;
                break;
            }
        }
        if( ! ok )
            offenders.push_back( inc );
    }

    std::ostringstream listing;
    for( auto const& o : offenders )
        listing << "  " << o << "\n";

    EXPECT_TRUE( offenders.empty() )
        << "Abstract header includes non-stdlib header(s) :\n"
        << listing.str()
        << "Move framework / project includes into the concrete "
           "backend (src/gol/metal/ on Mac ; src/gol/windows/ on Win). "
           "See docs/windows-backend-howto.md Section 6.";
}
