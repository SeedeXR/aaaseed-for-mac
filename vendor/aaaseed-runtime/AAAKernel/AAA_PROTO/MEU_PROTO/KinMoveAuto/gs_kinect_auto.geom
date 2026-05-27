////#version 330 compatibility
////#extension GL_EXT_geometry_shader4 : enable
////#extension GL_ARB_gpu_shader5 : enable // Enleve 1 warning sur
////#extension GL_ARB_explicit_attrib_location : enable

precision highp float;

// Layout.
//layout( triangles, invocations=1 ) in;
layout( triangles ) in;
layout( triangle_strip, max_vertices=3 ) out;
//layout( line_strip, max_vertices=2 ) out;

// Uniform.
//uniform sampler2D	aaa_tex2d[4];

// Input.
in VERT_BLOCK
{
	vec4 	pos_kin;
	vec4 	pos_world;
	vec2 	uv;
	vec3 	nor;
	float	depth;
} vert_in[3];

// Output.
out GEOM_BLOCK
{
	vec4 pos_kin;
	vec4 pos_world;
	vec4 normal;	//	w is free
	vec4 uv_depth_rgb;
} geom_block;

uniform sampler2D	aaa_samp2;
#define TEX_RGB				aaa_samp2

// CONST int	offset_ind			= b_flip_x ? 3 : 0;
CONST int	indexes				= aaa_gu_int[0];
CONST float edge_depth_max_2	= aaa_gu_float[0];
CONST float edge_ratio_max		= aaa_gu_float[1];
CONST float push_factor			= aaa_gu_float[3];

// 1-v1/2-v2/3-azure deal
//todo deal with lidar too ?
CONST int	s_cam_type			= aaa_vu_int[3];
CONST int	s_render_combined	= aaa_fu_int[1];


#define cxy	aaa_gu_vec4[3].xy
#define fxy	aaa_gu_vec4[3].zw
// CONST float	cx					= aaa_gu_vec4[3].x;
// CONST float	cy					= aaa_gu_vec4[3].y;
// CONST float	fx					= aaa_gu_vec4[3].z;
// CONST float fy					= aaa_gu_vec4[3].w;

#define	rgb_tex_size_over	aaa_fu_vec4[0].zw

CONST bool b_modelview			= aaa_gu_int[1] == 1;

// CONST vec4	rgb_mat_1			= aaa_gu_vec4[0];
// CONST vec4	rgb_mat_2			= aaa_gu_vec4[1];
// CONST vec4	rgb_mat_3			= aaa_gu_vec4[2];
CONST mat4 depth_to_rgb = mat4(	vec4( aaa_gu_vec4[0].xyz, 0),
       							vec4( aaa_gu_vec4[1].xyz, 0),
        						vec4( aaa_gu_vec4[2].xyz, 0),
        						vec4( aaa_gu_vec4[0].w, aaa_gu_vec4[1].w, aaa_gu_vec4[2].w, 1)
    						);

// Begin : Triangle utilities. //
vec3 triangle_center(in vec3 p0, in vec3 p1, in vec3 p2)
{
   return (p0 + p1 + p2) * .33333333333;
}

vec3 	triangle_normal (in vec3 p0, in vec3 p1, in vec3 p2)
{
#if 0
#define FZ 	500.
#define	FZO .002
	p0.z *= FZ;
	p0.z -= fract( p0.z ) ;
	p0.z *= FZO;
	p1.z *= FZ;
	p1.z -= fract( p1.z ) ;
	p1.z *= FZO;
	p2.z *= FZ;
	p2.z -= fract( p2.z ) ;
	p2.z *= FZO;
#endif
	p2 -= p1;
	p1 -= p0;

#define FV 	5.
#define	FVO .2

	p0 = cross( p2, p1 );
	p0 = normalize( p0 );

#if 0
	float angle = atan( p0.x, p0.y );

	angle *= FV;
	angle -= fract( angle ) ;
	angle *= FVO;

	p0.xy = vec2( cos(angle), sin(angle) );
#endif
	return p0;
}
// End : Triangle utilities. //

#if 0
// Begin : camera utilities. //
vec3 camera_direction_x(in mat4 p_mvp)	{	return vec3(p_mvp[0][0], p_mvp[1][0], p_mvp[2][0] );		}
vec3 camera_direction_y(in mat4 p_mvp)	{	return vec3(p_mvp[0][1], p_mvp[1][1], p_mvp[2][1] );		}
vec3 camera_direction(in mat4 p_mvp)	{	return vec3(p_mvp[0][2], p_mvp[1][2], p_mvp[2][2] );		}

vec3 camera_position(in mat4 p_mvp)
{
	mat4 inv = inverse(p_mvp);
	return vec3( inv[3] / inv[3][3]);

//	vec4 pos = vec4( 0,0,0,1 ) * inv;
//	return pos.xyz / pos.w;
//	return -p_mvp[3].xyz * p_mvp[3].w;
//	return vec3( p_mvp[0].w,  p_mvp[1].w, p_mvp[2].w );
}

float camera_distance(in mat4 p_mvp, in vec3 p_xyz)
{
	vec3 cam_pos = camera_position(p_mvp);
	return length(cam_pos - p_xyz);
}

vec2 camera_projected_uv(in mat4 p_mvp, in vec3 p_xyz)
{
	vec4 pos4 = p_mvp * vec4(p_xyz, 1.0);
	return ((pos4.xy / pos4.w)); // + 1.0) * 0.5;
}

float get_fov_vertical(in float p_fov_hor, in float p_aspect)
{
	return (2.0 * atan((p_fov_hor * 0.5) / p_aspect));
}

float get_fov_horizontal(in float p_fov_ver, in float p_aspect)
{
	return (2.0 * atan(tan(p_fov_ver * 0.5) / p_aspect));
}
#endif

#if 0
vec3 kinect_pos;
vec3 kinect_dir;

// End : camera utilities. //
vec4 displace_point( in vec3 pos_in, float depth )
{
	vec3 dir = normalize( pos_in - kinect_pos );
	vec4 pos;
	//vec2 uv = vert_in[i].tex_uv - .5;
	//displace.xyz = kinect_pos.xyz - (vx * uv.x * CONE_FX + vy * uv.y * CONE_FY - vz) * depth ;
	pos.xyz = kinect_pos + dir * depth / dot( kinect_dir, dir );
	pos.w = 1.0;
	//vec4 displace = vec4(0,0,1, 0.0);
	return pos;
}
#endif

float norm2( in vec3 a )			{	return dot( a, a );	}

const float	OUT = 100.;
const float	OUT_TEST = OUT - 1.;
//const int order[6] = {	0,1,2,	1,0,2	};

void main()
{
	//todo opt
	if( vert_in[0].depth >= OUT_TEST || vert_in[1].depth >= OUT_TEST || vert_in[2].depth >= OUT_TEST )
		return;

	//vec4 pos_world[3];
	vec4 a = vert_in[0].pos_kin;
	vec4 b = vert_in[1].pos_kin;
	vec4 c = vert_in[2].pos_kin;

	vec3 ab = a.xyz - b.xyz;
	vec3 ac = a.xyz - c.xyz;
	vec3 bc = b.xyz - c.xyz;
#if 0
	if( edge_depth_max_2 > 0. )
	{
		// Distance between triangles discard.
		if( abs(ab.z) > edge_depth_max_2  || abs(ac.z) > edge_depth_max_2 || abs(bc.z) > edge_depth_max_2 )
			return;
	}
#endif


	float ab2 = norm2( ab );
	float ac2 = norm2( ac );
	float bc2 = norm2( bc );
#if 1
	if( edge_ratio_max > 1. )
	{
		float f2 = edge_ratio_max * edge_ratio_max;
		//	eliminate degenerate triangle testing size ratio
		if( bc2 > f2 * ab2 || ac2 > f2 * bc2 || ab2 > f2 * ac2 )
			return;
	}
#endif
#if 1
	if( edge_depth_max_2 > 0. )
	{
		// Distance between triangles discard.
		if( ab2 > edge_depth_max_2 || ac2 > edge_depth_max_2 || bc2 > edge_depth_max_2 )
			return;
	}
#endif

	//depth -= 1;
		//quick hack for the floor
		//if( pos_world[i].y - pos_world[i].z * .15 < .2 )
		//	return;
/*
	//	eliminate triangle spawing more than a certain distance
	float f = aaa_gu_float[0];
	float mi = min( min( d[0], d[1] ), d[2] );
	float ma = max( max( d[0], d[1] ), d[2] );
	if( (ma-mi) > f )
		return;

	float ab = abs( d[0] - d[1] );
	float bc = abs( d[1] - d[2] );
	float ac = abs( d[2] - d[0] );
#if 1
#	if 1
	if( bc > f ) return;
	if( ac > f ) return;
	if( ab > f ) return;
#	else
	f *= 4;
	if( bc > f*d[0] ) return;
	if( ac > f*d[1] ) return;
	if( ab > f*d[2] ) return;
#	endif
#else
	f *= 100.;
	if( bc > f*ab ) return;
	if( ac > f*bc ) return;
	if( ab > f*ac ) return;
#endif
*/

/*
#if 0
	depth = texture2D(aaa_tex2d[0], (vert_in[0].tex_uv + vert_in[1].tex_uv) * .5, 1 ).r * 8.;
	if ( depth <= 0.3 )
		return;
	pos_world[3] = displace_point( (vert_in[0].pos_world.xyz + vert_in[1].pos_world.xyz) * .5, depth );

	depth = texture2D(aaa_tex2d[0], (vert_in[1].tex_uv + vert_in[2].tex_uv) * .5, 1 ).r * 8.;
	if ( depth <= 0.3 )
		return;
	pos_world[4] = displace_point( (vert_in[1].pos_world.xyz + vert_in[2].pos_world.xyz) * .5, depth );

	depth = texture2D(aaa_tex2d[0], (vert_in[2].tex_uv + vert_in[0].tex_uv) * .5, 1 ).r * 8.;
	if ( depth <= 0.3 )
		return;
	pos_world[5] = displace_point( (vert_in[2].pos_world.xyz + vert_in[0].pos_world.xyz) * .5, depth );
	vec3 nor[3];
	nor[0] = triangle_normal( pos_world[0].xyz, pos_world[3].xyz, pos_world[5].xyz );
	nor[1] = triangle_normal( pos_world[1].xyz, pos_world[4].xyz, pos_world[3].xyz );
	nor[2] = triangle_normal( pos_world[2].xyz, pos_world[5].xyz, pos_world[4].xyz );
#endif
*/

//	Compute the normal

//	vec3 nor;
//	vec3 nor	= triangle_normal( pos_world[0].xyz, pos_world[1].xyz, pos_world[2].xyz );
//	nor			= normalize( gl_NormalMatrix * nor );

	//nor_fix = ( nor[0] + nor[1] + nor[2] ) * .3333333;
	//nor = max( nor, 0. );

	bool b_generate_uv_for_rgb = s_cam_type == 3 && s_render_combined > 3;
	for( int i=0; i<3; ++i )
	{
		int ind = (indexes>>(i*2)) & 0x3;

		vec3 nor					= vert_in[ind].nor;
		vec4 pos					= vert_in[ind].pos_world;
		vec4 pos_kin				= vert_in[ind].pos_kin;
		geom_block.pos_kin			= pos_kin;
		geom_block.pos_world		= pos;
		geom_block.uv_depth_rgb.xy	= vert_in[ind].uv;

		// retro project from kinect space (depth) to rgb camera space
		if( b_generate_uv_for_rgb )
		{
			vec4 pos_rgb = depth_to_rgb * pos_kin;
			pos_rgb.xyz = pos_rgb.xyz / pos_rgb.w;
			vec2 uv = ((fxy * pos_rgb.xy) / pos_rgb.z + cxy) * rgb_tex_size_over;
			uv.y = 1. - uv.y;
			geom_block.uv_depth_rgb.zw = uv;				
			
			//push along normal
			//so fun we should use another texture
			//pos.xyz += nor * push_factor * (.5 + .5 * sin( uv.x * 30.));
			//pos.xyz += nor * push_factor * texture( TEX_RGB,  uv	).g;
			//pos.xyz += nor * push_factor * saturate( sin( dot( uv-.5, uv-.5 ) * 30. ) );
			pos.xyz += nor * push_factor;
		}
		else
		{
			//push along normal
			pos.xyz += nor * push_factor;
		}


		geom_block.pos_world = pos;
		if( b_modelview )
		{
			gl_Position = gl_ModelViewProjectionMatrix * pos;
			//nor = normalize( gl_NormalMatrix * nor );
			nor = gl_NormalMatrix * nor;	// check this 
		}
		else
			gl_Position = gl_ProjectionMatrix * pos;

		geom_block.normal = vec4( nor, 0. );

		EmitVertex();
	}

	EndPrimitive();
}

















