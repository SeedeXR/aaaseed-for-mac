
in VS_out
{
	vec4	pos_world;
	vec4	color;
} fs_in;

//	output
out vec4	fragColor;

//	AAASeed uniform implicit
uniform sampler3D	aaa_samp0;
uniform sampler2D	aaa_samp1;
uniform sampler3D	aaa_samp2;

//uniform sampler3D	aaa_tex3d[4];
//uniform sampler2D	aaa_tex2d[4];
//uniform sampler3D	aaa_tex3d[4];

//uniform sampler3D samp = 0;

//	AAAseed uniform pixel shader
//uniform float		aaa_fu_float[8];
//	0	color_factor
//	1	alpha_factor
//	2	size_factor on w axe
//	3	v on color palette
//uniform int			aaa_fu_int[4];
//	0	mode
//	1	step_nb
//	2 : 0 grey, 1 color using palette
//uniform vec4		aaa_fu_vec4[8];
//	0	crop_min
//	1	crop_max
//	2	alpha_min,	alpha_max,	alpha_stop


const vec4 color_dbg[4] = vec4[4]	(	vec4( 1,1,1, 1 ),	vec4( 1,0,0, 1 ),	vec4( 0,1,0, 1 ),	vec4( 0,0,1, 1 ) );
vec4 get_color_dbg( in int unit )	{	return color_dbg[ aaa_tex_dim[unit] ];	}

vec3 get_gradient_3d( sampler3D samp, vec3 at, float d )
{
	vec2 d2 = vec2( d, 0 );
 	return vec3(	texture( samp, at - d2.xyy ).r - texture( samp, at + d2.xyy ).r,
					texture( samp, at - d2.yxy ).r - texture( samp, at + d2.yxy ).r,
					texture( samp, at - d2.yyx ).r - texture( samp, at + d2.yyx ).r
				);
}
vec3 get_normal( sampler3D samp, vec3 at, float d )
 {
	vec2 d2 = vec2( d, 0 );
 	vec3 n = vec3(	texture( samp, at - d2.xyy ).r - texture( samp, at + d2.xyy ).r,
					texture( samp, at - d2.yxy ).r - texture( samp, at + d2.yxy ).r,
					texture( samp, at - d2.yyx ).r - texture( samp, at + d2.yyx ).r
				);
	return normalize(n);
}

float	get_red( sampler3D samp, vec3 at )	{	return texture( samp, at ).r;	}
float	get_red0( vec3 at )					{	return texture( aaa_samp0, at ).r;	}
float	get_red1( vec3 at )					{	return texture( aaa_samp2, at ).r;	}
vec4	map_grey( float grey )				{	return texture( aaa_samp1, vec2( grey, aaa_fu_float[3] ) );	}
vec4	get_color0( vec3 at )
{
	//	2	alpha_min,	alpha_max,	alpha_stop
	float g = texture( aaa_samp0, at ).r;
	if( g == clamp( g, aaa_fu_vec4[2].x, aaa_fu_vec4[2].y ) )
	{
		if( aaa_fu_int[2] == 0 )
			return vec4(g);
		else
			return texture( aaa_samp1, vec2( g, aaa_fu_float[3] ) );
	}
	else
		return vec4(0);
}
vec4	get_color1( vec3 at )
{
	return texture( aaa_samp1, vec2( texture( aaa_samp2, at ).r, aaa_fu_float[4] ) );
}


void main (void)
{
//	discard;
	vec3 uvw = fs_in.pos_world.xyz * vec3( 1., 1., aaa_fu_float[2] ) + .5;

	if( any( lessThan( uvw, aaa_fu_vec4[0].xyz ) ) )
		discard;
	if( any( greaterThan( uvw, aaa_fu_vec4[1].xyz ) ) )
		discard;

	if( aaa_fu_int[0] <= 0 )
	{
		if( aaa_fu_int[0] == 0 )
		{
			fragColor.xyz = uvw;
			fragColor.a = 1.;
		}
		else
		{	//color according to dim unit
			fragColor = get_color_dbg( -aaa_fu_int[0]-1 );
		}
		return;
	}

	vec4 tex;
//	if( aaa_fu_int[0] < 2 )
//	{
		tex = get_color0( uvw );
		tex.rgb *= aaa_fu_float[0];
		tex.a *= aaa_fu_float[1];
//	}
	fragColor = tex * fs_in.color;
}
