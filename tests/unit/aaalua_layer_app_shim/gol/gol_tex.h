// tests/unit/aaalua_layer_app_shim/gol/gol_tex.h
//
// Tests-private SHADOW for the engine's gol/gol_tex.h. Used ONLY by the
// aaaseed_layer_app_compile_tests OBJECT-lib target.
//
// app.cpp pulls gol/gol_tex.h transitively via image/bind_img_base.h. The
// real header has ~70 inline functions calling raw OpenGL functions
// (glGenTextures, glTexParameteri, etc.) plus a c_tex_unit class. The Mac
// port routes through GOL::MetalBackend not GL ; providing real GL link
// surface for a compile-only test is gold-plating.
//
// Strategy : declare only c_tex_unit (used by bind_img_base.h as
// `GOL::get_tex_unit_cur()->set_bind_2d(-2)`) + the few helper symbols
// app.cpp's transitive cone touches. Omit the ~70 GL-calling inline bodies.

#ifdef AAA_GOL_TEX_H
#error "GOL_TEX_H included more than once."
#endif
#define AAA_GOL_TEX_H 1

#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"   // resolves to our shim (same dir)
#endif

namespace GOL
{

//	c_tex_unit minimal declaration. bind_img_base.h's inline body calls
//	get_tex_unit_cur()->set_bind_2d(-2). We declare just that surface.
class	c_tex_unit
{
public:
	c_tex_unit();
	void	set_bind_2d( INT32 const bind );
	INT32	get_bind_2d() const;
};

extern	c_tex_unit*	tex_unit_cur;

inline c_tex_unit*	get_tex_unit_cur()	{ return tex_unit_cur; }

}	//namespace GOL
