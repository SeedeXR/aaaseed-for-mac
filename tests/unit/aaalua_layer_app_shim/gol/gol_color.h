// tests/unit/aaalua_layer_app_shim/gol/gol_color.h
//
// Tests-private SHADOW for the engine's gol/gol_color.h. Used ONLY by the
// aaaseed_layer_app_compile_tests OBJECT-lib target.
//
// app.cpp pulls gol/gol_color.h via draw/color.h. The real header has
// inline glColor3fv / glColor4fv / glTexEnvfv calls + 140+ named color
// enum entries. For compile-only mode, we only need the enum class
// `aaa_color` to be declared (the rest of draw/color.h doesn't reach
// gol_color.h's GL-calling bodies on app.cpp's call graph).

#ifdef AAA_GOL_COLOR_H
#error "GOL_COLOR_H included more than once."
#endif
#define AAA_GOL_COLOR_H 1

#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif

namespace GOL
{

//	Named color enum used as a type tag by draw/color.h. Only need it to
//	be a declared type ; values are irrelevant for compile-only.
enum class aaa_color : int
{
    WHITE = 0,
    BLACK,
    RED,
    GREEN,
    BLUE,
    AAA_COLOR_NB
};

}	//namespace GOL
