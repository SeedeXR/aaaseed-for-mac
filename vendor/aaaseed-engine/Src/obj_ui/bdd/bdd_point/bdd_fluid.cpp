#define	AAAMSA() 0
#if AAAMSA()
#	define	MSA_HOST_AAASEED 1
#	include "MSA/MSAFluid/MSAFluid.h"
#else
//#	include "OpenFramework/MsaFluids/ofxMSAFluidDrawer.h"
#endif
#include "obj_ui/bdd/bdd_point/bdd_fluid.h"
#include "obj_ui/bdd/bdd_multitouch/bdd_blob_tracking.h"

#if !AAAMSA()
#	include "OpenFramework/MsaFluids/ofxMSAFluidDrawer.h"
#endif

#include "image/bind_img_2d.h"
#include "time/aaa_time.h"
#include "image/convert/color_space.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "draw/model.h"

FACTORY_CREATE_PROP_V1( c_bdd_fluid, bdd_fluid, Fluid Joe Stam, bdd_fluid, sub_menu="Point"; );

enum RENDER_TYPE : INT32
{
	RENDER_COLOR = 0,
	RENDER_MOTION,
	RENDER_SPEED,
	RENDER_VECTOR,
	RENDER_TYPE_MAX,
};

static C_PCHAR_C	str_render_type[RENDER_TYPE_MAX] =
{
	"Color",
	"Motion",
	"Speed",
	"Vector"
};

namespace n_bdd_fluid
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 33 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32 MOUSE_BLOB_PARAM_NB = 12;
	CONSTEXPR INT32 GROUP_NB			= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	MOUSE_BLOB_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_BIND_2D_ALONE(	image_dst_bind			)
	//todo regroup with PARAM_DEF_IMG_SIZE_FORMAT ?
		PARAM_DEF_IMG_SIZE(			size_x,					200, 128	)
		PARAM_DEF_IMG_SIZE(			size_y,					200, 128	)

		PARAM_DEF_BOOL_OFF(			restart_trig			)
		PARAM_DEF_BOOL_OFF(			randomize_trig			)
		PARAM_DEF_SYMBO_PSTR_ZERO(	render,	str_render_type )
		PARAM_DEF_BOOL_ON(			use_rgb					)
		PARAM_DEF_BOOL_ON(			use_alpha				)
		PARAM_DEF_BOOL_ON(			alpha_flip				)
		PARAM_DEF_BOOL_OFF(			wrap_x					)
		PARAM_DEF_BOOL_OFF(			wrap_y					)

		PARAM_DEF_REAL_ONE(			time_factor				)
		PARAM_DEF_REAL_ONE(			fade_speed				)

		PARAM_DEF_REAL_ZERO(		color_diffusion			)
		PARAM_DEF_REAL_POS(			viscosity,				1., 0.001	)
		PARAM_DEF_BOOL_OFF(			vorticity_confinement	)
		PARAM_DEF_INT32(			solver_iteration_nb,	1, 10,	1, 512	)

//		PARAM_DEF_POINT_UV( speed )

		PARAM_DEF_GROUP_CLOSED( Mouse_Blob_Inject, MOUSE_BLOB_PARAM_NB )
			PARAM_DEF_BOOL_ON(		inject_mouse				)
			PARAM_DEF_BOOL_OFF(		inject_blob					)
			PARAM_DEF_REAL_POS_ONE(	inject_size_x				)
			PARAM_DEF_REAL_POS_ONE(	inject_size_y				)
			PARAM_DEF_REAL_POS(		inject_size_mouse,			.2, 0.05	)
			PARAM_DEF_REAL_POS(		inject_size_blob,			2., 1		)
			PARAM_DEF_BOOL_ON(		inject_color				)
			PARAM_DEF_REAL_ONE(		inject_color_factor			)
			PARAM_DEF_REAL_INF(		inject_mouse_color_freq,	.25, .1	)
			PARAM_DEF_REAL_INF(		inject_blob_color_freq,		.25, .1	)
			PARAM_DEF_BOOL_ON(		inject_velocity				)
			PARAM_DEF_REAL_ONE(		inject_velocity_factor		)

		PARAM_DEF_BOOL_ON(			smoke_src				)
		PARAM_DEF_BOOL_ON(			image_src				)
		PARAM_DEF_BIND_2D_ALONE(	image_src_bind			)
		PARAM_DEF_REAL_ZERO(		image_threshold			)
		PARAM_DEF_BOOL_OFF(			image_use_mouse_color	)
		PARAM_DEF_FP32_ONE(			image_color_factor		)
		PARAM_DEF_BOOL_ON(			gradient_src			)
		PARAM_DEF_BIND_2D_ALONE(	gradient_src_bind		)
		PARAM_DEF_BOOL_OFF(			gradient_linear			)
		PARAM_DEF_REAL_ZERO(		gradient_factor			)
		PARAM_DEF_SCALE_UVF(		scale					)
		PARAM_DEF_POINT_UV(			justify					)
		PARAM_DEF_BOOL_OFF(			clamped					)
		//PARAM_DEF_IMG_DST(		image_dst				)
	};
}

void	c_bdd_fluid::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();
	param_set_pt( h, _img_dst_bind );
	param_set_pt( h, _size_x );
	param_set_pt( h, _size_y );

	param_set_pt( h, _b_restart_trig_ui );
	param_set_pt( h, _b_randomize_trig );

	param_set_pt( h, _s_render );
	param_set_pt( h, _b_rgb );
	param_set_pt( h, _b_alpha );
	param_set_pt( h, _b_alpha_flip );
	param_set_pt( h, _b_wrap_x );
	param_set_pt( h, _b_wrap_y );

	param_set_pt( h, _time_factor );
	param_set_pt( h, _fade_speed );
	param_set_pt( h, _color_diffusion );
	param_set_pt( h, _viscosity );
	param_set_pt( h, _b_vorticity_confinement );
	param_set_pt( h, _solver_iteration_nb );

//	param_set_pt_2( h, _speed );
	++h;
		param_set_pt( h, _b_inject_mouse );
		param_set_pt( h, _b_inject_blob );
		param_set_pt_4( h, _inject_size );
		param_set_pt( h, _b_inject_color );
		param_set_pt( h, _inject_color_factor );
		param_set_pt( h, _inject_color_mouse_freq );
		param_set_pt( h, _inject_color_blob_freq );
		param_set_pt( h, _b_inject_speed );
		param_set_pt( h, _inject_speed_factor );
		//param_set_pt( h, _img_dst_index );

		param_set_pt( h, _b_smoke_src );
		param_set_pt( h, _b_img_src );
		param_set_pt( h, _img_src_bind_ui );
		param_set_pt( h, _img_threshold );
		param_set_pt( h, _b_img_use_mouse_color );
		param_set_pt( h, _img_color_factor );
		param_set_pt( h, _b_gradient_src );
		param_set_pt( h, _gradient_src_bind_ui );
		param_set_pt( h, _b_gradient_linear_ui );
		param_set_pt( h, _gradient_factor );

		param_set_pt_3( h, _scale );
		param_set_pt_2( h, _justify);
		param_set_pt( h, _b_clamped_ui );

	err_param_init_pt(h);
}

void c_bdd_fluid::init()
{
	param_init_with( n_bdd_fluid::param, n_bdd_fluid::PARAM_NB_MAX);
}

CONSTRUCTOR_CREATE(c_bdd_fluid)
,_drawer(nullptr)
,_solver(nullptr)
{
	init();
//	alloc();
}

c_bdd_fluid::~c_bdd_fluid()
{
	obj_delete(	_drawer );
	obj_delete(	_solver );
//	dealloc();
}

void	c_bdd_fluid::restart()
{
	_b_restart_trig_ui = true;
}

void	c_bdd_fluid::fill_rgb_from( c_img_2d* src )
{
	//hack
	if( !src )
	{
		return;
	}
	INT32	sx = _solver->getWidth() - 2;
	INT32	sy = _solver->getHeight() - 2;

	FP32	col[3];

	REAL	u;
	REAL	v;
	REAL	du = OVER_ONE_AS_REAL( (sx-1) * _scale[0] * _scale[2] );
	REAL	dv = OVER_ONE_AS_REAL( (sy-1) * _scale[1] * _scale[2] );

	v = REAL(.5) * (_justify[1]+REAL(1)) * (REAL(1)-REAL(1)/(_scale[1]*_scale[2])) + dv * REAL(.5) ;
	for( int j=1; j <= sy; ++j )
	{
		u = REAL(.5) * (_justify[0]+REAL(1)) * (REAL(1)-REAL(1)/(_scale[0]*_scale[2])) + du * REAL(.5) ;
		for( int i=1; i <= sx; ++i )
		{
			if( !_b_clamped_ui || ( INSIDE_01(u) && INSIDE_01(v) ) )
			{
				src->get_color3r_from_uv( col, u, v, _b_clamped_ui );	//todo have a fn with gradient
			
				if( _img_threshold < col[1] )
				{
					if( _b_img_use_mouse_color )
						mul_v3( col, _rgb );
					scale_v3( col, _img_color_factor );
#if AAAMSA()
					MSA::Color	tmp( col[0], col[1], col[2] );
					_solver->addColorAtCell( i, j, tmp );
#else
					_solver->addColorAtCell( i, j, col );
#endif
				}
			}
			u += du;
		}
		v += dv;
	}
}

void	c_bdd_fluid::fill_speed_from( c_img_2d* src )
{
	INT32	sx = _solver->getWidth();
	INT32	sy = _solver->getHeight();

	REAL	fu	= -_gradient_factor;	// * OVER_ONE(sx);
	REAL	fv	= -_gradient_factor * sx * OVER_ONE_AS_REAL(sy);	// * OVER_ONE(sy);
	sx -= 2;
	sy -= 2;

	REAL	vec[3];

	REAL	u;
	REAL	v;
	REAL	du = OVER_ONE_AS_REAL( (sx-1) * _scale[0] * _scale[2] );
	REAL	dv = OVER_ONE_AS_REAL( (sy-1) * _scale[1] * _scale[2] );


	v = REAL(.5) * (_justify[1]+REAL(1)) * (REAL(1)-REAL(1)/(_scale[1]*_scale[2])) + dv * REAL(.5);
	for( int j=1; j <= sy; ++j )
	{
		u = REAL(.5) * (_justify[0]+REAL(1)) * (REAL(1)-REAL(1)/(_scale[0]*_scale[2])) + du * REAL(.5);
		for( int i=1; i < sx; ++i )
		{
			if( !_b_clamped_ui || ( INSIDE_01(u) && INSIDE_01(v) ) )
			{
				src->get_gradient_from_uv( vec, u, v, _b_clamped_ui, aaa::COMPO::GREY, _b_gradient_linear_ui );
#if AAAMSA()
				MSA::Vec2f force( vec[0]*fu, vec[1]*fv );
				_solver->addForceAtCell( i, j, force );
#else
				_solver->addForceAtCell( i, j, vec[0]*fu, vec[1]*fv );
#endif
			}
			u += du;
		}
		v += dv;
	}  
}

void	c_bdd_fluid::add_blob( REAL x, REAL y, REAL dx, REAL dy, REAL size_x, REAL size_y )
{
	REAL speed = dx*dx + dy*dy;
	if( speed > .000001 )
	{
		REAL	sx = REAL(_solver->getWidth() );
		REAL	sy = REAL(_solver->getHeight());
		INT32	spx = MAX0( INT32((sx-2) * size_x * REAL(.5)) );
		INT32	spy = MAX0( INT32((sy-2) * size_y * REAL(.5)) );
		for( INT32 i=-spx; i<=spx; ++i )
		{
			REAL fx = i / REAL(spx+1);
			INT32 ix = INT32(x*sx + i);
			for( INT32 j=-spy; j<=spy; ++j )
			{
				REAL fy = j / REAL(spy+1);
				REAL s = fx*fx + fy*fy;
				if( s < 1. )
				{
					s = REAL(1) - SQRT(s);
					INT32 iy = INT32(y*sy + j);
					if( _b_inject_speed )
					{
#if AAAMSA()
						MSA::Vec2f force( s*dx*_inject_speed_factor, s*dy*_inject_speed_factor );
						_solver->addForceAtCell( ix, iy, force );
#else
						_solver->addForceAtCell( ix, iy, s*dx*_inject_speed_factor, s*dy*_inject_speed_factor );
#endif
					}
					if( _b_inject_color )
					{
						s *= _inject_color_factor;
#if AAAMSA()
						if( _b_rgb )
							_solver->addColorAtCell(	ix,	iy, MSA::Color( _rgb[0]*s, _rgb[1]*s, _rgb[2]*s ) );
						else
							_solver->addColorAtCell(	ix,	iy, MSA::Color( s, 0, 0 ) );
#else
						if( _b_rgb )
							_solver->addColorAtCell(	ix,	iy, _rgb[0]*s, _rgb[1]*s, _rgb[2]*s );
						else
							_solver->addColorAtCell(	ix,	iy, s );
#endif
					}
				}
			}
		}
	}	
}

void	c_bdd_fluid::update()
{
	if( !_drawer )
	{
		GOOD_PRINT_STRING( "bdd_fluid step 1");
#if AAAMSA()
		_drawer = new MSA::FluidDrawerGl;
#else
		_drawer = new ofxMSAFluidDrawer;
#endif
		GOOD_PRINT_STRING( "bdd_fluid step 2");
		if( !_drawer )
			return;
		GOOD_PRINT_STRING( "bdd_fluid step 3");
		_solver = _drawer->setup( _size_x, _size_y );
		if( !_solver )
			return;
		_drawer->reset();
	}
	GOOD_PRINT_STRING( "bdd_fluid step 4");
	if( !_solver )
		return;
	GOOD_PRINT_STRING( "bdd_fluid step 5");
	_drawer->set_dst_img( g_bind_img_2d->get_img( _img_dst_bind, aaa::PIXEL_FORMAT::RGBA_8, _size_x, _size_y, true, nullptr, __FUNCTION__ ) );
	_drawer->setSize( _size_x, _size_y );
	if( _delta_t.update() )
		_b_restart_trig_ui = true;
	if( _b_restart_trig_ui )
	{
#if	AAA_DEBUG()
		DBG_PRINT_STRING( "fluid restart");
#endif;
		if( _drawer )
			_drawer->reset();
		_b_restart_trig_ui = false;
	}
	if( _b_randomize_trig )
	{
#if	AAA_DEBUG()
		DBG_PRINT_STRING( "fluid randomize");
#endif;
		if( _solver )
			_solver->randomizeColor();
		_b_randomize_trig = false;
	}

	REAL t = REAL(aaa::time::get());
	_solver->enableRGB( _b_rgb );
	_solver->setDeltaT( t * _time_factor );
	_solver->setFadeSpeed( _fade_speed );

	_solver->setVisc( _viscosity );
	_solver->setColorDiffusion( _color_diffusion );
	_solver->enableVorticityConfinement( _b_vorticity_confinement );
	_solver->setSolverIterations( _solver_iteration_nb );
	_solver->setWrap( _b_wrap_x, _b_wrap_y );

/*
	//	deal with restart
	if( _delta_t.update() )
	{
		DBG_PRINT_STRING( "boid time restart" );
		restart();
	}
*/
	
	FP32	hsv[3];
	hsv[1] = 1.;
	hsv[2] = 1.;

	if( _b_inject_blob && (_b_inject_color || _b_inject_speed) )
	{
		c_bdd_blob_tracking*	bt = c_bdd_blob_tracking::cur;
		if( bt )
		{
			INT32 nb = bt->get_finger_nb();
			for( INT32 i=0; i<nb; ++i )
			{
				c_finger* f = bt->get_finger( i );
				hsv[0] = f->_id_tracking * _inject_color_blob_freq;
				aaa::color::rgb_from_hsv( _rgb, hsv );
				add_blob(	f->_center.get_x(),		f->_center.get_y(),
							f->_delta.get_x(),		f->_delta.get_y(),
							f->_box.get_size_x()*_inject_size[0]*_inject_size[3],
							f->_box.get_size_y()*_inject_size[1]*_inject_size[3]
						);
			}
		}
	}

	hsv[0] = t * _inject_color_mouse_freq;
	aaa::color::rgb_from_hsv( _rgb, hsv );
	if( _b_inject_mouse && (_b_inject_color || _b_inject_speed) )
	{	
		REAL x,y;
		c_mouse::get_cur()->get_xy_render( x,y );
		add_blob( x,y, _x_last-x, _y_last-y, _inject_size[0]*_inject_size[2], _inject_size[1]*_inject_size[2] );
		_x_last = x;
		_y_last = y;
	}
	if( _b_smoke_src )
		for( INT32 i=2; i<=2; ++i )
		{
			INT32 ii = i/10;
#if AAAMSA()
			_solver->addForceAtCell( ii,0, MSA::Vec2f(0., .1) );
			_solver->addColorAtCell( ii,0, MSA::Color(1, 1, 1) );
#else
			_solver->addForceAtCell( ii,0, float(0), float(.1) );
			_solver->addColorAtCell( ii,0, float(1), float(1), float(1) );
#endif
		}
	if( _b_img_src )
		fill_rgb_from( g_bind_img_2d->get_ready(_img_src_bind_ui) );
	if( _b_gradient_src )
		fill_speed_from( g_bind_img_2d->get_ready(_gradient_src_bind_ui) );
	_drawer->update();

}

void	c_bdd_fluid::draw_single()
{
	bool b = _b_alpha;
	bool b_flip = _b_alpha_flip;

	switch( _s_render )
	{
	case 0:	_drawer->drawColor( 0, 0, 256, 256, b, b_flip );	break;
	case 1:	_drawer->drawMotion( 0, 0, 256, 256, b, b_flip );	break;
	case 2:	_drawer->drawSpeed( 0, 0, 256, 256, b, b_flip );	break;
	case 3: 
		{
			REAL size[3];
			c_model::cur->get_size_v3( size );
			_drawer->drawVectors( -size[0]*REAL(.5), -size[1]*REAL(.5), size[0], size[1] );
		}
		break;
	}
}

void	c_bdd_fluid::draw_multiple()
{
}

