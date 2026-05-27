
#ifdef AAA_BIND_IMG_2D_H
#error "BIND_IMG_2D_H included more than once."
#endif
#define AAA_BIND_IMG_2D_H 1


#ifndef AAA_BIND_IMG_BASE_H
#	include "bind_img_base.h"
#endif
#ifndef	AAA_IMG_H
#	include "image/img.h"
#endif

class c_bind_img_2d final : public c_bind_img<c_img_2d>
{
public:
	static UINT32 bank_nb_ui;
	static UINT32 bank_size_ui;

	c_bind_img_2d( C_PCHAR_C name, C_PCHAR_C ext, UINT32 CONST bank_nb, UINT32 CONST bank_size )
		: c_bind_img<c_img_2d>( name, ext, bank_nb, bank_size )
	{
		init();
	}

	virtual	INT32		get_cur_index() CONST;
	virtual	INT32		get_ui_index()	CONST;

			//todo extend the concept of mapping to the fixed bind, we use it for fbo now (Maa 2020)
			//todo not sure it is called always right ( e.g. load_bind... )
			INT32		make_tex_index(				INT32 bank,  INT32 bind );
			CONST CHAR*	get_tex_index_str(			INT32 bank,  INT32 bind );

	virtual	void		init();

			void		init_img_from_tex(			INT32 CONST dst_bind, INT32 CONST src_bind, INT32 size_x, INT32 size_y, C_PCHAR_C str_verbose = nullptr );
			void		copy_tex_to_tex(			INT32 CONST src_bind, INT32 CONST dst_bind, bool b_mipmap_generate = true, bool const b_dst_set_format = true );
			void		move_to_gpu(				INT32 start,	INT32 nb					);

//	static	void		get_color4r_from_uv(		INT32 index,	REAL* color, REAL u, REAL v, bool b_clamped );

			REAL		get_value_max_from_rect_xy(	INT32 index,	INT32 x_begin, INT32 x_end, INT32 y_begin, INT32 y_end, aaa::COMPO what );

	//hack a grab/release bind/unbind lock/unlock
			c_img_2d*	get_img(					INT32 CONST index,
													aaa::PIXEL_FORMAT CONST format, INT32 sx, INT32 sy, bool CONST b_min,
													FP32 CONST * CONST rgba, C_PCHAR_C signature );
			c_img_2d*	get_img_ignore_bgr(			INT32 CONST index,
													aaa::PIXEL_FORMAT CONST format, INT32 sx, INT32 sy, bool CONST b_min,
													FP32 CONST * CONST rgba, C_PCHAR_C signature );
			c_img_2d*	get_img_cur(				aaa::PIXEL_FORMAT CONST format, INT32 CONST sx, INT32 CONST sy, bool CONST b_min,
													FP32 CONST * CONST rgba, C_PCHAR_C signature );
			void		do_after_a_compute(			bool b_force_nearest = false );

			bool		is_size_cur(				INT32& size_x,	INT32& size_y				);

//	static	AAA_ERR		save_texture_compressed(	INT32 index,	C_PCHAR_C filename	);
			AAA_ERR		save(						INT32 index,	C_PCHAR_C filename,	c_img_utils::FILE_TYPE type,	bool b_verbose	);

			c_img_2d*	get_with_image_size(		INT32 index,	INT32 size_x, INT32 size_y, aaa::PIXEL_FORMAT format	);
//	static	c_img_2d*	get_with_image_size(		INT32 index,	INT32 size_x, INT32 size_y, INT32 channel_nb,	c_img_2d::MEM_TYPE type	);
};

extern	void	bank_2d_move_to_gpu(				INT32 index_bank	);
extern	void	bank_2d_move_to_gpu_all();
extern	void	bank_2d_move_to_gpu_current();

extern	AAA_ERR	bank_2d_load_at_bind(				o_str CONST & filename_in,	INT32 index			);
extern	AAA_ERR	bank_2d_load_at_bank(				o_str CONST & filename_in,	INT32 index_bank	);
extern	AAA_ERR	bank_2d_load_in_bank_cur(			o_str CONST & filename_in	);
extern	AAA_ERR	bank_2d_load_at_bind_cur(			o_str CONST & filename_in	);
extern	AAA_ERR	bank_2d_save(						o_str CONST & filename_in,	INT32 index_bank	);
extern	AAA_ERR	bank_2d_save_cur(					o_str CONST & filename_in	);
extern	AAA_ERR	bank_2d_save_all(					o_str CONST & filename_in	);

extern	void	bind_img_2d_init();
extern	void	bind_img_2d_deinit();

extern	c_bind_img_2d* g_bind_img_2d;

