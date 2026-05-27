#include "aaa_def.h"

#include "ui/gol_ui.h"

#include "gol/gol.h"
#include "gol/gol_os.h"
#include "gol/gol_light.h"
#include "gol/gol_debug.h"
#include "gol/gol_tex.h"
#include "gol/gol_draw.h"
#include "gol/gol_pbo.h"
#include "gol/gol_list.h"
#include "gol/gol_shader.h"

#include "gl/ubo.h"
#include "gl/ssbo.h"
#include "gl/vao.h"
#include "gl/ibo.h"
//#include "gl/fbo.h"

#include "infrastructure/param/param_declare.h"
#include "draw/tex.h"
#include "draw/seeddraw.h"

//todo move these (param concerned) in others places ?
#include "obj_ui/bdd/util/bdd.h"
#include "infrastructure/layer/layer.h"
#include "draw/render.h"
#include "draw/map.h"
#include "obj_ui/bdd/bdd_image/bdd_feedback.h"
#include "image/img_base.h"

extern	INT32	s_bbox_force;

CONSTEXPR C_PCHAR_C	bbox_force_str[ 5 ] =
{
	"OFF",
	"radius by layer",
	"radius by object",
	"bbox by layer",
	"bbox by object",
};

FACTORY_CREATE_V1( c_gol_ui, gol_ui, GOL Preferences, gol_ui );

//todo break in two list (debug pref)
namespace	n_gol_ui
{
	CONSTEXPR INT32	BASE_PARAM_NB			= 0;
	CONSTEXPR INT32	OPENGL_INFO_PARAM_NB	= 19;
	CONSTEXPR INT32	GOL_PARAM_NB			= 9;
	CONSTEXPR INT32	OPENGL_EXT_PARAM_NB		= 2;
	CONSTEXPR INT32	GOL_ERROR_PARAM_NB		= 4;
	CONSTEXPR INT32	GOL_DEBUG_PARAM_NB		= 25;
	CONSTEXPR INT32	GLOBAL_RENDER_PARAM_NB	= 20;
	CONSTEXPR INT32	GOL_POINT_PARAM_NB		= 4;
	CONSTEXPR INT32	GOL_LINE_PARAM_NB		= 6;
	CONSTEXPR INT32	GOL_MODERN_PARAM_NB		= 6;
	CONSTEXPR INT32	FBO_PARAM_NB			= 6;
	CONSTEXPR INT32	TEXTURE_PARAM_NB		= 49;
	CONSTEXPR INT32	VERTEX_RENDER_PARAM_NB	= 8;
	CONSTEXPR INT32	GOL_MEMORY_PARAM_NB		= 21;
	CONSTEXPR INT32	GROUP_PARAM_NB			= 13;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	OPENGL_INFO_PARAM_NB
									+	GOL_PARAM_NB
									+	OPENGL_EXT_PARAM_NB	
									+	GOL_ERROR_PARAM_NB
									+	GOL_DEBUG_PARAM_NB
									+	GLOBAL_RENDER_PARAM_NB
									+	GOL_POINT_PARAM_NB
									+	GOL_LINE_PARAM_NB
									+	GOL_MODERN_PARAM_NB
									+	FBO_PARAM_NB
									+	TEXTURE_PARAM_NB
									+	VERTEX_RENDER_PARAM_NB
									+	GOL_MEMORY_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_GROUP_CLOSED( OpenGL Info, OPENGL_INFO_PARAM_NB )
			PARAM_DEF_STR_LOCKED(	glew_version				)
			PARAM_DEF_STR_LOCKED(	opengl_vendor				)
			PARAM_DEF_BOOL_LOCKED(	opengl_is_nvidia			)
			PARAM_DEF_BOOL_LOCKED(	opengl_is_amd				)
			PARAM_DEF_BOOL_LOCKED(	opengl_is_intel				)
			PARAM_DEF_BOOL_LOCKED(	opengl_is_mesa				)
			PARAM_DEF_BOOL_LOCKED(	opengl_is_apple				)
			PARAM_DEF_STR_LOCKED(	opengl_renderer				)
			PARAM_DEF_STR_LOCKED(	opengl_version				)
			PARAM_DEF_INT32_LOCKED(	opengl_version_major		)
			PARAM_DEF_INT32_LOCKED(	opengl_version_minor		)
			PARAM_DEF_STR_LOCKED(	opengl_shading				)
			PARAM_DEF_STR_LOCKED(	opengl_profile				)
			PARAM_DEF_BOOL_LOCKED(	opengl_forward_compatible	)
			PARAM_DEF_BOOL_LOCKED(	opengl_debug				)
			PARAM_DEF_BOOL_LOCKED(	opengl_robust_access		)
			PARAM_DEF_BOOL_LOCKED(	opengl_no_error				)

			PARAM_DEF_INT32_LOCKED(	opengl_light_nb				)
			PARAM_DEF_INT32_LOCKED(	opengl_subpixel				)

		PARAM_DEF_GROUP_CLOSED( Flush Finish Sync, GOL_PARAM_NB )
			PARAM_DEF_BOOL_ON(		flush_allow				)
			PARAM_DEF_BOOL_ON(		finish_allow			)
			PARAM_DEF_BOOL_ON(		finish_force			)
			PARAM_DEF_BOOL_LOCKED(	vsync_can				)
			PARAM_DEF_BOOL_ON(		vsync_update			)
			PARAM_DEF_BOOL_ON(		vsync_asked				)
			PARAM_DEF_INT32_LOCKED(	vsync_state				)
			PARAM_DEF_BOOL_ON(		swap_buffer				)
			PARAM_DEF_BOOL_ON(		no_erase_force_no_swap	)

		PARAM_DEF_GROUP_CLOSED( Extensions, OPENGL_EXT_PARAM_NB )
			PARAM_DEF_BOOL_LOCKED(	blend_equation_advanced_can	)
			PARAM_DEF_BOOL_LOCKED(	blend_equation_advanced_coherent_can	)

		PARAM_DEF_GROUP_CLOSED( Error Check, GOL_ERROR_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		error_check							)
			PARAM_DEF_BOOL_OFF(		error_check_by_frame				)
			PARAM_DEF_BOOL_OFF(		error_check_by_layer				)
			PARAM_DEF_BOOL_OFF(		error_check_by_call					)

		PARAM_DEF_GROUP_CLOSED( Debug, GOL_DEBUG_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		debug_callback						)		
			PARAM_DEF_INT32_LOCKED(	debug_callback_nb					)

			PARAM_DEF_BOOL_ON(		debug_severity_low_show				)
			PARAM_DEF_INT32_LOCKED(	debug_severity_low_nb				)
			PARAM_DEF_BOOL_ON(		debug_severity_medium_show			)
			PARAM_DEF_INT32_LOCKED(	debug_severity_medium_nb			)
			PARAM_DEF_BOOL_ON(		debug_severity_high_show			)
			PARAM_DEF_INT32_LOCKED(	debug_severity_high_nb				)
			PARAM_DEF_BOOL_OFF(		debug_severity_notification_show	)
			PARAM_DEF_INT32_LOCKED(	debug_severity_notification_nb		)

			PARAM_DEF_BOOL_ON(		debug_error							)
			PARAM_DEF_BOOL_OFF(		debug_deprecated					)	// don't show deprecated errors
			PARAM_DEF_BOOL_ON(		debug_undefined						)
			PARAM_DEF_BOOL_ON(		debug_portability					)

			PARAM_DEF_BOOL_ON(		debug_performance					)
			PARAM_DEF_BOOL_OFF(		debug_performance_pbo_nvidia		)
			PARAM_DEF_BOOL_OFF(		debug_performance_shader_recompiled_by_state_nvidia	)
			PARAM_DEF_BOOL_OFF(		debug_performance_buffer_usage		)

			PARAM_DEF_BOOL_ON(		debug_other									)
			PARAM_DEF_BOOL_OFF(		debug_other_uniform_array_ignored_nvidia	)
			PARAM_DEF_BOOL_OFF(		debug_other_buffer_nvidia					)
			PARAM_DEF_BOOL_OFF(		debug_other_buffer_detailed_nvidia			)
			PARAM_DEF_BOOL_OFF(		debug_other_framebuffer_detailed_nvidia		)
			PARAM_DEF_BOOL_OFF(		debug_other_texture_base_level_undefined	)
			PARAM_DEF_BOOL_OFF(		debug_other_buffer_usage					)

		PARAM_DEF_GROUP_CLOSED( Global Allow, GLOBAL_RENDER_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		state_cache_optimisation_disable	)
			PARAM_DEF_BOOL_ON(		dither_use				)
			PARAM_DEF_BOOL_ON(		alpha_test_allow		)
			PARAM_DEF_BOOL_ON(		depth_allow				)
			PARAM_DEF_BOOL_ON(		fog_allow				)
			PARAM_DEF_BOOL_ON(		light_allow				)
			PARAM_DEF_BOOL_ON(		stencil_allow			)
			PARAM_DEF_BOOL_LOCKED(	multisampling_can		)
			PARAM_DEF_BOOL_ON(		multisampling_allow		)
			PARAM_DEF_BOOL_ON(		clipping_allow			)
			PARAM_DEF_BOOL_ON(		logic_op_allow			)
			PARAM_DEF_BOOL_ON(		bounding_box_on_current	)
			PARAM_DEF_SYMBO(		bounding_box_force,		1,0,	4,bbox_force_str )
			PARAM_DEF_BOOL_OFF(		line_force				)

			PARAM_DEF_BOOL_OFF(		polygon_smooth_allow	)
			PARAM_DEF_BOOL_ON(		list_allow				)
			PARAM_DEF_BOOL_OFF(		list_in_layer_allow		)
			PARAM_DEF_BOOL_OFF(		list_in_layer_force		)
			PARAM_DEF_BOOL_OFF(		list_in_multiple_allow	)
			PARAM_DEF_BOOL_ON(		feedback_allow			)

		PARAM_DEF_GROUP_CLOSED( Point, GOL_POINT_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		point_smooth_allow	)
			PARAM_DEF_FP32_LOCKED(	point_size_min		)
			PARAM_DEF_FP32_LOCKED(	point_size_max		)
			PARAM_DEF_FP32_LOCKED(	point_size_step		)

		PARAM_DEF_GROUP_CLOSED( Line, GOL_LINE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		line_smooth_allow		)
			PARAM_DEF_FP32_LOCKED(	line_smooth_size_min	)
			PARAM_DEF_FP32_LOCKED(	line_smooth_size_max	)
			PARAM_DEF_FP32_LOCKED(	line_smooth_size_step	)
			PARAM_DEF_FP32_LOCKED(	line_aliased_size_min	)
			PARAM_DEF_FP32_LOCKED(	line_aliased_size_max	)

		PARAM_DEF_GROUP_CLOSED( Vertex, VERTEX_RENDER_PARAM_NB )
			PARAM_DEF_BOOL_ON(		vertex_call_avoid			)
			PARAM_DEF_BOOL_OFF(		test_using_array_element	)
			PARAM_DEF_BOOL_ON(		polygon_use					)	//todo check and extend use

			PARAM_DEF_BOOL_ON(		VBO_allow					)
			PARAM_DEF_BOOL_LOCKED(	VAO_can						)
			PARAM_DEF_BOOL_ON(		VAO_allow					)
			PARAM_DEF_BOOL_LOCKED(	VAO_do						)
			PARAM_DEF_BOOL_OFF(		VAO_element_bind_include	)
				
		PARAM_DEF_GROUP_CLOSED( Modern, GOL_MODERN_PARAM_NB )
			PARAM_DEF_BOOL_LOCKED(	named_buffer_can			)
			PARAM_DEF_BOOL_ON(		named_buffer_asked			)
			PARAM_DEF_BOOL_LOCKED(	named_buffer_use			)
			PARAM_DEF_BOOL_LOCKED(	direct_state_access_can		)
			PARAM_DEF_BOOL_ON(		direct_state_access_asked	)
			PARAM_DEF_BOOL_LOCKED(	direct_state_access_use		)

		PARAM_DEF_GROUP_CLOSED( Frame Buffer Object, FBO_PARAM_NB )
			PARAM_DEF_INT32_LOCKED_XY(	viewport_max				)
			PARAM_DEF_INT32_LOCKED(		fbo_color_attachment_nb_max	)
			PARAM_DEF_BOOL_LOCKED(		fbo_can						)
			PARAM_DEF_BOOL_ON(			fbo_allow					)
			PARAM_DEF_BOOL_OFF(			fbo_verbose					)

		PARAM_DEF_GROUP_CLOSED( Texture, TEXTURE_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(	texture_size_max						)
			PARAM_DEF_BOOL_OFF(		texture_verbose							)
			PARAM_DEF_INT32_LOCKED(	texture_unit_nb_max_possible			)
			PARAM_DEF_INT32_LOCKED(	texture_unit_nb_max_GOL					)
			PARAM_DEF_INT32_LOCKED(	texture_unit_nb_used					)
			PARAM_DEF_INT32_LOCKED(	texture_sampler_nb_max_possible			)
			PARAM_DEF_INT32_LOCKED(	texture_sampler_nb_max_GOL				)
			PARAM_DEF_INT32_LOCKED(	texture_sampler_nb_max					)
			PARAM_DEF_BOOL_OFF(		texture_unit_verbose					)
			PARAM_DEF_BOOL_ON(		texture_unit_dim_set					)	//todo change how we use shade in particular use our own fix_path using the dim we store
			PARAM_DEF_BOOL_ON(		texture_allow							)
			PARAM_DEF_BOOL_ON(		texture_npot_allow						)
			PARAM_DEF_BOOL_ON(		texture_mono_is_alpha					)
			PARAM_DEF_BOOL_LOCKED(	texture_filtering_can					)
			PARAM_DEF_BOOL_OFF(		texture_filtering_anisotropic			)
			PARAM_DEF_FP32(			texture_filtering_anisotropic_value,	2, 1, 1, PARAM_MAX_FP32)
			PARAM_DEF_FP32_LOCKED(	texture_filtering_anisotropic_max		)
			PARAM_DEF_BOOL_OFF(		texture_force_reload			)		
			PARAM_DEF_BOOL_ON(		texture_lod						)
			PARAM_DEF_BOOL_OFF(		texture_compression				)
			PARAM_DEF_BOOL_LOCKED(	texture_compression_can			)
			PARAM_DEF_BOOL_OFF(		texture_compression_verbose		)
			PARAM_DEF_SYMBO(		texture_default_format,			1, 0,	(INT32)GOL::INTERNAL_TYPE_FORCE::MAX_NB - 1,	(CHAR**)GOL::internal_type_asked_str)
			//			PARAM_DEF_SYMBO_LOCKED(	texture_default_format_out,			1, 0,	(INT32)GOL::INTERNAL_TYPE::MAX_NB - 1,			(CHAR**)GOL::internal_type_str			)
			PARAM_DEF_BOOL_LOCKED(	texture_float_can				)
			PARAM_DEF_BOOL_LOCKED(	texture_swizzle_can				)
			PARAM_DEF_BOOL_LOCKED(	texture_mipmap_generate_can		)
			PARAM_DEF_BOOL_ON(		texture_mipmap_generate_allow	)
			PARAM_DEF_BOOL_OFF(		texture_sub_image_do_mipmap		)
			PARAM_DEF_BOOL_LOCKED(	copy_image_sub_can				)
			PARAM_DEF_BOOL_ON(		copy_image_sub_allow			)

			PARAM_DEF_GROUP_CLOSED( 1D, 5 )
				PARAM_DEF_BOOL_ON(	texture_1d_mipmap_generate	)
				//				PARAM_DEF_BOOL_OFF(		texture_1d_mipmap_generate_compressed	)
				PARAM_DEF_SYMBO(	texture_1d_minification,	0, 1,	PT_NB_STR(tex::minmag_mode_str),	tex::minmag_mode_str )
				PARAM_DEF_SYMBO(	texture_1d_magnification,	0, 1,	1,									tex::minmag_mode_str )
				PARAM_DEF_BOOL_OFF(	texture_1d_force_nearest	)
				PARAM_DEF_BOOL_OFF(	texture_1d_force_linear		)

			PARAM_DEF_GROUP( 2D, 6 )
				PARAM_DEF_BOOL_ON(	texture_2d_mipmap_generate	)
				PARAM_DEF_BOOL_OFF(	texture_2d_mipmap_generate_compressed	)
				PARAM_DEF_SYMBO(	texture_2d_minification,	0, 1,	PT_NB_STR(tex::minmag_mode_str),	tex::minmag_mode_str)
				PARAM_DEF_SYMBO(	texture_2d_magnification,	0, 1,	1,									tex::minmag_mode_str)
				PARAM_DEF_BOOL_OFF(	texture_2d_force_nearest	)
				PARAM_DEF_BOOL_OFF(	texture_2d_force_linear		)

			PARAM_DEF_GROUP_CLOSED( 3D, 5 )
				PARAM_DEF_BOOL_ON(	texture_3d_mipmap_generate	)
				//				PARAM_DEF_BOOL_OFF(		texture_3d_mipmap_generate_compressed	)
				PARAM_DEF_SYMBO(	texture_3d_minification,	0, 1,	PT_NB_STR(tex::minmag_mode_str),	tex::minmag_mode_str)
				PARAM_DEF_SYMBO(	texture_3d_magnification,	0, 1,	1,									tex::minmag_mode_str)
				PARAM_DEF_BOOL_OFF(	texture_3d_force_nearest	)
				PARAM_DEF_BOOL_OFF(	texture_3d_force_linear		)

		PARAM_DEF_GROUP_CLOSED( Memory, GOL_MEMORY_PARAM_NB )
			PARAM_DEF_DOUBLE_LOCKED(	memory_used_texture_MiB				)
			PARAM_DEF_DOUBLE_LOCKED(	memory_used_pbo_MiB					)
			PARAM_DEF_DOUBLE_LOCKED(	memory_used_fbo_MiB					)
			PARAM_DEF_BOOL_OFF(			opengl_memory_allow					)
			PARAM_DEF_REAL_LOCKED(		opengl_memory_total_MiB				)
			PARAM_DEF_REAL_LOCKED(		opengl_memory_free_MiB				)
			PARAM_DEF_REAL_LOCKED(		opengl_memory_free_largest_MiB		)
			PARAM_DEF_REAL_LOCKED(		opengl_memory_auxiliary_free_MiB	)
			PARAM_DEF_REAL_LOCKED(		opengl_memory_auxiliary_free_largest_MiB	)
			PARAM_DEF_INT32_LOCKED(		buffer_nb_current					)
			PARAM_DEF_INT32_LOCKED(		buffer_nb_generated					)
			PARAM_DEF_INT32_LOCKED(		gl_item_nb_current					)
			PARAM_DEF_INT32_LOCKED(		gl_item_nb_generated				)
			PARAM_DEF_INT32_LOCKED(		gl_ubo_nb_current					)
			PARAM_DEF_INT32_LOCKED(		gl_ubo_nb_generated					)
			PARAM_DEF_INT32_LOCKED(		gl_ssbo_nb_current					)
			PARAM_DEF_INT32_LOCKED(		gl_ssbo_nb_generated				)
			PARAM_DEF_INT32_LOCKED(		gl_vao_nb_current					)
			PARAM_DEF_INT32_LOCKED(		gl_vao_nb_generated					)
			PARAM_DEF_INT32_LOCKED(		gl_ibo_nb_current					)
			PARAM_DEF_INT32_LOCKED(		gl_ibo_nb_generated					)
//			PARAM_DEF_INT32_LOCKED(		gl_fbo_nb_current					)
//			PARAM_DEF_INT32_LOCKED(		gl_fbo_nb_generated					)
	};
}

void	c_gol_ui::param_init_pt_static()
{
	INT32	h = 0;

	++h;	//	OPENGL
		param_set_pt(		h, GOL::o_glew_version						);
		param_set_pt(		h, GOL::CTX::get_vendor_pt()				);
		param_set_pt(		h, GOL::CTX::b_ogl_is_nvidia				);
		param_set_pt(		h, GOL::CTX::b_ogl_is_amd					);
		param_set_pt(		h, GOL::CTX::b_ogl_is_intel					);
		param_set_pt(		h, GOL::CTX::b_ogl_is_mesa					);
		param_set_pt(		h, GOL::CTX::b_ogl_is_apple					);
		param_set_pt(		h, GOL::CTX::get_renderer_pt()				);
		param_set_pt(		h, GOL::CTX::get_version_pt()				);
		param_set_pt(		h, GOL::CTX::get_version_major_pt()			);
		param_set_pt(		h, GOL::CTX::get_version_minor_pt()			);
		param_set_pt(		h, GOL::CTX::get_shading_pt()				);
		param_set_pt(		h, GOL::CTX::get_profile_pt()				);
		param_set_pt(		h, GOL::CTX::get_forward_compatible_pt()	);
		param_set_pt(		h, GOL::CTX::get_debug_pt()					);
		param_set_pt(		h, GOL::CTX::get_robust_access_pt()			);
		param_set_pt(		h, GOL::CTX::get_no_error_pt()				);

		param_set_pt(		h, GOL::light_nb							);
		param_set_pt(		h, GOL::subpixel_bits						);

	++h;	//	GOL
		param_set_pt(		h, GOL::b_flush_allow						);
		param_set_pt(		h, GOL::b_finish_allow						);
		param_set_pt(		h, GOL::b_finish_force						);
		param_set_pt(		h, GOL::OS::b_vsync_can						);
		param_set_pt(		h, GOL::OS::b_vsync_allow_ui				);
		param_set_pt(		h, GOL::OS::b_vsync_asked_ui				);
		param_set_pt(		h, GOL::OS::vsync_state						);
		param_set_pt(		h, draw::get_swap_buffer_ui_pt()			);
		param_set_pt(		h, draw::get_no_erase_force_no_swap_pt()	);

	++h;	//	GOL EXT
		param_set_pt(		h, GOL::b_blend_equation_advanced_can			);
		param_set_pt(		h, GOL::b_blend_equation_advanced_coherent_can	);

	++h;	//	ERROR
		param_set_pt(		h, GOL::b_check_error						);
		param_set_pt(		h, GOL::b_check_error_by_frame_ui			);
		param_set_pt(		h, GOL::b_check_error_by_layer_ui			);
		param_set_pt(		h, GOL::b_check_error_by_call_ui			);

	++h;	//	GOL DEBUG
		param_set_pt(		h, GOL::b_debug_callback_ui					);

		param_set_pt(		h, GOL::error_count							);

		param_set_pt(		h, GOL::b_debug_severity_low_ui				);
		param_set_pt(		h, GOL::error_count_low						);
		param_set_pt(		h, GOL::b_debug_severity_medium_ui			);
		param_set_pt(		h, GOL::error_count_medium					);
		param_set_pt(		h, GOL::b_debug_severity_high_ui			);
		param_set_pt(		h, GOL::error_count_high					);
		param_set_pt(		h, GOL::b_debug_severity_notification_ui	);
		param_set_pt(		h, GOL::error_count_notification			);

		param_set_pt(		h, GOL::b_debug_error_ui											);
		param_set_pt(		h, GOL::b_debug_deprecated_ui										);
		param_set_pt(		h, GOL::b_debug_undefined_ui										);
		param_set_pt(		h, GOL::b_debug_portability_ui										);

		param_set_pt(		h, GOL::b_debug_performance_ui										);
		param_set_pt(		h, GOL::b_debug_performance_pbo_nvidia_ui							);
		param_set_pt(		h, GOL::b_debug_performance_shader_recompiled_by_state_nvidia_ui	);
		param_set_pt(		h, GOL::b_debug_performance_buffer_usage_ui							);

		param_set_pt(		h, GOL::b_debug_other_ui											);
		param_set_pt(		h, GOL::b_debug_other_uniform_array_ignored_nvidia_ui				);
		param_set_pt(		h, GOL::b_debug_other_buffer_nvidia_ui								);
		param_set_pt(		h, GOL::b_debug_other_buffer_detailed_nvidia_ui						);
		param_set_pt(		h, GOL::b_debug_other_framebuffer_detailed_nvidia_ui				);
		param_set_pt(		h, GOL::b_debug_other_texture_base_level_undefined_ui				);
		param_set_pt(		h, GOL::b_debug_other_buffer_usage_ui								);

	//	Global allow
	++h;	//	RENDERING
		param_set_pt(		h, GOL::__b_state_cache_no					);
		param_set_pt(		h, GOL::b_dither_asked_ui					);
		param_set_pt(		h, GOL::b_alpha_test_allow_ui				);
		param_set_pt(		h, GOL::b_depth_allow_ui					);
		param_set_pt(		h, GOL::b_fog_allow_ui						);
		param_set_pt(		h, GOL::b_lighting_allow_ui					);
		param_set_pt(		h, GOL::b_stencil_allow						);
		param_set_pt(		h, GOL::b_multisampling_can					);
		param_set_pt(		h, GOL::b_multisampling_allow_ui			);
		param_set_pt(		h, c_bdd::gb_allow_clipping					);
		param_set_pt(		h, GOL::b_logic_op_allow					);
		param_set_pt(		h, gb_bbox_see_on_cur						);
		param_set_pt(		h, s_bbox_force								);
		param_set_pt(		h, GOL::b_force_line						);
		param_set_pt(		h, GOL::b_polygon_smooth_allow				);
		param_set_pt(		h, GOL::b_list_allow						);
		param_set_pt(		h, c_layer::b_allow_list					);
		param_set_pt(		h, c_layer::b_force_list					);
		param_set_pt(		h, c_multiple::_b_allow_list				);
		param_set_pt(		h, c_bdd_feedback::get_allow_pt()			);

	++h;	//	GOL POINT
		param_set_pt(		h, GOL::b_point_smooth_allow				);
		param_set_pt_2(		h, GOL::point_size_range					);
		param_set_pt(		h, GOL::point_size_granularity				);

	++h;	//	GOL LINE
		param_set_pt(		h, GOL::b_line_smooth_allow					);
		param_set_pt_2(		h, GOL::line_size_smooth_range				);
		param_set_pt(		h, GOL::line_size_smooth_granularity		);
		param_set_pt_2(		h, GOL::line_size_aliased_range				);

	++h;	//	VERTEX
		param_set_pt(		h, GOL::b_draw_avoid_vertex_use				);
		param_set_pt(		h, GOL::b_draw_test_using_array_element		);
		param_set_pt(		h, GOL::b_poly_use							);

		param_set_pt(		h, GOL::b_vbo_allow							);
		param_set_pt(		h, GOL::b_vao_can							);
		param_set_pt(		h, GOL::b_vao_allow							);
		param_set_pt(		h, GOL::b_vao_do							);
		param_set_pt(		h, GOL::b_vao_element_bind					);

	++h;	//	MODERN
		param_set_pt(		h, GOL::b_named_buffer_can					);
		param_set_pt(		h, GOL::b_named_buffer_asked_ui				);
		param_set_pt(		h, GOL::b_named_buffer_use					);
		param_set_pt(		h, GOL::b_direct_state_access_can			);
		param_set_pt(		h, GOL::b_direct_state_access_asked_ui		);
		param_set_pt(		h, GOL::b_direct_state_access_use			);
	
	++h;	//	FBO
		param_set_pt_2(	h, GOL::viewport_max						);
		param_set_pt(		h, GOL::fbo_color_attachment_nb_max			);
		param_set_pt(		h, GOL::b_fbo_can							);
		param_set_pt(		h, GOL::b_fbo_allow_ui						);
		param_set_pt(		h, GOL::b_fbo_verbose						);

	++h;	//	TEXTURE
		param_set_pt(		h, GOL::tex_size_max						);
		param_set_pt(		h, GOL::b_tex_verbose_ui					);		
		param_set_pt(		h, GOL::tex_unit_nb_context					);
		param_set_pt(		h, GOL::TEX_UNIT_NB_MAX						);
		param_set_pt(		h, GOL::tex_unit_nb							);
		param_set_pt(		h, GOL::sampler_nb_context					);
		param_set_pt(		h, GOL::TEX_SAMPLER_NB_MAX					);
		param_set_pt(		h, GOL::sampler_nb							);
		param_set_pt(		h, GOL::b_tex_unit_verbose_ui				);
		param_set_pt(		h, GOL::b_tex_unit_dim_use_ui				);
		param_set_pt(		h, GOL::b_tex_allow_ui						);
		param_set_pt(		h, GOL::b_tex_npot_allow_ui					);
		param_set_pt(		h, GOL::b_mono_is_alpha_ui					);
		param_set_pt(		h, GOL::b_anisotropic_filtering_can			);
		param_set_pt(		h, GOL::b_anisotropic_filtering_allow_ui	);
		param_set_pt(		h, GOL::anisotropy_value					);
		param_set_pt(		h, GOL::anisotropy_max						);

		param_set_pt(		h, c_img_base::gb_force_reload_ui			);
		param_set_pt(		h, c_map::b_lod_allow_ui					);
		param_set_pt(		h, GOL::b_tex_compressed_allow_ui			);
		param_set_pt(		h, GOL::b_tex_compressed_can				);
		param_set_pt(		h, tex::b_compression_verbose_ui			);
		param_set_pt(		h, GOL::get_internal_type_def_asked_pt()	);
		//param_set_pt(		h, GOL::get_internal_type_def_pt()			);
		param_set_pt(		h, GOL::b_tex_float_can						);
		param_set_pt(		h, GOL::b_tex_swizzle_can					);
		param_set_pt(		h, GOL::b_fbo_can							);	//yes the conditions are the same
		param_set_pt(		h, GOL::b_mipmap_generate_allow_ui			);
		param_set_pt(		h, tex::b_sub_mipmap_do						);
		param_set_pt(		h, GOL::b_copy_image_sub_can				);
		param_set_pt(		h, GOL::b_copy_image_sub_allow_ui			);

		++h;
			param_set_pt(		h, tex1d._b_mipmap_generate_allow_ui		);
			//			param_set_pt(		h, tex1d._b_mipmap_generate_compressed_ui	);
			param_set_pt(		h, tex1d._s_minification_ui					);
			param_set_pt(		h, tex1d._s_magnification_ui				);
			param_set_pt(		h, tex1d.get_force_nearest_pt()				);
			param_set_pt(		h, tex1d.get_force_linear_pt()				);

		++h;
			param_set_pt(		h, tex2d._b_mipmap_generate_allow_ui		);
			param_set_pt(		h, tex2d._b_mipmap_generate_compressed_ui	);
			param_set_pt(		h, tex2d._s_minification_ui					);
			param_set_pt(		h, tex2d._s_magnification_ui				);
			param_set_pt(		h, tex2d.get_force_nearest_pt()				);
			param_set_pt(		h, tex2d.get_force_linear_pt()				);

		++h;
			param_set_pt(		h, tex3d._b_mipmap_generate_allow_ui		);
			//			param_set_pt(		h, tex23._b_mipmap_generate_compressed_ui	);
			param_set_pt(		h, tex3d._s_minification_ui					);
			param_set_pt(		h, tex3d._s_magnification_ui				);
			param_set_pt(		h, tex3d.get_force_nearest_pt()				);
			param_set_pt(		h, tex3d.get_force_linear_pt()				);

	++h;	//	GOL MEMORY
		param_set_pt(		h, tex::tex_mem_total_mb					);
		param_set_pt(		h, GOL::pbo_size_mb_out						);
		param_set_pt(		h, GOL::fbo_size_mb							);
		param_set_pt(		h, GOL::b_mem_info_do						);
		param_set_pt(		h, GOL::mem_total_mb						);
		param_set_pt(		h, GOL::mem_free_mb							);
		param_set_pt(		h, GOL::mem_free_largest_block_mb			);
		param_set_pt(		h, GOL::mem_free_auxiliary_mb				);
		param_set_pt(		h, GOL::mem_free_auxiliary_largest_block_mb	);
		param_set_pt(		h, GOL::buffer_nb_current					);
		param_set_pt(		h, GOL::buffer_nb_generated					);
		param_set_pt(		h, gl::item::get_nb_pt()					);
		param_set_pt(		h, gl::item::get_nb_created_pt()			);
		param_set_pt(		h, gl::ubo::get_nb_pt()						);
		param_set_pt(		h, gl::ubo::get_nb_created_pt()				);
		param_set_pt(		h, gl::ssbo::get_nb_pt()					);
		param_set_pt(		h, gl::ssbo::get_nb_created_pt()			);
		param_set_pt(		h, gl::c_vao::get_nb_pt()					);
		param_set_pt(		h, gl::c_vao::get_nb_created_pt()			);
		param_set_pt(		h, gl::ibo::get_nb_pt()						);
		param_set_pt(		h, gl::ibo::get_nb_created_pt()				);
//		param_set_pt(		h, gl::fbo::get_nb_pt()						);
//		param_set_pt(		h, gl::fbo::get_nb_created_pt()				);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_gol_ui)
{
	param_init_with( n_gol_ui::param, n_gol_ui::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR(c_gol_ui)


void	c_gol_ui::update_before()
{
	GOL::update();
	GOL::update_debug();
	mem::update();
}

void	c_gol_ui::update_after()
{
	GOL::update_meminfo();
	mem::update_info();
}


c_gol_ui*	c_gol_ui::cur	= nullptr;
