#include "time/delta_t.h"
#include "time/aaa_time.h"

// return true for RESET/RESTART condition
bool	c_delta_t::update()
{
	auto t = aaa::time::get();
	if( t != _t_last )
	{
		_dt = t - _t_last;
		_t_last = t;
		if( _dt < 0. )
		{
			_dt = t - aaa::time::get_last();
			if( _dt < 0. )
				_dt = 0.0;
			return true;
		}
	}
	return false;
}

DOUBLE	c_delta_t::update_v2()
{
	auto t = aaa::time::get();
	if( t == _t_last )
		return 0;
	_dt = t - _t_last;
	_t_last = t;
	return _dt;
}

c_delta_t::c_delta_t()
{
	_t_last = REAL_BIG_VALUE;
}