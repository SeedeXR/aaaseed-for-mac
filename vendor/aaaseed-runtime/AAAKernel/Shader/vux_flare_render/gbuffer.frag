

#define FLAT_NORMAL 0
#define ENABLE_NORMAL_MAP 1

#define ENABLE_EMISSIVE_TEXTURE 1

#ifndef ENABLE_NORMAL_FALLOFF
#	define ENABLE_NORMAL_FALLOFF 1
#endif

#if USE_AAA_UNIFORM == 1
#	define uv_xy_fxy		aaa_fu_vec4[0]
#	define mesh_color		aaa_fu_vec4[1]
#	define discard_uv		aaa_fu_int[0]
#	define colored_mesh		aaa_fu_int[1]
#	define alpha_threshold	aaa_fu_float[0]
#else
#	define uv_xy_fxy		vec4(0.,0.,1.,1.)
#	define alpha_threshold	0.
#	define discard_uv		0
#	define colored_mesh		vec4(0.,0.,1.,1.)
#	define mesh_color		0
#endif

//pipeline inputs
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
	layout(binding = 0) uniform sampler2D diffuse_texture;
	layout(binding = 1) uniform sampler2D normalmap_texture;
	layout(binding = 2) uniform sampler2D roughness_texture;
	layout(binding = 3) uniform sampler2D specular_metal_texture;
#if ENABLE_EMISSIVE_TEXTURE == 1
	layout(binding = 4) uniform sampler2D emissive_texture;
#endif


void output_surface( st_surface surface )
{
	out_diffuse_rough 		= vec4( surface.diffuse.rgb,	surface.roughness				);
	out_normal_occlusion 	= vec4( surface.normal,			surface.ao_factor				);
	out_specular_reflection = vec4( surface.specular, 		surface.reflection_intensity	);
	out_emissive 			= vec4( surface.emissive,		1.0								);
}

void main()
{
	st_surface surface;

	vec2 uv = ( in_texcoord.xy + uv_xy_fxy.xy ) * uv_xy_fxy.wz;
	if( colored_mesh == 1 )
		surface.diffuse = mesh_color;
	else
		surface.diffuse	 = texture(	diffuse_texture, uv );
	if( discard_uv == 1 )
	{
		if( uv.x < 0.0 || uv.x > 1.0 )
			discard;
		if( uv.y < 0.0 || uv.y > 1.0 )
			discard;
	}
	//todo
	if( surface.diffuse.a < alpha_threshold )
		discard;

	surface.world_position = in_position_world;

#if FLAT_NORMAL == 1
	vec3 dx = dFdx(in_position_world);
	vec3 dy = dFdy(in_position_world);
	surface.normal = normalize( cross(dx,dy) );
#else
#	if ENABLE_NORMAL_MAP == 1
	vec3 normal = texture(	normalmap_texture, uv ).xyz;
	normal = vec3(2.0) * normal - vec3(1.0);
	normal = normalize(in_tangent_frame * normal);
	surface.normal = normal;
#	else
	surface.normal = normalize(in_normal_world);
#	endif
#endif

	surface.roughness 	= texture(	roughness_texture,		uv).r;
	surface.specular	= texture(	specular_metal_texture, uv).rgb;
#if ENABLE_EMISSIVE_TEXTURE == 1
    surface.emissive	= texture(	emissive_texture,		uv).rgb;
#endif

	do_surface_material( surface, aaa_material.material );

    output_surface(surface);
}