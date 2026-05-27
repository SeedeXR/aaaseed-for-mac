#include "particle.h"
#include "obj_ui/com/net.h"
#include "obj_ui/com/net_util_obj.h"

UINT8*	c_particle::write_to_mem( UINT8* pt ) const
{
	pt = c_net::write_value( pt, get_id() );
	pt = c_net::write_value( pt, get_flags() );
	pt = c_net::write_value( pt, _color_map_v );

	pt = c_net::write_value_v3( pt, get_speed() );
	pt = c_net::write_value_v3( pt, get_pos() );
	pt = c_net::write_value_v4( pt, _color );

	pt = c_net::write_value( pt, _dt, _death - _birth );

	return pt;
}

void	c_particle::write_to_mem( c_net_buf_obj* net_buf ) const
{
	net_buf->write_value( get_id() );
	net_buf->write_value( get_flags() );
	net_buf->write_value( _color_map_v );

	net_buf->write_value_v3( get_speed() );
	net_buf->write_value_v3( get_pos() );
	net_buf->write_value_v4( _color );

	net_buf->write_value( _dt, _death - _birth );
}

//todo chack if pos_to_draw_ should be used here ?
CONST UINT8*	c_particle::read_from_mem( UINT8 CONST * pt, REAL t )
{
	REAL	life;
	UINT32	dummy;

	pt = c_net::read_value( pt, &dummy );
	set_id( dummy );

	pt = c_net::read_value( pt, &dummy );
	set_flags( dummy );

	pt = c_net::read_value( pt, &_color_map_v );

	pt = c_net::read_value_v3( pt, get_speed() );
	pt = c_net::read_value_v3( pt, get_pos() );
	pt = c_net::read_value_v4( pt, _color );

	pt = c_net::read_value( pt, &_dt, &life );
	_birth = t - _dt;
	_death = _birth + life;
	_life_over_one = OVER_ONE_AS_REAL( life );

	return pt;
}
