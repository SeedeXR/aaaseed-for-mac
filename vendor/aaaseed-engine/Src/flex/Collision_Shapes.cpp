#include "Collision_Shapes.h"
#include "aaa_mem.h"
#include "math/TQuaternion.h"
#include "math/aaa_math.h"
#include "math/v.h"
#include "Buffer_Cuda_Host.h"


#if AAA_USE_FLEX()

c_flex_collision_shapes::c_flex_collision_shapes()
	:_collider_max_nb(0)
	,_collider_nb(0)
	,_geometry(nullptr)
	,_position(nullptr)
	,_rotation(nullptr)
	,_flags(nullptr)
{
}

c_flex_collision_shapes::~c_flex_collision_shapes()
{
	release();
}


void c_flex_collision_shapes::reset( NvFlexLibrary* library, UINT32 CONST collider_max_nb )
{
	if( _collider_max_nb != collider_max_nb )
	{
		release();
		_collider_max_nb = collider_max_nb;

		_geometry = new c_flex_buffer_cuda_host();
		_geometry	->reset( library, collider_max_nb, sizeof(NvFlexCollisionGeometry) );

		_position = new c_flex_buffer_cuda_host();
		_position	->reset( library, collider_max_nb, 4*sizeof(FP32) );

		_rotation = new c_flex_buffer_cuda_host();
		_rotation	->reset( library, collider_max_nb, 4*sizeof(FP32) );

		_flags = new c_flex_buffer_cuda_host();
		_flags		->reset( library, collider_max_nb, sizeof(INT32) );
	}
}

void c_flex_collision_shapes::attach(NvFlexSolver* solver)
{
	NvFlexSetShapes(
		solver, 
		_geometry->get_flex_buffer(),
		_position->get_flex_buffer(),
		_rotation->get_flex_buffer(),
		_position->get_flex_buffer(),	//todo deal with prev position
		_rotation->get_flex_buffer(),	//todo deal with prev rotation
		_flags->get_flex_buffer(),
		_collider_nb);
}

void c_flex_collision_shapes::set_active_collision_shapes( st_flex_collision_shape* const shapes, UINT32 CONST nb )
{
	if( nb > _collider_max_nb )
		return;

	//can be 0
	_collider_nb = nb;

	//only write if > 0

	if( nb > 0 )
	{
		NvFlexCollisionGeometry*	p_geometry	= (NvFlexCollisionGeometry*)_geometry->map();
		FP32*					p_position	= (FP32*)				_position->map();
		FP32*					p_rotation	= (FP32*)				_rotation->map();
		INT32*						p_flags		= (INT32*)					_flags->map();

		
//todo increment pointers
		for( UINT32 i = 0; i < nb; i++ ) 
		{
			st_flex_collision_shape& shap = shapes[i];
			p_geometry[i]		=	shap._geometry;

			cpy_v3( p_position + i*4,	shap._position );
			p_position[i*4+3]	=  1.f;

			Quaternion<float> quat;
			quat.set(	shap._rotation[0], shap._rotation[1], shap._rotation[2] );

 //todo convert to quaternion
			p_rotation[i*4]		=  quat.v.x;
			p_rotation[i*4+1]	=  quat.v.y;
			p_rotation[i*4+2]	=  quat.v.z;
			p_rotation[i*4+3]	=  quat.w;

			p_flags[i]			= NvFlexMakeShapeFlagsWithChannels( shap._type, false, eNvFlexPhaseShapeChannelMask );
		}

		_geometry->unmap();
		_position->unmap();
		_rotation->unmap();
		_flags->unmap();
	}
}


void c_flex_collision_shapes::release()
{
	SAFE_DELETE( _geometry );
	SAFE_DELETE( _position );
	SAFE_DELETE( _rotation );
	SAFE_DELETE( _flags	);
}

#endif //#if AAA_USE_FLEX()