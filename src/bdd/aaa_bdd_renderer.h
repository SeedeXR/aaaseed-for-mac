// src/bdd/aaa_bdd_renderer.h
//
// c145 (Task #30 honest closure) : abstract C++ interface for a future
// v2 Metal-side BDD renderer. The vendor BDD subsystem
// (`vendor/aaaseed-engine/Src/obj_ui/bdd/`) is 80+ files / ~15K LOC of
// OpenGL immediate-mode + FTGL text + custom mesh + curve3d code. A
// faithful Metal port is multi-week work (est. 3-4 weeks) and is OUT
// OF SCOPE for v1.
//
// What this header DOES :
//   - Declares the contract a Mac-native BDD renderer must satisfy
//     when v2 lands : a small handful of opaque-handle entry points
//     (`draw_mesh`, `draw_curve3d`, `draw_cell`, ...) parameterised by
//     plain data structs (no GL types, no GOL types, no engine types).
//   - Lets us unit-test the contract today (a no-op subclass compiled
//     against this header proves the interface is well-formed).
//   - Documents the v1 / v2 boundary explicitly so future agents know
//     what HAS landed (aaa_bdd_cell_draw_mac math helpers) vs what
//     has NOT (the renderer itself).
//
// What this header DOES NOT :
//   - Provide any implementation. There is intentionally no .cpp /
//     .mm for this header. The v2 cycle adds `BddRendererMetal` as a
//     concrete subclass in `src/bdd/metal/`.
//   - Expose Metal types. `void*` handles are used wherever a
//     Metal-side resource is referenced ; the v2 concrete impl is
//     where `id<MTLTexture>` / `id<MTLBuffer>` etc. surface (kept out
//     of the public header per c134-A `void*`-ABI doctrine).
//
// Hermetic Mac sub-lib doctrine :
//   - Pure C++. No Metal types in the public surface.
//   - std::string / std::vector only. No `o_str`, no `aaa_mem.h`.
//   - No vendor edits.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace aaa
{
namespace bdd
{

//	Opaque handle for renderer-owned resources. The renderer
//	implementation is free to map this to any internal lookup (vector
//	index, hash key, Metal id pointer). `0` is reserved as
//	"unallocated / invalid".
using MeshHandle    = std::uint32_t;
using TextureHandle = std::uint32_t;
using CurveHandle   = std::uint32_t;

constexpr MeshHandle    k_invalid_mesh    = 0;
constexpr TextureHandle k_invalid_texture = 0;
constexpr CurveHandle   k_invalid_curve   = 0;

//	A single BDD cell : position + size + color + recursion level.
//	The Mac-side renderer projects this to a sphere / hexagon / circle
//	per the vendor's `cell_draw_obj_default` shape selector.
struct CellDraw
{
    double  pos[3];        //	world-space center of the cell
    double  size;          //	cell radius / scale
    float   color[4];      //	rgba in [0..1]
    int     level;         //	recursion depth (0 = root)
    int     shape;         //	0=hexagon, 1=circle, 2=disk, 3=sphere
};

//	A curve3d path : N control points in row-major flat array.
//	`pts` has size 3*pt_count ; `pt_count` >= 2 for a meaningful curve.
struct CurveData
{
    std::vector< double > pts;     //	x0,y0,z0, x1,y1,z1, ...
    int                   pt_count;
    float                 color[4];
    float                 line_width;
};

//	Abstract renderer contract. v1 has no concrete impl ; v2 lands one
//	(planned : `BddRendererMetal` in `src/bdd/metal/`). Tests in v1
//	subclass this with a no-op impl to verify the interface compiles.
//
//	Lifetime : the host owns the renderer ; the renderer owns whatever
//	MTLBuffer / MTLTexture resources it allocates from MeshHandle /
//	TextureHandle. Destruction releases all owned resources.
class Renderer
{
public:
    virtual ~Renderer() = default;

    //	Resource lifecycle ---------------------------------------------

    //	Upload a static mesh : `vertices` is flat xyz, `indices` is
    //	triangle index buffer. Returns handle ; k_invalid_mesh on
    //	allocation failure.
    virtual MeshHandle create_mesh(
            std::vector< float > const& vertices,
            std::vector< std::uint32_t > const& indices ) = 0;

    //	Free a previously-created mesh. Idempotent : k_invalid_mesh
    //	or unknown handle is a no-op.
    virtual void destroy_mesh( MeshHandle handle ) = 0;

    //	Upload an R8 / RGBA8 texture (format inferred from
    //	`bytes_per_pixel` ; 1 = R8, 4 = RGBA8). Returns handle ;
    //	k_invalid_texture on failure.
    virtual TextureHandle create_texture(
            int width, int height, int bytes_per_pixel,
            std::vector< std::uint8_t > const& pixels ) = 0;

    //	Free a previously-created texture. Idempotent.
    virtual void destroy_texture( TextureHandle handle ) = 0;

    //	Per-frame draw ops ---------------------------------------------

    //	Draw a previously-created mesh with the given model matrix
    //	(row-major float[16]) and rgba color.
    virtual void draw_mesh(
            MeshHandle handle,
            float const* model_4x4,
            float const* rgba ) = 0;

    //	Draw a 3D curve as a line strip with the given color +
    //	line width.
    virtual void draw_curve3d( CurveData const& curve ) = 0;

    //	Draw a single BDD cell at its world-space position with the
    //	given shape + color + recursion level (used for level-based
    //	alpha fall-off via `aaa::bdd::alpha_for_level`).
    virtual void draw_cell( CellDraw const& cell ) = 0;

    //	Diagnostic -----------------------------------------------------

    //	Human-readable name of the concrete impl ("metal", "noop", ...).
    //	Used by tests + the HUD to identify which renderer is live.
    virtual std::string backend_name() const = 0;
};

}   //	namespace bdd
}   //	namespace aaa
