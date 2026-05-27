
#ifdef AAA_SCREEN_DEF_H
#error "SCREEN_DEF_H included more than once."
#endif
#define AAA_SCREEN_DEF_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

extern		INT32	s_screen_def_type;
extern		REAL	screen_def_cos_freq;
extern		REAL	screen_def_cos_limit;
extern		REAL	screen_def_z_factor;
extern		REAL	screen_def_y_origin;
extern		REAL	screen_def_y_factor;

class	c_screen_def final : public c_obj_ui
{
	FACTORY_DECLARE(c_screen_def,c_obj_ui);
private:
	INT32	_s_screen_def;
	REAL	_cos_freq;
	REAL	_cos_limit;
	REAL	_z_factor;
	REAL	_y_origin;
	REAL	_y_factor;

	INT32	_net_channel;
public:

	virtual	void	update();
	virtual	void	param_init_pt();
};

extern	c_screen_def*	screen_def_cur;

