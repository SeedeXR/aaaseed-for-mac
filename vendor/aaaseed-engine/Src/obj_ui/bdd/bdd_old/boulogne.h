
#ifdef AAA_BOULOGNE_H
#error "BOULOGNE_H included more than once."
#endif
#define AAA_BOULOGNE_H 1


#define	APP_SPECIAL_BOUL() 0

#if	APP_SPECIAL_BOUL()

#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_boul final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_boul,c_obj_active_ui);
private:
	REAL	layers_time_out;
	REAL	spiral_layers_time;
	REAL	spiral_time;
	INT32	layers_spiral[4];
	REAL	longvue_angle;
	REAL	longvue_x;
	REAL	longvue_y;
public:

	virtual	void	param_init_pt();

	virtual	void	update();
};

extern	c_boul*	boul;

#endif

