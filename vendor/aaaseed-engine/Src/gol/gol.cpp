
#include "gol/gol.h"
#include "gol/gol_os.h"
#include "gol/gol_debug.h"
#include "gol/gol_list.h"
#include "gol/gol_draw.h"
#include "gol/gol_tex.h"
#include "gol/gol_color.h"
#include "gol/gol_light.h"
#include "gol/gol_shader.h"
#include "err.h"
#include "spy.h"
//todo clean interaction GOL shader
#include "shaders/shading.h"



//	maa use http://www.glprogramming.com/red/appendixb.html for reference

namespace GOL	{
//	pour memoire
#if	AAA_SCREEN_DEFORMATION()
	INT32	s_screen_def_type = 0;
	REAL	screen_def_cos_freq;
	REAL	screen_def_cos_limit;
	REAL	screen_def_z_factor;
	REAL	screen_def_y_origin;
	REAL	screen_def_y_factor;
#endif //#if	AAA_SCREEN_DEFORMATION()

	bool	b_have_alpha			= false;

	GLenum	draw_buffer_cur = GL_NONE;	// get_draw_buffer() return wrong until set_draw_buffer is called

	bool	b_mask_red				= true;
	bool	b_mask_green			= true;
	bool	b_mask_blue				= true;
	bool	b_mask_alpha			= true;

	FP32	point_size_range[2];			// data from gl at init
	FP32	point_size_granularity;			// data from gl at init
	bool	b_point_smooth_allow	= false;
	bool	b_point_smooth_state	= false;
	FP32	point_size_state		= 1.;

	FP32	line_size_aliased_range[2];		// data from gl at init
	FP32	line_size_smooth_range[2];		// data from gl at init
	FP32	line_size_smooth_granularity;	// data from gl at init
	bool	b_line_smooth_allow		= false;
	bool	b_line_smooth_state		= false;
	FP32	line_width_state		= 1.;

	bool	b_polygon_smooth_allow	= false;
	bool	b_polygon_smooth_state	= false;

	GLenum	s_polygon_mode_back		= GL_FILL;
	GLenum	s_polygon_mode_front	= GL_FILL;
	bool	b_force_line			= false;

	INT32	s_cull					= GL_NONE;
	INT32	s_cull_private			= GL_BACK;
	bool	b_culling				= false;

	bool	b_clockwise				= false;	//gl default

	bool	b_color_material_state	= false;

	bool	b_lighting_allow_ui		= false;
	bool	b_lighting				= false;

	bool		b_alpha_test_allow_ui	= true;
	bool		b_alpha_test			= false;
	GLenum		alpha_func				= GL_ALWAYS;
	GLclampf	alpha_ref				= 0.;

	bool	b_depth_allow_ui		= true;
	bool	b_depth_test			= false;
	bool	b_depth_write			= true;
	GLenum	depth_func				= GL_LESS;

	bool	b_blend_equation_advanced_can			= false;
	bool	b_blend_equation_advanced_coherent_can	= false;

	bool	b_blend							= false;	
	GLenum	blend_src_factor		= GL_ONE;
	GLenum	blend_dst_factor		= GL_ZERO;

	GLenum	shade_model_cur			= GL_SMOOTH;

	bool	b_fog_allow_ui			= true;
	bool	b_fog					= false;

	bool	b_stencil_allow			= false;
	bool	b_stencil_state			= false;

	bool	b_dither_state			= true;
	bool	b_dither_asked_ui		= true;

	bool	b_scissor_state			= false;

	bool	b_draw_avoid_vertex_use			= true;
	bool	b_draw_test_using_array_element	= false;
	bool	b_poly_use;

	bool	b_client_state_vertex	= false;
	bool	b_client_state_normal	= false;
	bool	b_client_state_texcoor	= false;
	bool	b_client_state_color	= false;

	bool	b_logic_op_allow		= false;
	bool	b_logic_op_state		= false;
	INT32	s_logic_op				= GL_COPY;

	bool	b_flush_allow			= true;
	bool	b_finish_allow			= true;
	bool	b_finish_force			= true;

	INT32	bit_nb_red			;
	INT32	bit_nb_green		;
	INT32	bit_nb_blue			;
	INT32	bit_nb_alpha		;
	INT32	bit_nb_depth		;
	INT32	bit_nb_index		;
	INT32	bit_nb_stencil		;

	INT32	bit_nb_accum_red	;
	INT32	bit_nb_accum_green	;
	INT32	bit_nb_accum_blue	;
	INT32	bit_nb_accum_alpha	;

//LIGHTING
//
	void	flip_lighting_allow()
	{
		b_lighting_allow_ui = !b_lighting_allow_ui;
		SWITCH_PRINT_STATE( "Light_Allow", b_lighting_allow_ui );
	}


	FINLINE void	reset_depth()
	{
		disable_depth_test_direct();
		enable_depth_write_direct();
		set_depth_func_direct( GL_LESS );
	}

	void	reset()
	{
		reset_depth();
		reset_blend();
		reset_alpha_test();
	//Dither
		b_dither_state			= !b_dither_asked_ui;
		set_dither( b_dither_asked_ui );
	//Scissor
		b_scissor_state			= true;
		set_scissor( false );
	//Point
		b_point_smooth_state	= true;
		disable_point_smooth();
		reset_point_size();
	//Line
		reset_line_smooth();
		reset_line_width();
	//Poly
		b_polygon_smooth_state	= true;
		disable_polygon_smooth();
	//Color
		b_mask_red				= false;
		b_mask_green			= false;
		b_mask_blue				= false;
		b_mask_alpha			= false;
		set_mask_color( true, true, true, true );

		b_color_material_state	= true;
		disable_color_material();
	//Light
		reset_lighting();
		reset_light();
	//Shade Model
		reset_shade_model();		
	//Fog
		reset_fog();
	//Texture
		reset_texture();
	//Stencil
		b_stencil_state			= true;
		disable_stencil();
	//Logic_op
		b_logic_op_state		= true;
		disable_logic_op();
		s_logic_op = GL_CLEAR;
		logic_op( GL_COPY );

		GOL::disable( GL_CLIP_PLANE0 );
		GOL::disable( GL_CLIP_PLANE1 );
		GOL::disable( GL_CLIP_PLANE2 );
		GOL::disable( GL_CLIP_PLANE3 );
		GOL::disable( GL_CLIP_PLANE4 );
		GOL::disable( GL_CLIP_PLANE5 );
	//Polygon
		reset_polygon_mode();
		reset_front_face_clockwise();
		reset_cull();
	}

//MEM
//
	enum MEMINFO : INT32
	{
		MEMINFO_NONE = 0,
		MEMINFO_AMD,
		MEMINFO_NVIDIA,
		MEMINFO_UNKNOWN,
	};

	bool	b_mem_info_can = false;
	bool	b_mem_info_do = false;
	UINT32	mem_info_vendor = MEMINFO_NONE;
	REAL	mem_total_mb = .0;
	REAL	mem_free_mb = .0;
	REAL	mem_free_largest_block_mb = .0;
	REAL	mem_free_auxiliary_mb = .0;
	REAL	mem_free_auxiliary_largest_block_mb = .0;

	void	update_meminfo_ati()
	{
		GLint param[ 4 ];

#ifndef	GL_TOTAL_PHYSICAL_MEMORY_ATI
#	define	GL_TOTAL_PHYSICAL_MEMORY_ATI	0x87FE
#endif
		get_integer( GL_TOTAL_PHYSICAL_MEMORY_ATI, param );
		mem_total_mb = param[0] / REAL(1024);

		// each gives us the same answer
		//			get_integer( GL_VBO_FREE_MEMORY_ATI, param );
		get_integer( GL_TEXTURE_FREE_MEMORY_ATI, param );
		//			get_integer ( GL_RENDERBUFFER_FREE_MEMORY_ATI, param );
		//GOOD_PRINT_STRING_1T( "Total memory free in the pool: %i Kibyte\n", param[0]);
		//GOOD_PRINT_STRING_1T( "Largest available free block in the pool: %i Kibyte\n", param[1]);
		//GOOD_PRINT_STRING_1T( "Total auxiliary memory free: %i Kibyte\n", param[2]);
		//GOOD_PRINT_STRING_1T( "Largest auxiliary free block: %i Kibyte\n", param[3]);
		mem_free_mb							= param[0] / REAL(1024);
		mem_free_largest_block_mb			= param[1] / REAL(1024);
		mem_free_auxiliary_mb				= param[2] / REAL(1024);
		mem_free_auxiliary_largest_block_mb = param[3] / REAL(1024);
	}
	void	update_meminfo_nvidia()
	{
		//DBG_PRINT_STRING( "nVidia OpenGL MEMINFO present but not retrieved" );
		#define	GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX			0x9047
		#define	GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX		0x9048
		#define	GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX	0x9049
		#define	GPU_MEMORY_INFO_EVICTION_COUNT_NVX				0x904A
		#define	GPU_MEMORY_INFO_EVICTED_MEMORY_NVX				0x904B
	
		GLint param[ 4 ];
		//get_integer( GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, param );
		//gol_total_memory_mb = REAL( param[0] >> 10 );

		get_integer( GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, param );
		mem_total_mb = REAL( param[0] >> 10 );

		get_integer( GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, param );
		mem_free_mb = REAL( param[0] >> 10 );

		//get_integer( GPU_MEMORY_INFO_EVICTION_COUNT_NVX, param );
		//get_integer( GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, param );

		mem_free_largest_block_mb = 0 / 1024;
		mem_free_auxiliary_mb = 0 / 1024;
		mem_free_auxiliary_largest_block_mb = 0 / 1024;

	}
	void	update_meminfo()
	{
		if( b_mem_info_can && b_mem_info_do )
		{
			switch( mem_info_vendor )
			{
			case MEMINFO_AMD :		update_meminfo_ati();			break;
			case MEMINFO_NVIDIA :	update_meminfo_nvidia();		break;
			}
		}
	}

	void	init_meminfo()
	{
		if( have_extension( "GL_ATI_meminfo" ) )
		{
			b_mem_info_can = true;
			mem_info_vendor = MEMINFO_AMD;
			update_meminfo();
		}
		else if( have_extension( "GL_NVX_gpu_memory_info" ) )
		{
			b_mem_info_can = true;
			mem_info_vendor = MEMINFO_NVIDIA;
			update_meminfo();
		}
		else
		{
			b_mem_info_can = false;
		}
	}

//COLOR
//
	void	reset_scale_bias()
	{
		GOL::set_pixel_transferf(	GL_RED_SCALE,	1.	);
		GOL::set_pixel_transferf(	GL_RED_BIAS,	0.	);
		GOL::set_pixel_transferf(	GL_GREEN_SCALE,	1.	);
		GOL::set_pixel_transferf(	GL_GREEN_BIAS,	0.	);
		GOL::set_pixel_transferf(	GL_BLUE_SCALE,	1.	);
		GOL::set_pixel_transferf(	GL_BLUE_BIAS,	0.	);
		GOL::set_pixel_transferf(	GL_ALPHA_SCALE,	1.	);
		GOL::set_pixel_transferf(	GL_ALPHA_BIAS,	0.	);
	}

//FBO
//
	bool	b_fbo_can		= false;
	bool	b_fbo_allow_ui	= false;
	bool	b_fbo_do		= false;
	bool	b_fbo_verbose	= true;

	DOUBLE	fbo_size_mb	= .0;
	GLuint	fbo_id_cur;

	void	init_fbo()
	{
		b_fbo_can = test_version_or_extensions( 3,0, "GL_ARB_framebuffer_object", "GL_EXT_framebuffer_object" );
		if( b_fbo_can )
			GOOD_PRINT_STRING( "FBO can use" );
		else
			WARNING_PRINT_STRING( "No Frame buffer Object in this Opengl Context." );
			
		if( b_fbo_can )
		{
			fbo_color_attachment_nb_max = GOL::get_integer( GL_MAX_COLOR_ATTACHMENTS );
			GOOD_PRINT_STRING( "\tColor Attachments     : %d",	fbo_color_attachment_nb_max );
			GOOD_PRINT_STRING( "\tRenderbuffer Size Max : %d",	GOL::get_integer( GL_MAX_RENDERBUFFER_SIZE ) );
		}
		else
			fbo_color_attachment_nb_max = 0;

		fbo_id_cur = -42;
		bind_fbo( 0 );

		if( b_fbo_can )
			GOOD_PRINT_STRING( "Automatic generation of MipMap possible" );
		else
		{
			WARNING_PRINT_STRING( "No automatic generation of MipMap. in this Opengl Context." );
			WARNING_PRINT_STRING( "Need version 3.0 at minimum or GL_ARB_framebuffer_object or GL_EXT_framebuffer_object extension" );
		}
	}

	GLuint	gen_fbo()		
	{
		GLuint	fbo_id;
		glGenFramebuffers( 1, &fbo_id );
		return fbo_id;
	}
	void	delete_fbo( GLuint& fbo_id )
	{
		glDeleteFramebuffers( 1, &fbo_id );
	}
	void	bind_fbo( GLuint CONST fbo_id )
	{
		if( is_state_cache_no() || fbo_id_cur != fbo_id )
		{
			if( b_fbo_verbose )
			{
				if( fbo_id )
					DBG_PRINT_STRING( "GOL bind FBO %d", fbo_id );
				else
					DBG_PRINT_STRING( "GOL unbind FBO" );
			}
			glBindFramebuffer( GL_FRAMEBUFFER, fbo_id );
			fbo_id_cur = fbo_id;
		}
		else
		{
			if( b_fbo_verbose )
				DBG_PRINT_STRING( "GOL try to bind again FBO cur %d", fbo_id );
		}
	}

//RENDER BUFFER
//
	GLuint	render_buffer_cur;
	void	init_render_buffer()
	{
		render_buffer_cur = 0;
	}
	GLuint	gen_render_buffer()
	{
		GLuint	id;
		glGenRenderbuffers( 1, &id );
		return id;
	}
	void	delete_render_buffer( GLuint& render_buffer )
	{
		glDeleteRenderbuffers( 1, &render_buffer );
		if( render_buffer_cur == render_buffer )
			render_buffer_cur = 0;
	}
	bool	b_point_sprite_can = false;
	void	init_point_sprite()
	{
		b_point_sprite_can = test_version_or_extensions( 0,0, "GL_ARB_point_sprite" );
	}

//VBO
	bool	b_vbo_allow			= true;

//VAO
	bool	b_vao_allow			= false;
	bool	b_vao_element_bind	= false;
	bool	b_vao_can			= false;
	bool	b_vao_do			= false;
	GLuint	vao_cur = 0;
	void	init_vao()
	{
		b_vao_can = test_version_or_extensions( 3,0, "GL_ARB_vertex_array_object" );
	}
	void	update_vao()
	{
		b_vao_do = b_vao_can && b_vao_allow;
	}

	void	finish_always()
	{
		SPY_PUSH_RANGE( "glFinish", spy::GOL_LOW );
		TBUF_ADD( tbuf::CH_GL_FINISH, 1., "start" );
			glFinish();
		TBUF_ADD( tbuf::CH_GL_FINISH, 0, "stop" );
		SPY_POP_RANGE();
	}

	void	finish()
	{
		if( b_finish_allow )
			finish_always();
	}

	void	flush_always()
	{
		SPY_PUSH_RANGE( "glFlush", spy::GOL_LOW );
		TBUF_ADD( tbuf::CH_GL_FLUSH, 1., "start" );
			glFlush();
		TBUF_ADD( tbuf::CH_GL_FLUSH, 0, "stop" );
		SPY_POP_RANGE();
	}
	void	flush()
	{
		if( b_flush_allow )
			flush_always();
		if( b_finish_force )
			finish();
	}
		
	INT32	viewport_max[2];
	INT32	subpixel_bits;
	INT32	fbo_color_attachment_nb_max = 0;

	void	print_bitplanes()
	{
		GOOD_PRINT_STRING( "Bitplanes numbers :" );
		GOOD_PRINT_STRING( "REGULAR" );
		GOOD_PRINT_STRING( "\tRED\tGREEN\tBLUE\tALPHA\tDEPTH\tINDEX\tSTENCIL" );
		GOOD_PRINT_STRING( "\t%d\t%d\t%d\t%d\t%d\t%d\t%d", 
							bit_nb_red, bit_nb_green, bit_nb_blue, bit_nb_alpha, 
							bit_nb_depth, bit_nb_index, bit_nb_stencil
						);
		GOOD_PRINT_STRING( "ACCUMULATION BUFFER" );
		GOOD_PRINT_STRING( "\tRED\tGREEN\tBLUE\tALPHA" );
		GOOD_PRINT_STRING( "\t%d\t%d\t%d\t%d", 
							bit_nb_accum_red, bit_nb_accum_green, bit_nb_accum_blue, bit_nb_accum_alpha
						);
	}

	void	print_subpixel()
	{
		GOOD_PRINT_STRING( "Subpixel : %d Bits", GOL::subpixel_bits );
	}

	void	print_buffers()
	{
		GOOD_PRINT_STRING( "Buffers : STEREO %s DOUBLE %s ACCUM %d", 
							get_boolean_str( GL_STEREO ), 
							get_boolean_str( GL_DOUBLEBUFFER ), 
							get_integer( GL_AUX_BUFFERS ) 
						 );
	}

	C_PCHAR skip = "";	//todo refine

	void print_int(		C_PCHAR text, GLenum CONST what )
	{
		GOOD_PRINT_STRING( "%s%s : %d", skip, text,	get_integer( what )	);
	}
	FP32 print_float(	C_PCHAR text, GLenum CONST what )
	{
		FP32 fp32 = get_fp32( what );
		GOOD_PRINT_STRING( "%s%s : %f", skip, text, fp32 );
		return fp32;
	}

	//todo check all useful glGet to printat start and display as param 
	void print_misc()
	{
	//POINT
		GOL::get_real( GL_POINT_SIZE_RANGE, point_size_range );
		GOOD_PRINT_STRING( "Point size range: %f to %f", point_size_range[0], point_size_range[1] );

		point_size_granularity = print_float( "\tGranularity",	GL_POINT_SIZE_GRANULARITY );
		
	//LINE
		GOL::get_real( GL_ALIASED_LINE_WIDTH_RANGE, line_size_aliased_range );
		GOOD_PRINT_STRING( "Line Aliased size range: %f to %f", line_size_aliased_range[0], line_size_aliased_range[1] );

		GOL::get_real( GL_SMOOTH_LINE_WIDTH_RANGE, line_size_smooth_range );
		GOOD_PRINT_STRING( "Line Smooth size range: %f to %f", line_size_smooth_range[0], line_size_smooth_range[1] );

		line_size_smooth_granularity = print_float( "\tGranularity",	GL_SMOOTH_LINE_WIDTH_GRANULARITY );

	
	//MORE
		GOOD_PRINT_STRING( "Viewport max dimension : %dx%d", viewport_max[0], viewport_max[1] );

		print_int( "Draw Buffers",					GL_MAX_DRAW_BUFFERS		);

		print_int( "Max Samples",					GL_MAX_SAMPLES			);

		print_int( "Evaluation Order",				GL_MAX_EVAL_ORDER					);
		print_int( "List Nesting",					GL_MAX_LIST_NESTING					);
		print_int( "Clip Planes",					GL_MAX_CLIP_PLANES					);
		print_int( "Pixel Map Table Planes",		GL_MAX_PIXEL_MAP_TABLE				);
		print_int( "Attrib Stack Depth",			GL_MAX_ATTRIB_STACK_DEPTH			);
		print_int( "Modelview Stack Depth",			GL_MAX_MODELVIEW_STACK_DEPTH		);
		print_int( "Name Stack Depth",				GL_MAX_NAME_STACK_DEPTH				);
		print_int( "Projection Stack Depth",		GL_MAX_PROJECTION_STACK_DEPTH		);
		print_int( "Texture Stack Depth",			GL_MAX_TEXTURE_STACK_DEPTH			);
		print_int( "Client Attrib Stack Depth",		GL_MAX_CLIENT_ATTRIB_STACK_DEPTH	);
		print_int( "Color Matrix Stack Depth",		GL_MAX_COLOR_MATRIX_STACK_DEPTH		);
	//	print_int( "Program Matrices",				GL_MAX_PROGRAM_MATRICES_ARB )		);
	}



	void print_shader()
	{
		GOOD_PRINT_STRING( "Shaders infos :" );
		skip = "\t";

		print_int( "maximum 4-component generic vertex attributes vertex shader",	GL_MAX_VERTEX_ATTRIBS				);
		print_int( "Recommended maximum vertex array Vertices",						GL_MAX_ELEMENTS_VERTICES			);
		print_int( "Recommended maximum vertex array Indices",						GL_MAX_ELEMENTS_INDICES				);

		print_int( "Geometry Shaders Max Vertices Out",								GL_MAX_GEOMETRY_OUTPUT_VERTICES		);

		//GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET
		//	single integer value containing the maximum offset that may be added to a vertex binding offset.

		print_int( "maximum number of vertex buffers that may be bound",			GL_MAX_VERTEX_ATTRIB_BINDINGS		);
	
		//GL_MAX_UNIFORM_BUFFER_BINDINGS
		//	maximum number of uniform buffer binding points on the context, which must be at least 36.

		//must be at least 16384.
		print_int( "mmaximum size in basic machine units of a uniform block",		GL_MAX_UNIFORM_BLOCK_SIZE			);

		//must be at least 1024
		print_int( "maximum number of explicitly assignable uniform locations",		GL_MAX_UNIFORM_LOCATIONS			);

		//must be at least 60
		print_int( "number components for varying variables",						GL_MAX_VARYING_COMPONENTS			);

		//must be at least 15.
		print_int( "number 4-vectors for varying variables",						GL_MAX_VARYING_VECTORS				);
		//which is equal to the value of GL_MAX_VARYING_COMPONENTS ??? by 4 say Maa

		//GL_MAX_VARYING_FLOATS
		//	maximum number of interpolators available for processing varying variables used by vertex and fragment shaders. This value represents the number of individual floating-point values that can be interpolated; varying variables declared as vectors, matrices, and arrays will all consume multiple interpolators. The value must be at least 32.

		//GL_MAX_VERTEX_ATTRIBS
		//	data returns one value, the maximum number of 4-component generic vertex attributes accessible to a vertex shader. The value must be at least 16. See glVertexAttrib.

		//GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
		//	maximum number of shader storage buffer binding points on the context, which must be at least 8.

		//GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS
		//GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS
		//GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS
		//GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS
		//GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS
		//GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS
		//GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
		//	maximum number of active shader storage blocks that may be accessed by a type shader.

		//GL_MAX_COMBINED_UNIFORM_BLOCKS
		//GL_MAX_VERTEX_UNIFORM_BLOCKS
		//GL_MAX_GEOMETRY_UNIFORM_BLOCKS
		//GL_MAX_COMPUTE_UNIFORM_BLOCKS
		//GL_MAX_FRAGMENT_UNIFORM_BLOCKS
		//	maximum number of uniform blocks per type shader. See glUniformBlockBinding.

		//GL_MAX_VERTEX_UNIFORM_VECTORS
		//GL_MAX_FRAGMENT_UNIFORM_VECTORS
		//	maximum number of individual 4-vectors of floating-point, integer, or boolean values that can be held in uniform variable storage for a fragment shader. The value is equal to the value of GL_MAX_type_UNIFORM_COMPONENTS divided by 4. See glUniform.

		//GL_MAX_VERTEX_UNIFORM_COMPONENTS
		//GL_MAX_COMPUTE_UNIFORM_COMPONENTS
		//GL_MAX_GEOMETRY_UNIFORM_COMPONENTS
		//GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
		//	maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a type shader. See glUniform.

		//GL_MAX_COMBINED_ATOMIC_COUNTERS
		//GL_MAX_VERTEX_ATOMIC_COUNTERS
		//GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS
		//GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS
		//GL_MAX_GEOMETRY_ATOMIC_COUNTERS
		//GL_MAX_COMPUTE_ATOMIC_COUNTERS
		//GL_MAX_FRAGMENT_ATOMIC_COUNTERS

		//	maximum number of atomic counters available to compute shaders.

		//GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS
		//	maximum number of atomic counter buffers that may be accessed by a compute shader.

		//GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS
		//GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS
		//GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS
		//GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
		//	number of words for type shader uniform variables in all uniform blocks (including default). The value must be at least 1. See glUniform.

//		GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
//			number of invocations in a single local work group (i.e., the product of the three dimensions) that may be dispatched to a compute shader.
//		GL_MAX_COMPUTE_WORK_GROUP_COUNT
//		GL_MAX_COMPUTE_WORK_GROUP_SIZE

		//GL_MAX_DRAW_BUFFERS
		//must be at least 8. See glDrawBuffers.
		print_int( "maximum number of simultaneous outputs that may be written in a fragment shader",			GL_MAX_DRAW_BUFFERS				);

		//GL_MAX_GEOMETRY_INPUT_COMPONENTS
		//GL_MAX_FRAGMENT_INPUT_COMPONENTS
		//	maximum number of components of the inputs read by the type shader

		//GL_MAX_VERTEX_OUTPUT_COMPONENTS
		//GL_MAX_GEOMETRY_OUTPUT_COMPONENTS
		//	maximum number of components of outputs written by a type shader, which must be at least 128.
		skip = "";
	}

	void	print_implementation()
	{
		print_shader();
		print_buffers();
		print_misc();
		print_extension();
	}

	namespace
	{
		bool	b_need_init = true;
	}

	void	init()
	{
		GOOD_PRINT_STRING( "Using GLEW %s", glewGetString( GLEW_VERSION ) );
		if( b_need_init )
		{
			init_debug();
			CTX::init_info();
			CTX::print_versions();

			get_error( "at begin of GOL::init()" );

			bit_nb_red			= get_integer( GL_RED_BITS			);
			bit_nb_green		= get_integer( GL_GREEN_BITS		);
			bit_nb_blue			= get_integer( GL_BLUE_BITS			);
			bit_nb_alpha		= get_integer( GL_ALPHA_BITS		);
			bit_nb_depth		= get_integer( GL_DEPTH_BITS		);
			bit_nb_index		= get_integer( GL_INDEX_BITS		);
			bit_nb_stencil		= get_integer( GL_STENCIL_BITS		);

			bit_nb_accum_red	= get_integer( GL_ACCUM_RED_BITS	);
			bit_nb_accum_green	= get_integer( GL_ACCUM_GREEN_BITS	);
			bit_nb_accum_blue	= get_integer( GL_ACCUM_BLUE_BITS	);
			bit_nb_accum_alpha	= get_integer( GL_ACCUM_ALPHA_BITS	);

			print_bitplanes();

			init_fbo();		//need to be done before init_texture()
			init_texture();
			print_texture_implementation();

			GLint	param_int[2];
			get_integer( GL_MAX_VIEWPORT_DIMS, param_int );
			cpy_v2( viewport_max, param_int );

			subpixel_bits = get_integer( GL_SUBPIXEL_BITS );
			print_subpixel();

			init_light();

			print_implementation();
			get_error( "after print_implementation()" );

			b_have_alpha = bit_nb_alpha > 0;

			init_render_buffer();
			init_vao();

			init_shaders();
			init_meminfo();
			init_point_sprite();

			b_need_init = false;
		}

		glDisableClientState( GL_VERTEX_ARRAY );
		b_client_state_vertex = false;
		GOL::enable_client_state_vertex();
		glDisableClientState( GL_NORMAL_ARRAY );
		b_client_state_normal = false;
		glDisableClientState( GL_COLOR_ARRAY );
		b_client_state_color = false;
		list_base_last = -1;

		//todo check these
		GOL::disable( GL_AUTO_NORMAL );
		GOL::enable( GL_NORMALIZE );
	}

	void	deinit()
	{
		deinit_debug();
		deinit_texture();
	}

//	pour memoire
#if	AAA_SCREEN_DEFORMATION()
	FINLINE	void	BANANA_transform_kernel_3v( REAL* dst )
	{
		REAL	over = 1. / dst[2];
		REAL	tmp = ( 1. - COS_TURN( dst[0] * over * screen_def_cos_freq ) );
		if( tmp > screen_def_cos_limit )
			tmp = screen_def_cos_limit;
		tmp *= dst[2];
		dst[2] += tmp * screen_def_z_factor;

		//	over = 1./dst[2];
		dst[1] += tmp * screen_def_y_factor * ( dst[1] * over-screen_def_y_origin );

		/*
		REAL	v0n;
		over = 1./vec[2];
		v0n = vec[0]*over;
		tmp = ( 1.- COS_INT( v0n*cos_freq ) ) ;
		vec[2] += tmp * vec[2] * z_factor;

		//	over = 1./vec[2];
		tmp = ( vec[1]*over-y_origin ) * ( 1.-COS_INT( v0n*cos_freq_for_y ) );
		vec[1] += tmp * vec[2] * y_factor;
		*/
	}

	FINLINE	void	BANANA_transform3v( FP32* dst )
	{
		c_seedcam::get_cur()->coor_bdd_to_camera( dst );
		BANANA_transform_kernel_3v( dst );
		if( s_screen_def_type == 3 )
		{
			c_seedcam::get_cur()->coor_camera_to_bdd( dst );
		}
	}

	FINLINE	void	BANANA_transform3v( FP32* dst, FP32 CONST* src )
	{
		c_seedcam::get_cur()->coor_bdd_to_camera( dst, src );
		BANANA_transform_kernel_3v( dst );
		if( s_screen_def_type == 3 )
		{
			c_seedcam::get_cur()->coor_camera_to_bdd( dst );
		}
	}
#endif // #if	AAA_SCREEN_DEFORMATION()

	void	update_shaders()
	{
		// update shaders
		b_shaders_use			= b_shaders_can	&& b_shaders_allow;
		b_shaders_include_use	= b_shaders_use && b_shaders_include_can	&& b_shaders_include_allow;
		b_shaders_geometry_use	= b_shaders_use && b_shaders_geometry_can	&& b_shaders_geometry_allow;
		b_shaders_compute_use	= b_shaders_use && b_shaders_compute_can	&& b_shaders_compute_allow;
	}

	void	update()
	{
		b_named_buffer_use			= b_named_buffer_can			&& b_named_buffer_asked_ui;
		b_direct_state_access_use	= b_direct_state_access_can		&& b_direct_state_access_asked_ui;
		b_anisotropic_filtering_do	= b_anisotropic_filtering_can	&& b_anisotropic_filtering_allow_ui;
		b_multisampling_do			= b_multisampling_can			&& b_multisampling_allow_ui;
		b_copy_image_sub_do			= b_copy_image_sub_can			&& b_copy_image_sub_allow_ui;	
		b_fbo_do					= b_fbo_can						&& b_fbo_allow_ui;		
		b_mipmap_generate_do		= b_fbo_can						&& b_mipmap_generate_allow_ui;

		b_check_error_by_call		= b_check_error					&& b_check_error_by_call_ui;
		b_check_error_by_layer		= b_check_error					&& (b_check_error_by_call || b_check_error_by_layer_ui);
		b_check_error_by_frame		= b_check_error					&& (b_check_error_by_call || b_check_error_by_frame_ui);

		update_vao();

		update_shaders();
	}

	FP32	color_cur[4]	= { -1., -1., -1., -1. };

	void	draw_arrays_instanced(		GLenum CONST mode, INT32 CONST count, INT32 CONST first,	INT32 CONST instance_count	)
	{
		if( c_shading::get_cur() )
			glDrawArraysInstanced( mode, first,	count, instance_count );
	}

}	//namespace GOL

