#ifdef AAA_FLEX_COLLISION_SHAPES_H
#error "FLEX/COLLISION_SHAPES_H included more than once."
#endif
#define AAA_FLEX_COLLISION_SHAPES_H 1



#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif


#if AAA_USE_FLEX()
#	ifndef AAA_OBJ_H
#		include "infrastructure/obj/obj.h"
#	endif

	class c_flex_buffer_cuda_host;

	struct st_flex_collision_shape
	{
		NvFlexCollisionShapeType	_type;
		NvFlexCollisionGeometry		_geometry;
		FP32						_position[3];
		FP32						_rotation[3];	// in radians
	};



class c_flex_collision_shapes final : public c_obj
{
private:
	UINT32						_collider_max_nb;
	UINT32						_collider_nb;

	c_flex_buffer_cuda_host*	_geometry;
	c_flex_buffer_cuda_host*	_position;
	c_flex_buffer_cuda_host*	_rotation;
	c_flex_buffer_cuda_host*	_flags;

	void release();

public:
	c_flex_collision_shapes();
	~c_flex_collision_shapes();

	void reset( NvFlexLibrary * library, UINT32 CONST collider_max_nb );
	void attach( NvFlexSolver * solver);
	void set_active_collision_shapes( st_flex_collision_shape* const shapes, UINT32 CONST nb );
};

#endif	//#if AAA_USE_FLEX()