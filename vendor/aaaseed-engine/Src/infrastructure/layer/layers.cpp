
#include "aaa_def.h"
#include "layers.h"	 
#include "draw/picking.h"
#include "draw/stereo.h"
#include "fbo/fbo.h"
#include "infrastructure/bind/bind.h"
#include "infrastructure/layer/layers_att.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/layer/app.h"
#include "infrastructure/param/traxs.h"
#include "infrastructure/viewport.h"
#include "language/lua/aaalua_wrap.h"
#include "media/video/vbl.h"
#include "obj_ui/multi_screen.h"
#include "ui/obj_value.h"
#include "time/speed.h"
#include "time_buf.h"
#include "spy.h"
#include "infrastructure/aaa_const.h"
#include "obj_ui/transfo/transfo_trs.h"
#include "infrastructure/namer.h"
#include "gol/gol.h"
#include "gol/gol_matrix.h"


FACTORY_CREATE_V1( c_layers, layers, Group(Layers), layers_param );

bool		c_layers::b_traxs_update;

c_layers*	c_layers::ui			= nullptr;
c_layers*	c_layers::cur			= nullptr;
c_layers*	c_layers::layers_cam_ui	= nullptr;


bool		c_layers::b_save_one_file;
bool		c_layers::b_load_one_file;

CONSTEXPR	C_PCHAR_C	c_layers::str_layer_letter[LAYER_NB_MAX+1] =
{
	"No",
	"Layer_A", "Layer_B", "Layer_C", "Layer_D", "Layer_E",
	"Layer_F", "Layer_G", "Layer_H", "Layer_I", "Layer_J",
	"Layer_K", "Layer_L", "Layer_M", "Layer_N", "Layer_O",
	"Layer_P", "Layer_Q", "Layer_R", "Layer_S", "Layer_T",
	"Layer_U", "Layer_V", "Layer_W", "Layer_X", "Layer_Y",
	"Layer_Z",
};

namespace
{
	c_layers*	layers_cam_owner = nullptr;

	enum VIEWPORT_SELECTOR : INT32
	{
		VIEWPORT_CURRENT = 0,
		VIEWPORT_LEFT,
		VIEWPORT_ALL,
		VIEWPORT_RIGHT,
		VIEWPORT_SELECTOR_MAX_NB,
	};
	CONSTEXPR C_PCHAR_C str_viewport_selector[VIEWPORT_SELECTOR_MAX_NB] =
	{
		"CURRENT",
		"left",
		"Left_Right",
		"right",
	};

	enum STEREO_SELECTOR : INT32
	{
		STEREO_FIELD_UPDATE = 0,
		STEREO_FIELD_ALL,
		STEREO_FIELD_NOT_UPDATE,
		STEREO_SELECTOR_MAX_NB,
	};
	CONSTEXPR C_PCHAR_C str_stereo_selector[STEREO_SELECTOR_MAX_NB] =
	{
		"field_update",
		"field_all",
		"field_not_update"
	};

	enum FBO_USE : INT32
	{
		FBO_CUR = 0,
		FBO_NONE,
		FBO_OWNER,
		FBO_ONCE,
		FBO_USE_MAX_NB
	};

	enum CAM_USE : INT32
	{
		CAM_CUR = 0,
		CAM_ONCE,
		CAM_OWNER,
		CAM_MAIN,
		CAM_USE_MAX_NB
	};
	CONSTEXPR C_PCHAR_C str_cur_once_owner_main[CAM_USE_MAX_NB] =
	{
		"CURRENT",
		"Once",
		"Owner",
		"main",
	};

}

namespace	n_layers
{
	CONSTEXPR INT32	BASE_PARAM_NB		=	1;
	CONSTEXPR INT32	GROUP_BASE_PARAM_NB	=	8;
	CONSTEXPR INT32	CAMERA_PARAM_NB		=	3;
	//	CONSTEXPR	INT32	TIMING_PARAM_NB		=	1;
#if AAA_STATE_COMPILE()
	CONSTEXPR INT32	MORE_PARAM_NB		=	18;
#else
	CONSTEXPR INT32	MORE_PARAM_NB		=	17;
#endif
	CONSTEXPR INT32	GROUP_PARAM_NB		= 3;
	CONSTEXPR INT32	PARAM_NB_MAX		=	BASE_PARAM_NB
									//		+	TIMING_PARAM_NB
											+	GROUP_BASE_PARAM_NB
											+	CAMERA_PARAM_NB
											+	MORE_PARAM_NB
											+	GROUP_PARAM_NB
											+	c_layers::LAYER_NB_MAX;


	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active				)
		//PARAM_DEF_GROUP_CLOSED( Timing, TIMING_PARAM_NB )
		PARAM_DEF_GROUP_CLOSED(	Base, GROUP_BASE_PARAM_NB )
			PARAM_DEF_SYMBO_MIN_MAX(	viewport,			VIEWPORT_ALL, VIEWPORT_LEFT,			VIEWPORT_LEFT, VIEWPORT_SELECTOR_MAX_NB - 1,	str_viewport_selector	)
			PARAM_DEF_SYMBO_MIN_MAX(	stereo,				STEREO_FIELD_UPDATE, STEREO_FIELD_ALL,	STEREO_FIELD_UPDATE, STEREO_FIELD_NOT_UPDATE,	str_stereo_selector		)
			PARAM_DEF_BOOL_OFF(			lua				)
			PARAM_DEF_NONE(				Values			)
			PARAM_DEF_BOOL_OFF(			Traxs			)
			PARAM_DEF_SYMBO_MIN_MAX(	FBO,			0, 1,	0, 3,	gstr::borrow_once )
			PARAM_DEF_SYMBO_PSTR_ZERO(	use_camera,		str_cur_once_owner_main		)
			PARAM_DEF_BOOL_OFF(			transfo			)

		PARAM_DEF_GROUP_CLOSED(	Group More, MORE_PARAM_NB + 1 + CAMERA_PARAM_NB )
			PARAM_DEF_REF(				name_symbo		)
			PARAM_DEF_STR(				comment			)
			PARAM_DEF_BOOL_OFF(			multiple_only	)
			PARAM_DEF_BOOL_OFF(			skip_trig		)
#if AAA_STATE_COMPILE()
			{	nullptr,	PARAM_BOOL|PARAM_STATE_INSENSITIVE,	"state_sensitive",	1, 0,	0, 1,	nullptr, nullptr },
#endif
			PARAM_DEF_SYMBO_MIN_MAX(	index_cur,			1, 0,	0, c_layers::LAYER_NB_MAX-1,	&c_layers::str_layer_letter[1]	)
//			PARAM_DEF_SYMBO_MIN_MAX(	index_begin,		1, 0,	0, c_layers::LAYER_NB_MAX-1,	&c_layers::str_layer_letter[1]	)
			PARAM_DEF_SYMBO_MIN_MAX(	index_end,			1, 0,	0, c_layers::LAYER_NB_MAX-1,	&c_layers::str_layer_letter[1]	)

			PARAM_DEF_GROUP_CLOSED(		Camera, CAMERA_PARAM_NB )
				PARAM_DEF_INT32(			camera_index_view,	1, 0,	0, c_layers::CAMERA_NB - 1 )
				PARAM_DEF_INT32(			camera_index_ui,	1, 0,	0, c_layers::CAMERA_NB - 1 )
				PARAM_DEF_BOOL_ON(			camera_lock_view_to_ui		)

			PARAM_DEF_INT32(			traxs_channel_offset,	1, 0,	-CHANNEL_NB_MAX, CHANNEL_NB_MAX )
			PARAM_DEF_INT32(			traxs_control_offset,	1, 0,	-CHANNEL_NB_MAX, CHANNEL_NB_MAX )
			PARAM_DEF_BOOL_OFF(			off_at_load				)
			PARAM_DEF_BOOL_OFF(			on_at_load				)
			PARAM_DEF_BOOL_ON(			used_for_picking		)

			PARAM_DEF_NONE(				timing_layers			)

			PARAM_DEF_BOOL_ON(			time_buffer_use			)
			PARAM_DEF_REAL_ONE(			time_buffer_value		)
			PARAM_DEF_BOOL_OFF(			do_gl_finish_at_end		)

			PARAM_DEF_STR_LOCKED(		file_name				)
			PARAM_DEF_INT32_LOCKED(		layers_id_local			)

		PARAM_DEF_BOOL_OFF( Layer_A )
		PARAM_DEF_BOOL_OFF( Layer_B )
		PARAM_DEF_BOOL_OFF( Layer_C )
		PARAM_DEF_BOOL_OFF( Layer_D )
		PARAM_DEF_BOOL_OFF( Layer_E )
		PARAM_DEF_BOOL_OFF( Layer_F )
		PARAM_DEF_BOOL_OFF( Layer_G )
		PARAM_DEF_BOOL_OFF( Layer_H )
		PARAM_DEF_BOOL_OFF( Layer_I )
		PARAM_DEF_BOOL_OFF( Layer_J )
		PARAM_DEF_BOOL_OFF( Layer_K )
		PARAM_DEF_BOOL_OFF( Layer_L )
		PARAM_DEF_BOOL_OFF( Layer_M )
		PARAM_DEF_BOOL_OFF( Layer_N )
		PARAM_DEF_BOOL_OFF( Layer_O )
		PARAM_DEF_BOOL_OFF( Layer_P )
		PARAM_DEF_BOOL_OFF( Layer_Q )
		PARAM_DEF_BOOL_OFF( Layer_R )
		PARAM_DEF_BOOL_OFF( Layer_S )
		PARAM_DEF_BOOL_OFF( Layer_T )
		PARAM_DEF_BOOL_OFF( Layer_U )
		PARAM_DEF_BOOL_OFF( Layer_V )
		PARAM_DEF_BOOL_OFF( Layer_W )
		PARAM_DEF_BOOL_OFF( Layer_X )
		PARAM_DEF_BOOL_OFF( Layer_Y )
		PARAM_DEF_BOOL_OFF( Layer_Z )
	};

	PARAM_DEF_MAKE_INDEX( layer_a );	//replace CONST	INT32	PARAM_INDEX_LAYER = PARAM_NB_MAX - c_layers::LAYER_NB_MAX;
	PARAM_DEF_MAKE_INDEX( lua );
	PARAM_DEF_MAKE_INDEX( values );
	PARAM_DEF_MAKE_INDEX( traxs );
	PARAM_DEF_MAKE_INDEX( fbo );
	PARAM_DEF_MAKE_INDEX( use_camera );
	PARAM_DEF_MAKE_INDEX( camera );
	PARAM_DEF_MAKE_INDEX( transfo );
	PARAM_DEF_MAKE_INDEX( camera_index_view );	
	PARAM_DEF_MAKE_INDEX( camera_index_ui );
	PARAM_DEF_MAKE_INDEX( timing_layers );
	PARAM_DEF_MAKE_INDEX( base );
	PARAM_DEF_MAKE_INDEX( group_more );

	//CONST  int Layer_A_INDEX = []()
	//{
	//	for( int index = 0; index < PARAM_NB_MAX; ++index )
	//	{ 
	//        if( str_is_equal_nocase( param[index].get_name(), "Layer_A" ) )
	//	        return index;
	//	}
	//	return -1;
	//}();
}

INT32	c_layers::layer_get_index_from_param( p_param param )
{
	INT32 index = param->get_id() - 1;
	index -= n_layers::PARAM_INDEX_layer_a;
	return index;
}

//todo is this safe with several instance of layers
namespace
{
	o_str sum_up;
	o_str o_cam;
	o_str o_fbo;
	o_str o_more;
	o_str o_base;
}


void	c_layers::build_sum_up( o_str& o ) CONST
{
	o.erase();
	if( _b_multiple_only_ui )
		o.add( "MO " );

	o_base.erase();
	if( _b_traxs_ui )
		o_base.add( "TRAX " );

	if( is_lua() && _lua )
	{
		o_base.add( _lua->get_script_name() );
		o_base.add_space();
	}
//FBO
	switch( _s_fbo )
	{
	case FBO_CUR:		o_base.add( "fbo " );	break;
	case FBO_NONE:		break;
	case FBO_OWNER:
	case FBO_ONCE:
		o_base.add( (_s_fbo==FBO_OWNER) ? "FBO" : "Fbo" );
		if( is_obj_exist_and_active(_fbo) )
		{
			INT32 bind = _fbo->get_color_attachment_bind(0);
			if( bind>=0 )
				o_base.add( bind );
			else
				o_base.add( "xx" );
		}
		o_base.add_space();
		break;
	}
//CAMERA
	switch( _s_camera_ui )
	{
	case CAM_CUR:		o_base.add( "cam " );	break;							
	case CAM_MAIN:		o_base.add( "ModuleCam " );
						break;
	case CAM_ONCE:		
	case CAM_OWNER:		o_base.add( (_s_camera_ui==CAM_OWNER) ? "CAM_" : "Cam_" );
						o_base.add( get_camera_ui_index() );
						o_base.add_space();
						break;
	}
//TRANSFO
	if( _b_transfo_ui )
		o_base.add( "TRS " );

	o.add( o_base );

//REST
	INT32	i		=	MAX( 0,					_layer_pt.get_index_begin()	);
	INT32	i_end	=	MIN( _i_layer_end,		_layer_pt.get_index_end()	);
	for( ; i <= i_end; ++i )
	{
		c_layer* p_layer = _layer_pt.get(i);
		if( p_layer )
			o.add_char( ( (_b_layer_active[i] && p_layer->is_active()) ? 'A' : 'a' ) + i );
	}
}

#if 1
void	c_layers::param_init_pt_static()
{
	INT32	h=0;

	//	param_next_set_list( h, 18 );
	param_set_pt(	h, get_pt_active()		);

	++h;
		param_set_pt(	h, _s_viewport_selector		);
		param_set_pt(	h, _s_stereo_selector		);
		param_set_pt(	h, _b_lua_ui				);
	//VALUES
	 	++h;
	//TRAXS
		param_set_pt(	h, _b_traxs_ui			);
	//FBO
		param_set_pt(	h, _s_fbo				);
	//CAMERA
		param_set_pt(	h, _s_camera_ui			);
	//TRANSFO
		param_set_pt(	h, _b_transfo_ui		);
	//MORE
	++h;
		param_set_pt( h, get_name_symbo()			);
		param_set_pt( h, get_comment()			);
		param_set_pt( h, _b_multiple_only_ui	);
		param_set_pt( h, _b_skip_trig_ui		);
#if AAA_STATE_COMPILE()
		param_set_pt( h, _b_state_sensitive_ui	);
#endif
		param_set_pt( h, _i_layer_cur		);
//		param_set_pt( h, _i_layer_begin		);
		param_set_pt( h, _i_layer_end		);

		++h;
			param_set_pt( h, _i_camera_view				);
			param_set_pt( h, _i_camera_ui					);
			param_set_pt( h, _b_camera_lock_view_to_ui	);

		param_set_pt(	h, _traxs_channel_offset		);
		param_set_pt(	h, _traxs_control_offset		);
		param_set_pt(	h, _b_off_at_load_ui			);
		param_set_pt(	h, _b_on_at_load_ui				);
		param_set_pt(	h, _b_used_for_picking_ui		);

		++h; // param_attach_obj(	h, _speed_layers_ui			);

			param_set_pt(	h, _b_time_buffer_include_ui	);
			param_set_pt(	h, _time_buffer_value			);
			param_set_pt(	h, _b_do_gl_finish_at_end_ui	);

			param_set_pt(	h, get_name_search()	);
			param_set_pt(	h, _id_local_ui			);

	for( INT32 i = 0; i < LAYER_NB_MAX; ++i )
		param_set_pt(	h, _b_layer_active[i]	);

	set_param_nb_used(h);
	err_param_init_pt(h);
}
//todo separate needed for coherency and for ui
void	c_layers::param_init_pt()
{
// LUA	
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_lua,					(c_obj_ui*) _lua			);
// VALUES
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_values,				(c_obj_ui*) _values			); // values
// TRAXS
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_traxs,				_traxs						);
// FBO
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_fbo,					_fbo						);
// USE_CAMERA
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_use_camera,			get_camera_view()			);
// TRANSFO
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_transfo,				_transfo					);
// CAMERA
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_camera_index_view,	get_camera(_i_camera_view)	);
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_camera_index_ui,		get_camera_ui()				);
// SPEED
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_timing_layers,		_speed_layers_ui			);

	INT32 h = n_layers::PARAM_INDEX_layer_a;
	for( INT32 i = 0; i < LAYER_NB_MAX; ++i )
		param_attach_obj( h, _layer_pt.get( i ) );
}

void	c_layers::prepare_for_ui()
{
	build_sum_up( sum_up );

	INT32 h=0;
// GROUP_BASE
	get_param( n_layers::PARAM_INDEX_base )->set_comment( o_base );
// LUA	
	if( _lua )
		get_param( n_layers::PARAM_INDEX_lua )->set_comment( _lua->get_script_name() );
// VALUES
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_values, (c_obj_ui*) _values	); // values
// TRAXS
	param_attach_obj_no_inc(	n_layers::PARAM_INDEX_traxs, _traxs	 );
// FBO
	c_fbo*	fbo;
	switch( _s_fbo )
	{
	case FBO_OWNER:
	case FBO_ONCE:		fbo = _fbo;		break;
	default:			fbo = nullptr;	break;	// maa had to be careful with the init
	}
	if( fbo )
		fbo->build_sumup( o_fbo );
	else
		o_fbo.erase();
	get_param( n_layers::PARAM_INDEX_fbo )->set_comment(	o_fbo					);
// USE_CAMERA
	switch( _s_camera_ui )
	{
	case CAM_CUR:	o_cam.erase();				break;
	case CAM_MAIN:	o_cam.set( "ModuleCam" );	break;
	case CAM_OWNER:
	case CAM_ONCE:	o_cam.set( (_s_camera_ui==CAM_OWNER) ? "CAM_" : "Cam_" );
					o_cam.add( get_camera_ui_index() );
					break;
	}
	get_param( n_layers::PARAM_INDEX_use_camera )->set_comment(	o_cam );
// GROUP_MORE
	o_more.erase();
	o_more.add( _comment );
	o_more.add_space();
	o_more.add( get_name_symbo() );
	o_more.add_space();
	if( _b_multiple_only_ui )
		o_more.set( "MO " );
	get_param( n_layers::PARAM_INDEX_group_more )->set_comment(	o_more );

// CAMERA
	get_param( n_layers::PARAM_INDEX_camera)->set_comment( o_cam );
// SPEED
	_speed_layers->build_comment( sum_up, true, false );
	get_param(n_layers::PARAM_INDEX_timing_layers)->set_comment( sum_up );
	_speed_layers_ui->update(); //todo in param_init_pt

	h = n_layers::PARAM_INDEX_layer_a;
	for( INT32 i = 0; i < LAYER_NB_MAX; ++i )
	{
		auto p_layer = _layer_pt.get( i );
		auto param = get_param(h);
		if( p_layer )
		{
			p_layer->build_sum_up( sum_up );
			param->set_comment( sum_up );
		}
		else
			param->clear_comment();
		++h;
	}

	//SUPER::prepare_for_ui();
	set_param_nb_used( n_layers::PARAM_INDEX_layer_a + MIN( _i_layer_end + 1, LAYER_NB_MAX ) );	//	MIN is a secu
}

#else

void	c_layers::param_init_pt()
{
	INT32	h=0;

	//	param_next_set_list( h, 18 );
	param_set_pt(	h, get_pt_active()						);

	build_sum_up( sum_up );
	get_param(h)->set_comment( o_base );
	++h;
		param_set_pt(	h, _s_viewport_selector					);
		param_set_pt(	h, _s_stereo_selector					);
		param_attach_obj_no_inc(	h, (c_obj_ui*) _lua		);
		if( _lua )
			get_param(h)->set_comment( _lua->get_script_name() );
		param_set_pt(				h, _b_lua_ui				);

		param_attach_obj(			h, (c_obj_ui*) _values	);
		param_attach_obj_no_inc(	h, _traxs				);
		param_set_pt(				h, _b_traxs_ui			);
	//FBO
		c_fbo*	fbo;
		switch( _s_fbo )
		{
		case FBO_OWNER:
		case FBO_ONCE:		fbo = _fbo;		break;
		default:			fbo = nullptr;	break;	// maa had to be careful with the init
		}
		if( fbo )
			fbo->build_sumup( o_fbo );
		else
			o_fbo.erase();
		param_attach_obj_no_inc(	h, _fbo					);
		get_param(h)->set_comment(	o_fbo					);
		param_set_pt(				h, _s_fbo				);
	//CAMERA
		switch( _s_camera_ui )
		{
		case CAM_CUR:	o_cam.erase();				break;
		case CAM_MAIN:	o_cam.set( "ModuleCam" );	break;
		case CAM_OWNER:
		case CAM_ONCE:	o_cam.set( (_s_camera_ui==CAM_OWNER) ? "CAM_" : "Cam_" );
						o_cam.add( get_camera_ui_index() );
						break;
		}
		param_attach_obj_no_inc(	h, get_camera_view()	);
		get_param(h)->set_comment(	o_cam					);
		param_set_pt(				h, _s_camera_ui			);
	//TRANSFO
		param_attach_obj_no_inc(	h, _transfo				);
		param_set_pt(				h, _b_transfo_ui		);
	//MORE
		o_more.erase();
		o_more.add( _comment );
		o_more.add_space();
		o_more.add( get_name_symbo() );
		o_more.add_space();
		if( _b_multiple_only_ui )
			o_more.set( "MO " );
		get_param(h)->set_comment(	o_more			);

		++h;
			param_set_pt( h, get_name_symbo()			);
			param_set_pt( h, get_comment()			);
			param_set_pt( h, _b_multiple_only_ui	);
			param_set_pt( h, _b_skip_trig_ui		);
#if AAA_STATE_COMPILE()
			param_set_pt( h, _b_state_sensitive_ui	);
#endif
//			param_set_pt( h, _i_layer_begin		);
			param_set_pt( h, _i_layer_end		);
			param_set_pt( h, _i_layer_cur		);
			get_param(h)->set_comment( o_cam );

			++h;
				param_attach_obj_no_inc(	h, get_camera(_i_camera_view )	);
				param_set_pt(				h, _i_camera_view				);
				param_attach_obj_no_inc(	h, get_camera_ui()				);
				param_set_pt(				h, _i_camera_ui					);
				param_set_pt(				h, _b_camera_lock_view_to_ui	);

			param_set_pt(	h, _traxs_channel_offset		);
			param_set_pt(	h, _traxs_control_offset		);
			param_set_pt(	h, _b_off_at_load_ui			);
			param_set_pt(	h, _b_on_at_load_ui				);
			param_set_pt(	h, _b_used_for_picking_ui		);

			_speed_layers->build_comment( sum_up, true, false );
			get_param(h)->set_comment( sum_up );
			_speed_layers_ui->update();
			param_attach_obj(	h, _speed_layers_ui			);

			param_set_pt(	h, _b_time_buffer_include_ui	);
			param_set_pt(	h, _time_buffer_value			);
			param_set_pt(	h, _b_do_gl_finish_at_end_ui	);

			param_set_pt(	h, get_name_search()	);
			param_set_pt(	h, _id_local_ui			);

		for( INT32 i = 0; i < LAYER_NB_MAX; ++i )
		{
			auto p_layer = _layer_pt.get( i );
			if( p_layer )
			{
				p_layer->build_sum_up( sum_up );
				get_param(h)->set_comment( sum_up );
			}
			else
				get_param(h)->clear_comment();
			param_attach_obj_no_inc(	h, p_layer );
			param_set_pt(				h, _b_layer_active[i]	);
		}

	set_param_nb_used(h);
	err_param_init_pt(h);
}

void	c_layers::prepare_for_ui()
{
	SUPER::prepare_for_ui();
	set_param_nb_used( n_layers::PARAM_INDEX_layer_a + MIN( _i_layer_end + 1, LAYER_NB_MAX ) );	//	MIN is a secu
}
#endif

void	c_layers::set_active( bool CONST in )
{
	if( is_active() != in )
		c_obj_active_ui::set_active( in );
}

//todo add an index check here and/or layer_new()
c_layer*	c_layers::layer_get_always_from_index( INT32 index )
{
	c_layer*	p = _layer_pt.get(index);
	if( !p )
	{
		obj_new( p );
		p->set_id_local( index + 1 );
		_layer_pt.set( index, p );
	}
	return	p;
}


c_obj_ui*	c_layers::get_obj_sub_by_index( INT32 CONST index ) CONST
{
	return layer_get_from_index( index );
}


c_layer*	c_layers::layer_new( INT32 index )
{
	c_layer*	layer;
	if( _layer_pt.get(index) )
	{
		layer = nullptr;
		box_err( "The current layer already exist" );
	}
	else
		layer = layer_get_always_from_index( index );
	return layer;
}

//todonow clean this
//todonow should be async
void	c_layers::layer_forget( INT32 index )
{
	_layer_pt.delete_obj(index);
	//ui->set_focus();
	// 2023 April we had a crash when forgetting layer so we leave less trace
	param_attach_obj_no_inc( n_layers::PARAM_INDEX_layer_a + index, nullptr );
}

bool	c_layers::swap_layer( c_param* to_move, INT32 inc )
{
	//	get	indexes to swap
	INT32 i_src = layer_get_index_from_param(to_move);
	if( !_layer_pt.check_index(i_src) )
		return false;
	INT32 i_dst = i_src + inc;
	if( !_layer_pt.check_index(i_dst) )
		return false;

	_layer_pt.swap( i_src, i_dst );
	SWAP( _b_layer_active[i_src], _b_layer_active[i_dst] );	

	//swap params done thru set_layer_index
	//	swap_param( n_layers::PARAM_INDEX_layer_a + i_src, n_layers::PARAM_INDEX_layer_a + i_dst );
	swap_param_expand( n_layers::PARAM_INDEX_layer_a + i_src+1, n_layers::PARAM_INDEX_layer_a + i_dst+1 );

	//set index for name and param_header text will be adjusted
	if( auto pt = _layer_pt.get(i_dst) )
		pt->set_id_local( i_dst+1 );
	else
	{
		p_param param = get_param( i_dst + n_layers::PARAM_INDEX_layer_a );
		param->clear_comment();
	}
	if( auto pt = _layer_pt.get(i_src) )
		pt->set_id_local( i_src+1 );
	else
	{
		p_param param = get_param( i_src + n_layers::PARAM_INDEX_layer_a );
		param->clear_comment();
	}

	return true;
}

c_layer*	c_layers::layer_load( INT32 index, o_str CONST & filename )
{
	SPY_PUSH_RANGE2( "layer_load", spy::LAYER, filename );

		//todo	solve dynamic load better that with this
		c_layer* layer_cur = c_layer::get_cur();	//we do that or map	will crash on load but now it crash later if it is a deleted layer
			c_layer::set_ui_null();

			c_layer* p = layer_get_always_from_index( index );
			c_namer::set_file_only( p, filename );
			//todo be smarter and relative
			p->load_from_existing_file( filename );
		//	DBG_HEAP_IS_CORRUPT();
		c_layer::set_cur( layer_cur );

	SPY_POP_RANGE2();
	return p;
}

AAA_ERR		c_layers::layer_save_to_file( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
	filename.replace_fname( "fx__" );
	SPY_PUSH_RANGE2( "layer save all", spy::LAYER, filename );

		for( UINT32 i = 0; i < LAYER_NB_MAX; ++i )
		{
			c_layer*	p_layer = _layer_pt.get(i);
			if( p_layer )
			{
				filename.set_char( -1, 'a' + i );
				p_layer->save_to_file( filename );
			}
		}

	o_str::pop_name();
	SPY_POP_RANGE2();
	return	AAA_OK;
}

AAA_ERR		c_layers::layer_load_from_file( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
	filename.add_ext( "layers" );
	SPY_PUSH_RANGE2( "layer load all", spy::LAYER, filename );

		c_bind*	bind = nullptr;
		if( c_file::is_exist(filename) )
		{
			obj_get( bind );
			if( bind )
			{
				bind->set( LAYER_NB_MAX, "Layer Bind", "layers", aaa::file::TYPE_IO_NONE, 1 );
				bind->load_from_existing_file( filename );
			}
			else
				box_err( "can't allocate a c_layers" );
		}

		o_str CONST & layer_ext	= c_layer::the_factory().get_file_ext();
	
		if( bind )
		{
			o_str& full = o_str::push_name();
				for( UINT32 i = 0; i < LAYER_NB_MAX; ++i )
				{		
					o_str CONST & o = bind->get_o_str(i);
					if( !o.is_empty() )
					{
						full.set( filename );
						full.replace_fname( o );
						full.add_ext( layer_ext );
						if( c_file::is_exist(full) )
							layer_load( i, full );
					}
				}
			o_str::pop_name();
			delete bind;
		}
		else
		{
			filename.replace_fname( "fx__" );
			INT32 pos = filename.get_len()-1;
			filename.add_ext( layer_ext );

			for( INT32 i = 0; i < LAYER_NB_MAX; ++i )
			{
				filename.set_char( pos, 'a' + i );
				if( c_file::is_exist(filename) )
					layer_load( i, filename );
			}
		}

	SPY_POP_RANGE2();
	o_str::pop_name();
	return	AAA_OK;
}

void	c_layers::init()
{
	SPY_PUSH_RANGE_OBJ( "layers::init()", spy::LAYERS );

		create_cameras();
		_i_camera_view		= 0;
		_i_camera_ui		= 0;
		obj_get( _values );

		_b_timed			= false;

		_i_layer_end		= 0;
		_speed_layers		= new c_speed( true, 2, g_speed_master->get_pb_print_group(), "total", this  );
		obj_get( _speed_layers_ui );
		_speed_layers_ui->set_timing( _speed_layers );

	SPY_POP_RANGE2();
}

void	c_layers::deinit()
{
	obj_delete( _traxs );
	obj_delete( _lua );
	obj_delete( _cameras );
	obj_delete( _transfo );
	obj_delete( _values );
	obj_delete( _fbo );

	_layer_pt.delete_objs();

	obj_delete( _speed_layers_ui );
	obj_delete( _speed_layers );
}

CONSTRUCTOR_CREATE( c_layers )
	,_layer_pt(	LAYER_NB_MAX )
	,_id_local_ui		{0}
	,_values			{nullptr}
	,_lua_recursion		{0}	
	,_lua				{nullptr}
	,_traxs				{nullptr}
	,_cam_used			{nullptr}
	,_cam_prev			{nullptr}
	,_cameras			{nullptr}
	,_b_transfo_done	{false}
	,_transfo			{nullptr}
	,_b_skip_rest_trig	{false}
	,_fbo				{nullptr}
	,_speed_layers		{nullptr}
	,_speed_layers_ui	{nullptr}	
{
	if( is_obj_first() )
		c_layers_att::c_init();
	init();

	//DBG_PRINT_STRING( "Constant is %d",  n_layers::PARAM_INDEX_layer_a );
	param_init_with( n_layers::param, n_layers::PARAM_NB_MAX ); // layers_param, LAYERS_PARAM_NB_MAX);
}

namespace { 
	o_str obj_name( "Group_" );
	o_str param_prefix( "g" );
}

void	c_layers::set_id_local( INT32 CONST id_local )
{
	_id_local_ui = id_local;

	obj_name.drop_at( 6 );
	obj_name.add( id_local );
	//strnum::make( str_layers_name+6, 2, id_local );
	set_name( obj_name.get() );

	make_param_header_name();
}

o_str CONST & c_layers::make_param_prefix( INT32 index )
{
	param_prefix.drop_at( 1 );
	param_prefix.add( index + 1 );
	return param_prefix;
}

c_layers::~c_layers()
{
	deinit();
	if( is_obj_first() )
		c_layers_att::c_deinit();

	//maa : I added it but I am not sure it is not already done
	if( get_cur() == this )
		set_cur_null();
	if( get_ui() == this )
		set_ui_null();
	if( get_layers_cam_ui() == this )
		set_layers_cam_ui( nullptr );
}

//todo this will need to be refined
void	c_layers::draw_fbo_and_cam()
{
	bool	b_refresh_cam = false;

//______________	
//	FBO			
	if( GOL::b_fbo_do )
	{
		c_fbo* fbo;
		switch( _s_fbo )
		{
		case FBO_CUR :
			fbo = c_fbo::get_cur();
			break;
		case FBO_ONCE :	// we deal with this in draw_end()
		case FBO_OWNER :
			obj_get( _fbo );
			fbo =  _fbo->is_active() ? _fbo : nullptr;
			break;
		default:
			debug_break( "%s() fbo type unknown : %d", __FUNCTION__, _s_fbo );
		case FBO_NONE :	// None
			fbo = nullptr;
			break;
		}

		if( fbo )
			//todo this should be cleaner
			//hack	the once owner current strategy is not totally clear
		{
			if( _b_time_buffer_include_ui && c_tbuf::is_active() )
			{
				c_tbuf::add( tbuf::CH_FBO_RENDER, 1, "FBO", this );
				_b_timed = true;
			}
			if( c_fbo::get_cur() != fbo )
			{
				fbo->update();
				if( fbo->is_ready_to_use() )
				{
					fbo->draw();
					//todomona decide at some point because for example we redraw the axis at each fbo
					//todoqqq if we change viewport we need to update cam
					//todoqqq fbo / camera / viewport strategy
					b_refresh_cam = true;
					fbo->clear();
				}
			}
		}
		else
			c_fbo::set_cur( nullptr );
	}

//______________	
//	CAM			
//	then we find the camera
	c_seedcam* cam_cur = c_seedcam::get_cur();
	//c_seedcam* cam_prev = c_seedcam::get_prev();
	//if( cam_prev and cam_cur != cam_prev )
	//{
	//	c_seedcam::set_prev( nullptr );
	//	cam_cur->update_ubo();
	//}

	c_seedcam* cam;
	switch( _s_camera_ui )
	{
	case CAM_CUR:
		cam = cam_cur;
		if( cam )
			break;	//if we got it fine we are done
		//else we use the module cam
	case CAM_MAIN:
		cam = c_module::get_cur()->get_camera();
		break;
	case CAM_ONCE:
		_cam_prev = cam_cur;
	case CAM_OWNER:
		cam = get_camera_view();
		break;
	}

	//	and update it if we need to
	if( b_refresh_cam || cam != c_seedcam::get_cur() )
	{
		layers_cam_owner = this;
		cam->update();

		if( cam->is_axe_visible() )
		{
			switch( g_app->get_axe_visible() )
			{
			case 1:		if( c_layers::get_layers_cam_ui() == this )
			case 2:		{
							g_app->axe_draw();
						}
				  break;
			}
		}
	}
	_cam_used = cam;
}

bool	c_layers::draw_begin() // TRAXS FBO CAM TRANSFO
{
	_b_timed = false;
//	mem::DBG_HEAP_CHECK();
	//	we update the traxs first so they can change all the rest
	//	except lua done before 
	if( _b_traxs_ui && _traxs && is_traxs_update() )
	{
		//TODO	make ui is better and autoconnect work fine
		_traxs->update( _traxs_channel_offset, _traxs_control_offset );
	}
	if( _b_skip_trig_ui )
	{
		_b_skip_trig_ui = false;
		return false;
	}

	draw_fbo_and_cam();

	//	if( c_seedcam::get_ui() == nullptr && this == c_layers::get_ui() )
	if( this == c_layers::get_ui() )
	{
		//todo we need better for lua MU
		c_layers::set_layers_cam_ui( layers_cam_owner );
		//2025 June M�a removed to avoid problem with APP_GP
		// c_seedcam::set_ui( _cam_used );
		//if( _s_camera_ui > 0 )		
		//	c_seedcam::set_ui( get_camera_ui() );
		//else
		//	c_seedcam::set_ui( c_seedcam::get_cur() );	//todo should be refine we need the current ui cam 
	}

	if(	!c_viewport::is_valid_cur() )
		return false;

	if (	( viewport_ref != viewport_render )
		&&	( viewport_ref != viewport_fbo )
		&&	( c_multi_screen::cur->get_index() != 0 )
		)	//todoqqq check multi_screen and fbo
	{
		return false;
	}

	if( _b_transfo_ui )
	{
		obj_get( _transfo );
		_transfo->update();
		if( _transfo->is_todo() )
		{
			GOL::matrix::push();
			_transfo->do_it_low();
			_b_transfo_done = true;
		}
	}
	return true;
}

void	c_layers::draw_end()
{
	if( _b_transfo_done )
	{
		GOL::matrix::pop();
		_b_transfo_done = false;
	}

	if( _cam_prev )
	{
		_cam_prev->switch_to();
		_cam_prev = nullptr;
	}

	if( GOL::b_fbo_do )
	{
		switch( _s_fbo )
		{
		case FBO_ONCE:
			c_fbo::set_cur( c_fbo::get_prev() );
			break;
		}
	}

	if( _b_timed )
	{
		c_tbuf::add( tbuf::CH_FBO_RENDER, 0., nullptr, this );
		_b_timed = false;
	}
}

FINLINE	void	c_layers::draw_layer_all()
{
	INT32	i		=	MAX(	0,				_layer_pt.get_index_begin()	);
	INT32	i_end	=	MIN(	_i_layer_end,	_layer_pt.get_index_end()	);
	for( ; i <= i_end; ++i )
	{
		if( _b_layer_active[i] )
		{
			c_layer*	p_layer = _layer_pt.get(i);
			if( is_obj_exist_and_active( p_layer ) )
				p_layer->update_then_draw();
			else
			{	//done also in layer::update ( maa try it to deal with bdd:ui for gabu )
				if( p_layer == c_layer::get_ui() && !c_picking::is_cur() )
					p_layer->set_ui_with_cur();
			}
		}
	}
}

FINLINE	void	c_layers::draw_common( bool b_vbl )
{
	if( _s_stereo_selector != STEREO_FIELD_ALL )
	{
		if( (_s_stereo_selector == STEREO_FIELD_UPDATE) != g_stereo->is_field_for_update() )
			return;
	}

	//	Ok we work on this one now
	set_cur( this );
	SPY_PUSH_RANGE_OBJ( "layers::draw", spy::LAYERS );
	
#if AAA_VBL_USE()
		if( b_vbl )
			vbl::update();
#endif

		if( _b_lua_ui )
		{
			obj_get( _lua )->update();
			if( _lua->is_skip_rest() )
				_b_skip_rest_trig = true;
		}

		if( _b_skip_rest_trig )
			_b_skip_rest_trig  = false;
		else
		{
			//GOL::print_errors( "before draw_common()" );
			if( draw_begin() )
				draw_layer_all();
			draw_end();
			GOL::check_error_debug( "after draw_common()" );
		}

		if( _b_do_gl_finish_at_end_ui )
			GOL::finish();

	SPY_POP_RANGE2();
}

void	c_layers::draw_single_low()
{
	if( !c_multi_screen::cur )
	{
		dbg_print( "%() No c_multi_screen::cur", __FUNCTION__ );
		draw::set_render_first_pass( true ); 
		draw_common( true );
	}
	else if( c_multi_screen::cur->get_nb() == 1 )
	{
		draw::set_render_first_pass( true ); 
		draw_common( true );
	}
	else if( _s_viewport_selector == VIEWPORT_ALL || _s_viewport_selector == VIEWPORT_CURRENT )
	{
		draw::set_render_first_pass( c_multi_screen::cur->get_index() == 0 ); 
		draw_common( true );
	}
	else if( _s_viewport_selector == VIEWPORT_LEFT )
	{
		if( c_multi_screen::cur->get_index() == 0 )
		{
			draw::set_render_first_pass( true ); 
			draw_common( true );
		}
	}
	else if( _s_viewport_selector == VIEWPORT_RIGHT )
	{
		if( c_multi_screen::cur->get_index() == 1 )
		{
			draw::set_render_first_pass( true ); 
			draw_common( true );
		}
	}
}

void	c_layers::draw_multiple_low()
{
	bool b_vbl = (c_multiple::cur->get_index() & 0x3f ) == 0x3f;
	if( _b_time_buffer_include_ui )
	{
		TBUF_ADD_OBJ( tbuf::CH_DRAW_LAYERS, _time_buffer_value, "layers multiple", this );
		draw_common( b_vbl );
		TBUF_ADD_OBJ( tbuf::CH_DRAW_LAYERS, 0., nullptr, this );
	}
	else
		draw_common( b_vbl );

}

FINLINE	void	c_layers::draw_tbuf_mess_low( REAL r, C_PCHAR str )
{
	_speed_layers->begin();
		if( _b_time_buffer_include_ui && c_tbuf::is_active() )
		{
			c_tbuf::add( tbuf::CH_DRAW_LAYERS, _time_buffer_value, str, this );
			draw_single_low();
			c_tbuf::add( tbuf::CH_DRAW_LAYERS, 0., nullptr, this );
		}
		else
		{
			draw_single_low();
		}
	_speed_layers->end();
}

bool	c_layers::update_then_draw_lua()
{
	//update();		//	small opt this is an empty virtual fn (2013) for now
	if( is_active() )
	{
		if( _lua_recursion >= 16 )
		{
			set_ref_error( "layers recursion max allowed is 16" );
			return false;
		}
		++_lua_recursion;
		draw_tbuf_mess_low( 1., "layers by lua" );
		--_lua_recursion;
	}
	return true;
}

void	c_layers::traxs_draw()
{
	if( is_active() && _b_traxs_ui && _traxs )	//todoqq b_traxs should be in trax ?
		_traxs->draw();
}

void	c_layers::traxs_set_focus()
{
	IF_THIS_NULL_RETURN();
	_traxs->set_focus();
}

CHAR	g_str_local_traxs[] = "Local Traxs";

void	c_layers::create_traxs()
{
	obj_get( _traxs );
	if ( !_traxs )
	{
		box_err( "can't allocate traxs for c_layers" );
	}
	else
	{
		_traxs->set_name( g_str_local_traxs );
		_traxs->set_nb( TRAX_BY_LAYERS );
	}
}

c_traxs*	c_layers::get_traxs_always()
{
	if( !_traxs )
		create_traxs();
	return _traxs;
}

c_trax*		c_layers::traxs_plug_out( INT32 index, c_obj_ui* obj, p_param param )
{
	return this ? get_traxs_always()->trax_plug_out( index, obj, param ) : nullptr ;
}

c_trax*		c_layers::traxs_plug_in( INT32 index, c_obj_ui* obj, p_param param )
{
#if AAA_DEBUG()
	DBG_PRINT_STRING("%s() will do on param %s", __FUNCTION__, param->get_name().get() );
#endif
	return this ? get_traxs_always()->trax_plug_in( index, obj, param ) : nullptr ;
}

void	c_layers::create_cameras()
{
	obj_get( _cameras );
	if( !_cameras )
	{
		box_err( "can't allocate cameras for c_layers" );
	}
	else
	{
		_cameras->set_nb( CAMERA_NB );
		_cameras->set_name( "Local Cameras" );
		_cameras->set_root( this );
	}
}

/*
FINLINE	c_cameras*	c_layers::get_cameras()
{
	if( !_cameras )	{	create_cameras();	}
	return _cameras;
}
*/

namespace {	CHAR	layers_base_text[] = "  P00 ";	}

CHAR*	c_layers::get_menu_header()
{
	*(layers_base_text+1) = ( this == c_layers::get_ui() ) ? '+' : ' ';
	return layers_base_text;
}

namespace {	CONST CHAR  transfo_ext[] = "transfo";	}

//aqua	deal with replace ext
AAA_ERR	c_layers::save_to_file_private( o_str CONST & filename_in )
{
	SPY_PUSH_RANGE2( "save :", spy::LAYERS, filename_in );
	cur = this;
	o_str& filename = o_str::push_name();

		//	c_file::dir_change_to_def();
		filename.set_fname_relative_ext_no( filename_in );

		if_obj_save_add_ext( _lua,		filename );
		if( _traxs )
			_traxs->save_to_file_add_ext(	filename );
		_values->save_to_file_add_ext(	filename );
	
		c_obj_ui::save_to_file_add_ext( filename );

		_cameras->save_to_file_add_ext(	filename );
		if_obj_save_add_ext( _fbo,		filename );

		//optimize
		if( _transfo )
		{
			filename.add_ext( transfo_ext );
			_transfo->save_to_file(		filename );
		}
		filename.drop_ext();
		layer_save_to_file(				filename );

	o_str::pop_name();
	cur = nullptr;	//voir param_load
	SPY_POP_RANGE2();
	return	AAA_OK;
}

//aqua	deal with replace ext
AAA_ERR	c_layers::load_from_file_private( o_str CONST & filename_in )
{
	SPY_PUSH_RANGE2( "load :", spy::LAYERS, filename_in );
	AAA_ERR	retcode;
	cur = this;
	o_str& filename = o_str::push_name();

		//	c_file::dir_change_to_def();
		filename.set_fname_relative( filename_in );
		//should load first

		get_traxs_always();	//	make sure we have a traxs object
		try_obj_load_add_ext( _lua, filename );
		_traxs->load_from_file_add_ext( filename );
		_values->load_from_file_add_ext( filename );

		//hack	retcode is a hack here
		retcode = c_obj_ui::load_from_file_add_ext( filename );	//todo load_before and load_after is more standard ?
															//	but empty layer file will skip reading the rest 
															//calling with o_str will loop
		_cameras->load_from_file_add_ext( filename );

		try_obj_load_add_ext( _fbo, filename );

		filename.add_ext( transfo_ext );	//opt test before ?
			try_obj_load_with_this_filename( _transfo, filename );
			if( _transfo )
				c_namer::set_file_forced( _transfo, filename );
		filename.drop_ext();

		layer_load_from_file( filename );

		_i_layer_end = MIN( _i_layer_end, _layer_pt.get_index_end() );

		if( _b_off_at_load_ui )
			set_active( false );
		if( _b_on_at_load_ui )
			set_active( true );
		
	o_str::pop_name();
	cur = nullptr;	//voir param_load
					//todoqq should be done on loads of obj ?
	SPY_POP_RANGE2();
	return	retcode;
}


//
//	VFILE
//
namespace { C_PCHAR_C vfile_ext = "aaa_layers_all"; }

AAA_ERR		c_layers::save_to_vfile(	o_str CONST & filename_in )
{
	o_str& vfilename = o_str::push_name(filename_in);
		vfilename.add_ext( vfile_ext );
		c_file::open_vfile_save( vfilename );
			AAA_ERR err = save_to_file_private( filename_in );
		c_file::close_vfile();
	o_str::pop_name();
	return err;
}

AAA_ERR		c_layers::load_from_vfile(	o_str CONST & filename_in )
{
	o_str& vfilename = o_str::push_name(filename_in);
		vfilename.add_ext( vfile_ext );
		c_file::open_vfile_load( vfilename );
			AAA_ERR err = load_from_file_private( filename_in );
		c_file::close_vfile();
	o_str::pop_name();
	return err;
}

AAA_ERR		c_layers::save_to_file(		o_str CONST & filename_in )
{
	AAA_ERR err;
	SPY_PUSH_RANGE_OBJ( "layers::save", spy::LAYERS );
		if( b_save_one_file )
			err = save_to_vfile(			filename_in );
		else
			err = save_to_file_private(		filename_in );
	SPY_POP_RANGE2();
	return err;
}

AAA_ERR		c_layers::load_from_file(	o_str CONST & filename_in )
{
	AAA_ERR err;
	SPY_PUSH_RANGE_OBJ( "layers::load", spy::LAYERS );
		if( b_load_one_file )
			err = load_from_vfile(			filename_in );
		else
			err = load_from_file_private(	filename_in );
	SPY_POP_RANGE2();
	return err;
}

//CAMERA
c_seedcam*	c_layers::set_camera_ui( INT32 in )
{
	IF_THIS_NULL_RETURN_NULL();
	_i_camera_ui = IMOD( in, CAMERA_NB );
	return get_camera_ui();
}

c_seedcam*	c_layers::camera_ui_prev()
{
	IF_THIS_NULL_RETURN_NULL();
	return set_camera_ui( _i_camera_ui-1);
}

c_seedcam*	c_layers::camera_ui_next()
{
	IF_THIS_NULL_RETURN_NULL();
	return set_camera_ui( _i_camera_ui+1 );
}

void	c_layers::set_camera_lock_view_to_ui( bool b_in )
{
	if( _b_camera_lock_view_to_ui != b_in )
	{
		_b_camera_lock_view_to_ui = b_in;
		SWITCH_PRINT_STATE("Camera Lock View to UI", _b_camera_lock_view_to_ui );
	}
}

void	c_layers::flip_camera_lock_view_to_ui()
{
	IF_THIS_NULL_RETURN();
	set_camera_lock_view_to_ui( !_b_camera_lock_view_to_ui );
}


c_layer*	c_layers::layer_ui_get() CONST
{
	return layer_get_from_index( _i_layer_cur );
}

void	c_layers::layer_ui_set()
{
	IF_THIS_NULL_RETURN();
	layer_ui_set( _i_layer_cur );
}

void	c_layers::layer_ui_set( INT32 index_in )
{
	IF_THIS_NULL_RETURN();
	index_in = IMOD( index_in, LAYER_NB_MAX );
	if( _i_layer_cur != index_in )
	{
		_i_layer_cur = index_in;
		//refresh
	}
	c_layer::set_ui( layer_get_raw_from_index( _i_layer_cur ) );
		
	//propagate_up
	if( c_layers::get_ui() != this )
		become_ui();
	else
		ui_sync();
}

void	c_layers::become_ui()
{
	IF_THIS_NULL_RETURN();
	c_obj_ui* obj = get_root();
	if( obj->is_class<c_module>() )
	{
		c_module* module = reinterpret_cast<c_module*>(obj);
		c_module::set_ui( module );	//todo Maa May 2018 unclear but when module was added to MU we need this line so ui_sync don't fuck up
		module->layers_ui_set( _id_local_ui-1 );
	}
	else
		set_ui( this );
}

void	layer_ui_new()
{
	if( c_layers::get_ui() )
	{
		INT32		index = c_layers::get_ui()->layer_ui_get_index();
		c_layer*	layer = c_layers::get_ui()->layer_new( index );
		c_layers::get_ui()->layer_ui_set( index );
	}
	else
		ERR_PRINT_STRING( "Can't create a layer outside an existing Layers" );
}

void	layer_ui_forget()
{
	if( c_layers::get_ui() )
	{
		INT32 CONST index = c_layers::get_ui()->layer_ui_get_index();
		c_layers* layers = c_layers::get_ui();
		if( layers->do_dialog_forget( "Layer", index ) )
			layers->layer_forget( index );
	}
}
