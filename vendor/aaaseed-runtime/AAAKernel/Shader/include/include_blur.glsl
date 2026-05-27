
#if __VERSION__ < 130
#	define TEXTURE2D texture2D
#else
#	define TEXTURE2D texture
#endif

vec3 blur_gaussian( in sampler2D tex, in vec2 uv, in vec2 move )
{
	const float A22 = 0.0030;
	const float A21 = 0.0133;
	const float A20 = 0.0219;
	const float A11 = 0.0596;
	const float A10 = 0.0983;
	const float A00 = 0.1621;

    vec3	blur;
	blur =	A00 *	TEXTURE2D( tex, uv ).rgb;

	blur +=	A10 *	TEXTURE2D( tex, uv + vec2(-move.x, 0.) ).rgb;
	blur +=	A10 *	TEXTURE2D( tex, uv + vec2(move.x, 0.) ).rgb;
	blur +=	A10 *	TEXTURE2D( tex, uv + vec2(0., -move.y) ).rgb;
	blur +=	A10 *	TEXTURE2D( tex, uv + vec2(0., move.y) ).rgb;

	blur +=	A11 *	TEXTURE2D( tex, uv + vec2(-move.x, -move.y) ).rgb;
	blur +=	A11 *	TEXTURE2D( tex, uv + vec2(move.x, -move.y) ).rgb;
	blur +=	A11 *	TEXTURE2D( tex, uv + vec2(-move.x, move.y) ).rgb;
	blur +=	A11 *	TEXTURE2D( tex, uv + vec2(move.x, move.y) ).rgb;

	blur +=	A20 *	TEXTURE2D( tex, uv + vec2(-move.x*2., 0.) ).rgb;
	blur +=	A20 *	TEXTURE2D( tex, uv + vec2(move.x*2., 0.) ).rgb;
	blur +=	A20 *	TEXTURE2D( tex, uv + vec2(0., -move.y*2.) ).rgb;
	blur +=	A20 *	TEXTURE2D( tex, uv + vec2(0., move.y*2.) ).rgb;

	blur +=	A21 *	TEXTURE2D( tex, uv + vec2(-move.x, -move.y*2.) ).rgb;
	blur +=	A21 *	TEXTURE2D( tex, uv + vec2(move.x, -move.y*2.) ).rgb;
	blur +=	A21 *	TEXTURE2D( tex, uv + vec2(-move.x, move.y*2.) ).rgb;
	blur +=	A21 *	TEXTURE2D( tex, uv + vec2(move.x, move.y*2.) ).rgb;

	blur +=	A21 *	TEXTURE2D( tex, uv + vec2(-move.x*2., -move.y) ).rgb;
	blur +=	A21 *	TEXTURE2D( tex, uv + vec2(move.x*2., -move.y) ).rgb;
	blur +=	A21 *	TEXTURE2D( tex, uv + vec2(-move.x*2., move.y) ).rgb;
	blur +=	A21 *	TEXTURE2D( tex, uv + vec2(move.x*2., move.y) ).rgb;

	blur +=	A22 *	TEXTURE2D( tex, uv + vec2(-move.x*2., -move.y*2.) ).rgb;
	blur +=	A22 *	TEXTURE2D( tex, uv + vec2(move.x*2., -move.y*2.) ).rgb;
	blur +=	A22 *	TEXTURE2D( tex, uv + vec2(-move.x*2., move.y*2.) ).rgb;
	blur +=	A22 *	TEXTURE2D( tex, uv + vec2(move.x*2., move.y*2.) ).rgb;
	return blur;
}

//todo use for react_dif not a blur because ignore center : merge and refine
vec4  rd_blur_3x3( in sampler2D tex, in vec2 uv, in vec2 duv )
{
	 vec4 result	= TEXTURE2D( tex, uv + vec2( -duv.x,      0 ) )
					+ TEXTURE2D( tex, uv + vec2(  duv.x,      0 ) )
					+ TEXTURE2D( tex, uv + vec2(      0, -duv.y ) ) 
					+ TEXTURE2D( tex, uv + vec2(      0,  duv.y ) ); 

	vec4 corner		= TEXTURE2D( tex, uv - duv )
					+ TEXTURE2D( tex, uv + vec2( -duv.x,  duv.y ) )
					+ TEXTURE2D( tex, uv + vec2(  duv.x, -duv.y ) ) 
					+ TEXTURE2D( tex, uv + duv );
		
	return result * .20 + corner * .05;
}


vec4 rd_blur_5x5( in sampler2D tex, in vec2 uv, in vec2 duv )
{
	vec4 result		= TEXTURE2D( tex, uv + duv * vec2( -1,  0 ) )
					+ TEXTURE2D( tex, uv + duv * vec2(  1,  0 ) )
					+ TEXTURE2D( tex, uv + duv * vec2(  0, -1 ) )
					+ TEXTURE2D( tex, uv + duv * vec2(  0,  1 ) )

					+ TEXTURE2D( tex, uv - duv )					
					+ TEXTURE2D( tex, uv + duv * vec2( -1,  1 ) )
					+ TEXTURE2D( tex, uv + duv * vec2(  1, -1 ) ) 	
					+ TEXTURE2D( tex, uv + duv )						

					+ TEXTURE2D( tex, uv + duv * vec2( -2., 0 ) )
					+ TEXTURE2D( tex, uv + duv * vec2(  2,  0 ) )
					+ TEXTURE2D( tex, uv + duv * vec2(  0, -2 ) )
					+ TEXTURE2D( tex, uv + duv * vec2(  0,  2 ) );

	vec4 corner		= TEXTURE2D( tex, uv + duv * vec2( -1, -2 ) )
					+ TEXTURE2D( tex, uv + duv * vec2( -2, -1	) )
					+ TEXTURE2D( tex, uv + duv * vec2( -1,  2	) )
					+ TEXTURE2D( tex, uv + duv * vec2( -2,  1 ) )
					+ TEXTURE2D( tex, uv + duv * vec2(  1, -2 ) )
					+ TEXTURE2D( tex, uv + duv * vec2(  2, -1	) )
					+ TEXTURE2D( tex, uv + duv * vec2(  1,  2	) )
					+ TEXTURE2D( tex, uv + duv * vec2(  2,  1	) );
			
	return (result + corner * .6)/16.8;
}

//todo do somethimg with these
vec4 blur13( in sampler2D tex, in vec2 uv, in vec2 resolution, in vec2 direction )
{
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.411764705882353) * direction;
	vec2 off2 = vec2(3.2941176470588234) * direction;
	vec2 off3 = vec2(5.176470588235294) * direction;
	color += TEXTURE2D( tex, uv) * 0.1964825501511404;
	color += TEXTURE2D( tex, uv + (off1 / resolution)) * 0.2969069646728344;
	color += TEXTURE2D( tex, uv - (off1 / resolution)) * 0.2969069646728344;
	color += TEXTURE2D( tex, uv + (off2 / resolution)) * 0.09447039785044732;
	color += TEXTURE2D( tex, uv - (off2 / resolution)) * 0.09447039785044732;
	color += TEXTURE2D( tex, uv + (off3 / resolution)) * 0.010381362401148057;
	color += TEXTURE2D( tex, uv - (off3 / resolution)) * 0.010381362401148057;
	return color;
}

vec4 blur9( in sampler2D tex, in vec2 uv, in vec2 resolution, in vec2 direction )
{
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.3846153846) * direction;
	vec2 off2 = vec2(3.2307692308) * direction;
	color += TEXTURE2D( tex, uv) * 0.2270270270;
	color += TEXTURE2D( tex, uv + (off1 / resolution)) * 0.3162162162;
	color += TEXTURE2D( tex, uv - (off1 / resolution)) * 0.3162162162;
	color += TEXTURE2D( tex, uv + (off2 / resolution)) * 0.0702702703;
	color += TEXTURE2D( tex, uv - (off2 / resolution)) * 0.0702702703;
	return color;
}

vec4 blur5( in sampler2D tex, in vec2 uv, in vec2 resolution, in vec2 direction )
{
	vec4 color = vec4(0.0);
	vec2 off1 = vec2(1.3333333333333333) * direction;
	color += TEXTURE2D( tex, uv) * 0.29411764705882354;
	color += TEXTURE2D( tex, uv + (off1 / resolution)) * 0.35294117647058826;
	color += TEXTURE2D( tex, uv - (off1 / resolution)) * 0.35294117647058826;
	return color; 
}

//todo do something with these (from Jean I think)
#define aaa_B00 0.195346
#define	aaa_B10 0.123317
#define aaa_B11 0.077847
vec3 blur_gauss_3( in sampler2D tex, in vec2 uv, in vec2 duv )
{
    vec3	blur;

	blur =	aaa_B00 *	TEXTURE2D( tex, uv ).rgb;

	blur +=	aaa_B10 *	TEXTURE2D( tex, uv + duv * vec2(-1,  0) ).rgb;
	blur +=	aaa_B10 *	TEXTURE2D( tex, uv + duv * vec2( 1,  0) ).rgb;
	blur +=	aaa_B10 *	TEXTURE2D( tex, uv + duv * vec2( 0, -1) ).rgb;
	blur +=	aaa_B10 *	TEXTURE2D( tex, uv + duv * vec2( 0,  1) ).rgb;
	blur +=	aaa_B11 *	TEXTURE2D( tex, uv + duv * vec2(-1, -1) ).rgb;
	blur +=	aaa_B11 *	TEXTURE2D( tex, uv + duv * vec2( 1, -1) ).rgb;
	blur +=	aaa_B11 *	TEXTURE2D( tex, uv + duv * vec2(-1,  1) ).rgb;
	blur +=	aaa_B11 *	TEXTURE2D( tex, uv + duv * vec2( 1,  1) ).rgb;
    return blur;
}
vec3 blur_lod_3( in sampler2D tex, in ivec2 uv, in vec2 ts, in int mipmap_level )
{
    vec3	blur;
    vec2    decal = 1./ts;

	blur =	aaa_B00 *	texelFetch( tex, uv, mipmap_level ).rgb;

	blur +=	aaa_B10 *	texelFetch( tex, uv + ivec2(-decal.x, 0.), mipmap_level ).rgb;
	blur +=	aaa_B10 *	texelFetch( tex, uv + ivec2(decal.x, 0.), mipmap_level ).rgb;
	blur +=	aaa_B10 *	texelFetch( tex, uv + ivec2(0., -decal.y), mipmap_level ).rgb;
	blur +=	aaa_B10 *	texelFetch( tex, uv + ivec2(0., decal.y), mipmap_level ).rgb;

	blur +=	aaa_B11 *	texelFetch( tex, uv + ivec2(-decal.x, -decal.y), mipmap_level ).rgb;
	blur +=	aaa_B11 *	texelFetch( tex, uv + ivec2(decal.x, -decal.y), mipmap_level ).rgb;
	blur +=	aaa_B11 *	texelFetch( tex, uv + ivec2(-decal.x, decal.y), mipmap_level ).rgb;
	blur +=	aaa_B11 *	texelFetch( tex, uv + ivec2(decal.x, decal.y), mipmap_level ).rgb;

    return blur;
}























//end should be x * 100 lines
