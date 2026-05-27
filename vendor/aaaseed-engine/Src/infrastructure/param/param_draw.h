
#ifdef AAA_PARAM_DRAW_H
#error "PARAM_DRAW_H included more than once."
#endif
#define AAA_PARAM_DRAW_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


class c_obj_ui;
class c_param;

extern	INT32		line_drawn;

extern	void		param_draw( c_obj_ui* obj );
extern	c_param *	param_get_from_line( INT32 line );

extern	INT32		get_int32_param_line_offset();
extern	REAL		get_real_param_line_offset();
extern	void		set_real_param_line_offset( REAL in );

namespace aaa::param::flat
{
	extern	c_obj_ui*	obj_found;

	extern	bool		b_char_draw_slider;

	extern	REAL		comment_scale_x;
	extern	REAL		comment_scale_y;
	extern	REAL		comment_trax_scale_x;
	extern	REAL		comment_trax_scale_y;

	extern	REAL		comment_scale_x_over;
	extern	REAL		comment_scale_y_over;
	extern	REAL		comment_trax_scale_x_over;
	extern	REAL		comment_trax_scale_y_over;

	void	scale_update();

	FINLINE void	scale_comment_before();
	FINLINE void	scale_comment_after();
	FINLINE void	scale_comment_trax_before();
	FINLINE void	scale_comment_trax_after();
}

