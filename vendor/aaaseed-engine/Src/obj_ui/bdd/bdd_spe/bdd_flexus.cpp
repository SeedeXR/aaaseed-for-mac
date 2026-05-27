#include "bdd_flexus.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/bdd/bdd_geo/bdd_tube_path.h"

#include "obj_ui/deformer/def_node.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/layer/layers.h"
#include "draw/model.h"
#include "draw/axe.h"


FACTORY_CREATE_PROP_V1( c_bdd_flexus, bdd_flexus, flexus navigation , bdd_flexus, sub_menu="Special"; );

c_bdd_flexus*	bdd_flexus = nullptr;

namespace	n_bdd_flexus
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 4 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32	IN_PARAM_NB		= 7;
	CONSTEXPR INT32	OUT_PARAM_NB	= 3;
	CONSTEXPR INT32	NAV_IN_NB		= 3;
	CONSTEXPR INT32	NAV_OUT_NB		= 18;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 4;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	IN_PARAM_NB
									+	OUT_PARAM_NB
									+	NAV_IN_NB
									+	NAV_OUT_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(	active )
		PARAM_DEF_BOOL_OFF(	navigation_with_ship )
		PARAM_DEF_BOOL_OFF(	freeze_position )
		PARAM_DEF_INT32(	tube_channel,			2, 1,	1, 1024	)

		PARAM_DEF_GROUP_CLOSED( In, IN_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		restart_trig )
			PARAM_DEF_REAL_ZERO(	forward )
			PARAM_DEF_REAL_ZERO(	left_right )
			PARAM_DEF_REAL(			hmd_yaw,	.5, 0,	-1, 1 )
			PARAM_DEF_REAL(			hmd_pitch,	.5, 0,	-1, 1 )
			PARAM_DEF_REAL(			hmd_roll,	.5, 0,	-1, 1 )
			PARAM_DEF_BOOL_OFF(		camera_deform )

		PARAM_DEF_GROUP_CLOSED( Out, OUT_PARAM_NB )
			PARAM_DEF_REAL_LOCKED(	cam_yaw )
			PARAM_DEF_REAL_LOCKED(	cam_pitch )
			PARAM_DEF_REAL_LOCKED(	cam_roll )

		PARAM_DEF_GROUP_CLOSED( Navigation In, NAV_IN_NB )
			PARAM_DEF_REAL_ZERO(	t_size	)
			PARAM_DEF_REAL_ZERO(	rot_factor )
			PARAM_DEF_REAL_ZERO(	tra_factor )

		PARAM_DEF_GROUP_CLOSED( Navigation Out, NAV_OUT_NB )
			PARAM_DEF_REAL_LOCKED(	s_size )
			PARAM_DEF_REAL_ZERO(	s )
			PARAM_DEF_REAL_ZERO(	s_normalized )
			PARAM_DEF_REAL_ZERO(	y )
			PARAM_DEF_REAL_ZERO(	t )

			PARAM_DEF_REAL_LOCKED(	cam_x )
			PARAM_DEF_REAL_LOCKED(	cam_y )
			PARAM_DEF_REAL_LOCKED(	cam_z )

			PARAM_DEF_REAL_LOCKED(	cam_target_x )
			PARAM_DEF_REAL_LOCKED(	cam_target_y )
			PARAM_DEF_REAL_LOCKED(	cam_target_z )

			PARAM_DEF_REAL_LOCKED(	angle_ship )
			PARAM_DEF_REAL_LOCKED(	angle_camera )
			PARAM_DEF_REAL_LOCKED(	angle_ship_to_tube )
			PARAM_DEF_REAL_LOCKED(	angle_camera_to_tube )
			PARAM_DEF_REAL_LOCKED(	phase_in_front )
			PARAM_DEF_REAL_LOCKED(	pitch )
			PARAM_DEF_REAL_LOCKED(	speed )
	};
}

void	c_bdd_flexus::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_nav_ship );
	param_set_pt( h, _b_freeze_ship_position );
	param_set_pt( h, _tube_channel );

	++h;
		param_set_pt( h, _b_restart_trig_ui );
		param_set_pt( h, _in_forward_speed );
		param_set_pt( h, _in_lr );
		param_set_pt_3( h, _hmd_rot );
		param_set_pt( h, _b_camera_deform );

	++h;
		param_set_pt_3( h, _cam_rot );

	++h;
		param_set_pt( h, _t_size );
		param_set_pt( h, _rot_factor );
		param_set_pt( h, _tra_factor );

	++h;
		param_set_pt( h, _s_size );
		param_set_pt( h, _s );
		param_set_pt_3( h, _coor_in_path );
		param_set_pt_3( h, _cam_pos );
		param_set_pt_3( h, _cam_target );
		param_set_pt( h, _angle_ship_to_world );
		param_set_pt( h, _angle_cam );
		param_set_pt( h, _angle_ship_to_tube );
		param_set_pt( h, _angle_camera_to_tube );
		param_set_pt( h, _phase_in_front );
		param_set_pt( h, _pitch );
		param_set_pt( h, _speed );

	err_param_init_pt( h );
}

namespace {
	bool	b_first_flexus = true;
}

void	c_bdd_flexus::init()
{
	_tube_channel = -1;
	_bdd_mocap = nullptr;
	_bdd_tube_path = nullptr;
	clear_v3( _cam_pos_last );

	_s_size = 0;
	clear_v3( _cam_rot );
	clear_v3( _cam_pos );
	_angle_ship_to_world = 0;
	_angle_cam = 0;
	_angle_ship_to_tube = 0;
	_angle_camera_to_tube = 0;
	_phase_in_front = 0;
	_pitch = 0;
	_speed = 0;
}

CONSTRUCTOR_CREATE(c_bdd_flexus)
{
	param_init_with( n_bdd_flexus::param, n_bdd_flexus::PARAM_NB_MAX ); // flexus_param, FLEXUS_PARAM_NB_MAX);
	init();
}

c_bdd_flexus::~c_bdd_flexus()
{
}

REAL	c_bdd_flexus::get_dt()
{
	return REAL(_delta_t.get_dt());
}

void	c_bdd_flexus::draw()
{
	REAL	size[3];
	c_model::cur->get_size_v3( size );

	GOL::matrix::push();
		GOL::matrix::translate3v( _cam_pos );
		GOL::matrix::rotate_y( -_angle_cam );
		GOL::matrix::scale3v( size );
		n_axe::draw_origin();
	GOL::matrix::pop();
}

void	c_bdd_flexus::ship_update()
{
	if( _b_restart_trig_ui )
	{
		_s = 0;
		_b_restart_trig_ui = false;
	}

	if( _delta_t.update() )
	{
		clear_v3( _coor_in_path );
		_angle_ship_to_world = 0;
	}
	REAL dt = REAL(_delta_t.get_dt());
	//t = .002;

	_s_size = _bdd_mocap->get_len( _tube_channel );
	if( _s_size == 0. )
		return;

	REAL	tgn[3];
	DOUBLE	s_new;
	DOUBLE	t_new;

	_bdd_mocap->get_tangent( tgn, _tube_channel, _coor_in_path[0] );	//	this a node not a channel
	REAL angle_tube_to_world = REAL( ATAN2_TURN( tgn[2], tgn[0] ) );
	_pitch = REAL( ATAN2_TURN( SQRT( tgn[0]*tgn[0] + tgn[2]*tgn[2] ), tgn[1] ) );

	if( _b_nav_ship )
	{
		if( !_b_freeze_ship_position )
			_angle_ship_to_world -= _rot_factor * _in_lr * dt;
		_angle_ship_to_tube = _angle_ship_to_world - angle_tube_to_world;
	}
	else
	{
		_angle_ship_to_world = angle_tube_to_world;
		_angle_ship_to_tube = 0;
	}
	
	REAL	sin_ship_to_tube;
	REAL	cos_ship_to_tube;
	GET_SIN_COS_TURN( sin_ship_to_tube, cos_ship_to_tube, _angle_ship_to_tube );

	if( _b_freeze_ship_position )
	{
		_phase_in_front = 0;
		_speed = 0;
	}
	else
	{
		_phase_in_front = _in_forward_speed;
		_speed = _tra_factor * _phase_in_front;
	}
	s_new = _s + _speed  * dt * cos_ship_to_tube;
	t_new = _coor_in_path[2] + _speed * dt * sin_ship_to_tube;

	s_new /= _s_size;
	s_new = FMOD( s_new );
	//todo this was bpi stuff
	/*
	DOUBLE	a,b;
	DOUBLE	tmp;

	a = (world.ct-world.rt);
	if( t_new > a )
	{
		a = s_new - world.cs;
		a /= world.rs;
		b = t_new - world.ct;
		b /= world.rt;
		tmp = a*a+b*b;
		if( tmp > 1.) 
			tmp = SQRT(tmp);
		s_new = (s_new-world.cs)/tmp + world.cs;
		t_new = (t_new-world.ct)/tmp + world.ct;
		printf( "Recalage ellipse\n" );
	}
	_s = s_new*_s_size;
	_coor_in_path[2] = t_new;
	}
	else
	*/
	if( t_new > _t_size )
	{
		//todo this was bpi stuff
		/*if( ABS( s_new-world.cs ) < world.ds )
		{
			_s = s_new*_s_size;
			_coor_in_path[2] = t_new;
			printf( "Recalage couloir\n" );
		}
		else if( _coor_in_path[2] > _t_size )
		{
			_coor_in_path[2] = t_new;
		}
		else
		*/
		{
			REAL CONST tmp = (REAL)_t_size;
			_s = REAL(s_new * _s_size);
			_coor_in_path[2] = CLAMP( REAL(t_new), -tmp, tmp );
		}	
	}
	else 
	{
		REAL CONST tmp = (REAL)_t_size;
		_s = REAL(s_new * _s_size);
		_coor_in_path[2] = CLAMP( REAL(t_new), -tmp, tmp );
	}	

	_coor_in_path[0] = _s/_s_size;
	//	_speed = ABS( _speed);
	_phase_in_front *= cos_ship_to_tube;

	_pitch_ship_to_world = _pitch * cos_ship_to_tube;
	_cam_rot[0] = _hmd_rot[0] + _angle_cam + REAL(.5);
	_cam_rot[1] = _hmd_rot[1] - REAL(_pitch_ship_to_world);

	_angle_cam = -_angle_ship_to_world;

	_angle_camera_to_tube = -angle_tube_to_world - _cam_rot[0];
	_cam_rot[2] = _hmd_rot[2];

	add_v3( _cam_target, _cam_pos, tgn );

	_bdd_mocap->coor_to_world_one( _cam_pos, _tube_channel, _coor_in_path );
	_speed = dist_v3r( _cam_pos_last, _cam_pos ) / dt;
	cpy_v3(  _cam_pos_last,  _cam_pos );


	if( _deformer_flexus && _b_camera_deform )
		_deformer_flexus->apply( _cam_pos, 1);
}

void	c_bdd_flexus::update()
{
	if( !is_active() )
		return;

	if( b_first_flexus )
	{
		c_layers*	p_layers;
		//todo this should not go in a clean software
		//todo get it by ref
		if( _layers_mocap = layers_get_from_name_short( "Mocap" ) )
			_bdd_mocap = _layers_mocap->layer_get_always_from_index(0)->bdd_get_always<c_bdd_mocap>();
		if( _layers_flexus = layers_get_from_name_short( "Flexus" ) )
			_bdd_tube_path = _layers_flexus->layer_get_always_from_index(0)->bdd_get_always<c_bdd_tube_path>();

		if( p_layers = layers_get_from_name_short( "RenderRue" ) )
		{
			_deformer_flexus = p_layers->layer_get_always_from_index(0)->get_deformer();
			if( _deformer_flexus )
				_deformer_flexus->update();
		}
		_b_freeze_ship_position = false;

		b_first_flexus = false;
		return;
	}

	if( _bdd_mocap==0 )
		return;

	ship_update();
}
