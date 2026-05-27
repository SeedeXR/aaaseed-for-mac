#include "obj_ui/transfo/transfo_trs.h"
#include "gol/gol.h"
#include "ui/strsymbo.h"
#include "math/billboard.h"
#include "infrastructure/param/param_declare.h"


FACTORY_CREATE_V1( c_transfo_trs, transfo_trs, Transfo TRS, transfo_trs );

c_transfo_trs*		c_transfo_trs::transfo1_cur = nullptr;
c_transfo_trs*		c_transfo_trs::transfo2_cur = nullptr;

namespace
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 18;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active )
		PARAM_DEF_REF(			name_symbo )

		PARAM_DEF_BOOL_OFF(		translate )
		PARAM_DEF_POINT_XYZ(	translate )

		PARAM_DEF_BOOL_OFF(		rotate )
		PARAM_DEF_BOOL_OFF(		rotate_first )
		PARAM_DEF_BOOL_OFF(		rotate_billboard )
		PARAM_DEF_SYMBO(		rotate_order,	0, aaa::matrix::ORDER_ZYX,		5,	gstr::rot_order		)
		PARAM_DEF_POINT_XYZ(	rotate	)

		PARAM_DEF_BOOL_OFF(		scale )
		PARAM_DEF_SCALE_XYZF(	scale )
	};

	PARAM_DEF_MAKE_INDEX( translate	);
	PARAM_DEF_MAKE_INDEX( rotate	);
	PARAM_DEF_MAKE_INDEX( scale		);
}


void	c_transfo_trs::param_init_pt_static()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, get_name_symbo() );

	param_set_pt( h, _b_tra_ui );
	param_set_pt_3( h, _tra );

	param_set_pt( h, _b_rot_ui );
	param_set_pt( h, _b_rotate_first_ui );
	param_set_pt( h, _b_billboard_ui );
	param_set_pt( h, _s_rotate_order_ui );
	param_set_pt_3( h, _rot );

	param_set_pt( h, _b_sca_ui );
	param_set_pt_4( h, _sca );

	err_param_init_pt(h);
}

void	c_transfo_trs::prepare_for_ui()
{
	bool b;
	bool b_unused = !is_active();

	param_set_unused( PARAM_INDEX_translate, b_unused );
	b = !_b_tra_ui || b_unused;
	param_set_unused_3( PARAM_INDEX_translate+1, b );

	param_set_unused( PARAM_INDEX_rotate, b_unused );
	b = !_b_rot_ui || b_unused;
	param_set_unused_6( PARAM_INDEX_rotate+1, b );

	param_set_unused( PARAM_INDEX_scale, b_unused );
	b = !_b_sca_ui || b_unused;
	param_set_unused_4( PARAM_INDEX_scale+1, b );
}

CONSTRUCTOR_CREATE(c_transfo_trs)
{
	param_init_with( param, PARAM_NB_MAX );
}

c_transfo_trs::~c_transfo_trs()
{
	if( transfo1_cur == this )
		transfo1_cur = nullptr;
	if( transfo2_cur == this )
		transfo2_cur = nullptr;
}

void	c_transfo_trs::update_private()
{
	_b_tra = _b_tra_ui && is_not_null_v3( _tra );
	_b_rot = _b_rot_ui && ( _b_billboard_ui || is_not_null_v3( _rot ) );
	_b_sca = _b_sca_ui && !is_all_one_v4( _sca );

	//todoq nice try/hack
	if( _b_billboard_ui && _b_rot )
	{
		if( _b_tra && !_b_rotate_first_ui )
			billboard::compute( _rot, _tra );
/*	experimental should be an option
	compatibly issue for the moment
			{
			REAL t[3];
			billboard::compute( t, tra);
			rot[1] = t[1];
			rot[2] = t[2];
			}
*/
		else
			billboard::compute( _rot );
	}
	_b_todo = _b_rot || _b_tra || _b_sca; 
}

FINLINE	void	c_transfo_trs::rotate()
{
	if( _b_billboard_ui )
	{
		GOL::matrix::rotate_y( _rot[1] );
		GOL::matrix::rotate_z( _rot[2] );
		GOL::matrix::rotate_x( _rot[0] );
	}
	else
		GOL::matrix::rotatev( _rot, _s_rotate_order_ui );
}

void	c_transfo_trs::do_it_low()
{
	if( _b_rot && _b_rotate_first_ui )
		rotate();
	if( _b_tra )
		GOL::matrix::translate3v( _tra );
	if( _b_rot && !_b_rotate_first_ui )
		rotate();
	if( _b_sca )
	{
		if( _sca[3] != 1. )
		{
			REAL vec[3];
			scale_v3( vec, _sca, _sca[3] );
			GOL::matrix::scale3v( vec );
		}
		else
			GOL::matrix::scale3v( _sca );
	}
}

void	c_transfo_trs::set_tra( REAL CONST * CONST  in )	{	cpy_v3( _tra, in );	}
void	c_transfo_trs::set_rot( REAL CONST * CONST  in )	{	cpy_v3( _rot, in );	}
void	c_transfo_trs::set_sca( REAL CONST * CONST  in )	{	cpy_v3( _sca, in );	}
