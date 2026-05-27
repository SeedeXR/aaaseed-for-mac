
#include "gl/ssbo.h"
#include "spy.h"
#include "err.h"

namespace gl
{

namespace {
	UINT32	nb_instance = 0;
	UINT32	nb_instance_created = 0;
	void inc_count()
	{
		++nb_instance;
		++nb_instance_created;
	}
}

UINT32*	ssbo::get_nb_pt()			{	return &nb_instance;		}
UINT32*	ssbo::get_nb_created_pt()	{	return &nb_instance_created;	}

C_PCHAR_C	ssbo::get_type_name() { return "ssbo"; }

ssbo * ssbo::make( UINT32 CONST size, bool CONST b_dynamic, char CONST * CONST data, C_PCHAR_C name, bool CONST b_immediate )
{
	ssbo * res = new ssbo();
	res->init( size, b_dynamic, data );
	if( name )
		res->set_name( name );
	res->request_init_ogl( b_immediate );
	return res;
}

//ssbo * ssbo::make(
//	ssbo_props CONST &	props, 
//	bool CONST			b_immediate)
//{
//	return make( props._size, props._dynamic, props._data, b_immediate );
//}

void ssbo::reclaim( ssbo * CONST pt, bool CONST b_immediate )
{
	pt->request_release( b_immediate );
//	pt->request_release( false );	  // b_immediate is silently discarded
}


ssbo::ssbo()
	:_data(	nullptr	)
	,_size( 0 )
	,_b_dynamic(false)
{
	inc_count();
}

ssbo::~ssbo()
{
	IF_FREE_ALIGNED_AND_NULL( _data );

	if( nb_instance > 0 )
		--nb_instance;
	else
		debug_break( "%s() destructor called with ssbo count already at zero", __FUNCTION__ );
}

void ssbo::init( UINT32 CONST size, bool CONST b_dynamic, char CONST * CONST data )
{
	_b_dynamic = b_dynamic;
	_data = (char*)MALLOC_ALIGNED_SIGNATURE( size, 32, __FUNCTION__ );
	if( _data )
	{
		_size = size;
		if( data )
			MEMCPY( _data, data, size, __FUNCTION__ );
		else
			MEMCLEAR( _data, size );
	}
	else
	{
		_size = 0;
		debug_break( "%s() can't alloc memory of size %d ", __FUNCTION__, size );
	}
}

//void ssbo::init( ssbo_props CONST & p_props )
//{
//	_dynamic = p_props._dynamic;
//	_size = p_props._size;
//	_data = (char*)MALLOC_ALIGNED_SIGNATURE( _size, 32, __FUNCTION__ );
//	if( p_props._data )
//		MEMCPY( _data, p_props._data, _size, __FUNCTION__ );
//	else
//		MEMCLEAR( _data, _size );
//}


void ssbo::init_ogl()
{
	if( _data )
	{
		gen_id();

		SPY_PUSH_RANGE2( "ssbo::define_buffer", spy::GOL, _o_name.get() )
			GLenum CONST usage = _b_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
			GOL::bind_buffer( GL_SHADER_STORAGE_BUFFER, _gol_id );
				GOL::set_buffer_data( GL_SHADER_STORAGE_BUFFER, _size, _data, usage );
			GOL::unbind_buffer( GL_SHADER_STORAGE_BUFFER );
		SPY_POP_RANGE2()
	}
	else
		debug_break( "%s() can't generate3 and init buffer for null data", __FUNCTION__ );
}

//void ssbo::update_ogl()
//{
//	void * dst = map_write();
//	MEMCPY( dst, _data, _size, __FUNCTION__ );
//	unmap();
//}

void ssbo::update_ogl()
{
	if( _data )
	{
		if( _b_dynamic )
		{
			SPY_PUSH_RANGE2( "ssbo::update_ogl dynamic", spy::GOL, _o_name.get() )
				void * dst = map_write();
					MEMCPY( dst, _data, _size, __FUNCTION__ );
				unmap();
			SPY_POP_RANGE2()
		}
		else
		{
			SPY_PUSH_RANGE2( "ssbo::update_ogl static", spy::GOL, _o_name.get() )
				GOL::bind_buffer( GL_SHADER_STORAGE_BUFFER, _gol_id );
					GOL::set_buffer_data( GL_SHADER_STORAGE_BUFFER, _size, _data, GL_STATIC_DRAW );
				GOL::unbind_buffer( GL_SHADER_STORAGE_BUFFER );
			SPY_POP_RANGE2()
		}
	}
	else
		debug_break( "%s() can't transfer null data to buffer", __FUNCTION__ );
}

void ssbo::release_ogl()
{
	del_id();
}


void * ssbo::map_write() CONST
{
	if( _gol_id )
	{
		if( GOL::b_named_buffer_use )
		{
			SPY_PUSH_RANGE2( "ssbo::map_unmap named", spy::GOL, _o_name.get() )
			return GOL::map_named_buffer( _gol_id, GL_WRITE_ONLY );
		}
		else
		{
			SPY_PUSH_RANGE2( "ssbo::map_unmap", spy::GOL, _o_name.get() )
			GOL::bind_buffer( GL_SHADER_STORAGE_BUFFER, _gol_id );
			return GOL::map_buffer( GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY );
		}
	}
	else
	{
		debug_break( "%s() can't map non existing buffer", __FUNCTION__ );
		return nullptr;
	}
}

void ssbo::unmap() CONST
{
	if( _gol_id )
	{
		if( GOL::b_named_buffer_use )
			GOL::unmap_named_buffer( _gol_id );
		else
		{
			GOL::unmap_buffer( GL_SHADER_STORAGE_BUFFER );
			GOL::unbind_buffer( GL_SHADER_STORAGE_BUFFER );
		}
		SPY_POP_RANGE2()
	}
	else
		debug_break( "%s() can't unmap non existing buffer", __FUNCTION__ );
}

void ssbo::write( void CONST * data, UINT32 CONST size, UINT32 CONST dst_offset, bool CONST b_immediate )
{
	if( _data )
	{	
		MEMCPY( _data + dst_offset, data, size, __FUNCTION__ );
		request_update_ogl(b_immediate);
	}
	else
		debug_break( "%s() can't write to non existing data", __FUNCTION__ );
}

void ssbo::read( void* output_data, UINT32 CONST size, UINT32 CONST dst_offset )
{
	if( _gol_id )
	{
		SPY_PUSH_RANGE2( "ssbo::read", spy::GOL, _o_name.get() );

			GOL::bind_buffer( GL_SHADER_STORAGE_BUFFER, _gol_id );

				glGetBufferSubData( GL_SHADER_STORAGE_BUFFER, dst_offset, size, output_data );
	
			GOL::unbind_buffer( GL_SHADER_STORAGE_BUFFER );

		SPY_POP_RANGE2()
	}
	else
		debug_break( "%s() can't read non existing buffer", __FUNCTION__ );
}

}//namespace gl