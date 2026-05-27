//#define settings_rcpFrame			vec2(aaa_fu_float[0], aaa_fu_float[1])

#define settings_rcpFrame			vec2(1.0/512.0,1.0/128.0)
#define bypass						aaa_vu_int[0]


CONST float	pullback_distance		= aaa_fu_float[9];
CONST float	df_min					= aaa_fu_float[10];
CONST float	df_max					= aaa_fu_float[11];
CONST float	field_str				= aaa_fu_float[12];
CONST float	field_gamma				= aaa_fu_float[13];


layout(binding = 0) uniform sampler2D g_input_texture_dist;
layout(binding = 1) uniform sampler2D g_input_texture_mask;

layout(location = 0) in vec2 in_texcoord;

layout(location = 0) out vec4 out_result;

float calcDist( vec2 p )
{
#if 1 //signed
	return texture(g_input_texture_dist,p).r * (texture(g_input_texture_mask,p).r * -2. + 1.);
#else //unsigned
	return texture(g_input_texture_dist,p).r;
#endif
}

vec2 comp_gradient(vec2 p)
{
	vec2 n;
    vec3 step = vec3( settings_rcpFrame.xy, 0. );
	n.x = calcDist( p + step.xz ) - calcDist( p - step.xz );
	n.y = calcDist( p + step.zy ) - calcDist( p - step.zy );

	return n;
}

void main()
{
    float df = calcDist(in_texcoord);

    vec2 grad = comp_gradient(in_texcoord);

    float field_dist_factor = field_str * pow( (df-df_max)/(df_min-df_max), field_gamma );

    vec2 force = grad * field_dist_factor;

    out_result = vec4(force,0.0, 1.0);
}