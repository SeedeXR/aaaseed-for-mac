#include "obj_ui/bdd/bdd_edit/bdd_curve_edit.h"
#include "obj_ui/bdd/bdd_edit/keys.h"
#include "strnum.h"
#include "draw/model.h"
#include <algorithm>
#include "gol/gol.h"
#include "gol/gol_matrix.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "draw/aaa_glut.h"
#include "infrastructure/data/datacube.h"
#include "infrastructure/namer.h"
#include "infrastructure/seedfile.h"
#include "ui/alphabet.h"
#include "draw/shape.h"
#include "draw/line.h"
#include "ui/keyboard.h"
#ifndef AAA_SYSTEMKEYBOARD_H
#include "system/win32/SystemKeyboard.h"
#endif

#include "obj_ui/tracker/trackers.h"
#include <stdarg.h>

#define	CURVE_HEADER  "# CURVE_EDIT "

extern	void	CURVE_PRINT_STRING( C_PCHAR_C fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( CURVE_HEADER, fmt, args );
	va_end(args);
}

//CURVE_EDIT
FACTORY_CREATE_PROP_V1( c_bdd_curve_edit, bdd_curve_edit, Curve Edit, bdd_curve_edit, sub_menu="UI"; );

c_instance_by_channel< c_bdd_curve_edit, c_bdd_curve_edit::CURVE_EDIT_CHANNEL_NB_MAX >	c_bdd_curve_edit::inst_by_channel;


static	c_control_key*	control_phase_cur	= nullptr;
static	REAL			phase_cur_cur		= 0.;

class c_control_key;

enum CURVE_MODE_TYPE : INT32
{
	CURVE_1D = 0,
	CURVE_2D,
	CURVE_3D,
	CURVE_MODE_MAX_NB,
};

static	C_PCHAR_C	curve_mode_str[CURVE_MODE_MAX_NB] = 
{
	"1D",
	"2D",
	"3D",
};

static	GLushort	stipple_auto[] =
{
	0x00FF,		// dashed
	0x1C47,		// dash/dot/dash
	0xF00F,		// long dashed lines
	0x0101,		// dotted
	0x8888,		// dotted lines
};

#define	CURVE_COLOR_AUTO_MAX	8

static	FP32 CONST color_auto[] =
{
	1,0,0,	1,	
	0,1,0,	1,
	0,0,1,	1,
	1,1,1,	1,
	1,1,0,	1,
	0,1,1,	1,
	1,0,1,	1,
	1,1,1,	.5,
};


c_bdd_curve_edit*	c_bdd_curve_edit::cur = nullptr;

namespace n_bdd_curve_edit
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 2 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 EDITOR_PARAM_NB		= 1;
	CONSTEXPR INT32 PHASE_PARAM_NB		= 13;
	CONSTEXPR INT32 CONTROLS_PARAM_NB	= 4;
	CONSTEXPR INT32 DRAW_PARAM_NB		= 25;
	CONSTEXPR INT32 UI_GLOBAL_PARAM_NB	= 4;
	CONSTEXPR INT32 TEXT_PARAM_NB		= 5;
	CONSTEXPR INT32 GRID_PARAM_NB		= 9;
	CONSTEXPR INT32 GRID_LOCK_PARAM_NB	= 4;
	CONSTEXPR INT32 VISU_PARAM_NB		= 2;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 9;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	EDITOR_PARAM_NB
									+	PHASE_PARAM_NB
									+	CONTROLS_PARAM_NB
									+	DRAW_PARAM_NB
									+	UI_GLOBAL_PARAM_NB
									+	TEXT_PARAM_NB
									+	GRID_PARAM_NB
									+	GRID_LOCK_PARAM_NB
									+	VISU_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
	
//		{	nullptr,	PARAM_INT32,	"datagrid_dst",		1, 0,		0, DATAGRID_NB - 1,				nullptr, nullptr },		
		PARAM_DEF_INT32(		channel_id,		2, 1,		1, c_bdd_curve_edit::CURVE_EDIT_CHANNEL_NB_MAX )
		PARAM_DEF_GROUP_CLOSED( visu, VISU_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	visu_type,		curve_mode_str )
			PARAM_DEF_AXE_X(			visu_axe )	

		PARAM_DEF_GROUP( phase stuff, PHASE_PARAM_NB )
			PARAM_DEF_BOOL_ON_STR(	phase_control,	gstr::current	)
			PARAM_DEF_BOOL_OFF(		phase_variable		)
			//PARAM_DEF_BOOL_ON(	follow_phase )
			PARAM_DEF_BOOL_OFF(		play )
			PARAM_DEF_BOOL_ON(		stop_on_phase )
			PARAM_DEF_BOOL_ON(		loop )
			PARAM_DEF_BOOL_ON(		forward )
			PARAM_DEF_BOOL_OFF(		restart_trig )
			PARAM_DEF_BOOL_OFF(		play_trig )
			PARAM_DEF_BOOL_OFF(		stop_trig )
			PARAM_DEF_BOOL_OFF(		previous_trig )
			PARAM_DEF_BOOL_OFF(		next_trig )
			PARAM_DEF_BOOL_OFF(		previous_step_trig )
			PARAM_DEF_BOOL_OFF(		next_step_trig )
		PARAM_DEF_REAL_ZERO( phase )

		PARAM_DEF_GROUP_CLOSED( controls, CONTROLS_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	default_type,	str_key_type )
			//PARAM_DEF_INT32_LOCK( control_nb )
			//	it save the control_nb and it is needed to load files 
			PARAM_DEF_INT32(			control_nb,		2, 1,	1, CONTROL_NB_MAX )
			PARAM_DEF_INT32(			control_id,		2, 1,	1, CONTROL_NB_MAX )
			PARAM_DEF_NONE(				control			)

		PARAM_DEF_GROUP_CLOSED( draw, DRAW_PARAM_NB + GRID_PARAM_NB + TEXT_PARAM_NB + 2 )
			PARAM_DEF_REAL_ZERO(	draw_u_min )
			PARAM_DEF_REAL_ONE(		draw_u_max )
			PARAM_DEF_REAL_ZERO(	draw_v_min )
			PARAM_DEF_REAL_ONE(		draw_v_max )
			PARAM_DEF_BOOL_OFF(		ui_draw )
			PARAM_DEF_BOOL_OFF(		ui_draw_axe )
			PARAM_DEF_BOOL_OFF(		ui_draw_phase_cur )

			PARAM_DEF_BOOL_ON(		ui_draw_curve )
			PARAM_DEF_BOOL_OFF(		ui_draw_curve_current_only )
			PARAM_DEF_BOOL_OFF(		ui_draw_last )
			PARAM_DEF_INT32(		draw_last_nb,	1, 10,		0, PARAM_MAX_INT32	)
			PARAM_DEF_BOOL_ON(		ui_draw_point )
			PARAM_DEF_BOOL_ON(		ui_draw_phase )
			PARAM_DEF_BOOL_OFF(		ui_color_auto )
			PARAM_DEF_INT32(		color_loop_max,	1, CURVE_COLOR_AUTO_MAX,	1, CURVE_COLOR_AUTO_MAX )
			PARAM_DEF_BOOL_OFF(		ui_draw_follow_phase )
			PARAM_DEF_BOOL_OFF(		crosshair_draw )
			PARAM_DEF_REAL_ZERO(	crosshair_u )
			PARAM_DEF_REAL_ZERO(	crosshair_v )

			PARAM_DEF_GROUP_CLOSED( Grid, GRID_PARAM_NB )
				PARAM_DEF_BOOL_OFF(		grid_draw )

				PARAM_DEF_REAL_ZERO(	grid_left		)
				PARAM_DEF_REAL_ONE(		grid_right		)
				PARAM_DEF_REAL_ZERO(	grid_bottom		)
				PARAM_DEF_REAL_ONE(		grid_top		)

				PARAM_DEF_INT32(		grid_nb_u,		1, 0,	0, PARAM_MAX_INT32	)
				PARAM_DEF_INT32(		grid_nb_u_sub,	2, 1,	1, PARAM_MAX_INT32	)
				PARAM_DEF_INT32(		grid_nb_v,		1, 0,	0, PARAM_MAX_INT32	)
				PARAM_DEF_INT32(		grid_nb_v_sub,	2, 1,	1, PARAM_MAX_INT32	)

			PARAM_DEF_SCALE_UVF( ui_marker_size )
			PARAM_DEF_POINT_XYZ( center )

			PARAM_DEF_GROUP_CLOSED( Text, TEXT_PARAM_NB )
				PARAM_DEF_REAL_ONE( text_line_width )
				PARAM_DEF_POINT_UV( text_offset )
				PARAM_DEF_SCALE_UV( text_scale )

		PARAM_DEF_GROUP_CLOSED( ui, UI_GLOBAL_PARAM_NB+GRID_LOCK_PARAM_NB+1 )
			PARAM_DEF_BOOL_OFF( ui_intercept )
			PARAM_DEF_BOOL_OFF( ui_lock_u )
			PARAM_DEF_BOOL_OFF( ui_lock_v )
			PARAM_DEF_BOOL_OFF( ui_phase )

			PARAM_DEF_GROUP_CLOSED( Grid Lock, GRID_LOCK_PARAM_NB )
				PARAM_DEF_BOOL_OFF(			ui_grid_lock_u )
				PARAM_DEF_REAL_POS_ZERO(	ui_grid_lock_u_size )
				PARAM_DEF_BOOL_OFF(			ui_grid_lock_v )
				PARAM_DEF_REAL_POS_ZERO(	ui_grid_lock_v_size )

		PARAM_DEF_GROUP_CLOSED( editor, EDITOR_PARAM_NB )
			PARAM_DEF_BOOL_OFF( clear_trig )
	};
}


void	c_bdd_curve_edit::param_init_pt()
{
	INT32	h = param_init_pt_geo();

//	param_set_pt( h, curve_edit_datagrid_id_ );
	param_set_pt( h, _channel_id );

	++h;
		param_set_pt( h, _s_visu_curve );
		param_set_pt( h, _s_visu_axe );

	++h;
		param_set_pt( h, _b_phase_control_ui );
		param_set_pt( h, _b_phase_rel_ui );
//		param_set_pt( h, _b_follow_phase_ui );
		param_set_pt( h, _b_play_ui );
		param_set_pt( h, _b_stop_on_phase_ui );
		param_set_pt( h, _b_loop_ui );
		param_set_pt( h, _b_forward_ui );
		param_set_pt( h, _b_restart_trig_ui );
		param_set_pt( h, _b_play_trig_ui );
		param_set_pt( h, _b_stop_trig );
		param_set_pt( h, _b_prev_trig_ui );
		param_set_pt( h, _b_next_trig_ui );
		param_set_pt( h, _b_prev_step_trig_ui );
		param_set_pt( h, _b_next_step_trig_ui );

	param_set_pt( h, _phase_cur );

	++h;
		param_set_pt( h, _type_default );
		param_set_pt( h, _control_nb );
		param_set_pt( h, _control_id_cur );
		param_attach_obj( h, _p_control_cur );

	++h;
		param_set_pt(	h,	_draw_min_ui[0]					);
		param_set_pt(	h,	_draw_max_ui[0]					);
		param_set_pt(	h,	_draw_min_ui[1]					);
		param_set_pt(	h,	_draw_max_ui[1]					);
		param_set_pt(	h,	_b_ui_draw_ui					);
		param_set_pt(	h,	_b_ui_draw_axe_ui				);
		param_set_pt(	h,	_b_ui_draw_phase_cur_ui			);
		param_set_pt(	h,	_b_ui_draw_curve_ui				);
		param_set_pt(	h,	_b_ui_draw_curve_cur_only_ui	);
		param_set_pt(	h,	_b_ui_draw_last_ui				);
		param_set_pt(	h,	_draw_last_nb					);
		param_set_pt(	h,	_b_ui_draw_point_ui				);
		param_set_pt(	h,	_b_ui_draw_phase_ui				);
		param_set_pt(	h,	_b_ui_color_auto_ui				);
		param_set_pt(	h,	_color_loop_max					);
		param_set_pt(	h,	_b_ui_draw_follow_phase_ui		);
		param_set_pt(	h,	_b_crosshair_draw_ui			);
		param_set_pt(	h,	_crosshair_u					);
		param_set_pt(	h,	_crosshair_v					);

		++h;
			param_set_pt(	h,	_b_grid_draw_ui		);
			param_set_pt(	h,	_grid_min[0]		);
			param_set_pt(	h,	_grid_max[0]		);
			param_set_pt(	h,	_grid_min[1]		);
			param_set_pt(	h,	_grid_max[1]		);
			param_set_pt(	h,	_grid_nb_u			);
			param_set_pt(	h,	_grid_nb_u_sub		);
			param_set_pt(	h,	_grid_nb_v			);
			param_set_pt(	h,	_grid_nb_v_sub		);

		param_set_pt_3(	h,	_ui_size_marker_ui	);
		param_set_pt_3(	h,	_center_ui			);

		++h;
			param_set_pt(		h, _text_line_width	);
			param_set_pt_2(	h, _text_offset		);
			param_set_pt_2(	h, _text_scale		);

	++h;
		param_set_pt( h, _b_ui_intercept_ui );
		param_set_pt( h, _b_ui_lock_u_ui );
		param_set_pt( h, _b_ui_lock_v_ui );
		param_set_pt( h, _b_ui_phase_ui );

	++h;
		param_set_pt( h, _b_grid_lock_u_ui );
		param_set_pt( h, _grid_lock_u );
		param_set_pt( h, _b_grid_lock_v_ui );
		param_set_pt( h, _grid_lock_v );

	++h;
		param_set_pt( h, _b_clear_trig_ui );

	err_param_init_pt( h );
}

void c_bdd_curve_edit::init()
{
	set_v2( _draw_min, REAL(0), REAL(0) );
	set_v2( _draw_max, REAL(1), REAL(1) );
//	_b_begin = true;
	_control_id_cur = 1;
	param_init_with( n_bdd_curve_edit::param, n_bdd_curve_edit::PARAM_NB_MAX );
}

CONSTRUCTOR_CREATE( c_bdd_curve_edit )
,_i_col_auto_cur( 0 )
,_control_phase(nullptr)
,_control_phase_data(nullptr)
,_b_need_save(false)
{
	_p_control_cur = control_get( 1 );
	obj_get( _control_phase_data );
	_control_phase_data->set_name( "The Phase" );
	init();
}

c_bdd_curve_edit::~c_bdd_curve_edit()
{
	if( cur == this )
		cur = nullptr;
	dealloc();
}

void	c_bdd_curve_edit::dealloc()
{
	control_dealloc();
	obj_delete( _control_phase_data );
}

FINLINE	INT32	c_bdd_curve_edit::build_id( INT32 id )
{
	return IMOD( id - 1, _control_nb ) + 1;
}

REAL	c_bdd_curve_edit::control_get_value( INT32 id_in, REAL in )
{
	c_control_key*	p_control = control_get( id_in );
	return p_control ? p_control->get_value( in ) : 0 ;
}

REAL	c_bdd_curve_edit::control_get_value_by_name( C_PCHAR_C name, REAL phase )
{
	c_control_key*	p_control  = control_get_by_name( name );
	return p_control ? p_control->get_value( phase ) : 0 ;
}

REAL	c_bdd_curve_edit::control_get_value_axe( INT32 id_in, REAL in, INT32 axe )
{
	c_control_key*	p_control = control_get( id_in );
	return p_control->get_value( in, axe );
}

REAL	c_bdd_curve_edit::control_get_value( INT32 id_in )
{
	return control_get_value( id_in, _phase_cur );
}

REAL	c_bdd_curve_edit::control_get_value_by_name( C_PCHAR_C name )
{
	return control_get_value_by_name( name, _phase_cur );
}

REAL	c_bdd_curve_edit::control_get_value_axe( INT32 id_in, INT32 axe )
{
	c_control_key*	p_control;
	p_control = control_get( id_in );
	return p_control->get_value( _phase_cur, axe );
}

c_control_key*	c_bdd_curve_edit::control_new()
{
	c_control_key*	p_control = nullptr;
	obj_get( p_control );
	_controls.push_back( p_control );
	return p_control;
}

FINLINE	c_control_key*	c_bdd_curve_edit::control_get_low( INT32 id_in )
{
	auto it = _controls.begin();
	advance( it, id_in );
	return *it;
//	return controls[ CLAMP( INT32(control_in-1), 0, INT32(controls.size()-1) ) ];
}

c_control_key*	c_bdd_curve_edit::control_get_if_exist( INT32 id_in )
{
	if( _controls.empty() || id_in<=0 || ((UINT32)id_in)>_controls.size() )
		return nullptr;
	return control_get_low( id_in-1 );
}


c_control_key*	c_bdd_curve_edit::control_get( INT32 id_in )
{
	if( _controls.empty() )
		control_new();
	id_in = CLAMP( id_in, 1, INT32(_controls.size()) ) - 1;
	return	control_get_low( id_in );
}

c_control_key*	c_bdd_curve_edit::control_get_by_name( C_PCHAR_C name )
{
	for( auto const & p_control : _controls )
	{
		if( str_is_equal_bothnocase( p_control->get_control_name(), name ) )
			return p_control;
	}			
	return nullptr;
}

c_control_key*	c_bdd_curve_edit::control_insert()
{
	c_control_key*	p_control;
	obj_new( p_control );
	//	p_key_cur->copy_to( p_key);
	if( !p_control )
		return nullptr;

	p_control->set_control_type( _type_default );
	p_control->set_phase_start(		_phase_cur );
	p_control->set_phase_stop(		_phase_cur );
	p_control->set_key_phase(		_phase_cur );
	_p_control_cur = p_control;
	_controls.push_back( p_control );

	_control_id_cur = (INT32)_controls.size();

	return p_control;
}

void	c_bdd_curve_edit::control_delete()
{
	if( _controls.size() <= 1 )
		return;

	//_controls.remove( p_control_cur_ ); //ok for list
	_controls.erase( remove( _controls.begin(), _controls.end(), _p_control_cur ), _controls.end() );
	obj_delete( _p_control_cur );	

	_control_id_cur = build_id( _control_id_cur );
}
void	c_bdd_curve_edit::control_dealloc()
{
	for( auto const & elt : _controls )
		delete elt;
	_controls.clear();
}

c_control_key*	c_bdd_curve_edit::control_next()
{
	INT32			index	=	build_id(  _control_id_cur + 1 );
	c_control_key*	control	=	control_get( index );
	_control_id_cur = index;
	return control;
}
c_control_key*	c_bdd_curve_edit::control_prev()
{
	INT32			index	=	build_id(  _control_id_cur - 1 );
	c_control_key*	control	=	control_get( index );
	_control_id_cur = index;
	return control;
}
void	c_bdd_curve_edit::clear()
{
	// clear curve edit, so remove everything, and recreate 1 point
	if( BOX_ASK_WAR( "Confirmation", "Are you sure you want to remove all curves / points ?" ) )
	{
		// 1st remove everything
		control_dealloc();
		_phase_cur = 0;

		// then add a new curve
		control_insert();
	}
}

//
//	FILE
//
namespace {
	CHAR*	str_control_ext = (CHAR*)"000.control_key";
	FINLINE void make_name( o_str& name, INT32 CONST i )
	{
		name.set_digits( -15, 3, i );
	}	
}

AAA_ERR	c_bdd_curve_edit::load_do_after( o_str CONST & filename )
{
#if AAA_STATE_COMPILE()
	if( !c_state_master::is_state_ref() )
		return AAA_OK;
#endif //AAA_STATE_COMPILE

	//prepare name
	o_str&	name = o_str::push_name( filename );

		name.replace_ext( str_control_ext );

		make_name( name, 0 );
		_control_phase_data->load_from_file( name );

		control_dealloc();
		for( INT32 i = 1; i <= _control_nb; ++i )
		{
			make_name( name, i );
			c_control_key* p_control = control_new();
			c_namer::set_file_only( p_control, name );
			p_control->load_from_file( name );
		}
		
	o_str::pop_name();
	channel_register();
	update();	//hack but need it or we lost key save
	return AAA_OK;
}

AAA_ERR	c_bdd_curve_edit::save_do_after( o_str CONST & filename )
{
	if( !_b_need_save )
		return AAA_OK;

	//prepare name
	o_str&	name = o_str::push_name( filename );

		name.replace_ext( str_control_ext );

		make_name( name, 0 );
		_control_phase_data->update();
		_control_phase_data->save_to_file( name );

		INT32 i = 0;
		for( auto const & p_control_key : _controls )
		{
			make_name( name, ++i );
			p_control_key->save_to_file( name );
		}
	
	o_str::pop_name();
	return AAA_OK;
}

void	c_bdd_curve_edit::channel_register()
{
	inst_by_channel.set( _channel_id, this);
}

void	c_bdd_curve_edit::update()
{
	_b_need_save = true;	//hack a little too simple

	channel_register();
	c_bdd_curve_edit::cur = this;
	if( _b_ui_intercept_ui )
		ui_register( this );

	c_key::set_marker_size( _ui_size_marker_ui[0] * _ui_size_marker_ui[2] * REAL(.1) ,
							_ui_size_marker_ui[1] * _ui_size_marker_ui[2] * REAL(.1) );

	if( _b_clear_trig_ui )
	{
		clear();
		_b_clear_trig_ui = false;
	}
	//unused	c_model::cur->get_size_v3( _size );
	cpy_v3( _center, _center_ui );
	if( _controls.empty() )
		control_new();

	cpy_v2( _draw_min, _draw_min_ui );
	cpy_v2( _draw_max, _draw_max_ui );
	ORDER( _draw_min[0], _draw_max[0] );
	ORDER( _draw_min[1], _draw_max[1] );

	//	deal with restart
	if( !_b_phase_control_ui )		
	{	// we get the phase from the last updated bdd_curve_edit
		_control_phase	= control_phase_cur;
		_phase_cur		= phase_cur_cur;
	}
	else
	{	//	this object deal with his phase
		if( _delta_t.update() )
			_b_restart_trig_ui = true;

		_control_phase_data->update();
		_control_phase		= _control_phase_data;
		control_phase_cur	= _control_phase_data;
		if( _b_restart_trig_ui )
		{
			_phase_cur = 0;
			_b_restart_trig_ui = false;
		}
		else
		{
			if( _b_prev_trig || _b_prev_trig_ui )
			{
				_b_play_ui		= !_b_play_ui;
				_b_forward_ui	= false;
				_b_prev_trig	= false;
				_b_prev_trig_ui	= false;
			}
			if( _b_next_trig || _b_next_trig_ui )
			{
				_b_play_ui		= !_b_play_ui;
				_b_forward_ui	= true;
				_b_next_trig	= false;
				_b_next_trig_ui	= false;
			}

			if( _b_play_ui )
			{
				_b_play_trig_ui = false;
				if( _b_stop_trig )
				{
					_b_play_ui			= false;
					_b_stop_trig	= false;
				}
			}
			else
			{
				_b_stop_trig = false;
				if( _b_play_trig_ui )
				{
					_b_play_ui		= true;
					_b_play_trig_ui	= false;
				}
			}

			if( _b_prev_step_trig || _b_prev_step_trig_ui )
			{
				_phase_cur = _control_phase->get_phase_prev( _phase_cur );
				_b_play_ui				= false;
				_b_prev_step_trig		= false;
				_b_prev_step_trig_ui	= false;
			}
			if( _b_next_step_trig || _b_next_step_trig_ui )
			{
				_phase_cur = _control_phase->get_phase_next( _phase_cur );
				_b_play_ui				= false;
				_b_next_step_trig		= false;
				_b_next_step_trig_ui	= false;
			}

			if( _b_play_ui )
			{
				REAL dt = REAL(_delta_t.get_dt());
				if( !_b_forward_ui )
					dt = -dt;
				if( _b_phase_rel_ui )
				{
					bool b_ret = _control_phase->add_phase( _phase_cur, dt, true );
					if( b_ret && _b_stop_on_phase_ui )
						_b_play_ui = false;
				}
				else
					_phase_cur += dt;
				if( _b_loop_ui )
					_phase_cur = _control_phase_data->loop_phase( _phase_cur );
			}
		}
		phase_cur_cur = _phase_cur;
	}

	if( _b_ui_phase_ui && _control_phase )
		_p_control_cur = _control_phase;
	else
		_p_control_cur = control_get( _control_id_cur );

	if( _b_ui_draw_follow_phase_ui )
		_center[0] -= _phase_cur;

	//	for_each( _controls.begin(), _controls.end(), mem_fun( &c_control_key::update ) );
	for( auto const & p_control_key : _controls )
	{
		p_control_key->set_phase( _phase_cur );
		p_control_key->update();
	}

	if( !ui::is_bdd_do() )
	{
		_control_nb = (INT32)_controls.size();
		_control_id_cur = build_id( _control_id_cur );
		if( _b_ui_phase_ui && _control_phase )
			_p_control_cur = _control_phase;
		else
			_p_control_cur = control_get( _control_id_cur );
	
		// change current key, following the phase
		//todo houla _p_control_cur->set_key_cur_by_phase( _phase_cur );
	}

	//todo deformer and normal : look at bdd_uv eventually

//	c_multiple::cur->set_nb( nb_u, nb_v, nb_axe );
}

void	c_bdd_curve_edit::draw_3d()
{
	ERR_PRINT_STRING( "%s{} not implemented yet", __FUNCTION__ );
}

CONST	REAL	GRID_LINE_SIZE = 1.;
CONST	REAL	GRID_SUB_LINE_SIZE = 1.;
CONST	REAL	GRID_ALPHA = 1.;
CONST	REAL	GRID_SUB_ALPHA = REAL(.7);

FINLINE	static	void	draw_grid_u( REAL u_beg, REAL du, REAL u_end, REAL va, REAL vb, REAL alpha, REAL line_width )
{
	GOL::color4( 1,1,1, alpha );
	GOL::set_line_width( line_width );
	draw_grid_line_x( u_beg, du, u_end, va, vb );
}
FINLINE	static	void	draw_grid_v( REAL v_beg, REAL dv, REAL v_end, REAL ua, REAL ub, REAL alpha, REAL line_width )
{
	GOL::color4( 1,1,1, alpha );
	GOL::set_line_width( line_width );
	draw_grid_line_y( v_beg, dv, v_end, ua, ub );
}
void	c_bdd_curve_edit::draw_grid()
{
	if( _grid_nb_u == 0 || _grid_nb_v == 0 )
		return;

	REAL	ua = _grid_min[0];	
	REAL	ub = _grid_max[0];	
	ORDER( ua, ub );
	ua	=	MAX(	ua,	_draw_min[0]	);
	ub	=	MIN(	ub,	_draw_max[0]	);

	REAL	va = _grid_min[1];
	REAL	vb = _grid_max[1];	
	ORDER( va, vb );
	va	=	MAX(	va,	_draw_min[1]	);
	vb	=	MIN(	vb,	_draw_max[1]	);

	// Draw Grid	
	if( _grid_nb_u > 0 )
	{
		REAL step = OVER_ONE_AS_REAL(_grid_nb_u);
		//	draw sub grid
		if( _grid_nb_u_sub > 1 )
			draw_grid_u( ua, step / REAL( _grid_nb_u_sub ), ub, va, vb, GRID_SUB_ALPHA, GRID_SUB_LINE_SIZE );
		//	draw grid
		draw_grid_u( ua, step, ub, va, vb, GRID_ALPHA, GRID_LINE_SIZE );
	}
	
	if( _grid_nb_v > 0 )
	{
		REAL step = OVER_ONE_AS_REAL(_grid_nb_v); 
		//	draw sub grid
		if( _grid_nb_v_sub > 1 )
			draw_grid_v( va, step / REAL(_grid_nb_v_sub ), vb, ua, ub, GRID_SUB_ALPHA, GRID_SUB_LINE_SIZE );
		//	draw grid
		draw_grid_v( va, step, vb, ua, ub, GRID_ALPHA, GRID_LINE_SIZE );
	}
}

void	c_bdd_curve_edit::draw_crosshair()
{
	if( !_b_crosshair_draw_ui	)
		return;
	GOL::color4( 0,1,1, .5 );
	REAL u = interpolate( _draw_min[0], _draw_max[0], _crosshair_u );
	REAL v = interpolate( _draw_min[1], _draw_max[1], _crosshair_v );
	GOL::begin( GL_LINES );
		GOL::vertex2( _draw_min[0], v );
		GOL::vertex2( _draw_max[0], v );
		GOL::vertex2( u, _draw_min[1] );
		GOL::vertex2( u, _draw_max[1] );
	GOL::end();
}

void	c_bdd_curve_edit::draw_control_cur_name()
{
	if( !_p_control_cur )
		return;
	C_PCHAR_C str = _p_control_cur->get_control_name();
	if( str && *str )
	{
		FP32 CONST * p_color;
		if( _b_ui_color_auto_ui )
			p_color = &color_auto[ _i_col_auto_cur * 4 ];
		else
			p_color = _p_control_cur->get_color();
		GOL::color4v( p_color );
		GOL::set_line_width( _text_line_width );
		aaa::alphabet::draw_str_xy( str, _text_offset[0], _text_offset[1], _text_scale[0], _text_scale[1], 0 );
	}
}
void	c_bdd_curve_edit::draw_axes()
{
	GOL::color_red();
	draw_line_x( _draw_min[0], _draw_max[0] );
	GOL::end();

	GOL::color_green();
	draw_line_y( _draw_min[1], _draw_max[1] );
	GOL::end();
}
void	c_bdd_curve_edit::draw_control_phase()
{
// Draw Phase
	bool	b_sel = ( _control_phase == _p_control_cur ) ;

	if( b_sel )
		GOL::set_line_width( 4. );

	if( _b_ui_draw_curve_ui )
		_control_phase->draw_phase( _draw_min[0], _draw_max[0] );
	if( _b_ui_draw_point_ui || _b_ui_intercept_ui )
		_control_phase->draw_ui( b_sel, nullptr, _s_visu_axe, _draw_min[0], _draw_max[0]  );

	if( b_sel )
		GOL::set_line_width( 1. );
}

void	c_bdd_curve_edit::draw_phase_2d()
{
// Draw Phase
// Draw point on curve	
	GOL::set_point_size( 10. );
	GOL::color_white();
	GOL::begin( GL_POINTS );

	REAL vec[2];
	for( auto const & p : _controls )
	{
		if( !_b_ui_draw_curve_cur_only_ui || p == _p_control_cur )
		{
			p->get_value_v2r( vec, _phase_cur );
			GOL::vertex2v(  vec );
		}
	}
	GOL::end();

// Draw phase and keys on a curve
	if( _b_ui_draw_phase_ui )
//		if( !mem::is_pointer_fucked( _p_control_cur ) )
		_p_control_cur->draw_phase_2d();

}

static FP32 cyan_50[4] = { 0., 1., 1., 0.5 };

void	c_bdd_curve_edit::draw_2d()
{
	GOL::matrix::translate3v( _center );
//DRAW GRID
	if( _b_grid_draw_ui )
		draw_grid();
//DRAW_AXE
	if( _b_ui_draw_axe_ui )
		draw_axes();
// DRAW PHASE
	if( _b_phase_control_ui )
	{
		draw_phase_2d();
		draw_control_phase();
	}
//DRAW CURVE
#if 1
	ERR_PRINT_STRING( "%s() crashing for now", __FUNCTION__ );
#else
	GOL::set_line_width( 3. );

	INT32	i = 0;
	for( auto const & p : _controls )
	{
		bool	b_sel =  ( p == _p_control_cur ) ;
		if( !_b_ui_draw_curve_cur_only || b_sel )
		{
			if( b_sel )
				GOL::set_line_width( 4. );

			FP32*	p_color;
			if( _b_ui_color_auto )
			{
				INT32 i_col = IMOD( i++, _color_loop_max );
				p_color = &color_auto[ i_col * 4 ];
				if( b_sel )
					_i_col_auto_cur = i_col;
			}
			else
				p_color = nullptr;

			if( _b_ui_draw_curve_ui )
				p->draw_curve_2d( p_color );
			if( _b_ui_draw_point_ui || _b_ui_intercept )
				p->draw_ui_2d( _b_ui_intercept && b_sel, p_color );
			if( b_sel )
				GOL::set_line_width( 1. );
		}
	}
#endif
	GOL::matrix::translate_negv( _center );
}

void	c_bdd_curve_edit::draw_1d()	
{
	GOL::matrix::translate3v( _center );
//DRAW GRID
	if( _b_grid_draw_ui )
		draw_grid();
//DRAW_AXE
	if( _b_ui_draw_axe_ui )
		draw_axes();
//DRAW PHASE
	if( _b_phase_control_ui )
		draw_control_phase();
//DRAW CURVES
	INT32	i = 0;

	for( auto const & p : _controls )
	{
		bool	b_sel =  ( p == _p_control_cur );
		if( !_b_ui_draw_curve_cur_only_ui || b_sel )
		{
			if( b_sel )
				GOL::set_line_width( 4. );

			FP32 CONST *	p_color;
			if( _b_ui_color_auto_ui )
			{
				INT32 i_col = IMOD( i++, _color_loop_max );
				p_color = &color_auto[ i_col * 4 ];
				if( b_sel )
					_i_col_auto_cur = i_col;
			}
			else
				p_color = nullptr;

			if( _b_ui_draw_curve_ui )
				p->draw_curve( p_color, _s_visu_axe, _draw_min[0], _draw_max[0] );
			if( _b_ui_draw_point_ui )
				p->draw_ui( _b_ui_intercept_ui && b_sel, p_color, _s_visu_axe, _draw_min[0], _draw_max[0] );
			if( b_sel )
				GOL::set_line_width( 1. );
		}
	}

//DRAW PHASE_CUR
	if( _b_ui_draw_phase_cur_ui )
	{
		GOL::color_red();
		draw_line( _phase_cur, _draw_min[1], _phase_cur, _draw_max[1] );
	}
	GOL::matrix::translate_negv( _center );

}

void	c_bdd_curve_edit::draw_single()
{
	if( _b_ui_draw_ui )
	{
		begin_ui();
			switch( _s_visu_curve )
			{
			case CURVE_1D:	draw_1d();	break;
			case CURVE_2D:	draw_2d();	break;
			case CURVE_3D:	draw_3d();	break;
			}
		end_ui();
	}
	draw_control_cur_name();
	draw_crosshair();
}

//	don't do anything
void	c_bdd_curve_edit::draw_multiple()
{
//	c_multiple::cur->init_index_w();
//			c_multiple::cur->next_index_w();

	INT32	nb_to_draw  = (INT32)_controls.size();
	c_multiple::cur->set_nb( nb_to_draw );

	INT32				row =	0;
	REAL CONST * CONST	size = c_multiple::cur->get_size();
	REAL				pos[3];
	clear_v3( pos );	// infact nothing really interesting is drawn here

	INT32	i = 0;
	for( auto const & p : _controls )
	{
		++i;
		bool	b_draw = false;
		// only show curves that are present for current phase
		if( INSIDE( _phase_cur, p->get_phase_start(), p->get_phase_stop() ) )
		{
			if( _b_ui_draw_curve_cur_only_ui )
			{
				if( i == _control_id_cur  )
					b_draw = true;
			}
			else if( _b_ui_draw_last_ui )
			{
				// draw last nb curves
				if( ( (i + _draw_last_nb ) >= _control_id_cur ) && ( i <= _control_id_cur ) )
					b_draw = true;
			}
			else
				b_draw = true;
			if( b_draw )
			{
				c_multiple::cur->set_index( row );
				c_multiple::cur->align_then_draw( pos, size );
			}
			++row;
		}
	}
}

//
//	UI
//
bool	c_bdd_curve_edit::do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y )
{
	if( !_b_ui_intercept_ui )
		return false;

	bool	b_return = false;

//	if( modifier_is_none(*modifiers) )
	if( _p_control_cur->do_key( c, b_special, modifiers, x, y ) )
	{
		_phase_cur = _p_control_cur->get_phase();
		return true;
	}
	else
	{
		b_return = true;
		if( b_special )
		{
			if( modifier::is_ctrl_on( *modifiers ) )
			{
				switch (c)
				{
				case keyboard::INSERT:
					control_insert();
					break;
				default:	b_return = false;		break;
				}
			}
			else
			{
				switch (c)
				{
				case keyboard::DOWN:
					if( modifier::is_shift_on( *modifiers ) )
						control_next();
					else
						control_next();
					break;
				case keyboard::UP:
					if( modifier::is_shift_on( *modifiers ) )
						control_prev();
					else
						control_prev();
					break;
				default:	b_return = false;		break;
				}
			}
		}
		else
		{
			if( modifier::is_ctrl_on( *modifiers ) )
			{
				switch (c)
				{
				case 127:	//del
							control_delete();		break;
				default:	b_return = false;		break;
				}
			}
			else if( modifier::is_alt_on( *modifiers ) )
			{
				switch (c)
				{
				case 127:	//del
							control_delete();		break;
				case keyboard::SPACE:
					if( modifier::is_shift_on( *modifiers ) )
						_b_prev_step_trig = true;
					else
						_b_next_step_trig = true;
					break;
				default:	b_return = false;		break;
				}
			}
			else if( modifier::is_shift_on( *modifiers ) )
			{
				switch (c)
				{
				case keyboard::SPACE:	_b_prev_trig = true;	break;
				default:				b_return = false;		break;
				}
			}
			else
			{
				switch (c)
				{						
				//case 'g':	//find close key
				//	{
				//	if( p_control_cur_ )
				//		p_control_cur_->key_go_phase( _phase_cur );
				//	}
				//	break;
				case 'r':	_b_restart_trig_ui = true;	break;
				//todo	revive
				//case keyboard::SPACE:
				//	b_next_trig_ = true;
				//	break;
				case 'p':
				case 'P':	_b_ui_phase_ui = true;	break;
				case 'c':
				case 'C':	_b_ui_phase_ui = false;	break;
				case 'g':
				case 'G':	_b_grid_lock_u_ui = !_b_grid_lock_u_ui;
							_b_grid_lock_v_ui = _b_grid_lock_u_ui;
							break;
				case 'u':
				case 'U':	_b_ui_lock_u_ui = false;	_b_ui_lock_v_ui = true;		break;
				case 'v':
				case 'V':	_b_ui_lock_u_ui = true;		_b_ui_lock_v_ui = false;	break;
				case 'f':
				case 'F':	_b_ui_lock_u_ui = false;	_b_ui_lock_v_ui = false;	break;
				////case 'a':
				////case 'A':
				////	_b_ui_draw_curve_cur = !_b_ui_draw_curve_cur;
				////	break;
				////case 's':
				////case 'S':
				////	_b_ui_draw_slave = !_b_ui_draw_slave;
				////	break;
				default:	b_return = false;		break;
				}
			}
		}
	}
	return b_return;
}

bool	c_bdd_curve_edit::mouse_down( FP32& u_start, FP32& v_start )
{
	switch( _s_visu_curve )
	{
	case CURVE_2D :		_p_control_cur->mouse_get_start_uv( u_start, v_start );			break;
	case CURVE_1D :
	default :			u_start = _p_control_cur->get_key_phase();
						v_start = _p_control_cur->get_key_value( _s_visu_axe );
						break;
	}
	return true;
}

void	c_bdd_curve_edit::mouse_move( FP32 u_in, FP32 v_in )
{
	switch( _s_visu_curve )
	{
	case CURVE_2D :
		if( _b_grid_lock_u_ui && _grid_lock_u != 0. )
			u_in = ROUND_FLOOR( u_in, _grid_lock_u );
		if( !_b_ui_lock_u_ui )
			_p_control_cur->set_key_value( REAL(u_in), 0 );
		if( _b_grid_lock_v_ui && _grid_lock_v != 0. )
			v_in = ROUND_FLOOR( v_in, _grid_lock_v );
		if( !_b_ui_lock_v_ui )
			_p_control_cur->set_key_value( REAL(v_in), 1 );
		break;
	case CURVE_3D :
	case CURVE_1D :
		if( _b_grid_lock_u_ui && _grid_lock_u != 0. )	
			u_in = ROUND_FLOOR( u_in, _grid_lock_u );
		if( !_b_ui_lock_u_ui )							
			_p_control_cur->set_key_phase( REAL(u_in) );
		if( _b_grid_lock_v_ui && _grid_lock_v != 0. )	
			v_in = ROUND_FLOOR( v_in, _grid_lock_v );
		if( !_b_ui_lock_v_ui )							
			_p_control_cur->set_key_value( REAL(v_in), _s_visu_axe );
		break;
	}
}

void	c_bdd_curve_edit::mouse_up( FP32 u_in, FP32 v_in )
{
	_p_control_cur->mouse_up();
}

