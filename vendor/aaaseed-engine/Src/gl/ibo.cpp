
// Main header
#include "gl/ibo.h"
#include "spy.h"
#include "err.h"

namespace gl {

ibo_props::ibo_props()
{
	_type		= GOL::IBO_TYPE::IBO_UINT32;
}

ibo_props::ibo_props(	GOL::IBO_TYPE CONST type, UINT32 CONST nb, void CONST * data, bool CONST b_dynamic )
{
	_type		= type;
	_nb			= nb;
	_data		= data;
	_b_dynamic	= b_dynamic;
}

ibo_props::ibo_props( INT32 CONST nb, void CONST * CONST data, bool CONST b_dynamic )
{
	_type		= GOL::IBO_TYPE::IBO_UINT32;
	_nb			= nb;
	_data		= data;
	_b_dynamic	= b_dynamic;
}

namespace{
	UINT32	nb_instance = 0;
	UINT32	nb_instance_created = 0;
	void inc_count()
	{
		++nb_instance;
		++nb_instance_created;
	}

	CONSTEXPR GLbitfield ACCESS = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
}

UINT32*	ibo::get_nb_pt()			{	return &nb_instance;			}
UINT32*	ibo::get_nb_created_pt()	{	return &nb_instance_created;	}

C_PCHAR_C	ibo::get_type_name() { return "ibo"; }

ibo::ibo()
{
	inc_count();
}

ibo::~ibo()
{
	if( nb_instance > 0 )
		--nb_instance;
	else
		debug_break( "%s() destructor called with ibo count already at zero", __FUNCTION__ );
}

//todo deal with 16bits and 8bits index
ibo * ibo::make( ibo_props CONST & props, C_PCHAR_C name, bool CONST b_immediate )
{
	ibo * res = new ibo();
	res->init(props);
	if( name )
		res->set_name( name );
	res->request_init_ogl(b_immediate);
	return res;
}
void ibo::reclaim( ibo * CONST pt, bool CONST b_immediate )
{
	pt->request_release( b_immediate );
}


uint32_t ibo::get_memory_size() CONST
{
	return _nb * GOL::ibo_byte_nb[ static_cast<INT32>(_type) ]; //sizeof(UINT32);
}

void ibo::init( ibo_props CONST & props )
{
	_data       = props._data;
	_type       = props._type;
	_gl_type    = GOL::ibo_gl_type[ static_cast<INT32>(_type) ];
	_nb         = props._nb;
	_b_dynamic  = props._b_dynamic;
 }

void ibo::init_ogl()
{
	gen_id();

	SPY_PUSH_RANGE( "ibo::define_buffer", spy::GOL  )
		GLenum CONST usage = _b_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
		GOL::bind_buffer( GL_ELEMENT_ARRAY_BUFFER, _gol_id );
			GOL::set_buffer_data( GL_ELEMENT_ARRAY_BUFFER, get_memory_size(), _data, usage );
		GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER ) ;
		_nb_allocated = _nb;
	 SPY_POP_RANGE()
}


void ibo::update_ogl()
{
	SPY_PUSH_RANGE( "ibo::update_ogl", spy::GOL  )
		if( _b_dynamic )
		{
			if( GOL::b_named_buffer_use )
			{
				void* dst = GOL::map_named_buffer_range( _gol_id, 0, get_memory_size(), ACCESS );
					MEMCPY( dst, _data, get_memory_size(), __FUNCTION__ );
				GOL::unmap_named_buffer( _gol_id );
			}
			else
			{
				GOL::bind_buffer( GL_ELEMENT_ARRAY_BUFFER, _gol_id );
					void* dst = GOL::map_buffer_range( GL_ELEMENT_ARRAY_BUFFER, 0, get_memory_size(), ACCESS );
						MEMCPY( dst, _data, get_memory_size(), __FUNCTION__ );	
					GOL::unmap_buffer( GL_ELEMENT_ARRAY_BUFFER );
				GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
			}
		}
		else
		{
			GLenum CONST usage = _b_dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
			GOL::bind_buffer( GL_ELEMENT_ARRAY_BUFFER, _gol_id );
				GOL::set_buffer_data( GL_ELEMENT_ARRAY_BUFFER, get_memory_size(), _data, GL_STATIC_DRAW );
			GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER ) ;
		}
	SPY_POP_RANGE()
}

void ibo::release_ogl()
{
	del_id();
}

void ibo::update( INT32 CONST * CONST data, bool CONST b_immediate )
{
	_data = data;
	request_update_ogl( b_immediate );
}

//todo check type
void ibo::update( ibo_props CONST &	props, bool CONST b_immediate )
{
	if( _nb_allocated < props._nb || _type != props._type )
	{
		//todo not sure this work in non immediate mode
		release_ogl();
		init( props );
		request_init_ogl( b_immediate );
		return;
	}

	_data = props._data;
	_nb = props._nb;
	request_update_ogl( b_immediate );
}

void ibo::draw_triangles() const
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements( GL_TRIANGLES, _nb, _gl_type, nullptr );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}
void ibo::draw_triangle_strip() const
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements( GL_TRIANGLE_STRIP, _nb, _gl_type, nullptr );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}
void ibo::draw_lines() const 
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements( GL_LINES, _nb, _gl_type, nullptr );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}
void ibo::draw_line_strip() const
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements( GL_LINE_STRIP, _nb, _gl_type, nullptr );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}
void ibo::draw_points() const
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements( GL_POINTS, _nb, _gl_type, nullptr );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}

void ibo::draw_triangles(       UINT32 CONST instance_nb ) CONST
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements_instanced( GL_TRIANGLES, _nb, _gl_type, nullptr, instance_nb );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}
void ibo::draw_triangle_strip(  UINT32 CONST instance_nb ) CONST
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements_instanced( GL_TRIANGLE_STRIP, _nb, _gl_type, nullptr, instance_nb );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}
void ibo::draw_lines(           UINT32 CONST instance_nb ) CONST
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements_instanced( GL_LINES, _nb, _gl_type, nullptr, instance_nb );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}
void ibo::draw_line_strip(      UINT32 CONST instance_nb ) CONST
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements_instanced( GL_LINE_STRIP, _nb, _gl_type, nullptr, instance_nb );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}
void ibo::draw_points(          UINT32 CONST instance_nb ) CONST
{
	GOL::bind_buffer(   GL_ELEMENT_ARRAY_BUFFER, _gol_id );
	GOL::draw_elements_instanced( GL_POINTS, _nb, _gl_type, nullptr, instance_nb );
	GOL::unbind_buffer( GL_ELEMENT_ARRAY_BUFFER );
}

}

