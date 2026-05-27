#include "Emitter_Image.h"
#include "gol/gol_shader.h"
#include "gol/gol.h"
#include "spy.h"

namespace{
	CONSTEXPR INT32 EMIT_GROUP_SIZE			= 32;
	CONSTEXPR INT32 FILTER_GROUP_SIZE		= 16;
	CONSTEXPR INT32 FILTER_EMIT_UBO_SLOT	= 1;
}

c_emitter_img::c_emitter_img()
	:_emit						{}
	,_buffer_position			(nullptr)
	,_buffer_velocity			(nullptr)
	,_buffer_accumulation		(nullptr)
	,_ubo_emit					(nullptr)
	,_b_valid					(false)
{
}

void c_emitter_img::release()
{
	gl::ssbo::release_and_null( _buffer_position );
	gl::ssbo::release_and_null( _buffer_velocity );
}

c_emitter_img::~c_emitter_img()
{
	release();
	gl::ssbo::release_and_null( _buffer_accumulation );
	gl::ubo::release_and_null(	_ubo_emit	);
}


void c_emitter_img::set_size( UINT32 CONST size_x, UINT32 CONST size_y )
{
//todo don't need realloc if data size don't change
//infact would be problematic if size change all the time
	if( !_buffer_accumulation )
		_buffer_accumulation = gl::ssbo::make( sizeof(INT32), false, nullptr );
	if( !_ubo_emit )
		_ubo_emit = gl::ubo::make( sizeof(GOL::st_emit), true, (char*)&_emit );
 

	if( _emit.filter._image_sx != size_x || _emit.filter._image_sy != size_y )
	{
		release();

		INT32 nb = size_x * size_y;
		_buffer_position = gl::ssbo::make( nb * sizeof(FP32) * 4, false, nullptr );
		_buffer_velocity = gl::ssbo::make( nb * sizeof(FP32) * 4, false, nullptr );

		_emit.filter._image_sx	= size_x;
		_emit.filter._image_sy	= size_y;

		_b_valid = _buffer_position && _buffer_velocity && _buffer_accumulation && _ubo_emit;
	}
}

UINT32 c_emitter_img::read_counter()
{
	if( _b_valid )
	{
		UINT32 nb = 0;
		_buffer_accumulation->read( &nb, 4, 0 );
		return nb;
	}
	else
		return 0;
}

void c_emitter_img::filter()
{
	if( _b_valid )
	{
		SPY_PUSH_RANGE( "emitter_image::filter()", spy::GOL )

			//clear accumulator
			INT32 accum_reset = 0;
			_buffer_accumulation->write( &accum_reset, sizeof(INT32), 0, true );

			//dispatch 2d (full image)
			_buffer_accumulation->bind(0);
			_buffer_position->bind(1);
			_buffer_velocity->bind(2);

			_ubo_emit->bind_and_write( FILTER_EMIT_UBO_SLOT, &_emit );

				GOL::dispatch_compute_2d( _emit.filter._image_sx, FILTER_GROUP_SIZE, _emit.filter._image_sy, FILTER_GROUP_SIZE);
				GOL::barrier_memory( GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT );

			_buffer_velocity->unbind(2);
			_buffer_position->unbind(1);
			_buffer_accumulation->unbind(0);

		SPY_POP_RANGE()
	}
}

void c_emitter_img::emit( UINT32 CONST max_emit_nb, UINT32 CONST begin_slot_idx )
{
	if( _b_valid )
	{
		SPY_PUSH_RANGE( "emitter_image::emit()", spy::GOL )

			_buffer_accumulation->bind( begin_slot_idx );
			_buffer_position->bind( begin_slot_idx + 1 );
			_buffer_velocity->bind( begin_slot_idx + 2 );

			_ubo_emit->bind( FILTER_EMIT_UBO_SLOT );

				GOL::dispatch_compute_1d( max_emit_nb, EMIT_GROUP_SIZE );
				GOL::barrier_memory( GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT );

			_buffer_velocity->unbind( begin_slot_idx + 2 );
			_buffer_position->unbind( begin_slot_idx + 1 );
			_buffer_accumulation->unbind( begin_slot_idx );

		SPY_POP_RANGE()
	}
}
