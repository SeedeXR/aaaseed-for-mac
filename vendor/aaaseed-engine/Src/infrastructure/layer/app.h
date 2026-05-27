
#ifdef AAA_APP_H
#error "APP_H included more than once."
#endif
#define AAA_APP_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_modules;

class	c_app final : public c_obj_ui
{
	FACTORY_DECLARE( c_app, c_obj_ui );
	
private:
	INT32		_s_view_type;
	INT32		_s_axe_visible;
	REAL		_grid_size;
	INT32		_s_origin_visible;
	REAL		_origin_size;

	REAL		_global_resolution;
	REAL		_global_size_factor;

	bool		_b_save_trig;

	c_modules*	_modules;

private:

public:
			void	dealloc();
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();

	FINLINE	REAL	get_global_size_factor()	CONST	{ return _global_size_factor; }
	FINLINE	REAL	get_global_resolution()		CONST	{ return _global_resolution; }

	FINLINE	bool	is_view_type_4()			CONST	{ return _s_view_type != 0; }
			void	flip_view_type();
	FINLINE	INT32	get_axe_visible()			CONST	{ return _s_axe_visible; }
			void	set_axe_visible(	INT32 CONST s_in );
			void	inc_axe_visible();
			void	dec_axe_visible();
	FINLINE	INT32	get_origin_visible()		CONST	{ return _s_origin_visible; }
			void	set_origin_visible(	INT32 CONST s_in );
			void	inc_origin_visible();
			void	dec_origin_visible();
	FINLINE	REAL	get_origin_size()			CONST	{ return _origin_size; }
			void	axe_draw();

	virtual AAA_ERR	load_do_after( o_str CONST & filename_in );
	virtual AAA_ERR	save_do_after( o_str CONST & filename_in );

	void	traxs_start_preroll();
	void	traxs_start_loop( bool b_in );
	void	traxs_stop_loop();

	void	traxs_draw();
};

extern	c_app* g_app;

