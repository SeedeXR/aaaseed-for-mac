
#ifdef AAA_SPOTS_H
#error "SPOTS_H included more than once."
#endif
#define AAA_SPOTS_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class	c_img_2d;
class	c_param_def;

namespace	n_spots
{
	extern	void			start(				c_img_2d* image,	bool in );
	extern	void			compute();
	extern	void			show();
	extern	void			set_focus();
	extern	void			init_from_image(	c_img_2d* image );
	extern	void			deinit();
}

