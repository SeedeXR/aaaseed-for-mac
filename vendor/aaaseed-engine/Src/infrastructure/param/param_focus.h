
#ifdef AAA_PARAM_FOCUS_H
#error "PARAM_FOCUS_H included more than once."
#endif
#define AAA_PARAM_FOCUS_H 1


#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_PARAM_H
#	include "infrastructure/param/param.h"
#endif


class c_obj_ui;

//
//	FOCUS AND INTERFACE
//
namespace focus_param
{
	extern	p_param		get_param				();
	extern	INT32		get_line				();
	extern	void		set_line				(	INT32 line );
	extern	c_obj_ui*	get_obj					();
	extern	void		focus_obj_down			();
	extern	void		focus_obj_up			();
	extern	void		focus_obj_prev			();
	extern	void		focus_obj_next			();

	extern	p_param		get_menu_param			();
	extern	c_obj_ui*	get_menu_obj			();

	extern	p_param		get_param_list			();
	extern	c_obj_ui*	get_flatland_obj_focus	();
}

extern	p_param		focus_menu_param_get		();

extern	bool		param_focus_menu_is_on_line	(	INT32 line	);
extern	bool		param_focus_set_on_line		(	INT32 line	);

//par	extern	void	focus_list_set( p_param st, INT32 nb=1, CHAR* list_name = nullptr);		
extern	void		param_focus_set				(	c_obj_ui* obj,	p_param param	);
extern	void		param_focus_remove			(	c_obj_ui* obj,	p_param param	);

//
//	PARAM_SET
//
extern	void		param_set_value_to_def		();
extern	void		param_set_value_to_ina		();

extern	AAA_ERR		param_list_set_value_to_def	(	p_param param,	INT32 nb	);
extern	AAA_ERR		param_list_init_value_to_ina(	p_param param,	INT32 nb	);
extern	AAA_ERR		param_list_set_value_to_ina	(	p_param param,	INT32 nb	);	// don't set the don't save param

//
//	PARAM DISPLAY
//
extern	void		enable_param_dbg_display	();
extern	void		disable_param_dbg_display	();
extern	void		flip_param_dbg_display		();
//extern	void		enable_param_dbg_display		(	INT32 which	);
//extern	void		disable_param_dbg_display		(	INT32 which	);
//extern	void		flip_param_dbg_display			(	INT32 which	);

//
//	PARAM EXPAND
//
extern	void		enable_param_expand			();
extern	void		disable_param_expand		();
extern	void		flip_param_expand			();
extern	void		flip_param_expand_out		();
extern	void		flip_param_expand_in		();
//extern	void		flip_param_expand_list		();

//
//	PARAM AND IN/OUT
//
extern	void		param_disconnect();

//	PARAM_UPDATE
//
//	used to refresh menu display_list
extern	void	param_update();

//	FIFO
typedef	struct	_ST_STACK_ELT
{
	c_obj_ui*	obj_ui;
	p_param		param_focus;
	INT32		line_focus;
	INT32		index_nb;
	p_param		param_list;
	o_str		name;
}
ST_STACK_ELT;

//	c_fifo : fixed-capacity LIFO of param-focus stack entries (`ST_STACK_ELT`).
//	Three globals are instantiated in `aaaseed.cpp` (extern in `seedmenu.cpp`, `aaalua_glue.cpp`) :
//
//	  - `fifo_def`           : the active focus FIFO used by the UI / param browsing path
//	                           (`switch_to`, `set_cur`, `clear`). Cleared on Lua reload.
//	  - `fifo_menu_focus`    : history attached to the "Focus" menu, switch_to fired by user click.
//	  - `fifo_menu_tracker`  : history attached to the "Tracker" menu, same model.
//
//	The two `fifo_menu_*` instances are hooked to their menus once at init ; only `fifo_def`
//	carries runtime focus state.
//todo	ugly but tmp
class c_fifo
{
private:
	std::deque<ST_STACK_ELT>	_stack;
	INT32	_cur_index;

			void		fill			( ST_STACK_ELT&	elt );

			void		clean			();
//			void		set_name		( CHAR* name_in, UINT32 index);
//			void		insert			( UINT32 index );
			void		insert_at_end	();
			void		get				( UINT32 index );

			void		_store			();
			void		_push			();
			void		_pop			();

protected:
public:
	static	void		store			();
	static	void		push			();
	static	void		pop				();
			void		remove			( c_obj_ui* obj_ui, p_param	param );

	c_fifo();

	void				clear			();
	void				switch_to		( INT32 index );

	void				set_cur			();
	INT32				get_size		();
	INT32				get_cur_index	();
	C_PCHAR_C			get_name		( UINT32 index );

};

