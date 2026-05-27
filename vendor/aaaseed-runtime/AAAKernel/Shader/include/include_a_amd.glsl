#version 440 compatibility
//todo a problem when we have opengl 3.3
//#extension GL_EXT_gpu_shader4 : enable
#extension GL_ARB_gpu_shader5 : enable
//#extension GL_EXT_geometry_shader4 : enable
#extension GL_ARB_explicit_attrib_location : enable
//#extension GL_ARB_shading_language_include : enable
//#extension GL_NV_shader_buffer_load : enable
//#extension GL_ARB_compute_variable_group_size : enable
//#extension GL_ARB_tessellation_shader : enable
#define AAA_GOL_VERSION 440

#define AAA_IS_AMD 1
#define AAA_IS_INTEL 0
#define AAA_IS_NVIDIA 0

#if AAA_GOL_VERSION >= 420
#	define CONST
#else
#	define CONST
#endif

float smoothstep_safe( in float edge0, in float edge1, in float v_in )
{
	return (edge0 < edge1) ? smoothstep( edge0, edge1, v_in ) : 1.0 - smoothstep( edge1, edge0, v_in );
}



//end should be 30 lines
