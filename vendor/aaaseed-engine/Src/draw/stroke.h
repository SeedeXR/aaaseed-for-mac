
#ifdef AAA_STROKE_H
#error "STROKE_H included more than once."
#endif
#define AAA_STROKE_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class c_stroke //: public c_obj
{
public:	
	struct	st_stroke_elt
	{
		FP32	_pos[3];	// x,y + z
		FP32	_att[3];	// pressure (negative for eraser), _rot_x, _rot_y
		INT32	_time;		//in millisecond (it cover enough in term of duration)
		INT32	_flags;
	};
private:
	std::vector<st_stroke_elt>	_data;
	bool						_b_data;
public:
	REAL						_pressure_max;
	INT32						_msec_origin;
	INT32						_msec_duration;

	CONSTEXPR static INT32 M_ACTIVE = 0x1;
	CONSTEXPR static INT32 M_ERASER = 0x2;

/*	st_stroke_elt& add( INT32 msec )
	{
		st_stroke_elt elt;
		elt._pos[0] = 0;
		elt._pos[1] = 0;
		elt._pos[2] = 0;
		elt._att[0] = 0;
		elt._att[1] = 0;
		elt._att[2] = 0;
		elt._flags = 0;
		elt._time = msec;
		_data.push_back( elt );
		
		return _data.back();
	}
*/
	FINLINE bool is_data()	CONST			{	return _b_data;					}
			bool is_pressure();
			void add( st_stroke_elt CONST & elt );
			void add( st_stroke_elt CONST & elt, FP32* offset );
			void add_raw( c_stroke CONST & stroke_in );
			void clear();
	FINLINE INT32 size()											{	return INT32(_data.size());		}
	FINLINE	std::vector<st_stroke_elt> CONST &	get_data()			{	return _data;					}
	FINLINE st_stroke_elt CONST *				get( INT32 index )	{	if(	index < _data.size() )		return &_data[index];	}
/*
	void add( INT32 msec, FP32 x, FP32 y, FP32 y, FP32 att1, FP32 att2, FP32 att3 );
	{
		st_stroke_elt elt;
		elt._pos[0] = x;
		elt._pos[1] = y;
		elt._pos[2] = z;
		elt._attr[0] = att1;
		elt._attr[1] = att2;
		elt._attr[2] = att3;
		elt._time = msec;
		_data.push( elt );
	}
*/
	c_stroke();
	~c_stroke();

	void	draw( INT32 prim_to_draw );

	void	save_to_file(	o_str CONST & filename		);
	INT32	process_time(	INT32 start					);
	void	offset(			REAL CONST * CONST _offset	);
}; 

class c_stroke_tablet final : public c_stroke
{
public:
	bool	_b_contact;
private:
public:
	c_stroke_tablet();
	virtual	~c_stroke_tablet();
	
	AAA_ERR	load_from_file( o_str CONST & filename );
}; 

class c_strokes_tablet final : public c_obj
{
public:
	c_strokes_tablet();
	virtual ~c_strokes_tablet();

	std::vector<c_stroke_tablet*>	_strokes;
	INT32							_point_nb;
	INT32							_msec_duration;

	void	erase( INT32 index_begin,	INT32 index_end		);
	void	erase_all();
	void	erase_last();
	void	erase_drawn_last();
	void	erase_move_all();

	void	offset( REAL CONST * CONST );

	void	process_time();
};

