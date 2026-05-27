
in ST_AAA_BV BV;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];
CONST int	type = aaa_fu_int[0];

#define LATEX 

layout(location = 0) out vec4 color_out_0;
//layout(location = 1) out vec4 color_out_1;

layout(binding = 0) uniform sampler1D TEX_1D;

vec4 get_texture_color( in int unit, in vec2 uv )
{
	int dim = aaa_tex_dim[unit];
	if( dim == 2 )
		return texture2D( aaa_tex2d[unit], uv );
	else if( dim == 0 )
		return vec4( 1, 1, 1,	1 );
//	else if( dim == 1 )	texture1D( TEX_1D, uv.x );
	else if( dim == 1 )
		return vec4( 1, 0, 0,	1 );
	return vec4( 0, 0, 1,	1 );
}

void main (void)
{
	vec4 src;

#if 0
	#if 1
		texture = blend_fs( texture );
	#endif
	//	FALL OFF
	#if 0
		float f = get_fall_off( BV.fall_off );
		//	f = 1. - f;
		texture.a = f;
		texture.xyz *= vec3( f, f, f );
	#endif
#else
	//gl_FrontColor = blend_fs( texture );
	//src = get_texture_color( 0, BV.tex_coor[0].st );
	src = vec4(1);
#endif


#if 0
	src = compute_fog4( src, BV.z_to_eye );
#endif

	float v = clamp( gray(src.rgb*BV.color.rgb), 0., 1. );
#if IS_LATEX == 0
	if( type == 1 )	// Toon
	{
		v = fract( v * (aaa_fu_int[1]+.001) );
		v = clamp( src.r - v / float(aaa_fu_int[1]), 0., 1. );
		src.rgb = vec3(v);
	}
	else if( type == 2 ) // BW
	{
		v = smoothstep( aaa_fu_float[0], aaa_fu_float[1], v );
		src.rgb *= vec3(v);
	}
	else if( type == 3 ) // flag to do input 3 colors or dump
	{
		if( v < aaa_fu_float[0] )
			src.rgb *= vec3(0,0,0.7);
		else if( v < aaa_fu_float[1] )
			src.rgb *= vec3(1,0,0);
		else
			src.rgb *= vec3(1,1,1);
	}
#endif
	src *= BV.color;
#if IS_LATEX == 0
	if( aaa_fu_int[2] > 0 )
	{
		vec2 s = textureSize( aaa_tex2d[1], 0 );
		vec2 uv =  BV.pos_world.zy * .28 * vec2(-1,1);
		uv = (uv + vec2(.5,.0)) * vec2(s.y/s.x,1)*4.;	//.5;	
		//src.rg = uv;
		if( aaa_fu_int[2] == 1 )
			src.rgb += texture( aaa_tex2d[1], uv ).rgb;
		else
			src.rgb *= texture( aaa_tex2d[1], uv ).rgb;
	}
#endif
//	gl_FragColor = src;
//	gl_FragColor.a = 1;
//	gl_FragColor = vec4(0,1,0, .5);
	color_out_0 = src;
//	color_out_1 = vec4( vec3(1.)-src.rgb, src.a );
}
