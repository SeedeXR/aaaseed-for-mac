#include "light.h"
#include "lights.h"
#include "time/aaa_time.h"
#include "draw/rect.h"
#include "infrastructure/param/param_declare.h"
#include "gol/gol.h"
#include "ui/alphabet.h"


FACTORY_CREATE_V1( c_light, light, Light, light );


CONSTEXPR	C_PCHAR_C	str_attenuation[5] =
{
	"OFF",
	"ALL",
	"CONSTANT",
	"LINEAR",
	"QUADRATIC",
};

CONSTEXPR	C_PCHAR_C	str_type[c_light::TYPE_MAX_NB] =
{
	"SUN",
	"BULB",
	"SPOT"
};

	#define LIGHTMAP_MODE_INTENSITY_MULTIPLY 0
	#define LIGHTMAP_MODE_INTENSITY_ADD 1
	#define LIGHTMAP_MODE_PURE_ADD 2
	#define LIGHTMAP_MODE_PURE_ADD_DIFFUSE 3

static CONSTEXPR C_PCHAR_C	intensity_map_mode_str[4] =
{
	"MULTIPLY",
	"ADD",
	"PURE_ADD",
	"PURE_ADD_DIFFUSE",
};

namespace	n_light
{
	CONSTEXPR	INT32	BASE_PARAM_NB			=	7;
	CONSTEXPR	INT32	MODERN_PARAM_NB			=	3;
	CONSTEXPR	INT32	SHADOW_PARAM_NB			=	11;
	CONSTEXPR	INT32	INTENSITY_MAP_PARAM_NB	=	7;
	CONSTEXPR	INT32	COLOR_PARAM_NB			=	16;
	CONSTEXPR	INT32	SPOT_PARAM_NB			=	5;
	CONSTEXPR	INT32	ATTE_PARAM_NB			=	4;
	CONSTEXPR	INT32	GROUP_PARAM_NB			=	7;
	CONSTEXPR	INT32	PARAM_NB_MAX		=	BASE_PARAM_NB
												+	MODERN_PARAM_NB
												+	SHADOW_PARAM_NB
												+	INTENSITY_MAP_PARAM_NB
												+	COLOR_PARAM_NB
												+	ATTE_PARAM_NB
												+	SPOT_PARAM_NB
												+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_GROUP(	Base, BASE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			ON			)	//todo should be active not ON
			PARAM_DEF_FP32_ONE(			Intensity	)
			PARAM_DEF_SYMBO_PSTR_ZERO(	type,		str_type )
			PARAM_DEF_BOOL_OFF(			camera_lock )
			PARAM_DEF_POINT_FP32_XYZ(	Position	)

		PARAM_DEF_GROUP_CLOSED( Modern, MODERN_PARAM_NB )
			PARAM_DEF_FP32_POS(		Radius,				1, 10.		)
			PARAM_DEF_FP32_POS_ONE(	Ambient_occlusion	)
			PARAM_DEF_FP32_POS_ONE(	Gamma				)

		PARAM_DEF_GROUP_CLOSED( Shadow, SHADOW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			shadow_active			)
			PARAM_DEF_UINT32_XY(		shadow_size,			512, 256,	1, PARAM_MAX_UINT32	)
			PARAM_DEF_FP32_ONE(			shadow_filter_width		)	
			PARAM_DEF_FP32_ZERO(		shadow_bias				)	
			PARAM_DEF_POINT_FP32_XYZ(	shadow_bbox_min			)
			PARAM_DEF_POINT_FP32_XYZ(	shadow_bbox_max			)

		PARAM_DEF_GROUP_CLOSED( Intensity Map, INTENSITY_MAP_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			intensity_map_active	)
			PARAM_DEF_SYMBO_PSTR_ZERO(	intensity_map_mode,		intensity_map_mode_str )
			PARAM_DEF_FP32_ONE(			intensity_map_factor	)
			PARAM_DEF_SCALE_FP32_XY(	intensity_map_scale		)
			PARAM_DEF_POINT_FP32_XY(	intensity_map_offset	)

		PARAM_DEF_GROUP_CLOSED( Color, COLOR_PARAM_NB )
			// todo why the Caps for Red Green...
			PARAM_DEF_COLOR_RGBG_BLACK(	Ambient )
			PARAM_DEF_COLOR_RGBG_BLACK(	Diffuse )
			PARAM_DEF_COLOR_RGBG_BLACK(	Specular )

			PARAM_DEF_GROUP_CLOSED(	Ignored by OpenGl, 3 )	//	but not by shader
				PARAM_DEF_FP32_ONE(	Ambient_Alpha	)
				PARAM_DEF_FP32_ONE(	Diffuse_Alpha	)
				PARAM_DEF_FP32_ONE(	Specular_Alpha	)

		PARAM_DEF_GROUP_CLOSED( Spot, SPOT_PARAM_NB )
			PARAM_DEF_FP32(				Spot_cutoff,			0., 45.,	0., 90. )
			PARAM_DEF_FP32_ZERO(		Spot_exponent			)
			PARAM_DEF_POINT_FP32_XYZ(	Spot_direction			)

		PARAM_DEF_GROUP_CLOSED( Attenuation, ATTE_PARAM_NB )
			PARAM_DEF_SYMBO(			attenuation_type,		4, 0,		4, str_attenuation )
			PARAM_DEF_FP32_POS_ONE(		attenuation_constant	)
			PARAM_DEF_FP32_POS_ZERO(	attenuation_linear		)
			PARAM_DEF_FP32_POS_ZERO(	attenuation_quadratic	)
	};
}


void	c_light::param_init_pt()
{
	INT32	h = 0;

	++h;
		param_set_pt(	h, get_pt_active()		);
		param_set_pt(	h, _intensity			);
		param_set_pt(	h, _modern._s_type		);
		param_set_pt(	h, _b_camera_lock		);
		param_set_pt_3(	h, _modern._position	);
	++h;
		param_set_pt(	h, _modern._radius			);
		param_set_pt(	h, _modern._ao_factor		);
		param_set_pt(	h, _modern._gamma			);
	++h;
		param_set_pt(	h, _modern._b_shadow_cast		);
		param_set_pt_2(	h, _modern._shadow_map_size		);
		param_set_pt(	h, _modern._shadow_filter_width	);
		param_set_pt(	h, _modern._shadow_bias			);
		param_set_pt_3(	h, _modern._shadow_bbox_min		);
		param_set_pt_3(	h, _modern._shadow_bbox_max		);
	++h;
		param_set_pt(	h, _modern._b_intensity_map			);
		param_set_pt(	h, _modern._s_intensity_map			);
		param_set_pt(	h, _modern._intensity_map_factor	);
		param_set_pt_2(	h, _modern._intensity_map_scale		);
		param_set_pt_2(	h, _modern._intensity_map_offset	);
	++h;
		param_set_pt_3(	h, _ambient_ui		);
		param_set_pt(	h, _ambient_ui[4]	);
		param_set_pt_3(	h, _diffuse_ui		);
		param_set_pt(	h, _diffuse_ui[4]	);
		param_set_pt_3(	h, _specular_ui		);
		param_set_pt(	h, _specular_ui[4]	);
		++h;
			param_set_pt( h, _ambient_ui[3]		);
			param_set_pt( h, _diffuse_ui[3]		);
			param_set_pt( h, _specular_ui[3]	);
	++h;
		param_set_pt(	h, _spot_cutoff		);
		param_set_pt(	h, _spot_exponent	);
		param_set_pt_3(	h, _spot_direction	);
	++h;
		param_set_pt(	h, _attenuation_type		);
		param_set_pt_3(	h, _attenuation_factor_ui	);

	err_param_init_pt(h);
}

void	c_light::set_index( INT32 index_in )
{
	_index = index_in;
	_gl_name = GOL::light_gl_name[index_in];
}

CONSTRUCTOR_CREATE( c_light )
{
	clear_v4( _ambient );
	clear_v4( _modern._color );
	clear_v4( _specular );
	init_name_with( "Light" );
	param_init_with( n_light::param, n_light::PARAM_NB_MAX ); // light_param, LIGHT_PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_light )

void	c_light::draw_sum_up()
{
	//	n_axe::draw_axe_3D();
	//GOL::color_black4();
	//draw_rect( -.1, -.1, 3.1, 1.1 );
	//draw();

	FP32	vec[3];

	scale_v3( vec, _ambient_ui, _ambient_ui[4] );
	GOL::color3v( vec );
	draw_rect( 0,0, 1,1 );

	scale_v3( vec, _diffuse_ui, _diffuse_ui[4] );
	GOL::color3v( vec );
	draw_rect( 1,0, 2,1 );

	scale_v3( vec, _specular_ui, _specular_ui[4] );
	GOL::color3v( vec );
	draw_rect( 2,0, 3,1 );
}

void	c_light::update()
{
	//hack	all lights_cur here are a hack
	c_lights* lights_cur = c_lights::get_cur();
	FP32 f = lights_cur->get_intensity_factor() * _intensity;
	if( lights_cur->get_psy() == 1 )
		f *= ( SIN_TURN(lights_cur->get_phase(_index)) + REAL(.75) ) * REAL(.5);

	scale_v3_cpy_v4( _ambient,			_ambient_ui,	f * _ambient_ui[4] );
	GOL::lightv( _gl_name, GL_AMBIENT, _ambient );
	
	scale_v3_cpy_v4( _modern._color,	_diffuse_ui,	f * _diffuse_ui[4] );
	GOL::lightv( _gl_name, GL_DIFFUSE, _modern._color );
	
	scale_v3_cpy_v4( _specular,			_specular_ui,	f * _specular_ui[4] );
	GOL::lightv( _gl_name, GL_SPECULAR, _specular );

	if( lights_cur->get_psy()==2 && lights_cur->get_stop()!=0 )
	{
		//	was	GOL::light( gl_index, GL_SPOT_EXPONENT, .5 );
		GOL::light( _gl_name, GL_SPOT_EXPONENT,	80. );
		GOL::light( _gl_name, GL_SPOT_CUTOFF,	90. );
	}
	else if( _modern._s_type == SPOT )
	{
//nvdia		GOL::light( gl_index, GL_SPOT_EXPONENT, GLfloat(_spot_exponent*(128.f*6.36666f)) );
// on ATI it generate glerror, on Nvidia it work fine and extend strings ???
		GOL::light( _gl_name, GL_SPOT_EXPONENT,	GLfloat( CLAMP_01( _spot_exponent ) * (128.f) ) );
		GOL::light( _gl_name, GL_SPOT_CUTOFF,	_spot_cutoff );
	}
	else
		GOL::light( _gl_name, GL_SPOT_CUTOFF,	180. );

	FP32 val[3];
	if( lights_cur->is_atte() )
	{
		if( _attenuation_type == 1 )	// ALL
			cpy_v3( val, _attenuation_factor_ui );
		else
		{
			val[0] = (_attenuation_type == 2) ? _attenuation_factor_ui[0] : FP32(1);
			val[1] = (_attenuation_type == 3) ? _attenuation_factor_ui[1] : FP32(0);
			val[2] = (_attenuation_type == 4) ? _attenuation_factor_ui[2] : FP32(0);
		}
	}
	else
		set_v3( val, 1, 0, 0 );

	GOL::light( _gl_name, GL_CONSTANT_ATTENUATION,	val[0] );
	GOL::light( _gl_name, GL_LINEAR_ATTENUATION,	val[1] );
	GOL::light( _gl_name, GL_QUADRATIC_ATTENUATION, val[2] );

//	if( b_active )
//		GOL::enable( _gl_name );
//	else
//		GOL::disable( _gl_name );
}

c_light::st_light_modern CONST * c_light::get_modern()	CONST
{
	return &_modern;
}
//void	c_light::force_enable()				{	GOL::enable_light( _index );	}
//void	c_light::force_disable()			{	GOL::disable_light( _index );	}

void	c_light::set_spot( FP32 CONST exponent, FP32 CONST cutoff, INT32 CONST atte_type, FP32 CONST atte_value )
{
	_spot_exponent = exponent;
	_spot_cutoff = cutoff;
	_attenuation_type = atte_type;
	if( _attenuation_type )
		_attenuation_factor_ui[ _attenuation_type - 1 ] = atte_value;
}

void	c_light::lock_to_camera()			{	_b_camera_lock = LIGHT_CAMERA;	}
void	c_light::lock_to_world()			{	_b_camera_lock = LIGHT_WORLD;	}

//void	c_light::set_ambient_4f(		FP32 r, FP32 g, FP32 b, FP32 a )	{	set_v4( _ambient,  r,g,b, a );		}
//void	c_light::set_diffuse_4f(		FP32 r, FP32 g, FP32 b, FP32 a )	{	set_v4( _diffuse,  r,g,b, a );		}
//void	c_light::set_specular_4f(		FP32 r, FP32 g, FP32 b, FP32 a )	{	set_v4( _specular, r,g,b, a );		}
//void	c_light::set_position_4f(		REAL x, REAL y, REAL z, REAL w )	{	set_v4( _position, x,y,z, w );		}
//void	c_light::set_spot_direction_3f(	REAL x, REAL y, REAL z )			{	set_v3( _spot_direction, x,y,z );	}

//todomatrix 
FINLINE void	c_light::set_pos_low( glm::mat4 CONST * CONST mat_view_inverse, FP32 CONST * CONST pos, FP32 CONST * CONST dir  )
{
	if( mat_view_inverse ) // light have to be set in world space
	{	// coor will be multiply by model view gl matrix so we have to go to world coor
		glm::vec4 v4 = *mat_view_inverse * *(glm::vec4*)pos;
		GOL::lightv( _gl_name, GL_POSITION, (FP32*)&v4 );
		if( dir )
		{
			v4 = *mat_view_inverse * glm::vec4( dir[0],dir[1],dir[2], 0. );
			GOL::lightv( _gl_name, GL_SPOT_DIRECTION, (FP32*)&v4 );	
		}
	}
	else
	{
		GOL::lightv( _gl_name, GL_POSITION, pos );
		if( dir )
			GOL::lightv( _gl_name, GL_SPOT_DIRECTION, dir );
	}
}



void	c_light::set_pos( glm::mat4 CONST * CONST mat_view_inverse )
{
//	_mat_view_inverse_store = *mat_view_inverse;
	_modern._position[3] = ( _modern._s_type != SUN ) ? FP32(1) : FP32(0) ;
	set_pos_low( is_camera_lock() ? mat_view_inverse : nullptr, _modern._position, _modern._s_type == SPOT ? _spot_direction : nullptr );
}

void	c_light::set_pos_psy( glm::mat4 CONST * CONST mat_view_inverse )
{
//	_mat_view_inverse_store = *mat_view_inverse;

	FP32	pos[4];

	FP32	t = FP32(aaa::time::get());
	FP32	phase = FP32(_index + 1) * t * FP32(.002);

	pos[0] = 4 * COS_TURN( phase * ( 8. - _index ) );
	pos[1] = REAL(8) + REAL(4) * COS_TURN( phase * ( FP32(2) + COS_TURN( _index * .1 ) ) );
	pos[2] = 4 * SIN_TURN( phase * ( _index + 1. ) );
	pos[3] = 1.;

	set_pos_low( nullptr, pos, unit_y_neg_v4fp32 );
}

//todo finish it: show type of light, direction
//void	c_light::draw()
//{
//	GOL::push_att();
//		GOL::push_polygon_mode( GL_FRONT_AND_BACK, GL_FILL );
//		GOL::push_color();
//		GOL::push_lighting( false );
//		GOL::push_cull();
//
//		GOL::matrix::push();
//			
//			if( is_camera_lock() )
//			{
//				GOL::matrix::mul_matrix( &_mat_view_inverse_store );	//did not function did not want to complexify
//			}
//			GOL::matrix::translate3v( _modern._position );
//
//			GOL::color3v( _modern._color );
//			draw_box( .2, .2, .2 );
//
//			GOL::color_white4();
//			GOL::matrix::translate_x( .2 );
//			//GOL::matrix::scale( 3. );
//			aaa::alphabet::draw_int32( get_index(), 0 );
//
//			//GOL::matrix::load_identity();
//			//n_axe::draw_null_3d( _target, 0.5 );
//		GOL::matrix::pop();
//		
//	GOL::pop_att();
//}
//-----------------------------------------------------------------------------

INT32 c_light::set_light_directional( st_directional_light * CONST dst ) CONST NOEXCEPT
{
	//shader want a normalized direction
	dst->_view_projection			=		_modern._shadow_view_projection;
	normalize_v3fp32(	dst->_direction,	_modern._position						);
	neg_v3(	dst->_direction	);
	cpy_v3(	dst->_color,					_modern._color							);
	dst->_gamma						=		_modern._gamma							;
//	dst->_ao_factor					=		_modern._ao_factor						;

	dst->_b_shadow					=		_modern._b_shadow_cast					;
	dst->_shadow_map_size_over[0]	=		OVER_ONE_AS_FP32( _modern._shadow_map_size[0]	);
	dst->_shadow_map_size_over[1]	=		OVER_ONE_AS_FP32( _modern._shadow_map_size[1]	);
	dst->_shadow_filter_width		=		_modern._shadow_filter_width			;
	dst->_shadow_bias				=		_modern._shadow_bias					;

	dst->_b_intensity_map			=		_modern._b_intensity_map				;
	dst->_intensity_map_mode		=		_modern._s_intensity_map				;
	dst->_intensity_map_factor		=		_modern._intensity_map_factor			;
	cpy_v2(	dst->_intensity_map_scale,		_modern._intensity_map_scale			);	
	cpy_v2(	dst->_intensity_map_offset,		_modern._intensity_map_offset			);
	
	return sizeof(st_directional_light);
}

INT32 c_light::set_light_spot( st_spot_light * CONST dst ) CONST NOEXCEPT
{
	dst->_view_projection			=		_modern._shadow_view_projection;
	cpy_v3(	dst->_position,					_modern._position						);
	normalize_v3fp32(	dst->_direction,	_spot_direction							);

	dst->_field_of_view				=		_spot_exponent							;
	dst->_field_of_view_decay		=		_spot_cutoff							;

	cpy_v3(	dst->_color,					_modern._color							);
	dst->_gamma						=		_modern._gamma							;
//	dst->_ao_factor					=		_modern._ao_factor						;

	dst->_b_shadow					=		_modern._b_shadow_cast					;
	dst->_shadow_map_size_over[0]	=		OVER_ONE_AS_FP32( _modern._shadow_map_size[0]	);
	dst->_shadow_map_size_over[1]	=		OVER_ONE_AS_FP32( _modern._shadow_map_size[1]	);
	dst->_shadow_filter_width		=		_modern._shadow_filter_width			;
	dst->_shadow_bias				=		_modern._shadow_bias					;

	dst->_b_intensity_map			=		_modern._b_intensity_map				;
	dst->_intensity_map_mode		=		_modern._s_intensity_map				;
	dst->_intensity_map_factor		=		_modern._intensity_map_factor			;
	cpy_v2(	dst->_intensity_map_scale,		_modern._intensity_map_scale			);	
	cpy_v2(	dst->_intensity_map_offset,		_modern._intensity_map_offset			);

	return sizeof(st_spot_light);
}

INT32 c_light::set_light_point( st_point_light * CONST dst ) CONST NOEXCEPT
{
	cpy_v3(	dst->_position,	_modern._position );
	cpy_v3(	dst->_color,	_modern._color );
	dst->_radius		=	_modern._radius;
	dst->_gamma			=	_modern._gamma;
	//dst->_ao_factor	=	_modern._ao_factor;

	return sizeof(st_point_light);
}

INT32 c_light::set_light_for_ssbo( void * CONST dst ) CONST NOEXCEPT
{
	if( !dst )
	{
		debug_break( "%s() can't deal with a Null dst", __FUNCTION__ );
		return 0;
	}
	INT32 CONST type = get_type();
	switch( type )
	{
	case SUN:	return set_light_directional(	(st_directional_light *)	dst ); 
	case BULB:	return set_light_point(			(st_point_light *)			dst ); 
	case SPOT:	return set_light_spot(          (st_spot_light *)           dst );
	default:
		err_print( "Unhandled light type %d.", type );
		break;
	}
	return 0;
}

//-----------------------------------------------------------------------------

//void c_light::build_cam_matrix_for_shadow( glm::mat4 * mat )
//{
//	//todo avoid duplication of code
//	FP32 dir[3];
//	normalize_v3fp32(	dir,	_modern._position );
//	neg_v3(	dir	);
//	aaa::matrix::lookat_bounding_box_min_max( mat, dir, _modern._shadow_bbox_min, _modern._shadow_bbox_max );
//}

void c_light::set_matrix_view_projection(  glm::mat4 CONST * CONST mat, glm::vec4 CONST * CONST position, glm::vec4 CONST * CONST direction )
{
	_modern._shadow_view_projection = *mat;
	if( get_type()==SPOT )
	{
		cpy_v3( _modern._position, &(position->x) );
		cpy_v3( _spot_direction, &(direction->x) );
	}
}

c_light* c_light::get_light( INT32 index )
{
	c_lights* lights_cur = c_lights::get_cur();
	if( lights_cur )
	{
		if( INSIDE_MIN_MAX( index, 0, c_lights::LIGHT_NB-1 ) )
			return lights_cur->get_light(index);
	}
	return nullptr;
}

