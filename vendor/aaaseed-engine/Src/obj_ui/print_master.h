
#ifdef AAA_PRINT_MASTER_H
#error "PRINT_MASTER_H included more than once."
#endif
#define AAA_PRINT_MASTER_H 1


#ifndef	AAA_OBJ_UI_H
#	include"infrastructure/obj/obj_ui.h"
#endif

class c_print_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_print_master, c_obj_ui );
protected:
	bool		_b_print_trig;
	bool		_b_print_with_dialog_trig;
	bool		_b_print_with_setup_trig;
	bool		_b_from_texture;
	INT32		_tex_bind;
	REAL		_justify[2];
	REAL		_scale_ui[3];
	REAL		_scale[2];

	INT32		_dialog_mode;

public:
	FINLINE	INT32	get_dialog_mode()	{ return _dialog_mode; }
	FINLINE	REAL	get_justify_x()		{ return _justify[0]; }
	FINLINE	REAL	get_justify_y()		{ return _justify[1]; }
	FINLINE	REAL	get_scale_x()		{ return _scale[0]; }
	FINLINE	REAL	get_scale_y()		{ return _scale[1]; }

	virtual	void	init();
	virtual	void	close();

	virtual	void	param_init_pt();
	virtual void	update();
};

extern	c_print_master*	g_print_master;

