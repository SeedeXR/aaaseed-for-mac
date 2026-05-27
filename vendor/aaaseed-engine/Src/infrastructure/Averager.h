
#ifdef AAA_AVERAGER_H
#error "AVERAGER_H included more than once."
#endif
#define AAA_AVERAGER_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#define	CALIBRATION_MAX	1.E+10

class c_obj_ui;

//todo	clean up and make subclasses to have it lighter
class	c_averager final	
{
public:
	static	INT32	size_def;
private:
	REAL	*_data;		//data filtered
	REAL	*_data_raw;	//raw date (not filtered)

	REAL	_last;
//	REAL	last_raw;
//	REAL	last_delta;
//	REAL	last_last_delta;
//	REAL	last_raw_good;
//	INT32	direction;
//	INT32	count;

	INT32	_index;
	INT32	_size;
	INT32	_average_size;
	REAL	_filter_factor;
	
	bool	_b_calibrating;
	REAL	_calibrating_min;
	REAL	_calibrating_max;

	bool	_b_calibrated;
	REAL	_calibrated_min;
	REAL	_calibrated_max;	
	REAL	_output_min;
	REAL	_output_max;

	bool	_b_draw_fixed;
//todo do a color object
	FP32	_color[4];

	REAL	_output_bias;
	REAL	_output_factor;

	void	dealloc();
public:
			c_averager( INT32 size_in = 0, INT32 CONST average_size_in = 0);
			~c_averager();

	FINLINE	INT32	get_size()										CONST	{ return _size; };
			void	set_size(				INT32 CONST new_size );
	FINLINE	INT32	get_average_size()								CONST	{ return _average_size; };
			void	set_average_size(		INT32 CONST new_size);
			void	set_filter_factor_low(	REAL CONST factor_in );
	FINLINE	void	set_filter_factor(		REAL CONST factor_in )
					{
						if ( _filter_factor != factor_in )
							set_filter_factor_low( factor_in);
					}
	
			REAL	get_raw(		INT32 index_wanted = 0 );
			REAL	get_average(	INT32 index_wanted = 0 );
	FINLINE	REAL	get(			INT32 CONST index = 0 )		{	return get_average( index );	}

			bool	put(	REAL alue,		c_obj_ui* CONST caller );
	FINLINE	bool	put_3d(	REAL* CONST pt,	c_obj_ui* CONST caller )	{	return put( *pt, caller );	}

			AAA_ERR	calibrate( REAL CONST min_out, REAL CONST max_out, REAL CONST min_in, REAL CONST max_in );
			void	calibrate_start();
			void	calibrate_stop();
	
			void	set_color( FP32 CONST red_in, FP32 CONST green_in, FP32 CONST blue_in, FP32 CONST alpha_in = 1.);

	FINLINE	bool	set_draw_fixed( bool CONST b_in )	{ return _b_draw_fixed = b_in; };
	FINLINE	bool	is_draw_fixed()		CONST			{ return _b_draw_fixed; };
	FINLINE	bool	toggle_draw_fixed()					{ return set_draw_fixed( !_b_draw_fixed); };

			void	draw_curve_raw_in_rect(		REAL       x, REAL CONST y, REAL CONST w, REAL CONST h, REAL       factor );
			void	draw_curve_average_in_rect( REAL       x, REAL CONST y, REAL CONST w, REAL CONST h, REAL       factor );
			void	draw_curve_in_rect(			REAL CONST x, REAL CONST y, REAL CONST w, REAL CONST h, REAL CONST factor, INT32 CONST draw_mode=3, FP32* CONST color = nullptr );
			void	draw_slider_in_rect(		REAL CONST x, REAL CONST y, REAL CONST w, REAL CONST h	);
};

