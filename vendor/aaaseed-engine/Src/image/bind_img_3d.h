
#ifdef AAA_BIND_IMG_3D_H
#error "BIND_IMG_3D_H included more than once."
#endif
#define AAA_BIND_IMG_3D_H 1


#ifndef AAA_BIND_IMG_BASE_H
#	include "bind_img_base.h"
#endif
#ifndef	AAA_IMG_3D_H
#	include "image/img_3d.h"
#endif

class c_bind_img_3d final : public c_bind_img<c_img_3d>
{
public:
	static	UINT32	bank_nb_ui;
	static	UINT32	bank_size_ui;

	c_bind_img_3d( C_PCHAR_C name, C_PCHAR_C ext, UINT32 CONST bank_nb, UINT32 CONST bank_size )
		: c_bind_img<c_img_3d>( name, ext, bank_nb, bank_size )
	{
		init();
	}

			c_img_3d*	get_img(		INT32 CONST index,
										aaa::PIXEL_FORMAT CONST format, INT32 sx, INT32 sy, INT32 sz, bool b_min,
										FP32 CONST* rgba, C_PCHAR_C signature );
			c_img_3d*	get_img_cur(	aaa::PIXEL_FORMAT CONST format, INT32 sx, INT32 sy, INT32 sz, bool b_min,
										FP32 CONST* rgba, C_PCHAR_C signature );
			void		do_after_a_compute( bool b_force_nearest = false );

	virtual	INT32		get_cur_index()	CONST;
	virtual	INT32		get_ui_index()	CONST;

	virtual	void		init();

			void		move_to_gpu( INT32 start, INT32 nb );
};


extern	void	bind_img_3d_init();
extern	void	bind_img_3d_deinit();

extern	c_bind_img_3d* g_bind_img_3d;

