
#if USE_AAA_UNIFORM == 1
#else
#endif


#define time    aaa_fu_float[0]


layout(binding = 0) uniform sampler2D g_input_texture_0;
layout(binding = 1) uniform sampler2D g_input_texture_1;
layout(binding = 2) uniform sampler2D g_input_texture_2;
layout(binding = 3) uniform sampler2D g_input_texture_3;


layout(location = 0) in vec4 layer_color;


layout(location = 0) out vec4 out_result;


void main(void)
{
    vec2 uv = gl_PointCoord;
    vec2 uv0 = uv-.5;
    float d = dot( uv0, uv0 );
    if( d > .25 )
        discard;

    float g = 1. - max( uv0.x, uv0.y ) * 2;
    //  out_result = vec4(g,g,g,1.) * layer_color;
	out_result = vec4(uv,0,1.);
//    out_result = vec4(1,1,1, 1. - d * 2);
	// out_result = vec4(in_texcoord, 0, 1);
	// out_result = layer_color;
}
