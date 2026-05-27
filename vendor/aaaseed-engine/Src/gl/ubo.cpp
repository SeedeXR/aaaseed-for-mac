
#include "gl/ubo.h"
#include "spy.h"
#include "gol/gol.h"
#include "err.h"

namespace gl {

namespace {
	UINT32	nb_instance = 0;
	UINT32	nb_instance_created = 0;
	void inc_count()
	{
		++nb_instance;
		++nb_instance_created;
	}

	//CONSTEXPR GLbitfield ACCESS = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	CONSTEXPR GLbitfield ACCESS = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
}

UINT32*	ubo::get_nb_pt()			{	return &nb_instance;			}
UINT32*	ubo::get_nb_created_pt()	{	return &nb_instance_created;	}

C_PCHAR_C	ubo::get_type_name() { return "ubo"; }

namespace {
	void * alloc_private( UINT32 const size, char const * const data )
	{
		void * mem_allocated = (char*) MALLOC_ALIGNED_SIGNATURE( size, 32, __FUNCTION__ );
		if( mem_allocated )
		{
			if( data )
				MEMCPY( mem_allocated, data, size, __FUNCTION__ );
			else
				MEMCLEAR( mem_allocated, size );
			return mem_allocated;
		}
		return nullptr;
	}
}

//todo count ubo ssbo ...
ubo * ubo::make( UINT32 const size, bool const b_dynamic, char const * data, C_PCHAR_C name, bool const b_immediate )
{
	void * mem = nullptr;
	if( data )
	{
		mem = alloc_private( size, data );
		if( !mem )
			ERR_PRINT_STRING( "Asked a ubo with memory can't allocate it in the ubo object" );
	}
	ubo* res = new ubo();
	res->_data = mem;
	res->_size = size;
	res->_b_dynamic = b_dynamic;
	if( name )
		res->set_name( name );
	res->request_init_ogl( b_immediate );
	return res;
}

//ubo * ubo::make(
//    ubo_props const &   p_props, 
//    bool const          p_immediate )
//{
//    ubo * res = new ubo();
//    res->init( p_props );
//    res->request_init_ogl( p_immediate );
//    return res;
//}
void ubo::reclaim( ubo * const pt,  bool const b_immediate )
{
	pt->request_release( b_immediate );
}

ubo::ubo()
	:_data(	nullptr	)
	,_size( 0 )
	,_b_dynamic(false)
{
	inc_count();
}

ubo::~ubo()
{
	IF_FREE_ALIGNED_AND_NULL( _data );

	if( nb_instance > 0 )
		--nb_instance;
	else
		debug_break( "%s() destructor called with ubo count already at zero", __FUNCTION__ );
}

//void ubo::init(ubo_props const & p_props)
//{
//    _props._size    = p_props._size;
//    _props._dynamic = p_props._dynamic;
//
//    _props._data = MALLOC( _props._size );
//    if( p_props._data )
//        MEMCPY( _props._data, p_props._data, _props._size, __FUNCTION__ );
//}

void ubo::init_ogl()
{
	//if( _data )
	//{
		gen_id();
		//todo check we have one

		SPY_PUSH_RANGE( "ubo::define_buffer", spy::GOL_HIGH );
			GLenum CONST usage = _b_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
			//if( GOL::b_named_buffer_use )
			//{
			//	GOL::bind_buffer( GL_UNIFORM_BUFFER, _gol_id );
			//		GOL::set_named_buffer_data( _gol_id, _size, _data, usage );
			//	GOL::unbind_buffer( GL_UNIFORM_BUFFER );
			//}
			//else
			{
				GOL::bind_buffer( GL_UNIFORM_BUFFER, _gol_id );
					GOL::set_buffer_data( GL_UNIFORM_BUFFER, _size, _data, usage );
				GOL::unbind_buffer( GL_UNIFORM_BUFFER );
			}
		SPY_POP_RANGE();
	//}
	//else
	//	debug_break( "%s() can't generate3 and init buffer for null data", __FUNCTION__ );
}

void ubo::bind_and_write( UINT32 const binding_index, void const * data, UINT32 const size )
{
	if( _gol_id==0 )
	{
		ERR_PRINT_STRING( "%s() can't because obj_id is 0, no Ubo", __FUNCTION__ );
		return;
	}
	if( data == nullptr )
	{
		ERR_PRINT_STRING( "%s() can't because no data passed", __FUNCTION__ );
		return;
	}
	
	SPY_PUSH_RANGE( "ubo::bind_and_write", spy::GOL );

		bind( binding_index );
		if( _b_dynamic )
		{
			if( GOL::b_named_buffer_use )
			{
				void * dst = GOL::map_named_buffer_range( _gol_id, 0, size, ACCESS );
					MEMCPY( dst, data, size, __FUNCTION__ );
				GOL::unmap_named_buffer( _gol_id );
			}
			else
			{
				GOL::bind_buffer( GL_UNIFORM_BUFFER, _gol_id );
					void * dst = GOL::map_buffer_range( GL_UNIFORM_BUFFER, 0, size, ACCESS );
						MEMCPY( dst, data, size, __FUNCTION__ );
					GOL::unmap_buffer( GL_UNIFORM_BUFFER );
				GOL::unbind_buffer( GL_UNIFORM_BUFFER );
			}
		}
		else
		{
			if( GOL::b_named_buffer_use )
				GOL::set_named_buffer_data( _gol_id, size, data, GL_STATIC_DRAW );
			else
			{
				GOL::bind_buffer( GL_UNIFORM_BUFFER, _gol_id );
					GOL::set_buffer_data( GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW );
				GOL::unbind_buffer( GL_UNIFORM_BUFFER);
			}
		}
		
	SPY_POP_RANGE();
}

void* ubo::map_write() CONST
{
	if( _gol_id )
	{
		SPY_PUSH_RANGE( "ubo::map_unmap", spy::GOL );

		if( GOL::b_named_buffer_use )
			return GOL::map_named_buffer_range( _gol_id, 0, _size, ACCESS );
		else
		{
			GOL::bind_buffer( GL_UNIFORM_BUFFER, _gol_id );
			return GOL::map_buffer_range( GL_UNIFORM_BUFFER, 0, _size, ACCESS );
		}
	}
	else
	{
		debug_break( "%s() can't map non existing buffer", __FUNCTION__ );
		return nullptr;
	}
}

void ubo::unmap() CONST
{
	if( _gol_id )
	{
		if( GOL::b_named_buffer_use )
			GOL::unmap_named_buffer( _gol_id );
		else
		{
			GOL::unmap_buffer( GL_UNIFORM_BUFFER );
			GOL::unbind_buffer( GL_UNIFORM_BUFFER );
		}
		SPY_POP_RANGE();
	}
	else
		debug_break( "%s() can't unmap non existing buffer", __FUNCTION__ );
}

void ubo::update_ogl()
{
	if( _data )
	{
		if( _b_dynamic )
		{
			void * dst = map_write();
				MEMCPY( dst, _data, _size, __FUNCTION__ );
			unmap();
		}
		else
		{
			SPY_PUSH_RANGE( "ubo::update_ogl", spy::GOL );
				GOL::bind_buffer( GL_UNIFORM_BUFFER, _gol_id );
					GOL::set_buffer_data( GL_UNIFORM_BUFFER, _size, _data, GL_STATIC_DRAW );
				GOL::unbind_buffer( GL_UNIFORM_BUFFER );
			SPY_POP_RANGE();
		}
	}
	else
		debug_break( "%s() can't transfer null data to buffer", __FUNCTION__ );
}
void ubo::release_ogl()
{
	del_id();
}

void ubo::write( void const * data, UINT32 const size, UINT32 const dst_offset, bool const b_immediate )
{
	if( _data )
	{
		MEMCPY( (static_cast<UINT8*>(_data)) + dst_offset, data, size, __FUNCTION__ );
		request_update_ogl( b_immediate );
	}
	else
		debug_break( "%s() can't write to non existing data", __FUNCTION__ );
}

void ubo::write_all( void const * data, bool const b_immediate )
{
	if( _data )
	{
		MEMCPY( _data, data, _size, __FUNCTION__ );
		request_update_ogl( b_immediate );
	}
	else
		debug_break( "%s() can't write to non existing data", __FUNCTION__ );
}

};
