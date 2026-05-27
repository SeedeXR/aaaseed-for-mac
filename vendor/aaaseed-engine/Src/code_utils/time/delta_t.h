
#ifdef AAA_DELTA_T_H
#error "DELTA_T_H included more than once."
#endif
#define AAA_DELTA_T_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class	c_delta_t final
{
private:
	DOUBLE	_t_last;
	DOUBLE	_dt;
public:
			c_delta_t();
			~c_delta_t() {};

			bool	update();
			DOUBLE	update_v2();
	FINLINE	DOUBLE	get_dt()		CONST	{	return _dt;				}
	FINLINE	DOUBLE	get_t()			CONST	{	return _t_last + _dt;	}
	FINLINE	DOUBLE	get_t_last()	CONST	{	return _t_last;			}
	FINLINE	void	force_dt( DOUBLE dt )	{	DOUBLE t = get_t();	_dt = dt; _t_last = t - dt;	 }	
};

