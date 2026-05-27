
#ifdef AAA_CLIP_H
#error "CLIP_H included more than once."
#endif
#define AAA_CLIP_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_clip final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_clip,c_obj_active_ui);

private:
	INT32	_plane_index;
	INT32	_s_clip_active;

	DOUBLE	_equation[4];

public:
	static	c_clip*		cur;

	static	void	disable();
	virtual	void	param_init_pt_static();
	//		void	enable();
	virtual	void	update();
};

