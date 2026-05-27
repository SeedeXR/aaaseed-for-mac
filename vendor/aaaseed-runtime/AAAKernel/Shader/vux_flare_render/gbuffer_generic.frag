

//shader pipeline inputs
layout(location = 0) in vec3 in_position_world;
layout(location = 1) in vec3 in_normal_world;
layout(location = 2) in vec2 in_texcoord;

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
layout(binding = 4) uniform sampler2D emissive_texture;

#if USE_AAA_UNIFORM == 1
#	define flat_normal      aaa_fu_int[ 0 ]
#	define use_textures     aaa_fu_int[ 1 ]
#	define uv_xy_fxy		aaa_fu_vec4[0]
#	define alpha_threshold	aaa_fu_float[0]
#else
#	define uv_xy_fxy		vec4(0.,0.,1.,1.)
#	define alpha_threshold	0.
#endif

void output_surface( st_surface surface )
{
	out_diffuse_rough 		= vec4( surface.diffuse.rgb,	surface.roughness				);
//	out_diffuse_rough 		= texture( diffuse_texture, in_texcoord);
 	out_normal_occlusion 	= vec4( surface.normal,			surface.ao_factor				);
//	out_normal_occlusion 	= vec4( in_normal_world, 1.0				);
    out_specular_reflection = vec4( surface.specular, 		surface.reflection_intensity	);
	out_emissive 			= vec4( surface.emissive,		1.0								);
}

void main()
{
	st_surface surface;
	surface.world_position = in_position_world;

	//todo tex offset
	vec2 uv = ( in_texcoord.xy + uv_xy_fxy.xy ) * uv_xy_fxy.wz;
	if( use_textures == 1 )
		surface.diffuse	 = texture(	diffuse_texture, uv );
	else
		surface.diffuse = vec4(1);
	//todo
	if( surface.diffuse.a < alpha_threshold )
	{
		discard;
	}

	if( flat_normal == 1 )
	{
		vec3 dx = dFdx(in_position_world);
		vec3 dy = dFdy(in_position_world);
		surface.normal = normalize(cross(dx,dy));
	}
	else
	{
		surface.normal = normalize(in_normal_world);
	}

	surface.roughness	= 1.0;
	surface.specular	= vec3(1);
	surface.emissive	= vec3(0.0);

	if( use_textures == 1 )
	{
//		surface.roughness		= texture( roughness_texture, uv).r;
//		surface.specular		= texture( specular_metal_texture, uv ).rgb;
//		surface.emissive		= texture( emissive_texture, uv).rgb;
	}


	do_surface_material( surface, aaa_material.material );
//	surface.emissive = surface.world_position;

	output_surface(surface);
	//out_diffuse_rough = vec4( uv,1, 1);
	//out_diffuse_rough = texture( diffuse_texture, uv );
}
