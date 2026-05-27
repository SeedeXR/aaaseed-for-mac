
#ifdef AAA_MULTI_SCREEN_H
#error "MULTI_SCREEN_H included more than once."
#endif
#define AAA_MULTI_SCREEN_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class c_app;

class c_multi_screen final : public c_obj_ui 
{
	FACTORY_DECLARE( c_multi_screen, c_obj_ui );
friend	c_app;
public:
	static	c_multi_screen*	def;
	static	c_multi_screen*	cur;
private:
	INT32	_nb;
	INT32	_index;

	INT32	_x_nb;
	INT32	_x_index;
	REAL	_x_overlap;
	bool	_b_x_frustum;
	bool	_b_x_right_to_left;
	REAL	_x_rot_step[3];
	REAL	_x_sca_step[3];

	INT32	_y_nb;
	INT32	_y_index;
	REAL	_y_overlap;
	bool	_b_y_frustum;
	bool	_b_y_top_to_bottom;
	REAL	_y_rot_step[3];
	REAL	_y_sca_step[3];
public:

	virtual	void	param_init_pt();
	
			void	set_view( INT32 x, INT32 y, INT32 sx, INT32 sy );
			void	update();
			bool	next();
	
	FINLINE	INT32	get_nb()		{ return _nb;		}
	FINLINE	INT32	get_index()		{ return _index;	}
	FINLINE	INT32	get_x_nb()		{ return _x_nb;		}
	FINLINE	INT32	get_x_index()	{ return _x_index;	}
	FINLINE	INT32	get_y_nb()		{ return _y_nb;		}
	FINLINE	INT32	get_y_index()	{ return _y_index;	}

			void	do_scale( REAL* tmp);
			void	do_rot( REAL* tmp);
			void	adapt_for_cam( FP32& left, FP32& right, FP32& top, FP32& bottom);
};
