
in ST_AAA_BV BV;

// Uniform variables for texturing
// uniform sampler2D	aaa_tex2d[4];

#if 0
uniform sampler2D	aaa_samp0;
uniform sampler2D	aaa_samp1;
uniform sampler2D	aaa_samp2;
#	define TEX_A	aaa_samp0
#	define TEX_B	aaa_samp1
#	define TEX_MASK	aaa_samp2
#else
	layout(binding = 0) uniform sampler2D TEX_A;
	layout(binding = 1) uniform sampler2D TEX_B;
	layout(binding = 2) uniform sampler2D TEX_MASK;
#endif

CONST vec3 col_gamma		= aaa_fu_vec4[3].rgb;
CONST vec3 col_offset		= aaa_fu_vec4[4].rgb;
CONST vec4 col_back_tint	= aaa_fu_vec4[5];

CONST vec3 col_incrust	= aaa_fu_vec4[6].xyz;

CONST float incrust_min			= aaa_fu_float[1];
CONST float incrust_max 		= aaa_fu_float[2];
CONST float mask_incrust_min	= aaa_fu_float[3];
CONST float mask_incrust_max	= aaa_fu_float[4];
//todo
//CONST float incrust_gain		= aaa_fu_float[7];
//CONST float incrust_bias		= aaa_fu_float[8];

CONST vec4 src1_mul		= aaa_fu_vec4[1];
CONST vec4 src2_mul		= aaa_fu_vec4[2];

CONST vec2 fuz			= aaa_fu_vec4[7].xy;

CONST bool b_inverse	= (aaa_fu_int[0] & 1)	!= 0;
CONST bool b_tint		= (aaa_fu_int[0] & 2)	!= 0;
CONST bool b_clamp		= (aaa_fu_int[0] & 4)	!= 0;
CONST int  s_incrust	= aaa_fu_int[1];
CONST bool b_mask		= (aaa_fu_int[3] & 1)	!= 0;
CONST bool b_mask_see	= (aaa_fu_int[3] & 16)	!= 0;
CONST bool b_mask_mul	= (aaa_fu_int[3] & 256)	!= 0;

CONST float MORMALIZE_COLOR_DIST_FACTOR = 1.0/sqrt(3.0);

void main()
{
	vec2 uv = BV.tex_coor[0].st;

	vec4 mask;
	if( b_mask )
	{
		mask = texture2D( TEX_MASK, uv);
		float mask_factor = compute_gray( mask.rgb ) * mask.a;
		if( mask_incrust_min > 0 || mask_incrust_max > 0 )
		{
			//mask_factor = linearstep( mask_incrust_min, mask_incrust_max, mask_factor );
			mask_factor = smoothstep( mask_incrust_min, mask_incrust_max, mask_factor );
			//mask_factor = gain( mask_factor, incrust_gain );
			//mask_factor = bias( mask_factor, incrust_bias );
		}
		if( mask_factor<=0 )
			discard;
		
		if( b_mask_mul )
			mask.a = mask_factor;
		else
			mask = vec4(  1,1,1, mask_factor );
		//mask = vec4(  mask.rgb, mask_factor );
		if( b_mask_see )
		{
			gl_FragColor = mask;
			return;
		}
	}
	else
		mask = vec4(1);

	vec4 color;
	vec4 src1_color = texture2D( TEX_A, uv );
	vec4 src2_color = texture2D( TEX_B, uv );

	// this is unclear should be documented
	float f = aaa_fu_float[0];
	color = src1_color * src1_mul * (1.-f) + src2_color * src2_mul * f;

	if( b_inverse )
		color.rgb = vec3(1) - color.rgb;

	switch( s_incrust)
	{
	case 1:
		{	//pure luma inscrust
			float v = gray(color.xyz);
			color.a = smoothstep( incrust_min, incrust_max, v );
		}
		break;
	case 2:
		{	//color inscrust
			vec3 diff = color.xyz - col_incrust;
			//diff = length(vec) ;
			//diff *= gray_conv;
			//v = distance( color.xyz, col_incrust ) / 3.;
			float v = length( diff ) * MORMALIZE_COLOR_DIST_FACTOR ;
			color.a = linearstep( incrust_min, incrust_max, v );
		}
	}

// TINT
	if( b_tint )
	{
		float g = gray( color.rgb );
		color.rgb = mix( col_back_tint.rgb, BV.color.rgb, g );
		color.a *= BV.color.a;
	}
	else
		color = color * BV.color;

	if( color.a<=0 )
		discard;

	//todo deal with the other cases
	//offset
	color.rgb += col_offset;
	//color.rgb - clamp( color.rgb, 0., 1. );
	//gamma
	color.rgb = pow( color.rgb, col_gamma );

	if( b_clamp )
		color = clamp_01( color );

	//vec2 fuz_factor = smoothstep( vec2(0),fuz, uv ) * smoothstep( vec2(0),fuz, vec2(1.)-uv );
	//vec2 fuz_factor = clamp_01( uv * fuz ) * clamp_01( (vec2(1.)-uv) * fuz );
	
	//color.a *= fuz_factor.x * fuz_factor.y;
	//color.a *= min(fuz_factor.x, fuz_factor.y );
	vec2 fuz_factor = clamp_01( min( fract( uv ), fract(vec2(1.)-uv) ) * fuz );
	//vec2 fuz_factor = clamp_01( fract( uv ) * fract(vec2(1.)-uv) * fuz );
	color.a *= (fuz_factor.x) * (fuz_factor.y);

#if 1
	gl_FragColor = color * mask;
#else
	float v = texture2D( TEX_A, BV.tex_coor[0].st ).r * .5;
	gl_FragColor.r = v;
	gl_FragColor = texture2D( TEX_A, BV.tex_coor[0].st );
	//gl_FragColor = vec4( BV.tex_coor[0].st, 0, 1 );
#endif
}
