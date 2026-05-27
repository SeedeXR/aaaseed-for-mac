#include "def_fn.h"
#include "time/aaa_time.h"
#include "ui/strsymbo.h"
#include "obj_ui/bdd/bdd_image/bdd_img_proc.h"


FACTORY_INSTANCE_V1( c_def_fn, def_fn, Deformer Function, def );

namespace	n_def_fn
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 7 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_NONE(			image_proc )

		PARAM_DEF_REAL_POS(		freq,		2., 1.	)
		PARAM_DEF_INT32(		harm_nb,	2, 1,	1, 32	)

		PARAM_DEF_REAL_ZERO(	strenght	)
		PARAM_DEF_AXE_X(		axe_dst		)

		PARAM_DEF_REAL_ZERO(	move_speed	)
		PARAM_DEF_AXE_X(		move_axe	)
	};
}

CONSTRUCTOR_CREATE(c_def_fn)
{
INT32	h = 0;
	init_name_with( "Fn_Mix" );
	param_init_with( n_def_fn::param, n_def_fn::PARAM_NB_MAX ); // def_fn_param, DEF_FN_PARAM_NB_MAX);

	move_value = 0.;
}

void	c_def_fn::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_attach_obj( h, bdd_img_proc_cur );

	param_set_pt( h, freq);
	param_set_pt( h, harm_nb);

	param_set_pt( h, strenght);
	param_set_pt( h, dst_axe);

	param_set_pt( h, move_speed);
	param_set_pt( h, move_index);

	err_param_init_pt(h);
}

c_def_fn::~c_def_fn()
{
}

void	c_def_fn::update()
{
	DOUBLE t = aaa::time::get();
	move_value = REAL( t * move_speed * freq );
//	img_proc_cur->update();
	set_deforming( true );
}


void	c_def_fn::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL		delta;
	REAL		vec[3];

	if( !bdd_img_proc_cur)
	{
		err_print( "bdd_img_proc_cur is NULL in %s()", __FUNCTION__ );
		return;
	}
	for( ; nb>0; --nb )
	{
		scale_v3( vec, src, freq);
		if ( move_speed != 0. )
			vec[move_index] += move_value; 
			
		delta = bdd_img_proc_cur->compute_3fv( vec);
		delta *= strenght;

		if( _b_add )
		{
			cpy_v3( dst, src );
			*(dst+dst_axe) += delta;
		}
		else
		{
			clear_v3( dst );
			*(dst+dst_axe) = delta;
		}

		src +=3;
		dst +=3;
	}
}

