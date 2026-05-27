// tests/unit/aaalua_array_shim/gol/gol_base.h
//
// Tests-private SHIM for the engine's gol/gol_base.h, used ONLY by the
// aaalua_array.cpp Stage 4 test target (continuation 51). Added to the
// target's include path BEFORE the engine include path so this file
// wins for the wrapper-test build only ; the engine's real
// gol/gol_base.h still resolves everywhere else.
//
// Why this shim is enough : aaalua_array.cpp never CALLS an OpenGL
// function. It uses 8 GLenum values purely as opaque integer type tags
// stashed in a userdata struct field and compared at runtime to
// validate "array of type X" claims. No glGen*, glBind*, glDraw*,
// glDelete* calls in the file -- verified by `grep -n "^[^/]*gl[A-Z]"`
// before this shim was written.
//
// Pattern this establishes : for further wrapper files that pull GL
// headers but don't call GL functions, shim the include path instead
// of substituting at source -- zero vendor/ touch.
//
// The numeric values match the official OpenGL constants so any
// engine code that compares fields populated here against a real GL-
// returned enum (on Windows) gets the same integer.

#ifdef AAA_GOL_BASE_H
#error "GOL_BASE_H included more than once."
#endif
#define AAA_GOL_BASE_H 1

#include <cstdint>

//	GLenum is an unsigned 32-bit integer in the OpenGL spec.
typedef std::uint32_t GLenum;

//	Numeric values match the OpenGL spec (gl.h / glcorearb.h).
//	These are the 8 the aaalua_array userdata layer uses as type tags.
#define GL_BYTE             0x1400u
#define GL_UNSIGNED_BYTE    0x1401u
#define GL_SHORT            0x1402u
#define GL_UNSIGNED_SHORT   0x1403u
#define GL_INT              0x1404u
#define GL_UNSIGNED_INT     0x1405u
#define GL_FLOAT            0x1406u
#define GL_DOUBLE           0x140Au
