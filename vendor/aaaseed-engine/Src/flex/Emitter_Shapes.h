#ifdef AAA_FLEX_EMITTER_SHAPES_H
#error "FLEX/EMITTER_SHAPES_H included more than once."
#endif
#define AAA_FLEX_EMITTER_SHAPES_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif

#if AAA_USE_FLEX()
#	ifndef AAA_AAA_TYPE_H
#		include "aaa_type.h"
#	endif


class c_particle_host;
class c_rigid_host;
//class c_spring_host;
//class c_triangle_host;

struct st_flex_rigid_box_creation_info
{
	FP32	_center[3];
	UINT32	_dim[3];
	FP32	_spacing;
	FP32	_velocity[3];
	FP32	_mass_inv;
	bool	_rigid;
	FP32	_stiffness;
	UINT32	_phase;
	INT32	_tag;
};


struct st_flex_rigid_sphere_creation_info
{
	FP32	_center[3];
	UINT32	_dim;
	FP32	_spacing;
	FP32	_velocity[3];
	FP32	_mass_inv;
	bool	_rigid;
	FP32	_stiffness;
	UINT32	_phase;
	INT32	_tag;
};

struct st_flex_rigid_cylinder_creation_info
{
	FP32	_center[3];
	UINT32	_dim;
	UINT32	_dim_z;
	FP32	_spacing;
	FP32	_velocity[3];
	FP32	_mass_inv;
	bool	_rigid;
	FP32	_stiffness;
	UINT32	_phase;
	INT32	_tag;
};



extern void flex_create_grid(     c_particle_host * particle_host, c_rigid_host * rigid_host, st_flex_rigid_box_creation_info      CONST * pt_info, FP32* mask );
extern void flex_create_sphere(   c_particle_host * particle_host, c_rigid_host * rigid_host, st_flex_rigid_sphere_creation_info   CONST * pt_info );
extern void flex_create_cylinder( c_particle_host * particle_host, c_rigid_host * rigid_host, st_flex_rigid_cylinder_creation_info CONST * pt_info );

#endif //#if AAA_USE_FLEX()
