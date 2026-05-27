
#include "bdd_grid_adjustable.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/model.h"
#include "gol/gol_color.h"
#include "gol/gol_draw.h"
#include "draw/aaa_glut.h"
#include "infrastructure/obj/command.h"
#include "draw/picking.h"
#include "Draw/texture.h"
#include "shaders/shading.h"
#include "infrastructure/layer/layer.h"
#include "ui/keyboard.h"
#include "ui/flatland.h"


#ifndef AAA_SYSTEMKEYBOARD_H
#	include "system/win32/SystemKeyboard.h"
#endif	
#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif


FACTORY_CREATE_PROP_V1( c_bdd_grid_adjustable, bdd_grid_adjustable, Adjustable Grid, grid_adjustable, sub_menu="UI"; );

static	c_grid_point3d*	grid_point3d_cur;

namespace
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 17 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 GRID_GENE_PARAM_NB	= 7 + c_bdd_uv::PARAM_UV_NB;
	CONSTEXPR INT32 UI_PARAM_NB			= 16;
	CONSTEXPR INT32 POINT_PARAM_NB		= 8;
	CONSTEXPR INT32 GROUP_NB			= 3;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GRID_GENE_PARAM_NB
									+	POINT_PARAM_NB
									+	UI_PARAM_NB
									+	GROUP_NB;
	CONSTEXPR INT32 MAX_EDIT_NB = 1024;
	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_POINT_XYZ(	center )

		PARAM_DEF_BOOL_OFF(	grid_reset_trig		)
		PARAM_DEF_BOOL_OFF(	curve_inherit_last	)
		PARAM_DEF_INT32(	dataset_id,			2,1,	1, c_bdd_grid_adjustable::DATASET_ID_MAX )
		PARAM_DEF_INT32(	curve_nb_u,			8,7,	4, MAX_EDIT_NB )
		PARAM_DEF_INT32(	curve_nb_v,			8,7,	4, MAX_EDIT_NB )
		PARAM_DEF_INT32(	curve_nb_u_new,		8,7,	4, MAX_EDIT_NB )
		PARAM_DEF_INT32(	curve_nb_v_new,		8,7,	4, MAX_EDIT_NB )
		PARAM_DEF_BOOL_OFF(	curve_nb_new_trig	)
		PARAM_DEF_BOOL_OFF(	negate_u_trig		)

		PARAM_DEF_GROUP( Grid_generated, GRID_GENE_PARAM_NB )
			PARAM_DEF_INT32(		nb_u,		2,16,	1, c_bdd_uv::MAX_ELT_NB )
			PARAM_DEF_INT32(		nb_v,		2,16,	1, c_bdd_uv::MAX_ELT_NB )
			PARAM_DEF_INT32(		nb_axe,		2,1,	1, c_bdd_uv::MAX_ELT_NB )
			PARAM_DEF_BDD_UV()
			PARAM_DEF_BOOL_OFF(		symmetry_u		)
			PARAM_DEF_BOOL_OFF(		symmetry_u_trig	)
			PARAM_DEF_BOOL_OFF(		symmetry_v		)
			PARAM_DEF_BOOL_OFF(		symmetry_v_trig	)

		PARAM_DEF_BOOL_ON(	grid_draw	)

		PARAM_DEF_GROUP( Ui, UI_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			ui_draw				)
			PARAM_DEF_BOOL_LOCKED(		ui_draw_selected	)
			PARAM_DEF_FP32_ZERO_ONE(	ui_alpha			)
			PARAM_DEF_BOOL_OFF(			ui_deform			)
			PARAM_DEF_REAL_ONE(			ui_size				)
			PARAM_DEF_POINT_UVA(		ui_pos				)
			PARAM_DEF_BOOL_ON(			ui_draw_curve		)
			PARAM_DEF_BOOL_ON(			ui_draw_point		)
//			PARAM_DEF_REAL_ONE(			ui_point_size		)
			PARAM_DEF_REAL_ONE(			ui_tgn_size			)
			PARAM_DEF_BOOL_OFF(			ui_intercept		)
			PARAM_DEF_BOOL_OFF(			ui_flip_u			)
			PARAM_DEF_BOOL_OFF(			ui_flip_v			)
			PARAM_DEF_BOOL_OFF(			ui_lock_u			)
			PARAM_DEF_BOOL_OFF(			ui_lock_v			)

		PARAM_DEF_BOOL_ON(	deform			)
		PARAM_DEF_BOOL_ON(	force_compute	)
		PARAM_DEF_BOOL_OFF(	catmull			)
		PARAM_DEF_BOOL_OFF(	edge_auto		)

		PARAM_DEF_GROUP( Point, POINT_PARAM_NB )
			PARAM_DEF_INT32(				ui_index_u,		0,1,	0,6	)
			PARAM_DEF_REAL_ZERO_SAVE_NOT(	ui_u )
			PARAM_DEF_INT32(				ui_index_v,		0,1,	0,6	)
			PARAM_DEF_REAL_ZERO_SAVE_NOT(	ui_v )

			PARAM_DEF_BOOL_OFF(				pos_reset_trig	)
			PARAM_DEF_REAL_ZERO_SAVE_NOT(	pos_u )
			PARAM_DEF_REAL_ZERO_SAVE_NOT(	pos_v )
			PARAM_DEF_REAL_ZERO_SAVE_NOT(	pos_axe )
		// was PARAM_DEF_POINT_UVA( ui ) but crash on bad init from state
	};

	C_PCHAR_C point_data_ext	= "point_data";
	C_PCHAR_C point_u_ext		= "point_u";
	C_PCHAR_C point_v_ext		= "point_v";
}

//	REAL	ui_tgn_size;

void	c_bdd_grid_adjustable::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_center(		h );
	param_set_pt( h, _b_grid_reset_trig_ui );

	param_set_pt( h, _b_curve_inherit_last_ui );

	param_set_pt( h, _dataset_id_ui );

	param_set_pt( h, _gad_nb_u_ui );
	param_set_pt( h, _gad_nb_v_ui );
	param_set_pt( h, _nb_u_new_ui );
	param_set_pt( h, _nb_v_new_ui );
	param_set_pt( h, _b_nb_new_trig_ui );

	param_set_pt( h, _b_negate_u_trig_ui );

	++h;
		param_set_pt_nb_axe(	h );
		param_set_pt_uv(		h );

		param_set_pt( h, _b_symmetry_u_ui		);
		param_set_pt( h, _b_symmetry_u_trig_ui	);
		param_set_pt( h, _b_symmetry_v_ui		);
		param_set_pt( h, _b_symmetry_v_trig_ui	);

	param_set_pt( h, _b_grid_draw_ui );
	
	++h;
		param_set_pt( h, _b_ui_draw_ui			);
		param_set_pt( h, _b_ui_draw_selected_ui	);
		param_set_pt( h, _ui_alpha_ui			);
		param_set_pt( h, _b_ui_deform_ui		);
		param_set_pt( h, _ui_size_ui			);
		param_set_pt_3( h, _ui_pos_ui			);
		param_set_pt( h, _b_ui_draw_curve_ui	);
		param_set_pt( h, _b_ui_draw_point_ui	);
	//	param_set_pt( h, _ui_point_size			);
		param_set_pt( h, _ui_tgn_size_ui		);
		param_set_pt( h, _b_ui_intercept_ui		);
		param_set_pt( h, _b_ui_flip_u_ui		);
		param_set_pt( h, _b_ui_flip_v_ui		);
		param_set_pt( h, _b_ui_lock_u_ui		);
		param_set_pt( h, _b_ui_lock_v_ui		);


	param_set_pt( h, _b_deform_ui			);
	param_set_pt( h, _b_compute_force_ui	);
	param_set_pt( h, _b_catmull_ui			);
	param_set_pt( h, _b_edge_auto_ui		);

	//todo	typical problem to be solved
	//			how to update this (//pbtosolve)
	++h;
		if( _controls )
			param_set_max_no_inc( h, REAL(_controls->get_nb_u()) );
		param_set_pt( h, _iu_cur_ui );
		param_set_pt_even_null( h, _u.get_point_pt( _iu_cur_ui ) );				//pbtosolve

		if( _controls )
			param_set_max_no_inc( h, REAL(_controls->get_nb_v()) );

		param_set_pt( h, _iv_cur_ui );
		param_set_pt_even_null( h, _v.get_point_pt( _iv_cur_ui ) );				//pbtosolve

		param_set_pt( h, _b_pos_reset_trig_ui );
		if( _p_controls )
			param_set_pt_3( h, _p_controls->get_point( _iu_cur_ui, _iv_cur_ui ) );
		else
			param_set_pt_null_v3( h );
		
	err_param_init_pt( h );
}

//todo should not be called by update() but in a smarter way
void	c_bdd_grid_adjustable::update_param_spe()
{
	INT32 h = PARAM_NB_MAX - 6;
	param_set_pt_even_null( h, _u.get_point_pt(_iu_cur) );				//todo pbtosolve
	++h;
	param_set_pt_even_null( h, _v.get_point_pt(_iv_cur) );				//todo pbtosolve
	if( _p_controls )
		param_set_pt_3( h, _p_controls->get_point( _iu_cur, _iv_cur ) );
	else
		param_set_pt_null_v3( h );
}

FINLINE	void	c_bdd_grid_adjustable::alloc_curve()
{
	//err_print( "%s() u/v -> %u %u", __FUNCTION__, _gad_nb_u_ui, _gad_nb_v_ui );
	// 	HEAP_IS_CORRUPT();
	//err_print( "%s() control -> %x %x", __FUNCTION__, _controls, _controls_def );
	if( !_controls )
	{
		debug_break( "%s() _controls is NULL, dataset is %d", __FUNCTION__, _dataset_id );
		_p_controls = nullptr;
	}
	else if( _controls->get_nb_u()==0 || _controls->get_nb_v()==0 )
	{
		//err_print( "%s() cas 1 _controls", __FUNCTION__ );
		_controls->set_nb( _gad_nb_u_ui, _gad_nb_v_ui );
		//err_print( "%s() cas 1 _controls_def", __FUNCTION__ );
		_controls_def->set_nb( _gad_nb_u_ui, _gad_nb_v_ui );

		_b_grid_reset_trig_ui = true;
		_p_controls = _controls;
		_p_controls_def_to_draw = _p_controls;
		//	HEAP_IS_CORRUPT();
		//err_print( "%s() cas 1 done", __FUNCTION__ );
	}
	else if( _controls->get_nb_u()!=_gad_nb_u_ui || _controls->get_nb_v()!=_gad_nb_v_ui )
	{
		//err_print( "%s() cas 2 copy", __FUNCTION__ );
		_controls_old->copy_from( _controls );
		//err_print( "%s() cas 2 _controls", __FUNCTION__ );
		_controls->set_nb( _gad_nb_u_ui, _gad_nb_v_ui );
		//err_print( "%s() cas 2 _controls_def", __FUNCTION__ );
		_controls_def->set_nb( _gad_nb_u_ui, _gad_nb_v_ui );
		//	HEAP_IS_CORRUPT();
		build_control_grid_from_old();
		//	HEAP_IS_CORRUPT();
		//_p_controls = _controls;
		//_p_controls_def_to_draw = _p_controls;
		//	HEAP_IS_CORRUPT();
		//err_print( "%s() cas 2 done", __FUNCTION__ );
	}
	else
	{
		//err_print( "%s() cas 3 none", __FUNCTION__ );
		_p_controls = nullptr;
	}
	//err_print( "%s() done", __FUNCTION__ );
}

void	c_bdd_grid_adjustable::update_curve( INT32 CONST dataset_id )
{
	if( _dataset_id != dataset_id )
	{
		_dataset_id = dataset_id;
		_controls = _map_controls.get( dataset_id );
		//err_print( "%s() dataset is now %d at 0x%x", __FUNCTION__, dataset_id, _controls );
		_b_need_update_param_spe = true;
		_b_compute_needed = true;
	}
	alloc_curve();
}


CONSTRUCTOR_CREATE( c_bdd_grid_adjustable )
,_gad_nb_u_ui(4)
,_gad_nb_v_ui(4)
,_iu_cur(1)	//avoid crash too
,_iv_cur(1)
,_dataset_id(-1)
,_b_need_update_param_spe(true)
,_map_controls(-42)
,_controls(nullptr)
,_p_controls(nullptr)
,_p_controls_def_to_draw(nullptr)
,_b_deform(true)

{
	param_init_with( param, PARAM_NB_MAX );
	_controls_def = new c_grid_point3d;
	_controls_old = new c_grid_point3d;
	_b_axe_logic_vert = true;
	update_curve( 1 );	//we need one here to avoid crash in init
	set_v3(_pt_last, -424242.);	//-424242 is a hack to be sure we do the first update 
//HEAP_IS_CORRUPT();
}

c_bdd_grid_adjustable::~c_bdd_grid_adjustable()
{
	_controls->dealloc();
	obj_delete( _controls_def );
	obj_delete( _controls_old );
}

namespace {
	CONST CHAR numb_ext[] = ".0000" ;
	FINLINE void make_name( o_str& name, INT32 CONST i )
	{
		name.set_digits( -4, 4, i );
	}
	//code below is a little weird because first file is named .point_data then the other are named .point_data.0002 .point_data.0003 and so on
	// and we try to be optimal in speed and memory
}

AAA_ERR	c_bdd_grid_adjustable::load_do_after( o_str CONST & filename )
{
	o_str&	name = o_str::push_name( filename );

		name.replace_ext( point_data_ext );

		INT32	dataset_id_store = _dataset_id_ui;	//	keep it to put it back
		c_file::push_vfile();	//not saved in the vfile we want to access it as a single file

			//err_print( "%s() read datasets", __FUNCTION__ );
			for( INT32 i = 1; i <= DATASET_ID_MAX; ++i )
			{
				
				if( i > 1 )
					make_name( name, i );
				if( c_file::is_exist(name) )
				{
					//err_print( "%s() read dataset %u", __FUNCTION__, i );
					//_dataset_id_ui = i;
					update_curve( i );	// we need this to set the size 
					_controls->read_float_from_file( name.get() );
				}
				if( i==1 )
					name.add( numb_ext );
			}

		c_file::pop_vfile();
		//err_print( "%s() set back dataset to %u", __FUNCTION__, dataset_id_last );
		_dataset_id_ui = dataset_id_store;

		update_curve( _dataset_id_ui );
		_b_grid_reset_trig_ui = false;

		name.drop_ext();
		name.replace_ext( point_u_ext );
		_u.read_float_from_file( name.get() );
		if( _u.get_nb() )
			DBG_PRINT_STRING( "point_u data loaded %d points", _u.get_nb() );

		name.replace_ext( point_v_ext );
		_v.read_float_from_file( name.get() );
		if( _v.get_nb() )
			DBG_PRINT_STRING( "point_v data loaded %d points", _v.get_nb() );
		//HEAP_IS_CORRUPT();

	_p_controls = _controls;
	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR	c_bdd_grid_adjustable::save_do_after( o_str CONST & filename )
{
	o_str&	name = o_str::push_name( filename );

		name.replace_ext( point_data_ext );

		c_file::push_vfile();
		//	INT32	dataset_id_store = _dataset_id_ui;	//	keep it to put it back

			for( INT32 i = 1; i <= DATASET_ID_MAX; ++i )
			{	
				if( c_grid_point3d* control = _map_controls.find(i) )
				{
					if( i > 1 )
						make_name( name, i );
					control->write_float_in_file( name.get() );
				}
				if( i==1 )
					name.add( numb_ext );
			}

		c_file::pop_vfile();
	//	_dataset_id_ui = dataset_id_store;

	//	update_curve();
		name.drop_ext();
		//todo check it went well
		if( _u.get_nb() > 0 )
		{
			name.replace_ext( point_u_ext );
			_u.write_float_in_file( name.get() );
		}
		if( _v.get_nb() > 0 )
		{
			name.replace_ext( point_v_ext );
			_v.write_float_in_file( name.get() );
		}

	o_str::pop_name();
	return AAA_OK;
}

void	c_bdd_grid_adjustable::build_control_grid_one_point( INT32 CONST iu, INT32 CONST iv )
{
	REAL*	pos = _controls->get_point_direct( iu, iv );

	/*	for( iu = gad_nb_u; iu > 0; --iu )
	{
	v = v_start;
	//		u = center[i_u] + size[0]*(u_center+ (u_range*(gad_nb_u/2-iu))/REAL(gad_nb_u-3)*.5-.5);
	*/
	REAL	u;
	if( _u.get_nb() )
		u = _u.get_point(iu);
	else
		u = REAL( iu - 1 ) / REAL( _controls->get_nb_u() - 3 );
/*	if( _i_axe == 0 )
		u = - u;
	else 
		u =  u - 1. ;
*/
//	pos[0] = _center[_i_u] + _size[0] * ( _u_center_ui + _u_range_ui * (u-.5) ) ;
	pos[0] = _center_ui[_i_u] + _size[0] * (u-REAL(.5)) ;

	REAL	v;
	if( _v.get_nb() )
		v = _v.get_point(iv);
	else
		v = REAL( iv - 1 ) / REAL( _controls->get_nb_v() - 3 );
//	pos[1] = _center[_i_v] + _size[1] * ( _v_center_ui + _v_range_ui * (v-.5)  );
	pos[1] = _center_ui[_i_v] + _size[1] * (v-REAL(.5)) ;

	pos[2] = _center_ui[_i_axe];

	_b_compute_needed = true;
}

void c_bdd_grid_adjustable::build_control_grid()
{
	INT32	nb_u = _controls->get_nb_u();
	INT32	nb_v = _controls->get_nb_v();

	for( INT32 iu = 0; iu < nb_u; ++iu )
		for( INT32 iv = 0 ; iv < nb_v; ++iv )
			build_control_grid_one_point( iu, iv );
}

void c_bdd_grid_adjustable::build_control_grid_from_old()
{
	REAL*	p_point;
	REAL	u,v;

	/*	for( iu = gad_nb_u; iu > 0; --iu )
	{
	v = v_start;
	//		u = center[i_u] + size[0]*(u_center+ (u_range*(gad_nb_u/2-iu))/REAL(gad_nb_u-3)*.5-.5);
	*/
	INT32	nb_u = _controls->get_nb_u();
	INT32	nb_v = _controls->get_nb_v();

	REAL	fu = OVER_ONE_AS_REAL( nb_u - 3 );
	REAL	fv = OVER_ONE_AS_REAL( nb_v - 3 );

	for( INT32 iu = 0; iu < nb_u; ++iu )
	{
		u = REAL(iu-1) * fu ;
		for( INT32 iv = 0 ; iv < nb_v; ++iv )
		{
			v = REAL(iv-1) * fv ;
			p_point = _controls->get_point_direct( iu, iv );	//todo should we use only the set
//			HEAP_IS_CORRUPT();
			get_point_from_uv( _controls_old, p_point, u, v );
			_b_compute_needed = true;
//			HEAP_IS_CORRUPT();
		}
	}
}
void	c_bdd_grid_adjustable::build_geo()
{
	REAL*	p_point = _point;
//	REAL*	p_normal = _normal;
	REAL	u_start, du;
	REAL	v, dv;

	build_u_du( u_start, du );
	build_v_dv( v,       dv );

	u_start += .5;
	v       += .5;
	
	for( INT32 iv = _nb_v_ui; iv > 0; --iv )
	{
		REAL u = u_start;
		for( INT32 iu = _nb_u_ui; iu > 0; --iu )
		{
			get_point_from_uv( p_point, u,v );
			p_point += 3;
			u += du;
		}
		v += dv;
	}
	build_geo_validate();
	_b_deform = _b_deform_ui;
}

void	c_bdd_grid_adjustable::update()
{
	if( _b_nb_new_trig_ui )
	{
		_gad_nb_u_ui = _nb_u_new_ui;
		_gad_nb_v_ui = _nb_v_new_ui;
		_b_nb_new_trig_ui = false;
	}
	if( _b_curve_inherit_last_ui && grid_point3d_cur )
	{
		_gad_nb_u_ui = grid_point3d_cur->get_nb_u();
		_gad_nb_v_ui = grid_point3d_cur->get_nb_v();
	}
	_b_intercept = _b_ui_intercept_ui && !( _b_curve_inherit_last_ui && grid_point3d_cur );
	if( _b_intercept )
	{
		ui_register( this );
		_b_ui_draw_ui = true;
		_b_ui_draw_selected_ui = true;
	}
	else
		_b_ui_draw_selected_ui = false;

	update_curve( _dataset_id_ui );
//	HEAP_IS_CORRUPT();
	if( _b_catmull != _b_catmull_ui )
	{
		_b_catmull = _b_catmull_ui;
		_b_compute_needed = true;
	}

	if( _b_curve_inherit_last_ui && grid_point3d_cur )
		_p_controls = grid_point3d_cur;
	else
	{
		_p_controls = _controls;
		if( !_p_controls )
		{
			return;
		}
	}
	grid_point3d_cur = _p_controls;

	set_point_control_cur( _iu_cur_ui, _iv_cur_ui );

	if( g_picked_def.get_number() > 0 )
	{
		UINT32	layer_id = c_layer::get_cur()->get_obj_ui_id();
		if( g_picked_def.picked_by_id( layer_id ) )
		{
			g_picked_def.print();
			//analyse picked
			st_picked	picked;
			picked.sub_id.clear();
			g_picked_def.get_highest_by_id( &picked, layer_id );
			g_picked_def.print();
			if( picked.nb == 2 )
			{
				if( picked.priority == 2 )
				{
					// point selected
					INT32 iu = picked.sub_id.back() - 1;
					picked.sub_id.pop_back();
					INT32 iv = picked.sub_id.back() - 1;
					picked.sub_id.pop_back();
					set_point_control_cur( iu, iv );
				}
			}
			// remove picked for this layer
			g_picked_def.remove_by_id( layer_id );
		}
	}

	update_uvw();
	if( _b_pos_reset_trig_ui )
	{
		build_control_grid_one_point( _iu_cur, _iv_cur );
		_b_pos_reset_trig_ui = false;
	}
	if( _b_grid_reset_trig_ui )
	{
		build_control_grid();
		_b_grid_reset_trig_ui = false;
	}
	if( _b_negate_u_trig_ui )
	{
		negate_u();
		_b_negate_u_trig_ui = false;
	}
	if( _b_symmetry_u_ui || _b_symmetry_u_trig_ui )
	{
		do_symmetry_u();
		_b_symmetry_u_trig_ui = false;
	}
	if( _b_symmetry_v_ui || _b_symmetry_v_trig_ui )
	{
		do_symmetry_v();
		_b_symmetry_v_trig_ui = false;
	}
	if( _p_controls )
	{
		auto pt = _p_controls->get_point( _iu_cur_ui, _iv_cur_ui ); 
		if( pt && is_diff_v3( _pt_last, pt ) )
			_b_compute_needed = true;
	}
	if(	build_geo_is_needed() || _b_compute_needed || _b_compute_force_ui || _b_deform != _b_deform_ui )
	{
		//todoopt to it less often
		if( _b_edge_auto_ui )
			do_auto_edge( _dataset_id_ui );
		build_geo();
		_b_compute_needed = false;
	}
	if( _nb_axe_ui == 1 )
	{
		do_deform_and_normal();
		auto def = c_def_node::get_cur();
		if( def->is_deforming() )
		{
			if( _b_ui_deform_ui )
			{
				def->apply( _controls_def->get_points(), _p_controls->get_points(), _p_controls->get_nb() );
				_p_controls_def_to_draw = _controls_def;
			}
			else
				_p_controls_def_to_draw = _p_controls;
		}
		else
			_p_controls_def_to_draw = _p_controls;
	}
	if( _b_need_update_param_spe )
	{
		update_param_spe();
		_b_need_update_param_spe = false;
	}

	if( c_multiple::cur )
		c_multiple::cur->set_nb( _nb_u_geo, _nb_v_geo, _nb_axe_ui );

	if( _p_controls )
	{
		auto pt =  _p_controls->get_point( _iu_cur_ui, _iv_cur_ui );
		if( pt )
			cpy_v3( _pt_last, pt );
	}
//		set_v3( _pt_last, -42. );	//hack
}

void	draw_cat_line( REAL* a, REAL* b, REAL* c, REAL* d, INT32 nb, bool b_catmull )
{
	REAL	dt = 1 / REAL( nb );	// todo maybe check if nb == 0
	REAL	t = 0;
	REAL	point[3];

	GOL::begin( GL_LINE_STRIP );
		for( INT32 i = nb; i >= 0; --i )
		{
			if( b_catmull )
				catmull_rom_3( point, a,b,c,d, t );
			else
				interpolate_v3( point, b, c, t );
			GOL::vertex3v( point );
			t += dt;
		}
	GOL::end();
}

void	c_bdd_grid_adjustable::draw_cat_line_oriented( REAL CONST * CONST a, REAL CONST * CONST b, REAL CONST * CONST c, REAL CONST * CONST d, INT32 CONST nb, REAL CONST size_in )
{
	REAL	a0[3], b0[3], c0[3], d0[3];

	a0[_i_u]	= a[0] * size_in;
	a0[_i_v]	= a[1] * size_in;
	a0[_i_axe]	= a[2] * size_in;

	b0[_i_u]	= b[0] * size_in;
	b0[_i_v]	= b[1] * size_in;
	b0[_i_axe]	= b[2] * size_in;

	c0[_i_u]	= c[0] * size_in;
	c0[_i_v]	= c[1] * size_in;
	c0[_i_axe]	= c[2] * size_in;

	d0[_i_u]	= d[0] * size_in;
	d0[_i_v]	= d[1] * size_in;
	d0[_i_axe]	= d[2] * size_in;

	draw_cat_line( a0, b0, c0, d0, nb, _b_catmull );
}

#define	GP( u,v )	controls->get_point_direct( (u), (v) )

void	c_bdd_grid_adjustable::get_point_from_uv( c_grid_point3d* controls, REAL* CONST dst, REAL u, REAL v )
{
	REAL	tmp;
	if( _u.get_nb() )
		tmp = _u.remap(u);
	else
		tmp = u * (controls->get_nb_u()-3) + 1;

	INT32 iu = I_FLOOR(tmp);
	u = tmp - iu;
	if( _v.get_nb() )
		tmp = _v.remap(v);
	else
		tmp = v * (controls->get_nb_v()-3) + 1;

	INT32 iv = I_FLOOR(tmp);
	v = tmp - iv;	
	if( _b_catmull )
	{
		// avoid out of limit
		if( iu < 1 )
		{
			u += iu - 1;
			iu = 1;
		}
		else if( iu > (controls->get_nb_u()-3) )	//hack to avoid out of limit
		{
			u += iu - (controls->get_nb_u()-3);
			iu = controls->get_nb_u()-3;
		}
		if( iv < 1 )
		{
			v += iv - 1;
			iv = 1;
		}
		else if( iv > (controls->get_nb_v()-3) )
		{
			v += iv - (controls->get_nb_v()-3);
			iv = controls->get_nb_v()-3;
		}

		REAL u_0[3];
		REAL u_1[3];
		REAL u_2[3];
		REAL u_3[3];

		catmull_rom_3( u_0,	GP( iu-1, iv-1 ),	GP( iu, iv-1 ),	GP( iu+1, iv-1 ),	GP( iu+2, iv-1 ),	u );
		catmull_rom_3( u_1,	GP( iu-1, iv ),		GP( iu, iv ),	GP( iu+1, iv ),		GP( iu+2, iv ),		u );
		catmull_rom_3( u_2,	GP( iu-1, iv+1 ),	GP( iu, iv+1 ),	GP( iu+1, iv+1 ),	GP( iu+2, iv+1 ),	u );
		catmull_rom_3( u_3,	GP( iu-1, iv+2 ),	GP( iu, iv+2 ),	GP( iu+1, iv+2 ),	GP( iu+2, iv+2 ),	u );

		catmull_rom_3( dst,	u_0, u_1, u_2, u_3,		v );
	}
	else
	{
		// avoid out of limit
		if( iu < 1 )
		{
			u += iu - 1;
			iu = 1;
		}
		else if( iu > (controls->get_nb_u()-3) )	//hack to avoid out of limit
		{
			u += iu - (controls->get_nb_u()-3);
			iu = controls->get_nb_u()-3;
		}
		if( iv < 1 )
		{
			v += iv - 1;
			iv = 1;
		}
		else if( iv > (controls->get_nb_v()-3) )
		{
			v += iv - (controls->get_nb_v()-3);
			iv = controls->get_nb_v()-3;
		}

		REAL	u_1[3];
		REAL	u_2[3];

		interpolate_v3( u_1,	GP( iu, iv ),		GP( iu+1, iv ),		u );
		interpolate_v3( u_2,	GP( iu, iv+1 ),		GP( iu+1, iv+1 ),	u );

		interpolate_v3( dst,	u_1, u_2,	v );
	}
}

void	c_bdd_grid_adjustable::get_point_from_uv( REAL* CONST dst, REAL CONST u, REAL CONST v )
{
	if( _b_deform_ui  )
	{
		FP32 vec[3];
		get_point_from_uv( _p_controls, vec, u,v );
		dst[_i_u]	= vec[0];
		dst[_i_v]	= vec[1];
		dst[_i_axe]	= vec[2];
	}
	else
	{
		dst[_i_u]	= _center_ui[_i_u] + _size[0] * (u-REAL(.5));
		dst[_i_v]	= _center_ui[_i_v] + _size[1] * (v-REAL(.5));
		dst[_i_axe]	= _center_ui[_i_axe];
	}
}

void	c_bdd_grid_adjustable::draw()
{
	if( _b_grid_draw_ui )
		SUPER::draw();
	if( _b_ui_draw_ui )
	{
		begin_ui();
			draw_ui( REAL(1), zero_v4fp32, _ui_alpha_ui );
			if( _ui_size_ui != 1. || is_not_null_v3( _ui_pos_ui ) )
			{
				REAL	cen[3];
				cen[_i_u] = _ui_pos_ui[0];
				cen[_i_v] = _ui_pos_ui[1];
				cen[_i_axe] = _ui_pos_ui[2];
				draw_ui( _ui_size_ui, cen, _ui_alpha_ui*REAL(.5) );
			}
		end_ui();
	}
}

void	c_bdd_grid_adjustable::draw_ui_point( INT32 CONST u, INT32 CONST v, REAL CONST size_in, REAL CONST * CONST pos_in, bool CONST b_cross )
{
	REAL to_draw[3];
	get_control_point_def( to_draw, u,v, size_in, pos_in );
	ui_draw_point( to_draw, b_cross ? REAL(1) : REAL(0) );
}

void	c_bdd_grid_adjustable::draw_ui_tgn( REAL CONST * CONST c, REAL CONST * CONST dir )
{
	REAL d[3];
	GOL::begin( GL_LINE_STRIP );
		GOL::vertex3v( c );
		add_v3( d, c, dir );
		GOL::vertex3v( d );
	GOL::end();
}

FINLINE	void	c_bdd_grid_adjustable::set_color_construction( FP32 CONST alpha_in )
{
		GOL::color4( 0,0,1, alpha_in );
}
FINLINE	void	c_bdd_grid_adjustable::set_color_ui( FP32 CONST alpha_in )
{
	if( _b_ui_draw_selected_ui )
		GOL::color4( 0,1,0, alpha_in );
	else
		set_color_construction( alpha_in );
}
FINLINE	void	c_bdd_grid_adjustable::set_color_ui_selected( FP32 CONST alpha_in )
{
	if( _b_ui_draw_selected_ui )
		GOL::color4( 1,0,0, alpha_in );
	else
		set_color_construction( alpha_in );
}

void	c_bdd_grid_adjustable::draw_ui( REAL CONST size_in, REAL CONST * CONST pos_in, FP32 CONST alpha_in )
{
	c_grid_point3d*	p_con = _p_controls_def_to_draw;
	if( !p_con )
		return;

	INT32 iu_beg = 0;
	INT32 iv_beg = 0;
	INT32 iu_end = p_con->get_nb_u() - 1;
	INT32 iv_end = p_con->get_nb_v() - 1;

	if( !_b_catmull )
	{
		iu_beg = 1;
		--iu_end; 
		iv_beg = 1;
		--iv_end; 
	}

	set_color_ui( alpha_in );

	//	Draw point
	if( _b_ui_draw_point_ui )
	{
		bool b_can_construction;
		c_picking::cur_push_name( -1 );
		for( INT32 iv=iv_beg; iv<=iv_end; ++iv )
		{
			b_can_construction = _b_edge_auto_ui && (iv==iv_beg || iv==iv_end);
			c_picking::cur_set_pick_ref( iv + 1 );
			for( INT32 iu=iu_beg; iu<=iu_end; ++iu )
			{
				c_picking::cur_push_name( iu + 1 );
				c_picking::cur_push_name( 2 );
				//todoopt avoid to do it each time
					if( is_control_point_moveable(iu,iv) )
					{
						if( _b_ui_draw_selected_ui && iu==_iu_cur && iv==_iv_cur )
						{
							set_color_ui_selected( alpha_in );
							draw_ui_point( iu,iv, size_in, pos_in, true );
							set_color_ui( alpha_in );
						}
						else
							draw_ui_point( iu,iv, size_in, pos_in, false );
					}
					else
					{
						set_color_construction( alpha_in );
						draw_ui_point( iu,iv, size_in, pos_in, iu==_iu_cur && iv==_iv_cur );
						set_color_ui( alpha_in );
					}
				c_picking::cur_pop_name();
				c_picking::cur_pop_name();
			}
		}
		c_picking::cur_pop_name();
	}

	/*
	GOL::begin( GL_LINE_STRIP );
		for( i=0; i<7; ++i )
		{
			GOL::vertex3v( _p_controls->get_point(i,0) );
		}
	GOL::end();
	*/

	//	Draw Lines
	if( _b_ui_draw_curve_ui )
	{
		REAL a[3];
		REAL b[3];
		REAL c[3];
		REAL d[3];
		if( !_b_catmull )
		{
			--iu_beg;
			++iu_end; 
		}
		//	draw lines along u
		for( INT32 iv = iv_beg; iv <= iv_end; ++iv )
		{
			for( INT32 iu = iu_beg; iu <= iu_end-3; ++iu )
			{
				get_control_point_def( a, iu,     iv, size_in, pos_in );
				get_control_point_def( b, iu + 1, iv, size_in, pos_in );
				get_control_point_def( c, iu + 2, iv, size_in, pos_in );
				get_control_point_def( d, iu + 3, iv, size_in, pos_in );

				if( iv == _iv_cur )
					set_color_ui_selected( alpha_in );
				draw_cat_line( a,b,c,d, 20, _b_catmull );
				if( iv == _iv_cur )	
					set_color_ui( alpha_in );
			}
		}
		//c_picking::cur_push_name( -1 );
		if( !_b_catmull )
		{
			++iu_beg;
			--iu_end; 
			--iv_beg;
			++iv_end; 
		}
		//	draw lines along v
		for( INT32 iu = iu_beg; iu <= iu_end; ++iu )
		{
			for( INT32 iv = iv_beg; iv <= iv_end-3; ++iv )
			{
				get_control_point_def( a, iu, iv,     size_in, pos_in );
				get_control_point_def( b, iu, iv + 1, size_in, pos_in );
				get_control_point_def( c, iu, iv + 2, size_in, pos_in );
				get_control_point_def( d, iu, iv + 3, size_in, pos_in );

				if( iu == _iu_cur )
					set_color_ui_selected( alpha_in );
				draw_cat_line( a,b,c,d, 20, _b_catmull );
				if( iu == _iu_cur )
					set_color_ui( alpha_in );
			}
		}
		if( !_b_catmull )
		{
			++iv_beg;
			--iv_end; 
		}
	}

	//	Draw tangent
	if( _b_catmull && _ui_tgn_size_ui != 0. )
	{
		REAL a[3];
		REAL b[3];
		REAL c[3];
		for( INT32 iv = iv_beg; iv <= iv_end; ++iv )
		{
			if( iv == _iv_cur )
				set_color_ui_selected( alpha_in );
			for( INT32 iu = iu_beg; iu <= iu_end-2; ++iu )
			{
				get_control_point_def( a, iu,     iv, size_in );
				get_control_point_def( b, iu + 1, iv, size_in, pos_in );
				get_control_point_def( c, iu + 2, iv, size_in );
				sub_then_scale_v3r( a, c, _ui_tgn_size_ui * REAL(.25) );

				draw_ui_tgn( b, a );
				scale_v3( a, -1 );
				draw_ui_tgn( b, a );
			}
			if( iv == _iv_cur )
				set_color_ui( alpha_in );
		}
		for( INT32 iu = iu_beg; iu <= iu_end; ++iu )
		{
			if( iu == _iu_cur )
				set_color_ui_selected( alpha_in );
			for( INT32 iv = iv_beg; iv <= iv_end-2; ++iv )
			{
				get_control_point_def( a, iu, iv,     size_in );
				get_control_point_def( b, iu, iv + 1, size_in, pos_in );
				get_control_point_def( c, iu, iv + 2, size_in );
				sub_then_scale_v3r( a, c, _ui_tgn_size_ui * REAL(.25) );
				
				draw_ui_tgn( b, a );
				scale_v3( a, -1 );
				draw_ui_tgn( b, a );
			}
			if( iu == _iu_cur )
				set_color_ui( alpha_in );
		}
	}
}

void	c_bdd_grid_adjustable::negate_u()
{
	REAL*	p;
	INT32	ib;

	INT32	nb_u = _p_controls->get_nb_u();
	INT32	nb_v = _p_controls->get_nb_v();

	for( INT32 iu = 0; iu < nb_u; ++iu )
		for( INT32 iv = 0; iv < nb_v; ++iv )
		{
			p = _p_controls->get_point_direct( iu, iv );
			*p = -*p;
		}
	
	for( INT32 iu = 0; iu < nb_u / 2; ++iu )
	{
		ib = nb_u - 1 - iu;
		for( INT32 iv = 0; iv < nb_v; ++iv )
			swap_v3( _p_controls->get_point_direct( iu,iv ), _p_controls->get_point_direct( ib,iv ) );
	}
	_b_grid_reset_trig_ui = true;
	_b_compute_needed = true;
}

bool	c_bdd_grid_adjustable::is_control_point_moveable( INT32 CONST iu, INT32 CONST iv )
{
	if( _p_controls )
	{
		INT32	nb_u = _p_controls->get_nb_u()-1;
		INT32	nb_v = _p_controls->get_nb_v()-1;
		if( _b_edge_auto_ui )
		{
			if(	iu==0 || iv==0 || iu==nb_u || iv==nb_v )
				return false;
		}
		if( _b_symmetry_u_ui && iu > nb_u/2 )
			return false;
		if( _b_symmetry_v_ui && iv > nb_v/2 )
			return false;
		return true;
	}
	return false;
}

void	c_bdd_grid_adjustable::do_symmetry_u()
{
	INT32 nb_u = _p_controls->get_nb_u();
	INT32 nb_v = _p_controls->get_nb_v();

	for( INT32 iu = 0; iu < nb_u/2; ++iu )
		for( INT32 iv = 0; iv < nb_v; ++iv )
		{
			auto s = _p_controls->get_point_direct( iu,			iv );
			auto d = _p_controls->get_point_direct( nb_u-iu-1,	iv );
			*d = -*s;
			*++d = *++s;
		}
	_b_compute_needed = true;
}
void	c_bdd_grid_adjustable::do_symmetry_v()
{
	INT32 nb_u = _p_controls->get_nb_u();
	INT32 nb_v = _p_controls->get_nb_v();

	for( INT32 iu = 0; iu < nb_u; ++iu )
		for( INT32 iv = 0; iv < nb_v/2; ++iv )
		{
			auto s = _p_controls->get_point_direct( iu, iv );
			auto d = _p_controls->get_point_direct( iu, nb_v-iv-1 );
			*d = *s;
			*++d = -*++s;
		}
	_b_compute_needed = true;
}


void	c_bdd_grid_adjustable::set_point_control_cur( INT32 iu, INT32 iv, bool CONST b_send_command )
{
	if( _b_catmull )
	{
		iu = IMOD( iu, _controls->get_nb_u() );
		iv = IMOD( iv, _controls->get_nb_v() );
	}
	else
	{
		iu = IMOD( iu-1, _controls->get_nb_u()-2 ) + 1 ;
		iv = IMOD( iv-1, _controls->get_nb_v()-2 ) + 1 ;
	}
	if( _iu_cur != iu || _iv_cur != iv )
	{
		_iu_cur = iu;
		_iv_cur = iv;
		_iu_cur_ui = iu;
		_iv_cur_ui = iv;

		_b_need_update_param_spe = true;

		if( b_send_command )
		{
			CHAR	str[1024];
			sprintf( str, "do_command( \"set_point_control_cur( %d, %d)\" )", _iu_cur, _iv_cur );
			command_send( this, str );
		}
	}
}

bool	c_bdd_grid_adjustable::enum_command(	o_str& o )
{
	o.set( "set_control_point( dataset, index_u, index_v, x,y,z )" );
	o.add_newline();
	o.add( "set_point_control_cur( index_u, index_v )" );
	o.add_newline();
	return true;
}

bool	c_bdd_grid_adjustable::do_command( C_PCHAR_C cmd )
{
	bool retcode = true;
	INT32	dataset;
	INT32	i_u;
	INT32	i_v;
	REAL	vec[3];

//	DBG_PRINT_STRING( cmd );
	if( sscanf( cmd, "\"set_control_point( %d, %d, %d, %f, %f, %f )", &dataset, &i_u, &i_v, &vec[0], &vec[1], &vec[2] ) == 6 )
		set_control_point( dataset, i_u, i_v, vec );
	else if( sscanf( cmd, "\"set_point_control_cur( %d, %d)", &i_u, &i_v ) == 2 )
		set_point_control_cur( i_u, i_v, false );	//	avoid ping pong
	else
	{
		err_print( "%s() unknown command %s", __FUNCTION__, cmd );
		retcode = false;
	}
	return retcode;
}
void	c_bdd_grid_adjustable::do_auto_edge( INT32 CONST dataset )
{
	c_grid_point3d* g = _map_controls.get( dataset );

	INT32 nb_u = g->get_nb_u();
	INT32 nb_v = g->get_nb_v();

	//todoopt better than mix
	for( INT32 v=1; v<=nb_v-2; ++v )
	{
		mix_v3(	g->get_point_direct(0,v),		g->get_point_direct(1,v),		REAL(2.),	g->get_point_direct(2,v),		REAL(-1.) );
		mix_v3(	g->get_point_direct(nb_u-1,v),	g->get_point_direct(nb_u-2,v),	REAL(2.),	g->get_point_direct(nb_u-3,v),	REAL(-1.) );
	}
	for( INT32 u=0; u<=nb_u-1; ++u )
	{
		mix_v3(	g->get_point_direct(u,0),		g->get_point_direct(u,1),		REAL(2.),	g->get_point_direct(u,2),		REAL(-1.) );
		mix_v3(	g->get_point_direct(u,nb_v-1),	g->get_point_direct(u,nb_v-2),	REAL(2.),	g->get_point_direct(u,nb_v-3),	REAL(-1.) );
	}
}


REAL*	c_bdd_grid_adjustable::get_control_point( INT32 CONST dataset, INT32 CONST index_u, INT32 CONST index_v )
{
	return _map_controls.get( dataset )->get_point( index_u, index_v );
}
REAL*	c_bdd_grid_adjustable::get_control_point( INT32 CONST index_u, INT32 CONST index_v ) 
{
	return get_control_point( _dataset_id, index_u, index_v );
}


void	c_bdd_grid_adjustable::set_control_point( INT32 CONST dataset, INT32 CONST index_u, INT32 CONST index_v, REAL CONST * CONST vec )
{
	_map_controls.get( dataset )->set_point( index_u, index_v, vec );
	_b_compute_needed = true; //hack should be only by dataset
}
void	c_bdd_grid_adjustable::set_control_point( INT32 CONST index_u, INT32 CONST index_v, REAL CONST * CONST vec )
{
	set_control_point( _dataset_id, index_u, index_v, vec );
}

bool	c_bdd_grid_adjustable::do_key( INT32 CONST c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y )
{
	bool	b_return = false;
	if( _b_intercept )
	{
		if( b_special )
		{
			if( modifier::is_none() )
			{
				b_return = true;
				switch( c )
				{
				case keyboard::RIGHT:	set_point_control_cur( _iu_cur + 1	, _iv_cur		);	break;
				case keyboard::LEFT:	set_point_control_cur( _iu_cur - 1	, _iv_cur		);	break;
				case keyboard::UP:		set_point_control_cur( _iu_cur		, _iv_cur + 1	);	break;
				case keyboard::DOWN:	set_point_control_cur( _iu_cur		, _iv_cur - 1	);	break;
				default:
					b_return = false;
					break;
				}
			}
		}
		else
		{
			if( modifier::is_alt_on() )
			{
				if( !modifier::is_ctrl_on() )
				{
					b_return = true;
					switch( c )
					{
					case 'u':	_gad_nb_u_ui = MAX( 4, _gad_nb_u_ui-1 );	break;
					case 'U':	_gad_nb_u_ui += 1;	break;
					case 'v':	_gad_nb_v_ui = MAX( 4, _gad_nb_v_ui-1 );	break;
					case 'V':	_gad_nb_v_ui += 1;	break;
					default:
						b_return = false;
						break;
					}
				}
			}
			else
			{
				b_return = true;
				if( modifier::is_ctrl_on() )
				{
					switch( c )
					{
					case 18:	// Ctrl r
						_b_grid_reset_trig_ui = true;
						break;
					case 25:	// Ctrl y
						if( !c_flatland::is_draw_focus() )
							redo();
						break;
					case 26:	// Ctrl z
						if( !c_flatland::is_draw_focus() )
							undo();
						break;
					default:
						b_return = false;
						break;
					}
				}
				else
				{
					switch( c )
					{
					case 'u':	//	u only is free
					case 'U':	_b_ui_lock_u_ui = false;
								_b_ui_lock_v_ui = true;
								break;
					case 'v':	//	v only is free
					case 'V':	_b_ui_lock_u_ui = true;
								_b_ui_lock_v_ui = false;
								break;
					case 'f':	//	free for u and v 
					case 'F':	_b_ui_lock_u_ui = false;
								_b_ui_lock_v_ui = false;
								break;
					case 'q':	_b_ui_intercept_ui = false;
								break;
					default:	b_return = false;
								break;
					}
				}
			}
		}
	}
	else
	{
		if( b_special )
		{
		}
		else
		{
			if( modifier::is_alt_on() )
			{
				b_return = true;
				switch ( c )
				{
				case 'Q':
				case 'q':
					_b_ui_draw_ui = !_b_ui_draw_ui;
					break;
				default:
					b_return = false;
					break;
				}
			}
			else
			{
			}
		}
	}
	return b_return;
}

void	c_bdd_grid_adjustable::cpy_cur_to_st_undo( st_undo * CONST pt )
{
	pt->dataset_id	= _dataset_id;
	pt->iu			= _iu_cur;
	pt->iv			= _iv_cur;
	cpy_v3( pt->pt, _p_controls->get_point( _iu_cur, _iv_cur ) );
}
void	c_bdd_grid_adjustable::process_st_undo_to_cur( st_undo * CONST pt )
{
	set_control_point( pt->dataset_id, pt->iu, pt->iv, pt->pt );
}
void	c_bdd_grid_adjustable::undo()
{
	if( _undo_store.dataset_id != -42 )
	{
		cpy_cur_to_st_undo( &_redo_store );
		process_st_undo_to_cur( &_undo_store );
		_undo_store.dataset_id = -42; // to avoid multiple undo
	}
}
void	c_bdd_grid_adjustable::redo()
{
	if( _redo_store.dataset_id != -42 )
	{
		cpy_cur_to_st_undo( &_undo_store );
		process_st_undo_to_cur( &_redo_store );
		_redo_store.dataset_id = -42; // to avoid multiple redo
	}
}

bool	c_bdd_grid_adjustable::mouse_down( FP32& u_start, FP32& v_start )
{
	REAL* pt = _p_controls->get_point( _iu_cur, _iv_cur );
	if( pt && is_control_point_moveable( _iu_cur, _iv_cur ) )
	{
		cpy_v2( _ui_start, pt );
		//todo deal with orientation in a more generic way at at the level up
		u_start = pt[0];
		v_start = pt[1];

		cpy_cur_to_st_undo( &_undo_store );
		return true;
	}
	return false;
}

void	c_bdd_grid_adjustable::mouse_move( FP32 u_in, FP32 v_in )
{	
	//todo peut etre a refaire pour passer les du dv plutot
	// ici on recupere lea valeur directement

	REAL*	pt = _p_controls->get_point( _iu_cur, _iv_cur );
	if( pt )
	{
		if( _b_ui_lock_u_ui )
			u_in =  pt[0];
		if( _b_ui_lock_v_ui )
			v_in =  pt[1];

		if( _b_ui_flip_u_ui )
			u_in = (_ui_start[0] * REAL(2)) - u_in;
		if( _b_ui_flip_v_ui )
			v_in = (_ui_start[1] * REAL(2)) - v_in; 

		REAL vec[3];
		CHAR str[1024];

		vec[0] = u_in;
		vec[1] = v_in;
		vec[2] = 0.;

		set_control_point( _dataset_id, _iu_cur, _iv_cur, vec );
		sprintf( str, "do_command( \"set_control_point( %d, %d, %d, %f, %f, %f )\" )", _dataset_id, _iu_cur, _iv_cur, vec[0], vec[1], vec[2] );
		command_send( this, str );
	}
}

void	c_bdd_grid_adjustable::generate_uv_from_pos( REAL* uv, INT32 CONST nb_u, INT32 nb_v, INT32 CONST dataset_id )
{
	c_grid_point3d* control = _map_controls.find( dataset_id );
	if( !control )
	{
		err_print( "%s() no dataset %d", __FUNCTION__, dataset_id );
		return;	//todo perhaps add a basic init
	}

//	u_start = c_map::cur->get_u_max();
//	du = c_map::cur->get_u_min() - u_start;
	REAL u_start = 0.;
	REAL du = OVER_ONE_AS_REAL(nb_u-1);

//	v_start = c_map::cur->get_v_min();
//	dv = c_map::cur->get_v_max() - v_start;
	REAL v_start = 0.;
	REAL dv = OVER_ONE_AS_REAL(nb_v-1);

	REAL ou = - _center_ui[0];
	REAL ov = - _center_ui[1];
	REAL fu = OVER_ONE_AS_REAL(_size[0]);
	REAL fv = OVER_ONE_AS_REAL(_size[1]);
	REAL v = v_start;
	for( nb_v; nb_v > 0; --nb_v )
	{
		REAL u = u_start;
		for( INT32 i_u = nb_u; i_u > 0; --i_u )
		{
			REAL vec[3];
			get_point_from_uv( control, vec, u, v );
			*uv++ = ( vec[0] + ou ) * fu + REAL(.5);
			*uv++ = ( vec[1] + ov ) * fv + REAL(.5);
			u += du;
		}
		v += dv;
	}
}
