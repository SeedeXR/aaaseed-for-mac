// tests/unit/aaalua_layer_app_shim/gol/gol_light.h
//
// Tests-private SHADOW for the engine's gol/gol_light.h. Used ONLY by the
// aaaseed_layer_app_compile_tests OBJECT-lib target.
//
// app.cpp pulls gol/gol_light.h via draw/lights.h. The real header has
// 8 GL_LIGHT0..7 macros + inline glLightfv calls + GOL::enable/disable
// calls. For compile-only, leave empty -- draw/lights.h's reach into
// gol_light.h is satisfied by the empty body when nothing in app.cpp's
// call graph instantiates the unused inline functions.

#ifdef AAA_GOL_LIGHT_H
#error "GOL_LIGHT_H included more than once."
#endif
#define AAA_GOL_LIGHT_H 1

#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif

namespace GOL
{
//	Empty : draw/lights.h doesn't reference any specific gol_light.h
//	identifier in app.cpp's call graph (verified by iteration).
}
