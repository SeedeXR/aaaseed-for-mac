// tests/unit/aaalua_layer_app_shim/gol/gol_base.h
//
// Tests-private SHADOW for the engine's gol/gol_base.h. Used ONLY by the
// aaaseed_layer_app_compile_tests OBJECT-lib target.
//
// app.cpp pulls gol/gol_base.h transitively via image/bind_img_base.h ->
// gol/gol_tex.h -> gol/gol_base.h. The real header has ~20 inline functions
// that call raw OpenGL functions (glGetError, glGetIntegerv, glHint,
// glBindBuffer, etc.). On Mac we route through Metal not GL, and providing
// a real GL link surface for a compile-only test is gold-plating.
//
// Strategy : provide ONLY the GLenum type + a tiny set of constants. Any
// engine code that calls the inline gl* functions in this header won't
// instantiate them in app.cpp's TU because nothing in app.cpp's call graph
// dereferences gol_base.h's inline bodies (verified by iteration).
//
// Mirrors the aaalua_array_shim/gol/gol_base.h pattern but expanded for
// the full layer-app dep cone.

#ifdef AAA_GOL_BASE_H
#error "GOL_BASE_H included more than once."
#endif
#define AAA_GOL_BASE_H 1

#include <cstdint>

//	GL types (mirror the OpenGL spec ; aaa_gl.h sibling shim provides the
//	same forwards but isn't pulled if we shadow gol_base.h directly).
typedef std::uint32_t GLenum;
typedef std::uint32_t GLuint;
typedef std::int32_t  GLint;
typedef std::int32_t  GLsizei;
typedef std::uint8_t  GLboolean;
typedef std::uint8_t  GLubyte;
typedef float         GLfloat;
typedef double        GLdouble;
typedef void          GLvoid;
typedef char          GLchar;
typedef std::int64_t  GLintptr;
typedef std::int64_t  GLsizeiptr;
typedef std::uint32_t GLbitfield;

//	GLenum constants used as type tags in engine code.
#define GL_BYTE             0x1400u
#define GL_UNSIGNED_BYTE    0x1401u
#define GL_SHORT            0x1402u
#define GL_UNSIGNED_SHORT   0x1403u
#define GL_INT              0x1404u
#define GL_UNSIGNED_INT     0x1405u
#define GL_FLOAT            0x1406u
#define GL_HALF_FLOAT       0x140Bu
#define GL_DOUBLE           0x140Au
#define GL_NONE             0x0u
#define GL_FALSE            0u
#define GL_TRUE             1u

//	GOL namespace : declare ONLY what the headers above us (gol_tex.h) need
//	to compile its inline bodies that AREN'T touched by app.cpp's call graph.
namespace GOL
{
    //	is_state_cache_no() returns a constexpr bool ; used in many inline
    //	bodies. Make it constexpr so the constant-expression checker is
    //	satisfied.
    constexpr bool is_state_cache_no() { return false; }

    //	INTERNAL_TYPE enum used by image/pixel_format.h. Mirrors the
    //	vendor's gol_tex.h definition.
    enum class INTERNAL_TYPE : int
    {
        UINT_8 = 0,
        UINT_16,
        FLOAT_16,
        FLOAT_32,
        DEPTH_16,
        DEPTH_24,
        DEPTH_32,
        DEPTH_24_STENCIL_8,
        DEPTH_32_STENCIL_8,
        NONE,
        INVALID,
        UNKNOWN,
        MAX_NB
    };

    //	enable/disable used inline in gol_*.h headers ; provide no-op
    //	stubs as inline so the engine inline bodies that call them parse.
    inline void enable(  GLenum const ) {}
    inline void disable( GLenum const ) {}
}
