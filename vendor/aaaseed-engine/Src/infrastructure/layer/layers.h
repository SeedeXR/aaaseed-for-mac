
#ifdef AAA_LAYERS_H
#error "LAYERS_H included more than once."
#endif
#define AAA_LAYERS_H 1


#ifndef AAA_LAYER_H
#	include "infrastructure/layer/layer.h"
#endif
#ifndef AAA_CAMERAS_H
#	include "infrastructure/cameras.h"
#endif


class	c_obj_value;
class	c_fbo;
class	c_transfo_trs;
class	c_layer;
class	c_traxs;
class	c_lua_wrap;
class	c_speed;
class	c_speed_ui;
class	c_trax;

class	c_layers final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_layers, c_obj_active_ui );
	static	c_layers*	cur;
	static	c_layers*	ui;
	static	c_layers*	layers_cam_ui;
public:
	static	bool		b_save_one_file;
	static	bool		b_load_one_file;
public:
			static	o_str CONST &  make_param_prefix( INT32 index );

	FINLINE	static	void		set_cur( c_layers* layers )	{	cur = layers;		}
	FINLINE	static	void		set_cur_null()				{	set_cur( nullptr );	}
	FINLINE	static	c_layers*	get_cur()					{	return cur;			}

	FINLINE	static	void		set_ui( c_layers* layers )	{	ui = layers;		}
	FINLINE	static	void		set_ui_null()				{	set_ui( nullptr );	}
	FINLINE	static	c_layers*	get_ui()					{	return ui;			}

	FINLINE	static	c_layers*	get_layers_cam_ui()						{	return layers_cam_ui;		}
	FINLINE	static	void		set_layers_cam_ui( c_layers* layers )	{	layers_cam_ui = layers;		}

	FINLINE static	c_seedcam*	get_layers_camera_used()	{
																c_layers* layers = get_cur();
																return layers ? layers->get_camera_used() : nullptr;
															}
//todoq should be dynamic
	CONSTEXPR	static	INT32	LAYER_NB_MAX	=	26;
	CONSTEXPR	static	INT32	CAMERA_NB		=	16;
	CONSTEXPR	static	INT32	TRAX_BY_LAYERS	=	32;

	static	C_PCHAR_C	str_layer_letter[LAYER_NB_MAX+1];
private:
	static	bool	b_traxs_update;
public:
	FINLINE	static	bool*		get_traxs_update_pt()		{	return &b_traxs_update; }
	FINLINE	static	bool		is_traxs_update()			{	return b_traxs_update; }
	FINLINE	static	void		set_traxs_update( bool in )	{	b_traxs_update = in; }
private:
	INT32			_s_viewport_selector;
	INT32			_s_stereo_selector;
	bool			_b_multiple_only_ui;
#if AAA_STATE_COMPILE()
	bool			_b_state_sensitive_ui;
#endif
	INT32			_id_local_ui;

	INT32			_i_layer_cur;
//	INT32			_i_layer_begin;
	INT32			_i_layer_end;
	bool			_b_layer_active[LAYER_NB_MAX];
	c_obj_ui_array_pt<c_layer> _layer_pt;
	
	c_seedcam*		_cam_prev;
	INT32			_s_camera_ui;
	INT32			_i_camera_view;
	INT32			_i_camera_ui;
	bool			_b_camera_lock_view_to_ui;

	c_obj_value*	_values;
	bool			_b_lua_ui;
	INT32			_lua_recursion;			
	c_lua_wrap*		_lua;

	c_traxs*		_traxs;
	INT32			_traxs_channel_offset;
	INT32			_traxs_control_offset;
	bool			_b_traxs_ui;

	bool			_b_off_at_load_ui;
	bool			_b_on_at_load_ui;
	bool			_b_used_for_picking_ui;
	bool			_b_time_buffer_include_ui;
	REAL			_time_buffer_value;
	bool			_b_do_gl_finish_at_end_ui;

	c_seedcam*		_cam_used;
	c_cameras*		_cameras;
	bool			_b_transfo_ui;
	bool			_b_transfo_done;
	c_transfo_trs*	_transfo;

	bool			_b_skip_rest_trig;	//	use for lua
	bool			_b_skip_trig_ui;	//	use for trax

	INT32			_s_fbo;
	c_fbo*			_fbo;
	bool			_b_timed;

	o_str			_comment;

	c_speed*		_speed_layers;
	c_speed_ui*		_speed_layers_ui;

	FINLINE void		draw_common( bool b_vbl );
	FINLINE void		draw_single_low();
			void		draw_multiple_low();
public:
			void		draw_fbo_and_cam();
			bool		draw_begin();
			void		draw_layer_all();
			void		draw_end();

	FINLINE	void		skip_rest()			{	_b_skip_rest_trig = true;	}
			void		build_sum_up( o_str& o ) CONST;
	FINLINE bool		is_lua() CONST		{	return _b_lua_ui;		}

//	virtual void		alloc() {}; 
//	virtual void		dealloc() {};
//	virtual	void		update();
	virtual	void		set_active( bool CONST in ) final override;

	virtual	void		param_init_pt_static();
	virtual	void		param_init_pt();
	virtual	void		prepare_for_ui();
			void		init();
			void		deinit();
	virtual	o_str*		get_comment()		{	return &_comment;	}
//LAYER
			c_layer*	layer_get_always_from_index(	INT32 index	);
	FINLINE c_layer*	layer_get_raw_from_index(		INT32 index	) CONST		{	return	_layer_pt.get(index);	}
	FINLINE c_layer*	layer_get_from_index(			INT32 index	) CONST
						{
							if( index < 0 || LAYER_NB_MAX <= index )
								return nullptr;
							return layer_get_raw_from_index( index );
						}
			c_layer*	layer_load(				INT32 index,	o_str CONST & filename	);
			AAA_ERR		layer_load_from_file(	o_str CONST & filename_in	);
			AAA_ERR		layer_save_to_file(		o_str CONST & filename_in	);

			void		layer_forget(			INT32 index		);
			c_layer*	layer_new(				INT32 index		);
			bool		swap_layer(				c_param* to_move,	INT32 inc	);
			INT32		layer_get_index_from_param( p_param param );
//LAYER_UI
			INT32		layer_ui_get_index() CONST			{	return _i_layer_cur;		}
			c_layer*	layer_ui_get() CONST;

			void		layer_ui_set();
			void		layer_ui_set(			INT32 index_in	);
			void		layer_ui_inc(			INT32 inc		)
						{
							layer_ui_set( _i_layer_cur + inc );
						}

//LAYERS
			void		set_id_local( INT32 CONST id_in );
	FINLINE	INT32		get_id_local()	CONST				{	return _id_local_ui; }

#if AAA_STATE_COMPILE()
	FINLINE	bool		is_state_sensitive()				{	return _b_state_sensitive_ui; }
#endif

	FINLINE	bool		get_layer_active_switch( INT32 i )	{	return _b_layer_active[i];	}

// FBO
private:
			void		create_fbo();

public:
	FINLINE	c_fbo*		get_fbo()							{	return _fbo;	}

//CAMERA
private:
			void		create_cameras();

public:
	FINLINE	INT32		get_camera_view_index()	CONST		{	return _b_camera_lock_view_to_ui ? _i_camera_ui : _i_camera_view; }
	FINLINE	INT32		get_camera_ui_index()	CONST		{	return _i_camera_ui; }

	FINLINE	c_seedcam*	get_camera( INT32 in )	CONST		{	return _cameras ? _cameras->get_always( in ) : nullptr; }
	FINLINE	c_seedcam*	get_camera_view()		CONST		{	return get_camera( get_camera_view_index() ); }
	FINLINE	c_seedcam*	get_camera_ui()			CONST		{	return get_camera( get_camera_ui_index() ); }

			c_seedcam*	set_camera_ui( INT32 in );

	FINLINE c_seedcam*	get_camera_used()		CONST		{	return _cam_used;	}
//	FINLINE	c_cameras*	get_cameras();


			c_seedcam*	camera_ui_prev();
			c_seedcam*	camera_ui_next();
			void		set_camera_lock_view_to_ui( bool b_in );
			void		flip_camera_lock_view_to_ui();
			void		draw_camera()			{	_cameras->draw();	}
//VALUES
	FINLINE	c_obj_value*	get_values()		CONST		{	return _values;		}

//DRAW
	FINLINE	bool		is_used_for_picking()	CONST		{	return _b_used_for_picking_ui;	}


private:
	FINLINE	void	draw_tbuf_mess_low( REAL r, C_PCHAR str );
	FINLINE	void		draw_tbuf_mess( REAL r, C_PCHAR str )
	{
		if( !_b_multiple_only_ui )
			draw_tbuf_mess_low( r, str );
	}
public:
	FINLINE	void	draw_for_picking()	//todoqqq check why it's virtual and not inline
	{
		if( is_active() && is_used_for_picking() )
			draw_tbuf_mess( -1., "layers for picking" );
	}
	FINLINE	void	inline_draw()
	{
		if( is_active() )
			draw_tbuf_mess( 1., "layers" );
	}
	virtual	void	draw()
	{
		inline_draw();
	}
	FINLINE	void	draw_by_multiple()	//now check with picking
	{
		if( is_active() )
			draw_multiple_low();
	}
	virtual	bool	update_then_draw_lua();

//TRAX
private:
			void		create_traxs();
public:
	FINLINE	c_traxs*	get_traxs()				CONST	{	return _traxs;	}
			c_traxs*	get_traxs_always();
			c_trax*		traxs_plug_out(			INT32 index,	c_obj_ui* obj,	p_param param	);
			c_trax*		traxs_plug_in(			INT32 index,	c_obj_ui* obj,	p_param param	);
			void		traxs_set_focus();
			void		traxs_draw();
	FINLINE	void		set_skip_trig()
	{
		_b_skip_trig_ui = true;
	}
//MENU
			CHAR*		get_menu_header();
//LOAD/SAVE
private:
			AAA_ERR		save_to_file_private(	o_str CONST &	filename_in );
			AAA_ERR		load_from_file_private(	o_str CONST &	filename_in );
			AAA_ERR		save_to_vfile(			o_str CONST &	filename_in );
			AAA_ERR		load_from_vfile(		o_str CONST &	filename_in );
public:
	virtual	AAA_ERR		save_to_file(			o_str CONST &	filename_in );
	virtual	AAA_ERR		load_from_file(			o_str CONST &	filename_in );

	virtual c_obj_ui*	get_obj_sub_by_index( INT32 CONST index ) CONST;

	virtual	void		become_ui();
};

extern	void		layer_ui_new();
extern	void		layer_ui_forget();

extern	CHAR		g_str_local_traxs[];
