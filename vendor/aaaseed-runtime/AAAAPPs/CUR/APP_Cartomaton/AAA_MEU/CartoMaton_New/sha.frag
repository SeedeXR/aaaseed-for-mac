#if USE_AAA_UNIFORM == 1
#else
#endif

#define u_min    aaa_fu_vec4[0].x
#define u_max    aaa_fu_vec4[0].y
#define v_min    aaa_fu_vec4[0].z
#define v_max    aaa_fu_vec4[0].w


layout(binding = 0) uniform sampler2D g_input_texture_0;

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 layer_color;

layout(location = 0) out vec4 fragColor;

CONST float v_edge     = (v_max - v_min) * aaa_fu_float[0]; //= 0.15; // 15% flou haut/bas
CONST float fade       = 1.0;
CONST float v_max_edge = v_max - v_edge;
CONST float v_min_edge = v_min + v_edge;

void main(void)
{
     
    float u = uv.x;
    float v = uv.y;

    //todo this should be done by drawing a smaller rect not here
    //todo this could done more in //
    if( u < u_min || u > u_max || v < v_min || v > v_max )
        discard;

    vec4 color = texture( g_input_texture_0, uv );

    //todo do both always to avoid test
    if( abs(v_max - 1.0) < 0.001 )
    {
        // Fade only bot
        fade = smoothstep_safe( v_min, v_min_edge, v );
    }
    else if (abs(v_min - 0.0) < 0.001)
    {
        // Fade only top
        fade = smoothstep_safe( v_max, v_max_edge, v );
    }
    else
    {
        // Fade bot and top
        float fade_top    = smoothstep_safe(v_max, v_max_edge, v);
        float fade_bottom = smoothstep_safe(v_min, v_min_edge, v);
        fade = min(fade_top, fade_bottom);
    }

    color.a = fade;
    fragColor = color * layer_color;
}
