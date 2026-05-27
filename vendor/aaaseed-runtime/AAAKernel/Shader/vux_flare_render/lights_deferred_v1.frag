
//same than lights_deferred
//	with gamma on point light
//	with no spot light
//	color_shadow dealt with

#define ENABLE_EMISSIVE 1
//for compat purposes until all data is in place
#ifndef ENABLE_SHADOW
#define ENABLE_SHADOW 1
#endif

#ifndef ENABLE_LIGHTMAP
#define ENABLE_LIGHTMAP 1
#endif


#if USE_AAA_UNIFORM == 1
#	define what					aaa_fu_int[0]
#	define pcf_half_size		aaa_fu_int[1]
#	define hack_ambient_offset	aaa_fu_float[0]
#	define hack_light_factor	aaa_fu_float[1]
#	define hack_shadow_factor	aaa_fu_float[2]
#	define hack_y_factor_top	aaa_fu_float[3]
#	define hack_y_factor_bot	aaa_fu_float[4]
#	define normal_center		aaa_fu_float[5]
#	define color_shadow 		aaa_fu_vec4[0]
#	define color_ambient 		aaa_fu_vec4[1]
#	define color_general 		aaa_fu_vec4[2]
#	define gamma_general 		aaa_fu_vec4[3]
#	define color_offset			aaa_fu_vec4[4]
#endif

#if AAA_GOL_VERSION >= 420
//todo should the bindings move to include ?
#	if 0
	layout(binding = 10) uniform light_count_block
	{
		uint directional;
		uint point;
		uint spot;
	} u_light_nb;
#	else
	layout(binding = 10) uniform light_blk { st_light_nb u_light_nb; };
#	endif

layout(std430, binding = 13) buffer directional_buffer	{	st_light_directional lights_directional[];	};
layout(std430, binding = 14) buffer point_buffer		{	st_light_point		 lights_point[];		};
layout(std430, binding = 15) buffer spot_buffer		    {	st_light_spot		 lights_spot[];			};

#endif

//pipeline inputs
layout(location = 0) in vec2 in_tex_uv;


#if 1
uniform sampler2D	aaa_tex2d[9];

#	define TEX_DIFFUSE_ROUGH			aaa_tex2d[0]
#	define TEX_NORMAL_AO				aaa_tex2d[1]
#	define TEX_SPECULAR_REFLECTION		aaa_tex2d[2]
#	define TEX_EMISSIVE					aaa_tex2d[3]

#	define TEX_DEPTH					aaa_tex2d[4]

#	define TEX_SHADOW_0					aaa_tex2d[5]
#	define TEX_LIGHTMAP_0				aaa_tex2d[6]
#	define TEX_SHADOW_1					aaa_tex2d[7]
#	define TEX_LIGHTMAP_1				aaa_tex2d[8]


const int shad_index[2]		= { 5, 7 };
const int lightmap_index[2]	= { 6, 8 };

#	define TEX_SHADOW(index) 			aaa_tex2d[     shad_index[index] ]
#	define TEX_LIGHTMAP(index) 			aaa_tex2d[ lightmap_index[index] ]

#else

//texture inputs
layout(binding = 0) uniform sampler2D TEX_DIFFUSE_ROUGH;
layout(binding = 1) uniform sampler2D TEX_NORMAL_AO;
layout(binding = 2) uniform sampler2D TEX_SPECULAR_REFLECTION;
#	if ENABLE_EMISSIVE == 1
layout(binding = 3) uniform sampler2D TEX_EMISSIVE;
#	endif

layout(binding = 4) uniform sampler2D TEX_DEPTH;

//todo we should pass it using fu_int
#	if ENABLE_SHADOW == 1
layout(binding = 5) uniform sampler2D TEX_SHADOW_0;
layout(binding = 7) uniform sampler2D TEX_SHADOW_1;
#	endif
#	if ENABLE_LIGHTMAP == 1
layout(binding = 6) uniform sampler2D TEX_LIGHTMAP_0;
layout(binding = 8) uniform sampler2D TEX_LIGHTMAP_1;
#	endif

#endif

#if ENABLE_LIGHTMAP == 1
#	define LIGHTMAP_MULTIPLY			0
#	define LIGHTMAP_ADD					1
#	define LIGHTMAP_PURE_ADD			2
#	define LIGHTMAP_PURE_ADD_DIFFUSE	3


//todo pass vec4 use alpha
vec3 get_light_map_color( in sampler2D light_map_tex, in st_light_directional l, in vec3 projected_tex_coord, out vec3 light_map_color )
{
	projected_tex_coord.xy *= l.intensity_map_scale;
	projected_tex_coord.xy += l.intensity_map_offset;

	light_map_color = texture( light_map_tex, projected_tex_coord.xy ).rgb * l.intensity_map_factor;

	if( l.intensity_map_mode == LIGHTMAP_MULTIPLY )
		return l.color * light_map_color;
	if( l.intensity_map_mode == LIGHTMAP_ADD )
		return l.color + light_map_color;
	return vec3(1.0);
}

vec3 finalize_light_map( in st_light_directional l, in vec3 surface_normal, in vec3 color, in vec3 light_map_color )
{
	if( l.intensity_map_mode == LIGHTMAP_PURE_ADD )
		return color + light_map_color;
	if( l.intensity_map_mode == LIGHTMAP_PURE_ADD_DIFFUSE )
	{
		float ndl = saturate( -dot(l.direction, surface_normal) );
		return color + light_map_color * ndl;
	}
	return color;
}
#endif

float normalize_and_compute_falloff( inout vec3 light_direction, in float radius, in float gamma )
{
	float light_radius_sq = radius * radius;
	float light_dist_inv = inversesqrt( dot( light_direction, light_direction ) );
	light_direction *= light_dist_inv;

	float dist_falloff = light_radius_sq * (light_dist_inv * light_dist_inv);
	//dist_falloff = max( 0, dist_falloff - inversesqrt(dist_falloff) );
	dist_falloff = max( 0, pow(dist_falloff, gamma) );

	return dist_falloff;
}

// shaded texture output
layout(location = 0) out vec4 out_result;

void do_out( in vec4 color )
{
	color += color_offset; 
	color *= color_general;
	color = clamp( color, 0.0, 1.0 );
	color = pow( color, gamma_general );

	// Debug: test burned out pixel (>1.0).
	//if (out_result.r > 1.0 || out_result.g > 1.0 || out_result.b > 1.0)
	//	out_result = vec4(1.0, 0.0, 0.0, 1.0);

	out_result = color;
	//out_result.a = color.a;

	//out_result = vec4(world_position.xyz, 1.0);
	//out_result = vec4( texture(aaa_samp1, in_tex_uv ).rgb , 1.0);
	//out_result = vec4( in_tex_uv.xy, 0.0, 1.0);
	//out_result = vec4(gl_FragCoord
	//out_result = vec4( vec3(shadow_total), 1.0 );
	//out_result = vec4( ambient_contrib, 1.0 );
}

void main()
{
#if USE_AAA_UNIFORM == 1
	if( what == 1 )
	{
		do_out( texture( TEX_DIFFUSE_ROUGH, in_tex_uv ) );
		return;	
	}
#endif

	vec2 screenClipPos 	= in_tex_uv * 2.0 - 1.0;
	float depth 		= texture( TEX_DEPTH, in_tex_uv ).r;
	if( depth >= 1.0 )
	{
#if 1
	discard;
#else
	do_out( vec4( 0,1,0, 1.0 ) );
	return;
#endif

	}
	depth = depth * 2.0 - 1.0;
	vec4 screen_coord 	= vec4( screenClipPos, depth, 1.0 );
	vec4 world_position = aaa_cam.view_projection_inverse * screen_coord;
	world_position = vec4( world_position.xyz / world_position.w, 1.0 );
	//if (depth >= 0.999)
	//{
	//    world_position.xyz = vec3(0);
	//}

	//fetch textures
	vec4 diffuse_rough 			= texture( TEX_DIFFUSE_ROUGH,		in_tex_uv );
	vec4 normal_ao 				= texture( TEX_NORMAL_AO,			in_tex_uv );
	vec4 specular_reflection 	= texture( TEX_SPECULAR_REFLECTION,	in_tex_uv );
#if ENABLE_EMISSIVE == 1
	vec4 emissive				= texture( TEX_EMISSIVE,			in_tex_uv );
#else
	vec4 emissive				= vec4(0);
#endif

#if USE_AAA_UNIFORM == 1
	if( what != 2 )
	{
		switch( what )
		{
		case 3:		do_out( vec4( vec3(normal_ao.rgb) * (1.-normal_center) + normal_center, 1.0 ) );	return;
		case 4:		do_out( screen_coord * .5 + normal_center );		return;
		case 5:		do_out( world_position );							return;
					//do_out( world_position.xyz * .01, 1. ) );			return;

		case 6:		do_out( vec4( diffuse_rough.rgb, 1.0 ) );			return;
		case 7: 	//do_out( vec4( abs(normal_ao.rgb), 1f ) );			return;
					do_out( vec4( normal_ao.rgb, 1.0 ) );				return;
		case 8:		do_out( vec4( specular_reflection.rgb, 1.0 ) );		return;
		case 9:		do_out( vec4( emissive.rgb, 1.0 ) );				return;
		case 10:	do_out( vec4( vec3(depth*.5+.5), 1.0 ) );			return;

		case 11:	do_out( vec4( diffuse_rough.aaa, 1.0 ) );			return;
		case 12:	do_out( vec4( normal_ao.aaa, 1.0 ) );				return;
		case 13:	do_out( vec4( specular_reflection.aaa, 1.0 ) );		return;
		case 14:	do_out( vec4( emissive.aaa, 1.0 ) );				return;
		}
	}
#endif


	//todo why not a global to avoid passing the surface
	st_surface surface;
	surface.world_position 			= world_position.xyz;

	surface.diffuse.rgb 			= diffuse_rough.rgb;
	surface.normal					= normal_ao.rgb;
	surface.specular 				= specular_reflection.rgb;
	surface.emissive 				= emissive.rgb;

	surface.roughness 				= diffuse_rough.a;
	surface.ao_factor 				= normal_ao.a;
	surface.reflection_intensity 	= specular_reflection.a;
	surface.ambient_intensity 		= emissive.a;

	//todo why aaa_cam.camera_position is wrong
#if 1
	vec4 cam_pos = vec4( aaa_cam.camera_position, 1.0 );
#else
	//vec4 cam_pos = vec4( 0,0,0, 1 );
	//todo find the good one
	//cam_pos = aaa_cam.view_inverse * cam_pos;
	cam_pos.xyz = aaa_cam.view_inverse[3].xyz;
#endif
	vec3 view_direction = normalize( cam_pos.xyz - surface.world_position );

	vec3 light_total  = vec3( 0.0 );
	float shadow_total = 1.0;
	int tex_index = 0;
//DIRECTIONAL
	for( int i = 0; i < u_light_nb.directional; ++i )
	{
		st_light_directional l = lights_directional[i];
		//for quick mode test
		//l.b_intensity_map = 1;
		//l.intensity_map_mode = 3;

		vec3 light_color;
#if ENABLE_LIGHTMAP == 1 || ENABLE_SHADOW == 1
		vec4 projected_light_coords = l.light_view_projection * world_position;
		// divide by w only necessairy when perspective used (pure directional can skip ut)
		projected_light_coords.xyz = projected_light_coords.xyz / projected_light_coords.w * 0.5 + 0.5;
#endif
#if ENABLE_LIGHTMAP == 1
		//apply light map intensity on lights, we preserve initial color as we might want to apply in pure mode after
		vec3 light_map_color;
		if( l.b_intensity_map != 0 )
			light_color = get_light_map_color( TEX_LIGHTMAP(tex_index), l, projected_light_coords.xyz, light_map_color );
		else
#endif
			light_color = l.color;

		vec3 light_contribution = compute_light( surface, -l.direction, view_direction, light_color * 4. );

		//apply "pure lights"
#if ENABLE_LIGHTMAP == 1
		if( l.b_intensity_map != 0 )
			light_contribution = finalize_light_map( l, surface.normal, light_contribution, light_map_color );
#endif

#if ENABLE_SHADOW == 1
		if( l.b_shadow != 0 )
		{
		//if( saturate(projected_light_coords.xy) == projected_light_coords.xy )
		//{
			float shadow_contrib;
			float bias = l.shadow_bias;
#if 0
		// Normal of the computed fragment, in camera space
		vec3 n = normalize( surface.normal );
		// Direction of the light (from the fragment to the light)
		vec3 vl = normalize( l.direction );
		// Cosine of the angle between the normal and the light direction,
		// clamped above 0
		//  - light is at the vertical of the triangle -> 1
		//  - light is perpendiular to the triangle -> 0
		//  - light is behind the triangle -> 0
		float cosTheta = clamp( dot( n,vl ), 0.0,1.0 );

		// ...variable bias
			bias *= .005;
			bias *= tan(acos(cosTheta));
			bias = clamp( bias, 0.,0.01 );
#endif
			//no filter of pcf
			if( pcf_half_size > 0 )
				shadow_contrib = shadow_pcf( TEX_SHADOW(tex_index), projected_light_coords.xyz, bias, l.shadow_filter_width * l.shadow_map_size_over, pcf_half_size );
			else
				shadow_contrib = shadow_no_filter( TEX_SHADOW(tex_index), projected_light_coords.xyz, bias );
			//out_result = vec4(shadow_contrib);
			//return;
			shadow_total *= shadow_contrib;

			light_contribution *= shadow_contrib;
			if( tex_index==0 )
				tex_index = 1;
		//}
		}
#endif

		float f;
		if( world_position.y > 0. )	f = hack_y_factor_top;
		else						f = hack_y_factor_bot;
		light_contribution *= 1. + f * world_position.y;

		//TODO check if light has shadows
		light_total += light_contribution;
	}

//POINT
	for( int i = 0; i < u_light_nb.point; ++i )
	{
		st_light_point l = lights_point[i];

		vec3 light_direction = l.position - surface.world_position;
		float dist_falloff = normalize_and_compute_falloff( light_direction, l.radius, l.gamma );

		light_total += compute_light( surface, light_direction, view_direction, l.color ) * dist_falloff;
	}

	light_total *= hack_light_factor;

	vec3 ambient_contrib = mix( color_shadow.rgb, color_ambient.rgb, shadow_total );

	if( depth < 1.0 )
		light_total += (surface.ambient_intensity * ambient_contrib) * surface.diffuse.rgb;

	do_out( vec4( light_total, 1.0 ) );
}

