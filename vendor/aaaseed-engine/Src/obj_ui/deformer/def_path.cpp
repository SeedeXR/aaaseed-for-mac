#include "def_path.h"
#include "infrastructure/param/param_declare.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/bdd/bdd_edit/bdd_curve3d.h"

FACTORY_INSTANCE_V1( c_def_path, def_path, Deformer path, def );

namespace n_def_path
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 10 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 GROUP_NB		= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_REF(			bdd_name_symbo	)
		PARAM_DEF_SYMBO_ZERO(	channel,		1, 0, 0, PARAM_MAX_UINT32, gstr::current )
		PARAM_DEF_BOOL_OFF(		size_canonical	)
		PARAM_DEF_POINT_UVA(	offset			)
		PARAM_DEF_SCALE_UVAF(	scale			)
	};
}

CONSTRUCTOR_CREATE( c_def_path )
,_bdd_target(nullptr)
{
	init_name_with( "Path" );
	param_init_with( n_def_path::param, n_def_path::PARAM_NB_MAX );
}

void c_def_path::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt_attach_obj(	h, _target_name_symbo,	_bdd_target );
	param_set_pt(				h, _channel				);
	param_set_pt(				h, _b_size_canonical	);
	param_set_pt(				h, _tra[2]				);
	param_set_pt(				h, _tra[1]				);
	param_set_pt(				h, _tra[0]				);
	param_set_pt(				h, _sca_ui[2]			);
	param_set_pt(				h, _sca_ui[1]			);
	param_set_pt(				h, _sca_ui[0]			);
	param_set_pt(				h, _sca_ui[3]			);

	err_param_init_pt(h);
}

c_def_path::~c_def_path()
{
}

void	c_def_path::update()
{
	if( !_target_name_symbo.is_empty() )
	{
		if( !_bdd_target || !_bdd_target->is_name_symbo( _target_name_symbo ) || !_bdd_target->get_root() )
		{
			c_bdd* bdd = find_bdd_by_name_symbo( _target_name_symbo );
			if( bdd )
			{
				if( c_bdd_mocap::is_instance(bdd) )
				{
					_bdd_target = (c_bdd*) bdd;
					_b_curve = false;
				}
				else if( c_bdd_curve_3d::is_instance(bdd) )
				{
					_bdd_target = (c_bdd*) bdd;
					_b_curve = true;
				}
				else
					err_print( "%s() found a bdd but this not a bdd_mocap or a bdd_curve_3d", __FUNCTION__ );
			}
		}
	}
	else
		_bdd_target = nullptr;
	if( _bdd_target )
	{
		_b_tra = is_not_null_v3( _tra );
		scale_v3( _sca, _sca_ui, _sca_ui[3] ); 
		if( !_b_size_canonical )
		{
			REAL len;
			if( _b_curve )
				len = ((c_bdd_curve_3d*)_bdd_target)->get_len( _channel );
			else
				len = ((c_bdd_mocap*)_bdd_target)->get_len( _channel );
			_sca[0] *= (len==0) ? 1 : 1/len ;
		}
		_b_sca = !is_all_one_v3( _sca );

	}
	set_deforming( _bdd_target != nullptr );
}

void	c_def_path::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	if( !_bdd_target )
		return;
			
	if( _b_tra )
	{
		if( _b_sca )
			add_const_then_mul_const_v3r( dst, src, _tra, _sca, nb );
		else
			offset_v3r( dst, src, _tra, nb );
	}
	else
	{
		mul_const_v3r( dst, src, _sca, nb );
	}
	if( _b_curve )
		((c_bdd_curve_3d*)_bdd_target)->coor_to_world( dst, _channel, dst, nb );
	else
		((c_bdd_mocap*)_bdd_target)->coor_to_world( dst, _channel, dst, nb );
}
