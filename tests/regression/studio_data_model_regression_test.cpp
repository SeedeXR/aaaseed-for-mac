// tests/regression/studio_data_model_regression_test.cpp
//
// c148 : Regression tests for the AAASeed ImGui Studio data model.
//
// These tests guard against regressions in the Studio's pure-C++ data
// model layer (no GPU, no ImGui context required). They verify exact
// expected states and sequences that must be preserved across refactors.
//
// Regression discipline per philosophy.md and todo.md Phase 7:
//   - A regression failure blocks the porting step from being marked done.
//   - Every test documents the specific invariant it guards.
//
// CTest label: "regression;studio".

#include <gtest/gtest.h>

#include "src/ui/studio/aaa_studio.h"

#include <cstdint>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static std::unique_ptr< aaa::ui::studio::Studio > make_studio()
{
    return std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
}

// ---------------------------------------------------------------------------
// REG-STUDIO-001 : Node id sequence is strictly monotonically increasing.
//
// Guards against id-counter resets or wrap-arounds that would break
// node identity in the node graph after remove/re-add cycles.
// ---------------------------------------------------------------------------
TEST( StudioRegression, NodeIdMonotonicallyIncreasing )
{
    auto studio = make_studio();

    std::vector< std::uint32_t > ids;
    for( int i = 0; i < 20; ++i )
        ids.push_back( studio->add_node( "N" + std::to_string( i ) ) );

    for( std::size_t i = 1; i < ids.size(); ++i )
        EXPECT_GT( ids[i], ids[i - 1] )
            << "id[" << i << "]=" << ids[i]
            << " not > id[" << (i-1) << "]=" << ids[i-1];
}

// ---------------------------------------------------------------------------
// REG-STUDIO-002 : Remove+re-add does not recycle the same id.
//
// Guards against a common bug where a freed id is immediately re-issued,
// making stale pointers appear valid.
// ---------------------------------------------------------------------------
TEST( StudioRegression, RemovedIdNotRecycled )
{
    auto studio = make_studio();

    std::uint32_t first = studio->add_node( "A" );
    studio->remove_node( first );
    std::uint32_t second = studio->add_node( "B" );

    EXPECT_NE( first, second )
        << "Studio recycled id " << first << " after remove";
}

// ---------------------------------------------------------------------------
// REG-STUDIO-003 : Link id sequence is strictly monotonically increasing.
// ---------------------------------------------------------------------------
TEST( StudioRegression, LinkIdMonotonicallyIncreasing )
{
    auto studio = make_studio();

    std::vector< std::uint32_t > link_ids;
    for( int i = 0; i < 10; ++i )
    {
        std::uint32_t a = studio->add_node( "A" + std::to_string( i ) );
        std::uint32_t b = studio->add_node( "B" + std::to_string( i ) );
        link_ids.push_back( studio->link_nodes( a, b ) );
    }

    for( std::size_t i = 1; i < link_ids.size(); ++i )
        EXPECT_GT( link_ids[i], link_ids[i - 1] )
            << "link_id[" << i << "] not strictly greater than previous";
}

// ---------------------------------------------------------------------------
// REG-STUDIO-004 : Node graph empty after removing all nodes.
//
// Guards against the list having stale entries after full clear.
// ---------------------------------------------------------------------------
TEST( StudioRegression, NodeListEmptyAfterRemoveAll )
{
    auto studio = make_studio();

    std::vector< std::uint32_t > ids;
    for( int i = 0; i < 8; ++i )
        ids.push_back( studio->add_node( "X" + std::to_string( i ) ) );

    for( auto id : ids )
        studio->remove_node( id );

    EXPECT_EQ( studio->nodes().size(), 0u )
        << "nodes() should be empty after removing all nodes";
    EXPECT_EQ( studio->links().size(), 0u )
        << "links() should be empty after removing all nodes";
}

// ---------------------------------------------------------------------------
// REG-STUDIO-005 : Editor text is preserved across node graph mutations.
//
// Guards against a regression where the editor text is accidentally
// cleared when nodes are added or removed.
// ---------------------------------------------------------------------------
TEST( StudioRegression, EditorTextPreservedAcrossNodeMutations )
{
    auto studio = make_studio();

    std::string const script = "local t = aaa.time()";
    studio->set_editor_text( script );

    std::uint32_t a = studio->add_node( "A" );
    std::uint32_t b = studio->add_node( "B" );
    studio->link_nodes( a, b );
    studio->remove_node( a );

    EXPECT_EQ( studio->editor_text(), script )
        << "editor_text() was corrupted by node graph mutation";
}

// ---------------------------------------------------------------------------
// REG-STUDIO-006 : Camera defaults are stable across multiple constructions.
//
// Guards against a regression where the camera default values change
// (e.g. due to an uninitialised struct field).
// ---------------------------------------------------------------------------
TEST( StudioRegression, CameraDefaultsStable )
{
    // Construct two independent studios and verify both have identical
    // camera defaults.
    auto s1 = make_studio();
    auto s2 = make_studio();

    auto const& c1 = s1->camera();
    auto const& c2 = s2->camera();

    EXPECT_FLOAT_EQ( c1.pos[0], c2.pos[0] );
    EXPECT_FLOAT_EQ( c1.pos[1], c2.pos[1] );
    EXPECT_FLOAT_EQ( c1.pos[2], c2.pos[2] );
    EXPECT_FLOAT_EQ( c1.fov_deg, c2.fov_deg );
    EXPECT_FLOAT_EQ( c1.near_z,  c2.near_z  );
    EXPECT_FLOAT_EQ( c1.far_z,   c2.far_z   );

    // Known defaults per aaa_studio.h
    EXPECT_FLOAT_EQ( c1.pos[2],  5.0f  );
    EXPECT_FLOAT_EQ( c1.fov_deg, 60.0f );
    EXPECT_FLOAT_EQ( c1.near_z,  0.01f );
    EXPECT_FLOAT_EQ( c1.far_z,   1000.0f );
}

// ---------------------------------------------------------------------------
// REG-STUDIO-007 : PerfRing survives wrap-around (>256 samples).
//
// Guards against the ring-buffer overflowing or crashing when more
// samples are pushed than the ring capacity.
// ---------------------------------------------------------------------------
TEST( StudioRegression, PerfRingWrapAroundSafe )
{
    auto studio = make_studio();

    EXPECT_NO_FATAL_FAILURE(
    {
        for( int i = 0; i < 1024; ++i )
            studio->push_perf_sample( float(i % 60) * 0.5f );
    } );
}

// ---------------------------------------------------------------------------
// REG-STUDIO-008 : Unlink non-existent link id is a no-op.
//
// Guards against an assert/crash when Lua scripts call studio.unlink()
// with a stale id.
// ---------------------------------------------------------------------------
TEST( StudioRegression, UnlinkStaleIdIsNoOp )
{
    auto studio = make_studio();
    EXPECT_NO_FATAL_FAILURE( studio->unlink( 0u )       );
    EXPECT_NO_FATAL_FAILURE( studio->unlink( 999999u )  );
}
