#include "draw/tex_anim.h"
#include "time/aaa_time.h"
#include "image/bind_img_2d.h"
#include "infrastructure/param/param_declare.h"


FACTORY_CREATE_V1( c_tex_anim, tex_anim, Texture Animation, anim );


void texture_anim_set_start( c_param* param )
{
	c_tex_anim::ui->set_start( bind_ui_get() );
}

void texture_anim_set_stop( c_param* param )
{
	c_tex_anim::ui->set_stop( bind_ui_get() );
}

namespace	n_tex_anim
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 11;
	CONSTEXPR INT32 OUT_PARAM_NB	= 2;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	OUT_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(			time_active )
		PARAM_DEF_BOOL_OFF(			pingpong )

		PARAM_DEF_INT32_POS_ZERO(	start )
		PARAM_DEF_INT32_POS_ZERO(	stop )
		PARAM_DEF_REAL_INF(			frequency, 1., 25. )
		PARAM_DEF_REAL_ZERO(		phase_offset )
		PARAM_DEF_BOOL_OFF(			restart_trig )
		PARAM_DEF_BOOL_OFF(			several_active )
		PARAM_DEF_REAL_ONE(			several_step )
		//todo understand this weird mecanism
		PARAM_DEF_INT32_POS_FN(		force_start, texture_anim_set_start )
		PARAM_DEF_INT32_POS_FN(		force_stop,  texture_anim_set_stop  )
	//todo	do it or destroy it : param "realtime"
		PARAM_DEF_GROUP( Out, OUT_PARAM_NB)
			PARAM_DEF_REAL_LOCKED(		phase )
			PARAM_DEF_INT32_LOCKED(		image_index )
	};
}

c_tex_anim*	c_tex_anim::def = nullptr;
c_tex_anim*	c_tex_anim::cur = nullptr;
c_tex_anim*	c_tex_anim::ui	= nullptr;

void	c_tex_anim::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_pingpong );

	param_set_max_no_inc( h, REAL(g_bind_img_2d->get_bind_max_nb()-1) );
	param_set_pt( h, _start );
	param_set_max_no_inc( h, REAL(g_bind_img_2d->get_bind_max_nb()-1) );
	param_set_pt( h, _stop );

	param_set_pt( h, _freq );
	param_set_pt( h, _phase_offset );
	param_set_pt( h, _b_restart_trig_ui );

	param_set_pt( h, _b_active_several_ui );
	param_set_pt( h, _several_image_step );

	//todo understand this weird mecanism we already used these but with not update fns
	param_set_max_no_inc( h, REAL(g_bind_img_2d->get_bind_max_nb()-1) );
	param_set_pt( h, _start );
	param_set_max_no_inc( h, REAL(g_bind_img_2d->get_bind_max_nb()-1) );
	param_set_pt( h, _stop );

	++h;
		param_set_pt( h, _phase_cur );
		param_set_pt( h, _image_index );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_tex_anim)
,_iw(0)
,_phase(0.)
,_phase_cur(0.)
, _image_index(0)
{
	param_init_with( n_tex_anim::param, n_tex_anim::PARAM_NB_MAX );	// tex_anim_param, TEX_ANIM_PARAM_NB_MAX);
}

c_tex_anim::~c_tex_anim()
{
	if( cur == this )
		cur = def;
	if( ui == this )
		ui = def;
}

//todoq deal with all case
INT32	c_tex_anim::get_index( REAL in )
{
	INT32 tmp = INT32(_image_nb * FMOD( in ));
	if( _b_pingpong )
		return _min + tmp;
	else if( _range >= 0 )
		return _min + tmp;
	else
		return _max - tmp;
}


void	c_tex_anim::update_low()
{
	_range = _stop - _start;
	if( _range > 0 )
	{
		_min = _start;
		_max = _stop;
		_image_nb = _range + 1;
	}
	else if( _range != 0 )
	{
		_min = _stop;
		_max = _start + 1; //hack +.99 for the moment
		_image_nb = 1 - _range;
	}
	else
	{
		_min = _max = _start;
		_image_nb = 1;
	}

	if( is_active() )
	{
		if( _delta_t.update() || _b_restart_trig_ui )
		{
			_phase = 0.;
			_b_restart_trig_ui = false;
		}
		else
			_phase += REAL(_delta_t.get_dt()) * _freq;
		tex_2d_bind( get_index(_phase) );
		c_map::get_cur()->do_wrap_2d();
	}
	else if( _b_restart_trig_ui )
	{
		_phase = 0.;
		_b_restart_trig_ui = false;
	}
		
	_phase_cur = _phase + _phase_offset;
	_image_index = get_index( _phase_cur );

	_several_phase_step = _several_image_step / REAL(_image_nb);
	_several_phase = _phase_cur;

	_b_active_several = _b_active_several_ui
						&& !c_map::get_cur()->is_0d()
						&& !c_map::get_cur()->is_texture_feedback();
}

//todo keep a flag to do it when needed only
void	c_tex_anim::load_textures()
{
	for( INT32 i=_min; i<=_max; ++i )
		tex_2d_bind( i );
}

