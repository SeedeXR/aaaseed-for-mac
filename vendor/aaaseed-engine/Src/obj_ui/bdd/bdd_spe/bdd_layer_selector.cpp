#include "obj_ui/bdd/bdd_spe/bdd_layer_selector.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"
#include "draw/seedcam.h"

FACTORY_CREATE_PROP_V1( c_bdd_layer_selector, bdd_layer_selector, Layer Selector, bdd_layer_selector, sub_menu="To Sort"; );

static	C_PCHAR_C	str_type[2] =
{
	"selector value",
	"camera to position"
};

namespace n_bdd_layer_selector
{
#define PARAM_LAYER( name ) \
	PARAM_DEF_SYMBO(		layer_##name,			1, 0,	c_layers::LAYER_NB_MAX, c_layers::str_layer_letter )	\
	PARAM_DEF_SYMBO(		layer_##name##_bis,		1, 0,	c_layers::LAYER_NB_MAX,	c_layers::str_layer_letter )	\
	PARAM_DEF_SYMBO(		layer_##name##_ter,		1, 0,	c_layers::LAYER_NB_MAX, c_layers::str_layer_letter	)	\
	PARAM_DEF_SYMBO(		layer_##name##_quatre,	1, 0,	c_layers::LAYER_NB_MAX, c_layers::str_layer_letter	)	\
	PARAM_DEF_REAL_ZERO(	layer_##name##_min	)	\
	PARAM_DEF_REAL_ONE(		layer_##name##_max	)

	CONSTEXPR INT32 BASE_PARAM_NB	= 40;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_SYMBO_PSTR_ONE(	type,			str_type	)
		PARAM_DEF_POINT_XYZ(		position		)
		PARAM_DEF_REAL_ZERO(		selector		)
		PARAM_DEF_REAL_ZERO(		min				)
		PARAM_DEF_REAL_ONE(			max				)
		PARAM_DEF_BOOL_OFF(			use_min			)
		PARAM_DEF_REAL_LOCKED(		selector_out	)
		PARAM_DEF_INT32(			nb_selector,	1, 0,		0, c_bdd_layer_selector::SELECTOR_NB	)

		PARAM_LAYER( high )
		PARAM_LAYER( 1 )
		PARAM_LAYER( 2 )
		PARAM_LAYER( 3 )
		PARAM_LAYER( 4 )
	};
}

void	c_bdd_layer_selector::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, _s_type );
	param_set_pt_3( h, _position );

	param_set_pt( h, _selector_ui );
	param_set_pt( h, _min );
	param_set_pt( h, _max );
	param_set_pt( h, _b_use_min );
	param_set_pt( h, _selector );
	param_set_pt( h, _nb_selector );


	for( INT32 i=0; i<SELECTOR_NB; ++i )
	{
		param_set_pt_n( h, _index[i], LAYER_BY_SELECTOR_NB );
		param_set_pt( h, _layer_min[i] );
		param_set_pt( h, _layer_max[i] );
	}

	err_param_init_pt(h);
}

void c_bdd_layer_selector::init()
{
	param_init_with( n_bdd_layer_selector::param, n_bdd_layer_selector::PARAM_NB_MAX ); // param, BDD_LAYER_SELECTOR_PARAM_NB_MAX);

	_nb_to_draw = 0;
}

CONSTRUCTOR_CREATE(c_bdd_layer_selector)
{
	init();
}
EMPTY_DESTRUCTOR(c_bdd_layer_selector)

void c_bdd_layer_selector::update()
{
	if( _s_type )
		_selector = c_seedcam::get_cur()->get_dist(_position);
	else
		_selector = _selector_ui;

	_nb_to_draw = 0;
	if( _min != _max && INSIDE( _selector, _min, _max )  )
	{
		c_layer* pt;
		REAL	tmp = (_selector-_min)/(_max-_min);
		INT32	tmp_index;

		for( INT32 i=0; i<_nb_selector; ++i )
		{
			if(( _b_use_min && INSIDE( tmp, _layer_min[i], _layer_max[i]) )
				|| INSIDE( tmp, i?_layer_max[i-1]:_layer_min[0], _layer_max[i]) )
			{
				for( INT32 j=0; j<LAYER_BY_SELECTOR_NB; ++j )
				{
					tmp_index = _index[i][j];
					if( tmp_index )
					{
						pt = c_layers::get_cur()->layer_get_raw_from_index(tmp_index-1);
						if( pt != c_layer::get_cur() && is_obj_exist_and_active(pt) )
						{
							_layer[_nb_to_draw++] = pt;
						}
					}
				}
			}
		}
	}
}

void c_bdd_layer_selector::draw()
{
	for( INT32 i=0; i<_nb_to_draw; ++i )
		_layer[i]->update_then_draw();
}
