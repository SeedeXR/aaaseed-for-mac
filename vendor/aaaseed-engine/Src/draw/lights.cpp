#include "draw/lights.h"
#include "time/aaa_time.h"
#include "draw/mat.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/param/param_declare.h"
#include "infrastructure/namer.h"
#include "ui/strsymbo.h"
#include "gl/ubo.h"
#include "gl/ssbo.h"
#include "spy.h"
#include "math/v.h"


/*
#define	LOW			0.5
#define	HIGH		0.8
#define	DONF		1.5
#define	GREY20		0.2,	0.2,	0.2,	1.
#define	GREY50		0.5,	0.5,	0.5,	1.
#define	GREYLOW		LOW,	LOW,	LOW,	LOW
#define	BLACK		0.,		0.,		0.,		0.
#define	WHITE		1.,		1.,		1.,		1.
#define	WHITE_DONF	DONF,	DONF,	DONF,		1.
#define	RED			1.,		0.,		0.,		1.
#define	GREEN		0.,		1.,		0.,		1.
#define	YELLOW		1.,		1.,		0.,		1.
#define	BLUE		0.,		0.,		1.,		1.
#define	RED_LOW			LOW,	0.,		0.,		1.
#define	GREEN_LOW		0.,		LOW,	0.,		1.
#define	BLUE_LOW		0.,		0.,		LOW,	1.
#define	SPECULAR		.3,		.3,		0.,		1.

#define	NZ		0.00001
#define	ZERO	0.
#define	ONE		1.

#define	LIGHT_ABS	LIGHT_WORLD
#define	LIGHT_REL	LIGHT_CAMERA

	{
		GREY50,
		WHITE,
		WHITE,
		{	1.,		NZ,		NZ,		0.},
		{	-1.,	0.,		0.},
		0., 180.,
		GL_LINEAR_ATTENUATION, 1.0,
		OFF,
		LIGHT_ABS
	},
	{
		GREY50,
		WHITE,
		WHITE,
		{	NZ,		1.,		NZ,		0.},
		{	0.,	-1.,	0.},
		0., 180.,
		GL_LINEAR_ATTENUATION, 1.0,
		OFF,
		LIGHT_ABS
	},
	{
		GREY50,
		WHITE,
		WHITE,
		{	NZ,		NZ,		1.,		0.},
		{	0.,	0.,		-1.},
		0., 180.,
		GL_LINEAR_ATTENUATION, 1.0,
		OFF,
		LIGHT_ABS
	},
	{
		RED_LOW,
		RED,
		SPECULAR,
		{	1.,		NZ,		NZ,	0.},
		{	-1.,	0.,		0.},
		0., 180.,
		GL_LINEAR_ATTENUATION, 1.0,
		OFF,
		LIGHT_ABS
	},
	{
		GREEN_LOW,
		GREEN,
		SPECULAR,
		{	NZ,		1.,		NZ,		0.},
		{	0.,	-1.,	0.},
		0., 180.,
		GL_LINEAR_ATTENUATION, 1.0,
		OFF,
		LIGHT_ABS
	},
	{
		BLUE_LOW,
		BLUE,
		SPECULAR,
		{	NZ,		NZ,		1.,		0.},
		{	0.,	0.,		-1.},
		0., 180.,
		GL_LINEAR_ATTENUATION, 1.0,
		OFF,
		LIGHT_ABS
	},
	{
		GREY50,
		BLUE,
		WHITE,
		{	0.,		0.,		-1.,		0.},
		{	0.,	0.,		1.},
		0., 180.,
		GL_LINEAR_ATTENUATION, 1.0,
		OFF,
		LIGHT_ABS
	}
*/

namespace{
	gl::ubo  * ubo_light_nb	    = nullptr;
	gl::ssbo * ssbo_directional = nullptr;
	gl::ssbo * ssbo_point       = nullptr;
	gl::ssbo * ssbo_spot        = nullptr;
}

FACTORY_CREATE_V1( c_lights, lights, Lights, lightg );

namespace	n_lights
{
	CONSTEXPR INT32	BASE_PARAM_NB		= 14;
	CONSTEXPR INT32	LIGHT_PARAM_NB		= c_lights::LIGHT_NB;
	CONSTEXPR INT32	MATERIAL_PARAM_NB	= 2;
	CONSTEXPR INT32	GROUP_PARAM_NB		= 3;
	CONSTEXPR INT32	PARAM_NB_MAX		=	BASE_PARAM_NB
											+	LIGHT_PARAM_NB
											+	MATERIAL_PARAM_NB
											+	GROUP_PARAM_NB;
	
	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_GROUP( Global, BASE_PARAM_NB )
			PARAM_DEF_FP32_ONE(		intensity_factor	)
			PARAM_DEF_COLOR_RGBGA(	ambient_global		)
			PARAM_DEF_BOOL_OFF(		flag_atte			)
			PARAM_DEF_BOOL_OFF(		local_viewer		)
			PARAM_DEF_BOOL_OFF(		two_side			)
			PARAM_DEF_INT32(		start,				1,0,					0,c_lights::LIGHT_NB-1 )
			PARAM_DEF_INT32(		stop,				1,c_lights::LIGHT_NB-1, 0, c_lights::LIGHT_NB-1 )
			PARAM_DEF_INT32(		psy,				1,0,					0,2 )
			PARAM_DEF_REAL_ZERO(	psy_frequency		)
			PARAM_DEF_REAL_ZERO(	psy_phase_offset	)

		PARAM_DEF_GROUP( Lights, LIGHT_PARAM_NB )
			PARAM_DEF_0_7( light, PARAM_DEF_BOOL_OFF_SAVE_NOT )

		PARAM_DEF_GROUP( Material, MATERIAL_PARAM_NB )
			PARAM_DEF_SYMBO_NEG_SAVE_NOT(	material_front,	8,-1,				-1, MATERIAL_MAX_NB-1, gstr::current )
			PARAM_DEF_SYMBO_NEG_SAVE_NOT(	material_back,	8,-1,				-1, MATERIAL_MAX_NB-1, gstr::current )
	};
}

void	c_lights::param_init_pt()
{
	INT32	h = 0;

	++h;
		param_set_pt(		h, _intensity_factor );
		param_set_pt_rgbfa( h, _ambient );
		param_set_pt(		h, _b_atte );
		param_set_pt(		h, _b_local_viewer );
		param_set_pt(		h, _b_two_side );
		param_set_pt(		h, _start );
		param_set_pt(		h, _stop );
		param_set_pt(		h, _psy );
		param_set_pt(		h, _psy_freq );
		param_set_pt(		h, _psy_phase_offset );

	++h;
		for( INT32 i = 0; i < LIGHT_NB; ++i )
		{
			param_attach_obj_no_inc( h, &_lights[i] );
			if( &_lights[i] )
			{
				if( c_lights_switch::ui )
					param_set_pt( h, c_lights_switch::ui->_b_on[i] );
				else
					param_set_pt_null( h );
			}
			else
				++h;
		}

	++h;
		if( c_layer::get_ui() )
		{
			param_attach_obj_no_inc( h, c_layer::get_ui()->get_material_front() );
			param_set_pt( h, c_layer::get_ui()->get_mat_front_ui_pt() );
			param_attach_obj_no_inc( h, c_layer::get_ui()->get_material_back() );
			param_set_pt( h, c_layer::get_ui()->get_mat_back_ui_pt() );
		}
		else
		{
			h += 2;
		}

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_lights )
{
	set_ambient_5f( FP32(.4), FP32(.3), FP32(.1), 1., 1. );
	param_init_with( n_lights::param, n_lights::PARAM_NB_MAX ); //  lights_param, LIGHTS_PARAM_NB_MAX);
	init();
}

c_lights::~c_lights()
{
	if( cur == this )
		set_cur_null();
	if( ui == this )
		set_ui_null();
}

void	c_lights::init()
{
	_phase_last = 0;
	_phase = 0;
	
	c_light*	l = _lights;
	for( INT32 i = 0; i < LIGHT_NB; ++i )
	{
		l->set_index( i );
		l->set_root( this );
		++l;
	}
}

void	c_lights::set_atte( bool value )
{
	_b_atte = value;
}

void	c_lights::set_intensity_factor( FP32 intensity_factor_in )
{
	_intensity_factor = intensity_factor_in;
}

void	c_lights::mult_intensity_factor( FP32 factor_in )
{
	set_intensity_factor( _intensity_factor * factor_in ); 
}

void	c_lights::set_ambient_5f( FP32 r, FP32 g, FP32 b, FP32 a, FP32 grey )
{
	_ambient[0] = r;
	_ambient[1] = g;
	_ambient[2] = b;
	_ambient[3] = a;
	_ambient[4] = grey;
}

void	c_lights::flip_psy()
{
	C_PCHAR	p;
	_psy = IMOD( _psy+1, 3);
	if( _psy == 0 )
		p = "OFF";
	else if( _psy == 1 )
		p = "Color";
	else
		p = "Position";

	SWITCH_PRINT_STRING( "Light Psy", p );
}

void	c_lights::set_pos( glm::mat4 CONST * mat_view_inverse )
{
	c_light*	l = &_lights[_start];

	if( _psy == 2 && _stop != 0 )
	{
		for( INT32 index = _start; index <= _stop; ++index )
		{
			if( l->is_active() )
				l->set_pos_psy( mat_view_inverse );
			++l;
		}
	}
	else
	{
		for( INT32 index = _start; index <= _stop; ++index )
		{
			if( l->is_active() )
				l->set_pos( mat_view_inverse );
			++l;
		}
	}
}

void	c_lights::set_local_viewer(	bool b )	{	_b_local_viewer = b;	}
void	c_lights::set_two_side(		bool b )	{	_b_two_side = b;		}

void	c_lights::set()
{
	//todo GL_LIGHT_MODEL_COLOR_CONTROL params must be either GL_SEPARATE_SPECULAR_COLOR or GL_SINGLE_COLOR
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER,		_b_local_viewer	? 1 : 0 );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE,			_b_two_side		? 1 : 0 );

	FP32	ambient_loc[4];
	scale_v3_cpy_v4( ambient_loc, _ambient );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT,			(GLfloat*) ambient_loc );

	if( _psy == 1 )
	{
		REAL	phase_new = REAL(aaa::time::get());
		REAL	value = phase_new - _phase_last;
		if( value <=  0. )
			_phase = 0.;
		else
			_phase += value * _psy_freq;
		_phase_last = phase_new;
	}
}

void	c_lights::update()
{
	set_cur( this );
	c_lights_switch::cur	= nullptr;	//	because light reinit the enable/disable

//todo	refine light update by light
	set();
//todoq	establish this again if necessary
//	bad with lights_switch now
	c_light* l = &_lights[0];
	INT32		index = 0;
/*
	for( ; index < _start; ++index )
	{
		l->force_disable();
		++l;
	}
*/
	for(  index = 0; index < LIGHT_NB; ++index )
	{
		if( l->is_active() )
			l->update();
//		if( l->is_active() )
//			l->force_enable();
//		else
//			l->force_disable();
		++l;
	}
/*	
	for( ; index < GOL::LIGHT_NB; ++index )
	{
		l->force_disable();
		++l;
	}
*/
}

void	c_lights::draw()
{
	c_light* l = &_lights[0];
	for( INT32 index = 0; index < LIGHT_NB; ++index )
	{
		if( l->is_active() )
			l->draw();
		++l;
	}
}

static CONST CHAR light_ext[] = "0.light";

AAA_ERR c_lights::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();
		filename.add_char( '0' );

		c_light*	l = _lights;
		for( INT32 i = 0; i < LIGHT_NB; ++i )
		{
			filename.set_char( -1, '0' + i );
			l->save_to_file_add_ext( filename );
			++l;
		}
	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR c_lights::load_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();
		filename.add_char( '0' );

		c_light*	l = _lights;
		for( INT32 i = 0; i < LIGHT_NB; ++i )
		{
			filename.set_char( -1, '0' + i );
			l->load_from_file_add_ext( filename );
			++l;
		}
	o_str::pop_name();
	return AAA_OK;
}


FACTORY_CREATE_V1( c_lights_switch, lights_switch, light switchs, lights_switch );

namespace	n_lights_switch
{
	CONSTEXPR INT32	BASE_PARAM_NB	=	c_lights::LIGHT_NB;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_0_7( light_switch, PARAM_DEF_BOOL_OFF )
	};
}


void	c_lights_switch::param_init_pt()
{
	INT32	h = 0;

	for( INT32 i = 0; i < c_lights::LIGHT_NB; ++i )
	{
		param_attach_obj_no_inc( h, _p_lights ? _p_lights->get_light( i ) : nullptr );
		param_set_pt( h, _b_on[i] );
	}
	err_param_init_pt( h );
}

void	c_lights_switch::build_sum_up( o_str& o )
{
	o.erase();
	for( INT32 i = 0; i < c_lights::LIGHT_NB; ++i )
	{
		if( _b_on[i] )
			o.add( i );
	}
}

CONSTRUCTOR_CREATE( c_lights_switch )
{
	param_init_with( n_lights_switch::param, n_lights_switch::PARAM_NB_MAX ); // lights_switch_param, LIGHTS_SWITCH_PARAM_NB_MAX);

	if( is_obj_first() )
	{
		ubo_light_nb		= gl::ubo::make( sizeof(st_light_nb), true, nullptr, "light_nb" );

		ssbo_directional	= gl::ssbo::make( sizeof(c_light::st_directional_light) * c_lights::LIGHT_NB,	true, nullptr,	"light_directional"	);
		ssbo_point			= gl::ssbo::make( sizeof(c_light::st_point_light)		* c_lights::LIGHT_NB,	true, nullptr,	"light_point"		);
		ssbo_spot           = gl::ssbo::make( sizeof(c_light::st_spot_light)        * c_lights::LIGHT_NB,	true, nullptr,	"light_spot"		);
	}
}

c_lights_switch::~c_lights_switch()
{

	if( is_obj_first() )
	{
		gl::ssbo::release_and_null(	ssbo_spot			);
		gl::ssbo::release_and_null(	ssbo_point			);
		gl::ssbo::release_and_null(	ssbo_directional	);

		gl::ubo::release_and_null(	ubo_light_nb		);
	}

	if( cur == this )
		cur = nullptr;
	if( ui == this )
		ui = nullptr;
}

void	c_lights_switch::update()
{
	_p_lights = c_lights::get_cur();
	
	c_light *		p_light;
	bool			b_wanted;
	st_light_nb		count{ 0, 0, 0 };
	char* pt;

// write directional lights ssbo.
	pt = (char*)ssbo_directional->map_write();
		for( INT32 i = 0; i < c_lights::LIGHT_NB; ++i )
		{
			SPY_PUSH_RANGE("ssbo_directional", spy::GOL_HIGH);
			p_light  = _p_lights->get_light(i); 
			b_wanted = _b_on[i] && p_light->is_active();
		
			GOL::set_light( i, b_wanted );

			if( b_wanted && (p_light->get_type() == c_light::SUN) )
			{
				pt += p_light->set_light_for_ssbo(pt);
				++count._directional_nb;
			}
			SPY_POP_RANGE();
		}
	ssbo_directional->unmap();
	ssbo_directional->bind( gl::ssbo_binding_index_directional_light );


// write point light ssbo.
	pt = (char*)ssbo_point->map_write();
		for( INT32 i = 0; i < c_lights::LIGHT_NB; ++i )		{
			p_light  = _p_lights->get_light(i);
			b_wanted = _b_on[i] && p_light->is_active();

			if( b_wanted && (p_light->get_type() == c_light::BULB) )
			{
				pt += p_light->set_light_for_ssbo(pt);
				++count._point_nb;
			}
		}
	ssbo_point->unmap();
	ssbo_point->bind( gl::ssbo_binding_index_point_light );


// write spot light ssbo.
	pt = (char*)ssbo_spot->map_write();
		for (INT32 i = 0; i < c_lights::LIGHT_NB; ++i)
		{
			p_light = _p_lights->get_light(i);
			b_wanted = _b_on[i] && p_light->is_active();

			if( b_wanted && (p_light->get_type() == c_light::SPOT) )
			{
				//TODO : set spot light
				pt += p_light->set_light_for_ssbo(pt);
				++count._spot_nb;
			}
		}
	ssbo_spot->unmap();
	ssbo_spot->bind( gl::ssbo_binding_index_spot_light );


	ubo_light_nb->bind_and_write( gl::ubo_binding_index_light_count, &count, sizeof(st_light_nb) );

	cur = this;
}

namespace {
	CHAR* 	print_state( CHAR* str, bool b )
	{
		sprintf(str, b ? "ON  " : "OFF " );
		str += 4;
		return str;
	}
}

void	c_lights_switch::dump_on()
{
	CHAR	str_tmp[128];
	CHAR	str_header[128];
	CHAR	str_light[128];
	CHAR	str_switch[128];
	CHAR*	pt_light;
	CHAR*	pt_switch;
	INT32	start = _p_lights->get_start();
	INT32	stop = _p_lights->get_stop();

//todo	had a header to printf
//todo	should handle a variable number of light as the rest in this file
	strcpy( str_header, "LIGHT :\t" );

	strcpy( str_light, "Light\t" );
	pt_light = str_light + strlen( str_light );

	strcpy( str_switch, "Switch\t" );
	pt_switch = str_switch + strlen( str_switch );

	for( INT32 i = start; i <= stop; ++i )
	{
		sprintf( str_tmp, "%d\t", i );
		strcat( str_header, str_tmp );

		pt_light = print_state( pt_light, _p_lights->get_light(i)->is_active() );

		pt_switch = print_state( pt_switch, _b_on[i] );
	}

	SWITCH_PRINT_STRING( str_header );
	SWITCH_PRINT_STRING( str_light );
	SWITCH_PRINT_STRING( str_switch );
}

void	c_lights_switch::flip( INT32 index )
{
	IF_THIS_NULL_RETURN();
	_b_on[index] = !_b_on[index];
	dump_on();
}

c_lights_switch*	c_lights_switch::cur = nullptr;
c_lights_switch*	c_lights_switch::ui = nullptr;

c_lights*	c_lights::def;
c_lights*	c_lights::cur = nullptr;
c_lights*	c_lights::ui = nullptr;

void c_lights::c_init()
{
	::obj_get( def );
	set_cur( def );
	set_ui( def );
}

void c_lights::c_deinit()
{
	SAFE_DELETE( def );
	set_cur_null();
	set_ui_null();
}
	