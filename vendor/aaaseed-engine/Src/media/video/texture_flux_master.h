
#ifdef AAA_TEX_FLUX_MASTER_H
#error "TEX_FLUX_MASTER_H included more than once."
#endif
#define AAA_TEX_FLUX_MASTER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_PIXEL_FORMAT_H
#	include "image/pixel_format.h"
#endif
#ifndef	AAA_CPU_H
#	include "CPU.h"
#endif
#ifndef AAA_COLOR_SPACE_H
#	include "image/convert/color_space.h"
#endif

class	c_texture_flux_master final : public c_obj_ui
{
	FACTORY_DECLARE(c_texture_flux_master, c_obj_ui);
private:
//	static	bool	b_init_pbo;

	bool	_b_convert_to_rgb_fast_ui;
//	bool	_b_convert_to_rgb_mmx_ui;
	bool	_b_convert_to_rgb_sse_ui;
	bool	_b_convert_to_rgb_sse2_ui;
	bool	_b_convert_to_rgb_sse3_ui;
	bool	_b_convert_to_rgb_avx2_ui;

	aaa::color::e_yuv_color_model	_s_yuv_bt_conv_ui;

	bool	_b_pbo_init;
	bool	_b_pbo_use_ui;
	bool	_b_pbo_doublebuffer_ui;
	bool	_b_pbo_buffer_range_ui;
	INT32	_pbo_requested_nb_ui;
	bool	_pbo_unique_by_tex_video_ui;

	//REAL	_agp_mem_app_read;
	//REAL	_agp_mem_app_write;
	//REAL	_agp_mem_pri;
	//REAL	_agp_mem_requested_mb;
	//REAL	_agp_mem_free_mb;

//	bool	_b_move_to_gpu_flip_bgr_ui;
//	bool	_b_move_to_gpu_force_32bits_ui;
	//bool	_b_pdr_enable;
	//bool	_b_pdr_disable;
	//bool	_b_pdr_flush;

	bool	_b_verbose_ui;

	bool	_b_move_from_gpu_use_pbo_ui;
//	bool	_b_move_from_gpu_use_agp_ui;
	bool	_b_move_from_gpu_bgr_ui;

//	bool	_b_skip_move_frame_ui;
	INT32	_image_flux_lock_fail_ui;
	bool	_b_image_flux_lock_fail_verbose_ui;
	bool	_b_timing_ui;

public:
	virtual	void	param_init_pt();
	//	virtual	void	update();

	virtual	AAA_ERR	load_do_after( o_str CONST & filename_in );

	virtual	void	update();
	//		c_img_2d::MEM_TYPE	get_mem_type();
	FINLINE	bool	is_timing()					CONST		{	return	_b_timing_ui;										}

	FINLINE	bool	is_convert_to_rgb_fast()	CONST		{	return	_b_convert_to_rgb_fast_ui;							}
//	FINLINE	bool	is_convert_to_rgb_mmx()		CONST		{	return	_b_convert_to_rgb_mmx_ui && c_cpu::b_MMX;			}
	FINLINE	bool	is_convert_to_rgb_sse()		CONST		{	return	_b_convert_to_rgb_sse_ui  && c_cpu::one->is_use_SSE();	}
	FINLINE	bool	is_convert_to_rgb_sse2()	CONST		{	return	_b_convert_to_rgb_sse2_ui && c_cpu::one->is_use_SSE2();	}
	FINLINE	bool	is_convert_to_rgb_sse3()	CONST		{	return	_b_convert_to_rgb_sse3_ui && c_cpu::one->is_use_SSE3();	}
	FINLINE	bool	is_convert_to_rgb_avx2()	CONST		{	return	_b_convert_to_rgb_avx2_ui && c_cpu::one->is_use_AVX2();	}

	FINLINE	bool	is_pbo_use()				CONST		{	return	_b_pbo_use_ui;					}
	FINLINE	bool	is_pbo_double_buffer()		CONST		{	return	_b_pbo_doublebuffer_ui;			}
	FINLINE	bool	is_pbo_buffer_range()		CONST		{	return	_b_pbo_buffer_range_ui;			}
	FINLINE	bool	is_pbo_one_by_tex_video()	CONST		{	return	_pbo_unique_by_tex_video_ui;	}

//	FINLINE	bool	get_pdr_enable()			CONST		{	return	_b_pdr_enable;	}
//	FINLINE	bool	get_pdr_disable()			CONST		{	return	_b_pdr_disable;	}
//	FINLINE	bool	get_pdr_flush()				CONST		{	return	_b_pdr_flush;	}

//	FINLINE	bool	is_move_to_gpu_flip_bgr()		CONST	{	return	_b_move_to_gpu_flip_bgr_ui;		}
//	FINLINE	bool	is_move_to_gpu_force_32bits()	CONST	{	return	_b_move_to_gpu_force_32bits_ui;	}

	FINLINE	bool	is_move_from_gpu_use_pbo()	CONST		{	return	_b_move_from_gpu_use_pbo_ui;	}
//	FINLINE	bool	is_move_from_gpu_use_agp()	CONST		{	return	_b_move_from_gpu_use_agp;		}
	FINLINE	bool	is_move_from_gpu_bgr()		CONST		{	return	_b_move_from_gpu_bgr_ui;		}

	FINLINE	bool	is_verbose()				CONST		{	return	_b_verbose_ui;						}

//	FINLINE	bool	is_skip_move_frame()				CONST {	return	_b_skip_move_frame_ui;				}
	FINLINE	bool	is_image_flux_lock_fail_verbose()	CONST {	return	_b_image_flux_lock_fail_verbose_ui;	}
	FINLINE	INT32	inc_image_flux_lock_fail()				{	return	++_image_flux_lock_fail_ui;			}
};

extern	c_texture_flux_master*	texture_flux_master;
