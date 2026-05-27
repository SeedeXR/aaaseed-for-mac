
in ST_AAA_BV BV;

// Uniform variables for texturing
// uniform sampler2D	aaa_tex2d[4];
uniform sampler2D	aaa_samp0;
uniform sampler2D	aaa_samp1;
#if 0
#	define TEX_A aaa_samp0
#	define TEX_B aaa_samp1
#else
	layout(binding = 0) uniform sampler2D TEX_A;
	layout(binding = 1) uniform sampler2D TEX_B;
#endif

CONST float incrust_min = aaa_fu_float[1];
CONST float incrust_max = aaa_fu_float[2];

CONST vec4 col_back		= aaa_fu_vec4[0];

CONST vec3 col_gamma	= aaa_fu_vec4[3].rgb;
CONST vec3 col_offset	= aaa_fu_vec4[4].rgb;

CONST vec4 src1_mul		= aaa_fu_vec4[1];
CONST vec4 src2_mul		= aaa_fu_vec4[2];

CONST bool b_inverse	= (aaa_fu_int[0]==1);
CONST bool b_clamp		= (aaa_fu_int[1]==1);
CONST bool b_tint		= (aaa_fu_int[2]==1);

void main()
{
	vec4 src1_color = texture2D( TEX_A, BV.tex_coor[0].st );
	vec4 src2_color = texture2D( TEX_B, BV.tex_coor[0].st );

	vec4 color;
	if( aaa_fu_int[3]==1 )
	{
		color.rgb = mix( src2_color.rgb, vec3(1.) - src2_color.rgb, src1_color.r );
        
        //if( color.rb == vec2(0) && color.g != 0 )
        {
            color.rgb = vec3(1);
        }

		color.a = 1.;
		//color = vec4(1);
	}
	else
	{
		// this is unclear should be documented
		float f = aaa_fu_float[0];
		color = mix( src1_color * src1_mul , src2_color * src2_mul, f );
        float green = color.g;

        if( color.r < green*.6 && color.b < green*.6 && green > .05)
        {
//            color.rgb = vec3(1);

            color.rgb = vec3(0,0,0);
            color.a = 1.-green;
            if( color.a < .005 )
                discard;
        }

#if 0
		if( b_inverse )
			color.rgb = vec3(1) - color.rgb;

		if( incrust_min!=0. || incrust_max!=0. )
			color.a = smoothstep( incrust_min, incrust_max, gray(color.xyz) );

// TINT
		if( b_tint )
		{
			float g = gray( color.rgb );
			color.rgb = mix( col_back.rgb, BV.color.rgb, g );
			color.a *= BV.color.a; 
		}
		else
#endif
			color = color * BV.color;

		//todo deal with the other cases
		//offset
		color.rgb += col_offset;
		//color.rgb - clamp( color.rgb, 0., 1. );
		//gamma
		color.rgb = pow( color.rgb, col_gamma );
	}

	if( b_clamp )
		color = clamp_01( color );
		
#if 1
//	if( BV.tex_coor[0].t  < .35 )
//		color *= 1.75;
	gl_FragColor = color;
//	gl_FragColor = vec4( color.r,0,0, 1 );	
//	gl_FragColor = vec4( color.xyz, 1 );
//	gl_FragColor = vec4(  fract(BV.tex_coor[0].st), 0, 1 );
#else
	//gl_FragColor = vec4(aaa_fu_float[0]);
	//gl_FragColor.r = src1_color.r;
//	float v = BV.tex_coor[0].s * BV.tex_coor[0].t;
	float v = texture2D( TEX_A, BV.tex_coor[0].st ).r * .5;
	//gl_FragColor.r = v==0. ? 0 : 1.;
	gl_FragColor.r = v;
	//gl_FragColor.a = 1.;
	gl_FragColor = texture2D( TEX_A, BV.tex_coor[0].st );
	//gl_FragColor = vec4( BV.tex_coor[0].st, 0, 1 );
#endif
}
