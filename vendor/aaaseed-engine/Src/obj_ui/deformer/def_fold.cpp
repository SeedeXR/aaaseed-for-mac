#include "def_fold.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#include "infrastructure/param/param_declare.h"
#include "math/aaa_math.h"

FACTORY_INSTANCE_V1( c_def_fold, def_fold, Deformer Fold, def );

//INT32 CONST	DEF_FOLD_BASE_PARAM_NB_MAX	= 7;
//INT32 CONST	DEF_FOLD_GROUP_PARAM_NB_MAX	= 0;
//INT32 CONST	DEF_FOLD_PARAM_NB_MAX =		DEF_FOLD_BASE_PARAM_NB_MAX
//										+	DEF_FOLD_GROUP_PARAM_NB_MAX;

static	C_PCHAR_C	str_src_select[2] =
{
	"Less",
	"More",
};


namespace	n_def_fold
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 6 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_AXE_X(			axe				)
		PARAM_DEF_REAL_ZERO(		origin			)
		PARAM_DEF_SYMBO_PSTR_ZERO(	side,	str_src_select )	
		PARAM_DEF_REAL_ZERO(		rotation		)
		PARAM_DEF_BOOL_OFF(			flip_rotation	)
		PARAM_DEF_REAL_ZERO(		offset			)	
	};
}

CONSTRUCTOR_CREATE(c_def_fold)
{
	init_name_with( "Fold Deformer" );
	param_init_with( n_def_fold::param, n_def_fold::PARAM_NB_MAX ); // def_fold_param, DEF_FOLD_PARAM_NB_MAX);
	init();
}

void	c_def_fold::init()
{
}

void	c_def_fold::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, i_axe);
	param_set_pt( h, origin);
	param_set_pt( h, s_how);
	param_set_pt( h, rotation);
	param_set_pt( h, b_rotation);
	param_set_pt( h, offset);

	err_param_init_pt(h);
}

c_def_fold::~c_def_fold()
{
}

void	c_def_fold::update()
{
	//	prepare the axes
	axe_build_index_vert( i_u, i_v, i_axe );
	set_deforming( true );
}

void	c_def_fold::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL	dist;
	REAL	the_sin;
	REAL	the_cos;


	if( b_rotation )
		dist = rotation;
	else
		dist = -rotation;

	the_sin = SIN_TURN(dist);
	the_cos = COS_TURN(dist);

	if( src != dst )
		MEMCPY( dst, src, (size_t) (nb * sizeof(REAL) * 3), __FUNCTION__ );

	dst -= 3;
	for(; nb>0; --nb )
	{
		dst += 3;

		dist = dst[i_u] - origin;

		if( (dist > 0.) == s_how )
		{
			dst[i_u] = origin + dist * the_cos;
			dst[i_v] += dist * the_sin + offset;
		}
	}
}

