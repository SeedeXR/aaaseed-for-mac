
#ifdef AAA_BIND_IMG_H
#error "BIND_IMG_H included more than once."
#endif
#define AAA_BIND_IMG_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

CONSTEXPR	INT32	BIND_1D_MAX_NB = 12;

extern	void	tex_1d_bind(				INT32 val );
													  
extern	void	tex_2d_bind(				INT32 val );
extern	void	tex_2d_bind_no_gpu_move(	INT32 val );

extern	void	tex_3d_bind(				INT32 val );
extern	void	tex_3d_bind_no_gpu_move(	INT32 val );

extern	INT32	tex_3d_get_bind_cur();
extern	INT32	tex_3d_get_bind_ui();

extern	INT32	tex_2d_get_bind_cur();
extern	INT32	tex_2d_get_bind_ui();

extern	void	bind_ui_set(				INT32 val );
extern	INT32	bind_ui_get();

extern	void	bind_img_init();
extern	void	bind_img_deinit();

