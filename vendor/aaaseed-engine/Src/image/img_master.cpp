#include "img_master.h"
#include "img_utils.h"
#include "infrastructure/param/param_declare.h"
#include "wrapper/aaa_opencv.h"
#include "bind_img_2d.h"
#include "bind_img_3d.h"
#include "obj_ui/com/net.h"



FACTORY_CREATE_V1( c_img_master, img_master, Image Master, img_master );

namespace
{
	static	CONST	INT32	BASE_PARAM_NB	= 17;
	static	CONST	INT32	RESCALE_PARAM_NB = 5;
	static	CONST	INT32	NET_PARAM_NB	= 4;
	static	CONST	INT32	DEBUG_PARAM_NB	= 3;
	static	CONST	INT32	OPENCV_PARAM_NB	= 16;
	static	CONST	INT32	GROUP_PARAM_NB	= 4;
	static	CONST	INT32	PARAM_NB		=	BASE_PARAM_NB
											+	RESCALE_PARAM_NB
											+	NET_PARAM_NB
											+	DEBUG_PARAM_NB
											+	OPENCV_PARAM_NB
											+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		PARAM_DEF_BOOL_OFF(			free_when_on_board			)

		PARAM_DEF_BOOL_OFF(			tga_read_custom				)
		PARAM_DEF_SYMBO_PSTR_ZERO(	lib_used,					c_img_utils::str_lib_name	)
		PARAM_DEF_BOOL_OFF(			lib_print_format_trig		)

		PARAM_DEF_BOOL_OFF(			read_rgb_add_alpha			)
		PARAM_DEF_BOOL_OFF(			read_bgr_keep				)

		PARAM_DEF_INT32(			reader_2d_thread_nb,		1,2,	1,64	) 
		PARAM_DEF_INT32(			reader_2d_thread_sleep,		0,1,	-1,2000	)
		PARAM_DEF_INT32_LOCKED(		reader_2d_waiting_nb		)
		PARAM_DEF_INT32_LOCKED(		reader_2d_reading_nb		)

		PARAM_DEF_INT32(			reader_3d_thread_nb,		1,2,	1,64	) 
		PARAM_DEF_INT32(			reader_3d_thread_sleep,		0,1,	-1,2000	)
		PARAM_DEF_INT32_LOCKED(		reader_3d_waiting_nb		)
		PARAM_DEF_INT32_LOCKED(		reader_3d_reading_nb		)

		PARAM_DEF_BOOL_OFF(			compress_write				)
		PARAM_DEF_BOOL_OFF(			compress_read				)

		PARAM_DEF_BOOL_OFF(			bdd_img_force_compute		)

		PARAM_DEF_GROUP( Scale On Load, RESCALE_PARAM_NB )
			PARAM_DEF_BOOL_OFF_SAVE_NOT(	scale_on_load_active	)
			PARAM_DEF_FP32_ZERO_ONE(		scale_on_load_factor	)
			PARAM_DEF_INT32(				scale_on_load_size_min,	1024,512, 4,1024*64 )
			PARAM_DEF_BOOL_OFF(				scale_on_load_save		)
			PARAM_DEF_STR(					scale_on_load_path		)

		PARAM_DEF_GROUP( Net, NET_PARAM_NB )
			PARAM_DEF_BOOL_ON(		net_in_active				)
			PARAM_DEF_BOOL_OFF(		net_in_update_continuous	)
			PARAM_DEF_BOOL_ON(		net_out_active				)
			PARAM_DEF_INT32(		net_out_blk_size_max,		1024*16,1024*32,	1024,c_net::BLK_SIZE_MAX )


		PARAM_DEF_GROUP( Debug, DEBUG_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		memory_check_after_read		)
			PARAM_DEF_BOOL_ON(		unimplemented_verbose		)
			PARAM_DEF_BOOL_OFF(		unimplemented_break			)

		PARAM_DEF_GROUP( OpenCV, OPENCV_PARAM_NB )
	//		PARAM_DEF_STR_LOCKED(	opencv_version				)
			PARAM_DEF_INT32_LOCKED(	opencv_version_major		)
			PARAM_DEF_INT32_LOCKED(	opencv_version_minor		)
			PARAM_DEF_INT32_LOCKED(	opencv_version_revision		)
			PARAM_DEF_BOOL_OFF(		opencv_print_info_trig		)

			PARAM_DEF_BOOL_ON(		opencv_use_ipp				)

			PARAM_DEF_BOOL_LOCKED(	opencv_opencl_can			)
			PARAM_DEF_BOOL_OFF(		opencv_opencl_allow			)
			PARAM_DEF_BOOL_LOCKED(	opencv_opencl_use			)
//			PARAM_DEF_INT32_POS(	opencv_ocl_device_use,		1,0	)

			PARAM_DEF_INT32_LOCKED(	opencv_cuda_device_nb		)
			PARAM_DEF_INT32_POS(	opencv_cuda_device_asked,	1,0	)
			PARAM_DEF_INT32_LOCKED(	opencv_cuda_device_used		)
			PARAM_DEF_BOOL_ON(		opencv_cuda_allow			)
			PARAM_DEF_BOOL_LOCKED(	opencv_cuda_use				)

			PARAM_DEF_BOOL_LOCKED(	opencv_opengl_can_forced	)
			PARAM_DEF_BOOL_OFF(		opencv_opengl_allow			)
			PARAM_DEF_BOOL_LOCKED(	opencv_opengl_use			)
	};

}

CONSTRUCTOR_CREATE( c_img_master )
{
	param_init_with( param, PARAM_NB );
}


void	c_img_master::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, c_img_base::gb_free_when_on_board_ui		);
	
	param_set_pt( h, _b_tga_read_custom							);
	param_set_pt( h, c_img_utils::s_lib							);
	param_set_pt( h, c_img_utils::b_lib_print_format_trig_ui	);

	param_set_pt( h, _b_read_rgb_add_alpha	);
	param_set_pt( h, _b_read_bgr_keep		);

	auto CONST & info_2d = g_bind_img_2d->_st_reader_info;
	param_set_pt( h, info_2d._thread_nb		);
	param_set_pt( h, info_2d._sleep_time	);
	param_set_pt( h, info_2d._waiting_nb	);
	param_set_pt( h, info_2d._reading_nb	);

	auto CONST & info_3d = g_bind_img_2d->_st_reader_info;
	param_set_pt( h, info_3d._thread_nb	);
	param_set_pt( h, info_3d._sleep_time	);
	param_set_pt( h, info_3d._waiting_nb	);
	param_set_pt( h, info_3d._reading_nb	);

	param_set_pt( h, _b_compress_write		);
	param_set_pt( h, _b_compress_read		);

	param_set_pt( h, _b_bdd_img_force_compute	);

	++h;
		param_set_pt( h, _b_scale_on_load						);
		param_set_pt( h, _scale_on_load_factor					);
		param_set_pt( h, _scale_on_load_size_min				);
		param_set_pt( h, _b_scale_on_load_save					);
		param_set_pt( h, _scale_on_load_path					);

	++h;
		param_set_pt( h, _b_net_in_active						);
		param_set_pt( h, _b_net_in_update_continuous			);
		param_set_pt( h, _b_net_out_active						);
		param_set_pt( h, _net_out_blk_size_max					);
		
	++h;
		param_set_pt( h, _b_memory_check_after_reading			);
		param_set_pt( h, _b_unimplemented_verbose				);
		param_set_pt( h, _b_unimplemented_break					);

	++h;
		param_set_pt( h, aaa::c_cv::opencv_version_major		);
		param_set_pt( h, aaa::c_cv::opencv_version_minor		);
		param_set_pt( h, aaa::c_cv::opencv_version_revision		);
		param_set_pt( h, aaa::c_cv::b_display_info_trig			);

		param_set_pt( h, aaa::c_cv::b_use_ipp_ui				);

		param_set_pt( h, aaa::c_cv::b_opencl_can_out			);
		param_set_pt( h, aaa::c_cv::b_opencl_allow_ui			);
		param_set_pt( h, aaa::c_cv::b_opencl_use_out			);
//		param_set_pt( h, aaa::c_cv::ocl_device_use				);

		param_set_pt( h, aaa::c_cv::cuda_device_nb				);
		param_set_pt( h, aaa::c_cv::cuda_device_index_asked_ui	);
		param_set_pt( h, aaa::c_cv::cuda_device_index_used		);
		param_set_pt( h, aaa::c_cv::b_cuda_allow_ui				);
		param_set_pt( h, aaa::c_cv::b_cuda_use_out				);

		param_set_pt( h, aaa::c_cv::b_opengl_can_out			);
		param_set_pt( h, aaa::c_cv::b_opengl_allow_ui			);
		param_set_pt( h, aaa::c_cv::b_opengl_use_out			);

	err_param_init_pt( h );
}

void	c_img_master::update()
{
}
EMPTY_DESTRUCTOR( c_img_master )

c_img_master*	g_img_master = nullptr;

