
#ifdef AAA_MOUSE_H
#error "MOUSE_H included more than once."
#endif
#define AAA_MOUSE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_SYSTEMMOUSE_H
#	include "system/shared/SystemMouse.h"
#endif

class c_mouse
{
friend class ui;
private:
	static	REAL			sensitivity;
	static	REAL			multiplier;
	static	REAL			wheel_factor;

	static	REAL			get_multiplicator( REAL CONST mul );
	static	REAL			get_factor();
	static	c_mouse*		cur;

	static	bool			b_grabbed;
	static	bool			b_verbose;
	
public:
	static	DOUBLE			time_last_move;
	static	c_mouse*		get_cur()				{	return cur;				}
	static	FINLINE	REAL*	get_multiplier_pt()		{	return &multiplier;		}
	static	FINLINE	REAL*	get_sensitivity_pt()	{	return &sensitivity;	}
	static	FINLINE REAL*	get_wheel_factor_pt()	{	return &wheel_factor;	}

	static	FINLINE	REAL	get_wheel_factor()		{	return wheel_factor;	}

	static	void			set_grabbed( bool in );
	static	FINLINE	bool	is_grabbed()			{	return b_grabbed;		}

	static	void			set_verbose( bool CONST in );
	static	FINLINE	bool*	get_verbose_pt() 		{	return &b_verbose;		}
	static	void			flip_verbose();

private:
			INT32			_x_pixel;
			INT32			_y_pixel;
			bool			_b_but_state[	mouse::BUTTON_NB ];
			REAL			_but_time_down[	mouse::BUTTON_NB ];
			REAL			_but_time_up[	mouse::BUTTON_NB ];

			void			update_xy();
public:
			void			get_xy_pixel(	INT32& x,		INT32& y		);
			void			get_xy_render(	FP32& x,		FP32& y			);

			void			set_xy_pixel(	INT32 CONST x,	INT32 CONST y	);
//			void			set_xy_pixel_and_cursor(	INT32 CONST x,	INT32 CONST y );

			bool			get_but_state( mouse::BUTTON button ) CONST	{ return _b_but_state[ mouse::make_index(button) ]; }
};


