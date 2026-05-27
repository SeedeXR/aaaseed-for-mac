
#ifdef AAA_GOL_UI_H
#error "GOL_UI_H included more than once."
#endif
#define AAA_GOL_UI_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

// GOL Preference and UI
class	c_gol_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_gol_ui, c_obj_ui );
public:
	static	c_gol_ui*	cur;	//todo singleton

protected:
public:

	virtual	void	param_init_pt_static();

	void	update_before();
	void	update_after();

	//	virtual	AAA_ERR	load_do_after(	o_str CONST & filename );
};
