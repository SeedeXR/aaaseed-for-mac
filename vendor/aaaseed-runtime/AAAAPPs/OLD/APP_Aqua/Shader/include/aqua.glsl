vec4 aqua_compute_fog4( in vec4 color_in, in float dist )
{
 	float fog;
	//	linear version
	fog = (dist) * .2;
	//	fog = exp2( -gl_Fog.density * gl_FogFragCoord * LOG2E );
		//The equation for fog allows us to move a portion of
		//the calculation (-gl_Fog.density * LOG2E) to the CPU
		//increasing the efficiency of our shader by doing a
		//scalar multiplication on the CPU rather than the GPU.
        //	fog = exp2( gl_FogFragCoord * -0.144270 ); 	//	from 3Dlabs FOG_DENSITY .15
	//	fog = exp2( -gl_Fog.density * gl_Fog.density * gl_FogFragCoord * gl_FogFragCoord * LOG2E );
		//The equation for fog allows us to move a portion of
		//the calculation (-gl_Fog.density * gl_Fog.density * LOG2E)
		//to the CPU increasing the efficiency of our shader by doing a
		//scalar multiplication on the CPU rather than the GPU.
		//	fog = exp2(gl_FogFragCoord * gl_FogFragCoord * -0.014427); 	//	from 3Dlabs FOG_DENSITY .15 bug here

	fog = clamp( fog, 0.0, 1.0 );
	return mix( color_in, vec4( .0, 0.05	, .1, 1. ), fog );
	//   texture = vec4( mix( vec3(gl_Fog.color), vec3(texture), fog), texture.a);
}


// --------------------------------------------------------------------------
// GET COLOR FUNCTIONS

vec4 get_tex_rgba( in int unit, in vec2 coor )
{
	if( aaa_tex_dim[unit] == 2 )
		return texture( aaa_tex2d[unit], coor );
	return vec4( 0, 1, 0, 1 );
}
vec4 get_tex_rgb1( in int unit, in vec2 coor )
{
	if( aaa_tex_dim[unit] == 2 )
		return vec4( texture( aaa_tex2d[unit], coor ).xyz, 1. );
	return vec4( 0, 1, 0, 1 );
}
float get_tex_caustic( in vec2 coor )
{
//	if( aaa_tex_dim[8] == 2 )
		//this because grey texture in alpha
		return texture( aaa_tex2d[8], coor ).r;
//	return 0.;
}
float get_tex_caustic_3( in vec2 coor )
{
	if( aaa_tex_dim[3] == 2 )
		//this because grey texture in alpha
		return texture( aaa_tex2d[3], coor ).r;
	return 0.;
}

/*
//	REFLECTION
vec4 get_color_refle( in int unit, in vec2 coor )
{
	if( aaa_tex_dim[unit] == 2 )
	{
		vec4	tex = texture( aaa_tex2d[unit], coor );
		tex.a = 1;
		return tex;
	}
	return vec4( 0, 0, 0, 0 );
}
vec4 get_color_refle_2( in vec2 coor )
{
	if( aaa_tex_dim[2] == 2 )
	{
		vec4	tex = texture( aaa_tex2d[2], coor );
		tex.a = 1;
		return tex;
	}
	return vec4( 0, 0, 0, 0 );
}

//	CAUSTIC
float get_color_caustic( in int unit, in vec2 coor )
{
	if( aaa_tex_dim[unit] == 2 )
	{
		//this because grey texture in alpha
		return texture( aaa_tex2d[unit], coor ).r;
	}
	return 0.;
}
float get_color_caustic_3( in vec2 coor )
{
	if( aaa_tex_dim[3] == 2 )
	{
		//this because grey texture in alpha
		return texture( aaa_tex2d[3], coor ).r;
	}
	return 0.;
}
*/

// --------------------------------------------------------------------------
// GET FALLOFF FUNCTION
float	get_fall_off( in float f )
{
	if( f < 0. )
		f = 0.;
	#if 0
		f = 1. - f;
		f *= 1.1;
		if( f > 1. )
			f -=  (f - 1.) * 10.;
	#endif
	f = f * f;
	return f;
}
