
#if USE_AAA_UNIFORM == 1
#else
#endif


layout(binding = 0) uniform sampler2D g_input_texture_0;
// layout(binding = 1) uniform sampler2D g_input_texture_1;

layout(location = 0) in vec2 in_texcoord;	
layout(location = 1) in vec4 layer_color;

layout(location = 0) out vec4 out_result;

const float A = aaa_fu_float[0];

void main()
{
    vec2 uv = in_texcoord;
    // test to show it go thru
    if( uv.x < A )
        discard;
    vec4 color = texture( g_input_texture_0, uv );
   
	out_result = color * layer_color;
}