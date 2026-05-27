
#ifdef AAA_AAARECT_H
#error "AAARECT_H included more than once."
#endif
#define AAA_AAARECT_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class c_aaarect
{
private:
	REAL	_l;
	REAL	_b;
	REAL	_sx;
	REAL	_sy;
public:
	FINLINE	REAL	get_left()		{ return _l; }
	FINLINE	REAL	get_bottom()	{ return _b; }
	FINLINE	REAL	get_size_x()	{ return _sx; }
	FINLINE	REAL	get_size_y()	{ return _sy; }
	FINLINE	REAL	get_right()		{ return _l + _sx; }
	FINLINE	REAL	get_top()		{ return _b + _sy; }

	FINLINE	void	set_lbwh( REAL left, REAL bottom, REAL size_x, REAL size_y )
							{
								_l = left;
								_b = bottom;
								_sx = size_x;
								_sy = size_y;
							}
	FINLINE	void	set_lbrt( REAL left, REAL bottom, REAL right, REAL top )
							{
								_l = left;
								_b = bottom;
								_sx = right - left;
								_sy = top - bottom;
							}
	FINLINE	REAL	get_center_hori() {	return	REAL( _l + _sx*.5 ); }
	FINLINE	REAL	get_center_vert() {	return	REAL( _b + _sy*.5 ); }
};


