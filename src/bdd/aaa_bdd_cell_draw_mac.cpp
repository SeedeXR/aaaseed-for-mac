// src/bdd/aaa_bdd_cell_draw_mac.cpp
//
// Implementation of the portable math helpers declared in
// aaa_bdd_cell_draw_mac.h. Pure C++ ; no Objective-C, no Metal,
// no engine link.
//
// Citation map to the vendor source (read-only reference) :
//   vendor/aaaseed-engine/Src/obj_ui/bdd/bdd_cell_draw.cpp
//     347-367 : branch_make_pos()          -> aaa::bdd::branch_make_pos
//     339-345 : set_color alpha lerp       -> aaa::bdd::alpha_for_level
//     392-425 : radius / size_new          -> aaa::bdd::radius_for_count
//                                            + aaa::bdd::size_per_child
//                                            + aaa::bdd::grid_for_count
//
// The vendor calls use trigonometry in "unit-turn" form (`COS_TURN(a)`
// == cos(2*pi*a)). We expand that inline with `std::cos(2*pi*a)` so
// the port has no engine-macro pull-in.

#include "aaa_bdd_cell_draw_mac.h"

#include <cmath>
#include <algorithm>

namespace aaa
{
namespace bdd
{

namespace
{
//	2*pi to double precision. Local to this TU so we don't drag in
//	`<numbers>` (C++20 std::numbers::pi is fine on AppleClang but kept
//	off the header to minimise surface).
constexpr double k_two_pi = 6.28318530717958647692;
}   //	anonymous

std::vector< std::string > distrib_labels()
{
    //	Order MUST match enum Distrib values (Circle = 0, Square = 1)
    //	-- enum params persist by string label per CODE_STYLE.md, so
    //	these labels are effectively part of the .deproj contract.
    return { std::string( "ON CIRCLE" ), std::string( "ON PLANE" ) };
}

void branch_make_pos(
        double*  pos,
        double   radius,
        int      index,
        int      nb_u,
        int      nb_v,
        Distrib  mode,
        double   angle_begin,
        double   angle_end )
{
    if( !pos )
        return;
    if( nb_u <= 0 )
        nb_u = 1;
    if( nb_v <= 0 )
        nb_v = 1;

    switch( mode )
    {
    case Distrib::Circle:
        {
            //	Vendor 351-356 : a = begin + (end-begin)*index/nb_u
            double const a = angle_begin
                + ( angle_end - angle_begin ) * double( index ) / double( nb_u );
            pos[0] += radius * std::cos( k_two_pi * a );
            pos[1] += radius * std::sin( k_two_pi * a );
        }
        break;
    case Distrib::Square:
        {
            //	Vendor 358-364 : u = index mod nb_u ; v = index / nb_u ;
            //	pos.x += s*(2u+1)/nb_u - 1 ; pos.y -= s*(2v+1)/nb_v - 1
            int const u_idx = index % nb_u;
            int const v_idx = index / nb_u;
            pos[0] += radius * ( double( 2*u_idx + 1 ) / double( nb_u ) - 1.0 );
            pos[1] -= radius * ( double( 2*v_idx + 1 ) / double( nb_v ) - 1.0 );
        }
        break;
    }
}

double radius_for_count( int nb_to_draw, double size, double level_radius )
{
    if( nb_to_draw <= 1 )
        return 0.0;
    return 0.5 * size * level_radius;
}

GridDims grid_for_count( int nb_to_draw, Distrib mode )
{
    GridDims out{ 1, 1 };
    if( nb_to_draw <= 0 )
        return out;
    if( mode == Distrib::Circle )
    {
        out.nb_u = nb_to_draw;
        out.nb_v = 1;
        return out;
    }
    //	Square : ceil(sqrt(N)) x ceil(N / nb_u). Vendor 414-420 uses
    //	`I_CEIL` (engine helper) ; we use std::ceil + cast.
    double const sq = std::sqrt( double( nb_to_draw ) );
    int const u = std::max( 1, int( std::ceil( sq ) ) );
    int const v = std::max( 1, int( std::ceil( double( nb_to_draw ) / double( u ) ) ) );
    out.nb_u = u;
    out.nb_v = v;
    return out;
}

double size_per_child(
        int    nb_to_draw,
        double parent_size,
        int    nb_u,
        int    nb_v,
        Distrib mode,
        double level_radius )
{
    if( nb_to_draw <= 0 )
        return 0.0;
    double size_new = 0.0;
    if( mode == Distrib::Circle )
    {
        //	Vendor 406-410 : size_new = pi * size / nb_to_draw
        double const pi = k_two_pi * 0.5;
        size_new = pi * parent_size / double( nb_to_draw );
    }
    else
    {
        //	Vendor 413-421 : size_new = size / max(nb_u, nb_v)
        int const denom = std::max( 1, std::max( nb_u, nb_v ) );
        size_new = parent_size / double( denom );
    }
    return size_new * level_radius;
}

double alpha_for_level(
        int    level,
        int    level_max,
        double alpha_begin,
        double alpha_end )
{
    if( level_max <= 0 )
        return alpha_begin;
    //	Vendor 343 : interpolate( alpha_begin, alpha_end,
    //	    (level_max - level) / level_max )
    //	-> level == level_max -> t = 0 -> alpha_begin
    //	-> level == 0         -> t = 1 -> alpha_end
    double const t = double( level_max - level ) / double( level_max );
    //	clamp [0, 1] so out-of-range `level` doesn't extrapolate.
    double const tc = ( t < 0.0 ) ? 0.0 : ( ( t > 1.0 ) ? 1.0 : t );
    return alpha_begin + ( alpha_end - alpha_begin ) * tc;
}

}   //	namespace bdd
}   //	namespace aaa
