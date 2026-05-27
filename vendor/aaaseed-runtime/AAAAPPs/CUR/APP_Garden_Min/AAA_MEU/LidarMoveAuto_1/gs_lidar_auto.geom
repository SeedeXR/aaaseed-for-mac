////#version 330 compatibility
////#extension GL_EXT_geometry_shader4 : enable
////#extension GL_ARB_gpu_shader5 : enable // Enleve 1 warning sur
////#extension GL_ARB_explicit_attrib_location : enable

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
	vec4 normal_depth;
	vec2 uv;
} geom_block;

CONST bool	b_flip_x			= ( aaa_gu_int[0]==1 );
CONST float edge_depth_max_2	= aaa_gu_float[0];
CONST float edge_ratio_max		= aaa_gu_float[1];
CONST float push_factor			= aaa_gu_float[3];


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

float norm2( in vec3 a )	{	return dot( a, a );	}

const float	OUT = 100.;
const float	OUT_TEST = OUT - 1.;

void main()
{
	//todo opt
	if( vert_in[0].depth >= OUT_TEST || vert_in[1].depth >= OUT_TEST || vert_in[2].depth >= OUT_TEST )
		return;

	{
		vec4 pos[3];
		pos[0]	= vert_in[0].pos_world;
		pos[1]	= vert_in[1].pos_world;
		pos[2]	= vert_in[2].pos_world;

		//depth -= 1;
		//quick hack for the floor
		//if( pos[i].y - pos[i].z * .15 < .2 )
		//	return;
#if 0
		//	eliminate triangle spawing more than a certain distance
		float f = aaa_gu_float[0];
		float mi = min( min( d[0], d[1] ), d[2] );
		float ma = max( max( d[0], d[1] ), d[2] );
		if( (ma-mi) > f )
			return;

		float ab = abs( d[0] - d[1] );
		float bc = abs( d[1] - d[2] );
		float ac = abs( d[2] - d[0] );
#		if 1
#			if 1
				if( bc > f ) return;
				if( ac > f ) return;
				if( ab > f ) return;
#			else
				f *= 4;
				if( bc > f*d[0] ) return;
				if( ac > f*d[1] ) return;
				if( ab > f*d[2] ) return;
#			endif
#		else
			f *= 100.;
			if( bc > f*ab ) return;
			if( ac > f*bc ) return;
			if( ab > f*ac ) return;
#		endif
#endif
#if 1
	if( edge_ratio_max > 1. )
	{
		//	eliminate degenerate triangle testing size ratio
		float ab = distance( pos[0].xyz, pos[1].xyz );
		float bc = distance( pos[1].xyz, pos[2].xyz );
		if( bc > edge_ratio_max * ab ) return;
		float ac = distance( pos[2].xyz, pos[0].xyz );
		if( ac > edge_ratio_max * bc ) return;
		if( ab > edge_ratio_max * ac ) return;
	}
#endif
#if 1
	if( edge_depth_max_2 > 0. )
	{
		// Distance between triangles discard.
		if ( norm2(	pos[0].xyz - pos[1].xyz ) > edge_depth_max_2 ) return;
		if ( norm2(	pos[0].xyz - pos[2].xyz ) > edge_depth_max_2 ) return;
		if ( norm2(	pos[1].xyz - pos[2].xyz ) > edge_depth_max_2 ) return;
	}
#endif
#if 0
		depth = texture2D(aaa_tex2d[0], (vert_in[0].tex_uv + vert_in[1].tex_uv) * .5, 1 ).r * 8.;
		if ( depth <= 0.3 )
			return;
		pos[3] = displace_point( (vert_in[0].pos_world.xyz + vert_in[1].pos_world.xyz) * .5, depth );

		depth = texture2D(aaa_tex2d[0], (vert_in[1].tex_uv + vert_in[2].tex_uv) * .5, 1 ).r * 8.;
		if ( depth <= 0.3 )
			return;
		pos[4] = displace_point( (vert_in[1].pos_world.xyz + vert_in[2].pos_world.xyz) * .5, depth );

		depth = texture2D(aaa_tex2d[0], (vert_in[2].tex_uv + vert_in[0].tex_uv) * .5, 1 ).r * 8.;
		if ( depth <= 0.3 )
			return;
		pos[5] = displace_point( (vert_in[2].pos_world.xyz + vert_in[0].pos_world.xyz) * .5, depth );
		vec3 nor[3];
		nor[0] = triangle_normal( pos[0].xyz, pos[3].xyz, pos[5].xyz );
		nor[1] = triangle_normal( pos[1].xyz, pos[4].xyz, pos[3].xyz );
		nor[2] = triangle_normal( pos[2].xyz, pos[5].xyz, pos[4].xyz );
#endif
	}

//	Compute the normal

//	vec3 nor;
//	vec3 nor	= triangle_normal( pos[0].xyz, pos[1].xyz, pos[2].xyz );
//	nor			= normalize( gl_NormalMatrix * nor );

	//nor_fix = ( nor[0] + nor[1] + nor[2] ) * .3333333;
	//nor = max( nor, 0. );

	for( int i=0; i<3; ++i )
	{
		vec3 nor;
		vec4 kin;
		vec4 pos;
		vec2 uv;

		if( b_flip_x )
		{
			kin		= vert_in[i].pos_kin;
			kin.x	= - kin.x;
			pos 	= vert_in[i].pos_world;
			pos.x	= - pos.x ;
			nor 	= vert_in[i].nor;
			nor.x 	= - nor.x ;
			uv		= vert_in[i].uv;
		}
		else
		{	//triangle turning in other order
			#if 1 //this is buggy on intel
				int ind = ( i<2 ) ? 1-i : i;
				kin	= vert_in[ind].pos_kin;
				pos = vert_in[ind].pos_world;
				nor = vert_in[ind].nor;
				uv	= vert_in[ind].uv;
			#else
				if( i==0 )
				{
					pos = vert_in[1].pos_world;
					nor = vert_in[1].nor;
				}
				else if(i==1)
				{
					pos = vert_in[0].pos_world;
					nor = vert_in[0].nor;
				}
				else
				{
					pos = vert_in[2].pos_world;
					nor = vert_in[2].nor;
				}
			#endif
		}
		geom_block.uv			= uv;
		//push along normal
		pos.xyz += nor * push_factor;
		geom_block.pos_kin = kin;
		geom_block.pos_world = pos;
		if( aaa_gu_int[1] == 1 )	pos 			= gl_ModelViewProjectionMatrix * pos;
		else						pos				= gl_ProjectionMatrix * pos;
		gl_Position 	= pos;
//		nor = normalize( gl_NormalMatrix * nor );
		nor = gl_NormalMatrix * nor;
		geom_block.normal_depth = vec4( nor.x, nor.y, nor.z, -pos.z );

		EmitVertex();
	}

	EndPrimitive();
}

















