
#ifdef AAA_TEX_ANIM_H
#error "TEX_ANIM_H included more than once."
#endif
#define AAA_TEX_ANIM_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_BIND_IMG_H
#	include "image/bind_img.h"
#endif
#ifndef AAA_MAP_H
#	include "draw/map.h"
#endif


class	c_tex_anim final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_tex_anim, c_obj_active_ui );
public:
	static	c_tex_anim*	def;
	static	c_tex_anim*	cur;
	static	c_tex_anim*	ui;
private:
	INT32		_start;
	INT32		_stop;

	INT32		_min;
	INT32		_max;
	INT32		_image_nb;
	INT32		_range;

	INT32		_iw;

	c_delta_t	_delta_t;
	

	REAL		_freq;
	REAL		_phase_offset;
	REAL		_phase;
	REAL		_phase_cur;

	INT32		_image_index;

	REAL		_several_image_step;
	REAL		_several_phase_step;
	REAL		_several_phase;

	bool		_b_restart_trig_ui;
	bool		_b_pingpong;
	bool		_b_active_several;
	bool		_b_active_several_ui;


public:
	virtual	void	param_init_pt();
	virtual	void	update_low();
	FINLINE	void	update()
	{
		if( is_active() || _b_active_several_ui )
			update_low();
		else
			_b_active_several = false;
		c_tex_anim::cur = this;
	}

			INT32	get_index( REAL in );
	FINLINE	INT32	get_index_several( REAL in )
	{
		return get_index( _phase_cur + in * _several_phase_step );
	}
	FINLINE	void	bind_this( INT32 in )
	{
		if( _b_active_several )
		{
			tex_2d_bind( get_index_several(REAL(in)) );
			c_map::get_cur()->do_wrap_2d();
		}
	}
	FINLINE	void	bind_this_w( INT32 in, INT32 nb_u )
	{
		if( _b_active_several )
		{
			tex_2d_bind( get_index_several(REAL(in+nb_u*_iw)) );
			c_map::get_cur()->do_wrap_2d();
		}
	}
	FINLINE	void	bind_next()
	{
		if( _b_active_several )
		{
			_several_phase += _several_phase_step;
			tex_2d_bind( get_index(_several_phase) );
			c_map::get_cur()->do_wrap_2d();
		}
	}

	FINLINE	void	set_start( INT32 start_in )	{ _start = start_in; }
	FINLINE	void	set_stop( INT32 stop_in )	{ _stop = stop_in; }

	FINLINE	INT32	get_start()			CONST	{ return _start; }
	FINLINE	INT32	get_stop()			CONST	{ return _stop; }

	FINLINE	INT32	get_min()			CONST	{ return _min; }
	FINLINE	INT32	get_max()			CONST	{ return _max; }
	FINLINE	INT32	get_image_nb()		CONST	{ return _image_nb; }
	FINLINE	INT32	get_range()			CONST	{ return _range; }

	FINLINE void	set_iw( INT32 iw )			{ _iw = iw;  }
	FINLINE	bool	is_tex_several()	CONST	{ return _b_active_several; }
			void	load_textures();
};

