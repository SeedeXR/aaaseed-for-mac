AAA_DEFINE_FN_BASE_DEPRECIATED()

layout(location = 0) in vec3 in_position;
//layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec2 on_texcoord;
layout(location = 1) out flat vec4 layer_color;

void main(void)
{
    gl_Position = AAA_transform_model_to_projection( vec4(in_position, 1.0));
    on_texcoord = gl_MultiTexCoord0.xy;
    layer_color = gl_Color;
}