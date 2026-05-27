// src/bdd/aaa_bdd_cell_draw_mac.h
//
// c145 (Task #30 honest closure) : Mac-native port of the PURE-C++
// portion of `vendor/aaaseed-engine/Src/obj_ui/bdd/bdd_cell_draw.cpp`.
//
// The vendor file is a 527-LOC drawer for a tree-of-cells UI : it mixes
// (a) cell-positioning math (distrib_circle / distrib_square / alpha
// fall-off / radius from cell count) which is pure C++ + STL ;
// (b) GL state push/pop + immediate-mode draws (`GOL::push_att`,
// `GOL::begin(GL_LINES)`, `glVertex3v`, font upload via FTGL,
// `draw_circle_z` etc) which is the v2 Metal-renderer surface.
//
// What lands here (portable, ~120 LOC) :
//   - `Distrib` enum (CIRCLE / SQUARE) + label table.
//   - `branch_make_pos()` : compute child cell position for a given
//     branch index / count, mirrors vendor `c_bdd_cell_draw::branch_
//     make_pos` (lines 347-367) ; pure trigonometry, std::sin / std::cos.
//   - `radius_for_count()` : the `nb_to_draw == 1 ? 0 : size*lvl_radius*.5`
//     branch radius computation (vendor 392-401), pure scalar math.
//   - `size_per_child()` : the per-distribution-mode child-cell size
//     computation (vendor 404-425), pure scalar math.
//   - `alpha_for_level()` : the `interpolate(alpha_begin, alpha_end,
//     (level_max-level)/level_max)` alpha fall-off (vendor 339-345),
//     pure scalar math.
//
// What does NOT land here (deferred to v2) :
//   - `draw_text()` : touches FTGL, GOL::push_att, GOL::matrix::*.
//   - `draw_obj_internal()` / `draw_branches()` : encode GL_LINES /
//     immediate-mode primitives that the Mac Metal renderer must
//     replace from scratch (Task #30b, est. 3-4 weeks of work --
//     parking until a v2 cycle authorizes it).
//   - `update()` / `draw()` / `param_init_pt()` : require the full
//     `c_obj_ui` / `c_param` infrastructure to be Mac-linkable, which
//     is the c119-A doctrine-limits wall.
//
// Renamed to `aaa::bdd::*` namespace to avoid colliding with the
// vendor's `c_bdd_cell_draw` (which never compiles on Mac today --
// the namespace separation is forward-looking : when the v2 Metal
// renderer lands, it will internally call into `aaa::bdd::*` for the
// portable math without dragging the vendor class).
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure CPU. No Metal touch.
//   - std::string / std::vector / <cmath> only. No `o_str`,
//     no `aaa_mem.h`, no `aaa_str.h`, no `aaa_type.h`.
//   - No vendor edits. No link to `aaaseed_code_utils`.
//   - No Objective-C in the header (kept pure C++).
//
// Companion : `src/bdd/aaa_bdd_renderer.h` exposes the abstract
// renderer interface that a future v2 Metal-side impl will fulfill.

#pragma once

#include <string>
#include <vector>

namespace aaa
{
namespace bdd
{

//	Mirrors vendor `enum CELL_DISTRIB : INT32 { DISTRIB_CIRCLE = 0,
//	DISTRIB_SQUARE, DISTRIB_NB }`. Stable numeric values so a future
//	v2 Metal renderer can pass the enum through a param-bound int
//	without translation.
enum class Distrib : int
{
    Circle = 0,
    Square = 1,
};

//	Stable label table used for enum-by-string serialization (matches
//	the vendor's `str_distrib[]` -- enum params serialize by string
//	per CODE_STYLE.md). Two entries : "ON CIRCLE" / "ON PLANE".
std::vector< std::string > distrib_labels();

//	Position helper. Given a parent cell's accumulated `pos[3]` and a
//	branch `radius`, write the index'th child's xy offset into
//	`pos[0..1]`. `nb_u` / `nb_v` are the grid extents for the SQUARE
//	mode (uniform 1D index for CIRCLE).
//
//	Mirrors vendor `c_bdd_cell_draw::branch_make_pos` (bdd_cell_draw.
//	cpp:347-367). `angle_begin` / `angle_end` are unit-turn fractions
//	(0..1 == 0..2pi) -- consistent with the vendor's `COS_TURN` /
//	`SIN_TURN` (multiply by 2pi internally).
void branch_make_pos(
        double*  pos,
        double   radius,
        int      index,
        int      nb_u,
        int      nb_v,
        Distrib  mode,
        double   angle_begin,
        double   angle_end );

//	Radius of the child-distribution ring for a parent whose direct
//	branch count is `nb_to_draw`. Mirrors vendor 392-401 :
//	  - 1 child  -> 0 (no ring needed, single cell on the axis)
//	  - N children -> 0.5 * size * level_radius
double radius_for_count( int nb_to_draw, double size, double level_radius );

//	Per-child size after subdivision. `nb_u` / `nb_v` are the grid
//	extents (CIRCLE uses N x 1, SQUARE uses ceil(sqrt(N)) x ceil(N/u)).
//	Mirrors vendor 404-425 ; `pi` injected explicitly for testability.
double size_per_child(
        int    nb_to_draw,
        double parent_size,
        int    nb_u,
        int    nb_v,
        Distrib mode,
        double level_radius );

//	`(nb_u, nb_v)` grid extents that satisfy `nb_u * nb_v >= nb_to_draw`
//	for the SQUARE mode. Mirrors vendor 414-420 (ceil(sqrt(N)) +
//	ceil(N/u)). For CIRCLE mode, returns `(nb_to_draw, 1)`.
struct GridDims
{
    int nb_u;
    int nb_v;
};
GridDims grid_for_count( int nb_to_draw, Distrib mode );

//	Alpha multiplier for a given recursion level. `level` counts down
//	from `level_max` to 0 ; alpha is interpolated between `alpha_begin`
//	at the root and `alpha_end` at the deepest level. Mirrors vendor
//	`set_color` body lines 339-345 (just the alpha scalar, not the
//	GOL::color4v emit).
double alpha_for_level(
        int    level,
        int    level_max,
        double alpha_begin,
        double alpha_end );

}   //	namespace bdd
}   //	namespace aaa
