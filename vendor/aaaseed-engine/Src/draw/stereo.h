
#ifdef AAA_STEREO_H
#error "STEREO_H included more than once."
#endif
#define AAA_STEREO_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_stereo final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_stereo, c_obj_active_ui );
public:
	static	bool	b_quad_buffer_have;
private:
			bool	_b_eye_active;
			bool	_b_do_flip;
			bool	_b_eye_swap;
			bool	_b_right_buffer;
			bool	_b_right_eye;
			bool	_b_quad_buffer_asked;
			bool	_b_quad_buffer_used;
			REAL	_eye_inter;
			REAL	_eye_angle_degree;
			bool	_b_line_code;
			bool	_b_line_code_blue;
			INT32	_line_code_pixel_nb;
			bool	_b_frustum_active;
			bool	_b_frustum_left_centered;
			REAL	_frustum_inter;
			REAL	_near_plane_offset;
			INT32	_gl_buffer_displayed;
			bool	_b_field_first;
			bool	_b_field_for_update;

	FINLINE	bool	is_line_code_blue()			CONST	{ return _b_line_code_blue;			}
protected:
public:
	virtual	void	param_init_pt();

	FINLINE	bool	is_quad_buffer()			CONST	{ return _b_quad_buffer_used;		}
	FINLINE	bool	is_line_code()				CONST	{ return _b_line_code;				}

	FINLINE	bool	is_eye_active()				CONST	{ return _b_eye_active;				}
	FINLINE	REAL	get_eye_inter()				CONST	{ return _eye_inter;				}
	FINLINE	REAL	get_eye_angle_degree()		CONST	{ return _eye_angle_degree;			}

	FINLINE	bool	is_frustum_active()			CONST	{ return _b_frustum_active;			}
	FINLINE	bool	is_frustum_left_centered()	CONST	{ return _b_frustum_left_centered;	}
	FINLINE	REAL	get_frustum_inter()			CONST	{ return _frustum_inter;			}
	FINLINE	REAL	get_nearplane_offset()		CONST	{ return _near_plane_offset;		}

	virtual	void	update();

			void	set_buffer( bool b_back );
			void	draw_line_code();
			INT32	get_y_bottom()				CONST;

			void	flip_right();
	FINLINE	bool	is_right_eye()				CONST	{ return _b_right_eye;				}
	FINLINE	bool	is_right_buffer()			CONST	{ return _b_right_buffer;			}
			void	set_right_buffer( bool in );

	FINLINE	bool	is_field_first()			CONST	{ return _b_field_first;			}
	FINLINE	bool	is_field_for_update()		CONST	{ return _b_field_for_update;		}
	FINLINE	INT32	get_gl_buffer_displayed()	CONST	{ return _gl_buffer_displayed;		}
};

extern	c_stereo*	g_stereo;

