

#define FLAT_NORMAL 1
#define ENABLE_NORMAL_MAP 0

#define ENABLE_EMISSIVE_TEXTURE 0

//shader pipeline inputs
layout(location = 0) in vec3 in_position_world;
layout(location = 1) in vec3 in_normal_world;
layout(location = 2) in vec2 in_texcoord;
#if ENABLE_NORMAL_MAP == 1
layout(location = 3) in mat3 in_tangent_frame;
#endif

//gbuffer outputs
layout(location = 0, index = 0) out vec4 out_diffuse_rough;
layout(location = 1, index = 0) out vec4 out_normal_occlusion;
layout(location = 2, index = 0) out vec4 out_specular_reflection;
layout(location = 3, index = 0) out vec4 out_emissive;

//texture inputs
	layout(binding = 0) uniform sampler2D g_diffuse_texture;
	layout(binding = 1) uniform sampler2D g_normalmap_texture;
	layout(binding = 3) uniform sampler2D g_specular_metal_texture;
#if ENABLE_EMISSIVE_TEXTURE == 1
	layout(binding = 4) uniform sampler2D g_emissive_texture;
#endif

void output_surface( st_surface surface )
{
#if SHADING_WORKFLOW == METALLIC_WORKFLOW
    float metallic  = surface.specular.x;
    vec3  albedo	= surface.diffuse - surface.diffuse * metallic;
    vec3  specular	= mix( vec3(0.03), surface.diffuse, metallic );	 // 0.03 default specular value for dielectric.
#elif  SHADING_WORKFLOW == SPECULAR_WORKFLOW
    vec3  albedo	= surface.diffuse;
    vec3  specular	= surface.specular;
#endif

	out_diffuse_rough 		= vec4( albedo,				surface.roughness				);
 	out_normal_occlusion 	= vec4( surface.normal,		surface.ao_factor				);
    out_specular_reflection = vec4( specular, 			surface.reflection_intensity	);
	out_emissive 			= vec4( surface.emissive,	1.0								);
}

void main()
{
    st_surface surface;
	surface.world_position = in_position_world;

#if FLAT_NORMAL == 1
	vec3 dx = dFdx(in_position_world);
	vec3 dy = dFdy(in_position_world);
	surface.normal = normalize(cross(dx,dy));
#else
#	if ENABLE_NORMAL_MAP == 1
	vec3 normal = texture(g_normalmap_texture, in_texcoord).xyz;
	normal = vec3(2.0) * normal - vec3(1.0);
	normal = normalize(in_tangent_frame * normal);
	surface.normal = normal;
#	else
	surface.normal = normalize(in_normal_world);
#	endif
#endif

    surface.diffuse 	= texture(g_diffuse_texture, in_texcoord).xyz * material.diffuse;
    surface.roughness	= texture(g_diffuse_texture, in_texcoord).x * material.roughness;
	surface.specular	= make_surface_specular( material.specular, texture(g_specular_metal_texture, in_texcoord) );

#if ENABLE_EMISSIVE_TEXTURE == 1
    surface.emissive	= texture(g_emissive_texture, in_texcoord).xyz * material.emissive;
#endif

	surface.ao_factor				= material.ao_factor;
	surface.reflection_intensity	= material.reflection_intensity;
	surface.ambient_intensity		= material.ambient_intensity;

    output_surface(surface);
}
