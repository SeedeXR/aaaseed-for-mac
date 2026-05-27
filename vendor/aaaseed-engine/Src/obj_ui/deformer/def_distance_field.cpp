#include "def_distance_field.h"


FACTORY_INSTANCE_V1( c_def_distance_field, def_distance_field, Deformer Distance Field, def );

namespace n_def_distance_field
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 9 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_REAL_INF(	scale,			0.5,  0.5)
		PARAM_DEF_REAL(		max_scaling,	1.5,  1.5,	0,20	)
		PARAM_DEF_REAL(		min_scaling,	0.5,  0.5,	0,20	)
		PARAM_DEF_REAL_INF(	decay_factor,	5.0,  5.0	)
		PARAM_DEF_REAL(		tangent_ratio,	0.7,  0.7,	0,1	)

		PARAM_DEF_REF(		bdd_src_01	)
		PARAM_DEF_REF(		bdd_src_02	)
		PARAM_DEF_REF(		bdd_src_03	)
		PARAM_DEF_REF(		bdd_src_04	)
	};
}

CONSTRUCTOR_CREATE(c_def_distance_field)
{
	init_name_with( "DistanceField" );
	param_init_with( n_def_distance_field::param, n_def_distance_field::PARAM_NB_MAX);
	init();
}

void c_def_distance_field::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt(				h, _scale			);
	param_set_pt(				h, _max_scaling		);
	param_set_pt(				h, _min_scaling		);
	param_set_pt(				h, _decay_factor	);
	param_set_pt(				h, _tangent_ratio	);

	param_set_pt_attach_obj(	h, _target_name_symbo_KL,		_field_target_src[0] );
	param_set_pt_attach_obj(	h, _target_name_symbo_KC,		_field_target_src[1] );
	param_set_pt_attach_obj(	h, _target_name_symbo_KR,		_field_target_src[2] );
	param_set_pt_attach_obj(	h, _target_name_symbo_SO,		_field_target_src[3] );

	err_param_init_pt(h);
}

void c_def_distance_field::init()
{
	//hack. ED
	_field_target_src[0] = (c_bdd_field_gene *) find_from_top_by_name_symbo( _target_name_symbo_KL );
	_field_target_src[1] = (c_bdd_field_gene *) find_from_top_by_name_symbo( _target_name_symbo_KC );
	_field_target_src[2] = (c_bdd_field_gene *) find_from_top_by_name_symbo( _target_name_symbo_KR );
	_field_target_src[3] = (c_bdd_field_gene *) find_from_top_by_name_symbo( _target_name_symbo_SO );
}

c_def_distance_field::~c_def_distance_field()
{
}

void	c_def_distance_field::update()
{
	bool b_deform = false;
	for( INT32 i=0; i<BDD_SRC_NB; ++i )
	{
		_b_field_valid[i] = _field_target_src[i] && _field_target_src[i]->is_valid();
		b_deform |= _b_field_valid[i];
	}
	set_deforming( b_deform );
}


void	c_def_distance_field::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL grad[3];
	REAL scaling;
	c_bdd_field_gene * field;
	REAL new_dist;
	REAL dist;
	REAL tangent[3];

	REAL fish_size2 = _fish_size * _fish_size; // gradient map is with squared dists.
	for( ; nb>0; --nb )
	{
		scaling = 0;
		dist = FLT_MAX;
		field = nullptr;
		//todo: check if the assignement in the if is okay. ED
		//to prepare an array of valid field
		if(			_b_field_valid[0] && _field_target_src[0]->is_inside(src)	&& (new_dist = _field_target_src[0]->get_distance(src)) < dist )
		{
			field = _field_target_src[0];
			dist = new_dist;
		}
		else if(	_b_field_valid[1] && _field_target_src[1]->is_inside(src)	&& (new_dist = _field_target_src[1]->get_distance(src)) < dist )
		{
			field = _field_target_src[1];
			dist = new_dist;
		}
		else if(	_b_field_valid[2] && _field_target_src[2]->is_inside(src)	&& (new_dist = _field_target_src[2]->get_distance(src)) < dist )
		{
			field = _field_target_src[2];
			dist = new_dist;
		}
		else if(	_b_field_valid[3] && _field_target_src[3]->is_inside(src)	&& (new_dist = _field_target_src[3]->get_distance(src)) < dist )
		{
			field = _field_target_src[3];
			dist = new_dist;
		}
		//else if (src[0] >  0    && src[0] < 20.753
		//	&&   src[1] >  0.3 && src[1] < 8.77
		//	&&   src[2] > -7.995/2.0f    && src[2] < 7.995/2.0f)
		//{
		//	// in the human cage.
		//	REAL dz = 4 - abs(src[2]);
		//	REAL dx = MIN(src[0], 20.7 - src[0]);
		//	if ( dz < dx )
		//	{
		//		grad[0] = REAL(0.); grad[1] = REAL(0.); grad[2] = REAL(src[2] > 0 ? 1. : -1.);
		//		scaling = ABS(src[2]);
		//	}
		//	else
		//	{
		//		grad[0] = REAL(src[0] < 10 ? -1. : 1.); grad[1] = REAL(0.); grad[2] = REAL(0.);
		//		scaling = ABS(10 - src[0]);
		//	}
		//}
		else
			// far away.
			scaling = 0.;

		if( field )
		{

			REAL max = _max_scaling * fish_size2;
			REAL min = MAX(_min_scaling * fish_size2, _grid_step * 1.2);
			// maek sure it's bigger than the grid step.

			field->get_gradient( grad, src );
			scaling = field->get_gradient_norm( src, min, max, _scale, _decay_factor );
			if( scaling != 0.0f && _tangent_ratio != 0.0f )
			{
				cross_y_v3r( tangent, grad );
				if( src[2] > 0 )
				{
					// Make window tangents have a positive x component.
					// Less chances to get stuck.
					tangent[0] = -tangent[0];
					tangent[2] = -tangent[2];
				}
				normalize_v3r( tangent );
				normalize_scale_v3r( grad, 1 - _tangent_ratio );
				add_scale_v3( grad, tangent, _tangent_ratio );
			}

			normalize_scale_v3r( grad, scaling );
			add_v3( dst, src, grad );
		}
		else
			cpy_v3( dst, src );
		src += 3;
		dst += 3;
	}
}

void	c_def_distance_field::update_fish_length(REAL CONST len)
{
	_fish_size = len;
}
