#version 420
//tod use the uniform done in includes
layout(binding = 11) uniform world_block
{
    mat4 world;
} u_world;

layout(binding = 12) uniform camera
{
  mat4  view;
  mat4  projection;
  mat4  view_projection;
  mat4  view_inverse;
  mat4  projection_inverse;
  mat4  view_projection_inverse;
  vec4  coc_factors;
  vec4  camera_position;
  float  plane_near;
  float  plane_far;
} u_camera;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_position_world;
layout(location = 2) out vec2 out_texcoord;

void main(void)
{
    mat4 wvp = u_camera.view_projection * u_world.world;
    gl_Position = wvp * vec4(in_position, 1.0);

    vec4 position_world = u_world.world * vec4(in_position,1.0);
    vec4 normal_world = u_world.world * vec4(in_normal,0.0);

    out_position_world = position_world.xyz;
    out_normal = normalize(normal_world.xyz);
    out_texcoord = in_texcoord;
}