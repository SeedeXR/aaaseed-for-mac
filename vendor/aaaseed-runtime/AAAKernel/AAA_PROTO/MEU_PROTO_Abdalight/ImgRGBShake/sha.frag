#if USE_AAA_UNIFORM == 1
#else
#endif


layout(binding = 0) uniform sampler2D g_input_texture_0;

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 layer_color;
layout(location = 0) out vec4 fragColor;

CONST float offset = aaa_fu_float[1];
CONST float angle  = radians(aaa_fu_float[2]);

CONST float u = offset * cos(angle);
CONST float v = offset * sin(angle);

void main(void)
{
    vec4 color;

    color.r = texture( g_input_texture_0, uv - vec2( u, v ) ).r;
    color.g = texture( g_input_texture_0, uv ).g;
    color.b = texture( g_input_texture_0, uv + vec2( u, v ) ).b;
    color.a = 1;

    fragColor = color * layer_color;
}
