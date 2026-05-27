
#include "gl/Item.h"
#include "gl/Engine.h"
#include "gol/gol_base.h"

namespace gl
{

namespace
{
	UINT32	nb_instance = 0;
	UINT32	nb_instance_created = 0;
	void inc_count()
	{
		++nb_instance;
		++nb_instance_created;
	}
}

UINT32*	item::get_nb_pt()			{	return &nb_instance;			}
UINT32*	item::get_nb_created_pt()	{	return &nb_instance_created;	}

item::item()
	// Members initialization
	:_gol_id(0)
{
	inc_count();
}

item::~item()
{
	if( nb_instance > 0 )
		--nb_instance;
	else
		debug_break( "%s() destructor called with gl::item count already at zero", __FUNCTION__ );
}

void item::gen_id()
{
	if( _gol_id != 0 )
		debug_break( "we already have a buffer in this object" );
	else
		GOL::gen_buffer( _gol_id );
}
void item::del_id()
{
	if( _gol_id )
	{
		GOL::delete_buffer( _gol_id );
		_gol_id = 0;
	}
	else
		debug_break( "ask to delete a 0 buffer id" );
}

void	gl::item::set_name( C_PCHAR_C name )
{
	_o_name.set( get_type_name() );
	_o_name.add_char( ' ' );
	_o_name.add( _gol_id );
	if( name )
	{
		_o_name.add_char( ' ' );
		_o_name.add( name );
	}
}


void gl::item::request_init_ogl()	{	engine::get_instance()->add_queue_init( this );					}
void gl::item::request_update_ogl()	{	engine::get_instance()->add_queue_update( this );				}
void gl::item::request_release()	{	engine::get_instance()->add_queue_release_and_delete( this );	}


} // namespace gl
