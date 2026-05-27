#include "def_bdd_blob.h"
#include "ui/strsymbo.h"
#include "math/v_base.h"
#if	AAA_DEBUG()
	#include "float.h"	//	for nan
#endif
#include "obj_ui/bdd/bdd_point/bdd_blob.h"


FACTORY_INSTANCE_V1( c_def_bdd_blob, def_bdd_blob, Deformer Bdd Blob, def );

namespace n_def_bdd_blob
{
	CONSTEXPR INT32 BASE_PARAM_NB	= c_deformer::BASE_PARAM_NB + 13;
	CONSTEXPR INT32 ORIGIN_PARAM_NB	= 3;
	CONSTEXPR INT32 GROUP_NB		= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	ORIGIN_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_REF(			bdd_blob_src	)

		PARAM_DEF_BOOL_OFF(		Gradient		)
		PARAM_DEF_REAL_ZERO(	Normal			)

		//		{	nullptr,	PARAM_REAL,		"exponent",			0, 1,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
		//		PARAM_DEF_GAIN(	shape_gain	)
		//		PARAM_DEF_BIAS(	shape_bias	)

		PARAM_DEF_AXE_X(		axe_src			)
		PARAM_DEF_AXE_Y(		axe_dst			)
		
		PARAM_DEF_SCALE_UVAF(	scale			)
		PARAM_DEF_REAL_ZERO(	strenght		)
		PARAM_DEF_REAL_ZERO(	offset			)
		PARAM_DEF_BOOL_OFF(		clamped			)
		PARAM_DEF_BOOL_OFF(		white_multiply	)

		PARAM_DEF_GROUP( Origin, ORIGIN_PARAM_NB )	
			PARAM_DEF_POINT_XYZ(	origin)
	};
}

CONSTRUCTOR_CREATE(c_def_bdd_blob)
	,_bdd_target(nullptr)
{
	init_name_with( "Def Bdd Blob" );
	param_init_with( n_def_bdd_blob::param, n_def_bdd_blob::PARAM_NB_MAX );
	init();
}

void c_def_bdd_blob::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt_attach_obj( h, _scr_name_sym, _bdd_target );

	param_set_pt( h, _b_gradient );
	param_set_pt( h, _normal );

	param_set_pt( h, _i_src_axe );
	param_set_pt( h, _i_dst_axe );
	param_set_pt_4( h, _scale_ui );
	param_set_pt( h, _strenght );
	param_set_pt( h, _offset );
	param_set_pt( h, _b_clamped );
	param_set_pt( h, _b_white_multiply );

	++h;
	param_set_pt_3( h, _origin);

	err_param_init_pt(h);
}

void c_def_bdd_blob::init()
{
}

c_def_bdd_blob::~c_def_bdd_blob()
{
}

void	c_def_bdd_blob::update()
{
	if( !_scr_name_sym.is_empty() )
	{
		if( !_bdd_target || !_bdd_target->get_root() || strcmp( _scr_name_sym.get(), _bdd_target->get_name_symbo().get())!=0 )
			_bdd_target = (c_bdd_blob*) find_by_class_and_name_symbo( "bdd_blob", _scr_name_sym );
	}
	else
		_bdd_target = nullptr;

	if( _bdd_target )
	{
		scale_v3( _scale, _scale_ui, _scale_ui[3] );
		if( _scale[0]!=0. && _scale[1]!=0. )
		{
			//	prepare the axes
			axe_build_index( _i_src_u, _i_src_v, _i_src_axe );
			set_deforming( true );
			return;
		}
	}
	set_deforming( false );
}


void	c_def_bdd_blob::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	if( !_bdd_target )
		return;	//todo un peu rapide non

	REAL	u;
	REAL	v;

	REAL	o_u;
	REAL	o_v;

	REAL	pos[3];
	pos[2] = 0.;

	o_u = _origin[_i_src_u];
	o_v = _origin[_i_src_v];

	if( _b_gradient )
	{
		REAL	grad[3];
		INT32	i_dst_u;
		INT32	i_dst_v;
		REAL	normal = _strenght * _normal;
		axe_build_index( i_dst_u, i_dst_v, _i_dst_axe );
		for( ; nb>0; --nb )
		{
			REAL r[3];

			cpy_v3( dst, src );

			u = *(src + _i_src_u) / _scale[0] - o_u;
			v = *(src + _i_src_v) / _scale[1] - o_v;
#if	AAA_DEBUG()
			if( _isnan(u) || _isnan(v) )
				debug_break();
#endif
/*			_img->get_gradient_from_uv_v2r( r, u, v, _b_clamped );

			//to regroup to optimize
			if( _b_white_multiply )
				scale_v2r( r, _img->get_value_from_uv( u, v, _b_clamped, true );
*/
			pos[0] = u;
			pos[1] = v;
			_bdd_target->get_field_gradient_at( grad, pos ); 
			clear_v3(r);

			*(dst + i_dst_u) += r[0] * _strenght;
			*(dst + i_dst_v) += r[1] * _strenght;
			if( normal != 0. )
			{		
				*(dst + i_dst_u) += r[1] * normal;
				*(dst + i_dst_v) += -r[0] * normal;
			}
			//next one
			src += 3;
			dst += 3;
		}
	}
	else
	{
		for( ; nb>0; --nb )
		{
			REAL r;

			cpy_v3( dst, src);

			u = *(src + _i_src_u) / _scale[0] - o_u ;
			v = *(src + _i_src_v) / _scale[1] - o_v ;

			pos[0] = u;
			pos[1] = v;
			r = _bdd_target->get_field_at( pos ); 
			if( _b_white_multiply )
				r *= r;

			r = r * _strenght + _offset;
			*(dst + _i_dst_axe) += r;

			//next one
			src += 3;
			dst += 3;
		}
	}
}
