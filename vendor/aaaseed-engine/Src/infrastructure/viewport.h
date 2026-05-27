
#ifdef AAA_VIEWPORT_H
#error "VIEWPORT_H included more than once."
#endif
#define AAA_VIEWPORT_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_viewport final : public c_obj_ui
{
	FACTORY_DECLARE( c_viewport, c_obj_ui );
private:
	static	c_viewport*	cur;
			INT32		_x;
			INT32		_y;
			INT32		_sx;
			INT32		_sy;
			bool		_b_valid;
public:
	enum VIEWPORT_TYPE : INT32
	{
		VIEWPORT_RENDER = 0,
		VIEWPORT_CURVE,
		VIEWPORT_FOCUS,
		VIEWPORT_INFO,
		VIEWPORT_WINDOW,
		VIEWPORT_FBO,
		VIEWPORT_TYPE_MAX_NB
	};
	static C_PCHAR_C	str_viewport[VIEWPORT_TYPE_MAX_NB];


	static	FINLINE	c_viewport*	get_cur()		{	return cur;		}

			FINLINE	bool		is_valid()		{	return _b_valid;	}
	static	FINLINE	bool		is_valid_cur()	{	return get_cur()->is_valid();	}

	virtual	void	param_init_pt();

			void	doit();
			void	do_scissor();

	FINLINE	INT32	get_x()			{	return _x;				}
	FINLINE	INT32	get_y()			{	return _y;				}
	FINLINE	INT32	get_sx()		{	return _sx;				}
	FINLINE	INT32	get_sy()		{	return _sy;				}
	FINLINE	INT32	get_right()		{	return _x + _sx;		}
	FINLINE	INT32	get_top()		{	return _y + _sy;		}
	FINLINE	REAL	get_ratio_x()	{	return _sx / REAL(_sy);	}
	FINLINE	REAL	get_ratio_y()	{	return _sy / REAL(_sx);	}

	FINLINE	void	get_rect(	INT32& x,	INT32& y,	INT32& sx,	INT32& sy	)	CONST 
			{
				x = _x;
				y = _y;
				sx = _sx;
				sy = _sy;
			}
			void	set_rect(	INT32 x,	INT32 y,	INT32 sx,	INT32 sy	)
			{
				_b_valid = true;
				if( sx <= 0 )
				{
					sx = 0;	
					_b_valid = false;
				}
				if( sy <= 0 )
				{
					sy = 0;
					_b_valid = false;
				}
				_x = x;
				_y = y;
				_sx = sx;
				_sy = sy;
			}

	FINLINE	void	set_rect_do( INT32 x,	INT32 y,	INT32 sx,	INT32 sy )
			{
				set_rect( x,y, sx,sy );
				doit();
			}
	FINLINE	bool	is_in_x(				INT32 x				)	{	return	get_x() <= x && x <= get_right();	}
	FINLINE	bool	is_in_y(				INT32 y				)	{	return	get_y() <= y && y <= get_top();		}
	FINLINE	bool	is_in(					INT32 x,	INT32 y )	{	return	is_in_x(x) && is_in_y(y);			}

			REAL	convert_pix_to_cano_x(	INT32 in );
			REAL	convert_pix_to_cano_y(	INT32 in );

			INT32	convert_cano_to_pix_x(	REAL in );
			INT32	convert_cano_to_pix_y(	REAL in );

			void	convert_pix_x_center(					REAL& out,		REAL in			);
			void	convert_pix_y_center(					REAL& out,		REAL in			);
			void	convert_pix_y_center_hori_scale(		REAL& out,		REAL in			);
			void	convert_pix_xy_center_hori_scale(		REAL& dst_x,	REAL& dst_y,	REAL src_x, REAL src_y );

			void	convert_pix_x_center_crop(				REAL& out,		REAL in			);
			void	convert_pix_y_center_crop(				REAL& out,		REAL in			);
			void	convert_pix_y_center_hori_scale_crop(	REAL& out,		REAL in			);
			void	convert_pix_xy_center_hori_scale_crop(	REAL& dst_x,	REAL& dst_y,	REAL src_x,		REAL src_y );
};

extern	c_viewport*	viewport_ref;
extern	c_viewport*	viewport_render;
extern	c_viewport*	viewport_fbo;

extern	void viewport_set(	c_viewport::VIEWPORT_TYPE index	);
extern	void viewport_init();
extern	void viewport_deinit();

