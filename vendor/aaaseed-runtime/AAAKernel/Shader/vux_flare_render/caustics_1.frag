

#ifndef ENABLE_NORMAL_FALLOFF
#	define ENABLE_NORMAL_FALLOFF 1
#endif

#if USE_AAA_UNIFORM == 1
	//todo move it to a vec4 ?
	#define direction			vec3( aaa_fu_vec4[0] )
	#define caustic_color 		vec3( aaa_fu_float[0], aaa_fu_float[1], aaa_fu_float[2] )
	#define caustic_factor 		aaa_fu_float[3]
	#define caustic_scaling 	vec2( aaa_fu_float[4], aaa_fu_float[5] )
	#define normal_factor		aaa_fu_float[6]
	#define time 		    	aaa_fu_float[7]

	#define S_RENDER			aaa_fu_int[0]
	#define ITERATION_NB		aaa_fu_int[1]

#else
	layout(binding = 0) uniform caustics_settings_block
	{
		vec3    caustic_color;
		float   caustic_intensity;
		vec2    caustic_scaling;
		float 	normal_factor;
		float   time;
		int 	iteration_nb;
	} u_caustics_settings;

	#define caustic_color   	u_caustics_settings.caustic_color
//we pre compute in lua to avoi compute in shader]
	#define caustic_intensity 	u_caustics_settings.caustic_intensity
	#define caustic_scaling 	u_caustics_settings.caustic_scaling
	#define normal_factor		u_caustics_settings.normal_factor
	#define time 				u_caustics_settings.time
	#define ITERATION_NB		u_caustics_settings.iteration_nb
#endif



//pipeline inputs
layout(location = 0) in vec2 in_texcoord;

//outputs
layout(location = 0) out vec4 out_result;

//texture inputs
	layout(binding = 0) uniform sampler2D g_input_texture;
	layout(binding = 1) uniform sampler2D g_depth_texture;
#if ENABLE_NORMAL_FALLOFF == 1
	layout(binding = 2) uniform sampler2D g_normal_texture;
#endif


void main()
{
	if( S_RENDER==1 )
	{
		out_result = vec4( texture(g_input_texture, in_texcoord).rgb, 1.0 );
//		out_result = vec4(.5);
		return;
	}

	vec2 screen_clip = in_texcoord;
	screen_clip = screen_clip * vec2(2.0) - vec2(1.0);
	//screen_clip.y *= -1.0;
	float depth_base = texture(g_depth_texture, in_texcoord).r;
	float depth = depth_base;
	depth = depth * 2.0 - 1.0;

	vec4 screen_coord = vec4(screen_clip, depth, 1.0);

	vec4 world_position = aaa_cam.view_projection_inverse * screen_coord;
	///vec4 world_position= aaa_cam.view_inverse * aaa_cam.projection_inverse * screen_coord;
	world_position.xyz /= world_position.w;

	//vec4 diff = world_position2 - screen_coord;


#if 1
//todo move to fns in include
	vec3 dir = normalize(direction);
//todo set axe use in fn names
	vec3 u = normalize( cross( dir, vec3(0.,0.,1.) ) );
	vec3 v = cross( u, dir );
//todo check if we can construction the matrix in a simplier way and do calculateion in reverse : world_position.xyz * mat_dir
#	if 0
	mat3 mat_dir = mat3( vec3( u.x, v.x, dir.x ), vec3( u.y, v.y, dir.y ), vec3( u.z, v.z, dir.z ) );
	vec2 uv = (mat_dir * world_position.xyz).xy * caustic_scaling;
#	else
	mat3 mat_dir = mat3( vec3(u), vec3(v), vec3(dir) );
	vec2 uv = (world_position.xyz * mat_dir).xy * caustic_scaling;
#	endif
#else
	//todo, project texture better, for now we do planar on xz
	vec2 uv = world_position.xz * caustic_scaling;
#endif

	vec2 p = mod( uv * PI * 2.0, PI * 2.0 ) - 250.0;
	vec2 i = vec2( p );
	float c = 1.0;

	for( int n = 0; n < ITERATION_NB; n++ )
	{
		float t = time * (1.0 - (3.5 / float(n+1)));
		i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
		c += 1.0 / length( vec2(p.x / (sin(i.x+t)*caustic_factor),p.y / (cos(i.y+t)*caustic_factor)) );
	}
	c /= float(ITERATION_NB);
	c = 1.17 - pow( c, 1.4 ) ;
	vec3 result_color = vec3( pow(abs(c), 8. ));
	result_color *= caustic_color;

#if ENABLE_NORMAL_FALLOFF == 1
//todo use the direction here too
	vec3 norm = texture(g_normal_texture, in_texcoord).rgb;
	float ndl = clamp( dot(norm,vec3(0.,1.,0.)), 0., 1. );
	ndl = mix(1.0, ndl, normal_factor);
	result_color *= ndl;
#endif

	vec4 color = texture( g_input_texture, in_texcoord );

	switch( S_RENDER )
	{
	case 2:
		color.rgb = result_color;
		break; 
	case 3:
		if( depth_base < 1.0 )
			color.rgb = result_color;
		break;
	case 4:
		if( depth_base < 1.0 )
			color.rgb += result_color;
		break;
	case 5:
		if( depth_base < 1.0 )
			color.rgb -= result_color;
		break;
	}

	out_result = vec4( color.rgb, 1.0 );
	//out_result = vec4(.8);


}


