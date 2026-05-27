#version 460 compatibility
//todo a problem when we have opengl 3.3
//#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_gpu_shader5 : enable
//#extension GL_EXT_geometry_shader4 : enable
#extension GL_ARB_explicit_attrib_location : enable
//#extension GL_ARB_shading_language_include : enable
//#extension GL_NV_shader_buffer_load : enable
//#extension GL_ARB_compute_variable_group_size : enable
//#extension GL_ARB_tessellation_shader : enable
#define AAA_GOL_VERSION 460

#define AAA_IS_AMD 0
#define AAA_IS_INTEL 0
#define AAA_IS_NVIDIA 1

#if AAA_GOL_VERSION >= 420
#	define CONST
#else
#	define CONST
#endif

//#extension	GL_NV_compute_shader_derivatives : enable	// stopvert and frag to compile

#define smoothstep_safe smoothstep




//end should be 30 lines
