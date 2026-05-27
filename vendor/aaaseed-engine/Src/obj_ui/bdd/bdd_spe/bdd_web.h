
#ifdef AAA_BDD_WEB_H
#error "BDD_WEB_H included more than once."
#endif
#define AAA_BDD_WEB_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_WEB_UTIL_AWESOMIUM_H
#	include "web_util_awesomium.h"
#endif
#ifndef AAA_WEB_UTIL_WKE_H
#	include "web_util_wke.h"
#endif

class c_pbo;

class c_web_interface;
class c_web_interface_awe;
class c_web_interface_wke;

class	c_bdd_web final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_web, c_bdd );
public:
	enum WEB_LIB : INT32
	{
		WEB_LIB_GLOBAL,	//	Use global flag
		WEB_LIB_WKE,
		WEB_LIB_AWE,
		WEB_LIB_MAX
	} WEB_LIB;
	static	C_PCHAR_C	library_str[ WEB_LIB_MAX ];
	static	UINT32		s_web_lib;
protected:
	bool					_b_verbose;
	bool					_b_loaded;
	bool					_b_init;
	UINT32					_library;
	UINT32					_library_ui;
	o_str					_http_address;
	o_str					_http_address_cur;
	o_str					_http_title;
	o_str					_tooltip;
	bool					_b_ui_intercept_ui;

	bool					_b_page_loaded;
	bool					_b_page_load_failed;

	bool					_b_dirty;
	bool					_b_mouse_move_trig;
	REAL					_mouse_x;
	REAL					_mouse_y;
	bool					_b_mouse_left_down_trig;
	bool					_b_mouse_left_up_trig;
	bool					_b_mouse_right_trig;
	bool					_b_mouse_middle_trig;
	bool					_b_mouse_wheel_trig;
	INT32					_mouse_wheel_scroll;
	bool					_b_zoom_in_trig;
	bool					_b_zoom_out_trig;
	bool					_b_zoom_reset_trig;
	bool					_b_text_trig;
	bool					_b_select_all_trig;
	bool					_b_unselect_all_trig;
	bool					_b_history_backward;
	bool					_b_history_forward;
	bool					_b_history_trig;
	INT32					_history_offset;

	REAL					_render_max_fps;
	bool					_b_load_at_start;
	bool					_b_load_trig;

//	bool					_b_bind_on_cpu_keep;
	bool					_b_move_to_gpu;
	INT32					_bind_dst;
	INT32					_bind_dst_ui;
	INT32					_size_x_ui;
	INT32					_size_y_ui;
	bool					_b_back_transparent_ui;
	UINT8*					_buffer;
	INT32					_size_x;
	INT32					_size_y;
//	bool					_b_use_alpha;
//	INT32					_ch_nb_ui;
//	INT32					_ch_nb;

	c_web_interface*		_web_interface;
#if	AAA_LIB_USE_WKE()
	c_web_interface_wke*	_web_interface_wke;
#endif
#if	AAA_LIB_USE_AWESOMIUM()
	c_web_interface_awe*	_web_interface_awe;
#endif
//	bool					_b_texture_size_min;

	c_pbo*					_pbo;
	bool					_b_use_pbo_ui;

		//	void	move_to_gpu( INT32 bind, UINT8* data, INT32 w, INT32 h, INT32 channel_nb, bool b_bgr );
			void	init_awesomium();
			void	dealloc_buffer();
			void	alloc_buffer();
			void	close();

public:
	virtual	void	update();


			void	init();
	virtual	void	param_init_pt();

	virtual	void	draw();

	//
	//	UI_INTERCEPT_UI
	//
	virtual	bool	can_ui_intercept()			{	return true; }
	virtual bool	set_ui_intercept( bool b )	{	return _b_ui_intercept_ui = b; }
	virtual bool	is_ui_intercept()			{	return _b_ui_intercept_ui; }

	virtual	bool	do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y );

	virtual	bool	mouse_down( FP32& u_start, FP32& v_start );
	virtual	void	mouse_move( FP32 u_in, FP32 v_in );
	virtual	void	mouse_up( FP32 u_in, FP32 v_in );
};
