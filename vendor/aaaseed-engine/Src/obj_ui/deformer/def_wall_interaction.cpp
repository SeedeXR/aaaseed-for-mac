#include "def_wall_interaction.h"
#include "math/v.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_wall_interaction, def_wall_interaction, Wall interaction field, def );

namespace n_def_wall_interaction
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 10 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_BOOL_OFF_SAVE_NOT(	b_debug )
		PARAM_DEF_REAL_INF(	interaction_dist,		3,		3		)
		PARAM_DEF_REAL_INF(	scale,					.1,		1		)
		PARAM_DEF_REAL(		max_radius,				5,		5,		0,	20	)
		PARAM_DEF_REAL(		min_radius,				1.5,	1.5,	0,	20	)
		PARAM_DEF_REAL_INF(	decay_factor,			3,		3		)

		PARAM_DEF_REAL(		sphere_height,			0,		0,		0,	10	)
		PARAM_DEF_REAL(		flatten_ratio_x,		1,		1,      0,	1	)
		PARAM_DEF_REAL(		flatten_ratio_y,		0.3,	0.3,	0,	1	)
		PARAM_DEF_REAL(		flatten_ratio_z,		0.1,	0.1,    0,	1	)

	};
}

CONSTRUCTOR_CREATE(c_def_wall_interaction)
{
	init_name_with( "WallInteraction" );
	param_init_with( n_def_wall_interaction::param, n_def_wall_interaction::PARAM_NB_MAX);
	init();
}

void c_def_wall_interaction::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt(	h, _b_debug				);
	param_set_pt(	h, _interaction_dist	);
	param_set_pt(	h, _scale				);
	param_set_pt(	h, _max_radius			);
	param_set_pt(	h, _min_radius			);
	param_set_pt(	h, _decay_factor		);

	param_set_pt(	h, _sphere_height		);
	param_set_pt(	h, _flatten_ratio_x		);
	param_set_pt(	h, _flatten_ratio_y		);
	param_set_pt(	h, _flatten_ratio_z		);

	err_param_init_pt(h);
}

void c_def_wall_interaction::init()
{
	//todo: init _point_left and _point_right.
	REAL left[3] {0, 0, 0}, right[3] {0, 0, 0};

	// KLs.
	left[2]  = REAL(-7.995 * 0.5f);
	right[2] = REAL(-7.995 * 0.5f);

	left[0] = 0.0f;
	right[0] = 4.46f + 0.75f * 0.5f;
	_KL1.set_points( left, right );

	left[0] = right[0];
	right[0] += 4.46f + 0.75f;
	_KL2.set_points( left, right );

	left[0] = right[0];
	right[0] += 4.46f + 0.75f;
	_KL3.set_points( left, right );

	left[0] = right[0];
	right[0] += 4.46f + 0.75f;
	_KL4.set_points( left, right );

	putchar(0);

	// KRs.
	left[2]  = REAL(7.995 * 0.5f);
	right[2] = REAL(7.995 * 0.5f);

	left[0] = 0.0f;
	right[0] = 4.46f + 0.75f * 0.5f;
	_KR1.set_points( left, right );

	left[0] = right[0];
	right[0] += 4.46f + 0.75f;
	_KR2.set_points( left, right );

	left[0] = right[0];
	right[0] += 4.46f + 0.75f;
	_KR3.set_points( left, right );

	left[0] = right[0];
	right[0] += 4.46f + 0.75f;
	_KR4.set_points( left, right );

	putchar(0);

	// KC.
	left[0] = 20.753f;
	left[2] = REAL(-7.995 * 0.5f);
	right[0] = 20.753f;
	right[2] = REAL(7.995 * 0.5f);
	_KC.set_points( left, right );

	_walls = (c_wall **)MALLOC( 9 * sizeof(size_t) );
	_walls[0] = &_KL1;
	_walls[1] = &_KL2;
	_walls[2] = &_KL3;
	_walls[3] = &_KL4;
	_walls[4] = &_KC;
	_walls[5] = &_KR1;
	_walls[6] = &_KR2;
	_walls[7] = &_KR3;
	_walls[8] = &_KR4;
}

c_def_wall_interaction::~c_def_wall_interaction()
{
	delete _walls;
}

void	c_def_wall_interaction::update()
{
	set_deforming( true );
}

void	c_def_wall_interaction::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL gradient[3];
	REAL force;
	for( ; nb>0; --nb )
	{
		for( INT32 i = 0; i < 9; ++i )
		{
			c_wall CONST * wall = _walls[i];
			if( wall->is_relevant(src) )
			{
				force = wall->get_force(
					src,
					_min_radius,
					_max_radius,
					_scale,
					_decay_factor,
					_interaction_dist );
				if( force > 0.0f )
				{
					REAL sphere_center[3];
					wall->get_sphere_center( sphere_center, _sphere_height );
					sub_v3( gradient, src, sphere_center );

					gradient[0] *= _flatten_ratio_x;
					gradient[1] *= _flatten_ratio_y;
					gradient[2] *= _flatten_ratio_z;
					normalize_scale_v3r( gradient, force );
					add_v3( dst, src, gradient );
					break;
				}
			}
		}
		src += 3;
		dst += 3;
	}
}

void	c_def_wall_interaction::reset_walls()
{
	for( INT32 i = 0; i < 9; ++i )
		_walls[i]->reset();
}
void	c_def_wall_interaction::add_human(REAL CONST * CONST pos, REAL CONST coverage)
{
	//if (coverage != 0.0f)
	//	printf("(%f %f %f) %f\n", pos[0], pos[1], pos[2], coverage);
	for( INT32 i = 0; i < 9; ++i )
		if( _walls[i]->get_distance_to_wall(pos) < _interaction_dist )
		//if( _walls[i]->is_relevant(pos) )
			_walls[i]->add_human( pos, coverage, _interaction_dist );
}
void	c_def_wall_interaction::end_humans()
{
	for( INT32 i = 0; i < 9; ++i )
		_walls[i]->end_humans();

	if( _b_debug )
	{
		DBG_PRINT_STRING("Wall interaction:" );
		if( _KL1.get_coverage() != 0.0f )
			DBG_PRINT_STRING( "KL1: (%f %f), %f", _KL1.get_barycenter()[0], _KL1.get_barycenter()[2], _KL1.get_coverage() );
		if( _KL2.get_coverage() != 0.0f )
			DBG_PRINT_STRING( "KL2: (%f %f), %f", _KL2.get_barycenter()[0], _KL2.get_barycenter()[2], _KL2.get_coverage() );
		if( _KL3.get_coverage() != 0.0f )
			DBG_PRINT_STRING( "KL3: (%f %f), %f", _KL3.get_barycenter()[0], _KL3.get_barycenter()[2], _KL3.get_coverage() );
		if( _KL4.get_coverage() != 0.0f )
			DBG_PRINT_STRING( "KL4: (%f %f), %f", _KL4.get_barycenter()[0], _KL4.get_barycenter()[2], _KL4.get_coverage() );

		if( _KR1.get_coverage() != 0.0f )
			DBG_PRINT_STRING( "KR1: (%f %f), %f", _KR1.get_barycenter()[0], _KR1.get_barycenter()[2], _KR1.get_coverage() );
		if( _KR2.get_coverage() != 0.0f )
			DBG_PRINT_STRING( "KR2: (%f %f), %f", _KR2.get_barycenter()[0], _KR2.get_barycenter()[2], _KR2.get_coverage() );
		if( _KR3.get_coverage() != 0.0f )
			DBG_PRINT_STRING( "KR3: (%f %f), %f", _KR3.get_barycenter()[0], _KR3.get_barycenter()[2], _KR3.get_coverage() );
		if( _KR4.get_coverage() != 0.0f )
			DBG_PRINT_STRING( "KR4: (%f %f), %f", _KR4.get_barycenter()[0], _KR4.get_barycenter()[2], _KR4.get_coverage() );
	}
}

//-----------------------------------------------------------------------------

void	c_wall::set_points(REAL CONST * CONST left, REAL CONST * CONST right)
{
	_left_point[0]  =  left[0];  _left_point[1] =  left[1];  _left_point[2] =  left[2];
	_right_point[0] = right[0]; _right_point[1] = right[1]; _right_point[2] = right[2];
}

REAL	c_wall::get_distance_to_wall(REAL CONST * CONST pos) CONST
{
	if( is_KC() )
	{
		// special case: KC.
		if( ABS(pos[2]) < 7.995 * 0.5f )
			return ABS(pos[0] - 20.753f);
		else
		{
			REAL norm2_left  = dist_squared_v3r(_left_point,  pos);
			REAL norm2_right = dist_squared_v3r(_right_point, pos);
			return SQRT(norm2_left < norm2_right ? norm2_left : norm2_right);
		}
	}
	else
	{
		if( pos[0] >= _left_point[0] && pos[0] <= _right_point[0] )
			return ABS(pos[2] - _left_point[2]);
		else
		{
			REAL norm2_left  = dist_squared_v3r(_left_point,  pos);
			REAL norm2_right = dist_squared_v3r(_right_point, pos);
			return SQRT(norm2_left < norm2_right ? norm2_left : norm2_right);
		}
	}
}

REAL	c_wall::get_distance_to_cylinder_center(REAL CONST * CONST pos) CONST
{
	if( _coverage == 0.0f || !is_relevant(pos) )
		return FLT_MAX;

	if( is_KC() )
	{
		// KC.
		REAL dy = pos[1] - _barycenter[1];
		REAL dz = pos[2] - _barycenter[2];
		return SQRT(dy*dy + dz*dz);
	}
	else
	{
		// KL/KR.
		REAL dx = pos[0] - _barycenter[0];
		REAL dy = pos[1] - _barycenter[1];
		return SQRT(dx*dx + dy*dy);
	}
}

bool	c_wall::is_relevant( REAL CONST * CONST pos ) CONST
{
	//todo: KL3 human should affect KL4 boids.s
	REAL CONST z = _right_point[2];
	if( z <= 0 )
		// KLs.
		return pos[2] < 0 && pos[0] >= _left_point[0] && pos[0] <= _right_point[0];
	else if( z >= 0 )
	{
		// KRs.
		return pos[2] > 0 && pos[0] >= _left_point[0] && pos[0] <= _right_point[0];
	}
	return false;
}

void	c_wall::reset()
{
	_coverage = 0.0f;
	_barycenter[0] = 0.0f; _barycenter[1] = 0.0f; _barycenter[2] = 0.0f;
}


void	c_wall::add_human(REAL CONST * CONST pos, REAL CONST coverage, REAL CONST interaction_dist)
{
	// min_dist;
	if( get_distance_to_wall(pos) < interaction_dist ) {
		// only one point per screen anyway, no need to barycenter.
		_barycenter[0] = pos[0];
		_barycenter[1] = pos[1];
		_barycenter[2] = pos[2];
		_coverage = coverage;
	}
}

void	c_wall::end_humans()
{
	// nothing to do here anymore.

	//if (_coverage != 0.0f)
	//{
	//	if( is_KC() )
	//	{
	//		// bary y-z
	//		_barycenter[1] /= _coverage;
	//		_barycenter[2] /= _coverage;
	//	}
	//	else
	//	{
	//		// bary x-y
	//		_barycenter[0] /= _coverage;
	//		_barycenter[1] /= _coverage;
	//	}
	//}
}

bool	c_wall::is_KC() CONST
{
	return _left_point[0] == _right_point[0];
}
bool	c_wall::is_KL() CONST
{
	return !is_KC() && _left_point[2] <= 0;
}
bool	c_wall::is_KR() CONST
{
	return !is_KC() && _left_point[2] >= 0;
}

REAL	c_wall::get_force(
	REAL CONST * CONST pos,
	REAL CONST min_radius,
	REAL CONST max_radius,
	REAL CONST scale,
	REAL CONST decay_factor,
	REAL CONST interaction_dist) CONST
{
	// Note: does not take coverage into account anymore.
	REAL dist = get_distance_to_cylinder_center(pos);
	
	REAL dist_human_to_wall = REAL(MIN(ABS(_barycenter[2] - 7.995 * 0.5f), ABS(_barycenter[2] + 7.995 * 0.5f)));
	REAL sphere_radius = min_radius + (max_radius - min_radius) * (1.0f - dist_human_to_wall / interaction_dist);
	// no repulsor beyond a certain limit.
	REAL CONST max_dist = sphere_radius;
	REAL CONST min_dist = max_dist * 0.33f;
	if (dist > max_dist)
		return 0.;

	if ( dist < min_dist)
		return scale;

	// between _min_threshold and _max_threshold
	FP32 t = (max_dist - dist) / MAX(max_dist - min_dist, 0.1);
	t = REAL(pow(t, decay_factor * (7.995 * 0.5f - ABS(_barycenter[2]))));
	dist = scale * t;
	return dist;
}

void	c_wall::get_sphere_center(REAL * sphere, REAL CONST sphere_height) CONST
{
	sphere[0] = _barycenter[0];
	sphere[1] = sphere_height;
	sphere[2] = (is_KC() ? -1 : 1) * 7.995f * 0.5f; // on the wall.
}
