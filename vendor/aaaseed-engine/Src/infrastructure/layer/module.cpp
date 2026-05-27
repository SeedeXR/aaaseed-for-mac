
#include "aaa_def.h"
#include "module.h"
		 
#include "draw/lights.h"
#include "draw/mat.h"
#include "draw/seedcam.h"
#include "draw/seeddraw.h"
#include "infrastructure/namer.h"
#include "infrastructure/seedfile.h"
#include "infrastructure/bind/bind.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/param/param_declare.h"
#include "infrastructure/param/traxs.h"
#include "language/lua/aaalua_wrap.h"
#include "obj_ui/multi_screen.h"
#include "strnum.h"
#include "time/speed.h"
#include "ui/obj_value.h"
#include "ui/strsymbo.h"
#include "time_buf.h"
#include "spy.h"
#include "file/aaa_dir.h"
#include "gol/gol.h"


FACTORY_CREATE_V1( c_module, module, Module(Layerss), layerss_param );

bool		c_module::b_traxs_update;

c_module*	c_module::cur = nullptr;
c_module*	c_module::ui  = nullptr;
bool		c_module::b_force_load_all = false;

bool		c_module::b_save_one_file;
bool		c_module::b_load_one_file;


namespace	n_module
{
	CONSTEXPR INT32 BASE_NB_MAX		=	1;
//	CONSTEXPR INT32 TIMING_NB_MAX	=	1;
	CONSTEXPR INT32 DETAIL_NB_MAX	=	21
#if AAA_STATE_COMPILE()
												+ 1
#endif //AAA_STATE_COMPILE
												;
	CONSTEXPR INT32 CAMERA_NB_MAX	=	6;
	CONSTEXPR INT32 LAYERS_NB_MAX	=	c_module::LAYERS_NB;
	CONSTEXPR INT32 GROUP_NB_MAX	=	2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
								//	+	TIMING_NB_MAX
									+	DETAIL_NB_MAX
									+	CAMERA_NB_MAX
									+	LAYERS_NB_MAX
									+	GROUP_NB_MAX;
	CONSTEXPR INT32 PARAM_INDEX_LAYERS = PARAM_NB_MAX - LAYERS_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active			)

		PARAM_DEF_GROUP_CLOSED( Module Params, DETAIL_NB_MAX + 1 + CAMERA_NB_MAX )
			//PARAM_DEF_GROUP_CLOSED( Timing, TIMING_NB_MAX )

			PARAM_DEF_REF(			name_symbo								)
			PARAM_DEF_STR(			comment									)
			PARAM_DEF_BOOL_OFF(		multiple_only							)
			PARAM_DEF_BOOL_OFF_STR(	module_render_multiple, gstr::current	)
			PARAM_DEF_BOOL_OFF(		module_lua								)
			PARAM_DEF_NONE(			module_values							)
			PARAM_DEF_BOOL_ON(		module_traxs							)
			PARAM_DEF_BOOL_OFF_STR(	module_materials,		gstr::current	)
			PARAM_DEF_BOOL_OFF_STR(	module_lights,			gstr::current	)

			PARAM_DEF_NONE(			module_camera							)
			PARAM_DEF_GROUP_CLOSED( module_camera Stuff, CAMERA_NB_MAX )
				PARAM_DEF_SCALE_XYZ(	camera_global_scale						)
				//	{	nullptr,	PARAM_SYMBOLIC,	"camera_euler_order",	0., GOL_ORDER_ZYX,		0., 5., nullptr, gstr::rot_order },
				PARAM_DEF_REAL_ZERO(	camera_global_yaw						)
				PARAM_DEF_REAL_ZERO(	camera_global_pitch						)
				PARAM_DEF_REAL_ZERO(	camera_global_roll )

			PARAM_DEF_BOOL_ON( 		picking_use								)

			PARAM_DEF_NONE(			Timing	)
			PARAM_DEF_BOOL_ON( 		time_buffer_use							)
			PARAM_DEF_REAL_ONE( 	time_buffer_value						)
			PARAM_DEF_BOOL_OFF(		do_gl_flush_at_end						)
			PARAM_DEF_BOOL_OFF(		do_gl_finish_at_end						)

			PARAM_DEF_INT32(		layers_index_cur,	1, 0.,					0, LAYERS_NB_MAX-1 )
//			PARAM_DEF_INT32(		layers_index_begin,	1, 0.,					0, LAYERS_NB_MAX-1 )
			PARAM_DEF_INT32(		layers_index_end,	1, LAYERS_NB_MAX-1.,	0, LAYERS_NB_MAX-1 )
			PARAM_DEF_INT32_LOCKED(	module_id_local							)

			PARAM_DEF_BOOL_ON(		load_all								)
			PARAM_DEF_BOOL_LOCKED(	loaded									)

#if AAA_STATE_COMPILE()
			PARAM_DEF_NONE(			State									)
#endif //AAA_STATE_COMPILE
//		PARAM_DEF_GROUP( Groups_of_layer, LAYERS_NB_MAX )
	//default constructor of c_param_def complete the array init
			//PARAM_DEF_BOOL_OFF_SAVE_NOT( active )
			//...

	};
}


c_obj_ui*	c_module::get_obj_sub_by_index( INT32 CONST index ) CONST
{
	return layers_get_from_index( index );
}


INT32	c_module::layers_get_index_from_param( p_param param )
{
	INT32 index = param->get_id() - 1;
	index -= n_module::PARAM_INDEX_LAYERS;
	return index;
}

void	c_module::build_sum_up( o_str& o ) CONST
{
	o.erase();
	if( _b_multiple_only_ui )
		o.add( "MO "		);
	if( is_lua() && _lua )
	{
		o.add( _lua->get_script_name() );
		o.add_space();
	}
	if( _b_trax_ui )
		o.add( "TRAX "		);
	if( _b_materials_ui )
		o.add( "MATS "		);
	if( _b_lights_ui )
		o.add( "LIGHTS "	);
}

namespace {	
	o_str o_tmp;
	o_str sum_up;
}

void	c_module::param_init_pt()
{
	INT32	h=0;

	param_set_pt(		h, get_pt_active()		);

	build_sum_up( sum_up );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt(				h, get_name_symbo()			);
		param_set_pt(				h, get_comment()			);
		param_set_pt(				h, _b_multiple_only_ui		);

		param_attach_obj_no_inc(	h, _multi_screen			);
		param_set_pt(				h, _b_multi_screen_ui		);

		param_attach_obj_no_inc(	h, (c_obj_ui*) _lua			);
		if( _lua )
			get_param(h)->set_comment( _lua->get_script_name()	);
		param_set_pt(				h, _b_lua_ui				);

		param_attach_obj(			h, _values					);

		param_attach_obj_no_inc(	h, _traxs					);
		param_set_pt(				h, _b_trax_ui				);

		param_attach_obj_no_inc(	h, _materials				);
		param_set_pt(				h, _b_materials_ui			);

		param_attach_obj_no_inc(	h, _lights					);
		param_set_pt(				h, _b_lights_ui				);

		param_attach_obj(			h, _camera					);
		++h;
			param_set_pt_3(			h, _camera_sca_ui			);
		//	param_set_pt(				h, gs_euler_order			);
			param_set_pt_3(			h, _camera_rot_ui			);

		param_set_pt( 				h, _b_used_for_picking_ui 		);

		//if( g_timing_master.is_run() )
		//{
		//	CHAR	buf[256];
		//	sprintf( buf, "%.2f", _timing_module->get_interval_last_ms() );
		//	get_param(h)->set_comment( buf );
		//}
		_speed_module->build_comment( sum_up, true, false );
		get_param(h)->set_comment( sum_up );
		//	++h;		_speed_module_ui->update();
		param_attach_obj(			h, _speed_module_ui				);

		param_set_pt( 				h, _b_time_buffer_include_ui 	);
		param_set_pt( 				h, _time_buffer_value 			);
		param_set_pt( 				h, _b_flush_at_end_ui 			);
		param_set_pt( 				h, _b_finish_at_end_ui 			);

		param_set_pt(				h, _i_layers_cur 				);
//		param_set_pt( 				h, _i_layers_begin 				);
		param_set_pt( 				h, _i_layers_end 				);

		param_set_pt(				h, _id_local_ui					);

		param_set_pt(				h, _b_load_all_ui				);
		param_set_pt(				h, _b_loaded_all_out			);

#if AAA_STATE_COMPILE()
		param_attach_obj(			h, _state_control				);
	//	param_set_pt( h, _b_state );
#endif //AAA_STATE_COMPILE

//	get_param(h)->set_list_nb( MIN( _i_layers_end+1, LAYERS_NB ) );
//	++h;
		for( INT32 i = 0; i < _i_layers_end; ++i )
		{
			if( auto layers = layers_get_from_index( i ) )
			{
				layers->build_sum_up( o_tmp );
				get_param(h+i)->set_comment( o_tmp );
			}
		}

	set_param_nb_used( h + LAYERS_NB );
		
	//	err_param_init_pt(h);
}

void	c_module::prepare_for_ui()
{
	if( !_b_loaded_all_out )
		load_states( true );

	//SUPER::prepare_for_ui();
	set_param_nb_used( n_module::PARAM_NB_MAX-LAYERS_NB + MIN( _i_layers_end + 1, LAYERS_NB ) );	//	MIN is a secu
}

CONSTRUCTOR_CREATE(c_module)
	,_layers_nb			(0)
	,_i_layers_end		(0)	//init needed or first param_init_pt() will randomly have a long loop 
	,_b_loaded_all_out	(false)
	,_hd_layers			{nullptr}
	,_bind				{nullptr}
	,_multi_screen		{nullptr}
	,_materials			{nullptr}
	,_lights			{nullptr}
	,_values			{nullptr}
	,_lua_recursion		{0}
	,_lua				{nullptr}
	,_traxs				{nullptr}
	,_camera			{nullptr}
#if AAA_STATE_COMPILE()
	,_state_control		{nullptr}
#endif //AAA_STATE_COMPILE
//	,_speed_module		{nullptr}
	,_speed_module_ui	{nullptr}
	,_i_layers_cur		(0)
{
	_speed_module = new c_speed( true, 1, g_speed_master->get_pb_print_module(), "total"	, this  );
	obj_get( _speed_module_ui );
	_speed_module_ui->set_timing( _speed_module );

	param_init_with( n_module::param, n_module::PARAM_NB_MAX );
	alloc();
}

namespace { 
	o_str o_obj_prefix( "Module_" );
	o_str o_param_prefix( "m000" );
	o_str o_prefix;
}


o_str CONST & c_module::make_param_prefix( INT32 index )
{
	o_param_prefix.set_digits( 1, 3, index + 1 );
	return o_param_prefix;
}


void	c_module::set_id_local( INT32 CONST id_local )
{
	_id_local_ui = id_local;
	
	o_obj_prefix.drop_at( 7 );
	o_obj_prefix.add( id_local );
	set_name( o_obj_prefix.get() );

	make_param_header_name();
}

//par
void	c_module::layers_param_update( INT32 i )
{
	auto			layers = layers_get_from_index( i );
	c_param * CONST	param  = get_param( n_module::PARAM_INDEX_LAYERS + i );

	param->attach_obj( layers );
	C_PCHAR name;
	if( layers )
	{
		param->set_pt( layers->get_pt_active() );
		param->set_type( TYPE_BOOL | M_SAVE_NOT );	//	PARAM_SAVE_NOT necessary for the state changes and cleaner
		name = layers->get_my_filename();
		if( !name )
			debug_break( "%s() not sure this should happen: layers->get_my_filename() return NULL", __FUNCTION__ );
	}
	else
	{
		param->set_pt_null();
		param->set_type( TYPE_NONE | M_SAVE_NOT );
		param->clear_comment();
		name = nullptr;
	}

	if( name )
	{
		o_prefix.set( c_layers::make_param_prefix(i) );
		o_prefix.add_space();
		o_prefix.add_dir_name( name );
		param->set_name( o_prefix );
	}
	else
		param->set_name( c_layers::make_param_prefix(i) );

}

c_module::~c_module()
{
	if( get_cur() == this )
		set_cur_null();
	if( get_ui() == this )
		set_ui_null();

	dealloc();
}

AAA_ERR	c_module::alloc()
{
	AAA_ERR	retcode = ERR_ANY;

	obj_get( _values );
	obj_get( _camera );
	obj_get( _materials );
	obj_get( _lights );
#if AAA_STATE_COMPILE()
	obj_get( _state_control );
#endif //AAA_STATE_COMPILE

	retcode = layers_alloc();
	if ( ERR(retcode) )
		box_err( "Can't allocate the layers" );

	return retcode;
}

void	c_module::dealloc()
{
	obj_delete( _traxs );
	obj_delete( _lua );
	obj_delete( _multi_screen );

	layers_dealloc();

	obj_delete( _values );
	obj_delete( _camera );
	obj_delete( _materials );
	obj_delete( _lights );
#if AAA_STATE_COMPILE()
	obj_delete( _state_control );
#endif //AAA_STATE_COMPILE

	obj_delete( _speed_module_ui );
	obj_delete( _speed_module );
}

// LAYERS
//
AAA_ERR	c_module::layers_alloc()
{
	AAA_ERR	retcode = ERR_ANY;

//todoqq change the def layers_nb (careful with the params in layers)
	_layers_nb = LAYERS_NB;
	// todofranz dealloc
	_hd_layers = new c_layers*[_layers_nb] ();
	if( _hd_layers )
	{
//	() after new already initialized to nullptr
//		c_layers**	hd = _hd_layers;
//		for( INT32 i = _layers_nb; i > 0; --i )
//			*hd++ = nullptr;

		obj_get( _bind );
		if( _bind )
		{
			_bind->set( _layers_nb, "Layers Bind", "layers_bind", aaa::file::TYPE_IO_NONE, 1, true );	//todoqq c_list should become dynamic
			retcode = AAA_OK;
		}
		for( INT32 i = 0; i < _layers_nb; ++i )
			layers_param_update(i);
	}
	return retcode; 
}

void	c_module::layers_dealloc()
{
	if( _hd_layers )
	{
		for( INT32 i = _layers_nb - 1; i >= 0; --i )	//todonow should be cleared before a load too
			obj_delete( _hd_layers[i] );
		SAFE_DELETE_ARRAY( _hd_layers );
	}
	obj_delete( _bind );
	_layers_map.clear();	//todonow update layers_map in layers new and delete
							//todonow do it also for trax_name
}

c_layers*	c_module::layers_get_always_from_index( INT32 index )
{
	if( index < 0 || index >= _layers_nb)
	{
		box_err( "%s() wrong layers index : %d", __FUNCTION__, index );
		return nullptr;
	}
	c_layers*	p = _hd_layers[index];
	if( !p )
	{
		obj_new( p );
		_hd_layers[index] = p;
		p->set_id_local( index + 1 );
	}
	return p;
}

namespace
{	
	INT32	layers_index_to_create = -1;
}

c_layers*	c_module::layers_new( INT32 index )
{
/*	other way
	CHAR*	str = layers_bind_list->get_str( i_layers_cur);
	if ( *str )
*/
	c_layers*	p = layers_get_always_from_index( index );
	if(p)
	{
		if( p->is_my_filename() )
			box_err( "The current Layers already exist" );
		else
		{
			layers_index_to_create = index;
			ask_type_io_save( aaa::file::TYPE_IO_LAYERS_NEW, this );
		}
	}
	return p;
}
c_layers*	c_module::layers_new()
{
	return layers_new( _i_layers_cur );
}

//toqqq this should be bad specially since state
void	c_module::layers_new_from_file( o_str CONST & filename_in )
{
	if( layers_index_to_create < 0 )
		return;

	o_str& filename = o_str::push_name();

	//	dir_name.set( c_file::dir_get_def() );	//hack ??
		filename.set_fname_relative_ext_no( filename_in, _dir_name );	// name have to be be fx not fx.layers or fx.*
		filename.convert_to_slash();

		c_layers*	p = layers_get_always_from_index( layers_index_to_create );
		p->set_my_filename( filename );
		_bind->set_item( layers_index_to_create, filename );
		_layers_map.set( filename.get(), p );
		layers_param_update( layers_index_to_create );
		layers_index_to_create = -1;

	o_str::pop_name();
}

//todo clean this
//todo should be async or deferred or...
void	c_module::layers_forget( INT32 index )
{
	_bind->clear_item( index );
	obj_delete( _hd_layers[index] );
// now in c_layers
//	if( c_layers::get_ui() == p )
//		c_layers::set_ui_null();
	layers_param_update( index );
	//ui->set_focus();
}

c_layers*	c_module::layers_get_from_name_short( C_PCHAR_C str )
{	
	c_layers* p; 
	return _layers_map.get( str, p ) ? p : nullptr;
}

//todo skip the missing one ???
void	c_module::layers_ui_set()
{
	IF_THIS_NULL_RETURN();
	layers_ui_set(_i_layers_cur);
}

void	c_module::layers_ui_set( INT32 index_in )
{
	IF_THIS_NULL_RETURN();

	//change layers
	index_in = IMOD(index_in, _layers_nb );
	if( _i_layers_cur != index_in )
	{	
		_i_layers_cur = index_in;
		layers_ui_refresh();
	}
	c_layers* layers_ui = layers_ui_get();
	c_layers::set_ui( layers_ui );
	//propagate down
	if( layers_ui )
		layers_ui->layer_ui_set();
	//propagate up
	if( ui != this )
		become_ui();
	else
		ui_sync();
}

void	c_module::set_ui( c_module* module )
{
	if( module )
	{
		ui = module;
#if AAA_STATE_COMPILE()
		_state_control->set_ui();
#endif //AAA_STATE_COMPILE
		//propagate down
		module->layers_ui_set();
		ui_sync();
	}
}

void		c_module::become_ui()
{	
	IF_THIS_NULL_RETURN();
	c_modules::get_ui()->module_ui_set( _id_local_ui-1 );
}
c_layers*	c_module::layers_ui_get()
{	
	return layers_get_from_index( _i_layers_cur );
}

void	c_module::layers_ui_refresh()
{
	layers_menu_update();
//todo	revive this
//		SWITCH_PRINT_STATE( "Layers", "Group %d Layer %d", i_layers_cur, c_layers::get_ui()->layer_ui_get_index() );
}

void	c_module::layers_ui_next()
{
	IF_THIS_NULL_RETURN();
	layers_ui_set( _i_layers_cur + 1 );
}
void	c_module::layers_ui_prev()
{
	IF_THIS_NULL_RETURN();
	layers_ui_set( _i_layers_cur - 1);
}

void	c_module::layers_menu_update()
{
#if	!AAA_MENU_LOCKED()
	for( INT32 i = 0; i <= _i_layers_end; ++i )
	{
		if( auto pt = layers_get_from_index( i ) )
			_bind->menu_item_set_before_after( i, pt->get_menu_header(), "", pt->is_active() );
	}
#endif
}

INT32	c_module::layers_menu_build( INT32 menu_id, PT_MENU_FN pt_fn )
{
	return _bind->menu_build( menu_id, pt_fn );
}

//hack
bool	c_module::swap_layers( c_param* to_move, INT32 inc )
{
	//	get	indexes to swap
	INT32 i_src = layers_get_index_from_param(to_move);
	if( i_src < 0 || _layers_nb <= i_src )
		return false;
	INT32 i_dst = i_src + inc;
	if( i_dst < 0 || _layers_nb <= i_dst )
		return false;

	//swap layer
	SWAP( _hd_layers[i_src], _hd_layers[i_dst] );
	if( _hd_layers[i_src] )
		_hd_layers[i_src]->set_id_local(i_src+1);
	if( _hd_layers[i_dst] )
		_hd_layers[i_dst]->set_id_local(i_dst+1);

	_bind->swap_item( i_dst, i_src );	//todonow check what we do with layers_bind_list

//param
//	swap_param( n_module::PARAM_INDEX_LAYERS + i_src, n_module::PARAM_INDEX_LAYERS + i_dst );
	swap_param_expand( n_module::PARAM_INDEX_LAYERS + i_src+1, n_module::PARAM_INDEX_LAYERS + i_dst+1 );
	 
	layers_param_update( i_src );
	layers_param_update( i_dst );

	return true;
}

void	c_module::layers_save_to_file( o_str CONST & filename_in )
{
	c_dir::change_to_def();

	o_str& filename = o_str::push_name();
		filename.set_fname_relative( filename_in );

#if AAA_STATE_COMPILE()
		if( c_state_master::is_state_ref() )
#endif //AAA_STATE_COMPILE
			_bind->save_to_file_add_ext( filename );
	
		for( INT32 i = 0; i < _layers_nb; ++i )
		{
			o_str CONST & str = _bind->get_o_str(i);
			if( !str.is_empty() )
			{
				if( auto pt = layers_get_from_index( i ) )
					pt->save_to_file( str.get() );
			}
		}
	o_str::pop_name();
}

void	c_module::layers_map_build()
{
	o_str& str  = o_str::push_name();
	o_str& strb = o_str::push_name();
		for( INT32 i = 0; i < _layers_nb; ++i )
		{
			if( auto p = layers_get_from_index( i ) )
			{
				str.set( _bind->get_o_str(i) );
				str.drop_fname();
				// remove / at end 
				str.drop_at( -1 );
				strb.set_fname_pure( str );
				_layers_map.set( strb.get(), p );
			}
		}
	o_str::pop_name();
	o_str::pop_name();
}

void	c_module::traxs_set_focus()
{
	IF_THIS_NULL_RETURN();
	_traxs->set_focus();
}

CHAR	g_str_module_traxs[] = "Module Traxs";

void	c_module::create_traxs()
{
	obj_get( _traxs );
	if ( !_traxs )
		box_err( "can't allocate traxs for c_module");
	else
	{
		_traxs->set_name( g_str_module_traxs );
		_traxs->set_nb( 32 );
	}
}

FINLINE	c_traxs*	c_module::get_traxs()
{
	if( !_traxs )
		create_traxs();
	return _traxs;
}

c_trax*	c_module::traxs_plug_out( INT32 index, c_obj_ui* obj, p_param param )
{
	IF_THIS_NULL_RETURN_NULL();
	return get_traxs()->trax_plug_out( index, obj, param );
}

c_trax*	c_module::traxs_plug_in( INT32 index, c_obj_ui* obj, p_param param )
{
	IF_THIS_NULL_RETURN_NULL();
	return get_traxs()->trax_plug_in( index, obj, param );
}

//todo	 do this seriously
//		next 3 fns
void	c_module::traxs_start_preroll()
{
/*
	traxs->start_preroll();
	for( INT32 i=0; i<=i_layers_end; ++i )
	{
		if( auto pt = layers_get_from_index(i) )
		{
			if( auto ptraxs = pt->get_traxs_existing() )
				ptraxs->start_preroll();
		}
	}
*/
}

void	c_module::traxs_start_loop( bool b_in )
{
/*
	traxs->start_loop(b_in);
	for( INT32 i=0; i<=i_layers_end; ++i )
	{
		if( auto pt = layers_get_from_index(i) )
		{
			if( auto ptraxs = pt->get_traxs_existing() )
				ptraxs->start_loop(b_in);
		}
	}
*/
}

void	c_module::traxs_stop_loop()
{
/*
	traxs->stop_loop();
	for( INT32 i=0; i<=i_layers_end; ++i )
	{
		if( auto pt = layers_get_from_index(i) )
		{
			if( auto ptraxs = pt->get_traxs_existing() )
				ptraxs->stop_loop();
		}
	}
*/
}

void	c_module::traxs_draw()
{
	if( _b_trax_ui )
		get_traxs()->draw();
	for( INT32 i = 0; i <= _i_layers_end; ++i )
	{
		if( auto pt = layers_get_from_index(i) )
			pt->traxs_draw();
	}
}

void	c_module::traxs_update()
{
	if( _b_trax_ui && _traxs )	//done for new module
		_traxs->update( 0, 0 );
}

//bool	b_loading_post = false;
FINLINE	void	c_module::update()
{
	if( !_b_loaded_all_out && is_active() )
		load_states( true );

	bool b_goon = true;
	if( _b_lua_ui )
	{
		obj_get( _lua )->update();
		if( _lua->is_skip_rest() )
			b_goon = false;
	}
	if( b_goon )
	{
		if( is_traxs_update() )
			traxs_update();
		if( _b_materials_ui )
			_materials->update();
		if( _b_lights_ui )
			_lights->update();
#if AAA_STATE_COMPILE()
		//if( _b_state )
		_state_control->update();
#endif //AAA_STATE_COMPILE
	}
}

FINLINE	void	c_module::draw_common()
{
	for( INT32 i = 0; i <= _i_layers_end; ++i )
	{
		if( auto pt = layers_get_from_index(i) )
			pt->inline_draw();
	}
	if( _b_flush_at_end_ui )
		GOL::flush();
	if( _b_finish_at_end_ui )
		GOL::finish();
}

FINLINE	void	c_module::draw_for_picking()
{
	if( !_b_multiple_only_ui && is_active() &&	is_used_for_picking() )
	{
		bool b_tbuf = _b_time_buffer_include_ui && c_tbuf::is_active();
		if( b_tbuf )
			c_tbuf::add( tbuf::CH_DRAW_MODULE, _time_buffer_value, "draw picking", this );
		for( INT32 i = 0; i <= _i_layers_end; ++i )
		{
			if( auto pt = layers_get_from_index(i) )
				pt->draw_for_picking();
		}
		if( b_tbuf )
			c_tbuf::add( tbuf::CH_DRAW_MODULE, 0., "stop", this );
	}
	//	c_seedcam::cur = _camera;	//todonow	this look buggy???
}

void	c_module::draw()
{
	if( !_b_multiple_only_ui && is_active() )
	{
		bool b_tbuf = _b_time_buffer_include_ui && c_tbuf::is_active();
		if( b_tbuf )
			c_tbuf::add( tbuf::CH_DRAW_MODULE, _time_buffer_value, "draw", this );
		draw_common();
		if( b_tbuf )
			c_tbuf::add( tbuf::CH_DRAW_MODULE, 0., "stop", this );
		//	c_seedcam::cur = _camera;	//todonow	this look buggy???
	}
}

void	c_module::draw_by_multiple_low()
{
//todo	c_module_att::push();
//	if(( c_multiple::cur->get_index() & 0x3f ) == 0x3f )
//		vbl::update();

	bool b_tbuf = _b_time_buffer_include_ui && c_tbuf::is_active();
	if( b_tbuf )
		c_tbuf::add( tbuf::CH_DRAW_MODULE, _time_buffer_value, "draw multiple", this );
	draw_common();
	if( b_tbuf )
		c_tbuf::add( tbuf::CH_DRAW_MODULE, 0., "stop", this );

	//	c_seedcam::cur = _camera;	//todonow	this look buggy???
//todo	c_module_att::pop();
}

void	c_module::update_then_draw_for_picking()
{
	_speed_module->begin();
	set_cur( this );	//	we set for draw
	if( _b_multi_screen_ui )
	{
		obj_get( _multi_screen );
		c_multi_screen*	multi_screen_last = _multi_screen;
		_multi_screen->update();
		while( _multi_screen->next() )
		{
			draw::set_view( draw::get_view() );
			update();
			draw_for_picking();
		}
		multi_screen_last->update();
	}
	else
	{
		update();
		draw_for_picking();
	}
	_speed_module->end();
}

bool	c_module::update_then_draw_lua()
{
	if( _lua_recursion >= 16 )
	{
		set_ref_error( "layers recursion max allowed is 16" );
		return false;
	}
	update();
	if( is_active() )
	{
		_speed_module->begin();
		draw_by_multiple_low();
		_speed_module->end();
	}
	return true;
}

void	c_module::update_then_draw()
{
	_speed_module->begin();
	set_cur( this );	//	we set for draw
	if( _b_multi_screen_ui )
	{
		obj_get( _multi_screen );
		c_multi_screen*	multi_screen_last = _multi_screen;
		_multi_screen->update();
		while( _multi_screen->next() )
		{
			draw::set_view( draw::get_view() );
			update();
			draw();
		}
		multi_screen_last->update();
	}
	else
	{
		update();
		draw();
	}
	_speed_module->end();
//todoqqq	add flag to test this so we can detect where it get corrupted
//	HEAP_IS_CORRUPT();
}

AAA_ERR	c_module::load_do_before( o_str CONST & filename_in )
{
//	SPY_PUSH_RANGE( "Load Module", spy::FILE_HIGH );

	auto& dir = c_dir::get_def();
	set_cur( this );	//	we set for load
	set_dir_name( dir );	//hack ??

#if AAA_STATE_COMPILE()
	//ugly stuff ?
	if( !c_state_master::is_state_ref() )
		return AAA_OK;
#endif //AAA_STATE_COMPILE

	o_str& filename = o_str::push_name();

		filename.set_fname_relative_ext_no( filename_in );
#if AAA_DEBUG()
		DBG_PRINT_STRING( "Default directory is %s",  c_dir::get_def().get() );
#endif
		GOOD_PRINT_STRING( "Loading Module : %s/%s", dir.get(), filename.get() );

		get_traxs();	//todoqq we force creation of traxs to	make sure we have a traxs object
						//	infact we create it even if not needed (infact it is needed)
						//	but the point is that the mechanism should change
						//	some debate for materials
		try_obj_load_add_ext( _lua,				filename );
		_traxs->load_from_file_add_ext(			filename );

		try_obj_load_add_ext( _multi_screen,	filename );	//todonownow
		_materials->load_from_file_add_ext(		filename );	//obj_load don't call do_before...
		_lights->load_from_file_add_ext(		filename );	//obj_load don't call do_before...

		_bind->load_from_file_add_ext(			filename );
#if AAA_STATE_COMPILE()
		_state_control->load_from_file_add_ext( filename );
#endif //AAA_STATE_COMPILE

	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR	c_module::load_do_after_low( o_str CONST & filename_in )
{
	SPY_PUSH_RANGE2( "Module after", spy::FILE_HIGH, filename_in );
	o_str& filename = o_str::push_name();

		c_dir::change_to_def();
		filename.set_fname_pure( filename_in );
		filename.drop_ext();

		for( INT32 i = 0; i < _layers_nb; ++i )
		{
			o_str CONST & str = _bind->get_o_str( i );
			if( !str.is_empty() )
			{
				DBG_HEAP_CHECK();
				c_layers*	pt = layers_get_always_from_index( i );
				c_namer::set_dir_and_file( pt, str );	//todo useful ??
				pt->load_from_file( str );
			}
		}
		c_layers::set_cur_null();	//voir param_load

#if AAA_STATE_COMPILE()
		if( c_state_master::is_state_ref() )
		{
#endif //AAA_STATE_COMPILE

			for( INT32 i = 0; i < _layers_nb; ++i )
				layers_param_update( i );
			layers_ui_refresh();

			_values->load_from_file_add_ext( filename );
			_camera->load_from_file_add_ext( filename );

			layers_map_build();

			INT32 index = LAYERS_NB; 
			while( --index > 0 )
			{
				if( _hd_layers[index] )
					break;
			}
			_i_layers_end = MIN( _i_layers_end, index );

#if AAA_STATE_COMPILE()
		}
#endif //AAA_STATE_COMPILE
		_b_loaded_all_out = true;

	o_str::pop_name();
	SPY_POP_RANGE2();
	return AAA_OK;
}

AAA_ERR	c_module::load_do_after( o_str CONST & filename_in )
{
	if( is_active() || _b_load_all_ui || b_force_load_all )
		return load_do_after_low( filename_in );
	return AAA_OK;
}

namespace {
	CONST CHAR  module_all_ext[] = "aaa_module_all";
};

void	c_module::load_states( bool b_just_rest )
{
	c_namer*	namer = get_namer();
	if( !namer )
		return;

	SPY_PUSH_RANGE( b_just_rest ? "Module Load States" : "Module load rest", spy::FILE_HIGH );
	c_module* module_store = get_cur(); 
	set_cur( this );

#if AAA_STATE_COMPILE()
	_state_control->set_cur();
#endif // AAA_STATE_COMPILE

	o_str& dir_name = o_str::push_name( namer->get_dir() );

#if AAA_STATE_COMPILE()
		_state_control->set_index(0);
#endif //AAA_STATE_COMPILE

#if AAA_STATE_COMPILE()
		CHAR*	pt_index_str;
		for( INT32 state = 0; state <= c_state_master::STATE_INDEX_MAX; ++state )
		{	
			if( state > 0 )
			{	//	we build the dir_name for state > 0	
				if( state == 1 )
				{
					INT32	l = strlen( dir_name );
					pt_index_str = dir_name+l;
					strcpy( pt_index_str, ".state.000" );
					pt_index_str += 7;
				}
				strnum::make( pt_index_str, 3, state );
			}
#endif //AAA_STATE_COMPILE

			//	redefine def dir
			if( c_dir::is_exist( dir_name ) )
			{
				if( NOERR( c_dir::push_def( dir_name ) ) )
				{	//	we build the dir_name for state > 0	
#if AAA_STATE_COMPILE()
					if( state )
					{
						_state_control->set_action_switch( _state_control->get_index(), state );
						state_layers_do_action();
						_state_control->set_index(state);	// needed in fns called at low level
					}
#endif //AAA_STATE_COMPILE
					if( b_load_one_file  )
					{
						o_str& filename_b = o_str::push_name( namer->get_file() );
							filename_b.add_ext( module_all_ext );
							c_file::open_vfile_load( filename_b );
						o_str::pop_name();
					}

					if( b_just_rest )
						load_do_after_low( namer->get_file() );
					else
						load_from_file_add_ext( namer->get_file() );

					if( b_load_one_file  )
						c_file::close_vfile();
				}
				c_dir::pop_def();	// we need to be always symmetrical to the push
			}
			else
			{
#if AAA_STATE_COMPILE()
				if( state == 0 )
				{
#endif //AAA_STATE_COMPILE
					ERR_PRINT_STRING( "AAASeed will not load this module : No directory %s", dir_name.get() );
#if AAA_STATE_COMPILE()
				}
				break;
#endif //AAA_STATE_COMPILE
			}
#if AAA_STATE_COMPILE()
		}
		if( _state_control->get_index() != 0 )
		{
			_state_control->set_action_switch( _state_control->get_index(), 0 );
			state_layers_do_action();
			_state_control->set_index( 0 );
		}
		c_state_master::set_cur_null();
#endif //AAA_STATE_COMPILE

	o_str::pop_name();
	set_cur( module_store );

	SPY_POP_RANGE();
}

AAA_ERR	c_module::load_from_file(	o_str CONST & filename_in )
{
	c_namer::set_dir_and_file( this, filename_in );
	c_namer::set_namer_start( this );
	//todo add retcode to load_state and use it
	load_states( false );
	return AAA_OK;
}

//todonow do we need to set_cur module somewhere in save
//we save before to avoid module params to be saved by returning FALSE
AAA_ERR c_module::save_do_before( o_str CONST & filename_in )
{
	set_cur( this );	//	we set for save (perhaps not useful because encode in ntrax right now)

	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();	//hack
		layers_save_to_file( filename );

#if AAA_STATE_COMPILE()
		if( c_state_master::is_state_ref() )
#endif //AAA_STATE_COMPILE
		{
			if_obj_save_add_ext( _lua,				filename );
			_values->save_to_file_add_ext(			filename );
			_camera->save_to_file_add_ext(			filename );

			if( _traxs )
				_traxs->save_to_file_add_ext(		filename );

			if_obj_save_add_ext( _multi_screen,		filename );
			_materials->save_to_file_add_ext(		filename );
			_lights->save_to_file_add_ext(			filename );
#if AAA_STATE_COMPILE()
			_state_control->save_to_file_add_ext( filename );
#endif //AAA_STATE_COMPILE
		}
	o_str::pop_name();
	return AAA_OK;
}

void	c_module::save_states( o_str CONST & filename_in )
{
	if( !_b_loaded_all_out )
	{
		GOOD_PRINT_STRING( "%s don't need to be saved : skipping", filename_in.get() );
		return;
	}

	o_str& dir_name = o_str::push_name();
	o_str& filename = o_str::push_name();		
		//	namer = pt->get_namer();
		//	strcpy( dir_name, filename_in );
		dir_name.set_dir_name( filename_in );

#if AAA_STATE_COMPILE()
		_state_control->set_cur();
		INT32 state_index_prev = _state_control->get_index();
	
		if( state_index_prev )
			_state_control->set_action_switch( state_index_prev, 0 );
		else
			_state_control->set_action_store( 0 );
		state_layers_do_action();
		_state_control->set_index(0);	// needed in fns called at low level
#endif //AAA_STATE_COMPILE

#if AAA_STATE_COMPILE()
		CHAR*	pt_index_str;
		INT32 state;

		for( state = 0; state <= _state_control->get_index_end(); ++state )
		{
			if( state > 0 )
			{	//	we build the dir_name for state > 0	
				if( state == 1 )
				{
					INT32	l = strlen(dir_name);
					pt_index_str = dir_name+l;
					strcpy( pt_index_str, ".state.000" );
					pt_index_str += 7;
				}
				strnum::make( pt_index_str, 3, state );
			}
#endif //AAA_STATE_COMPILE
			if( NOERR( c_dir::push_def( dir_name, true ) ) )
			{
#if AAA_STATE_COMPILE()
				if( state )
				{
					_state_control->set_action_switch( _state_control->get_index(), state );
					state_layers_do_action();
					_state_control->set_index(state);	// needed in fns called at low level
				}
#endif //AAA_STATE_COMPILE
				filename.set_fname_pure( filename_in );
				if( b_save_one_file )
				{
					o_str& filename_b = o_str::push_name( filename );
						filename_b.add_ext( module_all_ext );
						c_file::open_vfile_save( filename_b );
					o_str::pop_name();
				}

				save_to_file_add_ext( filename );

				if( b_save_one_file )
					c_file::close_vfile();
			}
			else
			{
				box_err( "Can't create dir \"%s\" to save module\nAAASeed will not save this module.", dir_name.get() );
			}
			c_dir::pop_def();	// we need to be always symmetrical to the push
#if AAA_STATE_COMPILE()
		}

		if( _state_control->get_index() != state_index_prev )
		{
			_state_control->set_action_switch( state, state_index_prev );
			state_layers_do_action();
			_state_control->set_index( state_index_prev );
		}
		c_state_master::set_cur_null();
#endif //AAA_STATE_COMPILE
	o_str::pop_name();
	o_str::pop_name();
}

/*
AAA_ERR	c_module::save_to_file(	C_PCHAR_C		filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		save_states( filename );
	o_str::pop_name();
	return AAA_OK;
}
*/

#if AAA_STATE_COMPILE()
void	c_module::state_layers_do_action()
{
	for( INT32 i = 0; i < LAYERS_NB; ++i )
	{
		if( auto pt = layers_get_from_index(i) )
			if( pt->is_state_sensitive() )
				pt->state_do_action();
	}
}
#endif //AAA_STATE_COMPILE
