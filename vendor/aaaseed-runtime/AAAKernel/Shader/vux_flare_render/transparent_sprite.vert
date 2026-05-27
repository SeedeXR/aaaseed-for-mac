

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

layout(location = 0) out vec2 out_texcoord;
layout(location = 1) out vec3 out_position_world;

void main(void)
{
	vec3 p = in_position;
	p = mat3(aaa_cam.view_inverse) * p;

	vec4 position_world = aaa_model.world_transform * vec4(0,0,0,1);
	position_world.xyz += p;

	gl_Position = aaa_cam.view_projection * position_world;

    out_position_world 	= position_world.xyz;
    out_texcoord 		= in_texcoord;
}