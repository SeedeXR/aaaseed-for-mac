#include "texture_flux_master.h"
#include "gol/gol_pbo.h"
#include "media/Video/TextureBuffer.h"
#ifndef	AAA_PARAM_DECLARE_H
#	include "infrastructure/param/param_declare.h"
#endif
#include "draw/tex.h"

FACTORY_CREATE_V1( c_texture_flux_master, texture_flux_master, Texture Flux Master, tex_flux_master );

namespace n_texture_flux_master
{
	CONSTEXPR INT32	BASE_PARAM_NB			= 1;
	CONSTEXPR INT32	PBO_PARAM_NB			= 10;
//	CONSTEXPR INT32	AGP_PARAM_NB			= 5;
	CONSTEXPR INT32	CONVERT_PARAM_NB		= 7;
	CONSTEXPR INT32	MOVE_TO_GPU_PARAM_NB	= 0;
	CONSTEXPR INT32	MOVE_FROM_GPU_PARAM_NB	= 2;
	CONSTEXPR INT32	IMAGE_FLUX_PARAM_NB		= 2;
	CONSTEXPR INT32	GROUP_NB				= 4;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	PBO_PARAM_NB
							//		+	AGP_PARAM_NB
									+	CONVERT_PARAM_NB
									+	MOVE_TO_GPU_PARAM_NB
									+	MOVE_FROM_GPU_PARAM_NB
									+	IMAGE_FLUX_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON(			verbose						)

		PARAM_DEF_GROUP( Pixel Buffer Object, PBO_PARAM_NB )
			PARAM_DEF_BOOL_LOCKED(		pbo_can						)
			PARAM_DEF_BOOL_OFF(			pbo_use						)		//todonow cleaner ui
			PARAM_DEF_BOOL_ON(			pbo_use_several				)		//todonow cleaner ui
																			//todo if false LV Kusama fail
			PARAM_DEF_BOOL_OFF(			pbo_use_double_buffer		)
			PARAM_DEF_BOOL_OFF(			pbo_use_buffer_range		)
			PARAM_DEF_INT32_POS(		pbo_requested_nb, 1, 16		)
			PARAM_DEF_INT32_LOCKED(		pbo_free_nb					)
			PARAM_DEF_BOOL_ON(			pbo_unique_by_tex_video		)
			PARAM_DEF_BOOL_OFF(			pbo_free_dealloc			)
			PARAM_DEF_DOUBLE_LOCKED(	pbo_size_used				)
		//PARAM_DEF_GROUP( AGP, AGP_PARAM_NB )
		//	PARAM_DEF_REAL_ONE_ZERO( agp_mem_application_read )
		//	PARAM_DEF_REAL_ZERO_ONE( agp_mem_application_write )
		//	PARAM_DEF_REAL_ZERO_ONE( agp_mem_priority )
		//	PARAM_DEF_REAL_POS( agp_mem_requested_MB, 0, 16 )
		//	PARAM_DEF_REAL_LOCKED( agp_mem_free_MB )

		PARAM_DEF_GROUP( Convert, CONVERT_PARAM_NB )
			PARAM_DEF_BOOL_ON(		convert_to_rgb_fast			)
//			PARAM_DEF_BOOL_OFF(		convert_use_MMX				)
			PARAM_DEF_BOOL_OFF(		convert_use_SSE				)
			PARAM_DEF_BOOL_OFF(		convert_use_SSE2			)
			PARAM_DEF_BOOL_OFF(		convert_use_SSE3			)
			PARAM_DEF_BOOL_OFF(		convert_use_AVX2			)
			PARAM_DEF_SYMBO_PSTR(	convert_yuv_color_space,	0,1,	aaa::color::yuv_color_model_str )
			PARAM_DEF_BOOL_OFF(		timing_print				)

//		PARAM_DEF_GROUP( move_to_gpu, MOVE_TO_GPU_PARAM_NB )
//			PARAM_DEF_BOOL_OFF(		move_to_gpu_flip_bgr		)
//			PARAM_DEF_BOOL_OFF(		move_to_gpu_force_32bits	)
//			PARAM_DEF_BOOL_OFF(		move_to_gpu_pdr_enable )
//			PARAM_DEF_BOOL_OFF(		move_to_gpu_pdr_flush )
//			PARAM_DEF_BOOL_OFF(		move_to_gpu_pdr_disable )

		PARAM_DEF_GROUP( move_from_gpu, MOVE_FROM_GPU_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		move_from_gpu_use_pbo		)
//			PARAM_DEF_BOOL_OFF(		move_from_gpu_use_agp )
			PARAM_DEF_BOOL_ON(		move_from_gpu_bgr			)

		PARAM_DEF_GROUP( image_flux, IMAGE_FLUX_PARAM_NB )
//			PARAM_DEF_BOOL_OFF(		skip_move_frame )
			PARAM_DEF_INT32_LOCKED(	image_flux_lock_fail )
			PARAM_DEF_BOOL_OFF(		image_flux_lock_fail_verbose )
	};
}

void	c_texture_flux_master::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, _b_verbose_ui );

	++h;
		param_set_pt( h, GOL::b_pbo_can					);
		param_set_pt( h, _b_pbo_use_ui					);
		param_set_pt( h, GOL::b_pbo_use_several_ui		);
		param_set_pt( h, _b_pbo_doublebuffer_ui			);
		param_set_pt( h, _b_pbo_buffer_range_ui			);
		param_set_pt( h, _pbo_requested_nb_ui			);
		param_set_pt( h, GOL::pbo_free_nb_out			);
		param_set_pt( h, _pbo_unique_by_tex_video_ui	);
		param_set_pt( h, GOL::b_pbo_free_dealloc_ui		);
		param_set_pt( h, GOL::pbo_size_mb_out			);

	//++h;
	//	param_set_pt( h, _agp_mem_app_read );
	//	param_set_pt( h, _agp_mem_app_write );
	//	param_set_pt( h, _agp_mem_pri );
	//	param_set_pt( h, _agp_mem_requested_mb );
	//	param_set_pt( h, _agp_mem_free_mb );

	++h;
		param_set_pt( h, _b_convert_to_rgb_fast_ui	);
//		param_set_pt( h, _b_convert_to_rgb_mmx_ui	);
		param_set_pt( h, _b_convert_to_rgb_sse_ui	);
		param_set_pt( h, _b_convert_to_rgb_sse2_ui	);
		param_set_pt( h, _b_convert_to_rgb_sse3_ui	);
		param_set_pt( h, _b_convert_to_rgb_avx2_ui	);
		param_set_pt( h, _s_yuv_bt_conv_ui			);
		param_set_pt( h, _b_timing_ui				);

//	++h;
//		param_set_pt( h, _b_move_to_gpu_flip_bgr_ui );
//		param_set_pt( h, _b_move_to_gpu_force_32bits_ui );
		//param_set_pt( h, _b_pdr_enable );
		//param_set_pt( h, _b_pdr_flush );
		//param_set_pt( h, _b_pdr_disable );

	++h;
		param_set_pt( h, _b_move_from_gpu_use_pbo_ui );
//		param_set_pt( h, _b_move_from_gpu_use_agp_ui );
		param_set_pt( h, _b_move_from_gpu_bgr_ui );

	++h;
//		param_set_pt( h, _b_skip_move_frame_ui );
		param_set_pt( h, _image_flux_lock_fail_ui );
		param_set_pt( h, _b_image_flux_lock_fail_verbose_ui );

	err_param_init_pt( h );	//to transform this with a return for all obj
}

CONSTRUCTOR_CREATE( c_texture_flux_master )
	,_image_flux_lock_fail_ui	(	0		)
	,_b_pbo_init				(	false	)
{
	param_init_with( n_texture_flux_master::param, n_texture_flux_master::PARAM_NB_MAX );
}

//EMPTY_DESTRUCTOR( c_texture_flux_master )
c_texture_flux_master::~c_texture_flux_master()
{
	//if( _b_pbo_use_ui )
	GOL::pbo_dealloc_buf();
}

AAA_ERR	c_texture_flux_master::load_do_after( o_str CONST & filename )
{
	//if( _b_pbo_use_ui )
	//{
	//	GOL::pbo_alloc_buf( _pbo_requested_nb_ui );
	//}
	//else
	//{
	//	INT32	agp_mem_requested = _agp_mem_requested_mb * 1024. * 1024.;
	//	c_texture_buffer::init_agp_memory( agp_mem_requested, _agp_mem_app_read, _agp_mem_app_write, _agp_mem_pri );
	//}
	return AAA_OK;
}

void	c_texture_flux_master::update()
{
//	_agp_mem_free_mb = c_texture_buffer::get_agp_free_size();
	if( _b_pbo_use_ui && !_b_pbo_init )
	{
		GOL::pbo_alloc_buf( _pbo_requested_nb_ui );
		_b_pbo_init = true;
	}
	tex::update();
	aaa::color::set_yuv_color_model_used( _s_yuv_bt_conv_ui );
}

c_texture_flux_master*	texture_flux_master = nullptr;
