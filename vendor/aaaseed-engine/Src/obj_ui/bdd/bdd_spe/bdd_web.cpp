#include "bdd_web.h"
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "draw/pbo.h"
#include "ui/seed_ui.h"
#include "draw/tex.h"
#include "media/video/texture_flux_master.h"


C_PCHAR_C	c_bdd_web::library_str[ c_bdd_web::WEB_LIB_MAX ] =
{
	"Global",
	"WKE",
	"Awesomium"
};

UINT32	c_bdd_web::s_web_lib = c_bdd_web::WEB_LIB_WKE;

FACTORY_CREATE_PROP_V1( c_bdd_web, bdd_web, Web Browser, bdd_web, sub_menu="Special"; );

namespace n_bdd_web
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 12 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32 RENDER_PARAM_NB	= 8;
	CONSTEXPR INT32 UI_PARAM_NB		= 19;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 2;	
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	UI_PARAM_NB
								+	RENDER_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(			active				)
		PARAM_DEF_BOOL_OFF(			verbose				)
		PARAM_DEF_SYMBO_PSTR_ZERO(	library,			c_bdd_web::library_str	)

		PARAM_DEF_BOOL_OFF(			ui_intercept		)
		PARAM_DEF_BOOL_OFF(			load_at_start		)
		PARAM_DEF_BOOL_OFF(			load_trig			)
		PARAM_DEF_REF(				http_address		)
		PARAM_DEF_STR_LOCKED(		http_address_cur	)
		PARAM_DEF_STR_LOCKED(		http_title			)
		PARAM_DEF_STR_LOCKED(		tooltip				)
		PARAM_DEF_BOOL_LOCKED(		loaded				)
		PARAM_DEF_BOOL_LOCKED(		loading_failed		)

		PARAM_DEF_GROUP_CLOSED( Render, RENDER_PARAM_NB )
			PARAM_DEF_BIND_2D_CURRENT_SEL(	bind_dst			)
			PARAM_DEF_BANK_BIND_2D_OUT(		bind_out			)
			PARAM_DEF_BOOL_ON(				move_to_gpu			)
//			PARAM_DEF_BOOL_ON(				bind_on_cpu_keep	)
//			PARAM_DEF_BOOL_OFF(				use_pbo				)
		//todo regroup with PARAM_DEF_IMG_SIZE_FORMAT ?
			PARAM_DEF_IMG_SIZE(				render_size_x,		512, 1024	)
			PARAM_DEF_IMG_SIZE(				render_size_y,		512, 1024	)
			//PARAM_DEF_BOOL_OFF( use_alpha )
			PARAM_DEF_BOOL_OFF(				background_transparent					)
			PARAM_DEF_REAL(					render_max_fps,		25, 50, 1., 200.	)
			PARAM_DEF_BOOL_LOCKED(			dirty )

		PARAM_DEF_GROUP_CLOSED( Interaction, UI_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		mouse_move_trig			)
			PARAM_DEF_REAL_ZERO(	mouse_x					)
			PARAM_DEF_REAL_ZERO(	mouse_y					)
			PARAM_DEF_BOOL_OFF(		mouse_left_down_trig	)
			PARAM_DEF_BOOL_OFF(		mouse_left_up_trig		)
			PARAM_DEF_BOOL_OFF(		mouse_right_trig		)
			PARAM_DEF_BOOL_OFF(		mouse_middle_trig		)
			PARAM_DEF_BOOL_OFF(		mouse_wheel_trig		)
			PARAM_DEF_INT32_ONE(	mouse_wheel_scroll		)
			PARAM_DEF_BOOL_OFF(		zoom_in_trig			)
			PARAM_DEF_BOOL_OFF(		zoom_out_trig			)
			PARAM_DEF_BOOL_OFF(		zoom_reset_trig			)
			PARAM_DEF_BOOL_OFF(		text_trig				)
			PARAM_DEF_BOOL_OFF(		select_all_trig			)
			PARAM_DEF_BOOL_OFF(		unselect_all_trig		)
			PARAM_DEF_BOOL_OFF(		history_backward_trig	)
			PARAM_DEF_BOOL_OFF(		history_forward_trig	)
			PARAM_DEF_BOOL_OFF(		history_trig			)
			PARAM_DEF_INT32_ONE(	history_offset			)
	};
}

void	c_bdd_web::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_verbose );
	param_set_pt( h, _library_ui );

	param_set_pt( h, _b_ui_intercept_ui );
	param_set_pt( h, _b_load_at_start );
	param_set_pt( h, _b_load_trig );
	param_set_pt( h, _http_address );
	param_set_pt( h, _http_address_cur );
	param_set_pt( h, _http_title );
	param_set_pt( h, _tooltip );
	param_set_pt( h, _b_page_loaded );
	param_set_pt( h, _b_page_load_failed );

	++h;
		param_set_pt( h, _bind_dst_ui			);
		param_set_pt( h, _bind_dst				);
		param_set_pt( h, _b_move_to_gpu			);
//		param_set_pt( h, _b_bind_on_cpu_keep	);
//		param_set_pt( h, _b_use_pbo_ui			);
		param_set_pt( h, _size_x_ui				);
		param_set_pt( h, _size_y_ui				);
//		param_set_pt( h, _b_use_alpha			);
		param_set_pt( h, _b_back_transparent_ui	);
		param_set_pt( h, _render_max_fps		);
		param_set_pt( h, _b_dirty				);

	++h;
		param_set_pt( h, _b_mouse_move_trig );
		param_set_pt( h, _mouse_x );
		param_set_pt( h, _mouse_y );
		param_set_pt( h, _b_mouse_left_down_trig );
		param_set_pt( h, _b_mouse_left_up_trig );
		param_set_pt( h, _b_mouse_right_trig );
		param_set_pt( h, _b_mouse_middle_trig );
		param_set_pt( h, _b_mouse_wheel_trig );
		param_set_pt( h, _mouse_wheel_scroll );
		param_set_pt( h, _b_zoom_in_trig );
		param_set_pt( h, _b_zoom_out_trig );
		param_set_pt( h, _b_zoom_reset_trig );
		param_set_pt( h, _b_text_trig );
		param_set_pt( h, _b_select_all_trig );
		param_set_pt( h, _b_unselect_all_trig );
		param_set_pt( h, _b_history_backward );
		param_set_pt( h, _b_history_forward );
		param_set_pt( h, _b_history_trig );
		param_set_pt( h, _history_offset );

	err_param_init_pt( h );
}

//	virtual	void	update()					{}
//	virtual	bool	can_ui_intercept()			{	return false; }
CONSTRUCTOR_CREATE( c_bdd_web )
	,_buffer				(	nullptr	)
	,_pbo					(	nullptr	)
	,_web_interface			(	nullptr	)
#if	AAA_LIB_USE_AWESOMIUM()
	,_web_interface_awe		(	nullptr	)
#endif
#if	AAA_LIB_USE_WKE()
	,_web_interface_wke		(	nullptr	)
#endif
	,_b_loaded				(	false	)
	,_b_init				(	false	)
	,_size_x				(	0		)
	,_size_y				(	0		)
//	,_ch_nb					(	0		)
	,_b_dirty				(	false	)
	,_library				(	c_bdd_web::WEB_LIB_GLOBAL	)
{
//	_web_interface = (c_web_interface*) new c_web_interface_awe;
//	_web_interface = (c_web_interface*) new c_web_interface_wke;
	param_init_with( n_bdd_web::param, n_bdd_web::PARAM_NB );
}

c_bdd_web::~c_bdd_web()
{
	close();

	_web_interface = nullptr;
#if	AAA_LIB_USE_AWESOMIUM()
	SAFE_DELETE( _web_interface_awe );
#endif
#if	AAA_LIB_USE_WKE()
	SAFE_DELETE( _web_interface_wke );
#endif
	dealloc_buffer();
}

void	c_bdd_web::close()
{
#if	AAA_LIB_USE_AWESOMIUM()
	if( _web_interface_awe )
		_web_interface_awe->end();
#endif
#if	AAA_LIB_USE_WKE()
	if( _web_interface_wke )
		_web_interface_wke->end();
#endif
}


void	c_bdd_web::dealloc_buffer()
{
	SAFE_DELETE_ARRAY( _buffer );
	SAFE_DELETE( _pbo );
}

void	c_bdd_web::alloc_buffer()
{
	// we could use data from a c_img_2d, but we need to flip the image anyway, so we use a local buffer for now
	dealloc_buffer();
	_buffer = new UINT8[ _size_x_ui * _size_y_ui * 4 ];
	_size_x = _size_x_ui;
	_size_y = _size_y_ui;
//	_ch_nb	= _ch_nb_ui;

	c_img_2d*	img = g_bind_img_2d->get_always( _bind_dst );
	if( img )
	{
		tex_2d_bind_no_gpu_move( _bind_dst );	//was tex_2d_bind
	//	img->set_mem_type( texture_flux_master->get_mem_type() );
		img->init_with_size( _size_x, _size_y, aaa::PIXEL_FORMAT::BGRA_8, __FUNCTION__ );
		img->set_cpu_keep( true );
	//	img->set_bgr( true );
		img->move_to_gpu( __FUNCTION__, _bind_dst );
	}
}


void	c_bdd_web::update()
{
	if( !is_active() )
		return;

	UINT32	s_library = 0;
	s_library = ( _library_ui == WEB_LIB_GLOBAL ) ? s_web_lib : _library_ui;
	if( s_library != _library )
	{
		_web_interface = nullptr;
		//SAFE_DELETE( _web_interface );
		_library = s_library;
		_b_init = false;
	}
	if( _web_interface == nullptr )
	{
		switch( s_library )
		{
		case WEB_LIB_AWE:
#if	AAA_LIB_USE_AWESOMIUM()
			if( !_web_interface_awe )
			{
				_web_interface_awe = new c_web_interface_awe;
				_b_init = false;
			}
			_web_interface = _web_interface_awe;
			_web_interface_awe->resume();
#endif
#if	AAA_LIB_USE_WKE()
			if( _web_interface_wke )
				_web_interface_wke->pause();
#endif
			break;
		case WEB_LIB_WKE:
		default:
#if	AAA_LIB_USE_WKE()
//			_web_interface = (c_web_interface*) new c_web_interface_wke;	break;
			if( !_web_interface_wke )
			{
				_web_interface_wke = new c_web_interface_wke;
				_b_init = false;
			}
			_web_interface = _web_interface_wke;
			_web_interface_wke->resume();
#endif
#if	AAA_LIB_USE_AWESOMIUM()
			if( _web_interface_awe )
				_web_interface_awe->pause();
#endif
			break;
		}
	}

	if( !_web_interface )
		return;

	if( !_b_init )
	{
		if( _web_interface->begin( _size_x_ui, _size_y_ui, _b_back_transparent_ui, INT32(_render_max_fps) ) )
		{
			if( _b_load_at_start )
				_b_load_trig = true;
			_b_init = true;
		}
	}
	if( !_b_init )
		return;

	if( _web_interface->is_transparent() != _b_back_transparent_ui )
		_web_interface->set_transparent( _b_back_transparent_ui );
	if( _b_load_trig )
	{
		if( _http_address.get_len() > 0 )
		{
			_web_interface->load_url( _http_address.get() );
			_b_loaded = true;
		}
		_b_load_trig = false;
	}
	if( !_b_loaded )
		return;

	//	web_core->update();

	//	FP32 x,y;
	//	c_mouse::get_cur()->get_pos_in_render( x,y );
	//	DBG_PRINT_STRING( "Mouse is at %.2f, %.2f", x, y );

	_web_interface->focus();

	//todo we should detect change
	_bind_dst = g_bind_img_2d->build_index_from_param( _bind_dst_ui );

	// _ch_nb_ui = _b_use_alpha ? 4 : 3;
	// _ch_nb_ui = 4;
	if( _size_x != _size_x_ui || _size_y != _size_y_ui )//|| _ch_nb != _ch_nb_ui )
	{
		_web_interface->resize( _size_x_ui, _size_y_ui );
		alloc_buffer();
	}

	_b_dirty = _web_interface->is_dirty();
	if( _b_dirty )
	{
		//GOL::check_error_debug( "before c_bdd_web::before _web_interface->render " );
		_web_interface->render( _buffer, _size_x, 4 );
		//GOL::check_error_debug( "before c_bdd_web::after _web_interface->render " );
		c_img_2d*	src_img = g_bind_img_2d->get_always( _bind_dst );
		if( src_img )
		{
			bool	b_pbo_recreate = false;
			if( src_img->is_changed() )	// if image is recreated in bind_img::get_img() then is_changed() is true
				b_pbo_recreate = true;

			src_img->erase_filename();
		//	src_img->set_bgr( true );

			src_img->copy_from_src( _buffer, _size_x * 4, aaa::PIXEL_FORMAT::BGRA_8, aaa::PIXEL_FORMAT::RGBA_8, _size_x, _size_y, true );
			//bool b_use_pbo = _b_use_pbo_ui && texture_flux_master->is_pbo_use();
			//if( b_use_pbo )
			//{
			//	if( IS_NULL( _pbo ) )
			//		_pbo = new c_pbo;
			//	if( b_pbo_recreate )
			//		_pbo->allocate( src_img, _bind_dst );
			//	_pbo->load_data( src_img, _bind_dst );
			//}
			if( _b_move_to_gpu )
			{
				tex_2d_bind_no_gpu_move( _bind_dst );
				//if ( b_use_pbo )
				//	_pbo->move_to_gpu( src_img );
				//else
					src_img->move_to_gpu( __FUNCTION__, _bind_dst );
			}
		}
		else
		{
			ERR_PRINT_STRING( "bdd_web could not get img" );
		}
	}

	if( _b_mouse_move_trig )
	{
		_web_interface->inject_mouse_move( INT32(_size_x * _mouse_x), INT32(_size_y * _mouse_y) );
		_b_mouse_move_trig = false;
	}
	if( _b_mouse_left_down_trig )
	{
		_web_interface->inject_mouse_down( 0 );
		_b_mouse_left_down_trig = false;
	}
	if( _b_mouse_left_up_trig )
	{
		_web_interface->inject_mouse_up( 0 );
		_b_mouse_left_up_trig = false;
	}
	if( _b_mouse_right_trig )
	{
		_web_interface->inject_mouse_up( 2 );
		_b_mouse_right_trig = false;
	}
	if( _b_mouse_middle_trig )
	{
		_web_interface->inject_mouse_up( 1 );
		_b_mouse_middle_trig = false;
	}
	if( _b_mouse_wheel_trig )
	{
		_web_interface->inject_mouse_wheel( _mouse_wheel_scroll );
		_b_mouse_wheel_trig = false;
	}

	if( _b_zoom_in_trig )
	{
		_web_interface->zoom_in();
		_b_zoom_in_trig = false;
	}
	if( _b_zoom_out_trig )
	{
		_web_interface->zoom_out();
		_b_zoom_out_trig = false;
	}
	if( _b_zoom_reset_trig )
	{
		_web_interface->zoom_reset();
		_b_zoom_reset_trig = false;
	}

	if( _b_select_all_trig )
	{
		_web_interface->select_all();
		_b_select_all_trig = false;
	}
/*
if( _b_unselect_all_trig )
	{
		_web_interface->deselect_all();
		_b_unselect_all_trig = false;
	}
	//	this only a test
	if( _b_text_trig )
	{
		//if(event.unicode)
		{
			//unsigned short character = event.unicode;
			//if((event.unicode & 0xFF80) == 0)
			//character = event.unicode & 0x7F;

			_web_view->injectKeyboardEvent( 0, WM_CHAR, 'a', 0 );
			_b_text_trig = false;
			//_web_view->injectKeyboardEvent(0, WM_KEYDOWN, MapVirtualKey(event.scancode, MAPVK_VSC_TO_VK_EX), 0);
		}
	}
*/
	if( _b_history_backward )
	{
		_web_interface->goto_history_offset( -1 );
		_b_history_backward = false;
	}
	if( _b_history_forward )
	{
		_web_interface->goto_history_offset( 1 );
		_b_history_forward = false;
	}
	if( _b_history_trig )
	{
		_web_interface->goto_history_offset( _history_offset );
		_b_history_trig = false;
	}

	_web_interface->set_verbose( _b_verbose );
	_web_interface->update();
	_http_address_cur.set( _web_interface->get_address_cur() );
	_http_title.set( _web_interface->get_title() );
	_tooltip.set( _web_interface->get_tooltip() );
	_b_page_loaded = _web_interface->is_page_loaded();
	_b_page_load_failed = _web_interface->is_page_load_failed();
}

void	c_bdd_web::draw()
{
	//if( b_active_ && b_init_ )
	//	pf_draw();
}

bool	c_bdd_web::mouse_down( FP32& u_start, FP32& v_start )
{
	u_start = FP32(0);
	v_start = FP32(0);
	return false;
}

void	c_bdd_web::mouse_up( FP32 u_in, FP32 v_in )
{
}

void	c_bdd_web::mouse_move( FP32 u_in, FP32 v_in )
{
}

bool	c_bdd_web::do_key( INT32 c, bool CONST  b_special, INT32* modifiers, INT32* x, INT32* y )
{
	return false;
}
