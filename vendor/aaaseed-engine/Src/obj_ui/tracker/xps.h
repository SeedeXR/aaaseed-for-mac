
#ifdef AAA_XPS_H
#error "XPS_H included more than once."
#endif
#define AAA_XPS_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_xps final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_xps, c_obj_active_ui );
private:
	INT32	_color_back;
	INT32	_color_side;
	INT32	_color_front;
	INT32	_luma;
public:
	static c_xps*	def;	// when we run
	static c_xps*	out;	// when we exit

	virtual	void	param_init_pt_static();
	virtual	void	update();
};


