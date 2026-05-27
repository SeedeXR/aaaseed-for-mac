// tests/unit/aaa_bdd_cell_draw_mac_test.cpp
//
// c145 (Task #30 honest closure) : unit tests for the portable
// `aaa::bdd::*` math helpers in `src/bdd/aaa_bdd_cell_draw_mac.{h,cpp}`,
// plus a compile-only proof that the `aaa::bdd::Renderer` abstract
// interface (`src/bdd/aaa_bdd_renderer.h`) is well-formed (a no-op
// subclass instantiates).
//
// Hermetic test : no engine link, no Metal, no Objective-C. Pure C++.
// Labels : `unit;bdd;phase3`.

#include "src/bdd/aaa_bdd_cell_draw_mac.h"
#include "src/bdd/aaa_bdd_renderer.h"

#include <gtest/gtest.h>

#include <cmath>

namespace
{

constexpr double k_eps = 1e-9;

//	No-op Renderer subclass : compiles iff the abstract interface is
//	self-consistent (all pure-virtuals match across declaration +
//	override). Verified at test time by `Renderer.NoOpSubclassCompiles`.
class NoopRenderer : public aaa::bdd::Renderer
{
public:
    aaa::bdd::MeshHandle create_mesh(
            std::vector< float > const&,
            std::vector< std::uint32_t > const& ) override
    {
        return aaa::bdd::k_invalid_mesh + 1;     //	any non-zero
    }
    void destroy_mesh( aaa::bdd::MeshHandle ) override {}

    aaa::bdd::TextureHandle create_texture(
            int, int, int,
            std::vector< std::uint8_t > const& ) override
    {
        return aaa::bdd::k_invalid_texture + 1;
    }
    void destroy_texture( aaa::bdd::TextureHandle ) override {}

    void draw_mesh( aaa::bdd::MeshHandle, float const*, float const* ) override
    {
        ++draw_mesh_calls;
    }
    void draw_curve3d( aaa::bdd::CurveData const& ) override
    {
        ++draw_curve3d_calls;
    }
    void draw_cell( aaa::bdd::CellDraw const& ) override
    {
        ++draw_cell_calls;
    }
    std::string backend_name() const override { return std::string( "noop" ); }

    int draw_mesh_calls    = 0;
    int draw_curve3d_calls = 0;
    int draw_cell_calls    = 0;
};

}   //	anonymous

TEST( BddCellDrawMac, DistribLabelsMatchVendorOrder )
{
    auto const labels = aaa::bdd::distrib_labels();
    ASSERT_EQ( labels.size(), 2u );
    //	Order MUST match the vendor's `str_distrib[]` table -- these
    //	strings are part of the .deproj contract per CODE_STYLE.md
    //	enum-by-label persistence rule.
    EXPECT_EQ( labels[ 0 ], std::string( "ON CIRCLE" ) );
    EXPECT_EQ( labels[ 1 ], std::string( "ON PLANE" ) );
    //	Indices correspond to enum values.
    EXPECT_EQ( static_cast< int >( aaa::bdd::Distrib::Circle ), 0 );
    EXPECT_EQ( static_cast< int >( aaa::bdd::Distrib::Square ), 1 );
}

TEST( BddCellDrawMac, BranchMakePosCircleAtAngleBeginPlacesXAxis )
{
    //	With angle_begin = 0, index = 0, nb_u = 4 :
    //	  a = 0 + (1 - 0) * 0 / 4 = 0 turn
    //	  pos[0] += radius * cos(0) = +radius
    //	  pos[1] += radius * sin(0) = 0
    double pos[ 3 ] = { 0.0, 0.0, 0.0 };
    aaa::bdd::branch_make_pos(
            pos, /*radius=*/2.0, /*index=*/0, /*nb_u=*/4, /*nb_v=*/1,
            aaa::bdd::Distrib::Circle, /*angle_begin=*/0.0, /*angle_end=*/1.0 );
    EXPECT_NEAR( pos[ 0 ], 2.0, k_eps );
    EXPECT_NEAR( pos[ 1 ], 0.0, k_eps );

    //	Same setup, index = 1 of 4 around full turn :
    //	  a = 0.25 turn => cos = 0, sin = 1
    double pos2[ 3 ] = { 0.0, 0.0, 0.0 };
    aaa::bdd::branch_make_pos(
            pos2, 2.0, 1, 4, 1,
            aaa::bdd::Distrib::Circle, 0.0, 1.0 );
    EXPECT_NEAR( pos2[ 0 ], 0.0, k_eps );
    EXPECT_NEAR( pos2[ 1 ], 2.0, k_eps );
}

TEST( BddCellDrawMac, BranchMakePosSquareMatchesVendorFormula )
{
    //	Square mode, index = 0 in 2x2 grid, radius = 1 :
    //	  u = 0, v = 0
    //	  pos[0] += 1 * ((2*0+1)/2 - 1) = -0.5
    //	  pos[1] -= 1 * ((2*0+1)/2 - 1) = +0.5
    double pos[ 3 ] = { 0.0, 0.0, 0.0 };
    aaa::bdd::branch_make_pos(
            pos, /*radius=*/1.0, /*index=*/0, /*nb_u=*/2, /*nb_v=*/2,
            aaa::bdd::Distrib::Square, 0.0, 0.0 );
    EXPECT_NEAR( pos[ 0 ], -0.5, k_eps );
    EXPECT_NEAR( pos[ 1 ], +0.5, k_eps );

    //	Index = 3 (u=1, v=1) :
    //	  pos[0] += 1 * ((2*1+1)/2 - 1) = +0.5
    //	  pos[1] -= 1 * ((2*1+1)/2 - 1) = -0.5
    double pos2[ 3 ] = { 0.0, 0.0, 0.0 };
    aaa::bdd::branch_make_pos(
            pos2, 1.0, 3, 2, 2,
            aaa::bdd::Distrib::Square, 0.0, 0.0 );
    EXPECT_NEAR( pos2[ 0 ], +0.5, k_eps );
    EXPECT_NEAR( pos2[ 1 ], -0.5, k_eps );
}

TEST( BddCellDrawMac, RadiusAndGridForCount )
{
    //	radius_for_count : 1-child special case returns 0.
    EXPECT_EQ( aaa::bdd::radius_for_count( 1, 4.0, 0.5 ), 0.0 );
    //	N>1 returns 0.5 * size * level_radius.
    EXPECT_NEAR( aaa::bdd::radius_for_count( 5, 4.0, 0.5 ), 1.0, k_eps );
    //	0/negative : 0.
    EXPECT_EQ( aaa::bdd::radius_for_count( 0, 4.0, 0.5 ), 0.0 );

    //	Circle mode : nb_u = count, nb_v = 1.
    auto g_circle = aaa::bdd::grid_for_count( 7, aaa::bdd::Distrib::Circle );
    EXPECT_EQ( g_circle.nb_u, 7 );
    EXPECT_EQ( g_circle.nb_v, 1 );

    //	Square mode : ceil(sqrt(N)) x ceil(N/u).
    //	N = 5 -> u = 3, v = 2.
    auto g_sq = aaa::bdd::grid_for_count( 5, aaa::bdd::Distrib::Square );
    EXPECT_EQ( g_sq.nb_u, 3 );
    EXPECT_EQ( g_sq.nb_v, 2 );
    //	N = 9 -> u = 3, v = 3.
    auto g_sq9 = aaa::bdd::grid_for_count( 9, aaa::bdd::Distrib::Square );
    EXPECT_EQ( g_sq9.nb_u, 3 );
    EXPECT_EQ( g_sq9.nb_v, 3 );
}

TEST( BddCellDrawMac, AlphaForLevelIsMonotonicAndClamped )
{
    //	level == level_max -> alpha_begin.
    EXPECT_NEAR(
            aaa::bdd::alpha_for_level( 10, 10, 0.2, 0.8 ),
            0.2, k_eps );
    //	level == 0 -> alpha_end.
    EXPECT_NEAR(
            aaa::bdd::alpha_for_level( 0, 10, 0.2, 0.8 ),
            0.8, k_eps );
    //	level == 5 -> midpoint (0.2 + 0.6 * 0.5 = 0.5).
    EXPECT_NEAR(
            aaa::bdd::alpha_for_level( 5, 10, 0.2, 0.8 ),
            0.5, k_eps );
    //	level_max <= 0 -> alpha_begin (degenerate fallback, no NaN).
    EXPECT_NEAR(
            aaa::bdd::alpha_for_level( 4, 0, 0.3, 0.9 ),
            0.3, k_eps );
    //	level > level_max -> clamp to alpha_begin (t clamped to 0).
    EXPECT_NEAR(
            aaa::bdd::alpha_for_level( 20, 10, 0.2, 0.8 ),
            0.2, k_eps );
    //	level < 0 -> clamp to alpha_end (t clamped to 1).
    EXPECT_NEAR(
            aaa::bdd::alpha_for_level( -5, 10, 0.2, 0.8 ),
            0.8, k_eps );
}

TEST( BddCellDrawMac, SizePerChildCircleMatchesPiOverN )
{
    //	Circle mode : pi * parent_size / nb_to_draw, then * level_radius.
    constexpr double pi = 3.14159265358979323846;
    double const expected = pi * 4.0 / 8.0 * 0.5;       //	= 0.78539...
    EXPECT_NEAR(
            aaa::bdd::size_per_child(
                    /*nb_to_draw=*/8, /*parent_size=*/4.0,
                    /*nb_u=*/8, /*nb_v=*/1,
                    aaa::bdd::Distrib::Circle, /*level_radius=*/0.5 ),
            expected, 1e-6 );

    //	Square mode : parent_size / max(nb_u, nb_v) * level_radius.
    //	5 children -> 3x2 grid -> max=3 -> 4/3 * 0.5 = 0.666...
    EXPECT_NEAR(
            aaa::bdd::size_per_child(
                    5, 4.0, 3, 2,
                    aaa::bdd::Distrib::Square, 0.5 ),
            4.0 / 3.0 * 0.5, k_eps );
}

TEST( Renderer, NoOpSubclassCompilesAndDispatches )
{
    //	If the abstract interface is well-formed, NoopRenderer compiles
    //	and a base-class pointer dispatches virtuals correctly.
    NoopRenderer noop;
    aaa::bdd::Renderer* r = &noop;

    EXPECT_EQ( r->backend_name(), std::string( "noop" ) );

    aaa::bdd::CellDraw cell{};
    cell.pos[ 0 ] = 0.0;
    cell.pos[ 1 ] = 0.0;
    cell.pos[ 2 ] = 0.0;
    cell.size     = 1.0;
    cell.color[ 0 ] = 1.0f;
    cell.color[ 1 ] = 1.0f;
    cell.color[ 2 ] = 1.0f;
    cell.color[ 3 ] = 1.0f;
    cell.level    = 0;
    cell.shape    = 3;     //	sphere
    r->draw_cell( cell );
    EXPECT_EQ( noop.draw_cell_calls, 1 );

    aaa::bdd::CurveData curve;
    curve.pts = { 0., 0., 0., 1., 1., 1. };
    curve.pt_count   = 2;
    curve.color[ 0 ] = 1.0f;
    curve.line_width = 1.5f;
    r->draw_curve3d( curve );
    EXPECT_EQ( noop.draw_curve3d_calls, 1 );

    float model[ 16 ] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    float rgba[ 4 ]   = { 1, 1, 1, 1 };
    r->draw_mesh( aaa::bdd::k_invalid_mesh + 7, model, rgba );
    EXPECT_EQ( noop.draw_mesh_calls, 1 );

    //	Lifecycle round-trip : create -> destroy never throws or
    //	asserts. Verifies the resource-handle ABI.
    auto h_mesh = r->create_mesh( {}, {} );
    EXPECT_NE( h_mesh, aaa::bdd::k_invalid_mesh );
    r->destroy_mesh( h_mesh );
    r->destroy_mesh( aaa::bdd::k_invalid_mesh );     //	idempotent
    auto h_tex = r->create_texture( 1, 1, 4, { 0, 0, 0, 0 } );
    EXPECT_NE( h_tex, aaa::bdd::k_invalid_texture );
    r->destroy_texture( h_tex );
}
