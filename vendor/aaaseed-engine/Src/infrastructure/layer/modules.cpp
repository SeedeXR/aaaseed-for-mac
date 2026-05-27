
#include "aaa_def.h"
#include "modules.h"
		 
#include "gol/gol_draw.h"
#include "gol/gol_matrix.h"
#include "strnum.h"
#include "draw/mat.h"
#include "draw/lights.h"
#include "draw/picking.h"
#include "draw/render.h"
#include "draw/seedcam.h"
#include "draw/tex.h"
#include "draw/texture.h"
#include "fbo/fbo.h"
#include "infrastructure/namer.h"
#include "infrastructure/viewport.h"
#include "infrastructure/bind/bind.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/param/param_focus.h"
#include "infrastructure/param/traxs.h"
#include "language/lua/aaalua_wrap.h"
#include "obj_ui/bdd/bdd_image/bdd_feedback.h"
#include "obj_ui/bdd/bdd_ui_pref.h"
#include "obj_ui/transfo/transfo_three.h"
#include "obj_ui/transfo/transfo_trs.h"
#include "shaders/shading.h"
#include "time/speed.h"
#include "ui/obj_value.h"
#include "ui/aaa_menu.h"
#include "action.h"
#include "spy.h"
#include "file/aaa_dir.h"
#include "gol/gol.h"


FACTORY_CREATE_V1( c_modules, modules, Modules, modules );

c_modules*	c_modules::main	= nullptr;
c_modules*	c_modules::cur	= nullptr;
c_modules*	c_modules::ui	= nullptr;

//todonow	make it dynamic	
static	CONSTEXPR INT32 MODULE_NB = (64*3);

namespace n_modules
{
	CONSTEXPR INT32 BASE_NB_MAX		= 2;
	//CONSTEXPR INT32 TIMING_NB_MAX	= 1;
	CONSTEXPR INT32 MORE_NB_MAX		= 10;

	CONSTEXPR INT32 MODULE_NB_MAX	= MODULE_NB;
	CONSTEXPR INT32 GROUP_NB_MAX	= 1;

	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_NB_MAX
								//	+	TIMING_NB_MAX
									+	MORE_NB_MAX
									+	MODULE_NB_MAX
									+	GROUP_NB_MAX;
	CONSTEXPR INT32 PARAM_MODULE_INDEX = PARAM_NB_MAX-MODULE_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON( active )

		//PARAM_DEF_GROUP_CLOSED( Timing, TIMING_NB_MAX )
		PARAM_DEF_NONE( timing_modules )

		PARAM_DEF_GROUP_CLOSED( More Global, MORE_NB_MAX )
			PARAM_DEF_BOOL_ON(	Lua_Global		)
			PARAM_DEF_NONE(		Values_Global	)
			PARAM_DEF_BOOL_ON(	Traxs_Global	)
//			PARAM_DEF_NONE( state_master )

			PARAM_DEF_BOOL_OFF(	module_param_prefix_always	)
			PARAM_DEF_BOOL_OFF(	module_load_only_one_file	)
			PARAM_DEF_BOOL_OFF(	module_save_only_one_file	)
			PARAM_DEF_BOOL_ON(	group_load_only_one_file	)
			PARAM_DEF_BOOL_ON(	group_save_only_one_file	)

			PARAM_DEF_INT32_SAVE_NOT(	module_index_cur,	1, 0,					0, MODULE_NB - 1	)
//			PARAM_DEF_INT32(			module_index_begin,	1, 0,					0, MODULE_NB - 1	)
			PARAM_DEF_INT32(			module_index_end,	1, MODULE_NB_MAX - 1,	0, MODULE_NB - 1	)
	};
}

INT32	c_modules::module_get_index_from_param( p_param param )
{
	INT32 index = get_param_index( param );
	index -= n_modules::PARAM_NB_MAX - MODULE_NB;
	return index;
}

static	o_str sum_up;

void	c_modules::param_init_pt()
{
	INT32	h=0;

	param_set_pt(	h, get_pt_active()						);

	//if( g_timing_master.is_run() )
	//{
	//	CHAR	buf[256];
	//	sprintf( buf, "%.2f", _timing_modules->get_interval_last_ms() );
	//	get_param(h)->set_comment( buf );
	//}
	_speed_modules->build_comment( sum_up, true, false );
	get_param(h)->set_comment( sum_up );
//	++h;
	_speed_modules_ui->update();
	param_attach_obj(	h, _speed_modules_ui		);

	++h;
		param_attach_obj_no_inc(	h, (c_obj_ui*) _lua		);
		param_set_pt(				h, _b_lua_ui			);
		param_attach_obj(			h, (c_obj_ui*) _values	);
		param_attach_obj_no_inc(	h, _traxs				);
		param_set_pt(				h, _b_trax_ui			);
	//	param_attach_obj( h, (c_obj_ui*) c_state_master::cur );

		param_set_pt(	h,	_b_module_param_prefix_always	);
		param_set_pt(	h,	c_module::b_load_one_file		);
		param_set_pt(	h,	c_module::b_save_one_file		);
		param_set_pt(	h,	c_layers::b_load_one_file		);
		param_set_pt(	h,	c_layers::b_save_one_file		);

		param_set_pt(	h, _i_module_ui					);
//		param_set_pt(	h, _i_module_begin				);
		param_set_pt(	h, _i_module_end				);

		set_param_nb_used( h+MODULE_NB );
//	err_param_init_pt(h);
}

void	c_modules::prepare_for_ui()
{
//	SUPER::prepare_for_ui();
	set_param_nb_used( n_modules::PARAM_NB_MAX-MODULE_NB + MIN( _i_module_end+1, MODULE_NB ) );	//	MIN is a secu
}


CONSTRUCTOR_CREATE(c_modules)
//	,_process_fps			(.0f)
{
	init();

	param_init_with( n_modules::param, n_modules::PARAM_NB_MAX );
	
	alloc();
	module_param_update_all();
}

c_modules::~c_modules()
{
	if( get_cur() == this )
		set_cur(  nullptr );
	if( get_ui() == this )
		set_ui(  nullptr );
	if( get_main() == this )
		set_main(  nullptr );
	dealloc();
}

void	c_modules::init()
{
	_traxs = nullptr;
//	_camera = nullptr;
	obj_get( _values = nullptr );
	obj_get( _transfo_base );
	obj_get( _transfo_base_three );
	_speed_modules = new c_speed( true, 0, nullptr, "total", this );
	obj_get( _speed_modules_ui );
	_speed_modules_ui->set_timing( _speed_modules );
}

CHAR	g_str_global_traxs[] = "Global Traxs";

AAA_ERR	c_modules::alloc()
{
	AAA_ERR	retcode = ERR_ANY;

	obj_get( _traxs );
	if( !_traxs )
		box_err( "can't allocate traxs for c_modules" );
	else
	{
		_traxs->set_name( g_str_global_traxs );
		_traxs->set_nb( 64 );
	}

	retcode = module_alloc();
	if( ERR(retcode) )
		box_err( "Can't allocate the module" );

	return retcode; 
}

void	c_modules::dealloc()
{
	module_dealloc();

	obj_delete( _lua );
	obj_delete( _traxs );
//	obj_delete( _camera );
	obj_delete( _values );

	obj_delete( _transfo_base );
	obj_delete( _transfo_base_three );

	obj_delete( _speed_modules_ui );
	obj_delete( _speed_modules );
}


/*
void	c_modules::module_init()
{
	_hd_module = nullptr;
	_bind = nullptr;
	_i_module_cur = 0;
}
*/

AAA_ERR	c_modules::module_alloc()
{
	AAA_ERR	retcode = ERR_ANY;

//todoqq change the def module_nb (careful with the params in module)
	_module_nb = MODULE_NB;
	_hd_module = new c_module*[_module_nb];
	if( _hd_module )
	{
		c_module**	p = _hd_module;
		for( INT32 i = _module_nb; i > 0; --i )
			*p++ = nullptr;

		obj_get( _bind );
		if( _bind )
		{
			_bind->set( _module_nb, "Module Bind", "module_bind", aaa::file::TYPE_IO_NONE, 1, true );
			retcode = AAA_OK;
		}
	}
	return retcode; 
}

void	c_modules::module_dealloc()
{
	if( _hd_module )
	{
		//todonownow no more arrays
		for( INT32 i = _module_nb - 1; i >= 0; --i )	//todonow should be cleared before a load too
			obj_delete( _hd_module[i] );
		SAFE_DELETE_ARRAY( _hd_module );
	}
	obj_delete( _bind );
	_module_map.clear();	//todonow update module_map in c_module new and delete
							//todonow do it also for trax_name
}

c_module*	c_modules::module_get_always_from_index( INT32 index )
{
	if( index < 0 || _module_nb <= index )
	{
		box_err( "wrong module index in module_get_always_from_index()" );
		return nullptr;
	}
	c_module*	p = _hd_module[index];
	if( !p )
	{
		obj_new( p );
		_hd_module[index] = p;
		p->set_id_local( index+1 );
	}
	return p;
}

c_obj_ui*	c_modules::get_obj_sub_by_index( INT32 CONST index ) CONST
{
	return module_get_from_index( index );
}

void	c_modules::module_param_update_all()
{
	for( INT32 i = 0; i < MODULE_NB; ++i )
		module_param_update( i );
}

namespace{
	o_str	o_prefix;
}
//par
void	c_modules::module_param_update( INT32 i )
{
	auto	module = module_get_from_index(i);
	p_param	param = get_param( n_modules::PARAM_MODULE_INDEX+i );
	o_str& o = o_str::push_name();

		param->attach_obj( module );
		if( module )
		{
			param->set_pt( module->get_pt_active() );
			param->set_type( aaa::param::make_type( TYPE_BOOL, M_SAVE_NOT ) );	//	PARAM_SAVE_NOT was necessary for the state changes and cleaner

			o_str CONST & name = _bind->get_o_str(i);
			if( !name.is_empty() )
			{	//	just keep the bottom dir name
				o_str& ob = o_str::push_name();
					ob.set_dir_name( name );
					o.set_fname_pure( ob );
				o_str::pop_name();
			}
			if( o.is_empty() )
				debug_break( "%s() not sure this should happen we got an empty name to display module", __FUNCTION__ );
		}
		else
		{
			param->set_pt_null();
			param->set_type( aaa::param::make_type( TYPE_NONE, M_SAVE_NOT ) );	//	PARAM_SAVE_NOT was necessary for the state changes and cleaner			
		}

		if( o.is_empty() )
			param->set_name( c_module::make_param_prefix(i) );
		else
		{
			if( _b_module_param_prefix_always )
			{
				o_prefix.set( c_module::make_param_prefix(i) );
				o_prefix.add_space();
				o_prefix.add( o );
				param->set_name( o_prefix );
			}
			else
				param->set_name( o );
		}

	o_str::pop_name();
}

c_module*	c_modules::module_new( INT32 index )
{
/*	other way
	CHAR*	str	= module_bind_list->get_str( i_module_cur );
	if ( *str )
*/
	c_module*	p = module_get_always_from_index( index );
	if(p)
	{
		if( p->is_my_filename() )
			box_err( "The current Module already exist" );
		else
		{
			module_ui_set( index ); // needed before after dialog new is done for this index
			p->do_hack_new_module();
			ask_type_io_save( aaa::file::TYPE_IO_MODULE_NEW, this );
		}
	}
	return p;
}
c_module*	c_modules::module_new()
{
	return module_new( _i_module_ui );
}

//todo clean this
//todo should be async or deferred or...
void	c_modules::module_forget( INT32 index )
{
	_bind->clear_item( index );
	obj_delete( _hd_module[index] );
	module_param_update( index );
	//ui->set_focus();
}

//toqqq this should be bad specially since state
c_module*	c_modules::module_new_from_file( o_str CONST & filename_in, INT32 CONST index )
{
	o_str& filename = o_str::push_name();

		if( index >= 0 ) 
			_i_module_ui = index;
		c_module*	p = module_get_always_from_index( _i_module_ui );

		//	make sure the module have the right dir_name
		filename.set_dir_name( filename_in );
		p->set_dir_name( filename );

		//	do the rest
		filename.set_fname_relative_ext_no( filename_in );
		filename.convert_to_slash();

		p->set_my_filename( filename );
		_bind->set_item( _i_module_ui, filename );
		_module_map.set( filename.get(), p );
		module_param_update( _i_module_ui );

		c_module::set_ui( p );

	o_str::pop_name();
	return p;
}

c_module*	c_modules::module_get_from_name_short( C_PCHAR_C str )
{
	c_module* p; 
	return _module_map.get( str, p ) ? p : nullptr;
} 

void	c_modules::module_ui_set( INT32 index_in )
{
	//	change module
	index_in = IMOD( index_in, _module_nb );
	if( _i_module_ui != index_in )
	{
		_i_module_ui = index_in;
		module_ui_refresh();
	}
	c_module::set_ui( module_ui_get() );
}


c_module*	c_modules::module_ui_get()
{
	return module_get_from_index( _i_module_ui );
}

void	c_modules::module_ui_refresh()
{
//	DBG_SWITCH_STRING( "Module", "Group %d Layer %d", i_module_cur, c_module::ui->layer_ui_get_index() );
	module_menu_update();
}

void	c_modules::module_ui_next()
{
	IF_THIS_NULL_RETURN();
	module_ui_set( _i_module_ui + 1 );
}

void	c_modules::module_ui_prev()
{
	IF_THIS_NULL_RETURN();
	module_ui_set( _i_module_ui - 1 );
}

void	c_modules::module_menu_update()
{
#if	!AAA_MENU_LOCKED()
	// it fuck up the menu attached so we detach to force reatach
	//todo remove when we have a more dynamic menu reatached all the time
	menu::detach_all();
	for( INT32 i = 0; i <= _i_module_end; ++i )
	{
		auto pt = module_get_from_index(i);
		_bind->menu_item_set_before_after(	i,
											i==_i_module_ui ? "O ": "  ",
											nullptr,
											is_obj_exist_and_active(pt) ); 
	}
#endif
}

//2023 Jan M�a removed
//void	c_modules::module_menu_build( INT32 menu_id, PT_MENU_FN pt_fn )
//{
//	return _bind->menu_build( menu_id, pt_fn );
//}

void	c_modules::draw_module_picking_low()
{
	//	GOL::polygon_mode( GL_FRONT_AND_BACK, GL_FILL);
	for( INT32 i = 0; i <= _i_module_end; ++i )
	{
		auto pt = module_get_from_index(i);
		if( is_obj_exist_and_active(pt) )
			pt->update_then_draw_for_picking();
	}
}

void	c_modules::draw_module_low()
{
	for( INT32 i = 0; i <= _i_module_end; ++i )
	{
		auto pt = module_get_from_index(i);
		if( is_obj_exist_and_active(pt) )
			pt->update_then_draw();
	}
}

void	c_modules::draw_module()
{
	if( is_active() )
	{
		_speed_modules->begin();
		if( c_picking::is_cur() )
		{
			TBUF_ADD_OBJ( tbuf::CH_DRAW_MODULES, 2., "start draw picking" , this );
			draw_module_picking_low();
			TBUF_ADD_OBJ( tbuf::CH_DRAW_MODULES, 0., "stop", this );
		}
		else
		{
			TBUF_ADD_OBJ( tbuf::CH_DRAW_MODULES, 1., "start draw", this );
			draw_module_low();
			TBUF_ADD_OBJ( tbuf::CH_DRAW_MODULES, 0., "stop", this );

//old			if( c_viewport::is_valid() )
//old				cell_draw_cur->draw();
		}
//		c_module::set_cur_null();	//	was set in draw
		_speed_modules->end();
	}
}

void	c_modules::draw_before()
{
	TBUF_ADD( tbuf::CH_RENDER, REAL(1.2), "modules_draw_before" );

	c_seedcam::set_cur( nullptr );	//hack done to make sure it will be changed correctly
	//fbo
	c_fbo::disable();
	c_fbo::set_cur( nullptr );
	c_fbo::set_prev( nullptr );

	//todo these should be unknown here (subscription mechanism ?)
	c_bdd::set_cur( c_bdd::bdd_empty );
	c_transfo_trs::transfo1_cur = _transfo_base;
	c_transfo_three::cur = _transfo_base_three;
	c_transfo_trs::transfo2_cur = _transfo_base;

	c_bdd_ui_pref::begin_frame();

	c_render::begin();
	//todo is the right place
	//	deal texture by texture with it
	tex::begin();
	//todo check we need it

	GOL::enable_line_smooth();
	GOL::enable_point_smooth();

	c_shading::disable();

#if	!AAA_WATCHDOG()
	c_bdd_feedback::reset_grab();
#endif
}

void	c_modules::draw_after()
{
	TBUF_ADD( tbuf::CH_RENDER, 1.2, "modules_draw_after" );

	GOL::unbind_vao();
	c_texturing::disable();
	c_shading::disable();
	
//fbo
	if( GOL::b_fbo_do )
	{
		c_fbo::set_cur( nullptr );
		c_fbo::set_prev( nullptr );
	}

	c_render::end();

	viewport_render->doit();
}

void	c_modules::draw()
{
	set_cur( this );	
	draw_before();
	draw_module();
	draw_after();
}

//hack
bool	c_modules::swap_module( c_param* to_move, INT32 inc )
{
	//	get	indexes to swap
	INT32 i_src = module_get_index_from_param(to_move);
	if( i_src < 0 || _module_nb <= i_src )
		return false;
	INT32 i_dst = i_src + inc;
	if( i_dst < 0 || _module_nb <= i_dst )
		return false;

#if AAA_DEBUG()
	c_param	CONST * CONST param_src = get_param( n_modules::PARAM_MODULE_INDEX + i_src );
	c_param	CONST * CONST param_dst = get_param( n_modules::PARAM_MODULE_INDEX + i_dst );
	DBG_PRINT_STRING( "swap %s(%d) with %s(%d)", param_src->get_name().get(), i_src, param_dst->get_name().get(), i_dst );
#endif

	//swap module
	SWAP( _hd_module[i_src], _hd_module[i_dst] );
	if( _hd_module[i_src] )
		_hd_module[i_src]->set_id_local(i_src+1);
	if( _hd_module[i_dst] )
		_hd_module[i_dst]->set_id_local(i_dst+1);

	_bind->swap_item( i_dst, i_src );	//todonow check what we do with module_bind_list

//param
//	swap_param( n_modules::PARAM_MODULE_INDEX + i_src, n_modules::PARAM_MODULE_INDEX + i_dst);
	swap_param_expand( n_modules::PARAM_MODULE_INDEX + i_src+1, n_modules::PARAM_MODULE_INDEX + i_dst+1 );

	module_param_update( i_src );
	module_param_update( i_dst );

	return true;
}

void	c_modules::module_map_build()
{
	o_str&	str  = o_str::push_name();
	o_str&	strb = o_str::push_name();
		for( INT32 i = 0; i < _module_nb; ++i )
		{
			if( auto p = module_get_from_index(i) )
			{
				o_str CONST & pt = _bind->get_o_str( i );
				if( !pt.is_empty() )
				{
					str.set( pt );
					str.drop_fname();
					// remove / at end 
					str.drop_at( -1 );
					strb.set_fname_pure( str );
					_module_map.set( strb.get(), p );
				}
			}
		}
	o_str::pop_name();
	o_str::pop_name();
}

void	c_modules::traxs_set_focus()
{
	IF_THIS_NULL_RETURN();
	_traxs->set_focus();
}

//todo	 do this seriously
//		next 3 fns
void	c_modules::traxs_start_preroll()
{
/*
	traxs->start_preroll();
	for( INT32 i=0; i<=i_module_end; ++i )
	{
		if( auto pt = module_get_from_index(i) )
		{
			if( auto ptraxs = pt->get_traxs_existing() )
				ptraxs->start_preroll();
		}
	}
*/
}

void	c_modules::traxs_start_loop( bool b_in )
{
/*
	traxs->start_loop(b_in);
	for( INT32 i=0; i<=i_module_end; ++i )
	{
		if( auto pt = module_get_from_index(i) )
		{
			if( auto ptraxs = pt->get_traxs_existing() )
				ptraxs->start_loop(b_in);
		}
	}
*/
}

void	c_modules::traxs_stop_loop()
{
/*
	INT32		i;
	c_module*	pt;
	c_traxs*	ptraxs;
	traxs->stop_loop();
	for( INT32 i=0; i<=i_module_end; ++i )
	{
		if( auto pt = module_get_from_index(i) )
		{
			if( auto ptraxs = pt->get_traxs_existing() )
				ptraxs->stop_loop();
		}
	}
*/
}

void	c_modules::traxs_draw()
{
	if( _b_trax_ui )
		_traxs->draw();
	for( INT32 i = 0; i <= _i_module_end; ++i )
	{
		if( auto pt = module_get_from_index(i) )
			pt->traxs_draw();
	}
}

void	c_modules::traxs_update()
{
	if( _b_trax_ui )
		_traxs->update( 0, 0 );
}

bool	c_modules::b_traxs_update;

void	c_modules::update()
{
	if( is_active() )
	{
		set_cur( this );	
		draw::set_render_first_pass( true ); // this make sure traxs are updated
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
			//	g_layers_for_cam = nullptr;
			c_materials::get_def()->update();
			c_lights::get_def()->update();
			//	c_state_master::cur->update();
		}
	}
}

//todo bad in multitask lock eventually
namespace
{	
	c_modules* modules_store = nullptr;
}

AAA_ERR	c_modules::load_do_before( o_str CONST & filename_in )
{
	modules_store = c_modules::get_cur();
	c_modules::set_cur( this );
	
	c_module::set_cur_null();	//this just a security should be not needed

	o_str& filename = o_str::push_name();

		filename.set_fname_relative_ext_no( filename_in );

		try_obj_load_add_ext(	_lua,		filename );
		_traxs->load_from_file_add_ext(		filename );
		_values->load_from_file_add_ext(	filename );
		_bind->load_from_file_add_ext(		filename );
		//	c_state_master::cur->load_from_file_add_ext( filename );
		// we don't do it at alloc we don't have Gol all set
//		obj_get( _camera );
//		try_obj_load_add_ext(	_camera,	filename );

	o_str::pop_name();
	return AAA_OK;
}

void	c_modules::module_load_from_file( INT32 i, o_str CONST & filename )
{
	if( !filename.is_empty() )
	{
		c_module* pt = module_get_always_from_index( i );
		pt->load_from_file( filename );
		DBG_HEAP_CHECK();
	}
}

void	c_modules::module_load_from_file( o_str CONST & filename )
{
	module_load_from_file( _i_module_ui, filename );
}

void	c_modules::module_load_all()
{
	DBG_PRINT_STRING( "LOAD Module BEGIN" );
	for( INT32 i = 0; i < _module_nb; ++i )
		module_load_from_file( i, _bind->get_o_str( i ) );

	c_module::set_cur_null();	// was set in module load
	DBG_PRINT_STRING( "LOAD module DONE" );

	module_param_update_all();

	module_ui_refresh();

	module_map_build();
	module_ui_set( _i_module_ui );
}

AAA_ERR	c_modules::load_do_after( o_str CONST & filename )
{
	module_load_all();

	c_modules::set_cur( modules_store );
	return AAA_OK;
}

/*
void	c_modules::module_save_states( c_module* pt, CHAR* filename_in )
{
//	CHAR			filename[AAA_PATH_MAX()];
//	CHAR			dir_name[AAA_PATH_MAX()];
	CHAR*			pt_index_str;

	INT32			state_cur;
	c_state_master*	state_master = c_state_master::cur;

//	namer = pt->get_namer();
//	strcpy( dir_name, filename_in );
	fname::cpy_dir_name( dir_name, filename_in );
	fname::cpy_fname_pure( filename, filename_in );

	state_cur = state_master->get_index();
	if( state_cur )
	{
		switch_state_from_to( state_cur, 0 );
		state_master->set_index(0);	// needed in fns called at low level
		state_cur = 0;
	}
	
	for( INT32 state_index = 0; state_index <= state_master->get_index_end(); ++state_index )
	{	
		if( state_index > 0 )
		{
			if( state_index == 1 ) 
			{
				INT32	l = strlen(dir_name);
				pt_index_str = dir_name+l;
				strcpy( pt_index_str, ".state.000" );
				pt_index_str += 7;
			}
			strnum::make( pt_index_str, 3, state_index );
		}

		if( NOERR(c_file::dir_push_def( dir_name, true )) )
		{
			if( state_cur != state_index )
			{
			switch_state_from_to( state_cur, state_index );
			state_master->set_index(state_index);	// needed in fns called at low level
			state_cur = state_index;
			}
			fname::cpy_fname_pure( filename, filename_in );
			if( b_module_save_one_file )
			{
//				CHAR	filename_b[AAA_PATH_MAX()];
				fname::add_ext( filename_b, filename, "aaa_module_all" );
				c_file::open_vfile_save( filename_b );
			}
			pt->save_to_file( filename );
			c_file::close_vfile();
		}
		else
		{
			BOX_ERR( "Can't create %s to save module\nAAASeed will not save this module.", dir_name );
		}
		c_file::dir_pop_def();	// we need to be always symmetrical to the push
	}
	if( state_cur != 0 )
	{
		switch_state_from_to( state_cur, 0 );
		state_master->set_index( 0 );
	}
}
*/

AAA_ERR	c_modules::save_do_before( o_str CONST & filename_in )
{
	modules_store = c_modules::get_cur();
	c_modules::set_cur( this );
	return AAA_OK;
}

void	c_modules::module_save_to_file( o_str CONST & filename_in )
{
	c_dir::change_to_def();
	o_str& filename = o_str::push_name();
		filename.set_fname_relative( filename_in );

		_bind->save_to_file_add_ext( filename );
//		c_state_master::cur->save_to_file( filename );

		for( INT32 i = 0; i < _module_nb; ++i )
		{
			o_str CONST & str = _bind->get_o_str(i);
			if( !str.is_empty() )
			{
				if( auto pt = module_get_from_index(i) )
				{
//					module_save_states( pt, str );
					pt->save_states( str );
				}
			}
		}
		c_module::set_cur_null();	// was set in module save
	o_str::pop_name();
}

AAA_ERR c_modules::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name(	filename_in );
		filename.drop_ext();

		module_save_to_file(			filename );
		// we don't do it at alloc we don't have Gol all set
//		obj_get( _camera );
//		_camera->save_to_file_add_ext(	filename );
		_values->save_to_file_add_ext(	filename );
		_traxs->save_to_file_add_ext(	filename );
		if_obj_save_add_ext(	_lua,	filename );

		c_modules::set_cur( modules_store );
	o_str::pop_name();
	return AAA_OK;
}

c_layers*	c_modules::layers_get_from_name_short( C_PCHAR_C str )
{
	c_layers*	layers = nullptr;
	for( INT32 i = 0; i <= _i_module_end; ++i )
	{
		if( auto pt = module_get_from_index(i) )
		{
			layers = pt->layers_get_from_name_short( str );
			if( layers )
				break;
		}
	}
	return layers;
}

c_layers*	layers_get_from_name_short( C_PCHAR_C str )
{
	return c_modules::get_cur()->layers_get_from_name_short( str );
}

void ui_sync()
{
	c_obj_ui*	obj = focus_param::get_flatland_obj_focus();
	if( obj )
	{
		c_obj_ui*	ui = nullptr;
		if(			c_module::is_instance(obj) )
			ui = c_module::get_ui();
		else if(	c_layers::is_instance(obj) )
			ui = c_layers::get_ui();
		else if(	c_layer::is_instance(obj) )
			ui = c_layer::get_ui();
		if( ui )
			ui->set_focus();
		else
		{
			C_PCHAR	name = obj->get_name_str();
			if(		 str_is_equal( name, g_str_local_traxs	) )	
				action::doit(action::TRAXS_LAYERS_FOCUS);
			else if( str_is_equal( name, g_str_module_traxs ) )
				action::doit(action::TRAXS_MODULE_FOCUS);
			else if( str_is_equal( name, g_str_global_traxs ) )
				action::doit(action::TRAXS_MODULES_FOCUS);
		}
	}
}

