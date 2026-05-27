#include "AAA_event.h"
#include "obj_ui/com/net.h"
#include <unordered_map>
#include "language/lua/aaalua_master.h"
#include "language/lua/aaalua_wrap.h"

bool	c_event::b_verbose		= false;
bool	c_event::b_verbose_sys	= false;


void	c_event::set_verbose( bool in )
{
	b_verbose = in;
	SWITCH_PRINT_STATE( "Verbose Event", b_verbose );
}

void	c_event::flip_verbose()
{
	set_verbose( !b_verbose );
}

typedef	std::unordered_map< UINT32, CREATE_EVENT_FN >	MAP_EVENT_TYPE;

//	this the usual trick to make sure it is defined before used
FINLINE	MAP_EVENT_TYPE&	the_map_event()
{
	static	MAP_EVENT_TYPE					map_event;
	return map_event;
}

void	c_event::register_cod4( UINT32 cod4, CREATE_EVENT_FN create_fn )
{
	the_map_event()[cod4] = create_fn;
}

c_event::c_event()
	:_type		( 'none' )
	,_sub_type	( 'none' )
	,_value_nb	(0)
	,_value		{}
{
}

c_event::~c_event()
{
}

UINT8*	c_event::write_to_mem( UINT8* pt )
{
	pt = c_net::write_value( pt, get_type(), get_sub_type() );

	INT32	nb = get_value_nb();
	pt = c_net::write_value( pt, (nb<<16) | nb );
	
	for( INT32 i = 0; i < nb; ++i )
		pt = c_net::write_value( pt, get_value(i) );

	return pt;
}

UINT8 CONST * c_event::read_from_mem( UINT8 CONST * pt )
{
	pt = c_net::read_value( pt, &_type, &_sub_type );
	
	INT32	test;
	pt = c_net::read_value( pt, &test );
	INT32	nb = test >> 16;
	test = test & 0xffff ;
	if( nb != test )
	{
		ERR_PRINT_STRING( "bad format for event in memory, skipping" );
		return nullptr;
	}
	set_value_nb(nb);

	for( INT32 i = 0; i < nb; ++i )
		pt = c_net::read_value( pt, &_value[i] );

	return pt;
}

bool	c_event::is_to_send()
{
	return false;
}
void	c_event::process_low()
{
	ERR_PRINT_STRING( "%s() try to process an event with no process code", __FUNCTION__ );
}

void	c_event::process()
{
	if( is_obj_exist_and_active(net) )
		net->send_event( this );
	//	EVENT.hook is called here
	auto b_used = g_lua_master->is_hook_event() && g_lua_wrap_master->do_fn_b_pass_ssiiiii( nullptr, "EVENT", "hook", get_type_str(), get_sub_type_str(), _value[0], _value[1], _value[2], _value[3], _value[4] );
	if( b_used )
	{
		if( b_verbose )
			VERBOSE_PRINT_STRING( "Consumed by Lua" );
	}
	else
		process_low();
}

UINT8 CONST *	c_event::do_from_mem( UINT8 CONST * pt )
{
	UINT32	cod4;
	c_net::read_value( pt, &cod4 );

	MAP_EVENT_TYPE::iterator it = the_map_event().find( cod4 );
	if( it != the_map_event().end() )
	{
		CREATE_EVENT_FN	fn = it->second;
		c_event*	ev = fn();
		//c_event_keyboard ev;
		pt = ev->read_from_mem( pt );
		if( pt )
			ev->process_low();
		delete ev;
	}
	else
		debug_break( "low level processing of a BLK_EVENT unimplemented for now" );
	
	return pt;
}
