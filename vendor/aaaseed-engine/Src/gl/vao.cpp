
// Main header
#include "vao.h"
#include "spy.h"
#include "err.h"

namespace gl {


void c_vao_slot_props::init( GOL::VAO_TYPE CONST type, void CONST * CONST data, UINT32 CONST nb, bool CONST b_dynamic )
{
	_type          = type;
	_data          = data;
	_nb            = nb;
	_b_dynamic     = b_dynamic;    	//todo extend because more than 2 in gl
}

c_vao_slot_props::c_vao_slot_props()
{
	init( GOL::VAO_TYPE::UNDEFINED, nullptr, 0, true );
}
c_vao_slot_props::~c_vao_slot_props()
{
}
c_vao_slot_props::c_vao_slot_props( GOL::VAO_TYPE CONST type, void CONST* CONST data, UINT32 CONST nb, bool CONST b_dynamic )
{
	init( type, data, nb, b_dynamic );
}


//void c_vao_props::add_slot( gl::c_vao_slot && slot )
//{
//    _slots.emplace_back( std::move(slot) );
//}
//void c_vao_props::add_slot( gl::c_vao_slot CONST & slot )
//{
//    _slots.emplace_back( slot );
//}
c_vao_slot_props* c_vao_props::add_slot( GOL::VAO_TYPE CONST type, void CONST * CONST data, UINT32 CONST nb, bool CONST b_dynamic )
{
	_slots.emplace_back( c_vao_slot_props( type, data, nb, b_dynamic ) );
	c_vao_slot_props* pt = &(_slots.back());
	pt->set_index( (INT32)(_slots.size()) - 1 );
	return &(_slots.back());
}


void c_vao_slot::init(  c_vao_slot_props CONST & src )
{
	_type			= src._type;
	_data			= src._data;
	_nb				= src._nb;
	_index			= src._index;
	_nb_allocated	= 0;
	_b_dirty		= true;
	_b_dynamic		= src._b_dynamic;    	//todo extend because more than 2 in gl

	INT32 CONST int_type = static_cast<INT32>(_type);
	_elt_byte_nb	= GOL::vao_byte_nb[ int_type ];
	_compo_nb		= GOL::vao_compo_nb[ int_type ];
	_gl_type		= GOL::vao_gl_type[ int_type ];

}

c_vao_slot::c_vao_slot()
{
}
c_vao_slot::~c_vao_slot()
{
	if( _gol_id )
		release_ogl();
}
//gl::c_vao_slot::c_vao_slot( GOL::VAO_TYPE CONST type, void CONST * CONST data, UINT32 CONST nb, bool CONST b_dynamic )
//{
//	 init( type, data, nb, b_dynamic );
//}

C_PCHAR_C	c_vao_slot::get_type_name() { return "vao_slot"; }

void c_vao_slot::init_ogl()
{
	gen_id();

	GOL::bind_buffer( GL_ARRAY_BUFFER, _gol_id );
	if( _data )
	{
		SPY_PUSH_RANGE( "vao::define_buffer", spy::GOL )
			GLenum CONST usage = _b_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
			GOL::set_buffer_data( GL_ARRAY_BUFFER, _elt_byte_nb * _nb, _data, usage );
			_nb_allocated = _nb;
			_b_dirty = false;
		SPY_POP_RANGE()
	}
	

	switch( _type )
	{
	case GOL::VAO_TYPE::UINT4:      GOL::set_vertex_attrib_pointer_int( _index, _compo_nb, _gl_type );						break;
	case GOL::VAO_TYPE::FP32:
	case GOL::VAO_TYPE::VEC2:
	case GOL::VAO_TYPE::VEC3:
	case GOL::VAO_TYPE::VEC4:       GOL::set_vertex_attrib_pointer(     _index, _compo_nb, _gl_type );						break;
	case GOL::VAO_TYPE::UNDEFINED:  GOL::ERR_PRINT_STRING( "%s() slot %d of undefined type",__FUNCTION__, _index		);  break;
	default:                        GOL::ERR_PRINT_STRING( "%s() slot %d of type %d",       __FUNCTION__, _index, _type	);  break;
	}

	GOL::enable_vertex_attrib_array( _index );
}

void c_vao_slot::release_ogl()
{
	del_id();
}

namespace{
	CONSTEXPR GLbitfield ACCESS = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
}

void c_vao_slot::update_ogl()
{
	if( _b_dirty )
	{
		UINT32 byte_nb = _elt_byte_nb * _nb;
		//do not use map on non dynamic as drive 
		if( _b_dynamic )
		{
			if( GOL::b_named_buffer_use )
			{
				SPY_PUSH_RANGE2( "vao::update_buffer named", spy::GOL, _o_name.get() )
					void* dst = GOL::map_named_buffer_range( _gol_id, 0, byte_nb, ACCESS );
						MEMCPY( dst, _data, byte_nb, __FUNCTION__ );
					GOL::unmap_named_buffer( _gol_id );
				SPY_POP_RANGE2()
			}
			else
			{
				SPY_PUSH_RANGE2( "vao::update_buffer", spy::GOL, _o_name.get() )					
					GOL::bind_buffer( GL_ARRAY_BUFFER, _gol_id );
						void* dst = GOL::map_buffer_range( GL_ARRAY_BUFFER, 0, byte_nb, ACCESS );
							MEMCPY( dst, _data, byte_nb, __FUNCTION__ );
						GOL::unmap_buffer( GL_ARRAY_BUFFER );
					GOL::unbind_buffer( GL_ARRAY_BUFFER );					
				SPY_POP_RANGE2()
			}
		}
		else
		{
			SPY_PUSH_RANGE2( "vao::update_buffer", spy::GOL, _o_name.get() )
				GOL::bind_buffer( GL_ARRAY_BUFFER, _gol_id );
					GOL::set_buffer_data( GL_ARRAY_BUFFER, byte_nb, _data, GL_STATIC_DRAW );
				GOL::unbind_buffer( GL_ARRAY_BUFFER );
			SPY_POP_RANGE2()
		}
		_b_dirty = false;
	}
}

//deal with type at some point
void c_vao_slot::set_data( c_vao_slot_props CONST & src, bool CONST b_immediate )
{
	_data = src._data;
	_nb = src._nb;
	_b_dirty = true;
	request_update_ogl( b_immediate );
}

void c_vao_slot::set_data( void CONST * CONST data, UINT32 size, bool CONST b_immediate )
{
	_data = data;
	_nb = size;
	_b_dirty = true;
	request_update_ogl( b_immediate );
}
void c_vao_slot::set_data( void CONST * CONST data, bool CONST b_immediate )
{
	_data = data;
	_b_dirty = true;
	request_update_ogl( b_immediate );
}

namespace {
	UINT32	nb_instance = 0;
	UINT32	nb_instance_created = 0;
	void inc_count()
	{
		++nb_instance;
		++nb_instance_created;
	}
}

UINT32*	c_vao::get_nb_pt()			{	return &nb_instance;			}
UINT32*	c_vao::get_nb_created_pt()	{	return &nb_instance_created;	}

C_PCHAR_C c_vao::get_type_name() { return "vao"; }

c_vao* c_vao::make( c_vao_props CONST & props, C_PCHAR_C name, bool CONST b_immediate )
{
	c_vao * res = new c_vao();
	res->init( props );
	if( name )
		res->set_name( name );
	res->request_init_ogl( b_immediate );
	return res;
}
void c_vao::reclaim( c_vao * CONST pt, bool CONST b_immediate )
{
	pt->request_release( b_immediate );
}

c_vao::c_vao()
{
	inc_count();
}

c_vao::~c_vao()
{
	//todo deal with _pt_slot
	// gl and obj
	if( nb_instance > 0 )
		--nb_instance;
	else
		debug_break( "%s() destructor called with vao count already at zero", __FUNCTION__ );
}

void c_vao::init( c_vao_props CONST & props )
{
	UINT32 nb = props.get_slot_nb();
	 
	_pt_slot = new c_vao_slot[nb];
	for( UINT32 i=0; i<nb; ++i )
		_pt_slot[i].init( props.get_slot(i) );
	_slot_nb = nb;
}

void c_vao::init_ogl()
{	
	GOL::gen_vao( _gol_id );
	bind();

		for( UINT32 i = 0; i < _slot_nb; ++i ) 
		{
			auto & slot = _pt_slot[i];
			slot.init_ogl();
		}
		GOL::unbind_buffer( GL_ARRAY_BUFFER );

	unbind();
}


void c_vao::update_ogl()
{
	UINT32 CONST nb = _slot_nb;
	for( UINT32 i = 0; i < nb; ++i )
		_pt_slot[i].update_ogl();
}

void c_vao::release_ogl()
{
	SAFE_DELETE_ARRAY(_pt_slot);
	GOL::delete_vao( _gol_id );
	_slot_nb = 0;
}

void c_vao::bind() CONST
{
	GOL::bind_vao( _gol_id );
}
void c_vao::unbind() CONST
{
	GOL::unbind_vao();
}

void c_vao::set_slot_data( CONST INT32 idx, void CONST * CONST data, bool CONST b_immediate )
{
	auto & slot = _pt_slot[idx];
	slot.set_data( data, b_immediate );
}

void c_vao::update( c_vao_props CONST & props, bool CONST b_immediate )
{
	bool    b_recreate = false;
	UINT32 nb_slot = props.get_slot_nb();
	if( _slot_nb != nb_slot )
		b_recreate = true;
	else
	{
		for( UINT32 i = 0; i < nb_slot; ++i )
		{
			if( props.get_slot(i).get_nb() > _pt_slot[i].get_nb_allocated() )
			{
				b_recreate = true;
				break;
			}
		}
	}
	if( b_recreate )
	{
		// todo not sure this work in non immediate mode
		release_ogl();
		init( props );
		request_init_ogl( b_immediate );
		//init_ogl();
	}
	else
	{
		for( UINT32 i = 0; i < nb_slot; ++i )
		{
			//todo we do not check type here
			_pt_slot[i].set_data( props.get_slot(i), b_immediate );
		}
	}

}

}