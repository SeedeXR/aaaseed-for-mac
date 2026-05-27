
#ifdef AAA_LENTICULAR_H
#error "LENTICULAR_H included more than once."
#endif
#define AAA_LENTICULAR_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_lenti final : public c_obj_ui
{
	FACTORY_DECLARE(c_lenti,c_obj_ui);
private:
	INT32	_img_start;
	INT32	_img_nb;
	bool	_b_img_increasing;
	INT32	_pixel_by_img_by_lens;
	bool	_b_lens_vertical;
protected:
public:
	static	c_lenti* cur;	//todo singleton

	virtual	void		param_init_pt_static();
			AAA_ERR		save_image( o_str CONST & filename );
};



