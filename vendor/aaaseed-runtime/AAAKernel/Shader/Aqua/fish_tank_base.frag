

// GLSL fragment shader
// Apply texture

// --------------------------------------------------------------------------
// VARIABLES

#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
in VS_out
{
	TEX_COOR_VEC	tex_impli;
	TEX_COOR_VEC	tex_refle;
	TEX_COOR_VEC	tex_objec;
	vec4			color;
	float			fall_off;
	float			z_for_fog;
	vec3			normal;
	float			fall_top;
} fs_in;


//	output
out		vec4	fragColor;

//	AAASeed uniform implicit
uniform	sampler2D	aaa_tex2d[16];


vec4 get_tex_rgba( in int unit, in vec2 coor )
{
	if( aaa_tex_dim[unit] == 2 )
		return texture( aaa_tex2d[unit], coor );
	return vec4( 0, 1, 0, 1 );
}
float get_tex_caustic( in vec2 coor )
{
//	if( aaa_tex_dim[8] == 2 )
		//this because grey texture in alpha
		return texture( aaa_tex2d[8], coor ).r;
//	return 0.;
}

// --------------------------------------------------------------------------
// MAIN  FUNCTION
#define FISH_ECRAN_MIX .35

void main (void)
{
	vec2 uv = fs_in.tex_impli.st;

	// --------------------------
	// AJOUT DIFFUS TEXTURE
	vec4 diffu		= get_tex_rgba( 0, uv );
	vec4 ao			= get_tex_rgba( 1, uv );
	vec4 mask		= get_tex_rgba( 2, uv );
	vec4 diffu_bis	= get_tex_rgba( 3, uv );
//	vec2 proj_uv	= gl_FragCoord.st * aaa_fu_vec4[0].xy + aaa_fu_vec4[0].zw;
	vec2 proj_uv	= gl_FragCoord.st * vec2(1./4096,1./1630) - vec2( .10 + (aaa_fu_float[7]+1) * .237, .24 );
	proj_uv *= vec2(3.1,1.9);
	vec4 projection;
	if( max( min( proj_uv, vec2(1.) ), vec2(0.) ) ==  proj_uv )
		projection = texture( aaa_tex2d[7], proj_uv );
	else
		projection = diffu;

	if( aaa_fu_int[3] > 0 )
	{
		if( aaa_fu_int[3] == 1 )
			fragColor = fs_in.color;
		else
			fragColor = texture( aaa_tex2d[aaa_fu_int[3]-2], uv );
		return;
	}

	// --------------------------
	// AJOUT LUMIERE
	vec4	dif		= mix( diffu,				diffu_bis,			aaa_fu_float[4] );
			dif		= mix( dif,					dif * ao, 			aaa_fu_float[1] );
			dif.rgb	= mix( dif.rgb,				(vec3(1.-FISH_ECRAN_MIX)+dif.rgb*FISH_ECRAN_MIX)*projection.rgb, 	aaa_fu_float[5] * projection.a );
	vec4	tex		= mix( fs_in.color*dif, 	dif,				aaa_fu_float[0] );
//	mask.rgb	= mix( mask.rgb, vec3(1.), aaa_fu_float[2] );
//	tex.rgb *= mask.rgb;
	tex.rgb *= aaa_fu_float[2];

//	vec4 reflec	= get_tex_rgb1( 2, fs_in.tex_refle.st );
//	float f = 1 - get_fall_off( fs_in.fall_off );
//	tex = 1.0*tex + aaa_fu_float[1] * f * f * f * f * diffu * mask.r;

	// --------------------------
	// AJOUT CAUSTIC
	float fn = fs_in.fall_top;
	if( fn > 0. )
	{
		float caustic = get_tex_caustic( fs_in.tex_objec.st );
		fn = clamp( fn, 0.0, 1.0 );
		float r = caustic * fn;
		tex.rgb += aaa_fu_float[3] * vec3( r, r, r*.2 ) * mask.r;
	}


	//tex.a = diffu.a;

	//tex = blend_fs( tex );
	// --------------------------
	// AJOUT BROUILLARD
	if( aaa_fu_int[0] > 0 )
	{
		//float fog = get_fog( fs_in.z_for_fog );
		//tex = vec4( fog, fog, fog, 1.);
		tex = compute_fog4( tex, -fs_in.z_for_fog );
	}

//	tex = aqua_compute_fog4( tex, fs_in.z_for_fog );
	fragColor = tex;
}
