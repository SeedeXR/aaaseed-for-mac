
#ifdef AAA_DUST_H
#error "DUST_H included more than once."
#endif
#define AAA_DUST_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern	void	dust_draw(		c_img_2d* image, INT32 nb, REAL size_cloud );
extern	void	dust_show_draw( c_img_2d* image, INT32 nb, REAL size_cloud );
extern	void	dust_prev_draw( c_img_2d* image, INT32 nb, REAL size_cloud );
extern	void	dust_new_draw(	c_img_2d* image, INT32 nb, REAL size_cloud );


