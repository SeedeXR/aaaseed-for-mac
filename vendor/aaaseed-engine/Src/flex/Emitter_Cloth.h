#ifdef AAA_FLEX_EMITTER_CLOTH_H
#error "FLEX/EMITTER_CLOTH_H included more than once."
#endif
#define AAA_FLEX_EMITTER_CLOTH_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

enum class e_flex_cloth_pin_mode : INT32
{
	NONE					= 0,
	ALL_BORDERS 			= 1,
	ALL_CORNERS 			= 2,
	TOP_BORDER				= 3,
	CLOTH_PIN_MODE_MAX_NB	= 4
};

enum class e_flex_mass_mode : INT32
{
	INIT_MASS_PARTICLE	= 0,
	INIT_MASS_DENSITY	= 1,
	INIT_MASS_MAX_NB	= 2,
};

struct st_flex_spring_init
{
	bool	b_use;
	FP32	stiffness;
	bool	b_length_factor;
	FP32	length_factor;
};



#if AAA_USE_FLEX()
class c_particle_host;
class c_spring_host;
class c_triangle_host;




struct st_flex_cloth_grid_creation_info
{
	FP32					center[3];
	FP32					size[2];
	UINT32					nb_u;
	UINT32					nb_v;
	UINT32					grid_uv_modulo;
	UINT32					phase;
	FP32					velocity[3];
	e_flex_mass_mode		s_mass_mode;
	FP32					mass_inv;
	FP32					density;
	e_flex_cloth_pin_mode	s_pin_mode;
	UINT32					pin_border_size;
	st_flex_spring_init		stretch;
	st_flex_spring_init		bend;
	st_flex_spring_init		shear;
	st_flex_spring_init		tether;
	bool					b_triangle_use;
};



void flex_create_cloth_grid( INT32 CONST i_axe, c_particle_host * particle_host, c_spring_host * spring_host, c_triangle_host * triangles_host, st_flex_cloth_grid_creation_info CONST * CONST info );

#endif //#if AAA_USE_FLEX()
