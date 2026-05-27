

#ifdef AAA_BITMAP_H
#error "BITMAP_H included more than once."
#endif
#define AAA_BITMAP_H 1

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

enum PIXEL_TYPE : INT32
{
	PIXEL_RGB_8 = 0,
	PIXEL_BGRA_8,
	PIXEL_YUYV_8,
	PIXEL_YVYU_8,
	PIXEL_TYPE_MAX_NB
};

INT32	byte_per_pixel[PIXEL_TYPE_MAX_NB]=
{
	3,4,2,2
};

class	c_bitmap
{
protected:
	INT32		_sx;
	INT32		_sy;

	void*		_data;
	INT32		_data_line_inc;

	PIXEL_TYPE	_pixel_type;

public:
//	ACCESSORS	SET
	FINLINE	void	set_size_x( INT32 sx )			{ _sx	= sx; }
	FINLINE	void	set_size_y( INT32 sy )			{ _sy	= sy; }

	FINLINE	void	set_data_line_inc( INT32 in )	{ _data_line_inc = in; }
	FINLINE	void	set_data( void* in )			{ _data = in; }

	FINLINE	void	set_pixel_type( PIXEL_TYPE in )	{ _pixel_type = in; }

//	ACCESSORS	GET
	FINLINE	INT32		get_size_x()				{ return _sx; }
	FINLINE	INT32		get_size_y()				{ return _sy; }

	FINLINE	void*		get_data()					{ return _data; }
	FINLINE	INT32		get_data_line_inc()			{ return _data_line_inc; }

	FINLINE	PIXEL_TYPE	get_pixel_type()			{ return _pixel_type; }
	FINLINE	INT32		get_byte_per_pixel()		{ return byte_per_pixel[_pixel_type]; }
};
