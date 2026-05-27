
#ifndef	AAA_SENSOR_6DOF_H
#	include "sensor_6dof.h"
#endif
#include "draw/seedcam.h"
#include "math/gainbias.h"
#include "ui/strsymbo.h"
#include "infrastructure/flux_filter.h"
#include "gol/gol_matrix.h"
#include "infrastructure/param/param_declare.h"

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/matrix_decompose.hpp>
//#include <glm/gtc/quaternion.hpp> 


FACTORY_CREATE_V1( c_sensor_6dof, sensor_6dof, Sensor 6 Degree Of Freedom, sensor_6dof );

namespace	n_sensor_6dof
{
	CONSTEXPR INT32 BASE_PARAM_NB				= 12;
	CONSTEXPR INT32 MATRIX_PARAM_NB				= 16;
	CONSTEXPR INT32 TRANSLATION_PARAM_NB		= 13;
	CONSTEXPR INT32 ROTATION_PARAM_NB			= 12;
	CONSTEXPR INT32 OUT_TRANSLATION_PARAM_NB	= 6;
	CONSTEXPR INT32 OUT_ROTATION_PARAM_NB		= 7;
	CONSTEXPR INT32 OUT_VALUE_PARAM_NB			= c_sensor_6dof::VALUE_NB;
	CONSTEXPR INT32 OUT_BUTTON_PARAM_NB			= c_sensor_6dof::BUTTON_NB;
	CONSTEXPR INT32 OUT_TOUCH_PARAM_NB			= c_sensor_6dof::BUTTON_NB;
	CONSTEXPR INT32 GROUP_PARAM_NB				= 8;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	MATRIX_PARAM_NB
									+	TRANSLATION_PARAM_NB
									+	ROTATION_PARAM_NB
									+	OUT_TRANSLATION_PARAM_NB
									+	OUT_ROTATION_PARAM_NB
									+	OUT_VALUE_PARAM_NB
									+	OUT_BUTTON_PARAM_NB
									+	OUT_TOUCH_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_LOCKED(	absolute				)
		PARAM_DEF_BOOL_OFF(		camera_influence		)
		PARAM_DEF_BOOL_OFF(		camera_influence_target )
		PARAM_DEF_NONE(			camera_target			)
		PARAM_DEF_BOOL_ON(		camera_influence_tra	)
		PARAM_DEF_BOOL_ON(		camera_influence_rot	)
	
		PARAM_DEF_REAL_ONE(		sensibility				)
		PARAM_DEF_REAL_ONE(		sensibility_tra			)
		PARAM_DEF_REAL_ONE(		sensibility_rot			)

		PARAM_DEF_GROUP_CLOSED( Matrix, MATRIX_PARAM_NB )
			PARAM_DEF_MATRIX_REAL(	m )

		PARAM_DEF_GROUP_CLOSED( Translation,	TRANSLATION_PARAM_NB )	
			PARAM_DEF_POINT_XYZ(	translation_in				)
			PARAM_DEF_SCALE_XYZ(	translation_factor			)
			PARAM_DEF_REAL_ONE(		translation_factor_global	)
			PARAM_DEF_POINT_XYZ(	translation_offset			)
			PARAM_DEF_BOOL_OFF(		translation_offset_trig		)
			PARAM_DEF_BIAS(			translation_bias			)
			PARAM_DEF_REAL(			translation_filter,			1,0,	0,FLUX_FILTER_MAX )

		PARAM_DEF_GROUP_CLOSED( Rotation,		ROTATION_PARAM_NB )
			PARAM_DEF_ROT_YPR(		rotation_in					)
			PARAM_DEF_REAL_ONE(		rotation_factor_pitch		)
			PARAM_DEF_REAL_ONE(		rotation_factor_yaw			)
			PARAM_DEF_REAL_ONE(		rotation_factor_roll		)
			PARAM_DEF_REAL_ONE(		rotation_factor_global		)
			PARAM_DEF_POINT_XYZ(	rotation_offset				)
			PARAM_DEF_BIAS(			rotation_bias				)
			PARAM_DEF_REAL(			rotation_filter,			1,0,	0,FLUX_FILTER_MAX )

		PARAM_DEF_GROUP( OUT_TRANSLATION,		OUT_TRANSLATION_PARAM_NB )
			PARAM_DEF_POINT_XYZ(	translation_speed			)
			PARAM_DEF_POINT_XYZ(	translation					)

		PARAM_DEF_GROUP( OUT_ROTATION,			OUT_ROTATION_PARAM_NB )
			PARAM_DEF_SYMBO(		rotation_order,				0,aaa::matrix::ORDER_ZYX,	5,gstr::rot_order	)
			PARAM_DEF_POINT_XYZ(	rotation_speed				)
			PARAM_DEF_POINT_XYZ(	rotation					)

		PARAM_DEF_BOOL_OFF(		vibrate		)
		PARAM_DEF_INT32(		vibrate_len,	100,1000,		0,3999	)
	
		PARAM_DEF_BOOL_LOCKED(	button_changed )
		PARAM_DEF_GROUP( OUT_VALUE, OUT_VALUE_PARAM_NB )
			PARAM_DEF_8( value, PARAM_DEF_REAL_LOCKED )

		PARAM_DEF_GROUP( OUT_BUTTON, OUT_BUTTON_PARAM_NB )	
			PARAM_DEF_8( button, PARAM_DEF_BOOL_LOCKED )

		PARAM_DEF_GROUP( OUT_TOUCH, OUT_TOUCH_PARAM_NB )	
			PARAM_DEF_8( touch, PARAM_DEF_BOOL_LOCKED )

	};
}

void	c_sensor_6dof::param_init_pt()
{
	INT32	h = 0;
//	param_set_pt(		h, b_verbose						);	
	param_set_pt(		h, _b_absolute						);
	param_set_pt(		h, _b_camera_influence_ui			);
	param_set_pt(		h, _b_camera_influence_target_ui	);
	param_attach_obj(	h, _cam_target						);
	param_set_pt(		h, _b_camera_influence_tra_ui		);
	param_set_pt(		h, _b_camera_influence_rot_ui		);

	param_set_pt(		h, _sensibility_ui					);
	param_set_pt(		h, _sensibility_tra_ui				);
	param_set_pt(		h, _sensibility_rot_ui				);

	++h;
		param_set_pt_n(	h, _mat, 16 );

	++h;
		param_set_pt_3( h, &_data_in[0] );
		param_set_pt_4( h, _tra_factor );
		param_set_pt_3( h, _tra_offset );
		param_set_pt( h, _b_tra_offset_trig_ui );
		param_set_pt( h, _tra_bias );
		param_set_pt( h, _tra_filter );

	++h;
		param_set_pt_3( h, &_data_in[3] );
		param_set_pt_4( h, _rot_factor );
		param_set_pt_3( h, _rot_offset );
		param_set_pt( h, _rot_bias );
		param_set_pt( h, _rot_filter );

	++h;
		param_set_pt_3( h, _tra_speed );
		param_set_pt_3( h, _tra );

	++h;
		param_set_pt( h, _s_rotation_order );
		param_set_pt_3( h, _rot_speed );
		param_set_pt_3( h, _rot );

	param_set_pt( h, _b_vibrate_ui			);
	param_set_pt( h, _vibrate_time		);

	param_set_pt( h, _b_button_change_out	);
	++h;
		for( INT32 i = 0; i < VALUE_NB; ++i )
			param_set_pt( h, _value_out[i] );
	++h;
		for( INT32 i = 0; i < BUTTON_NB; ++i )
			param_set_pt( h, _b_pressed_out[i] );
	++h;
		for( INT32 i = 0; i < BUTTON_NB; ++i )
			param_set_pt( h, _b_touched_out[i] );

	err_param_init_pt( h );
}

void	c_sensor_6dof::init()
{
	_b_absolute = false;
	for( INT32 i = 0; i < CHANNEL_NB; ++i )
	{
		_sensor->control_put( i + 1, 0. );
		_data_in[i] = 0.;
	}
	for( INT32 i = 0; i < VALUE_NB; ++i )
		_value_out[i] = 0.;

	for( INT32 i = 0; i < BUTTON_NB; ++i )
	{
		_b_pressed_out[i] = false;
		_b_touched_out[i] = false;
	}

}

//c_sensor_6dof::c_sensor_6dof() : c_sensor(CHANNEL_NB)
CONSTRUCTOR_CREATE( c_sensor_6dof )
,	_cam_target			{ nullptr }
,	_b_button_change_out{ false }
{
	_sensor = new c_sensor(CHANNEL_NB);
	_s_rotation_order = aaa::matrix::ORDER_ZYX;	//todo this is here and not in init() to avoid crash in param_printui

	param_init_with( n_sensor_6dof::param, n_sensor_6dof::PARAM_NB_MAX ); // sensor_6dof_param, SENSOR_6DOF_PARAM_NB);
	init();
}

c_sensor_6dof::~c_sensor_6dof()
{
	// todofranz dealloc
}

void c_sensor_6dof::update_sensor_6dof()
{
//	printf( "update low\n" );
// button flip only when entry flip
	c_seedcam* cam = nullptr;
	if( _b_camera_influence_ui )
		cam = _b_camera_influence_target_ui ? _cam_target : c_seedcam::get_ui_or_find_unlock_silent();
	
	for( INT32 i = 0; i < BUTTON_NB; ++i )
	{
		//bool b = _data_in[6+i]==1.;
		//if( _b_but[i] != b )
		{
			//_b_but[i] = b;
			if( _b_pressed_out[i] && cam )
			{
				if( i==0 )
					cam->reset();
				else
					cam->flip_flying();
			}
		}
		//_sensor->control_put( 7+i, _data_in[6+i] );
	}
	//_sensor->control_put( 9, _data_in[8]);

	if( _sensor->b_verbose )
	{//
		DBG_PRINT_STRING( "%f %f %f %f %f %f",
				_data_in[0], 
				_data_in[1], 
				_data_in[2], 
				_data_in[3], 
				_data_in[4], 
				_data_in[5] );
	}

	//deal with time	
	_delta_t.update();
	REAL dt = REAL(_delta_t.get_dt());

	REAL	vec[3];
//TRANSLATION
	_sensor->control_set_filter_factor( 1, _tra_filter ); 
	_sensor->control_set_filter_factor( 2, _tra_filter) ; 
	_sensor->control_set_filter_factor( 3, _tra_filter ); 

	REAL	tra = _tra_factor[3] * _sensibility_tra_ui * _sensibility_ui;

	if( _b_absolute )
		for( INT32 i=0; i<3; ++i )
		{	//todo use bias ?
			vec[i] = _data_in[i];
		}
	else
		for( INT32 i=0; i<3; ++i )
		{
			REAL tmp = ABS( _data_in[i] );
			bias_slick( tmp, _tra_bias );
			vec[i] = (_data_in[i]<0) ? -tmp : tmp;
		}

	//	set zero pos
	if( _b_tra_offset_trig_ui )
	{
		mul_scale_v3( _tra_offset, vec, _tra_factor, -tra );
		_b_tra_offset_trig_ui = false;
	}

	_sensor->control_put( 1, vec[0] * _tra_factor[0] * tra + _tra_offset[0] );
	_sensor->control_put( 2, vec[1] * _tra_factor[1] * tra + _tra_offset[1] );
	_sensor->control_put( 3, vec[2] * _tra_factor[2] * tra + _tra_offset[2] );

	if( _b_absolute )
	{
		set_v3( _tra, _sensor->control_get(1), _sensor->control_get(2), _sensor->control_get(3) );
		if( dt != 0. )
			scale_v3( _tra_speed, _tra, 1./dt );
	}
	else
	{
		set_v3( _tra_speed, _sensor->control_get(1), _sensor->control_get(2), _sensor->control_get(3) );
		scale_v3( _tra, _tra_speed, dt );
	}

//	speed = sqrt( tra[0]*tra[0] + tra[1]*tra[1] ) /dt;
//	printf( " dt = %f\n", dt );
//ROTATION
	REAL	rot = _rot_factor[3] * _sensibility_rot_ui * _sensibility_ui;

	_sensor->control_set_filter_factor( 4, _rot_filter); 
	_sensor->control_set_filter_factor( 5, _rot_filter); 
	_sensor->control_set_filter_factor( 6, _rot_filter); 
	if( _b_absolute )
		for( INT32 i=3; i<6; ++i )
		{	//todo use bias ?
			vec[i-3] = _data_in[i];		//hack ? i-3
		}
	else
		for( INT32 i=3; i<6; ++i )
		{
			REAL tmp = ABS( _data_in[i] );
			bias_slick( tmp, _rot_bias );
			vec[i-3] = (_data_in[i]<0) ? -tmp : tmp;
		}

	_sensor->control_put( 4, vec[0] * _rot_factor[0] * rot + _rot_offset[0] );
	_sensor->control_put( 5, vec[1] * _rot_factor[1] * rot + _rot_offset[1] );
	_sensor->control_put( 6, vec[2] * _rot_factor[2] * rot + _rot_offset[2] );

	if( _b_absolute )
	{
		set_v3( _rot, _sensor->control_get(4), _sensor->control_get(5), _sensor->control_get(6) );
		if( dt != 0. )
			scale_v3( _rot_speed, _rot, 1./dt);
	}
	else
	{
		set_v3( _rot_speed, _sensor->control_get(4), _sensor->control_get(5), _sensor->control_get(6) );
		scale_v3( _rot, _rot_speed, dt);
	}

	//todo make it better
	if( _sensor->b_verbose )
	{
		DBG_PRINT_STRING( "%f %f %f %f %f %f",
				_sensor->control_get(1),
				_sensor->control_get(2),
				_sensor->control_get(3),
				_sensor->control_get(4),
				_sensor->control_get(5),
				_sensor->control_get(6)	);
	}

	if( cam )
	{
		if( _b_absolute )
		{
			//vec[2] = -_rot[0];
			//vec[1] = -_rot[1];
			//vec[0] = _rot[2];
			//cam->set_euler( &vec[0] );
			REAL vec[3];
			if( _b_camera_influence_rot_ui )
			{
				neg_v3( vec, _rot );
				cam->set_euler( vec );
				cam->set_euler_order( _s_rotation_order );
				cam->set_rot( zero_v4fp32 );
			}
			if( _b_camera_influence_tra_ui )
			{
				neg_v3( vec, _tra );
				cam->set_tra( vec );
			}
		}
		else
		{
			cam->influence_by_6dof( _b_camera_influence_tra_ui ? _tra : zero_v4fp32, _b_camera_influence_rot_ui ? _rot : zero_v4fp32 );
		}
	}
}

//bool c_sensor_6dof::set_target( INT32 slot, c_obj_ui* target )		//slot start at 1
//{
//	if( target )
//	{
//		if( c_seedcam::is_instance(target) )
//		{
//			_cam_target = (c_seedcam*) target;
//			return true;
//		}
//	}
//	_cam_target = nullptr;
//	return target ==  nullptr;
//}
//
//c_obj_ui* c_sensor_6dof::get_target( INT32 slot ) CONST				//slot start at 1
//{
//	return _cam_target;
//}

glm::vec3 QuatToEulerXYZ( const glm::quat& q )
{
	double ysqr = q.y * q.y;
	double t0 = -2.0f * ( ysqr + q.z * q.z ) + 1.0f;
	double t1 = +2.0f * ( q.x * q.y - q.w * q.z );
	double t2 = ( q.x * q.z + q.w * q.y );
	double t3 = +2.0f * ( q.y * q.z - q.w * q.x );
	double t4 = -2.0f * ( q.x * q.x + ysqr ) + 1.0f;

	t2 = t2>0.5f ? 0.5f : ( t2<-0.5f ? -0.5f : -2.0f * t2 );

	auto pitch = std::asin( t2 );
	auto roll = std::atan2( t3, t4 );
	auto yaw = std::atan2( t1, t0 );
	return glm::vec3( yaw, pitch, roll );
}

void c_sensor_6dof::set_matrix_in( CONST float* matrix )
{
	cpy_v16( _mat, matrix );

	glm::vec3	sca;
	glm::quat	orientation;
	glm::vec3	tra;
	glm::vec3	skew;
	glm::vec4	perspective;

	glm::decompose(	(glm::mat4x4&)*matrix, sca, orientation, tra, skew, perspective );

	set_tra_in( tra.x, tra.y, tra.z );

	//pitch yaw roll
	glm::vec3	rot = glm::eulerAngles( orientation ) * REAL(PI_TIME_2_OVER_1) ;
	set_rot_in( -rot.x, -rot.y, -rot.z );
}